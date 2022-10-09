/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WaterSetHeightAction.h"

#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../world/ConstructionClearance.h"
#include "../world/Park.h"
#include "../world/Surface.h"

WaterSetHeightAction::WaterSetHeightAction(const CoordsXY& coords, uint8_t height)
    : _coords(coords)
    , _height(height)
{
}

uint16_t WaterSetHeightAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void WaterSetHeightAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_coords) << DS_TAG(_height);
}

GameActions::Result WaterSetHeightAction::Query() const
{
    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = { _coords, _height * COORDS_Z_STEP };

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode
        && gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES)
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_NONE, STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY);
    }

    StringId errorMsg = CheckParameters();
    if (errorMsg != STR_NONE)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, errorMsg);
    }

    if (!LocationValid(_coords))
    {
        return GameActions::Result(GameActions::Status::NotOwned, STR_NONE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
    {
        if (!map_is_location_in_park(_coords))
        {
            return GameActions::Result(GameActions::Status::Disallowed, STR_NONE, STR_LAND_NOT_OWNED_BY_PARK);
        }
    }

    SurfaceElement* surfaceElement = map_get_surface_element_at(_coords);
    if (surfaceElement == nullptr)
    {
        log_error("Could not find surface element at: x %u, y %u", _coords.x, _coords.y);
        return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);
    }

    int32_t zHigh = surfaceElement->GetBaseZ();
    int32_t zLow = _height * COORDS_Z_STEP;
    if (surfaceElement->GetWaterHeight() > 0)
    {
        zHigh = surfaceElement->GetWaterHeight();
    }
    if (zLow > zHigh)
    {
        int32_t temp = zHigh;
        zHigh = zLow;
        zLow = temp;
    }

    if (auto res2 = MapCanConstructAt({ _coords, zLow, zHigh }, { 0b1111, 0b1111 }); res2.Error != GameActions::Status::Ok)
    {
        return res2;
    }
    if (surfaceElement->HasTrackThatNeedsWater())
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_NONE, STR_NONE);
    }

    res.Cost = 250;

    return res;
}

GameActions::Result WaterSetHeightAction::Execute() const
{
    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = { _coords, _height * COORDS_Z_STEP };

    int32_t surfaceHeight = tile_element_height(_coords);
    FootpathRemoveLitter({ _coords, surfaceHeight });
    if (!gCheatsDisableClearanceChecks)
        WallRemoveAtZ({ _coords, surfaceHeight });

    SurfaceElement* surfaceElement = map_get_surface_element_at(_coords);
    if (surfaceElement == nullptr)
    {
        log_error("Could not find surface element at: x %u, y %u", _coords.x, _coords.y);
        return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);
    }

    if (_height > surfaceElement->base_height)
    {
        surfaceElement->SetWaterHeight(_height * COORDS_Z_STEP);
    }
    else
    {
        surfaceElement->SetWaterHeight(0);
    }
    map_invalidate_tile_full(_coords);

    res.Cost = 250;

    return res;
}

StringId WaterSetHeightAction::CheckParameters() const
{
    auto mapSizeMax = GetMapSizeMaxXY();
    if (_coords.x > mapSizeMax.x || _coords.y > mapSizeMax.y)
    {
        return STR_OFF_EDGE_OF_MAP;
    }

    if (_height < MINIMUM_WATER_HEIGHT)
    {
        return STR_TOO_LOW;
    }

    if (_height > MAXIMUM_WATER_HEIGHT)
    {
        return STR_TOO_HIGH;
    }

    return STR_NONE;
}
