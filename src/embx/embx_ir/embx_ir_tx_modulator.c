/**
 * @file embx_ir_tx.c
 * @created 10/3/2018
 * @author bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 *
 * @brief This module pulse width modulates the pin defined by TC_PWM_OUT_PIN at a frequency suitable for IR.
 * @details 
 */ 
#include <asf.h>
#include "embx_ir_common.h"
#include "embx/embx_ir/embx_ir_tx_modulator.h"

static struct tc_module tc_instance_ir_mod;
static struct tc_config config_tc_ir_mod;


static void embx_ir_tx_modulator_set_config( enum gclk_generator gclk, embx_ir_tx_mod_freq_t ir_freq )
{
	tc_get_config_defaults(&config_tc_ir_mod);

	config_tc_ir_mod.clock_source = gclk;
	config_tc_ir_mod.counter_size = TC_COUNTER_SIZE_8BIT;
	
	/* For 8-bit counter, NORMAL_FREQ toggles GPIO output pin every time the counter == period value */
	/* Modulation Frequency = 8000000 / x */
	config_tc_ir_mod.wave_generation = TC_WAVE_GENERATION_NORMAL_FREQ;
	config_tc_ir_mod.counter_8_bit.period = ir_freq;
	config_tc_ir_mod.pwm_channel[TC_IR_MODULATOR_CHANNEL].enabled = true;
	config_tc_ir_mod.pwm_channel[TC_IR_MODULATOR_CHANNEL].pin_out = TC_IR_MODULATOR_PIN;
	config_tc_ir_mod.pwm_channel[TC_IR_MODULATOR_CHANNEL].pin_mux = TC_IR_MODULATOR_MUX;	
}

/**
* @brief Initializes and enables the IR modulator.
* @param gclk - specifies the clock from conf_clocks.h that the modulator will use 
* @param ir_freq - enumerated value used to set the timer's period.
* @param start_counting - true if this function should start the counter counting.
*/
void embx_ir_tx_modulator_init(enum gclk_generator gclk, embx_ir_tx_mod_freq_t ir_freq, bool start_counting )
{	
	/* Set the configuration */
	embx_ir_tx_modulator_set_config( gclk, ir_freq );

	/* Initialize the timer hardware with the configuration */
	tc_init(&tc_instance_ir_mod, TC_IR_MODULATOR_MODULE, &config_tc_ir_mod);

	/* This enables the counter and starts the timer counting */
	tc_enable(&tc_instance_ir_mod);
	
	if( start_counting == false ) {
		tc_stop_counter(&tc_instance_ir_mod);		
	}		
}

/**
* @brief Resets the TC hardware to defaults, must be initialized again after reset.
*/
void embx_ir_tx_modulator_reset(void)
{
	tc_reset(&tc_instance_ir_mod);	
}

/**
* @brief To enable, the module must first be initialized with an appropriate configuration.
* @details - After being enabled, the counter starts counting ....
*/
void embx_ir_tx_modulator_enable(void)
{
	tc_enable(&tc_instance_ir_mod);
}

/**
* @brief Disables the modulator.  May be re-enabled without being reinitialized .
*/
void embx_ir_tx_modulator_disable(void)
{
	tc_disable(&tc_instance_ir_mod);	
}

/**  @brief Stops the timer and resets it to 0.*/
void embx_ir_tx_modulator_stop(void)
{
	tc_stop_counter(&tc_instance_ir_mod);
}

/**
* @brief Starts the timer, it must be enabled first.
*/
void embx_ir_tx_modulator_start(void)
{
	tc_start_counter(&tc_instance_ir_mod);
}

/**
* @brief Modify the modulation frequency.  
* @param[in] ir_freq the new frequency selected from the enumerated common
* IR modulation frequencies.
* The Frequency can be modified on the fly.
*/
void embx_ir_tx_modulator_set_freq(embx_ir_tx_mod_freq_t ir_freq )
{
	tc_set_top_value(&tc_instance_ir_mod, ir_freq);
}

#if 0
/**
* @brief GPIO pin toggles every time the 8-bit counter overflows.
* @details - The GPIO pin period is 1/2 of the 8=bit counter overflow. 
* The frequency can be modified on the fly while the timer is running.
*/
enum status_code embx_ir_tx_modulator_compute_and_set_freq(uint32_t ir_freq )
{
	uint32_t period =  (EMBX_IR_MODULATOR_GCLK_FREQ >> 1) / ir_freq;
	
	if( period > EMBX_IR_TX_MODULATOR_MAX_PERIOD ) {
		period = EMBX_IR_TX_MODULATOR_MAX_PERIOD;
	}	else if( period < EMBX_IR_TX_MODULATOR_MIN_PERIOD ) {
		period = EMBX_IR_TX_MODULATOR_MIN_PERIOD;
	}
	
	config_tc_ir_mod.counter_8_bit.period = period;		
	
	tc_set_top_value(&tc_instance_ir_mod, period);
	
	return STATUS_OK;	
}
#endif

