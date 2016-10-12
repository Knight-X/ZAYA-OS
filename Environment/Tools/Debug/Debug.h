/*******************************************************************************
 *
 * @file Debug.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief Debugging Interface
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

#ifndef __DEBUG_H
#define __DEBUG_H

/********************************* INCLUDES ***********************************/

/***************************** MACRO DEFINITIONS ******************************/
#define DEBUG_LEVEL_INFO				1
#define DEBUG_LEVEL_WARNING				2
#define DEBUG_LEVEL_ERROR				3

/* TODO : Define a output for that macro */
#define DEBUG_PRINT(level, message, ...)

#if ENABLE_DEBUG_ASSERT

	#ifdef WIN32

		#include <assert.h>

		/* Breaks execution in case of fail */
		#define DEBUG_ASSERT(condition) { assert(condition); }

		/* Breaks execution and print message in case of fail */
		#define DEBUG_ASSERT_MESSAGE(condition, message) { DEBUG_MESSAGE(message); assert(condition); }

	#else /* WIN32 */

		#define DEBUG_ASSERT(condition)
		#define DEBUG_ASSERT_MESSAGE(condition, message)

	#endif /* WIN32 */

#else
	#define DEBUG_ASSERT(condition)
	#define DEBUG_ASSERT_MESSAGE(condition, message)
#endif	/* ENABLE_DEBUG_ASSERT */
/***************************** TYPE DEFINITIONS *******************************/

/*************************** FUNCTION DEFINITIONS *****************************/

#endif	/* __DEBUG_H */
