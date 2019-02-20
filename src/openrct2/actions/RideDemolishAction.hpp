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
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../management/NewsItem.h"
#include "../ride/Ride.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Banner.h"
#include "../world/Park.h"
#include "../world/Sprite.h"
#include "GameAction.h"
#include "MazeSetTrackAction.hpp"
#include "TrackRemoveAction.hpp"

using namespace OpenRCT2;

DEFINE_GAME_ACTION(RideDemolishAction, GAME_COMMAND_DEMOLISH_RIDE, GameActionResult)
{
private:
    NetworkRideId_t _rideIndex{ -1 };
    uint8_t _modifyType = RIDE_MODIFY_DEMOLISH;

public:
    RideDemolishAction()
    {
    }
    RideDemolishAction(ride_id_t rideIndex, uint8_t modifyType)
        : _rideIndex(rideIndex)
        , _modifyType(modifyType)
    {
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideIndex) << DS_TAG(_modifyType);
    }

    GameActionResult::Ptr Query() const override
    {
        Ride* ride = get_ride(_rideIndex);
        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command for ride %u", uint32_t(_rideIndex));
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_DEMOLISH_RIDE, STR_NONE);
        }

        if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE && _modifyType == RIDE_MODIFY_DEMOLISH)
        {
            return std::make_unique<GameActionResult>(
                GA_ERROR::NO_CLEARANCE, STR_CANT_DEMOLISH_RIDE,
                STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE);
        }

        GameActionResult::Ptr result = std::make_unique<GameActionResult>();

        if (_modifyType == RIDE_MODIFY_RENEW)
        {
            if (ride->status != RIDE_STATUS_CLOSED)
            {
                return std::make_unique<GameActionResult>(
                    GA_ERROR::DISALLOWED, STR_CANT_REFURBISH_RIDE, STR_MUST_BE_CLOSED_FIRST);
            }

            if (ride->num_riders > 0)
            {
                return std::make_unique<GameActionResult>(
                    GA_ERROR::DISALLOWED, STR_CANT_REFURBISH_RIDE, STR_RIDE_NOT_YET_EMPTY);
            }

            if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED) || RideAvailableBreakdowns[ride->type] == 0)
            {
                return std::make_unique<GameActionResult>(
                    GA_ERROR::DISALLOWED, STR_CANT_REFURBISH_RIDE, STR_CANT_REFURBISH_NOT_NEEDED);
            }

            result->ErrorTitle = STR_CANT_REFURBISH_RIDE;
            result->Cost = GetRefurbishPrice(ride);
        }

        return result;
    }

    GameActionResult::Ptr Execute() const override
    {
        Ride* ride = get_ride(_rideIndex);
        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command for ride %u", uint32_t(_rideIndex));
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_DEMOLISH_RIDE, STR_NONE);
        }

        switch (_modifyType)
        {
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

        ride_clear_for_construction(ride);
        ride_remove_peeps(ride);
        ride_stop_peeps_queuing(ride);

        sub_6CB945(ride);
        ride_clear_leftover_entrances(ride);
        news_item_disable_news(NEWS_ITEM_RIDE, _rideIndex);

        for (auto& banner : gBanners)
        {
            if (banner.type != BANNER_NULL && banner.flags & BANNER_FLAG_LINKED_TO_RIDE && banner.ride_index == _rideIndex)
            {
                banner.flags &= 0xFB;
                banner.string_idx = STR_DEFAULT_SIGN;
            }
        }

        uint16_t spriteIndex;
        rct_peep* peep;
        FOR_ALL_GUESTS (spriteIndex, peep)
        {
            uint8_t ride_id_bit = _rideIndex % 8;
            uint8_t ride_id_offset = _rideIndex / 8;

            // clear ride from potentially being in rides_been_on
            peep->rides_been_on[ride_id_offset] &= ~(1 << ride_id_bit);
            if (peep->state == PEEP_STATE_WATCHING)
            {
                if (peep->current_ride == _rideIndex)
                {
                    peep->current_ride = RIDE_ID_NULL;
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
                if (peep->voucher_type == VOUCHER_TYPE_RIDE_FREE && peep->voucher_arguments == _rideIndex)
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
                peep->guest_heading_to_ride_id = RIDE_ID_NULL;
            }
            if (peep->favourite_ride == _rideIndex)
            {
                peep->favourite_ride = RIDE_ID_NULL;
            }

            for (int32_t i = 0; i < PEEP_MAX_THOUGHTS; i++)
            {
                if (peep->thoughts[i].type != PEEP_THOUGHT_TYPE_NONE && peep->thoughts[i].item == _rideIndex)
                {
                    // Clear top thought, push others up
                    memmove(&peep->thoughts[i], &peep->thoughts[i + 1], sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - i - 1));
                    peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;
                    peep->thoughts[PEEP_MAX_THOUGHTS - 1].item = PEEP_THOUGHT_ITEM_NONE;
                    // Next iteration, check the new thought at this index
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
            int32_t x = (ride->overall_view.x * 32) + 16;
            int32_t y = (ride->overall_view.y * 32) + 16;
            int32_t z = tile_element_height(x, y);

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

        scrolling_text_invalidate();
        gfx_invalidate_screen();

        return res;
    }

    money32 MazeRemoveTrack(uint16_t x, uint16_t y, uint16_t z, uint8_t direction) const
    {
        auto setMazeTrack = MazeSetTrackAction(x, y, z, false, direction, _rideIndex, GC_SET_MAZE_TRACK_FILL);
        setMazeTrack.SetFlags(GetFlags());

        auto execRes = GameActions::ExecuteNested(&setMazeTrack);
        if (execRes->Error == GA_ERROR::OK)
        {
            return execRes->Cost;
        }

        return MONEY32_UNDEFINED;
    }

    money32 DemolishTracks() const
    {
        money32 refundPrice = 0;

        uint8_t oldpaused = gGamePaused;
        gGamePaused = 0;

        tile_element_iterator it;

        tile_element_iterator_begin(&it);
        while (tile_element_iterator_next(&it))
        {
            if (it.element->GetType() != TILE_ELEMENT_TYPE_TRACK)
                continue;

            if (it.element->AsTrack()->GetRideIndex() != _rideIndex)
                continue;

            int32_t x = it.x * 32, y = it.y * 32;
            int32_t z = it.element->base_height * 8;

            uint8_t rotation = it.element->GetDirection();
            uint8_t type = it.element->AsTrack()->GetTrackType();

            if (type != TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP)
            {
                auto trackRemoveAction = TrackRemoveAction(
                    type, it.element->AsTrack()->GetSequenceIndex(), { x, y, z, rotation });
                trackRemoveAction.SetFlags(GAME_COMMAND_FLAG_5);

                auto removRes = GameActions::ExecuteNested(&trackRemoveAction);

                if (removRes->Error != GA_ERROR::OK)
                {
                    tile_element_remove(it.element);
                }
                else
                {
                    refundPrice += removRes->Cost;
                }

                tile_element_iterator_restart_for_tile(&it);
                continue;
            }

            static constexpr const LocationXY16 DirOffsets[] = {
                { 0, 0 },
                { 0, 16 },
                { 16, 16 },
                { 16, 0 },
            };

            for (uint8_t dir = 0; dir < 4; dir++)
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
        res->Cost = GetRefurbishPrice(ride);

        ride_renew(ride);

        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_EVER_BEEN_OPENED;
        ride->last_crash_type = RIDE_CRASH_TYPE_NONE;

        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE | RIDE_INVALIDATE_RIDE_CUSTOMER;

        if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
        {
            int32_t x = (ride->overall_view.x * 32) + 16;
            int32_t y = (ride->overall_view.y * 32) + 16;
            int32_t z = tile_element_height(x, y);

            res->Position = { x, y, z };
        }

        window_close_by_number(WC_DEMOLISH_RIDE_PROMPT, _rideIndex);

        return res;
    }

    money32 GetRefurbishPrice(const Ride* ride) const
    {
        return -GetRefundPrice(ride) / 2;
    }

    money32 GetRefundPrice(const Ride* ride) const
    {
        return ride_get_refund_price(ride);
    }
};
