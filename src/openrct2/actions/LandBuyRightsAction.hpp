/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../actions/LandSetHeightAction.hpp"
#include "../audio/audio.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "GameAction.h"

enum class LandBuyRightSetting : uint8_t
{
    BuyLand,
    BuyConstructionRights,
    Count
};

DEFINE_GAME_ACTION(LandBuyRightsAction, GAME_COMMAND_BUY_LAND_RIGHTS, GameActionResult)
{
private:
    MapRange _range;
    uint8_t _setting = static_cast<uint8_t>(LandBuyRightSetting::Count);

    constexpr static rct_string_id _ErrorTitles[] = { STR_CANT_BUY_LAND, STR_CANT_BUY_CONSTRUCTION_RIGHTS_HERE };

public:
    LandBuyRightsAction() = default;

    LandBuyRightsAction(const MapRange& range, LandBuyRightSetting setting)
        : _range(range)
        , _setting(static_cast<uint8_t>(setting))
    {
    }

    LandBuyRightsAction(const CoordsXY& coord, LandBuyRightSetting setting)
        : _range(coord.x, coord.y, coord.x, coord.y)
        , _setting(static_cast<uint8_t>(setting))
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_range) << DS_TAG(_setting);
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

        MapRange normRange = _range.Normalise();
        // Keep big coordinates within map boundaries
        auto aX = std::max<decltype(normRange.GetLeft())>(32, normRange.GetLeft());
        auto bX = std::min<decltype(normRange.GetRight())>(gMapSizeMaxXY, normRange.GetRight());
        auto aY = std::max<decltype(normRange.GetTop())>(32, normRange.GetTop());
        auto bY = std::min<decltype(normRange.GetBottom())>(gMapSizeMaxXY, normRange.GetBottom());

        MapRange validRange = MapRange{ aX, aY, bX, bY };

        CoordsXYZ centre{ (validRange.GetLeft() + validRange.GetRight()) / 2 + 16,
                          (validRange.GetTop() + validRange.GetBottom()) / 2 + 16, 0 };
        centre.z = tile_element_height(centre);

        res->Position = centre;
        res->Expenditure = ExpenditureType::LandPurchase;

        // Game command modified to accept selection size
        for (auto y = validRange.GetTop(); y <= validRange.GetBottom(); y += COORDS_XY_STEP)
        {
            for (auto x = validRange.GetLeft(); x <= validRange.GetRight(); x += COORDS_XY_STEP)
            {
                if (!LocationValid({ x, y }))
                    continue;
                auto result = map_buy_land_rights_for_tile({ x, y }, isExecuting);
                if (result->Error == GA_ERROR::OK)
                {
                    res->Cost += result->Cost;
                }
            }
        }
        if (isExecuting)
        {
            map_count_remaining_land_rights();
        }
        return res;
    }

    GameActionResult::Ptr map_buy_land_rights_for_tile(const CoordsXY& loc, bool isExecuting) const
    {
        if (_setting >= static_cast<uint8_t>(LandBuyRightSetting::Count))
        {
            log_warning("Tried calling buy land rights with an incorrect setting. setting = %u", _setting);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, _ErrorTitles[0], STR_NONE);
        }

        SurfaceElement* surfaceElement = map_get_surface_element_at(loc);
        if (surfaceElement == nullptr)
        {
            log_error("Could not find surface. x = %d, y = %d", loc.x, loc.y);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, _ErrorTitles[_setting], STR_NONE);
        }

        auto res = MakeResult();
        switch (static_cast<LandBuyRightSetting>(_setting))
        {
            case LandBuyRightSetting::BuyLand: // 0
                if ((surfaceElement->GetOwnership() & OWNERSHIP_OWNED) != 0)
                { // If the land is already owned
                    return res;
                }

                if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) != 0
                    || (surfaceElement->GetOwnership() & OWNERSHIP_AVAILABLE) == 0)
                {
                    return MakeResult(GA_ERROR::NOT_OWNED, _ErrorTitles[_setting], STR_LAND_NOT_FOR_SALE);
                }
                if (isExecuting)
                {
                    surfaceElement->SetOwnership(OWNERSHIP_OWNED);
                    update_park_fences_around_tile(loc);
                }
                res->Cost = gLandPrice;
                return res;

            case LandBuyRightSetting::BuyConstructionRights: // 2
                if ((surfaceElement->GetOwnership() & (OWNERSHIP_OWNED | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)) != 0)
                { // If the land or construction rights are already owned
                    return res;
                }

                if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) != 0
                    || (surfaceElement->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE) == 0)
                {
                    return MakeResult(GA_ERROR::NOT_OWNED, _ErrorTitles[_setting], STR_CONSTRUCTION_RIGHTS_NOT_FOR_SALE);
                }

                if (isExecuting)
                {
                    surfaceElement->SetOwnership(surfaceElement->GetOwnership() | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED);
                    uint16_t baseZ = surfaceElement->GetBaseZ();
                    map_invalidate_tile({ loc, baseZ, baseZ + 16 });
                }
                res->Cost = gConstructionRightsPrice;
                return res;

            default:
                log_warning("Tried calling buy land rights with an incorrect setting. setting = %u", _setting);
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, _ErrorTitles[0], STR_NONE);
        }
    }
};
