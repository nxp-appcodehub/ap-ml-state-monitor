/*
 * Copyright 2021, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sensor_collect.h"
#include "sensor_raw.h"
#include "capture.h"
#if SENSOR_COLLECT_ACTION == SENSOR_COLLECT_LOG_EXT && SENSOR_COLLECT_LOG_EXT_SDCARD
#include "sdcard_capture.h"
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "fsl_common.h"
#include "fsl_debug_console.h"

#include "model_configuration.h"
#include "labels.h"

#if SENSOR_COLLECT_ACTION == SENSOR_COLLECT_LOG_EXT
const char *g_SensorCollectLabel = 0;
static uint64_t g_SensorCollectDuration_us = 0;
static uint64_t g_SensorCollectDuration_samples = 0;

#elif SENSOR_COLLECT_ACTION == SENSOR_COLLECT_RUN_INFERENCE
#if SENSOR_FEED_VALIDATION_DATA
#include "vdset.h"

#if STATIC_ACCEL_VDSET == VDSET_CLOG
static const float *vdset_ptr = &vdset_clog[0][0];
static const unsigned long vdset_samples = vdset_clog_samples;
#elif STATIC_ACCEL_VDSET == VDSET_FRICTION
static const float *vdset_ptr = &vdset_friction[0][0];
static const unsigned long vdset_samples = vdset_friction_samples;
#elif STATIC_ACCEL_VDSET == VDSET_ON
static const float *vdset_ptr = &vdset_on[0][0];
static const unsigned long vdset_samples = vdset_on_samples;
#elif STATIC_ACCEL_VDSET == VDSET_OFF
static const float *vdset_ptr = &vdset_off[0][0];
static const unsigned long vdset_samples = vdset_off_samples;
#endif /* STATIC_ACCEL_VDSET */
#endif /* SENSOR_FEED_VALIDATION_DATA */

#if SENSOR_COLLECT_RUN_INFENG == SENSOR_COLLECT_INFENG_TENSORFLOW
#include "tfmodel.h"
status_t (*SNS_MODEL_Init)(void) = &TFMODEL_Init;
status_t (*SNS_MODEL_RunInference)(void *inputData, size_t size, int8_t *predClass, int32_t *tinf_us, uint8_t verbose) =
         &TFMODEL_RunInference;

#elif SENSOR_COLLECT_RUN_INFENG == SENSOR_COLLECT_INFENG_DEEPVIEWRT
#include "rtmmodel.h"
status_t (*SNS_MODEL_Init)(void) = &RTMMODEL_Init;
status_t (*SNS_MODEL_RunInference)(void *inputData, size_t size, int8_t *predClass, int32_t *tinf_us, uint8_t verbose) =
         &RTMMODEL_RunInference;

#elif SENSOR_COLLECT_RUN_INFENG == SENSOR_COLLECT_INFENG_GLOW
#include "glowmodel.h"
status_t (*SNS_MODEL_Init)(void) = &GLOWMODEL_Init;
status_t (*SNS_MODEL_RunInference)(void *inputData, size_t size, int8_t *predClass, int32_t *tinf_us, uint8_t verbose) =
         &GLOWMODEL_RunInference;
#endif /* SENSOR_COLLECT_RUN_INFENG */

static float g_clsfInputData[CLSF_CHANNELS * CLSF_WINDOW] __attribute__((aligned(32)));
#endif /* SENSOR_COLLECT_ACTION */

QueueHandle_t g_sensorCollectQueue = NULL;
TimerHandle_t g_sensorCollectTimer = NULL;

typedef struct _sensor_data
{
    uint64_t sampleNum;
    uint64_t ts_us;
    int16_t rawDataSensor[3];
    int8_t temperature;
} sensor_data_t;

#if SENSOR_EVALUATE_MODEL
typedef struct _model_validation
{
    uint8_t readingInput;           // Flag to notify when reading data from the console
    uint8_t classTarget;            // Target class
    uint16_t predCount;             // Counter for total predictions
    uint16_t predCountOk;           // Counter for cases predicted OK
    uint16_t predSize;              // Total pool size for predictions
} model_validation_t;
static model_validation_t validation;
#endif

