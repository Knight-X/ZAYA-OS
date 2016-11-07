/*******************************************************************************
 *
 * @file Debug.h
 *
 * @author Murat Cakmak
 *
 * @brief Debugging Interface
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
