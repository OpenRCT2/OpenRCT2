/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Fountain.h"

#include "../Game.h"
#include "../core/DataSerialiser.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "../scenario/Scenario.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/Scenery.h"
#include "EntityRegistry.h"

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

static constexpr const std::array _fountainDirectionsNegative = {
    CoordsXY{ -COORDS_XY_STEP, 0 },
    CoordsXY{ -COORDS_XY_STEP, -COORDS_XY_STEP },
    CoordsXY{ 0, 0 },
    CoordsXY{ -COORDS_XY_STEP, 0 },
    CoordsXY{ 0, 0 },
    CoordsXY{ 0, -COORDS_XY_STEP },
    CoordsXY{ 0, -COORDS_XY_STEP },
    CoordsXY{ -COORDS_XY_STEP, -COORDS_XY_STEP },
};

static constexpr const std::array _fountainDirectionsPositive = {
    CoordsXY{ COORDS_XY_STEP, 0 },
    CoordsXY{ 0, 0 },
    CoordsXY{ 0, COORDS_XY_STEP },
    CoordsXY{ COORDS_XY_STEP, COORDS_XY_STEP },
    CoordsXY{ COORDS_XY_STEP, COORDS_XY_STEP },
    CoordsXY{ COORDS_XY_STEP, 0 },
    CoordsXY{ 0, 0 },
    CoordsXY{ 0, COORDS_XY_STEP },
};
constexpr auto _FountainChanceOfStoppingEdgeMode = 0x3333;   // 0.200
constexpr auto _FountainChanceOfStoppingRandomMode = 0x2000; // 0.125

// rct2: 0x0097F040
const uint8_t _fountainDirections[] = {
    0, 1, 2, 3, 0, 1, 2, 3,
};

// rct2: 0x0097F048
const uint8_t _fountainDirectionFlags[] = {
    0, 0, FOUNTAIN_FLAG::DIRECTION, FOUNTAIN_FLAG::DIRECTION, FOUNTAIN_FLAG::DIRECTION, FOUNTAIN_FLAG::DIRECTION, 0, 0,
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
    FOUNTAIN_FLAG::FAST,                                                   // FAST_RANDOM_CHASERS
};

template<> bool EntityBase::Is<JumpingFountain>() const
{
    return Type == EntityType::JumpingFountain;
}

void JumpingFountain::StartAnimation(const JumpingFountainType newType, const CoordsXY& newLoc, const TileElement* tileElement)
{
    int32_t randomIndex;
    auto newZ = tileElement->GetBaseZ();

    // Change pattern approximately every 51 seconds
    uint32_t pattern = (gCurrentTicks >> 11) & 7;
    switch (static_cast<PATTERN>(pattern))
    {
        case PATTERN::CYCLIC_SQUARES:
            // 0, 1, 2, 3
            for (int32_t i = 0; i < 4; i++)
            {
                JumpingFountain::Create(
                    newType, { newLoc + _fountainDirectionsPositive[i], newZ }, _fountainDirections[i],
                    _fountainDirectionFlags[i] | _fountainPatternFlags[pattern], 0);
            }
            break;
        case PATTERN::BOUNCING_PAIRS:
            // random [0, 2 or 1, 3]
            randomIndex = scenario_rand() & 1;
            for (int32_t i = randomIndex; i < 4; i += 2)
            {
                JumpingFountain::Create(
                    newType, { newLoc + _fountainDirectionsPositive[i], newZ }, _fountainDirections[i],
                    _fountainDirectionFlags[i] | _fountainPatternFlags[pattern], 0);
            }
            break;
        case PATTERN::RACING_PAIRS:
            // random [0 - 3 and 4 - 7]
            randomIndex = scenario_rand() & 3;
            JumpingFountain::Create(
                newType, { newLoc + _fountainDirectionsPositive[randomIndex], newZ }, _fountainDirections[randomIndex],
                _fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern], 0);
            randomIndex += 4;
            JumpingFountain::Create(
                newType, { newLoc + _fountainDirectionsPositive[randomIndex], newZ }, _fountainDirections[randomIndex],
                _fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern], 0);
            break;
        default:
            // random [0 - 7]
            randomIndex = scenario_rand() & 7;
            JumpingFountain::Create(
                newType, { newLoc + _fountainDirectionsPositive[randomIndex], newZ }, _fountainDirections[randomIndex],
                _fountainDirectionFlags[randomIndex] | _fountainPatternFlags[pattern], 0);
            break;
    }
}

