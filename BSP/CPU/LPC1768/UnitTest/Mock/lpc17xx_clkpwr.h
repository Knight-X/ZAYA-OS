/*******************************************************************************
 *
 * @file lpc17xx_clkpwr.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Mock Definitions for lpc17xx_clkpwr.h
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

#ifndef LPC17XX_CLKPWR_H_
#define LPC17XX_CLKPWR_H_

/********************************* INCLUDES ***********************************/

#include "LPC17xx.h"

/***************************** MACRO DEFINITIONS ******************************/

/** Power Control for Peripherals bit mask */
#define CLKPWR_PCONP_BITMASK				0xEFEFF7DE

/** Timer/Counter 0 power/clock control bit */
#define	 CLKPWR_PCONP_PCTIM0				((uint32_t)(1<<1))
/* Timer/Counter 1 power/clock control bit */
#define	 CLKPWR_PCONP_PCTIM1				((uint32_t)(1<<2))
/** Timer 2 power/clock control bit */
#define	 CLKPWR_PCONP_PCTIM2				((uint32_t)(1<<22))
/** Timer 3 power/clock control bit */
#define	 CLKPWR_PCONP_PCTIM3				((uint32_t)(1<<23))

#define CLKPWR_PCLKSEL_BITMASK(p)			_SBF(p,0x03)

#define CLKPWR_PCLKSEL_SET(p,n)				_SBF(p,n)

/** Peripheral clock divider bit position for TIMER0 */
#define	CLKPWR_PCLKSEL_TIMER0  				((uint32_t)(2))

#define	CLKPWR_PCLKSEL_CCLK_DIV_4  			((uint32_t)(0))

#define TIM_CTCR_MODE_MASK  				0x3

#endif /* #ifndef LPC17XX_CLKPWR_H_ */
