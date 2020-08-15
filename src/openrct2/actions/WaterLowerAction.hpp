/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "../audio/audio.h"
#include "GameAction.h"
#include "WaterSetHeightAction.hpp"

DEFINE_GAME_ACTION(WaterLowerAction, GAME_COMMAND_LOWER_WATER, GameActionResult)
{
private:
    MapRange _range;

public:
    WaterLowerAction()
    {
    }
    WaterLowerAction(MapRange range)
        : _range(range)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_range);
    }

    GameActionResult::Ptr Query() const override
    {
        return QueryExecute(false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return QueryExecute(true);
    }

private:
    GameActionResult::Ptr QueryExecute(bool isExecuting) const
    {
        auto res = MakeResult();

        // Keep big coordinates within map boundaries
        auto aX = std::max<decltype(_range.GetLeft())>(32, _range.GetLeft());
        auto bX = std::min<decltype(_range.GetRight())>(gMapSizeMaxXY, _range.GetRight());
        auto aY = std::max<decltype(_range.GetTop())>(32, _range.GetTop());
        auto bY = std::min<decltype(_range.GetBottom())>(gMapSizeMaxXY, _range.GetBottom());

        MapRange validRange = MapRange{ aX, aY, bX, bY };

        res->Position.x = ((validRange.GetLeft() + validRange.GetRight()) / 2) + 16;
        res->Position.y = ((validRange.GetTop() + validRange.GetBottom()) / 2) + 16;
        int16_t z = tile_element_height(res->Position);
        int16_t waterHeight = tile_element_water_height(res->Position);
        if (waterHeight != 0)
        {
            z = waterHeight;
        }
        res->Position.z = z;
        res->Expenditure = ExpenditureType::Landscaping;

        uint8_t minHeight = GetLowestHeight(validRange);
        bool hasChanged = false;
        bool withinOwnership = false;
        for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += COORDS_XY_STEP)
        {
            for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += COORDS_XY_STEP)
            {
                if (!LocationValid({ x, y }))
                    continue;

                auto* surfaceElement = map_get_surface_element_at(CoordsXY{ x, y });
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
                if (height == 0)
                    continue;

                if (height < minHeight)
                    continue;

                height -= 2;
                auto waterSetHeightAction = WaterSetHeightAction({ x, y }, height);
                waterSetHeightAction.SetFlags(GetFlags());
                auto result = isExecuting ? GameActions::ExecuteNested(&waterSetHeightAction)
                                          : GameActions::QueryNested(&waterSetHeightAction);
                if (result->Error == GA_ERROR::OK)
                {
                    res->Cost += result->Cost;
                    hasChanged = true;
                }
                else
                {
                    result->ErrorTitle = STR_CANT_LOWER_WATER_LEVEL_HERE;
                    return result;
                }
            }
        }

        if (!withinOwnership)
        {
            GameActionResult::Ptr ownerShipResult = std::make_unique<GameActionResult>(
                GA_ERROR::DISALLOWED, STR_LAND_NOT_OWNED_BY_PARK);
            ownerShipResult->ErrorTitle = STR_CANT_LOWER_WATER_LEVEL_HERE;
            return ownerShipResult;
        }

        if (isExecuting && hasChanged)
        {
            audio_play_sound_at_location(SoundId::LayingOutWater, res->Position);
        }
        // Force ride construction to recheck area
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

        return res;
    }

private:
    uint8_t GetLowestHeight(MapRange validRange) const
    {
        // The lowest height to lower the water to is the highest water level in the selection
        uint8_t minHeight{ 0 };
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

                uint8_t height = surfaceElement->GetWaterHeight() / COORDS_Z_STEP;
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
};