void JumpingFountain::Create(
    const JumpingFountainType newType, const CoordsXYZ& newLoc, const int32_t direction, const int32_t newFlags,
    const int32_t iteration)
{
    auto* jumpingFountain = CreateEntity<JumpingFountain>();
    if (jumpingFountain != nullptr)
    {
        jumpingFountain->Iteration = iteration;
        jumpingFountain->FountainFlags = newFlags;
        jumpingFountain->sprite_direction = direction << 3;
        jumpingFountain->sprite_width = 33;
        jumpingFountain->sprite_height_negative = 36;
        jumpingFountain->sprite_height_positive = 12;
        jumpingFountain->MoveTo(newLoc);
        jumpingFountain->FountainType = newType;
        jumpingFountain->NumTicksAlive = 0;
        jumpingFountain->frame = 0;
    }
}

void JumpingFountain::Update()
{
    NumTicksAlive++;
    // Originally this would not update the frame on the following
    // ticks: 1, 3, 6, 9, 11, 14, 17, 19, 22, 25
    // This change was to simplify the code base. There is a small increase
    // in speed of the fountain jump because of this change.
    if (NumTicksAlive % 3 == 0)
    {
        return;
    }

    Invalidate();
    frame++;

    switch (FountainType)
    {
        case JumpingFountainType::Water:
            if (frame == 11 && (FountainFlags & FOUNTAIN_FLAG::FAST))
            {
                AdvanceAnimation();
            }
            if (frame == 16 && !(FountainFlags & FOUNTAIN_FLAG::FAST))
            {
                AdvanceAnimation();
            }
            break;
        case JumpingFountainType::Snow:
            if (frame == 16)
            {
                AdvanceAnimation();
            }
            break;
        default:
            break;
    }

    if (frame == 16)
    {
        EntityRemove(this);
    }
}

JumpingFountainType JumpingFountain::GetType() const
{
    return FountainType;
}

void JumpingFountain::AdvanceAnimation()
{
    const JumpingFountainType newType = GetType();
    const int32_t direction = (sprite_direction >> 3) & 7;
    const CoordsXY newLoc = CoordsXY{ x, y } + CoordsDirectionDelta[direction];

    int32_t availableDirections = 0;
    for (uint32_t i = 0; i < _fountainDirectionsNegative.size(); i++)
    {
        if (IsJumpingFountain(newType, { newLoc + _fountainDirectionsNegative[i], z }))
        {
            availableDirections |= 1 << i;
        }
    }

    if (availableDirections == 0)
    {
        return;
    }

    if (FountainFlags & FOUNTAIN_FLAG::TERMINATE)
    {
        return;
    }

    if (FountainFlags & FOUNTAIN_FLAG::GOTO_EDGE)
    {
        GoToEdge({ newLoc, z }, availableDirections);
        return;
    }

    if (FountainFlags & FOUNTAIN_FLAG::BOUNCE)
    {
        Bounce({ newLoc, z }, availableDirections);
        return;
    }

    if (FountainFlags & FOUNTAIN_FLAG::SPLIT)
    {
        Split({ newLoc, z }, availableDirections);
        return;
    }

    Random({ newLoc, z }, availableDirections);
}

bool JumpingFountain::IsJumpingFountain(const JumpingFountainType newType, const CoordsXYZ& newLoc)
{
    const int32_t pathBitFlagMask = newType == JumpingFountainType::Snow ? PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW
                                                                         : PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER;

    TileElement* tileElement = map_get_first_element_at(newLoc);
    if (tileElement == nullptr)
        return false;
    do
    {
        if (tileElement->GetType() != TileElementType::Path)
            continue;
        if (tileElement->GetBaseZ() != newLoc.z)
            continue;
        if (tileElement->AsPath()->AdditionIsGhost())
            continue;
        if (!tileElement->AsPath()->HasAddition())
            continue;

        auto* pathBitEntry = tileElement->AsPath()->GetAdditionEntry();
        if (pathBitEntry != nullptr && pathBitEntry->flags & pathBitFlagMask)
        {
            return true;
        }
    } while (!(tileElement++)->IsLastForTile());

    return false;
}

void JumpingFountain::GoToEdge(const CoordsXYZ& newLoc, const int32_t availableDirections) const
{
    int32_t direction = (sprite_direction >> 3) << 1;
    if (availableDirections & (1 << direction))
    {
        CreateNext(newLoc, direction);
        return;
    }

    direction++;
    if (availableDirections & (1 << direction))
    {
        CreateNext(newLoc, direction);
        return;
    }

    const uint32_t randomIndex = scenario_rand();
    if ((randomIndex & 0xFFFF) < _FountainChanceOfStoppingEdgeMode)
    {
        return;
    }

    if (FountainFlags & FOUNTAIN_FLAG::SPLIT)
    {
        Split(newLoc, availableDirections);
        return;
    }

    direction = randomIndex & 7;
    while (!(availableDirections & (1 << direction)))
    {
        direction = (direction + 1) & 7;
    }

    CreateNext(newLoc, direction);
}

