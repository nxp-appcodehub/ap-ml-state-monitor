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
#include "mpp_config.h"

#ifdef ENABLE_ACCELEROMETRE_Fxls8974
#include <FreeRTOS.h>
#include <task.h>
#include <stdlib.h>
#include <stdio.h>
#include "fsl_device_registers.h"
#include "issdk_hal.h"
#include "fxls8974_drv.h"
#include "board.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "hal_accelerometer_dev.h"
#include "hal.h"
#include "hal_utils.h"
#include "frdm_stbi_a8974_shield.h"
#include "timers.h"
#include "app_config.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__cplusplus)
}
#endif


/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define SENSOR_COLLECT_QUEUE_ITEMS  200     // Queue to hold samples
#define FXLS8974_DATA_SIZE 6

/*******************************************************************************
 * Constants
 ******************************************************************************/

/*! @brief Register settings for Normal (non buffered) mode. */
const registerwritelist_t fxls8974_Config_Normal[] = {
		 /* Set Full-scale range as 2G. */
		    {FXLS8974_SENS_CONFIG1, FXLS8974_SENS_CONFIG1_FSR_2G, FXLS8974_SENS_CONFIG1_FSR_MASK},
		    /* Set Wake Mode ODR Rate as 200Hz. */
			{FXLS8974_SENS_CONFIG3,FXLS8974_SENS_CONFIG3_WAKE_ODR_200HZ, FXLS8974_SENS_CONFIG3_WAKE_ODR_MASK},
		    __END_WRITE_DATA__};

/*! Command definition to read the Data Ready Status */
static const registerreadlist_t FXLS8974_STATUS_READ[] = {
    {.readFrom = FXLS8974_INT_STATUS, .numBytes = 1},
    __END_READ_DATA__
};

