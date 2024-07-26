/*
 * Copyright 2021, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>

#include "sdcard_capture.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_sd.h"
#include "fsl_sd_disk.h"
#include "ff.h"
#include "diskio.h"
#include "sdmmc_config.h"
#include "limits.h"
#include "labels.h"

#include "sensor_collect.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SDCARD_MKFS_EN              0               // Enable/Disable MKFS
#define SDCARD_CAPT_TSYNC_US        1000000         // The periodicity for capture sync

/*******************************************************************************
 * Variables
 ******************************************************************************/
static FATFS g_fileSystem; /* File system object */

/*! @brief SD card detect flag  */
static volatile bool s_cardInserted     = false;
static volatile bool s_cardInsertStatus = false;
/*! @brief Card semaphore  */
static SemaphoreHandle_t s_CardDetectSemaphore = NULL;

/*! @brief Pointer to the capture file on the SD card  */
static FIL g_fp;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief call back function for SD card detect.
 *
 * @param isInserted  true,  indicate the card is insert.
 *                    false, indicate the card is remove.
 * @param userData
 */
static void SDCARD_DetectCallBack(bool isInserted, void *userData);
static status_t SDCARD_MountFS(void);

static status_t SDCARD_AppendToFile(FIL *fp, char *buff, unsigned int btw);
static void SDCARD_SyncFile(FIL *fp);
static void SDCARD_CloseFile(FIL *fp);

/*******************************************************************************
 * Code
 ******************************************************************************/
static void SDCARD_DetectCallBack(bool isInserted, void *userData)
{
    s_cardInsertStatus = isInserted;
    xSemaphoreGiveFromISR(s_CardDetectSemaphore, NULL);
}

/*static*/ void SDCARD_Detect()
{
    s_CardDetectSemaphore = xSemaphoreCreateBinary();

    BOARD_SD_Config(&g_sd, SDCARD_DetectCallBack, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

    /* SD host init function */
    if (SD_HostInit(&g_sd) == kStatus_Success)
    {
        while (true)
        {
            /* take card detect semaphore */
            if (xSemaphoreTake(s_CardDetectSemaphore, portMAX_DELAY) == pdTRUE)
            {
                if (s_cardInserted != s_cardInsertStatus)
                {
                    s_cardInserted = s_cardInsertStatus;

                    if (s_cardInserted)
                    {
                        PRINTF("Card inserted\r\n");
                        /* power off card */
                        SD_SetCardPower(&g_sd, false);
                        /* power on the card */
                        SD_SetCardPower(&g_sd, true);
                        /* make file system */
                        if (SDCARD_MountFS() != kStatus_Success)
                        {
                            continue;
                        }
                        else
                        {
                            break;
                        }
                    }
                }

                if (!s_cardInserted)
                {
                    PRINTF("\r\nPlease insert a card into board.\r\n");
                }
            }
        }
    }
    else
    {
        PRINTF("\r\nSD host init fail\r\n");
    }
}

static status_t SDCARD_MountFS(void)
{
    FRESULT error;
    const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
#if FF_USE_MKFS && SDCARD_MKFS_EN
    BYTE work[FF_MAX_SS];
#endif

    if (f_mount(&g_fileSystem, driverNumberBuffer, 0U))
    {
        PRINTF("Mount volume failed.\r\n");
        return kStatus_Fail;
    }

#if (FF_FS_RPATH >= 2U)
    error = f_chdrive((char const *)&driverNumberBuffer[0U]);
    if (error)
    {
        PRINTF("Change drive failed.\r\n");
        return kStatus_Fail;
    }
#endif

#if FF_USE_MKFS && SDCARD_MKFS_EN
    PRINTF("\r\nMake file system......The time may be long if the card capacity is big.\r\n");
    if (f_mkfs(driverNumberBuffer, 0, work, sizeof work))
    {
        PRINTF("Make file system failed.\r\n");
        return kStatus_Fail;
    }
#endif /* FF_USE_MKFS */

    PRINTF("Mount file system succeeded\r\n");

    return kStatus_Success;
}

static status_t SDCARD_AppendToFile(FIL *fp, char *buff, unsigned int btw)
{
    UINT bytesWritten = 0U;
    FRESULT error;

    error = f_write(fp, buff, btw, &bytesWritten);
    if ((error) || (bytesWritten != btw))
    {
        PRINTF("Write file failed (%d, %d, %d)\r\n", error, btw, bytesWritten);
        return kStatus_Fail;
    }

    return kStatus_Success;
}

static void SDCARD_SyncFile(FIL *fp)
{
    f_sync(fp);
}

static void SDCARD_CloseFile(FIL *fp)
{
    f_close(fp);
}

uint8_t SDCARD_CheckExisting(char *fileName)
{
    uint8_t status = 1;
    FRESULT error;
    FIL fp;

    error = f_open(&fp, _T(fileName), FA_OPEN_EXISTING);
    if (error == FR_NO_FILE)
    {
        status = 0;
    }
    else if (error == FR_OK)
    {
        PRINTF("\tFile exists\r\n");
        status = 1;
    }
    else if (error == FR_INVALID_NAME)
    {
        PRINTF("\tFile name format is invalid\r\n");
        PRINTF("\t(max size 12 chrs)\r\n");
        PRINTF("\t(illegal chrs \"*+,:;<=>\?[]|\x7F)\r\n");
        status = 2;
    }
    else
    {
        PRINTF("\tFile error %d\r\n", error);
        status = 3;
    }
    f_close(&fp);

    return status;
}

status_t SDCARD_CaptureOpen(char *fileName)
{
    status_t status = kStatus_Success;
    FRESULT error;

    error = f_open(&g_fp, _T(fileName), FA_WRITE);
    if (error)
    {
        if (error == FR_EXIST)
        {
            PRINTF("File exists.\r\n");
        }
        /* if file not exists, create a new file */
        else if (error == FR_NO_FILE)
        {
            if (f_open(&g_fp, _T(fileName), (FA_WRITE | FA_CREATE_NEW)) != FR_OK)
            {
                PRINTF("Create file failed.\r\n");
                status = kStatus_Fail;
            }
        }
        else
        {
            PRINTF("Open file failed.\r\n");
            status = kStatus_Fail;
        }
    }
    return status;
}

void SDCARD_CaptureData(uint64_t ts_us, uint64_t sample_num, uint64_t totalDuration_samples, char *buf, uint32_t size)
{
    static uint64_t tsLastSync_us = 0;

    if (kStatus_Success == SDCARD_AppendToFile(&g_fp, buf, size))
    {
        printf("\rProgress: %.2f%% (%.2f/%ld)          ",
                (float)sample_num/totalDuration_samples * 100,
                (float)ts_us/60000000, (uint32_t)(totalDuration_samples/(60 * SENSOR_COLLECT_RATE_HZ)));

        /* Synchronize the file on the SD card */
        if (ts_us - tsLastSync_us >= SDCARD_CAPT_TSYNC_US)
        {
            SDCARD_SyncFile(&g_fp);
            tsLastSync_us = ts_us;
        }

        /* Close the file on the SD card */
        if ((sample_num >= totalDuration_samples) && (totalDuration_samples != 0))
        {
            SDCARD_CloseFile(&g_fp);
        }
    }
    else
    {
        PRINTF("SDCARD_LogData - failed to append\r\n");
    }
}

void SDCARD_CaptureClose()
{
    // SDCARD_CloseFile(&g_fp);
}
