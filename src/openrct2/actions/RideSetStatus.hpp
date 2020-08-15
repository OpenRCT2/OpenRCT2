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
#include "../common.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"
#include "../world/Sprite.h"
#include "GameAction.h"

static rct_string_id _StatusErrorTitles[] = {
    STR_CANT_CLOSE,
    STR_CANT_OPEN,
    STR_CANT_TEST,
    STR_CANT_SIMULATE,
};

DEFINE_GAME_ACTION(RideSetStatusAction, GAME_COMMAND_SET_RIDE_STATUS, GameActionResult)
{
private:
    NetworkRideId_t _rideIndex{ RideIdNewNull };
    uint8_t _status = RIDE_STATUS_CLOSED;

public:
    RideSetStatusAction()
    {
    }
    RideSetStatusAction(ride_id_t rideIndex, uint8_t status)
        : _rideIndex(rideIndex)
        , _status(status)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override
    {
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("status", _status);
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideIndex) << DS_TAG(_status);
    }

    GameActionResult::Ptr Query() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();

        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid game command for ride %u", uint32_t(_rideIndex));
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorTitle = STR_RIDE_DESCRIPTION_UNKNOWN;
            res->ErrorMessage = STR_NONE;
            return res;
        }

        if (_status >= RIDE_STATUS_COUNT)
        {
            log_warning("Invalid ride status %u for ride %u", uint32_t(_status), uint32_t(_rideIndex));
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorTitle = STR_RIDE_DESCRIPTION_UNKNOWN;
            res->ErrorMessage = STR_NONE;
            return res;
        }

        res->ErrorTitle = _StatusErrorTitles[_status];

        Formatter ft(res->ErrorMessageArgs.data());
        ft.Increment(6);
        ride->FormatNameTo(ft);
        if (_status != ride->status)
        {
            if (_status == RIDE_STATUS_SIMULATING && (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
            {
                // Simulating will force clear the track, so make sure player can't cheat around a break down
                res->Error = GA_ERROR::DISALLOWED;
                res->ErrorMessage = STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING;
                return res;
            }
            else if (_status == RIDE_STATUS_TESTING || _status == RIDE_STATUS_SIMULATING)
            {
                if (!ride_is_valid_for_test(ride, _status, false))
                {
                    res->Error = GA_ERROR::UNKNOWN;
                    res->ErrorMessage = gGameCommandErrorText;
                    return res;
                }
            }
            else if (_status == RIDE_STATUS_OPEN)
            {
                if (!ride_is_valid_for_open(ride, _status == RIDE_STATUS_OPEN, false))
                {
                    res->Error = GA_ERROR::UNKNOWN;
                    res->ErrorMessage = gGameCommandErrorText;
                    return res;
                }
            }
        }
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->Expenditure = ExpenditureType::RideRunningCosts;

        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid game command for ride %u", uint32_t(_rideIndex));
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorTitle = STR_RIDE_DESCRIPTION_UNKNOWN;
            res->ErrorMessage = STR_NONE;
            return res;
        }

        res->ErrorTitle = _StatusErrorTitles[_status];

        Formatter ft(res->ErrorMessageArgs.data());
        ft.Increment(6);
        ride->FormatNameTo(ft);
        if (!ride->overall_view.isNull())
        {
            auto location = ride->overall_view.ToTileCentre();
            res->Position = { location, tile_element_height(res->Position) };
        }

        switch (_status)
        {
            case RIDE_STATUS_CLOSED:
                if (ride->status == _status || ride->status == RIDE_STATUS_SIMULATING)
                {
                    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
                    {
                        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CRASHED;
                        ride_clear_for_construction(ride);
                        ride_remove_peeps(ride);
                    }
                }

                ride->status = RIDE_STATUS_CLOSED;
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
                ride->race_winner = SPRITE_INDEX_NULL;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
                window_invalidate_by_number(WC_RIDE, _rideIndex);
                break;
            case RIDE_STATUS_SIMULATING:
            {
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CRASHED;
                ride_clear_for_construction(ride);
                ride_remove_peeps(ride);

                if (!ride_is_valid_for_test(ride, _status, true))
                {
                    res->Error = GA_ERROR::UNKNOWN;
                    res->ErrorMessage = gGameCommandErrorText;
                    return res;
                }

                ride->status = _status;
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
                ride->race_winner = SPRITE_INDEX_NULL;
                ride->current_issues = 0;
                ride->last_issue_time = 0;
                ride_get_measurement(ride);
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
                window_invalidate_by_number(WC_RIDE, _rideIndex);
                break;
            }
            case RIDE_STATUS_TESTING:
            case RIDE_STATUS_OPEN:
            {
                if (ride->status == _status)
                {
                    return res;
                }

                if (ride->status == RIDE_STATUS_SIMULATING)
                {
                    ride_clear_for_construction(ride);
                    ride_remove_peeps(ride);
                }

                // Fix #3183: Make sure we close the construction window so the ride finishes any editing code before opening
                //            otherwise vehicles get added to the ride incorrectly (such as to a ghost station)
                rct_window* constructionWindow = window_find_by_number(WC_RIDE_CONSTRUCTION, _rideIndex);
                if (constructionWindow != nullptr)
                {
                    window_close(constructionWindow);
                }

                if (_status == RIDE_STATUS_TESTING)
                {
                    if (!ride_is_valid_for_test(ride, _status, true))
                    {
                        res->Error = GA_ERROR::UNKNOWN;
                        res->ErrorMessage = gGameCommandErrorText;
                        return res;
                    }
                }
                else if (!ride_is_valid_for_open(ride, _status == RIDE_STATUS_OPEN, true))
                {
                    res->Error = GA_ERROR::UNKNOWN;
                    res->ErrorMessage = gGameCommandErrorText;
                    return res;
                }

                ride->race_winner = SPRITE_INDEX_NULL;
                ride->status = _status;
                ride->current_issues = 0;
                ride->last_issue_time = 0;
                ride_get_measurement(ride);
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
                window_invalidate_by_number(WC_RIDE, _rideIndex);
                break;
            }
            default:
                Guard::Assert(false, "Invalid status passed: %u", _status);
                break;
        }
        auto windowManager = OpenRCT2::GetContext()->GetUiContext()->GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_CAMPAIGN_RIDE_LIST));

        return res;
    }
};
