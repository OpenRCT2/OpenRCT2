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

#include <limits>

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../Cheats.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/Math.hpp"
#include "../core/Util.hpp"
#include "../Game.h"
#include "../Input.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../network/network.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../scenario/Scenario.h"
#include "../sprites.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/LargeScenery.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Sprite.h"
#include "Peep.h"
#include "Staff.h"
#include "../windows/Intent.h"

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
bool gPathFindDebug = false;
utf8 gPathFindDebugPeepName[256];
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

uint8  gGuestChangeModifier;
uint16 gNumGuestsInPark;
uint16 gNumGuestsInParkLastWeek;
uint16 gNumGuestsHeadingForPark;

money16 gGuestInitialCash;
uint8   gGuestInitialHappiness;
uint8   gGuestInitialHunger;
uint8   gGuestInitialThirst;

uint32 gNextGuestNumber;

uint8 gPeepWarningThrottle[16];

LocationXYZ16 gPeepPathFindGoalPosition;
bool          gPeepPathFindIgnoreForeignQueues;
uint8         gPeepPathFindQueueRideIndex;
// uint32 gPeepPathFindAltStationNum;


static uint8             _unk_F1AEF0;
static rct_tile_element * _peepRideEntranceExitElement;

static void * _crowdSoundChannel = nullptr;

static void   peep_128_tick_update(rct_peep * peep, sint32 index);
static bool   peep_should_find_bench(rct_peep * peep);
static void   peep_update_favourite_ride(rct_peep * peep, Ride * ride);
static sint16 peep_calculate_ride_satisfaction(rct_peep * peep, Ride * ride);
static void   peep_update_ride_nausea_growth(rct_peep * peep, Ride * ride);
static void   peep_ride_is_too_intense(rct_peep * peep, sint32 rideIndex, bool peepAtRide);
static void   peep_tried_to_enter_full_queue(rct_peep * peep, sint32 rideIndex);
static bool   peep_find_ride_to_look_at(rct_peep * peep, uint8 edge, uint8 * rideToView, uint8 * rideSeatToView);
static void   peep_easter_egg_peep_interactions(rct_peep * peep);
static void   peep_head_for_nearest_ride_type(rct_peep * peep, sint32 rideType);
static void   peep_head_for_nearest_ride_with_flags(rct_peep * peep, sint32 rideTypeFlags);
static void   peep_give_real_name(rct_peep * peep);
static void   peep_release_balloon(rct_peep * peep, sint16 spawn_height);

// clang-format off
static constexpr const char *gPeepEasterEggNames[] = {
    "MICHAEL SCHUMACHER",
    "JACQUES VILLENEUVE",
    "DAMON HILL",
    "MR BEAN",
    "CHRIS SAWYER",
    "KATIE BRAYSHAW",
    "MELANIE WARN",
    "SIMON FOSTER",
    "JOHN WARDLEY",
    "LISA STIRLING",
    "DONALD MACRAE",
    "KATHERINE MCGOWAN",
    "FRANCES MCGOWAN",
    "CORINA MASSOURA",
    "CAROL YOUNG",
    "MIA SHERIDAN",
    "KATIE RODGER",
    "EMMA GARRELL",
    "JOANNE BARTON",
    "FELICITY ANDERSON",
    "KATIE SMITH",
    "EILIDH BELL",
    "NANCY STILLWAGON",
    "DAVID ELLIS"
};

/** rct2: 0x00981DB0 */
static struct
{
    uint8 action;
    uint8 flags;
} PeepThoughtToActionMap[] = {
    { PEEP_ACTION_SHAKE_HEAD, 1 },
    { PEEP_ACTION_EMPTY_POCKETS, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_WOW, 1 },
    { PEEP_ACTION_NONE_2, 2 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 4 },
    { PEEP_ACTION_SHAKE_HEAD, 4 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_WAVE, 0 },
    { PEEP_ACTION_JOY, 1 },
    { PEEP_ACTION_CHECK_TIME, 1 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_WAVE, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_WAVE, 0 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_DISGUST, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_BEING_WATCHED, 0 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_SHAKE_HEAD, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_JOY, 0 },
    { PEEP_ACTION_NONE_2, 1 },
};

/** rct2: 0x00981F2C, 0x00981F2E */
static constexpr const LocationXY16 _981F2C[] = {
    { 7, 12 }, { 12, 25 }, { 25, 20 }, { 20, 7 }, { 7, 20 }, { 20, 25 }, { 25, 12 }, { 12, 7 },
};

static uint8 PeepSpecialSpriteToSpriteTypeMap[] = {
    PEEP_ACTION_SPRITE_TYPE_NONE,
    PEEP_ACTION_SPRITE_TYPE_HOLD_MAT,
    PEEP_ACTION_SPRITE_TYPE_STAFF_MOWER
};

static uint8 PeepActionToSpriteTypeMap[] = {
    PEEP_ACTION_SPRITE_TYPE_CHECK_TIME,
    PEEP_ACTION_SPRITE_TYPE_EAT_FOOD,
    PEEP_ACTION_SPRITE_TYPE_SHAKE_HEAD,
    PEEP_ACTION_SPRITE_TYPE_EMPTY_POCKETS,
    PEEP_ACTION_SPRITE_TYPE_SITTING_EAT_FOOD,
    PEEP_ACTION_SPRITE_TYPE_SITTING_LOOK_AROUND_LEFT,
    PEEP_ACTION_SPRITE_TYPE_SITTING_LOOK_AROUND_RIGHT,
    PEEP_ACTION_SPRITE_TYPE_WOW,
    PEEP_ACTION_SPRITE_TYPE_THROW_UP,
    PEEP_ACTION_SPRITE_TYPE_JUMP,
    PEEP_ACTION_SPRITE_TYPE_STAFF_SWEEP,
    PEEP_ACTION_SPRITE_TYPE_DROWNING,
    PEEP_ACTION_SPRITE_TYPE_STAFF_ANSWER_CALL,
    PEEP_ACTION_SPRITE_TYPE_STAFF_ANSWER_CALL_2,
    PEEP_ACTION_SPRITE_TYPE_STAFF_CHECKBOARD,
    PEEP_ACTION_SPRITE_TYPE_STAFF_FIX,
    PEEP_ACTION_SPRITE_TYPE_STAFF_FIX_2,
    PEEP_ACTION_SPRITE_TYPE_STAFF_FIX_GROUND,
    PEEP_ACTION_SPRITE_TYPE_STAFF_FIX_3,
    PEEP_ACTION_SPRITE_TYPE_STAFF_WATERING,
    PEEP_ACTION_SPRITE_TYPE_JOY,
    PEEP_ACTION_SPRITE_TYPE_READ_MAP,
    PEEP_ACTION_SPRITE_TYPE_WAVE,
    PEEP_ACTION_SPRITE_TYPE_STAFF_EMPTY_BIN,
    PEEP_ACTION_SPRITE_TYPE_WAVE_2,
    PEEP_ACTION_SPRITE_TYPE_TAKE_PHOTO,
    PEEP_ACTION_SPRITE_TYPE_CLAP,
    PEEP_ACTION_SPRITE_TYPE_DISGUST,
    PEEP_ACTION_SPRITE_TYPE_DRAW_PICTURE,
    PEEP_ACTION_SPRITE_TYPE_BEING_WATCHED,
    PEEP_ACTION_SPRITE_TYPE_WITHDRAW_MONEY
};

static constexpr const bool SpriteTypeToSlowWalkMap[] = {
    false, false, false, false, false, false, false, false,
    false, false, false, true,  false, false, true,  true,
    true,  true,  true,  false, true,  false, true,  true,
    true,  false, false, true,  true,  false, false, true,
    true,  true,  true,  true,  true,  true,  false, true,
    false, true,  true,  true,  true,  true,  true,  true,
};

// These arrays contain the base minimum and maximum nausea ratings for peeps, based on their nausea tolerance level.
static constexpr const ride_rating NauseaMinimumThresholds[] = {
    0, 0, 200, 400
};

static constexpr const ride_rating NauseaMaximumThresholds[] = {
    300, 600, 800, 1000
};

// clang-format on

bool rct_sprite::IsPeep()
{
    return peep.sprite_identifier == SPRITE_IDENTIFIER_PEEP;
}

rct_peep * rct_sprite::AsPeep()
{
    rct_peep * result = nullptr;
    if (IsPeep())
    {
        return (rct_peep *)this;
    }
    return result;
}

void rct_peep::Invalidate()
{
    invalidate_sprite_2((rct_sprite*)this);
}

void rct_peep::MoveTo(sint16 destX, sint16 destY, sint16 destZ)
{
    sprite_move(destX, destY, destZ, (rct_sprite*)this);
}

rct_peep * try_get_guest(uint16 spriteIndex)
{
    rct_sprite * sprite = try_get_sprite(spriteIndex);
    if (sprite == nullptr)
        return nullptr;
    if (sprite->unknown.sprite_identifier != SPRITE_IDENTIFIER_PEEP)
        return nullptr;
    if (sprite->peep.type != PEEP_TYPE_GUEST)
        return nullptr;
    return &sprite->peep;
}

sint32 peep_get_staff_count()
{
    uint16     spriteIndex;
    rct_peep * peep;
    sint32     count = 0;

    FOR_ALL_STAFF(spriteIndex, peep)
    count++;

    return count;
}

/**
 *
 *  rct2: 0x0068F0A9
 */
void peep_update_all()
{
    sint32     i;
    uint16     spriteIndex;
    rct_peep * peep;

    if (gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        return;

    spriteIndex = gSpriteListHead[SPRITE_LIST_PEEP];
    i           = 0;
    while (spriteIndex != SPRITE_INDEX_NULL)
    {
        peep        = &(get_sprite(spriteIndex)->peep);
        spriteIndex = peep->next;

        if ((uint32)(i & 0x7F) != (gCurrentTicks & 0x7F))
        {
            peep->Update();
        }
        else
        {
            peep_128_tick_update(peep, i);
            if (peep->linked_list_type_offset == SPRITE_LIST_PEEP * 2)
            {
                peep->Update();
            }
        }

        i++;
    }
}

/**
 *
 *  rct2: 0x0069BC9A
 */
static uint8 peep_assess_surroundings(sint16 centre_x, sint16 centre_y, sint16 centre_z)
{
    if ((tile_element_height(centre_x, centre_y) & 0xFFFF) > centre_z)
        return PEEP_THOUGHT_TYPE_NONE;

    uint16 num_scenery   = 0;
    uint16 num_fountains = 0;
    uint16 nearby_music  = 0;
    uint16 num_rubbish   = 0;

    sint16 initial_x = Math::Max(centre_x - 160, 0);
    sint16 initial_y = Math::Max(centre_y - 160, 0);
    sint16 final_x   = Math::Min(centre_x + 160, 8192);
    sint16 final_y   = Math::Min(centre_y + 160, 8192);

    for (sint16 x = initial_x; x < final_x; x += 32)
    {
        for (sint16 y = initial_y; y < final_y; y += 32)
        {
            rct_tile_element * tileElement = map_get_first_element_at(x / 32, y / 32);

            do
            {
                Ride *              ride;
                rct_scenery_entry * scenery;

                switch (tile_element_get_type(tileElement))
                {
                case TILE_ELEMENT_TYPE_PATH:
                    if (!footpath_element_has_path_scenery(tileElement))
                        break;

                    scenery = get_footpath_item_entry(footpath_element_get_path_scenery_index(tileElement));
                    if (scenery == nullptr)
                    {
                        return PEEP_THOUGHT_TYPE_NONE;
                    }
                    if (footpath_element_path_scenery_is_ghost(tileElement))
                        break;

                    if (scenery->path_bit.flags & (PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER | PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW))
                    {
                        num_fountains++;
                        break;
                    }
                    if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
                    {
                        num_rubbish++;
                    }
                    break;
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                    num_scenery++;
                    break;
                case TILE_ELEMENT_TYPE_TRACK:
                    ride = get_ride(track_element_get_ride_index(tileElement));
                    if (ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC && ride->status != RIDE_STATUS_CLOSED &&
                        !(ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)))
                    {

                        if (ride->type == RIDE_TYPE_MERRY_GO_ROUND)
                        {
                            nearby_music |= 1;
                            break;
                        }

                        if (ride->music == MUSIC_STYLE_ORGAN)
                        {
                            nearby_music |= 1;
                            break;
                        }

                        if (ride->type == RIDE_TYPE_DODGEMS)
                        {
                            // Dodgems drown out music?
                            nearby_music |= 2;
                        }
                    }
                    break;
                }
            } while (!tile_element_is_last_for_tile(tileElement++));
        }
    }

    rct_litter * litter;
    for (uint16 sprite_idx = gSpriteListHead[SPRITE_LIST_LITTER]; sprite_idx != SPRITE_INDEX_NULL; sprite_idx = litter->next)
    {
        litter = &(get_sprite(sprite_idx)->litter);

        sint16 dist_x = abs(litter->x - centre_x);
        sint16 dist_y = abs(litter->y - centre_y);
        if (Math::Max(dist_x, dist_y) <= 160)
        {
            num_rubbish++;
        }
    }

    if (num_fountains >= 5 && num_rubbish < 20)
        return PEEP_THOUGHT_TYPE_FOUNTAINS;

    if (num_scenery >= 40 && num_rubbish < 8)
        return PEEP_THOUGHT_TYPE_SCENERY;

    if (nearby_music == 1 && num_rubbish < 20)
        return PEEP_THOUGHT_TYPE_MUSIC;

    if (num_rubbish < 2 && !gCheatsDisableLittering)
        // if disable littering cheat is enabled, peeps will not have the "clean and tidy park" thought
        return PEEP_THOUGHT_TYPE_VERY_CLEAN;

    return PEEP_THOUGHT_TYPE_NONE;
}

/**
 *
 *  rct2: 0x0068F9A9
 */
static void peep_update_hunger(rct_peep * peep)
{
    if (peep->hunger >= 3)
    {
        peep->hunger -= 2;

        peep->energy_target = Math::Min(peep->energy_target + 2, PEEP_MAX_ENERGY_TARGET);
        peep->toilet      = Math::Min(peep->toilet + 1, 255);
    }
}

/**
 *
 *  rct2: 0x0068F93E
 */
static void peep_leave_park(rct_peep * peep)
{
    peep->guest_heading_to_ride_id = 0xFF;
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
    {
        if (peep->peep_is_lost_countdown < 60)
        {
            return;
        }
    }
    else
    {
        peep->peep_is_lost_countdown = 254;
        peep->peep_flags |= PEEP_FLAGS_LEAVING_PARK;
        peep->peep_flags &= ~PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_GO_HOME, PEEP_THOUGHT_ITEM_NONE);

    rct_window * w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != nullptr)
        window_event_invalidate_call(w);
    window_invalidate_by_number(WC_PEEP, peep->sprite_index);
}

/**
 * Main purpose is to decide when peeps leave the park due to
 * low happiness, low energy and (if appropriate) low money.
 *
 *  rct2: 0x0068F8CD
 */
static void peep_decide_whether_to_leave_park(rct_peep * peep)
{
    if (peep->energy_target >= 33)
    {
        peep->energy_target -= 2;
    }

    if (gClimateCurrent.Temperature >= 21 && peep->thirst >= 5)
    {
        peep->thirst--;
    }

    if (peep->outside_of_park != 0)
    {
        return;
    }

    /* Peeps that are happy enough, have enough energy and
     * (if appropriate) have enough money will always stay
     * in the park. */
    if (!(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK))
    {
        if (gParkFlags & PARK_FLAGS_NO_MONEY)
        {
            if (peep->energy >= 70 && peep->happiness >= 60)
            {
                return;
            }
        }
        else
        {
            if (peep->energy >= 55 && peep->happiness >= 45 && peep->cash_in_pocket >= MONEY(5, 00))
            {
                return;
            }
        }
    }

    // Approx 95% chance of staying in the park
    if ((scenario_rand() & 0xFFFF) > 3276)
    {
        return;
    }

    // In the remaining 5% chance the peep leaves the park.
    peep_leave_park(peep);
}

// clang-format off
/** rct2: 0x009822F4, 0x00982310 */
static constexpr const uint8 item_consumption_time[] = {
    0,      // SHOP_ITEM_BALLOON
    0,      // SHOP_ITEM_TOY
    0,      // SHOP_ITEM_MAP
    0,      // SHOP_ITEM_PHOTO
    0,      // SHOP_ITEM_UMBRELLA
    100,    // SHOP_ITEM_DRINK
    150,    // SHOP_ITEM_BURGER
    120,    // SHOP_ITEM_CHIPS
    60,     // SHOP_ITEM_ICE_CREAM
    50,     // SHOP_ITEM_CANDYFLOSS
    0,      // SHOP_ITEM_EMPTY_CAN
    0,      // SHOP_ITEM_RUBBISH
    0,      // SHOP_ITEM_EMPTY_BURGER_BOX
    150,    // SHOP_ITEM_PIZZA
    0,      // SHOP_ITEM_VOUCHER
    75,     // SHOP_ITEM_POPCORN
    133,    // SHOP_ITEM_HOT_DOG
    110,    // SHOP_ITEM_TENTACLE
    0,      // SHOP_ITEM_HAT
    50,     // SHOP_ITEM_TOFFEE_APPLE
    0,      // SHOP_ITEM_TSHIRT
    80,     // SHOP_ITEM_DOUGHNUT
    90,     // SHOP_ITEM_COFFEE
    0,      // SHOP_ITEM_EMPTY_CUP
    170,    // SHOP_ITEM_CHICKEN
    115,    // SHOP_ITEM_LEMONADE
    0,      // SHOP_ITEM_EMPTY_BOX
    0,      // SHOP_ITEM_EMPTY_BOTTLE
    0xFF,   // UNUSED
    0xFF,   // UNUSED
    0xFF,   // UNUSED
    0xFF,   // UNUSED
    0,      // SHOP_ITEM_PHOTO2
    0,      // SHOP_ITEM_PHOTO3
    0,      // SHOP_ITEM_PHOTO4
    70,     // SHOP_ITEM_PRETZEL
    85,     // SHOP_ITEM_CHOCOLATE
    95,     // SHOP_ITEM_ICED_TEA
    90,     // SHOP_ITEM_FUNNEL_CAKE
    0,      // SHOP_ITEM_SUNGLASSES
    130,    // SHOP_ITEM_BEEF_NOODLES
    120,    // SHOP_ITEM_FRIED_RICE_NOODLES
    100,    // SHOP_ITEM_WONTON_SOUP
    110,    // SHOP_ITEM_MEATBALL_SOUP
    110,    // SHOP_ITEM_FRUIT_JUICE
    90,     // SHOP_ITEM_SOYBEAN_MILK
    100,    // SHOP_ITEM_SUJEONGGWA
    130,    // SHOP_ITEM_SUB_SANDWICH
    75,     // SHOP_ITEM_COOKIE
    0,      // SHOP_ITEM_EMPTY_BOWL_RED
    0,      // SHOP_ITEM_EMPTY_DRINK_CARTON
    0,      // SHOP_ITEM_EMPTY_JUICE_CUP
    115,    // SHOP_ITEM_ROAST_SAUSAGE
    0       // SHOP_ITEM_EMPTY_BOWL_BLUE
};

/** rct2: 009823AC */
static constexpr const uint8 crowded_thoughts[] = {
    PEEP_THOUGHT_TYPE_LOST,
    PEEP_THOUGHT_TYPE_TIRED,
    PEEP_THOUGHT_TYPE_BAD_LITTER,
    PEEP_THOUGHT_TYPE_HUNGRY,
    PEEP_THOUGHT_TYPE_THIRSTY,
    PEEP_THOUGHT_TYPE_VERY_CLEAN,
    PEEP_THOUGHT_TYPE_CROWDED,
    PEEP_THOUGHT_TYPE_SCENERY,
    PEEP_THOUGHT_TYPE_VERY_CLEAN,
    PEEP_THOUGHT_TYPE_MUSIC,
    PEEP_THOUGHT_TYPE_WATCHED,
    PEEP_THOUGHT_TYPE_NOT_HUNGRY,
    PEEP_THOUGHT_TYPE_NOT_THIRSTY,
    PEEP_THOUGHT_TYPE_BATHROOM,
    PEEP_THOUGHT_TYPE_NONE,
    PEEP_THOUGHT_TYPE_NONE,
};

/** rct2: 0x00982326 */
static constexpr const uint8 peep_item_containers[] = {
    0xFF,                           // PEEP_ITEM_BALLOON
    0xFF,                           // PEEP_ITEM_TOY
    0xFF,                           // PEEP_ITEM_MAP
    0xFF,                           // PEEP_ITEM_PHOTO
    0xFF,                           // PEEP_ITEM_UMBRELLA
    SHOP_ITEM_EMPTY_CAN,            // PEEP_ITEM_DRINK
    SHOP_ITEM_EMPTY_BURGER_BOX,     // PEEP_ITEM_BURGER
    SHOP_ITEM_RUBBISH,              // PEEP_ITEM_CHIPS
    0xFF,                           // PEEP_ITEM_ICE_CREAM
    0xFF,                           // PEEP_ITEM_CANDYFLOSS
    0xFF,                           // PEEP_ITEM_EMPTY_CAN
    0xFF,                           // PEEP_ITEM_RUBBISH
    0xFF,                           // PEEP_ITEM_EMPTY_BURGER_BOX
    SHOP_ITEM_RUBBISH,              // PEEP_ITEM_PIZZA
    0xFF,                           // PEEP_ITEM_VOUCHER
    SHOP_ITEM_RUBBISH,              // PEEP_ITEM_POPCORN
    0xFF,                           // PEEP_ITEM_HOT_DOG
    0xFF,                           // PEEP_ITEM_TENTACLE
    0xFF,                           // PEEP_ITEM_HAT
    0xFF,                           // PEEP_ITEM_TOFFEE_APPLE
    0xFF,                           // PEEP_ITEM_TSHIRT
    0xFF,                           // PEEP_ITEM_DOUGHNUT
    SHOP_ITEM_EMPTY_CUP,            // PEEP_ITEM_COFFEE
    0xFF,                           // PEEP_ITEM_EMPTY_CUP
    SHOP_ITEM_EMPTY_BOX,            // PEEP_ITEM_CHICKEN
    SHOP_ITEM_EMPTY_BOTTLE,         // PEEP_ITEM_LEMONADE
    0xFF,                           // PEEP_ITEM_EMPTY_BOX
    0xFF,                           // PEEP_ITEM_EMPTY_BOTTLE
};

/** rct2: 0x00982342 */
static constexpr const uint8 peep_extra_item_containers[] = {
    0xFF,                           // PEEP_ITEM_PHOTO2
    0xFF,                           // PEEP_ITEM_PHOTO3
    0xFF,                           // PEEP_ITEM_PHOTO4
    0xFF,                           // PEEP_ITEM_PRETZEL
    SHOP_ITEM_EMPTY_CUP,            // PEEP_ITEM_CHOCOLATE
    SHOP_ITEM_EMPTY_CUP,            // PEEP_ITEM_ICED_TEA
    0xFF,                           // PEEP_ITEM_FUNNEL_CAKE
    0xFF,                           // PEEP_ITEM_SUNGLASSES
    SHOP_ITEM_EMPTY_BOWL_BLUE,      // PEEP_ITEM_BEEF_NOODLES
    SHOP_ITEM_EMPTY_BOWL_BLUE,      // PEEP_ITEM_FRIED_RICE_NOODLES
    SHOP_ITEM_EMPTY_BOWL_RED,       // PEEP_ITEM_WONTON_SOUP
    SHOP_ITEM_EMPTY_BOWL_RED,       // PEEP_ITEM_MEATBALL_SOUP
    SHOP_ITEM_EMPTY_JUICE_CUP,      // PEEP_ITEM_FRUIT_JUICE
    SHOP_ITEM_EMPTY_DRINK_CARTON,   // PEEP_ITEM_SOYBEAN_MILK
    SHOP_ITEM_EMPTY_DRINK_CARTON,   // PEEP_ITEM_SU_JONGKWA
    0xFF,                           // PEEP_ITEM_SUB_SANDWICH
    0xFF,                           // PEEP_ITEM_COOKIE
    0xFF,                           // PEEP_ITEM_EMPTY_BOWL_RED
    0xFF,                           // PEEP_ITEM_EMPTY_DRINK_CARTON
    0xFF,                           // PEEP_ITEM_EMPTY_JUICE_CUP
    0xFF,                           // PEEP_ITEM_ROAST_SAUSAGE
    0xFF,                           // PEEP_ITEM_EMPTY_BOWL_BLUE
};
// clang-format on

