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
#else
#define NO_RCT2 1
#endif

#if defined(__LP64__) || defined(_WIN64)
	#define PLATFORM_64BIT
#else
	#define PLATFORM_32BIT
#endif

// C99's restrict keywords guarantees the pointer in question, for the whole of its lifetime,
// will be the only way to access a given memory region. In other words: there is no other pointer
// aliasing the same memory area. Using it lets compiler generate better code. If your compiler
// does not support it, feel free to drop it, at some performance hit.
#ifdef __cplusplus
	#ifdef _MSC_VER
		#define RESTRICT __restrict
	#else
		#define RESTRICT __restrict__
	#endif
#else
	#ifdef _MSC_VER
		#define RESTRICT __restrict
	#else
		#define RESTRICT restrict
	#endif
#endif
#ifndef RESTRICT
	#define RESTRICT
#endif

#ifdef __cplusplus
#define assert_struct_size(x, y) static_assert(sizeof(x) == (y), "Improper struct size")
#else
	// Visual Studio does not know _Static_assert
	#if !defined(_MSC_VER)
		#define assert_struct_size(x, y) _Static_assert(sizeof(x) == (y), "Improper struct size")
	#else
		#define assert_struct_size(x, y)
	#endif // !defined(_MSC_VER)
#endif

#ifdef PLATFORM_X86
	#ifndef FASTCALL
		#ifdef __GNUC__
			#define FASTCALL __attribute__((fastcall))
		#elif defined(_MSC_VER)
			#define FASTCALL __fastcall
		#else
			#pragma message "Not using fastcall calling convention, please check your compiler support"
			#define FASTCALL
		#endif
	#endif // FASTCALL
#else // PLATFORM_X86
	#define FASTCALL
#endif // PLATFORM_X86

/**
 * x86 register structure, only used for easy interop to RCT2 code.
 */
#pragma pack(push, 1)
typedef struct registers {
	union {
		int eax;
		short ax;
		struct {
			char al;
			char ah;
		};
	};
	union {
		int ebx;
		short bx;
		struct {
			char bl;
			char bh;
		};
	};
	union {
		int ecx;
		short cx;
		struct {
			char cl;
			char ch;
		};
	};
	union {
		int edx;
		short dx;
		struct {
			char dl;
			char dh;
		};
	};
	union {
		int esi;
		short si;
	};
	union {
		int edi;
		short di;
	};
	union {
		int ebp;
		short bp;
	};
} registers;
assert_struct_size(registers, 7 * 4);
#pragma pack(pop)

#endif
