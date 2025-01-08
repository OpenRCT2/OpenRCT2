/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Map.h"
#include "EntityBase.h"

class DataSerialiser;
struct PaintSession;

enum class JumpingFountainType : uint8_t
{
    Water,
    Snow
};

struct JumpingFountain : EntityBase
{
    static constexpr auto cEntityType = EntityType::JumpingFountain;

    uint16_t frame;
    JumpingFountainType FountainType;
    uint8_t NumTicksAlive;
    uint8_t FountainFlags;
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
        JumpingFountainType newType, const CoordsXYZ& newLoc, int32_t direction, int32_t newFlags, int32_t iteration);
    static bool IsJumpingFountain(JumpingFountainType newType, const CoordsXYZ& newLoc);
};

namespace OpenRCT2::FOUNTAIN_FLAG
{
    const uint32_t FAST = 1 << 0;
    const uint32_t GOTO_EDGE = 1 << 1;
    const uint32_t SPLIT = 1 << 2;
    const uint32_t TERMINATE = 1 << 3;
    const uint32_t BOUNCE = 1 << 4;
    const uint32_t DIRECTION = 1 << 7;
}; // namespace OpenRCT2::FOUNTAIN_FLAG
