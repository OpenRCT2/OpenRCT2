/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../audio/Audio.h"
#include "../core/FlagHolder.hpp"
#include "../core/Money.hpp"
#include "../localisation/StringIdType.h"
#include "ObjectTypes.h"

enum class CursorID : uint8_t;

namespace OpenRCT2
{
    enum class WallSceneryFlag : uint8_t
    {
        hasPrimaryColour,
        hasGlass,
        cannotBuildOnSlope,
        isDoubleSided,
        isDoor,
        hasLongDoorAnimation,
        hasSecondaryColour,
        hasTertiaryColour,
    };
    using WallSceneryFlags = FlagHolder<uint8_t, WallSceneryFlag>;

    enum class WallSceneryFlag2 : uint8_t
    {
        disablePrimaryColour,
        // bits 1 and 2 were used for the door sound mask, now moved to a separate field.
        isTransparent = 3,
        isAnimated = 4,
    };
    using WallSceneryFlags2 = FlagHolder<uint8_t, WallSceneryFlag2>;

    struct WallSceneryEntry
    {
        static constexpr auto kObjectType = ObjectType::walls;

        StringId name;
        uint32_t image;
        CursorID tool_id;
        WallSceneryFlags flags;
        uint8_t height;
        WallSceneryFlags2 flags2;
        Audio::DoorSoundType doorSound;
        money64 price;
        ObjectEntryIndex scenery_tab_id;
        uint8_t scrolling_mode;
    };
} // namespace OpenRCT2
