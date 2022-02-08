/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "model_selection.h"

tflite::MicroOpResolver &MODEL_GetOpsResolver(tflite::ErrorReporter* errorReporter)
{
#if TFLITE_MODEL_QUANTIZED_EN
    static tflite::MicroMutableOpResolver<7> s_microOpResolver(errorReporter);

    s_microOpResolver.AddQuantize();
    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddMaxPool2D();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();
    s_microOpResolver.AddDequantize();
#else
    static tflite::MicroMutableOpResolver<5> s_microOpResolver(errorReporter);

    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddMaxPool2D();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();
#endif

    return s_microOpResolver;
}
