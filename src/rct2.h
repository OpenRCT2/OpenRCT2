/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef _RCT2_H_
#define _RCT2_H_

#include <SDL_platform.h>

#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES
#endif
#undef M_PI

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
	#include <time.h>
#endif

typedef int8_t sint8;
typedef int16_t sint16;
typedef int32_t sint32;
typedef int64_t sint64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef char utf8;
typedef utf8* utf8string;
typedef const utf8* const_utf8string;
typedef wchar_t utf16;
typedef utf16* utf16string;

typedef uint32 codepoint_t;
typedef uint8 colour_t;

#define rol8(x, shift)		(((uint8)(x) << (shift)) | ((uint8)(x) >> (8 - (shift))))
#define ror8(x, shift)		(((uint8)(x) >> (shift)) | ((uint8)(x) << (8 - (shift))))
#define rol16(x, shift)		(((uint16)(x) << (shift)) | ((uint16)(x) >> (16 - (shift))))
#define ror16(x, shift)		(((uint16)(x) >> (shift)) | ((uint16)(x) << (16 - (shift))))
#define rol32(x, shift)		(((uint32)(x) << (shift)) | ((uint32)(x) >> (32 - (shift))))
#define ror32(x, shift)		(((uint32)(x) >> (shift)) | ((uint32)(x) << (32 - (shift))))
#define rol64(x, shift)		(((uint64)(x) << (shift)) | ((uint32)(x) >> (64 - (shift))))
#define ror64(x, shift)		(((uint64)(x) >> (shift)) | ((uint32)(x) << (64 - (shift))))

#ifndef __cplusplus
// in C++ you should be using Math::Min and Math::Max
#ifndef min
	#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
	#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#endif // __cplusplus

#define sgn(x)				((x > 0) ? 1 : ((x < 0) ? -1 : 0))
#define clamp(l, x, h)		(min(h, max(l, x)))

// Rounds an integer down to the given power of 2. y must be a power of 2.
#define floor2(x, y)		((x) & (~((y) - 1)))


#ifndef __cplusplus
// in C++ you should be using Util::CountOf
#ifdef __GNUC__
/**
 * Force a compilation error if condition is true, but also produce a
 * result (of value 0 and type size_t), so the expression can be used
 * e.g. in a structure initializer (or where-ever else comma expressions
 * aren't permitted).
 */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))

/* &a[0] degrades to a pointer: a different type from an array */
#define __must_be_array(a) \
        BUILD_BUG_ON_ZERO(__builtin_types_compatible_p(typeof(a), typeof(&a[0])))

// based on http://lxr.free-electrons.com/source/include/linux/kernel.h#L54
#define countof(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))
#elif defined (_MSC_VER)
	#define countof(arr)			_countof(arr)
#else
	#define countof(arr)			(sizeof(arr) / sizeof((arr)[0]))
#endif // __GNUC__
#endif // __cplusplus

#ifndef _MSC_VER
// use similar struct packing as MSVC for our structs
#pragma pack(1)
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#define STUB() log_warning("Function %s at %s:%d is a stub.\n", __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define _strcmpi _stricmp
#define _stricmp(x, y) strcasecmp((x), (y))
#define _strnicmp(x, y, n) strncasecmp((x), (y), (n))
#define _strdup(x) strdup((x))

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define RCT2_ENDIANESS __ORDER_LITTLE_ENDIAN__
#define LOBYTE(w) ((uint8_t)(w))
#define HIBYTE(w) ((uint8_t)(((uint16_t)(w)>>8)&0xFF))
#endif // __BYTE_ORDER__

#ifndef RCT2_ENDIANESS
#error Unknown endianess!
#endif // RCT2_ENDIANESS

#endif // defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

#if !(_POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)
	char *strndup(const char *src, size_t size);
#endif // !(POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)

// BSD and OS X has MAP_ANON instead of MAP_ANONYMOUS
#ifndef MAP_ANONYMOUS
	#define MAP_ANONYMOUS MAP_ANON
#endif

#include "version.h"

#define OPENRCT2_MASTER_SERVER_URL	"https://servers.openrct2.website"

// Time (represented as number of 100-nanosecond intervals since 0001-01-01T00:00:00Z)
typedef uint64 datetime64;

#define DATETIME64_MIN ((datetime64)0)

