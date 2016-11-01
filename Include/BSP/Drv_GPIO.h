/*******************************************************************************
 *
 * @file Drv_GPIO.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief GPIO Driver Interface
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
#ifndef __DRV_GPIO_H
#define __DRV_GPIO_H

/********************************* INCLUDES ***********************************/
#include "postypes.h"
/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/
/* GPIO Pin States */
typedef enum
{
	DRV_GPIO_PINSTATE_LOW = 0,
	DRV_GPIO_PINSTATE_HIGH = 1
} Drv_GPIO_PinState;

/*************************** FUNCTION DEFINITIONS *****************************/
void Drv_GPIO_Init(void);
void Drv_GPIO_ConfigurePin(uint32_t port, uint32_t pin, uint32_t functionNo, uint32_t driveMode);
void Drv_GPIO_WritePin(uint32_t port, uint32_t pin, Drv_GPIO_PinState state);
Drv_GPIO_PinState Drv_GPIO_ReadPin(uint32_t port, uint32_t pin);

#endif	/* __DRV_GPIO_H */
