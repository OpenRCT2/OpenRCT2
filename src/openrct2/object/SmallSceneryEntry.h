/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../interface/Cursors.h"
#include "ObjectTypes.h"

enum SMALL_SCENERY_FLAGS : uint32_t
{
    SMALL_SCENERY_FLAG_FULL_TILE = (1 << 0),            // 0x1
    SMALL_SCENERY_FLAG_VOFFSET_CENTRE = (1 << 1),       // 0x2
    SMALL_SCENERY_FLAG_REQUIRE_FLAT_SURFACE = (1 << 2), // 0x4
    SMALL_SCENERY_FLAG_ROTATABLE = (1 << 3),            // 0x8; when set, user can set rotation, otherwise rotation is automatic
    SMALL_SCENERY_FLAG_ANIMATED = (1 << 4),             // 0x10
    SMALL_SCENERY_FLAG_CAN_WITHER = (1 << 5),           // 0x20
    SMALL_SCENERY_FLAG_CAN_BE_WATERED = (1 << 6),       // 0x40
    SMALL_SCENERY_FLAG_ANIMATED_FG = (1 << 7),          // 0x80
    SMALL_SCENERY_FLAG_DIAGONAL = (1 << 8),             // 0x100
    SMALL_SCENERY_FLAG_HAS_GLASS = (1 << 9),            // 0x200
    SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR = (1 << 10),  // 0x400
    SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1 = (1 << 11),    // 0x800
    SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4 = (1 << 12),    // 0x1000
    SMALL_SCENERY_FLAG_IS_CLOCK = (1 << 13),            // 0x2000
    SMALL_SCENERY_FLAG_SWAMP_GOO = (1 << 14),           // 0x4000
    SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS = (1 << 15),   // 0x8000
    SMALL_SCENERY_FLAG17 = (1 << 16),                   // 0x10000
    SMALL_SCENERY_FLAG_STACKABLE = (1 << 17),           // 0x20000; means scenery item can be placed in the air and over water
    SMALL_SCENERY_FLAG_NO_WALLS = (1 << 18),            // 0x40000
    SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR = (1 << 19), // 0x80000
    SMALL_SCENERY_FLAG_NO_SUPPORTS = (1 << 20),          // 0x100000
    SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED = (1 << 21),  // 0x200000
    SMALL_SCENERY_FLAG_COG = (1 << 22),                  // 0x400000
    SMALL_SCENERY_FLAG_BUILD_DIRECTLY_ONTOP = (1 << 23), // 0x800000; means supports can be built on this object. Used for base
                                                         // blocks.
    SMALL_SCENERY_FLAG_HALF_SPACE = (1 << 24),           // 0x1000000
    SMALL_SCENERY_FLAG_THREE_QUARTERS = (1 << 25),       // 0x2000000
    SMALL_SCENERY_FLAG_PAINT_SUPPORTS = (1 << 26),       // 0x4000000; used for scenery items which are support structures
    SMALL_SCENERY_FLAG27 = (1 << 27),                    // 0x8000000

    // Added by OpenRCT2:
    SMALL_SCENERY_FLAG_IS_TREE = (1 << 28),
    SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR = (1 << 29),
};

struct SmallSceneryEntry
{
    StringId name;
    uint32_t image;
    uint32_t flags;
    uint8_t height;
    CursorID tool_id;
    money32 price;
    money32 removal_price;
    uint8_t* frame_offsets;
    uint16_t animation_delay;
    uint16_t animation_mask;
    uint16_t num_frames;
    ObjectEntryIndex scenery_tab_id;

    constexpr bool HasFlag(const uint32_t _flags) const
    {
        return (flags & _flags) != 0;
    }
};
