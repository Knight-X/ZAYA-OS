/*******************************************************************************
 *
 * @file Drv_CPUCore.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief CPU Core Driver Interface
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
#ifndef __DRV_CPUCORE_H
#define __DRV_CPUCORE_H

/********************************* INCLUDES ***********************************/
#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/
typedef void(*Drv_CPUCore_TaskStartPoint)(void* arg);

/*
 * Task Control Block (TCB)
 *
 *  Includes all task specific information including required information for
 *	Context Switching.
 */
typedef struct
{
	/*
	 * Actual top address of stack of task.
	 *
	 *  When a user task is started, task starts to use stack from end of
	 *  stack in descending order.
	 *  When a task preempted, we need to save actual position of stack to
	 *  backup next execution of task.
	 *
	 *	[IMP] This value must be first item in that data structure. When we pass
	 *	a TCB to context switcher mechanism, HW looks for first address.
	 */
	reg32_t* topOfStack;
} TCB;

/*
 * Callback to get next TCB from upper layer (e.g. Kernel).
 *
 * @param none
 *
 * @return Next TCB to be switched.
 */
typedef TCB* (*Drv_CPUCore_CSGetNextTCBCallback)(void);

/*************************** FUNCTION DEFINITIONS *****************************/
/**
* Initializes actual CPU and its components/peripherals.
*
* @param none
* @return none
*/
void Drv_CPUCore_Init(void);

/*
 * Halts all system.
 *
 * @param none
 * @return none
 *
 */
void Drv_CPUCore_Halt(void);

/*
 * Enables all interrupts.
 *
 * @param none
 * @return none
 *
 */
void Drv_CPUCore_EnableInterrupts(void);

/*
 * Disables all interrupts.
 *
 * @param none
 * @return none
 *
 */
void Drv_CPUCore_DisableInterrupts(void);

/*
 * Starts Context Switching
 *  Configures HW for CS and starts first task
 *
 * @param initialTCB Initial (First) TCB (Task) for Context Switching
 * @param getNextTCBCB Client code must provide callback to provide TCB for
 *        context switching. When a context switching is occurred, this function
 *		  is called.
 *
 *
 * @return none
 */
void Drv_CPUCore_CSStart(TCB* initialTCB, Drv_CPUCore_CSGetNextTCBCallback getNextTCBCB);

/*
 * Switches running task to provided new TCB
 *
 * @param none
 *
 * @return none
 *
 */
void Drv_CPUCore_CSYield(void);

/*
 * Initializes task stack
 *
 * @param tcb to be initialized Task Control Block (TCB)
 * @param stack to be initialized task stack
 * @param stackSize Stack Size
 * @param taskStartPoint Start Point (Function) of Task
 *
 * @return none
 */
reg32_t* Drv_CPUCore_CSInitializeTCB(uint8_t* stack, uint32_t stackSize,
									 Drv_CPUCore_TaskStartPoint startPoint);

/*
 * Jumps to other image on system.
 * It is used to pass control from Bootloader to Application (e.g. Firmware)
 *
 * @param image image address of Application in memory.
 *
 * @return none
 */
void Drv_CPUCore_JumpToImage(reg32_t imageAddress);

/*
 * Returns actual frequency of CPU
 *
 * @param none
 * @return CPU Core Clock Frequency
 */
uint32_t Drv_CPUCore_GetCPUFrequency(void);

#endif	/* __DRV_CPUCORE_H */
