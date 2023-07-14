/*
 * Copyright 2020-2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */
#ifndef _HAL_STATIC_ACCELEROMETER_H_
#define _HAL_STATIC_ACCELEROMETER_H_

#include "mpp_api_types.h"
#include "hal_types.h"

typedef struct _static_accelerometer static_accelerometer_t;

typedef enum _hal_StaticAccelerometer_status
{
    MPP_kStatus_HAL_StaticAccelSuccess = 0,  /*!< Successfully */
	MPP_kStatus_HAL_StaticAccelError        /*!< Error occurs on HAL Static Accelerometer data */

} hal_StaticAccelerometer_status_t;

/*! @brief Operation that needs to be implemented by an image element */
typedef struct _static_accelerometer_operator
{
    /* initialize the elt */
    hal_StaticAccelerometer_status_t (*init)(static_accelerometer_t *elt, mpp_static_accel_params_t *config, void *param, unsigned long size);
    /* dequeue a buffer from the elt */
    hal_StaticAccelerometer_status_t (*dequeue)(const static_accelerometer_t *elt, hw_buf_desc_t *out_buf, int channel);
} static_accelerometer_operator_t;

/*! @brief Structure that characterize the image element. */
typedef struct
{
    /* buffer resolution */
    int height;
    int width;
    int channel;
    int format;
    int frequency;
} static_accelerometer_static_config_t;

/*! @brief Attributes of a an image element. */
struct _static_accelerometer
{
    /* unique id which is assigned by image manager */
    int id;
   /* operations */
    const static_accelerometer_operator_t *ops;
    /* static configs */
    static_accelerometer_static_config_t config;
    uint8_t *buffer;
};


#endif /*_HAL_STATIC_ACCELEROMETER_H_*/
