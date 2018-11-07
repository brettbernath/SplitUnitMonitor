/**
 * @file embx_ir_rx_gpio.h
 * @date 10/11/2018
 * @author: bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 *
 * @brief The embx_ir_rx_gpio module handles the input pin connected to an IR receiver.
 * @details - This module configures a GPIO pin to act as an EXTINT channel where
 *            the pin will generate an interrupt based upon a rising or falling edge
 *            generated by the IR receiver.
 *            The module provides methods to initialize, enable, and disable the EXTINT channel.  It also provides a
 *            callback function that handles the rising or falling edge events.  The call back function simply
 *            calls the embx_rx_ir_phy_state_machine.  The state machine handles the event.
*/ 
#ifndef EMBX_IR_RX_GPIO_H_
#define EMBX_IR_RX_GPIO_H_

/** @brief Records the number of IR receiver events */
typedef struct {
	uint32_t rising_edge_events;
	uint32_t falling_edge_events;	
} embx_ir_rx_gpio_stats_t;

/** @brief The pin that is connected to the IR receiver */
#define EMBX_IR_RX_EIC_PIN			(PIN_PA18A_EIC_EXTINT2)		/** BOARD_D10 */
#define EMBX_IR_RX_EIC_MUX			(MUX_PA18A_EIC_EXTINT2)     /** The mux setting */
#define EMBX_IR_RX_EIC_LINE			(2)							/* Line refers to the number of the EXTINT i.e. EXTINT2 - This number is the CHANNEL  */
#define EMBX_IR_RX_EIC_CHANNEL		(EMBX_IR_RX_EIC_LINE)       /* Line refers to the number of the EXTINT i.e. EXTINT2 - This number is the CHANNEL  */

/**
* @brief Initializes the module.
* @details Resets the statistics, configures the EXTINT channel gpio pin to trigger events on both rising and falling
* edges, and uses an internal pullup resistor.  Registers the callback, but does not enable it.
* The enable function (see below) enables the callback and allows the module to function.
*/
extern void embx_ir_rx_gpio_init(void);
/** @brief - Enables the module.*/
extern void embx_ir_rx_gpio_enable(void);
/** @brief - Disables the module.*/
extern void embx_ir_rx_gpio_disable(void);

#endif /* EMBX_IR_RX_GPIO_H_ */