/*
 * Copyright 2021-2022 NXP
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

/* Sensor and model includes. */
#include "sensor_collect.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_EnableIDCache(void);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Enable instruction and data caches.
 */
void BOARD_EnableIDCache(void)
{
#if defined(__ICACHE_PRESENT) && __ICACHE_PRESENT
    if (SCB_CCR_IC_Msk != (SCB_CCR_IC_Msk & SCB->CCR))
    {
        SCB_EnableICache();
    }
#endif
#if defined(__DCACHE_PRESENT) && __DCACHE_PRESENT
    if (SCB_CCR_DC_Msk != (SCB_CCR_DC_Msk & SCB->CCR))
    {
        SCB_EnableDCache();
    }
#endif
}

/*!
 * @brief Application entry point.
 */
int main(void)
{
    /* Init board hardware. */
#if !(defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1) && \
      defined(XIP_BOOT_HEADER_DCD_ENABLE) && (XIP_BOOT_HEADER_DCD_ENABLE == 1))
    BOARD_ConfigMPU();
#else
    BOARD_EnableIDCache();
#endif

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    printf("\r\n\r\nStarting Application...\r\n");
    // printf("CPU: %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));

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
