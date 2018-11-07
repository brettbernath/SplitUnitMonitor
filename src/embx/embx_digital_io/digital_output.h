/**
 * @file digital_output.h
 * @details 
 * @date 9/15/2018 
 * @author bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 *
 * @brief This digital_output module is configures a gpio pin as an output pin and provides methods to change and read back the level.
 * @details - This module was implemented primarily to map the pins that are going to be used as digital
 * outputs into the digital_output_t enumeration so that I will remember which pins have been reserved as GPIO.
 */ 
#ifndef DIGITAL_OUTPUT_H_
#define DIGITAL_OUTPUT_H_

/** A list of the pins on the board reserved for GPIO purposes.  Maps between the board and the SAMD21 nomenclature. */
#define PIN_D7	PIN_PA21

/** Enumeration of the pins reserved for use as digital outputs. */
typedef enum
{
	pin_d7 = PIN_D7,
} digital_output_t;

/**
* @brief Configures a GPIO pin as a digital output with readback capability.
* @details - This function must called once for each pin that is to be used as 
* a digital output.
* @param[in] - digital_output_t pin which pin to use.
* @param[in] - the initial level of the pin.
* @returns - void
*/
extern void digital_output_init(digital_output_t pin, bool level);

/**
* @brief Sets the port pin to a given level.
* @param[in] - digital_output_t pin - the pin to use.
* @param[in] - bool level - what to set it to ...
* @returns - void
*/
extern void digital_output_set_output(digital_output_t pin, bool level);

/**
* @brief Toggles the level of the port pin.
* @param[in] - digital_output_t pin - the pin to use.
* @returns - void
*/
extern void digital_output_toggle(digital_output_t pin);

/**
* @brief Reads the level of the port pin.
* @param[in] - digital_output_t pin - the pin to read.
* @returns - void
*/
extern bool digital_output_get_input(digital_output_t pin);

#endif /* DIGITAL_OUTPUT_H_ */