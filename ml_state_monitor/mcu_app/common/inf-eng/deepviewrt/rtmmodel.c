/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "rtmmodel.h"

#include "deepview_rt.h"
#include "deepview_ops.h"

#include "model_selection.h"
#include "labels.h"

/**
 * The mempool holds the intermediate buffers for evaluating the
 * model.  This buffer can be multiple megabytes in size and therefore
 * should be stored in the SDRAM.  You may adjust this size if your
 * particular model requires more or less memory as reported by the
 * conversion tool.  If insufficient memory is provided an error will
 * be reported by nn_context_model_load().
 */
/**
 *The maximum runtime memroy size,
 *adjust it according model's runtime size and board's SDRAM size.
 */
#if RTM_MODEL_QUANTIZED_EN
#define MEMPOOL_SIZE 6 * 1024
#else
#define MEMPOOL_SIZE 11 * 1024
#endif

/* DeepViewRT Model definition from model.S */
extern const unsigned char model_rtm_start;
extern const unsigned char model_rtm_end;

/**
 * The DeepViewRT Memory Pool buffer holds intermediate buffers and is
 * stored in SDRAM for maximum storage space.
 */
#define __SECTION(type, bank) __attribute__ ((section("." #type ".$" #bank)))
#define __BSS(bank) __SECTION(bss, bank)
__BSS(BOARD_SDRAM) uint8_t mempool[MEMPOOL_SIZE] __attribute__((aligned(32)));

#if RTM_LOAD_MODEL_TO_RAM
/**
 * MEMBLOB_SIZE needs to be at least as large as the RTM model file.
 */
#define MEMBLOB_SIZE 50 * 1024
__BSS(BOARD_SDRAM) uint8_t memblob[MEMBLOB_SIZE] __attribute__((aligned(32)));
#endif

/**
 * The cache is used by DeepViewRT to optimize certain internal
 * loops.  It is optional but if used should be placed in the
 * fastest available memories, in this case we use the SRAM_DTC.
 */
#define RTM_ENABLE_CACHE 1
#if RTM_ENABLE_CACHE
#define RTM_CACHE_SIZE 256
/**
 * The DeepViewRT Cache buffer stored in SRAM_DTC for maximum performance.
 */
__BSS(SRAM_DTC_cm7) uint8_t rtm_cache[RTM_CACHE_SIZE] __attribute__((aligned(32)));
#else
#define RTM_CACHE_SIZE 0
uint8_t *rtm_cache = NULL;
#endif

struct
{
    const int32_t *in_shape;
    const int32_t *out_shape;
    int32_t in_dim;
    int32_t out_dim;
    int32_t in_tensor_shape[4];
    int32_t in_tensor_size;
    const void *out_scores;

    NNContext *context;
    NNTensor *in_tensor;
    NNTensor *out_tensor;
    NNTensorType in_tensor_type;
    NNTensorType out_tensor_type;
} rtmmodel;

extern uint32_t TIMER_GetTimeInUS();

