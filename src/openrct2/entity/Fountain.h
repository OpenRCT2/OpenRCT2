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
#include "../world/Location.hpp"
#include "EntityBase.h"

struct PaintSession;

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
        Water,
        Snow
    };

    struct JumpingFountain : EntityBase
    {
        static constexpr auto cEntityType = EntityType::jumpingFountain;

        uint16_t frame;
        JumpingFountainType FountainType;
        uint8_t NumTicksAlive;
        FountainFlags fountainFlags;
        int16_t TargetX;
        int16_t TargetY;
        uint16_t Iteration;
        void Update();
        static void StartAnimation(JumpingFountainType newType, const CoordsXY& newLoc, const TileElement* tileElement);
        void Serialise(DataSerialiser& stream);
        void Paint(PaintSession& session, int32_t imageDirection) const;

    private:
        JumpingFountainType GetType() const;
        void AdvanceAnimation();
        void GoToEdge(const CoordsXYZ& newLoc, int32_t availableDirections) const;
        void Bounce(const CoordsXYZ& newLoc, int32_t availableDirections);
        void Split(const CoordsXYZ& newLoc, int32_t availableDirections) const;
        void Random(const CoordsXYZ& newLoc, int32_t availableDirections) const;
        void CreateNext(const CoordsXYZ& newLoc, int32_t direction) const;
        static void Create(
            JumpingFountainType newType, const CoordsXYZ& newLoc, int32_t direction, FountainFlags newFlags, int32_t iteration);
        static bool IsJumpingFountain(JumpingFountainType newType, const CoordsXYZ& newLoc);
    };
} // namespace OpenRCT2
