/*******************************************************************************
 *
 * @file DrvCPUCoreAssembly.c
 *
 * @author Murat Cakmak (MC)
 *
 * @brief CPU Core specific (Cortex-M3) driver implementation for assembly
 * 		  functions.
 *
 *        Includes two type of assembly code of same implementations
 *        	- ARMCC Assembly
 *        	- GCC (GNU C) Assembly
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

#include "Drv_CPUCore.h"
#include "Drv_CPUCore_Internal.h"

#include "LPC17xx.h"
#include "cmsis_armcc.h"

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/* Vector Table Offset Register */
#define REG_SCB_VTOR_ADDR					(0xE000ED08)

/*
 * (Comment from FreeRTOS)
 * !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
 *
 *  See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html.
 */
#define MAX_SYSCALL_INTERRUPT_PRIORITY 		(191) /* equivalent to 0xb0, or priority 11. */

/*  */
#define LOAD_EXEC_RETURN_CODE 				(0xfffffffd)

/*
 * Super-Visor Call Types
 */
/* Start Context Switching using first task */
#define CPUCORE_SVCALL_START_CS				(0)
/* Task Yield */
#define CPUCORE_SVCALL_YIELD				(1)
/* Raise Privilege Mode */
#define CPUCORE_SVCALL_RAISE_PRIVILEGE		(2)

/***************************** TYPE DEFINITIONS *******************************/

/**************************** FUNCTION PROTOTYPES *****************************/
PRIVATE ALWAYS_INLINE ASSEMBLY_FUNCTION reg32_t GetPSP(void);
PRIVATE ALWAYS_INLINE ASSEMBLY_FUNCTION void SetPSP(reg32_t uiPSP);
PRIVATE ALWAYS_INLINE ASSEMBLY_FUNCTION void StoreRegisterToPSP(void);
PRIVATE ALWAYS_INLINE ASSEMBLY_FUNCTION void LoadRegisterFromPSP(void);

/******************************** VARIABLES ***********************************/

extern TCB* currentTCB;
/***************************** PRIVATE FUNCTIONS ******************************/

/***************************** PUBLIC FUNCTIONS *******************************/

#if defined(__ARMCC_VERSION) /* ARMCC Assembly Area */

/**
 * ISR Function to Handle Hard Fault
 * 
 *  Stack type should be differantied for systems which uses Main (MSP) and 
 *  Process (PSP) stack at same time. This handler function gets actual stack
 *  and passes stack content to Hard Fault Process function. In this way,
 *  stack content (e.g. PC which source of fault) can be dumped to output and 
 * 	root cause of fault can be easily found from disassembly output of build. 
 *  
 */
ASSEMBLY_FUNCTION void SPOS_HardFault_Handler(void) 
{
	/*
	 * Check Bit 3 of LR to determine actual stack (MSP or PSP).
	 */
	TST lr, #4
	ITE EQ
	MRSEQ r0, MSP
	MRSNE r0, PSP
	
	/* Call process funtion with actual stack pointer */
	B __cpp(HardFault_Handler)
}

/*
 * ISR for PendSV IRQ
 *
 *  We use PendSV Interrupts for Context Switching
 *
 */
void POS_PendSV_Handler(void)
{
	/* First, We need to store register to current process stack (PSP) */
	StoreRegisterToPSP();

	/* Get Stack address of current process */
	currentTCB->topOfStack = (reg32_t*)GetPSP();

	/* Get next TCB from Upper Layer (e.g. Kernel) */
	currentTCB = GetNextTCBCallBack();
	
	/* Set process stack (PSP) to new application stack */
	SetPSP((reg32_t)currentTCB->topOfStack);
	
	/* 
	 * Set control register. 
	 *  No need to set Stack Pointer (PSP/MSP) because "In Handler mode this bit
	 *  reads as zero and ignores writes (Comment from Cortex M3 TRM)"
	 *
	 * Just set privileged bit. 
	 */
	__set_CONTROL((currentTCB->flags.privileged == 0));
	
	/* Load registers using new application registers which already kept in its stack */
	LoadRegisterFromPSP();
	
}

/*
 * SVC (Super-Visor Call) IRQ Handler
 *
 *  We are using super-visor calls to handle context switching and system calls
 *  from unprivileged applications.
 *
 *   Please see
 *   infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0179b/ar01s02s07.html
 *   to see how to pass parameter to SVC Calls
 *
 */
ASSEMBLY_FUNCTION void POS_SVC_Handler(void)
{
	IMPORT SVCHandler

	TST lr, #4
	MRSEQ r0, MSP
	MRSNE r0, PSP

	/* Call SVCHandler function to process SVC Call */
	B SVCHandler
}

/*
 * Returns Process Stack Pointer (PSP)
 */
PRIVATE ALWAYS_INLINE ASSEMBLY_FUNCTION reg32_t GetPSP(void)
{
    MRS r0, psp
    BX lr
}

/*
 * Returns Process Stack Pointer (PSP)
 */
