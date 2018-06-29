/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "common.h"

namespace Editor
{
    void Load();
    void ConvertSaveToScenario();
    void LoadTrackDesigner();
    void LoadTrackManager();
    bool LoadLandscape(const utf8 *path);

    bool CheckPark();
    int32_t CheckObjectSelection();

    void OpenWindowsForCurrentStep();
    void GameCommandEditScenarioOptions(int32_t*, int32_t*, int32_t*, int32_t*, int32_t*, int32_t*, int32_t*);

    uint8_t GetSelectedObjectFlags(int32_t objectType, size_t index);
    void ClearSelectedObject(int32_t objectType, size_t index, uint32_t flags);
    void SetSelectedObject(int32_t objectType, size_t index, uint32_t flags);
} // namespace Editor

enum RCT2_EDITOR_STEP
{
    EDITOR_STEP_OBJECT_SELECTION,           // 0
    EDITOR_STEP_LANDSCAPE_EDITOR,           // 1
    EDITOR_STEP_INVENTIONS_LIST_SET_UP,     // 2
    EDITOR_STEP_OPTIONS_SELECTION,          // 3
    EDITOR_STEP_OBJECTIVE_SELECTION,        // 4
    EDITOR_STEP_SAVE_SCENARIO,              // 5
    EDITOR_STEP_ROLLERCOASTER_DESIGNER,     // 6
    EDITOR_STEP_TRACK_DESIGNS_MANAGER       // 7
};

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

void editor_open_windows_for_current_step();

void game_command_edit_scenario_options(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);

#endif
