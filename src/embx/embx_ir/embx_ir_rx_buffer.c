/**
* @file embx_ir_rx_buffer.c
* @date 10/30/2018
* @author bbernath
* @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
* 
* @brief The embx_ir_rx_buffer module implements a buffer to store data received via a gpio pin from and IR receiver.
* @details The module is structured around it's data.  Some terms:
*		-A buffer is defined as an array of buffer elements
*		-A buffer element represents the state of the GPIO pin for a given interval of time while receiving data from the IR receiver.
*
*	The module implements methods that allow users to reset the buffer, store and retrieve data, gather statistics, ...
*/ 
#include <asf.h>
#include "embx/embx_ir/embx_ir_rx_buffer.h"

/** The buffers used to store data received via IR */
static embx_ir_rx_buf_t embx_ir_rx_buf[EMBX_IR_RX_NUMBER_OF_BUFFERS];

/** Records any errors that may occur */
static embx_ir_rx_buf_err_t embx_ir_rx_buf_err = {0, 0};

/** The buffer index used by the interrupt service routines */
static uint8_t idx_ir_rx_buf_isr = 0;

/** 
* @brief Resets a single buffer to a known state.
* @params idx - the index of the buffer to reset.  This value is boundary checked.
* @returns STATUS_OK if the index is valid else STATUS_ERR_NO_MEMORY
*/
enum status_code embx_ir_rx_phy_buf_reset(uint8_t idx)
{
	enum status_code rval = STATUS_OK;
	uint16_t i;
	
	if( idx < EMBX_IR_RX_NUMBER_OF_BUFFERS ) {
		embx_ir_rx_buf[idx].state = EMBX_IR_RX_BUF_EMPTY;
		embx_ir_rx_buf[idx].status = STATUS_OK;
		embx_ir_rx_buf[idx].size = 0;
		for( i = 0; i < EMBX_IR_RX_BUF_SZ; i++ ) {
			embx_ir_rx_buf[idx].elem[i].gpio_state = EMBX_IR_RX_GPIO_STATE_UNINITIALIZED;
		}
	} else {
		rval = STATUS_ERR_NO_MEMORY	;
	}
	return rval;
}

/**
* @brief - Resets the module statistics to 0.
*/
static void embx_ir_rx_phy_buf_reset_stats(void)
{
	embx_ir_rx_buf_err.overflows = 0 ;
	embx_ir_rx_buf_err.no_memory = 0 ;		
}

/**
* @brief - Initializes the module to a known state.
* @details - Resets each buffer and it's elements, resets the module statistics, and sets the index to 0
*/
void embx_ir_rx_phy_buf_init(void)
{
	uint8_t idx = 0;
	while( 1 ) {
		if( embx_ir_rx_phy_buf_reset(idx++) != STATUS_OK ) { break; }
	}
	
	embx_ir_rx_phy_buf_reset_stats();	
	
	idx_ir_rx_buf_isr = 0;
}

/**
* @brief Returns the address of the current buffer element in the current buffer.
* @details Returns the address AND increments the index to the next buffer element in the buffer.
* @returns STATUS_OK if all is well or STATUS_ERR_NO_MEMORY if there are no more buffers or STATUS_ERR_OVERFLOW is there are no free buffer elements
*/
enum status_code embx_ir_rx_buf_isr_get_elem(embx_ir_rx_buf_elem_t **buf_elem)
{
	enum status_code rval = STATUS_OK;
	uint16_t sz;
	
	/** The current buffer remains EMPTY until the complete function is called */
	if( embx_ir_rx_buf[idx_ir_rx_buf_isr].state == EMBX_IR_RX_BUF_EMPTY ) {
		sz = embx_ir_rx_buf[idx_ir_rx_buf_isr].size;
		if(  sz < EMBX_IR_RX_BUF_SZ ) { /** Make sure that we will not overrun the buffer */
			*buf_elem = &(embx_ir_rx_buf[idx_ir_rx_buf_isr].elem[sz]); /** Get the next element from the buffer */
			embx_ir_rx_buf[idx_ir_rx_buf_isr].size++; /** Increment the buffer size */
		} else { /* No more buffer elements available in the buffer, the date will be dropped, return an error */
			embx_ir_rx_buf_err.overflows++;
			rval = STATUS_ERR_OVERFLOW;
		}
	} else { /* No Available Buffers */
		embx_ir_rx_buf_err.no_memory++;
		rval = STATUS_ERR_NO_MEMORY;		
	}
		
	embx_ir_rx_buf[idx_ir_rx_buf_isr].status = rval;			
	
	return rval;
}

/**
* @brief Call to indicate that the buffer's data is is ready for processing.
* @details The buf_state is set to EMBX_IR_RX_BUF_FULL to allow the background loop to process the buffer.
* @params status_code - This is meant for the caller to set status variable in the buffer.  Possible values are
*  STATUS_OK or STATUS_ERR_TIMEOUT.
* @params status_code - Only STATUS_OK
*/
extern enum status_code embx_ir_rx_buf_complete(enum status_code buffer_status)
{
	enum status_code rval = STATUS_OK;
	embx_ir_rx_buf[idx_ir_rx_buf_isr].state = EMBX_IR_RX_BUF_FULL; /** The current buffer is full */
	embx_ir_rx_buf[idx_ir_rx_buf_isr].status = buffer_status; /** The caller sets the status */
	idx_ir_rx_buf_isr++; /** increment the buffer index and */
	if( idx_ir_rx_buf_isr == EMBX_IR_RX_NUMBER_OF_BUFFERS ) { /** check the boundary */
		idx_ir_rx_buf_isr = 0;		
	}		
	return rval;
}