PRIVATE ALWAYS_INLINE ASSEMBLY_FUNCTION void SetPSP(reg32_t uiPSP)
{
    MSR psp, r0
    BX lr
}

/*
 * Stores actual values of register to preempted process' stack to use them
 * in next context switching
 */
PRIVATE ALWAYS_INLINE ASSEMBLY_FUNCTION void StoreRegisterToPSP(void)
{
    MRS r0, psp
    STMDB r0!, {r4-r11}
    MSR psp, r0
    BX lr
}

/*
 * Loads next process's register values to CPU registers. 
 */
PRIVATE ALWAYS_INLINE ASSEMBLY_FUNCTION void LoadRegisterFromPSP(void)
{
    MRS r0, psp
    LDMFD r0!, {r4-r11}
    MSR psp, r0
    BX lr
}

/*
 * Switchies Context to first task
 *
 */
ASSEMBLY_FUNCTION void SwitchToFirstTask(void)
{
	extern currentTCB;

	PRESERVE8

	ldr	r3, =currentTCB		/* Restore the context. */
	ldr r1, [r3]			/* Get the currentTCB address. */
	ldr r0, [r1]			/* The first item in currentTCB is the task top of stack. */
	
	ldmia r0!, {r4-r11}		/* Pop the registers that are not automatically saved on exception entry and the critical nesting count. */
	msr psp, r0				/* Restore the task stack pointer. */
	isb
	mov r0, #0
	msr	basepri, r0
	ldr r14, =LOAD_EXEC_RETURN_CODE		/* Load exec return code. */
	bx r14
}

/*
 * SVC Request Handler
 *  Handles and processes specific Super-Visor Calls
 */
void SVCHandler(uint32_t * svc_args)
{
	uint32_t svc_number;

	/*
     * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0179b/ar01s02s07.html
     * Stack contains:    * r0, r1, r2, r3, r12, r14, the return address and xPSR
	 * First argument (r0) is svc_args[0]
	 */
	svc_number = ((char *)svc_args[6])[-2];

	switch(svc_number)
	{
		case CPUCORE_SVCALL_START_CS:
			SwitchToFirstTask();
			break;
		case CPUCORE_SVCALL_YIELD:
			/* Set a PendSV to request a context switch. */
			SCB->ICSR = (reg32_t)SCB_ICSR_PENDSVSET_Msk;

			/*
			 * Barriers are normally not required but do ensure the code is completely
			 * within the specified behavior for the architecture. (Note From FreeRTOS)
			 */
			__DMB();
			break;
		case CPUCORE_SVCALL_RAISE_PRIVILEGE:
			/* Not defined yet */
			break;
		default:
			break;
	}
}

/*
 * Starts context switching by calling SVC interrupt
 *
 * @param none
 *
 * @return This function does not return, jumps to SVC ISR and SVC ISR continues
 *         with the Kernel (and user) defined tasks.
 *
 */
ASSEMBLY_FUNCTION void StartContextSwitching(void)
{
	PRESERVE8

	/* Use the NVIC offset register to locate the stack. */
	ldr r0, =REG_SCB_VTOR_ADDR
	ldr r0, [r0]
	ldr r0, [r0]

	/* Set the msp back to the start of the stack. */
	msr msp, r0
	
	/* Globally enable interrupts. */
	cpsie i
	cpsie f
	dsb
	isb
	
	/* Call SVC to start the first task. */
	svc #CPUCORE_SVCALL_START_CS
}

/*
 * http://www.nxp.com/documents/application_note/AN10866.pdf
 * Thanks to section '5.3.1 User application execution'
*/
/*
 * Starts context switching by calling SVC interrupt
 *
 * @param none
 *
 * @return This function does not return, jumps to SVC ISR and SVC ISR continues
 *         with the Kernel (and user) defined tasks.
 *
 */
