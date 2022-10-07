/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
struct paint_session;

struct Balloon : EntityBase
{
    static constexpr auto cEntityType = EntityType::Balloon;
    uint16_t frame;
    uint16_t popped;
    uint8_t time_to_move;
    uint8_t colour;
    static void Create(const CoordsXYZ& balloonPos, int32_t colour, bool isPopped);
    void Update();
    void Pop();
    void Press();
    void Serialise(DataSerialiser& stream);
    void Paint(paint_session& session, int32_t imageDirection) const;
};
