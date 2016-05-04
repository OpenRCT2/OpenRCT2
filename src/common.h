#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#ifndef _COMMON_H_
#define _COMMON_H_

#include "diagnostic.h"
#include "rct2.h"

#define SafeFree(x) do { free(x); (x) = NULL; } while (0)

#define SafeDelete(x) do { delete (x); (x) = nullptr; } while (0)
#define SafeDeleteArray(x) do { delete[] (x); (x) = nullptr; } while (0)

#ifndef interface
	#define interface struct
#endif
#define abstract = 0

#if defined(__i386__) || defined(_M_IX86)
#define PLATFORM_X86
#endif

#ifdef PLATFORM_X86
#ifdef __GNUC__
#define FASTCALL __attribute__((fastcall))
#elif _MSC_VER
#define FASTCALL __fastcall
#else
#pragma message "Not using fastcall calling convention, please check your compiler support"
#define FASTCALL
#endif
#else // PLATFORM_X86
#define FASTCALL
#endif // PLATFORM_X86

#endif
