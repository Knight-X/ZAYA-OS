/*******************************************************************************
 *
 * @file Board.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Board Interface for Hardware Abstraction Layer
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

#ifndef __BOARD_H
#define __BOARD_H

/********************************* INCLUDES ***********************************/
#include "BSPConfig.h"

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/

/*************************** FUNCTION DEFINITIONS *****************************/
void Board_Init(void);

/*
 * BOARD LED INTERFACE
 */
#if BOARD_ENABLE_LED_INTERFACE

void Board_LedInit(void);
void Board_LedOn(uint32_t ledNo);
void Board_LedOff(uint32_t ledNo);

#endif /* BOARD_ENABLE_LED_INTERFACE */

#endif	/* __BOARD_H */
