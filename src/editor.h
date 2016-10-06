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

#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "object.h"
#include "rct2.h"

typedef enum {
	EDITOR_STEP_OBJECT_SELECTION,			// 0
	EDITOR_STEP_LANDSCAPE_EDITOR,			// 1
	EDITOR_STEP_INVENTIONS_LIST_SET_UP,		// 2
	EDITOR_STEP_OPTIONS_SELECTION,			// 3
	EDITOR_STEP_OBJECTIVE_SELECTION,		// 4
	EDITOR_STEP_SAVE_SCENARIO,				// 5
	EDITOR_STEP_ROLLERCOASTER_DESIGNER,		// 6
	EDITOR_STEP_TRACK_DESIGNS_MANAGER		// 7
} RCT2_EDITOR_STEP;

extern uint8 * gEditorSelectedObjects[OBJECT_ENTRY_GROUP_COUNT];

void editor_load();
void editor_convert_save_to_scenario();
void trackdesigner_load();
void trackmanager_load();
bool editor_load_landscape(const utf8 *path);

void editor_open_windows_for_current_step();

bool editor_check_park();
int editor_check_object_selection();

bool editor_check_object_group_at_least_one_selected(int objectType);

#endif
