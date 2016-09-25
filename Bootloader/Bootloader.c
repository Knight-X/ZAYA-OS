/*******************************************************************************
 *
 * @file Bootloader.c
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Bootloader Implementation
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

#include "DRV_CPUCore.h"

#include "BSPConfig.h"

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/
/*
 * Bootlader Internal Settings
 */
typedef struct
{
	reg32_t nextLevelImageAddress; /* Image address to Jump from Bootloader */
} BootloaderSettings;

/**************************** FUNCTION PROTOTYPES *****************************/

/******************************** VARIABLES ***********************************/
PRIVATE BootloaderSettings settings;
/**************************** PRIVATE FUNCTIONS ******************************/

/***************************** PUBLIC FUNCTIONS *******************************/

int main()
{
	/* TODO : Remove Static Assignment */
	settings.nextLevelImageAddress = 0x8000;

    /*
     * Check new image attempt
     */

    /*
     * TODO Image Security Checks Here
     */

    /* Our image(application) is validated and ready to run */
	Drv_CPUCore_JumpToImage(settings.nextLevelImageAddress);

    return 0;
}
