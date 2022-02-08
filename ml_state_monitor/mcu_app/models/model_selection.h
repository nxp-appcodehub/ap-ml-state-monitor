/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MODEL_SELECTION_H_
#define MODEL_SELECTION_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * If *_MODEL_QUANTIZED_EN is set to 1 then the quantized version of the model
 * will be used
 */
#define TFLITE_MODEL_QUANTIZED_EN   0
#define RTM_MODEL_QUANTIZED_EN      0
#define GLOW_MODEL_QUANTIZED_EN     0

/**
 * If *_LOAD_MODEL_TO_RAM is set to 1 then the model will be
 * copied from FLASH into RAM before being loaded.
 */
#define TFLITE_LOAD_MODEL_TO_RAM    1
#define RTM_LOAD_MODEL_TO_RAM       1
#define GLOW_LOAD_MODEL_TO_RAM      1

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MODEL_SELECTION_H_ */
