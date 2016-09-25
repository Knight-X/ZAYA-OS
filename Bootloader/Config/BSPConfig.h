/*******************************************************************************
 *
 * @file BSPConfig.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Project Specific BSP Configurations
 *
 * @see https://github.com/ZA-YA/ZAYA-OS/wiki
 *
 ******************************************************************************
 *
 * GNU GPLv2
 *
 * Copyright (c) 2016 ZAYA
 *
 *  See GNU GPLv2 License Details in the Root Directory.
 *
 ******************************************************************************/
#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

/********************************* INCLUDES ***********************************/
#include "postypes.h"
/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/

/* BOARD LED INTERFACE */
#define BOARD_ENABLE_LED_INTERFACE 1

/* BOARD LCD INTERFACE */
#define BOARD_ENABLE_LCD_INTERFACE 0

#if (BOARD_ENABLE_LED_INTERFACE && BOARD_ENABLE_LCD_INTERFACE)
    #error "LED and LCD interfaces can not be used at same time!"
#endif

#define CPU_TIMER_MAX_TIMER_COUNT       (30)


#endif	/* __BOARD_CONFIG_H */
