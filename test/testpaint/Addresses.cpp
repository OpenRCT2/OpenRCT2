/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Addresses.h"

#if defined(__GNUC__)
#    ifdef __clang__
#        define DISABLE_OPT __attribute__((noinline, optnone))
#    else
#        define DISABLE_OPT __attribute__((noinline, optimize("O0")))
#    endif // __clang__
#else
#    define DISABLE_OPT
#endif // defined(__GNUC__)

// This variable serves a purpose of identifying a crash if it has happened inside original code.
// When switching to original code, stack frame pointer is modified and prevents breakpad from providing stack trace.
volatile int32_t _originalAddress = 0;

int32_t DISABLE_OPT RCT2_CALLPROC_X(
    int32_t address, int32_t _eax, int32_t _ebx, int32_t _ecx, int32_t _edx, int32_t _esi, int32_t _edi, int32_t _ebp)
{
    int32_t result = 0;
    _originalAddress = address;
#if defined(PLATFORM_X86) && !defined(NO_RCT2)
#    ifdef _MSC_VER
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
#    else
    // clang-format off
    __asm__ volatile("\
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
        : "eax","ecx","edx","esi","edi","memory");
    // clang-format on
#    endif // _MSC_VER
#endif     // PLATFORM_X86
    _originalAddress = 0;
    // lahf only modifies ah, zero out the rest
    return result & 0xFF00;
}
