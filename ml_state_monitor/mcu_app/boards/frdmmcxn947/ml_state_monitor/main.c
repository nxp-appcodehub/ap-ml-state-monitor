/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Board includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_power.h"
#include "frdmmcxn947.h"
#include "systick_utils.h"

/* Sensor and model includes. */
#include "sensor_collect.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
int main(void)
{
    /*! Initialize the MCU hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_SystickEnable();
    BOARD_InitDebugConsole();

    printf("\r\nStarting Application...\r\n");
    printf("CPU: %ld Hz\r\n", CLOCK_GetFreq(kCLOCK_CoreSysClk));

    if (xTaskCreate(MainTask, "MainTask", 1024, NULL, configMAX_PRIORITIES - 1, NULL) != pdPASS)
    {
        printf("Task creation failed!.\r\n");
        while (1);
    }

    vTaskStartScheduler();
    while (1);
}
