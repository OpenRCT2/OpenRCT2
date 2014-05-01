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

#include <stddef.h>
#include <stdlib.h>

typedef signed char sint8;
typedef signed short sint16;
typedef signed long sint32;
typedef signed long long sint64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;

#define rol8(x, shift)		(((uint8)(x) << (shift)) | ((uint8)(x) >> (8 - (shift))))
#define ror8(x, shift)		(((uint8)(x) >> (shift)) | ((uint8)(x) << (8 - (shift))))
#define rol16(x, shift)		(((uint16)(x) << (shift)) | ((uint16)(x) >> (16 - (shift))))
#define ror16(x, shift)		(((uint16)(x) >> (shift)) | ((uint16)(x) << (16 - (shift))))
#define rol32(x, shift)		(((uint32)(x) << (shift)) | ((uint32)(x) >> (32 - (shift))))
#define ror32(x, shift)		(((uint32)(x) >> (shift)) | ((uint32)(x) << (32 - (shift))))
#define rol64(x, shift)		(((uint64)(x) << (shift)) | ((uint32)(x) >> (64 - (shift))))
#define ror64(x, shift)		(((uint64)(x) >> (shift)) | ((uint32)(x) << (64 - (shift))))
#define sgn(x)				((x > 0) ? 1 : ((x < 0) ? -1 : 0))
#define clamp(l, x, h)		(min(h, max(l, x)))

void rct2_finish();

enum {
	SCREEN_FLAGS_PLAYING = 0,
	SCREEN_FLAGS_TITLE_DEMO = 1,
	SCREEN_FLAGS_SCENARIO_EDITOR = 2,
	SCREEN_FLAGS_TRACK_DESIGNER = 4,
	SCREEN_FLAGS_TRACK_MANAGER = 8,
};

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
	PATH_ID_CSS10,
	PATH_ID_CSS11,
	PATH_ID_CSS12,
	PATH_ID_CSS13,
	PATH_ID_CSS14,
	PATH_ID_CSS15,
	PATH_ID_CSS16,
	PATH_ID_CSS3,
	PATH_ID_GAMECFG,
	PATH_ID_TUT640A,
	PATH_ID_TUT640B,
	PATH_ID_TUT640C,
	PATH_ID_TUT800A,
	PATH_ID_TUT800B,
	PATH_ID_TUT800C,
	PATH_ID_KANJI,
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
	PATH_ID_TRACKSIDX,
	PATH_ID_CSS41,
	PATH_ID_SIXFLAGS_MAGICMOUNTAIN,
	PATH_ID_SIXFLAGS_BUILDYOUROWN,
	PATH_ID_CSS42,
	PATH_ID_CSS43,
	PATH_ID_CSS44,
	PATH_ID_CSS45,
	PATH_ID_CSS46

};

enum {
	GAME_FLAGS_PARK_OPEN = (1 << 0),
    GAME_FLAGS_BELOW_TREE_HEIGHT_ONLY = (1 << 5),
	GAME_FLAGS_NO_MONEY = (1 << 11),
	GAME_FLAGS_PARK_FREE_ENTRY = (1 << 13)
};

void rct2_endupdate();
char *get_file_path(int pathId);
void get_system_info();
void get_system_time();
void *rct2_malloc(size_t numBytes);
void *rct2_realloc(void *block, size_t numBytes);
void rct2_free(void *block);

#endif
