/*****************************************************************************
* Copyright (c) 2014 Ted John, Duncan Frost
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

#ifndef _CURSORS_H_
#define _CURSORS_H_

enum {
	CURSOR_ARROW = 0,
	CURSOR_BLANK = 1,
	CURSOR_UP_ARROW = 2,
	CURSOR_UP_DOWN_ARROW = 3,
	CURSOR_HAND_POINT = 4,
	CURSOR_ZZZ = 5,
	CURSOR_DIAGONAL_ARROWS = 6,
	CURSOR_PICKER = 7,
	CURSOR_TREE_DOWN = 8,
	CURSOR_FOUNTAIN_DOWN = 9,
	CURSOR_STATUE_DOWN = 10,
	CURSOR_BENCH_DOWN = 11,
	CURSOR_CROSS_HAIR = 12,
	CURSOR_BIN_DOWN = 13,
	CURSOR_LAMPPOST_DOWN = 14,
	CURSOR_FENCE_DOWN = 15,
	CURSOR_FLOWER_DOWN = 16,
	CURSOR_PATH_DOWN = 17,
	CURSOR_DIG_DOWN = 18,
	CURSOR_WATER_DOWN = 19,
	CURSOR_HOUSE_DOWN = 20,
	CURSOR_VOLCANO_DOWN = 21,
	CURSOR_WALK_DOWN = 22,
	CURSOR_PAINT_DOWN = 23,
	CURSOR_ENTRANCE_DOWN = 24,
	CURSOR_HAND_OPEN = 25,
	CURSOR_HAND_CLOSED = 26,
	CURSOR_COUNT
};

extern unsigned char blank_cursor_data[32 * 4];
extern unsigned char blank_cursor_mask[32 * 4];

#define BLANK_CURSOR_HOTX 0
#define BLANK_CURSOR_HOTY 0

extern unsigned char up_arrow_cursor_data[32 * 4];
extern unsigned char up_arrow_cursor_mask[32 * 4];

#define UP_ARROW_CURSOR_HOTX 15
#define UP_ARROW_CURSOR_HOTY 0

extern unsigned char up_down_arrow_cursor_data[32 * 4];
extern unsigned char up_down_arrow_cursor_mask[32 * 4];

#define UP_DOWN_ARROW_CURSOR_HOTX 7
#define UP_DOWN_ARROW_CURSOR_HOTY 31

extern unsigned char zzz_cursor_data[32 * 4];
extern unsigned char zzz_cursor_mask[32 * 4];
#define ZZZ_CURSOR_HOTX 0
#define ZZZ_CURSOR_HOTY 0

extern unsigned char diagonal_arrow_cursor_data[32 * 4];
extern unsigned char diagonal_arrow_cursor_mask[32 * 4];
#define DIAGONAL_ARROW_CURSOR_HOTX 0
#define DIAGONAL_ARROW_CURSOR_HOTY 0

extern unsigned char picker_cursor_data[32 * 4];
extern unsigned char picker_cursor_mask[32 * 4];
#define PICKER_CURSOR_HOTX 15
#define PICKER_CURSOR_HOTY 31

extern unsigned char tree_down_cursor_data[32 * 4];
extern unsigned char tree_down_cursor_mask[32 * 4];
#define TREE_DOWN_CURSOR_HOTX 7
#define TREE_DOWN_CURSOR_HOTY 31

extern unsigned char fountain_down_cursor_data[32 * 4];
extern unsigned char fountain_down_cursor_mask[32 * 4];
#define FOUNTAIN_DOWN_CURSOR_HOTX 7
#define FOUNTAIN_DOWN_CURSOR_HOTY 31

extern unsigned char statue_down_cursor_data[32 * 4];
extern unsigned char statue_down_cursor_mask[32 * 4];
#define STATUE_DOWN_CURSOR_HOTX 7
#define STATUE_DOWN_CURSOR_HOTY 31

extern unsigned char bench_down_cursor_data[32 * 4];
extern unsigned char bench_down_cursor_mask[32 * 4];
#define BENCH_DOWN_CURSOR_HOTX 7
#define BENCH_DOWN_CURSOR_HOTY 31

extern unsigned char cross_hair_cursor_data[32 * 4];
extern unsigned char cross_hair_cursor_mask[32 * 4];
#define CROSS_HAIR_CURSOR_HOTX 15
#define CROSS_HAIR_CURSOR_HOTY 15

extern unsigned char bin_down_cursor_data[32 * 4];
extern unsigned char bin_down_cursor_mask[32 * 4];
#define BIN_DOWN_CURSOR_HOTX 7
#define BIN_DOWN_CURSOR_HOTY 31

extern unsigned char lamppost_down_cursor_data[32 * 4];
extern unsigned char lamppost_down_cursor_mask[32 * 4];
#define LAMPPOST_DOWN_CURSOR_HOTX 7
#define LAMPPOST_DOWN_CURSOR_HOTY 31

extern unsigned char fence_down_cursor_data[32 * 4];
extern unsigned char fence_down_cursor_mask[32 * 4];
#define FENCE_DOWN_CURSOR_HOTX 7
#define FENCE_DOWN_CURSOR_HOTY 31

extern unsigned char flower_down_cursor_data[32 * 4];
extern unsigned char flower_down_cursor_mask[32 * 4];

#define FLOWER_DOWN_CURSOR_HOTX 7
#define FLOWER_DOWN_CURSOR_HOTY 31

extern unsigned char path_down_cursor_data[32 * 4];
extern unsigned char path_down_cursor_mask[32 * 4];
#define PATH_DOWN_CURSOR_HOTX 7
#define PATH_DOWN_CURSOR_HOTY 31

extern unsigned char dig_down_cursor_data[32 * 4];
extern unsigned char dig_down_cursor_mask[32 * 4];

#define DIG_DOWN_CURSOR_HOTX 7
#define DIG_DOWN_CURSOR_HOTY 31

extern unsigned char water_down_cursor_data[32 * 4];
extern unsigned char water_down_cursor_mask[32 * 4];
#define WATER_DOWN_CURSOR_HOTX 7
#define WATER_DOWN_CURSOR_HOTY 31

extern unsigned char house_down_cursor_data[32 * 4];
extern unsigned char house_down_cursor_mask[32 * 4];
#define HOUSE_DOWN_CURSOR_HOTX 7
#define HOUSE_DOWN_CURSOR_HOTY 31

extern unsigned char volcano_down_cursor_data[32 * 4];
extern unsigned char volcano_down_cursor_mask[32 * 4];
#define VOLCANO_DOWN_CURSOR_HOTX 7
#define VOLCANO_DOWN_CURSOR_HOTY 31

extern unsigned char walk_down_cursor_data[32 * 4];
extern unsigned char walk_down_cursor_mask[32 * 4];
#define WALK_DOWN_CURSOR_HOTX 7
#define WALK_DOWN_CURSOR_HOTY 31

extern unsigned char paint_down_cursor_data[32 * 4];
extern unsigned char paint_down_cursor_mask[32 * 4];
#define PAINT_DOWN_CURSOR_HOTX 8
#define PAINT_DOWN_CURSOR_HOTY 30

extern unsigned char entrance_down_cursor_data[32 * 4];
extern unsigned char entrance_down_cursor_mask[32 * 4];
#define ENTRANCE_DOWN_CURSOR_HOTX 7
#define ENTRANCE_DOWN_CURSOR_HOTY 31

extern unsigned char hand_open_cursor_data[32 * 4];
extern unsigned char hand_open_cursor_mask[32 * 4];
#define HAND_OPEN_CURSOR_HOTX 0
#define HAND_OPEN_CURSOR_HOTY 0

extern unsigned char hand_closed_cursor_data[32 * 4];
extern unsigned char hand_closed_cursor_mask[32 * 4];
#define HAND_CLOSED_CURSOR_HOTX 0
#define HAND_CLOSED_CURSOR_HOTY 0

#endif