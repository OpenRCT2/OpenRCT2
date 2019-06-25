/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
    const uint32_t FAST = 1 << 0;
    const uint32_t GOTO_EDGE = 1 << 1;
    const uint32_t SPLIT = 1 << 2;
    const uint32_t TERMINATE = 1 << 3;
    const uint32_t BOUNCE = 1 << 4;
    const uint32_t DIRECTION = 1 << 7;
}; // namespace FOUNTAIN_FLAG

static constexpr const LocationXY16 _fountainDirectionsNegative[] = {
    { -32, 0 }, { -32, -32 }, { 0, 0 }, { -32, 0 }, { 0, 0 }, { 0, -32 }, { 0, -32 }, { -32, -32 },
};

static constexpr const LocationXY16 _fountainDirectionsPositive[] = { { 32, 0 },  { 0, 0 },  { 0, 32 }, { 32, 32 },
                                                                      { 32, 32 }, { 32, 0 }, { 0, 0 },  { 0, 32 } };

// rct2: 0x0097F040
const uint8_t _fountainDirections[] = { 0, 1, 2, 3, 0, 1, 2, 3 };

// rct2: 0x0097F048
const uint8_t _fountainDirectionFlags[] = {
    0, 0, FOUNTAIN_FLAG::DIRECTION, FOUNTAIN_FLAG::DIRECTION, FOUNTAIN_FLAG::DIRECTION, FOUNTAIN_FLAG::DIRECTION, 0, 0
};

// rct2: 0x0097F050
const uint8_t _fountainPatternFlags[] = {
    FOUNTAIN_FLAG::TERMINATE,                                              // CYCLIC_SQUARES
    FOUNTAIN_FLAG::FAST | FOUNTAIN_FLAG::GOTO_EDGE,                        // CONTINUOUS_CHASERS
    FOUNTAIN_FLAG::BOUNCE,                                                 // BOUNCING_PAIRS
    FOUNTAIN_FLAG::FAST | FOUNTAIN_FLAG::SPLIT,                            // SPROUTING_BLOOMS
    FOUNTAIN_FLAG::GOTO_EDGE,                                              // RACING_PAIRS
    FOUNTAIN_FLAG::FAST | FOUNTAIN_FLAG::GOTO_EDGE | FOUNTAIN_FLAG::SPLIT, // SPLITTING_CHASERS
    0,                                                                     // DOPEY_JUMPERS
    FOUNTAIN_FLAG::FAST                                                    // FAST_RANDOM_CHASERS
};

void rct_jumping_fountain::Begin(const int32_t newType, const int32_t newX, const int32_t newY, const TileElement* tileElement)
{
    int32_t randomIndex;
    const int32_t newZ = tileElement->base_height * 8;

    // Change pattern approximately every 51 seconds
    uint32_t pattern = (gCurrentTicks >> 11) & 7;
    switch (static_cast<PATTERN>(pattern))
    {
        case PATTERN::CYCLIC_SQUARES:
            // 0, 1, 2, 3
            for (int32_t i = 0; i < 4; i++)
            {
                rct_jumping_fountain::Create(
                    newType, newX + _fountainDirectionsPositive[i].x, newY + _fountainDirectionsPositive[i].y, newZ,
                    _fountainDirections[i], _fountainDirectionFlags[i] | _fountainPatternFlags[pattern], 0);
            }
            break;
        case PATTERN::BOUNCING_PAIRS:
            // random [0, 2 or 1, 3]
            randomIndex = scenario_rand() & 1;
            for (int32_t i = randomIndex; i < 4; i += 2)
            {
                rct_jumping_fountain::Create(
                    newType, newX + _fountainDirectionsPositive[i].x, newY + _fountainDirectionsPositive[i].y, newZ,
                    _fountainDirections[i], _fountainDirectionFlags[i] | _fountainPatternFlags[pattern], 0);
            }
            break;
        case PATTERN::RACING_PAIRS:
            // random [0 - 3 and 4 - 7]
            randomIndex = scenario_rand() & 3;
            rct_jumping_fountain::Create(
                newType, newX + _fountainDirectionsPositive[randomIndex].x, newY + _fountainDirectionsPositive[randomIndex].y,
                newZ, _fountainDirections[randomIndex], _fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern],
                0);
            randomIndex += 4;
            rct_jumping_fountain::Create(
                newType, newX + _fountainDirectionsPositive[randomIndex].x, newY + _fountainDirectionsPositive[randomIndex].y,
                newZ, _fountainDirections[randomIndex], _fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern],
                0);
            break;
        default:
            // random [0 - 7]
            randomIndex = scenario_rand() & 7;
            rct_jumping_fountain::Create(
                newType, newX + _fountainDirectionsPositive[randomIndex].x, newY + _fountainDirectionsPositive[randomIndex].y,
                newZ, _fountainDirections[randomIndex], _fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern],
                0);
            break;
    }
}

