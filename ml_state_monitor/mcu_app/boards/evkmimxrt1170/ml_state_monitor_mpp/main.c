/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* @brief This example application shows usage of MultiMedia Pipeline library preview
 *        to build a graph with time series data from accelerometer and an NN model:
 *        accelerometer -> inference engine (model: tflight )
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "string.h"
#include "stdbool.h"

/* MCU includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "board_init.h"
#include "app_config.h"
#include "version.h"

/* MPP includes */
#include "mpp_api.h"

/* Models */
#include "model_configuration.h"
#include "model_postproc.h"
#include "vdset.h"
#if INFERENCE_ENGINE == INFERENCE_ENGINE_TFLM
#include "tensorflow_model.h"
#elif INFERENCE_ENGINE == INFERENCE_ENGINE_GLOW
#include "glow_model.h"
#else
#error "ERROR: An inference engine must be selected"
#endif

typedef struct _user_data_t {
    int inference_frame_num;
    mpp_t mp;
    mpp_elem_handle_t elem;
    mpp_labeled_rect_t labels[1];
    char *stats_buf;
} user_data_t;

/*******************************************************************************
 * Variables declaration
 ******************************************************************************/

/* define this flag to enable MPP stop and start */
#ifndef CONFIG_STOP_MPP
#define CONFIG_STOP_MPP 0
#endif

#if (CONFIG_STOP_MPP == 1)
#define MPP_STOP_DELAY_MS 2500
#endif

/* define data source variables */
#if (SOURCE_STATIC_ACCEL_DATA == 1)

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
#endif

#else
static const char s_accelerometer_name[] = "Fxls8974";
#endif

#define INF_STATS_BUF_SZ    512
#define STATS_PRINT_PERIOD_MS 1000
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void app_task(void *params);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
int main()
{
    BaseType_t ret;
    TaskHandle_t handle = NULL;

    /* Init board hardware. */
    BOARD_Init();

    ret = xTaskCreate(
          app_task,
          "app_task",
          configMINIMAL_STACK_SIZE + 1000,
          NULL,
          MPP_APP_MAX_PRIO,
          &handle);

    if (pdPASS != ret)
    {
        PRINTF("Failed to create app_task task");
        while (1);
    }

    vTaskStartScheduler();
    for (;;)
        vTaskSuspend(NULL);
    return 0;
}

int mpp_event_listener(mpp_t mpp, mpp_evt_t evt, void *evt_data, void *user_data) {
    status_t ret;
    const mpp_inference_cb_param_t *inf_output;

    /* user_data handle contains application private data */
    user_data_t *app_priv = (user_data_t *)user_data;

    switch(evt) {
    case MPP_EVENT_INFERENCE_OUTPUT_READY:
        /* cast evt_data pointer to correct structure matching the event */
        inf_output = (const mpp_inference_cb_param_t *) evt_data;
        ret = MODEL_ProcessOutput(
                inf_output,
                app_priv->mp,
                app_priv->elem,
                app_priv->labels,
                &app_priv->stats_buf, INF_STATS_BUF_SZ);
        if (ret != kStatus_Success)
            PRINTF("mpp_event_listener: process output error!");
        app_priv->inference_frame_num++;
        break;
    case MPP_EVENT_INVALID:
    default:
        /* nothing to do */
        break;
    }

    return 0;
}

