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

#include "addresses.h"

#if defined(__GNUC__)
    #ifdef __clang__
        #define DISABLE_OPT __attribute__((noinline,optnone))
    #else
        #define DISABLE_OPT __attribute__((noinline,optimize("O0")))
    #endif // __clang__
#else
#define DISABLE_OPT
#endif // defined(__GNUC__)

// This variable serves a purpose of identifying a crash if it has happened inside original code.
// When switching to original code, stack frame pointer is modified and prevents breakpad from providing stack trace.
volatile sint32 _originalAddress = 0;

sint32 DISABLE_OPT RCT2_CALLPROC_X(sint32 address, sint32 _eax, sint32 _ebx, sint32 _ecx, sint32 _edx, sint32 _esi, sint32 _edi, sint32 _ebp)
{
    sint32 result = 0;
    _originalAddress = address;
#if defined(PLATFORM_X86) && !defined(NO_RCT2)
    #ifdef _MSC_VER
    __asm {
        push ebp
        push address
        mov eax, _eax
        mov ebx, _ebx
        mov ecx, _ecx
        mov edx, _edx
        mov esi, _esi
        mov edi, _edi
        mov ebp, _ebp
        call [esp]
        lahf
        pop ebp
        pop ebp
        /* Load result with flags */
        mov result, eax
    }
    #else
    __asm__ volatile ( "\
        \n\
        push %%ebx \n\
        push %%ebp \n\
        push %[address]     \n\
        mov %[eax], %%eax   \n\
        mov %[ebx], %%ebx   \n\
        mov %[ecx], %%ecx   \n\
        mov %[edx], %%edx   \n\
        mov %[esi], %%esi   \n\
        mov %[edi], %%edi   \n\
        mov %[ebp], %%ebp   \n\
        call *(%%esp)   \n\
        lahf \n\
        add $4, %%esp   \n\
        pop %%ebp \n\
        pop %%ebx \n\
        /* Load result with flags */ \n\
        mov %%eax, %[result] \n\
        " : [address] "+m" (address), [eax] "+m" (_eax), [ebx] "+m" (_ebx), [ecx] "+m" (_ecx), [edx] "+m" (_edx), [esi] "+m" (_esi), [edi] "+m" (_edi), [ebp] "+m" (_ebp), [result] "+m" (result)
        :
        : "eax","ecx","edx","esi","edi","memory"
    );
    #endif
#endif // PLATFORM_X86
    _originalAddress = 0;
    // lahf only modifies ah, zero out the rest
    return result & 0xFF00;
}
