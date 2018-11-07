/**
 * @file embx_ir_common.h
 * @date 10/5/2018
 * @author bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 * 
 * @brief Implements common types and definitions for the IR Tx Module.
 * @details This file contains common types and definitions used by the IR module. 
 */ 
#ifndef EMBX_IR_COMMON_H_
#define EMBX_IR_COMMON_H_

/** Define to see output on a GPIO pin */
#define DEBUG_IR_TX_PHY			(1)
#define DEBUG_IR_TX_PHY_PIN		PIN_D7

/** GCLK Frequency = 8MHz, Prescaler = 64, Clock ticks, Typical IR Modulation Frequencies listed together with counter periods */
typedef enum {
	KHz_30 = 132,
	KHz_33 = 120,
	KHz_36 = 110,
	KHz_38 = 104,
	KHz_40 = 99,
	KHz_56 = 70
} embx_ir_tx_mod_freq_t;

/** The GCLK used to clock the timer counter.  Verify that it is enabled in conf_clocks.h */
#define EMBX_IR_MODULATOR_GCLK			((enum gclk_generator)GCLK_GENERATOR_3)

/** The frequency of the GCLK */
#define EMBX_IR_MODULATOR_GCLK_FREQ		((uint32_t)8000000)

/** Define the endian-ness of the protocol. */
#define EMBX_IR_BIG_ENDIAN			(0)
#define EMBX_IR_LITTLE_ENDIAN		(1)
#define EMBX_IR_ENDIANESS		(EMBX_IR_LITTLE_ENDIAN)

#endif /* EMBX_IR_COMMON_H_ */