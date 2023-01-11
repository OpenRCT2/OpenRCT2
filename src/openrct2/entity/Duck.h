/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "EntityBase.h"

class DataSerialiser;
struct CoordsXY;
struct PaintSession;

struct Duck : EntityBase
{
    static constexpr auto cEntityType = EntityType::Duck;
    enum class DuckState : uint8_t
    {
        FlyToWater,
        Swim,
        Drink,
        DoubleDrink,
        FlyAway,
    };
    uint16_t frame;
    int16_t target_x;
    int16_t target_y;
    DuckState state;

    static void Create(const CoordsXY& pos);
    static void RemoveAll();
    void Press();
    void Update();
    uint32_t GetFrameImage(int32_t direction) const;
    bool IsFlying();
    void Remove();
    void Serialise(DataSerialiser& stream);
    void Paint(PaintSession& session, int32_t imageDirection) const;

private:
    void UpdateFlyToWater();
    void UpdateSwim();
    void UpdateDrink();
    void UpdateDoubleDrink();
    void UpdateFlyAway();
};
