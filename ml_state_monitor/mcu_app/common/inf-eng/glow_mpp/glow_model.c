/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "glow_model.h"

GLOW_MEM_ALIGN(GLOW_MODEL_MEM_ALIGN)
const uint8_t GLOW_MODEL_weights_bin[] = {
    #include GLOW_MODEL_WEIGHTS_TXT
};