/*! @brief Address of Raw Accel Data in Normal Mode. */
static const registerreadlist_t FXLS8974_ACCEL_READ[] = {
    {.readFrom = FXLS8974_OUT_X_LSB, .numBytes = FXLS8974_DATA_SIZE},
    __END_READ_DATA__
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
status_t HAL_ACCELEROMETER_GetOneFrame(fxls8974_acceldata_t *rawDataAccel);
hal_accelerometer_status_t HAL_AccelerometerDev_Fxls8974_Enqueue(const accelerometer_dev_t *dev, void *data);
/*******************************************************************************
 * Variables
 ******************************************************************************/
// Current buffer used by timer
volatile static int writerBuffNum = 0;
// Current buffer used by dequeue
volatile static int readerBuffNum = 0;
// Position in the writer buffer
static int stridePos = 0;
// Position in the validation data
static int sampleNum = 0;

typedef struct  {
	uint32_t ts_ms[CLSF_STRIDE];
	float data[CLSF_STRIDE*CLSF_CHANNELS];
} acc_samples_t;

// 3 buffers for sliding window: two for current window and one for next
static acc_samples_t g_clsfInputData[3] __attribute__((aligned(32)));
static TimerHandle_t g_sensorCollectTimer = NULL;
static uint64_t t0;
static uint8_t dataAccel[FXLS8974_DATA_SIZE];
static ARM_DRIVER_I2C *I2Cdrv = &I2C_S_DRIVER; // Now using the shield.h value!!!
static fxls8974_i2c_sensorhandle_t FXLS8974drv;

/*******************************************************************************
 * Code
 ******************************************************************************/
//Collect one sample and put it in one of the 3 buffers
static void SENSOR_Collect_TimerCB(TimerHandle_t xTimer)
{
	float sens_val = 0 ;
	fxls8974_acceldata_t rawDataAccel;
#if PRINT_SENSOR_DATA
    static char log[100];
    static uint8_t logSize = sizeof(log);
#endif

    HAL_ACCELEROMETER_GetOneFrame(&rawDataAccel);

    uint32_t timestamp = (uint32_t)((TIMER_GetTimeInUS() - t0)/1000);
	for (int i = 0; i < CLSF_CHANNELS; i++)
	{
		sens_val = (float)*(&rawDataAccel.accel[0] + i);
		g_clsfInputData[writerBuffNum].data[CLSF_CHANNELS * stridePos + i] = sens_val;
	}
	g_clsfInputData[writerBuffNum].ts_ms[stridePos] = timestamp;

#if PRINT_SENSOR_DATA
	snprintf(log, logSize, "%ld: %ld, %ld, %ld\r\n", timestamp, (int32_t)rawDataAccel.accel[0], (int32_t)rawDataAccel.accel[1], (int32_t)rawDataAccel.accel[2]);
	PRINTF(log);
#endif

    stridePos++;
	if (stridePos == CLSF_STRIDE) {
		stridePos = 0;
		writerBuffNum = (writerBuffNum + 1) % 3;
	}

    sampleNum++;

	return;
}

hal_accelerometer_status_t HAL_AccelerometerDev_Fxls8974_Init(
   accelerometer_dev_t *dev, mpp_accelerometer_params_t *config, accelerometer_dev_callback_t callback, void *param)
{
	uint8_t whoami;
	int32_t status;
    hal_accelerometer_status_t ret = kStatus_HAL_AccelerometerSuccess;
    HAL_LOGD("++HAL_AccelerometerDev_Fxls8974_Init( param[%p]\n", param);

    dev->config.width = config->width;
    dev->config.height = config->height;
    dev->config.pitch = config->channel;
    dev->config.frequency = config->frequency;

    /* Only FXLS8974_SENS_CONFIG3_WAKE_ODR_200HZ supported now */
    if (dev->config.frequency != 200) {
    	return kStatus_HAL_AccelerometerError;
    }

    /*! Initialize the I2C driver. */
    status = I2Cdrv->Initialize(I2C_S_SIGNAL_EVENT);
    if (ARM_DRIVER_OK != status)
    {
        PRINTF("\r\n I2C Initialization Failed\r\n");
        return kStatus_HAL_AccelerometerError;
    }

    /*! Set the I2C Power mode. */
    status = I2Cdrv->PowerControl(ARM_POWER_FULL);
    if (ARM_DRIVER_OK != status)
    {
        PRINTF("\r\n I2C Power Mode setting Failed\r\n");
        return kStatus_HAL_AccelerometerError;
    }

    /*! Set the I2C bus speed. */
    status = I2Cdrv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
    if (ARM_DRIVER_OK != status)
    {
        PRINTF("\r\n I2C Control Mode setting Failed\r\n");
        return kStatus_HAL_AccelerometerError;
    }

#if defined(CPU_MIMXRT1052DVL6B) || defined(CPU_MIMXRT1062DVL6A) || defined(CPU_MIMXRT1064DVL6A)
    /* Init output RST GPIO. This is quick fix for FRDM_STBC_AGM01 REV B board*/
    GPIO_PinWrite(GPIO1, 21, 1);
#endif


    /*! Initialize the FXLS8974 sensor driver. */
    status = FXLS8974_I2C_Initialize(&FXLS8974drv, &I2C_S_DRIVER, I2C_S_DEVICE_INDEX, FXLS8974_I2C_ADDR,
    		&whoami);
    if (SENSOR_ERROR_NONE != status)
    {
        PRINTF("\r\n FXLS8974 Sensor Initialization Failed\r\n");
        return kStatus_HAL_AccelerometerError;
    }

    /*!  Set the task to be executed while waiting for I2C transactions to complete. */
    FXLS8974_I2C_SetIdleTask(&FXLS8974drv, (registeridlefunction_t)NULL, SMC);

    /*! Configure the FXLS8974 sensor driver. */
    status = FXLS8974_I2C_Configure(&FXLS8974drv, fxls8974_Config_Normal);
    if (SENSOR_ERROR_NONE != status)
    {
        PRINTF("\r\n FXLS8974 Sensor Configuration Failed, Err = %d\r\n", (int)status);
        return kStatus_HAL_AccelerometerError;
    }

    HAL_LOGD("--HAL_AccelerometerDev_Fxls8974_Init\n");
    return ret;
}

hal_accelerometer_status_t HAL_AccelerometerDev_Fxls8974_Getbufdesc(const accelerometer_dev_t *dev, hw_buf_desc_t *out_buf, mpp_memory_policy_t *policy)
{
    hal_accelerometer_status_t ret = kStatus_HAL_AccelerometerSuccess;
    HAL_LOGD("++HAL_AccelerometerDev_Fxls8974_Getbufdesc(out_buf=[%p])\n", out_buf);
    do
    {
        if ((out_buf == NULL) || (policy == NULL))
        {
            HAL_LOGE("\nNULL pointer to buffer descriptor\n");
            ret = kStatus_HAL_AccelerometerError;
            break;
        }
        /* set memory policy */
        *policy = HAL_MEM_ALLOC_NONE;
        out_buf->alignment = 32;
        out_buf->cacheable = false;
        out_buf->stride = dev->config.pitch;

    } while (false);
    HAL_LOGD("--HAL_AccelerometerDev_Fxls8974_Getbufdesc\n");
    return ret;
}

hal_accelerometer_status_t HAL_AccelerometerDev_Fxls8974_Deinit(accelerometer_dev_t *dev)
{
    hal_accelerometer_status_t ret = kStatus_HAL_AccelerometerSuccess;
    return ret;
}

status_t HAL_ACCELEROMETER_GetOneFrame(fxls8974_acceldata_t *rawDataAccel)
{
    status_t status      = kStatus_Success;
    uint8_t dataReady[1] = {0};

#if 1
    /*! Wait for data ready from the FXLS8974. */
    do
    {
        status = FXLS8974_I2C_ReadData(&FXLS8974drv, FXLS8974_STATUS_READ, dataReady);
    } while (0 == (dataReady[0] & FXLS8974_INT_STATUS_SRC_DRDY_MASK));

    /*! Read the raw sensor data from the FXLS8974. */
    status = FXLS8974_I2C_ReadData(&FXLS8974drv, FXLS8974_ACCEL_READ, dataAccel);
    if (ARM_DRIVER_OK != status)
    {
        PRINTF("\r\n FXLS8974 Read Failed. \r\n");
        return status;
    }
#endif

    /*! Convert the raw sensor data to signed 16-bit container for display to the debug port. */
    rawDataAccel->accel[0] = ((int16_t)dataAccel[1] << 8) | dataAccel[0];
    rawDataAccel->accel[1] = ((int16_t)dataAccel[3] << 8) | dataAccel[2];
    rawDataAccel->accel[2] = ((int16_t)dataAccel[5] << 8) | dataAccel[4];

    return status;
}
hal_accelerometer_status_t HAL_AccelerometerDev_Fxls8974_Start(const accelerometer_dev_t *dev)
{
    status_t status      = kStatus_Success;

    HAL_LOGD("++HAL_AccelerometerDev_Fxls8974_Start\n");

    t0 = TIMER_GetTimeInUS();
    uint32_t timer_ms = 1000/dev->config.frequency;
    g_sensorCollectTimer = xTimerCreate("g_sensorCollectTimer", pdMS_TO_TICKS(timer_ms), pdTRUE, ( void * ) 0, SENSOR_Collect_TimerCB);
	if (NULL == g_sensorCollectTimer)
	{
		PRINTF("collect timer create failed!\r\n");
		return kStatus_HAL_AccelerometerError;
	}
	else
	{
		if (xTimerStart(g_sensorCollectTimer, 0) != pdPASS )
		{
			PRINTF("collect timer start failed!\r\n");
			return kStatus_HAL_AccelerometerError;
		}
	}

    HAL_LOGD("--HAL_AccelerometerDev_Fxls8974_Start\n");
    return status;
}


hal_accelerometer_status_t HAL_AccelerometerDev_Fxls8974_Stop(const accelerometer_dev_t *dev)
{
    hal_accelerometer_status_t ret = kStatus_HAL_AccelerometerSuccess;
    status_t status = kStatus_Success;
    HAL_LOGD("++\n");
    /* TO DO */
    if (status != kStatus_Success) {
        HAL_LOGE("Error with status=%d\n", (int)status);
        ret = kStatus_HAL_AccelerometerError;
    }
    HAL_LOGD("--\n");
    return ret;
}

hal_accelerometer_status_t HAL_AccelerometerDev_Fxls8974_Dequeue(const accelerometer_dev_t *dev, hw_buf_desc_t *out_buf, int format)
{
	//static uint64_t ts_us;
    static int windowNum = 0;
    static int dequeue = 0;
    hal_accelerometer_status_t ret = kStatus_HAL_AccelerometerSuccess;

    HAL_LOGD("++HAL_AccelerometerDev_Fxls8974_Dequeue\n");

    /* First dequeue is called with buffers empty */
    if (dequeue == 0) {
    	//PRINTF ("%d: busy dequeue, window number %d \n", (int)(ts_us/1000), windowNum);
    	dequeue = 1;
    	return kStatus_HAL_AccelerometerError;
    }

    /* Second dequeue is called with second buffer empty */
    if (dequeue == 1) {
    	//PRINTF ("%d: busy dequeue, window number %d \n", (int)(ts_us/1000), windowNum);
    	dequeue = 2;
    	return kStatus_HAL_AccelerometerError;
    }

    //int initialReader = readerBuffNum;
    //ts_us = TIMER_GetTimeInUS() - t0;
	windowNum++;

	int size = CLSF_STRIDE * CLSF_CHANNELS * sizeof(float);

    while (writerBuffNum == readerBuffNum) {}
    memcpy(out_buf->addr, g_clsfInputData[readerBuffNum].data, size);


#if (HAL_ENABLE_SD_LOG_EXT == 1)

	//PRINTF("%d: dequeue window %d from buffer [%d]\n", (int)(ts_us/1000), windowNum, initialReader);
	memcpy(out_buf->addr + size, g_clsfInputData[readerBuffNum].ts_ms, CLSF_STRIDE* sizeof(float));
	readerBuffNum = (readerBuffNum + 1) % 3;

	ret = kStatus_HAL_AccelerometerSuccess;
	HAL_LOGD("--HAL_AccelerometerDev_Fxls8974_Dequeue\n");
	return ret;
#else
	readerBuffNum = (readerBuffNum + 1) % 3;
	// Wait for writerBuffNum to be full
        while (writerBuffNum == readerBuffNum) {}
        // Do not advance to overlap one buffer next time
        memcpy(out_buf->addr + size, g_clsfInputData[readerBuffNum].data, size);
        //PRINTF("%d: dequeue window %d from buffer [%d] and [%d]\n", (int)(ts_us/1000), windowNum, initialReader, readerBuffNum);

#endif

		ret = kStatus_HAL_AccelerometerSuccess;
	    HAL_LOGD("--HAL_AccelerometerDev_Fxls8974_Dequeue\n");

    return ret;
}

const static accelerometer_dev_operator_t accelerometer_dev_fxls8974_ops = {
    .init        = HAL_AccelerometerDev_Fxls8974_Init,
    .deinit      = HAL_AccelerometerDev_Fxls8974_Deinit,
    .start       = HAL_AccelerometerDev_Fxls8974_Start,
    .stop        = HAL_AccelerometerDev_Fxls8974_Stop,
    .enqueue     = HAL_AccelerometerDev_Fxls8974_Enqueue,
    .dequeue     = HAL_AccelerometerDev_Fxls8974_Dequeue,
    .get_buf_desc = HAL_AccelerometerDev_Fxls8974_Getbufdesc,
};


hal_accelerometer_status_t HAL_AccelerometerDev_Fxls8974_Enqueue(const accelerometer_dev_t *dev, void *data)
{
    int error = 0;
    HAL_LOGD("++HAL_AccelerometerDev_Fxls8974_Enqueue\n");
    /* nothing to do, see HAL_AccelerometerDev_Fxls8974_Dequeue() */
    HAL_LOGD("--HAL_AccelerometerDev_Fxls8974_Enqueue\n");
    return error;
}

int accelerometer_sim_setup(const char *name, accelerometer_dev_t *dev, _Bool defconfig)
{
    dev->ops = &accelerometer_dev_fxls8974_ops;

    return 0;
}

#endif /* ENABLE_ACCELEROMETRE_Fxls8974 */
