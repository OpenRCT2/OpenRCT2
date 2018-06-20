/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"
#include "GameAction.h"

struct RideSetNameAction : public GameActionBase<GAME_COMMAND_SET_RIDE_NAME, GameActionResult>
{
private:
    int32_t _rideIndex = -1;
    std::string _name;

public:
    RideSetNameAction() {}
    RideSetNameAction(int32_t rideIndex, const std::string& name)
        : _rideIndex(rideIndex),
        _name(name)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _rideIndex << _name;
    }

    GameActionResult::Ptr Query() const override
    {
        Ride *ride = get_ride(_rideIndex);
        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command for ride %u", _rideIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_RENAME_RIDE_ATTRACTION, STR_NONE);
        }

        if (_name.empty())
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_RENAME_RIDE_ATTRACTION, STR_INVALID_RIDE_ATTRACTION_NAME);
        }

        rct_string_id newUserStringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER | USER_STRING_DUPLICATION_PERMITTED, _name.c_str());
        if (newUserStringId == 0)
        {
            // TODO: Probably exhausted, introduce new error.
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_CANT_RENAME_RIDE_ATTRACTION, STR_NONE);
        }
        user_string_free(newUserStringId);

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        rct_string_id newUserStringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER | USER_STRING_DUPLICATION_PERMITTED, _name.c_str());

        Ride *ride = get_ride(_rideIndex);
        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command for ride %u", _rideIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_RENAME_RIDE_ATTRACTION, STR_NONE);
        }

        user_string_free(ride->name);
        ride->name = newUserStringId;

        gfx_invalidate_screen();

        // Refresh windows that display ride name
        auto windowManager = OpenRCT2::GetContext()->GetUiContext()->GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_RIDE_LIST));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_GUEST_LIST));

        auto res = std::make_unique<GameActionResult>();
        res->Position.x = ride->overall_view.x * 32 + 16;
        res->Position.y = ride->overall_view.y * 32 + 16;
        res->Position.z = tile_element_height(res->Position.x, res->Position.y);

        return res;
    }
};
