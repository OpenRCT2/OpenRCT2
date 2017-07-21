#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../core/MemoryStream.h"
#include "../localisation/string_ids.h"
#include "GameAction.h"

extern "C"
{
    #include "../cheats.h"
    #include "../interface/window.h"
    #include "../world/park.h"
    #include "../ride/ride.h"
}

struct RideSetStatusAction : public GameAction<GAME_COMMAND_SET_RIDE_STATUS, GA_FLAGS::ALLOW_WHILE_PAUSED>
{
public:
    uint8 RideIndex;
    uint8 Status;

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << RideIndex;
        stream << Status;
    }

    GameActionResult::Ptr Query() const override
    {
        if (RideIndex >= MAX_RIDES)
        {
            log_warning("Invalid game command for ride %u", RideIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_SELECTION_OF_OBJECTS);
        }
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS;

        rct_ride *ride = get_ride(RideIndex);

        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command for ride %u", RideIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        if (ride->overall_view.xy != RCT_XY8_UNDEFINED) 
        {
            res->Position.x = ride->overall_view.x * 32 + 16;
            res->Position.y = ride->overall_view.y * 32 + 16;
            res->Position.z = map_element_height(res->Position.x, res->Position.y);
        }

        switch (Status) 
        {
        case RIDE_STATUS_CLOSED:
            if (ride->status == Status) {
                if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)) {
                    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CRASHED;
                    ride_clear_for_construction(RideIndex);
                    ride_remove_peeps(RideIndex);
                }
            }

            ride->status = RIDE_STATUS_CLOSED;
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
            ride->race_winner = SPRITE_INDEX_NULL;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
            window_invalidate_by_number(WC_RIDE, RideIndex);
            return res;
        case RIDE_STATUS_TESTING:
        case RIDE_STATUS_OPEN:
            {
                if (ride->status == Status)
                {
                    return res;
                }

                // Fix #3183: Make sure we close the construction window so the ride finishes any editing code before opening
                //            otherwise vehicles get added to the ride incorrectly (such as to a ghost station)
                rct_window *constructionWindow = window_find_by_number(WC_RIDE_CONSTRUCTION, RideIndex);
                if (constructionWindow != NULL) {
                    window_close(constructionWindow);
                }

                if (Status == RIDE_STATUS_TESTING) {
                    if (!ride_is_valid_for_test(RideIndex, Status == RIDE_STATUS_OPEN, 1)) {
                        //*ebx = MONEY32_UNDEFINED;
                        return res;
                    }
                }
                else if (!ride_is_valid_for_open(RideIndex, Status == RIDE_STATUS_OPEN, 1)) {
                    //*ebx = MONEY32_UNDEFINED;
                    return res;
                }

                ride->race_winner = SPRITE_INDEX_NULL;
                ride->status = Status;
                ride_get_measurement(RideIndex, NULL);
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
                window_invalidate_by_number(WC_RIDE, RideIndex);
            }
            return res;
        default:
            Guard::Assert(false, "Invalid status passed: %u", Status);
        }

        return res;
    }
};