/**
 *
 *  rct2: 0x0068F41A
 *  Called every 128 ticks
 */
static void peep_128_tick_update(rct_peep * peep, sint32 index)
{
    if (peep->type == PEEP_TYPE_STAFF)
    {
        if (peep->staff_type != STAFF_TYPE_SECURITY)
            return;

        uint8 sprite_type = PEEP_SPRITE_TYPE_SECURITY_ALT;
        if (peep->state != PEEP_STATE_PATROLLING)
            sprite_type = PEEP_SPRITE_TYPE_SECURITY;

        if (peep->sprite_type == sprite_type)
            return;

        peep->sprite_type                = sprite_type;
        peep->action_sprite_image_offset = 0;
        peep->no_action_frame_num         = 0;
        if (peep->action < PEEP_ACTION_NONE_1)
            peep->action = PEEP_ACTION_NONE_2;

        peep->peep_flags &= ~PEEP_FLAGS_SLOW_WALK;
        if (SpriteTypeToSlowWalkMap[sprite_type])
        {
            peep->peep_flags |= PEEP_FLAGS_SLOW_WALK;
        }

        peep->action_sprite_type = 0xFF;
        peep->UpdateCurrentActionSpriteType();
        return;
    }

    if ((uint32)(index & 0x1FF) == (gCurrentTicks & 0x1FF))
    {
        /* Effect of masking with 0x1FF here vs mask 0x7F,
         * which is the condition for calling this function, is
         * to reduce how often the content in this conditional
         * is executed to once every four calls. */
        if (peep->peep_flags & PEEP_FLAGS_CROWDED)
        {
            uint8 thought_type = crowded_thoughts[scenario_rand() & 0xF];
            if (thought_type != PEEP_THOUGHT_TYPE_NONE)
            {
                peep_insert_new_thought(peep, thought_type, PEEP_THOUGHT_ITEM_NONE);
            }
        }

        if (peep->peep_flags & PEEP_FLAGS_EXPLODE && peep->x != LOCATION_NULL)
        {
            if (peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_SITTING)
            {
                audio_play_sound_at_location(SOUND_CRASH, peep->x, peep->y, peep->z);

                sprite_misc_explosion_cloud_create(peep->x, peep->y, peep->z + 16);
                sprite_misc_explosion_flare_create(peep->x, peep->y, peep->z + 16);

                peep->Remove();
                return;
            }
            else
            {
                peep->peep_flags &= ~PEEP_FLAGS_EXPLODE;
            }
        }

        if (peep->peep_flags & PEEP_FLAGS_HUNGER)
        {
            if (peep->hunger >= 15)
                peep->hunger -= 15;
        }

        if (peep->peep_flags & PEEP_FLAGS_BATHROOM)
        {
            if (peep->toilet <= 180)
                peep->toilet += 50;
        }

        if (peep->peep_flags & PEEP_FLAGS_HAPPINESS)
        {
            peep->happiness_target = 5;
        }

        if (peep->peep_flags & PEEP_FLAGS_NAUSEA)
        {
            peep->nausea_target = 200;
            if (peep->nausea <= 130)
                peep->nausea = 130;
        }

        if (peep->angriness != 0)
            peep->angriness--;

        if (peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_SITTING)
        {
            peep->surroundings_thought_timeout++;
            if (peep->surroundings_thought_timeout >= 18)
            {
                peep->surroundings_thought_timeout = 0;
                if (peep->x != LOCATION_NULL)
                {

                    uint8 thought_type = peep_assess_surroundings(peep->x & 0xFFE0, peep->y & 0xFFE0, peep->z);

                    if (thought_type != PEEP_THOUGHT_TYPE_NONE)
                    {
                        peep_insert_new_thought(peep, thought_type, PEEP_THOUGHT_ITEM_NONE);
                        peep->happiness_target = Math::Min(PEEP_MAX_HAPPINESS, peep->happiness_target + 45);
                    }
                }
            }
        }

        peep_update_sprite_type(peep);

        if (peep->state == PEEP_STATE_ON_RIDE || peep->state == PEEP_STATE_ENTERING_RIDE)
        {
            peep->time_on_ride = Math::Min(255, peep->time_on_ride + 1);

            if (peep->peep_flags & PEEP_FLAGS_WOW)
            {
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_WOW2, PEEP_THOUGHT_ITEM_NONE);
            }

            if (peep->time_on_ride > 15)
            {
                peep->happiness_target = Math::Max(0, peep->happiness_target - 5);

                if (peep->time_on_ride > 22)
                {
                    Ride * ride = get_ride(peep->current_ride);

                    uint8 thought_type = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE) ? PEEP_THOUGHT_TYPE_GET_OUT
                                                                                                : PEEP_THOUGHT_TYPE_GET_OFF;

                    peep_insert_new_thought(peep, thought_type, peep->current_ride);
                }
            }
        }

        if (peep->state == PEEP_STATE_WALKING && peep->outside_of_park == 0 && !(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) &&
            peep->no_of_rides == 0 && peep->guest_heading_to_ride_id == 0xFF)
        {

            uint32 time_duration = gScenarioTicks - peep->time_in_park;
            time_duration /= 2048;

            if (time_duration >= 5)
            {
                peep->PickRideToGoOn();

                if (peep->guest_heading_to_ride_id == 0xFF)
                {
                    peep->happiness_target = Math::Max(peep->happiness_target - 128, 0);
                    peep_leave_park(peep);
                    peep_update_hunger(peep);
                    goto loc_68F9F3;
                }
            }
        }

        if ((scenario_rand() & 0xFFFF) <= ((peep->item_standard_flags & PEEP_ITEM_MAP) ? 8192U : 2184U))
        {
            peep->PickRideToGoOn();
        }

        if ((uint32)(index & 0x3FF) == (gCurrentTicks & 0x3FF))
        {
            /* Effect of masking with 0x3FF here vs mask 0x1FF,
             * which is used in the encompassing conditional, is
             * to reduce how often the content in this conditional
             * is executed to once every second time the encompassing
             * conditional executes. */

            if (peep->outside_of_park == 0 && (peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_SITTING))
            {

                uint8 num_thoughts         = 0;
                uint8 possible_thoughts[5] = { 0 };

                if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
                {
                    possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_GO_HOME;
                }
                else
                {
                    if (peep->energy <= 70 && peep->happiness < 128)
                    {
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_TIRED;
                    }

                    if (peep->hunger <= 10 && !peep->HasFood())
                    {
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_HUNGRY;
                    }

                    if (peep->thirst <= 25 && !peep->HasFood())
                    {
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_THIRSTY;
                    }

                    if (peep->toilet >= 160)
                    {
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_BATHROOM;
                    }

                    if (!(gParkFlags & PARK_FLAGS_NO_MONEY) && peep->cash_in_pocket <= MONEY(9, 00) && peep->happiness >= 105 &&
                        peep->energy >= 70)
                    {
                        /* The energy check was originally a second check on happiness.
                         * This was superfluous so should probably check something else.
                         * Guessed that this should really be checking energy, since
                         * the addresses for happiness and energy are quite close,
                         * 70 is also the threshold for tired thoughts (see above) and
                         * it makes sense that a tired peep might not think about getting
                         * more money. */
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_RUNNING_OUT;
                    }
                }

                if (num_thoughts != 0)
                {
                    uint8 chosen_thought = possible_thoughts[scenario_rand() % num_thoughts];

                    peep_insert_new_thought(peep, chosen_thought, PEEP_THOUGHT_ITEM_NONE);

                    switch (chosen_thought)
                    {
                    case PEEP_THOUGHT_TYPE_HUNGRY:
                        peep_head_for_nearest_ride_with_flags(peep, RIDE_TYPE_FLAG_SELLS_FOOD);
                        break;
                    case PEEP_THOUGHT_TYPE_THIRSTY:
                        peep_head_for_nearest_ride_with_flags(peep, RIDE_TYPE_FLAG_SELLS_DRINKS);
                        break;
                    case PEEP_THOUGHT_TYPE_BATHROOM:
                        peep_head_for_nearest_ride_with_flags(peep, RIDE_TYPE_FLAG_IS_BATHROOM);
                        break;
                    case PEEP_THOUGHT_TYPE_RUNNING_OUT:
                        peep_head_for_nearest_ride_type(peep, RIDE_TYPE_CASH_MACHINE);
                        break;
                    }
                }
            }
        }
        else
        {
            /* This branch of the conditional is executed on the
             * remaining times the encompassing conditional is
             * executed (which is also every second time, but
             * the alternate time to the true branch). */
            if (peep->nausea >= 140)
            {
                uint8 thought_type = PEEP_THOUGHT_TYPE_SICK;
                if (peep->nausea >= 200)
                {
                    thought_type = PEEP_THOUGHT_TYPE_VERY_SICK;
                    peep_head_for_nearest_ride_type(peep, RIDE_TYPE_FIRST_AID);
                }
                peep_insert_new_thought(peep, thought_type, PEEP_THOUGHT_ITEM_NONE);
            }
        }

        switch (peep->state)
        {
        case PEEP_STATE_WALKING:
        case PEEP_STATE_LEAVING_PARK:
        case PEEP_STATE_ENTERING_PARK:
            peep_decide_whether_to_leave_park(peep);
            peep_update_hunger(peep);
            break;

        case PEEP_STATE_SITTING:
            if (peep->energy_target <= 135)
                peep->energy_target += 5;

            if (peep->thirst >= 5)
            {
                peep->thirst -= 4;
                peep->toilet = Math::Min(255, peep->toilet + 3);
            }

            if (peep->nausea_target >= 50)
                peep->nausea_target -= 6;

            // In the original this branched differently
            // but it would mean setting the peep happiness from
            // a thought type entry which i think is incorrect.
            peep_update_hunger(peep);
            break;

        case PEEP_STATE_QUEUING:
            if (peep->time_in_queue >= 2000)
            {
                /* Peep happiness is affected once the peep has been waiting
                 * too long in a queue. */
                rct_tile_element * tileElement = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);
                bool               found       = false;
                do
                {
                    if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_PATH)
                        continue;
                    if (tileElement->base_height != peep->next_z)
                        continue;

                    // Check if the footpath has a queue line TV monitor on it
                    if (footpath_element_has_path_scenery(tileElement) && !footpath_element_path_scenery_is_ghost(tileElement))
                    {
                        uint8               pathSceneryIndex = footpath_element_get_path_scenery_index(tileElement);
                        rct_scenery_entry * sceneryEntry     = get_footpath_item_entry(pathSceneryIndex);
                        if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_QUEUE_SCREEN)
                        {
                            found = true;
                        }
                    }
                    break;
                } while (!tile_element_is_last_for_tile(tileElement++));

                if (found)
                {
                    /* Queue line TV monitors make the peeps waiting in the queue
                     * slowly happier, up to a certain level. */
                    /* Why don't queue line TV monitors start affecting the peeps
                     * as soon as they join the queue?? */
                    if (peep->happiness_target < 90)
                        peep->happiness_target = 90;

                    if (peep->happiness_target < 165)
                        peep->happiness_target += 2;
                }
                else
                {
                    /* Without a queue line TV monitor peeps waiting too long
                     * in a queue get less happy. */
                    peep->happiness_target = Math::Max(peep->happiness_target - 4, 0);
                }
            }
            peep_update_hunger(peep);
            break;
        case PEEP_STATE_ENTERING_RIDE:
            if (peep->sub_state == 17 || peep->sub_state == 15)
            {
                peep_decide_whether_to_leave_park(peep);
            }
            peep_update_hunger(peep);
            break;
        }

    loc_68F9F3:
        // Idle peep happiness tends towards 127 (50%).
        if (peep->happiness_target >= 128)
            peep->happiness_target--;
        else
            peep->happiness_target++;

        peep->nausea_target = Math::Max(peep->nausea_target - 2, 0);

        if (peep->energy <= 50)
        {
            peep->energy = Math::Max(peep->energy - 2, 0);
        }

        if (peep->hunger < 10)
        {
            peep->hunger = Math::Max(peep->hunger - 1, 0);
        }

        if (peep->thirst < 10)
        {
            peep->thirst = Math::Max(peep->thirst - 1, 0);
        }

        if (peep->toilet >= 195)
        {
            peep->toilet--;
        }

        if (peep->state == PEEP_STATE_WALKING && peep->nausea_target >= 128)
        {

            if ((scenario_rand() & 0xFF) <= (uint8)((peep->nausea - 128) / 2))
            {
                if (peep->action >= PEEP_ACTION_NONE_1)
                {
                    peep->action                     = PEEP_ACTION_THROW_UP;
                    peep->action_frame               = 0;
                    peep->action_sprite_image_offset = 0;
                    peep->UpdateCurrentActionSpriteType();
                    peep->Invalidate();
                }
            }
        }
    }

    // Remaining content is executed every call.

    // 68FA89
    if (peep->time_to_consume == 0 && peep->HasFood())
    {
        peep->time_to_consume += 3;
    }

    if (peep->time_to_consume != 0 && peep->state != PEEP_STATE_ON_RIDE)
    {

        peep->time_to_consume = Math::Max(peep->time_to_consume - 3, 0);

        if (peep->HasDrink())
        {
            peep->thirst = Math::Min(peep->thirst + 7, 255);
        }
        else
        {
            peep->hunger = Math::Min(peep->hunger + 7, 255);
            peep->thirst = Math::Max(peep->thirst - 3, 0);
            peep->toilet = Math::Min(peep->toilet + 2, 255);
        }

        if (peep->time_to_consume == 0)
        {
            sint32 chosen_food = bitscanforward(peep->HasFoodStandardFlag());
            if (chosen_food != -1)
            {
                peep->item_standard_flags &= ~(1 << chosen_food);

                uint8 discard_container = peep_item_containers[chosen_food];
                if (discard_container != 0xFF)
                {
                    peep->item_standard_flags |= (1 << discard_container);
                }

                peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                peep_update_sprite_type(peep);
            }
            else
            {
                chosen_food = bitscanforward(peep->HasFoodExtraFlag());
                if (chosen_food != -1)
                {
                    peep->item_extra_flags &= ~(1 << chosen_food);
                    uint8 discard_container = peep_extra_item_containers[chosen_food];
                    if (discard_container != 0xFF)
                    {
                        if (discard_container >= 32)
                            peep->item_extra_flags |= (1 << (discard_container - 32));
                        else
                            peep->item_standard_flags |= (1 << discard_container);
                    }

                    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                    peep_update_sprite_type(peep);
                }
            }
        }
    }

    uint8 energy        = peep->energy;
    uint8 energy_target = peep->energy_target;
    if (energy >= energy_target)
    {
        energy -= 2;
        if (energy < energy_target)
            energy = energy_target;
    }
    else
    {
        energy = Math::Min(PEEP_MAX_ENERGY_TARGET, energy + 4);
        if (energy > energy_target)
            energy = energy_target;
    }

    if (energy < PEEP_MIN_ENERGY)
        energy = PEEP_MIN_ENERGY;

    /* Previous code here suggested maximum energy is 128. */
    energy = Math::Min(static_cast<uint8>(PEEP_MAX_ENERGY), energy);

    if (energy != peep->energy)
    {
        peep->energy = energy;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;
    }

    uint8 happiness        = peep->happiness;
    uint8 happiness_growth = peep->happiness_target;
    if (happiness >= happiness_growth)
    {
        happiness = Math::Max(happiness - 4, 0);
        if (happiness < happiness_growth)
            happiness = happiness_growth;
    }
    else
    {
        happiness = Math::Min(255, happiness + 4);
        if (happiness > happiness_growth)
            happiness = happiness_growth;
    }

    if (happiness != peep->happiness)
    {
        peep->happiness = happiness;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;
    }

    uint8 nausea        = peep->nausea;
    uint8 nausea_growth = peep->nausea_target;
    if (nausea >= nausea_growth)
    {
        nausea = Math::Max(nausea - 4, 0);
        if (nausea < nausea_growth)
            nausea = nausea_growth;
    }
    else
    {
        nausea = Math::Min(255, nausea + 4);
        if (nausea > nausea_growth)
            nausea = nausea_growth;
    }

    if (nausea != peep->nausea)
    {
        peep->nausea = nausea;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;
    }
}

/*
 * rct2: 0x68F3AE
 * Set peep state to falling if path below has gone missing, return true if current path is valid, false if peep starts falling.
 */
bool rct_peep::CheckForPath()
{
    path_check_optimisation++;
    if ((path_check_optimisation & 0xF) != (sprite_index & 0xF))
    {
        // This condition makes the check happen less often 
        // As a side effect peeps hover for a short,
        // random time when a path below them has been deleted
        return true;
    }

    rct_tile_element * tile_element = map_get_first_element_at(next_x / 32, next_y / 32);

    uint8 map_type = TILE_ELEMENT_TYPE_PATH;
    if (next_var_29 & ((1 << 4) | (1 << 3)))
    {
        map_type = TILE_ELEMENT_TYPE_SURFACE;
    }

    sint32 height = next_z;

    do
    {
        if (tile_element_get_type(tile_element) == map_type)
        {
            if (height == tile_element->base_height)
            {
                // Found a suitable path or surface
                return true;
            }
        }
    } while (!tile_element_is_last_for_tile(tile_element++));

    // Found no suitable path
    SetState(PEEP_STATE_FALLING);
    return false;
}

uint8 rct_peep::GetActionSpriteType()
{
    if (action >= PEEP_ACTION_NONE_1)
    { // PEEP_ACTION_NONE_1 or PEEP_ACTION_NONE_2
        return PeepSpecialSpriteToSpriteTypeMap[special_sprite];
    }
    else if (action < Util::CountOf(PeepActionToSpriteTypeMap))
    {
        return PeepActionToSpriteTypeMap[action];
    }
    else
    {
        openrct2_assert(action >= Util::CountOf(PeepActionToSpriteTypeMap) && action < PEEP_ACTION_NONE_1,
                        "Invalid peep action %u", action);
        return 0;
    }
}

/*
 *  rct2: 0x00693B58
 */
void rct_peep::UpdateCurrentActionSpriteType()
{
    if (sprite_type >= Util::CountOf(g_peep_animation_entries))
    {
        return;
    }
    uint8 newActionSpriteType = GetActionSpriteType();
    if (action_sprite_type == newActionSpriteType)
    {
        return;
    }

    Invalidate();
    action_sprite_type = newActionSpriteType;

    const rct_sprite_bounds * spriteBounds = g_peep_animation_entries[sprite_type].sprite_bounds;
    sprite_width                     = spriteBounds[action_sprite_type].sprite_width;
    sprite_height_negative           = spriteBounds[action_sprite_type].sprite_height_negative;
    sprite_height_positive           = spriteBounds[action_sprite_type].sprite_height_positive;

    Invalidate();
}

/* rct2: 0x00693BE5 */
void rct_peep::SwitchToSpecialSprite(uint8 special_sprite_id)
{
    if (special_sprite_id == special_sprite)
        return;

    special_sprite = special_sprite_id;

    // If NONE_1 or NONE_2
    if (action >= PEEP_ACTION_NONE_1)
    {
        action_sprite_image_offset = 0;
    }
    UpdateCurrentActionSpriteType();
}

/**
 *
 *  rct2: 0x0069A512
 */
void rct_peep::RemoveFromRide()
{
    if (state == PEEP_STATE_QUEUING)
    {
        RemoveFromQueue();
    }
    StateReset();
}

void rct_peep::StateReset()
{
    SetState(PEEP_STATE_1);
    SwitchToSpecialSprite(0);
}

/**
 *
 *  rct2: 0x69C308
 * Check if lost.
 */
void rct_peep::CheckIfLost()
{
    if (!(peep_flags & PEEP_FLAGS_LOST))
    {
        if (gRideCount < 2)
            return;
        peep_flags ^= PEEP_FLAGS_21;

        if (!(peep_flags & PEEP_FLAGS_21))
            return;

        time_lost++;
        if (time_lost != 254)
            return;
        time_lost = 230;
    }
    peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_LOST, PEEP_THOUGHT_ITEM_NONE);

    happiness_target = Math::Max(happiness_target - 30, 0);
}

/**
 *
 *  rct2: 0x69C26B
 * Check if cant find ride.
 */
void rct_peep::CheckCantFindRide()
{
    if (guest_heading_to_ride_id == 0xFF)
        return;

    // Peeps will think "I can't find ride X" twice before giving up completely.
    if (peep_is_lost_countdown == 30 || peep_is_lost_countdown == 60)
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_CANT_FIND, guest_heading_to_ride_id);
        happiness_target = Math::Max(happiness_target - 30, 0);
    }

    peep_is_lost_countdown--;
    if (peep_is_lost_countdown != 0)
        return;

    guest_heading_to_ride_id = 0xFF;
    rct_window * w                 = window_find_by_number(WC_PEEP, sprite_index);

    if (w)
    {
        window_event_invalidate_call(w);
    }

    window_invalidate_by_number(WC_PEEP, sprite_index);
}

/**
 *
 *  rct2: 0x69C2D0
 * Check if cant find exit.
 */
void rct_peep::CheckCantFindExit()
{
    if (!(peep_flags & PEEP_FLAGS_LEAVING_PARK))
        return;

    // Peeps who can't find the park exit will continue to get less happy until they find it.
    if (peep_is_lost_countdown == 1)
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_CANT_FIND_EXIT, PEEP_THOUGHT_ITEM_NONE);
        happiness_target = Math::Max(happiness_target - 30, 0);
    }

    if (--peep_is_lost_countdown == 0)
        peep_is_lost_countdown = 90;
}

/** rct2: 0x00981D7C, 0x00981D7E */
static constexpr const LocationXY16 word_981D7C[4] = { { -2, 0 }, { 0, 2 }, { 2, 0 }, { 0, -2 } };

bool rct_peep::UpdateAction()
{
    sint16 actionX, actionY, xy_distance;
    return UpdateAction(&actionX, &actionY, &xy_distance);
}

/**
 *
 *  rct2: 0x6939EB
 * Also used to move peeps to the correct position to
 * start an action. Returns true if the correct destination
 * has not yet been reached. xy_distance is how close the
 * peep is to the target.
 *
 * @param actionX (ax)
 * @param actionY (cx)
 * @param distance (bp)
 * @param peep (esi)
 */
