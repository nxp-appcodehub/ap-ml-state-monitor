/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RTM_RTMMODEL_H_
#define RTM_RTMMODEL_H_

#include <stdint.h>

#include "fsl_common.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
status_t RTMMODEL_Init(void);
status_t RTMMODEL_RunInference(void *inputData, size_t size, int8_t *predClass, int32_t *tinf_us, uint8_t verbose);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* RTM_RTMMODEL_H_ */
