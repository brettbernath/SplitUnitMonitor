/**
 * @file embx_ir_tx.h
 * @date 10/3/2018
 * @author: bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 */ 
#ifndef EMBX_IR_TX_H_
#define EMBX_IR_TX_H_

#define EMBX_IR_TX_MODULATOR_MAX_PERIOD (255)
#define EMBX_IR_TX_MODULATOR_MIN_PERIOD (25)

#define TC4_BOARD_A1				PIN_PB08E_TC4_WO0	/* Alternatively PA22 */
#define TC4_BOARD_A2				PIN_PB09E_TC4_WO1   /* PB09 is BOARD_A2 - Alternatively PA23, BOARD_A2 */
#define TC4_BOARD_A1_MUX			MUX_PB08E_TC4_WO0
#define TC4_BOARD_A2_MUX			MUX_PB09E_TC4_WO1
#define TC4_CHANNEL_0				(0)
#define TC4_CHANNEL_1				(1)

#define TC_IR_MODULATOR_MODULE			TC4
#define TC_IR_MODULATOR_PIN				TC4_BOARD_A2
#define TC_IR_MODULATOR_MUX				TC4_BOARD_A2_MUX
#define TC_IR_MODULATOR_CHANNEL			TC4_CHANNEL_1


/** @brief Call to initialize and enable the modulator. */
extern void embx_ir_tx_modulator_init(enum gclk_generator gclk, embx_ir_tx_mod_freq_t ir_freq, bool start_counting);
extern void embx_ir_tx_modulator_reset(void);
extern void embx_ir_tx_modulator_enable(void);
extern void embx_ir_tx_modulator_disable(void);
extern void embx_ir_tx_modulator_start(void);
extern void embx_ir_tx_modulator_stop(void);
extern void embx_ir_tx_modulator_set_freq(embx_ir_tx_mod_freq_t ir_freq );
#if 0
extern enum status_code embx_ir_tx_modulator_compute_and_set_freq(uint32_t ir_freq );
#endif
#endif /* EMBX_IR_TX_H_ */