/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ACCELEROMETER_DATA_VDSET_H_
#define ACCELEROMETER_DATA_VDSET_H_

#include "app_config.h"

/* Headers for validation data */
#if (SOURCE_STATIC_ACCEL_DATA == 1)
#if (STATIC_ACCEL_VDSET == VDSET_CLOG)
#include "vdset_clog.h"
#elif (STATIC_ACCEL_VDSET == VDSET_FRICTION)
#include "vdset_friction.h"
#elif (STATIC_ACCEL_VDSET == VDSET_ON)
#include "vdset_on.h"
#elif (STATIC_ACCEL_VDSET == VDSET_OFF)
#include "vdset_off.h"
#endif
#endif

#endif /* ACCELEROMETER_DATA_VDSET_H_ */
