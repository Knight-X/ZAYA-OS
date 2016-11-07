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

/*************************** FUNCTION DEFINITIONS *****************************/
/*
 * Initializes Scheduler.
 *
 * @param tcbList Task List (Pool) to be scheduled
 *
 * @return none
 */
void Scheduler_Init(Application* tcbList);

/*
 * Returns ready TCB to run.
 *
 * @param none
 *
 * @return A ready Application to run. Returns NULL if there is no ready app
 */
Application* Scheduler_GetNextApp(void);

/*
 * Terminates current active Application
 *
 * @param none
 * @return none
 */
void Scheduler_TerminateApplication(void);

#endif	/* __SCHEDULER_H */
