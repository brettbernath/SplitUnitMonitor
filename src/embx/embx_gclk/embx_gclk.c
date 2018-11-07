/**
 * @file - embx_gclk.c
 * @date - Created: 10/3/2018 5:17:52 PM
 * @author - bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 *
 * @brief The embx_gclk module allows viewing the various clocks on a GPIO pin. 
 * @details - clock configuration is performed in "config/conf_clocks.h"
 */ 
#include <asf.h>
#include "embx/embx_gclk/embx_gclk.h"

void embx_gclk_view_on_gpio(enum gclk_generator gclk)
{
	uint8_t gpio_pin;
	struct system_pinmux_config pin_config;
	
	system_pinmux_get_config_defaults(&pin_config);
	pin_config.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;

	switch(gclk) {
		case GCLK_GENERATOR_0:
		pin_config.mux_position = MUX_PA14H_GCLK_IO0; /* PA14 -> D02 */
		gpio_pin = PIN_PA14H_GCLK_IO0;
		break;
		case GCLK_GENERATOR_1:
		pin_config.mux_position = MUX_PA15H_GCLK_IO1; /* PA15 -> D05 */
		gpio_pin = PIN_PA15H_GCLK_IO1;
		break;
		case GCLK_GENERATOR_2:
		pin_config.mux_position = MUX_PA16H_GCLK_IO2; /* PA16 -> D11 */
		gpio_pin = PIN_PA16H_GCLK_IO2;
		break;
		case GCLK_GENERATOR_3:
		pin_config.mux_position = MUX_PA17H_GCLK_IO3; /* PA17 -> D13 */
		gpio_pin = PIN_PA17H_GCLK_IO3;
		break;
		case GCLK_GENERATOR_4:
		pin_config.mux_position = MUX_PA10H_GCLK_IO4; /* PA10 -> D1 */
		gpio_pin = PIN_PA10H_GCLK_IO4;
		break;
		case GCLK_GENERATOR_5:
		pin_config.mux_position = MUX_PA11H_GCLK_IO5; /* PA11 -> D0 */
		gpio_pin = 	PIN_PA11H_GCLK_IO5;
		break;
		case GCLK_GENERATOR_6:
		pin_config.mux_position = MUX_PA22H_GCLK_IO6; /* PA22 -> BOARD_30_TX */
		gpio_pin = 	PIN_PA22H_GCLK_IO6;
		break;
		case GCLK_GENERATOR_7:
		pin_config.mux_position = MUX_PA23H_GCLK_IO7; /* PA23 -> BOARD_31_RX */
		gpio_pin = 	PIN_PA23H_GCLK_IO7;
		break;
		default: return;
	}
	system_pinmux_pin_set_config(gpio_pin, &pin_config);
}


