/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "tfmodel.h"

#include "model.h"
#include "output_postproc.h"

struct
{
    tensor_dims_t inputDims;
    tensor_type_t inputType;
    tensor_dims_t outputDims;
    tensor_type_t outputType;
    uint8_t* inputData;
    uint8_t* outputData;
} tfmodel;

extern uint32_t TIMER_GetTimeInUS();

//#define TF_ENGINE_DISABLE

status_t TFMODEL_Init(void)
{
#ifndef TF_ENGINE_DISABLE
    if (MODEL_Init() != kStatus_Success)
    {
        printf("Failed initializing model\r\n");
        for (;;) {}
    }

    tfmodel.inputData = MODEL_GetInputTensorData(&tfmodel.inputDims, &tfmodel.inputType);
    tfmodel.outputData = MODEL_GetOutputTensorData(&tfmodel.outputDims, &tfmodel.outputType);
#endif

    return kStatus_Success;
}

status_t TFMODEL_RunInference(void *inputData, size_t size, int8_t *predClass, int32_t *tinf_us, uint8_t verbose)
{
    status_t err = 0;
    uint64_t start = 0;

#ifndef TF_ENGINE_DISABLE
    memcpy(tfmodel.inputData, inputData, size);

    start = TIMER_GetTimeInUS();
    err = MODEL_RunInference();
    *tinf_us = TIMER_GetTimeInUS() - start;
#endif

    if (kStatus_Success != err)
    {
        *predClass = -1;
        printf("MODEL_RunInference ERROR\r\n");
        return kStatus_Fail;
    }

    MODEL_ProcessOutput(tfmodel.outputData, &tfmodel.outputDims, tfmodel.outputType, *tinf_us, predClass, verbose);

    return kStatus_Success;
}
