/*
 * Copyright 2021, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Description: This file implements the ISSDK FXLS8974 sensor driver.
 * It is used for reading data from FXLS8974 sensors.
 */

#include <stdio.h>

/*  SDK Includes */
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"

/* CMSIS Includes */
#include "Driver_I2C.h"

/* ISSDK Includes */
#include "issdk_hal.h"
#include "sensor/sensor_raw.h"

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************************
 * Macro Definitions
 ******************************************************************************/
#define FXLS8974_DATA_SIZE 6

/*******************************************************************************
 * Constants
 ******************************************************************************/
/*! @brief Register settings for Normal (non buffered) mode. */
const registerwritelist_t cFxls8974ConfigNormal[] = {
    /* Set Full-scale range as 2G. */
    {FXLS8974_SENS_CONFIG1, FXLS8974_SENS_CONFIG1_FSR_2G, FXLS8974_SENS_CONFIG1_FSR_MASK},
    /* Set Wake Mode ODR Rate as 400Hz. */
    {FXLS8974_SENS_CONFIG3, FXLS8974_SENS_CONFIG3_WAKE_ODR_400HZ, FXLS8974_SENS_CONFIG3_WAKE_ODR_MASK},
    __END_WRITE_DATA__};

/*! @brief Address of DATA Ready Status Register. */
const registerreadlist_t cFxls8974DRDYEvent[] = {{.readFrom = FXLS8974_INT_STATUS, .numBytes = 1}, __END_READ_DATA__};

/*! @brief Address of Raw Accel Data in Normal Mode. */
const registerreadlist_t cFxls8974OutputNormal[] = {{.readFrom = FXLS8974_OUT_X_LSB, .numBytes = FXLS8974_DATA_SIZE},
                                                    __END_READ_DATA__};

ARM_DRIVER_I2C *I2Cdrv = &I2C_S_DRIVER; // Now using the shield.h value!!!
fxls8974_i2c_sensorhandle_t fxls8974Driver;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
status_t SENSOR_Init(void)
{
    status_t status;
    uint8_t whoami;

    /*! Initialize FXLS8974CF */
    BOARD_InitFXLS8974();

    /*! Initialize the I2C driver. */
    status = I2Cdrv->Initialize(I2C_S_SIGNAL_EVENT);
    if (ARM_DRIVER_OK != status)
    {
        PRINTF("\r\n I2C Initialization Failed\r\n");
        return status;
    }

    /*! Set the I2C Power mode. */
    status = I2Cdrv->PowerControl(ARM_POWER_FULL);
    if (ARM_DRIVER_OK != status)
    {
        PRINTF("\r\n I2C Power Mode setting Failed\r\n");
        return status;
    }

    /*! Set the I2C bus speed. */
    status = I2Cdrv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
    if (ARM_DRIVER_OK != status)
    {
        PRINTF("\r\n I2C Control Mode setting Failed\r\n");
        return status;
    }

    /*! Initialize FXLS8974 sensor driver. */
    status = FXLS8974_I2C_Initialize(&fxls8974Driver, &I2C_S_DRIVER, I2C_S_DEVICE_INDEX, FXLS8974_I2C_ADDR,
                                     &whoami);
    if (ARM_DRIVER_OK != status)
    {
        PRINTF("\r\n Sensor Initialization Failed\r\n");
        return status;
    }
    if ((FXLS8964_WHOAMI_VALUE == whoami) || (FXLS8967_WHOAMI_VALUE == whoami))
    {
        // PRINTF("\r\n Successfully Initialized Gemini with WHO_AM_I = 0x%X\r\n", whoami);
    }
    else if ((FXLS8974_WHOAMI_VALUE == whoami) || (FXLS8968_WHOAMI_VALUE == whoami))
    {
        // PRINTF("\r\n Successfully Initialized Timandra with WHO_AM_I = 0x%X\r\n", whoami);
    }
    else if ((FXLS8971_WHOAMI_VALUE == whoami) || (FXLS8961_WHOAMI_VALUE == whoami))
    {
        // PRINTF("\r\n Successfully Initialized Chiron with WHO_AM_I = 0x%X\r\n", whoami);
    }
    else if (FXLS8962_WHOAMI_VALUE == whoami)
    {
        // PRINTF("\r\n Successfully Initialized Newstein with WHO_AM_I = 0x%X\r\n", whoami);
    }
    else
    {
        PRINTF("\r\n Bad WHO_AM_I = 0x%X\r\n", whoami);
        return -1;
    }

    /*! Configure the FXLS8974 sensor. */
    status = FXLS8974_I2C_Configure(&fxls8974Driver, cFxls8974ConfigNormal);
    if (ARM_DRIVER_OK != status)
    {
        PRINTF("\r\n FXLS8974 Sensor Configuration Failed, Err = %d\r\n", status);
        return status;
    }

    return status;
}

status_t SENSOR_Run(void *rawSensorData)
{
    status_t status      = kStatus_Success;
    uint8_t dataReady;
    uint8_t data[FXLS8974_DATA_SIZE];
    int16_t *rawDataAccel = rawSensorData;

    /*! Wait for data ready from the FXLS8974. */
    do
    {
        status = FXLS8974_I2C_ReadData(&fxls8974Driver, cFxls8974DRDYEvent, &dataReady);
    } while (0 == (dataReady & FXLS8974_INT_STATUS_SRC_DRDY_MASK));

    /*! Read new raw sensor data from the FXLS8974. */
    status = FXLS8974_I2C_ReadData(&fxls8974Driver, cFxls8974OutputNormal, data);
    if (ARM_DRIVER_OK != status)
    {
        PRINTF("\r\n Read Failed. \r\n");
        return status;
    }

    /*! Convert the raw sensor data to signed 16-bit container for display to the debug port. */
    rawDataAccel[0] = ((int16_t)data[1] << 8) | data[0];
    rawDataAccel[1] = ((int16_t)data[3] << 8) | data[2];
    rawDataAccel[2] = ((int16_t)data[5] << 8) | data[4];

    /* NOTE: PRINTF is relatively expensive in terms of CPU time, specially when used with-in execution loop. */
    // PRINTF("\r\n X=%5d Y=%5d Z=%5d\r\n", rawData.accel[0], rawData.accel[1], rawData.accel[2]);

    return status;
}