void rct_jumping_fountain::Create(
    const int32_t newType, const int32_t newX, const int32_t newY, const int32_t newZ, const int32_t direction,
    const int32_t newFlags, const int32_t iteration)
{
    const auto jumpingFountain = reinterpret_cast<rct_jumping_fountain*>(create_sprite(SPRITE_IDENTIFIER_MISC));
    if (jumpingFountain != nullptr)
    {
        jumpingFountain->iteration = iteration;
        jumpingFountain->fountain_flags = newFlags;
        jumpingFountain->sprite_direction = direction << 3;
        jumpingFountain->sprite_width = 33;
        jumpingFountain->sprite_height_negative = 36;
        jumpingFountain->sprite_height_positive = 12;
        jumpingFountain->sprite_identifier = SPRITE_IDENTIFIER_MISC;
        sprite_move(newX, newY, newZ, reinterpret_cast<rct_sprite*>(jumpingFountain));
        jumpingFountain->type = newType == JUMPING_FOUNTAIN_TYPE_SNOW ? SPRITE_MISC_JUMPING_FOUNTAIN_SNOW
                                                                      : SPRITE_MISC_JUMPING_FOUNTAIN_WATER;
        jumpingFountain->num_ticks_alive = 0;
        jumpingFountain->frame = 0;
    }
}

void rct_jumping_fountain::Update()
{
    num_ticks_alive++;
    // Originally this would not update the frame on the following
    // ticks: 1, 3, 6, 9, 11, 14, 17, 19, 22, 25
    // This change was to simplify the code base. There is a small increase
    // in speed of the fountain jump because of this change.
    if (num_ticks_alive % 3 == 0)
    {
        return;
    }

    invalidate_sprite_0(reinterpret_cast<rct_sprite*>(this));
    frame++;

    switch (type)
    {
        case SPRITE_MISC_JUMPING_FOUNTAIN_WATER:
            if (frame == 11 && (fountain_flags & FOUNTAIN_FLAG::FAST))
            {
                Continue();
            }
            if (frame == 16 && !(fountain_flags & FOUNTAIN_FLAG::FAST))
            {
                Continue();
            }
            break;
        case SPRITE_MISC_JUMPING_FOUNTAIN_SNOW:
            if (frame == 16)
            {
                Continue();
            }
            break;
    }

    if (frame == 16)
    {
        sprite_remove(reinterpret_cast<rct_sprite*>(this));
    }
}

int32_t rct_jumping_fountain::GetType() const
{
    const int32_t fountainType = type == SPRITE_MISC_JUMPING_FOUNTAIN_SNOW ? JUMPING_FOUNTAIN_TYPE_SNOW
                                                                           : JUMPING_FOUNTAIN_TYPE_WATER;
    return fountainType;
}

void rct_jumping_fountain::Continue()
{
    const int32_t newType = GetType();
    const int32_t direction = (sprite_direction >> 3) & 7;
    const int32_t newX = x + CoordsDirectionDelta[direction].x;
    const int32_t newY = y + CoordsDirectionDelta[direction].y;
    const int32_t newZ = z;

    int32_t availableDirections = 0;
    for (int32_t i = 0; i < 8; i++)
    {
        if (IsJumpingFountain(newType, newX + _fountainDirectionsNegative[i].x, newY + _fountainDirectionsNegative[i].y, newZ))
        {
            availableDirections |= 1 << i;
        }
    }

    if (availableDirections == 0)
    {
        return;
    }

    if (fountain_flags & FOUNTAIN_FLAG::TERMINATE)
    {
        return;
    }

    if (fountain_flags & FOUNTAIN_FLAG::GOTO_EDGE)
    {
        GoToEdge(newX, newY, newZ, availableDirections);
        return;
    }

    if (fountain_flags & FOUNTAIN_FLAG::BOUNCE)
    {
        Bounce(newX, newY, newZ, availableDirections);
        return;
    }

    if (fountain_flags & FOUNTAIN_FLAG::SPLIT)
    {
        Split(newX, newY, newZ, availableDirections);
        return;
    }

    Random(newX, newY, newZ, availableDirections);
}

