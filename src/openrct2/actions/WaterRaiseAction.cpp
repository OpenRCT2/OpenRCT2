/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WaterRaiseAction.h"

#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../audio/Audio.h"
#include "../ride/RideConstruction.h"
#include "../world/Map.h"
#include "../world/tile_element/SurfaceElement.h"
#include "WaterSetHeightAction.h"

namespace OpenRCT2::GameActions
{
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

    Result WaterRaiseAction::Query(GameState_t& gameState) const
    {
        return QueryExecute(gameState, false);
    }

    Result WaterRaiseAction::Execute(GameState_t& gameState) const
    {
        return QueryExecute(gameState, true);
    }

    Result WaterRaiseAction::QueryExecute(GameState_t& gameState, bool isExecuting) const
    {
        auto res = Result();

        auto validRange = ClampRangeWithinMap(_range);
        res.position.x = ((validRange.GetX1() + validRange.GetX2()) / 2) + 16;
        res.position.y = ((validRange.GetY1() + validRange.GetY2()) / 2) + 16;
        int32_t z = TileElementHeight(res.position);
        int16_t waterHeight = TileElementWaterHeight(res.position);
        if (waterHeight != 0)
        {
            z = waterHeight;
        }
        res.position.z = z;
        res.expenditure = ExpenditureType::landscaping;

        auto maxHeight = GetHighestHeight(gameState, validRange) / kCoordsZStep;
        bool hasChanged = false;
        bool withinOwnership = false;
        for (int32_t y = validRange.GetY1(); y <= validRange.GetY2(); y += kCoordsXYStep)
        {
            for (int32_t x = validRange.GetX1(); x <= validRange.GetX2(); x += kCoordsXYStep)
            {
                if (!LocationValid({ x, y }))
                    continue;

                auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });
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
                auto result = isExecuting ? ExecuteNested(&waterSetHeightAction, gameState)
                                          : QueryNested(&waterSetHeightAction, gameState);
                if (result.error == Status::ok)
                {
                    res.cost += result.cost;
                    hasChanged = true;
                }
                else
                {
                    result.errorTitle = STR_CANT_RAISE_WATER_LEVEL_HERE;
                    return result;
                }
            }
        }

        if (!withinOwnership)
        {
            return Result(Status::disallowed, STR_CANT_RAISE_WATER_LEVEL_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }

        if (isExecuting && hasChanged)
        {
            Audio::Play3D(Audio::SoundId::layingOutWater, res.position);
        }
        // Force ride construction to recheck area
        _currentTrackSelectionFlags.set(TrackSelectionFlag::recheck);

        return res;
    }

    uint16_t WaterRaiseAction::GetHighestHeight(const GameState_t& gameState, const MapRange& validRange) const
    {
        // The highest height to raise the water to is the lowest water level in the selection
        uint16_t maxHeight = 255 * kCoordsZStep;
        for (int32_t y = validRange.GetY1(); y <= validRange.GetY2(); y += kCoordsXYStep)
        {
            for (int32_t x = validRange.GetX1(); x <= validRange.GetX2(); x += kCoordsXYStep)
            {
                if (gLegacyScene != LegacyScene::scenarioEditor && !gameState.cheats.sandboxMode)
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
} // namespace OpenRCT2::GameActions
