/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "SpriteBase.h"

class DataSerialiser;
struct CoordsXY;

struct Duck : MiscEntity
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
    int16_t target_x;
    int16_t target_y;
    DuckState state;

    void Update();
    uint32_t GetFrameImage(int32_t direction) const;
    bool IsFlying();
    void Remove();
    void Serialise(DataSerialiser& stream);

private:
    void UpdateFlyToWater();
    void UpdateSwim();
    void UpdateDrink();
    void UpdateDoubleDrink();
    void UpdateFlyAway();
};

void create_duck(const CoordsXY& pos);
void duck_press(Duck* duck);
void duck_remove_all();
