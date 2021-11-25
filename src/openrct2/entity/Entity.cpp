/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Entity.h"

#include "../core/DataSerialiser.h"
#include "../peep/Peep.h"
#include "../peep/Staff.h"
#include "../ride/Vehicle.h"
#include "Balloon.h"
#include "Duck.h"
#include "Fountain.h"
#include "MoneyEffect.h"
#include "Particle.h"

void EntityBase::Serialise(DataSerialiser& stream)
{
    stream << Type;
    stream << sprite_index;
    stream << x;
    stream << y;
    stream << z;
    stream << sprite_direction;
}
