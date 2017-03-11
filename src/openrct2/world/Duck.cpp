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

#include "../core/Util.hpp"

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
    FLY_AWAY
};

static void duck_update_fly_to_water(rct_duck * duck);
static void duck_update_swim(rct_duck * duck);
static void duck_update_drink(rct_duck * duck);
static void duck_update_double_drink(rct_duck * duck);
static void duck_update_fly_away(rct_duck * duck);

static const rct_xy16 DuckMoveOffset[4] =
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

static void duck_invalidate(rct_duck *duck)
{
    invalidate_sprite_0((rct_sprite *)duck);
}

static void duck_update_fly_to_water(rct_duck *duck)
{
    if ((gCurrentTicks & 3) != 0) return;

    duck->frame++;
    if (duck->frame >= Util::CountOf(DuckAnimationFlyToWater))
    {
        duck->frame = 0;
    }

    duck_invalidate(duck);
    sint32 manhattanDistance = abs(duck->target_x - duck->x) + abs(duck->target_y - duck->y);
    sint32 direction = duck->sprite_direction >> 3;
    sint32 x = duck->x + DuckMoveOffset[direction].x;
    sint32 y = duck->y + DuckMoveOffset[direction].y;
    sint32 manhattanDistanceN = abs(duck->target_x - x) + abs(duck->target_y - y);

    rct_map_element * mapElement = map_get_surface_element_at(duck->target_x >> 5, duck->target_y >> 5);
    sint32 waterHeight = mapElement->properties.surface.terrain & 0x1F;
    if (waterHeight == 0)
    {
        duck->state = DUCK_STATE::FLY_AWAY;
        duck_update_fly_away(duck);
    }
    else
    {
        waterHeight <<= 4;
        sint32 z = abs(duck->z - waterHeight);

        if (manhattanDistanceN <= manhattanDistance)
        {
            if (z > manhattanDistanceN)
            {
                z = duck->z - 2;
                if (waterHeight >= duck->z)
                {
                    z += 4;
                }
                duck->frame = 1;
            }
            else
            {
                z = duck->z;
            }
            sprite_move(x, y, z, (rct_sprite*)duck);
            duck_invalidate(duck);
        }
        else
        {
            if (z > 4)
            {
                duck->state = DUCK_STATE::FLY_AWAY;
                duck_update_fly_away(duck);
            }
            else
            {
                duck->state = DUCK_STATE::SWIM;
                duck->frame = 0;
                duck_update_swim(duck);
            }
        }
    }
}

/**
 *
 *  rct2: 0x00674282
 */
static void duck_update_swim(rct_duck *duck)
{
    if ((gCurrentTicks + duck->sprite_index) & 3)
        return;

    uint32 randomNumber = scenario_rand();
    if ((randomNumber & 0xFFFF) < 0x666)
    {
        if (randomNumber & 0x80000000)
        {
            duck->state = DUCK_STATE::DOUBLE_DRINK;
            duck->frame = -1;
            duck_update_double_drink(duck);
        }
        else
        {
            duck->state = DUCK_STATE::DRINK;
            duck->frame = -1;
            duck_update_drink(duck);
        }
    }
    else
    {
        sint32 currentMonth = date_get_month(gDateMonthsElapsed);
        if (currentMonth >= MONTH_SEPTEMBER && (randomNumber >> 16) < 218)
        {
            duck->state = DUCK_STATE::FLY_AWAY;
            duck_update_fly_away(duck);
        }
        else
        {
            duck_invalidate(duck);
            sint32 landZ = map_element_height(duck->x, duck->y);
            sint32 waterZ = (landZ >> 16) & 0xFFFF;
            landZ &= 0xFFFF;

            if (duck->z < landZ || waterZ == 0)
            {
                duck->state = DUCK_STATE::FLY_AWAY;
                duck_update_fly_away(duck);
            }
            else
            {
                duck->z = waterZ;
                randomNumber = scenario_rand();
                if ((randomNumber & 0xFFFF) <= 0xAAA)
                {
                    randomNumber >>= 16;
                    duck->sprite_direction = randomNumber & 0x18;
                }

                sint32 direction = duck->sprite_direction >> 3;
                sint32 x = duck->x + DuckMoveOffset[direction].x;
                sint32 y = duck->y + DuckMoveOffset[direction].y;
                landZ = map_element_height(x, y);
                waterZ = (landZ >> 16) & 0xFFFF;
                landZ &= 0xFFFF;

                if (duck->z >= landZ && duck->z == waterZ)
                {
                    sprite_move(x, y, waterZ, (rct_sprite*)duck);
                    duck_invalidate(duck);
                }
            }
        }
    }
}

static void duck_update_drink(rct_duck * duck)
{
    duck->frame++;
    if (DuckAnimationDrink[duck->frame] == 0xFF)
    {
        duck->state = DUCK_STATE::SWIM;
        duck->frame = 0;
        duck_update_swim(duck);
    }
    else
    {
        duck_invalidate(duck);
    }
}

static void duck_update_double_drink(rct_duck * duck)
{
    duck->frame++;
    if (DuckAnimationDoubleDrink[duck->frame] == 0xFF)
    {
        duck->state = DUCK_STATE::SWIM;
        duck->frame = 0;
        duck_update_swim(duck);
    }
    else
    {
        duck_invalidate(duck);
    }
}

static void duck_update_fly_away(rct_duck * duck)
{
    if ((gCurrentTicks & 3) == 0)
    {
        duck->frame++;
        if (duck->frame >= Util::CountOf(DuckAnimationFlyAway))
        {
            duck->frame = 0;
        }

        duck_invalidate(duck);
        sint32 direction = duck->sprite_direction >> 3;
        sint32 x = duck->x + (DuckMoveOffset[direction].x * 2);
        sint32 y = duck->y + (DuckMoveOffset[direction].y * 2);
        if (map_is_location_valid(x, y))
        {
            sprite_remove((rct_sprite *)duck);
        }
        else
        {
            sint32 z = min(duck->z + 2, 496);
            sprite_move(x, y, z, (rct_sprite*)duck);
            duck_invalidate(duck);
        }
    }
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
            sint32 offset_xy = scenario_rand() & 0x1E;
            targetX += offset_xy;
            targetY += offset_xy;
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
            duck_update_fly_to_water(duck);
            break;
        case DUCK_STATE::SWIM:
            duck_update_swim(duck);
            break;
        case DUCK_STATE::DRINK:
            duck_update_drink(duck);
            break;
        case DUCK_STATE::DOUBLE_DRINK:
            duck_update_double_drink(duck);
            break;
        case DUCK_STATE::FLY_AWAY:
            duck_update_fly_away(duck);
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
}
