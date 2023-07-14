/*
 * Copyright 2020-2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MODEL_POSTPROCESS_H_
#define _MODEL_POSTPROCESS_H_

#include "mpp_api_types.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * Process the output tensors
 *
 * @param [in] inf_out: inference output (tensor data and descritption)
 * @param [in] mpp: the mpp of element to update
 * @param [in] elem: the element to update
 * @param [in] rects: the rectangles to update
 * @param [out] stats: buffer to return stats
 * @param [in]  stats_sz: stats buffer size
 *
 * @return: 0 if succeeded, else failed.
 */
int32_t MODEL_ProcessOutput(const mpp_inference_cb_param_t *inf_out, void *mpp,
        mpp_elem_handle_t elem, mpp_labeled_rect_t *rects, char **pstats, int stats_sz);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* _MODEL_POSTPROCESS_H_ */
