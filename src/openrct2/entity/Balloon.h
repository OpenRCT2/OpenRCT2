/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "EntityBase.h"

struct CoordsXYZ;

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

namespace OpenRCT2
{
    class DataSerialiser;

    struct Balloon : EntityBase
    {
        static constexpr auto kEntityType = EntityType::balloon;
        uint16_t frame;
        uint16_t popped;
        uint8_t timeToMove;
        Drawing::Colour colour;

        static void create(const CoordsXYZ& balloonPos, Drawing::Colour colour, bool isPopped);
        void update();
        void pop(bool playSound);
        void press();
        void serialise(DataSerialiser& stream);
        bool collides() const;
    };
} // namespace OpenRCT2
