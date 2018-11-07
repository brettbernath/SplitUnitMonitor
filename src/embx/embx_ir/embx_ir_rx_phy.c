/**
 * @file embx_ir_rx_phy.c
 * @date 10/24/2018
 * @author: bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 *
 * @brief The module controls the reception of data from an external IR receiver connected to the SAMD21G18A via a GPIO pin.
 * @details This module uses a timer and a gpio submodule implemented in embx_ir_rx_gpio.c,.h to periodically sample the GPIO line
 * connected to the IR receiver.  Sampling of received data is controlled by a state machine and utilizes embx_ir_rx_buffer.c,.h
 * to store the marks/space and timing information.
 * 
 */ 
#include <asf.h>
#include "embx/embx_ir/embx_ir_rx_gpio.h"
#include "embx/embx_ir/embx_ir_rx_phy.h"

/**
* @brief The TC used by the PHY.
*/
static struct tc_module tc_instance_ir_rx_phy;

/** 
* @brief The state variable used to store the current state of the state machine. 
*/
static embx_ir_rx_phy_state_t embx_ir_rx_phy_state = EMBX_IR_RX_PHY_STATE_SYNCRONIZE;

/**
* @brief Stores the number of timer overflows that occur in each state of the machine.
* Note - The timer should not overflow in the IDLE or MARKING state.
*/ 
static embx_ir_rx_phy_timer_overflows_t embx_ir_rx_phy_timer_overflow = {0, 0, 0};
	
/**	
* @brief
*/
static embx_ir_rx_phy_stats_t embx_ir_rx_phy_stats = {0};
 	
/**
* @brief - Handles the case when the state machine wants to re-synchronize after an error.
*/
static inline void handle_resync(void)
{
	/* Change the state back to SYNC for the next reception */
	embx_ir_rx_phy_state = EMBX_IR_RX_PHY_STATE_SYNCRONIZE;
	/* restart the timer with the SYNC delay */
	embx_ir_rx_phy_restart_timer(EMBX_IR_RX_PHY_SYNC_DELAY);
	
	embx_ir_rx_phy_stats.resyncs++;
}

/**
* @brief - Handles buffer overflows
*/
static inline void handle_overflow(void)
{
	if( embx_ir_rx_buf_complete(STATUS_ERR_OVERFLOW) == STATUS_OK ) {
		/* Change the state back to IDLE for the next reception, no need to start timer */
		embx_ir_rx_phy_state = EMBX_IR_RX_PHY_STATE_IDLE;
	} else {
		embx_ir_rx_phy_stats.buffer_overflows++;
		handle_resync();
	}	
}

/**
* @brief Handle the syncronization logic.
* @details To be syncronized, the state machine is looking for a timeout to occur without receiving any data.
*/
static inline void handle_state_synchronize(embx_ir_rx_event_t event, uint32_t count)
{
	if( event == EMBX_IR_RX_TIMER_EVENT_TIMEOUT ) { /** A timeout event has occured */
		if( count > EMBX_IR_RX_PHY_SYNC_DELAY ) { /** Check if the elapsed time is greater than the required time for synchronization to occur */
			embx_ir_rx_phy_state = EMBX_IR_RX_PHY_STATE_IDLE; /** If so, we are synced so move to the idle state */
		} else { /** Otherwise start over */
			embx_ir_rx_phy_restart_timer(EMBX_IR_RX_PHY_SYNC_DELAY); 
		}
	} else { /** If a GPIO event happened, start over */
		embx_ir_rx_phy_restart_timer(EMBX_IR_RX_PHY_SYNC_DELAY);
	}	
}