uint64_t TIMER_GetTimeInUS()
{
    uint64_t us = ((SystemCoreClock / configTICK_RATE_HZ) - SysTick->VAL) / (SystemCoreClock / 1000000);
    us += (uint64_t) xTaskGetTickCount() * portTICK_PERIOD_MS * 1000;
    return us;
}

int64_t os_clock_now()
{
    return ((int64_t) TIMER_GetTimeInUS()) * (int) 1e3;
}

#if SENSOR_COLLECT_ACTION == SENSOR_COLLECT_LOG_EXT
void SENSOR_Collect_LogExt_Task(void *pvParameters)
{
    sensor_data_t sensorData;

    printf("Start collecting data ...\r\n");

    static char buf[100];
    static uint8_t bufSize = sizeof(buf);
    static uint8_t bufSizeLog = 0;

    bufSizeLog = snprintf(buf, bufSize, "class,time[ms],Ax,Ay,Az\r\n");

#if SENSOR_COLLECT_LOG_EXT_SDCARD
    SDCARD_CaptureData(0, 0, 0, buf, bufSizeLog);
#else
    printf("%.*s", bufSizeLog, buf);
#endif

    while (1)
    {
        if (NULL != g_sensorCollectQueue && xQueueReceive(g_sensorCollectQueue, &sensorData, portMAX_DELAY) == pdPASS )
        {
            bufSizeLog = snprintf(buf, bufSize, "%s,%ld,%d,%d,%d\r\n", g_SensorCollectLabel, (uint32_t)(sensorData.ts_us/1000),
                    sensorData.rawDataSensor[0], sensorData.rawDataSensor[1], sensorData.rawDataSensor[2]);

#if SENSOR_COLLECT_LOG_EXT_SDCARD
                SDCARD_CaptureData(sensorData.ts_us, sensorData.sampleNum, g_SensorCollectDuration_samples, buf, bufSizeLog);
#else
                printf("%.*s", bufSizeLog, buf);
#endif
        }
    }

    vTaskDelete(NULL);
}
#elif SENSOR_COLLECT_ACTION == SENSOR_COLLECT_RUN_INFERENCE
#if SENSOR_EVALUATE_MODEL
void EvaluateModel_ReadInput()
{
    uint8_t i;

    validation.predCountOk = 0;
    validation.predCount = 0;

    printf("\r\nModel Evaluation:\r\n");
    printf("Class to evaluate (provide only the numeric index):\r\n");
    printf("( ");
    for (i = 0; i < sizeof(labels)/sizeof(labels[0]); i++)
    {
        printf("%d:%s ", i, labels[i]);
    }
    printf(")\r\n");
    printf("\t>>> ");
    fflush(stdout);
    scanf("%hhd", &validation.classTarget);

    printf("\r\nPool size (total number of predictions to compute):\r\n");
    printf("\t>>> ");
    fflush(stdout);
    scanf("%hd", &validation.predSize);

    printf("\r\n\r\n");
}
#endif

