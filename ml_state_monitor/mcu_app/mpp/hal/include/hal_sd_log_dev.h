/*
 * Copyright 2020-2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */
#ifndef _HAL_SD_LOG_H_
#define _HAL_SD_LOG_H_

#include "hal.h"
#include "mpp_api_types.h"
#include "hal_types.h"

typedef struct _sd_log_dev sd_log_dev_t;
typedef struct _sd_dev_private_capability sd_dev_private_capability_t;

/*! @brief The mpp callback function prototype */
typedef int (*mpp_sd_callback_t)(mpp_t mpp, mpp_evt_t evt, void *evt_data, void *user_data);

typedef enum _hal_sd_log_status
{
	kStatus_HAL_SdLogSuccess = 0, /*!< Successfully */
    kStatus_HAL_SdLogTxBusy,      /*!< Display tx is busy */
    kStatus_HAL_SdLogNonBlocking, /*!< Display will return immediately */
    kStatus_HAL_SdLogError,       /*!< Error occurs on HAL Display */

} hal_sd_log_status_t;

/*! @brief Operation that needs to be implemented by an image element */
typedef struct _sd_log_operator
{
	hal_sd_log_status_t (*init)(sd_log_dev_t *dev, void *user_data);
    /* deinitialize the dev */
	hal_sd_log_status_t (*deinit)(const sd_log_dev_t *dev);
    /* start the dev */
	hal_sd_log_status_t (*start)(sd_log_dev_t *dev);
    /* stop the dev */
	hal_sd_log_status_t (*stop)(sd_log_dev_t *dev);
    /* blit a buffer to the dev */
	hal_sd_log_status_t (*write)(const sd_log_dev_t *dev, float *frame);
    /* get buffer descriptors and policy */
	hal_sd_log_status_t (*get_buf_desc)(const sd_log_dev_t *dev, hw_buf_desc_t *in_buf, mpp_memory_policy_t *policy);
} sd_log_operator_t;

/*! @brief Structure that characterize the image element. */
struct _sd_dev_private_capability
{
    /* pixel format */
    mpp_pixel_format_t format;
    /* number of input buffers */
    int nbFrameBuffer;
    /* array of pointers to framebuffer */
    void **frameBuffers;

    /* param for the callback */
    void *user_data;
};

/*! @brief Attributes of a an image element. */
struct _sd_log_dev
{
    /* unique id which is assigned by image manager */
    int id;
    /* name of the device */
    char name[MPP_DEVICE_NAME_MAX_LENGTH];
   /* operations */
    const sd_log_operator_t *ops;
    /* private capability */
    sd_dev_private_capability_t cap;
};


#endif /*_HAL_SD_LOG_H_*/
