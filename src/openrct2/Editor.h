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

enum RCT2_EDITOR_STEP
{
    EDITOR_STEP_OBJECT_SELECTION,       // 0
    EDITOR_STEP_LANDSCAPE_EDITOR,       // 1
    EDITOR_STEP_INVENTIONS_LIST_SET_UP, // 2
    EDITOR_STEP_OPTIONS_SELECTION,      // 3
    EDITOR_STEP_OBJECTIVE_SELECTION,    // 4
    EDITOR_STEP_SAVE_SCENARIO,          // 5
    EDITOR_STEP_ROLLERCOASTER_DESIGNER, // 6
    EDITOR_STEP_TRACK_DESIGNS_MANAGER   // 7
};

void editor_open_windows_for_current_step();

#endif
