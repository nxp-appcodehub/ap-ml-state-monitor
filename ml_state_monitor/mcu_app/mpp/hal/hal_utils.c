/*
 * Copyright 2020-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "fsl_debug_console.h"
#include <stdio.h>
#include <stdarg.h>
#include "hal_freertos.h"

/* TODO - fix pixel format definition issue */
#include "mpp_api_types.h"
#include "hal_utils.h"

#define LOG_STRING_MAX_SIZE 128

static const char* hal_camera_names[] = {
    "rgb_sim",
    "ir_sim",
    "yuv_sim",
    "MipiOv5640"
};

static const char* hal_accelerometer_names[] = {
		"Fxls8974"
};

static const char* hal_display_names[] = {
    "opencv0",
    "opencv1",
    "Lcdifv2Rk055ah"
};

static const char* hal_sd_log_names[] ={
    "SD_card",
};

uint64_t TIMER_GetTimeInUS()
{
    uint64_t us = ((SystemCoreClock / configTICK_RATE_HZ) - SysTick->VAL) / (SystemCoreClock / 1000000);
    us += (uint64_t) xTaskGetTickCount() * portTICK_PERIOD_MS * 1000;
    return us;
}

void LOG(int cond, const char* module, const char* lvl_str, const char* format, ...)
{
    char args_buffer[LOG_STRING_MAX_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(args_buffer, format, args);
    if (cond) {
        PRINTF("\r[%u]", (unsigned int)GET_TICK());
        PRINTF(":%s:%s:(%s:%u)", module, lvl_str, __func__, __LINE__);
        PRINTF(":%s", args_buffer);
    }
    va_end(args);
}

typedef int (*display_setup_func_t) (display_dev_t *);
int display_sim_setup(display_dev_t *);
static display_setup_func_t display_setup_funcs[] =
{
    display_sim_setup,
    display_sim_setup,
    display_sim_setup,  /* TODO find a more elegant way to setup display */
};

int setup_display_dev(const char *name, display_dev_t *dev)
{
    int i, found = 0;
    for (i = 0; i < sizeof(hal_display_names)/sizeof(hal_display_names[0]); i++)
        if (!strcmp(name, hal_display_names[i])) {
            found = 1;
            break;
        }
    if (!found)
        return MPP_ERROR;

    display_setup_func_t disp_setup_f;
    disp_setup_f = display_setup_funcs[i];
    if (disp_setup_f)
        return disp_setup_f(dev);

    return MPP_ERROR;

}

typedef int (*sd_log_setup_func_t) (sd_log_dev_t *);
int sd_log_sim_setup(sd_log_dev_t *);
static sd_log_setup_func_t sd_log_setup_funcs[] =
{
	sd_log_sim_setup,
	sd_log_sim_setup,
	sd_log_sim_setup,  /* TODO find a more elegant way to setup sd_log */
};

int setup_sd_log_dev(const char *name, sd_log_dev_t *dev)
{
    int i, found = 0;
    for (i = 0; i < sizeof(hal_sd_log_names)/sizeof(hal_sd_log_names[0]); i++)
        if (!strcmp(name, hal_sd_log_names[i])) {
            found = 1;
            break;
        }
    if (!found)
        return MPP_ERROR;

    sd_log_setup_func_t sd_setup_f;
    sd_setup_f = sd_log_setup_funcs[i];
    if (sd_setup_f)
        return sd_setup_f(dev);

    return MPP_ERROR;

}

typedef int (*camera_setup_func_t)(const char *name, camera_dev_t *, _Bool);
/* simulated-camera setup function */
int camera_sim_setup(const char *name, camera_dev_t *dev, _Bool defconfig);
/* TODO find a more elegant way to setup camera
 * allows multiple cameras, and disabling some cameras at compile time */
static camera_setup_func_t camera_setup_funcs[] =
{
    0,
    0,
    0,
    0,
};


typedef int (*accelerometer_setup_func_t)(const char *name, accelerometer_dev_t *, _Bool);


/* simulated-accelerometer setup function */
int accelerometer_sim_setup(const char *name, accelerometer_dev_t *dev, _Bool defconfig);
/* TODO find a more elegant way to setup accelerometer
 * allows multiple accelerometers, and disabling some accelerometers at compile time */
static accelerometer_setup_func_t accelerometer_setup_funcs[] =
{
	accelerometer_sim_setup,
	accelerometer_sim_setup,
	accelerometer_sim_setup,
	accelerometer_sim_setup,
};

int setup_accelerometer_dev(const char *name, accelerometer_dev_t *dev, _Bool defconfig)
{
    int i, found = 0;
    /* search name */
    for (i = 0; i < sizeof(hal_accelerometer_names)/sizeof(hal_accelerometer_names[0]); i++)
        if (!strcmp(name, hal_accelerometer_names[i])) {
            found = 1;
            break;
        }
    if (!found)
        return MPP_ERROR;

    /* call name-specific accelerometer setup function*/
    accelerometer_setup_func_t accelerometer_setup_f;
    accelerometer_setup_f = accelerometer_setup_funcs[i];
    if (accelerometer_setup_f)
        return accelerometer_setup_f(name, dev, defconfig);

    return MPP_ERROR;
}

int setup_camera_dev(const char *name, camera_dev_t *dev, _Bool defconfig)
{
    int i, found = 0;
    /* search name */
    for (i = 0; i < sizeof(hal_camera_names)/sizeof(hal_camera_names[0]); i++)
        if (!strcmp(name, hal_camera_names[i])) {
            found = 1;
            break;
        }
    if (!found)
        return MPP_ERROR;

    /* call name-specific camera setup function*/
    camera_setup_func_t cam_setup_f;
    cam_setup_f = camera_setup_funcs[i];
    if (cam_setup_f)
        return cam_setup_f(name, dev, defconfig);

    return MPP_ERROR;
}

int setup_static_image(static_image_t *);

int setup_static_image_elt(static_image_t *elt)
{
    return setup_static_image(elt);
}

int setup_static_accelerometer(static_accelerometer_t *);

int setup_static_accelerometer_elt(static_accelerometer_t *elt)
{
    return setup_static_accelerometer(elt);
}

mpp_pixel_format_t hal_fsl_to_mpp_pixeltype(video_pixel_format_t fsl_typ)
{
    mpp_pixel_format_t mpp_typ;
    switch(fsl_typ)
    {
    case kVIDEO_PixelFormatXYUV:
        mpp_typ = MPP_PIXEL_YUV1P444;
        break;
    case kVIDEO_PixelFormatXRGB8888:
        mpp_typ = MPP_PIXEL_ARGB;
        break;
    case kVIDEO_PixelFormatRGB565:
        mpp_typ = MPP_PIXEL_RGB565;
        break;
    case kVIDEO_PixelFormatBGR888:
        mpp_typ = MPP_PIXEL_BGR;
        break;
    case kVIDEO_PixelFormatRGB888:
        mpp_typ = MPP_PIXEL_RGB;
        break;
    default:
        mpp_typ = MPP_PIXEL_INVALID;
        break;
    }
    return mpp_typ;
}

/* Computes the checksum of the buffer
 * using Pisano with End-Around Carry algorithm
 * which is almost as reliable but faster than CRC32.
 * See https://hackaday.io/project/178998-peac-pisano-with-end-around-carry-algorithm
 * Note: with odd buffer size, last byte is ignored.
 * param: 'size_b': size in bytes of buffer
 * param: 'pbuf': pointer to buffer
 * return: a 32bit checksum
 **/
 
uint32_t calc_checksum(int size_b, void *pbuf)
{
    uint16_t *pw = (uint16_t *)pbuf;  /* pointer to 16b word in input buffer */
    uint16_t x = 0x1234;
    uint32_t y = 0xABCD;
    uint32_t c = size_b;
    int w_cnt = size_b/2; /* 16b word count */
    while(w_cnt > 0)
    {
        c += x;
        c += y;
        y  = x + *pw;
        x  = (uint16_t) c;
        c >>= 16;
        w_cnt--;
        pw++;
    }
    return (x | (y << 16));
}

#ifdef __cplusplus
}
#endif