ASSEMBLY_FUNCTION void JumpToImage(uint32_t imageAddress)
{
   LDR SP, [R0]     /* Load new stack pointer address */
   LDR PC, [R0, #4] /* Load new program counter address */
}

/*
 * Triggers a context switch.
 * 
 */
void Drv_CPUCore_CSYield(bool privileged)
{
	if (privileged)
	{
		/*
		 * This is a bit hacky code and may need revisit. 
		 * In case of privileged mode, SVC Call did not work so when we called
		 * this function we get "Unknown Instruction" exception in privileged 
		 * mode. (E.g. when Kernel needs to force a context switching)
		 * While privileged mode can access SCB->ICSR mode, we can directly 
		 * trigger PendSV register for Context Switching. 
		 * 
		 * On the other hand, caller of this function (Kernel or Unpriviliged 
		 * user app) should be aware of its mode (Priv or unpriv)
		 * 
		 */
		SCB->ICSR = (reg32_t)SCB_ICSR_PENDSVSET_Msk;
	}
	else
	{
		/* 
		 * For a privileged system we could trigger the PendSV interrupt using 
		 * " Interrupt Control and State Register (ICSR)" for Context
		 * Switching. 
		 * But for an ecosystem which includes also unprivileged process's, 
		 * it is forbidden to access ICSR register by unprivileged process. 
		 * 
		 * Therefore, we need to make specific Super-Visor Call to trigger PendSV
		 * for context switching. 
		 */
		__asm
		{
			svc #CPUCORE_SVCALL_YIELD
		}
	}
}

#else /* GNU C - GCC Assembly Area */
/*
 * TODO : [IMP] Until we use assembly code, we will not test Assembly modules.
 * So compile assembly blocks only for "Project Builds"
 */
#if !defined(UNIT_TEST)

/*
 * Note: By default GNU ARM compiler stores and restore a Frame Pointer
 * using "r7" and do stack alignment when entering into and exiting functions.
 * To avoid such optimizations we need to declare our handlers as "naked".
 *
 * REF : http://www.thewireframecommunity.com/writing-a-basic-multitasking-os-for-arm-cortex-m3-processor
 *
 */
ASSEMBLY_FUNCTION void POS_PendSV_Handler(void)  	__attribute__ ((naked));
ASSEMBLY_FUNCTION void POS_SVC_Handler(void) 		__attribute__ ((naked));

/*
 * ISR for PendSV Exception
 *
 *  We use PendSV Interrupts for Context Switching
 *
 * Implementation copied from FreeRTOS
 */
ASSEMBLY_FUNCTION void POS_PendSV_Handler(void)
{
	__asm volatile
	(
		"	mrs r0, psp							\n"
		"	isb									\n"
		"										\n"
		"	ldr	r3, currentTCBConst				\n" /* Get the location of the current TCB. */
		"	ldr	r2, [r3]						\n"
		"										\n"
		"	stmdb r0!, {r4-r11}					\n" /* Save the remaining registers. */
		"	str r0, [r2]						\n" /* Save the new top of stack into the first member of the TCB. */
		"										\n"
		"	stmdb sp!, {r3, r14}				\n"
		"	mov r0, %0							\n"
		"	msr basepri, r0						\n"
		"	bl SwitchContext					\n"
		"	mov r0, #0							\n"
		"	msr basepri, r0						\n"
		"	ldmia sp!, {r3, r14}				\n"
		"										\n"	/* Restore the context, including the critical nesting count. */
		"	ldr r1, [r3]						\n"
		"	ldr r0, [r1]						\n" /* The first item in currentTCB is the task top of stack. */
		"	ldmia r0!, {r4-r11}					\n" /* Pop the registers. */
		"	msr psp, r0							\n"
		"	isb									\n"
		"	bx r14								\n"
		"										\n"
		"	.align 4							\n"
		"currentTCBConst: .word currentTCB		\n"
		::"i"(MAX_SYSCALL_INTERRUPT_PRIORITY)
	);
}

/*
 * ISR for SVC Exception
 *
 *  We use SVC Interrupt to start Context Switching
 *
 * Implementation copied from FreeRTOS
 */
ASSEMBLY_FUNCTION void POS_SVC_Handler(void)
{
	__asm volatile
	(
		"	ldr	r3, currentTCBConst2			\n" /* Restore the context. */
		"	ldr r1, [r3]						\n" /* Use currentTCBConst2 to get the currentTCB address. */
		"	ldr r0, [r1]						\n" /* The first item in currentTCB is the task top of stack. */
		"	ldmia r0!, {r4-r11}					\n" /* Pop the registers that are not automatically saved on exception entry and the critical nesting count. */
		"	msr psp, r0							\n" /* Restore the task stack pointer. */
		"	isb									\n"
		"	mov r0, #0 							\n"
		"	msr	basepri, r0						\n"
		"	orr r14, #0xd						\n"
		"	bx r14								\n"
		"										\n"
		"	.align 4							\n"
		"currentTCBConst2: .word currentTCB		\n"
	);
}

/*
 * Starts context switching by calling SVC interrupt
 *
 * @param none
 *
 * @return This function does not return, jumps to SVC ISR and SVC ISR continues
 *         with the Kernel (and user) defined tasks.
 *
 */
ASSEMBLY_FUNCTION void StartContextSwitching(void)
{
	__asm volatile
	(
		" ldr r0, =0xE000ED08 	\n" /* Use the NVIC offset register to locate the stack. */
		//" ldr r0, %0 			\n" /* Use the NVIC offset register to locate the stack. */
		" ldr r0, [r0] 			\n"
		" ldr r0, [r0] 			\n"
		" msr msp, r0			\n" /* Set the msp back to the start of the stack. */
		" cpsie i				\n" /* Globally enable interrupts. */
		" cpsie f				\n"
		" dsb					\n"
		" isb					\n"
		" svc 0					\n" /* System call to start first task. */
		" nop					\n"
		//::"i"(REG_SCB_VTOR_ADDR)
	);
}

#endif /* #if !defined(UNITY) */

#endif /* #if ARMCC | GCC */
