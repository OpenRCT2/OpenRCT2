#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#pragma once

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../localisation/StringIds.h"
#include "../ui/UiContext.h"
#include "../world/Sprite.h"
#include "../world/Banner.h"
#include "GameAction.h"

using namespace OpenRCT2;

struct SignSetNameAction : public GameActionBase<GAME_COMMAND_SET_SIGN_NAME, GameActionResult>
{
private:
    sint32 _bannerIndex;
    std::string _name;

public:
    SignSetNameAction() {}
    SignSetNameAction(sint32 bannerIndex, const std::string& name)
        : _bannerIndex(bannerIndex),
        _name(name)
    {
    }

    uint16 GetActionFlags() const override
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
        if (_bannerIndex >= MAX_BANNERS || _bannerIndex < 0)
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

        sint32 x = banner->x << 5;
        sint32 y = banner->y << 5;

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
            sint32 rideIndex = banner_get_closest_ride_index(x, y, 16);
            if (rideIndex == -1)
            {
                return MakeResult();
            }

            banner->colour = rideIndex;
            banner->flags |= BANNER_FLAG_LINKED_TO_RIDE;

            rct_string_id prev_string_id = banner->string_idx;
            banner->string_idx = STR_DEFAULT_SIGN;
            user_string_free(prev_string_id);

            gfx_invalidate_screen();
        }

        return MakeResult();
    }
};
