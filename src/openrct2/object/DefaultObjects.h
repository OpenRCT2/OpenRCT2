/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <array>
#include <cstdint>
#include <string_view>

using ObjectEntryIndex = uint16_t;

namespace OpenRCT2
{
    struct IObjectManager;

    /**
     * Used by Scenario Editor, Track Designer and Track Designs Manager.
     */
    extern const std::array<std::string_view, 3> kMinimumRequiredObjects;

    /**
     * Used by Track Designer and Track Designs Manager.
     */
    extern const std::array<std::string_view, 3> kMinimumRequiredObjectsDesigner;

    /**
     * Used by the Scenario Editor and Track Designer.
     */
    extern const std::array<std::string_view, 82> kCommonScenarioAndTrackDesignerObjects;

    /**
     * Used only by the Scenario Editor.
     */
    extern const std::array<std::string_view, 37> kDefaultScenarioObjects;

    /**
     * Used to distinguish official campaigns from user-created ones. Also determines the order of official campaigns in the
     * Scenario UI.
     */
    extern const std::array<std::string_view, 12> kDefaultCampaignObjects;

    ObjectEntryIndex GetDefaultStationObject(IObjectManager& objectManager);
} // namespace OpenRCT2