/**
* @brief Handle the Idle state logix.
* @details In the IDLE state, the machine is looking for a FALLING_EDGE event.  We can always be IDLE so a TIMEOUT should never occur.
*  When a FALLING_EDGE event occurs, the state machine will move to the MARKING state.
*/
static inline void handle_state_idle(embx_ir_rx_event_t event)
{
	if( event == EMBX_IR_RX_GPIO_EVENT_FALLING_EDGE ) { /* Falling edge detected, so transition to MARKING */
		/* Start the counter to time the first mark */
		embx_ir_rx_phy_restart_timer(EMBX_IR_RX_PHY_MARK_DELAY);
				
		/* Change the state to MARKING */
		embx_ir_rx_phy_state = EMBX_IR_RX_PHY_STATE_MARKING;
	} else if( event == EMBX_IR_RX_TIMER_EVENT_TIMEOUT ) { /* This shouldn't happen so if idle_timer_overflows is > 0 something is wrong */
		embx_ir_rx_phy_timer_overflow.idle++;
	}
}

/** 
* @brief Handles when A MARK has been received based on a RISING_EDGE event in the SPACING state.
* @params uint32_t count - The timer value in ticks that measures the duration of the LOW line or MARKINg state that preceded the RISING_EDGE event
* @returns status_code - STATUS_OK if all went well 
*                        STATUS_ERR_OVERFLOW if the current buffer is out of buffer elements
*                        STATUS_ERR_NO_MEMORY if we are out of buffers.
*/
static inline enum status_code handle_received_mark(uint32_t count)
{
	embx_ir_rx_buf_elem_t *rx_buf_elem;

	/** Get a buffer element from the current buffer */
	enum status_code rval = embx_ir_rx_buf_isr_get_elem(&rx_buf_elem);
	if( rval == STATUS_OK ) {	/** A buffer element is available so record relevant info */
		rx_buf_elem->gpio_state = EMBX_IR_RX_GPIO_STATE_MARK; /** This is a MARK */
		rx_buf_elem->ticks = count + EMBX_IR_RX_PHY_MARK_DELAY * embx_ir_rx_phy_timer_overflow.mark; /** This is the duration of the MARK in timer ticks */
		rx_buf_elem->time_us = rx_buf_elem->ticks * EMBX_IR_RX_PHY_USEC_PER_TICK; /** This is the duration of the MARK in us */
		
		/** A SPACE or an IDLE follows a MARK so restart the counter to time the SPACE */
		embx_ir_rx_phy_restart_timer(EMBX_IR_RX_PHY_SPACE_DELAY); 
		/** Change the state to SPACING */
		embx_ir_rx_phy_state = EMBX_IR_RX_PHY_STATE_SPACING;		
	} else if( rval == STATUS_ERR_OVERFLOW ) { /** Buffer is out of buffer elements */
		handle_overflow();
	} else if (rval == STATUS_ERR_NO_MEMORY ) { /* All the buffers are full */
		handle_resync();
	}
	return rval;
}

static inline enum status_code handle_received_space(uint32_t count)
{
	embx_ir_rx_buf_elem_t *rx_buf_elem;
	enum status_code rval = embx_ir_rx_buf_isr_get_elem(&rx_buf_elem);
	if( rval == STATUS_OK ) {
		rx_buf_elem->gpio_state = EMBX_IR_RX_GPIO_STATE_SPACE;
		rx_buf_elem->ticks = count;
		rx_buf_elem->time_us = rx_buf_elem->ticks * EMBX_IR_RX_PHY_USEC_PER_TICK;
					
		/* Restart the counter to time the MARK */
		embx_ir_rx_phy_restart_timer(EMBX_IR_RX_PHY_MARK_DELAY);
		/* Change the state */
		embx_ir_rx_phy_state = EMBX_IR_RX_PHY_STATE_MARKING;		
	}  else if( rval == STATUS_ERR_OVERFLOW ) { /* Buffer is out of buffer elements */
		handle_overflow();
	} else if (rval == STATUS_ERR_NO_MEMORY ) { /* All the buffers are full */
		handle_resync();
	}
	return rval;
}

/**
* @brief Handles when a reception has been completed.
* @details - On success, the buffer is marked as full and the state is changed to IDLE.
*            On failure, the state machine is returned to the SYNCRONIZING state.
*/
static inline void handle_rx_complete(enum status_code buffer_status)
{
	if( embx_ir_rx_buf_complete(buffer_status) == STATUS_OK ) {
		/* Change the state back to IDLE for the next reception, no need to start timer */
		embx_ir_rx_phy_state = EMBX_IR_RX_PHY_STATE_IDLE;
	} else {
		handle_resync();
	}	
}

