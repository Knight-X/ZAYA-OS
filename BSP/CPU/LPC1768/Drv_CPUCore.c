/*******************************************************************************
 *
 * @file Drv_CPUCore.c
 *
 * @author Murat Cakmak (MC)
 *
 * @brief CPU specific (Cortex-M3) hardware abstraction implementation.
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

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/* Index of PENDSV Interrupt in System Handlers Priority (SCB->SHP) Register */
#define SCB_SHP_PENDSV_INDEX			(10)

/* Initial Stack Value for Program Status Register (PSR) . */
#define TASK_INITIAL_PSR				(0x01000000)

/* 
 * PC (Program Counter) mask for task
 * Cortex-M spec offers that first bit (bit-0) of task start address should be 
 * zero. 
 */
#define TASK_START_ADDRESS_MASK			((reg32_t)0xfffffffeUL)

/*
 * Mask to set Vector Table offset address.
 *  In case of jump from bootloader to Firmware, Vector table must be set for
 *  application.
 *
 *  TODO : This mask locates Vector table in Flash (Start of Image) but Vector
 *  table can be located in SRAM. To do that Bit29 should be set.
 */
#define VECTOR_TABLE_SET_MASK           (0x1FFFFF80)

/* 
 * Priority of Kernel Interrupts. 
 *  Lowest priority!
 */
#define KERNEL_INTERRUPT_PRIORITY       (255)

/***************************** TYPE DEFINITIONS *******************************/
/*
 * Map for Stack Initialization of a Task Stack
 *
 * This struct is a map of a stack so all items should be consequent without
 * gaps. Therefore, we need to pack it to avoid CPU specific alignment and
 * padding even packing has overhead.
 */
TYPEDEF_STRUCT_PACKED
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
} TaskStackMap;

/**************************** FUNCTION PROTOTYPES *****************************/

extern void SystemInit(void);

/******************************** VARIABLES ***********************************/

/*
 * Reference of Current(Running) Task TCB.
 *
 * This variable is marked as internal because Assembly functions which located at different
 * files use this variable for context switching.
 */
INTERNAL TCB* currentTCB;

/*
 * Callback to get next TCB from Upper Layer (e.g. Kernel)
 */
INTERNAL Drv_CPUCore_CSGetNextTCBCallback GetNextTCBCallBack;

/**************************** PRIVATE FUNCTIONS ******************************/

/*
 *  A task must not exit or return to its caller. If so break all execution. 
 */
PRIVATE void ErrorOnTaskExit(void)
{
	Drv_CPUCore_Halt();
}

/***************************** PUBLIC FUNCTIONS *******************************/
/*
 * Initializes actual CPU and its components/peripherals.
 *
 * @param none
 * @return none
 */
void Drv_CPUCore_Init(void)
{
	/* Initialize System (Clocks, peripherals etc.) first */
	SystemInit();
}

/*
 * Resets device
 */
void Drv_CPUCore_ResetDevice(void)
{
	NVIC_SystemReset();
}

/*
 * Halts all system. 
 */
void Drv_CPUCore_Halt(void)
{
	/* Disable interrupts to avoid execution thru interrupts */
	__disable_irq();

	/* Block Execution */
	ENDLESS_WHILE_LOOP;
}

/*
 * Enables all interrupts.
 *
 * @param none
 * @return none
 *
 */
void Drv_CPUCore_EnableInterrupts(void)
{
	__enable_irq();
}

/*
 * Disables all interrupts.
 *
 * @param none
 * @return none
 *
 */
void Drv_CPUCore_DisableInterrupts(void)
{
	__disable_irq();
}

/*
 * Starts Context Switching
 *  Configures HW for CS and starts first task
 *  
 * @param getNextTCB Callback which provided by upper layer to get next TCB
 *
 * @return none
 */
void Drv_CPUCore_CSStart(Drv_CPUCore_CSGetNextTCBCallback getNextTCB)
{
	GetNextTCBCallBack = getNextTCB;

    
	/* Disable interrupts to avoid any interruption during Context Switching Initialization */
	__disable_irq();
    
    /*
     * Set priorities for Kernel Interrupts using System Handlers Priority (SHP)
     *  SHB Registers are byte accessible. 
     *  - PENDSV    : SHB[10]
     */
    /* TODO should we move to more common area than this function */
	SCB->SHP[SCB_SHP_PENDSV_INDEX] = KERNEL_INTERRUPT_PRIORITY;
    
    /* 
	 * Everything for CS is ready now let's trigger Context Switching
	 */
	StartContextSwitching();
}

/*
 * Initializes task stack according to Cortex-M3 Architecture.   
 *
 * @param stack to be initialized task stack
 * @param stackSize Stack Size
 * @param taskStartPoint Start Point (Function) of Task
 *
 * @return top of stack after initialization. 
 */
PUBLIC reg32_t* Drv_CPUCore_CSInitializeTCB(reg32_t topOfStack, reg32_t taskStartPoint)
{
	TaskStackMap* stackMap;

	/* Calculate the top of user stack aligned address */
	topOfStack -= sizeof(reg32_t);
	/* Align Stack Address with 8 */
	topOfStack = topOfStack & (~(0x0007));

	/* Map Stack Address */
	stackMap = (TaskStackMap*)topOfStack;

	/*
	 * Stack uses memory in descending order so after mapping the stack address
	 * with the the top of stack in previous line, we need to decrease this
	 * pointer to access stack content Because C structs work in ascending order
	 */
	stackMap--;

	/*
	 * Stack Initialization
	 */

	/* Set a Program Status Value for initial state */
	stackMap->PSR = TASK_INITIAL_PSR;

	/* Set Program Counter using User Task Start Point (Function) */
	stackMap->PC = ((uintptr_t)taskStartPoint) & TASK_START_ADDRESS_MASK;

	/* Set link register to handle case of fail */
	stackMap->LR = (uintptr_t)ErrorOnTaskExit;

	/* We do not pass argument so R0 must be zero. */
	stackMap->R0 = (uintptr_t)NULL;

	/*
	 * We already initialized a TCB for a Application (Process).
	 * Application starts to use its stack from this address.
	 *
	 * Actual stack address for execution start
	 */
	return (reg32_t*)stackMap;
}

/*
 * Jumps to other image on system.
 * It is used to pass control from Bootloader to Application (e.g. Firmware)
 *
 * @param image image address of Application in memory.
 *
 * @return none
 */
void Drv_CPUCore_JumpToImage(reg32_t imageAddress)
{
    /*
     * Change the Vector Table to the USER_FLASH_START in case the user
     * application uses interrupts
     */
    SCB->VTOR = imageAddress & VECTOR_TABLE_SET_MASK;

    /* We are ready to jump to other image now */
    JumpToImage(imageAddress);
}


uint32_t Drv_CPUCore_GetCPUFrequency(void)
{
	return SystemCoreClock;
}
