/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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

bool rct_sprite::IsDuck()
{
    return this->duck.sprite_identifier == SPRITE_IDENTIFIER_MISC && this->duck.type == SPRITE_MISC_DUCK;
}

rct_duck* rct_sprite::AsDuck()
{
    rct_duck* result = nullptr;
    if (IsDuck())
    {
        return (rct_duck*)this;
    }
    return result;
}

void rct_duck::Invalidate()
{
    invalidate_sprite_1((rct_sprite*)this);
}

void rct_duck::Remove()
{
    Invalidate();
    sprite_remove((rct_sprite*)this);
}

void rct_duck::MoveTo(const CoordsXYZ& destination)
{
    sprite_move(destination.x, destination.y, destination.z, (rct_sprite*)this);
}

void rct_duck::UpdateFlyToWater()
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

void rct_duck::UpdateSwim()
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

void rct_duck::UpdateDrink()
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

void rct_duck::UpdateDoubleDrink()
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

void rct_duck::UpdateFlyAway()
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

uint32_t rct_duck::GetFrameImage(int32_t direction) const
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

    sprite->duck.sprite_identifier = SPRITE_IDENTIFIER_MISC;
    sprite->duck.type = SPRITE_MISC_DUCK;
    sprite->duck.sprite_width = 9;
    sprite->duck.sprite_height_negative = 12;
    sprite->duck.sprite_height_positive = 9;
    sprite->duck.target_x = targetPos.x;
    sprite->duck.target_y = targetPos.y;
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
    sprite->duck.sprite_direction = direction << 3;
    sprite_move(targetPos.x, targetPos.y, 496, sprite);
    sprite->duck.state = DUCK_STATE::FLY_TO_WATER;
    sprite->duck.frame = 0;
}

void duck_update(rct_duck* duck)
{
    switch ((DUCK_STATE)duck->state)
    {
        case DUCK_STATE::FLY_TO_WATER:
            duck->UpdateFlyToWater();
            break;
        case DUCK_STATE::SWIM:
            duck->UpdateSwim();
            break;
        case DUCK_STATE::DRINK:
            duck->UpdateDrink();
            break;
        case DUCK_STATE::DOUBLE_DRINK:
            duck->UpdateDoubleDrink();
            break;
        case DUCK_STATE::FLY_AWAY:
            duck->UpdateFlyAway();
            break;
    }
}

void duck_press(rct_duck* duck)
{
    audio_play_sound_at_location(SoundId::Quack, { duck->x, duck->y, duck->z });
}

void duck_remove_all()
{
    uint16_t nextSpriteIndex;
    for (uint16_t spriteIndex = gSpriteListHead[SPRITE_LIST_MISC]; spriteIndex != SPRITE_INDEX_NULL;
         spriteIndex = nextSpriteIndex)
    {
        rct_sprite_generic* sprite = &(get_sprite(spriteIndex)->generic);
        nextSpriteIndex = sprite->next;
        if (sprite->type == SPRITE_MISC_DUCK)
        {
            invalidate_sprite_1((rct_sprite*)sprite);
            sprite_remove((rct_sprite*)sprite);
        }
    }
}

uint32_t duck_get_frame_image(const rct_duck* duck, int32_t direction)
{
    return duck->GetFrameImage(direction);
}