// Represent fixed point numbers. dp = decimal point
typedef uint8 fixed8_1dp;
typedef uint8 fixed8_2dp;
typedef sint16 fixed16_1dp;
typedef sint16 fixed16_2dp;
typedef sint32 fixed32_1dp;
typedef sint32 fixed32_2dp;

// Money is stored as a multiple of 0.10.
typedef fixed8_1dp money8;
typedef fixed16_1dp money16;
typedef fixed32_1dp money32;

// Construct a fixed point number. For example, to create the value 3.65 you
// would write FIXED_2DP(3,65)
#define FIXED_XDP(x, whole, fraction)	((whole) * (10 * x) + (fraction))
#define FIXED_1DP(whole, fraction)		FIXED_XDP(1, whole, fraction)
#define FIXED_2DP(whole, fraction)		FIXED_XDP(10, whole, fraction)

// Construct a money value in the format MONEY(10,70) to represent 10.70. Fractional part must be two digits.
#define MONEY(whole, fraction)			((whole) * 10 + ((fraction) / 10))

#define MONEY_FREE						MONEY(0,00)
#define MONEY32_UNDEFINED				((money32)0x80000000)

typedef void (EMPTY_ARGS_VOID_POINTER)();
typedef unsigned short rct_string_id;

typedef struct {
	uint32 installLevel;
	char title[260];
	char path[260];
	uint32 var_20C;
	uint8 pad_210[256];
	char expansionPackNames[16][128];
	uint32 activeExpansionPacks;		//0xB10
} rct2_install_info;

enum {
	// Although this is labeled a flag it actually means when
	// zero the screen is in playing mode.
	SCREEN_FLAGS_PLAYING = 0,
	SCREEN_FLAGS_TITLE_DEMO = 1,
	SCREEN_FLAGS_SCENARIO_EDITOR = 2,
	SCREEN_FLAGS_TRACK_DESIGNER = 4,
	SCREEN_FLAGS_TRACK_MANAGER = 8,
};

#define SCREEN_FLAGS_EDITOR (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)

enum {
	PATH_ID_G1,
	PATH_ID_PLUGIN,
	PATH_ID_CSS1,
	PATH_ID_CSS2,
	PATH_ID_CSS4,
	PATH_ID_CSS5,
	PATH_ID_CSS6,
	PATH_ID_CSS7,
	PATH_ID_CSS8,
	PATH_ID_CSS9,
	PATH_ID_CSS11,
	PATH_ID_CSS12,
	PATH_ID_CSS13,
	PATH_ID_CSS14,
	PATH_ID_CSS15,
	PATH_ID_CSS3,
	PATH_ID_CSS17,
	PATH_ID_CSS18,
	PATH_ID_CSS19,
	PATH_ID_CSS20,
	PATH_ID_CSS21,
	PATH_ID_CSS22,
	PATH_ID_SCORES,
	PATH_ID_CSS23,
	PATH_ID_CSS24,
	PATH_ID_CSS25,
	PATH_ID_CSS26,
	PATH_ID_CSS27,
	PATH_ID_CSS28,
	PATH_ID_CSS29,
	PATH_ID_CSS30,
	PATH_ID_CSS31,
	PATH_ID_CSS32,
	PATH_ID_CSS33,
	PATH_ID_CSS34,
	PATH_ID_CSS35,
	PATH_ID_CSS36,
	PATH_ID_CSS37,
	PATH_ID_CSS38,
	PATH_ID_CUSTOM1,
	PATH_ID_CUSTOM2,
	PATH_ID_CSS39,
	PATH_ID_CSS40,
	PATH_ID_CSS41,
	PATH_ID_SIXFLAGS_MAGICMOUNTAIN,
	PATH_ID_CSS42,
	PATH_ID_CSS43,
	PATH_ID_CSS44,
	PATH_ID_CSS45,
	PATH_ID_CSS46,
	PATH_ID_CSS50,
	PATH_ID_END
};

extern const char * const RCT2FilePaths[PATH_ID_END];

extern uint32 gCurrentDrawCount;

int rct2_init();
void rct2_dispose();
void rct2_update();
void rct2_draw();
void substitute_path(char *dest, const char *path, const char *filename);
int check_mutex();
int check_file_paths();
int check_file_path(int pathId);
int check_files_integrity();
const char *get_file_path(int pathId);
void get_system_info();
void get_system_time();
void get_local_time();
void rct2_quit();

int rct2_open_file(const char *path);

#endif
