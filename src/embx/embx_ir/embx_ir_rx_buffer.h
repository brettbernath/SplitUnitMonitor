/**
 * @file embx_ir_rx_buffer.h
 * @date 10/30/2018
 * @author bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 */ 

#ifndef EMBX_IR_RX_BUFFER_H_
#define EMBX_IR_RX_BUFFER_H_

/** The number of IR Rx Data Buffers */
#define EMBX_IR_RX_NUMBER_OF_BUFFERS	(4)

/** The number of Buffer Elements per IR Rx Data Buffer */
#define EMBX_IR_RX_BUF_SZ   (256)

/**
* @brief embx_ir_rx_gpio_state_t describes the state of the GPIO pin connected to the IR receiver.
* @details - A MARK is when the GPIO pin is LOW.  A SPACE is when the GPIO pin is HI.
*/
typedef enum {
	EMBX_IR_RX_GPIO_STATE_MARK = 0,
	EMBX_IR_RX_GPIO_STATE_SPACE = 1,
	EMBX_IR_RX_GPIO_STATE_UNINITIALIZED = 2,
} embx_ir_rx_gpio_state_t;

/**
* @brief embx_ir_rx_buf_elem_t is an element of a data buffer. 
* @details An ir rx buffer element records the state of the line and records the duration of that state.
*  For any given interval of time the line may be a mark or space.  This is stored in gpio_state.
*  The duration of the mark or space is recorded in ticks and time_us (time in microseconds). 
*/
typedef struct {
	embx_ir_rx_gpio_state_t gpio_state; /** MARK, SPACE, or UNKNOWN */
	uint32_t ticks;
	uint32_t time_us;
} embx_ir_rx_buf_elem_t;

/**
* @brief embx_ir_rx_buf_state_t indicates whether the buffer is full or empty.
*/
typedef enum {
	EMBX_IR_RX_BUF_EMPTY = 0,
	EMBX_IR_RX_BUF_FULL,
} embx_ir_rx_buf_state_t;

/**
* @brief embx_ir_rx_buf_t is a descriptor for a single buffer.
* @details size is incremented for each buffer element added to the array.
*/
typedef struct {
	enum status_code status;
	uint16_t size; /** The size of the buf */
	embx_ir_rx_buf_state_t state; /** Set to FULL by the interrupt handler, set to EMPTY initially and when the main loop finishes with the buffer */
	embx_ir_rx_buf_elem_t elem[EMBX_IR_RX_BUF_SZ];
} embx_ir_rx_buf_t;

/**
* @brief Count the errors that occur within the module.
*/
typedef struct {
	uint32_t overflows; /** Buffer is out of elements */
	uint32_t no_memory; /** Out of buffers */
} embx_ir_rx_buf_err_t;

/** 
* @brief Resets a single buffer to a known state.
* @params idx - the index of the buffer to reset.  This value is boundary checked.
* @returns STATUS_OK if the index is valid else STATUS_ERR_NO_MEMORY
*/
extern enum status_code embx_ir_rx_phy_buf_reset(uint8_t idx);

/** @brief - Initializes all the date buffers to a known state */
extern void embx_ir_rx_phy_buf_init(void);

/** 
	@brief returns the next empty buffer element from the current buffer. 
	@details only to be called from within the ISR 
*/
extern enum status_code embx_ir_rx_buf_isr_get_elem(embx_ir_rx_buf_elem_t **buf_elem);

/** 
	@brief Marks the current buffer as FULL and ready for processing. 
	@details - only to be called from within the ISR ....
	@params - status - allows the caller to mark the buffer with a status value.
*/
extern enum status_code embx_ir_rx_buf_complete(enum status_code buffer_status);

#endif /* EMBX_IR_RX_BUFFER_H_ */