void JumpingFountain::Bounce(const CoordsXYZ& newLoc, const int32_t availableDirections)
{
    Iteration++;
    if (Iteration < 8)
    {
        int32_t direction = ((sprite_direction >> 3) ^ 2) << 1;
        if (availableDirections & (1 << direction))
        {
            CreateNext(newLoc, direction);
        }
        else
        {
            direction++;
            if (availableDirections & (1 << direction))
            {
                CreateNext(newLoc, direction);
            }
        }
    }
}

void JumpingFountain::Split(const CoordsXYZ& newLoc, int32_t availableDirections) const
{
    if (Iteration < 3)
    {
        const auto newType = GetType();
        int32_t direction = ((sprite_direction >> 3) ^ 2) << 1;
        availableDirections &= ~(1 << direction);
        availableDirections &= ~(1 << (direction + 1));

        for (direction = 0; direction < 8; direction++)
        {
            if (availableDirections & (1 << direction))
            {
                JumpingFountain::Create(
                    newType, newLoc, direction >> 1, FountainFlags & ~FOUNTAIN_FLAG::DIRECTION, Iteration + 1);
            }
            direction++;
            if (availableDirections & (1 << direction))
            {
                JumpingFountain::Create(
                    newType, newLoc, direction >> 1, FountainFlags | FOUNTAIN_FLAG::DIRECTION, Iteration + 1);
            }
        }
    }
}

void JumpingFountain::Random(const CoordsXYZ& newLoc, int32_t availableDirections) const
{
    const uint32_t randomIndex = scenario_rand();
    if ((randomIndex & 0xFFFF) >= _FountainChanceOfStoppingRandomMode)
    {
        int32_t direction = randomIndex & 7;
        while (!(availableDirections & (1 << direction)))
        {
            direction = (direction + 1) & 7;
        }
        CreateNext(newLoc, direction);
    }
}

void JumpingFountain::CreateNext(const CoordsXYZ& newLoc, int32_t direction) const
{
    const auto newType = GetType();
    int32_t newFlags = FountainFlags & ~FOUNTAIN_FLAG::DIRECTION;
    if (direction & 1)
    {
        newFlags |= FOUNTAIN_FLAG::DIRECTION;
    }
    JumpingFountain::Create(newType, newLoc, direction >> 1, newFlags, Iteration);
}

void JumpingFountain::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);
    stream << frame;
    stream << FountainType;
    stream << NumTicksAlive;
    stream << FountainFlags;
    stream << TargetX;
    stream << TargetY;
    stream << Iteration;
}

void JumpingFountain::Paint(paint_session& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    // TODO: Move into sprites.h
    constexpr uint32_t JumpingFountainSnowBaseImage = 23037;
    constexpr uint32_t JumpingFountainWaterBaseImage = 22973;

    rct_drawpixelinfo& dpi = session.DPI;
    if (dpi.zoom_level > ZoomLevel{ 0 })
    {
        return;
    }

    uint16_t height = z + 6;
    imageDirection = imageDirection / 8;

    // Fountain is firing anti clockwise
    bool reversed = (FountainFlags & FOUNTAIN_FLAG::DIRECTION);
    // Fountain rotation
    bool rotated = (sprite_direction / 16) & 1;
    bool isAntiClockwise = (imageDirection / 2) & 1; // Clockwise or Anti-clockwise

    // These cancel each other out
    if (reversed != rotated)
    {
        isAntiClockwise = !isAntiClockwise;
    }

    uint32_t baseImageId = (FountainType == JumpingFountainType::Snow) ? JumpingFountainSnowBaseImage
                                                                       : JumpingFountainWaterBaseImage;
    uint32_t imageId = baseImageId + imageDirection * 16 + frame;
    constexpr std::array antiClockWiseBoundingBoxes = {
        CoordsXY{ -COORDS_XY_STEP, -3 },
        CoordsXY{ 0, -3 },
    };
    constexpr std::array clockWiseBoundingBoxes = {
        CoordsXY{ -COORDS_XY_STEP, 3 },
        CoordsXY{ 0, 3 },
    };

    auto bb = isAntiClockwise ? antiClockWiseBoundingBoxes : clockWiseBoundingBoxes;

    PaintAddImageAsParentRotated(
        session, imageDirection, imageId, { 0, 0, height }, { 32, 1, 3 }, { bb[imageDirection & 1], height });
}
