/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "Duck.h"

#include "../Game.h"
#include "../audio/audio.h"
#include "../core/DataSerialiser.h"
#include "../localisation/Date.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "../scenario/Scenario.h"
#include "../sprites.h"
#include "../world/Surface.h"
#include "EntityRegistry.h"

#include <algorithm>
#include <iterator>
#include <limits>

constexpr const int32_t DUCK_MAX_STATES = 5;

// clang-format off
static constexpr const CoordsXY DuckMoveOffset[] =
{
    { -1,  0 },
    {  0,  1 },
    {  1,  0 },
    {  0, -1 },
};

static constexpr const uint8_t DuckAnimationFlyToWater[] =
{
    8, 9, 10, 11, 12, 13
};

static constexpr const uint8_t DuckAnimationSwim[] =
{
    0
};

static constexpr const uint8_t DuckAnimationDrink[] =
{
    1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0xFF
};

static constexpr const uint8_t DuckAnimationDoubleDrink[] =
{
    4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6,
    6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 0, 0, 0, 0, 0xFF
};

static constexpr const uint8_t DuckAnimationFlyAway[] =
{
    8, 9, 10, 11, 12, 13
};

static constexpr const uint8_t * DuckAnimations[] =
{
    DuckAnimationFlyToWater,    // FLY_TO_WATER
    DuckAnimationSwim,          // SWIM
    DuckAnimationDrink,         // DRINK
    DuckAnimationDoubleDrink,   // DOUBLE_DRINK
    DuckAnimationFlyAway,       // FLY_AWAY
};
// clang-format on

template<> bool EntityBase::Is<Duck>() const
{
    return Type == EntityType::Duck;
}

bool Duck::IsFlying()
{
    return this->state == DuckState::FlyAway || this->state == DuckState::FlyToWater;
}

void Duck::Remove()
{
    Invalidate();
    EntityRemove(this);
}

void Duck::UpdateFlyToWater()
{
    if ((gCurrentTicks & 3) != 0)
        return;

    frame++;
    if (frame >= std::size(DuckAnimationFlyToWater))
    {
        frame = 0;
    }

    Invalidate();
    int32_t manhattanDistance = abs(target_x - x) + abs(target_y - y);
    int32_t direction = sprite_direction >> 3;
    auto destination = CoordsXYZ{ CoordsXY{ x, y } + DuckMoveOffset[direction], 0 };
    int32_t manhattanDistanceN = abs(target_x - destination.x) + abs(target_y - destination.y);

    auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ target_x, target_y });
    int32_t waterHeight = surfaceElement != nullptr ? surfaceElement->GetWaterHeight() : 0;
    if (waterHeight == 0)
    {
        state = DuckState::FlyAway;
        UpdateFlyAway();
    }
    else
    {
        destination.z = abs(z - waterHeight);

        if (manhattanDistanceN <= manhattanDistance)
        {
            if (destination.z > manhattanDistanceN)
            {
                destination.z = z - 2;
                if (waterHeight >= z)
                {
                    destination.z += 4;
                }
                frame = 1;
            }
            else
            {
                destination.z = z;
            }
            MoveTo(destination);
        }
        else
        {
            if (destination.z > 4)
            {
                state = DuckState::FlyAway;
                UpdateFlyAway();
            }
            else
            {
                state = DuckState::Swim;
                frame = 0;
                UpdateSwim();
            }
        }
    }
}

void Duck::UpdateSwim()
{
    if (((gCurrentTicks + sprite_index.ToUnderlying()) & 3) != 0)
        return;

    uint32_t randomNumber = scenario_rand();
    if ((randomNumber & 0xFFFF) < 0x666)
    {
        if (randomNumber & 0x80000000)
        {
            state = DuckState::DoubleDrink;
            frame = std::numeric_limits<uint16_t>::max();
            UpdateDoubleDrink();
        }
        else
        {
            state = DuckState::Drink;
            frame = std::numeric_limits<uint16_t>::max();
            UpdateDrink();
        }
    }
    else
    {
        int32_t currentMonth = date_get_month(gDateMonthsElapsed);
        if (currentMonth >= MONTH_SEPTEMBER && (randomNumber >> 16) < 218)
        {
            state = DuckState::FlyAway;
            UpdateFlyAway();
        }
        else
        {
            Invalidate();
            int16_t landZ = tile_element_height({ x, y });
            int16_t waterZ = tile_element_water_height({ x, y });

            if (z < landZ || waterZ == 0)
            {
                state = DuckState::FlyAway;
                UpdateFlyAway();
            }
            else
            {
                z = waterZ;
                randomNumber = scenario_rand();
                if ((randomNumber & 0xFFFF) <= 0xAAA)
                {
                    randomNumber >>= 16;
                    sprite_direction = randomNumber & 0x18;
                }

                int32_t direction = sprite_direction >> 3;
                auto destination = CoordsXYZ{ CoordsXY{ x, y } + DuckMoveOffset[direction], 0 };
                landZ = tile_element_height(destination);
                waterZ = tile_element_water_height(destination);

                if (z > landZ && z == waterZ)
                {
                    destination.z = waterZ;
                    MoveTo(destination);
                    Invalidate();
                }
            }
        }
    }
}

