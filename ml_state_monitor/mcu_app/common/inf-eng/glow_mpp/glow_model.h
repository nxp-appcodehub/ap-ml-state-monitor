/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GLOW_GLOWMODEL_H_
#define GLOW_GLOWMODEL_H_

#include <stdint.h>

#include "fsl_common.h"
#include "app_config.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if MODEL_QUANTIZED
#if (SELECTED_MODEL == MODEL_CNN)
#include "glow/model_cnn_quant.h"
#define GLOW_MODEL_WEIGHTS_TXT	"glow/model_cnn_quant.weights.txt"
#elif (SELECTED_MODEL == MODEL_LCNN)
#include "glow/model_lcnn_quant.h"
#define GLOW_MODEL_WEIGHTS_TXT	"glow/model_lcnn_quant.weights.txt"
#elif (SELECTED_MODEL == MODEL_MLP)
#include "glow/model_mlp_quant.h"
#define GLOW_MODEL_WEIGHTS_TXT	"glow/model_mlp_quant.weights.txt"
#elif (SELECTED_MODEL == MODEL_LRESNET)
#include "glow/model_lresnet_quant.h"
#define GLOW_MODEL_WEIGHTS_TXT	"glow/model_lresnet_quant.weights.txt"
#endif
#else
#if (SELECTED_MODEL == MODEL_CNN)
#include "glow/model_cnn.h"
#define GLOW_MODEL_WEIGHTS_TXT	"glow/model_cnn.weights.txt"
#elif (SELECTED_MODEL == MODEL_LCNN)
#include "glow/model_lcnn.h"
#define GLOW_MODEL_WEIGHTS_TXT	"glow/model_lcnn.weights.txt"
#elif (SELECTED_MODEL == MODEL_MLP)
#include "glow/model_mlp.h"
#define GLOW_MODEL_WEIGHTS_TXT	"glow/model_mlp.weights.txt"
#elif (SELECTED_MODEL == MODEL_LRESNET)
#include "glow/model_lresnet.h"
#define GLOW_MODEL_WEIGHTS_TXT	"glow/model_lresnet.weights.txt"
#endif
#endif

#if MODEL_QUANTIZED
#if (SELECTED_MODEL == MODEL_CNN)
#define GLOW_MODEL_Inference_funct 		&glow_model_cnn_quant

#define GLOW_MODEL_MEM_ALIGN            MODEL_CNN_QUANT_MEM_ALIGN
#define GLOW_MODEL_CONSTANT_MEM_SIZE    MODEL_CNN_QUANT_CONSTANT_MEM_SIZE
#define GLOW_MODEL_MUTABLE_MEM_SIZE     MODEL_CNN_QUANT_MUTABLE_MEM_SIZE
#define GLOW_MODEL_ACTIVATIONS_MEM_SIZE MODEL_CNN_QUANT_ACTIVATIONS_MEM_SIZE
#define GLOW_MODEL_acceleration         MODEL_CNN_QUANT_acceleration
#define GLOW_MODEL_Identity             MODEL_CNN_QUANT_Identity

#elif (SELECTED_MODEL == MODEL_LCNN)
#define GLOW_MODEL_Inference_funct 		&glow_model_lcnn_quant

#define GLOW_MODEL_MEM_ALIGN            MODEL_LCNN_QUANT_MEM_ALIGN
#define GLOW_MODEL_CONSTANT_MEM_SIZE    MODEL_LCNN_QUANT_CONSTANT_MEM_SIZE
#define GLOW_MODEL_MUTABLE_MEM_SIZE     MODEL_LCNN_QUANT_MUTABLE_MEM_SIZE
#define GLOW_MODEL_ACTIVATIONS_MEM_SIZE MODEL_LCNN_QUANT_ACTIVATIONS_MEM_SIZE
#define GLOW_MODEL_acceleration         MODEL_LCNN_QUANT_acceleration
#define GLOW_MODEL_Identity             MODEL_LCNN_QUANT_Identity

#elif (SELECTED_MODEL == MODEL_MLP)
#define GLOW_MODEL_Inference_funct 		&glow_model_mlp_quant

#define GLOW_MODEL_MEM_ALIGN            MODEL_MLP_QUANT_MEM_ALIGN
#define GLOW_MODEL_CONSTANT_MEM_SIZE    MODEL_MLP_QUANT_CONSTANT_MEM_SIZE
#define GLOW_MODEL_MUTABLE_MEM_SIZE     MODEL_MLP_QUANT_MUTABLE_MEM_SIZE
#define GLOW_MODEL_ACTIVATIONS_MEM_SIZE MODEL_MLP_QUANT_ACTIVATIONS_MEM_SIZE
#define GLOW_MODEL_acceleration         MODEL_MLP_QUANT_input_1
#define GLOW_MODEL_Identity             MODEL_MLP_QUANT_Identity

