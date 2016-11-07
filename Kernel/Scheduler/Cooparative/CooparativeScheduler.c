/*******************************************************************************
 *
 * @file CooparativeScheduler.c
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Basic Cooparative Scheduler Implementation.
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

#include "OSConfig.h"

#if (OS_SCHEDULER == OS_SCHEDULER_COOPARATIVE)

/********************************* INCLUDES ***********************************/
#include "Kernel.h"
#include "Scheduler.h"

#include "Kernel_Internal.h"

#include "Debug.h"
#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/
/* Task count */
#define TASK_COUNT              NUM_OF_USER_TASKS

/***************************** TYPE DEFINITIONS *******************************/
/*
 * Cooparative Scheduler Internal Data Structure
 */
typedef struct
{
    /* Task pool for all user tasks */
    Application* taskPool;
    /* Task index to track current task in cooparative scheduling */
    int32_t taskIndex;
} CooparativeScheduler;
/**************************** FUNCTION PROTOTYPES *****************************/

/******************************** VARIABLES ***********************************/
/*
 * Cooparative scheduler internal data
 */
PRIVATE CooparativeScheduler scheduler;

/**************************** PRIVATE FUNCTIONS *******************************/

/***************************** PUBLIC FUNCTIONS *******************************/
/*
 * Initializes Scheduler
 */
PUBLIC void Scheduler_Init(Application* appList)
{
	Application* app = &appList[0];
	int32_t i;
	
    scheduler.taskPool = appList;
    scheduler.taskIndex = -1;
	
	for (i = 0; i < TASK_COUNT; i++)
	{
		app->state = AppState_Ready;
		app->id = i;
		
		app++;
	}
}

/*
 * Yields task in Scheduler side.
 *
 *  Primitive implementation for Cooparative scheduling.
 */
PUBLIC Application* Scheduler_GetNextApp(void)
{
    Application* nextApp = NULL;
	Application* app;
	uint32_t tryCount = TASK_COUNT;
	
	while (tryCount-- > 0)
	{
		/* Calculate task indexx for next yield */
		scheduler.taskIndex = (scheduler.taskIndex + 1) % TASK_COUNT;
		
		/* Get next (ready) task from task pool */
		app = &scheduler.taskPool[scheduler.taskIndex];
		
		if (app->state == AppState_Ready)
		{
			nextApp = app;
			activeApp = nextApp;
			break;
		}
	}

    return nextApp;
}

/*
 * Terminates current active application
 */
PUBLIC void Scheduler_TerminateApplication(void)
{
	activeApp->state = AppState_Terminated;
}

#endif /* #if (OS_SCHEDULER == OS_SCHEDULER_COOPARATIVE) */