void SENSOR_Collect_RunInf_Task(void *pvParameters)
{
    status_t status = kStatus_Success;
    sensor_data_t sensorData;
    int32_t tinf_us;
    uint8_t predClass;
    float sens_val;
    static uint16_t clsfSampIdx = 0;
    static uint32_t tinfTotal_us = 0;

    printf("SENSOR_Collect_Task started\r\n");

    status = SNS_MODEL_Init();
    if (kStatus_Success != status)
    {
        printf("[ERROR] Failed to initialize the model\r\n");
        goto exit_task;
    }

    while (1)
    {
#if SENSOR_EVALUATE_MODEL
        if (validation.predCount >= validation.predSize)
        {
            validation.readingInput = 1;
            EvaluateModel_ReadInput();
            validation.readingInput = 0;
        }
#endif
        if (NULL != g_sensorCollectQueue && xQueueReceive(g_sensorCollectQueue, &sensorData, portMAX_DELAY) == pdPASS )
        {
            /* Copy data into the storing buffer */
            for (int i = 0; i < CLSF_CHANNELS; i++)
            {
#if SENSOR_RAW_DATA_NORMALIZE
                sens_val = (((float)*(&sensorData.rawDataSensor[0] + i))-model_mean[i])/model_std[i];
#else
                sens_val = *(&sensorData.rawDataSensor.accel[0] + i);
#endif

#if SENSOR_COLLECT_DATA_FORMAT == SENSOR_COLLECT_DATA_FORMAT_INTERLEAVED
                g_clsfInputData[CLSF_CHANNELS * clsfSampIdx + i] = sens_val;
#elif SENSOR_COLLECT_DATA_FORMAT == SENSOR_COLLECT_DATA_FORMAT_BLOCKS
                g_clsfInputData[i * CLSF_WINDOW + clsfSampIdx] = sens_val;
#endif
            }

            if (++clsfSampIdx >= CLSF_WINDOW)
            {
                /* Run Inference */
                tinf_us = 0;
                SNS_MODEL_RunInference((void*)g_clsfInputData, sizeof(g_clsfInputData), (int8_t*)&predClass, &tinf_us, SENSOR_COLLECT_INFENG_VERBOSE_EN);

                /* Copy the samples to be retained */
#if SENSOR_COLLECT_DATA_FORMAT == SENSOR_COLLECT_DATA_FORMAT_INTERLEAVED
                memcpy(&g_clsfInputData[0],
                       &g_clsfInputData[CLSF_CHANNELS * (CLSF_WINDOW - CLSF_OFFSET)],
                       CLSF_CHANNELS * CLSF_OFFSET * sizeof(g_clsfInputData[0]));
#elif SENSOR_COLLECT_DATA_FORMAT == SENSOR_COLLECT_DATA_FORMAT_BLOCKS
                for (int i = 0; i < CLSF_CHANNELS; i++)
                {
                    memcpy(&g_clsfInputData[i * CLSF_WINDOW],
                           &g_clsfInputData[i * CLSF_WINDOW + (CLSF_WINDOW - CLSF_OFFSET)],
                           CLSF_OFFSET * sizeof(g_clsfInputData[0]));
                }
#endif
                clsfSampIdx = CLSF_OFFSET;

#if SENSOR_EVALUATE_MODEL
                /* Evaluate performance */
                validation.predCount++;
                if (validation.classTarget == predClass)
                {
                    validation.predCountOk++;
                }
                PRINTF("\rInference %d?%d | t %ld us | count %d/%d/%d | %s      ",
                       validation.classTarget, predClass, tinf_us, validation.predCountOk,
                       validation.predCount, validation.predSize, labels[predClass]);

                tinfTotal_us += tinf_us;
                if (validation.predCount >= validation.predSize)
                {
                    printf("\r\nPrediction Accuracy for class %s %.2f%%\r\n", labels[validation.classTarget],
                            (float)(validation.predCountOk * 100)/validation.predCount);
                    printf("Average Inference Time %.1f (us)\r\n", (float)tinfTotal_us/validation.predCount);
                    tinfTotal_us = 0;
                }
#else
                static uint32_t predCount = 0;
                PRINTF("\rInference run: t %ld us | count %d | [%d] %s      ",
                        tinf_us, predCount++, predClass, labels[predClass]);
#endif
            }
        }
    }

exit_task:
    vTaskDelete(NULL);
}
#endif  /* SENSOR_COLLECT_ACTION */

static status_t SENSOR_Collect_PushData(const sensor_data_t sensorData)
{
    if (!(NULL != g_sensorCollectQueue && xQueueSend(g_sensorCollectQueue, (void*)&sensorData, /*portMAX_DELAY*/0) == pdPASS))
    {
        printf("WARN Data Loss\r\n");
        return kStatus_Fail;
    }

    return kStatus_Success;
}

