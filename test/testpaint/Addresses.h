#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _ADDRESSES_H_
#define _ADDRESSES_H_

#include <openrct2/common.h>

#ifdef USE_MMAP
    #if defined(PLATFORM_64BIT)
        #define GOOD_PLACE_FOR_DATA_SEGMENT ((uintptr_t)0x200000000)
    #elif defined(PLATFORM_32BIT)
        #define GOOD_PLACE_FOR_DATA_SEGMENT ((uintptr_t)0x09000000)
    #else
        #error "Unknown platform"
    #endif
#else
    #define GOOD_PLACE_FOR_DATA_SEGMENT ((uintptr_t)0x8a4000)
#endif

#define RCT2_ADDRESS(address, type)             ((type*)(GOOD_PLACE_FOR_DATA_SEGMENT - 0x8a4000 + (address)))
#define RCT2_GLOBAL(address, type)              (*((type*)(GOOD_PLACE_FOR_DATA_SEGMENT - 0x8a4000 + (address))))

/**
 * Returns the flags register
 *
 * Flags register is as follows:
 * 0bSZ0A_0P0C_0000_0000
 * S = Signed flag
 * Z = Zero flag
 * C = Carry flag
 * A = Adjust flag
 * P = Parity flag
 * All other bits are undefined.
 */
sint32 RCT2_CALLPROC_X(sint32 address, sint32 _eax, sint32 _ebx, sint32 _ecx, sint32 _edx, sint32 _esi, sint32 _edi, sint32 _ebp);



#endif