#elif (SELECTED_MODEL == MODEL_LRESNET)
#define GLOW_MODEL_Inference_funct 		&glow_model_lresnet_quant

#define GLOW_MODEL_MEM_ALIGN            MODEL_LRESNET_QUANT_MEM_ALIGN
#define GLOW_MODEL_CONSTANT_MEM_SIZE    MODEL_LRESNET_QUANT_CONSTANT_MEM_SIZE
#define GLOW_MODEL_MUTABLE_MEM_SIZE     MODEL_LRESNET_QUANT_MUTABLE_MEM_SIZE
#define GLOW_MODEL_ACTIVATIONS_MEM_SIZE MODEL_LRESNET_QUANT_ACTIVATIONS_MEM_SIZE
#define GLOW_MODEL_acceleration         MODEL_LRESNET_QUANT_input_2
#define GLOW_MODEL_Identity             MODEL_LRESNET_QUANT_Identity

#endif
#else
#if (SELECTED_MODEL == MODEL_CNN)
#define GLOW_MODEL_Inference_funct 		&glow_model_cnn

#define GLOW_MODEL_MEM_ALIGN            MODEL_CNN_MEM_ALIGN
#define GLOW_MODEL_CONSTANT_MEM_SIZE    MODEL_CNN_CONSTANT_MEM_SIZE
#define GLOW_MODEL_MUTABLE_MEM_SIZE     MODEL_CNN_MUTABLE_MEM_SIZE
#define GLOW_MODEL_ACTIVATIONS_MEM_SIZE MODEL_CNN_ACTIVATIONS_MEM_SIZE
#define GLOW_MODEL_acceleration         MODEL_CNN_acceleration
#define GLOW_MODEL_Identity             MODEL_CNN_Identity

#elif (SELECTED_MODEL == MODEL_LCNN)
#define GLOW_MODEL_Inference_funct 		&glow_model_lcnn

#define GLOW_MODEL_MEM_ALIGN            MODEL_LCNN_MEM_ALIGN
#define GLOW_MODEL_CONSTANT_MEM_SIZE    MODEL_LCNN_CONSTANT_MEM_SIZE
#define GLOW_MODEL_MUTABLE_MEM_SIZE     MODEL_LCNN_MUTABLE_MEM_SIZE
#define GLOW_MODEL_ACTIVATIONS_MEM_SIZE MODEL_LCNN_ACTIVATIONS_MEM_SIZE
#define GLOW_MODEL_acceleration         MODEL_LCNN_acceleration
#define GLOW_MODEL_Identity             MODEL_LCNN_Identity

#elif (SELECTED_MODEL == MODEL_MLP)
#define GLOW_MODEL_Inference_funct 		&glow_model_mlp

#define GLOW_MODEL_MEM_ALIGN            MODEL_MLP_MEM_ALIGN
#define GLOW_MODEL_CONSTANT_MEM_SIZE    MODEL_MLP_CONSTANT_MEM_SIZE
#define GLOW_MODEL_MUTABLE_MEM_SIZE     MODEL_MLP_MUTABLE_MEM_SIZE
#define GLOW_MODEL_ACTIVATIONS_MEM_SIZE MODEL_MLP_ACTIVATIONS_MEM_SIZE
#define GLOW_MODEL_acceleration         MODEL_MLP_input_1
#define GLOW_MODEL_Identity             MODEL_MLP_Identity

#elif (SELECTED_MODEL == MODEL_LRESNET)
#define GLOW_MODEL_Inference_funct 		&glow_model_lresnet

#define GLOW_MODEL_MEM_ALIGN            MODEL_LRESNET_MEM_ALIGN
#define GLOW_MODEL_CONSTANT_MEM_SIZE    MODEL_LRESNET_CONSTANT_MEM_SIZE
#define GLOW_MODEL_MUTABLE_MEM_SIZE     MODEL_LRESNET_MUTABLE_MEM_SIZE
#define GLOW_MODEL_ACTIVATIONS_MEM_SIZE MODEL_LRESNET_ACTIVATIONS_MEM_SIZE
#define GLOW_MODEL_acceleration         MODEL_LRESNET_input_2
#define GLOW_MODEL_Identity             MODEL_LRESNET_Identity
#endif
#endif

#define GLOW_MODEL_TENSOR_SIZE 			4
#define GLOW_MODEL_TENSOR_TYPE			MPP_TENSOR_TYPE_FLOAT32

extern const uint8_t GLOW_MODEL_weights_bin[];

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* GLOW_GLOWMODEL_H_ */
