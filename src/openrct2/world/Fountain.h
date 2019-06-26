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

struct JumpingFountain : rct_sprite_generic
{
    uint8_t numTicksAlive;
    uint8_t fountainFlags;
    int16_t targetX;
    int16_t targetY;
    uint16_t iteration;

    void Update();
    static void Begin(int32_t newType, int32_t newX, int32_t newY, const TileElement* tileElement);

private:
    int32_t GetType() const;
    void Continue();
    void GoToEdge(int32_t newX, int32_t newY, int32_t newZ, int32_t availableDirections) const;
    void Bounce(int32_t newX, int32_t newY, int32_t newZ, int32_t availableDirections);
    void Split(int32_t newX, int32_t newY, int32_t newZ, int32_t availableDirections) const;
    void Random(int32_t newX, int32_t newY, int32_t newZ, int32_t availableDirections) const;
    void CreateNext(int32_t newX, int32_t newY, int32_t newZ, int32_t direction) const;
    static void Create(
        int32_t newType, int32_t newX, int32_t newY, int32_t newZ, int32_t direction, int32_t newFlags, int32_t iteration);
    static bool IsJumpingFountain(int32_t newType, int32_t newX, int32_t newY, int32_t newZ);
};

enum
{
    JUMPING_FOUNTAIN_TYPE_WATER,
    JUMPING_FOUNTAIN_TYPE_SNOW
};
