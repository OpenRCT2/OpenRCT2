/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"
#include "EntityBase.h"

namespace OpenRCT2
{
    class DataSerialiser;

    enum class FountainFlag : uint8_t
    {
        fast,
        goToEdge,
        split,
        terminate,
        bounce,
        direction = 7,
    };
    using FountainFlags = FlagHolder<uint8_t, FountainFlag>;

    struct TileElement;

    enum class JumpingFountainType : uint8_t
    {
        water,
        snow
    };

    struct JumpingFountain : EntityBase
    {
        static constexpr auto kEntityType = EntityType::jumpingFountain;

        uint16_t frame;
        JumpingFountainType fountainType;
        uint8_t numTicksAlive;
        FountainFlags fountainFlags;
        int16_t targetX;
        int16_t targetY;
        uint16_t iteration;
        void update();
        static void startAnimation(JumpingFountainType newType, const CoordsXY& newLoc, const TileElement* tileElement);
        void serialise(DataSerialiser& stream);

    private:
        JumpingFountainType getType() const;
        void advanceAnimation();
        void goToEdge(const CoordsXYZ& newLoc, int32_t availableDirections) const;
        void bounce(const CoordsXYZ& newLoc, int32_t availableDirections);
        void split(const CoordsXYZ& newLoc, int32_t availableDirections) const;
        void random(const CoordsXYZ& newLoc, int32_t availableDirections) const;
        void createNext(const CoordsXYZ& newLoc, int32_t direction) const;
        static void create(
            JumpingFountainType newType, const CoordsXYZ& newLoc, int32_t direction, FountainFlags newFlags, int32_t iteration);
        static bool isJumpingFountain(JumpingFountainType newType, const CoordsXYZ& newLoc);
    };
} // namespace OpenRCT2
