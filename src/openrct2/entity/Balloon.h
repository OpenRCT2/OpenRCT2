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

class DataSerialiser;
struct CoordsXYZ;
struct PaintSession;

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

struct Balloon : EntityBase
{
    static constexpr auto cEntityType = EntityType::balloon;
    uint16_t frame;
    uint16_t popped;
    uint8_t time_to_move;
    OpenRCT2::Drawing::Colour colour;
    static void Create(const CoordsXYZ& balloonPos, OpenRCT2::Drawing::Colour colour, bool isPopped);
    void Update();
    void Pop(bool playSound);
    void Press();
    void Serialise(DataSerialiser& stream);
    void Paint(PaintSession& session, int32_t imageDirection) const;
    bool Collides() const;
};
