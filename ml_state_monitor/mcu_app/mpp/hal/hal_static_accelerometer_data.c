/*
 * Copyright 2020-2023 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */
#include "hal.h"
#include "hal_utils.h"
#include "timers.h"
#include "app_config.h"

typedef struct  {
	uint64_t ts_us[CLSF_STRIDE];
	float data[CLSF_STRIDE*CLSF_CHANNELS];
} staticacc_samples_t;

// 3 buffers for sliding window: two for current window and one for next
static staticacc_samples_t g_clsfInputData[3] __attribute__((aligned(32)));

static uint64_t t0 = 0;
TimerHandle_t g_sensorCollectTimer = NULL;

// Current buffer used by timer
volatile static int writerBuffNum = 0;
// Current buffer used by dequeue
volatile static int readerBuffNum = 0;
// Position in the writer buffer
static int stridePos = 0;
// Position in the validation data
static int sampleNum = 0;

static char *validation_data = NULL;
static unsigned long validation_size;

uint64_t TIMER_GetTimeInUS();

//Collect one sample and put it in one of the 3 buffers
static void SENSOR_Collect_TimerCB(TimerHandle_t xTimer)
{
	static uint64_t ts_us;

	ts_us = TIMER_GetTimeInUS() - t0;
    //PRINTF("%d: write sample %d to buffer [%d][%d]\n", (int)(ts_us/1000), sampleNum, writerBuffNum, (int)stridePos);

	for (int i = 0; i < CLSF_CHANNELS; i++)
	{
		memcpy(&g_clsfInputData[writerBuffNum].data[CLSF_CHANNELS * stridePos + i], validation_data + (sizeof(float)) * (i * validation_size + sampleNum), sizeof(float));
	}
	g_clsfInputData[writerBuffNum].ts_us[stridePos] = ts_us;

    stridePos++;
	if (stridePos == CLSF_STRIDE) {
		stridePos = 0;
		writerBuffNum = (writerBuffNum + 1) % 3;
	}

    sampleNum++;
    if (sampleNum == validation_size) {
    	sampleNum = 0;
    }

	return;
}

hal_StaticAccelerometer_status_t HAL_StaticAccelData_Init(static_accelerometer_t *elt, mpp_static_accel_params_t *config, void *param, unsigned long size)
{
	hal_StaticAccelerometer_status_t ret = MPP_kStatus_HAL_StaticAccelSuccess;

    HAL_LOGD("++HAL_StaticAccelData_Init\n");
    elt->config.width = config->width;
    elt->config.height = config->height;
    elt->config.channel = config->channel;
    elt->config.frequency = config->frequency;
    elt->buffer = param;
    HAL_LOGD("--HAL_StaticAccelData_Init\n");

    validation_data = param;
    validation_size = size;

	t0 = TIMER_GetTimeInUS();
    uint32_t timer_ms = 1000/elt->config.frequency;
    g_sensorCollectTimer = xTimerCreate("g_sensorCollectTimer", pdMS_TO_TICKS(timer_ms), pdTRUE, ( void * ) 0, SENSOR_Collect_TimerCB);
    if (NULL == g_sensorCollectTimer)
    {
        //PRINTF("collect timer create failed!\r\n");
        return MPP_kStatus_HAL_StaticAccelError;
    }
    else
    {
        if (xTimerStart(g_sensorCollectTimer, 0) != pdPASS )
        {
            //PRINTF("collect timer start failed!\r\n");
            return MPP_kStatus_HAL_StaticAccelError;
        }
    }

    return ret;
}

//TODO: add dequeue size
/* Starting mechanism: needs two 64 buffers */
hal_StaticAccelerometer_status_t HAL_StaticAccelData_Dequeue(const static_accelerometer_t *elt, hw_buf_desc_t *out_buf, int channel)
{
	hal_StaticAccelerometer_status_t ret = MPP_kStatus_HAL_StaticAccelSuccess;
	// static_accelerometer_static_config_t config = elt->config;
	// int slide_window = config.height * config.channel;
    static int windowNum = 0;
    static int dequeue = 0;
	uint64_t ts_us __attribute__((unused));

    int initialReader __attribute__((unused)) = readerBuffNum;
    ts_us = TIMER_GetTimeInUS() - t0;

    /* First dequeue is called with buffers empty */
    if (dequeue == 0) {
    	//PRINTF ("%d: busy dequeue, window number %d \n", (int)(ts_us/1000), windowNum);
    	dequeue = 1;
    	return MPP_kStatus_HAL_StaticAccelError;
    }

    /* Second dequeue is called with second buffer empty */
    if (dequeue == 1) {
    	//PRINTF ("%d: busy dequeue, window number %d \n", (int)(ts_us/1000), windowNum);
    	dequeue = 2;
    	return MPP_kStatus_HAL_StaticAccelError;
    }

	windowNum++;

	int size = CLSF_STRIDE * CLSF_CHANNELS * sizeof(float);

    // Wait for writerBuffNum to be full
    while (writerBuffNum == readerBuffNum) {}
	memcpy(out_buf->addr, g_clsfInputData[readerBuffNum].data, size);
	readerBuffNum = (readerBuffNum + 1) % 3;

#if (HAL_ENABLE_SD_LOG_EXT == 1)
    // PRINTF("%d: dequeue window %d from buffer [%d]\n", (int)(ts_us/1000), windowNum, initialReader);
#else

    // Wait for writerBuffNum to be full
    while (writerBuffNum == readerBuffNum) {}
	memcpy(out_buf->addr + size, g_clsfInputData[readerBuffNum].data, size);
	// Do not advance to overlap one buffer next time

    PRINTF("%d:\n", windowNum);
    //PRINTF("%d: dequeue window %d from buffer [%d] and [%d]\n", (int)(ts_us/1000), windowNum, initialReader, readerBuffNum);
#endif

	HAL_LOGI("--HAL_STATIC_ACCELEROMETER_Dequeue\n");
	return ret;
}

const static static_accelerometer_operator_t static_AccelerometerData_ops = {
    .init        = HAL_StaticAccelData_Init,
    .dequeue     = HAL_StaticAccelData_Dequeue,
};

int setup_static_accelerometer(static_accelerometer_t *elt)
{
    elt->ops = &static_AccelerometerData_ops;
    return 0;
}

