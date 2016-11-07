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

/*
 * Exception Types
 * 
 */
typedef enum
{
	Exception_NoException = 0,
	
	/*
	 * Following exceptions specify generic exceptions. 
	 * It means exact type of exception is not extracted. 
	 */
	Exception_HardFault,
	Exception_MemoryFault, 
	Exception_BusFault,
	Exception_UsageFault,
	
	/*
	 * Exact types of Exceptions
	 */
	Exception_DivideByZero = 10,
} Exception;

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
	
	/*
	 * Task Specific Flags
	 */
	struct
	{
		/* Indicates whether if task runs in privileged or unpriviliged state */
		uint32_t privileged : 1;
	} flags;
} TCB;

/*
 * Callback to get next TCB from upper layer (e.g. Kernel).
 *
 * @param none
 *
 * @return Next TCB to be switched.
 */
typedef TCB* (*Drv_CPUCore_CSGetNextTCBCallback)(void);

/*
 * Prinout Callback
 *  When upper layer decided to print stack content, it also provide a printer
 *  function to dump stack content to debug output. In this way, upper layer 
 *  can forward debug messages to suitable output environment. 
 * 
 * @param message Message to print out
 */
typedef void (*PrintOutCallback)(uint8_t* message);

/*
 * Stack dumper callback. 
 * When an exception is occurred, Exception Management management also provides
 * callback to dump stack content to a debug output. 
 *
 * @param printOut Provided (by upper layer) method to use dumping stack. 
 */
typedef void (*StackDumpCallback)(PrintOutCallback printOut);

/*
 * Exception Callback.
 *  Upper layer uses this callback type to register itself to be informed about
 *  exceptions. 
 *  Exception management interface, provides exception type and helper value. 
 *  This function also provides a stack dumper function. 
 *  Upper layer can call this function to dump stack. 
 *
 * @param exception Type of Exception
 * @param val When exception is an exact type (like DivByZero), value is not 
 *        meaningfull and probably just zero. But for general exception type, 
 * 		  upper layer can use this value to debug exception. It keeps register 
 *		  value. 
 * @param stackDump Function to dump stack content 
 */
typedef void (*ExceptionCallback)(Exception exception, uint32_t val, StackDumpCallback stackDump);

/*************************** FUNCTION DEFINITIONS *****************************/
/**
* Initializes actual CPU and its components/peripherals.
*
* @param none
* @return none
*/
void Drv_CPUCore_Init(void);

/*
 * Resets Devices
 *
 * @param none
 * @param none
 *
 */
void Drv_CPUCore_ResetDevice(void);

/*
 * Initializes Exception Management
 *
 * @param excCallback Client Callback to handle exception
 * @param none
 *
 */
void Drv_CPUCore_InitializeExceptions(ExceptionCallback excCallback);

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
 * @param privileged Actual State (priv or unpriv) of system.
 *
 * @return none
 *
 */
void Drv_CPUCore_CSYield(bool privileged);

/*
 * Initializes task stack
 *
 * @param tcb to be initialized Task Control Block (TCB)
 * @param stack to be initialized task stack
 * @param taskStartPoint Start Point (Function) of Task
 *
 * @return none
 */
reg32_t* Drv_CPUCore_CSInitializeTCB(reg32_t topOfStack, reg32_t startPoint);

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
