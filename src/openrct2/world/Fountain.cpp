#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "Fountain.h"

#include "../Game.h"
#include "../scenario/Scenario.h"
#include "Footpath.h"
#include "Map.h"
#include "Scenery.h"
#include "Sprite.h"

enum class PATTERN
{
    CYCLIC_SQUARES,
    CONTINUOUS_CHASERS,
    BOUNCING_PAIRS,
    SPROUTING_BLOOMS,
    RACING_PAIRS,
    SPLITTING_CHASERS,
    DOPEY_JUMPERS,
    FAST_RANDOM_CHASERS,
};

namespace FOUNTAIN_FLAG
{
    const uint32 FAST         = 1 << 0;
    const uint32 GOTO_EDGE    = 1 << 1;
    const uint32 SPLIT        = 1 << 2;
    const uint32 TERMINATE    = 1 << 3;
    const uint32 BOUNCE       = 1 << 4;
    const uint32 DIRECTION    = 1 << 7;
};

static constexpr const LocationXY16 _fountainDirectionsNegative[] =
{
    { -32,   0 },
    { -32, -32 },
    {   0,   0 },
    { -32,   0 },
    {   0,   0 },
    {   0, -32 },
    {   0, -32 },
    { -32, -32 },
};

static constexpr const LocationXY16 _fountainDirectionsPositive[] =
{
    { 32,  0 },
    {  0,  0 },
    {  0, 32 },
    { 32, 32 },
    { 32, 32 },
    { 32,  0 },
    {  0,  0 },
    {  0, 32 }
};

// rct2: 0x0097F040
const uint8 _fountainDirections[] = { 0, 1, 2, 3, 0, 1, 2, 3 };

// rct2: 0x0097F048
const uint8 _fountainDirectionFlags[] = { 0, 0, FOUNTAIN_FLAG::DIRECTION, FOUNTAIN_FLAG::DIRECTION, FOUNTAIN_FLAG::DIRECTION, FOUNTAIN_FLAG::DIRECTION, 0, 0 };

// rct2: 0x0097F050
const uint8 _fountainPatternFlags[] = {
    FOUNTAIN_FLAG::TERMINATE,                                               // CYCLIC_SQUARES
    FOUNTAIN_FLAG::FAST | FOUNTAIN_FLAG::GOTO_EDGE,                         // CONTINUOUS_CHASERS
    FOUNTAIN_FLAG::BOUNCE,                                                  // BOUNCING_PAIRS
    FOUNTAIN_FLAG::FAST | FOUNTAIN_FLAG::SPLIT,                             // SPROUTING_BLOOMS
    FOUNTAIN_FLAG::GOTO_EDGE,                                               // RACING_PAIRS
    FOUNTAIN_FLAG::FAST | FOUNTAIN_FLAG::GOTO_EDGE | FOUNTAIN_FLAG::SPLIT,  // SPLITTING_CHASERS
    0,                                                                      // DOPEY_JUMPERS
    FOUNTAIN_FLAG::FAST                                                     // FAST_RANDOM_CHASERS
};

static sint32 jumping_fountain_get_type(const rct_jumping_fountain * jumpingFountain);
static void jumping_fountain_continue(rct_jumping_fountain * jumpingFountain);
static bool is_jumping_fountain(sint32 type, sint32 x, sint32 y, sint32 z);

static void jumping_fountain_goto_edge(const rct_jumping_fountain * jumpingFountain, sint32 x, sint32 y, sint32 z, sint32 availableDirections);
static void jumping_fountain_bounce(rct_jumping_fountain * jumpingFountain, sint32 x, sint32 y, sint32 z, sint32 availableDirections);
static void jumping_fountain_split(const rct_jumping_fountain * jumpingFountain, sint32 x, sint32 y, sint32 z, sint32 availableDirections);
static void jumping_fountain_random(const rct_jumping_fountain * jumpingFountain, sint32 x, sint32 y, sint32 z, sint32 availableDirections);
static void jumping_fountain_create_next(const rct_jumping_fountain * jumpingFountain, sint32 x, sint32 y, sint32 z, sint32 direction);

