/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WaterLowerAction.h"

#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../audio/Audio.h"
#include "../ride/RideConstruction.h"
#include "../world/Map.h"
#include "../world/tile_element/SurfaceElement.h"
#include "WaterSetHeightAction.h"

namespace OpenRCT2::GameActions
{
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

    Result WaterLowerAction::Query(GameState_t& gameState) const
    {
        return QueryExecute(gameState, false);
    }

    Result WaterLowerAction::Execute(GameState_t& gameState) const
    {
        return QueryExecute(gameState, true);
    }

    Result WaterLowerAction::QueryExecute(GameState_t& gameState, bool isExecuting) const
    {
        auto res = Result();

        auto validRange = ClampRangeWithinMap(_range);
        res.position.x = ((validRange.GetX1() + validRange.GetX2()) / 2) + 16;
        res.position.y = ((validRange.GetY1() + validRange.GetY2()) / 2) + 16;
        int16_t z = TileElementHeight(res.position);
        int16_t waterHeight = TileElementWaterHeight(res.position);
        if (waterHeight != 0)
        {
            z = waterHeight;
        }
        res.position.z = z;
        res.expenditure = ExpenditureType::landscaping;

        uint8_t minHeight = GetLowestHeight(gameState, validRange);
        bool hasChanged = false;
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

                uint8_t height = surfaceElement->GetWaterHeight() / kCoordsZStep;
                if (height == 0)
                    continue;

                if (height < minHeight)
                    continue;

                height -= 2;
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
                    result.errorTitle = STR_CANT_LOWER_WATER_LEVEL_HERE;
                    return result;
                }
            }
        }

        if (!withinOwnership)
        {
            return Result(Status::disallowed, STR_CANT_LOWER_WATER_LEVEL_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }

        if (isExecuting && hasChanged)
        {
            Audio::Play3D(Audio::SoundId::layingOutWater, res.position);
        }
        // Force ride construction to recheck area
        _currentTrackSelectionFlags.set(TrackSelectionFlag::recheck);

        return res;
    }

    uint8_t WaterLowerAction::GetLowestHeight(const GameState_t& gameState, const MapRange& validRange) const
    {
        // The lowest height to lower the water to is the highest water level in the selection
        uint8_t minHeight{ 0 };
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
} // namespace OpenRCT2::GameActions
