/*
 * Copyright 2020-2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/* TO DO
 * @brief hal accelerometer device declaration. accelerometer devices can enqueue and dequeue frames as well as react to events from
 * input devices via the "inputNotify" function. Camera devices can use any number of interfaces, including MIPI and CSI
 * as long as the HAL driver implements the necessary functions found in accelerometer_dev_operator_t.
 * Examples of camera devices include the Orbbec U1S 3D SLM camera module, and the OnSemi MT9M114 camera module.
 */

#ifndef _HAL_ACCELEROMETER_dev_H_
#define _HAL_ACCELEROMETER_dev_H_

#include "hal.h"
#include "mpp_api_types.h"
#include "fsl_video_common.h"
#include "hal_types.h"

/**
 * @brief declare the accelerometer dev ##name
 */
#define HAL_ACCELEROMETER_DEV_DECLARE(name) int HAL_AccelerometerDev_##name##_Register();

/**
 * @brief register the accelerometer dev ##name
 */
#define HAL_ACCELEROMETER_DEV_REGISTER(name, ret)                             \
    {                                                                  \
        ret = HAL_AccelerometerDev_##name##_Register();                       \
        if (ret != 0)                                                  \
        {                                                              \
            HAL_LOGE("HAL_CameraDev_%s_Register error %d\n", "##name", ret); \
            return ret;                                                \
        }                                                              \
    }

typedef struct _accelerometer_dev accelerometer_dev_t;

typedef enum _hal_accelerometer_status
{
    kStatus_HAL_AccelerometerSuccess = 0,  /*!< Successfully */
    kStatus_HAL_AccelerometerBusy,         /*!< Accelerometer is busy */
    kStatus_HAL_AccelerometerNonBlocking,  /*!< Accelerometer will return immediately */
    kStatus_HAL_AccelerometerError         /*!< Error occurs on HAL accelerometer */
} hal_accelerometer_status_t;

/*! @brief Type of events that are supported by calling the callback function */
typedef enum _accelerometer_event
{
    /* accelerometer new frame is available */
    kAccelerometerEvent_SendFrame,
    /* accelerometer device finished the init process */
    kAccelerometerEvent_AccelerometerDeviceInit,
    kAccelerometerEvent_Count
} accelerometer_event_t;

/**
 * @brief Callback function to notify accelerometer manager that one frame is dequeued
 * @param dev Device structure of the accelerometer device calling this function
 * @param event id of the event that took place
 * @param param Parameters
 * @param fromISR True if this operation takes place in an irq, 0 otherwise
 * @return 0 if the operation was successfully
 */
typedef int (*accelerometer_dev_callback_t)(const accelerometer_dev_t *dev, accelerometer_event_t event, void *param, uint8_t fromISR);

/*! @brief Operation that needs to be implemented by a accelerometer device */
typedef struct _accelerometer_dev_operator
{
    /* initialize the dev */
    hal_accelerometer_status_t (*init)(accelerometer_dev_t *dev, mpp_accelerometer_params_t *config, accelerometer_dev_callback_t callback, void *param);
    /* deinitialize the dev */
    hal_accelerometer_status_t (*deinit)(accelerometer_dev_t *dev);
    /* start the dev */
    hal_accelerometer_status_t (*start)(const accelerometer_dev_t *dev);
    /* stop the dev */
    hal_accelerometer_status_t (*stop)(const accelerometer_dev_t *dev);
    /* enqueue a buffer to the dev */
    hal_accelerometer_status_t (*enqueue)(const accelerometer_dev_t *dev, void *data);
    /* dequeue a buffer from the dev */
    hal_accelerometer_status_t (*dequeue)(const accelerometer_dev_t *dev, hw_buf_desc_t *out_buf, int format);
    /* get buffer descriptors and policy */
    hal_accelerometer_status_t (*get_buf_desc)(const accelerometer_dev_t *dev, hw_buf_desc_t *out_buf, mpp_memory_policy_t *policy);
} accelerometer_dev_operator_t;

/*! @brief Structure that characterize the accelerometer device. */
typedef struct
{
    /* buffer resolution */
    int height;
    int width;
    int pitch;
    int format;
    int frequency;
} accelerometer_dev_static_config_t;

typedef struct
{
    /* callback */
	accelerometer_dev_callback_t callback;
    /* param for the callback */
    void *param;
} accelerometer_dev_private_capability_t;

/*! @brief Attributes of a accelerometer device. */
struct _accelerometer_dev
{
    /* unique id which is assigned by accelerometer manager during registration */
    int id;
    /* name of the device */
    char name[MPP_DEVICE_NAME_MAX_LENGTH];

    /* operations */
    const accelerometer_dev_operator_t *ops;
    /* static configs */
    accelerometer_dev_static_config_t config;
    /* private capability */
    accelerometer_dev_private_capability_t cap;
};

#endif /*_HAL_ACCELEROMETER_dev_H_*/
