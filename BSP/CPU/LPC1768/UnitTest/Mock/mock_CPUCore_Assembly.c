/*******************************************************************************
 *
 * @file mockCPUCore_Assembly.c
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Mock Implementation for Assembly functions of CPU
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

/********************************* INCLUDES ***********************************/

#include "Drv_CPUCore_Internal.h"
#include "LPC17xx.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/

/**************************** FUNCTION PROTOTYPES *****************************/
extern void SwitchContext(void);

/******************************** VARIABLES ***********************************/

/********************************** FUNCTIONS *********************************/
/*
 *  Mock Implementation of PendSV ISR
 */
void POS_PendSV_Handler(void)
{
	/* PendSV ISR normally calls SwitchContext() function to get next TCB */
	SwitchContext();
}

/*
 * Mock Implementation of SVC ISR
 */
void POS_SVC_Handler(void)
{
	/* Just set flag to inform about SVC Call */
	lpcMockObjects.flags.svc_handler_call = 1;
}

/*
 * Mock Implementation of StartContextSwitching
 */
void StartContextSwitching(void)
{
	/* Original function enables global interrupts before SVC ISR */
	__enable_irq();

	/* Jump to SVC. SVC ISR starts context switching */
	POS_SVC_Handler();
}
