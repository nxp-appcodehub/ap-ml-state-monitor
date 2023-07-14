/*
 * Copyright 2021-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MPP_CONFIG_H
#define _MPP_CONFIG_H

#include "app_config.h"

/* Set here all the static configuration of the Media Processing Pipeline */

//#define ENABLE_ACCELEROMETRE_Fxls8974 1

/* Log level configuration
 * ERR:   0
 * INFO:  1
 * DEBUG: 2
 */
#ifndef HAL_LOG_LEVEL
#define HAL_LOG_LEVEL 0
#endif

/* MPP version */
#define MPP_VERSION_MAJOR 1
#define MPP_VERSION_MINOR 0
#ifdef MPP_COMMIT
#define MPP_VERSION_COMMIT MPP_COMMIT
#else
#define MPP_VERSION_COMMIT 0
#endif

/* Workaround for the PXP bug (iMXRT1170) where BGR888 is output instead
 * of RGB888 [MPP-97].
 */
#ifndef IMXRT1170_PXP_WORKAROUND_OUT_RGB
#define IMXRT1170_PXP_WORKAROUND_OUT_RGB 1
#endif

/**
 *  Mutex lock timeout definition
 *  An arbitrary default value is defined to 5 seconds
 *  user can modify it at build time by adding the flag
 *       -DCONFIG_MAX_MUTEX_TIME_MS=value
 *  value unit should be milliseconds
 * */
#ifdef CONFIG_MAX_MUTEX_TIME_MS
#define MAX_MUTEX_TIME_MS   CONFIG_MAX_MUTEX_TIME_MS
#else
#define MAX_MUTEX_TIME_MS   (5000)
#endif /* CONFIG_MAX_MUTEX_TIME_MS */

#define HAL_MAX_MUTEX_TIME_MS   (MAX_MUTEX_TIME_MS / portTICK_PERIOD_MS)

#endif /* _MPP_CONFIG_H */
