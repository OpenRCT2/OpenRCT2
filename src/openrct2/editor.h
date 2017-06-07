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

#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "object.h"
#include "rct2.h"

typedef enum {
    EDITOR_STEP_OBJECT_SELECTION,           // 0
    EDITOR_STEP_LANDSCAPE_EDITOR,           // 1
    EDITOR_STEP_INVENTIONS_LIST_SET_UP,     // 2
    EDITOR_STEP_OPTIONS_SELECTION,          // 3
    EDITOR_STEP_OBJECTIVE_SELECTION,        // 4
    EDITOR_STEP_SAVE_SCENARIO,              // 5
    EDITOR_STEP_ROLLERCOASTER_DESIGNER,     // 6
    EDITOR_STEP_TRACK_DESIGNS_MANAGER       // 7
} RCT2_EDITOR_STEP;

enum
{
    EDIT_SCENARIOOPTIONS_SETNOMONEY,
    EDIT_SCENARIOOPTIONS_SETINITIALCASH,
    EDIT_SCENARIOOPTIONS_SETINITIALLOAN,
    EDIT_SCENARIOOPTIONS_SETMAXIMUMLOANSIZE,
    EDIT_SCENARIOOPTIONS_SETANNUALINTERESTRATE,
    EDIT_SCENARIOOPTIONS_SETFORBIDMARKETINGCAMPAIGNS,
    EDIT_SCENARIOOPTIONS_SETAVERAGECASHPERGUEST,
    EDIT_SCENARIOOPTIONS_SETGUESTINITIALHAPPINESS,
    EDIT_SCENARIOOPTIONS_SETGUESTINITIALHUNGER,
    EDIT_SCENARIOOPTIONS_SETGUESTINITIALTHIRST,
    EDIT_SCENARIOOPTIONS_SETGUESTSPREFERLESSINTENSERIDES,
    EDIT_SCENARIOOPTIONS_SETGUESTSPREFERMOREINTENSERIDES,
    EDIT_SCENARIOOPTIONS_SETCOSTTOBUYLAND,
    EDIT_SCENARIOOPTIONS_SETCOSTTOBUYCONSTRUCTIONRIGHTS,
    EDIT_SCENARIOOPTIONS_SETPARKCHARGEMETHOD,
    EDIT_SCENARIOOPTIONS_SETPARKCHARGEENTRYFEE,
    EDIT_SCENARIOOPTIONS_SETFORBIDTREEREMOVAL,
    EDIT_SCENARIOOPTIONS_SETFORBIDLANDSCAPECHANGES,
    EDIT_SCENARIOOPTIONS_SETFORBIDHIGHCONSTRUCTION,
    EDIT_SCENARIOOPTIONS_SETPARKRATINGHIGHERDIFFICULTLEVEL,
    EDIT_SCENARIOOPTIONS_SETGUESTGENERATIONHIGHERDIFFICULTLEVEL,
};

extern uint8 * gEditorSelectedObjects[OBJECT_ENTRY_GROUP_COUNT];

void editor_load();
void editor_convert_save_to_scenario();
void trackdesigner_load();
void trackmanager_load();
bool editor_load_landscape(const utf8 *path);

void editor_open_windows_for_current_step();

bool editor_check_park();
sint32 editor_check_object_selection();

bool editor_check_object_group_at_least_one_selected(sint32 objectType);

void game_command_edit_scenario_options(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);

#endif
