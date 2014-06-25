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

#include "addresses.h"
#include "game.h"
#include "peep.h"
#include "string_ids.h"
#include "sprite.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"

void window_peep_emptysub(){};

static rct_widget window_peep_widgets[] = { //Only first 4 fields done so far
	{ 1,	0,		0,		191,	0,		156,	0x0FFFFFFFF, STR_NONE },						// panel / background
	{ 20,	0,		1,		190,	1,		14,		865, 829},			// title bar
	{ 21,	0,		179,	189,	2,		13,		824, 828},			// close x button
	{ 2,	1,		1,		191,	43,		156,	0x0FFFFFFFF, STR_NONE },						// tab content panel
	{ 8, 	1,		3,		33,		17,		70,		STR_PAGE_1, STR_NONE },						// page dropdown
	{ 8, 	1, 		73,		64,		17,		69,		876, STR_NONE },						// page dropdown button
	{ 8, 	1,		65,		95,		17,		70,		0x0FFFFFFFF, STR_INFORMATION_TYPE_TIP },		// information type dropdown
	{ 8, 	1, 		96,		126,	17,		69,		876, STR_INFORMATION_TYPE_TIP },		// information type dropdown button
	{ 8,	1,		127,	157,	17,		69,		5192, STR_SHOW_GUESTS_ON_MAP_TIP },	// map
	{ 8,	1,		158,	188,	17,		43,		0x02000144E, STR_INDIVIDUAL_GUESTS_TIP },	// tab 1
	{ 12,	1,		3,		166,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 17,	1,		3,		166,	72,		326,	3,				STR_NONE },						// guest list
	{ 12,	1,		3,		166,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 9,	1,		167,	190,	72,		326,	3,				STR_NONE },	
	{ 9,	1,		167,	190,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 9,	1,		167,	190,	72,		326,	3,				STR_NONE },						// guest list
	{ 9,	1,		167,	190,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 26,	0,		0,		1,		72,		326,	3,				STR_NONE },
	{ -1,	-1,		-1,		20,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 0,	0,		829,	21,		72,		326,	3,				STR_NONE },						// guest list
	{ 0,	0,		828,	258,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ -1,	-1,		-1,		264,	72,		326,	3,				STR_NONE },	
	{ 0,	32,		1938,	264,	72,		326,	3,				STR_NONE },						// guest list
	{ 0,	32,		1940,	264,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 0,	32,		1941,	264,	72,		326,	3,				STR_NONE },	
	{ 0,	32,		1943,	264,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 0,	32,		1944,	26,		72,		326,	3,				STR_NONE },						// guest list
	{ 0,	32,		-1,		-1,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 156,	0,		865,	0,		72,		326,	3,				STR_NONE },
	{ 13,	0,		824,	0,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 14,	0,		-1,		-1,		72,		326,	3,				STR_NONE },						// guest list
	{ 43,	0,		5198,	8192,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 43,	0,		5198,	8192,	72,		326,	3,				STR_NONE },
	{ 43,	0,		5198,	8192,	72,		326,	3,				STR_NONE },
	{ 43,	0,		5198,	8192,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 43,	0,		5198,	8192,	72,		326,	3,				STR_NONE },						// guest list
	{ 43,	0,		5198,	8192,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 143,	0,		2,		0,		72,		326,	3,				STR_NONE },
	{ 191,	0,		0,		156,	72,		326,	3,				STR_NONE },						// guest list
	{ 190,	0,		1,		14,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 189,	0,		2,		13,		72,		326,	3,				STR_NONE },	
	{ 191,	0,		43,		156,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 33,	0,		17,		43,		72,		326,	3,				STR_NONE },						// guest list
	{ 64,	0,		17,		43,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 95,	0,		17,		43,		72,		326,	3,				STR_NONE },
	{ 126,	0,		17,		43,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 157,	0,		17,		43,		72,		326,	3,				STR_NONE },						// guest list
	{ 188,	0,		17,		43,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 1,	0,		0,		191,	72,		326,	3,				STR_NONE },	
	{ 20,	0,		1,		190,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 21,	0,		179,	189,	72,		326,	3,				STR_NONE },
	{ 2,	1,		0,		191,	72,		326,	3,				STR_NONE },
	{ 8,	1,		3,		33,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 8,	1,		34,		64,		72,		326,	3,				STR_NONE },						// guest list
	{ 8,	1,		65,		95,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 8,	1,		96,		126,	72,		326,	3,				STR_NONE },
	{ 8,	1,		127,	157,	72,		326,	3,				STR_NONE },						// guest list
	{ 8,	1,		158,	188,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 26,	0,		0,		1,		72,		326,	3,				STR_NONE },	
	{ -1,	-1,		-1,		20,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 0,	0,		829,	21,		72,		326,	3,				STR_NONE },						// guest list
	{ 0,	0,		828,	258,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ -1,	-1,		-1,		264,	72,		326,	3,				STR_NONE },
	{ 0,	32,		1938,	264,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 0,	32,		1940,	264,	72,		326,	3,				STR_NONE },						// guest list
	{ 0,	32,		1941,	264,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 0,	32,		1942,	264,	72,		326,	3,				STR_NONE },
	{ 0,	32,		1943,	264,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 0,	32,		1944,	26,		72,		326,	3,				STR_NONE },
	{ 73,	1,		-1,		-1,		72,		326,	3,				STR_NONE },
	{ 14,	0,		1463,	0,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 15,	0,		824,	0,		72,		326,	3,				STR_NONE },						// guest list
	{ 73,	1,		-1,		-1,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 70,	0,		3440,	0,		72,		326,	3,				STR_NONE },	
	{ 69,	0,		876,	0,		72,		326,	3,				STR_NONE },						// guest list
	{ 70,	0,		-1,		-1,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 69,	0,		876,	0,		72,		326,	3,				STR_NONE },	
	{ 69,	0,		5192,	0,		17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 43,	0,		5198,	8192,	72,		326,	3,				STR_NONE },						// guest list
	{ 43,	0,		5198,	8192,	17,		43,		0x02000144E,	STR_SUMMARISED_GUESTS_TIP },	// tab 2
	{ 70,	1,		3,		0,		72,		326,	3,				STR_NONE },
	{ WIDGETS_END },
};

static void* window_guest_list_events[] = {
	0x696A75,
	0x696A06,
	0x696FBE,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	0x696F45,
	window_peep_emptysub,
	window_peep_emptysub,
	0x696A5F,
	0x696A54,
	window_peep_emptysub,
	window_peep_emptysub,
	0x696A49,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	0x696A6A,
	0x697076,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	0x696749,
	0x696887,
	0x69707C
};

/**
 * rct2: 0x006989E9
 *
 */
void window_peep_open(rct_peep* peep){
	
	if (peep->type == PEEP_TYPE_STAFF){
		RCT2_CALLPROC_X(0x006989E9, 0, 0, 0, (int)peep, 0, 0, 0);
	}

	rct_window* window;

	window = window_bring_to_front_by_id(WC_PEEP, peep->sprite_index);
	if (window == NULL){
		window = window_create_auto_pos(192, 157, (uint32*)window_guest_list_events, WC_PEEP, 0);
		window->widgets = window_peep_widgets;
	}
	RCT2_CALLPROC_X(0x006989E9, 0, 0, 0, (int)peep, 0, 0, 0);
	
}
