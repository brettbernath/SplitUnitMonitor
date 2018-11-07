/**
 * @file embx_ir_tx_phy.h
 * @date 10/3/2018
 * @author bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 *
 * @brief Implementation of the IR Tx PHY module.
 * @details Contains functions that provide an interface between the protocol layer and the IR modulator.
 * Data to be transmitted is received from the IR tx protocol layer in the form of a PHY descriptor.
 * Each PHY descriptor describes a mark or space and the amount of time to transmit the mark or space.
 * The PHY layer uses the descriptor information to control the ir tx modulator.
 * A timer is used to control the timing of when to turn the IR modulator on and off.
 */ 
#ifndef EMBX_IR_TX_PHY_H_
#define EMBX_IR_TX_PHY_H_

/** The PHY TX Module uses TC3 */
#define TC_IR_TX_PHY_MODULE			TC3

/** @brief The prescaler is used to divide the 8 MHz GCLK by 64 resulting in an 8 us clk for the timer. */
#define EMBX_IR_TX_PHY_PRESCALER TC_CLOCK_PRESCALER_DIV64
#define EMBX_IR_TX_PHY_CLK_FREQ				(EMBX_IR_MODULATOR_GCLK / 64) /* 125 kHz */	
#define EMBX_IR_TX_PHY_USEC_PER_TICK		(8)   /* 8 us = 8000000 / 64 */

/** 
* @brief Returns the current state of the Tx PHY. 
* @returns - TRUE if a transmission is currently in progress; else false. 
*/
extern bool embx_ir_tx_phy_get_state(void);
/** @brief Sends or transmits a queue of descriptors */
extern enum status_code embx_ir_tx_phy_send(void);
/** @brief - Test bench for the module. */
extern void embx_time_tb(void);

#endif /* EMBX_IR_TX_PHY_H_ */