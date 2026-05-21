/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../localisation/StringIdType.h"
#include "EntityBase.h"

#include <utility>

struct CoordsXYZ;
struct PaintSession;

namespace OpenRCT2
{
    class DataSerialiser;

    struct MoneyEffect : EntityBase
    {
        static constexpr auto cEntityType = EntityType::moneyEffect;

        uint16_t frame;
        uint16_t moveDelay;
        uint8_t numMovements;
        uint8_t guestPurchase;
        money64 value;
        int16_t offsetX;
        uint16_t wiggle;

        static void CreateAt(money64 value, const CoordsXYZ& effectPos, bool guestPurchase);
        static void Create(money64 value, const CoordsXYZ& loc);
        void SetValue(money64 value);
        void Update();
        std::pair<StringId, money64> GetStringId() const;
        void serialise(DataSerialiser& stream);
        void Paint(PaintSession& session, int32_t imageDirection) const;
    };
} // namespace OpenRCT2
