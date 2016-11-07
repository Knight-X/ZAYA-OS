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
#include "ProjectConfig.h"

/***************************** MACRO DEFINITIONS ******************************/
/*
 * Default Debug Level. Disabled. 
 */
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 					DEBUG_LEVEL_DISABLE
#endif

#ifndef DEBUG_ENABLE_ASSERTS
#define DEBUG_ENABLE_ASSERTS			0
#endif


/*
 * Debug Output Types
 */
#define DEBUG_OUTPUT_UART				1
#define DEBUG_OUTPUT_UVISION			2

/*
 * Debug Levels
 */
#define DEBUG_LEVEL_DISABLED			0
#define DEBUG_LEVEL_ERROR				1
#define DEBUG_LEVEL_WARNING				2
#define DEBUG_LEVEL_INFO				3

/*
 * Debug Print Interface 
 */
#if DEBUG_OUTPUT == DEBUG_OUTPUT_UVISION
#define DEBUG_PRINTF(message, ...)   printf(message, ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(message, ...)
#endif

/*
 * As default does not define print out for info, warning and error outputs 
 */
#define DEBUG_PRINT_INFO(message, ...)
#define DEBUG_PRINT_WARNING(message, ...)
#define DEBUG_PRINT_ERROR(message, ...)

#if DEBUG_LEVEL >= DEBUG_LEVEL_ERROR

	#undef 	DEBUG_PRINT_ERROR
	/*
	 * Implement if Error Debug outputs are allowed
	 */
	#define DEBUG_PRINT_ERROR(message, ...) DEBUG_PRINTF(message, ##__VA_ARGS__)

#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_WARNING

	#undef  DEBUG_PRINT_WARNING
	/*
	 * Implement if Warning Debug outputs are allowed
	 */
	#define DEBUG_PRINT_WARNING(message, ...) 	DEBUG_PRINTF(message, ##__VA_ARGS__)

#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO

	#undef  DEBUG_PRINT_INFO
	/*
	 * Implement if Info Debug outputs are enabled
	 */
	#define DEBUG_PRINT_INFO(message, ...) 		DEBUG_PRINTF(message, ##__VA_ARGS__)

#endif

#if ENABLE_DEBUG_ASSERT

	#ifdef WIN32

		#include <assert.h>

		/* Breaks execution in case of fail */
		#define DEBUG_ASSERT(condition) { assert(condition); }

		/* Breaks execution and print message in case of fail */
		#define DEBUG_ASSERT_MESSAGE(condition, message) { DEBUG_PRINTF(message); assert(condition); }

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
