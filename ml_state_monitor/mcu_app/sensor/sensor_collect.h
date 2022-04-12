/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SENSOR_MODEL_H_
#define _SENSOR_MODEL_H_

#include <stdint.h>
#include <stdio.h>

#include "fsl_common.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* Settings for sensor data acquisition */
#define SENSOR_COLLECT_QUEUE_ITEMS  200     // Queue to hold samples
#define SENSOR_COLLECT_RATE_HZ      200     // Sampling frequency
#define SENSOR_ACC_RESOLUTION       8192    // signed 14-bit resolution for X-Y-Z axis

/* Settings for how to feed the model */
#define CLSF_CHANNELS   3                   // Number of channels
#define CLSF_WINDOW     128                 // Window length in samples
#define CLSF_OFFSET     CLSF_WINDOW/2       // Number of samples to retain between inferences
#if CLSF_WINDOW <= CLSF_OFFSET
#error "The classifier window length should be larger than offset"
#endif

/* Action to be performed */
#define SENSOR_COLLECT_LOG_EXT                  1   // Collect and log data externally
#define SENSOR_COLLECT_RUN_INFERENCE            2   // Collect data and run inference

/* Inference engine to be used */
#define SENSOR_COLLECT_INFENG_TENSORFLOW        1   // TensorFlow
#define SENSOR_COLLECT_INFENG_DEEPVIEWRT        2   // DeepViewRT
#define SENSOR_COLLECT_INFENG_GLOW              3   // Glow

/* Data format to be used to feed the model */
#define SENSOR_COLLECT_DATA_FORMAT_BLOCKS       1   // Blocks of samples
#define SENSOR_COLLECT_DATA_FORMAT_INTERLEAVED  2   // Interleaved samples


/* Parameters to be configured by the user: */
/* Configure the action to be performed */
#define SENSOR_COLLECT_ACTION                   SENSOR_COLLECT_RUN_INFERENCE

#if SENSOR_COLLECT_ACTION == SENSOR_COLLECT_LOG_EXT
/* If the SD card log is not enabled the sensor data will be streamed to the terminal */
#define SENSOR_COLLECT_LOG_EXT_SDCARD           1   // Redirect the log to SD card, otherwise print to console

#elif SENSOR_COLLECT_ACTION == SENSOR_COLLECT_RUN_INFERENCE
#define SENSOR_COLLECT_RUN_INFENG               SENSOR_COLLECT_INFENG_TENSORFLOW
#define SENSOR_FEED_VALIDATION_DATA             1   // Feed the model with data recorded previously for validation
#define SENSOR_RAW_DATA_NORMALIZE               1   // Normalize the raw data
#define SENSOR_EVALUATE_MODEL                   1   // Evaluate the model's performance by computing the accuracy
#define SENSOR_COLLECT_INFENG_VERBOSE_EN        0   // Enable verbosity

#if (SENSOR_COLLECT_RUN_INFENG == SENSOR_COLLECT_INFENG_TENSORFLOW ||\
        SENSOR_COLLECT_RUN_INFENG == SENSOR_COLLECT_INFENG_DEEPVIEWRT ||\
        SENSOR_COLLECT_RUN_INFENG == SENSOR_COLLECT_INFENG_GLOW)
#define SENSOR_COLLECT_DATA_FORMAT              SENSOR_COLLECT_DATA_FORMAT_INTERLEAVED
#if (SENSOR_COLLECT_RUN_INFENG == SENSOR_COLLECT_INFENG_DEEPVIEWRT) &&\
    (defined __CORTEX_M) && (__CORTEX_M != 7U)
#error "Unsupported DeepViewRT inference engine for the selected core"
#endif /* SENSOR_COLLECT_INFENG_DEEPVIEWRT && __CORTEX_M */
#else
#error "Unsupported inference engine"
#endif /* SENSOR_COLLECT_RUN_INFENG */
#else
#error "Unsupported action"
#endif /* SENSOR_COLLECT_ACTION */

uint64_t TIMER_GetTimeInUS();

void MainTask(void *pvParameters);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SENSOR_MODEL_H_ */