static void app_task(void *params)
{
    static char inf_stats_buf[INF_STATS_BUF_SZ];
    user_data_t user_data = {0};
    user_data.stats_buf = inf_stats_buf;
    int ret;

    PRINTF("\r\nStarting Application:\r\n");
    PRINTF("\tCPU: %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CoreSysClk));
    PRINTF("\tApplication version: %s\r\n", ML_STATE_MONITOR_MPP_MCU_APP_VERSION);
    PRINTF("\tMPP version: %s\r\n", mpp_get_version());
#if INFERENCE_ENGINE == INFERENCE_ENGINE_TFLM
    PRINTF("\tInference Engine: TensorFlow-Lite Micro \r\n");
#elif INFERENCE_ENGINE == INFERENCE_ENGINE_GLOW
    PRINTF("\tInference Engine: Glow \r\n");
#else
#error "Please select inference engine"
#endif

    /* initialize multimedia pipeline */
    uint32_t dequeue_frequency = CLSF_STRIDE * (1000 /SENSOR_COLLECT_RATE_HZ);
    ret = mpp_api_init(dequeue_frequency);
    if (ret)
        goto err;

    mpp_t mp;
    mpp_params_t mpp_params;
    memset(&mpp_params, 0, sizeof(mpp_params));
    mpp_params.evt_callback_f = &mpp_event_listener;
    mpp_params.mask = MPP_EVENT_ALL;
    mpp_params.cb_userdata = &user_data;
    mpp_params.exec_flag = MPP_EXEC_RC;

    mp = mpp_create(&mpp_params, &ret);
    if (mp == MPP_INVALID)
        goto err;

    user_data.mp = mp;

    /* add input element */
#if (SOURCE_STATIC_ACCEL_DATA == 1)
    /* add accelerometer static data input element*/
    mpp_static_accel_params_t accelerometer_params;
    memset(&accelerometer_params, 0, sizeof (accelerometer_params));
    accelerometer_params.height = CLSF_WINDOW;
    accelerometer_params.width =  1;
    accelerometer_params.channel = CLSF_CHANNELS;
    accelerometer_params.frequency = SENSOR_COLLECT_RATE_HZ;
    mpp_static_accelerometer_add(mp, &accelerometer_params, vdset_ptr, vdset_samples);

#else
    /* add Fxls8974 accelerometer input element*/
    mpp_accelerometer_params_t accelerometer_params;
    memset(&accelerometer_params, 0 , sizeof(accelerometer_params));
    accelerometer_params.height = CLSF_WINDOW;
    accelerometer_params.width =  1;
    accelerometer_params.channel = CLSF_CHANNELS;
    accelerometer_params.frequency = SENSOR_COLLECT_RATE_HZ;
    ret = mpp_accelerometer_add(mp, s_accelerometer_name, &accelerometer_params, false);
    if (ret) {
        PRINTF("Failed to add accelerometer %s\n", s_accelerometer_name);
        goto err;
    }
#endif

    /* add processing element */
#if (SENSOR_COLLECT_LOG_EXT == 1) && (SOURCE_STATIC_ACCEL_DATA == 0)
    /* add SD element to record the accelerometer data */
    mpp_sd_params_t sd_params;
    sd_params.period_sync = 1000000; /*SDCARD_CAPT_TSYNC_US*/
    memset(&sd_params, 0, sizeof(sd_params));

    mpp_sd_add(mp, &sd_params);
    if (ret) {
        PRINTF("Failed to add element SD LOG\n");
        goto err;
    }
#else
    /* add TFlite inference element with model to process data */
    mpp_element_params_t mobilenet_params;
    memset(&mobilenet_params, 0 , sizeof(mpp_element_params_t));

#if INFERENCE_ENGINE == INFERENCE_ENGINE_GLOW
    mobilenet_params.ml_inference.model_data = GLOW_MODEL_weights_bin;
    mobilenet_params.ml_inference.inference_params.constant_weight_MemSize = GLOW_MODEL_CONSTANT_MEM_SIZE;
    mobilenet_params.ml_inference.inference_params.mutable_weight_MemSize = GLOW_MODEL_MUTABLE_MEM_SIZE;
    mobilenet_params.ml_inference.inference_params.activations_MemSize = GLOW_MODEL_ACTIVATIONS_MEM_SIZE;
    mobilenet_params.ml_inference.inference_params.num_inputs = 1;
    mobilenet_params.ml_inference.inference_params.inputs_offsets[0] = GLOW_MODEL_acceleration;
    mobilenet_params.ml_inference.inference_params.outputs_offsets[0] = GLOW_MODEL_Identity;
    mobilenet_params.ml_inference.inference_params.model_input_tensors_type = MPP_TENSOR_TYPE_FLOAT32;
    mobilenet_params.ml_inference.inference_params.model_entry_point = GLOW_MODEL_Inference_funct;
    mobilenet_params.ml_inference.type = MPP_INFERENCE_TYPE_GLOW ;
#elif INFERENCE_ENGINE == INFERENCE_ENGINE_TFLM
    mobilenet_params.ml_inference.model_data = model_fan_clsf;
    mobilenet_params.ml_inference.model_size = model_fan_clsf_len;
    mobilenet_params.ml_inference.type = MPP_INFERENCE_TYPE_TFLITE;
#endif

    mobilenet_params.ml_inference.model_input_mean = model_mean;
    mobilenet_params.ml_inference.model_input_std = model_std;
    mobilenet_params.ml_inference.inference_params.num_inputs = 1;
    mobilenet_params.ml_inference.inference_params.num_outputs = 1;
    mobilenet_params.ml_inference.tensor_order = MPP_TENSOR_ORDER_NHWC;

    ret = mpp_element_add(mp, MPP_ELEMENT_INFERENCE, &mobilenet_params, NULL);
    if (ret) {
        PRINTF("Failed to add element INFERENCE");
        goto err;
    }

    /* close the pipeline with a null sink */
    ret = mpp_nullsink_add(mp);
    if (ret) {
        PRINTF("Failed to add NULL sink\n");
        goto err;
    }
#endif

    /* start pipeline */
    ret = mpp_start(mp, 1);
    if (ret) {
        PRINTF("Failed to start pipeline");
        goto err;
    }
#if (CONFIG_STOP_MPP == 1)
    const TickType_t xDelay = MPP_STOP_DELAY_MS / portTICK_PERIOD_MS;
    vTaskDelay(xDelay);
    PRINTF("MPP STOP\n");
    mpp_stop(mp);
    vTaskDelay(xDelay);
    PRINTF("MPP START\n");
    ret = mpp_start(mp, 0);
    if (ret) {
        PRINTF("Failed to start MPP\n");
        goto err;
    }
    vTaskDelay(xDelay);
#endif

    /* main loop */
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = STATS_PRINT_PERIOD_MS / portTICK_PERIOD_MS;
    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        xTaskDelayUntil( &xLastWakeTime, xFrequency );
    }

err:
    for (;;)
    {
        PRINTF("Error building application pipeline : ret %d\r\n", ret);
        vTaskSuspend(NULL);
    }
}