void jumping_fountain_begin(sint32 type, sint32 x, sint32 y, const rct_tile_element * tileElement)
{
    sint32 randomIndex;
    sint32 z = tileElement->base_height * 8;

    // Change pattern approximately every 51 seconds
    uint32 pattern = (gCurrentTicks >> 11) & 7;
    switch ((PATTERN)pattern) {
    case PATTERN::CYCLIC_SQUARES:
        // 0, 1, 2, 3
        for (sint32 i = 0; i < 4; i++)
        {
            jumping_fountain_create(
                type,
                x + _fountainDirectionsPositive[i].x,
                y + _fountainDirectionsPositive[i].y,
                z,
                _fountainDirections[i],
                _fountainDirectionFlags[i] | _fountainPatternFlags[pattern],
                0
            );
        }
        break;
    case PATTERN::BOUNCING_PAIRS:
        // random [0, 2 or 1, 3]
        randomIndex = scenario_rand() & 1;
        for (sint32 i = randomIndex; i < 4; i += 2)
        {
            jumping_fountain_create(
                type,
                x + _fountainDirectionsPositive[i].x,
                y + _fountainDirectionsPositive[i].y,
                z,
                _fountainDirections[i],
                _fountainDirectionFlags[i] | _fountainPatternFlags[pattern],
                0
            );
        }
        break;
    case PATTERN::RACING_PAIRS:
        // random [0 - 3 and 4 - 7]
        randomIndex = scenario_rand() & 3;
        jumping_fountain_create(
            type,
            x + _fountainDirectionsPositive[randomIndex].x,
            y + _fountainDirectionsPositive[randomIndex].y,
            z,
            _fountainDirections[randomIndex],
            _fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern],
            0
        );
        randomIndex += 4;
        jumping_fountain_create(
            type,
            x + _fountainDirectionsPositive[randomIndex].x,
            y + _fountainDirectionsPositive[randomIndex].y,
            z,
            _fountainDirections[randomIndex],
            _fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern],
            0
        );
        break;
    default:
        // random [0 - 7]
        randomIndex = scenario_rand() & 7;
        jumping_fountain_create(
            type,
            x + _fountainDirectionsPositive[randomIndex].x,
            y + _fountainDirectionsPositive[randomIndex].y,
            z,
            _fountainDirections[randomIndex],
            _fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern],
            0
        );
        break;
    }
}

void jumping_fountain_create(sint32 type, sint32 x, sint32 y, sint32 z, sint32 direction, sint32 flags, sint32 iteration)
{
    rct_jumping_fountain * jumpingFountain = (rct_jumping_fountain *)create_sprite(SPRITE_IDENTIFIER_MISC);
    if (jumpingFountain != nullptr)
    {
        jumpingFountain->iteration = iteration;
        jumpingFountain->fountain_flags = flags;
        jumpingFountain->sprite_direction = direction << 3;
        jumpingFountain->sprite_width = 33;
        jumpingFountain->sprite_height_negative = 36;
        jumpingFountain->sprite_height_positive = 12;
        jumpingFountain->sprite_identifier = SPRITE_IDENTIFIER_MISC;
        sprite_move(x, y, z, (rct_sprite *)jumpingFountain);
        jumpingFountain->misc_identifier = type == JUMPING_FOUNTAIN_TYPE_SNOW ?
            SPRITE_MISC_JUMPING_FOUNTAIN_SNOW :
            SPRITE_MISC_JUMPING_FOUNTAIN_WATER;
        jumpingFountain->num_ticks_alive = 0;
        jumpingFountain->frame = 0;
    }
}

