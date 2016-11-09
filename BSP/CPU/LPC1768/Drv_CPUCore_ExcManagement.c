/*******************************************************************************
 *
 * @file Drv_CPUCore_ExcManagement.c
 *
 * @author Murat Cakmak
 *
 * @brief Exception Management for LPC17xx
 *
 * @see https://github.com/P-LATFORM/SP-OS/wiki
 *
 ******************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 SP-OS
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
 ******************************************************************************/

/********************************* INCLUDES ***********************************/

#include "Drv_CPUCore.h"

#include "DrvConfig.h"

#include "LPC17xx.h"

/***************************** MACRO DEFINITIONS ******************************/
/* Flag to enable DivByZero Exception */
#define REG_SCB_CFSR_USGFAULT_DIVBYZERO_Msk			(1<<25)

#define REG_SCB_CFSR_USGFAULT_IACCVIOL_Msk			(1<<0)
#define REG_SCB_CFSR_USGFAULT_DACCVIOL_Msk			(1<<1)
#define REG_SCB_CFSR_USGFAULT_MUNSTKERR_Msk			(1<<3)
#define REG_SCB_CFSR_USGFAULT_MSTKERR_Msk			(1<<4)
#define REG_SCB_CFSR_USGFAULT_MMARVALID_Msk			(1<<7)

/***************************** TYPE DEFINITIONS *******************************/
/* 
 * Stack content (Registers) for Cortex M3. 
 *  Do not change order. 
 */
enum { r0, r1, r2, r3, r12, lr, pc, psr} RegistersInStack ;

/**************************** FUNCTION PROTOTYPES *****************************/

/******************************** VARIABLES ***********************************/
/*
 * Callback function to inform Client Code about Exception
 */
PRIVATE ExceptionCallback userExcCallback;

#if ENABLE_STACK_DUMP
/*
 * Stack Content of faulty process. 
 *  Used for stack dump
 */
PRIVATE reg32_t* lastFaultStack;

/*
 * Just a simple controller flag to avoid wrong calls of stack dump
 */
PRIVATE bool faultStackIsValid = false;
#endif /* ENABLE_STACK_DUMP */
/**************************** PRIVATE FUNCTIONS *******************************/

#if ENABLE_STACK_DUMP
/*
 * Prints out stack content using externally provided printOut function. 
 *  Lines are formatted. On the other hand, hex representation could be better, 
 *  but due to decrease code size we printed out registers in decimal format. 
 *  
 */
PRIVATE void stackDump(PrintOutCallback printOut)
{
	uint8_t line[64];
	
	/*
	 * Stack Dump function is provided to upper layer (e.g. Kernel) when an 
	 * exception is occurred. A code can call this function whenever wants. 
	 * This flags avoid to process this function. 
	 * This function is only valid for one time after exception. Second attempt
	 * does not do anything. 
	 */
	if (faultStackIsValid == false) return;	
	faultStackIsValid = false;
	
	sprintf((char*)line ,"\nr0 =%d", lastFaultStack[r0]);
	printOut(line);
	sprintf((char*)line ,"\nr1 =%d", lastFaultStack[r1]);
	printOut(line);
	sprintf((char*)line ,"\nr2 =%d", lastFaultStack[r2]);
	printOut(line);
	sprintf((char*)line ,"\nr3 =%d", lastFaultStack[r3]);
	printOut(line);
	sprintf((char*)line ,"\nr12=%d", lastFaultStack[r12]);
	printOut(line);	
	sprintf((char*)line ,"\nlr =%d", lastFaultStack[lr]);
	printOut(line);
	sprintf((char*)line ,"\npc =%d", lastFaultStack[pc]);
	printOut(line);
	sprintf((char*)line ,"\npsr=%d", lastFaultStack[psr]);
}

#endif
/*
 * Processes Usage Fault
 */
PRIVATE ALWAYS_INLINE void processUsageFault(Exception* exc, reg32_t* val)
{
	/* Divide by Zero exception */
	if (SCB->CFSR & REG_SCB_CFSR_USGFAULT_DIVBYZERO_Msk)
	{
		*exc = Exception_DivideByZero;
		*val = 0;
	}
	else
	{
		/*
		 * Other exceptions are not processes.
		 *  TODO should be also processes when needed. 
		 */
		*exc = Exception_UsageFault;
		*val = ((SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk) >> SCB_CFSR_USGFAULTSR_Pos);
	}
}

/*
 * Processes BUS Faults
 */
PRIVATE ALWAYS_INLINE void processBusFault(Exception* exc, reg32_t* val)
{
	/*
	 * Not processed in detail.
	 * TODO shoud be revisited. 
	 */
	*exc = Exception_BusFault;
	*val = ((SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk) >> SCB_CFSR_BUSFAULTSR_Pos);
}