void Duck::UpdateDrink()
{
    frame++;
    if (DuckAnimationDrink[frame] == 0xFF)
    {
        state = DuckState::Swim;
        frame = 0;
        UpdateSwim();
    }
    else
    {
        Invalidate();
    }
}

void Duck::UpdateDoubleDrink()
{
    frame++;
    if (DuckAnimationDoubleDrink[frame] == 0xFF)
    {
        state = DuckState::Swim;
        frame = 0;
        UpdateSwim();
    }
    else
    {
        Invalidate();
    }
}

void Duck::UpdateFlyAway()
{
    if ((gCurrentTicks & 3) == 0)
    {
        frame++;
        if (frame >= std::size(DuckAnimationFlyAway))
        {
            frame = 0;
        }

        Invalidate();

        int32_t direction = sprite_direction >> 3;
        auto destination = CoordsXYZ{ x + (DuckMoveOffset[direction].x * 2), y + (DuckMoveOffset[direction].y * 2),
                                      std::min<int32_t>(z + 2, 496) };
        if (map_is_location_valid(destination))
        {
            MoveTo(destination);
        }
        else
        {
            Remove();
        }
    }
}

uint32_t Duck::GetFrameImage(int32_t direction) const
{
    uint32_t imageId = 0;
    if (EnumValue(state) < DUCK_MAX_STATES)
    {
        // TODO: Check frame is in range
        uint8_t imageOffset = DuckAnimations[EnumValue(state)][frame];
        imageId = SPR_DUCK + (imageOffset * 4) + (direction / 8);
    }
    return imageId;
}

void Duck::Create(const CoordsXY& pos)
{
    auto* duck = CreateEntity<Duck>();
    if (duck == nullptr)
        return;

    CoordsXY targetPos = pos;

    int32_t offsetXY = scenario_rand() & 0x1E;
    targetPos.x += offsetXY;
    targetPos.y += offsetXY;

    duck->sprite_width = 9;
    duck->sprite_height_negative = 12;
    duck->sprite_height_positive = 9;
    duck->target_x = targetPos.x;
    duck->target_y = targetPos.y;
    uint8_t direction = scenario_rand() & 3;
    switch (direction)
    {
        case 0:
            targetPos.x = GetMapSizeMaxXY().x - (scenario_rand() & 0x3F);
            break;
        case 1:
            targetPos.y = scenario_rand() & 0x3F;
            break;
        case 2:
            targetPos.x = scenario_rand() & 0x3F;
            break;
        case 3:
            targetPos.y = GetMapSizeMaxXY().y - (scenario_rand() & 0x3F);
            break;
    }
    duck->sprite_direction = direction << 3;
    duck->MoveTo({ targetPos.x, targetPos.y, 496 });
    duck->state = Duck::DuckState::FlyToWater;
    duck->frame = 0;
}

void Duck::Update()
{
    switch (state)
    {
        case DuckState::FlyToWater:
            UpdateFlyToWater();
            break;
        case DuckState::Swim:
            UpdateSwim();
            break;
        case DuckState::Drink:
            UpdateDrink();
            break;
        case DuckState::DoubleDrink:
            UpdateDoubleDrink();
            break;
        case DuckState::FlyAway:
            UpdateFlyAway();
            break;
    }
}

void Duck::Press()
{
    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::Quack, { x, y, z });
}

void Duck::RemoveAll()
{
    for (auto duck : EntityList<Duck>())
    {
        duck->Remove();
    }
}

void Duck::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);
    stream << frame;
    stream << target_x;
    stream << target_y;
    stream << state;
}

void Duck::Paint(paint_session& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    rct_drawpixelinfo& dpi = session.DPI;
    if (dpi.zoom_level > ZoomLevel{ 1 })
        return;

    uint32_t imageId = GetFrameImage(imageDirection);
    if (imageId != 0)
    {
        PaintAddImageAsParent(session, ImageId(imageId), { 0, 0, z }, { 1, 1, 0 });
    }
}
