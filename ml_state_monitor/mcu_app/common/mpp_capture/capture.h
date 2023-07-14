/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef SENSOR_CAPTURE_H_
#define SENSOR_CAPTURE_H_

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void CAPT_Init(uint8_t *captClassLabelIdx, uint64_t *captDuration_us, uint64_t *captDuration_samples);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* SENSOR_CAPTURE_H_ */
