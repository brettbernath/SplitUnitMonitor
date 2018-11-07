/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include "config/conf_clocks.h"
#include "embx/embx_gclk/embx_gclk.h"

#if defined(__GNUC__)
void board_init(void) WEAK __attribute__((alias("system_board_init")));
#elif defined(__ICCARM__)
void board_init(void);
#  pragma weak board_init=system_board_init
#endif

void system_board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	
	if( CONF_CLOCK_GCLK_0_OUTPUT_ENABLE == true ) {
		embx_gclk_view_on_gpio(GCLK_GENERATOR_0);
	}
	if( CONF_CLOCK_GCLK_1_OUTPUT_ENABLE == true ) {
		embx_gclk_view_on_gpio(GCLK_GENERATOR_1);			
	}
	if( CONF_CLOCK_GCLK_3_OUTPUT_ENABLE == true ) {
		embx_gclk_view_on_gpio(GCLK_GENERATOR_3);			
	}	
	

}