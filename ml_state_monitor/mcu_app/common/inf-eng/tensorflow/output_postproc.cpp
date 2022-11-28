/*
 * Copyright 2020-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "output_postproc.h"
#include "get_top_n.h"
#include "labels.h"
#ifdef EIQ_GUI_PRINTF
#include "chgui.h"
#endif

#define DETECTION_TRESHOLD  0
#define NUM_RESULTS         1

status_t MODEL_ProcessOutput(const uint8_t* data, const tensor_dims_t* dims,
                             tensor_type_t type, int inferenceTime,
                             int8_t *predClass, uint8_t verbose)
{
    const float threshold = (float)DETECTION_TRESHOLD / 100;
    result_t topResults[NUM_RESULTS];
    const char* label = "No label detected";
    *predClass = -1;

    /* Find best label candidates. */
    MODEL_GetTopN(data, dims->data[dims->size - 1], type, NUM_RESULTS, threshold, topResults);

    if (verbose)
    {
        printf("\r\n");
        printf("----------------------------------------\r\n");
        const float* output = reinterpret_cast<const float*>(data);
        for (int i = 0; i < 4; i++) {
            printf("%d-%.5f ", i, output[i]);
        }
        printf("\r\n");
    }

    float confidence = 0;
    if (topResults[0].index >= 0)
    {
        auto result = topResults[0];
        confidence = result.score;
        int index = result.index;
        if (confidence * 100 > DETECTION_TRESHOLD)
        {
            label = labels[index];
            *predClass = index;
        }
    }

    if (verbose)
    {
        int score = (int)(confidence * 100);
        printf("----------------------------------------\r\n");
        printf("     Inference time: %d us\r\n", inferenceTime);
        printf("     Detected: %-10s (%d%%)\r\n", label, score);
        printf("----------------------------------------\r\n");
    }

#ifdef EIQ_GUI_PRINTF
    GUI_PrintfToBuffer(GUI_X_POS, GUI_Y_POS, "Detected: %.20s (%d%%)", label, score);
#endif

    return kStatus_Success;
}
