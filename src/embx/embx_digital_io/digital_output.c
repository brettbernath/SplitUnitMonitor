/**
* @file digital_output.c
* @date 9/15/2018 1:39:44 PM
* @author bbernath
* @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
* 
* @brief This digital_output module is configures a gpio pin as an output pin and provides methods to change and read back the level.
* @details - This module was implemented primarily to map the pins that are going to be used as digital
* outputs into the digital_output_t enumeration so that I will remember which pins have been reserved as GPIO.
*/ 
#include <asf.h>
#include "embx/embx_digital_io/digital_output.h"

/** 
* @brief Configures a GPIO pin as a digital output with readback capability.
* @param[in] - digital_output_t pin which pin to use.  
* @param[in] - the initial level of the pin.
* @returns - void
*/
void digital_output_init(digital_output_t pin, bool level)
{
	struct port_config pin_config;
	
	port_get_config_defaults(&pin_config);
	pin_config.direction = PORT_PIN_DIR_OUTPUT_WTH_READBACK;
	port_pin_set_config(pin, &pin_config);
	digital_output_set_output(pin, level);
}

/**
* @brief Sets the port pin to a given level.
* @param[in] - digital_output_t pin - the pin to use.
* @param[in] - bool level - what to set it to ...
* @returns - void
*/
void digital_output_set_output(digital_output_t pin, bool level)
{
	port_pin_set_output_level(pin, level);
}

/**
* @brief Toggles the level of the port pin.
* @param[in] - digital_output_t pin - the pin to use.
* @returns - void
*/
void digital_output_toggle(digital_output_t pin)
{
	port_pin_toggle_output_level(pin);
}

/**
* @brief Reads the level of the port pin.
* @param[in] - digital_output_t pin - the pin to read.
* @returns - void
*/
bool digital_output_get_input(digital_output_t pin)
{
	return port_pin_get_output_level(pin);
}