/**
* @brief Implements the State Machine that handles received events from the GPIO pin connected to the IR receiver and the timer.
* @details - Initially, the state machine is in the SYNCHRONIZE state.  In this state, it waits for a period of time where the
* line is SPACING without any received MARKS.  When this condition met, the line is considered to be IDLE.  An IDLE period terminates
* by a falling edge on the GPIO.  This starts the MARKING state of the line.  A timer is started to measure the duration of
* the MARKING period when the line is LOW.  The Marking State is terminated by a rising edge on the GPIO.  The marking time and state is saved in a rx_buffer_elem 
* and the Spacing State begins.  A timer is started to measure the duration of the SPACE.  The SPACING state may be terminated by a falling edge on the GPIO or
* by a TIMER timerout.  A falling edge event from the GPIO causes the spacing time and state to be saved into a rx_buf_elem.  A timeout signals the end
* of the data reception and the state machine returns to the IDLE state.
* To measure the duration of marks and spaces, the timer is used.  
* @params embx_ir_rx_event_t - the event that the state machine handles.  Currently, the GPIO generates rising and falling edge events and the timer generates timeout events.
*/
void embx_rx_ir_phy_state_machine(embx_ir_rx_event_t event)
{
	uint32_t count = tc_get_count_value(&tc_instance_ir_rx_phy);
	tc_stop_counter(&tc_instance_ir_rx_phy);
		
	switch(embx_ir_rx_phy_state)
	{
		case EMBX_IR_RX_PHY_STATE_SYNCRONIZE:
			handle_state_synchronize(event, count);
		break;
		case EMBX_IR_RX_PHY_STATE_IDLE: /* Line is High */
			handle_state_idle(event);
		break;
		case EMBX_IR_RX_PHY_STATE_MARKING: /* Line is Low or MARKING */
			if( event == EMBX_IR_RX_GPIO_EVENT_RISING_EDGE ) { /* Rising edge detected, so transition to SPACING */
				/* Store the duration of the last mark and the last line state (MARK) */
				handle_received_mark(count);
				embx_ir_rx_phy_timer_overflow.mark = 0; /* Set back to 0 */				
			} else if( event == EMBX_IR_RX_TIMER_EVENT_TIMEOUT ) { /* Should not timeout while MARKING */
				if( embx_ir_rx_phy_timer_overflow.mark == EMBX_IR_RX_PHY_TIMER_OVERFLOWS_MARK ) {
					embx_ir_rx_phy_timer_overflow.mark = 0;
					/* Reception is done, mark the buffer as full */
					handle_rx_complete(STATUS_ERR_TIMEOUT);
				} else {
					embx_ir_rx_phy_timer_overflow.mark++;						
					embx_ir_rx_phy_restart_timer(EMBX_IR_RX_PHY_MARK_DELAY);
				}
			}
		break;
		case EMBX_IR_RX_PHY_STATE_SPACING:						
			if( event == EMBX_IR_RX_GPIO_EVENT_FALLING_EDGE ) { /* Falling Edge detected, handle the received space and change state back to marking */
				handle_received_space(count);
			} else if( event == EMBX_IR_RX_TIMER_EVENT_TIMEOUT ) {
				if( embx_ir_rx_phy_timer_overflow.space == EMBX_IR_RX_PHY_TIMER_OVERFLOWS_SPACE ) {			
					embx_ir_rx_phy_timer_overflow.space = 0;
					/* Reception is done, mark the buffer as full */
					handle_rx_complete(STATUS_OK);
				} else {
					embx_ir_rx_phy_timer_overflow.space++;
					embx_ir_rx_phy_restart_timer(EMBX_IR_RX_PHY_SPACE_DELAY);													
				}				
			}			
		break;
		default:
			handle_resync();
		break;
	}	
}


