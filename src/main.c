/**
 * @file main.c
 * @brief 
 * @date
 * @author bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 */
#include <asf.h>
#include "embx/embx_gclk/embx_gclk.h"
#include "embx/embx_ir/embx_ir_common.h"
#include "embx/embx_ir/embx_ir_tx_phy_descriptor.h"
#include "embx/embx_ir/embx_ir_tx_phy.h"
#include "embx/embx_ir/embx_ir_rx_phy.h"

int main (void)
{
	system_init();
	delay_init();
	
	embx_ir_rx_phy_tb();
	embx_time_tb();

	while(1) ;
	
}
