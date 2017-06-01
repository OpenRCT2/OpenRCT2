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

#pragma once

#include "common.h"

#define OPENRCT2_NAME               "OpenRCT2"
#define OPENRCT2_VERSION            "0.0.8"

#if defined(__amd64__) || defined(_M_AMD64)
    #define OPENRCT2_ARCHITECTURE       "x86-64"
#elif defined(__i386__) || defined(_M_IX86)
    #define OPENRCT2_ARCHITECTURE       "x86"
#elif defined(__aarch64__)
    #define OPENRCT2_ARCHITECTURE       "AArch64"
#elif defined(__arm__) || defined(_M_ARM)
    #define OPENRCT2_ARCHITECTURE       "ARMv7"
#elif defined(__powerpc__) || defined(_M_PPC)
    #define OPENRCT2_ARCHITECTURE       "PowerPC"
#endif

#ifndef OPENRCT2_ARCHITECTURE
    #error "OPENRCT2_ARCHITECTURE is undefined. Please add identification."
#endif

// Platform
#ifdef __WINDOWS__
    #define OPENRCT2_PLATFORM       "Windows"
#endif
#ifdef __LINUX__
    #define OPENRCT2_PLATFORM       "Linux"
#endif
#ifdef __MACOSX__
    #define OPENRCT2_PLATFORM       "macOS"
#endif
#ifdef __FREEBSD__
    #define OPENRCT2_PLATFORM       "FreeBSD"
#endif
#ifndef OPENRCT2_PLATFORM
    #error Unknown platform!
#endif

#define OPENRCT2_TIMESTAMP          __DATE__ " " __TIME__

#ifdef __cplusplus
extern "C"
{
#endif
    extern const char gVersionInfoFull[];
#ifdef __cplusplus
}
#endif
