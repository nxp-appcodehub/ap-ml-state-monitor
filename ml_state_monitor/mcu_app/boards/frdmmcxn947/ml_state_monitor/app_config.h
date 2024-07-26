/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_CONFIG_H
#define _APP_CONFIG_H

/*******************************************************************************
 * Hardware configuration
 ******************************************************************************/
/*
 * DO NOT CHANGE!
 * AVAILABLE sensor boards
 */
#define ACCEL_4_CLICK_BOARD    1  //Uses FXLS8974 Accelerometer
#define FRDM_STBI_A8974_BOARD  2  //Uses FXLS8974 Accelerometer

/*
 * USER CONFIGURATION!
 * SELECT which sensor board is being with the FRDM-MCXN947
 */
#define SENSOR_BOARD           FRDM_STBI_A8974_BOARD

#if defined(SENSOR_BOARD) && (SENSOR_BOARD == FRDM_STBI_A8974_BOARD)
#elif defined(SENSOR_BOARD) && (SENSOR_BOARD == ACCEL_4_CLICK_BOARD)
#else
#error "ERROR: Undefined/unknown sensor board"
#endif /* SENSOR_BOARD */


/*******************************************************************************
 * Functional scenario configuration
 ******************************************************************************/
/*
 * DO NOT CHANGE!
 * AVAILABLE actions to be performed
 */
#define SENSOR_COLLECT_LOG_EXT                  1   // Collect and log data externally
#define SENSOR_COLLECT_RUN_INFERENCE            2   // Collect data and run inference

/*
 * DO NOT CHANGE!
 * AVAILABLE inference engine to be used
 */
#define SENSOR_COLLECT_INFENG_TENSORFLOW        1   // TensorFlow

/*
 * DO NOT CHANGE!
 * AVAILABLE data format to be used to feed the model
 */
#define SENSOR_COLLECT_DATA_FORMAT_BLOCKS       1   // Blocks of samples
#define SENSOR_COLLECT_DATA_FORMAT_INTERLEAVED  2   // Interleaved samples

/*
 * DO NOT CHANGE!
 * AVAILABLE pre-recorded validation datasets
 */
#define VDSET_CLOG                              0
#define VDSET_FRICTION                          1
#define VDSET_ON                                2
#define VDSET_OFF                               3

/*
 * USER CONFIGURATION!
 * SELECT the action to be performed
 * Set to SENSOR_COLLECT_LOG_EXT to collect sensor data for training.
 * Set to SENSOR_COLLECT_RUN_INFERENCE to run ML inference.
 */
#define SENSOR_COLLECT_ACTION                   SENSOR_COLLECT_RUN_INFERENCE

#if SENSOR_COLLECT_ACTION == SENSOR_COLLECT_LOG_EXT
/*
 * USER CONFIGURATION!
 * SELECT where to log the sensor data
 * Set to 0 to redirect the sensor data to the debug console.
 * Set to 1 to redirect the sensor data to the SD card.
 */
#define SENSOR_COLLECT_LOG_EXT_SDCARD           1   // Redirect the log to SD card, otherwise print to console

#elif SENSOR_COLLECT_ACTION == SENSOR_COLLECT_RUN_INFERENCE
/*
 * USER CONFIGURATION!
 * SELECT inference behavior
 */
#define SENSOR_COLLECT_RUN_INFENG               SENSOR_COLLECT_INFENG_TENSORFLOW // Inference engine to be used
#define SENSOR_FEED_VALIDATION_DATA             1   // Feed the model with data from memory (recorded previously for validation)
#define SENSOR_RAW_DATA_NORMALIZE               1   // Normalize the raw data
#define SENSOR_EVALUATE_MODEL                   0   // Evaluate the model's performance by computing the accuracy
#define SENSOR_COLLECT_INFENG_VERBOSE_EN        0   // Enable verbosity
#define SENSOR_COLLECT_DATA_FORMAT              SENSOR_COLLECT_DATA_FORMAT_INTERLEAVED

#if (SENSOR_FEED_VALIDATION_DATA == 1)
/*
 * USER CONFIGURATION!
 * SELECT the static dataset for validation
 */
#define STATIC_ACCEL_VDSET                      VDSET_OFF
#endif


/*
 * DO NOT CHANGE!
 */
#define SOURCE_STATIC_ACCEL_DATA                SENSOR_FEED_VALIDATION_DATA

/*
 * DO NOT CHANGE!
 * Sanity check
 */
#if (SENSOR_COLLECT_RUN_INFENG != SENSOR_COLLECT_INFENG_TENSORFLOW)
#error "Unsupported inference engine"
#endif /* SENSOR_COLLECT_RUN_INFENG */
#else
#error "Unsupported action"
#endif /* SENSOR_COLLECT_ACTION */


/*******************************************************************************
 * ML models configuration
 ******************************************************************************/
/*
 * USER CONFIGURATION!
 * SELECT model type
 * 0 - Float; 1 - Quant; 2 - Quant Neutron Converted
 */
#define TFLITE_MODEL_TYPE           2

/*
 * USER CONFIGURATION!
 * SELECT if loading model to RAM memory
 * If set to 1 then the model will be copied from FLASH into RAM
 */
#define TFLITE_LOAD_MODEL_TO_RAM    1

#endif /* _APP_CONFIG_H */
