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
struct CoordsXYZ;
struct CoordsXYZD;

enum LitterType : uint8_t
{
    LITTER_TYPE_SICK,
    LITTER_TYPE_SICK_ALT,
    LITTER_TYPE_EMPTY_CAN,
    LITTER_TYPE_RUBBISH,
    LITTER_TYPE_EMPTY_BURGER_BOX,
    LITTER_TYPE_EMPTY_CUP,
    LITTER_TYPE_EMPTY_BOX,
    LITTER_TYPE_EMPTY_BOTTLE,
    LITTER_TYPE_EMPTY_BOWL_RED,
    LITTER_TYPE_EMPTY_DRINK_CARTON,
    LITTER_TYPE_EMPTY_JUICE_CUP,
    LITTER_TYPE_EMPTY_BOWL_BLUE,
};

struct Litter : SpriteBase
{
    static constexpr auto cEntityType = EntityType::Litter;
    LitterType SubType;
    uint32_t creationTick;
    void Serialise(DataSerialiser& stream);
};

extern const rct_string_id litterNames[12];

void litter_create(const CoordsXYZD& litterPos, LitterType type);
void litter_remove_at(const CoordsXYZ& litterPos);
