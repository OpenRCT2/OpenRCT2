#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../core/Math.hpp"
#include "../core/Util.hpp"
#include "../sprites.h"

extern "C"
{
    #include "../audio/audio.h"
    #include "../game.h"
    #include "../localisation/date.h"
    #include "../scenario/scenario.h"
    #include "sprite.h"
}

enum DUCK_STATE
{
    FLY_TO_WATER,
    SWIM,
    DRINK,
    DOUBLE_DRINK,
    FLY_AWAY,
};
constexpr sint32 DUCK_MAX_STATES = 5;

static const rct_xy16 DuckMoveOffset[] =
{
    { -1,  0 },
    {  0,  1 },
    {  1,  0 },
    {  0, -1 },
};

static const uint8 DuckAnimationFlyToWater[] =
{
    8, 9, 10, 11, 12, 13
};

static const uint8 DuckAnimationSwim[] =
{
    0
};

static const uint8 DuckAnimationDrink[] =
{
    1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0xFF
};

static const uint8 DuckAnimationDoubleDrink[] =
{
    4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6,
    6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 0, 0, 0, 0, 0xFF
};

static const uint8 DuckAnimationFlyAway[] =
{
    8, 9, 10, 11, 12, 13
};

static const uint8 * DuckAnimations[] =
{
    DuckAnimationFlyToWater,    // FLY_TO_WATER
    DuckAnimationSwim,          // SWIM
    DuckAnimationDrink,         // DRINK
    DuckAnimationDoubleDrink,   // DOUBLE_DRINK
    DuckAnimationFlyAway,       // FLY_AWAY
};

bool rct_sprite::IsDuck()
{
    return this->duck.sprite_identifier == SPRITE_IDENTIFIER_MISC &&
           this->duck.misc_identifier == SPRITE_MISC_DUCK;
}

rct_duck * rct_sprite::AsDuck()
{
    rct_duck * result = nullptr;
    if (IsDuck())
    {
        return (rct_duck *)result;
    }
    return result;
}

void rct_duck::Invalidate()
{
    invalidate_sprite_0((rct_sprite *)this);
}

void rct_duck::Remove()
{
    sprite_remove((rct_sprite *)this);
}

void rct_duck::MoveTo(sint16 destX, sint16 destY, sint16 destZ)
{
    sprite_move(destX, destY, destZ, (rct_sprite *)this);
}

