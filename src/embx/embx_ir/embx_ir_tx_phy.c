/**
 * @file embx_ir_tx_phy.c
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
#include <asf.h>
#include "embx/embx_ir/embx_ir_common.h"
#include "embx/embx_ir/embx_ir_tx_modulator.h"
#include "embx/embx_digital_io/digital_output.h"
#include "embx/embx_ir/embx_ir_tx_phy_descriptor.h"
#include "embx/embx_ir/embx_ir_tx_phy.h"

/** 
* @brief State variable used to guard the descriptor queue while a transmission is in progress.  
* @details Set to true by the send function and cleared when the transmission is complete.
*/
static bool volatile embx_ir_tx_in_progress = false;

/** @brief The TC used by the PHY. */
static struct tc_module tc_instance_ir_tx_phy;

/**
* @brief The callback function occurs when the timer or TC times out.
* @details The TC times out when transmission of a mark or a space should be completed.
* When this occurs, the callback gets the next descriptor from the q and sets up the 
* next mark or space. 
* If a descriptor is not available, then transmission has completed. 
* Upon completion, this function clears the state variable.
* Transmission is started by the SEND function.
*/
static void tc_callback_ir_tx_phy( struct tc_module *const module_inst)
{
	embx_ir_tx_phy_descriptor_t *current_phy_descriptor;

	if( embx_ir_tx_phy_descriptor_get(&current_phy_descriptor) == STATUS_OK ) {
		
		tc_stop_counter(&tc_instance_ir_tx_phy);
		tc_set_top_value(&tc_instance_ir_tx_phy, current_phy_descriptor->period);

		if( current_phy_descriptor->repeat_cnt < current_phy_descriptor->max_repeat_cnt ) {
			current_phy_descriptor->repeat_cnt++;
			embx_ir_tx_phy_decrement_tx_index(current_phy_descriptor->decrement);
		} else if( current_phy_descriptor->max_repeat_cnt == EMBX_IR_TX_PHY_REPEAT_FOREVER ) {
			embx_ir_tx_phy_decrement_tx_index(current_phy_descriptor->decrement);
		}

		switch( current_phy_descriptor->phy_interval_type ) {
			case mark:
#ifdef DEBUG_IR_TX_PHY				
				port_pin_set_output_level(DEBUG_IR_TX_PHY_PIN, true);
#endif				
				embx_ir_tx_modulator_start();
			break;
			case space:
#ifdef DEBUG_IR_TX_PHY				
				port_pin_set_output_level(DEBUG_IR_TX_PHY_PIN, false);
#endif				
				embx_ir_tx_modulator_stop();
			break;
			default:
			break;
		}

		tc_start_counter(&tc_instance_ir_tx_phy);
		
	} else {
#ifdef DEBUG_IR_TX_PHY
		port_pin_set_output_level(DEBUG_IR_TX_PHY_PIN, false);
#endif		
		embx_ir_tx_modulator_stop();
		tc_stop_counter(&tc_instance_ir_tx_phy);
		embx_ir_tx_in_progress = false;
	}
}

/**
* @brief register and enable the callback function for the TC OVERFLOW interrupt
*/
static void embx_time_configure_tc_callbacks(void)
{
	tc_register_callback(&tc_instance_ir_tx_phy, tc_callback_ir_tx_phy, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance_ir_tx_phy, TC_CALLBACK_OVERFLOW);
}

/**
* @brief Initializes the IR TX PHY module. 
* @details The TC is configured to use an 8-bit counter clocked at 8 Mhz / EMBX_IR_TX_PHY_PRESCALER.  Currently, 
* this results in 8 usec per tick.  This function initializes, enables, and stops the counter.
* @param gclk which clock to use....See embx_ir_common.h
* @returns void
*/
static void embx_ir_tx_modulator_phy_init(enum gclk_generator gclk,  embx_ir_tx_mod_freq_t modulator_frequency)
{
	struct tc_config config_tc;
	
	tc_get_config_defaults(&config_tc);
	
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_source = gclk; /* 8 MHz */
	config_tc.clock_prescaler = EMBX_IR_TX_PHY_PRESCALER;  /* 8 us per tick  */
	
	tc_init(&tc_instance_ir_tx_phy, TC_IR_TX_PHY_MODULE, &config_tc);
	tc_enable(&tc_instance_ir_tx_phy);	
	tc_stop_counter(&tc_instance_ir_tx_phy);

	embx_time_configure_tc_callbacks();	
	embx_ir_tx_modulator_init(EMBX_IR_MODULATOR_GCLK, modulator_frequency, false);
#ifdef DEBUG_IR_TX_PHY
	digital_output_init(DEBUG_IR_TX_PHY_PIN, false);
#endif	
}

/**
* @brief Returns the state of the PHY.
* @details Set to true by the SEND function.  Cleared when the transmission has completed.
* @returns true if the PHY is busy.  Try again later.
*          false if the PHY is free.   
*/
bool embx_ir_tx_phy_get_state(void)
{
	bool state;
	system_interrupt_enter_critical_section();
	state = embx_ir_tx_in_progress;
	system_interrupt_leave_critical_section();	
	return state;
}

/**
* @brief Call to start an IR transmission.
* @details Transmits the first descriptor in the transmission q.  After the first descriptor has completed,
* an interrupt is generated and the remaining descriptors are transmitted from the interrupts.
* @param void 
* @returns enum status_code STATUS_BUSY if there is already a transmission in progress.
*                           STATUS_OK if not busy.
*/
enum status_code embx_ir_tx_phy_send(void)
{
	enum status_code status = STATUS_OK;
	embx_ir_tx_phy_descriptor_t *current_phy_descriptor;

	if( embx_ir_tx_in_progress == true) return STATUS_BUSY;

	tc_stop_counter(&tc_instance_ir_tx_phy);

	if( embx_ir_tx_phy_descriptor_get(&current_phy_descriptor) == STATUS_OK ) {

		embx_ir_tx_in_progress = true;

		tc_set_top_value(&tc_instance_ir_tx_phy, current_phy_descriptor->period);

		if( current_phy_descriptor->phy_interval_type == mark ) {
#ifdef DEBUG_IR_TX_PHY				
			port_pin_set_output_level(DEBUG_IR_TX_PHY_PIN, true);
#endif			
			embx_ir_tx_modulator_start();
		} else if ( current_phy_descriptor->phy_interval_type == space ) {
#ifdef DEBUG_IR_TX_PHY
			port_pin_set_output_level(DEBUG_IR_TX_PHY_PIN, false);
#endif			
			embx_ir_tx_modulator_stop();
		}
		
		tc_start_counter(&tc_instance_ir_tx_phy);
		
	} else {
#ifdef DEBUG_IR_TX_PHY
port_pin_set_output_level(DEBUG_IR_TX_PHY_PIN, false);
#endif		
		embx_ir_tx_in_progress = false;		
		embx_ir_tx_modulator_stop();
	}		
	return status;	
}

/** @brief Test bench */
void embx_time_tb(void)
{			
	embx_ir_tx_modulator_phy_init(EMBX_IR_MODULATOR_GCLK, KHz_38);
	
	while(1) {
		if(embx_ir_tx_in_progress == false ) {
			embx_ir_tx_phy_descriptor_fill(true, mark, 384, 0, 0);
			embx_ir_tx_phy_descriptor_fill(false, space, 1192, 1, 2);
			embx_ir_tx_phy_send();
		}
		delay_ms(20);
	}
	
}


