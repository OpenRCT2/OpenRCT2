/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include "../common.h"

#include "Map.h"
#include "Sprite.h"

enum
{
    JUMPING_FOUNTAIN_TYPE_WATER,
    JUMPING_FOUNTAIN_TYPE_SNOW
};

void jumping_fountain_begin(sint32 type, sint32 x, sint32 y, const rct_tile_element * tileElement);
void jumping_fountain_create(sint32 type, sint32 x, sint32 y, sint32 z, sint32 direction, sint32 flags, sint32 iteration);
void jumping_fountain_update(rct_jumping_fountain * jumpingFountain);

