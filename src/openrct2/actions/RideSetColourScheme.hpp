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
#include "../common.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../world/Park.h"
#include "../world/Sprite.h"
#include "GameAction.h"

struct RideSetColourSchemeAction : public GameActionBase<GAME_COMMAND_SET_COLOUR_SCHEME, GameActionResult>
{
private:
    int32_t _x, _y, _z, _direction, _trackType;
    uint16_t _newColourScheme;

public:
    RideSetColourSchemeAction()
    {
    }
    RideSetColourSchemeAction(int32_t x, int32_t y, int32_t z, int32_t direction, int32_t trackType, uint16_t newColourScheme)
        : _x(x)
        , _y(y)
        , _z(z)
        , _direction(direction)
        , _trackType(trackType)
        , _newColourScheme(newColourScheme)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_x) << DS_TAG(_y) << DS_TAG(_z) << DS_TAG(_direction) << DS_TAG(_trackType) << DS_TAG(_newColourScheme);
    }

    GameActionResult::Ptr Query() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->ErrorTitle = STR_CANT_SET_COLOUR_SCHEME;
        //set_format_arg_on(res->ErrorMessageArgs.data(), 6, rct_string_id, ride->name);
        //set_format_arg_on(res->ErrorMessageArgs.data(), 8, uint32_t, ride->name_arguments);

        if (_x == 123456789)
        {
            log_warning("Invalid game command for ride %u", _x);
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
            return res;
        }
        
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;

        //Ride* ride = get_ride(_rideIndex);
        res->ErrorTitle = STR_CANT_SET_COLOUR_SCHEME;
//        set_format_arg_on(res->ErrorMessageArgs.data(), 6, rct_string_id, ride->name);
//        set_format_arg_on(res->ErrorMessageArgs.data(), 8, uint32_t, ride->name_arguments);

        int32_t x = _x, y = _y, z = _z;
        sub_6C683D(&x, &y, &z, _direction, _trackType, _newColourScheme, nullptr, 4);


//        if (ride->type == RIDE_TYPE_NULL)
//        {
//            log_warning("Invalid game command for ride %u", _rideIndex);
//            res->Error = GA_ERROR::INVALID_PARAMETERS;
//            res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
//            return res;
//        }
//
//        if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
//        {
//            res->Position.x = ride->overall_view.x * 32 + 16;
//            res->Position.y = ride->overall_view.y * 32 + 16;
//            res->Position.z = tile_element_height(res->Position.x, res->Position.y);
//        }

//        switch (_status)
//        {
//            case RIDE_STATUS_CLOSED:
//                if (ride->status == _status)
//                {
//                    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
//                    {
//                        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CRASHED;
//                        ride_clear_for_construction(_rideIndex);
//                        ride_remove_peeps(_rideIndex);
//                    }
//                }
//
//                ride->status = RIDE_STATUS_CLOSED;
//                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
//                ride->race_winner = SPRITE_INDEX_NULL;
//                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
//                window_invalidate_by_number(WC_RIDE, _rideIndex);
//                break;
//            case RIDE_STATUS_TESTING:
//            case RIDE_STATUS_OPEN:
//            {
//                if (ride->status == _status)
//                {
//                    return res;
//                }
//
//                // Fix #3183: Make sure we close the construction window so the ride finishes any editing code before opening
//                //            otherwise vehicles get added to the ride incorrectly (such as to a ghost station)
//                rct_window* constructionWindow = window_find_by_number(WC_RIDE_CONSTRUCTION, _rideIndex);
//                if (constructionWindow != nullptr)
//                {
//                    window_close(constructionWindow);
//                }
//
//                if (_status == RIDE_STATUS_TESTING)
//                {
//                    if (!ride_is_valid_for_test(_rideIndex, _status == RIDE_STATUS_OPEN, 1))
//                    {
//                        res->Error = GA_ERROR::UNKNOWN;
//                        res->ErrorMessage = gGameCommandErrorText;
//                        return res;
//                    }
//                }
//                else if (!ride_is_valid_for_open(_rideIndex, _status == RIDE_STATUS_OPEN, 1))
//                {
//                    res->Error = GA_ERROR::UNKNOWN;
//                    res->ErrorMessage = gGameCommandErrorText;
//                    return res;
//                }
//
//                ride->race_winner = SPRITE_INDEX_NULL;
//                ride->status = _status;
//                ride_get_measurement(_rideIndex, nullptr);
//                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
//                window_invalidate_by_number(WC_RIDE, _rideIndex);
//                break;
//            }
//            default:
//                Guard::Assert(false, "Invalid status passed: %u", _status);
//                break;
//        }
        return res;
    }
};
