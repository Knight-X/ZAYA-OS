/*******************************************************************************
 *
 * @file postypes.h
 *
 * @author Murat Cakmak (MC)
 *
 * @brief ZAYA-OS Specific Type Definitions which extend standard types
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

#ifndef __POS_TYPES_H
#define __POS_TYPES_H

/********************************* INCLUDES ***********************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/***************************** MACRO DEFINITIONS ******************************/

#ifndef PUBLIC
#define PUBLIC
#endif /* PUBLIC */

#ifndef INTERNAL
#define INTERNAL
#endif /* INTERNAL */

#ifndef PRIVATE
#define PRIVATE static
#endif /* PRIVATE */

#ifndef RESULT_SUCCESS
#define RESULT_SUCCESS			(0)
#endif /* RESULT_SUCCESS */

#ifndef RESULT_FAIL
#define RESULT_FAIL				(-1)
#endif /* RESULT_FAIL */

#undef INLINE
#undef ALWAYS_INLINE
#undef ASSEMBLY_FUNCTION


#if defined(WIN32)

	#define INLINE                  __inline
	#define ALWAYS_INLINE           __forceinline
	#define ASSEMBLY_FUNCTION
	#define PACKED
    #define TYPEDEF_STRUCT_PACKED	typedef struct
    #define NO_INLINE

#elif defined(__ARMCC_VERSION)

	#define INLINE                  			__inline
	#define ALWAYS_INLINE           			__forceinline
	#define ASSEMBLY_FUNCTION       			__asm
	#define PACKED								__packed
    #define TYPEDEF_STRUCT_PACKED				PACKED typedef struct
    #define NO_INLINE               			__attribute__((noinline))
	#define LOCATE_AT(symbol, addr)				symbol __attribute__((section(".ARM.__at_" ##addr)))

#else /* GCC */

	/*
	 * TODO [IMP] When we run splint for code analysis, splint does not analyze
	 * for GCC (__GNUC__) as default so we can not use following compiler switch
	 * to differentiate GCC defines
	 * 		#if defined ( __GNUC__ )
	 * 		#endif
	 *
	 * 	So let's use else case to assume all other platforms (than WIN32, ARM) are
	 * 	GCC for now. But we need to specify GCC for splint explicitly.
	 */
	#define INLINE                  __inline__
    /*
     * Unit tests show all warnings which break tests so let's use inline
     * instead of always inline for gcc builds
     */
	#define ALWAYS_INLINE           __inline__   /* __attribute__((always_inline)) */
	#define ASSEMBLY_FUNCTION
	#define PACKED					__attribute__((packed))
    #define TYPEDEF_STRUCT_PACKED	typedef struct PACKED
    #define NO_INLINE

#endif

#ifndef ENDLESS_WHILE_LOOP
#define ENDLESS_WHILE_LOOP 				for (;;)
#endif

#ifndef SUPER_LOOP
#define SUPER_LOOP ENDLESS_WHILE_LOOP
#endif

#ifndef MATH_MIN
#define MATH_MIN(x, y)	(((x) < (y)) ? (x) : (y))
#endif

#ifndef MATH_MAX
#define MATH_MAX(x, y)	(((x) > (y)) ? (x) : (y))
#endif
/***************************** TYPE DEFINITIONS *******************************/
typedef volatile uint32_t reg32_t;
/*************************** FUNCTION DEFINITIONS *****************************/

#endif	/* __POS_TYPES_H */