void jumping_fountain_update(rct_jumping_fountain * jumpingFountain)
{
    jumpingFountain->num_ticks_alive++;
    // Originaly this would not update the frame on the following
    // ticks: 1, 3, 6, 9, 11, 14, 17, 19, 22, 25
    // This change was to simplefy the code base. There is a small increase
    // in speed of the fountain jump because of this change.
    if ((jumpingFountain->num_ticks_alive % 3) == 0)
    {
        return;
    }

    invalidate_sprite_0((rct_sprite *)jumpingFountain);
    jumpingFountain->frame++;

    switch (jumpingFountain->misc_identifier) {
    case SPRITE_MISC_JUMPING_FOUNTAIN_WATER:
        if (jumpingFountain->frame == 11 && (jumpingFountain->fountain_flags & FOUNTAIN_FLAG::FAST))
        {
            jumping_fountain_continue(jumpingFountain);
        }
        if (jumpingFountain->frame == 16 && !(jumpingFountain->fountain_flags & FOUNTAIN_FLAG::FAST))
        {
            jumping_fountain_continue(jumpingFountain);
        }
        break;
    case SPRITE_MISC_JUMPING_FOUNTAIN_SNOW:
        if (jumpingFountain->frame == 16)
        {
            jumping_fountain_continue(jumpingFountain);
        }
        break;
    }

    if (jumpingFountain->frame == 16)
    {
        sprite_remove((rct_sprite*)jumpingFountain);
    }
}


static sint32 jumping_fountain_get_type(const rct_jumping_fountain * jumpingFountain)
{
    sint32 type = jumpingFountain->misc_identifier == SPRITE_MISC_JUMPING_FOUNTAIN_SNOW ?
        JUMPING_FOUNTAIN_TYPE_SNOW :
        JUMPING_FOUNTAIN_TYPE_WATER;
    return type;
}

static void jumping_fountain_continue(rct_jumping_fountain * jumpingFountain)
{
    sint32 type = jumping_fountain_get_type(jumpingFountain);
    sint32 direction = (jumpingFountain->sprite_direction >> 3) & 7;
    sint32 x = jumpingFountain->x + CoordsDirectionDelta[direction].x;
    sint32 y = jumpingFountain->y + CoordsDirectionDelta[direction].y;
    sint32 z = jumpingFountain->z;

    sint32 availableDirections = 0;
    for (sint32 i = 0; i < 8; i++)
    {
        if (is_jumping_fountain(type, x + _fountainDirectionsNegative[i].x, y + _fountainDirectionsNegative[i].y, z))
        {
            availableDirections |= 1 << i;
        }
    }

    if (availableDirections == 0)
    {
        return;
    }

    if (jumpingFountain->fountain_flags & FOUNTAIN_FLAG::TERMINATE)
    {
        return;
    }

    if (jumpingFountain->fountain_flags & FOUNTAIN_FLAG::GOTO_EDGE)
    {
        jumping_fountain_goto_edge(jumpingFountain, x, y, z, availableDirections);
        return;
    }

    if (jumpingFountain->fountain_flags & FOUNTAIN_FLAG::BOUNCE)
    {
        jumping_fountain_bounce(jumpingFountain, x, y, z, availableDirections);
        return;
    }

    if (jumpingFountain->fountain_flags & FOUNTAIN_FLAG::SPLIT)
    {
        jumping_fountain_split(jumpingFountain, x, y, z, availableDirections);
        return;
    }

    jumping_fountain_random(jumpingFountain, x, y, z, availableDirections);
}

static bool is_jumping_fountain(sint32 type, sint32 x, sint32 y, sint32 z)
{
    z = z >> 3;

    sint32 pathBitFlagMask = type == JUMPING_FOUNTAIN_TYPE_SNOW ?
        PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW :
        PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER;

    rct_tile_element * tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH) continue;
        if (tileElement->base_height != z) continue;
        if (footpath_element_path_scenery_is_ghost(tileElement)) continue;
        if (!footpath_element_has_path_scenery(tileElement)) continue;

        uint8 additionIndex = footpath_element_get_path_scenery_index(tileElement);
        rct_scenery_entry * sceneryEntry = get_footpath_item_entry(additionIndex);
        if (sceneryEntry != reinterpret_cast<void*>(-1) && sceneryEntry->path_bit.flags & pathBitFlagMask)
        {
            return true;
        }
    }
    while (!(tileElement++)->IsLastForTile());

    return false;
}

