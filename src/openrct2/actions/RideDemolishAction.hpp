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
#include "../GameState.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../management/NewsItem.h"
#include "../ride/Ride.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Banner.h"
#include "../world/Sprite.h"
#include "../world/Park.h"
#include "GameAction.h"
#include "MazeSetTrackAction.hpp"

using namespace OpenRCT2;

struct RideDemolishAction : public GameActionBase<GAME_COMMAND_DEMOLISH_RIDE, GameActionResult>
{
private:
    sint32 _rideIndex = -1;
    uint8 _modifyType = RIDE_MODIFY_DEMOLISH;

public:
    RideDemolishAction() {}
    RideDemolishAction(sint32 rideIndex, uint8 modifyType) :
        _rideIndex(rideIndex),
        _modifyType(modifyType)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _rideIndex << _modifyType;
    }

    GameActionResult::Ptr Query() const override
    {
        Ride *ride = get_ride(_rideIndex);
        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command for ride %u", _rideIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_DEMOLISH_RIDE, STR_NONE);
        }

        if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE && _modifyType == RIDE_MODIFY_DEMOLISH)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::NO_CLEARANCE, STR_CANT_DEMOLISH_RIDE,
                STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE);
        }

        GameActionResult::Ptr result = std::make_unique<GameActionResult>();

        if (_modifyType == RIDE_MODIFY_RENEW)
        {
            if (ride->status != RIDE_STATUS_CLOSED)
            {
                return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, STR_CANT_REFURBISH_RIDE,
                    STR_MUST_BE_CLOSED_FIRST);
            }

            if (ride->num_riders > 0)
            {
                return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, STR_CANT_REFURBISH_RIDE,
                    STR_RIDE_NOT_YET_EMPTY);
            }

            if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED) || RideAvailableBreakdowns[ride->type] == 0)
            {
                return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, STR_CANT_REFURBISH_RIDE,
                    STR_CANT_REFURBISH_NOT_NEEDED);
            }

            result->ErrorTitle = STR_CANT_REFURBISH_RIDE;
            result->Cost = GetRefurbishPrice();
        }

        return result;
    }

    GameActionResult::Ptr Execute() const override
    {
        Ride *ride = get_ride(_rideIndex);
        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command for ride %u", _rideIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_DEMOLISH_RIDE, STR_NONE);
        }

        switch (_modifyType) {
        case RIDE_MODIFY_DEMOLISH:
            return DemolishRide(ride);
        case RIDE_MODIFY_RENEW:
            return RefurbishRide(ride);
        }

        return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_DO_THIS);
    }

