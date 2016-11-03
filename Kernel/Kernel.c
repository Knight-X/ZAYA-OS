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
#include "Kernel.h"
#include "Kernel_Internal.h"
#include "Scheduler.h"

#include "Board.h"

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/

/**************************** FUNCTION PROTOTYPES *****************************/
/* 
 * IDLE Task Definitions
 */
PRIVATE OS_USER_TASK_START_POINT(IdleTaskFunc);

/******************************** VARIABLES ***********************************/
/*
 * Idle Task
 * 
 *  We use idle task for side things which maintains system
 */
KERNEL_TASK(IdleTask, IdleTaskFunc, IDLE_TASK_STACK_SIZE, IDLE_TASK_PRIORITY);

/*
 * Task Pool.
 * 
 *  Keeps all kernel and user tasks.
 */
PRIVATE Application kernelTaskPool[NUM_OF_USER_TASKS];

/*
 * Task Pool. 
 * 
 *  Keeps all kernel and user tasks. 
 */
PRIVATE Application idleApp;

/**************************** PRIVATE FUNCTIONS ******************************/

/*
 * Idle System Task Code Block 
 *
 */
PRIVATE KERNEL_TASK_START_POINT(IdleTaskFunc)
{
    while (1)
    {
        /* For now just yield */
        OS_Yield();
    }
}

/*
 * Initialize a new task
 * 
 * @param tcb to be initialized new task (TSB)
 */
PRIVATE ALWAYS_INLINE void InitializeNewTask(Application* app)
{
	AppBaseType* userApp = app->info;
    
	/* Initialize stack of user task according to CPU architecture */
	app->tcb.topOfStack = Kernel_InitializeTCB(userApp->stack,
											   userApp->stackSize,
											   userApp->taskStartPoint);
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
	/* Start context switching using
	 *  - Idle task
	 *  - TCB provider Callback
	 */
	Kernel_StartContextSwitching(&(idleApp.tcb), SchedulerGetNextApp);
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
	Application* app = &kernelTaskPool[0];
	int32_t taskIndex = 0;

	/*
	 * Each User task creates its type and we collect user tasks in a single
	 * container (startupApplications) to manage startup applications.
	 * To do that we defined this container type as "void* array".
	 * While we can not use pointer operations on "void* arrays", we cast this
	 * array to a non - typeless array (int* as a word of a CPU which  most
	 * suitable type for pointer array arithmetic)
	 *
	 */
    int* appPtr = (int*)startupApplications;

	/* Initialize user tasks */
    for (taskIndex = 0; taskIndex < NUM_OF_USER_TASKS; taskIndex++, app++, appPtr++)
	{
		/* Save User Task Info into TCB */
		app->info = (AppBaseType*)*appPtr;

		/* Initialize New Task */
        InitializeNewTask(app);
	}

	/* Initialize idle task */
	idleApp.info = (AppBaseType*)OS_USER_TASK_PREFIX(IdleTask);
    InitializeNewTask(&idleApp);
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
	Scheduler_Init(kernelTaskPool, &idleApp);

	/* Initialize User Space */
	OS_InitializeUserSpace();
}

PRIVATE ALWAYS_INLINE void InitializeHW(void)
{
	/* Initialize CPU First */
	Kernel_InitializeCPU();
}

/***************************** PUBLIC FUNCTIONS *******************************/
PUBLIC void OS_Yield(void)
{
	/* Just trigger Low Level Yield */
	Drv_CPUCore_CSYield();
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
    /* Initialize HW First */
    InitializeHW();
 
	/* Initialize Kernel */
	InitializeKernel();

	/* Start Task Scheduling */
	StartScheduling();

	return 0;
}
