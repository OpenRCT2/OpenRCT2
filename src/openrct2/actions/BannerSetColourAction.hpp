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
#include "../management/Finance.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(BannerSetColourAction, GAME_COMMAND_SET_BANNER_COLOUR, GameActionResult)
{
private:
    CoordsXYZD _loc;
    uint8_t _primaryColour;

public:
    BannerSetColourAction() = default;

    BannerSetColourAction(const CoordsXYZD& loc, uint8_t primaryColour)
        : _loc(loc)
        , _primaryColour(primaryColour)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_primaryColour);
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
        res->Expenditure = ExpenditureType::Landscaping;
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = _loc.z;
        res->ErrorTitle = STR_CANT_REPAINT_THIS;

        if (!LocationValid(_loc))
        {
            log_error("Invalid x / y coordinates: x = %d, y = %d", _loc.x, _loc.y);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REPAINT_THIS);
        }

        if (_primaryColour > 31)
        {
            log_error("Invalid primary colour: colour = %u", _primaryColour);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REPAINT_THIS);
        }

        if (!map_can_build_at({ _loc.x, _loc.y, _loc.z - 16 }))
        {
            return MakeResult(GA_ERROR::NOT_OWNED, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        auto bannerElement = map_get_banner_element_at(_loc, _loc.direction);

        if (bannerElement == nullptr)
        {
            log_error(
                "Could not find banner at: x = %d, y = %d, z = %d, direction = %u", _loc.x, _loc.y, _loc.z, _loc.direction);
            return MakeResult(GA_ERROR::UNKNOWN, STR_CANT_REPAINT_THIS);
        }

        auto index = bannerElement->GetIndex();
        if (index >= MAX_BANNERS || index == BANNER_INDEX_NULL)
        {
            log_error("Invalid banner index: index = %u", index);
            return MakeResult(GA_ERROR::UNKNOWN, STR_CANT_REPAINT_THIS);
        }

        if (isExecuting)
        {
            auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
            intent.putExtra(INTENT_EXTRA_BANNER_INDEX, index);
            context_broadcast_intent(&intent);

            auto banner = GetBanner(index);
            banner->colour = _primaryColour;
            map_invalidate_tile_zoom1({ _loc, _loc.z, _loc.z + 32 });
        }

        return res;
    }
};
