/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "glowmodel.h"
#include "glow_bundle_utils.h"

#include "labels.h"

#include "model_selection.h"

// ----------------------------- Bundle API -----------------------------
// Bundle includes.
#include "glow_bundle_utils.h"
#if GLOW_MODEL_QUANTIZED_EN
#include "glow_model_fan_clsf_quant.h"
#else
#include "glow_model_fan_clsf.h"
#endif

#if GLOW_MODEL_QUANTIZED_EN
#define MODEL_MEM_ALIGN             GLOW_MODEL_FAN_CLSF_QUANT_MEM_ALIGN
#define MODEL_CONSTANT_MEM_SIZE     GLOW_MODEL_FAN_CLSF_QUANT_CONSTANT_MEM_SIZE
#define MODEL_MUTABLE_MEM_SIZE      GLOW_MODEL_FAN_CLSF_QUANT_MUTABLE_MEM_SIZE
#define MODEL_ACTIVATIONS_MEM_SIZE  GLOW_MODEL_FAN_CLSF_QUANT_ACTIVATIONS_MEM_SIZE
#define MODEL_acceleration          GLOW_MODEL_FAN_CLSF_QUANT_acceleration
#define MODEL_Identity              GLOW_MODEL_FAN_CLSF_QUANT_Identity
#else
#define MODEL_MEM_ALIGN             GLOW_MODEL_FAN_CLSF_MEM_ALIGN
#define MODEL_CONSTANT_MEM_SIZE     GLOW_MODEL_FAN_CLSF_CONSTANT_MEM_SIZE
#define MODEL_MUTABLE_MEM_SIZE      GLOW_MODEL_FAN_CLSF_MUTABLE_MEM_SIZE
#define MODEL_ACTIVATIONS_MEM_SIZE  GLOW_MODEL_FAN_CLSF_ACTIVATIONS_MEM_SIZE
#define MODEL_acceleration          GLOW_MODEL_FAN_CLSF_acceleration
#define MODEL_Identity              GLOW_MODEL_FAN_CLSF_Identity
#endif

// Statically allocate memory for constant weights (model weights) and initialize.
GLOW_MEM_ALIGN(MODEL_MEM_ALIGN)
#if GLOW_LOAD_MODEL_TO_RAM
uint8_t constantWeight[MODEL_CONSTANT_MEM_SIZE] = {
#else
const uint8_t constantWeight[MODEL_CONSTANT_MEM_SIZE] = {
#endif
#if GLOW_MODEL_QUANTIZED_EN
#include "glow_model_fan_clsf_quant.weights.txt"
#else
#include "glow_model_fan_clsf.weights.txt"
#endif
};

// Statically allocate memory for mutable weights (model input/output data).
GLOW_MEM_ALIGN(MODEL_MEM_ALIGN)
uint8_t mutableWeight[MODEL_MUTABLE_MEM_SIZE];

// Statically allocate memory for activations (model intermediate results).
GLOW_MEM_ALIGN(MODEL_MEM_ALIGN)
uint8_t activations[MODEL_ACTIVATIONS_MEM_SIZE];

// Bundle input data absolute address.
uint8_t *inputAddr = GLOW_GET_ADDR(mutableWeight, MODEL_acceleration);

// Bundle output data absolute address.
uint8_t *outputAddr = GLOW_GET_ADDR(mutableWeight, MODEL_Identity);

// ---------------------------- Application -----------------------------
// Model input data size (bytes).
#define MODEL_INPUT_SIZE    (128 * 3 * sizeof(float))

// Model number of output classes.
#define MODEL_OUTPUT_CLASS  4

extern uint32_t TIMER_GetTimeInUS();

status_t GLOWMODEL_Init(void)
{
    return kStatus_Success;
}

status_t GLOWMODEL_RunInference(void *inputData, size_t size, int8_t *predClass, int32_t *tinf_us, uint8_t verbose)
{
    status_t err = 0;
    uint64_t start = 0;

    memcpy(inputAddr, inputData, size);

    start = TIMER_GetTimeInUS();
#if GLOW_MODEL_QUANTIZED_EN
    err = glow_model_fan_clsf_quant(constantWeight, mutableWeight, activations);
#else
    err = glow_model_fan_clsf(constantWeight, mutableWeight, activations);
#endif
    *tinf_us = TIMER_GetTimeInUS() - start;

    if (kStatus_Success != err)
    {
        *predClass = -1;
        printf("MODEL_RunInference ERROR %ld\r\n", err);
        return kStatus_Fail;
    }

    float *out_data = (float*)(outputAddr);
    float max_val = 0.0;
    uint32_t max_idx = 0;
    for(int i = 0; i < MODEL_OUTPUT_CLASS; i++) {
      if (out_data[i] > max_val) {
        max_val = out_data[i];
        max_idx = i;
      }
    }
    *predClass = max_idx;

    if (verbose)
    {
        printf("\r\n----------------------------------------\r\n");
        printf("GLOW predictions: ");
        for (int i = 0; i < MODEL_OUTPUT_CLASS; i++)
        {
            printf("%.5f ", out_data[i]);
        }
        printf("\r\n");
        printf("----------------------------------------\r\n");
        printf("     Inference time: %ld us\r\n", *tinf_us);
        printf("     Detected: %-10s (%d%%)\r\n", labels[max_idx], (int) (max_val * 100));
        printf("----------------------------------------\r\n");
        printf("\r\n");
    }

    return kStatus_Success;
}
