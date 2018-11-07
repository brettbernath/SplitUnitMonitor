/**
 * @file - embx_gclk.h
 * @date - Created: 10/3/2018 5:17:52 PM
 * @author - bbernath
 * @copyright Copyright (C) 2018 Brett Bernath. All Rights Reserved.
 *
 * @brief The embx_gclk module allows viewing the various clocks on a GPIO pin. 
 * @details - clock configuration is performed in "config/conf_clocks.h"
 */ 

#ifndef EMBX_GCLK_H_
#define EMBX_GCLK_H_

extern void embx_gclk_view_on_gpio(enum gclk_generator gclk);


#endif /* EMBX_GCLK_H_ */