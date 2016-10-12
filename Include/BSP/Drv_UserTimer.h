/*******************************************************************************
 *
 * @file Drv_UserTimer.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief User Timer Driver Interface
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
#ifndef __DRV_USERTIMER_H
#define __DRV_USERTIMER_H

/********************************* INCLUDES ***********************************/

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/
#define DRV_TIMER_INVALID_HANDLE        (-1)

/***************************** TYPE DEFINITIONS *******************************/
typedef int32_t Drv_TimerHandle;
typedef void (*Drv_TimerCallback)(void);

/*************************** FUNCTION DEFINITIONS *****************************/
void Drv_UserTimer_Init(void);
Drv_TimerHandle Drv_UserTimer_Create(Drv_TimerCallback userTimerCB);
void Drv_UserTimer_Remove(Drv_TimerHandle timer);
void Drv_UserTimer_Start(Drv_TimerHandle timer, uint32_t timeout);
void Drv_UserTimer_Stop(Drv_TimerHandle timer, uint32_t timeout);
void Drv_UserTimer_DelayUs(uint32_t microseconds);
void Drv_UserTimer_DelayMs(uint32_t milliseconds);

#endif	/* __DRV_USERTIMER_H */
