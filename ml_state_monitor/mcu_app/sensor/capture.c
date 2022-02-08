/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "capture.h"
#include "sensor_collect.h"
#include "labels.h"
#if SENSOR_COLLECT_ACTION == SENSOR_COLLECT_LOG_EXT && SENSOR_COLLECT_LOG_EXT_SDCARD
#include "sdcard_capture.h"
#endif

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CAPT_DUR_MAX_MINUTES    (24 * 60) // The maximum duration for a capture

typedef enum
{
    CAPT_MENU_GetClassLabel,
    CAPT_MENU_GetDuration,
    CAPT_MENU_GetFileName,
    CAPT_MENU_ValidateConfiguration,
    CAPT_MENU_SelectionDone,
} capt_menu_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/
struct
{
    capt_menu_t menu;           // Current menu selection
    uint8_t classLabel;         // The class label to record (numeric index)
    uint16_t duration;          // The duration of the capture in minutes
#if SENSOR_COLLECT_LOG_EXT_SDCARD
    char fileName[20];          // The file name on the SD card
#endif
} g_capt;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void CAPT_GetCfg();

/*******************************************************************************
 * Code
 ******************************************************************************/
inline static void CAPT_MENU_INSERT_PROMPT()
{
    PRINTF("\t>>> ");
}

inline static uint8_t CAPT_MENU_CONFIRM()
{
    char selection;

    while (true)
    {
        PRINTF("\tDo you want to continue? [y/n] ");
        SCANF("%c", &selection);
        PRINTF("\r\n");

        if (selection == 'y')
        {
            return 1;
        }
        else if (selection == 'n')
        {
            return 0;
        }
    }
}

inline static uint8_t CAPT_MENU_CHECK_INRANGE(uint32_t x, uint32_t min, uint32_t max)
{
    if (min <= x && x <= max)
    {
        return 1;
    }
    return 0;
}

static void CAPT_GetCfg()
{
    uint32_t i;
    uint8_t countdown_sec = 5;

    g_capt.menu = CAPT_MENU_GetClassLabel;

    DbgConsole_Flush();
    PRINTF("\r\nProvide the required configuration to start the recording\r\n");
    while (g_capt.menu != CAPT_MENU_SelectionDone)
    {
        switch (g_capt.menu)
        {
            case CAPT_MENU_GetClassLabel:
                PRINTF("Class to record (provide only the numeric index):\r\n");
                PRINTF("( ");
                for (i = 0; i < sizeof(labels)/sizeof(labels[0]); i++)
                {
                    PRINTF("%d:%s ", i, labels[i]);
                }
                PRINTF(")\r\n");

                CAPT_MENU_INSERT_PROMPT();
                SCANF("%d", &g_capt.classLabel);
                PRINTF("\r\n\t%d\r\n", g_capt.classLabel);

                if (!CAPT_MENU_CHECK_INRANGE(g_capt.classLabel, 0, sizeof(labels)/sizeof(labels[0]) -1))
                {
                    PRINTF("\tUnsupported class\r\n");
                    break;
                }

                if (CAPT_MENU_CONFIRM())
                {
                    g_capt.menu = CAPT_MENU_GetDuration;
                }
                break;

            case CAPT_MENU_GetDuration:
                PRINTF("Duration in minutes:\r\n");

                CAPT_MENU_INSERT_PROMPT();
                SCANF("%d", &g_capt.duration);
                PRINTF("\r\n\t%d\r\n", g_capt.duration);

                if (!CAPT_MENU_CHECK_INRANGE(g_capt.duration, 1, CAPT_DUR_MAX_MINUTES))
                {
                    PRINTF("\tSupported duration [1, %d] minutes\r\n", CAPT_DUR_MAX_MINUTES);
                    break;
                }

                if (CAPT_MENU_CONFIRM())
                {
#if SENSOR_COLLECT_LOG_EXT_SDCARD
                    g_capt.menu = CAPT_MENU_GetFileName;
#else
                    g_capt.menu = CAPT_MENU_ValidateConfiguration;
#endif
                }
                break;

#if SENSOR_COLLECT_LOG_EXT_SDCARD
            case CAPT_MENU_GetFileName:
                PRINTF("SD card filename:\r\n");

                CAPT_MENU_INSERT_PROMPT();
                SCANF("%s", g_capt.fileName);
                PRINTF("\r\n\t%s\r\n", g_capt.fileName);

                if (SDCARD_CheckExisting(g_capt.fileName))
                {
                    break;
                }

                if (CAPT_MENU_CONFIRM())
                {
                    g_capt.menu = CAPT_MENU_ValidateConfiguration;
                }
                break;
#endif

            case CAPT_MENU_ValidateConfiguration:
                PRINTF("A new recording for this configuration will start:\r\n");
                PRINTF("\t%Class: %d-%s\r\n", g_capt.classLabel, labels[g_capt.classLabel]);
                PRINTF("\tDuration: %d minutes\r\n", g_capt.duration);
#if SENSOR_COLLECT_LOG_EXT_SDCARD
                PRINTF("\tFilename: %s\r\n", g_capt.fileName);
#endif

                if (CAPT_MENU_CONFIRM())
                {
                    g_capt.menu = CAPT_MENU_SelectionDone;
#if SENSOR_COLLECT_LOG_EXT_SDCARD
                    if (kStatus_Success != SDCARD_CaptureOpen(g_capt.fileName))
                    {
                        SDCARD_CaptureClose();
                        g_capt.menu = CAPT_MENU_GetFileName;
                        PRINTF("\tCannot open file\r\n");
                    }
#endif
                }
                else
                {
                    g_capt.menu = CAPT_MENU_GetClassLabel;
                }
                break;

            case CAPT_MENU_SelectionDone:
                break;

            default: break;
        }
        PRINTF("\r\n");
    }

    PRINTF("The recording will start in ");
    while (countdown_sec--)
    {
        PRINTF("%d ", countdown_sec + 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    PRINTF("\r\n");

}

void CAPT_Init(uint8_t *captClassLabelIdx, uint64_t *captDuration_us, uint64_t *captDuration_samples)
{
#if SENSOR_COLLECT_LOG_EXT_SDCARD
    PRINTF("SDCard initialization \r\n");
    PRINTF("(Please insert an SD card into the socket)\r\n");
    SDCARD_Detect();
#endif

    CAPT_GetCfg();
    *captClassLabelIdx = g_capt.classLabel;
    *captDuration_us = (uint64_t)g_capt.duration * 60000000;
    *captDuration_samples = (uint64_t)g_capt.duration * 60 * SENSOR_COLLECT_RATE_HZ;
}
