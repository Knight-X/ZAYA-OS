/*******************************************************************************
 *
 * @file Drv_CPUCore_MPU.c
 *
 * @author MC
 *
 * @brief Memory Protection Implementation for LPC17xx
 *
 * @see https://github.com/P-LATFORM/P-OS/wiki
 *
 ******************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 P-OS
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
 *******************************************************************************/

/********************************* INCLUDES ***********************************/

#include "Drv_CPUCore.h"
#include "Drv_CPUCore_Internal.h"

#include "LPC17xx.h"

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/
/* Defines to enable/disable MPU Regions */
#define MPU_REGION_DISABLE								(0)
#define MPU_REGION_ENABLE								(1)

/* Defines to mark regions as valid/invalid */
#define MPU_REGION_NOTVALID								(0)
#define MPU_REGION_VALID								(1)

/*
 * MPU Region Types
 */
 /* Privileged Regions*/
#define MPU_REGION_PRIVILEGED_CODE						(0)
#define MPU_REGION_PRIVILEGED_RAM						(1)
#define MPU_REGION_PRIVILEGED_GPIO						(2)
#define MPU_REGION_PRIVILEGED_PERIPHERALS				(3)

/* Shared Regions */
#define MPU_REGION_SHARED_CODE							(4)
#define MPU_REGION_SHARED_RAM							(5)

/* User Regions */
#define MPU_REGION_UNPRIVILEGED_USER_CODE				(6)
#define MPU_REGION_UNPRIVILEGED_USER_RAM				(7)

/*
 * Details of MPU Regions
 */
/* Flash (Code) Area */
#define MPU_REGION_FLASH_START							(0x0UL)
#define MPU_REGION_FLASH_SIZE							(0x80000)
#define MPU_REGION_FLASH_SIZE_VALUE						(18)

/* Device RAM */
#define MPU_REGION_SRAM_START							(0x10000000UL)
#define MPU_REGION_SRAM_SIZE							(0x8000)
#define MPU_REGION_SRAM_SIZE_VALUE						(14)

/* GPIO */
#define MPU_REGION_GPIO_START 							(LPC_GPIO_BASE)
#define MPU_REGION_GPIO_SIZE 							(0x3FFF)
#define MPU_REGION_GPIO_SIZE_VALUE						(13)

/* Peripherals */
#define MPU_REGION_PERIPHERALS_START					(0x40000000UL)
#define MPU_REGION_PERIPHERALS_END						(0x5FFFFFFFUL)
#define MPU_REGION_PERIPHERALS_SIZE_VALUE				(28)

/*
 * AP Encoding Types
 *  Specifies Read/Write Writes
 */
/* Read/Write  */
#define MPU_AP_ENCODING_RW								(3)
/* Read Only */
#define MPU_AP_ENCODING_RO								(6)
/* Privileged Read/Write */
#define MPU_AP_ENCODING_PRIVILEGED_RW					(1)
/* Privileged Read Only */
#define MPU_AP_ENCODING_PRIVILEGED_RO					(5)

/*
 * Access Types
 */
#define MPU_ACCESS_CACHEABLE_BUFFERABLE					(7)

/*
 * Aligns Region address for MPU
 *
 * Region address should be aligned with its size and address should be set
 * according to its address. See ADDR field of MPU RABR register for details.
 * While addr register needs only required address length (and rest of is
 * reserved bits), we are setting all address bits (including address  reserved)
 */
#define MPU_ALIGN_REGION_ADDR(addr)						((addr) >>MPU_RBAR_ADDR_Pos)


#define MPU_SMALLEST_PERMITTED_REGION_SIZE				(32)

/*
 * Default Region Specific MPU Register Settings
 *
 */
/*
 * Privileged FLASH
 */
/* RBAR Settings for Code (Flash) Region */
#define MPU_FLASH_RBAR_VAL \
			(MPU_REGION_PRIVILEGED_CODE) | \
			(MPU_RBAR_VALID_Msk) | \
			(MPU_ALIGN_REGION_ADDR(MPU_REGION_FLASH_START) << MPU_RBAR_ADDR_Pos)

/* RASR Settings for Code (Flash) Region */
#define MPU_FLASH_RASR_VAL \
			(MPU_AP_ENCODING_PRIVILEGED_RO << MPU_RASR_AP_Pos) | \
			(MPU_ACCESS_CACHEABLE_BUFFERABLE << MPU_RASR_B_Pos) | \
			(MPU_REGION_FLASH_SIZE_VALUE << MPU_RASR_SIZE_Pos) | \
			(MPU_RASR_ENABLE_Msk)

/*
 * Privileged RAM
 */
/* RBAR Settings for RAM Region */
#define MPU_RAM_RBAR_VAL \
			(MPU_REGION_PRIVILEGED_RAM) | \
			(MPU_RBAR_VALID_Msk) | \
			(MPU_ALIGN_REGION_ADDR(MPU_REGION_SRAM_START) << MPU_RBAR_ADDR_Pos)