bool rct_jumping_fountain::IsJumpingFountain(const int32_t newType, const int32_t newX, const int32_t newY, int32_t newZ)
{
    newZ >>= 3;

    const int32_t pathBitFlagMask = newType == JUMPING_FOUNTAIN_TYPE_SNOW ? PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW
                                                                          : PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER;

    TileElement* tileElement = map_get_first_element_at(newX >> 5, newY >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (tileElement->base_height != newZ)
            continue;
        if (tileElement->AsPath()->AdditionIsGhost())
            continue;
        if (!tileElement->AsPath()->HasAddition())
            continue;

        const uint8_t additionIndex = tileElement->AsPath()->GetAdditionEntryIndex();
        rct_scenery_entry* sceneryEntry = get_footpath_item_entry(additionIndex);
        if (sceneryEntry != nullptr && sceneryEntry->path_bit.flags & pathBitFlagMask)
        {
            return true;
        }
    } while (!(tileElement++)->IsLastForTile());

    return false;
}

void rct_jumping_fountain::GoToEdge(
    const int32_t newX, const int32_t newY, const int32_t newZ, const int32_t availableDirections) const
{
    int32_t direction = (sprite_direction >> 3) << 1;
    if (availableDirections & (1 << direction))
    {
        CreateNext(newX, newY, newZ, direction);
        return;
    }

    direction++;
    if (availableDirections & (1 << direction))
    {
        CreateNext(newX, newY, newZ, direction);
        return;
    }

    const uint32_t randomIndex = scenario_rand();
    if ((randomIndex & 0xFFFF) < 0x3333)
    {
        return;
    }

    if (fountain_flags & FOUNTAIN_FLAG::SPLIT)
    {
        Split(newX, newY, newZ, availableDirections);
        return;
    }

    direction = randomIndex & 7;
    while (!(availableDirections & (1 << direction)))
    {
        direction = (direction + 1) & 7;
    }

    CreateNext(newX, newY, newZ, direction);
}

void rct_jumping_fountain::Bounce(const int32_t newX, const int32_t newY, const int32_t newZ, const int32_t availableDirections)
{
    iteration++;
    if (iteration < 8)
    {
        int32_t direction = ((sprite_direction >> 3) ^ 2) << 1;
        if (availableDirections & (1 << direction))
        {
            CreateNext(newX, newY, newZ, direction);
        }
        else
        {
            direction++;
            if (availableDirections & (1 << direction))
            {
                CreateNext(newX, newY, newZ, direction);
            }
        }
    }
}

void rct_jumping_fountain::Split(const int32_t newX, const int32_t newY, const int32_t newZ, int32_t availableDirections) const
{
    if (iteration < 3)
    {
        const int32_t newType = GetType();
        int32_t direction = ((sprite_direction >> 3) ^ 2) << 1;
        availableDirections &= ~(1 << direction);
        availableDirections &= ~(1 << (direction + 1));

        for (direction = 0; direction < 8; direction++)
        {
            if (availableDirections & (1 << direction))
            {
                rct_jumping_fountain::Create(
                    newType, newX, newY, newZ, direction >> 1, fountain_flags & ~FOUNTAIN_FLAG::DIRECTION, iteration + 1);
            }
            direction++;
            if (availableDirections & (1 << direction))
            {
                rct_jumping_fountain::Create(
                    newType, newX, newY, newZ, direction >> 1, fountain_flags | FOUNTAIN_FLAG::DIRECTION, iteration + 1);
            }
        }
    }
}

void rct_jumping_fountain::Random(int32_t newX, int32_t newY, int32_t newZ, int32_t availableDirections) const
{
    const uint32_t randomIndex = scenario_rand();
    if ((randomIndex & 0xFFFF) >= 0x2000)
    {
        int32_t direction = randomIndex & 7;
        while (!(availableDirections & (1 << direction)))
        {
            direction = (direction + 1) & 7;
        }
        CreateNext(newX, newY, newZ, direction);
    }
}

void rct_jumping_fountain::CreateNext(int32_t newX, int32_t newY, int32_t newZ, int32_t direction) const
{
    const int32_t newType = GetType();
    int32_t newFlags = fountain_flags & ~FOUNTAIN_FLAG::DIRECTION;
    if (direction & 1)
    {
        newFlags |= FOUNTAIN_FLAG::DIRECTION;
    }
    rct_jumping_fountain::Create(newType, newX, newY, newZ, direction >> 1, newFlags, iteration);
}
