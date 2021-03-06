/*******************************************************************************
 *
 * @file Bootloader.c
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Bootloader application implementation.
 *          - Listens for upgrade attempt
 *              - Upgrades Firmware
 *          - Validates Firmware Image
 *              - Jumps to Firmware if Firmware has a valid signature
 *
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

#include "Drv_UART.h"
#include "Drv_UserTimer.h"
#include "Drv_Timer.h"
#include "Drv_CPUCore.h"

#include "Bootloader_Internal.h"
#include "Bootloader_Config.h"
#include "BSPConfig.h"

#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/

/*
 * Bootlader Internal Settings
 */
typedef struct
{
	/* Meta Data of Firmware */
	FirmwareInfo* firmwareInfo;
} BootloaderSettings;

/**************************** FUNCTION PROTOTYPES *****************************/

/******************************** VARIABLES ***********************************/
/* Bootloader internal settings */
PRIVATE BootloaderSettings settings = { { 0 } };

/**************************** PRIVATE FUNCTIONS ******************************/
/*
 * Reads Firmware Area and returns Meta Data of Firmware
 *
 */
PRIVATE ALWAYS_INLINE void GetMetaData(FirmwareInfo** metaData)
{
#if !SIMULATION_MODE
	*metaData = (FirmwareInfo*)FIRMWARE_START_ADDRESS;
#else
	/* Should be implemented */
#endif
}

/*
 * Checks user interaction for new FW Upgrade Request.
 *
 */
PRIVATE ALWAYS_INLINE bool CheckAndWaitForUpgradeAttemmp(void)
{
	/* TODO If there is no image return true */

	/* TODO Check external attemp like a pin */

	return (true);
}

/*
 * Checks whether image (firmware) is valid.
 *  Valid image is an image which signed with valid signature.
 *
 */
PRIVATE ALWAYS_INLINE bool IsValidImage(void)
{
	BLStatusCode statusCode = BL_Status_Success;

	/* Get Meta Data of Firmware */
	GetMetaData(&settings.firmwareInfo);

	if (settings.firmwareInfo->header.imageSize == 0xFFFFFFFF)
	{
		statusCode = BL_StatusUpgrade_NoFirmware;
		goto invalid_image;
	}

	/* Check whether image is valid */
	statusCode = BL_ValidateImage(settings.firmwareInfo);

	if (BL_Status_Success != statusCode)
	{
		goto invalid_image;
	}

	return true;

invalid_image :
    DEBUG_PRINT(DEBUG_LEVEL_ERROR, "\nBL Err:%d", statusCode);
	return false;
}

/*
 * Initializes HW
 *
 */
PRIVATE ALWAYS_INLINE int32_t InitializeHW(void)
{
    extern void SystemInit(void);
    extern void SystemCoreClockUpdate(void);

    /*
     * SystemInit function was already called in startup.s file and it is
     * moved here to call it explicitly
     */
    SystemInit();

	/* Initialize Drivers */
	Drv_UART_Init();

    return RESULT_SUCCESS;
}

/***************************** PUBLIC FUNCTIONS *******************************/
/*
 * Bootloader application entry point
 *
 */
int main(void)
{
	bool upgradeFW = false;
	bool validImage = false;

    /* Initialize HW First */
    InitializeHW();

    /* Initialize Bootloader Security */
    BL_SecurityInit();

    do
    {
        /* Check new image attempt */
        upgradeFW = CheckAndWaitForUpgradeAttemmp();
        if (true == upgradeFW)
        {
			BL_UpgradeFirmware();
        }

        /* Check Whether Firmware is valid (signed) */
        validImage = IsValidImage();

		/* TODO Sleep in case of fail */

        /* Try until have a valid image */
    } while (false == validImage);

    /*
     * Firmware is a validated image so just jump to firmware.
     */
    BL_JumpToFirmware((uint32_t)settings.firmwareInfo->image);

    return 0;
}
