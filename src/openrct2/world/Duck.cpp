/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Game.h"
#include "../audio/audio.h"
#include "../localisation/Date.h"
#include "../scenario/Scenario.h"
#include "../sprites.h"
#include "../world/Surface.h"
#include "Sprite.h"

#include <algorithm>
#include <iterator>
#include <limits>

// clang-format off
enum DUCK_STATE
{
    FLY_TO_WATER,
    SWIM,
    DRINK,
    DOUBLE_DRINK,
    FLY_AWAY,
};
constexpr const int32_t DUCK_MAX_STATES = 5;

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

template<> bool SpriteBase::Is<Duck>() const
{
    return sprite_identifier == SPRITE_IDENTIFIER_MISC && type == SPRITE_MISC_DUCK;
}

void Duck::Invalidate()
{
    Invalidate1();
}

bool Duck::IsFlying()
{
    return this->state == DUCK_STATE::FLY_AWAY || this->state == DUCK_STATE::FLY_TO_WATER;
}

void Duck::Remove()
{
    Invalidate();
    sprite_remove(this);
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

    auto surfaceElement = map_get_surface_element_at(CoordsXY{ target_x, target_y });
    int32_t waterHeight = surfaceElement != nullptr ? surfaceElement->GetWaterHeight() : 0;
    if (waterHeight == 0)
    {
        state = DUCK_STATE::FLY_AWAY;
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
            Invalidate();
        }
        else
        {
            if (destination.z > 4)
            {
                state = DUCK_STATE::FLY_AWAY;
                UpdateFlyAway();
            }
            else
            {
                state = DUCK_STATE::SWIM;
                frame = 0;
                UpdateSwim();
            }
        }
    }
}

void Duck::UpdateSwim()
{
    if (((gCurrentTicks + sprite_index) & 3) != 0)
        return;

    uint32_t randomNumber = scenario_rand();
    if ((randomNumber & 0xFFFF) < 0x666)
    {
        if (randomNumber & 0x80000000)
        {
            state = DUCK_STATE::DOUBLE_DRINK;
            frame = std::numeric_limits<uint16_t>::max();
            UpdateDoubleDrink();
        }
        else
        {
            state = DUCK_STATE::DRINK;
            frame = std::numeric_limits<uint16_t>::max();
            UpdateDrink();
        }
    }
    else
    {
        int32_t currentMonth = date_get_month(gDateMonthsElapsed);
        if (currentMonth >= MONTH_SEPTEMBER && (randomNumber >> 16) < 218)
        {
            state = DUCK_STATE::FLY_AWAY;
            UpdateFlyAway();
        }
        else
        {
            Invalidate();
            int16_t landZ = tile_element_height({ x, y });
            int16_t waterZ = tile_element_water_height({ x, y });

            if (z < landZ || waterZ == 0)
            {
                state = DUCK_STATE::FLY_AWAY;
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

                if (z >= landZ && z == waterZ)
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
        state = DUCK_STATE::SWIM;
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
        state = DUCK_STATE::SWIM;
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
            Invalidate();
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
    if (state < DUCK_MAX_STATES)
    {
        // TODO: Check frame is in range
        uint8_t imageOffset = DuckAnimations[state][frame];
        imageId = SPR_DUCK + (imageOffset * 4) + (direction / 8);
    }
    return imageId;
}

void create_duck(const CoordsXY& pos)
{
    rct_sprite* sprite = create_sprite(SPRITE_IDENTIFIER_MISC);
    if (sprite == nullptr)
        return;

    CoordsXY targetPos = pos;

    int32_t offsetXY = scenario_rand() & 0x1E;
    targetPos.x += offsetXY;
    targetPos.y += offsetXY;

    sprite->generic.sprite_identifier = SPRITE_IDENTIFIER_MISC;
    sprite->generic.type = SPRITE_MISC_DUCK;
    auto duck = sprite->generic.As<Duck>();
    if (duck == nullptr)
        return; // can never happen
    duck->sprite_width = 9;
    duck->sprite_height_negative = 12;
    duck->sprite_height_positive = 9;
    duck->target_x = targetPos.x;
    duck->target_y = targetPos.y;
    uint8_t direction = scenario_rand() & 3;
    switch (direction)
    {
        case 0:
            targetPos.x = 8191 - (scenario_rand() & 0x3F);
            break;
        case 1:
            targetPos.y = scenario_rand() & 0x3F;
            break;
        case 2:
            targetPos.x = scenario_rand() & 0x3F;
            break;
        case 3:
            targetPos.y = 8191 - (scenario_rand() & 0x3F);
            break;
    }
    duck->sprite_direction = direction << 3;
    duck->MoveTo({ targetPos.x, targetPos.y, 496 });
    duck->state = DUCK_STATE::FLY_TO_WATER;
    duck->frame = 0;
}

void Duck::Update()
{
    switch (static_cast<DUCK_STATE>(state))
    {
        case DUCK_STATE::FLY_TO_WATER:
            UpdateFlyToWater();
            break;
        case DUCK_STATE::SWIM:
            UpdateSwim();
            break;
        case DUCK_STATE::DRINK:
            UpdateDrink();
            break;
        case DUCK_STATE::DOUBLE_DRINK:
            UpdateDoubleDrink();
            break;
        case DUCK_STATE::FLY_AWAY:
            UpdateFlyAway();
            break;
    }
}

void duck_press(Duck* duck)
{
    audio_play_sound_at_location(SoundId::Quack, { duck->x, duck->y, duck->z });
}

void duck_remove_all()
{
    for (auto duck : EntityList<Duck>(EntityListId::Misc))
    {
        duck->Remove();
    }
}
