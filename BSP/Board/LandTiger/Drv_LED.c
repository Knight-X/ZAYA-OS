/*******************************************************************************
 *
 * @file Drv_LED.c
 *
 * @author Murat Cakmak (MC)
 *
 * @brief LED Implementation for Actual Board (LandTiger)
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

#include "BSPConfig.h"

#if BOARD_ENABLE_LED_INTERFACE

/********************************* INCLUDES ***********************************/
#include "Drv_GPIO.h"

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/
#define BOARD_LED_COUNT			8

/***************************** TYPE DEFINITIONS *******************************/

/**************************** FUNCTION PROTOTYPES *****************************/

/******************************** VARIABLES ***********************************/

/**************************** PRIVATE FUNCTIONS *******************************/

/***************************** PUBLIC FUNCTIONS *******************************/
void Board_LedInit(void)
{
	int pin;
    /*
     * Configure LCD Pins
     * P2.0 ... P2.7
     * 
     */    
	for (pin = 0; pin < BOARD_LED_COUNT; pin++)
	{
		Drv_GPIO_ConfigurePin(2, pin, 0, 0);
	}

	/* LCD cannot be work with LCD at same time.So disable LCD Control pins */   
    /* P0.19 ... P0.25 */
    for (pin = 19; pin <= 25; pin++)
    {
        Drv_GPIO_ConfigurePin(0, pin, 0, 0);
        Drv_GPIO_WritePin(0, pin, DRV_GPIO_PINSTATE_HIGH);
    }
}

void Board_LedOn(uint32_t ledNo)
{
    if (ledNo >= BOARD_LED_COUNT) return;
    
    Drv_GPIO_WritePin(2, ledNo, DRV_GPIO_PINSTATE_HIGH);
}

void Board_LedOff(uint32_t ledNo)
{
    if (ledNo >= BOARD_LED_COUNT) return;
    
    Drv_GPIO_WritePin(2, ledNo, DRV_GPIO_PINSTATE_LOW);
}

#endif /* BOARD_ENABLE_LED_INTERFACE */
