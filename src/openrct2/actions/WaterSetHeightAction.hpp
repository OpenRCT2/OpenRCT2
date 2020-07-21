/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../world/Park.h"
#include "../world/Surface.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(WaterSetHeightAction, GAME_COMMAND_SET_WATER_HEIGHT, GameActionResult)
{
private:
    CoordsXY _coords;
    uint8_t _height;

public:
    WaterSetHeightAction()
    {
    }
    WaterSetHeightAction(const CoordsXY& coords, uint8_t height)
        : _coords(coords)
        , _height(height)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_coords) << DS_TAG(_height);
    }

    GameActionResult::Ptr Query() const override
    {
        auto res = MakeResult();
        res->Expenditure = ExpenditureType::Landscaping;
        res->Position = { _coords, _height * COORDS_Z_STEP };

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode
            && gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES)
        {
            return MakeResult(GA_ERROR::DISALLOWED, STR_NONE, STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY);
        }

        rct_string_id errorMsg = CheckParameters();
        if (errorMsg != STR_NONE)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE, errorMsg);
        }

        if (!LocationValid(_coords))
        {
            return MakeResult(GA_ERROR::NOT_OWNED, STR_NONE, STR_LAND_NOT_OWNED_BY_PARK);
        }

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
        {
            if (!map_is_location_in_park(_coords))
            {
                return MakeResult(GA_ERROR::DISALLOWED, STR_NONE, STR_LAND_NOT_OWNED_BY_PARK);
            }
        }

        SurfaceElement* surfaceElement = map_get_surface_element_at(_coords);
        if (surfaceElement == nullptr)
        {
            log_error("Could not find surface element at: x %u, y %u", _coords.x, _coords.y);
            return MakeResult(GA_ERROR::UNKNOWN, STR_NONE);
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

        if (auto res2 = MapCanConstructAt({ _coords, zLow, zHigh }, { 0b1111, 0b1111 }); res2->Error != GA_ERROR::OK)
        {
            return MakeResult(
                GA_ERROR::NO_CLEARANCE, STR_NONE, res2->ErrorMessage.GetStringId(), res2->ErrorMessageArgs.data());
        }
        if (surfaceElement->HasTrackThatNeedsWater())
        {
            return MakeResult(GA_ERROR::DISALLOWED, STR_NONE);
        }

        res->Cost = 250;

        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = MakeResult();
        res->Expenditure = ExpenditureType::Landscaping;
        res->Position = { _coords, _height * COORDS_Z_STEP };

        int32_t surfaceHeight = tile_element_height(_coords);
        footpath_remove_litter({ _coords, surfaceHeight });
        if (!gCheatsDisableClearanceChecks)
            wall_remove_at_z({ _coords, surfaceHeight });

        SurfaceElement* surfaceElement = map_get_surface_element_at(_coords);
        if (surfaceElement == nullptr)
        {
            log_error("Could not find surface element at: x %u, y %u", _coords.x, _coords.y);
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);
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

        res->Cost = 250;

        return res;
    }

private:
    rct_string_id CheckParameters() const
    {
        if (_coords.x > gMapSizeMaxXY || _coords.y > gMapSizeMaxXY)
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
};