private:

    GameActionResult::Ptr DemolishRide(Ride * ride) const
    {
        money32 refundPrice = DemolishTracks();

        ride_clear_for_construction(_rideIndex);
        ride_remove_peeps(_rideIndex);
        ride_stop_peeps_queuing(_rideIndex);

        sub_6CB945(_rideIndex);
        news_item_disable_news(NEWS_ITEM_RIDE, _rideIndex);

        for (auto &banner : gBanners)
        {
            if (banner.type != BANNER_NULL &&
                banner.flags & BANNER_FLAG_LINKED_TO_RIDE &&
                banner.colour == _rideIndex)
            {
                banner.flags &= 0xFB;
                banner.string_idx = STR_DEFAULT_SIGN;
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
                if (peep->thoughts[i].type != PEEP_THOUGHT_TYPE_NONE &&
                    peep->thoughts[i].item == _rideIndex)
                {
                    // Clear top thought, push others up
                    memmove(&peep->thoughts[i], &peep->thoughts[i + 1], sizeof(rct_peep_thought)*(PEEP_MAX_THOUGHTS - i - 1));
                    peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;
                    peep->thoughts[PEEP_MAX_THOUGHTS - 1].item = PEEP_THOUGHT_ITEM_NONE;
                    //Next iteration, check the new thought at this index
                    i--;
                }
            }
        }

        user_string_free(ride->name);
        ride->type = RIDE_TYPE_NULL;
        gParkValue = GetContext()->GetGameState()->GetPark().CalculateParkValue();

        auto res = std::make_unique<GameActionResult>();
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        res->Cost = refundPrice;

        if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
        {
            sint32 x = (ride->overall_view.x * 32) + 16;
            sint32 y = (ride->overall_view.y * 32) + 16;
            sint32 z = tile_element_height(x, y);

            res->Position = { x, y, z };
        }

        // Close windows related to the demolished ride
        if (!(GetFlags() & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED))
        {
            window_close_by_number(WC_RIDE_CONSTRUCTION, _rideIndex);
        }
        window_close_by_number(WC_RIDE, _rideIndex);
        window_close_by_number(WC_DEMOLISH_RIDE_PROMPT, _rideIndex);
        window_close_by_class(WC_NEW_CAMPAIGN);

        // Refresh windows that display the ride name
        auto windowManager = OpenRCT2::GetContext()->GetUiContext()->GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_RIDE_LIST));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_GUEST_LIST));

        return res;
    }

    money32 MazeRemoveTrack(uint16 x, uint16 y, uint16 z, uint8 direction) const
    {
        auto setMazeTrack = MazeSetTrackAction(x, y, z, false, direction, _rideIndex, GC_SET_MAZE_TRACK_FILL);
        setMazeTrack.SetFlags(GetFlags());

        auto queryRes = setMazeTrack.Query();
        if (queryRes->Error == GA_ERROR::OK)
        {
            auto execRes = setMazeTrack.Execute();
            if (execRes->Error == GA_ERROR::OK)
            {
                return execRes->Cost;
            }
        }

        return MONEY32_UNDEFINED;
    }

    money32 DemolishTracks() const
    {
        money32 refundPrice = 0;

        uint8 oldpaused = gGamePaused;
        gGamePaused = 0;

        tile_element_iterator it;

        tile_element_iterator_begin(&it);
        while (tile_element_iterator_next(&it))
        {
            if (it.element->GetType() != TILE_ELEMENT_TYPE_TRACK)
                continue;

            if (track_element_get_ride_index(it.element) != _rideIndex)
                continue;

            sint32 x = it.x * 32, y = it.y * 32;
            sint32 z = it.element->base_height * 8;

            uint8 rotation = tile_element_get_direction(it.element);
            uint8 type = track_element_get_type(it.element);

            if (type != TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP)
            {
                money32 removePrice = game_do_command(
                    x,
                    GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_APPLY | (rotation << 8),
                    y,
                    type | (tile_element_get_track_sequence(it.element) << 8),
                    GAME_COMMAND_REMOVE_TRACK,
                    z,
                    0);

                if (removePrice == MONEY32_UNDEFINED)
                    tile_element_remove(it.element);
                else
                    refundPrice += removePrice;

                tile_element_iterator_restart_for_tile(&it);
                continue;
            }

            static constexpr const LocationXY16 DirOffsets[] =
            {
                { 0, 0 },
                { 0, 16 },
                { 16, 16 },
                { 16, 0 },
            };

            for (uint8 dir = 0; dir < 4; dir++)
            {
                const LocationXY16& off = DirOffsets[dir];
                money32 removePrice = MazeRemoveTrack(x + off.x, y + off.y, z, dir);
                if (removePrice != MONEY32_UNDEFINED)
                    refundPrice += removePrice;
                else
                    break;
            }

            tile_element_iterator_restart_for_tile(&it);
        }

        gGamePaused = oldpaused;
        return refundPrice;
    }

    GameActionResult::Ptr RefurbishRide(Ride * ride) const
    {
        auto res = std::make_unique<GameActionResult>();
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        res->Cost = GetRefurbishPrice();

        ride_renew(ride);

        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_EVER_BEEN_OPENED;
        ride->last_crash_type = RIDE_CRASH_TYPE_NONE;

        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE | RIDE_INVALIDATE_RIDE_CUSTOMER;

        if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
        {
            sint32 x = (ride->overall_view.x * 32) + 16;
            sint32 y = (ride->overall_view.y * 32) + 16;
            sint32 z = tile_element_height(x, y);

            res->Position = { x, y, z };
        }

        window_close_by_number(WC_DEMOLISH_RIDE_PROMPT, _rideIndex);

        return res;
    }

    money32 GetRefurbishPrice() const
    {
        return -GetRefundPrice() / 2;
    }

    money32 GetRefundPrice() const
    {
        return ride_get_refund_price(_rideIndex);
    }
};