static void jumping_fountain_goto_edge(const rct_jumping_fountain * jumpingFountain, sint32 x, sint32 y, sint32 z, sint32 availableDirections)
{
    sint32 direction = (jumpingFountain->sprite_direction >> 3) << 1;
    if (availableDirections & (1 << direction))
    {
        jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
        return;
    }

    direction++;
    if (availableDirections & (1 << direction))
    {
        jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
        return;
    }

    uint32 randomIndex = scenario_rand();
    if ((randomIndex & 0xFFFF) < 0x3333)
    {
        return;
    }

    if (jumpingFountain->fountain_flags & FOUNTAIN_FLAG::SPLIT)
    {
        jumping_fountain_split(jumpingFountain, x, y, z, availableDirections);
        return;
    }

    direction = randomIndex & 7;
    while (!(availableDirections & (1 << direction)))
    {
        direction = (direction + 1) & 7;
    }

    jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
}

static void jumping_fountain_bounce(rct_jumping_fountain * jumpingFountain, sint32 x, sint32 y, sint32 z, sint32 availableDirections)
{
    jumpingFountain->iteration++;
    if (jumpingFountain->iteration < 8)
    {
        sint32 direction = ((jumpingFountain->sprite_direction >> 3) ^ 2) << 1;
        if (availableDirections & (1 << direction))
        {
            jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
        }
        else
        {
            direction++;
            if (availableDirections & (1 << direction))
            {
                jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
            }
        }
    }
}

static void jumping_fountain_split(const rct_jumping_fountain * jumpingFountain, sint32 x, sint32 y, sint32 z, sint32 availableDirections)
{
    if (jumpingFountain->iteration < 3)
    {
        sint32 type = jumping_fountain_get_type(jumpingFountain);
        sint32 direction = ((jumpingFountain->sprite_direction >> 3) ^ 2) << 1;
        availableDirections &= ~(1 << direction);
        availableDirections &= ~(1 << (direction + 1));

        for (direction = 0; direction < 8; direction++)
        {
            if (availableDirections & (1 << direction))
            {
                jumping_fountain_create(
                    type,
                    x, y, z,
                    direction >> 1,
                    jumpingFountain->fountain_flags & ~FOUNTAIN_FLAG::DIRECTION,
                    jumpingFountain->iteration + 1
                );
            }
            direction++;
            if (availableDirections & (1 << direction))
            {
                jumping_fountain_create(
                    type,
                    x, y, z,
                    direction >> 1,
                    jumpingFountain->fountain_flags | FOUNTAIN_FLAG::DIRECTION,
                    jumpingFountain->iteration + 1
                );
            }
        }
    }
}

static void jumping_fountain_random(const rct_jumping_fountain * jumpingFountain, sint32 x, sint32 y, sint32 z, sint32 availableDirections)
{
    uint32 randomIndex = scenario_rand();
    if ((randomIndex & 0xFFFF) >= 0x2000)
    {
        sint32 direction = randomIndex & 7;
        while (!(availableDirections & (1 << direction)))
        {
            direction = (direction + 1) & 7;
        }
        jumping_fountain_create_next(jumpingFountain, x, y, z, direction);
    }
}

static void jumping_fountain_create_next(const rct_jumping_fountain * jumpingFountain, sint32 x, sint32 y, sint32 z, sint32 direction)
{
    sint32 type = jumping_fountain_get_type(jumpingFountain);
    sint32 flags = jumpingFountain->fountain_flags & ~FOUNTAIN_FLAG::DIRECTION;
    if (direction & 1)
    {
        flags |= FOUNTAIN_FLAG::DIRECTION;
    }
    jumping_fountain_create(type, x, y, z, direction >> 1, flags, jumpingFountain->iteration);
}
