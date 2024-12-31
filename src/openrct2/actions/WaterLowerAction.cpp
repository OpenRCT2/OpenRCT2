/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WaterLowerAction.h"

#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../audio/audio.h"
#include "../ride/RideConstruction.h"
#include "../world/tile_element/SurfaceElement.h"
#include "WaterSetHeightAction.h"

using namespace OpenRCT2;

WaterLowerAction::WaterLowerAction(MapRange range)
    : _range(range)
{
}

void WaterLowerAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_range);
}

uint16_t WaterLowerAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void WaterLowerAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_range);
}

GameActions::Result WaterLowerAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result WaterLowerAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result WaterLowerAction::QueryExecute(bool isExecuting) const
{
    auto res = GameActions::Result();

    auto validRange = ClampRangeWithinMap(_range);
    res.Position.x = ((validRange.GetLeft() + validRange.GetRight()) / 2) + 16;
    res.Position.y = ((validRange.GetTop() + validRange.GetBottom()) / 2) + 16;
    int16_t z = TileElementHeight(res.Position);
    int16_t waterHeight = TileElementWaterHeight(res.Position);
    if (waterHeight != 0)
    {
        z = waterHeight;
    }
    res.Position.z = z;
    res.Expenditure = ExpenditureType::Landscaping;

    uint8_t minHeight = GetLowestHeight(validRange);
    bool hasChanged = false;
    bool withinOwnership = false;
    for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += kCoordsXYStep)
    {
        for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += kCoordsXYStep)
        {
            if (!LocationValid({ x, y }))
                continue;

            auto* surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });
            if (surfaceElement == nullptr)
                continue;

            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !GetGameState().Cheats.sandboxMode)
            {
                if (!MapIsLocationInPark(CoordsXY{ x, y }))
                {
                    continue;
                }
            }
            withinOwnership = true;

            uint8_t height = surfaceElement->GetWaterHeight() / kCoordsZStep;
            if (height == 0)
                continue;

            if (height < minHeight)
                continue;

            height -= 2;
            auto waterSetHeightAction = WaterSetHeightAction({ x, y }, height);
            waterSetHeightAction.SetFlags(GetFlags());
            auto result = isExecuting ? GameActions::ExecuteNested(&waterSetHeightAction)
                                      : GameActions::QueryNested(&waterSetHeightAction);
            if (result.Error == GameActions::Status::Ok)
            {
                res.Cost += result.Cost;
                hasChanged = true;
            }
            else
            {
                result.ErrorTitle = STR_CANT_LOWER_WATER_LEVEL_HERE;
                return result;
            }
        }
    }

    if (!withinOwnership)
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_CANT_LOWER_WATER_LEVEL_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        ;
    }

    if (isExecuting && hasChanged)
    {
        Audio::Play3D(Audio::SoundId::LayingOutWater, res.Position);
    }
    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    return res;
}

uint8_t WaterLowerAction::GetLowestHeight(const MapRange& validRange) const
{
    // The lowest height to lower the water to is the highest water level in the selection
    uint8_t minHeight{ 0 };
    for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += kCoordsXYStep)
    {
        for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += kCoordsXYStep)
        {
            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !GetGameState().Cheats.sandboxMode)
            {
                if (!MapIsLocationInPark(CoordsXY{ x, y }))
                {
                    continue;
                }
            }

            auto* surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });
            if (surfaceElement == nullptr)
                continue;

            uint8_t height = surfaceElement->GetWaterHeight() / kCoordsZStep;
            if (height == 0)
                continue;

            if (height > minHeight)
            {
                minHeight = height;
            }
        }
    }

    return minHeight;
}
