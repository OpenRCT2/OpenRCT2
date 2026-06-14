/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2::Editor
{
    enum class Step : uint8_t
    {
        objectSelection,       // 0
        landscapeEditor,       // 1
        inventionsListSetUp,   // 2
        optionsSelection,      // 3
        objectiveSelection,    // 4
        scenarioDetails,       // 5
        saveScenario,          // 6
        rollerCoasterDesigner, // 7
        designsManager,        // 8
        invalid = 255,         // 255
    };
}
