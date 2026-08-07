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

    enum WALL_SCENERY_2_FLAGS
    {
        WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR = (1 << 0), // 0x1
        WALL_SCENERY_2_DOOR_SOUND_MASK = 0b0110,
        WALL_SCENERY_2_DOOR_SOUND_SHIFT = 1,
        WALL_SCENERY_2_IS_TRANSPARENT = (1 << 3), // 0x8
        WALL_SCENERY_2_ANIMATED = (1 << 4),       // 0x10
    };

    struct WallSceneryEntry
    {
        static constexpr auto kObjectType = ObjectType::walls;

        StringId name;
        uint32_t image;
        CursorID tool_id;
        WallSceneryFlags flags;
        uint8_t height;
        uint8_t flags2;
        money64 price;
        ObjectEntryIndex scenery_tab_id;
        uint8_t scrolling_mode;

        Audio::DoorSoundType getDoorSoundType() const;
    };
} // namespace OpenRCT2
