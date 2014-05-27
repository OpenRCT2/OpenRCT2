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

#ifndef _SCENARIO_H_
#define _SCENARIO_H_

#include "rct2.h"
#include "object.h"

/**
 * SV6/SC6 header chunk
 * size: 0x20
 */
typedef struct {
	uint16 type;				// 0x00
	uint16 num_packed_objects;	// 0x02
	uint32 version;				// 0x04
	uint32 magic_number;		// 0x08
	uint8 pad_0C[0x14];
} rct_s6_header;

/**
 * SC6 information chunk
 * size: 0x198
 */
typedef struct {
	uint8 var_000;
	uint8 category;				// 0x01
	uint8 objective_type;		// 0x02
	uint8 objective_arg_1;		// 0x03
	sint32 objective_arg_2;		// 0x04
	sint16 objective_arg_3;		// 0x08
	uint8 pad_00A[0x3E];
	char name[64];				// 0x48
	char details[256];			// 0x88
	rct_object_entry entry;		// 0x188
} rct_s6_info;

/*
 * Scenario scores file header.
 * size: 0x10
 */
typedef struct {
	uint32 var_0;
	uint32 var_4;
	uint32 var_8;
	uint32 scenario_count;		// 0x0C
} rct_scenario_scores_header;

/**
 * Scenario basic structure, mainly for scenario select
 * size: 0x02B0
 */
typedef struct {
	char path[256];				// 0x0000
	uint8 category;				// 0x0100
	uint8 pad_0101[0x1F];
	sint8 objective_type;		// 0x0120
	sint8 objective_arg_1;		// 0x0121
	sint32 objective_arg_2;		// 0x0122
	sint16 objective_arg_3;		// 0x0126
	char name[64];				// 0x0128
	char details[256];			// 0x0168
	sint32 flags;				// 0x0268
	uint32 company_value;		// 0x026C
	char completed_by[64];		// 0x0270
} rct_scenario_basic;

enum {
	SCENARIO_FLAGS_VISIBLE = (1 << 0),
	SCENARIO_FLAGS_COMPLETED = (1 << 1),
	SCENARIO_FLAGS_SIXFLAGS = (1 << 2)
};

enum {
	S6_TYPE_SAVEDGAME,
	S6_TYPE_SCENARIO
};

enum {
	SCENARIO_CATEGORY_BEGINNER,
	SCENARIO_CATEGORY_CHALLENGING,
	SCENARIO_CATEGORY_EXPERT,
	SCENARIO_CATEGORY_REAL,
	SCENARIO_CATEGORY_BUILDYOUROWN
};

enum {
	OBJECTIVE_NONE,
	OBJECTIVE_GUESTS_BY,
	OBJECTIVE_PARK_VALUE_BY,
	OBJECTIVE_HAVE_FUN,
	OBJECTIVE_BUILD_THE_BEST,
	OBJECTIVE_10_ROLLERCOASTERS,
	OBJECTIVE_GUESTS_AND_RATING,
	OBJECTIVE_MONTHLY_RIDE_INCOME,
	OBJECTIVE_10_ROLLERCOASTERS_LENGTH,
	OBJECTIVE_FINISH_5_ROLLERCOASTERS,
	OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE,
	OBJECTIVE_MONTHLY_FOOD_INCOME
};

// Scenario list
extern int gScenarioListCount;
extern int gScenarioListCapacity;
extern rct_scenario_basic *gScenarioList;

int scenario_scores_save();
void scenario_load_list();
int scenario_load_basic(const char *path);
void scenario_load(const char *path);
void scenario_load_and_play(const rct_scenario_basic *scenario);
void scenario_update();
int scenario_rand();

#endif
