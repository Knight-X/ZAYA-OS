/*******************************************************************************
 *
 * @file Kernel_Internal.h
 *
 * @author Murat Cakmak
 *
 * @brief P-OS Kernel Internal Interface.
 *
 * @see https://github.com/P-LATFORM/P-OS/wiki
 *
 ******************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 P-OS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************************/
#ifndef __KERNEL_INTERNAL_H
#define __KERNEL_INTERNAL_H

/********************************* INCLUDES ***********************************/
#include "Drv_Timer.h"
#include "Drv_CPUCore.h"

#include "OSConfig.h"
#include "SysConfig.h"

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/*
 * Number of Kernel Tasks
 *
 *  - Does not exist for now
 */
#define NUM_OF_KERNEL_TASKS				(0)

/*
 * Returns Numbers of User Tasks
 */
#define NUM_OF_USER_TASKS				OS_MAX_USER_APP

/*
 * Number of all task including kernel and user tasks
 */
#define NUM_OF_ALL_TASKS \
			(NUM_OF_KERNEL_TASKS + NUM_OF_USER_TASKS)

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

/* TODO This value should be common for all images (BL, FW, User Apps)*/
#define APP_IMAGE_SIGNATURE_LENGTH       (256)

#define APP_IMAGE_METADATA_LENGTH        (256 + FIRMWARE_SIGNATURE_LENGTH)

#define APP_IMAGE_META_DATA_PADDING_SIZE \
			((APP_IMAGE_SIGNATURE_LENGTH - sizeof(AppImageMetaDataHeader)) / sizeof(uint32_t))

/*
 * Following defines are just wrapper definitions and covers Driver Layer APIs.
 *  A generic OS architecture should not dependent to external modules
 *  (like Driver or BSP). Therefore we uses wrapper functions to abstract
 *  external modules.
 */

/* Wrapper function definition to initialize CPU */
#define Kernel_InitializeCPU            Drv_CPUCore_Init

/* Wrapper function definition to initialize Exception Management */
#define Kernel_InitializeExceptions		Drv_CPUCore_InitializeExceptions
			
#define Kernel_ActivateMemoryProtection Drv_CPUCore_InitializeMPU

/* Wrapper function definition to start context switching */
#define Kernel_StartContextSwitching    Drv_CPUCore_CSStart

/* Wrapper function definition for task yield */
#define Kernel_Yield					Drv_CPUCore_CSYield

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

/***************************** TYPE DEFINITIONS *******************************/
/*
 * Wrapper Timer Handle definition to abstract external definition in kernel.
 */
typedef TimerHandle KernelTimerHandle;

/*
 * Application States
 */
typedef enum
{
	AppState_New,
	AppState_Ready,
	AppState_Running,
	AppState_Terminated
} ApplicationState;

/*
 * Image Meta Data Header
 */
typedef struct
{
	reg32_t codeAddress;
	uint32_t codeSize;
	reg32_t ramAddress;
	uint32_t ramSize;
} AppImageMetaDataHeader;

/*
 * Application Image and its attributes
 *  SP-OS user applications must be specialized for SP-OS and needs to have a
 *  meta data which includes image attributes, image signature.
 *  After that, SP-OS User Application image must also start from main()
 *  function instead of Reset_Vector so second word of user app image must keep
 *  address of main function of User App.
 *
 *  Kernel can access to User Application attributes by casting start address of
 *  image to this structure.
 */
typedef struct
{
	/* Meta Data Header of User Application  */
    AppImageMetaDataHeader metaDataHeader;
	/* Padding to keep signature and image align */
    uint32_t padding[APP_IMAGE_META_DATA_PADDING_SIZE];
	/*
	 * User Image Signature
	 *  Used to validate user application.
	 */
    uint8_t imageSignature[APP_IMAGE_SIGNATURE_LENGTH];
	/*
	 * User Application Image
	 *  - First Word of image keeps Stack Pointer (Stack Start Address)
	 *  - Second Word of image keeps PC (Program Counter - Start point for
	 *    code execution)
	 *     SP-OS User Images does not have and Reset Vector, they starts from
	 *     main() function directly.
	 */
    struct
	{
		reg32_t sp;		/* Stack Pointer (SP) */
		reg32_t pc;		/* Program Counter (PC) */
	} image;
} AppImageInfo;

/*
 * User Application
 */
typedef struct
{
	/* TCB of User Application */
	TCB tcb;

	/* Application ID */
	int32_t id;
	
	/*
	 * User Application Image Info.
	 * Includes metadata, signature and image.
	 */
	AppImageInfo* info;
	
	/* Actual State of Application */
	ApplicationState state;

} Application;
/*************************** FUNCTION DEFINITIONS *****************************/

/********************************* VARIABLES *******************************/
extern INTERNAL Application* activeApp;

#endif	/* __KERNEL_INTERNAL_H */