bool rct_peep::UpdateAction(sint16 * actionX, sint16 * actionY, sint16 * xy_distance)
{
    _unk_F1AEF0 = action_sprite_image_offset;
    if (action == PEEP_ACTION_NONE_1)
    {
        action = PEEP_ACTION_NONE_2;
    }

    *actionX = x - destination_x;
    *actionY = y - destination_y;

    sint32 x_delta = abs(*actionX);
    sint32 y_delta = abs(*actionY);

    *xy_distance = x_delta + y_delta;

    if (action == PEEP_ACTION_NONE_1 || action == PEEP_ACTION_NONE_2)
    {
        if (*xy_distance <= destination_tolerance)
        {
            return false;
        }
        sint32 nextDirection = 0;
        if (x_delta < y_delta)
        {
            nextDirection = 8;
            if (*actionY >= 0)
            {
                nextDirection = 24;
            }
        }
        else
        {
            nextDirection = 16;
            if (*actionX >= 0)
            {
                nextDirection = 0;
            }
        }
        sprite_direction = nextDirection;
        *actionX                     = x + word_981D7C[nextDirection / 8].x;
        *actionY                     = y + word_981D7C[nextDirection / 8].y;
        no_action_frame_num++;
        const rct_peep_animation * peepAnimation = g_peep_animation_entries[sprite_type].sprite_animation;
        const uint8 *              imageOffset   = peepAnimation[action_sprite_type].frame_offsets;
        if (no_action_frame_num >= peepAnimation[action_sprite_type].num_frames)
        {
            no_action_frame_num = 0;
        }
        action_sprite_image_offset = imageOffset[no_action_frame_num];
        return true;
    }

    const rct_peep_animation * peepAnimation = g_peep_animation_entries[sprite_type].sprite_animation;
    action_frame++;

    // If last frame of action
    if (action_frame >= peepAnimation[action_sprite_type].num_frames)
    {
        action_sprite_image_offset = 0;
        action                     = 0xFF;
        UpdateCurrentActionSpriteType();
        Invalidate();
        *actionX = x;
        *actionY = y;
        return true;
    }
    action_sprite_image_offset = peepAnimation[action_sprite_type].frame_offsets[action_frame];

    // If not throwing up and not at the frame where sick appears.
    if (action != PEEP_ACTION_THROW_UP || action_frame != 15)
    {
        Invalidate();
        *actionX = x;
        *actionY = y;
        return true;
    }

    // We are throwing up
    hunger /= 2;
    nausea_target /= 2;

    if (nausea < 30)
        nausea = 0;
    else
        nausea -= 30;

    window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;

    // Create sick at location
    litter_create(x, y, z, sprite_direction,
                  (sprite_index & 1) ? LITTER_TYPE_SICK_ALT : LITTER_TYPE_SICK);

    sint32 sound_id = SOUND_COUGH_1 + (scenario_rand() & 3);
    audio_play_sound_at_location(sound_id, x, y, z);

    Invalidate();
    *actionX = x;
    *actionY = y;
    return true;
}

/**
 *  rct2: 0x0069A409
 * Decreases rider count if on/entering a ride.
 */
void peep_decrement_num_riders(rct_peep * peep)
{
    if (peep->state == PEEP_STATE_ON_RIDE || peep->state == PEEP_STATE_ENTERING_RIDE)
    {

        Ride * ride = get_ride(peep->current_ride);
        ride->num_riders = std::max(0, ride->num_riders - 1);
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
    }
}

/* Part of 0x0069B8CC rct2: 0x0069BC31 */
static void set_sprite_type(rct_peep * peep, uint8 type)
{
    if (peep->sprite_type == type)
        return;

    peep->sprite_type                = type;
    peep->action_sprite_image_offset = 0;
    peep->no_action_frame_num         = 0;

    if (peep->action >= PEEP_ACTION_NONE_1)
        peep->action = PEEP_ACTION_NONE_2;

    peep->peep_flags &= ~PEEP_FLAGS_SLOW_WALK;
    assert(type < Util::CountOf(SpriteTypeToSlowWalkMap));
    if (SpriteTypeToSlowWalkMap[type])
    {
        peep->peep_flags |= PEEP_FLAGS_SLOW_WALK;
    }

    peep->action_sprite_type = 0xFF;
    peep->UpdateCurrentActionSpriteType();

    if (peep->state == PEEP_STATE_SITTING)
    {
        peep->action                  = PEEP_ACTION_NONE_1;
        peep->next_action_sprite_type = 7;
        peep->SwitchNextActionSpriteType();
    }
    if (peep->state == PEEP_STATE_WATCHING)
    {
        peep->action                  = PEEP_ACTION_NONE_1;
        peep->next_action_sprite_type = 2;
        peep->SwitchNextActionSpriteType();
    }
}

struct item_pref_t
{
    uint8  type; // 0 for standard, 1 for extra
    uint32 item; // And this with the relevant flags
    uint8  sprite_type;
};

// clang-format off
static item_pref_t item_order_preference[] = {
        { 0, PEEP_ITEM_ICE_CREAM, PEEP_SPRITE_TYPE_ICE_CREAM },
        { 0, PEEP_ITEM_CHIPS, PEEP_SPRITE_TYPE_CHIPS },
        { 0, PEEP_ITEM_PIZZA, PEEP_SPRITE_TYPE_PIZZA },
        { 0, PEEP_ITEM_BURGER, PEEP_SPRITE_TYPE_BURGER },
        { 0, PEEP_ITEM_DRINK, PEEP_SPRITE_TYPE_DRINK },
        { 0, PEEP_ITEM_COFFEE, PEEP_SPRITE_TYPE_COFFEE },
        { 0, PEEP_ITEM_CHICKEN, PEEP_SPRITE_TYPE_CHICKEN },
        { 0, PEEP_ITEM_LEMONADE, PEEP_SPRITE_TYPE_LEMONADE },
        { 0, PEEP_ITEM_CANDYFLOSS, PEEP_SPRITE_TYPE_CANDYFLOSS },
        { 0, PEEP_ITEM_POPCORN, PEEP_SPRITE_TYPE_PIZZA },
        { 0, PEEP_ITEM_HOT_DOG, PEEP_SPRITE_TYPE_HOT_DOG  },
        { 0, PEEP_ITEM_TENTACLE, PEEP_SPRITE_TYPE_TENTACLE },
        { 0, PEEP_ITEM_TOFFEE_APPLE, PEEP_SPRITE_TYPE_TOFFEE_APPLE },
        { 0, PEEP_ITEM_DOUGHNUT, PEEP_SPRITE_TYPE_DOUGHNUT },
        { 1, PEEP_ITEM_PRETZEL, PEEP_SPRITE_TYPE_PRETZEL },
        { 1, PEEP_ITEM_COOKIE, PEEP_SPRITE_TYPE_PRETZEL },
        { 1, PEEP_ITEM_CHOCOLATE, PEEP_SPRITE_TYPE_COFFEE },
        { 1, PEEP_ITEM_ICED_TEA, PEEP_SPRITE_TYPE_COFFEE },
        { 1, PEEP_ITEM_FUNNEL_CAKE, PEEP_SPRITE_TYPE_FUNNEL_CAKE },
        { 1, PEEP_ITEM_BEEF_NOODLES, PEEP_SPRITE_TYPE_NOODLES },
        { 1, PEEP_ITEM_FRIED_RICE_NOODLES, PEEP_SPRITE_TYPE_NOODLES },
        { 1, PEEP_ITEM_WONTON_SOUP, PEEP_SPRITE_TYPE_SOUP },
        { 1, PEEP_ITEM_MEATBALL_SOUP, PEEP_SPRITE_TYPE_SOUP },
        { 1, PEEP_ITEM_FRUIT_JUICE, PEEP_SPRITE_TYPE_JUICE },
        { 1, PEEP_ITEM_SOYBEAN_MILK, PEEP_SPRITE_TYPE_SU_JONGKWA },
        { 1, PEEP_ITEM_SU_JONGKWA, PEEP_SPRITE_TYPE_SU_JONGKWA },
        { 1, PEEP_ITEM_SUB_SANDWICH, PEEP_SPRITE_TYPE_SANDWICH },
        { 1, PEEP_ITEM_ROAST_SAUSAGE, PEEP_SPRITE_TYPE_SAUSAGE },
        { 0, PEEP_ITEM_BALLOON, PEEP_SPRITE_TYPE_BALLOON },
        { 0, PEEP_ITEM_HAT, PEEP_SPRITE_TYPE_HAT },
        { 1, PEEP_ITEM_SUNGLASSES, PEEP_SPRITE_TYPE_SUNGLASSES },
        { 0xFF, 0xFFFFFFFF, 0xFF}
};
// clang-format on

/**
 *
 *  rct2: 0x0069B8CC
 */
void peep_update_sprite_type(rct_peep * peep)
{
    if (peep->sprite_type == PEEP_SPRITE_TYPE_BALLOON && (scenario_rand() & 0xFFFF) <= 327)
    {
        bool isBalloonPopped = false;
        if (peep->x != LOCATION_NULL)
        {
            if ((scenario_rand() & 0xFFFF) <= 13107)
            {
                isBalloonPopped = true;
                audio_play_sound_at_location(SOUND_BALLOON_POP, peep->x, peep->y, peep->z);
            }
            create_balloon(peep->x, peep->y, peep->z + 9, peep->balloon_colour, isBalloonPopped);
        }
        peep->item_standard_flags &= ~PEEP_ITEM_BALLOON;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }

    if (climate_is_raining() && (peep->item_standard_flags & PEEP_ITEM_UMBRELLA) && peep->x != LOCATION_NULL)
    {
        sint32 x = peep->x & 0xFFE0;
        sint32 y = peep->y & 0xFFE0;

        if (x < 0x1FFF && y < 0x1FFF)
        {
            rct_tile_element * tile_element = map_get_first_element_at(x / 32, y / 32);
            while (true)
            {
                if ((peep->z / 8) < tile_element->base_height)
                    break;

                if (tile_element_is_last_for_tile(tile_element))
                {
                    set_sprite_type(peep, PEEP_SPRITE_TYPE_UMBRELLA);
                    return;
                }
                tile_element++;
            }
        }
    }

    for (item_pref_t * item_pref = item_order_preference; item_pref->type != 0xFF; item_pref++)
    {
        if (item_pref->type == 0)
        {
            if (peep->item_standard_flags & item_pref->item)
            {
                set_sprite_type(peep, item_pref->sprite_type);
                return;
            }
        }
        else
        {
            if (peep->item_extra_flags & item_pref->item)
            {
                set_sprite_type(peep, item_pref->sprite_type);
                return;
            }
        }
    }

    if (peep->state == PEEP_STATE_WATCHING && peep->standing_flags & (1 << 1))
    {
        set_sprite_type(peep, PEEP_SPRITE_TYPE_WATCHING);
        return;
    }

    if (peep->nausea > 170)
    {
        set_sprite_type(peep, PEEP_SPRITE_TYPE_VERY_NAUSEOUS);
        return;
    }

    if (peep->nausea > 140)
    {
        set_sprite_type(peep, PEEP_SPRITE_TYPE_NAUSEOUS);
        return;
    }

    if (peep->energy <= 64 && peep->happiness < 128)
    {
        set_sprite_type(peep, PEEP_SPRITE_TYPE_HEAD_DOWN);
        return;
    }

    if (peep->energy <= 80 && peep->happiness < 128)
    {
        set_sprite_type(peep, PEEP_SPRITE_TYPE_ARMS_CROSSED);
        return;
    }

    if (peep->toilet > 220)
    {
        set_sprite_type(peep, PEEP_SPRITE_TYPE_REQUIRE_BATHROOM);
        return;
    }

    set_sprite_type(peep, PEEP_SPRITE_TYPE_NORMAL);
}

/**
 * Call after changing a peeps state to insure that all relevant windows update.
 * Note also increase ride count if on/entering a ride.
 *  rct2: 0x0069A42F
 */
void peep_window_state_update(rct_peep * peep)
{
    rct_window * w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != nullptr)
        window_event_invalidate_call(w);

    if (peep->type == PEEP_TYPE_GUEST)
    {
        if (peep->state == PEEP_STATE_ON_RIDE || peep->state == PEEP_STATE_ENTERING_RIDE)
        {
            Ride * ride = get_ride(peep->current_ride);
            ride->num_riders++;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
        }

        window_invalidate_by_number(WC_PEEP, peep->sprite_index);
        window_invalidate_by_class(WC_GUEST_LIST);
    }
    else
    {
        window_invalidate_by_number(WC_PEEP, peep->sprite_index);
        window_invalidate_by_class(WC_STAFF_LIST);
    }
}

void peep_pickup(rct_peep * peep)
{
    peep->RemoveFromRide();
    peep->Invalidate();

    sprite_move(LOCATION_NULL, peep->y, peep->z, (rct_sprite *)peep);
    peep->SetState(PEEP_STATE_PICKED);
    peep->sub_state = 0;
}

void peep_pickup_abort(rct_peep * peep, sint32 old_x)
{
    if (!peep)
        return;

    if (peep->state != PEEP_STATE_PICKED)
        return;

    sprite_move(old_x, peep->y, peep->z + 8, (rct_sprite *)peep);
    peep->Invalidate();

    if (peep->x != (sint16)LOCATION_NULL)
    {
        peep->SetState(PEEP_STATE_FALLING);
        peep->action                     = 0xFF;
        peep->special_sprite             = 0;
        peep->action_sprite_image_offset = 0;
        peep->action_sprite_type         = 0;
        peep->path_check_optimisation                     = 0;
    }

    gPickupPeepImage = UINT32_MAX;
}

bool peep_pickup_place(rct_peep * peep, sint32 x, sint32 y, sint32 z, bool apply)
{
    if (!peep)
        return false;

    rct_tile_element * tileElement = map_get_path_element_at(x / 32, y / 32, z);

    if (!tileElement)
    {
        tileElement = map_get_surface_element_at({x, y});
    }

    if (!tileElement)
        return false;

    sint32 dest_x = x & 0xFFE0;
    sint32 dest_y = y & 0xFFE0;

    // Set the coordinate of destination to be exactly
    // in the middle of a tile.
    dest_x += 16;
    dest_y += 16;
    // Set the tile coordinate to top left of tile
    sint32 tile_y = dest_y & 0xFFE0;
    sint32 tile_x = dest_x & 0xFFE0;

    sint32 dest_z = tileElement->base_height * 8 + 16;

    if (!map_is_location_owned(tile_x, tile_y, dest_z))
    {
        gGameCommandErrorTitle = STR_ERR_CANT_PLACE_PERSON_HERE;
        return false;
    }

    if (!map_can_construct_at(tile_x, tile_y, dest_z / 8, (dest_z / 8) + 1, 15))
    {
        if (gGameCommandErrorText != STR_RAISE_OR_LOWER_LAND_FIRST)
        {
            if (gGameCommandErrorText != STR_FOOTPATH_IN_THE_WAY)
            {
                gGameCommandErrorTitle = STR_ERR_CANT_PLACE_PERSON_HERE;
                return false;
            }
        }
    }

    if (apply)
    {
        sprite_move(dest_x, dest_y, dest_z, (rct_sprite *)peep);
        peep->Invalidate();
        peep->SetState(PEEP_STATE_FALLING);
        peep->action                     = 0xFF;
        peep->special_sprite             = 0;
        peep->action_sprite_image_offset = 0;
        peep->action_sprite_type         = 0;
        peep->path_check_optimisation                     = 0;
        sprite_position_tween_reset();

        if (peep->type == PEEP_TYPE_GUEST)
        {
            peep->action_sprite_type = 0xFF;
            peep->happiness_target   = Math::Max(peep->happiness_target - 10, 0);
            peep->UpdateCurrentActionSpriteType();
        }

        network_set_pickup_peep(game_command_playerid, nullptr);
    }

    return true;
}

bool peep_pickup_command(uint32 peepnum, sint32 x, sint32 y, sint32 z, sint32 action, bool apply)
{
    if (peepnum >= MAX_SPRITES)
    {
        log_error("Failed to pick up peep for sprite %d", peepnum);
        return false;
    }
    rct_peep * peep = GET_PEEP(peepnum);
    if (!peep || peep->sprite_identifier != SPRITE_IDENTIFIER_PEEP)
    {
        return false;
    }
    switch (action)
    {
    case 0: // pickup
    {
        if (!peep_can_be_picked_up(peep))
        {
            return false;
        }
        rct_peep * existing = network_get_pickup_peep(game_command_playerid);
        if (existing)
        {
            // already picking up a peep
            bool result = peep_pickup_command(existing->sprite_index, network_get_pickup_peep_old_x(game_command_playerid), 0,
                                              0, 1, apply);
            if (existing == peep)
            {
                return result;
            }
            if (game_command_playerid == network_get_current_player_id())
            {
                // prevent tool_cancel()
                input_set_flag(INPUT_FLAG_TOOL_ACTIVE, false);
            }
        }
        if (apply)
        {
            network_set_pickup_peep(game_command_playerid, peep);
            network_set_pickup_peep_old_x(game_command_playerid, peep->x);
            peep_pickup(peep);
        }
    }
    break;
    case 1: // cancel
        if (apply)
        {
            peep_pickup_abort(network_get_pickup_peep(game_command_playerid), x);
            network_set_pickup_peep(game_command_playerid, nullptr);
        }
        break;
    case 2: // place
        if (network_get_pickup_peep(game_command_playerid) != peep)
        {
            return false;
        }
        if (!peep_pickup_place(peep, x, y, z, apply))
        {
            return false;
        }
        break;
    }
    return true;
}

void game_command_pickup_guest(sint32 * eax, sint32 * ebx, sint32 * ecx, sint32 * edx, sint32 * esi, sint32 * edi, sint32 * ebp)
{
    sint32 peepnum = *eax;
    sint32 x       = *edi;
    sint32 y       = *ebp;
    sint32 z       = *edx;
    sint32 action  = *ecx;
    if (peep_pickup_command(peepnum, x, y, z, action, *ebx & GAME_COMMAND_FLAG_APPLY))
    {
        *ebx = 0;
    }
    else
    {
        *ebx = MONEY32_UNDEFINED;
    }
}

/**
 *
 *  rct2: 0x0069A535
 */
void peep_sprite_remove(rct_peep * peep)
{
    peep->RemoveFromRide();
    peep->Invalidate();

    window_close_by_number(WC_PEEP, peep->sprite_index);

    window_close_by_number(WC_FIRE_PROMPT, peep->sprite_identifier);

    if (peep->type == PEEP_TYPE_GUEST)
    {
        window_invalidate_by_class(WC_GUEST_LIST);

        news_item_disable_news(NEWS_ITEM_PEEP_ON_RIDE, peep->sprite_index);
    }
    else
    {
        window_invalidate_by_class(WC_STAFF_LIST);

        gStaffModes[peep->staff_id] = 0;
        peep->type                  = 0xFF;
        staff_update_greyed_patrol_areas();
        peep->type = PEEP_TYPE_STAFF;

        news_item_disable_news(NEWS_ITEM_PEEP, peep->sprite_index);
    }
    sprite_remove((rct_sprite *)peep);
}

/**
 * New function removes peep from park existence. Works with staff.
 */
void rct_peep::Remove()
{
    if (type == PEEP_TYPE_GUEST)
    {
        if (outside_of_park == 0)
        {
            decrement_guests_in_park();
            auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
            context_broadcast_intent(&intent);
        }
        if (state == PEEP_STATE_ENTERING_PARK)
        {
            decrement_guests_heading_for_park();
        }
    }
    peep_sprite_remove(this);
}

/**
 * Falling and its subset drowning
 *  rct2: 0x690028
 */
void rct_peep::UpdateFalling()
{
    if (action == PEEP_ACTION_DROWNING)
    {
        // Check to see if we are ready to drown.
        sint16 actionX, actionY, xy_distance;

        UpdateAction(&actionX, &actionY, &xy_distance);
        if (action == PEEP_ACTION_DROWNING)
            return;

        if (gConfigNotifications.guest_died)
        {
            set_format_arg(0, rct_string_id, name_string_idx);
            set_format_arg(2, uint32, id);
            news_item_add_to_queue(NEWS_ITEM_BLANK, STR_NEWS_ITEM_GUEST_DROWNED, actionX | (actionY << 16));
        }

        gParkRatingCasualtyPenalty = Math::Min(gParkRatingCasualtyPenalty + 25, 1000);
        Remove();
        return;
    }

    // If not drowning then falling. Note: peeps 'fall' after leaving a ride/enter the park.
    rct_tile_element * tile_element  = map_get_first_element_at(x / 32, y / 32);
    rct_tile_element * saved_map    = nullptr;
    sint32            saved_height = 0;

    if (tile_element != nullptr)
    {
        do
        {
            // If a path check if we are on it
            if (tile_element_get_type(tile_element) == TILE_ELEMENT_TYPE_PATH)
            {
                sint32 height = map_height_from_slope(x, y, tile_element->properties.surface.slope) +
                                tile_element->base_height * 8;

                if (height < z - 1 || height > z + 4)
                    continue;

                saved_height = height;
                saved_map    = tile_element;
                break;
            } // If a surface get the height and see if we are on it
            else if (tile_element_get_type(tile_element) == TILE_ELEMENT_TYPE_SURFACE)
            {
                // If the surface is water check to see if we could be drowning
                if (map_get_water_height(tile_element) > 0)
                {
                    sint32 height = map_get_water_height(tile_element) * 16;

                    if (height - 4 >= z && height < z + 20)
                    {
                        // Looks like we are drowning!
                        Invalidate();
                        MoveTo(x, y, height);
                        // Drop balloon if held
                        peep_release_balloon(this, height);

                        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_DROWNING, PEEP_THOUGHT_ITEM_NONE);

                        action                     = PEEP_ACTION_DROWNING;
                        action_frame               = 0;
                        action_sprite_image_offset = 0;

                        UpdateCurrentActionSpriteType();
                        Invalidate();
                        peep_window_state_update(this);
                        return;
                    }
                }
                sint32 map_height = tile_element_height(0xFFFF & x, 0xFFFF & y) & 0xFFFF;
                if (map_height < z || map_height - 4 > z)
                    continue;
                saved_height = map_height;
                saved_map    = tile_element;
            } // If not a path or surface go see next element
            else
                continue;
        } while (!tile_element_is_last_for_tile(tile_element++));
    }

    // This will be null if peep is falling
    if (saved_map == nullptr)
    {
        Invalidate();
        if (z <= 1)
        {
            // Remove peep if it has gone to the void
            Remove();
            return;
        }
        MoveTo(x, y, z - 2);
        Invalidate();
        return;
    }

    Invalidate();
    MoveTo(x, y, saved_height);
    Invalidate();

    next_x = x & 0xFFE0;
    next_y = y & 0xFFE0;
    next_z = saved_map->base_height;

    sint32 edx = saved_map->properties.surface.slope & TILE_ELEMENT_SLOPE_W_CORNER_DN;
    if (tile_element_get_type(saved_map) != TILE_ELEMENT_TYPE_PATH)
    {
        edx = 8;
    }
    next_var_29 = edx;
    SetState(PEEP_STATE_1);
}

/**
 *
 *  rct2: 0x6902A2
 */
void rct_peep::Update1()
{
    if (!CheckForPath())
        return;

    if (type == PEEP_TYPE_GUEST)
    {
        SetState(PEEP_STATE_WALKING);
    }
    else
    {
        SetState(PEEP_STATE_PATROLLING);
    }

    destination_x         = x;
    destination_y         = y;
    destination_tolerance = 10;
    direction             = sprite_direction >> 3;
}

void rct_peep::SetState(uint8 new_state)
{
    peep_decrement_num_riders(this);
    state = new_state;
    peep_window_state_update(this);
}

/**
 *
 *  rct2: 0x690009
 */
void rct_peep::UpdatePicked()
{
    if (gCurrentTicks & 0x1F)
        return;
    sub_state++;
    if (sub_state == 13)
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_HELP, PEEP_THOUGHT_ITEM_NONE);
    }
}

