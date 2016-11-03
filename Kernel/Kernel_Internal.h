/*******************************************************************************
 *
 * @file Kernel_Internal.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Kernel Internal Interface.
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
#ifndef __KERNEL_INTERNAL_H
#define __KERNEL_INTERNAL_H

/********************************* INCLUDES ***********************************/
#include "Drv_Timer.h"
#include "Drv_CPUCore.h"

#include "UserStartupInfo.h"

#include "OSConfig.h"
#include "SysConfig.h"

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/*
 * Number of Kernel Tasks
 *
 *  - We just have an idle task.
 */
#define NUM_OF_KERNEL_TASKS				(1)

/*
 * Returns Numbers of User Tasks
 */
#define NUM_OF_USER_TASKS \
			(sizeof(startupApplications) / sizeof(void*))

/*
 * Number of all task including kernel and user tasks
 */
#define NUM_OF_ALL_TASKS \
			(NUM_OF_KERNEL_TASKS + NUM_OF_USER_TASKS)

/*
 * Size of idle stack.
 *
 *  128 byte should be enough
 */
#define IDLE_TASK_STACK_SIZE		    (128)

/*
 * Idle Task Priority
 *  Lower Priority (0)
 */
#define IDLE_TASK_PRIORITY              (0)

/*
 * Kernel Task Creater Definition.
 * There is no difference between kernel and user task so use USER_TASK
 * definition for also kernel task
 */
#define KERNEL_TASK						OS_USER_TASK

/*
 * Start Point Definition (Function Prototype) for Kernel Tasks
 */
#define KERNEL_TASK_START_POINT			OS_USER_TASK_START_POINT

/*
 * Kernel Timer Priority
 *  Kernel Timer is a critical timer so priority selected as High.
 */
#define KERNEL_TIMER_PRIORITY           DRV_TIMER_PRI_HIGH

/*
 * Following defines are just wrapper definitions and covers Driver Layer APIs.
 *  A generic OS architecture should not dependent to external modules
 *  (like Driver or BSP). Therefore we uses wrapper functions to abstract
 *  external modules.
 */

/* Wrapper function definition to initialize CPU */
#define Kernel_InitializeCPU            Drv_CPUCore_Init

/* Wrapper function definition to start context switching */
#define Kernel_StartContextSwitching    Drv_CPUCore_CSStart

/* Wrapper function definition to initialize task stack */
#define Kernel_InitializeTCB      		Drv_CPUCore_CSInitializeTCB

/* Wrapper function definition to yield running task to */
#define Kernel_Switch                 	Drv_CPUCore_CSYield

/* Wrapper function definition to create a Timer */
#define Kernel_CreatePreemptionTimer    Drv_Timer_Create

/* Wrapper function definition to start Timer */
#define Kernel_StartPreemptionTimer     Drv_Timer_Start

/* Wrapper function definitions to get time stamp */
#define Kernel_GetPreemptionTimeStamp   Drv_Timer_ReadElapsedTimeInUs

/********************************* VARIABLES *******************************/

/*
 * User Space Applications which are started at startup
 * (after system initialization)
 *
 * We use following user-defined container to access user applications
 */
extern void* startupApplications[];

/***************************** TYPE DEFINITIONS *******************************/
/*
 * Wrapper Timer Handle definition to abstract external definition in kernel.
 */
typedef TimerHandle KernelTimerHandle;

/*
 * Base type for User Task
 *
 *  Each static user task implements its type implicitly using OS_USER_TASK()
 *  macro and specifies task specific features. e.g stack size.
 *  We can handle task specific differencies using this base type.
 */
typedef struct
{
	/*
	 * User Task start point.
	 *
	 * User task starts with that point (function).
	 * Program Counter (PC) is set to this value when task is started.
	 */
	OSUserTaskStartPoint taskStartPoint;
    /*
     * User Task Priority
     */
    uint32_t priority;
	/*
	 * Stack size of User Task
	 */
	uint32_t stackSize;
	/*
	 * Start address of task stack.
	 *
	 * We use this variable for just start point of user task.
	 * Boundary check can be done by stackSize variable.
	 *
	 * NOTE : While each task has different stack size, we define size as
	 * '1' to get start point of user stack. It could be zero but zero-sized
	 * arrays are not portable and some platforms does not support it.
	 *
	 */
	uint8_t stack[1];
} AppBaseType;

/*
 * User Application
 */
typedef struct
{
	/* TCB of User Application */
	TCB tcb;
	/* User defined information of Application */
	AppBaseType* info;
} Application;
/*************************** FUNCTION DEFINITIONS *****************************/

#endif	/* __KERNEL_INTERNAL_H */
