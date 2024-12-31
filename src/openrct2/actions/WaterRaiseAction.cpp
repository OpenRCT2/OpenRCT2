/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WaterRaiseAction.h"

#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../audio/audio.h"
#include "../ride/RideConstruction.h"
#include "../world/tile_element/SurfaceElement.h"
#include "WaterSetHeightAction.h"

using namespace OpenRCT2;

WaterRaiseAction::WaterRaiseAction(MapRange range)
    : _range(range)
{
}

void WaterRaiseAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_range);
}

uint16_t WaterRaiseAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void WaterRaiseAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_range);
}

GameActions::Result WaterRaiseAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result WaterRaiseAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result WaterRaiseAction::QueryExecute(bool isExecuting) const
{
    auto res = GameActions::Result();

    auto validRange = ClampRangeWithinMap(_range);
    res.Position.x = ((validRange.GetLeft() + validRange.GetRight()) / 2) + 16;
    res.Position.y = ((validRange.GetTop() + validRange.GetBottom()) / 2) + 16;
    int32_t z = TileElementHeight(res.Position);
    int16_t waterHeight = TileElementWaterHeight(res.Position);
    if (waterHeight != 0)
    {
        z = waterHeight;
    }
    res.Position.z = z;
    res.Expenditure = ExpenditureType::Landscaping;

    auto maxHeight = GetHighestHeight(validRange) / kCoordsZStep;
    bool hasChanged = false;
    bool withinOwnership = false;
    for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += kCoordsXYStep)
    {
        for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += kCoordsXYStep)
        {
            if (!LocationValid({ x, y }))
                continue;

            auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });
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

            if (surfaceElement->BaseHeight > maxHeight)
                continue;

            if (height != 0)
            {
                if (height > maxHeight)
                    continue;
                if (height + 2 > UINT8_MAX)
                {
                    height = UINT8_MAX;
                }
                else
                {
                    height += 2;
                }
            }
            else
            {
                height = surfaceElement->BaseHeight + 2;
            }
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
                result.ErrorTitle = STR_CANT_RAISE_WATER_LEVEL_HERE;
                return result;
            }
        }
    }

    if (!withinOwnership)
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_CANT_RAISE_WATER_LEVEL_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (isExecuting && hasChanged)
    {
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::LayingOutWater, res.Position);
    }
    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    return res;
}

uint16_t WaterRaiseAction::GetHighestHeight(const MapRange& validRange) const
{
    // The highest height to raise the water to is the lowest water level in the selection
    uint16_t maxHeight = 255 * kCoordsZStep;
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

            auto height = surfaceElement->GetBaseZ();
            if (surfaceElement->GetWaterHeight() > 0)
            {
                height = surfaceElement->GetWaterHeight();
            }

            if (maxHeight > height)
            {
                maxHeight = height;
            }
        }
    }

    return maxHeight;
}
