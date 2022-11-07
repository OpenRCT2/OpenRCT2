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
struct PaintSession;

struct MoneyEffect : EntityBase
{
    static constexpr auto cEntityType = EntityType::MoneyEffect;

    uint16_t frame;
    uint16_t MoveDelay;
    uint8_t NumMovements;
    uint8_t Vertical;
    money64 Value;
    int16_t OffsetX;
    uint16_t Wiggle;

    static void CreateAt(money64 value, const CoordsXYZ& effectPos, bool vertical);
    static void Create(money64 value, const CoordsXYZ& loc);
    void Update();
    std::pair<StringId, money64> GetStringId() const;
    void Serialise(DataSerialiser& stream);
    void Paint(PaintSession& session, int32_t imageDirection) const;
};
