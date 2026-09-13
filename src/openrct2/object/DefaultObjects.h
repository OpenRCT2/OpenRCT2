/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Object.h"

#include <array>

namespace OpenRCT2
{
    /**
     * Used by all editor modes: Scenario Editor, Track Designer and Track Designs Manager.
     */
    extern const std::array<std::string_view, 3> kMinimumRequiredObjects;

    /**
     * Used by the Scenario Editor and Track Designer.
     */
    extern const std::array<std::string_view, 82> kCommonScenarioAndTrackDesignerObjects;

    /**
     * Used only by the Scenario Editor.
     */
    extern const std::array<std::string_view, 37> kDefaultScenarioObjects;
} // namespace OpenRCT2
