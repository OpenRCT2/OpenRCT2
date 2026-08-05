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

namespace OpenRCT2
{
    constexpr int32_t WC_MAIN_WINDOW__0 = 0;

    constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT = 27;
    constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE = 32;
    constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_EXIT = 33;
    constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_ROTATE = 34;

    constexpr int32_t WC_MAZE_CONSTRUCTION__WIDX_MAZE_DIRECTION_GROUPBOX = WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT;
    constexpr int32_t WC_MAZE_CONSTRUCTION__WIDX_MAZE_ENTRANCE = WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE;
    constexpr int32_t WC_MAZE_CONSTRUCTION__WIDX_MAZE_EXIT = WC_RIDE_CONSTRUCTION__WIDX_EXIT;

    constexpr int32_t WC_SCENERY__WIDX_SCENERY_BACKGROUND = 0;
    constexpr int32_t WC_SCENERY__WIDX_SCENERY_TAB_1 = 15;
    constexpr int32_t WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON = 5;
    constexpr int32_t WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON = 10;

    constexpr int32_t WC_PEEP__WIDX_PATROL = 10;
    constexpr int32_t WC_PEEP__WIDX_ACTION_LBL = 13;
    constexpr int32_t WC_PEEP__WIDX_PICKUP = 14;

    constexpr int32_t WC_TRACK_DESIGN_LIST__WIDX_ROTATE = 8;
    constexpr int32_t WC_TRACK_DESIGN_PLACE__WIDX_ROTATE = 3;

    constexpr int32_t WC_EDITOR_PARK_ENTRANCE__WIDX_ROTATE_ENTRANCE_BUTTON = 6;
    constexpr int32_t WC_EDITOR_OBJECT_SELECTION__WIDX_TAB_1 = 22;

    constexpr int32_t WC_STAFF__WIDX_PICKUP = 9;
} // namespace OpenRCT2