status_t RTMMODEL_Init(void)
{
    volatile NNError err;

    /**
     * The model and model_size will be setup at startup based on the model_rtm_start
     * and model_rtm_end variables from the model.S file.
     */
    const uint8_t *model_end = &model_rtm_end;
    const uint8_t *model = &model_rtm_start;
    int model_size = model_end - model;
    if (model_size < 1) {
        printf("[ERROR] invalid model_size (%d) verify model.S implementation.\r\n", model_size);
        return kStatus_Fail;
    }

    /**
     * The NNContext structure holds runtime model data including the memory pool
     * and optional cache.  The first parameter is for the engine which is not used
     * on MCU devices, but the same API is provided across all platforms.
     *
     * If MEMPOOL_SIZE is 0 then each layer's tensor will be allocated on the heap
     * using malloc instead of using the optimized memory map.  If MEMPOOL_SIZE is
     * greater than 0 but mempool is NULL then the pool will be allocated from the
     * heap and the optimized memory map WILL be used.
     *
     * If CACHE_SIZE is 0 then no cache will be used, convolutions especially will
     * take significantly longer.  If CACHE_SIZE is greater than 0 but cache is
     * NULL then it will be allocated on the heap.  This will provide better performance
     * but will depend on the performance of heap memory, if this is SDRAM the model
     * will take approximately 10x longer than if the cache is located in SRAM_DTC.
     *
     * If the heap is too small for the configuration context will be NULL.
     */
    rtmmodel.context = nn_context_init(NULL, MEMPOOL_SIZE, mempool, RTM_CACHE_SIZE, rtm_cache);
    if (!rtmmodel.context) {
        printf("[ERROR] insufficient memory to create context\r\n");
        return kStatus_Fail;
    }

#if RTM_LOAD_MODEL_TO_RAM
    if(model_size < MEMBLOB_SIZE){
        memcpy(memblob,model,model_size);
        model = (const uint8_t*)memblob;
        printf("Model loaded to SDRAM...\r\n");
    } else {
        printf("Model too large (%d) for SDRAM buffer (%d)\r\n", model_size, MEMBLOB_SIZE);
    }
#endif

    /**
     * Loads the model into the context.  If the model is invalid because of corruption
     * or alignment an error will be returned.  If the provided MEMPOOL_SIZE is insufficient
     * an error will also be returned.  Many of these internal errors will also be logged
     * through stderr.
     */
    err = nn_context_model_load(rtmmodel.context, (size_t) model_size, model);
    if (err) {
        printf("[ERROR] failed to load model: %s\r\n", nn_strerror(err));
        return kStatus_Fail;
    }

    /**
     * Acquire the input tensor, will be used for loading the sample image into the model.
     */
    size_t input_index = (size_t)(nn_model_inputs(model,NULL)[0]);
    rtmmodel.in_tensor = nn_context_tensor_index(rtmmodel.context, input_index);
    if (!rtmmodel.in_tensor) {
        printf("[ERROR] failed to retrieve input tensor\r\n");
        return kStatus_Fail;
    }

    rtmmodel.in_tensor_type = nn_tensor_type(rtmmodel.in_tensor);
    rtmmodel.out_tensor_type = nn_tensor_type(rtmmodel.out_tensor);

    /**
     * Acquire the output tensor, will be used for reading out results of model evaluation.
     */
    size_t output_index = (size_t)(nn_model_outputs(model,NULL)[0]);
    rtmmodel.out_tensor = nn_context_tensor_index(rtmmodel.context, output_index);
    if (!rtmmodel.out_tensor) {
        printf("[ERROR] failed to retrieve output tensor\r\n");
        return kStatus_Fail;
    }

    rtmmodel.in_shape = nn_tensor_shape(rtmmodel.in_tensor);
    rtmmodel.out_shape = nn_tensor_shape(rtmmodel.out_tensor);
    rtmmodel.in_dim = nn_tensor_dims(rtmmodel.in_tensor);
    rtmmodel.out_dim = nn_tensor_dims(rtmmodel.out_tensor);

    printf("Type In %d, Out %d\r\n", rtmmodel.in_tensor_type, rtmmodel.out_tensor_type);
    printf("Dim In %ld, Out %ld\r\n", rtmmodel.in_dim, rtmmodel.out_dim);
    printf("In  shape: ");
    rtmmodel.in_tensor_size = 1;
    for (int i = 0; i < rtmmodel.in_dim; i++)
    {
        rtmmodel.in_tensor_shape[i] = rtmmodel.in_shape[i];
        rtmmodel.in_tensor_size *= rtmmodel.in_shape[i];
        printf("%ld ", rtmmodel.in_shape[i]);
    }
    rtmmodel.in_tensor_size *= 4;
    printf("= %ld\r\n", rtmmodel.in_tensor_size);

    printf("Out shape: ");
    for (int i = 0; i < rtmmodel.out_dim; i++)
    {
        printf("%ld ", rtmmodel.out_shape[i]);
    }
    printf("\r\n");

    return kStatus_Success;
}

status_t RTMMODEL_RunInference(void *inputData, size_t size, int8_t *predClass, int32_t *tinf_us, uint8_t verbose)
{
    volatile NNError err;

    *predClass = -1;

    if (size != rtmmodel.in_tensor_size)
    {
        printf("[ERROR] MODEL_RunInference wrong input size\r\n");
        return kStatus_Fail;
    }

    err = nn_tensor_assign(rtmmodel.in_tensor, NNTensorType_F32, rtmmodel.in_dim,
                           rtmmodel.in_tensor_shape, inputData);

    if (err) {
        printf("[ERROR] MODEL_RunInference failed to load input data: %s\r\n", nn_strerror(err));
        return kStatus_Fail;
    }

    /**
     * The nn_context_run function performs the actual model evaluation.  This causes all layers
     * in the graph to be evaluated.  If any error happens on any layer this function will return
     * an error and more details might be reported to stderr depending on the cause.
     *
     * A common warning can be reported when insufficient cache is provided leading to performance
     * degradations.  These do not affect the accuracy of the results but do translate to longer
     * inference times.
     */
    uint64_t start = TIMER_GetTimeInUS();
    err = nn_context_run(rtmmodel.context);
    *tinf_us = TIMER_GetTimeInUS() - start;

    if (err) {
        printf("[ERROR] MODEL_RunInference: %d - %s\r\n", err, nn_strerror(err));
        return kStatus_Fail;
    }

    /**
     * Classification models are typically arranged in a one-hot encoding.  This means the output is
     * a vector representing the known labels, the largest element in this vector represents the
     * inferred label.  This "argmax" can be used as an index into our known labels to report a text
     * label result.  If a label is not provided we instead report the argmax value, this can also
     * happen if labels were provided but argmax was beyond the provided labels.
     */
    int argmax;
    float softmax;
    nn_argmax(rtmmodel.out_tensor, &argmax, &softmax, sizeof(softmax));
    *predClass = argmax;

    if (verbose)
    {
        printf("\r\n----------------------------------------\r\n");
        printf("RTM predictions: ");
        rtmmodel.out_scores = nn_tensor_mapro(rtmmodel.out_tensor);
        for (int i = 0; i < rtmmodel.out_shape[1]/*4*/; i++)
        {
            printf("%.5f ", ((float*)rtmmodel.out_scores)[i]);
        }
        nn_tensor_unmap(rtmmodel.out_tensor);
        printf("\r\n");
        printf("----------------------------------------\r\n");
        printf("     Inference time: %ld us\r\n", *tinf_us);
        printf("     Detected: %-10s (%d%%)\r\n", labels[argmax], (int) (softmax * 100));
        printf("----------------------------------------\r\n");
        printf("\r\n");
    }

    return kStatus_Success;
}
