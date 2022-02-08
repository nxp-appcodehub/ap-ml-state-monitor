/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.
   Copyright 2021 NXP

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

/* File modified by NXP. Changes are described in file
   /middleware/eiq/tensorflow-lite/readme.txt in section "Release notes" */

#include <stdio.h>

#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "model.h"
#include "model_selection.h"
#if TFLITE_MODEL_QUANTIZED_EN
#include "model_fan_clsf_quant.h"
#else
#include "model_fan_clsf.h"
#endif

static tflite::ErrorReporter* s_errorReporter = nullptr;
static const tflite::Model* s_model = nullptr;
static tflite::MicroInterpreter* s_interpreter = nullptr;
static TfLiteTensor* inputTensor;

extern tflite::MicroOpResolver &MODEL_GetOpsResolver(tflite::ErrorReporter* errorReporter);

// An area of memory to use for input, output, and intermediate arrays.
// (Can be adjusted based on the model needs.)
//constexpr int kTensorArenaSize = 512 * 1024;
#if TFLITE_MODEL_QUANTIZED_EN
constexpr int kTensorArenaSize = 4 * 1024;
#else
constexpr int kTensorArenaSize = 9 * 1024;
#endif
static uint8_t s_tensorArena[kTensorArenaSize] __ALIGNED(16);

#if TFLITE_LOAD_MODEL_TO_RAM
/**
 * TFLITE_MEMBLOB_SIZE needs to be at least as large as the tflite model file.
 */
#define TFLITE_MEMBLOB_SIZE 50 * 1024
uint8_t tflite_memblob[TFLITE_MEMBLOB_SIZE] __attribute__((aligned(4)));
#endif

status_t MODEL_Init(void)
{
    // Set up logging. Google style is to avoid globals or statics because of
    // lifetime uncertainty, but since this has a trivial destructor it's okay.
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroErrorReporter micro_error_reporter;
    s_errorReporter = &micro_error_reporter;

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.

#if TFLITE_MODEL_QUANTIZED_EN
     const uint8_t *model = model_fan_clsf_quant;
    const int model_size = model_fan_clsf_quant_len;
#else
    const uint8_t *model = model_fan_clsf;
    const int model_size = model_fan_clsf_len;
#endif

#if TFLITE_LOAD_MODEL_TO_RAM
    if(model_size < TFLITE_MEMBLOB_SIZE){
        memcpy(tflite_memblob,model,model_size);
        model = (const uint8_t*)tflite_memblob;
        printf("Model loaded to SDRAM...\r\n");
    } else {
        printf("Model too large (%d) for SDRAM buffer (%d)\r\n", model_size, TFLITE_MEMBLOB_SIZE);
    }
#endif

    s_model = tflite::GetModel(model);
    if (s_model->version() != TFLITE_SCHEMA_VERSION)
    {
        TF_LITE_REPORT_ERROR(s_errorReporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             s_model->version(), TFLITE_SCHEMA_VERSION);
        return kStatus_Fail;
    }

    // Pull in only the operation implementations we need.
    // This relies on a complete list of all the ops needed by this graph.
    // An easier approach is to just use the AllOpsResolver, but this will
    // incur some penalty in code space for op implementations that are not
    // needed by this graph.
    //
    // tflite::AllOpsResolver resolver;
    // NOLINTNEXTLINE(runtime-global-variables)
    tflite::MicroOpResolver &micro_op_resolver = MODEL_GetOpsResolver(s_errorReporter);

    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
        s_model, micro_op_resolver, s_tensorArena, kTensorArenaSize, s_errorReporter);
    s_interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = s_interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(s_errorReporter, "AllocateTensors() failed");
        return kStatus_Fail;
    }

    // Get information about the memory area to use for the model's input.
    inputTensor = s_interpreter->input(0);

    return kStatus_Success;
}

status_t MODEL_RunInference(void)
{
    if (s_interpreter->Invoke() != kTfLiteOk)
    {
        printf("Invoke failed!\r\n");
        return kStatus_Fail;
    }

    return kStatus_Success;
}

uint8_t* GetTensorData(TfLiteTensor* tensor, tensor_dims_t* dims, tensor_type_t* type)
{
    switch (tensor->type)
    {
        case kTfLiteFloat32:
            *type = kTensorType_FLOAT32;
            break;
        case kTfLiteUInt8:
            *type = kTensorType_UINT8;
            break;
        case kTfLiteInt8:
            *type = kTensorType_INT8;
            break;
        default:
            assert("Unknown input tensor data type");
    };

    dims->size = tensor->dims->size;
    assert(dims->size <= MAX_TENSOR_DIMS);
    for (int i = 0; i < tensor->dims->size; i++)
    {
        dims->data[i] = tensor->dims->data[i];
    }

    return tensor->data.uint8;
}

uint8_t* MODEL_GetInputTensorData(tensor_dims_t* dims, tensor_type_t* type)
{
    return GetTensorData(inputTensor, dims, type);
}

uint8_t* MODEL_GetOutputTensorData(tensor_dims_t* dims, tensor_type_t* type)
{
    TfLiteTensor* outputTensor = s_interpreter->output(0);

    return GetTensorData(outputTensor, dims, type);
}
