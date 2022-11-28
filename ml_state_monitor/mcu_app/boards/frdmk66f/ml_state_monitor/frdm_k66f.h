/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file frdm_k66f.h
 * @brief The frdm_k66f.h file defines GPIO pin mappings for FRDM-K66F board
 */

#ifndef FRDM_K66F_H_
#define FRDM_K66F_H_

#include "pin_mux.h"
#include "fsl_smc.h"
#include "MK66F18.h"
#include "RTE_Device.h"
#include "gpio_driver.h"

// I2C_S1: Pin mapping and driver information for default I2C
#define I2C_S1_DRIVER       Driver_I2C0
#define I2C_S1_DEVICE_INDEX I2C0_INDEX
#define I2C_S1_SIGNAL_EVENT I2C0_SignalEvent_t

#endif /* FRDM_K66F_H_ */
