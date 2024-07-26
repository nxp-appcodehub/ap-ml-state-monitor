/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TENSORFLOW_TFLM_MODEL_SELECT_H_
#define TENSORFLOW_TFLM_MODEL_SELECT_H_

#include <stdint.h>

#include "fsl_common.h"
#include "model.h"
#include "app_config.h"
#if TFLITE_MODEL_TYPE == 0
#include "tensorflow_model_cnn.h"
#elif TFLITE_MODEL_TYPE == 1
#include "tensorflow_model_cnn_quant.h"
#elif TFLITE_MODEL_TYPE == 2
#include "tensorflow_model_cnn_quant_neutron.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if TFLITE_MODEL_TYPE == 0
#define TFLM_MODEL_DATA             tensorflow_model_cnn
#define TFLM_MODEL_DATA_LEN         tensorflow_model_cnn_len
#define TFLM_TENSOR_ARENA_SIZE_B    (9 * 1024)
#elif TFLITE_MODEL_TYPE == 1
#define TFLM_MODEL_DATA             tensorflow_model_cnn_quant
#define TFLM_MODEL_DATA_LEN         tensorflow_model_cnn_quant_len
#define TFLM_TENSOR_ARENA_SIZE_B    (4 * 1024)
#elif TFLITE_MODEL_TYPE == 2
#define TFLM_MODEL_DATA             tensorflow_model_cnn_quant_neutron
#define TFLM_MODEL_DATA_LEN         tensorflow_model_cnn_quant_neutron_len
#define TFLM_TENSOR_ARENA_SIZE_B    (4 * 1024)
#endif

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* TENSORFLOW_TFLM_MODEL_SELECT_H_ */