/* RASR Settings for RAM Region */
#define MPU_RAM_RASR_VAL \
			(MPU_AP_ENCODING_PRIVILEGED_RW << MPU_RASR_AP_Pos) | \
			(MPU_ACCESS_CACHEABLE_BUFFERABLE << MPU_RASR_B_Pos) | \
			(MPU_REGION_SRAM_SIZE_VALUE << MPU_RASR_SIZE_Pos) | \
			(MPU_RASR_ENABLE_Msk)

/*
 * Privileged GPIO
 */
/* RBAR Settings for GPIO Region */
#define MPU_GPIO_RBAR_VAL \
			(MPU_REGION_PRIVILEGED_GPIO) | \
			(MPU_RBAR_VALID_Msk) | \
			(MPU_ALIGN_REGION_ADDR(MPU_REGION_GPIO_START) << MPU_RBAR_ADDR_Pos)

/* RASR Settings for GPIO Region */
#define MPU_GPIO_RASR_VAL \
			(MPU_AP_ENCODING_RW << MPU_RASR_AP_Pos) | \
			(MPU_ACCESS_CACHEABLE_BUFFERABLE << MPU_RASR_B_Pos) | \
			(MPU_REGION_GPIO_SIZE_VALUE << MPU_RASR_SIZE_Pos) | \
			(MPU_RASR_ENABLE_Msk)

/*
 * Privileged Peripherals
 */
/* RBAR Settings for Peripherals Region */
#define MPU_PERIPHERALS_RBAR_VAL \
			(MPU_REGION_PRIVILEGED_PERIPHERALS) | \
			(MPU_RBAR_VALID_Msk) | \
			(MPU_ALIGN_REGION_ADDR(MPU_REGION_PERIPHERALS_START) << MPU_RBAR_ADDR_Pos)

/* RASR Settings for Peripherals Region */
#define MPU_PERIPHERALS_RASR_VAL \
			(MPU_AP_ENCODING_RW << MPU_RASR_AP_Pos) | \
			(MPU_RASR_XN_Msk) | \
			(MPU_REGION_PERIPHERALS_SIZE_VALUE << MPU_RASR_SIZE_Pos) | \
			(MPU_RASR_ENABLE_Msk)

/*
 * UnPrivileged Shared Flash
 */
/* RBAR Settings for Shared Code (Flash) Region */
#define MPU_SHARED_FLASH_RBAR_VAL(start) \
			(MPU_REGION_SHARED_CODE) | \
			(MPU_RBAR_VALID_Msk) | \
			(MPU_ALIGN_REGION_ADDR(start) << MPU_RBAR_ADDR_Pos)

/* RASR Settings for Shared Code (Flash) Region */
#define MPU_SHARED_FLASH_RASR_VAL(size) \
			(MPU_AP_ENCODING_RO << MPU_RASR_AP_Pos) | \
			(MPU_ACCESS_CACHEABLE_BUFFERABLE << MPU_RASR_B_Pos) | \
			(getRegionSizeValue(size) << MPU_RASR_SIZE_Pos) | \
			(MPU_RASR_ENABLE_Msk)

/*
 * UnPrivileged Shared RAM
 */
/* RBAR Settings for RAM Region */
#define MPU_SHARED_RAM_RBAR_VAL(start) \
			(MPU_REGION_SHARED_RAM) | \
			(MPU_RBAR_VALID_Msk) | \
			(MPU_ALIGN_REGION_ADDR(start) << MPU_RBAR_ADDR_Pos)

/* RASR Settings for RAM Region */
#define MPU_SHARED_RAM_RASR_VAL(size) \
			(MPU_AP_ENCODING_RW << MPU_RASR_AP_Pos) | \
			(MPU_ACCESS_CACHEABLE_BUFFERABLE << MPU_RASR_B_Pos) | \
			(getRegionSizeValue(size) << MPU_RASR_SIZE_Pos) | \
			(MPU_RASR_ENABLE_Msk)


/*
 * UnPrivileged User Flash
 */
/* RBAR Settings for Code (Flash) Region */
#define MPU_UFLASH_RBAR_VAL(start) \
			(MPU_REGION_UNPRIVILEGED_USER_CODE) | \
			(MPU_RBAR_VALID_Msk) | \
			(MPU_ALIGN_REGION_ADDR(start) << MPU_RBAR_ADDR_Pos)

/* RASR Settings for Code (Flash) Region */
#define MPU_UFLASH_RASR_VAL(size) \
			(MPU_AP_ENCODING_RO << MPU_RASR_AP_Pos) | \
			(MPU_ACCESS_CACHEABLE_BUFFERABLE << MPU_RASR_B_Pos) | \
			(getRegionSizeValue(size) << MPU_RASR_SIZE_Pos) | \
			(MPU_RASR_ENABLE_Msk)

/*
 * UnPrivileged User RAM
 */
