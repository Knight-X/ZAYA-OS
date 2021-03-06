/*******************************************************************************
 *
 * @file unittest_CPUCore.c
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Unit test file for CPU module
/*******************************************************************************
 *
 * @file BSPConfig.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Project Specific BSP Configurations
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

/*
 * [IMP] Our code base has some endless while loops which can not give back
 * its execution to Unit Test framework. To avoid this, we used macros for
 * endless while loops and we are overriding this macro for unit tests.
 * We have to implement (override) ENDLESS_WHILE_LOOP before posttypes.h
 * because if it is created before it will not be created in posttypes.h anymore.
 */
#define ENDLESS_WHILE_LOOP
#include "postypes.h"

/* Let's include mock source files to simulate external module behaviours */
#include "Mock/mock_CPUCore_Assembly.c"
#include "Mock/mock_GPIO.c"

/* Include CPU source file for WHITE-BOX unit testing */
#include "../Drv_CPUCore.c"

/* Include Unity Framework */
#include "unity.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/

/*
 * Map for Stack Initialization of a Task Stack
 */
typedef struct
{
	reg32_t R4;
	reg32_t R5;
	reg32_t R6;
	reg32_t R7;
	reg32_t R8;
	reg32_t R9;
	reg32_t R10;
	reg32_t R11;
	reg32_t R0;
	reg32_t R1;
	reg32_t R2;
	reg32_t R3;
	reg32_t R12;
	reg32_t LR;  		/* Link Register */
	reg32_t PC;	 		/* Program Counter */
	reg32_t PSR; 		/* Program Status Register */
} StackMap;

/**************************** FUNCTION PROTOTYPES *****************************/

/******************************** VARIABLES ***********************************/

/**************************** INTERNAL FUNCTIONS ******************************/
/**
 * @brief Constructor Method for each test case
 *
 */
void setUp(void)
{
	/* Clear all registers for each test */
	ResetRegistersAndObjects();
}

/**
 * @brief Destructor Method for each test case
 *
 */
void tearDown(void)
{
	/* For now, nothing to do */
}

/*
 * An user task to use it in task creation tests
 */
void taskStartPoint(void* arg)
{
	/* Avoid compiler warning for unused variables */
	(void)arg;
}

TCB* GetNextTCB(void)
{
	static TCB tcb;
	return &tcb;
}

/***************************** TEST FUNCTIONS *******************************/

/*
 * Tests CPU Init function.
 *
 * CPU_Init() does not do anything(it is an empty function) so nothing to test.
 * Function just called to complete Code Coverage.
 *
 */
void test_CPU_Init(void)
{
	Drv_CPUCore_Init();

	TEST_ASSERT(1);
}

/*
 * Tests CPU_Halt function.
 *  When this function is called system should be broken.
 *   - Interrupts should be disabled.
 *   - Execution should be blocked in a endless while loop.
 */
void test_CPU_Halt(void)
{
	Drv_CPUCore_Halt();

	/* CPU_Halt disables interrupts */
	TEST_ASSERT((lpcMockObjects.flags.interrupt_disabled == 1));

	/* TODO : Check also whether execution entered into endless while loop or not */
}

/*
 * Tests Functionality which starts Context Switching
 */
void test_CPU_CS_Start(void)
{
	/* Content of This TCB is not important for us. */
	static TCB initialTCB;

	Drv_CPUCore_CSStart(&initialTCB, GetNextTCB);

	/* Check for internal global variables which keep next task (TCB)*/
	TEST_ASSERT((currentTCB == &initialTCB));

	/* Interrupts must be enable after that CS_Start function */
	TEST_ASSERT((lpcMockObjects.flags.interrupt_disabled == 0));

	/* PENDSV Priority Located at Index 10 and check set interrupt value of PENDSV Interrupt */
	TEST_ASSERT((SCB->SHP[10] == KERNEL_INTERRUPT_PRIORITY))

	/* Check whether if SVC function is called or not! */
	TEST_ASSERT((lpcMockObjects.flags.svc_handler_call == 1));
}

/*
 * Tests Stack Initialization
 */
void test_CPU_CS_InitializeTaskStack(void)
{
	/* 32 Depth should be enough for initialized stack area */
	reg32_t testStack[32];
	StackMap* stackMap;
	TCB tcb;

	tcb.topOfStack = Drv_CPUCore_CSInitializeTCB((reg32_t)&testStack[32], (reg32_t)taskStartPoint);

	/* Cast Stack to Stack Map to access fields easy */
	stackMap = (StackMap*)tcb.topOfStack;

	/* Check Address Alignment first. Address should be multiple of 8 */
	TEST_ASSERT((((uintptr_t)tcb.topOfStack) & 0x7) == 0);

	/* Check Program Status Register (PSR) */
	TEST_ASSERT(stackMap->PSR == TASK_INITIAL_PSR);

	/* Check Program Counter (PC) */
	TEST_ASSERT(stackMap->PC == (((uintptr_t)taskStartPoint) & TASK_START_ADDRESS_MASK));

	/* Check Link Register */
	TEST_ASSERT(stackMap->LR == ((uintptr_t)ErrorOnTaskExit));

	/* Check R0 register. We dont pass any argument so should be zero. */
	TEST_ASSERT(stackMap->R0 == 0);

	/*
	 * When CPU_CS_InitializeTaskStack() function is called, it implicitly
	 * (by triggering HW) calls taskStartPoint(). Or in case of fail
	 * (e.g. execution returns from taskStartPoint:  It must not! )
	 * ErrorOnTaskExit() functions are called.
	 * They are trivial functions for testing and let's call them explicitly
	 * here for 100% code coverage.
	 */
	taskStartPoint(NULL);
	ErrorOnTaskExit();
}

/*
 * Tests address alignment of a stack after initialization
 */
void test_CPU_CS_StackAlignmentTest(void)
{
	/* 32 Depth should be enough for initialized stack area */
	reg32_t testStack[32];
	int32_t i;
	uint8_t* stackStartAddr;
	TCB tcb;

	/*
	 * Address of Top of Stack should be aligned with 8 so trying sequential 8
	 * addresses should be enough for alignment check
	 */
	for (i = 0; i < 8; i++)
	{
		/* Increase stack offset by one */
		stackStartAddr = ((uint8_t*)&testStack[30]) + i;

		/* Give a raw and get top of initialized stack address */
		tcb.topOfStack = Drv_CPUCore_CSInitializeTCB((reg32_t)stackStartAddr, (reg32_t)taskStartPoint);

		/* Check Address Alignment first. Address should be multiple of 8 */
		TEST_ASSERT((((uintptr_t)tcb.topOfStack) & 0x7) == 0);
	}
}
