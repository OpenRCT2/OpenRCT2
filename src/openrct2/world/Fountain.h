/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
    static void StartAnimation(int32_t newType, const CoordsXY newLoc, const TileElement* tileElement);

private:
    int32_t GetType() const;
    void AdvanceAnimation();
    void GoToEdge(CoordsXYZ newLoc, int32_t availableDirections) const;
    void Bounce(CoordsXYZ newLoc, int32_t availableDirections);
    void Split(CoordsXYZ newLoc, int32_t availableDirections) const;
    void Random(CoordsXYZ newLoc, int32_t availableDirections) const;
    void CreateNext(CoordsXYZ newLoc, int32_t direction) const;
    static void Create(int32_t newType, CoordsXYZ newLoc, int32_t direction, int32_t newFlags, int32_t iteration);
    static bool IsJumpingFountain(int32_t newType, CoordsXYZ newLoc);
};

enum
{
    JUMPING_FOUNTAIN_TYPE_WATER,
    JUMPING_FOUNTAIN_TYPE_SNOW
};
