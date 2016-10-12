/*******************************************************************************
 *
 * @file Drv_Flash.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Flash Driver Interface
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

#ifndef __DRV_FLASH_H
#define __DRV_FLASH_H

/********************************* INCLUDES ***********************************/
#include "postypes.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/
#define FLASH_STATUS_SUCCESS                (0)
#define FLASH_STATUS_BUSY                   (1)
#define FLASH_STATUS_FAILURE                (2)

/*************************** FUNCTION DEFINITIONS *****************************/
void Drv_Flash_Init(void);

int32_t Drv_Flash_PrepareBlock(uint32_t blockNo);
int32_t Drv_Flash_PrepareBlockRange(uint32_t startBlockNo, uint32_t endBlockNo);

int32_t Drv_Flash_EraseBlock(uint32_t blockNo);
int32_t Drv_Flash_EraseBlockRange(uint32_t startBlockNo, uint32_t endBlockNo);

int32_t Drv_Flash_Write(uint32_t address, uint8_t* data, uint32_t length);
int32_t Drv_Flash_WriteBlock(uint32_t blockNo, uint8_t* data, uint32_t length);

int32_t Drv_Flash_GetBlockNoOfAddress(uint32_t address);

uint32_t Drv_Flash_GetSize(void);
#endif	/* __DRV_FLASH_H */
