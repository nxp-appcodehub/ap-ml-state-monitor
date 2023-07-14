/*
 * Copyright 2021-2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Set the operations used in NN models. This allows reducing the code size.
 * Important Notice: User may find the list of operations needed by its model using tool https://netron.app
 */

#include "mpp_config.h"
#include "model_configuration.h"

#if (TFLITE_ALL_OPS_RESOLVER == 0)

#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/kernels/softmax.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

tflite::MicroOpResolver &MODEL_GetOpsResolver(tflite::ErrorReporter* errorReporter)
{
#if MODEL_QUANTIZED
	//Model-select
#if SELECTED_MODEL == MODEL_LRESNET

    static tflite::MicroMutableOpResolver<10> s_microOpResolver(errorReporter);

    s_microOpResolver.AddQuantize();
    s_microOpResolver.AddMul();
    s_microOpResolver.AddAdd();
    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddRelu();
    s_microOpResolver.AddAveragePool2D();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();
    s_microOpResolver.AddDequantize();

#elif SELECTED_MODEL == MODEL_MLP

    static tflite::MicroMutableOpResolver<5> s_microOpResolver(errorReporter);

    s_microOpResolver.AddQuantize();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();
    s_microOpResolver.AddDequantize();

#elif SELECTED_MODEL == MODEL_LCNN

    static tflite::MicroMutableOpResolver<7> s_microOpResolver(errorReporter);

    s_microOpResolver.AddQuantize();
    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddMaxPool2D();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();
    s_microOpResolver.AddDequantize();

#elif SELECTED_MODEL == MODEL_CNN

    static tflite::MicroMutableOpResolver<7> s_microOpResolver(errorReporter);

    s_microOpResolver.AddQuantize();
    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddMaxPool2D();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();
    s_microOpResolver.AddDequantize();

#endif //Model-select

#else //MODEL_QUANTIZED
#if SELECTED_MODEL == MODEL_LRESNET  //Model-select

    static tflite::MicroMutableOpResolver<8> s_microOpResolver(errorReporter);

    s_microOpResolver.AddMul();
    s_microOpResolver.AddAdd();
    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddRelu();
    s_microOpResolver.AddAveragePool2D();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();

#elif SELECTED_MODEL == MODEL_MLP

    static tflite::MicroMutableOpResolver<3> s_microOpResolver(errorReporter);

    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();

#elif SELECTED_MODEL == MODEL_LCNN

    static tflite::MicroMutableOpResolver<5> s_microOpResolver(errorReporter);

    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddMaxPool2D();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();

#elif SELECTED_MODEL == MODEL_CNN

    static tflite::MicroMutableOpResolver<5> s_microOpResolver(errorReporter);

    s_microOpResolver.AddConv2D();
    s_microOpResolver.AddMaxPool2D();
    s_microOpResolver.AddReshape();
    s_microOpResolver.AddFullyConnected();
    s_microOpResolver.AddSoftmax();

#endif //Model-select

#endif //MODEL_QUANTIZED

    return s_microOpResolver;
}

#endif // TFLITE_ALL_OPS_RESOLVER
