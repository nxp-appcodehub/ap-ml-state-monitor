/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board_init.h"
#include "pin_mux.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_power.h"

void BOARD_Init()
{
    /* Init board hardware. */

    /* set BOD VBAT level to 1.65V */
    POWER_SetBodVbatLevel(kPOWER_BodVbatLevel1650mv, kPOWER_BodHystLevel50mv, false); /* TODO check need */

    /* attach main clock divide to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitBootPins();
    /* use PLL150M */
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /* init for display */

    /* Define the init structure for the output pin*/
    gpio_pin_config_t gpio_config = {
        kGPIO_DigitalOutput,
        1,
    };

    CLOCK_EnableClock(kCLOCK_InputMux);                        /* Enables the clock for the kCLOCK_InputMux block. 0 = Disable; 1 = Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Iocon);                           /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);

    /* attach 12 MHz clock to FLEXCOMM4 (I2C master) */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);
    GPIO_PortInit(GPIO,0);
    /* reset FLEXCOMM for I2C */
    RESET_PeripheralReset(kFC4_RST_SHIFT_RSTn);

    /* Init output GPIO. */
    GPIO_PortInit(GPIO, 1);
    GPIO_PinInit(GPIO, 1, 6, &gpio_config);
    CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);               //main clock source to clkout
    CLOCK_SetClkDiv(kCLOCK_DivClkOut,3,false);  //150MHz / 3 = 50MHz clkout to XCLK of camera module

}
