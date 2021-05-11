/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "SpriteBase.h"

class DataSerialiser;

struct Balloon : MiscEntity
{
    static constexpr auto cEntityType = EntityType::Balloon;
    uint16_t popped;
    uint8_t time_to_move;
    uint8_t colour;

    void Update();
    void Pop();
    void Press();
    void Serialise(DataSerialiser& stream);
};

void create_balloon(const CoordsXYZ& balloonPos, int32_t colour, bool isPopped);
void balloon_update(Balloon* balloon);
