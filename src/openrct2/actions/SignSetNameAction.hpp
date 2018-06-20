/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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

struct SignSetNameAction : public GameActionBase<GAME_COMMAND_SET_SIGN_NAME, GameActionResult>
{
private:
    int32_t _bannerIndex;
    std::string _name;

public:
    SignSetNameAction() = default;
    SignSetNameAction(int32_t bannerIndex, const std::string& name)
        : _bannerIndex(bannerIndex)
        , _name(name)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);
        stream << _bannerIndex << _name;
    }

    GameActionResult::Ptr Query() const override
    {
        if ((BannerIndex)_bannerIndex >= MAX_BANNERS || _bannerIndex < 0)
        {
            log_warning("Invalid game command for setting sign name, banner id = %d", _bannerIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        // Ensure user string space.
        rct_string_id string_id = user_string_allocate(USER_STRING_DUPLICATION_PERMITTED, _name.c_str());
        if (string_id != 0)
        {
            user_string_free(string_id);
        }
        else
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_ERR_CANT_SET_BANNER_TEXT);
        }

        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        rct_banner* banner = &gBanners[_bannerIndex];

        int32_t x = banner->x << 5;
        int32_t y = banner->y << 5;

        if (_name.empty() == false)
        {
            rct_string_id string_id = user_string_allocate(USER_STRING_DUPLICATION_PERMITTED, _name.c_str());
            if (string_id != 0)
            {
                rct_string_id prev_string_id = banner->string_idx;
                banner->string_idx = string_id;
                user_string_free(prev_string_id);

                banner->flags &= ~(BANNER_FLAG_LINKED_TO_RIDE);
                gfx_invalidate_screen();
            }
            else
            {
                return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_ERR_CANT_SET_BANNER_TEXT);
            }
        }
        else
        {
            // If empty name take closest ride name.
            uint8_t rideIndex = banner_get_closest_ride_index(x, y, 16);
            if (rideIndex == RIDE_ID_NULL)
            {
                return MakeResult();
            }

            banner->ride_index = rideIndex;
            banner->flags |= BANNER_FLAG_LINKED_TO_RIDE;

            rct_string_id prev_string_id = banner->string_idx;
            banner->string_idx = STR_DEFAULT_SIGN;
            user_string_free(prev_string_id);

            gfx_invalidate_screen();
        }

        return MakeResult();
    }
};
