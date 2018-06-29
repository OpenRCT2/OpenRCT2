/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/MemoryStream.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "GameAction.h"

#include "../Cheats.h"
#include "../interface/Window.h"
#include "../management/Finance.h"
#include "../world/Park.h"
#include "../world/Sprite.h"
#include "../ride/Ride.h"

static rct_string_id _StatusErrorTitles[] =
{
    STR_CANT_CLOSE,
    STR_CANT_OPEN,
    STR_CANT_TEST
};

struct RideSetStatusAction : public GameActionBase<GAME_COMMAND_SET_RIDE_STATUS, GameActionResult>
{
private:
    int32_t _rideIndex = -1;
    uint8_t _status = RIDE_STATUS_CLOSED;

public:
    RideSetStatusAction() {}
    RideSetStatusAction(uint8_t rideIndex, uint8_t status) :
        _rideIndex(rideIndex),
        _status(status)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _rideIndex << _status;
    }

    GameActionResult::Ptr Query() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        Ride *ride = get_ride(_rideIndex);
        res->ErrorTitle = _StatusErrorTitles[_status];
        set_format_arg_on(res->ErrorMessageArgs.data(), 6, rct_string_id, ride->name);
        set_format_arg_on(res->ErrorMessageArgs.data(), 8, uint32_t, ride->name_arguments);

        if (_rideIndex >= MAX_RIDES || _rideIndex < 0)
        {
            log_warning("Invalid game command for ride %u", _rideIndex);
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
            return res;
        }

        if (_status != ride->status)
        {
            if (_status == RIDE_STATUS_TESTING)
            {
                if (!ride_is_valid_for_test(_rideIndex, _status == RIDE_STATUS_OPEN, 0))
                {
                    res->Error = GA_ERROR::UNKNOWN;
                    res->ErrorMessage = gGameCommandErrorText;
                    return res;
                }
            }
            else if (_status == RIDE_STATUS_OPEN) {
                if (!ride_is_valid_for_open(_rideIndex, _status == RIDE_STATUS_OPEN, 0))
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
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS;

        Ride *ride = get_ride(_rideIndex);
        res->ErrorTitle = _StatusErrorTitles[_status];
        set_format_arg_on(res->ErrorMessageArgs.data(), 6, rct_string_id, ride->name);
        set_format_arg_on(res->ErrorMessageArgs.data(), 8,uint32_t, ride->name_arguments);

        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command for ride %u", _rideIndex);
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
            return res;
        }

        if (ride->overall_view.xy != RCT_XY8_UNDEFINED) 
        {
            res->Position.x = ride->overall_view.x * 32 + 16;
            res->Position.y = ride->overall_view.y * 32 + 16;
            res->Position.z = tile_element_height(res->Position.x, res->Position.y);
        }

        switch (_status) {
        case RIDE_STATUS_CLOSED:
            if (ride->status == _status)
            {
                if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
                {
                    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CRASHED;
                    ride_clear_for_construction(_rideIndex);
                    ride_remove_peeps(_rideIndex);
                }
            }

            ride->status = RIDE_STATUS_CLOSED;
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
            ride->race_winner = SPRITE_INDEX_NULL;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
            window_invalidate_by_number(WC_RIDE, _rideIndex);
            break;
        case RIDE_STATUS_TESTING:
        case RIDE_STATUS_OPEN:
            {
                if (ride->status == _status)
                {
                    return res;
                }

                // Fix #3183: Make sure we close the construction window so the ride finishes any editing code before opening
                //            otherwise vehicles get added to the ride incorrectly (such as to a ghost station)
                rct_window *constructionWindow = window_find_by_number(WC_RIDE_CONSTRUCTION, _rideIndex);
                if (constructionWindow != nullptr)
                {
                    window_close(constructionWindow);
                }

                if (_status == RIDE_STATUS_TESTING)
                {
                    if (!ride_is_valid_for_test(_rideIndex, _status == RIDE_STATUS_OPEN, 1))
                    {
                        res->Error = GA_ERROR::UNKNOWN;
                        res->ErrorMessage = gGameCommandErrorText;
                        return res;
                    }
                }
                else if (!ride_is_valid_for_open(_rideIndex, _status == RIDE_STATUS_OPEN, 1))
                {
                    res->Error = GA_ERROR::UNKNOWN;
                    res->ErrorMessage = gGameCommandErrorText;
                    return res;
                }

                ride->race_winner = SPRITE_INDEX_NULL;
                ride->status = _status;
                ride_get_measurement(_rideIndex, nullptr);
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
                window_invalidate_by_number(WC_RIDE, _rideIndex);
                break;
            }
        default:
            Guard::Assert(false, "Invalid status passed: %u", _status);
            break;
        }
        return res;
    }
};
