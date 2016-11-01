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

#include "Drv_CPUCore_Internal.h"
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

/***************************** TYPE DEFINITIONS *******************************/

/**************************** FUNCTION PROTOTYPES *****************************/

/******************************** VARIABLES ***********************************/

/***************************** PRIVATE FUNCTIONS ******************************/

/***************************** PUBLIC FUNCTIONS *******************************/

#if defined(__ARMCC_VERSION) /* ARMCC Assembly Area */

/*
 * ISR for PendSV Exception
 *
 *  We use PendSV Interrupts for Context Switching
 *
 * Implementation copied from FreeRTOS
 */
ASSEMBLY_FUNCTION void POS_PendSV_Handler( void )
{
	extern currentTCB;
	extern SwitchContext;

	PRESERVE8

	mrs r0, psp
	isb

	ldr	r3, =currentTCB			/* Get the location of the current TCB. */
	ldr	r2, [r3]

	stmdb r0!, {r4-r11}			/* Save the remaining registers. */
	str r0, [r2]				/* Save the new top of stack into the first member of the TCB. */

	stmdb sp!, {r3, r14}
	mov r0, #MAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0
	dsb
	isb
	bl SwitchContext
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r3, r14}

	ldr r1, [r3]
	ldr r0, [r1]				/* The first item in currentTCB is the task top of stack. */
	ldmia r0!, {r4-r11}			/* Pop the registers and the critical nesting count. */
	msr psp, r0
	isb
	bx r14
	nop
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
	orr r14, #0xd
	bx r14
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
ASSEMBLY_FUNCTION void StartContextSwitching(void){
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
	svc 0
	nop
	nop
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
