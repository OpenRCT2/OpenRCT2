/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "TileElement.h"

enum {
    WALL_ANIMATION_FLAG_ACROSS_TRACK = (1 << 2),
    // 3 - 6 animation frame number
    WALL_ANIMATION_FLAG_DIRECTION_BACKWARD = (1 << 7),
    WALL_ANIMATION_FLAG_ALL_FLAGS = WALL_ANIMATION_FLAG_ACROSS_TRACK | WALL_ANIMATION_FLAG_DIRECTION_BACKWARD
};

colour_t wall_get_primary_colour(const rct_tile_element * tileElement);
colour_t wall_get_secondary_colour(const rct_tile_element * wallElement);
colour_t wall_get_tertiary_colour(const rct_tile_element * tileElement);
void wall_set_primary_colour(rct_tile_element * tileElement, colour_t colour);
void wall_set_secondary_colour(rct_tile_element * wallElement, colour_t secondaryColour);
void wall_set_tertiary_colour(rct_tile_element * tileElement, colour_t colour);
uint8_t wall_get_animation_frame(const rct_tile_element * fenceElement);
void wall_set_animation_frame(rct_tile_element * wallElement, uint8_t frameNum);

money32 wall_remove(int16_t x, int16_t y, uint8_t baseHeight, uint8_t direction, uint8_t flags);
