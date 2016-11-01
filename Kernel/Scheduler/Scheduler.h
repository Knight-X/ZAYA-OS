/*******************************************************************************
 *
 * @file Scheduler.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Generic Scheduler Interface for Kernel
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
#ifndef __SCHEDULER_H
#define __SCHEDULER_H

/********************************* INCLUDES ***********************************/
#include "Kernel_Internal.h"

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/
/*
 * Callback Type for Context Switching.
 *  In case of preemption, schedulers notifies kernel using this callback
 *
 * @param nextTCB TCB of to be run next Task.
 */
typedef void (*SchedulerCSCallback)(TCB* nextTCB);

/*************************** FUNCTION DEFINITIONS *****************************/
/*
 * Initializes Scheduler.
 *
 * @param tcbList Task List (Pool) to be scheduled
 * @param idle TCB of Idle task. Scheduler may decide to run idle task if there
 *        does not exist any ready task.
 * @param csCallback Context Switching callback. When a context switching is
 *        decided by scheduler, scheduler notifies kernel using this callback
 *        and passes to be switched task (nextTCB param)
 *
 * @return none
 */
void Scheduler_Init(TCB* tcbList, TCB* idleTCB, SchedulerCSCallback csCallback);

/*
 * Yields task in Scheduler side.
 *
 *  When Kernel get a Yield() request, calls this function to notify scheduler
 *  about Yield. And then, scheduler prepares itself for yield.
 *
 * @param none
 * @return none
 *
 */
void Scheduler_Yield(void);

/*
 * Returns ready TCB to run.
 *
 * @param none
 *
 * @return A ready TCB to run.
 */
TCB* Scheduler_GetNextTCB(void);

#endif	/* __SCHEDULER_H */
