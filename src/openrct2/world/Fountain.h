/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Map.h"
#include "SpriteBase.h"

struct JumpingFountain : SpriteGeneric
{
    uint8_t NumTicksAlive;
    uint8_t FountainFlags;
    int16_t TargetX;
    int16_t TargetY;
    uint16_t Iteration;

    void Update();
    static void StartAnimation(int32_t newType, const CoordsXY& newLoc, const TileElement* tileElement);

private:
    int32_t GetType() const;
    void AdvanceAnimation();
    void GoToEdge(const CoordsXYZ& newLoc, int32_t availableDirections) const;
    void Bounce(const CoordsXYZ& newLoc, int32_t availableDirections);
    void Split(const CoordsXYZ& newLoc, int32_t availableDirections) const;
    void Random(const CoordsXYZ& newLoc, int32_t availableDirections) const;
    void CreateNext(const CoordsXYZ& newLoc, int32_t direction) const;
    static void Create(int32_t newType, const CoordsXYZ& newLoc, int32_t direction, int32_t newFlags, int32_t iteration);
    static bool IsJumpingFountain(int32_t newType, const CoordsXYZ& newLoc);
};

enum
{
    JUMPING_FOUNTAIN_TYPE_WATER,
    JUMPING_FOUNTAIN_TYPE_SNOW
};

namespace FOUNTAIN_FLAG
{
    const uint32_t FAST = 1 << 0;
    const uint32_t GOTO_EDGE = 1 << 1;
    const uint32_t SPLIT = 1 << 2;
    const uint32_t TERMINATE = 1 << 3;
    const uint32_t BOUNCE = 1 << 4;
    const uint32_t DIRECTION = 1 << 7;
}; // namespace FOUNTAIN_FLAG
