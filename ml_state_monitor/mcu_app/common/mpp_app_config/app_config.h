/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_CONFIG_H
#define _APP_CONFIG_H


/*******************************************************************************
 * Application parameters
 ******************************************************************************/
/*
 * This section provides a template for the application parameters.
 * These application parameters can be configured to other supported values
 */

/* SELECT data source: 0 - live accelerometer sensor data; 1 - static validation data */
#define SOURCE_STATIC_ACCEL_DATA 0

/* SELECT data usage: 0 - run ML inference; 1 - run SD log data capturing */
#define SENSOR_COLLECT_LOG_EXT 0

/* Available ML inference engines */
#define INFERENCE_ENGINE_TFLM 0
#define INFERENCE_ENGINE_GLOW 1

/* SELECT the ML inference engine */
#define INFERENCE_ENGINE INFERENCE_ENGINE_TFLM

/* Available ML NN models for inference */
#define MODEL_CNN       1
#define MODEL_LCNN      2
#define MODEL_MLP       3
#define MODEL_LRESNET   4

/* SELECT ML NN model for inference */
#define SELECTED_MODEL MODEL_CNN

/* SELECT quantization: 0 - no quantization; 1 - use quantized models */
#define MODEL_QUANTIZED   0

/* Available pre-recorded validation datasets */
#define VDSET_CLOG      0
#define VDSET_FRICTION  1
#define VDSET_ON        2
#define VDSET_OFF       3

#if (SOURCE_STATIC_ACCEL_DATA == 1)
/* SELECT the static dataset for validation */
#define STATIC_ACCEL_VDSET VDSET_OFF
#endif



/* Debug sensor data: 0 - no print;  1 - print sensor data */
#define PRINT_SENSOR_DATA 0

#if SOURCE_STATIC_ACCEL_DATA == 0
#define ENABLE_ACCELEROMETRE_Fxls8974 1
#else
#define ENABLE_ACCELEROMETRE_Fxls8974 0
#endif

/* Fxls8974 accelerometer frequency */
#define SENSOR_COLLECT_RATE_HZ      200     // Sampling frequency

/* Input data configurations */
#define CLSF_CHANNELS   3                   // Number of channels
#define CLSF_WINDOW     128                 // Window length in samples
#define CLSF_STRIDE     CLSF_WINDOW/2       // Sliding window length

#define HAL_ENABLE_SD_LOG_EXT SENSOR_COLLECT_LOG_EXT

#endif /* _APP_CONFIG_H */
