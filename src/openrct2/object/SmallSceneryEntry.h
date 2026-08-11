/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"
#include "../core/Money.hpp"
#include "../localisation/StringIdType.h"
#include "ObjectTypes.h"

enum class CursorID : uint8_t;

namespace OpenRCT2
{
    enum class SmallSceneryFlag : uint8_t
    {
        occupiesFullTile,
        vOffsetCentre,
        requiresFlatSurface,
        // User can choose rotation when set, otherwise rotation is automatic
        isRotatable,
        isAnimated,
        canWither,
        canBeWatered,
        hasOverlayImage,
        isDiagonal,
        hasGlass,
        hasPrimaryColour,
        isFountain,
        isCupidFountain,
        isClock,
        isSwampGoo,
        hasFrameOffsets,
        flag17,
        // Means item can be placed in the air and over water
        isStackable,
        prohibitWalls,
        hasSecondaryColour,
        hasNoSupports,
        isVisibleWhenZoomed,
        isCogwheel,
        // Means supports can be built on this object. Used for base blocks.
        allowSupportsAbove,
        occupiesHalfTile,
        occupiesThreeQuarters,
        // Used for scenery items which are support structures
        supportsHavePrimaryColour,
        flag27,

        // Added by OpenRCT2:
        isTree,
        hasTertiaryColour,
    };
    using SmallSceneryFlags = FlagHolder<uint32_t, SmallSceneryFlag>;

    struct SmallSceneryEntry
    {
        static constexpr auto kObjectType = ObjectType::smallScenery;

        StringId name;
        uint32_t image;
        SmallSceneryFlags flags;
        uint8_t height;
        CursorID tool_id;
        money64 price;
        money64 removal_price;
        uint8_t* frame_offsets;
        uint16_t FrameOffsetCount;
        uint16_t animation_delay;
        uint16_t animation_mask;
        uint16_t num_frames;
        ObjectEntryIndex scenery_tab_id;
    };
} // namespace OpenRCT2