/* From peep_update */
static void peep_update_thoughts(rct_peep * peep)
{
    // Thoughts must always have a gap of at least
    // 220 ticks in age between them. In order to
    // allow this when a thought is new it enters
    // a holding zone. Before it becomes fresh.
    sint32 add_fresh     = 1;
    sint32 fresh_thought = -1;
    for (sint32 i = 0; i < PEEP_MAX_THOUGHTS; i++)
    {
        if (peep->thoughts[i].type == PEEP_THOUGHT_TYPE_NONE)
            break;

        if (peep->thoughts[i].freshness == 1)
        {
            add_fresh = 0;
            // If thought is fresh we wait 220 ticks
            // before allowing a new thought to become fresh.
            if (++peep->thoughts[i].fresh_timeout >= 220)
            {
                peep->thoughts[i].fresh_timeout = 0;
                // Thought is no longer fresh
                peep->thoughts[i].freshness++;
                add_fresh = 1;
            }
        }
        else if (peep->thoughts[i].freshness > 1)
        {
            if (++peep->thoughts[i].fresh_timeout == 0)
            {
                // When thought is older than ~6900 ticks remove it
                if (++peep->thoughts[i].freshness >= 28)
                {
                    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;

                    // Clear top thought, push others up
                    if (i < PEEP_MAX_THOUGHTS - 2)
                    {
                        memmove(&peep->thoughts[i], &peep->thoughts[i + 1],
                                sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - i - 1));
                    }
                    peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;
                }
            }
        }
        else
        {
            fresh_thought = i;
        }
    }
    // If there are no fresh thoughts
    // a previously new thought can become
    // fresh.
    if (add_fresh && fresh_thought != -1)
    {
        peep->thoughts[fresh_thought].freshness = 1;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
    }
}

/**
 *
 *  rct2: 0x0068FC1E
 */
void rct_peep::Update()
{
    if (type == PEEP_TYPE_GUEST)
    {
        if (previous_ride != 255)
            if (++previous_ride_time_out >= 720)
                previous_ride = 255;

        peep_update_thoughts(this);
    }

    // Walking speed logic
    uint32 stepsToTake = energy;
    if (stepsToTake < 95 && state == PEEP_STATE_QUEUING)
        stepsToTake = 95;
    if ((peep_flags & PEEP_FLAGS_SLOW_WALK) && state != PEEP_STATE_QUEUING)
        stepsToTake /= 2;
    if (action == 255 && (next_var_29 & 4))
    {
        stepsToTake /= 2;
        if (state == PEEP_STATE_QUEUING)
            stepsToTake += stepsToTake / 2;
    }

    uint32 carryCheck = step_progress + stepsToTake;
    step_progress      = carryCheck;
    if (carryCheck <= 255)
    {
        peep_easter_egg_peep_interactions(this);
    }
    else
    {
        // loc_68FD2F
        switch (state)
        {
        case PEEP_STATE_FALLING:
            UpdateFalling();
            break;
        case PEEP_STATE_1:
            Update1();
            break;
        case PEEP_STATE_QUEUING_FRONT:
            UpdateRide();
            break;
        case PEEP_STATE_ON_RIDE:
            // No action
            break;
        case PEEP_STATE_LEAVING_RIDE:
            UpdateRide();
            break;
        case PEEP_STATE_WALKING:
            UpdateWalking();
            break;
        case PEEP_STATE_QUEUING:
            UpdateQueuing();
            break;
        case PEEP_STATE_ENTERING_RIDE:
            UpdateRide();
            break;
        case PEEP_STATE_SITTING:
            UpdateSitting();
            break;
        case PEEP_STATE_PICKED:
            UpdatePicked();
            break;
        case PEEP_STATE_PATROLLING:
            UpdatePatrolling();
            break;
        case PEEP_STATE_MOWING:
            UpdateMowing();
            break;
        case PEEP_STATE_SWEEPING:
            UpdateSweeping();
            break;
        case PEEP_STATE_ENTERING_PARK:
            UpdateEnteringPark();
            break;
        case PEEP_STATE_LEAVING_PARK:
            UpdateLeavingPark();
            break;
        case PEEP_STATE_ANSWERING:
            UpdateAnswering();
            break;
        case PEEP_STATE_FIXING:
            UpdateFixing(stepsToTake);
            break;
        case PEEP_STATE_BUYING:
            UpdateBuying();
            break;
        case PEEP_STATE_WATCHING:
            UpdateWatching();
            break;
        case PEEP_STATE_EMPTYING_BIN:
            UpdateEmptyingBin();
            break;
        case PEEP_STATE_USING_BIN:
            UpdateUsingBin();
            break;
        case PEEP_STATE_WATERING:
            UpdateWatering();
            break;
        case PEEP_STATE_HEADING_TO_INSPECTION:
            UpdateHeadingToInspect();
            break;
        case PEEP_STATE_INSPECTING:
            UpdateFixing(stepsToTake);
            break;
            // There shouldn't be any more
        default:
            assert(0);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x0069BF41
 */
void peep_problem_warnings_update()
{
    rct_peep * peep;
    Ride *     ride;
    uint16     spriteIndex;
    uint16     guests_in_park = gNumGuestsInPark;
    sint32     hunger_counter = 0, lost_counter = 0, noexit_counter = 0, thirst_counter = 0, litter_counter = 0,
           disgust_counter = 0, bathroom_counter = 0, vandalism_counter = 0;
    uint8 * warning_throttle = gPeepWarningThrottle;

    gRideCount = ride_get_count(); // refactor this to somewhere else

    FOR_ALL_GUESTS(spriteIndex, peep)
    {
        if (peep->outside_of_park != 0 || peep->thoughts[0].freshness > 5)
            continue;

        switch (peep->thoughts[0].type)
        {
        case PEEP_THOUGHT_TYPE_LOST: // 0x10
            lost_counter++;
            break;

        case PEEP_THOUGHT_TYPE_HUNGRY: // 0x14
            if (peep->guest_heading_to_ride_id == 0xFF)
            {
                hunger_counter++;
                break;
            }
            ride = get_ride(peep->guest_heading_to_ride_id);
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
                hunger_counter++;
            break;

        case PEEP_THOUGHT_TYPE_THIRSTY:
            if (peep->guest_heading_to_ride_id == 0xFF)
            {
                thirst_counter++;
                break;
            }
            ride = get_ride(peep->guest_heading_to_ride_id);
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_SELLS_DRINKS))
                thirst_counter++;
            break;

        case PEEP_THOUGHT_TYPE_BATHROOM:
            if (peep->guest_heading_to_ride_id == 0xFF)
            {
                bathroom_counter++;
                break;
            }
            ride = get_ride(peep->guest_heading_to_ride_id);
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_BATHROOM))
                bathroom_counter++;
            break;

        case PEEP_THOUGHT_TYPE_BAD_LITTER: // 0x1a
            litter_counter++;
            break;
        case PEEP_THOUGHT_TYPE_CANT_FIND_EXIT: // 0x1b
            noexit_counter++;
            break;
        case PEEP_THOUGHT_TYPE_PATH_DISGUSTING: // 0x1f
            disgust_counter++;
            break;
        case PEEP_THOUGHT_TYPE_VANDALISM: // 0x21
            vandalism_counter++;
            break;
        default:
            break;
        }
    }
    // could maybe be packed into a loop, would lose a lot of clarity though
    if (warning_throttle[0])
        --warning_throttle[0];
    else if (hunger_counter >= PEEP_HUNGER_WARNING_THRESHOLD && hunger_counter >= guests_in_park / 16)
    {
        warning_throttle[0] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_ARE_HUNGRY, 20);
        }
    }

    if (warning_throttle[1])
        --warning_throttle[1];
    else if (thirst_counter >= PEEP_THIRST_WARNING_THRESHOLD && thirst_counter >= guests_in_park / 16)
    {
        warning_throttle[1] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_ARE_THIRSTY, 21);
        }
    }

    if (warning_throttle[2])
        --warning_throttle[2];
    else if (bathroom_counter >= PEEP_BATHROOM_WARNING_THRESHOLD && bathroom_counter >= guests_in_park / 16)
    {
        warning_throttle[2] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_CANT_FIND_BATHROOM, 22);
        }
    }

    if (warning_throttle[3])
        --warning_throttle[3];
    else if (litter_counter >= PEEP_LITTER_WARNING_THRESHOLD && litter_counter >= guests_in_park / 32)
    {
        warning_throttle[3] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISLIKE_LITTER, 26);
        }
    }

    if (warning_throttle[4])
        --warning_throttle[4];
    else if (disgust_counter >= PEEP_DISGUST_WARNING_THRESHOLD && disgust_counter >= guests_in_park / 32)
    {
        warning_throttle[4] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISGUSTED_BY_PATHS, 31);
        }
    }

    if (warning_throttle[5])
        --warning_throttle[5];
    else if (vandalism_counter >= PEEP_VANDALISM_WARNING_THRESHOLD && vandalism_counter >= guests_in_park / 32)
    {
        warning_throttle[5] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISLIKE_VANDALISM, 33);
        }
    }

    if (warning_throttle[6])
        --warning_throttle[6];
    else if (noexit_counter >= PEEP_NOEXIT_WARNING_THRESHOLD)
    {
        warning_throttle[6] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_GETTING_LOST_OR_STUCK, 27);
        }
    }
    else if (lost_counter >= PEEP_LOST_WARNING_THRESHOLD)
    {
        warning_throttle[6] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_GETTING_LOST_OR_STUCK, 16);
        }
    }
}

void peep_stop_crowd_noise()
{
    if (_crowdSoundChannel != nullptr)
    {
        Mixer_Stop_Channel(_crowdSoundChannel);
        _crowdSoundChannel = nullptr;
    }
}

/**
 *
 *  rct2: 0x006BD18A
 */
void peep_update_crowd_noise()
{
    rct_viewport * viewport;
    uint16         spriteIndex;
    rct_peep *     peep;
    sint32         visiblePeeps;

    if (gGameSoundsOff)
        return;

    if (!gConfigSound.sound_enabled)
        return;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return;

    viewport = g_music_tracking_viewport;
    if (viewport == nullptr)
        return;

    // Count the number of peeps visible
    visiblePeeps = 0;

    FOR_ALL_GUESTS(spriteIndex, peep)
    {
        if (peep->sprite_left == LOCATION_NULL)
            continue;
        if (viewport->view_x > peep->sprite_right)
            continue;
        if (viewport->view_x + viewport->view_width < peep->sprite_left)
            continue;
        if (viewport->view_y > peep->sprite_bottom)
            continue;
        if (viewport->view_y + viewport->view_height < peep->sprite_top)
            continue;

        visiblePeeps += peep->state == PEEP_STATE_QUEUING ? 1 : 2;
    }

    // This function doesn't account for the fact that the screen might be so big that 100 peeps could potentially be very
    // spread out and therefore not produce any crowd noise. Perhaps a more sophisticated solution would check how many peeps
    // were in close proximity to each other.

    // Allows queuing peeps to make half as much noise, and at least 6 peeps must be visible for any crowd noise
    visiblePeeps = (visiblePeeps / 2) - 6;
    if (visiblePeeps < 0)
    {
        // Mute crowd noise
        if (_crowdSoundChannel != nullptr)
        {
            Mixer_Stop_Channel(_crowdSoundChannel);
            _crowdSoundChannel = nullptr;
        }
    }
    else
    {
        sint32 volume;

        // Formula to scale peeps to dB where peeps [0, 120] scales approximately logarithmically to [-3314, -150] dB/100
        // 207360000 maybe related to DSBVOLUME_MIN which is -10,000 (dB/100)
        volume = 120 - Math::Min(visiblePeeps, 120);
        volume = volume * volume * volume * volume;
        volume = (((207360000 - volume) >> viewport->zoom) - 207360000) / 65536 - 150;

        // Load and play crowd noise if needed and set volume
        if (_crowdSoundChannel == nullptr)
        {
            _crowdSoundChannel = Mixer_Play_Music(PATH_ID_CSS2, MIXER_LOOP_INFINITE, false);
            if (_crowdSoundChannel != nullptr)
            {
                Mixer_Channel_SetGroup(_crowdSoundChannel, MIXER_GROUP_SOUND);
            }
        }
        if (_crowdSoundChannel != nullptr)
        {
            Mixer_Channel_Volume(_crowdSoundChannel, DStoMixerVolume(volume));
        }
    }
}

/**
 *
 *  rct2: 0x0069BE9B
 */
void peep_applause()
{
    uint16     spriteIndex;
    rct_peep * peep;

    FOR_ALL_GUESTS(spriteIndex, peep)
    {
        if (peep->outside_of_park != 0)
            continue;

        // Release balloon
        peep_release_balloon(peep, peep->z + 9);

        // Clap
        if ((peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_QUEUING) && peep->action >= 254)
        {
            peep->action                     = PEEP_ACTION_CLAP;
            peep->action_frame               = 0;
            peep->action_sprite_image_offset = 0;
            peep->UpdateCurrentActionSpriteType();
            peep->Invalidate();
        }
    }

    // Play applause noise
    audio_play_sound(SOUND_APPLAUSE, 0, context_get_width() / 2);
}

/**
 *
 *  rct2: 0x0069C35E
 */
void peep_update_days_in_queue()
{
    uint16     sprite_index;
    rct_peep * peep;

    FOR_ALL_GUESTS(sprite_index, peep)
    {
        if (peep->outside_of_park == 0 && peep->state == PEEP_STATE_QUEUING)
        {
            if (peep->days_in_queue < 255)
            {
                peep->days_in_queue += 1;
            }
        }
    }
}

// clang-format off
/** rct2: 0x009823A0 */
static constexpr const enum PEEP_NAUSEA_TOLERANCE nausea_tolerance_distribution[] = {
    PEEP_NAUSEA_TOLERANCE_NONE,
    PEEP_NAUSEA_TOLERANCE_LOW, PEEP_NAUSEA_TOLERANCE_LOW,
    PEEP_NAUSEA_TOLERANCE_AVERAGE, PEEP_NAUSEA_TOLERANCE_AVERAGE, PEEP_NAUSEA_TOLERANCE_AVERAGE,
    PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH,
};

/** rct2: 0x009823BC */
static constexpr const uint8 trouser_colours[] = {
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_DARK_PURPLE,
    COLOUR_LIGHT_PURPLE,
    COLOUR_DARK_BLUE,
    COLOUR_SATURATED_GREEN,
    COLOUR_SATURATED_RED,
    COLOUR_DARK_ORANGE,
    COLOUR_BORDEAUX_RED,
};

/** rct2: 0x009823D5 */
static constexpr const uint8 tshirt_colours[] = {
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_DARK_PURPLE,
    COLOUR_LIGHT_PURPLE,
    COLOUR_DARK_BLUE,
    COLOUR_SATURATED_GREEN,
    COLOUR_SATURATED_RED,
    COLOUR_DARK_ORANGE,
    COLOUR_BORDEAUX_RED,
    COLOUR_WHITE,
    COLOUR_BRIGHT_PURPLE,
    COLOUR_LIGHT_BLUE,
    COLOUR_TEAL,
    COLOUR_DARK_GREEN,
    COLOUR_MOSS_GREEN,
    COLOUR_BRIGHT_GREEN,
    COLOUR_OLIVE_GREEN,
    COLOUR_DARK_OLIVE_GREEN,
    COLOUR_YELLOW,
    COLOUR_LIGHT_ORANGE,
    COLOUR_BRIGHT_RED,
    COLOUR_DARK_PINK,
    COLOUR_BRIGHT_PINK,
};
// clang-format on

/**
 *
 *  rct2: 0x0069A05D
 */
