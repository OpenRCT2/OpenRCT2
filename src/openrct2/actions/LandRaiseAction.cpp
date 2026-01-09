/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../audio/Audio.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideConstruction.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Map.h"
#include "../world/MapSelection.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SurfaceData.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"

namespace OpenRCT2::GameActions
{
    LandRaiseAction::LandRaiseAction(const CoordsXY& coords, MapRange range, MapSelectType selectionType)
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

    Result LandRaiseAction::Query(GameState_t& gameState) const
    {
        return QueryExecute(gameState, false);
    }

    Result LandRaiseAction::Execute(GameState_t& gameState) const
    {
        return QueryExecute(gameState, true);
    }

    Result LandRaiseAction::QueryExecute(GameState_t& gameState, bool isExecuting) const
    {
        auto res = Result();
        size_t tableRow = EnumValue(_selectionType);

        // The selections between MapSelectType::full and MapSelectType::edge0 are not included in the tables
        if (_selectionType >= MapSelectType::edge0 && _selectionType <= MapSelectType::edge3)
            tableRow -= EnumValue(MapSelectType::edge0) - EnumValue(MapSelectType::full) - 1;

        auto validRange = ClampRangeWithinMap(_range);

        res.position = { _coords.x, _coords.y, TileElementHeight(_coords) };
        res.expenditure = ExpenditureType::landscaping;

        if (isExecuting)
        {
            Audio::Play3D(Audio::SoundId::placeItem, { _coords.x, _coords.y, TileElementHeight(_coords) });
        }

        uint8_t minHeight = MapGetLowestLandHeight(validRange);
        bool withinOwnership = false;

        for (int32_t y = validRange.GetY1(); y <= validRange.GetY2(); y += kCoordsXYStep)
        {
            for (int32_t x = validRange.GetX1(); x <= validRange.GetX2(); x += kCoordsXYStep)
            {
                if (!LocationValid({ x, y }))
                    continue;
                auto* surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });
                if (surfaceElement == nullptr)
                    continue;

                if (gLegacyScene != LegacyScene::scenarioEditor && !gameState.cheats.sandboxMode)
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
                auto result = isExecuting ? ExecuteNested(&landSetHeightAction, gameState)
                                          : QueryNested(&landSetHeightAction, gameState);
                if (result.error == Status::ok)
                {
                    res.cost += result.cost;
                }
                else
                {
                    result.errorTitle = STR_CANT_RAISE_LAND_HERE;
                    return result;
                }
            }
        }

        if (!withinOwnership)
        {
            return Result(Status::disallowed, STR_CANT_RAISE_LAND_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }

        // Force ride construction to recheck area
        _currentTrackSelectionFlags.set(TrackSelectionFlag::recheck);

        return res;
    }
} // namespace OpenRCT2::GameActions
