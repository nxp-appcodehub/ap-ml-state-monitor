/*
 * Copyright 2023-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file frdmmcxn947.h
 * @brief The frdmmcxn947.h file defines GPIO pin mappings for frdmmcxn947 board
 */

#ifndef FRDM_MCXN947_H_
#define FRDM_MCXN947_H_

#include "pin_mux.h"
#include "RTE_Device.h"
#include "gpio_driver.h"
#include "clock_config.h"
#include "board.h"
#include "app_config.h"

// I2C4 Pin Handles
extern gpioHandleKSDK_t D14;
extern gpioHandleKSDK_t D15;

// SPI7 Handles
extern gpioHandleKSDK_t D11;
extern gpioHandleKSDK_t D12;
extern gpioHandleKSDK_t D13;
extern gpioHandleKSDK_t D10;

// UART0 Handle
extern gpioHandleKSDK_t D0;
extern gpioHandleKSDK_t D1;

extern gpioHandleKSDK_t D2;

// LPCXpresso54114 Internal Peripheral Pin Definitions
extern gpioHandleKSDK_t RED_LED;
extern gpioHandleKSDK_t GREEN_LED;
extern gpioHandleKSDK_t BLUE_LED;

// I2C_S1: Pin mapping and driver information for default I2C brought to shield
#if defined(SENSOR_BOARD) && (SENSOR_BOARD == FRDM_STBI_A8974_BOARD)
#define I2C2 LPI2C2
#define I2C_S1_DRIVER       Driver_I2C2
#define I2C_S1_DEVICE_INDEX I2C2_INDEX
#define I2C_S1_SIGNAL_EVENT I2C2_SignalEvent_t
#elif defined(SENSOR_BOARD) && (SENSOR_BOARD == ACCEL_4_CLICK_BOARD)
#define I2C3 LPI2C3
#define I2C_S1_DRIVER       Driver_I2C3
#define I2C_S1_DEVICE_INDEX I2C3_INDEX
#define I2C_S1_SIGNAL_EVENT I2C3_SignalEvent_t
#endif /* SENSOR_BOARD */

// SPI_S: Pin mapping and driver information default SPI brought to shield
#define SPI_S_DRIVER       Driver_SPI1
#define SPI_S_BAUDRATE     500000U ///< Transfer baudrate - 500k
#define SPI_S_DEVICE_INDEX SPI1_INDEX
#define SPI_S_SIGNAL_EVENT SPI1_SignalEvent_t

// UART: Driver information for default UART to communicate with HOST PC.
#define HOST_S_DRIVER       Driver_USART0
#define HOST_S_SIGNAL_EVENT HOST_SignalEvent_t
#define HOST_B_DRIVER       Driver_USART1
#define HOST_B_SIGNAL_EVENT HOST_SignalEvent_t

/* @brief  Ask use input to resume after specified samples have been processed. */
#define ASK_USER_TO_RESUME(x)                                                          \
    static bool askResume            = true;                                           \
    static uint16_t samplesToProcess = x - 1;                                          \
    if (askResume && !samplesToProcess--)                                              \
    {                                                                                  \
        PRINTF("\r\n Specified samples processed, press any key to continue... \r\n"); \
        GETCHAR();                                                                     \
        askResume = false;                                                             \
    }

/* @brief dummy arguement to Power Mode Wait Wrapper. */
#define SMC NULL
#define SPI1 LPSPI1

/*! @brief The FXLS8974 control pins. */
#define FXLS8974_INTF_SEL_PORT GPIO1
#define FXLS8974_INTF_SEL_PIN  21U
#define FXLS8974_BT_MODE_PORT  GPIO1
#define FXLS8974_BT_MODE_PIN   2U

/* @brief Kinetis style Wrapper API for Power Mode Wait (Wait for Interrupt). */
status_t SMC_SetPowerModeWait(void *arg);
/* @brief Kinetis style Wrapper API for Power Mode VLPR (Wait for Interrupt). */
status_t SMC_SetPowerModeVlpr(void *arg);
/* @brief Kinetis style Wrapper API for handling all Clock related configurations. */
void BOARD_BootClockRUN(void);
/* @brief Wrapper API for initialization of FXLS8974CF. */
void BOARD_InitFXLS8974(void);

#endif /* FRDM_MCXN947_H_ */
