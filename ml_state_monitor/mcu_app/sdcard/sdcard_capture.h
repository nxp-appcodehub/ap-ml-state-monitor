/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SDCARD_SDCARD_CAPTURE_H_
#define SDCARD_SDCARD_CAPTURE_H_

#include <stdint.h>
#include "fsl_common.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void SDCARD_Detect();
uint8_t SDCARD_CheckExisting(char *fileName);

status_t SDCARD_CaptureOpen(char *fileName);
void SDCARD_CaptureData(uint64_t ts_us, uint64_t sample_num, uint64_t totalDuration_samples, char *buf, uint32_t size);
void SDCARD_CaptureClose();

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* SDCARD_SDCARD_CAPTURE_H_ */
