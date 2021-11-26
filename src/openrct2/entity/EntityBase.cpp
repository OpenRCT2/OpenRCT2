/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EntityBase.h"

#include "../core/DataSerialiser.h"

void EntityBase::Serialise(DataSerialiser& stream)
{
    stream << Type;
    stream << sprite_index;
    stream << x;
    stream << y;
    stream << z;
    stream << sprite_direction;
}
