/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

DEFINE_GAME_ACTION(RideSetNameAction, GAME_COMMAND_SET_RIDE_NAME, GameActionResult)
{
private:
    NetworkRideId_t _rideIndex{ RideIdNewNull };
    std::string _name;

public:
    RideSetNameAction()
    {
    }
    RideSetNameAction(ride_id_t rideIndex, const std::string& name)
        : _rideIndex(rideIndex)
        , _name(name)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override
    {
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("name", _name);
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideIndex) << DS_TAG(_name);
    }

    GameActionResult::Ptr Query() const override
    {
        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid game command for ride %u", uint32_t(_rideIndex));
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_RENAME_RIDE_ATTRACTION, STR_NONE);
        }

        if (!_name.empty() && Ride::NameExists(_name, ride->id))
        {
            return std::make_unique<GameActionResult>(
                GA_ERROR::INVALID_PARAMETERS, STR_CANT_RENAME_RIDE_ATTRACTION, STR_ERROR_EXISTING_NAME);
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid game command for ride %u", uint32_t(_rideIndex));
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_RENAME_RIDE_ATTRACTION, STR_NONE);
        }

        if (_name.empty())
        {
            ride->SetNameToDefault();
        }
        else
        {
            ride->custom_name = _name;
        }

        scrolling_text_invalidate();
        gfx_invalidate_screen();

        // Refresh windows that display ride name
        auto windowManager = OpenRCT2::GetContext()->GetUiContext()->GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_CAMPAIGN_RIDE_LIST));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_RIDE_LIST));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_GUEST_LIST));

        auto res = std::make_unique<GameActionResult>();
        auto location = ride->overall_view.ToTileCentre();
        res->Position = { location, tile_element_height(location) };

        return res;
    }
};