static void SENSOR_Collect_TimerCB(TimerHandle_t xTimer)
{
    static uint32_t num_dropped = 0;
    static uint8_t first_run = 1;
    static uint64_t t0 = 0;

    static sensor_data_t sensorData =
    {
        .sampleNum = 0,
    };

    if (first_run)
    {
        first_run = 0;
        t0 = TIMER_GetTimeInUS();
        return;
    }

#if SENSOR_FEED_VALIDATION_DATA
    for (int i = 0; i < CLSF_CHANNELS; i++)
    {
        *(&sensorData.rawDataSensor[0] + i) = *(vdset_ptr + i * vdset_samples + sensorData.sampleNum);
    }
    if (++sensorData.sampleNum >= vdset_samples)
    {
        sensorData.sampleNum = 0;
    }
#else
    SENSOR_Run(&sensorData.rawDataSensor);
    sensorData.sampleNum++;
    sensorData.ts_us = TIMER_GetTimeInUS() - t0;
#endif

#if SENSOR_EVALUATE_MODEL
    if (validation.readingInput)
    {
        sensorData.sampleNum--;
        return;
    }
#endif

    if (kStatus_Success != SENSOR_Collect_PushData(sensorData))
    {
        sensorData.sampleNum--;
        num_dropped++;
    }

#if SENSOR_COLLECT_ACTION == SENSOR_COLLECT_LOG_EXT
    if (sensorData.sampleNum >= g_SensorCollectDuration_samples)
    {
#if SENSOR_COLLECT_LOG_EXT_SDCARD
        while(uxQueueMessagesWaiting(g_sensorCollectQueue) != 0)
        {
            taskYIELD();
        }
        SDCARD_CaptureClose();
        printf("\r\nDone\r\n");
#endif
        if (num_dropped > 0)
        {
            printf("\r\nDropped %lu samples.\r\n", num_dropped);
        }
        while(1) vTaskDelay(pdMS_TO_TICKS(1000));
    }
#endif
}

void MainTask(void *pvParameters)
{
    status_t status = kStatus_Success;

    printf("MainTask started\r\n");

#if !SENSOR_FEED_VALIDATION_DATA
    status = SENSOR_Init();
    if (status != kStatus_Success)
    {
        goto main_task_exit;
    }
#endif

    g_sensorCollectQueue = xQueueCreate(SENSOR_COLLECT_QUEUE_ITEMS, sizeof(sensor_data_t));
    if (NULL == g_sensorCollectQueue)
    {
        printf("collect queue create failed!\r\n");
        status = kStatus_Fail;
        goto main_task_exit;
    }

#if SENSOR_COLLECT_ACTION == SENSOR_COLLECT_LOG_EXT
    uint8_t captClassLabelIdx;
    CAPT_Init(&captClassLabelIdx, &g_SensorCollectDuration_us, &g_SensorCollectDuration_samples);
    g_SensorCollectLabel = labels[captClassLabelIdx];

    if (xTaskCreate(SENSOR_Collect_LogExt_Task, "SENSOR_Collect_LogExt_Task", 4096, NULL, configMAX_PRIORITIES - 1, NULL) != pdPASS)
    {
        printf("SENSOR_Collect_LogExt_Task creation failed!\r\n");
        status = kStatus_Fail;
        goto main_task_exit;
    }
#elif SENSOR_COLLECT_ACTION == SENSOR_COLLECT_RUN_INFERENCE
    if (xTaskCreate(SENSOR_Collect_RunInf_Task, "SENSOR_Collect_RunInf_Task", 4096, NULL, configMAX_PRIORITIES - 1, NULL) != pdPASS)
    {
        printf("SENSOR_Collect_RunInf_Task creation failed!\r\n");
        status = kStatus_Fail;
        goto main_task_exit;
    }
#endif

#if SENSOR_FEED_VALIDATION_DATA
    while (1)
    {
        SENSOR_Collect_TimerCB(NULL);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
#else
    g_sensorCollectTimer = xTimerCreate("g_sensorCollectTimer", (configTICK_RATE_HZ / SENSOR_COLLECT_RATE_HZ), pdTRUE, ( void * ) 0, SENSOR_Collect_TimerCB);

    if (NULL == g_sensorCollectTimer)
    {
        printf("collect timer create failed!\r\n");
        status = kStatus_Fail;
        goto main_task_exit;
    }
    else
    {
        if (xTimerStart(g_sensorCollectTimer, 0) != pdPASS )
        {
            printf("collect timer start failed!\r\n");
            status = kStatus_Fail;
            goto main_task_exit;
        }
    }
#endif

main_task_exit:

    printf("MainTask exit, error status %ld\r\n", status);
    vTaskDelete(NULL);
}