/* RBAR Settings for RAM Region */
#define MPU_URAM_RBAR_VAL(start) \
			(MPU_REGION_UNPRIVILEGED_USER_RAM) | \
			(MPU_RBAR_VALID_Msk) | \
			(MPU_ALIGN_REGION_ADDR(start) << MPU_RBAR_ADDR_Pos)

/* RASR Settings for RAM Region */
#define MPU_URAM_RASR_VAL(size) \
			(MPU_AP_ENCODING_RW << MPU_RASR_AP_Pos) | \
			(MPU_ACCESS_CACHEABLE_BUFFERABLE << MPU_RASR_B_Pos) | \
			(getRegionSizeValue(size) << MPU_RASR_SIZE_Pos) | \
			(MPU_RASR_ENABLE_Msk)

/***************************** TYPE DEFINITIONS *******************************/

/**************************** FUNCTION PROTOTYPES *****************************/

/******************************** VARIABLES ***********************************/

/***************************** PRIVATE FUNCTIONS ******************************/

/*
 * Returns formatted size of a regions
 *
 * Note from Cortex M3 TRM.
 * The SIZE field defines the size of the MPU memory region specified by the 
 * RNR. as follows:
 *	(Region size in bytes) = 2 ^ (SIZE+1)
 * The smallest permitted region size is 32B, corresponding to a SIZE value of 4. 
 */
PRIVATE uint32_t getRegionSizeValue(uint32_t actualSize)
{
	uint32_t regionSize;
	uint32_t sizeVal = 4;

	/* 32 is the smallest region size, 31 is the largest valid value for
	ulReturnValue. */
	for( regionSize = 32UL; sizeVal < 31UL; ( regionSize <<= 1UL ) )
	{
		if( actualSize <= regionSize )
		{
			break;
		}
		else
		{
			sizeVal++;
		}
	}

	/* Shift the code by one before returning so it can be written directly
	into the the correct bit position of the attribute register. */
	return sizeVal;
}

/*
 * Sets (Release) user app code section
 */
INTERNAL void MPUSetUserCodeSection(reg32_t sectionStart, uint32_t size)
{
	MPU->RBAR = MPU_UFLASH_RBAR_VAL(sectionStart);
	MPU->RASR = MPU_UFLASH_RASR_VAL(size);
}

/*
 * Sets (Release) user app ram section
 */
INTERNAL void MPUSetUserRAMSection(reg32_t sectionStart, uint32_t size)
{
	MPU->RBAR = MPU_URAM_RBAR_VAL(sectionStart);
	MPU->RASR = MPU_URAM_RASR_VAL(size);
}

/***************************** PUBLIC FUNCTIONS *******************************/

/*
 * Initializes MPU
 *
 *  NOTE This function initializes Flash, GPIO and Peripherals statically.
 *  On the other hand, a driver should provide a generic interface to support
 *  different needs of upper layers. Modify it for that purpose
 *
 */
PUBLIC void Drv_CPUCore_InitializeMPU(reg32_t sharedCodeStart, uint32_t sharedCodeSize,
							   reg32_t sharedRAMStart,  uint32_t sharedRAMSize)
{
	/* Enter Critical Section to ensure about integrity of MPU initialization */
	__disable_irq();

	/* FLASH (Code) */
	MPU->RBAR = MPU_FLASH_RBAR_VAL;
	MPU->RASR = MPU_FLASH_RASR_VAL;
	
	/* RAM */
	MPU->RBAR = MPU_RAM_RBAR_VAL;
	MPU->RASR = MPU_RAM_RASR_VAL;

	/* GPIO */
	MPU->RBAR = MPU_GPIO_RBAR_VAL;
	MPU->RASR = MPU_GPIO_RASR_VAL;

	/* PERIPHERALS */
	MPU->RBAR = MPU_PERIPHERALS_RBAR_VAL;
	MPU->RASR = MPU_PERIPHERALS_RASR_VAL;
	
	if (sharedCodeSize >= MPU_SMALLEST_PERMITTED_REGION_SIZE)
	{
		/* Shared (UnPrivileged) Code Section */
		MPU->RBAR = MPU_SHARED_FLASH_RBAR_VAL(sharedCodeStart);
		MPU->RASR = MPU_SHARED_FLASH_RASR_VAL(sharedCodeSize);
	}
	
	if (sharedRAMSize >= MPU_SMALLEST_PERMITTED_REGION_SIZE)
	{
		/* Shared (UnPrivileged) RAM Section */
		MPU->RBAR = MPU_SHARED_RAM_RBAR_VAL(sharedRAMStart);
		MPU->RASR = MPU_SHARED_RAM_RASR_VAL(sharedRAMSize);
	}

	/* Enable the MPU with the background region configured. */
	MPU->CTRL |= ( MPU_CTRL_ENABLE_Msk | MPU_CTRL_PRIVDEFENA_Msk );

	/* Exit from critical section */
	__enable_irq();
}
