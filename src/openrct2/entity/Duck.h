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

struct CoordsXY;

namespace OpenRCT2
{
    class DataSerialiser;

    struct Duck : EntityBase
    {
        static constexpr auto kEntityType = EntityType::duck;
        enum class DuckState : uint8_t
        {
            flyToWater,
            swim,
            drink,
            doubleDrink,
            flyAway,
        };
        uint16_t frame;
        int16_t targetX;
        int16_t targetY;
        DuckState state;

        static void create(const CoordsXY& pos);
        static void removeAll();
        void press();
        void update();
        uint32_t getFrameImage(int32_t direction) const;
        bool isFlying();
        void remove();
        void serialise(DataSerialiser& stream);

    private:
        void updateFlyToWater();
        void updateSwim();
        void updateDrink();
        void updateDoubleDrink();
        void updateFlyAway();
    };
} // namespace OpenRCT2
