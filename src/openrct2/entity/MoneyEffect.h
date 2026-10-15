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
        static constexpr auto kEntityType = EntityType::moneyEffect;

        uint16_t frame;
        uint16_t moveDelay;
        uint8_t numMovements;
        uint8_t guestPurchase;
        money64 value;
        int16_t offsetX;
        uint16_t wiggle;

        static void createAt(money64 value, const CoordsXYZ& effectPos, bool guestPurchase);
        static void create(money64 value, const CoordsXYZ& loc);
        void setValue(money64 value);
        void update();
        std::pair<StringId, money64> getStringId() const;
        void serialise(DataSerialiser& stream);
    };
} // namespace OpenRCT2