void rct_duck::UpdateFlyToWater()
{
    if ((gCurrentTicks & 3) != 0) return;

    frame++;
    if (frame >= Util::CountOf(DuckAnimationFlyToWater))
    {
        frame = 0;
    }

    Invalidate();
    sint32 manhattanDistance = abs(target_x - x) + abs(target_y - y);
    sint32 direction = sprite_direction >> 3;
    sint32 newX = x + DuckMoveOffset[direction].x;
    sint32 newY = y + DuckMoveOffset[direction].y;
    sint32 manhattanDistanceN = abs(target_x - newX) + abs(target_y - newY);

    rct_map_element * mapElement = map_get_surface_element_at(target_x >> 5, target_y >> 5);
    sint32 waterHeight = mapElement->properties.surface.terrain & 0x1F;
    if (waterHeight == 0)
    {
        state = DUCK_STATE::FLY_AWAY;
        UpdateFlyAway();
    }
    else
    {
        waterHeight <<= 4;
        sint32 newZ = abs(z - waterHeight);

        if (manhattanDistanceN <= manhattanDistance)
        {
            if (newZ > manhattanDistanceN)
            {
                newZ = z - 2;
                if (waterHeight >= z)
                {
                    newZ += 4;
                }
                frame = 1;
            }
            else
            {
                newZ = z;
            }
            MoveTo(newX, newY, newZ);
            Invalidate();
        }
        else
        {
            if (newZ > 4)
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
    if (((gCurrentTicks + sprite_index) & 3) != 0) return;

    uint32 randomNumber = scenario_rand();
    if ((randomNumber & 0xFFFF) < 0x666)
    {
        if (randomNumber & 0x80000000)
        {
            state = DUCK_STATE::DOUBLE_DRINK;
            frame = -1;
            UpdateDoubleDrink();
        }
        else
        {
            state = DUCK_STATE::DRINK;
            frame = -1;
            UpdateDrink();
        }
    }
    else
    {
        sint32 currentMonth = date_get_month(gDateMonthsElapsed);
        if (currentMonth >= MONTH_SEPTEMBER && (randomNumber >> 16) < 218)
        {
            state = DUCK_STATE::FLY_AWAY;
            UpdateFlyAway();
        }
        else
        {
            Invalidate();
            sint32 landZ = map_element_height(x, y);
            sint32 waterZ = (landZ >> 16) & 0xFFFF;
            landZ &= 0xFFFF;

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

                sint32 direction = sprite_direction >> 3;
                sint32 newX = x + DuckMoveOffset[direction].x;
                sint32 newY = y + DuckMoveOffset[direction].y;
                landZ = map_element_height(newX, newY);
                waterZ = (landZ >> 16) & 0xFFFF;
                landZ &= 0xFFFF;

                if (z >= landZ && z == waterZ)
                {
                    MoveTo(newX, newY, waterZ);
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
        if (frame >= Util::CountOf(DuckAnimationFlyAway))
        {
            frame = 0;
        }

        Invalidate();

        sint32 direction = sprite_direction >> 3;
        sint32 newX = x + (DuckMoveOffset[direction].x * 2);
        sint32 newY = y + (DuckMoveOffset[direction].y * 2);
        sint32 newZ = Math::Min(z + 2, 496);
        if (map_is_location_valid(newX, newY))
        {
            MoveTo(newX, newY, newZ);
            Invalidate();
        }
        else
        {
            Remove();
        }
    }
}

uint32 rct_duck::GetFrameImage(sint32 direction) const
{
    uint32 imageId = 0;
    if (state < DUCK_MAX_STATES)
    {
        // TODO: Check frame is in range
        uint8 imageOffset = DuckAnimations[state][frame];
        imageId = SPR_DUCK + (imageOffset * 4) + (direction / 8);
    }
    return imageId;
}

extern "C"
{
    void create_duck(sint32 targetX, sint32 targetY)
    {
        rct_sprite * sprite = create_sprite(2);
        if (sprite != nullptr)
        {
            sprite->duck.sprite_identifier = SPRITE_IDENTIFIER_MISC;
            sprite->duck.misc_identifier = SPRITE_MISC_DUCK;
            sprite->duck.var_14 = 9;
            sprite->duck.var_09 = 0xC;
            sprite->duck.var_15 = 9;
            sint32 offsetXY = scenario_rand() & 0x1E;
            targetX += offsetXY;
            targetY += offsetXY;
            sprite->duck.target_x = targetX;
            sprite->duck.target_y = targetY;
            uint8 direction = scenario_rand() & 3;
            switch (direction) {
            case 0:
                targetX = 8191 - (scenario_rand() & 0x3F);
                break;
            case 1:
                targetY = scenario_rand() & 0x3F;
                break;
            case 2:
                targetX = scenario_rand() & 0x3F;
                break;
            case 3:
                targetY = 8191 - (scenario_rand() & 0x3F);
                break;
            }
            sprite->duck.sprite_direction = direction << 3;
            sprite_move(targetX, targetY, 496, sprite);
            sprite->duck.state = DUCK_STATE::FLY_TO_WATER;
            sprite->duck.frame = 0;
        }
    }

    void duck_update(rct_duck * duck)
    {
        switch ((DUCK_STATE)duck->state) {
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

    void duck_press(rct_duck * duck)
    {
        audio_play_sound_at_location(SOUND_QUACK, duck->x, duck->y, duck->z);
    }

    void duck_remove_all()
    {
        uint16 nextSpriteIndex;
        for (uint16 spriteIndex = gSpriteListHead[SPRITE_LIST_MISC]; spriteIndex != SPRITE_INDEX_NULL; spriteIndex = nextSpriteIndex)
        {
            rct_unk_sprite * sprite = &(get_sprite(spriteIndex)->unknown);
            nextSpriteIndex = sprite->next;
            if (sprite->misc_identifier == SPRITE_MISC_DUCK)
            {
                sprite_remove((rct_sprite *)sprite);
            }
        }
    }

    uint32 duck_get_frame_image(const rct_duck * duck, sint32 direction)
    {
        return duck->GetFrameImage(direction);
    }
}
