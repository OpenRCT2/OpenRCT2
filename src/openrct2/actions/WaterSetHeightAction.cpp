/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WaterSetHeightAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../world/ConstructionClearance.h"
#include "../world/Footpath.h"
#include "../world/Park.h"
#include "../world/Wall.h"
#include "../world/tile_element/SurfaceElement.h"

using namespace OpenRCT2;

WaterSetHeightAction::WaterSetHeightAction(const CoordsXY& coords, uint8_t height)
    : _coords(coords)
    , _height(height)
{
}

void WaterSetHeightAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_coords);
    visitor.Visit("height", _height);
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
    res.Position = { _coords, _height * kCoordsZStep };

    auto& gameState = GetGameState();
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gameState.Cheats.sandboxMode
        && gameState.Park.Flags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES)
    {
        return GameActions::Result(GameActions::Status::Disallowed, kStringIdNone, STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY);
    }

    StringId errorMsg = CheckParameters();
    if (errorMsg != kStringIdNone)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, kStringIdNone, errorMsg);
    }

    if (!LocationValid(_coords))
    {
        return GameActions::Result(GameActions::Status::NotOwned, kStringIdNone, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gameState.Cheats.sandboxMode)
    {
        if (!MapIsLocationInPark(_coords))
        {
            return GameActions::Result(GameActions::Status::Disallowed, kStringIdNone, STR_LAND_NOT_OWNED_BY_PARK);
        }
    }

    SurfaceElement* surfaceElement = MapGetSurfaceElementAt(_coords);
    if (surfaceElement == nullptr)
    {
        LOG_ERROR("No surface element at: x %u, y %u", _coords.x, _coords.y);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
    }

    int32_t zHigh = surfaceElement->GetBaseZ();
    int32_t zLow = _height * kCoordsZStep;
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
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_ERR_INVALID_PARAMETER, STR_ERR_TRACK_ON_THIS_TILE_NEEDS_WATER);
    }

    res.Cost = 250;

    return res;
}

GameActions::Result WaterSetHeightAction::Execute() const
{
    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = { _coords, _height * kCoordsZStep };

    int32_t surfaceHeight = TileElementHeight(_coords);
    FootpathRemoveLitter({ _coords, surfaceHeight });
    if (!GetGameState().Cheats.disableClearanceChecks)
        WallRemoveAtZ({ _coords, surfaceHeight });

    SurfaceElement* surfaceElement = MapGetSurfaceElementAt(_coords);
    if (surfaceElement == nullptr)
    {
        LOG_ERROR("No surface element at: x %u, y %u", _coords.x, _coords.y);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
    }

    if (_height > surfaceElement->BaseHeight)
    {
        surfaceElement->SetWaterHeight(_height * kCoordsZStep);
    }
    else
    {
        surfaceElement->SetWaterHeight(0);
    }
    MapInvalidateTileFull(_coords);

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

    if (_height < kMinimumWaterHeight)
    {
        return STR_TOO_LOW;
    }

    if (_height > kMaximumWaterHeight)
    {
        return STR_TOO_HIGH;
    }

    return kStringIdNone;
}
