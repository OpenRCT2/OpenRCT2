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

#include "../core/MemoryStream.h"
#include "GameAction.h"

#include "../cheats.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../ride/ride.h"
#include "../world/park.h"

struct RideDemolishAction : public GameActionBase<GAME_COMMAND_DEMOLISH_RIDE, GameActionResult>
{
private:
    sint32 _rideIndex = -1;

public:
    RideDemolishAction() {}
    RideDemolishAction(sint32 rideIndex)
        : _rideIndex(rideIndex)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _rideIndex;
    }

    GameActionResult::Ptr Query() const override
    {
        Ride *ride = get_ride(_rideIndex);
        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command for ride %u", _rideIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::NO_CLEARANCE, 
                STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE);
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        Ride *ride = get_ride(_rideIndex);
        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command for ride %u", _rideIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        sint32 refundPrice = ride_get_refund_price(_rideIndex);

        ride_clear_for_construction(_rideIndex);
        ride_remove_peeps(_rideIndex);
        ride_stop_peeps_queuing(_rideIndex);

        sub_6CB945(_rideIndex);
        news_item_disable_news(NEWS_ITEM_RIDE, _rideIndex);

        for (sint32 i = 0; i < MAX_BANNERS; i++) 
        {
            rct_banner *banner = &gBanners[i];
            if (banner->type != BANNER_NULL &&
                banner->flags & BANNER_FLAG_LINKED_TO_RIDE &&
                banner->colour == _rideIndex)
            {
                banner->flags &= 0xFB;
                banner->string_idx = STR_DEFAULT_SIGN;
            }
        }

        uint16 spriteIndex;
        rct_peep *peep;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            uint8 ride_id_bit = _rideIndex % 8;
            uint8 ride_id_offset = _rideIndex / 8;

            // clear ride from potentially being in rides_been_on
            peep->rides_been_on[ride_id_offset] &= ~(1 << ride_id_bit);
            if (peep->state == PEEP_STATE_WATCHING)
            {
                if (peep->current_ride == _rideIndex)
                {
                    peep->current_ride = MAX_RIDES;
                    if (peep->time_to_stand >= 50)
                    { 
                        // make peep stop watching the ride
                        peep->time_to_stand = 50;
                    }
                }
            }

            // remove any free voucher for this ride from peep
            if (peep->item_standard_flags & PEEP_ITEM_VOUCHER)
            {
                if (peep->voucher_type == VOUCHER_TYPE_RIDE_FREE &&
                    peep->voucher_arguments == _rideIndex)
                {
                    peep->item_standard_flags &= ~(PEEP_ITEM_VOUCHER);
                }
            }

            // remove any photos of this ride from peep
            if (peep->item_standard_flags & PEEP_ITEM_PHOTO)
            {
                if (peep->photo1_ride_ref == _rideIndex)
                {
                    peep->item_standard_flags &= ~PEEP_ITEM_PHOTO;
                }
            }
            if (peep->item_extra_flags & PEEP_ITEM_PHOTO2)
            {
                if (peep->photo2_ride_ref == _rideIndex)
                {
                    peep->item_extra_flags &= ~PEEP_ITEM_PHOTO2;
                }
            }
            if (peep->item_extra_flags & PEEP_ITEM_PHOTO3)
            {
                if (peep->photo3_ride_ref == _rideIndex)
                {
                    peep->item_extra_flags &= ~PEEP_ITEM_PHOTO3;
                }
            }
            if (peep->item_extra_flags & PEEP_ITEM_PHOTO4)
            {
                if (peep->photo4_ride_ref == _rideIndex)
                {
                    peep->item_extra_flags &= ~PEEP_ITEM_PHOTO4;
                }
            }

            if (peep->guest_heading_to_ride_id == _rideIndex)
            {
                peep->guest_heading_to_ride_id = MAX_RIDES;
            }
            if (peep->favourite_ride == _rideIndex)
            {
                peep->favourite_ride = MAX_RIDES;
            }

            for (sint32 i = 0; i < PEEP_MAX_THOUGHTS; i++)
            {
                if (peep->thoughts[i].item == _rideIndex)
                {
                    // Clear top thought, push others up
                    memmove(&peep->thoughts[i], &peep->thoughts[i + 1], sizeof(rct_peep_thought)*(PEEP_MAX_THOUGHTS - i - 1));
                    peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;
                }
            }
        }

        user_string_free(ride->name);
        ride->type = RIDE_TYPE_NULL;
        gParkValue = calculate_park_value();

        auto res = std::make_unique<GameActionResult>();
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        res->Cost = refundPrice;

        if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
        {
            sint32 x = (ride->overall_view.x * 32) + 16;
            sint32 y = (ride->overall_view.y * 32) + 16;
            sint32 z = map_element_height(x, y);

            res->Position = { x, y, z };
        }

        if (!(GetFlags() & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED))
        {
            window_close_by_number(WC_RIDE_CONSTRUCTION, _rideIndex);
        }
        window_close_by_number(WC_RIDE, _rideIndex);
        window_close_by_number(WC_DEMOLISH_RIDE_PROMPT, _rideIndex);
        window_close_by_class(WC_NEW_CAMPAIGN);

        window_invalidate_by_class(WC_RIDE_LIST);

        return res;
    }
};
