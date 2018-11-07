/**
 * @file embx_ir_tx_phy_descriptor.c
 * @date 10/7/2018
 * @author: bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.   
 *
 * @brief An implementation of a IR phy layer transmission descriptor for use on the Atmel SamD21G embedded controller.
 */ 
#include <asf.h>
#include "embx/embx_ir/embx_ir_tx_phy_descriptor.h"
#include "embx/embx_ir/embx_ir_tx_phy.h"


/** The descriptors are stored in an array.  This defines the size of the array of IR PHY transmission descriptors. */
#define EMBX_IR_TX_PHY_DESCRIPTOR_Q_SZ (20)
/** This declares the array of IR PHY transmission descriptors */
embx_ir_tx_phy_descriptor_t phy_descriptor[EMBX_IR_TX_PHY_DESCRIPTOR_Q_SZ];

/** The fill index is used by the function to populate a descriptor within the array of descriptors. */
static uint8_t fill_index = 0;
/** The tx index is used by the timer call back function to manage the modulation of the IR device. */
static uint8_t tx_index = 0;

/**
* @brief Call to convert the mark space intervals from units of time (usec) to tc clock ticks.
* @details Expects that the usec field is populated.
* @returns void
*/
static inline void embx_ir_tx_phy_descriptor_tc_init(embx_ir_tx_phy_descriptor_t *pd)
{
	pd->period = pd->usec / EMBX_IR_TX_PHY_USEC_PER_TICK ;
	if( pd->period > UINT8_MAX ) {
		pd->overflows = pd->usec / (UINT8_MAX+1);
		pd->period = pd->usec % (UINT8_MAX+1);
	} else {
		pd->overflows = 0;
	}
}

/**
* @brief decrements the tx_index by the amount specified by the decrement parameter.
* @details If the decrement parameter is greater than the tx_index, then the tx_index will be set to 0.
* @param decrement the amount to decrement the tx_index.
* @returns void
*/
void embx_ir_tx_phy_decrement_tx_index(uint8_t decrement) 
{
	if( tx_index >= decrement ) {
		tx_index -= decrement;
	} else tx_index = 0;
}

/**
* @brief - Fills in an ir tx phy descriptor.
*
* Fill in an ir tx phy descriptor.  Each descriptor describes a mark or a space in units of time usec per mark or space.
* The descriptors are maintained as list.  The list is populated by calling this function repeatedly.  
* The reset_descriptor_list flag will cause the list to be reset and the parameters will be applied to the first 
* list element which describes the first mark or space to be transmitted.
* 
* Each descriptor may cause the previous N descriptors to be be re-tranmitted multiple times.
* 
* The max_repeat_cnt value cause the previous N descriptors to be re-transmitted max_repeat_cnt times.  
* The N descriptors will be transmitted then retransmitted max_repeat_cnt times.  The N descriptors
* will be transmitted a total of max_repeat_cnt + 1 times.
* Set the max_repeat_cnt value to EMBX_IR_TX_PHY_REPEAT_FOREVER to repeat forever ... useful for debugging.
* 
* The decrement value defines N or the number of descriptors that will be re-transmitted.  N includes the 
* descriptor filled in by this function.  D0, D1, D2    D2 is the current descriptor.  If decrement == 3 and max_repeat_count == 2
* you will see D0, D1, D2, D0, D1, D2, D0, D1, D1 on the wire.
*
* @param[in] bool reset_descriptor_list True if the descriptor describes the first mark or space in the transmission list.  
*										False if the descriptor should be added to the end of the q
* @param[in] phy_interval_t - either a mark or a space.
* @param[in] uint16_t - the number of usec that the mark or space should be transmitted for ...
* @param[in] max_repeat_count - the number of times the previous descriptors should be repeated.
* @param[in] decrement - how far back on the list a repeat should go.
*
* @returns - STATUS_BUSY if there is an IR transmission underway.  Try again later ...
*			 STATUS_ERR_OVERFLOW if there are not any free descriptors left.  Try increasing the q size and re-compiling.
*            STATUS_OK if the descriptor has been populated and added to the transmission q.
*/
enum status_code embx_ir_tx_phy_descriptor_fill(bool reset_descriptor_list, 
												embx_ir_tx_phy_interval_t phy_interval_type, 
												uint16_t usec, 
												int16_t max_repeat_cnt, uint8_t decrement)
{
	enum status_code status = STATUS_OK;

	if ( embx_ir_tx_phy_get_state() == true ) {
		return STATUS_BUSY;
	}
	
	if( reset_descriptor_list == true ) {
		fill_index = 0;
		tx_index = 0;
	}
	
	if( fill_index < EMBX_IR_TX_PHY_DESCRIPTOR_Q_SZ ) {		
		
		/* Transfer the parameters to the descriptor */
		phy_descriptor[fill_index].phy_interval_type = phy_interval_type;
		phy_descriptor[fill_index].usec = usec;
		embx_ir_tx_phy_descriptor_tc_init(&phy_descriptor[fill_index]);
		
		if( max_repeat_cnt >= 0 ) {
			phy_descriptor[fill_index].max_repeat_cnt = max_repeat_cnt;
		} else {
			phy_descriptor[fill_index].max_repeat_cnt = EMBX_IR_TX_PHY_REPEAT_FOREVER;
		}		
		phy_descriptor[fill_index].repeat_cnt = 0;		
		
		/* Ensure that the decrement value is will not result in a value out of bounds of the array */
		if( decrement <= (fill_index + 1) ) {
			phy_descriptor[fill_index].decrement = decrement;			
		} else {
			phy_descriptor[fill_index].decrement = 0;
		}		
		
		fill_index++;	
				
	} else {
		status = STATUS_ERR_OVERFLOW;
	}

	return status;
}

/**
* @brief Returns the current descriptor and increments the tx_index.
*
* @details If tx_index is equal to the fill_index, then we have transmitted all the descriptors.
* In this case, the function will return an error.  This error indicates to the caller
* that the transmission is complete as there are no more descriptors left to process.
* When this occurs, the index may be reset by calling the fill function
* with the reset flag set to true or by calling the decrement function.
*
* @param[out] - sets pd to the descriptor referenced by the current value of tx_index.
*
* @returns STATUS_OK if the descriptor returned to the caller is valid.
* or STATUS_ERR_BAD_DATA if there is no more data left in the q.
*/
enum status_code embx_ir_tx_phy_descriptor_get(embx_ir_tx_phy_descriptor_t **pd)
{
	enum status_code status = STATUS_OK;
	if( tx_index < fill_index ) {
		*pd = &phy_descriptor[tx_index]; 
		tx_index++;	
	} else {
		status = STATUS_ERR_BAD_DATA;
	}
	return status;
}