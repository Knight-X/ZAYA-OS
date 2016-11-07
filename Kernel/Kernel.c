/*******************************************************************************
 *
 * @file Kernel.c
 *
 * @author Murat Cakmak
 *
 * @brief Kernel Core Implementation.
 *
 *		- Initializes Hardware
 *		- Initializes Kernel
 *		- Initializes User Space Area
 *		- Starts Kernel
 *		- Starts User Space Applications
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

/********************************* INCLUDES ***********************************/
#include "Kernel.h"
#include "Kernel_Internal.h"
#include "Scheduler.h"

#include "Debug.h"

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/
/*
 * Kernel Internal Settings 
 */
typedef struct
{
	/* Internal Flags*/
	struct
	{
		uint32_t superVisorMode : 1;
	} flags;

	/*
	 * Task Pool.
	 * 
	 *  Keeps all kernel and user tasks.
	 */
	Application taskPool[NUM_OF_USER_TASKS];
} KernelSettings;
/**************************** FUNCTION PROTOTYPES *****************************/

/******************************** VARIABLES ***********************************/

#define APP_TEST_MODE	1

#if APP_TEST_MODE

PRIVATE AppImageInfo* userApps[NUM_OF_USER_TASKS] =
{
	/* Tries to statically allocated images */
	(AppImageInfo*)0x10000,
	(AppImageInfo*)0x20000
};

#endif

/* Kernel Internal Settings */
PRIVATE KernelSettings kernelSettings = { { 0 } };

/* Active Application */
INTERNAL Application* activeApp;

/**************************** PRIVATE FUNCTIONS ******************************/
/*
 * Simple printout interface to dump stack content. 
 */
PRIVATE void printOut(uint8_t* message)
{
	DEBUG_PRINTF((char*)message);
}

/*
 * Kernel Level Exception Handler.
 */
PRIVATE void exceptionHandler(Exception exception, uint32_t val, StackDumpCallback stackDump)
{
	DEBUG_PRINTF("\nERR Exc:%d-%d", exception, val);
	
	if (stackDump != NULL)
	{
		/* 
		 * Print stack content to specified output to provide more information 
		 * to find root cause of exception. 
		 */
		stackDump(printOut);
	}
	
	if (kernelSettings.flags.superVisorMode == false)
	{
		/*
		 * Exception is occurred in a User Application
		 */
		DEBUG_PRINT_ERROR("\nApp Exc. Terminating %d", activeApp->id);
		
		/* Terminate faulty user application */
		Scheduler_TerminateApplication();
		
		/* Yield to next application */
		Kernel_Yield(true);
	}
	else
	{
		/*
		 * UPS it is kernel crash. No way to restore. 
		 */
		DEBUG_PRINT_ERROR("\nOS Exc. Resetting device");

		/* May need a delay to print all output in previous line before reset */
		
		/*
		 * RESET The Device. 
		 */
		Drv_CPUCore_ResetDevice();		
	}
}

/*
 * Initialize a new task
 * 
 * @param tcb to be initialized new task (TSB)
 */
PRIVATE ALWAYS_INLINE void InitializeNewTask(Application* app)
{
	AppImageInfo* info = app->info;

	/* User application always work in unprivileged mode */
	app->tcb.flags.privileged = false;
	
	/* Initialize TCB of User Application */
	app->tcb.topOfStack = Kernel_InitializeTCB(info->image.sp, info->image.pc);
}

/**
 * Provides next TCB for Low Level Context Switching Mechanism.
 * Kernel registers this function to Driver Layer and when a Context Switching
 * is occurred, this function is responsible to provide next TCB.
 */
PRIVATE TCB* SchedulerGetNextApp(void)
{
	return &Scheduler_GetNextApp()->tcb;
}

/**
 * Starts Kernel after initialization
 * INLINED to avoid function call overhead.
 *
 * @param none
 *
 * @return none
 */
PRIVATE ALWAYS_INLINE void StartScheduling(void)
{
	Application* firstApp = Scheduler_GetNextApp();
	
	/* 
	 * Kernel finalized all initial tasks. 
	 * Now exit from Super Visor Mode before switching to User App
	 */
	kernelSettings.flags.superVisorMode = false;

	/* Start context switching using first task and TCB provider Callback */
	Kernel_StartContextSwitching(&firstApp->tcb, SchedulerGetNextApp);
}

/**
 * Initializes all tasks (TCB) for scheduling. 
 *
 * @param none
 *
 * @return none
 */
PRIVATE ALWAYS_INLINE void InitializeAllTasks(void)
{
	Application* app = &kernelSettings.taskPool[0];
	int32_t taskIndex = 0;

	/* Initialize all tasks */
	for (taskIndex = 0; taskIndex < NUM_OF_USER_TASKS; taskIndex++, app++)
	{
#if APP_TEST_MODE
		app->info = userApps[taskIndex];
#else
		#error "Should be implemented"
#endif

		InitializeNewTask(app);
	}
}

/**
 * Initializes Kernel and OS Components. Initializes also User Space.
 * INLINED to avoid function call overhead.
 *
 * @param none
 *
 * @return none
 */
PRIVATE ALWAYS_INLINE void InitializeKernel(void)
{
	/* Initialize all tasks before starting scheduling */
	InitializeAllTasks();

	/* Initialize Scheduler */
	Scheduler_Init(kernelSettings.taskPool);
}

PRIVATE ALWAYS_INLINE void InitializeHW(void)
{
	/* Initialize CPU First */
	Kernel_InitializeCPU();
	
	/* Initialize Exception Management */
	Kernel_InitializeExceptions(exceptionHandler);
}

/***************************** PUBLIC FUNCTIONS *******************************/
PUBLIC void OS_Yield(void)
{
	/* Just trigger Low Level Yield */
	Kernel_Yield(false);
}

/*
 * Kernel Start point.
 * Kernel is the owner of main function to start itself after system power-up. 
 * Initialize whole system and starts Kernel.
 *
 * @param none
 *
 */
int main(void)
{
	/* Initially, Kernel starts in Supervisor Mode */
	kernelSettings.flags.superVisorMode = true;
	
    /* Initialize HW First */
    InitializeHW();
	
	/* Initialize Kernel */
	InitializeKernel();
	
	/* Start Task Scheduling */
	StartScheduling();

	return 0;
}
