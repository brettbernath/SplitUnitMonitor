/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup group_common_boards
 * \defgroup user_board_group User board
 *
 * @{
 */

void system_board_init(void);

/** Name string macro */
#define BOARD_NAME                "SPARKFUN SAMD21G18A DEV BREAKOUT SUM"

/** @} */

/* PA00 - Pin 1 - Xin32 and PA01 - Pin 2 - Xout32 are used for the external crystal */

/* define labels on the board with the corresponding pins */
#define BOARD_LED_RXL		PIN_PB03			//Yellow LED
#define BOARD_LED_TXL		PIN_PA27			//Green LED
#define BOARD_LED_D13		PIN_PA17			//Blue LED

/* Analog board labels */
#define BOARD_A0_DAC		PIN_PA02
#define BOARD_A1			PIN_PB08
#define BOARD_A2			PIN_PB09		/* Configured as TC4_W1 Channel 1 Output, used as IR Tx Modulator output */
#define BOARD_A3			PIN_PA04
#define BOARD_A4			PIN_PA05
#define BOARD_A5			PIN_PB02

/* Serial board labels */
#define BOARD_38			PIN_PA13
#define BOARD_30_TX			PIN_PB22
#define BOARD_31_RX			PIN_PB23
#define BOARD_SWCLK			PIN_PA30
#define BOARD_SWDIO			PIN_PA31

/* SPI board labels */
#define BOARD_SCK			PIN_PB11
#define BOARD_MISO			PIN_PA12
#define BOARD_MOSI			PIN_PB10

/* Serial 1 board labels */
#define BOARD_D0_RX			PIN_PA11
#define BOARD_D1_TX			PIN_PA10

#define BOARD_D02			PIN_PA14
#define BOARD_D03			PIN_PA09
#define BOARD_D04			PIN_PA08
#define BOARD_D05			PIN_PA15
#define BOARD_D06			PIN_PA20
#define BOARD_D07			PIN_PA21		/* For debug mode, configured as GPIO output, used to view IR tx PHY output to modulator */
#define BOARD_D08			PIN_PA06
#define BOARD_D09			PIN_PA07
#define BOARD_D10			PIN_PA18		/* Configured as EXTINT2,  used for IR Rx */
#define BOARD_D11			PIN_PA16
#define BOARD_D12			PIN_PA19
#define BOARD_D13			PIN_PA17		/* Blue LED */

#define BOARD_AREF			PIN_PA03
#define BOARD_SDA			PIN_PA22
#define BOARD_SCL			PIN_PA23

/* Define LEDs on the board */
#define LED_BLUE		BOARD_LED_D13
#define LED_YELLOW		BOARD_LED_RXL
#define LED_GREEN		BOARD_LED_TXL

#ifdef __cplusplus
}
#endif

#endif // USER_BOARD_H
