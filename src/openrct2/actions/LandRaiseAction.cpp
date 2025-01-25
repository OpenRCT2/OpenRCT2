/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LandRaiseAction.h"

#include "../Context.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../actions/LandSetHeightAction.h"
#include "../audio/audio.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideConstruction.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SurfaceData.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"

using namespace OpenRCT2;

LandRaiseAction::LandRaiseAction(const CoordsXY& coords, MapRange range, uint8_t selectionType)
    : _coords(coords)
    , _range(range)
    , _selectionType(selectionType)
{
}

void LandRaiseAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_coords);
    visitor.Visit(_range);
    visitor.Visit("selectionType", _selectionType);
}

uint16_t LandRaiseAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void LandRaiseAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_coords) << DS_TAG(_range) << DS_TAG(_selectionType);
}

GameActions::Result LandRaiseAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result LandRaiseAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result LandRaiseAction::QueryExecute(bool isExecuting) const
{
    auto res = GameActions::Result();
    size_t tableRow = _selectionType;

    // The selections between MAP_SELECT_TYPE_FULL and MAP_SELECT_TYPE_EDGE_0 are not included in the tables
    if (_selectionType >= MAP_SELECT_TYPE_EDGE_0 && _selectionType <= MAP_SELECT_TYPE_EDGE_3)
        tableRow -= MAP_SELECT_TYPE_EDGE_0 - MAP_SELECT_TYPE_FULL - 1;

    auto validRange = ClampRangeWithinMap(_range);

    res.Position = { _coords.x, _coords.y, TileElementHeight(_coords) };
    res.Expenditure = ExpenditureType::Landscaping;

    if (isExecuting)
    {
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, { _coords.x, _coords.y, TileElementHeight(_coords) });
    }

    uint8_t minHeight = MapGetLowestLandHeight(validRange);
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

            uint8_t height = surfaceElement->BaseHeight;

            if (height > minHeight)
                continue;

            uint8_t currentSlope = surfaceElement->GetSlope();
            uint8_t newSlope = RaiseSurfaceCornerFlags(tableRow, currentSlope);
            if (newSlope & kTileSlopeRaiseOrLowerBaseHeight)
            {
                if (height + 2 > UINT8_MAX)
                {
                    height = UINT8_MAX;
                }
                else
                {
                    height += 2;
                }
            }
            newSlope &= kTileSlopeMask;

            auto landSetHeightAction = LandSetHeightAction({ x, y }, height, newSlope);
            landSetHeightAction.SetFlags(GetFlags());
            auto result = isExecuting ? GameActions::ExecuteNested(&landSetHeightAction)
                                      : GameActions::QueryNested(&landSetHeightAction);
            if (result.Error == GameActions::Status::Ok)
            {
                res.Cost += result.Cost;
            }
            else
            {
                result.ErrorTitle = STR_CANT_RAISE_LAND_HERE;
                return result;
            }
        }
    }

    if (!withinOwnership)
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_RAISE_LAND_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    return res;
}