rct_peep * peep_generate(sint32 x, sint32 y, sint32 z)
{
    if (gSpriteListCount[SPRITE_LIST_NULL] < 400)
        return nullptr;

    rct_peep * peep = (rct_peep *)create_sprite(1);

    move_sprite_to_list((rct_sprite *)peep, SPRITE_LIST_PEEP * 2);

    peep->sprite_identifier          = SPRITE_IDENTIFIER_PEEP;
    peep->sprite_type                = PEEP_SPRITE_TYPE_NORMAL;
    peep->outside_of_park            = 1;
    peep->state                      = PEEP_STATE_FALLING;
    peep->action                     = PEEP_ACTION_NONE_2;
    peep->special_sprite             = 0;
    peep->action_sprite_image_offset = 0;
    peep->no_action_frame_num         = 0;
    peep->action_sprite_type         = 0;
    peep->peep_flags                 = 0;
    peep->favourite_ride             = RIDE_ID_NULL;
    peep->favourite_ride_rating      = 0;

    const rct_sprite_bounds * spriteBounds = g_peep_animation_entries[peep->sprite_type].sprite_bounds;
    peep->sprite_width                     = spriteBounds[peep->action_sprite_type].sprite_width;
    peep->sprite_height_negative           = spriteBounds[peep->action_sprite_type].sprite_height_negative;
    peep->sprite_height_positive           = spriteBounds[peep->action_sprite_type].sprite_height_positive;

    peep->sprite_direction = 0;

    sprite_move(x, y, z, (rct_sprite *)peep);
    peep->Invalidate();

    peep->mass                  = (scenario_rand() & 0x1F) + 45;
    peep->path_check_optimisation                  = 0;
    peep->interaction_ride_index  = 0xFF;
    peep->type                    = PEEP_TYPE_GUEST;
    peep->previous_ride           = 0xFF;
    peep->thoughts->type          = PEEP_THOUGHT_TYPE_NONE;
    peep->window_invalidate_flags = 0;

    uint8 al = (scenario_rand() & 0x7) + 3;
    uint8 ah = Math::Min(al, static_cast<uint8>(7)) - 3;

    if (al >= 7)
        al = 15;

    if (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
    {
        ah = 0;
        al = 4;
    }

    if (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
    {
        ah = 9;
        al = 15;
    }

    peep->intensity = (al << 4) | ah;

    uint8 nausea_tolerance = scenario_rand() & 0x7;
    if (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
    {
        nausea_tolerance += 4;
    }

    peep->nausea_tolerance = nausea_tolerance_distribution[nausea_tolerance];

    /* Scenario editor limits initial guest happiness to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->happiness = gGuestInitialHappiness;
    /* Assume a default initial happiness of 0 is wrong and set
     * to 128 (50%) instead. */
    if (gGuestInitialHappiness == 0)
        peep->happiness = 128;
    /* Initial value will vary by -15..16 */
    sint8 happiness_delta = (scenario_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->happiness        = Math::Clamp(0, peep->happiness + happiness_delta, PEEP_MAX_HAPPINESS);
    peep->happiness_target = peep->happiness;
    peep->nausea           = 0;
    peep->nausea_target    = 0;

    /* Scenario editor limits initial guest hunger to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->hunger = gGuestInitialHunger;
    /* Initial value will vary by -15..16 */
    sint8 hunger_delta = (scenario_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->hunger = Math::Clamp(0, peep->hunger + hunger_delta, 255);

    /* Scenario editor limits initial guest thirst to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->thirst = gGuestInitialThirst;
    /* Initial value will vary by -15..16 */
    sint8 thirst_delta = (scenario_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->thirst = Math::Clamp(0, peep->thirst + thirst_delta, 0xFF);

    peep->toilet = 0;
    peep->time_to_consume   = 0;
    memset(&peep->rides_been_on, 0, 32);

    peep->no_of_rides = 0;
    memset(&peep->ride_types_been_on, 0, 16);
    peep->id              = gNextGuestNumber++;
    peep->name_string_idx = STR_GUEST_X;

    money32 cash = (scenario_rand() & 0x3) * 100 - 100 + gGuestInitialCash;
    if (cash < 0)
        cash = 0;

    if (gGuestInitialCash == 0)
    {
        cash = 500;
    }

    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        cash = 0;
    }

    if (gGuestInitialCash == (money16)(uint16)0xFFFF)
    {
        cash = 0;
    }

    peep->cash_in_pocket           = cash;
    peep->cash_spent               = 0;
    peep->time_in_park             = -1;
    peep->pathfind_goal.x          = 0xFF;
    peep->pathfind_goal.y          = 0xFF;
    peep->pathfind_goal.z          = 0xFF;
    peep->pathfind_goal.direction  = 0xFF;
    peep->item_standard_flags      = 0;
    peep->item_extra_flags         = 0;
    peep->guest_heading_to_ride_id = 0xFF;
    peep->litter_count             = 0;
    peep->disgusting_count         = 0;
    peep->vandalism_seen                   = 0;
    peep->paid_to_enter            = 0;
    peep->paid_on_rides            = 0;
    peep->paid_on_food             = 0;
    peep->paid_on_drink            = 0;
    peep->paid_on_souvenirs        = 0;
    peep->no_of_food               = 0;
    peep->no_of_drinks             = 0;
    peep->no_of_souvenirs          = 0;
    peep->surroundings_thought_timeout = 0;
    peep->angriness                = 0;
    peep->time_lost                   = 0;

    uint8 tshirt_colour = static_cast<uint8>(scenario_rand() % Util::CountOf(tshirt_colours));
    peep->tshirt_colour = tshirt_colours[tshirt_colour];

    uint8 trousers_colour = static_cast<uint8>(scenario_rand() % Util::CountOf(trouser_colours));
    peep->trousers_colour = trouser_colours[trousers_colour];

    /* It looks like 65 is about 50% energy level, so this initialises
     * a peep with approx 50%-100% energy (0x3F = 63, 63 + 65 = 128). */
    uint8 energy        = (scenario_rand() & 0x3F) + 65;
    peep->energy        = energy;
    peep->energy_target = energy;

    if (gParkFlags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES)
    {
        peep_give_real_name(peep);
    }
    peep_update_name_sort(peep);

    increment_guests_heading_for_park();

    return peep;
}

/**
 * rct2: 0x00698B0D
 * peep.sprite_index (eax)
 * thought.type (ebx)
 * argument_1 (ecx & ebx)
 * argument_2 (edx)
 */
void get_arguments_from_action(rct_peep * peep, uint32 * argument_1, uint32 * argument_2)
{
    Ride * ride;

    switch (peep->state)
    {
    case PEEP_STATE_FALLING:
        *argument_1 = peep->action == PEEP_ACTION_DROWNING ? STR_DROWNING : STR_WALKING;
        *argument_2 = 0;
        break;
    case PEEP_STATE_1:
        *argument_1 = STR_WALKING;
        *argument_2 = 0;
        break;
    case PEEP_STATE_ON_RIDE:
    case PEEP_STATE_LEAVING_RIDE:
    case PEEP_STATE_ENTERING_RIDE:
        *argument_1 = STR_ON_RIDE;
        ride        = get_ride(peep->current_ride);
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE))
            *argument_1 = STR_IN_RIDE;
        *argument_1 |= ((uint32)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_BUYING:
        ride        = get_ride(peep->current_ride);
        *argument_1 = STR_AT_RIDE | ((uint32)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_WALKING:
    case PEEP_STATE_USING_BIN:
        if (peep->guest_heading_to_ride_id != 0xFF)
        {
            ride        = get_ride(peep->guest_heading_to_ride_id);
            *argument_1 = STR_HEADING_FOR | ((uint32)ride->name << 16);
            *argument_2 = ride->name_arguments;
        }
        else
        {
            *argument_1 = (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) ? STR_LEAVING_PARK : STR_WALKING;
            *argument_2 = 0;
        }
        break;
    case PEEP_STATE_QUEUING_FRONT:
    case PEEP_STATE_QUEUING:
        ride        = get_ride(peep->current_ride);
        *argument_1 = STR_QUEUING_FOR | ((uint32)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_SITTING:
        *argument_1 = STR_SITTING;
        *argument_2 = 0;
        break;
    case PEEP_STATE_WATCHING:
        if (peep->current_ride != 0xFF)
        {
            ride        = get_ride(peep->current_ride);
            *argument_1 = STR_WATCHING_RIDE | ((uint32)ride->name << 16);
            *argument_2 = ride->name_arguments;
            if (peep->current_seat & 0x1)
                *argument_1 = STR_WATCHING_CONSTRUCTION_OF | ((uint32)ride->name << 16);
            else
                *argument_1 = STR_WATCHING_RIDE | ((uint32)ride->name << 16);
        }
        else
        {
            *argument_1 = (peep->current_seat & 0x1) ? STR_WATCHING_NEW_RIDE_BEING_CONSTRUCTED : STR_LOOKING_AT_SCENERY;
            *argument_2 = 0;
        }
        break;
    case PEEP_STATE_PICKED:
        *argument_1 = STR_SELECT_LOCATION;
        *argument_2 = 0;
        break;
    case PEEP_STATE_PATROLLING:
    case PEEP_STATE_ENTERING_PARK:
    case PEEP_STATE_LEAVING_PARK:
        *argument_1 = STR_WALKING;
        *argument_2 = 0;
        break;
    case PEEP_STATE_MOWING:
        *argument_1 = STR_MOWING_GRASS;
        *argument_2 = 0;
        break;
    case PEEP_STATE_SWEEPING:
        *argument_1 = STR_SWEEPING_FOOTPATH;
        *argument_2 = 0;
        break;
    case PEEP_STATE_WATERING:
        *argument_1 = STR_WATERING_GARDENS;
        *argument_2 = 0;
        break;
    case PEEP_STATE_EMPTYING_BIN:
        *argument_1 = STR_EMPTYING_LITTER_BIN;
        *argument_2 = 0;
        break;
    case PEEP_STATE_ANSWERING:
        if (peep->sub_state == 0)
        {
            *argument_1 = STR_WALKING;
            *argument_2 = 0;
        }
        else if (peep->sub_state == 1)
        {
            *argument_1 = STR_ANSWERING_RADIO_CALL;
            *argument_2 = 0;
        }
        else
        {
            ride        = get_ride(peep->current_ride);
            *argument_1 = STR_RESPONDING_TO_RIDE_BREAKDOWN_CALL | ((uint32)ride->name << 16);
            *argument_2 = ride->name_arguments;
        }
        break;
    case PEEP_STATE_FIXING:
        ride        = get_ride(peep->current_ride);
        *argument_1 = STR_FIXING_RIDE | ((uint32)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_HEADING_TO_INSPECTION:
        ride        = get_ride(peep->current_ride);
        *argument_1 = STR_HEADING_TO_RIDE_FOR_INSPECTION | ((uint32)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_INSPECTING:
        ride        = get_ride(peep->current_ride);
        *argument_1 = STR_INSPECTING_RIDE | ((uint32)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    }
}

/**
 * rct2: 0x00698342
 * thought.item (eax)
 * thought.type (ebx)
 * argument_1 (esi & ebx)
 * argument_2 (esi+2)
 */
void peep_thought_set_format_args(rct_peep_thought * thought)
{
    set_format_arg(0, rct_string_id, PeepThoughts[thought->type]);

    uint8 flags = PeepThoughtToActionMap[thought->type].flags;
    if (flags & 1)
    {
        Ride * ride = get_ride(thought->item);
        set_format_arg(2, rct_string_id, ride->name);
        set_format_arg(4, uint32, ride->name_arguments);
    }
    else if (flags & 2)
    {
        set_format_arg(2, rct_string_id, ShopItemStringIds[thought->item].singular);
    }
    else if (flags & 4)
    {
        set_format_arg(2, rct_string_id, ShopItemStringIds[thought->item].indefinite);
    }
    else
    {
    }
}

/** rct2: 0x00982004 */
static constexpr const bool peep_allow_pick_up[] = {
    true,  // PEEP_STATE_FALLING
    false, // PEEP_STATE_1
    false, // PEEP_STATE_QUEUING_FRONT
    false, // PEEP_STATE_ON_RIDE
    false, // PEEP_STATE_LEAVING_RIDE
    true,  // PEEP_STATE_WALKING
    true,  // PEEP_STATE_QUEUING
    false, // PEEP_STATE_ENTERING_RIDE
    true,  // PEEP_STATE_SITTING
    true,  // PEEP_STATE_PICKED
    true,  // PEEP_STATE_PATROLLING
    true,  // PEEP_STATE_MOWING
    true,  // PEEP_STATE_SWEEPING
    false, // PEEP_STATE_ENTERING_PARK
    false, // PEEP_STATE_LEAVING_PARK
    true,  // PEEP_STATE_ANSWERING
    false, // PEEP_STATE_FIXING
    false, // PEEP_STATE_BUYING
    true,  // PEEP_STATE_WATCHING
    true,  // PEEP_STATE_EMPTYING_BIN
    true,  // PEEP_STATE_USING_BIN
    true,  // PEEP_STATE_WATERING
    true,  // PEEP_STATE_HEADING_TO_INSPECTION
    false, // PEEP_STATE_INSPECTING
};

/**
 *
 *  rct2: 0x00698827
 * returns 1 on pickup (CF not set)
 */
sint32 peep_can_be_picked_up(rct_peep * peep)
{
    return peep_allow_pick_up[peep->state];
}

enum
{
    PEEP_FACE_OFFSET_ANGRY = 0,
    PEEP_FACE_OFFSET_VERY_VERY_SICK,
    PEEP_FACE_OFFSET_VERY_SICK,
    PEEP_FACE_OFFSET_SICK,
    PEEP_FACE_OFFSET_VERY_TIRED,
    PEEP_FACE_OFFSET_TIRED,
    PEEP_FACE_OFFSET_VERY_VERY_UNHAPPY,
    PEEP_FACE_OFFSET_VERY_UNHAPPY,
    PEEP_FACE_OFFSET_UNHAPPY,
    PEEP_FACE_OFFSET_NORMAL,
    PEEP_FACE_OFFSET_HAPPY,
    PEEP_FACE_OFFSET_VERY_HAPPY,
    PEEP_FACE_OFFSET_VERY_VERY_HAPPY,
};

static constexpr const sint32 face_sprite_small[] = {
    SPR_PEEP_SMALL_FACE_ANGRY,
    SPR_PEEP_SMALL_FACE_VERY_VERY_SICK,
    SPR_PEEP_SMALL_FACE_VERY_SICK,
    SPR_PEEP_SMALL_FACE_SICK,
    SPR_PEEP_SMALL_FACE_VERY_TIRED,
    SPR_PEEP_SMALL_FACE_TIRED,
    SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY,
    SPR_PEEP_SMALL_FACE_VERY_UNHAPPY,
    SPR_PEEP_SMALL_FACE_UNHAPPY,
    SPR_PEEP_SMALL_FACE_NORMAL,
    SPR_PEEP_SMALL_FACE_HAPPY,
    SPR_PEEP_SMALL_FACE_VERY_HAPPY,
    SPR_PEEP_SMALL_FACE_VERY_VERY_HAPPY,
};

static constexpr const sint32 face_sprite_large[] = {
    SPR_PEEP_LARGE_FACE_ANGRY_0,
    SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_0,
    SPR_PEEP_LARGE_FACE_VERY_SICK_0,
    SPR_PEEP_LARGE_FACE_SICK,
    SPR_PEEP_LARGE_FACE_VERY_TIRED,
    SPR_PEEP_LARGE_FACE_TIRED,
    SPR_PEEP_LARGE_FACE_VERY_VERY_UNHAPPY,
    SPR_PEEP_LARGE_FACE_VERY_UNHAPPY,
    SPR_PEEP_LARGE_FACE_UNHAPPY,
    SPR_PEEP_LARGE_FACE_NORMAL,
    SPR_PEEP_LARGE_FACE_HAPPY,
    SPR_PEEP_LARGE_FACE_VERY_HAPPY,
    SPR_PEEP_LARGE_FACE_VERY_VERY_HAPPY,
};

static sint32 get_face_sprite_offset(rct_peep * peep)
{

    // ANGRY
    if (peep->angriness > 0)
        return PEEP_FACE_OFFSET_ANGRY;

    // VERY_VERY_SICK
    if (peep->nausea > 200)
        return PEEP_FACE_OFFSET_VERY_VERY_SICK;

    // VERY_SICK
    if (peep->nausea > 170)
        return PEEP_FACE_OFFSET_VERY_SICK;

    // SICK
    if (peep->nausea > 140)
        return PEEP_FACE_OFFSET_SICK;

    // VERY_TIRED
    if (peep->energy < 46)
        return PEEP_FACE_OFFSET_VERY_TIRED;

    // TIRED
    if (peep->energy < 70)
        return PEEP_FACE_OFFSET_TIRED;

    sint32 offset = PEEP_FACE_OFFSET_VERY_VERY_UNHAPPY;
    // There are 7 different happiness based faces
    for (sint32 i = 37; peep->happiness >= i; i += 37)
    {
        offset++;
    }

    return offset;
}

/**
 * Function split into large and small sprite
 *  rct2: 0x00698721
 */
sint32 get_peep_face_sprite_small(rct_peep * peep)
{
    return face_sprite_small[get_face_sprite_offset(peep)];
}

/**
 * Function split into large and small sprite
 *  rct2: 0x00698721
 */
sint32 get_peep_face_sprite_large(rct_peep * peep)
{
    return face_sprite_large[get_face_sprite_offset(peep)];
}

/**
 *
 *  rct2: 0x0069A5A0
 * tests if a peep's name matches a cheat code, normally returns using a register flag
 * @param index (eax)
 * @param ride (esi)
 */
sint32 peep_check_easteregg_name(sint32 index, rct_peep * peep)
{
    char buffer[256];

    format_string(buffer, 256, peep->name_string_idx, &peep->id);
    return _stricmp(buffer, gPeepEasterEggNames[index]) == 0;
}

sint32 peep_get_easteregg_name_id(rct_peep * peep)
{
    char buffer[256];

    format_string(buffer, 256, peep->name_string_idx, &peep->id);

    for (uint32 i = 0; i < Util::CountOf(gPeepEasterEggNames); i++)
        if (_stricmp(buffer, gPeepEasterEggNames[i]) == 0)
            return static_cast<sint32>(i);

    return -1;
}

sint32 peep_is_mechanic(rct_peep * peep)
{
    return (peep->sprite_identifier == SPRITE_IDENTIFIER_PEEP && peep->type == PEEP_TYPE_STAFF &&
            peep->staff_type == STAFF_TYPE_MECHANIC);
}

bool rct_peep::HasItem(sint32 peepItem) const
{
    if (peepItem < 32)
    {
        return item_standard_flags & (1u << peepItem);
    }
    else
    {
        return item_extra_flags & (1u << (peepItem - 32));
    }
}

sint32 rct_peep::HasFoodStandardFlag() const
{
    return item_standard_flags &
           (PEEP_ITEM_DRINK | PEEP_ITEM_BURGER | PEEP_ITEM_CHIPS | PEEP_ITEM_ICE_CREAM | PEEP_ITEM_CANDYFLOSS |
            PEEP_ITEM_PIZZA | PEEP_ITEM_POPCORN | PEEP_ITEM_HOT_DOG | PEEP_ITEM_TENTACLE | PEEP_ITEM_TOFFEE_APPLE |
            PEEP_ITEM_DOUGHNUT | PEEP_ITEM_COFFEE | PEEP_ITEM_CHICKEN | PEEP_ITEM_LEMONADE);
}

sint32 rct_peep::HasFoodExtraFlag() const
{
    return item_extra_flags &
           (PEEP_ITEM_PRETZEL | PEEP_ITEM_CHOCOLATE | PEEP_ITEM_ICED_TEA | PEEP_ITEM_FUNNEL_CAKE | PEEP_ITEM_BEEF_NOODLES |
            PEEP_ITEM_FRIED_RICE_NOODLES | PEEP_ITEM_WONTON_SOUP | PEEP_ITEM_MEATBALL_SOUP | PEEP_ITEM_FRUIT_JUICE |
            PEEP_ITEM_SOYBEAN_MILK | PEEP_ITEM_SU_JONGKWA | PEEP_ITEM_SUB_SANDWICH | PEEP_ITEM_COOKIE |
            PEEP_ITEM_ROAST_SAUSAGE);
}

/**
 * To simplify check of 0x36BA3E0 and 0x11FF78
 * returns false on no food.
 */
bool rct_peep::HasFood() const
{
    return HasFoodStandardFlag() || HasFoodExtraFlag();
}

bool rct_peep::HasDrinkStandardFlag() const
{
    return item_standard_flags & (PEEP_ITEM_DRINK | PEEP_ITEM_COFFEE | PEEP_ITEM_LEMONADE);
}

bool rct_peep::HasDrinkExtraFlag() const
{
    return item_extra_flags &
           (PEEP_ITEM_CHOCOLATE | PEEP_ITEM_ICED_TEA | PEEP_ITEM_FRUIT_JUICE | PEEP_ITEM_SOYBEAN_MILK | PEEP_ITEM_SU_JONGKWA);
}

/**
 * To simplify check of NOT(0x12BA3C0 and 0x118F48)
 * returns 0 on no food.
 */
bool rct_peep::HasDrink() const
{
    return HasDrinkStandardFlag() || HasDrinkExtraFlag();
}

sint32 rct_peep::HasEmptyContainerStandardFlag() const
{
    return item_standard_flags & (PEEP_ITEM_EMPTY_CAN | PEEP_ITEM_EMPTY_BURGER_BOX | PEEP_ITEM_EMPTY_CUP |
                                        PEEP_ITEM_RUBBISH | PEEP_ITEM_EMPTY_BOX | PEEP_ITEM_EMPTY_BOTTLE);
}

sint32 rct_peep::HasEmptyContainerExtraFlag() const
{
    return item_extra_flags &
           (PEEP_ITEM_EMPTY_BOWL_RED | PEEP_ITEM_EMPTY_DRINK_CARTON | PEEP_ITEM_EMPTY_JUICE_CUP | PEEP_ITEM_EMPTY_BOWL_BLUE);
}

bool rct_peep::HasEmptyContainer() const
{
    return HasEmptyContainerStandardFlag() || HasEmptyContainerExtraFlag();
}

/**
 *
 *  rct2: 0x699F5A
 * al:thought_type
 * ah:thought_arguments
 * esi: peep
 */
void peep_insert_new_thought(rct_peep * peep, uint8 thought_type, uint8 thought_arguments)
{
    uint8 action = PeepThoughtToActionMap[thought_type].action;
    if (action != 0xFF && peep->action >= 254)
    {
        peep->action                     = action;
        peep->action_frame               = 0;
        peep->action_sprite_image_offset = 0;
        peep->UpdateCurrentActionSpriteType();
        peep->Invalidate();
    }

    for (sint32 i = 0; i < PEEP_MAX_THOUGHTS; ++i)
    {
        rct_peep_thought * thought = &peep->thoughts[i];
        // Remove the oldest thought by setting it to NONE.
        if (thought->type == PEEP_THOUGHT_TYPE_NONE)
            break;

        if (thought->type == thought_type && thought->item == thought_arguments)
        {
            // If the thought type has not changed then we need to move
            // it to the top of the thought list. This is done by first removing the
            // existing thought and placing it at the top.
            if (i < PEEP_MAX_THOUGHTS - 2)
            {
                memmove(thought, thought + 1, sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - i - 1));
            }
            break;
        }
    }

    memmove(&peep->thoughts[1], &peep->thoughts[0], sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - 1));

    peep->thoughts[0].type  = thought_type;
    peep->thoughts[0].item  = thought_arguments;
    peep->thoughts[0].freshness = 0;
    peep->thoughts[0].fresh_timeout = 0;

    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
}

void peep_set_map_tooltip(rct_peep * peep)
{
    if (peep->type == PEEP_TYPE_GUEST)
    {
        set_map_tooltip_format_arg(0, rct_string_id,
                                   (peep->peep_flags & PEEP_FLAGS_TRACKING) ? STR_TRACKED_GUEST_MAP_TIP : STR_GUEST_MAP_TIP);
        set_map_tooltip_format_arg(2, uint32, get_peep_face_sprite_small(peep));
        set_map_tooltip_format_arg(6, rct_string_id, peep->name_string_idx);
        set_map_tooltip_format_arg(8, uint32, peep->id);

        uint32 arg0 = 0, arg1 = 0;
        get_arguments_from_action(peep, &arg0, &arg1);
        set_map_tooltip_format_arg(12, uint32, arg0);
        set_map_tooltip_format_arg(16, uint32, arg1);
    }
    else
    {
        set_map_tooltip_format_arg(0, rct_string_id, STR_STAFF_MAP_TIP);
        set_map_tooltip_format_arg(2, rct_string_id, peep->name_string_idx);
        set_map_tooltip_format_arg(4, uint32, peep->id);

        uint32 arg0 = 0, arg1 = 0;
        get_arguments_from_action(peep, &arg0, &arg1);
        set_map_tooltip_format_arg(8, uint32, arg0);
        set_map_tooltip_format_arg(12, uint32, arg1);
    }
}

/**
 *  rct2: 0x00693BAB
 */
void rct_peep::SwitchNextActionSpriteType()
{
    // TBD: Add nextActionSpriteType as function parameter and make peep->next_action_sprite_type obsolete?
    if (next_action_sprite_type != action_sprite_type)
    {
        Invalidate();
        action_sprite_type               = next_action_sprite_type;
        const rct_sprite_bounds * spriteBounds = g_peep_animation_entries[sprite_type].sprite_bounds;
        sprite_width                     = spriteBounds[next_action_sprite_type].sprite_width;
        sprite_height_negative           = spriteBounds[next_action_sprite_type].sprite_height_negative;
        sprite_height_positive           = spriteBounds[next_action_sprite_type].sprite_height_positive;
        Invalidate();
    }
}

/**
 *
 *  rct2: 0x00693CBB
 */
static sint32 peep_update_queue_position(rct_peep * peep, uint8 previous_action)
{
    peep->time_in_queue++;
    if (peep->next_in_queue == SPRITE_INDEX_NULL)
        return 0;

    rct_peep * peep_next = GET_PEEP(peep->next_in_queue);

    sint16 x_diff = abs(peep_next->x - peep->x);
    sint16 y_diff = abs(peep_next->y - peep->y);
    sint16 z_diff = abs(peep_next->z - peep->z);

    if (z_diff > 10)
        return 0;

    if (x_diff < y_diff)
    {
        sint16 temp_x = x_diff;
        x_diff        = y_diff;
        y_diff        = temp_x;
    }

    x_diff += y_diff / 2;
    if (x_diff > 7)
    {
        if (x_diff > 13)
        {
            if ((peep->x & 0xFFE0) != (peep_next->x & 0xFFE0) || (peep->y & 0xFFE0) != (peep_next->y & 0xFFE0))
                return 0;
        }

        if (peep->sprite_direction != peep_next->sprite_direction)
            return 0;

        switch (peep_next->sprite_direction / 8)
        {
        case 0:
            if (peep->x >= peep_next->x)
                return 0;
            break;
        case 1:
            if (peep->y <= peep_next->y)
                return 0;
            break;
        case 2:
            if (peep->x <= peep_next->x)
                return 0;
            break;
        case 3:
            if (peep->y >= peep_next->y)
                return 0;
            break;
        }
    }

    sint16 xy_dist, x, y;
    if (peep->action < PEEP_ACTION_NONE_1)
        peep->UpdateAction(&x, &y, &xy_dist);

    if (peep->action != PEEP_ACTION_NONE_2)
        return 1;

    peep->action                  = PEEP_ACTION_NONE_1;
    peep->next_action_sprite_type = 2;
    if (previous_action != PEEP_ACTION_NONE_1)
        peep->Invalidate();
    return 1;
}

/**
 *
 *  rct2: 0x00693EF2
 */
static sint32 peep_return_to_centre_of_tile(rct_peep * peep)
{
    peep->direction ^= (1 << 1);
    peep->destination_x         = (peep->x & 0xFFE0) + 16;
    peep->destination_y         = (peep->y & 0xFFE0) + 16;
    peep->destination_tolerance = 5;
    return 1;
}

/**
 *
 *  rct2: 0x00693f2C
 */
static sint32 peep_interact_with_entrance(rct_peep * peep, sint16 x, sint16 y, rct_tile_element * tile_element, uint8 & pathing_result)
{
    uint8 entranceType = tile_element->properties.entrance.type;
    uint8 rideIndex    = tile_element->properties.entrance.ride_index;

    // Store some details to determine when to override the default
    // behaviour (defined below) for when staff attempt to enter a ride
    // to fix/inspect it.
    if (entranceType == ENTRANCE_TYPE_RIDE_EXIT)
    {
        pathing_result |= PATHING_RIDE_EXIT;
        _peepRideEntranceExitElement = tile_element;
    }
    else if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        pathing_result |= PATHING_RIDE_ENTRANCE;
        _peepRideEntranceExitElement = tile_element;
    }

    if (entranceType == ENTRANCE_TYPE_RIDE_EXIT)
    {
        // Default guest/staff behaviour attempting to enter a
        // ride exit is to turn around.
        peep->interaction_ride_index = 0xFF;
        return peep_return_to_centre_of_tile(peep);
    }

    if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        if (peep->type == PEEP_TYPE_STAFF)
        {
            // Default staff behaviour attempting to enter a
            // ride entrance is to turn around.
            peep->interaction_ride_index = 0xFF;
            return peep_return_to_centre_of_tile(peep);
        }

        if (peep->state == PEEP_STATE_QUEUING)
        {
            // Guest is in the ride queue.
            peep->sub_state                  = 11;
            peep->action_sprite_image_offset = _unk_F1AEF0;
            return 1;
        }

        // Guest is on a normal path, i.e. ride has no queue.
        if (peep->interaction_ride_index == rideIndex)
            // Peep is retrying the ride entrance without leaving
            // the path tile and without trying any other ride
            // attached to this path tile. i.e. stick with the
            // peeps previous decision not to go on the ride.
            return peep_return_to_centre_of_tile(peep);

        peep->time_lost     = 0;
        uint8 stationNum = (tile_element->properties.entrance.index >> 4) & 0x7;
        // Guest walks up to the ride for the first time since entering
        // the path tile or since considering another ride attached to
        // the path tile.
        if (!peep->ShouldGoOnRide(rideIndex, stationNum, false, false))
        {
            // Peep remembers that this is the last ride they
            // considered while on this path tile.
            peep->interaction_ride_index = rideIndex;
            return peep_return_to_centre_of_tile(peep);
        }

        // Guest has decided to go on the ride.
        peep->action_sprite_image_offset = _unk_F1AEF0;
        peep->interaction_ride_index     = rideIndex;

        Ride * ride                          = get_ride(rideIndex);
        uint16 previous_last                 = ride->last_peep_in_queue[stationNum];
        ride->last_peep_in_queue[stationNum] = peep->sprite_index;
        peep->next_in_queue                  = previous_last;
        ride->queue_length[stationNum]++;

        peep->current_ride         = rideIndex;
        peep->current_ride_station = stationNum;
        peep->days_in_queue        = 0;
        peep->SetState(PEEP_STATE_QUEUING);
        peep->sub_state     = 11;
        peep->time_in_queue = 0;
        if (peep->peep_flags & PEEP_FLAGS_TRACKING)
        {
            set_format_arg(0, rct_string_id, peep->name_string_idx);
            set_format_arg(2, uint32, peep->id);
            set_format_arg(6, rct_string_id, ride->name);
            set_format_arg(8, uint32, ride->name_arguments);
            if (gConfigNotifications.guest_queuing_for_ride)
            {
                news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, peep->sprite_index);
            }
        }
        return 1;
    }
    else
    {
        // PARK_ENTRANCE
        if (peep->type == PEEP_TYPE_STAFF)
            // Staff cannot leave the park, so go back.
            return peep_return_to_centre_of_tile(peep);

        // If not the centre of the entrance arch
        if (tile_element->properties.entrance.index & 0xF)
            return peep_return_to_centre_of_tile(peep);

        uint8 entranceDirection = tile_element_get_direction(tile_element);
        if (entranceDirection != peep->direction)
        {
            if ((entranceDirection ^ (1 << 1)) != peep->direction)
                return peep_return_to_centre_of_tile(peep);
            // Peep is leaving the park.
            if (peep->state != PEEP_STATE_WALKING)
                return peep_return_to_centre_of_tile(peep);

            if (!(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK))
            {
                // If the park is open and leaving flag isn't set return to centre
                if (gParkFlags & PARK_FLAGS_PARK_OPEN)
                    return peep_return_to_centre_of_tile(peep);
            }

            peep->destination_x += TileDirectionDelta[peep->direction].x;
            peep->destination_y += TileDirectionDelta[peep->direction].y;
            peep->destination_tolerance = 9;
            peep->Invalidate();
            sprite_move(x, y, peep->z, (rct_sprite *)peep);
            peep->Invalidate();

            peep->SetState(PEEP_STATE_LEAVING_PARK);

            peep->var_37 = 0;
            if (peep->peep_flags & PEEP_FLAGS_TRACKING)
            {
                set_format_arg(0, rct_string_id, peep->name_string_idx);
                set_format_arg(2, uint32, peep->id);
                if (gConfigNotifications.guest_left_park)
                {
                    news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_LEFT_PARK, peep->sprite_index);
                }
            }
            return 1;
        }

        // Peep is entering the park.

        if (peep->state != PEEP_STATE_ENTERING_PARK)
            return peep_return_to_centre_of_tile(peep);

        if (!(gParkFlags & PARK_FLAGS_PARK_OPEN))
        {
            peep->state  = PEEP_STATE_LEAVING_PARK;
            peep->var_37 = 1;
            decrement_guests_heading_for_park();
            peep_window_state_update(peep);
            return peep_return_to_centre_of_tile(peep);
        }

        uint8 entranceIndex = 0;
        for (entranceIndex = 0; entranceIndex < MAX_PARK_ENTRANCES; entranceIndex++)
        {
            if (gParkEntrances[entranceIndex].x == (x & 0xFFE0) && gParkEntrances[entranceIndex].y == (y & 0xFFE0))
                break;
        }

        sint16 z          = gParkEntrances[entranceIndex].z / 8;
        entranceDirection = gParkEntrances[entranceIndex].direction;

        sint16 next_x = (x & 0xFFE0) + TileDirectionDelta[entranceDirection].x;
        sint16 next_y = (y & 0xFFE0) + TileDirectionDelta[entranceDirection].y;

        // Make sure there is a path right behind the entrance, otherwise turn around
        bool               found           = false;
        rct_tile_element * nextTileElement = map_get_first_element_at(next_x / 32, next_y / 32);
        do
        {
            if (tile_element_get_type(nextTileElement) != TILE_ELEMENT_TYPE_PATH)
                continue;

            if (footpath_element_is_queue(nextTileElement))
                continue;

            if (footpath_element_is_sloped(nextTileElement))
            {
                uint8 slopeDirection = footpath_element_get_slope_direction(nextTileElement);
                if (slopeDirection == entranceDirection)
                {
                    if (z != nextTileElement->base_height)
                    {
                        continue;
                    }
                    found = true;
                    break;
                }

                if ((slopeDirection ^ (1 << 1)) != entranceDirection)
                    continue;

                if (z - 2 != nextTileElement->base_height)
                    continue;
                found = true;
                break;
            }
            else
            {
                if (z != nextTileElement->base_height)
                {
                    continue;
                }
                found = true;
                break;
            }
        } while (!tile_element_is_last_for_tile(nextTileElement++));

        if (!found)
        {
            peep->state  = PEEP_STATE_LEAVING_PARK;
            peep->var_37 = 1;
            decrement_guests_heading_for_park();
            peep_window_state_update(peep);
            return peep_return_to_centre_of_tile(peep);
        }

        money16 entranceFee = park_get_entrance_fee();
        if (entranceFee != 0)
        {
            if (peep->item_standard_flags & PEEP_ITEM_VOUCHER)
            {
                if (peep->voucher_type == VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE)
                {
                    entranceFee /= 2;
                    peep->item_standard_flags &= ~PEEP_ITEM_VOUCHER;
                    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                }
                else if (peep->voucher_type == VOUCHER_TYPE_PARK_ENTRY_FREE)
                {
                    entranceFee = 0;
                    peep->item_standard_flags &= ~PEEP_ITEM_VOUCHER;
                    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                }
            }
            if (entranceFee > peep->cash_in_pocket)
            {
                peep->state  = PEEP_STATE_LEAVING_PARK;
                peep->var_37 = 1;
                decrement_guests_heading_for_park();
                peep_window_state_update(peep);
                return peep_return_to_centre_of_tile(peep);
            }

            gTotalIncomeFromAdmissions += entranceFee;
            gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_ENTRANCE_TICKETS;
            peep->SpendMoney(peep->paid_to_enter, entranceFee);
            peep->peep_flags |= PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY;
        }

        gTotalAdmissions++;
        window_invalidate_by_number(WC_PARK_INFORMATION, 0);

        peep->var_37 = 1;
        peep->destination_x += TileDirectionDelta[peep->direction].x;
        peep->destination_y += TileDirectionDelta[peep->direction].y;
        peep->destination_tolerance = 7;

        peep->Invalidate();
        sprite_move(x, y, peep->z, (rct_sprite *)peep);
        peep->Invalidate();

        return 1;
    }
}

/**
 *
 *  rct2: 0x006946D8
 */
static sint32 peep_footpath_move_forward(rct_peep * peep, sint16 x, sint16 y, rct_tile_element * tile_element, bool vandalism)
{
    peep->next_x      = (x & 0xFFE0);
    peep->next_y      = (y & 0xFFE0);
    peep->next_z      = tile_element->base_height;
    peep->next_var_29 = tile_element->properties.path.type & 7;

    sint16 z = peep->GetZOnSlope(x, y);

    if (peep->type == PEEP_TYPE_STAFF)
    {
        peep->Invalidate();
        sprite_move(x, y, z, (rct_sprite *)peep);
        peep->Invalidate();
        return 1;
    }

    uint8 vandalThoughtTimeout = (peep->vandalism_seen & 0xC0) >> 6;
    // Advance the vandalised tiles by 1
    uint8 vandalisedTiles = (peep->vandalism_seen * 2) & 0x3F;

    if (vandalism == true)
    {
        // Add one more to the vandalised tiles
        vandalisedTiles |= 1;
        // If there has been 2 vandalised tiles in the last 6
        if (vandalisedTiles & 0x3E && (vandalThoughtTimeout == 0))
        {

            if ((scenario_rand() & 0xFFFF) <= 10922)
            {
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_VANDALISM, PEEP_THOUGHT_ITEM_NONE);
                peep->happiness_target = Math::Max(0, peep->happiness_target - 17);
            }
            vandalThoughtTimeout = 3;
        }
    }

    if (vandalThoughtTimeout && (scenario_rand() & 0xFFFF) <= 4369)
    {
        vandalThoughtTimeout--;
    }

    peep->vandalism_seen = (vandalThoughtTimeout << 6) | vandalisedTiles;
    uint16 crowded      = 0;
    uint8  litter_count = 0;
    uint8  sick_count   = 0;
    uint16 sprite_id    = sprite_get_first_in_quadrant(x, y);
    for (rct_sprite * sprite; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant)
    {
        sprite = get_sprite(sprite_id);
        if (sprite->unknown.sprite_identifier == SPRITE_IDENTIFIER_PEEP)
        {
            rct_peep * other_peep = (rct_peep *)sprite;
            if (other_peep->state != PEEP_STATE_WALKING)
                continue;

            if (abs(other_peep->z - peep->next_z * 8) > 16)
                continue;
            crowded++;
            continue;
        }
        else if (sprite->unknown.sprite_identifier == SPRITE_IDENTIFIER_LITTER)
        {
            rct_litter * litter = (rct_litter *)sprite;
            if (abs(litter->z - peep->next_z * 8) > 16)
                continue;

            litter_count++;
            if (litter->type != LITTER_TYPE_SICK && litter->type != LITTER_TYPE_SICK_ALT)
                continue;

            litter_count--;
            sick_count++;
        }
    }

    if (crowded >= 10 && peep->state == PEEP_STATE_WALKING && (scenario_rand() & 0xFFFF) <= 21845)
    {

        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CROWDED, PEEP_THOUGHT_ITEM_NONE);
        peep->happiness_target = Math::Max(0, peep->happiness_target - 14);
    }

    litter_count = Math::Min(static_cast<uint8>(3), litter_count);
    sick_count   = Math::Min(static_cast<uint8>(3), sick_count);

    uint8 disgusting_time  = peep->disgusting_count & 0xC0;
    uint8 disgusting_count = ((peep->disgusting_count & 0xF) << 2) | sick_count;
    peep->disgusting_count = disgusting_count | disgusting_time;

    if (disgusting_time & 0xC0 && (scenario_rand() & 0xFFFF) <= 4369)
    {
        // Reduce the disgusting time
        peep->disgusting_count -= 0x40;
    }
    else
    {
        uint8 total_sick = 0;
        for (uint8 time = 0; time < 3; time++)
        {
            total_sick += (disgusting_count >> (2 * time)) & 0x3;
        }

        if (total_sick >= 3 && (scenario_rand() & 0xFFFF) <= 10922)
        {
            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_PATH_DISGUSTING, PEEP_THOUGHT_ITEM_NONE);
            peep->happiness_target = Math::Max(0, peep->happiness_target - 17);
            // Reset disgusting time
            peep->disgusting_count |= 0xC0;
        }
    }

    uint8 litter_time  = peep->litter_count & 0xC0;
    litter_count       = ((peep->litter_count & 0xF) << 2) | litter_count;
    peep->litter_count = litter_count | litter_time;

    if (litter_time & 0xC0 && (scenario_rand() & 0xFFFF) <= 4369)
    {
        // Reduce the litter time
        peep->litter_count -= 0x40;
    }
    else
    {
        uint8 total_litter = 0;
        for (uint8 time = 0; time < 3; time++)
        {
            total_litter += (litter_count >> (2 * time)) & 0x3;
        }

        if (total_litter >= 3 && (scenario_rand() & 0xFFFF) <= 10922)
        {
            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_BAD_LITTER, PEEP_THOUGHT_ITEM_NONE);
            peep->happiness_target = Math::Max(0, peep->happiness_target - 17);
            // Reset litter time
            peep->litter_count |= 0xC0;
        }
    }

    peep->Invalidate();
    sprite_move(x, y, z, (rct_sprite *)peep);
    peep->Invalidate();
    return 1;
}

