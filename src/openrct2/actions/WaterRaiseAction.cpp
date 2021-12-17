/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WaterRaiseAction.h"

#include "../OpenRCT2.h"
#include "../audio/audio.h"
#include "../ride/RideConstruction.h"
#include "WaterSetHeightAction.h"

WaterRaiseAction::WaterRaiseAction(MapRange range)
    : _range(range)
{
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
    int32_t z = tile_element_height(res.Position);
    int16_t waterHeight = tile_element_water_height(res.Position);
    if (waterHeight != 0)
    {
        z = waterHeight;
    }
    res.Position.z = z;
    res.Expenditure = ExpenditureType::Landscaping;

    auto maxHeight = GetHighestHeight(validRange) / COORDS_Z_STEP;
    bool hasChanged = false;
    bool withinOwnership = false;
    for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += COORDS_XY_STEP)
    {
        for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += COORDS_XY_STEP)
        {
            if (!LocationValid({ x, y }))
                continue;

            auto surfaceElement = map_get_surface_element_at(CoordsXY{ x, y });
            if (surfaceElement == nullptr)
                continue;

            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
            {
                if (!map_is_location_in_park(CoordsXY{ x, y }))
                {
                    continue;
                }
            }
            withinOwnership = true;

            uint8_t height = surfaceElement->GetWaterHeight() / COORDS_Z_STEP;

            if (surfaceElement->base_height > maxHeight)
                continue;

            if (height != 0)
            {
                if (height > maxHeight)
                    continue;
                height += 2;
            }
            else
            {
                height = surfaceElement->base_height + 2;
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

uint16_t WaterRaiseAction::GetHighestHeight(MapRange validRange) const
{
    // The highest height to raise the water to is the lowest water level in the selection
    uint16_t maxHeight = 255 * COORDS_Z_STEP;
    for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += COORDS_XY_STEP)
    {
        for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += COORDS_XY_STEP)
        {
            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
            {
                if (!map_is_location_in_park(CoordsXY{ x, y }))
                {
                    continue;
                }
            }

            auto* surfaceElement = map_get_surface_element_at(CoordsXY{ x, y });
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
