/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file gpio_driver_irq.c
 * @brief The gpio_driver_irq.c file containes the Generic IRQ implmentation for GPIO.
*/

#include "fsl_gpio.h"
#include "gpio_driver.h"

extern void ksdk_gpio_handle_interrupt(GPIO_Type *base, port_number_t portNumber);

/*******************************************************************************
 * Functions - GPIOIRQ implementation
 ******************************************************************/
void PORT0_IRQHandler(void)
{
    ksdk_gpio_handle_interrupt(GPIO0, 0);
}
