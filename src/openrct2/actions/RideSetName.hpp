#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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
    sint32 _rideIndex = -1;
    std::string _name;

public:
    RideSetNameAction() {}
    RideSetNameAction(sint32 rideIndex, const std::string& name)
        : _rideIndex(rideIndex),
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
