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
#include "../Diagnostic.h"
#include "../common.h"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "../world/Banner.h"
#include "GameAction.h"

#include <string>

DEFINE_GAME_ACTION(SignSetNameAction, GAME_COMMAND_SET_SIGN_NAME, GameActionResult)
{
private:
    BannerIndex _bannerIndex;
    std::string _name;

public:
    SignSetNameAction() = default;
    SignSetNameAction(BannerIndex bannerIndex, const std::string& name)
        : _bannerIndex(bannerIndex)
        , _name(name)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_bannerIndex) << DS_TAG(_name);
    }

    GameActionResult::Ptr Query() const override
    {
        if (_bannerIndex >= MAX_BANNERS)
        {
            log_warning("Invalid game command for setting sign name, banner id = %d", _bannerIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }
        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto banner = GetBanner(_bannerIndex);

        if (!_name.empty())
        {
            banner->flags &= ~BANNER_FLAG_LINKED_TO_RIDE;
            banner->ride_index = RIDE_ID_NULL;
            banner->text = _name;
        }
        else
        {
            // If empty name take closest ride name.
            ride_id_t rideIndex = banner_get_closest_ride_index({ banner->position.ToCoordsXY(), 16 });
            if (rideIndex == RIDE_ID_NULL)
            {
                banner->flags &= ~BANNER_FLAG_LINKED_TO_RIDE;
                banner->ride_index = RIDE_ID_NULL;
                banner->text = {};
            }
            else
            {
                banner->flags |= BANNER_FLAG_LINKED_TO_RIDE;
                banner->ride_index = rideIndex;
                banner->text = {};
            }
        }

        scrolling_text_invalidate();
        gfx_invalidate_screen();
        return MakeResult();
    }
};
