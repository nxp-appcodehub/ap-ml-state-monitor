/*
 * Copyright 2019-2023 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief fxls8974 accelerometer module HAL accelerometer driver implementation.
 */

#include "app_config.h"

/* Enable SD logging only when needed to optimize footprint */
#if (HAL_ENABLE_SD_LOG_EXT == 1)
#include "mpp_config.h"
#include <FreeRTOS.h>
#include <labels.h>
#include <task.h>
#include <stdlib.h>
#include "board.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "hal_sd_log_dev.h"
#include "hal.h"
#include "hal_utils.h"
#include "issdk_hal.h"
#include "sdcard_capture.h"
#include "capture.h"
#include <inttypes.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__cplusplus)
}
#endif

static float g_buf[128*3];
static uint64_t g_SensorCollectDuration_us = 0;
static uint64_t g_SensorCollectDuration_samples = 0;
const char *g_SensorCollectLabel = 0;
static uint64_t t0 = 0 ;

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Settings for how to feed the model */
#define CLSF_CHANNELS   3                   // Number of channels
#define CLSF_WINDOW     128                 // Window length in samples
#define CLSF_OFFSET     CLSF_WINDOW/2       // Number of samples to retain between inferences

#define SENSOR_COLLECT_QUEUE_ITEMS  200     // Queue to hold samples
#define FXLS8974_DATA_SIZE 6

/*******************************************************************************
 * Constants
 ******************************************************************************/

hal_sd_log_status_t HAL_SD_LOG_Init(sd_log_dev_t *dev, void *user_data)
{

	uint8_t captClassLabelIdx;
	hal_sd_log_status_t ret = kStatus_HAL_SdLogSuccess;
    HAL_LOGD("++HAL_sd_logDev_Init\n");

	CAPT_Init(&captClassLabelIdx, &g_SensorCollectDuration_us, &g_SensorCollectDuration_samples);
	g_SensorCollectLabel = labels[captClassLabelIdx];

    dev->cap.frameBuffers = (void **)g_buf;
    dev->cap.user_data = user_data;

    HAL_LOGD("--HAL_sd_logDev_Init\n");

    return ret;
}

hal_sd_log_status_t HAL_SD_LOG_Deinit(const sd_log_dev_t *dev)
{
	hal_sd_log_status_t ret = kStatus_HAL_SdLogSuccess;
    return ret;
}

hal_sd_log_status_t HAL_SD_LOG_Start(sd_log_dev_t *dev)
{
	hal_sd_log_status_t ret = kStatus_HAL_SdLogSuccess;
    static uint8_t bufSizeLog = 0;
    static char tab[50];
    static uint8_t tabSize = sizeof(tab);

    HAL_LOGD("++HAL_sd_logDev_Start\n");
    bufSizeLog = snprintf(tab, tabSize, "class,time[ms],Ax,Ay,Az\r\n");
    SDCARD_CaptureData(0, 0, 0, tab, bufSizeLog);
    HAL_LOGD("--HAL_sd_logDev_Start\n");
    return ret;
}

hal_sd_log_status_t HAL_SD_LOG_Stop(sd_log_dev_t *dev)
{
	hal_sd_log_status_t ret = kStatus_HAL_SdLogSuccess;
    HAL_LOGD("++\n");
    HAL_LOGD("--\n");
    return ret;
}

hal_sd_log_status_t HAL_SD_LOG_Write(const sd_log_dev_t *dev, float *frame)
{
	static uint64_t sample_number = 0;
	static uint64_t ts_us;
    static uint8_t bufSizeLog = 0;
    static char tab[50];
    static uint8_t tabSize = sizeof(tab);
    static volatile float inputdata[4] ;
	hal_sd_log_status_t ret = kStatus_HAL_SdLogSuccess;
    HAL_LOGD("++HAL_sd_logDev_Write\n");

    if (sample_number < g_SensorCollectDuration_samples) {
		if (t0 == 0) {
			t0 = TIMER_GetTimeInUS();
		}
		ts_us = TIMER_GetTimeInUS() - t0;

		for(int sample =0 ; sample<64 ; sample++)
		{
			inputdata[0] = *(frame + 3*sample);
			inputdata[1] = *(frame + 3*sample + 1);
			inputdata[2] = *(frame + 3*sample + 2);
			inputdata[3] = *(uint32_t*)(frame + sample + 64 * 3);

			bufSizeLog = snprintf(tab, tabSize, "%s,%ld,%ld,%ld,%ld\r\n",g_SensorCollectLabel,(uint32_t)inputdata[3],(int32_t)inputdata[0], (int32_t)inputdata[1], (int32_t)inputdata[2]);
			sample_number++;
			SDCARD_CaptureData(ts_us, sample_number, g_SensorCollectDuration_samples, tab, bufSizeLog);
		}
    }

    HAL_LOGD("--HAL_sd_logDev_Write\n");
    return ret;
}

hal_sd_log_status_t HAL_SD_LOG_Getbufdesc(const sd_log_dev_t *dev, hw_buf_desc_t *in_buf, mpp_memory_policy_t *policy)
{
	hal_sd_log_status_t ret = kStatus_HAL_SdLogSuccess;
    HAL_LOGD("++HAL_sd_logDev_GetInput\n");

    do
    {
        if ((in_buf == NULL) || (policy == NULL))
        {
            HAL_LOGE("\nNULL pointer to buffer descriptor\n");
            ret = kStatus_HAL_SdLogError;
            break;
        }
        /* set memory policy */
        *policy = HAL_MEM_ALLOC_BOTH;
        in_buf->cacheable = false;
        in_buf->addr = (unsigned char *) (g_buf);
    } while (false);

    HAL_LOGD("--HAL_sd_logDev_GetInput\n");
    return ret;
}

const static sd_log_operator_t sd_log_ops = {
    .init        = HAL_SD_LOG_Init,
    .deinit      = HAL_SD_LOG_Deinit,
    .start       = HAL_SD_LOG_Start,
    .stop        = HAL_SD_LOG_Stop,
    .write        = HAL_SD_LOG_Write,
    .get_buf_desc    = HAL_SD_LOG_Getbufdesc,
};

int sd_log_sim_setup(sd_log_dev_t *elt)
{
    elt->ops = &sd_log_ops;
    return 0;
}
#else
int sd_log_sim_setup(void)
{
	/* DO nothing SD card not enabled */
	return 0;
}
#endif /*ENABLE_SD_LOG_EXT*/
