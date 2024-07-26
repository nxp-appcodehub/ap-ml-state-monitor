/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/kernels/neutron/neutron.h"
#include "app_config.h"

tflite::MicroOpResolver &MODEL_GetOpsResolver()
{
#if TFLITE_MODEL_TYPE == 0
    static tflite::MicroMutableOpResolver<5> s_microOpResolver;

    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddMaxPool2D();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();
#elif TFLITE_MODEL_TYPE == 1
    static tflite::MicroMutableOpResolver<7> s_microOpResolver;

    s_microOpResolver.AddQuantize();
    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddMaxPool2D();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();
    s_microOpResolver.AddDequantize();
#elif TFLITE_MODEL_TYPE == 2
    static tflite::MicroMutableOpResolver<5> s_microOpResolver;

    s_microOpResolver.AddQuantize();
    s_microOpResolver.AddMaxPool2D();
    s_microOpResolver.AddSoftmax();
    s_microOpResolver.AddDequantize();
    s_microOpResolver.AddCustom(tflite::GetString_NEUTRON_GRAPH(), tflite::Register_NEUTRON_GRAPH());  
#endif

    return s_microOpResolver;
}
