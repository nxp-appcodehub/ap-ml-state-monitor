/*
 * Copyright 2021 NXP
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
#include "fsl_sysmpu.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

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
    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    SYSMPU_Enable(SYSMPU, false);

    printf("\r\n\r\nStarting Application...\r\n");
    // printf("CPU: %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CoreSysClk));

    if (xTaskCreate(MainTask, "MainTask", 1024, NULL, configMAX_PRIORITIES - 1, NULL) !=
        pdPASS)
    {
        printf("Task creation failed!.\r\n");
        while (1)
            ;
    }

    vTaskStartScheduler();
    for (;;)
        ;
}