/*
 *  Processes Memory Management Faults
 */
PRIVATE ALWAYS_INLINE void processMemFault(Exception* exc, reg32_t* val)
{
	/* 
	 * Check whether Memory Management Fault Adddress Register is valid. 
	 * if so, SCB->MMFAR also provides additional info (e.g. unauthorized 
	 * address) for root cause of exception
	 */
	if (SCB->CFSR & REG_SCB_CFSR_USGFAULT_MMARVALID_Msk)
	{
		*val = SCB->MMFAR;
	}
	else
	{
		*val = 0;
	}
	
	if (SCB->CFSR & REG_SCB_CFSR_USGFAULT_IACCVIOL_Msk)
	{
		/* App tries to access an unauthorized code block */
		*exc = Exception_CodeAccessViolation;
	}
	else if (SCB->CFSR & REG_SCB_CFSR_USGFAULT_DACCVIOL_Msk)
	{
		/* App tries to access an unauthorized RAM block */
		*exc = Exception_DataAccessViolation;		
	}
	else
	{
		*exc = Exception_AccessViolation;
		*val = ((SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk) >> SCB_CFSR_MEMFAULTSR_Pos);
	}
}

/*
 * Processes forced Hard Fault Exception. 
 * 
 * NOTE it assumes just one exception (Usage, Mem or Bus) at a time. 
 */
PRIVATE ALWAYS_INLINE void processForcedException(Exception* exc, reg32_t* val)
{
	/* Let's clear forced fault flags first */
	SCB->HFSR |= SCB_HFSR_FORCED_Msk;
	
	if((SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk) != 0)
	{
		/* Usage Fault */
		processUsageFault(exc, val);
	}
	else if ((SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk) != 0)
	{
		/* Bus Fault */
		processBusFault(exc, val);
	}
	else if ((SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk) != 0)
	{
		/* Memory Management Fault */
		processMemFault(exc, val);
	}
	
	/* 
	 * We read CFSR for fault details. Now we can clear flag. 
	 * Bits are stick so write 1 to clear bits. See Cortex M3 TRM. 
	 */
	SCB->CFSR |= SCB->CFSR;
}

/*
 * Hard Fault Handler. 
 *  HW Hard Fault Handler exctract actual stack (MSP or PSP) and provides
 *  to this function. This function also process hard fault in details and 
 *  informs upper layer about exact type of fault .
 *  It also provides stack content to upper layer to help on root cause of 
 *  exception.
 */
INTERNAL void HardFault_Handler(uint32_t stack[])
{
	Exception exc = Exception_HardFault;
	reg32_t val = 0;
	
	if (SCB->HFSR & SCB_HFSR_FORCED_Msk)
	{
		/* 
		 * Forced exception (not handled by Usage, Bus or MemManagement
		 * Exceptions)
		 */
		processForcedException(&exc, &val);
	}
	else 
	{
		/*
		 * Other types not processed for now. But inform upper layer about 
		 * exception
		 */
		exc = Exception_HardFault;
		val = SCB->HFSR;
	}
	
	if (userExcCallback != NULL)
	{
#if ENABLE_STACK_DUMP
		
		lastFaultStack = stack;
		/* Allow only usage of stack dump for only one time*/
		faultStackIsValid = true;
		
		/* 
		 * Inform upper layer (e.g. Kernel) about exception 
		 *  - Exception : Type of exception
		 *  - Reg Value for unprocessed registers.
		 *  - Interface to dump stack content. 
		 */
		userExcCallback(exc, val, stackDump);
		
		/* Disallow second usage of stack dump */
		faultStackIsValid = false;
#else
		userExcCallback(exc, val, NULL);
#endif
	}
}

/***************************** PUBLIC FUNCTIONS *******************************/
/*
 * Initializes Exception Management
 */
void Drv_CPUCore_InitializeExceptions(ExceptionCallback excCallback)
{
	/*
	 * We could handle Usage, Bus and Memory Faults seperately but in 
	 * multitasking system, MSP and PSP should be also handled for each 
	 * exception. 
	 * Instead, we use Hard Fault (forced) Exception to handle all exceptions. 
	 *
	 * If required, use SCB->SHCSR register to enable other exceptions. 
	 */
	
	/* 
	 * Handle all events 
	 */
	SCB->CCR |= SCB_CCR_STKALIGN_Msk		/*  */
			 |  SCB_CCR_BFHFNMIGN_Msk
	         |  SCB_CCR_DIV_0_TRP_Msk
			 |  SCB_CCR_UNALIGN_TRP_Msk
			 |  SCB_CCR_USERSETMPEND_Msk
			 |  SCB_CCR_NONBASETHRDENA_Msk;
	
	/* Save user exception handler to inform when an exception is occurred */
	userExcCallback = excCallback;
}
