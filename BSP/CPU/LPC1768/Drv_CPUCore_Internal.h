/*******************************************************************************
*
* @file Drv_CPUCore_Internal.h
*
* @author Murat Cakmak (MC)
*
* @brief CPU specific Internal Definitions
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

#ifndef __DRV_CPUCORE_INTERNAL_H
#define __DRV_CPUCORE_INTERNAL_H

/********************************* INCLUDES ***********************************/
#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/

/**************************** FUNCTION PROTOTYPES *****************************/

/*
 * Callback to get next TCB from Upper Layer (e.g. Kernel)
 */
extern Drv_CPUCore_CSGetNextTCBCallback GetNextTCBCallBack;

/*
 * Generic Hard Fault Handler while HW Hard Fault handler is compiler 
 * (armcc, gcc) dependent. HW handler calls this handler to process hard
 * fault. 
 */
extern void HardFault_Handler(uint32_t stack[]);

/******************************** VARIABLES ***********************************/

/*************************** FUNCTION PROTOTYPES *****************************/
/*
 * Starts Context Switching on CPU.
 *
 * [IMP] To be switched task (currentTCB) must be set before calling this
 * function.
 *
 * @param none
 *
 * @return none
 */
INTERNAL void StartContextSwitching(void);

/*
 * Jump to specified image.
 *
 * @param imageAddress Image address to jump
 *
 * @return none
 */
INTERNAL void JumpToImage(reg32_t imageAddress);

#endif /* __DRV_CPUCORE_INTERNAL_H */
