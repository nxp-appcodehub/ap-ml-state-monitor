/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef INF_ENG_TENSORFLOW_TENSORFLOW_MODEL_H_
#define INF_ENG_TENSORFLOW_TENSORFLOW_MODEL_H_

#include "app_config.h"

#if INFERENCE_ENGINE == INFERENCE_ENGINE_TFLM

#if SELECTED_MODEL == MODEL_CNN
#if MODEL_QUANTIZED
#include "tensorflow_model_cnn_quant.h"
#define model_fan_clsf tensorflow_model_cnn_quant
#define model_fan_clsf_len tensorflow_model_cnn_quant_len
#else
#include "tensorflow_model_cnn.h"
#define model_fan_clsf tensorflow_model_cnn
#define model_fan_clsf_len tensorflow_model_cnn_len
#endif
#elif SELECTED_MODEL == MODEL_LCNN
#if MODEL_QUANTIZED
#include "tensorflow_model_lcnn_quant.h"
#define model_fan_clsf tensorflow_model_lcnn_quant
#define model_fan_clsf_len tensorflow_model_lcnn_quant_len
#else
#include "tensorflow_model_lcnn.h"
#define model_fan_clsf tensorflow_model_lcnn
#define model_fan_clsf_len tensorflow_model_lcnn_len
#endif
#elif SELECTED_MODEL == MODEL_MLP
#if MODEL_QUANTIZED
#include "tensorflow_model_mlp_quant.h"
#define model_fan_clsf tensorflow_model_mlp_quant
#define model_fan_clsf_len tensorflow_model_mlp_quant_len
#else
#include "tensorflow_model_mlp.h"
#define model_fan_clsf tensorflow_model_mlp
#define model_fan_clsf_len tensorflow_model_mlp_len
#endif
#elif SELECTED_MODEL == MODEL_LRESNET
#if MODEL_QUANTIZED
#include "tensorflow_model_lresnet_quant.h"
#define model_fan_clsf tensorflow_model_lresnet_quant
#define model_fan_clsf_len tensorflow_model_lresnet_quant_len
#else
#include "tensorflow_model_lresnet.h"
#define model_fan_clsf tensorflow_model_lresnet
#define model_fan_clsf_len tensorflow_model_lresnet_len
#endif
#endif // MODELS

#endif // TFML

#endif /* INF_ENG_TENSORFLOW_TENSORFLOW_MODEL_H_ */
