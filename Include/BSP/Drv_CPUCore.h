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
 *
 * @return none
 */
void Drv_CPUCore_CSStart(reg32_t* initialTCB);

/*
 * Switches running task to provided new TCB
 *
 * @param newTCB to be switched TCB
 *
 * @return none
 *
 */
void Drv_CPUCore_CSYieldTo(reg32_t* newTCB);

/*
 * Initializes task stack
 *
 * @param stack to be initialized task stack
 * @param stackSize Stack Size
 * @param taskStartPoint Start Point (Function) of Task
 *
 * @return top of stack after initialization. 
 *		   [IMP] Caller should keep top of stack address for new context switches.
 */
reg32_t* Drv_CPUCore_CSInitializeTaskStack(uint8_t* stack, uint32_t stackSize,
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