/**
 *
 *  rct2: 0x0069455E
 */
static sint32 peep_interact_with_path(rct_peep * peep, sint16 x, sint16 y, rct_tile_element * tile_element)
{

    // 0x00F1AEE2
    bool vandalism_present = false;
    if (footpath_element_has_path_scenery(tile_element) && (tile_element->flags & TILE_ELEMENT_FLAG_BROKEN) &&
        (tile_element->properties.path.edges & 0xF) != 0xF)
    {
        vandalism_present = true;
    }

    sint16 z = tile_element->base_height * 8;
    if (!map_is_location_owned(x, y, z))
    {
        if (peep->outside_of_park == 0)
            return peep_return_to_centre_of_tile(peep);
    }
    else
    {
        if (peep->outside_of_park == 1)
            return peep_return_to_centre_of_tile(peep);
    }

    if (peep->type == PEEP_TYPE_GUEST && footpath_element_is_queue(tile_element))
    {

        uint8 rideIndex = tile_element->properties.path.ride_index;

        if (peep->state == PEEP_STATE_QUEUING)
        {
            // Check if this queue is connected to the ride the
            // peep is queuing for, i.e. the player hasn't edited
            // the queue, rebuilt the ride, etc.
            if (peep->current_ride == rideIndex)
            {
                return peep_footpath_move_forward(peep, x, y, tile_element, vandalism_present);
            }
            // Queue got disconnected from the original ride.
            peep->interaction_ride_index = 0xFF;
            peep->RemoveFromRide();
            peep->SetState(PEEP_STATE_1);
            return peep_footpath_move_forward(peep, x, y, tile_element, vandalism_present);
        }

        // Peep is not queuing.
        peep->time_lost     = 0;
        uint8 stationNum = (tile_element->properties.path.additions & 0x70) >> 4;

        if ((tile_element->properties.path.type & (1 << 3)) // Queue has the ride sign on it
            && (footpath_element_get_direction(tile_element) ==
                ((peep->direction) ^ 2)) // Ride sign is facing the direction the peep is walking
        )
        {
            /* Peep is approaching the entrance of a ride queue.
             * Decide whether to go on the ride. */
            if (!peep->ShouldGoOnRide(rideIndex, stationNum, true, false))
            {
                // Peep has decided not to go on the ride.
                return peep_return_to_centre_of_tile(peep);
            }
        }
        else
        {
            /* Peep is approaching a queue tile without a ride
             * sign facing the peep. */
            return peep_footpath_move_forward(peep, x, y, tile_element, vandalism_present);
        }

        // Peep has decided to go on the ride at the queue.
        peep->interaction_ride_index = rideIndex;
        Ride * ride                  = get_ride(rideIndex);

        // Add the peep to the ride queue.
        uint16 old_last_peep                 = ride->last_peep_in_queue[stationNum];
        ride->last_peep_in_queue[stationNum] = peep->sprite_index;
        peep->next_in_queue                  = old_last_peep;
        ride->queue_length[stationNum]++;

        peep_decrement_num_riders(peep);
        peep->current_ride         = rideIndex;
        peep->current_ride_station = stationNum;
        peep->state                = PEEP_STATE_QUEUING;
        peep->days_in_queue        = 0;
        peep_window_state_update(peep);

        peep->sub_state             = 10;
        peep->destination_tolerance = 2;
        peep->time_in_queue         = 0;
        if (peep->peep_flags & PEEP_FLAGS_TRACKING)
        {
            set_format_arg(0, rct_string_id, peep->name_string_idx);
            set_format_arg(2, uint32, peep->id);
            set_format_arg(6, rct_string_id, ride->name);
            set_format_arg(8, uint32, ride->name_arguments);
            if (gConfigNotifications.guest_queuing_for_ride)
            {
                news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, peep->sprite_index);
            }
        }

        return peep_footpath_move_forward(peep, x, y, tile_element, vandalism_present);
    }
    else
    {
        peep->interaction_ride_index = 0xFF;
        if (peep->state == PEEP_STATE_QUEUING)
        {
            peep->RemoveFromRide();
            peep->SetState(PEEP_STATE_1);
        }
        return peep_footpath_move_forward(peep, x, y, tile_element, vandalism_present);
    }
}

/**
 *
 *  rct2: 0x00693F70
 */
static sint32 peep_interact_with_shop(rct_peep * peep, sint16 x, sint16 y, rct_tile_element * tile_element)
{
    uint8  rideIndex = track_element_get_ride_index(tile_element);
    Ride * ride      = get_ride(rideIndex);

    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
        return 0;

    if (peep->type == PEEP_TYPE_STAFF)
        return peep_return_to_centre_of_tile(peep);

    peep->time_lost = 0;

    if (ride->status != RIDE_STATUS_OPEN)
        return peep_return_to_centre_of_tile(peep);

    if (peep->interaction_ride_index == rideIndex)
        return peep_return_to_centre_of_tile(peep);

    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
        return peep_return_to_centre_of_tile(peep);

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY))
    {
        peep->time_lost = 0;
        if (!peep->ShouldGoOnRide(rideIndex, 0, false, false))
            return peep_return_to_centre_of_tile(peep);

        money16 cost = ride->price;
        if (cost != 0 && !(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            ride->total_profit += cost;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
            gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS;
            // TODO: Refactor? SpendMoney previously accepted nullptr to not track money, passing a temporary variable as a
            // workaround
            money16 money = 0;
            peep->SpendMoney(money, cost);
        }
        peep->destination_x         = (x & 0xFFE0) + 16;
        peep->destination_y         = (y & 0xFFE0) + 16;
        peep->destination_tolerance = 3;

        peep->current_ride = rideIndex;
        peep->SetState(PEEP_STATE_ENTERING_RIDE);
        peep->sub_state    = PEEP_SHOP_APPROACH;

        peep->time_on_ride = 0;
        ride->cur_num_customers++;
        if (peep->peep_flags & PEEP_FLAGS_TRACKING)
        {
            set_format_arg(0, rct_string_id, peep->name_string_idx);
            set_format_arg(2, uint32, peep->id);
            set_format_arg(6, rct_string_id, ride->name);
            set_format_arg(8, uint32, ride->name_arguments);
            rct_string_id string_id = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE) ? STR_PEEP_TRACKING_PEEP_IS_IN_X
                                                                                             : STR_PEEP_TRACKING_PEEP_IS_ON_X;
            if (gConfigNotifications.guest_used_facility)
            {
                news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, string_id, peep->sprite_index);
            }
        }
        return 1;
    }
    else
    {
        if (peep->guest_heading_to_ride_id == rideIndex)
            peep->guest_heading_to_ride_id = 0xFF;
        peep->action_sprite_image_offset = _unk_F1AEF0;
        peep->SetState(PEEP_STATE_BUYING);
        peep->current_ride = rideIndex;
        peep->sub_state    = 0;
        return 1;
    }
}

bool is_valid_path_z_and_direction(rct_tile_element * tileElement, sint32 currentZ, sint32 currentDirection)
{
    if (footpath_element_is_sloped(tileElement))
    {
        sint32 slopeDirection = footpath_element_get_slope_direction(tileElement);
        if (slopeDirection == currentDirection)
        {
            if (currentZ != tileElement->base_height)
                return false;
        }
        else
        {
            slopeDirection ^= 2;
            if (slopeDirection != currentDirection)
                return false;
            if (currentZ != tileElement->base_height + 2)
                return false;
        }
    }
    else
    {
        if (currentZ != tileElement->base_height)
            return false;
    }
    return true;
}

sint32 rct_peep::PerformNextAction(uint8 & pathing_result)
{
    rct_tile_element * tmpTile;
    return PerformNextAction(pathing_result, tmpTile);
}

/**
 *
 *  rct2: 0x00693C9E
 */
sint32 rct_peep::PerformNextAction(uint8 & pathing_result, rct_tile_element * & tile_result)
{
    pathing_result = 0;
    uint8 previousAction = action;

    if (action == PEEP_ACTION_NONE_1)
        action = PEEP_ACTION_NONE_2;

    if (state == PEEP_STATE_QUEUING)
    {
        if (peep_update_queue_position(this, previousAction))
            return 1;
    }

    sint16 actionX, actionY, xy_dist;
    if (!UpdateAction(&actionX, &actionY, &xy_dist))
    {
        pathing_result |= PATHING_DESTINATION_REACHED;
        uint8 result = 0;
        if (type == PEEP_TYPE_GUEST)
        {
            result = guest_path_finding(this);
        }
        else
        {
            result = staff_path_finding(this);
        }

        if (result != 0)
            return 1;

        if (!UpdateAction(&actionX, &actionY, &xy_dist))
            return 1;
    }

    if ((actionX & 0xFFE0) == next_x && (actionY & 0xFFE0) == next_y)
    {
        sint16 height = GetZOnSlope(actionX, actionY);
        Invalidate();
        MoveTo(actionX, actionY, height);
        Invalidate();
        return 1;
    }

    if (actionX < 32 || actionY < 32 || actionX >= gMapSizeUnits || actionY >= gMapSizeUnits)
    {
        if (outside_of_park == 1)
        {
            pathing_result |= PATHING_OUTSIDE_PARK;
        }
        return peep_return_to_centre_of_tile(this);
    }

    rct_tile_element * tileElement = map_get_first_element_at(actionX / 32, actionY / 32);
    sint16            base_z     = Math::Max(0, (z / 8) - 2);
    sint16            top_z      = (z / 8) + 1;

    do
    {
        if (base_z > tileElement->base_height)
            continue;
        if (top_z < tileElement->base_height)
            continue;
        if (tileElement->flags & TILE_ELEMENT_FLAG_GHOST)
            continue;

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_PATH)
        {
            if (peep_interact_with_path(this, actionX, actionY, tileElement))
            {
                tile_result = tileElement;
                return 1;
            }
        }
        else if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_TRACK)
        {
            if (peep_interact_with_shop(this, actionX, actionY, tileElement))
            {
                tile_result = tileElement;
                return 1;
            }
        }
        else if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_ENTRANCE)
        {
            if (peep_interact_with_entrance(this, actionX, actionY, tileElement, pathing_result))
            {
                tile_result = tileElement;
                return 1;
            }
        }
    } while (!tile_element_is_last_for_tile(tileElement++));

    if (type == PEEP_TYPE_STAFF || (next_var_29 & 0x18))
    {
        sint16 height = abs(tile_element_height(actionX, actionY) - z);

        if (height <= 3 || (type == PEEP_TYPE_STAFF && height <= 32))
        {
            interaction_ride_index = 0xFF;
            if (state == PEEP_STATE_QUEUING)
            {
                RemoveFromQueue();
                SetState(PEEP_STATE_1);
            }

            if (!map_is_location_in_park(actionX & 0xFFE0, actionY & 0xFFE0))
            {
                return peep_return_to_centre_of_tile(this);
            }

            tileElement = map_get_surface_element_at({ actionX, actionY });
            if (tileElement == nullptr)
                return peep_return_to_centre_of_tile(this);

            sint16 water_height = map_get_water_height(tileElement);
            if (water_height)
                return peep_return_to_centre_of_tile(this);

            next_x      = actionX & 0xFFE0;
            next_y      = actionY & 0xFFE0;
            next_z      = tileElement->base_height;
            next_var_29 = 8;

            height = GetZOnSlope(actionX, actionY);
            Invalidate();
            MoveTo(actionX, actionY, height);
            Invalidate();
            return 1;
        }
    }
    return peep_return_to_centre_of_tile(this);
}

// Used when no logging to an expend type required
void rct_peep::SpendMoney(money32 amount)
{
    money16 unused;
    SpendMoney(unused, amount);
}

/**
 *
 *  rct2: 0x0069926C
 * Expend type was previously an offset saved in 0x00F1AEC0
 */
void rct_peep::SpendMoney(money16 & peep_expend_type, money32 amount)
{
    assert(!(gParkFlags & PARK_FLAGS_NO_MONEY));

    cash_in_pocket = Math::Max(0, cash_in_pocket - amount);
    cash_spent += amount;

    peep_expend_type += (money16)amount;

    window_invalidate_by_number(WC_PEEP, sprite_index);

    gUnk141F568 = gUnk13CA740;
    finance_payment(-amount, gCommandExpenditureType);

    if (gConfigGeneral.show_guest_purchases && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        // HACK Currently disabled for multiplayer due to limitation of all sprites
        //      needing to be synchronised
        if (network_get_mode() == NETWORK_MODE_NONE && !gOpenRCT2Headless)
        {
            money_effect_create_at(amount, x, y, z, true);
        }
    }

    audio_play_sound_at_location(SOUND_PURCHASE, x, y, z);
}

void rct_peep::SetHasRidden(sint32 rideIndex)
{
    rides_been_on[rideIndex / 8] |= 1 << (rideIndex % 8);
    Ride * ride = get_ride(rideIndex);
    SetHasRiddenRideType(ride->type);
}

bool rct_peep::HasRidden(sint32 rideIndex) const
{
    return rides_been_on[rideIndex / 8] & (1 << (rideIndex % 8));
}

