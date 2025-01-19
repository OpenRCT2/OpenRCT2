/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "Duck.h"

#include "../Game.h"
#include "../GameState.h"
#include "../audio/audio.h"
#include "../core/DataSerialiser.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "../scenario/Scenario.h"
#include "../sprites.h"
#include "../world/tile_element/SurfaceElement.h"
#include "EntityRegistry.h"

#include <iterator>
#include <limits>

using namespace OpenRCT2;

constexpr int32_t kDuckMaxStates = 5;

// clang-format off
static constexpr CoordsXY kDuckMoveOffset[] =
{
    { -1,  0 },
    {  0,  1 },
    {  1,  0 },
    {  0, -1 },
};

static constexpr uint8_t kDuckAnimationFlyToWater[] =
{
    8, 9, 10, 11, 12, 13
};

static constexpr uint8_t kDuckAnimationSwim[] =
{
    0
};

static constexpr uint8_t kDuckAnimationDrink[] =
{
    1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0xFF
};

static constexpr uint8_t kDuckAnimationDoubleDrink[] =
{
    4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6,
    6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 0, 0, 0, 0, 0xFF
};

static constexpr uint8_t kDuckAnimationFlyAway[] =
{
    8, 9, 10, 11, 12, 13
};
// clang-format on

static constexpr const uint8_t* kDuckAnimations[] = {
    kDuckAnimationFlyToWater,  //  DuckState::FlyToWater,
    kDuckAnimationSwim,        //  DuckState::Swim,
    kDuckAnimationDrink,       //  DuckState::Drink,
    kDuckAnimationDoubleDrink, //  DuckState::DoubleDrink,
    kDuckAnimationFlyAway,     //  DuckState::FlyAway,
};

template<>
bool EntityBase::Is<Duck>() const
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
    const auto currentTicks = GetGameState().CurrentTicks;

    if ((currentTicks & 3) != 0)
        return;

    frame++;
    if (frame >= std::size(kDuckAnimationFlyToWater))
    {
        frame = 0;
    }

    Invalidate();
    int32_t manhattanDistance = abs(target_x - x) + abs(target_y - y);
    int32_t direction = Orientation >> 3;
    auto destination = CoordsXYZ{ CoordsXY{ x, y } + kDuckMoveOffset[direction], 0 };
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
    const auto currentTicks = GetGameState().CurrentTicks;

    if (((currentTicks + Id.ToUnderlying()) & 3) != 0)
        return;

    uint32_t randomNumber = ScenarioRand();
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
        int32_t currentMonth = GetDate().GetMonth();
        if (currentMonth >= MONTH_SEPTEMBER && (randomNumber >> 16) < 218)
        {
            state = DuckState::FlyAway;
            UpdateFlyAway();
        }
        else
        {
            Invalidate();
            int16_t landZ = TileElementHeight({ x, y });
            int16_t waterZ = TileElementWaterHeight({ x, y });

            if (z < landZ || waterZ == 0)
            {
                state = DuckState::FlyAway;
                UpdateFlyAway();
            }
            else
            {
                z = waterZ;
                randomNumber = ScenarioRand();
                if ((randomNumber & 0xFFFF) <= 0xAAA)
                {
                    randomNumber >>= 16;
                    Orientation = randomNumber & 0x18;
                }

                int32_t direction = Orientation >> 3;
                auto destination = CoordsXYZ{ CoordsXY{ x, y } + kDuckMoveOffset[direction], 0 };
                landZ = TileElementHeight(destination);
                waterZ = TileElementWaterHeight(destination);

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
    if (kDuckAnimationDrink[frame] == 0xFF)
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
    if (kDuckAnimationDoubleDrink[frame] == 0xFF)
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
    if ((GetGameState().CurrentTicks & 3) == 0)
    {
        frame++;
        if (frame >= std::size(kDuckAnimationFlyAway))
        {
            frame = 0;
        }

        Invalidate();

        int32_t direction = Orientation >> 3;
        auto destination = CoordsXYZ{ x + (kDuckMoveOffset[direction].x * 2), y + (kDuckMoveOffset[direction].y * 2),
                                      std::min<int32_t>(z + 2, 496) };
        if (MapIsLocationValid(destination))
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
    if (EnumValue(state) < kDuckMaxStates)
    {
        // TODO: Check frame is in range
        uint8_t imageOffset = kDuckAnimations[EnumValue(state)][frame];
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

    int32_t offsetXY = ScenarioRand() & 0x1E;
    targetPos.x += offsetXY;
    targetPos.y += offsetXY;

    duck->SpriteData.Width = 9;
    duck->SpriteData.HeightMin = 12;
    duck->SpriteData.HeightMax = 9;
    duck->target_x = targetPos.x;
    duck->target_y = targetPos.y;
    uint8_t direction = ScenarioRand() & 3;
    switch (direction)
    {
        case 0:
            targetPos.x = GetMapSizeMaxXY().x - (ScenarioRand() & 0x3F);
            break;
        case 1:
            targetPos.y = ScenarioRand() & 0x3F;
            break;
        case 2:
            targetPos.x = ScenarioRand() & 0x3F;
            break;
        case 3:
            targetPos.y = GetMapSizeMaxXY().y - (ScenarioRand() & 0x3F);
            break;
    }
    duck->Orientation = direction << 3;
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

void Duck::Paint(PaintSession& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    DrawPixelInfo& dpi = session.DPI;
    if (dpi.zoom_level > ZoomLevel{ 1 })
        return;

    uint32_t imageId = GetFrameImage(imageDirection);
    if (imageId != 0)
    {
        PaintAddImageAsParent(session, ImageId(imageId), { 0, 0, z }, { 1, 1, 0 });
    }
}
