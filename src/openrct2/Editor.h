/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"
#include "object/Object.h"

struct ResultWithMessage;

namespace Editor
{
    void Load();
    void ConvertSaveToScenario();
    void LoadTrackDesigner();
    void LoadTrackManager();
    bool LoadLandscape(const utf8* path);

    ResultWithMessage CheckPark();
    std::pair<ObjectType, rct_string_id> CheckObjectSelection();

    void OpenWindowsForCurrentStep();

    uint8_t GetSelectedObjectFlags(ObjectType objectType, size_t index);
    void ClearSelectedObject(ObjectType objectType, size_t index, uint32_t flags);
    void SetSelectedObject(ObjectType objectType, size_t index, uint32_t flags);
} // namespace Editor

enum class EditorStep : uint8_t
{
    ObjectSelection,       // 0
    LandscapeEditor,       // 1
    InventionsListSetUp,   // 2
    OptionsSelection,      // 3
    ObjectiveSelection,    // 4
    SaveScenario,          // 5
    RollercoasterDesigner, // 6
    DesignsManager,        // 7
    Invalid = 255,         // 255
};

extern EditorStep gEditorStep;

void editor_open_windows_for_current_step();