void rct_peep::SetHasRiddenRideType(sint32 rideType)
{
    ride_types_been_on[rideType / 8] |= 1 << (rideType % 8);
}

bool rct_peep::HasRiddenRideType(sint32 rideType) const
{
    return ride_types_been_on[rideType / 8] & (1 << (rideType % 8));
}

/**
 * Updates various peep stats upon entering a ride, as well as updating the
 * ride's satisfaction value.
 *  rct2: 0x0069545B
 */
void rct_peep::OnEnterRide(uint8 rideIndex)
{
    Ride * ride = get_ride(rideIndex);

    // Calculate how satisfying the ride is for the peep. Can range from -140 to +105.
    sint16 satisfaction = peep_calculate_ride_satisfaction(this, ride);

    // Update the satisfaction stat of the ride.
    uint8 rideSatisfaction = 0;
    if (satisfaction >= 40)
        rideSatisfaction = 3;
    else if (satisfaction >= 20)
        rideSatisfaction = 2;
    else if (satisfaction >= 0)
        rideSatisfaction = 1;

    ride_update_satisfaction(ride, rideSatisfaction);

    // Update various peep stats.
    if (no_of_rides < 255)
        no_of_rides++;

    SetHasRidden(current_ride);
    peep_update_favourite_ride(this, ride);
    happiness_target = Math::Clamp(0, happiness_target + satisfaction, PEEP_MAX_HAPPINESS);
    peep_update_ride_nausea_growth(this, ride);
}

/**
 * Check to see if the specified ride should become the peep's favourite.
 * For this, a "ride rating" is calculated based on the excitement of the ride and the peep's current happiness.
 * As this value cannot exceed 255, the happier the peep is, the more irrelevant the ride's excitement becomes.
 * Due to the minimum happiness requirement, an excitement rating of more than 3.8 has no further effect.
 *
 * If the ride rating is higher than any ride the peep has already been on and the happiness criteria is met,
 * the ride becomes the peep's favourite. (This doesn't happen right away, but will be updated once the peep
 * exits the ride.)
 */
static void peep_update_favourite_ride(rct_peep * peep, Ride * ride)
{
    peep->peep_flags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
    uint8 peepRideRating = Math::Clamp(0, (ride->excitement / 4) + peep->happiness, PEEP_MAX_HAPPINESS);
    if (peepRideRating >= peep->favourite_ride_rating)
    {
        if (peep->happiness >= 160 && peep->happiness_target >= 160)
        {
            peep->favourite_ride_rating = peepRideRating;
            peep->peep_flags |= PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        }
    }
}

/* rct2: 0x00695555 */
static sint16 peep_calculate_ride_value_satisfaction(rct_peep * peep, Ride * ride)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return -30;
    }

    if (ride->value == 0xFFFF)
    {
        return -30;
    }

    money16 ridePrice = ride_get_price(ride);
    if (ride->value >= ridePrice)
    {
        return -5;
    }

    if ((ride->value + ((ride->value * peep->happiness) / 256)) >= ridePrice)
    {
        return -30;
    }

    return 0;
}

/**
 * Calculate satisfaction based on the intensity and nausea of the ride.
 * The best possible score from this section is achieved by having the intensity and nausea
 * of the ride fall exactly within the peep's preferences, but lower scores can still be achieved
 * if the peep's happiness is enough to offset it.
 */
static sint16 peep_calculate_ride_intensity_nausea_satisfaction(rct_peep * peep, Ride * ride)
{
    if (!ride_has_ratings(ride))
    {
        return 70;
    }

    uint8       intensitySatisfaction = 3;
    uint8       nauseaSatisfaction    = 3;
    ride_rating maxIntensity          = (peep->intensity >> 4) * 100;
    ride_rating minIntensity          = (peep->intensity & 0xF) * 100;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= peep->happiness * 2;
    maxIntensity += peep->happiness;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= peep->happiness * 2;
    maxIntensity += peep->happiness;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity)
    {
        intensitySatisfaction--;
    }

    // Although it's not shown in the interface, a peep with Average or High nausea tolerance
    // has a minimum preferred nausea value. (For peeps with None or Low, this is set to zero.)
    ride_rating minNausea = NauseaMinimumThresholds[(peep->nausea_tolerance & 3)];
    ride_rating maxNausea = NauseaMaximumThresholds[(peep->nausea_tolerance & 3)];
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= peep->happiness * 2;
    maxNausea += peep->happiness;
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= peep->happiness * 2;
    maxNausea += peep->happiness;
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea)
    {
        nauseaSatisfaction--;
    }

    uint8 highestSatisfaction = Math::Max(intensitySatisfaction, nauseaSatisfaction);
    uint8 lowestSatisfaction  = Math::Min(intensitySatisfaction, nauseaSatisfaction);

    switch (highestSatisfaction)
    {
    default:
    case 0:
        return 70;
    case 1:
        switch (lowestSatisfaction)
        {
        default:
        case 0:
            return 50;
        case 1:
            return 35;
        }
    case 2:
        switch (lowestSatisfaction)
        {
        default:
        case 0:
            return 35;
        case 1:
            return 20;
        case 2:
            return 10;
        }
    case 3:
        switch (lowestSatisfaction)
        {
        default:
        case 0:
            return -35;
        case 1:
            return -50;
        case 2:
            return -60;
        case 3:
            return -60;
        }
    }
}

/**
 * The satisfaction values calculated here are used to determine how happy the peep is with the ride,
 * and also affects the satisfaction stat of the ride itself. The factors that affect satisfaction include:
 * - The price of the ride compared to the ride's value
 * - How closely the intensity and nausea of the ride matches the peep's preferences
 * - How long the peep was waiting in the queue
 * - If the peep has been on the ride before, or on another ride of the same type
 */
static sint16 peep_calculate_ride_satisfaction(rct_peep * peep, Ride * ride)
{
    sint16 satisfaction = peep_calculate_ride_value_satisfaction(peep, ride);
    satisfaction += peep_calculate_ride_intensity_nausea_satisfaction(peep, ride);

    // Calculate satisfaction based on how long the peep has been in the queue for.
    // (For comparison: peeps start thinking "I've been queueing for a long time" at 3500 and
    // start leaving the queue at 4300.)
    if (peep->time_in_queue >= 4500)
        satisfaction -= 35;
    else if (peep->time_in_queue >= 2250)
        satisfaction -= 10;
    else if (peep->time_in_queue <= 750)
        satisfaction += 10;

    // Peeps get a small boost in satisfaction if they've been on a ride of the same type before,
    // and this boost is doubled if they've already been on this particular ride.
    if (peep->HasRiddenRideType(ride->type))
        satisfaction += 10;

    if (peep->HasRidden(peep->current_ride))
        satisfaction += 10;

    return satisfaction;
}

/**
 * Update the nausea growth of the peep based on a ride. This is calculated based on:
 * - The nausea rating of the ride
 * - Their new happiness growth rate (the higher, the less nauseous)
 * - How hungry the peep is (+0% nausea at 50% hunger up to +100% nausea at 100% hunger)
 * - The peep's nausea tolerance (Final modifier: none: 100%, low: 50%, average: 25%, high: 12.5%)
 */
static void peep_update_ride_nausea_growth(rct_peep * peep, Ride * ride)
{
    uint32 nauseaMultiplier       = Math::Clamp(64, 256 - peep->happiness_target, 200);
    uint32 nauseaGrowthRateChange = (ride->nausea * nauseaMultiplier) / 512;
    nauseaGrowthRateChange *= Math::Max(static_cast<uint8>(128), peep->hunger) / 64;
    nauseaGrowthRateChange >>= (peep->nausea_tolerance & 3);
    peep->nausea_target = (uint8)Math::Min(peep->nausea_target + nauseaGrowthRateChange, 255u);
}

static bool peep_should_go_on_ride_again(rct_peep * peep, Ride * ride)
{
    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN))
        return false;
    if (!ride_has_ratings(ride))
        return false;
    if (ride->intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity)
        return false;
    if (peep->happiness < 180)
        return false;
    if (peep->energy < 100)
        return false;
    if (peep->nausea > 160)
        return false;
    if (peep->hunger < 30)
        return false;
    if (peep->thirst < 20)
        return false;
    if (peep->toilet > 170)
        return false;

    uint8 r = (scenario_rand() & 0xFF);
    if (r <= 128)
    {
        if (peep->no_of_rides > 7)
            return false;
        if (r > 64)
            return false;
    }

    return true;
}

static bool peep_should_preferred_intensity_increase(rct_peep * peep)
{
    if (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
        return false;
    if (peep->happiness < 200)
        return false;

    return (scenario_rand() & 0xFF) >= peep->intensity;
}

static bool peep_really_liked_ride(rct_peep * peep, Ride * ride)
{
    if (peep->happiness < 215)
        return false;
    if (peep->nausea > 120)
        return false;
    if (!ride_has_ratings(ride))
        return false;
    if (ride->intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity)
        return false;
    return true;
}

/**
 *
 *  rct2: 0x0069576E
 */
void rct_peep::OnExitRide(uint8 rideIndex)
{
    Ride * ride = get_ride(rideIndex);

    if (peep_flags & PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE)
    {
        peep_flags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        favourite_ride = rideIndex;
        // TODO fix this flag name or add another one
        window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
    happiness = happiness_target;
    nausea    = nausea_target;
    window_invalidate_flags |= PEEP_INVALIDATE_PEEP_STATS;

    if (peep_flags & PEEP_FLAGS_LEAVING_PARK)
        peep_flags &= ~(PEEP_FLAGS_PARK_ENTRANCE_CHOSEN);

    if (peep_should_go_on_ride_again(this, ride))
    {
        guest_heading_to_ride_id = rideIndex;
        peep_is_lost_countdown   = 200;
        peep_reset_pathfind_goal(this);

        rct_window * w = window_find_by_number(WC_PEEP, sprite_index);
        if (w != nullptr)
        {
            window_event_invalidate_call(w);
            widget_invalidate(w, WC_PEEP__WIDX_ACTION_LBL);
        }
    }

    if (peep_should_preferred_intensity_increase(this))
    {
        if (intensity <= 255 - 16)
        {
            intensity += 16;
        }
    }

    if (peep_really_liked_ride(this, ride))
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_WAS_GREAT, rideIndex);

        sint32 laugh = scenario_rand() & 7;
        if (laugh < 3)
        {
            audio_play_sound_at_location(SOUND_LAUGH_1 + laugh, x, y, z);
        }
    }

    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
}

/** Main logic to decide whether a peep should buy an item in question
 *
 * Also handles the purchase as well, so once it returns, the peep will have the
 * item and the money will have been deducted.
 *
 * eax: shopItem | (rideIndex << 8)
 * ecx: price
 * esi: *peep
 *
 * Returns 0 or 1 depending on if the peep decided to buy the item
 *
 *  rct2: 0x0069AF1E
 */
bool rct_peep::DecideAndBuyItem(uint8 rideIndex, sint32 shopItem, money32 price)
{
    Ride *  ride = get_ride(rideIndex);
    money32 itemValue;

    bool hasVoucher = false;

    if ((item_standard_flags & PEEP_ITEM_VOUCHER) && (voucher_type == VOUCHER_TYPE_FOOD_OR_DRINK_FREE) &&
        (voucher_arguments == shopItem))
    {
        hasVoucher = true;
    }

    if (HasItem(shopItem))
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_ALREADY_GOT, shopItem);
        return false;
    }

    if (shop_item_is_food_or_drink(shopItem))
    {
        sint32 food = -1;
        if ((food = HasFoodStandardFlag()) != 0)
        {
            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_HAVENT_FINISHED, bitscanforward(food));
            return false;
        }
        else if ((food = HasFoodExtraFlag()) != 0)
        {
            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_HAVENT_FINISHED, bitscanforward(food) + 32);
            return false;
        }
        else if (nausea >= 145)
            return false;
    }

    if ((shopItem == SHOP_ITEM_BALLOON) || (shopItem == SHOP_ITEM_ICE_CREAM) || (shopItem == SHOP_ITEM_CANDYFLOSS) ||
        (shopItem == SHOP_ITEM_SUNGLASSES))
    {
        if (climate_is_raining())
            return false;
    }

    if ((shopItem == SHOP_ITEM_SUNGLASSES) || (shopItem == SHOP_ITEM_ICE_CREAM))
    {
        if (gClimateCurrent.Temperature < 12)
            return false;
    }

    if (shop_item_is_food(shopItem) && (hunger > 75))
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_NOT_HUNGRY, PEEP_THOUGHT_ITEM_NONE);
        return false;
    }

    if (shop_item_is_drink(shopItem) && (thirst > 75))
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_NOT_THIRSTY, PEEP_THOUGHT_ITEM_NONE);
        return false;
    }

    if (shopItem == SHOP_ITEM_UMBRELLA && climate_is_raining())
        goto loc_69B119;

    if ((shopItem != SHOP_ITEM_MAP) && shop_item_is_souvenir(shopItem) && !hasVoucher)
    {
        if (((scenario_rand() & 0x7F) + 0x73) > happiness)
            return false;
        else if (no_of_rides < 3)
            return false;
    }

loc_69B119:
    if (!hasVoucher)
    {
        if (price != 0)
        {
            if (cash_in_pocket == 0)
            {
                peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_SPENT_MONEY, PEEP_THOUGHT_ITEM_NONE);
                return false;
            }
            if (price > cash_in_pocket)
            {
                peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_CANT_AFFORD, shopItem);
                return false;
            }
        }

        if (gClimateCurrent.Temperature >= 21)
            itemValue = get_shop_hot_value(shopItem);
        else if (gClimateCurrent.Temperature <= 11)
            itemValue = get_shop_cold_value(shopItem);
        else
            itemValue = get_shop_base_value(shopItem);

        if (itemValue < price)
        {
            itemValue -= price;
            if (shopItem == SHOP_ITEM_UMBRELLA)
            {
                if (climate_is_raining())
                    goto loc_69B221;
            }

            itemValue = -itemValue;
            if (happiness >= 128)
                itemValue /= 2;

            if (happiness >= 180)
                itemValue /= 2;

            if (itemValue > ((money16)(scenario_rand() & 0x07)))
            {
                // "I'm not paying that much for x"
                uint8 thought_type = (shopItem >= 32 ? (PEEP_THOUGHT_TYPE_PHOTO2_MUCH + (shopItem - 32))
                                                     : (PEEP_THOUGHT_TYPE_BALLOON_MUCH + shopItem));
                peep_insert_new_thought(this, thought_type, rideIndex);
                return false;
            }
        }
        else
        {
            itemValue -= price;
            itemValue = Math::Max(8, itemValue);

            if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
            {
                if (itemValue >= (money32)(scenario_rand() & 0x07))
                {
                    // "This x is a really good value"
                    uint8 thought_item = (shopItem >= 32 ? (PEEP_THOUGHT_TYPE_PHOTO2 + (shopItem - 32))
                                                         : (PEEP_THOUGHT_TYPE_BALLOON + shopItem));
                    peep_insert_new_thought(this, thought_item, rideIndex);
                }
            }

            sint32 happinessGrowth = itemValue * 4;
            happiness_target = Math::Min((happiness_target + happinessGrowth), PEEP_MAX_HAPPINESS);
            happiness        = Math::Min((happiness + happinessGrowth), PEEP_MAX_HAPPINESS);
        }
    }

loc_69B221:
    if (!hasVoucher)
    {
        if (gClimateCurrent.Temperature >= 21)
            itemValue = get_shop_hot_value(shopItem);
        else if (gClimateCurrent.Temperature <= 11)
            itemValue = get_shop_cold_value(shopItem);
        else
            itemValue = get_shop_base_value(shopItem);

        itemValue -= price;
        uint8 satisfaction = 0;
        if (itemValue > -8)
        {
            satisfaction++;
            if (itemValue > -3)
            {
                satisfaction++;
                if (itemValue > 3)
                    satisfaction++;
            }
        }

        ride_update_satisfaction(ride, satisfaction);
    }

    // The peep has now decided to buy the item (or, specifically, has not been
    // dissuaded so far).
    if (shopItem >= 32)
        item_extra_flags |= (1u << (shopItem - 32));
    else
        item_standard_flags |= (1u << shopItem);

    if (shopItem == SHOP_ITEM_TSHIRT)
        tshirt_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_HAT)
        hat_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_BALLOON)
        balloon_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_UMBRELLA)
        umbrella_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_MAP)
        peep_reset_pathfind_goal(this);

    uint16 consumptionTime    = item_consumption_time[shopItem];
    time_to_consume = Math::Min((time_to_consume + consumptionTime), 255);

    if (shopItem == SHOP_ITEM_PHOTO)
        photo1_ride_ref = rideIndex;

    if (shopItem == SHOP_ITEM_PHOTO2)
        photo2_ride_ref = rideIndex;

    if (shopItem == SHOP_ITEM_PHOTO3)
        photo3_ride_ref = rideIndex;

    if (shopItem == SHOP_ITEM_PHOTO4)
        photo4_ride_ref = rideIndex;

    window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    peep_update_sprite_type(this);
    if (peep_flags & PEEP_FLAGS_TRACKING)
    {
        set_format_arg(0, rct_string_id, name_string_idx);
        set_format_arg(2, uint32, id);
        set_format_arg(6, rct_string_id, ShopItemStringIds[shopItem].indefinite);
        if (gConfigNotifications.guest_bought_item)
        {
            news_item_add_to_queue(2, STR_PEEP_TRACKING_NOTIFICATION_BOUGHT_X, sprite_index);
        }
    }

    if (shop_item_is_food(shopItem))
        no_of_food++;

    if (shop_item_is_drink(shopItem))
        no_of_drinks++;

    if (shop_item_is_souvenir(shopItem))
        no_of_souvenirs++;

    money16 * expend_type   = &paid_on_souvenirs;
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_SHOP_STOCK;

    if (shop_item_is_food(shopItem))
    {
        expend_type             = &paid_on_food;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_FOODDRINK_STOCK;
    }

    if (shop_item_is_drink(shopItem))
    {
        expend_type             = &paid_on_drink;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_FOODDRINK_STOCK;
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        finance_payment(get_shop_item_cost(shopItem), gCommandExpenditureType);

    // Sets the expenditure type to *_FOODDRINK_SALES or *_SHOP_SALES appropriately.
    gCommandExpenditureType--;
    if (hasVoucher)
    {
        item_standard_flags &= ~PEEP_ITEM_VOUCHER;
        window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }
    else if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        SpendMoney(*expend_type, price);
    }
    ride->total_profit += (price - get_shop_item_cost(shopItem));
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
    ride->cur_num_customers++;
    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

    return true;
}

/**
 *
 *  rct2: 0x0069A98C
 */
void peep_reset_pathfind_goal(rct_peep * peep)
{

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (gPathFindDebug)
    {
        log_info("Resetting pathfind_goal for %s", gPathFindDebugPeepName);
    }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    peep->pathfind_goal.x         = 0xFF;
    peep->pathfind_goal.y         = 0xFF;
    peep->pathfind_goal.z         = 0xFF;
    peep->pathfind_goal.direction = 0xFF;
}

static bool peep_has_valid_xy(rct_peep * peep)
{
    if (peep->x != LOCATION_NULL)
    {
        if (peep->x < (256 * 32) && peep->y < (256 * 32))
        {
            return true;
        }
    }

    return false;
}

using easter_egg_function = void (*)(rct_peep * peep, rct_peep * otherPeep);

static void peep_apply_easter_egg_to_nearby_guests(rct_peep * peep, easter_egg_function easter_egg)
{
    if (!peep_has_valid_xy(peep))
        return;

    uint16 spriteIndex = sprite_get_first_in_quadrant(peep->x, peep->y);
    if (spriteIndex == SPRITE_INDEX_NULL)
        return;

    rct_peep * otherPeep = GET_PEEP(spriteIndex);
    for (; spriteIndex != SPRITE_INDEX_NULL; spriteIndex = otherPeep->next_in_quadrant)
    {
        otherPeep = GET_PEEP(spriteIndex);

        if (otherPeep->sprite_identifier != SPRITE_IDENTIFIER_PEEP)
            continue;

        if (otherPeep->type != PEEP_TYPE_GUEST)
            continue;

        sint32 zDiff = abs(otherPeep->z - peep->z);
        if (zDiff > 32)
            continue;

        easter_egg(peep, otherPeep);
    }
}

static void peep_give_passing_peeps_purple_clothes(rct_peep * peep, rct_peep * otherPeep)
{
    otherPeep->tshirt_colour   = COLOUR_BRIGHT_PURPLE;
    otherPeep->trousers_colour = COLOUR_BRIGHT_PURPLE;
    invalidate_sprite_2((rct_sprite *)otherPeep);
}

static void peep_give_passing_peeps_pizza(rct_peep * peep, rct_peep * otherPeep)
{
    if ((otherPeep->item_standard_flags & PEEP_ITEM_PIZZA))
        return;

    otherPeep->item_standard_flags |= PEEP_ITEM_PIZZA;

    sint32 peepDirection              = (peep->sprite_direction >> 3) ^ 2;
    sint32 otherPeepOppositeDirection = otherPeep->sprite_direction >> 3;
    if (peepDirection == otherPeepOppositeDirection)
    {
        if (otherPeep->action == PEEP_ACTION_NONE_1 || otherPeep->action == PEEP_ACTION_NONE_2)
        {
            peep->Invalidate();
            otherPeep->action                     = PEEP_ACTION_WAVE_2;
            otherPeep->action_frame               = 0;
            otherPeep->action_sprite_image_offset = 0;
            otherPeep->UpdateCurrentActionSpriteType();
            invalidate_sprite_2((rct_sprite *)otherPeep);
        }
    }
    invalidate_sprite_2((rct_sprite *)otherPeep);
}

static void peep_make_passing_peeps_sick(rct_peep * peep, rct_peep * otherPeep)
{
    if (peep == otherPeep)
        return;
    if (otherPeep->state != PEEP_STATE_WALKING)
        return;

    if (otherPeep->action == PEEP_ACTION_NONE_1 || otherPeep->action == PEEP_ACTION_NONE_2)
    {
        otherPeep->action                     = PEEP_ACTION_THROW_UP;
        otherPeep->action_frame               = 0;
        otherPeep->action_sprite_image_offset = 0;
        otherPeep->UpdateCurrentActionSpriteType();
        invalidate_sprite_2((rct_sprite *)otherPeep);
    }
}

static void peep_give_passing_peeps_ice_cream(rct_peep * peep, rct_peep * otherPeep)
{
    if (peep == otherPeep)
        return;
    if (otherPeep->item_standard_flags & PEEP_ITEM_ICE_CREAM)
        return;

    otherPeep->item_standard_flags |= PEEP_ITEM_ICE_CREAM;
    peep_update_sprite_type(otherPeep);
}

/**
 *
 *  rct2: 0x0068FD3A
 */
static void peep_easter_egg_peep_interactions(rct_peep * peep)
{
    if (peep->peep_flags & PEEP_FLAGS_PURPLE)
    {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_give_passing_peeps_purple_clothes);
    }

    if (peep->peep_flags & PEEP_FLAGS_PIZZA)
    {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_give_passing_peeps_pizza);
    }

    if (peep->peep_flags & PEEP_FLAGS_CONTAGIOUS)
    {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_make_passing_peeps_sick);
    }

    if (peep->peep_flags & PEEP_FLAGS_JOY)
    {
        if (scenario_rand() <= 1456)
        {
            if (peep->action == PEEP_ACTION_NONE_1 || peep->action == PEEP_ACTION_NONE_2)
            {
                peep->action                     = PEEP_ACTION_JOY;
                peep->action_frame               = 0;
                peep->action_sprite_image_offset = 0;
                peep->UpdateCurrentActionSpriteType();
                peep->Invalidate();
            }
        }
    }

    if (peep->peep_flags & PEEP_FLAGS_ICE_CREAM)
    {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_give_passing_peeps_ice_cream);
    }
}