/**
* @brief The callback function occurs when the TC times out.
* @details TC times out when ...
*/
static void tc_callback_ir_rx_phy( struct tc_module *const module_inst)
{
	embx_rx_ir_phy_state_machine(EMBX_IR_RX_TIMER_EVENT_TIMEOUT);
}
 
/**
* @brief register and enable the callback function for the TC OVERFLOW interrupt
*/
static void embx_time_configure_tc_callbacks(void)
{
	tc_register_callback(&tc_instance_ir_rx_phy, &tc_callback_ir_rx_phy, TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc_instance_ir_rx_phy, TC_CALLBACK_CC_CHANNEL0);
}

/**
* @brief Initializes the IR RX PHY timer
* @details TC is configured to use an 16-bit counter clocked at 8 Mhz / EMBX_IR_RX_PHY_PRESCALER.  Currently,
* this results in 8 usec per tick.  This function initializes, enables, and stops the counter.
* @param gclk which clock to use....See embx_ir_common.h
* @returns void
*/
static void embx_ir_rx_phy_tc_init(enum gclk_generator gclk)
{
	struct tc_config config_tc;
	
	tc_get_config_defaults(&config_tc);
	
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT;
	config_tc.clock_source = gclk; /* 8 MHz */
	config_tc.clock_prescaler = EMBX_IR_RX_PHY_PRESCALER;  /* 8 us per tick  */
	
	tc_init(&tc_instance_ir_rx_phy, TC_IR_RX_PHY_MODULE, &config_tc);
	tc_enable(&tc_instance_ir_rx_phy);
	tc_stop_counter(&tc_instance_ir_rx_phy);

	embx_time_configure_tc_callbacks();
}


/**
* @brief - Stops the counter and resets the value to 0.
*/
void embx_ir_rx_phy_stop_timer(void)
{
	tc_stop_counter(&tc_instance_ir_rx_phy);
}

/**
* @brief - Starts the counter.
*/
void embx_ir_rx_phy_start_timer(embx_ir_rx_phy_timeout_t timeout)
{
	tc_stop_counter(&tc_instance_ir_rx_phy);
	tc_set_compare_value(&tc_instance_ir_rx_phy, TC_COMPARE_CAPTURE_CHANNEL_0, timeout);  /* 20 ms = 8 us per tick * x ticks, x = 20 e3 / 8 e6 */
	tc_start_counter(&tc_instance_ir_rx_phy);
}

/**
* @brief - Re-starts the counter.
*/
void embx_ir_rx_phy_restart_timer(embx_ir_rx_phy_timeout_t timeout)
{
	tc_set_compare_value(&tc_instance_ir_rx_phy, TC_COMPARE_CAPTURE_CHANNEL_0, timeout);  /* 20 ms = 8 us per tick * x ticks, x = 20 e3 / 8 e6 */
	tc_start_counter(&tc_instance_ir_rx_phy);
}

/**
* @brief Call to initialize when the RxPhy 
*/
void embx_ir_rx_phy_init(void)
{
	embx_ir_rx_phy_state = EMBX_IR_RX_PHY_STATE_SYNCRONIZE;	
	embx_ir_rx_phy_tc_init(EMBX_IR_MODULATOR_GCLK);
	embx_ir_rx_gpio_init();	
}

void embx_ir_rx_phy_reset(void)
{
	tc_disable(&tc_instance_ir_rx_phy);
	tc_reset(&tc_instance_ir_rx_phy);
}

void embx_ir_rx_phy_enable(void)
{	
	embx_ir_rx_phy_buf_init();	
	embx_ir_rx_phy_state = EMBX_IR_RX_PHY_STATE_SYNCRONIZE;		
	embx_ir_rx_phy_start_timer(EMBX_IR_RX_PHY_SYNC_DELAY);
	embx_ir_rx_gpio_enable();	
}

void embx_ir_rx_phy_disable(void)
{
	tc_disable(&tc_instance_ir_rx_phy);
	embx_ir_rx_gpio_disable();
}

void embx_ir_rx_phy_tb(void)
{
	embx_ir_rx_phy_init();
	embx_ir_rx_phy_enable();
	while(1) ;
}