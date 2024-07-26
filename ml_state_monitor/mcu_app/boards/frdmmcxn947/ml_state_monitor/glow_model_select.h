/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GLOW_MODEL_SELECT_H_
#define GLOW_MODEL_SELECT_H_

#include <stdint.h>

#include "fsl_common.h"

#include "app_config.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if GLOW_MODEL_QUANTIZED_EN
#include "model_cnn_quant.h"
#else
#include "model_cnn.h"
#endif

#if GLOW_MODEL_QUANTIZED_EN
#define GLOW_MODEL_MEM_ALIGN            MODEL_CNN_QUANT_MEM_ALIGN
#define GLOW_MODEL_CONSTANT_MEM_SIZE    MODEL_CNN_QUANT_CONSTANT_MEM_SIZE
#define GLOW_MODEL_MUTABLE_MEM_SIZE     MODEL_CNN_QUANT_MUTABLE_MEM_SIZE
#define GLOW_MODEL_ACTIVATIONS_MEM_SIZE MODEL_CNN_QUANT_ACTIVATIONS_MEM_SIZE
#define GLOW_MODEL_acceleration         MODEL_CNN_QUANT_acceleration
#define GLOW_MODEL_Identity             MODEL_CNN_QUANT_Identity
#define GLOW_MODEL_Entry                glow_model_cnn_quant
#else
#define GLOW_MODEL_MEM_ALIGN            MODEL_CNN_MEM_ALIGN
#define GLOW_MODEL_CONSTANT_MEM_SIZE    MODEL_CNN_CONSTANT_MEM_SIZE
#define GLOW_MODEL_MUTABLE_MEM_SIZE     MODEL_CNN_MUTABLE_MEM_SIZE
#define GLOW_MODEL_ACTIVATIONS_MEM_SIZE MODEL_CNN_ACTIVATIONS_MEM_SIZE
#define GLOW_MODEL_acceleration         MODEL_CNN_acceleration
#define GLOW_MODEL_Identity             MODEL_CNN_Identity
#define GLOW_MODEL_Entry                glow_model_cnn
#endif

#if GLOW_MODEL_QUANTIZED_EN
#define GLOW_MODEL_WEIGHTS_TXT  "model_cnn_quant.weights.txt"
#else
#define GLOW_MODEL_WEIGHTS_TXT  "model_cnn.weights.txt"
#endif

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* GLOW_MODEL_SELECT_H_ */
