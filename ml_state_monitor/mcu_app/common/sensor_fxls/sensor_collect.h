/*
 * Copyright 2021, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SENSOR_MODEL_H_
#define _SENSOR_MODEL_H_

#include <stdint.h>
#include <stdio.h>

#include "fsl_common.h"
#include "app_config.h"

/*******************************************************************************
 * Application parameters
 ******************************************************************************/

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

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
uint64_t TIMER_GetTimeInUS();

void MainTask(void *pvParameters);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SENSOR_MODEL_H_ */
