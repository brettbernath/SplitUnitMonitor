/**
 * @file embx_ir_tx_phy_descriptor.h
 * @date 10/7/2018 
 * @author bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 *
 * @brief An implementation of a IR phy layer transmission descriptor for use on the Atmel SamD21G embedded controller.
 * @details This file defines the types, data structures, and functions that are used to interface with the descriptors.
 */ 
#ifndef EMBX_IR_TX_PHY_DESCRIPTOR_H_
#define EMBX_IR_TX_PHY_DESCRIPTOR_H_

/** 
*	Defines the value for the max_repeat_cnt parameter to cause the mark space pattern to be generated forever.
*   To generate a continuous mark space transmission, use this definition when calling the fill function.
*/
#define EMBX_IR_TX_PHY_REPEAT_FOREVER		(-1)

/**
* Defines a enum representing a mark or a space.
*/
typedef enum {
	unknown = -1,
	space = 0,
	mark = 1,
} embx_ir_tx_phy_interval_t;

/**
* Defines the IR Tx PHY descriptor.
*/
typedef struct {
	embx_ir_tx_phy_interval_t phy_interval_type; /** Indicates whether the time interval is a mark or a space. */
	uint16_t usec; /** The number of usec in the mark or space */
	uint16_t period;	 /** The value programmed into the top register: period = usec / EMBX_IR_TX_PHY_USEC_PER_TICK  */
	uint8_t overflows; /** used if the period is > 256, */
	int16_t repeat_cnt; /** The current value */
	int16_t max_repeat_cnt; /** number of times to repeat the previous descriptors, -1 indicates forever (for debugging) */ 
	uint8_t decrement; /** amount to go back for a repeat operation */
} embx_ir_tx_phy_descriptor_t;

/**
* @brief Decrements the tx_index.  Used by the phy tx callback during repeat operations.
*/
extern void embx_ir_tx_phy_decrement_tx_index(uint8_t decrement) ;
/** 
* @brief Gets the next descriptor from the transmission q.  Returns a STATUS_ERR_BAD_DATA if the transmission is complete.
*/
extern enum status_code embx_ir_tx_phy_descriptor_get(embx_ir_tx_phy_descriptor_t **pd);
/**
* @brief Called to populate a descriptor.
*/
extern enum status_code embx_ir_tx_phy_descriptor_fill(bool reset, 
													   embx_ir_tx_phy_interval_t phy_interval_type, 
													   uint16_t usec, 
													   int16_t max_repeat_cnt, uint8_t decrement);

#endif /* EMBX_IR_TX_PHY_DESCRIPTOR_H_ */