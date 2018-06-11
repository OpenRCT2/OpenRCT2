/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

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
