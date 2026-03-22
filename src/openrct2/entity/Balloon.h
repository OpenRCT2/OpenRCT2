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
        static constexpr auto cEntityType = EntityType::balloon;
        uint16_t frame;
        uint16_t popped;
        uint8_t time_to_move;
        Drawing::Colour colour;
        static void Create(const CoordsXYZ& balloonPos, Drawing::Colour colour, bool isPopped);
        void Update();
        void Pop(bool playSound);
        void Press();
        void Serialise(DataSerialiser& stream);
        bool Collides() const;
    };
} // namespace OpenRCT2
