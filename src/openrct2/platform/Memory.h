/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#if defined(__amd64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86)
    // Don't bother checking for CPUID, prefetch is available since Pentium 4
    #include <xmmintrin.h>
    // This cannot be expressed as `constexpr` function, exclude it from clang-tidy check
    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define PREFETCH(x) _mm_prefetch(reinterpret_cast<const char*>(x), _MM_HINT_T0)

#elif defined(_MSC_VER) && defined(_M_ARM64)
    // ARM64 prefetch is available since ARMv8.
    // MSVC's help (https://learn.microsoft.com/en-us/cpp/intrinsics/arm64-intrinsics?view=msvc-170)
    // is incorrect and points to a different header (arm64_neon.h), which doesn't contain prefetch.
    // Correct header info taken from
    // https://github.com/microsoft/wdkmetadata/blob/1ac0dd0719f19196334de12cf2a6dec20316d440/generation/WDK/IdlHeaders/km/crt/intrin.h#L2
    // and
    // https://github.com/microsoft/wdkmetadata/blob/1ac0dd0719f19196334de12cf2a6dec20316d440/generation/WDK/IdlHeaders/km/crt/intrin.h#L411
    #include <intrin.h>
    #define PREFETCH(x) __prefetch(x)

#elif defined(__GNUC__)
    // Let the compiler handle prefetch instruction
    #define PREFETCH(x) __builtin_prefetch(x)

#else
    #define PREFETCH(x)
#endif