/**
 * Gets the height including the bit depending on how far up the slope the peep
 * is.
 *  rct2: 0x00694921
 */
sint32 rct_peep::GetZOnSlope(sint32 tile_x, sint32 tile_y)
{
    if (tile_x == LOCATION_NULL)
        return 0;

    if (next_var_29 & 0x18)
    {
        return tile_element_height(tile_x, tile_y) & 0xFFFF;
    }

    sint32 height = next_z * 8;

    return height + map_height_from_slope(tile_x, tile_y, next_var_29);
}

/**
 *
 *  rct2: 0x00695B70
 */
static void peep_head_for_nearest_ride_type(rct_peep * peep, sint32 rideType)
{
    Ride * ride;

    if (peep->state != PEEP_STATE_SITTING && peep->state != PEEP_STATE_WATCHING && peep->state != PEEP_STATE_WALKING)
    {
        return;
    }
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (peep->x == LOCATION_NULL)
        return;
    if (peep->guest_heading_to_ride_id != 255)
    {
        ride = get_ride(peep->guest_heading_to_ride_id);
        if (ride->type == rideType)
        {
            return;
        }
    }

    uint32 rideConsideration[8]{};

    // FIX Originally checked for a toy,.likely a mistake and should be a map
    if ((peep->item_standard_flags & PEEP_ITEM_MAP) && rideType != RIDE_TYPE_FIRST_AID)
    {
        // Consider all rides in the park
        sint32 i;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride->type == rideType)
            {
                rideConsideration[i >> 5] |= (1u << (i & 0x1F));
            }
        }
    }
    else
    {
        // Take nearby rides into consideration
        sint32 cx = floor2(peep->x, 32);
        sint32 cy = floor2(peep->y, 32);
        for (sint32 x = cx - 320; x <= cx + 320; x += 32)
        {
            for (sint32 y = cy - 320; y <= cy + 320; y += 32)
            {
                if (x >= 0 && y >= 0 && x < (256 * 32) && y < (256 * 32))
                {
                    rct_tile_element * tileElement = map_get_first_element_at(x >> 5, y >> 5);
                    do
                    {
                        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_TRACK)
                            continue;

                        sint32 rideIndex = track_element_get_ride_index(tileElement);
                        ride             = get_ride(rideIndex);
                        if (ride->type == rideType)
                        {
                            rideConsideration[rideIndex >> 5] |= (1u << (rideIndex & 0x1F));
                        }
                    } while (!tile_element_is_last_for_tile(tileElement++));
                }
            }
        }
    }

    // Filter the considered rides
    uint8   potentialRides[256];
    uint8 * nextPotentialRide = &potentialRides[0];
    sint32  numPotentialRides = 0;
    for (sint32 i = 0; i < MAX_RIDES; i++)
    {
        if (!(rideConsideration[i >> 5] & (1u << (i & 0x1F))))
            continue;

        ride = get_ride(i);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL))
        {
            if (peep->ShouldGoOnRide(i, 0, false, true))
            {
                *nextPotentialRide++ = i;
                numPotentialRides++;
            }
        }
    }

    // Pick the closest ride
    sint32 closestRideIndex    = -1;
    sint32 closestRideDistance = std::numeric_limits<sint32>::max();
    for (sint32 i = 0; i < numPotentialRides; i++)
    {
        ride            = get_ride(potentialRides[i]);
        sint32 rideX    = ride->station_starts[0].x * 32;
        sint32 rideY    = ride->station_starts[0].y * 32;
        sint32 distance = abs(rideX - peep->x) + abs(rideY - peep->y);
        if (distance < closestRideDistance)
        {
            closestRideIndex    = potentialRides[i];
            closestRideDistance = distance;
        }
    }
    if (closestRideIndex == -1)
        return;

    // Head to that ride
    peep->guest_heading_to_ride_id = closestRideIndex;
    peep->peep_is_lost_countdown   = 200;
    peep_reset_pathfind_goal(peep);

    // Invalidate windows
    rct_window * w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != nullptr)
    {
        window_event_invalidate_call(w);
        widget_invalidate(w, WC_PEEP__WIDX_ACTION_LBL);
    }

    peep->time_lost = 0;
}

/**
 *
 *  rct2: 0x006958D0
 */
static void peep_head_for_nearest_ride_with_flags(rct_peep * peep, sint32 rideTypeFlags)
{
    Ride * ride;

    if (peep->state != PEEP_STATE_SITTING && peep->state != PEEP_STATE_WATCHING && peep->state != PEEP_STATE_WALKING)
    {
        return;
    }
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (peep->x == LOCATION_NULL)
        return;
    if (peep->guest_heading_to_ride_id != 255)
    {
        ride = get_ride(peep->guest_heading_to_ride_id);
        if (ride_type_has_flag(ride->type,
                               RIDE_TYPE_FLAG_IS_BATHROOM | RIDE_TYPE_FLAG_SELLS_DRINKS | RIDE_TYPE_FLAG_SELLS_FOOD))
        {
            return;
        }
    }

    if ((rideTypeFlags & RIDE_TYPE_FLAG_IS_BATHROOM) && peep->HasFood())
    {
        return;
    }

    uint32 rideConsideration[8]{};

    // FIX Originally checked for a toy,.likely a mistake and should be a map
    if (peep->item_standard_flags & PEEP_ITEM_MAP)
    {
        // Consider all rides in the park
        sint32 i;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride_type_has_flag(ride->type, rideTypeFlags))
            {
                rideConsideration[i >> 5] |= (1u << (i & 0x1F));
            }
        }
    }
    else
    {
        // Take nearby rides into consideration
        sint32 cx = floor2(peep->x, 32);
        sint32 cy = floor2(peep->y, 32);
        for (sint32 x = cx - 320; x <= cx + 320; x += 32)
        {
            for (sint32 y = cy - 320; y <= cy + 320; y += 32)
            {
                if (x >= 0 && y >= 0 && x < (256 * 32) && y < (256 * 32))
                {
                    rct_tile_element * tileElement = map_get_first_element_at(x >> 5, y >> 5);
                    do
                    {
                        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_TRACK)
                            continue;

                        sint32 rideIndex = track_element_get_ride_index(tileElement);
                        ride             = get_ride(rideIndex);
                        if (ride_type_has_flag(ride->type, rideTypeFlags))
                        {
                            rideConsideration[rideIndex >> 5] |= (1u << (rideIndex & 0x1F));
                        }
                    } while (!tile_element_is_last_for_tile(tileElement++));
                }
            }
        }
    }

    // Filter the considered rides
    uint8   potentialRides[256];
    uint8 * nextPotentialRide = &potentialRides[0];
    sint32  numPotentialRides = 0;
    for (sint32 i = 0; i < MAX_RIDES; i++)
    {
        if (!(rideConsideration[i >> 5] & (1u << (i & 0x1F))))
            continue;

        ride = get_ride(i);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL))
        {
            if (peep->ShouldGoOnRide(i, 0, false, true))
            {
                *nextPotentialRide++ = i;
                numPotentialRides++;
            }
        }
    }

    // Pick the closest ride
    sint32 closestRideIndex    = -1;
    sint32 closestRideDistance = std::numeric_limits<sint32>::max();
    for (sint32 i = 0; i < numPotentialRides; i++)
    {
        ride            = get_ride(potentialRides[i]);
        sint32 rideX    = ride->station_starts[0].x * 32;
        sint32 rideY    = ride->station_starts[0].y * 32;
        sint32 distance = abs(rideX - peep->x) + abs(rideY - peep->y);
        if (distance < closestRideDistance)
        {
            closestRideIndex    = potentialRides[i];
            closestRideDistance = distance;
        }
    }
    if (closestRideIndex == -1)
        return;

    // Head to that ride
    peep->guest_heading_to_ride_id = closestRideIndex;
    peep->peep_is_lost_countdown   = 200;
    peep_reset_pathfind_goal(peep);

    // Invalidate windows
    rct_window * w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != nullptr)
    {
        window_event_invalidate_call(w);
        window_invalidate(w);
    }

    peep->time_lost = 0;
}

/**
 *
 *  rct2: 0x0069C483
 */
static void peep_give_real_name(rct_peep * peep)
{
    // Generate a name_string_idx from the peep id using bit twiddling
    uint16 ax = (uint16)(peep->id + 0xF0B);
    uint16 dx = 0;
    dx |= ((ax & 0x400) ? 1 : 0) << 13;
    dx |= ((ax & 0x2000) ? 1 : 0) << 12;
    dx |= ((ax & 0x800) ? 1 : 0) << 11;
    dx |= ((ax & 0x400) ? 1 : 0) << 10;
    dx |= ((ax & 0x1) ? 1 : 0) << 9;
    dx |= ((ax & 0x40) ? 1 : 0) << 8;
    dx |= ((ax & 0x2) ? 1 : 0) << 7;
    dx |= ((ax & 0x4) ? 1 : 0) << 6;
    dx |= ((ax & 0x100) ? 1 : 0) << 5;
    dx |= ((ax & 0x20) ? 1 : 0) << 4;
    dx |= ((ax & 0x80) ? 1 : 0) << 3;
    dx |= ((ax & 0x8) ? 1 : 0) << 2;
    dx |= ((ax & 0x200) ? 1 : 0) << 1;
    dx |= ((ax & 0x10) ? 1 : 0) << 0;
    ax = dx & 0xF;
    dx *= 4;
    ax *= 4096;
    dx += ax;
    if (dx < ax)
    {
        dx += 0x1000;
    }
    dx /= 4;
    dx += REAL_NAME_START;
    peep->name_string_idx = dx;
}

static sint32 peep_compare(const void * sprite_index_a, const void * sprite_index_b)
{
    rct_peep const * peep_a = GET_PEEP(*(uint16 *)sprite_index_a);
    rct_peep const * peep_b = GET_PEEP(*(uint16 *)sprite_index_b);

    // Compare types
    if (peep_a->type != peep_b->type)
    {
        return peep_a->type - peep_b->type;
    }

    // Simple ID comparison for when both peeps use a number or a generated name
    const bool both_numbers = (peep_a->name_string_idx >= 767 && peep_a->name_string_idx <= 771 &&
                               peep_b->name_string_idx >= 767 && peep_b->name_string_idx <= 771);
    if (both_numbers)
    {
        return peep_a->id - peep_b->id;
    }
    const bool both_have_generated_names = (peep_a->name_string_idx >= REAL_NAME_START && peep_a->name_string_idx <= REAL_NAME_END &&
                                            peep_b->name_string_idx >= REAL_NAME_START && peep_b->name_string_idx <= REAL_NAME_END);
    if (both_have_generated_names)
    {
        rct_string_id peep_a_format = peep_a->name_string_idx + REAL_NAME_START;
        rct_string_id peep_b_format = peep_b->name_string_idx + REAL_NAME_START;

        uint16 peep_a_name = (peep_a_format % Util::CountOf(real_names));
        uint16 peep_b_name = (peep_b_format % Util::CountOf(real_names));

        if (peep_a_name == peep_b_name)
        {
            uint16 peep_a_initial = ((peep_a_format >> 10) % Util::CountOf(real_name_initials));
            uint16 peep_b_initial = ((peep_b_format >> 10) % Util::CountOf(real_name_initials));
            return peep_a_initial - peep_b_initial;
        }
        else
        {
            return peep_a_name - peep_b_name;
        }
    }

    // At least one of them has a custom name assigned
    // Compare their names as strings
    utf8   name_a[256];
    utf8   name_b[256];
    uint32 peepIndex = peep_a->id;
    format_string(name_a, 256, peep_a->name_string_idx, &peepIndex);
    peepIndex = peep_b->id;
    format_string(name_b, 256, peep_b->name_string_idx, &peepIndex);
    return strlogicalcmp(name_a, name_b);
}

/**
 *
 *  rct2: 0x00699115
 */
void peep_update_name_sort(rct_peep * peep)
{
    // Remove peep from sprite list
    uint16 nextSpriteIndex = peep->next;
    uint16 prevSpriteIndex = peep->previous;
    if (prevSpriteIndex != SPRITE_INDEX_NULL)
    {
        rct_peep * prevPeep = GET_PEEP(prevSpriteIndex);
        prevPeep->next      = nextSpriteIndex;
    }
    else
    {
        gSpriteListHead[SPRITE_LIST_PEEP] = nextSpriteIndex;
    }

    if (nextSpriteIndex != SPRITE_INDEX_NULL)
    {
        rct_peep * nextPeep = GET_PEEP(nextSpriteIndex);
        nextPeep->previous  = prevSpriteIndex;
    }

    rct_peep * otherPeep;
    uint16     spriteIndex;
    FOR_ALL_PEEPS(spriteIndex, otherPeep)
    {
        // Check if peep should go before this one
        if (peep_compare(&peep->sprite_index, &otherPeep->sprite_index) >= 0)
        {
            continue;
        }

        // Place peep before this one
        peep->previous      = otherPeep->previous;
        otherPeep->previous = peep->sprite_index;
        if (peep->previous != SPRITE_INDEX_NULL)
        {
            rct_peep * prevPeep = GET_PEEP(peep->previous);
            peep->next          = prevPeep->next;
            prevPeep->next      = peep->sprite_index;
        }
        else
        {
            peep->next                        = gSpriteListHead[SPRITE_LIST_PEEP];
            gSpriteListHead[SPRITE_LIST_PEEP] = peep->sprite_index;
        }
        goto finish_peep_sort;
    }

    // Place peep at the end
    FOR_ALL_PEEPS(spriteIndex, otherPeep)
    {
        if (otherPeep->next == SPRITE_INDEX_NULL)
        {
            otherPeep->next = peep->sprite_index;
            peep->previous  = otherPeep->sprite_index;
            peep->next      = SPRITE_INDEX_NULL;
            goto finish_peep_sort;
        }
    }

    gSpriteListHead[SPRITE_LIST_PEEP] = peep->sprite_index;
    peep->next                        = SPRITE_INDEX_NULL;
    peep->previous                    = SPRITE_INDEX_NULL;

finish_peep_sort:
    // This is required at the moment because this function reorders peeps in the sprite list
    sprite_position_tween_reset();
}

void peep_sort()
{
    // Count number of peeps
    uint16     sprite_index, num_peeps = 0;
    rct_peep * peep;
    FOR_ALL_PEEPS(sprite_index, peep)
    {
        num_peeps++;
    }

    // No need to sort
    if (num_peeps < 2)
        return;

    // Create a copy of the peep list and sort it using peep_compare
    uint16 * peep_list = (uint16 *)malloc(num_peeps * sizeof(uint16));
    sint32   i         = 0;
    FOR_ALL_PEEPS(sprite_index, peep)
    {
        peep_list[i++] = peep->sprite_index;
    }
    qsort(peep_list, num_peeps, sizeof(uint16), peep_compare);

    // Set the correct peep->next and peep->previous using the sorted list
    for (i = 0; i < num_peeps; i++)
    {
        peep           = GET_PEEP(peep_list[i]);
        peep->previous = (i > 0) ? peep_list[i - 1] : SPRITE_INDEX_NULL;
        peep->next     = (i + 1 < num_peeps) ? peep_list[i + 1] : SPRITE_INDEX_NULL;
    }
    // Make sure the first peep is set
    gSpriteListHead[SPRITE_LIST_PEEP] = peep_list[0];

    free(peep_list);

    i = 0;
    FOR_ALL_PEEPS(sprite_index, peep)
    {
        i++;
    }
    assert(i == num_peeps);
}

/**
 *
 *  rct2: 0x0069926C
 */
void peep_update_names(bool realNames)
{
    if (realNames)
    {
        gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        rct_peep * peep;
        uint16     spriteIndex;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (peep->name_string_idx == STR_GUEST_X)
            {
                peep_give_real_name(peep);
            }
        }
    }
    else
    {
        gParkFlags &= ~PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        rct_peep * peep;
        uint16     spriteIndex;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (peep->name_string_idx >= REAL_NAME_START && peep->name_string_idx <= REAL_NAME_END)
            {
                peep->name_string_idx = STR_GUEST_X;
            }
        }
    }

    peep_sort();
    gfx_invalidate_screen();
}

bool rct_peep::HeadingForRideOrParkExit() const
{
    return (peep_flags & PEEP_FLAGS_LEAVING_PARK) || (guest_heading_to_ride_id != 0xFF);
}

void peep_handle_easteregg_name(rct_peep * peep)
{
    peep->peep_flags &= ~PEEP_FLAGS_WAVING;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATIE_BRAYSHAW, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_WAVING;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PHOTO;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_CHRIS_SAWYER, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_PHOTO;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PAINTING;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_SIMON_FOSTER, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_PAINTING;
    }

    peep->peep_flags &= ~PEEP_FLAGS_WOW;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_JOHN_WARDLEY, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_WOW;
    }

    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_MELANIE_WARN, peep))
    {
        peep->happiness        = 250;
        peep->happiness_target = 250;
        peep->energy           = 127;
        peep->energy_target    = 127;
        peep->nausea           = 0;
        peep->nausea_target    = 0;
    }

    peep->peep_flags &= ~PEEP_FLAGS_LITTER;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_LISA_STIRLING, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_LITTER;
    }

    peep->peep_flags &= ~PEEP_FLAGS_LOST;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_DONALD_MACRAE, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_LOST;
    }

    peep->peep_flags &= ~PEEP_FLAGS_HUNGER;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATHERINE_MCGOWAN, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_HUNGER;
    }

    peep->peep_flags &= ~PEEP_FLAGS_BATHROOM;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_FRANCES_MCGOWAN, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_BATHROOM;
    }

    peep->peep_flags &= ~PEEP_FLAGS_CROWDED;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_CORINA_MASSOURA, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_CROWDED;
    }

    peep->peep_flags &= ~PEEP_FLAGS_HAPPINESS;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_CAROL_YOUNG, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_HAPPINESS;
    }

    peep->peep_flags &= ~PEEP_FLAGS_NAUSEA;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_MIA_SHERIDAN, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_NAUSEA;
    }

    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATIE_RODGER, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_LEAVING_PARK;
        peep->peep_flags &= ~PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PURPLE;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_EMMA_GARRELL, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_PURPLE;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PIZZA;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_JOANNE_BARTON, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_PIZZA;
    }

    peep->peep_flags &= ~PEEP_FLAGS_CONTAGIOUS;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_FELICITY_ANDERSON, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_CONTAGIOUS;
    }

    peep->peep_flags &= ~PEEP_FLAGS_JOY;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATIE_SMITH, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_JOY;
    }

    peep->peep_flags &= ~PEEP_FLAGS_ANGRY;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_EILIDH_BELL, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_ANGRY;
    }

    peep->peep_flags &= ~PEEP_FLAGS_ICE_CREAM;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_NANCY_STILLWAGON, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_ICE_CREAM;
    }

    peep->peep_flags &= ~PEEP_FLAGS_HERE_WE_ARE;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_DAVID_ELLIS, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_HERE_WE_ARE;
    }
}

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
void pathfind_logging_enable(rct_peep * peep)
{
#if defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
    /* Determine if the pathfinding debugging is wanted for this peep. */
    format_string(gPathFindDebugPeepName, sizeof(gPathFindDebugPeepName), peep->name_string_idx, &(peep->id));

    /* For guests, use the existing PEEP_FLAGS_TRACKING flag to
     * determine for which guest(s) the pathfinding debugging will
     * be output for. */
    if (peep->type == PEEP_TYPE_GUEST)
    {
        gPathFindDebug = peep->peep_flags & PEEP_FLAGS_TRACKING;
    }
    /* For staff, there is no tracking button (any other similar
     * suitable existing mechanism?), so fall back to a crude
     * string comparison with a compile time hardcoded name. */
    else
    {
        gPathFindDebug = strcmp(gPathFindDebugPeepName, "Mechanic Debug") == 0;
    }
#endif // defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
}

void pathfind_logging_disable()
{
#if defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
    gPathFindDebug = false;
#endif // defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
}
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

void peep_autoposition(rct_peep * newPeep)
{
    // Find a location to place new staff member

    newPeep->state = PEEP_STATE_FALLING;

    sint16     x, y, z;
    uint32     count = 0;
    uint16     sprite_index;
    rct_peep * guest = nullptr;

    // Count number of walking guests
    FOR_ALL_GUESTS(sprite_index, guest)
    {
        if (guest->state == PEEP_STATE_WALKING)
            ++count;
    }

    if (count > 0)
    {
        // Place staff at a random guest
        uint32 rand = scenario_rand_max(count);
        FOR_ALL_GUESTS(sprite_index, guest)
        {
            if (guest->state == PEEP_STATE_WALKING)
            {
                if (rand == 0)
                    break;
                --rand;
            }
        }

        x = guest->x;
        y = guest->y;
        z = guest->z;
    }
    else
    {
        // No walking guests; pick random park entrance
        count = 0;
        uint8 i;
        for (i = 0; i < MAX_PARK_ENTRANCES; ++i)
        {
            if (gParkEntrances[i].x != LOCATION_NULL)
                ++count;
        }

        if (count > 0)
        {
            uint32 rand = scenario_rand_max(count);
            for (i = 0; i < MAX_PARK_ENTRANCES; ++i)
            {
                if (gParkEntrances[i].x != LOCATION_NULL)
                {
                    if (rand == 0)
                        break;
                    --rand;
                }
            }

            uint8 dir = gParkEntrances[i].direction;
            x         = gParkEntrances[i].x;
            y         = gParkEntrances[i].y;
            z         = gParkEntrances[i].z;
            x += 16 + ((dir & 1) == 0 ? ((dir & 2) ? 32 : -32) : 0);
            y += 16 + ((dir & 1) == 1 ? ((dir & 2) ? -32 : 32) : 0);
        }
        else
        {
            // No more options; user must pick a location
            newPeep->state = PEEP_STATE_PICKED;
            x              = newPeep->x;
            y              = newPeep->y;
            z              = newPeep->z;
        }
    }

    sprite_move(x, y, z + 16, (rct_sprite *)newPeep);
    invalidate_sprite_2((rct_sprite *)newPeep);
}

void increment_guests_in_park()
{
    if (gNumGuestsInPark < UINT16_MAX)
    {
        gNumGuestsInPark++;
    }
    else
    {
        openrct2_assert(false, "Attempt to increment guests in park above max value (65535).");
    }
}

void increment_guests_heading_for_park()
{
    if (gNumGuestsHeadingForPark < UINT16_MAX)
    {
        gNumGuestsHeadingForPark++;
    }
    else
    {
        openrct2_assert(false, "Attempt to increment guests heading for park above max value (65535).");
    }
}

void decrement_guests_in_park()
{
    if (gNumGuestsInPark > 0)
    {
        gNumGuestsInPark--;
    }
    else
    {
        log_error("Attempt to decrement guests in park below zero.");
    }
}

void decrement_guests_heading_for_park()
{
    if (gNumGuestsHeadingForPark > 0)
    {
        gNumGuestsHeadingForPark--;
    }
    else
    {
        log_error("Attempt to decrement guests heading for park below zero.");
    }
}

static void peep_release_balloon(rct_peep * peep, sint16 spawn_height)
{
    if (peep->item_standard_flags & PEEP_ITEM_BALLOON)
    {
        peep->item_standard_flags &= ~PEEP_ITEM_BALLOON;

        if (peep->sprite_type == PEEP_SPRITE_TYPE_BALLOON && peep->x != LOCATION_NULL)
        {
            create_balloon(peep->x, peep->y, spawn_height, peep->balloon_colour, false);
            peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            peep_update_sprite_type(peep);
        }
    }
}
