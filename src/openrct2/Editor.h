/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
    bool LoadLandscape(const utf8* path);

    bool CheckPark();
    int32_t CheckObjectSelection();

    void OpenWindowsForCurrentStep();

    uint8_t GetSelectedObjectFlags(int32_t objectType, size_t index);
    void ClearSelectedObject(int32_t objectType, size_t index, uint32_t flags);
    void SetSelectedObject(int32_t objectType, size_t index, uint32_t flags);
} // namespace Editor

enum class RCT2EditorStep : uint8_t
{
    ObjectSelection,
    LandscapeEditor,
    InventionsListSetUp,
    OptionsSelection,
    ObjectiveSelection,
    SaveScenario,
    RollercoasterDesigner,
    TrackDesignsManager,
    None = 255
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

#endif
