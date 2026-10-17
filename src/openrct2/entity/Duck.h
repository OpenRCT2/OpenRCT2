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

        // state is packed into a single byte: the DuckState only needs 3 bits (5 possible values), so
        // the remaining bits are used to store the freeze flags below. This avoids needing to extend the
        // save format.
        static constexpr uint8_t kStateMask = 0x07;
        static constexpr uint8_t kPositionFrozenFlag = 0x08;
        static constexpr uint8_t kAnimationFrozenFlag = 0x10;

        uint16_t frame;
        int16_t targetX;
        int16_t targetY;
        uint8_t state{};

        DuckState getState() const
        {
            return static_cast<DuckState>(state & kStateMask);
        }
        void setState(DuckState newState)
        {
            state = static_cast<uint8_t>((state & ~kStateMask) | (static_cast<uint8_t>(newState) & kStateMask));
        }

        bool getPositionFrozen() const
        {
            return (state & kPositionFrozenFlag) != 0;
        }
        void setPositionFrozen(bool value)
        {
            state = static_cast<uint8_t>(value ? (state | kPositionFrozenFlag) : (state & ~kPositionFrozenFlag));
        }

        bool getAnimationFrozen() const
        {
            return (state & kAnimationFrozenFlag) != 0;
        }
        void setAnimationFrozen(bool value)
        {
            state = static_cast<uint8_t>(value ? (state | kAnimationFrozenFlag) : (state & ~kAnimationFrozenFlag));
        }

        static void create(const CoordsXY& pos);
        static void removeAll();
        void press();
        void update();
        uint32_t getFrameImage(int32_t direction) const;
        bool isFlying() const;
        void remove();
        void serialise(DataSerialiser& stream);

        // Returns the number of frames in the animation for the given state, or 0 if the state has no animation.
        static size_t getAnimationLength(DuckState state);

    private:
        void updateFlyToWater();
        void updateSwim();
        void updateDrink();
        void updateDoubleDrink();
        void updateFlyAway();
    };
} // namespace OpenRCT2
