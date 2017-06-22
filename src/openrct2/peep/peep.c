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

#include "../audio/audio.h"
#include "../audio/AudioMixer.h"
#include "../cheats.h"
#include "../config/Config.h"
#include "../Context.h"
#include "../game.h"
#include "../input.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../management/finance.h"
#include "../management/marketing.h"
#include "../management/news_item.h"
#include "../network/network.h"
#include "../OpenRCT2.h"
#include "../rct2.h"
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../ride/track.h"
#include "../scenario/scenario.h"
#include "../sprites.h"
#include "../util/util.h"
#include "../world/Climate.h"
#include "../world/entrance.h"
#include "../world/footpath.h"
#include "../world/map.h"
#include "../world/scenery.h"
#include "../world/sprite.h"
#include "peep.h"
#include "staff.h"

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
bool gPathFindDebug = false;
utf8 gPathFindDebugPeepName[256];
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1


uint8 gGuestChangeModifier;
uint16 gNumGuestsInPark;
uint16 gNumGuestsInParkLastWeek;
uint16 gNumGuestsHeadingForPark;

money16 gGuestInitialCash;
uint8 gGuestInitialHappiness;
uint8 gGuestInitialHunger;
uint8 gGuestInitialThirst;

uint32 gNextGuestNumber;

uint8 gPeepWarningThrottle[16];

rct_xyz16 gPeepPathFindGoalPosition;
bool gPeepPathFindIgnoreForeignQueues;
uint8 gPeepPathFindQueueRideIndex;
bool gPeepPathFindSingleChoiceSection;
// uint32 gPeepPathFindAltStationNum;
static bool _peepPathFindIsStaff;
static sint8 _peepPathFindNumJunctions;
static sint8 _peepPathFindMaxJunctions;
static sint32 _peepPathFindTilesChecked;
static uint8 _peepPathFindFewestNumSteps;

/* A junction history for the peep pathfinding heuristic search
 * The magic number 16 is the largest value returned by
 * peep_pathfind_get_max_number_junctions() which should eventually
 * be declared properly. */
static struct {
    rct_xyz8 location;
    uint8 direction;
} _peepPathFindHistory[16];

static uint8 _unk_F1AEF0;
static uint16 _unk_F1EE18;
static rct_map_element * _peepRideEntranceExitElement;
static uint32 _peepRideConsideration[8];
static uint8 _peepPotentialRides[256];

enum {
    PATH_SEARCH_DEAD_END,
    PATH_SEARCH_WIDE,
    PATH_SEARCH_THIN,
    PATH_SEARCH_JUNCTION,
    PATH_SEARCH_RIDE_QUEUE,
    PATH_SEARCH_RIDE_ENTRANCE,
    PATH_SEARCH_RIDE_EXIT,
    PATH_SEARCH_PARK_EXIT,
    PATH_SEARCH_SHOP_ENTRANCE,
    PATH_SEARCH_LIMIT_REACHED,
    PATH_SEARCH_LOOP,
    PATH_SEARCH_OTHER,
    PATH_SEARCH_FAILED
};

// Some text descriptions corresponding to the above enum for understandable debug messages
const char *gPathFindSearchText[] = {"DeadEnd", "Wide", "Thin", "Junction", "RideQueue", "RideEntrance", "RideExit", "ParkEntryExit", "ShopEntrance", "LimitReached", "PathLoop", "Other", "Failed"};



enum {
    F1EE18_DESTINATION_REACHED  = 1 << 0,
    F1EE18_OUTSIDE_PARK         = 1 << 1,
    F1EE18_RIDE_EXIT            = 1 << 2,
    F1EE18_RIDE_ENTRANCE        = 1 << 3,
};

static void * _crowdSoundChannel = NULL;

static void sub_68F41A(rct_peep *peep, sint32 index);
static void peep_update(rct_peep *peep);
static sint32 peep_has_empty_container(rct_peep* peep);
static sint32 peep_has_drink(rct_peep* peep);
static sint32 peep_has_food_standard_flag(rct_peep* peep);
static sint32 peep_has_food_extra_flag(rct_peep* peep);
static sint32 peep_empty_container_standard_flag(rct_peep* peep);
static sint32 peep_empty_container_extra_flag(rct_peep* peep);
static sint32 peep_should_find_bench(rct_peep* peep);
static void peep_stop_purchase_thought(rct_peep* peep, uint8 ride_type);
static void peep_switch_to_next_action_sprite_type(rct_peep* peep);
static sint32 peep_perform_next_action(rct_peep *peep);
static void peep_spend_money(rct_peep *peep, money16 *peep_expend_type, money32 amount);
static void peep_set_has_ridden(rct_peep *peep, sint32 rideIndex);
static bool peep_has_ridden(rct_peep *peep, sint32 rideIndex);
static void peep_set_has_ridden_ride_type(rct_peep *peep, sint32 rideType);
static bool peep_has_ridden_ride_type(rct_peep *peep, sint32 rideType);
static void peep_on_enter_or_exit_ride(rct_peep *peep, sint32 rideIndex, sint32 flags);
static void peep_update_favourite_ride(rct_peep *peep, rct_ride *ride);
static sint16 peep_calculate_ride_satisfaction(rct_peep *peep, rct_ride *ride);
static void peep_update_ride_nausea_growth(rct_peep *peep, rct_ride *ride);
static bool peep_decide_and_buy_item(rct_peep *peep, sint32 rideIndex, sint32 shopItem, money32 price);
static bool peep_should_use_cash_machine(rct_peep *peep, sint32 rideIndex);
static bool peep_should_go_on_ride(rct_peep *peep, sint32 rideIndex, sint32 entranceNum, sint32 flags);
static void peep_ride_is_too_intense(rct_peep *peep, sint32 rideIndex, bool peepAtRide);
static void peep_chose_not_to_go_on_ride(rct_peep *peep, sint32 rideIndex, bool peepAtRide, bool updateLastRide);
static void peep_tried_to_enter_full_queue(rct_peep *peep, sint32 rideIndex);
static bool peep_should_go_to_shop(rct_peep *peep, sint32 rideIndex, bool peepAtShop);
static bool peep_find_ride_to_look_at(rct_peep *peep, uint8 edge, uint8 *rideToView, uint8 *rideSeatToView);
static void peep_easter_egg_peep_interactions(rct_peep *peep);
static sint32 peep_get_height_on_slope(rct_peep *peep, sint32 x, sint32 y);
static void peep_pick_ride_to_go_on(rct_peep *peep);
static void peep_head_for_nearest_ride_type(rct_peep *peep, sint32 rideType);
static void peep_head_for_nearest_ride_with_flags(rct_peep *peep, sint32 rideTypeFlags);
static void peep_give_real_name(rct_peep *peep);
static sint32 guest_surface_path_finding(rct_peep* peep);
static void peep_read_map(rct_peep *peep);
static bool peep_heading_for_ride_or_park_exit(rct_peep *peep);
static bool peep_update_fixing_sub_state_0(rct_ride *ride);
static bool peep_update_fixing_sub_state_1(bool firstRun, rct_peep *peep, rct_ride *ride);
static bool peep_update_fixing_sub_state_2345(bool firstRun, rct_peep *peep, rct_ride *ride);
static bool peep_update_fixing_sub_state_6(bool firstRun, rct_peep *peep, rct_ride *ride);
static bool peep_update_fixing_sub_state_7(bool firstRun, rct_peep *peep, rct_ride *ride);
static bool peep_update_fixing_sub_state_8(bool firstRun, rct_peep *peep);
static bool peep_update_fixing_sub_state_9(bool firstRun, rct_peep *peep, rct_ride *ride);
static bool peep_update_fixing_sub_state_10(bool firstRun, rct_peep *peep, rct_ride *ride);
static bool peep_update_fixing_sub_state_11(bool firstRun, rct_peep *peep, rct_ride *ride);
static bool peep_update_fixing_sub_state_12(bool firstRun, rct_peep *peep, rct_ride *ride);
static bool peep_update_fixing_sub_state_13(bool firstRun, sint32 steps, rct_peep *peep, rct_ride *ride);
static bool peep_update_fixing_sub_state_14(bool firstRun, rct_peep *peep, rct_ride *ride);
static void peep_update_ride_inspected(sint32 rideIndex);
static void peep_release_balloon(rct_peep *peep, sint16 spawn_height);

bool loc_690FD0(rct_peep *peep, uint8 *rideToView, uint8 *rideSeatToView, rct_map_element *esi);

#ifdef DEBUG_DESYNC
#define peep_rand() scenario_rand_data(peep)
#else
#define peep_rand() scenario_rand()
#endif

const char *gPeepEasterEggNames[] = {
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
static struct {
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

static const bool SpriteTypeToSlowWalkMap[] = {
    false, false, false, false, false, false, false, false,
    false, false, false, true,  false, false, true,  true,
    true,  true,  true,  false, true,  false, true,  true,
    true,  false, false, true,  true,  false, false, true,
    true,  true,  true,  true,  true,  true,  false, true,
    false, true,  true,  true,  true,  true,  true,  true,
};

// These arrays contain the base minimum and maximum nausea ratings for peeps, based on their nausea tolerance level.
static const ride_rating NauseaMinimumThresholds[] = {
    0, 0, 200, 400
};

static const ride_rating NauseaMaximumThresholds[] = {
    300, 600, 800, 1000
};

// Locations of the spiral slide platform that a peep walks from the entrance of the ride to the
// entrance of the slide. Up to 4 locations for each 4 sides that an ride entrance can be located
// and 4 different rotations of the ride. 4 * 4 * 4 = 64 locations.
static const rct_xy16 SpiralSlideWalkingPath[64] = {
    {  56,   8 },
    {   8,   8 },
    {   8,  32 },
    {  32,  32 },
    {   8,   8 },
    {   8,   8 },
    {   8,  32 },
    {  32,  32 },
    {   8,  32 },
    {   8,  32 },
    {   8,  32 },
    {  32,  32 },
    {   8,  56 },
    {   8,  32 },
    {   8,  32 },
    {  32,  32 },
    {  56,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  56, -24 },
    {  56,  24 },
    {  32,  24 },
    {  32,   0 },
    {   8,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  32,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  24,   0 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    {  24, -24 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    { -24, -24 },
    {  24, -24 },
    {  24,   0 },
    {   0,   0 },
    {  24,  24 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    {  24,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    {   0,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    { -24,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    { -24,  56 },
    { -24,   8 },
    {   0,   8 },
    {   0,  32 },
};

sint32 peep_get_staff_count()
{
    uint16 spriteIndex;
    rct_peep *peep;
    sint32 count = 0;

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
    sint32 i;
    uint16 spriteIndex;
    rct_peep* peep;

    if (gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        return;

    spriteIndex = gSpriteListHead[SPRITE_LIST_PEEP];
    i = 0;
    while (spriteIndex != SPRITE_INDEX_NULL) {
        peep = &(get_sprite(spriteIndex)->peep);
        spriteIndex = peep->next;

        if ((uint32)(i & 0x7F) != (gCurrentTicks & 0x7F)) {
            peep_update(peep);
        } else {
            sub_68F41A(peep, i);
            if (peep->linked_list_type_offset == SPRITE_LIST_PEEP * 2) {
                peep_update(peep);
            }
        }

        i++;
    }
}

/**
 *
 *  rct2: 0x0069BC9A
 */
static uint8 peep_assess_surroundings(sint16 center_x, sint16 center_y, sint16 center_z){
    if ((map_element_height(center_x, center_y) & 0xFFFF) > center_z)
        return PEEP_THOUGHT_TYPE_NONE;

    uint16 num_scenery = 0;
    uint16 num_fountains = 0;
    uint16 nearby_music = 0;
    uint16 num_rubbish = 0;

    sint16 initial_x = max(center_x - 160, 0);
    sint16 initial_y = max(center_y - 160, 0);
    sint16 final_x = min(center_x + 160, 8192);
    sint16 final_y = min(center_y + 160, 8192);

    for (sint16 x = initial_x; x < final_x; x += 32){
        for (sint16 y = initial_y; y < final_y; y += 32){
            rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);

            do{
                rct_ride* ride;
                rct_scenery_entry* scenery;

                switch (map_element_get_type(mapElement)){
                case MAP_ELEMENT_TYPE_PATH:
                    if (!footpath_element_has_path_scenery(mapElement))
                        break;

                    scenery = get_footpath_item_entry(footpath_element_get_path_scenery_index(mapElement));
                    if (scenery == NULL) {
                        return PEEP_THOUGHT_TYPE_NONE;
                    }
                    if (footpath_element_path_scenery_is_ghost(mapElement))
                        break;

                    if (scenery->path_bit.flags &
                        (PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER |
                        PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW)){
                        num_fountains++;
                        break;
                    }
                    if (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN){
                        num_rubbish++;
                    }
                    break;
                case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
                case MAP_ELEMENT_TYPE_SCENERY:
                    num_scenery++;
                    break;
                case MAP_ELEMENT_TYPE_TRACK:
                    ride = get_ride(mapElement->properties.track.ride_index);
                    if (ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC &&
                        ride->status != RIDE_STATUS_CLOSED &&
                        !(ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))){

                        if (ride->type == RIDE_TYPE_MERRY_GO_ROUND){
                            nearby_music |= 1;
                            break;
                        }

                        if (ride->music == MUSIC_STYLE_ORGAN){
                            nearby_music |= 1;
                            break;
                        }

                        if (ride->type == RIDE_TYPE_DODGEMS){
                            // Dodgems drown out music?
                            nearby_music |= 2;
                        }
                    }
                    break;
                }
            } while (!map_element_is_last_for_tile(mapElement++));
        }
    }

    rct_litter* litter;
    for (uint16 sprite_idx = gSpriteListHead[SPRITE_LIST_LITTER]; sprite_idx != SPRITE_INDEX_NULL; sprite_idx = litter->next) {
        litter = &(get_sprite(sprite_idx)->litter);

        sint16 dist_x = abs(litter->x - center_x);
        sint16 dist_y = abs(litter->y - center_y);
        if (max(dist_x, dist_y) <= 160) {
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
static void peep_update_hunger(rct_peep *peep){
    if (peep->hunger >= 3){
        peep->hunger -= 2;
        peep->energy_growth_rate = min(peep->energy_growth_rate + 2, 255);
        peep->bathroom = min(peep->bathroom + 1, 255);
    }
}

/**
 *
 *  rct2: 0x0068F93E
 */
static void peep_leave_park(rct_peep* peep){
    peep->guest_heading_to_ride_id = 0xFF;
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK){
        if (peep->peep_is_lost_countdown < 60){
            return;
        }
    }
    else{
        peep->peep_is_lost_countdown = 254;
        peep->peep_flags |= PEEP_FLAGS_LEAVING_PARK;
        peep->peep_flags &= ~PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_GO_HOME, 0xFF);

    rct_window* w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != NULL) window_event_invalidate_call(w);
    window_invalidate_by_number(WC_PEEP, peep->sprite_index);
}

/**
 * Main purpose is to decide when peeps leave the park due to
 * low happiness, low energy and (if appropriate) low money.
 *
 *  rct2: 0x0068F8CD
 */
static void peep_decide_whether_to_leave_park(rct_peep *peep)
{
    if (peep->energy_growth_rate >= 33) {
        peep->energy_growth_rate -= 2;
    }

    if (gClimateCurrentTemperature >= 21 && peep->thirst >= 5) {
        peep->thirst--;
    }

    if (peep->outside_of_park != 0) {
        return;
    }

    /* Peeps that are happy enough, have enough energy and
     * (if appropriate) have enough money will always stay
     * in the park. */
    if (!(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)){
        if (gParkFlags & PARK_FLAGS_NO_MONEY) {
            if (peep->energy >= 70 && peep->happiness >= 60) {
                return;
            }
        } else {
            if (
                peep->energy >= 55 &&
                peep->happiness >= 45 &&
                peep->cash_in_pocket >= MONEY(5, 00)
            ) {
                return;
            }
        }
    }

    // Approx 95% chance of staying in the park
    if ((peep_rand() & 0xFFFF) > 3276) {
        return;
    }

    // In the remaining 5% chance the peep leaves the park.
    peep_leave_park(peep);
}

/** rct2: 0x009822F4, 0x00982310 */
static const uint8 byte_9822F4[] = {
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
    50,     // SHOP_ITEM_CANDY_APPLE
    0,      // SHOP_ITEM_TSHIRT
    80,     // SHOP_ITEM_DONUT
    90,     // SHOP_ITEM_COFFEE
    0,      // SHOP_ITEM_EMPTY_CUP
    170,    // SHOP_ITEM_CHICKEN
    115,    // SHOP_ITEM_LEMONADE
    0,      // SHOP_ITEM_EMPTY_BOX
    0,      // SHOP_ITEM_EMPTY_BOTTLE
    0xFF,
    0xFF,
    0xFF,
    0xFF,
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
    100,    // SHOP_ITEM_SU_JONGKWA
    130,    // SHOP_ITEM_SUB_SANDWICH
    75,     // SHOP_ITEM_COOKIE
    0,      // SHOP_ITEM_EMPTY_BOWL_RED
    0,      // SHOP_ITEM_EMPTY_DRINK_CARTON
    0,      // SHOP_ITEM_EMPTY_JUICE_CUP
    115,    // SHOP_ITEM_ROAST_SAUSAGE
    0       // SHOP_ITEM_EMPTY_BOWL_BLUE
};

/** rct2: 009823AC */
static const uint8 crowded_thoughts[] = {
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
static const uint8 peep_item_containers[] = {
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
    0xFF,                           // PEEP_ITEM_CANDY_APPLE
    0xFF,                           // PEEP_ITEM_TSHIRT
    0xFF,                           // PEEP_ITEM_DONUT
    SHOP_ITEM_EMPTY_CUP,            // PEEP_ITEM_COFFEE
    0xFF,                           // PEEP_ITEM_EMPTY_CUP
    SHOP_ITEM_EMPTY_BOX,            // PEEP_ITEM_CHICKEN
    SHOP_ITEM_EMPTY_BOTTLE,         // PEEP_ITEM_LEMONADE
    0xFF,                           // PEEP_ITEM_EMPTY_BOX
    0xFF,                           // PEEP_ITEM_EMPTY_BOTTLE
};

/** rct2: 0x00982342 */
static const uint8 peep_extra_item_containers[] = {
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

/**
 *
 *  rct2: 0x0068F41A
 */
static void sub_68F41A(rct_peep *peep, sint32 index)
{
    if (peep->type == PEEP_TYPE_STAFF){
        if (peep->staff_type != STAFF_TYPE_SECURITY)
            return;

        uint8 sprite_type = PEEP_SPRITE_TYPE_SECURITY_ALT;
        if (peep->state != PEEP_STATE_PATROLLING)
            sprite_type = PEEP_SPRITE_TYPE_SECURITY;

        if (peep->sprite_type == sprite_type)
            return;

        peep->sprite_type = sprite_type;
        peep->action_sprite_image_offset = 0;
        peep->no_action_frame_no = 0;
        if (peep->action < PEEP_ACTION_NONE_1)
            peep->action = PEEP_ACTION_NONE_2;

        peep->peep_flags &= ~PEEP_FLAGS_SLOW_WALK;
        if (SpriteTypeToSlowWalkMap[sprite_type]) {
            peep->peep_flags |= PEEP_FLAGS_SLOW_WALK;
        }

        peep->action_sprite_type = 0xFF;
        peep_update_current_action_sprite_type(peep);
        return;
    }

    if ((uint32)(index & 0x1FF) == (gCurrentTicks & 0x1FF)){
        /* Effect of masking with 0x1FF here vs mask 0x7F,
         * which is the condition for calling this function, is
         * to reduce how often the content in this conditional
         * is executed to once every four calls. */
        if (peep->peep_flags & PEEP_FLAGS_CROWDED){
            uint8 thought_type = crowded_thoughts[peep_rand() & 0xF];
            if (thought_type != PEEP_THOUGHT_TYPE_NONE){
                peep_insert_new_thought(peep, thought_type, 0xFF);
            }
        }

        if (peep->peep_flags & PEEP_FLAGS_EXPLODE && peep->x != MAP_LOCATION_NULL){
            audio_play_sound_at_location(SOUND_CRASH, peep->x, peep->y, peep->z);

            sprite_misc_explosion_cloud_create(peep->x, peep->y, peep->z + 16);
            sprite_misc_explosion_flare_create(peep->x, peep->y, peep->z + 16);

            peep_remove(peep);
            return;
        }

        if (peep->peep_flags & PEEP_FLAGS_HUNGER){
            if (peep->hunger >= 15)peep->hunger -= 15;
        }

        if (peep->peep_flags & PEEP_FLAGS_BATHROOM){
            if (peep->bathroom <= 180)peep->bathroom += 50;
        }

        if (peep->peep_flags & PEEP_FLAGS_HAPPINESS){
            peep->happiness_growth_rate = 5;
        }

        if (peep->peep_flags & PEEP_FLAGS_NAUSEA){
            peep->nausea_growth_rate = 200;
            if (peep->nausea <= 130)peep->nausea = 130;
        }

        if (peep->angriness != 0)
            peep->angriness--;

        if (peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_SITTING){
            peep->var_F2++;
            if (peep->var_F2 >= 18){
                peep->var_F2 = 0;
                if (peep->x != MAP_LOCATION_NULL){

                    uint8 thought_type = peep_assess_surroundings(peep->x & 0xFFE0, peep->y & 0xFFE0, peep->z);

                    if (thought_type != PEEP_THOUGHT_TYPE_NONE) {
                        peep_insert_new_thought(peep, thought_type, 0xFF);
                        peep->happiness_growth_rate = min(255, peep->happiness_growth_rate + 45);
                    }
                }
            }
        }

        peep_update_sprite_type(peep);

        if (peep->state == PEEP_STATE_ON_RIDE || peep->state == PEEP_STATE_ENTERING_RIDE){
            peep->time_on_ride = min(255, peep->time_on_ride + 1);

            if (peep->peep_flags & PEEP_FLAGS_WOW){
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_WOW2, 0xFF);
            }

            if (peep->time_on_ride > 15){
                peep->happiness_growth_rate = min(0, peep->happiness_growth_rate - 5);

                if (peep->time_on_ride > 22){
                    rct_ride* ride = get_ride(peep->current_ride);

                    uint8 thought_type = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE) ?
                        PEEP_THOUGHT_TYPE_GET_OUT :
                        PEEP_THOUGHT_TYPE_GET_OFF;

                    peep_insert_new_thought(peep, thought_type, peep->current_ride);
                }
            }
        }

        if (peep->state == PEEP_STATE_WALKING &&
            peep->outside_of_park == 0 &&
            !(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) &&
            peep->no_of_rides == 0 &&
            peep->guest_heading_to_ride_id == 0xFF){

            uint32 time_duration = gScenarioTicks - peep->time_in_park;
            time_duration /= 2048;

            if (time_duration >= 5){
                peep_pick_ride_to_go_on(peep);

                if (peep->guest_heading_to_ride_id == 0xFF){
                    peep->happiness_growth_rate = max(peep->happiness_growth_rate - 128, 0);
                    peep_leave_park(peep);
                    peep_update_hunger(peep);
                    goto loc_68F9F3;
                }
            }
        }

        if ((peep_rand() & 0xFFFF) <= ((peep->item_standard_flags & PEEP_ITEM_MAP) ? 8192U : 2184U)){
            peep_pick_ride_to_go_on(peep);
        }

        if ((uint32)(index & 0x3FF) == (gCurrentTicks & 0x3FF)){
            /* Effect of masking with 0x3FF here vs mask 0x1FF,
             * which is used in the encompassing conditional, is
             * to reduce how often the content in this conditional
             * is executed to once every second time the encompassing
             * conditional executes. */

            if (peep->outside_of_park == 0 &&
                (peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_SITTING)){

                uint8 num_thoughts = 0;
                uint8 possible_thoughts[5] = { 0 };

                if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK){
                    possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_GO_HOME;
                }
                else{
                    if (peep->energy <= 70 &&
                        peep->happiness < 128){
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_TIRED;
                    }

                    if (peep->hunger <= 10 &&
                        !peep_has_food(peep)){
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_HUNGRY;
                    }

                    if (peep->thirst <= 25 &&
                        !peep_has_food(peep)){
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_THIRSTY;
                    }

                    if (peep->bathroom >= 160){
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_BATHROOM;
                    }

                    if (!(gParkFlags & PARK_FLAGS_NO_MONEY) &&
                        peep->cash_in_pocket <= MONEY(9, 00) &&
                        peep->happiness >= 105 &&
                        peep->energy >= 70){
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

                if (num_thoughts != 0){
                    uint8 chosen_thought = possible_thoughts[peep_rand() % num_thoughts];

                    peep_insert_new_thought(peep, chosen_thought, 0xFF);

                    switch (chosen_thought){
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
        else{
            /* This branch of the conditional is executed on the
             * remaining times the encompassing conditional is
             * executed (which is also every second time, but
             * the alternate time to the true branch). */
            if (peep->nausea >= 140){
                uint8 thought_type = PEEP_THOUGHT_TYPE_SICK;
                if (peep->nausea >= 200){
                    thought_type = PEEP_THOUGHT_TYPE_VERY_SICK;
                    peep_head_for_nearest_ride_type(peep, RIDE_TYPE_FIRST_AID);
                }
                peep_insert_new_thought(peep, thought_type, 0xFF);
            }
        }


        switch (peep->state){
        case PEEP_STATE_WALKING:
        case PEEP_STATE_LEAVING_PARK:
        case PEEP_STATE_ENTERING_PARK:
            peep_decide_whether_to_leave_park(peep);
            peep_update_hunger(peep);
            break;

        case PEEP_STATE_SITTING:
            if (peep->energy_growth_rate <= 135)
                peep->energy_growth_rate += 5;

            if (peep->thirst >= 5){
                peep->thirst -= 4;
                peep->bathroom = min(255, peep->bathroom + 3);
            }

            if (peep->nausea_growth_rate >= 50)
                peep->nausea_growth_rate -= 6;

            // In the original this branched differently
            // but it would mean setting the peep happiness from
            // a thought type entry which i think is incorrect.
            peep_update_hunger(peep);
            break;

        case PEEP_STATE_QUEUING:
            if (peep->time_in_queue >= 2000){
                /* Peep happiness is affected once the peep has been waiting
                 * too long in a queue. */
                rct_map_element* mapElement = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);
                uint8 found = 0;
                do {
                    if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
                        continue;
                    if (mapElement->base_height != peep->next_z)
                        continue;

                    // Check if the footpath has a queue line TV monitor on it
                    if (footpath_element_has_path_scenery(mapElement) && !footpath_element_path_scenery_is_ghost(mapElement)){
                        uint8 pathSceneryIndex = footpath_element_get_path_scenery_index(mapElement);
                        rct_scenery_entry *sceneryEntry = get_footpath_item_entry(pathSceneryIndex);
                        if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_QUEUE_SCREEN){
                            found = 1;
                        }
                    }
                    break;
                } while (!map_element_is_last_for_tile(mapElement++));

                if (found){
                    /* Queue line TV monitors make the peeps waiting in the queue
                     * slowly happier, up to a certain level. */
                    /* Why don't queue line TV monitors start affecting the peeps
                     * as soon as they join the queue?? */
                    if (peep->happiness_growth_rate < 90)
                        peep->happiness_growth_rate = 90;

                    if (peep->happiness_growth_rate < 165)
                        peep->happiness_growth_rate += 2;
                }
                else{
                    /* Without a queue line TV monitor peeps waiting too long
                     * in a queue get less happy. */
                    peep->happiness_growth_rate = max(peep->happiness_growth_rate - 4, 0);
                }
            }
            peep_update_hunger(peep);
            break;
        case PEEP_STATE_ENTERING_RIDE:
            if (peep->sub_state == 17 ||
                peep->sub_state == 15){
                peep_decide_whether_to_leave_park(peep);
            }
            peep_update_hunger(peep);
            break;
        }

    loc_68F9F3:
        // Idle peep happiness tends towards 127 (50%).
        if (peep->happiness_growth_rate >= 128)
            peep->happiness_growth_rate--;
        else
            peep->happiness_growth_rate++;

        peep->nausea_growth_rate = max(peep->nausea_growth_rate - 2, 0);

        if (peep->energy <= 50){
            peep->energy = max(peep->energy - 2, 0);
        }

        if (peep->hunger < 10){
            peep->hunger = max(peep->hunger - 1, 0);
        }

        if (peep->thirst < 10){
            peep->thirst = max(peep->thirst - 1, 0);
        }

        if (peep->bathroom >= 195){
            peep->bathroom--;
        }

        if (peep->state == PEEP_STATE_WALKING &&
            peep->nausea_growth_rate >= 128){

            if ((peep_rand() & 0xFF) <= (uint8)((peep->nausea - 128) / 2)){
                if (peep->action >= PEEP_ACTION_NONE_1){
                    peep->action = PEEP_ACTION_THROW_UP;
                    peep->action_frame = 0;
                    peep->action_sprite_image_offset = 0;
                    peep_update_current_action_sprite_type(peep);
                    invalidate_sprite_2((rct_sprite*)peep);
                }
            }
        }
    }

    // Remaining content is executed every call.

    // 68FA89
    if (peep->var_42 == 0 &&
        peep_has_food(peep)){
        peep->var_42 += 3;
    }

    if (peep->var_42 != 0 &&
        peep->state != PEEP_STATE_ON_RIDE){

        peep->var_42 = max(peep->var_42 - 3, 0);

        if (peep_has_drink(peep)){
            peep->thirst = min(peep->thirst + 7, 255);
        }
        else{
            peep->hunger = min(peep->hunger + 7, 255);
            peep->thirst = max(peep->thirst - 3, 0);
            peep->bathroom = min(peep->bathroom + 2, 255);
        }

        if (peep->var_42 == 0){
            sint32 chosen_food = bitscanforward(peep_has_food_standard_flag(peep));
            if (chosen_food != -1){
                peep->item_standard_flags &= ~(1 << chosen_food);

                uint8 discard_container = peep_item_containers[chosen_food];
                if (discard_container != 0xFF){
                    peep->item_standard_flags |= (1 << discard_container);
                }

                peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                peep_update_sprite_type(peep);
            }
            else{
                chosen_food = bitscanforward(peep_has_food_extra_flag(peep));
                if (chosen_food != -1){
                    peep->item_extra_flags &= ~(1 << chosen_food);
                    uint8 discard_container = peep_extra_item_containers[chosen_food];
                    if (discard_container != 0xFF){
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

    uint8 energy = peep->energy;
    uint8 energy_growth = peep->energy_growth_rate;
    if (energy >= energy_growth){
        energy -= 2;
        if (energy < energy_growth)
            energy = energy_growth;
    }
    else{
        energy = min(255, energy + 4);
        if (energy > energy_growth)
            energy = energy_growth;
    }

    if (energy < 32)
        energy = 32;

    /* This suggests 100% energy is 128. */
    if (energy > 128)
        energy = 128;

    if (energy != peep->energy){
        peep->energy = energy;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;
    }

    uint8 happiness = peep->happiness;
    uint8 happiness_growth = peep->happiness_growth_rate;
    if (happiness >= happiness_growth){
        happiness = max(happiness - 4, 0);
        if (happiness < happiness_growth)
            happiness = happiness_growth;
    }
    else{
        happiness = min(255, happiness + 4);
        if (happiness > happiness_growth)
            happiness = happiness_growth;
    }

    if (happiness != peep->happiness){
        peep->happiness = happiness;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;
    }

    uint8 nausea = peep->nausea;
    uint8 nausea_growth = peep->nausea_growth_rate;
    if (nausea >= nausea_growth){
        nausea = max(nausea - 4, 0);
        if (nausea < nausea_growth)
            nausea = nausea_growth;
    }
    else{
        nausea = min(255, nausea + 4);
        if (nausea > nausea_growth)
            nausea = nausea_growth;
    }

    if (nausea != peep->nausea){
        peep->nausea = nausea;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;
    }
}

/*
 * rct2: 0x68F3AE
 * Set peep state to falling if path below has gone missing, return 1 if current path is valid, 0 if peep starts falling
 */
static sint32 checkForPath(rct_peep *peep){
    peep->var_C4++;
    if ((peep->var_C4 & 0xF) != (peep->sprite_index & 0xF)){
        // This condition makes the check happen less often so the peeps hover for a short,
        // random time when a path below them has been deleted
        return 1;
    }

    rct_map_element* map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

    uint8 map_type = MAP_ELEMENT_TYPE_PATH;
    if (peep->next_var_29 & ((1 << 4) | (1 << 3))){
        map_type = MAP_ELEMENT_TYPE_SURFACE;
    }

    sint32 z = peep->next_z;

    do {
        if (map_element_get_type(map_element) == map_type){
            if (z == map_element->base_height) {
                // Found a suitable path
                return 1;
            }
        }
    } while (!map_element_is_last_for_tile(map_element++));

    // Found no suitable path
    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_FALLING;
    peep_window_state_update(peep);
    return 0;
}

static uint8 peep_get_action_sprite_type(rct_peep* peep)
{
    if (peep->action >= PEEP_ACTION_NONE_1){ // PEEP_ACTION_NONE_1 or PEEP_ACTION_NONE_2
        return PeepSpecialSpriteToSpriteTypeMap[peep->special_sprite];
    } else if (peep->action < countof(PeepActionToSpriteTypeMap)) {
        return PeepActionToSpriteTypeMap[peep->action];
    } else {
        openrct2_assert(peep->action >= countof(PeepActionToSpriteTypeMap) && peep->action < PEEP_ACTION_NONE_1,
                        "Invalid peep action %u", peep->action);
        return 0;
    }
}


/*
*  rct2: 0x00693B58
*/
void peep_update_current_action_sprite_type(rct_peep* peep){
    if (peep->sprite_type >= countof(g_peep_animation_entries)) {
        return;
    }
    uint8 action_sprite_type = peep_get_action_sprite_type(peep);
    if (action_sprite_type == peep->action_sprite_type) {
        return;
    }

    invalidate_sprite_2((rct_sprite*)peep);
    peep->action_sprite_type = action_sprite_type;

    const rct_sprite_bounds* spriteBounds = g_peep_animation_entries[peep->sprite_type].sprite_bounds;
    peep->sprite_width = spriteBounds[action_sprite_type].sprite_width;
    peep->sprite_height_negative =  spriteBounds[action_sprite_type].sprite_height_negative;
    peep->sprite_height_positive =  spriteBounds[action_sprite_type].sprite_height_positive;
    // This is pointless as nothing will have changed.
    invalidate_sprite_2((rct_sprite*)peep);
}

/* 0x00693BE5 */
void peep_switch_to_special_sprite(rct_peep* peep, uint8 special_sprite_id){
    if (special_sprite_id == peep->special_sprite)return;

    peep->special_sprite = special_sprite_id;

    // If NONE_1 or NONE_2
    if (peep->action >= PEEP_ACTION_NONE_1){
        peep->action_sprite_image_offset = 0;
    }
    peep_update_current_action_sprite_type(peep);
}

/**
*
*  rct2: 0x0069A512
*/
void remove_peep_from_ride(rct_peep* peep)
{
    if (peep->state == PEEP_STATE_QUEUING) {
        remove_peep_from_queue(peep);
    }
    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_1;
    peep_window_state_update(peep);
    peep_switch_to_special_sprite(peep, 0);
}

static void peep_state_reset(rct_peep* peep){
    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_1;
    peep_window_state_update(peep);

    peep_switch_to_special_sprite(peep, 0);
}

/**
 *
 *  rct2: 0x69C308
 * Check if lost.
 */
static void peep_check_if_lost(rct_peep* peep){
    if (!(peep->peep_flags & PEEP_FLAGS_LOST)){
        if (gRideCount < 2) return;
        peep->peep_flags ^= PEEP_FLAGS_21;

        if (!(peep->peep_flags & PEEP_FLAGS_21)) return;

        peep->var_F4++;
        if (peep->var_F4 != 254)return;
        peep->var_F4 = 230;
    }
    peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_LOST, 0xFF);

    peep->happiness_growth_rate = max(peep->happiness_growth_rate - 30, 0);
}

/**
 *
 *  rct2: 0x69C26B
* Check if cant find ride.
*/
static void peep_check_cant_find_ride(rct_peep* peep){
    if (peep->guest_heading_to_ride_id == 0xFF)
        return;

    // Peeps will think "I can't find ride X" twice before giving up completely.
    if (peep->peep_is_lost_countdown == 30 || peep->peep_is_lost_countdown == 60) {
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CANT_FIND, peep->guest_heading_to_ride_id);
        peep->happiness_growth_rate = max(peep->happiness_growth_rate - 30, 0);
    }

    peep->peep_is_lost_countdown--;
    if (peep->peep_is_lost_countdown != 0)
        return;

    peep->guest_heading_to_ride_id = 0xFF;
    rct_window* w = window_find_by_number(WC_PEEP, peep->sprite_index);

    if (w){
        window_event_invalidate_call(w);
    }

    window_invalidate_by_number(WC_PEEP, peep->sprite_index);
}

/**
 *
 *  rct2: 0x69C2D0
* Check if cant find exit.
*/
static void peep_check_cant_find_exit(rct_peep* peep){
    if (!(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK))
        return;

    // Peeps who can't find the park exit will continue to get less happy until they find it.
    if (peep->peep_is_lost_countdown == 1) {
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CANT_FIND_EXIT, 0xFF);
        peep->happiness_growth_rate = max(peep->happiness_growth_rate - 30, 0);
    }

    if (--peep->peep_is_lost_countdown == 0)
        peep->peep_is_lost_countdown = 90;
}

/** rct2: 0x00981D7C, 0x00981D7E */
const rct_xy16 word_981D7C[4] = {
    { -2,  0 },
    {  0,  2 },
    {  2,  0 },
    {  0, -2 }
};

/**
 *
 *  rct2: 0x6939EB
 * Also used to move peeps to the correct position to
 * start an action. Returns 1 if the correct destination
 * has not yet been reached. xy_distance is how close the
 * peep is to the target.
 *
 * @param x (ax)
 * @param y (cx)
 * @param distance (bp)
 * @param peep (esi)
 */
static sint32 peep_update_action(sint16* x, sint16* y, sint16* xy_distance, rct_peep* peep){
    _unk_F1AEF0 = peep->action_sprite_image_offset;
    if (peep->action == 0xFE){
        peep->action = 0xFF;
    }

    *x = peep->x - peep->destination_x;
    *y = peep->y - peep->destination_y;

    sint32 x_delta = abs(*x);
    sint32 y_delta = abs(*y);

    *xy_distance = x_delta + y_delta;

    if (peep->action >= 0xFE){
        if (*xy_distance <= peep->destination_tolerence){
            return 0;
        }
        sint32 direction = 0;
        if (x_delta < y_delta){
            direction = 8;
            if (*y >= 0){
                direction = 24;
            }
        }
        else{
            direction = 16;
            if (*x >= 0){
                direction = 0;
            }
        }
        peep->sprite_direction = direction;
        *x = peep->x + word_981D7C[direction / 8].x;
        *y = peep->y + word_981D7C[direction / 8].y;
        peep->no_action_frame_no++;
        const rct_peep_animation * peepAnimation = g_peep_animation_entries[peep->sprite_type].sprite_animation;
        const uint8* imageOffset = peepAnimation[peep->action_sprite_type].frame_offsets;
        if (peep->no_action_frame_no >= peepAnimation[peep->action_sprite_type].num_frames){
            peep->no_action_frame_no = 0;
        }
        peep->action_sprite_image_offset = imageOffset[peep->no_action_frame_no];
        return 1;
    }

    const rct_peep_animation * peepAnimation = g_peep_animation_entries[peep->sprite_type].sprite_animation;
    peep->action_frame++;

    // If last frame of action
    if (peep->action_frame >= peepAnimation[peep->action_sprite_type].num_frames){
        peep->action_sprite_image_offset = 0;
        peep->action = 0xFF;
        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite*)peep);
        *x = peep->x;
        *y = peep->y;
        return 1;
    }
    peep->action_sprite_image_offset = peepAnimation[peep->action_sprite_type].frame_offsets[peep->action_frame];

    // If not throwing up and not at the frame where sick appears.
    if (peep->action != PEEP_ACTION_THROW_UP || peep->action_frame != 15){
        invalidate_sprite_2((rct_sprite*)peep);
        *x = peep->x;
        *y = peep->y;
        return 1;
    }

    // We are throwing up
    peep->hunger /= 2;
    peep->nausea_growth_rate /= 2;

    if (peep->nausea < 30)
        peep->nausea = 0;
    else
        peep->nausea -= 30;

    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;

    // Create sick at location
    litter_create(peep->x, peep->y, peep->z, peep->sprite_direction, (peep->sprite_index & 1) ? LITTER_TYPE_SICK_ALT: LITTER_TYPE_SICK);

    sint32 sound_id = SOUND_COUGH_1 + (peep_rand() & 3);
    audio_play_sound_at_location(sound_id, peep->x, peep->y, peep->z);

    invalidate_sprite_2((rct_sprite*)peep);
    *x = peep->x;
    *y = peep->y;
    return 1;
}

/**
*  rct2: 0x0069A409
* Decreases rider count if on/entering a ride.
*/
void peep_decrement_num_riders(rct_peep* peep){
    if (peep->state == PEEP_STATE_ON_RIDE
        || peep->state == PEEP_STATE_ENTERING_RIDE){

        rct_ride* ride = get_ride(peep->current_ride);
        ride->num_riders--;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
    }
}

/* Part of 0x0069B8CC rct2: 0x0069BC31 */
static void set_sprite_type(rct_peep* peep, uint8 type){
    if (peep->sprite_type == type)return;

    peep->sprite_type = type;
    peep->action_sprite_image_offset = 0;
    peep->no_action_frame_no = 0;

    if (peep->action >= PEEP_ACTION_NONE_1)
        peep->action = PEEP_ACTION_NONE_2;

    peep->peep_flags &= ~PEEP_FLAGS_SLOW_WALK;
    assert(type < countof(SpriteTypeToSlowWalkMap));
    if (SpriteTypeToSlowWalkMap[type]) {
        peep->peep_flags |= PEEP_FLAGS_SLOW_WALK;
    }

    peep->action_sprite_type = 0xFF;
    peep_update_current_action_sprite_type(peep);

    if (peep->state == PEEP_STATE_SITTING){
        peep->action = PEEP_ACTION_NONE_1;
        peep->next_action_sprite_type = 7;
        peep_switch_to_next_action_sprite_type(peep);
    }
    if (peep->state == PEEP_STATE_WATCHING){
        peep->action = PEEP_ACTION_NONE_1;
        peep->next_action_sprite_type = 2;
        peep_switch_to_next_action_sprite_type(peep);
    }
}

typedef struct item_pref_t {
    uint8 type; // 0 for standard, 1 for extra
    uint32 item; // And this with the relevant flags
    uint8 sprite_type;
} item_pref_t;

item_pref_t item_order_preference[] = {
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
        { 0, PEEP_ITEM_CANDY_APPLE, PEEP_SPRITE_TYPE_TOFFEE_APPLE },
        { 0, PEEP_ITEM_DONUT, PEEP_SPRITE_TYPE_DONUT },
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

/**
 *
 *  rct2: 0x0069B8CC
 */
void peep_update_sprite_type(rct_peep* peep)
{
    if (
        peep->sprite_type == PEEP_SPRITE_TYPE_BALLOON &&
        (peep_rand() & 0xFFFF) <= 327
    ) {
        bool isBalloonPopped = false;
        if (peep->x != SPRITE_LOCATION_NULL) {
            if ((peep_rand() & 0xFFFF) <= 13107) {
                isBalloonPopped = true;
                audio_play_sound_at_location(SOUND_BALLOON_POP, peep->x, peep->y, peep->z);
            }
            create_balloon(peep->x, peep->y, peep->z + 9, peep->balloon_colour, isBalloonPopped);
        }
        peep->item_standard_flags &= ~PEEP_ITEM_BALLOON;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }

    if (
        gClimateCurrentRainLevel != 0 &&
        (peep->item_standard_flags & PEEP_ITEM_UMBRELLA) &&
        peep->x != SPRITE_LOCATION_NULL
    ) {
        sint32 x = peep->x & 0xFFE0;
        sint32 y = peep->y & 0xFFE0;

        if (x < 0x1FFF && y < 0x1FFF) {
            rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);
            while (1) {
                if ((peep->z / 8) < map_element->base_height) break;

                if (map_element_is_last_for_tile(map_element)) {
                    set_sprite_type(peep, PEEP_SPRITE_TYPE_UMBRELLA);
                    return;
                }
                map_element++;
            }
        }
    }

    for (item_pref_t* item_pref = item_order_preference; item_pref->type != 0xFF; item_pref++) {
        if (item_pref->type == 0){
            if (peep->item_standard_flags & item_pref->item) {
                set_sprite_type(peep, item_pref->sprite_type);
                return;
            }
        } else {
            if (peep->item_extra_flags & item_pref->item) {
                set_sprite_type(peep, item_pref->sprite_type);
                return;
            }
        }
    }

    if (peep->state == PEEP_STATE_WATCHING && peep->standing_flags & (1 << 1)) {
        set_sprite_type(peep, PEEP_SPRITE_TYPE_WATCHING);
        return;
    }

    if (peep->nausea > 170) {
        set_sprite_type(peep, PEEP_SPRITE_TYPE_VERY_NAUSEOUS);
        return;
    }

    if (peep->nausea > 140) {
        set_sprite_type(peep, PEEP_SPRITE_TYPE_NAUSEOUS);
        return;
    }

    if (peep->energy <= 64 && peep->happiness < 128) {
        set_sprite_type(peep, PEEP_SPRITE_TYPE_HEAD_DOWN);
        return;
    }

    if (peep->energy <= 80 && peep->happiness < 128) {
        set_sprite_type(peep, PEEP_SPRITE_TYPE_ARMS_CROSSED);
        return;
    }

    if (peep->bathroom > 220) {
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
void peep_window_state_update(rct_peep* peep)
{
    rct_window* w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != NULL)
        window_event_invalidate_call(w);

    if (peep->type == PEEP_TYPE_GUEST) {
        if (peep->state == PEEP_STATE_ON_RIDE || peep->state == PEEP_STATE_ENTERING_RIDE) {
            rct_ride* ride = get_ride(peep->current_ride);
            ride->num_riders++;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
        }

        window_invalidate_by_number(WC_PEEP, peep->sprite_index);
        window_invalidate_by_class(WC_GUEST_LIST);
    } else {
        window_invalidate_by_number(WC_PEEP, peep->sprite_index);
        window_invalidate_by_class(WC_STAFF_LIST);
    }
}

void peep_pickup(rct_peep* peep)
{
    remove_peep_from_ride(peep);
    invalidate_sprite_2((rct_sprite*)peep);

    sprite_move(SPRITE_LOCATION_NULL, peep->y, peep->z, (rct_sprite*)peep);
    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_PICKED;
    peep->sub_state = 0;
    peep_window_state_update(peep);
}

void peep_pickup_abort(rct_peep* peep, sint32 old_x)
{
    if (!peep)
        return;

    if (peep->state != PEEP_STATE_PICKED)
        return;

    sprite_move(old_x, peep->y, peep->z + 8, (rct_sprite*)peep);
    invalidate_sprite_2((rct_sprite*)peep);

    if (peep->x != (sint16)SPRITE_LOCATION_NULL){
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_FALLING;
        peep_window_state_update(peep);
        peep->action = 0xFF;
        peep->special_sprite = 0;
        peep->action_sprite_image_offset = 0;
        peep->action_sprite_type = 0;
        peep->var_C4 = 0;
    }

    gPickupPeepImage = UINT32_MAX;
}

bool peep_pickup_place(rct_peep* peep, sint32 x, sint32 y, sint32 z, bool apply)
{
    if (!peep)
        return false;

    rct_map_element *mapElement = map_get_path_element_at(x / 32, y / 32, z);

    if (!mapElement) {
        mapElement = map_get_surface_element_at(x / 32, y / 32);
    }

    if (!mapElement)
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

    sint32 dest_z = mapElement->base_height * 8 + 16;

    if (!map_is_location_owned(tile_x, tile_y, dest_z)){
        gGameCommandErrorTitle = STR_ERR_CANT_PLACE_PERSON_HERE;
        return false;
    }

    if (!map_can_construct_at(tile_x, tile_y, dest_z / 8, (dest_z / 8) + 1, 15)){
        if (gGameCommandErrorText != STR_RAISE_OR_LOWER_LAND_FIRST) {
            if (gGameCommandErrorText != STR_FOOTPATH_IN_THE_WAY) {
                gGameCommandErrorTitle = STR_ERR_CANT_PLACE_PERSON_HERE;
                return false;
            }
        }
    }

    if (apply) {
        sprite_move(dest_x, dest_y, dest_z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        peep_decrement_num_riders(peep);
        peep->state = 0;
        peep_window_state_update(peep);
        peep->action = 0xFF;
        peep->special_sprite = 0;
        peep->action_sprite_image_offset = 0;
        peep->action_sprite_type = 0;
        peep->var_C4 = 0;
        sprite_position_tween_reset();

        if (peep->type == PEEP_TYPE_GUEST) {
            peep->action_sprite_type = 0xFF;
            peep->happiness_growth_rate = max(peep->happiness_growth_rate - 10, 0);
            peep_update_current_action_sprite_type(peep);
        }

        network_set_pickup_peep(game_command_playerid, 0);
    }

    return true;
}

bool peep_pickup_command(uint32 peepnum, sint32 x, sint32 y, sint32 z, sint32 action, bool apply)
{
    if (peepnum >= MAX_SPRITES) {
        log_error("Failed to pick up peep for sprite %d", peepnum);
        return false;
    }
    rct_peep* peep = GET_PEEP(peepnum);
    if (!peep || peep->sprite_identifier != SPRITE_IDENTIFIER_PEEP) {
        return false;
    }
    switch (action) {
        case 0: // pickup
            if (!peep_can_be_picked_up(peep)) {
                return false;
            }
            rct_peep* existing = network_get_pickup_peep(game_command_playerid);
            if (existing) {
                // already picking up a peep
                bool result = peep_pickup_command(existing->sprite_index, network_get_pickup_peep_old_x(game_command_playerid), 0, 0, 1, apply);
                if (existing == peep) {
                    return result;
                }
                if (game_command_playerid == network_get_current_player_id()) {
                    // prevent tool_cancel()
                    input_set_flag(INPUT_FLAG_TOOL_ACTIVE, false);
                }
            }
            if (apply) {
                network_set_pickup_peep(game_command_playerid, peep);
                network_set_pickup_peep_old_x(game_command_playerid, peep->x);
                peep_pickup(peep);
            }
            break;
        case 1: // cancel
            if (apply) {
                peep_pickup_abort(network_get_pickup_peep(game_command_playerid), x);
                network_set_pickup_peep(game_command_playerid, 0);
            }
            break;
        case 2: // place
            if (network_get_pickup_peep(game_command_playerid) != peep) {
                return false;
            }
            if (!peep_pickup_place(peep, x, y, z, apply)) {
                return false;
            }
            break;
    }
    return true;
}

void game_command_pickup_guest(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
{
    sint32 peepnum = *eax;
    sint32 x = *edi;
    sint32 y = *ebp;
    sint32 z = *edx;
    sint32 action = *ecx;
    if (peep_pickup_command(peepnum, x, y, z, action, *ebx & GAME_COMMAND_FLAG_APPLY)) {
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
void peep_sprite_remove(rct_peep* peep){
    remove_peep_from_ride(peep);
    invalidate_sprite_2((rct_sprite*)peep);

    window_close_by_number(WC_PEEP, peep->sprite_index);

    window_close_by_number(WC_FIRE_PROMPT, peep->sprite_identifier);

    if (peep->type == PEEP_TYPE_GUEST){
        window_invalidate_by_class(WC_GUEST_LIST);

        news_item_disable_news(NEWS_ITEM_PEEP_ON_RIDE, peep->sprite_index);
    }
    else{
        window_invalidate_by_class(WC_STAFF_LIST);

        gStaffModes[peep->staff_id] = 0;
        peep->type = 0xFF;
        staff_update_greyed_patrol_areas();
        peep->type = PEEP_TYPE_STAFF;

        news_item_disable_news(NEWS_ITEM_PEEP, peep->sprite_index);
    }
    sprite_remove((rct_sprite*)peep);
}

/**
 * New function removes peep from park existence. Works with staff.
 */
void peep_remove(rct_peep* peep){
    if (peep->type == PEEP_TYPE_GUEST){
        if (peep->outside_of_park == 0){
            decrement_guests_in_park();
            gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PEEP_COUNT;
        }
        if (peep->state == PEEP_STATE_ENTERING_PARK){
            decrement_guests_heading_for_park();
        }
    }
    peep_sprite_remove(peep);
}

/**
 * Falling and its subset drowning
 *  rct2: 0x690028
 */
static void peep_update_falling(rct_peep* peep){
    if (peep->action == PEEP_ACTION_DROWNING){
        // Check to see if we are ready to drown.
        sint16 x, y, xy_distance;

        peep_update_action(&x, &y, &xy_distance, peep);
        if (peep->action == PEEP_ACTION_DROWNING) return;

        if (gConfigNotifications.guest_died) {
            set_format_arg(0, rct_string_id, peep->name_string_idx);
            set_format_arg(2, uint32, peep->id);
            news_item_add_to_queue(NEWS_ITEM_BLANK, STR_NEWS_ITEM_GUEST_DROWNED, peep->x | (peep->y << 16));
        }

        gParkRatingCasualtyPenalty = min(gParkRatingCasualtyPenalty + 25, 1000);
        peep_remove(peep);
        return;
    }

    // If not drowning then falling. Note: peeps 'fall' after leaving a ride/enter the park.
    rct_map_element *map_element = map_get_first_element_at(peep->x / 32, peep->y / 32);
    rct_map_element *saved_map = NULL;
    sint32 saved_height = 0;

    if (map_element != NULL) {
        do {
            // If a path check if we are on it
            if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_PATH) {
                sint32 height = map_height_from_slope(peep->x, peep->y, map_element->properties.surface.slope)
                    + map_element->base_height * 8;

                if (height < peep->z - 1 || height > peep->z + 4) continue;

                saved_height = height;
                saved_map = map_element;
                break;
            } // If a surface get the height and see if we are on it
            else if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_SURFACE) {
                // If the surface is water check to see if we could be drowning
                if (map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK) {
                    sint32 height = (map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK) * 16;

                    if (height - 4 >= peep->z && height < peep->z + 20) {
                        // Looks like we are drowning!
                        invalidate_sprite_2((rct_sprite*)peep);
                        sprite_move(peep->x, peep->y, height, (rct_sprite*)peep);
                        // Drop balloon if held
                        peep_release_balloon(peep, height); 

                        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_DROWNING, -1);

                        peep->action = PEEP_ACTION_DROWNING;
                        peep->action_frame = 0;
                        peep->action_sprite_image_offset = 0;

                        peep_update_current_action_sprite_type(peep);
                        invalidate_sprite_2((rct_sprite*)peep);
                        peep_window_state_update(peep);
                        return;
                    }
                }
                sint32 map_height = map_element_height(0xFFFF & peep->x, 0xFFFF & peep->y) & 0xFFFF;
                if (map_height < peep->z || map_height - 4 > peep->z) continue;
                saved_height = map_height;
                saved_map = map_element;
            } // If not a path or surface go see next element
            else continue;
        } while (!map_element_is_last_for_tile(map_element++));
    }

    // This will be null if peep is falling
    if (saved_map == NULL){
        invalidate_sprite_2((rct_sprite*)peep);
        if (peep->z <= 1){
            // Remove peep if it has gone to the void
            peep_remove(peep);
            return;
        }
        sprite_move(peep->x, peep->y, peep->z - 2, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    invalidate_sprite_2((rct_sprite*)peep);
    sprite_move(peep->x, peep->y, saved_height, (rct_sprite*)peep);
    invalidate_sprite_2((rct_sprite*)peep);

    peep->next_x = peep->x & 0xFFE0;
    peep->next_y = peep->y & 0xFFE0;
    peep->next_z = saved_map->base_height;

    sint32 edx = saved_map->properties.surface.slope & 0x7;
    if (map_element_get_type(saved_map) != MAP_ELEMENT_TYPE_PATH){
        edx = 8;
    }
    peep->next_var_29 = edx;
    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_1;
    peep_window_state_update(peep);
}

/**
 *
 *  rct2: 0x00691677
 */
static void peep_try_get_up_from_sitting(rct_peep* peep){
    // Eats all food first
    if (peep_has_food(peep))return;

    peep->time_to_sitdown--;
    if (peep->time_to_sitdown) return;

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_WALKING;
    peep_window_state_update(peep);

    // Set destination to the center of the tile.
    peep->destination_x = (peep->x & 0xFFE0) + 16;
    peep->destination_y = (peep->y & 0xFFE0) + 16;
    peep->destination_tolerence = 5;
    peep_update_current_action_sprite_type(peep);
}

/** rct2: 0x00981F2C, 0x00981F2E */
static const rct_xy16 _981F2C[] = {
    {  7, 12 },
    { 12, 25 },
    { 25, 20 },
    { 20,  7 },
    {  7, 20 },
    { 20, 25 },
    { 25, 12 },
    { 12,  7 },
};

/**
 *
 *  rct2: 0x0069152B
 */
static void peep_update_sitting(rct_peep* peep){
    if (peep->sub_state == 0){
        if (!checkForPath(peep))return;
        //691541

        peep_perform_next_action(peep);
        if (!(_unk_F1EE18 & F1EE18_DESTINATION_REACHED)) return;

        sint32 ebx = peep->var_37 & 0x7;
        sint32 x = (peep->x & 0xFFE0) + _981F2C[ebx].x;
        sint32 y = (peep->y & 0xFFE0) + _981F2C[ebx].y;
        sint32 z = peep->z;

        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, z, (rct_sprite*)peep);

        peep->sprite_direction = ((peep->var_37 + 2) & 3) * 8;
        invalidate_sprite_2((rct_sprite*)peep);
        peep->action = 254;
        peep->next_action_sprite_type = 7;
        peep_switch_to_next_action_sprite_type(peep);

        peep->sub_state++;

        // Sets time to sit on seat
        peep->time_to_sitdown = (129 - peep->energy) * 16 + 50;
    }
    else if (peep->sub_state == 1){
        if (peep->action < 0xFE){
            sint16 x, y, xy_distance;
            peep_update_action(&x, &y, &xy_distance, peep);
            if (peep->action != 0xFF) return;

            peep->action = 0xFE;
            peep_try_get_up_from_sitting(peep);
            return;
        }

        if ((peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)){
            peep_decrement_num_riders(peep);
            peep->state = PEEP_STATE_WALKING;
            peep_window_state_update(peep);

            // Set destination to the center of the tile
            peep->destination_x = (peep->x & 0xFFE0) + 16;
            peep->destination_y = (peep->y & 0xFFE0) + 16;
            peep->destination_tolerence = 5;
            peep_update_current_action_sprite_type(peep);
            return;
        }

        if (peep->sprite_type == PEEP_SPRITE_TYPE_UMBRELLA) {
            peep_try_get_up_from_sitting(peep);
            return;
        }

        if (peep_has_food(peep)){
            if ((peep_rand() & 0xFFFF) > 1310){
                peep_try_get_up_from_sitting(peep);
                return;
            }
            peep->action = PEEP_ACTION_SITTING_EAT_FOOD;
            peep->action_frame = 0;
            peep->action_sprite_image_offset = 0;
            peep_update_current_action_sprite_type(peep);
            invalidate_sprite_2((rct_sprite*)peep);
            return;
        }

        sint32 rand = peep_rand();
        if ((rand & 0xFFFF) > 131){
            peep_try_get_up_from_sitting(peep);
            return;
        }
        if (peep->sprite_type == PEEP_SPRITE_TYPE_BALLOON || peep->sprite_type == PEEP_SPRITE_TYPE_HAT) {
            peep_try_get_up_from_sitting(peep);
            return;
        }

        peep->action = PEEP_ACTION_SITTING_LOOK_AROUND_LEFT;
        if (rand & 0x80000000){
            peep->action = PEEP_ACTION_SITTING_LOOK_AROUND_RIGHT;
        }

        if (rand & 0x40000000){
            peep->action = PEEP_ACTION_SITTING_CHECK_WATCH;
        }
        peep->action_frame = 0;
        peep->action_sprite_image_offset = 0;
        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }
}

/**
 *
 *  rct2: 0x006966A9
 */
void remove_peep_from_queue(rct_peep* peep)
{
    rct_ride* ride = get_ride(peep->current_ride);

    uint8 cur_station = peep->current_ride_station;
    ride->queue_length[cur_station]--;
    if (peep->sprite_index == ride->last_peep_in_queue[cur_station])
    {
        ride->last_peep_in_queue[cur_station] = peep->next_in_queue;
        return;
    }

    uint16 spriteId = ride->last_peep_in_queue[cur_station];
    while (spriteId != SPRITE_INDEX_NULL) {
        rct_peep* other_peep = GET_PEEP(spriteId);
        if (peep->sprite_index == other_peep->next_in_queue){
            other_peep->next_in_queue = peep->next_in_queue;
            return;
        }
        spriteId = other_peep->next_in_queue;
    }
}

/**
 *
 *  rct2: 0x00691C6E
 */
static rct_vehicle* peep_choose_car_from_ride(rct_peep* peep, rct_ride* ride, uint8* car_array, uint8 car_array_size){
    uint8 chosen_car = peep_rand();
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES)
        && ((chosen_car & 0xC) != 0xC)){
        chosen_car = (peep_rand() & 1) ? 0 : car_array_size - 1;
    }
    else{
        chosen_car = (chosen_car * (uint16)car_array_size) >> 8;
    }

    peep->current_car = car_array[chosen_car];

    rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);

    for (sint32 i = peep->current_car; i > 0; --i){
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    return vehicle;
}

/**
 *
 *  rct2: 0x00691CD1
 */
static void peep_choose_seat_from_car(rct_peep* peep, rct_ride* ride, rct_vehicle* vehicle){
    uint8 chosen_seat = vehicle->next_free_seat;

    if (ride->mode == RIDE_MODE_FORWARD_ROTATION ||
        ride->mode == RIDE_MODE_BACKWARD_ROTATION){

        chosen_seat = (((~vehicle->vehicle_sprite_type + 1) >> 3) & 0xF) * 2;
        if (vehicle->next_free_seat & 1){
            chosen_seat++;
        }
    }
    peep->current_seat = chosen_seat;
    vehicle->next_free_seat++;

    vehicle->peep[peep->current_seat] = peep->sprite_index;
    vehicle->peep_tshirt_colours[peep->current_seat] = peep->tshirt_colour;
}

/**
 *
 *  rct2: 0x00691D27
 */
static void peep_go_to_ride_entrance(rct_peep* peep, rct_ride* ride){
    sint32 x = ride->entrances[peep->current_ride_station] & 0xFF;
    sint32 y = ride->entrances[peep->current_ride_station] >> 8;
    sint32 z = ride->station_heights[peep->current_ride_station];

    rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

    uint8 direction = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    sint16 x_shift = word_981D6C[direction].x;
    sint16 y_shift = word_981D6C[direction].y;

    uint8 shift_multiplier = 21;
    rct_ride_entry* ride_type = get_ride_entry(ride->subtype);
    if (ride_type != NULL) {
        if (ride_type->vehicles[ride_type->default_vehicle].flags_a & VEHICLE_ENTRY_FLAG_A_MINI_GOLF ||
            ride_type->vehicles[ride_type->default_vehicle].flags_b & (VEHICLE_ENTRY_FLAG_B_12 | VEHICLE_ENTRY_FLAG_B_14)){
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x += x_shift;
    y += y_shift;

    peep->destination_x = x;
    peep->destination_y = y;
    peep->destination_tolerence = 2;

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_ENTERING_RIDE;
    peep->sub_state = 1;
    peep_window_state_update(peep);

    peep->var_AC = 0;
    peep->time_on_ride = 0;

    remove_peep_from_queue(peep);
}

/**
 *
 *  rct2: 0x00691A3B
 */
static void peep_update_ride_sub_state_0(rct_peep* peep){
    rct_ride* ride = get_ride(peep->current_ride);

    if (peep->destination_tolerence != 0){
        invalidate_sprite_2((rct_sprite*)peep);

        sint16 x, y, xy_distance;

        if (peep_update_action(&x, &y, &xy_distance, peep)){
            sint16 z = peep->z;
            if (xy_distance < 16){
                z = ride->station_heights[peep->current_ride_station] * 8 + 2;
            }
            sprite_move(x, y, z, (rct_sprite*)peep);
            invalidate_sprite_2((rct_sprite*)peep);
        }
        else{
            peep->destination_tolerence = 0;
            peep->sprite_direction ^= (1 << 4);
        }
    }

    uint8 car_array_size = 0xFF;
    uint8 car_array[255];

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES)){
        if (ride->num_riders >= ride->operation_option)
            return;
    }
    else{
        uint8 chosen_train = 0xFF;

        if (ride->mode == RIDE_MODE_BUMPERCAR || ride->mode == RIDE_MODE_RACE){
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
                return;

            for (sint32 i = 0; i < ride->num_vehicles; ++i){
                rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[i]);

                if (vehicle->next_free_seat >= vehicle->num_seats)
                    continue;

                if (vehicle->status != VEHICLE_STATUS_WAITING_FOR_PASSENGERS)
                    continue;
                chosen_train = i;
                break;
            }
        }
        else{
            chosen_train = ride->train_at_station[peep->current_ride_station];
        }
        if (chosen_train == 0xFF){
            return;
        }

        peep->current_train = chosen_train;
        uint8* car_array_pointer = car_array;

        sint32 i = 0;

        uint16 vehicle_id = ride->vehicles[chosen_train];
        rct_vehicle* vehicle = GET_VEHICLE(vehicle_id);

        for (; vehicle_id != SPRITE_INDEX_NULL;
            vehicle_id = vehicle->next_vehicle_on_train,
            i++){
            vehicle = GET_VEHICLE(vehicle_id);

            uint8 num_seats = vehicle->num_seats;
            if (vehicle_is_used_in_pairs(vehicle)){
                num_seats &= VEHICLE_SEAT_NUM_MASK;
                if (vehicle->next_free_seat & 1){
                    peep->current_car = i;
                    peep_choose_seat_from_car(peep, ride, vehicle);
                    peep_go_to_ride_entrance(peep, ride);
                    return;
                }
            }
            if (num_seats == vehicle->next_free_seat)
                continue;

            if (ride->mode == RIDE_MODE_FORWARD_ROTATION ||
                ride->mode == RIDE_MODE_BACKWARD_ROTATION)
            {
                uint8 position = (((~vehicle->vehicle_sprite_type + 1) >> 3) & 0xF) * 2;
                if (vehicle->peep[position] != SPRITE_INDEX_NULL)
                    continue;
            }

            *car_array_pointer++ = i;
        }

        car_array_size = (uint8)(car_array_pointer - car_array);

        if (car_array_size == 0)return;
    }

    if (ride->status != RIDE_STATUS_OPEN ||
        ride->vehicle_change_timeout != 0){
        if (peep->destination_tolerence == 0){
            remove_peep_from_queue(peep);
            peep_decrement_num_riders(peep);
            peep->state = PEEP_STATE_FALLING;
            peep_window_state_update(peep);
        }
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
        return;

    money16 ridePrice = ride_get_price(ride);
    if (ridePrice != 0) {
        if (!(peep->item_standard_flags & PEEP_ITEM_VOUCHER) ||
            !(peep->voucher_type == VOUCHER_TYPE_RIDE_FREE) ||
            !(peep->voucher_arguments == peep->current_ride)){

            if (peep->cash_in_pocket <= 0){
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_SPENT_MONEY, 0xFF);
                if (peep->destination_tolerence == 0){
                    remove_peep_from_queue(peep);
                    peep_decrement_num_riders(peep);
                    peep->state = PEEP_STATE_FALLING;
                    peep_window_state_update(peep);
                }
                return;
            }

            if (ridePrice > peep->cash_in_pocket){
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CANT_AFFORD_0, peep->current_ride);
                if (peep->destination_tolerence == 0){
                    remove_peep_from_queue(peep);
                    peep_decrement_num_riders(peep);
                    peep->state = PEEP_STATE_FALLING;
                    peep_window_state_update(peep);
                }
                return;
            }

            uint16 value = ride->value;
            if (value != RIDE_VALUE_UNDEFINED){
                if (value * 2 < ridePrice) {
                    peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_BAD_VALUE, peep->current_ride);
                    if (peep->destination_tolerence == 0){
                        remove_peep_from_queue(peep);
                        peep_decrement_num_riders(peep);
                        peep->state = PEEP_STATE_FALLING;
                        peep_window_state_update(peep);
                    }
                    return;
                }
            }
        }
    }

    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES)){
        rct_vehicle* vehicle = peep_choose_car_from_ride(peep, ride, car_array, car_array_size);
        peep_choose_seat_from_car(peep, ride, vehicle);
    }
    peep_go_to_ride_entrance(peep, ride);
}

/** rct2: 0x00981FD4, 0x00981FD6 */
static const rct_xy16 _981FD4[] = {
    {  8,  8 },
    {  8, 24 },
    { 24, 24 },
    { 24,  8 },
};

/**
 *
 *  rct2: 0x006921D3
 */
static void peep_update_ride_sub_state_1(rct_peep* peep){
    sint16 x, y, z, xy_distance;

    rct_ride* ride = get_ride(peep->current_ride);
    rct_ride_entry* ride_entry = get_ride_entry(ride->subtype);

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        uint16 distanceThreshold = 16;
        if (ride_entry != NULL) {
            uint8 vehicle = ride_entry->default_vehicle;
            if (ride_entry->vehicles[vehicle].flags_a & VEHICLE_ENTRY_FLAG_A_MINI_GOLF ||
                ride_entry->vehicles[vehicle].flags_b & (VEHICLE_ENTRY_FLAG_B_12 | VEHICLE_ENTRY_FLAG_B_14)) {
                distanceThreshold = 28;
            }
        }

        if (peep->sub_state == 1 && xy_distance < distanceThreshold) {
            peep->sub_state = 2;
        }

        invalidate_sprite_2((rct_sprite*)peep);

        z = ride->station_heights[peep->current_ride_station] * 8;

        distanceThreshold += 4;
        if (xy_distance < distanceThreshold) {
            z += RideData5[ride->type].z;
        }

        sprite_move(x, y, z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        x = ride->entrances[peep->current_ride_station] & 0xFF;
        y = ride->entrances[peep->current_ride_station] >> 8;
        z = ride->station_heights[peep->current_ride_station];

        rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

        uint8 direction_entrance = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

        if (ride->type == RIDE_TYPE_MAZE){
            peep->maze_last_edge = direction_entrance + 1;
            x *= 32;
            y *= 32;

            x += TileDirectionDelta[direction_entrance].x;
            y += TileDirectionDelta[direction_entrance].y;

            uint8 direction = direction_entrance * 4 + 11;
            if (peep_rand() & 0x40){
                direction += 4;
                peep->maze_last_edge += 2;
            }

            direction &= 0xF;
            // Direction is 11, 15, 3, or 7
            peep->var_37 = direction;
            peep->maze_last_edge &= 3;

            x += _981FD4[direction / 4].x;
            y += _981FD4[direction / 4].y;

            peep->destination_x = x;
            peep->destination_y = y;
            peep->destination_tolerence = 3;

            ride->cur_num_customers++;
            peep_on_enter_or_exit_ride(peep, peep->current_ride, 0);
            peep->sub_state = 17;
            return;
        }

        x = ride->station_starts[peep->current_ride_station] & 0xFF;
        y = ride->station_starts[peep->current_ride_station] >> 8;

        map_element = ride_get_station_start_track_element(ride, peep->current_ride_station);

        uint8 direction_track = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

        peep->var_37 = (direction_entrance << 2) | (direction_track << 4);

        x *= 32;
        y *= 32;

        assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
        const rct_xy16 slidePlatformDestination  = SpiralSlideWalkingPath[peep->var_37];

        x += slidePlatformDestination.x;
        y += slidePlatformDestination.y;

        peep->destination_x = x;
        peep->destination_y = y;
        peep->current_car = 0;

        ride->cur_num_customers++;
        peep_on_enter_or_exit_ride(peep, peep->current_ride, 0);
        peep->sub_state = 14;
        return;
    }

    rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
    for (sint32 i = peep->current_car; i != 0; --i){
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    ride_entry = get_ride_entry(vehicle->ride_subtype);
    rct_ride_entry_vehicle* vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

    if (vehicle_type->flags_b & VEHICLE_ENTRY_FLAG_B_10){
        x = ride->entrances[peep->current_ride_station] & 0xFF;
        y = ride->entrances[peep->current_ride_station] >> 8;
        z = ride->station_heights[peep->current_ride_station];

        rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

        uint8 direction_entrance = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

        x = ride->station_starts[peep->current_ride_station] & 0xFF;
        y = ride->station_starts[peep->current_ride_station] >> 8;

        map_element = ride_get_station_start_track_element(ride, peep->current_ride_station);

        uint8 direction_track = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

        vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
        ride_entry = get_ride_entry(vehicle->ride_subtype);
        vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

        uint8 cl = peep->current_seat;
        uint8 ch = peep->current_seat & 0xF8;

        if (ride->type != RIDE_TYPE_ENTERPRISE)
            direction_track *= 2;

        if (*vehicle_type->peep_loading_positions == 0){
            direction_track /= 2;
            cl = 0;
            ch = 0;
        }
        cl += direction_track;
        cl &= 0x7;
        cl += ch;
        peep->var_37 = (direction_entrance | cl * 4) * 4;

        x *= 32;
        y *= 32;
        x += 16;
        y += 16;

        if (ride->type == RIDE_TYPE_ENTERPRISE)
        {
            x = vehicle->x;
            y = vehicle->y;
        }

        x += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 1];
        y += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 2];

        peep->destination_x = x;
        peep->destination_y = y;
        peep->sub_state = 12;
        return;
    }

    if (vehicle_type->flags_b & VEHICLE_ENTRY_FLAG_B_15){
        peep->destination_x = vehicle->x;
        peep->destination_y = vehicle->y;
        peep->destination_tolerence = 15;
        peep->sub_state = 4;
        return;
    }

#ifdef NO_RCT2
    assert(peep->current_seat < vehicle_type->peep_loading_positions_count);
#endif
    sint8 load_position = vehicle_type->peep_loading_positions[peep->current_seat];

    switch (vehicle->sprite_direction / 8){
    case 0:
        peep->destination_x = vehicle->x - load_position;
        break;
    case 1:
        peep->destination_y = vehicle->y + load_position;
        break;
    case 2:
        peep->destination_x = vehicle->x + load_position;
        break;
    case 3:
        peep->destination_y = vehicle->y - load_position;
        break;
    }

    peep->sub_state = 4;
    return;
}

/**
 *
 *  rct2: 0x0069321D
 */
static void peep_go_to_ride_exit(rct_peep* peep, rct_ride* ride, sint16 x, sint16 y, sint16 z, uint8 exit_direction){
    z += RideData5[ride->type].z;

    sprite_move(x, y, z, (rct_sprite*)peep);
    invalidate_sprite_2((rct_sprite*)peep);

    assert(peep->current_ride_station < 4);
    x = ride->exits[peep->current_ride_station] & 0xFF;
    y = ride->exits[peep->current_ride_station] >> 8;
    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    sint16 x_shift = word_981D6C[exit_direction].x;
    sint16 y_shift = word_981D6C[exit_direction].y;

    sint16 shift_multiplier = 20;

    rct_ride_entry* ride_type = get_ride_entry(ride->subtype);
    if (ride_type != NULL) {
        rct_ride_entry_vehicle* vehicle_entry = &ride_type->vehicles[ride_type->default_vehicle];
        if (vehicle_entry->flags_a & VEHICLE_ENTRY_FLAG_A_MINI_GOLF ||
            vehicle_entry->flags_b & (VEHICLE_ENTRY_FLAG_B_12 | VEHICLE_ENTRY_FLAG_B_14)){
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    peep->destination_x = x;
    peep->destination_y = y;
    peep->destination_tolerence = 2;

    peep->sprite_direction = exit_direction * 8;
    peep->sub_state = 8;
    return;
}

/**
 *
 *  rct2: 0x006920B4
 */
static void peep_update_ride_sub_state_2_enter_ride(rct_peep* peep, rct_ride* ride)
{
    money16 ridePrice = ride_get_price(ride);
    if (ridePrice != 0) {
        if ((peep->item_standard_flags & PEEP_ITEM_VOUCHER) &&
            (peep->voucher_type == VOUCHER_TYPE_RIDE_FREE) &&
            (peep->voucher_arguments == peep->current_ride)){

            peep->item_standard_flags &= ~PEEP_ITEM_VOUCHER;
            peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
        }
        else{
            ride->total_profit += ridePrice;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
            gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS;
            peep_spend_money(peep, &peep->paid_on_rides, ridePrice);
        }
    }

    peep->sub_state++;
    uint8 queue_time = peep->days_in_queue;
    if (queue_time < 253)queue_time += 3;

    queue_time /= 2;
    if (queue_time != ride->queue_time[peep->current_ride_station]){
        ride->queue_time[peep->current_ride_station] = queue_time;
        window_invalidate_by_number(WC_RIDE, peep->current_ride);
    }

    if (peep->peep_flags & PEEP_FLAGS_TRACKING){
        set_format_arg(0, rct_string_id, peep->name_string_idx);
        set_format_arg(2, uint32, peep->id);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);

        rct_string_id msg_string;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE))
            msg_string = STR_PEEP_TRACKING_PEEP_IS_IN_X;
        else
            msg_string = STR_PEEP_TRACKING_PEEP_IS_ON_X;

        if (gConfigNotifications.guest_on_ride) {
            news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, msg_string, peep->sprite_index);
        }
    }

    if (ride->type == RIDE_TYPE_SPIRAL_SLIDE){
        peep_switch_to_special_sprite(peep, 1);
    }

    peep_update_ride_sub_state_1(peep);
}

/**
 *
 *  rct2: 0x00691FD4
 */
static void peep_update_ride_sub_state_2_rejoin_queue(rct_peep* peep, rct_ride* ride){
    sint16 x, y, z;
    x = ride->entrances[peep->current_ride_station] & 0xFF;
    y = ride->entrances[peep->current_ride_station] >> 8;
    z = ride->station_heights[peep->current_ride_station];

    rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

    uint8 direction_entrance = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

    x *= 32;
    y *= 32;
    x += 16 - word_981D6C[direction_entrance].x * 20;
    y += 16 - word_981D6C[direction_entrance].y * 20;

    peep->destination_x = x;
    peep->destination_y = y;
    peep->destination_tolerence = 2;

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_QUEUING_FRONT;
    peep->sub_state = 0;
    peep_window_state_update(peep);

    peep->next_in_queue = SPRITE_INDEX_NULL;

    ride->queue_length[peep->current_ride_station]++;

    uint16 current_last = ride->last_peep_in_queue[peep->current_ride_station];
    if (current_last == SPRITE_INDEX_NULL) {
        ride->last_peep_in_queue[peep->current_ride_station] = peep->sprite_index;
        return;
    }

    rct_peep* queue_peep;
    for (queue_peep = GET_PEEP(current_last);
        queue_peep->next_in_queue != SPRITE_INDEX_NULL;
        queue_peep = GET_PEEP(queue_peep->next_in_queue));

    queue_peep->next_in_queue = peep->sprite_index;
}
/**
 *
 *  rct2: 0x00691E42
 * Note: Before this was the entry
 * point for sub state 1 and 3. The
 * check has been removed that would
 * branch it out to 1 and 3. Now uses
 * separate functions.
 */
static void peep_update_ride_sub_state_2(rct_peep* peep){
    rct_ride* ride = get_ride(peep->current_ride);

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES)){
        if (ride->status != RIDE_STATUS_OPEN ||
            ride->vehicle_change_timeout != 0 ||
            (++peep->var_AC) == 0){

            peep_update_ride_sub_state_2_rejoin_queue(peep, ride);
            return;
        }

        peep_update_ride_sub_state_2_enter_ride(peep, ride);
        return;
    }

    rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
    for (sint32 i = peep->current_car; i != 0; --i){
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    rct_ride_entry* ride_entry = get_ride_entry(vehicle->ride_subtype);

    if (ride_entry->vehicles[0].flags_a & VEHICLE_ENTRY_FLAG_A_MINI_GOLF){
        vehicle->mini_golf_flags &= ~(1 << 5);


        for (size_t i = 0; i < ride->num_vehicles; ++i){
            if (ride->vehicles[i] == SPRITE_INDEX_NULL)
                continue;

            rct_vehicle* train = GET_VEHICLE(ride->vehicles[i]);
            rct_vehicle* second_vehicle = GET_VEHICLE(train->next_vehicle_on_train);

            if (second_vehicle->num_peeps == 0)
                continue;

            if (second_vehicle->mini_golf_flags & (1 << 5))
                continue;

            return;
        }
    }

    if (!vehicle_is_used_in_pairs(vehicle)){
        peep_update_ride_sub_state_2_enter_ride(peep, ride);
        return;
    }

    if (ride->mode == RIDE_MODE_FORWARD_ROTATION ||
        ride->mode == RIDE_MODE_BACKWARD_ROTATION){
        if (peep->current_seat & 1 ||
            !(vehicle->next_free_seat & 1)){
            peep_update_ride_sub_state_2_enter_ride(peep, ride);
            return;
        }
    }
    else{
        uint8 current_seat = (peep->current_seat & 0xFE) + 1;
        if (current_seat < vehicle->next_free_seat)
        {
            peep_update_ride_sub_state_2_enter_ride(peep, ride);
            return;
        }
    }

    rct_vehicle *currentTrain = GET_VEHICLE(ride->vehicles[peep->current_train]);
    if (ride->status == RIDE_STATUS_OPEN &&
        ++peep->var_AC != 0 &&
        !(currentTrain->update_flags & VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART)){
        return;
    }

    if (ride->mode != RIDE_MODE_FORWARD_ROTATION &&
        ride->mode != RIDE_MODE_BACKWARD_ROTATION){
        if (vehicle->next_free_seat - 1 != peep->current_seat)
            return;
    }

    vehicle->next_free_seat--;
    vehicle->peep[peep->current_seat] = SPRITE_INDEX_NULL;

    peep_update_ride_sub_state_2_rejoin_queue(peep, ride);
}

static void peep_update_ride_sub_state_5(rct_peep* peep){
    rct_ride* ride = get_ride(peep->current_ride);

    rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
    for (sint32 i = peep->current_car; i != 0; --i){
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    if (ride->mode != RIDE_MODE_FORWARD_ROTATION &&
        ride->mode != RIDE_MODE_BACKWARD_ROTATION){
        if (peep->current_seat != vehicle->num_peeps)
            return;
    }

    if (vehicle_is_used_in_pairs(vehicle)){
        rct_peep* seated_peep = GET_PEEP(vehicle->peep[peep->current_seat ^ 1]);
        if (seated_peep->sub_state != 5)
            return;

        vehicle->num_peeps++;
        ride->cur_num_customers++;

        vehicle->friction += seated_peep->var_41;
        invalidate_sprite_2((rct_sprite*)seated_peep);
        sprite_move(0x8000, 0, 0, (rct_sprite*)seated_peep);

        peep_decrement_num_riders(seated_peep);
        seated_peep->state = PEEP_STATE_ON_RIDE;
        peep_window_state_update(seated_peep);
        seated_peep->time_on_ride = 0;
        seated_peep->sub_state = 6;
        peep_on_enter_or_exit_ride(seated_peep, peep->current_ride, 0);
    }

    vehicle->num_peeps++;
    ride->cur_num_customers++;

    vehicle->friction += peep->var_41;
    invalidate_sprite_2((rct_sprite*)vehicle);

    invalidate_sprite_2((rct_sprite*)peep);
    sprite_move(0x8000, 0, 0, (rct_sprite*)peep);

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_ON_RIDE;
    peep_window_state_update(peep);

    peep->time_on_ride = 0;
    peep->sub_state = 6;

    peep_on_enter_or_exit_ride(peep, peep->current_ride, 0);
}

/**
 *
 *  rct2: 0x00693028
 */
static void peep_update_ride_sub_state_7(rct_peep* peep){
    rct_ride* ride = get_ride(peep->current_ride);

    rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
    uint8 ride_station = vehicle->current_station;

    for (sint32 i = peep->current_car; i != 0; --i){
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    // Check if ride is NOT Ferris Wheel.
    if (ride->mode != RIDE_MODE_FORWARD_ROTATION &&
            ride->mode != RIDE_MODE_BACKWARD_ROTATION){
        if (vehicle->num_peeps - 1 != peep->current_seat)
            return;
    }

    peep->action_sprite_image_offset++;
    if (peep->action_sprite_image_offset & 3)
        return;

    peep->action_sprite_image_offset = 0;

    vehicle->num_peeps--;
    vehicle->friction -= peep->var_41;
    invalidate_sprite_2((rct_sprite*)vehicle);

    peep->current_ride_station = ride_station;

    rct_ride_entry* ride_entry = get_ride_entry(vehicle->ride_subtype);
    rct_ride_entry_vehicle* vehicle_entry = &ride_entry->vehicles[vehicle->vehicle_type];

    if (!(vehicle_entry->flags_b & VEHICLE_ENTRY_FLAG_B_10)){
        sint16 x, y, z;
        assert(peep->current_ride_station < 4);
        x = ride->exits[peep->current_ride_station] & 0xFF;
        y = ride->exits[peep->current_ride_station] >> 8;
        z = ride->station_heights[peep->current_ride_station];

        rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

        uint8 exit_direction = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);
        exit_direction ^= (1 << 1);

        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_16)){

            for (; vehicle->is_child; vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride)){
                uint16 trackType = vehicle->track_type >> 2;
                if (trackType == TRACK_ELEM_FLAT || trackType > TRACK_ELEM_MIDDLE_STATION)
                    continue;

                rct_map_element* inner_map = map_get_first_element_at(vehicle->track_x / 32, vehicle->track_y / 32);
                for (;; inner_map++){
                    if (map_element_get_type(inner_map) != MAP_ELEMENT_TYPE_TRACK)
                        continue;
                    if (inner_map->base_height == vehicle->track_z / 8)
                        break;
                }

                uint8 al = (inner_map->properties.track.sequence & 0x70) >> 4;
                if (al == peep->current_ride_station)
                    break;
            }

            uint8 shift_multiplier = 12;
            uint8 direction = exit_direction;

            ride_entry = get_ride_entry(ride->subtype);

            if (ride_entry != NULL) {
                vehicle_entry = &ride_entry->vehicles[ride_entry->default_vehicle];

                if (vehicle_entry->flags_b & VEHICLE_ENTRY_FLAG_B_14){
                    shift_multiplier = 9;
                }

                if (vehicle_entry->flags_b & (VEHICLE_ENTRY_FLAG_B_12 | VEHICLE_ENTRY_FLAG_B_14)){
                    direction = ((vehicle->sprite_direction + 3) / 8) + 1;
                    direction &= 3;

                    if (vehicle->var_CD == 6)
                        direction ^= (1 << 1);
                }
            }


            sint16 x_shift = word_981D6C[direction].x;
            sint16 y_shift = word_981D6C[direction].y;

            x = vehicle->x + x_shift * shift_multiplier;
            y = vehicle->y + y_shift * shift_multiplier;
            z *= 8;

            peep_go_to_ride_exit(peep, ride, x, y, z, exit_direction);
            return;
        }

        x = vehicle->x + word_981D6C[exit_direction].x * 12;
        y = vehicle->y + word_981D6C[exit_direction].y * 12;

        sint8 load_position = vehicle_entry->peep_loading_positions[peep->current_seat];

        switch (vehicle->sprite_direction / 8){
        case 0:
            x -= load_position;
            break;
        case 1:
            y += load_position;
            break;
        case 2:
            x += load_position;
            break;
        case 3:
            y -= load_position;
            break;
        }

        z = ride->station_heights[peep->current_ride_station] * 8;

        peep_go_to_ride_exit(peep, ride, x, y, z, exit_direction);
        return;
    }

    sint16 x, y, z;
    x = ride->exits[peep->current_ride_station] & 0xFF;
    y = ride->exits[peep->current_ride_station] >> 8;
    z = ride->station_heights[peep->current_ride_station];

    rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

    uint8 exit_direction = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

    x = ride->station_starts[peep->current_ride_station] & 0xFF;
    y = ride->station_starts[peep->current_ride_station] >> 8;

    map_element = ride_get_station_start_track_element(ride, peep->current_ride_station);

    uint8 station_direction = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

    vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);

    ride_entry = get_ride_entry(vehicle->ride_subtype);
    rct_ride_entry_vehicle* vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

    uint8 cl = peep->current_seat;
    uint8 ch = peep->current_seat & 0xF8;

    if (ride->type != RIDE_TYPE_ENTERPRISE)
        station_direction *= 2;

    if (*vehicle_type->peep_loading_positions == 0){
        station_direction /= 2;
        cl = 0;
        ch = 0;
    }
    cl += station_direction;
    cl &= 0x7;
    cl += ch;
    peep->var_37 = ((exit_direction | cl * 4) * 4) | 1;

    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    if (ride->type == RIDE_TYPE_ENTERPRISE)
    {
        x = vehicle->x;
        y = vehicle->y;
    }

    sint16 exit_x = x + vehicle_type->peep_loading_positions[(peep->var_37 + 1) * 2 + 1];
    sint16 exit_y = y + vehicle_type->peep_loading_positions[(peep->var_37 + 1) * 2 + 2];

    z *= 8;
    z += RideData5[ride->type].z;

    if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        z += 15;

    sprite_move(exit_x, exit_y, z, (rct_sprite*)peep);
    invalidate_sprite_2((rct_sprite*)peep);

    x += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 1];
    y += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 2];

    peep->destination_x = x;
    peep->destination_y = y;
    peep->destination_tolerence = 2;
    peep->sub_state = 13;
}

/**
 *
 *  rct2: 0x0069376A
 */
static void peep_update_ride_prepare_for_state_9(rct_peep* peep){
    rct_ride* ride = get_ride(peep->current_ride);

    sint16 x = ride->exits[peep->current_ride_station] & 0xFF;
    sint16 y = ride->exits[peep->current_ride_station] >> 8;
    sint16 z = ride->station_heights[peep->current_ride_station];

    rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

    uint8 exit_direction = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    sint16 x_shift = word_981D6C[exit_direction].x;
    sint16 y_shift = word_981D6C[exit_direction].y;

    sint16 shift_multiplier = 20;

    rct_ride_entry* ride_type = get_ride_entry(ride->subtype);
    if (ride_type != NULL) {
        rct_ride_entry_vehicle* vehicle_entry = &ride_type->vehicles[ride_type->default_vehicle];
        if (vehicle_entry->flags_b & (VEHICLE_ENTRY_FLAG_B_12 | VEHICLE_ENTRY_FLAG_B_14)){
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    peep->destination_x = x;
    peep->destination_y = y;
    peep->destination_tolerence = 2;
    peep->sub_state = 9;
}

/**
 *
 *  rct2: 0x0069374F
 */
static void peep_update_ride_sub_state_8(rct_peep* peep){
    sint16 x, y, xy_distance;
    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    peep_update_ride_prepare_for_state_9(peep);
}

/**
 *
 *  rct2: 0x0069382E
 */
static void peep_update_ride_sub_state_9(rct_peep* peep){
    sint16 x, y, xy_distance;
    rct_ride* ride = get_ride(peep->current_ride);

    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);

        if (xy_distance >= 16){
            sint16 z = ride->station_heights[peep->current_ride_station] * 8;

            z += RideData5[ride->type].z;
            sprite_move(x, y, z, (rct_sprite*)peep);
            invalidate_sprite_2((rct_sprite*)peep);
            return;
        }

        peep_switch_to_special_sprite(peep, 0);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO){
        uint8 secondaryItem = RidePhotoItems[ride->type];
        if (peep_decide_and_buy_item(peep, peep->current_ride, secondaryItem, ride->price_secondary)) {
            ride->no_secondary_items_sold++;
        }
    }
    peep->sub_state = 18;
}

/**
 *
 *  rct2: 0x006926AD
 */
static void peep_update_ride_sub_state_12(rct_peep* peep){
    sint16 x, y, xy_distance;
    rct_ride* ride = get_ride(peep->current_ride);

    if (peep_update_action(&x, &y, &xy_distance, peep)){
        sint16 z;
        if (ride->type == RIDE_TYPE_MOTION_SIMULATOR){
            z = ride->station_heights[peep->current_ride_station] * 8 + 2;

            if ((peep->var_37 & 3) == 2){
                xy_distance -= 12;
                if (xy_distance < 0)
                    xy_distance = 0;

                if (xy_distance <= 15){
                    z += 15 - xy_distance;
                }
            }
        }
        else{
            z = peep->z;
        }
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    if ((peep->var_37 & 3) == 2){
        peep->sub_state = 5;
        return;
    }

    peep->var_37++;

    rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);

    x = ride->station_starts[peep->current_ride_station] & 0xFF;
    y = ride->station_starts[peep->current_ride_station] >> 8;

    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    if (ride->type == RIDE_TYPE_ENTERPRISE){
        x = vehicle->x;
        y = vehicle->y;
    }

    rct_ride_entry* ride_entry = get_ride_entry(vehicle->ride_subtype);
    rct_ride_entry_vehicle* vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

    x += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 1];
    y += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 2];

    peep->destination_x = x;
    peep->destination_y = y;
}

/**
 *
 *  rct2: 0x0069357D
 */
static void peep_update_ride_sub_state_13(rct_peep* peep){
    sint16 x, y, xy_distance;
    rct_ride* ride = get_ride(peep->current_ride);

    if (peep_update_action(&x, &y, &xy_distance, peep)){
        sint16 z;
        if (ride->type == RIDE_TYPE_MOTION_SIMULATOR){
            z = ride->station_heights[peep->current_ride_station] * 8 + 2;

            if ((peep->var_37 & 3) == 1){

                if (xy_distance > 15)
                    xy_distance = 15;

                z += xy_distance;
            }
        }
        else{
            z = peep->z;
        }
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    if ((peep->var_37 & 3) != 0){
        if ((peep->var_37 & 3) == 3){
            peep_update_ride_prepare_for_state_9(peep);
            return;
        }

        peep->var_37--;
        rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);

        x = ride->station_starts[peep->current_ride_station] & 0xFF;
        y = ride->station_starts[peep->current_ride_station] >> 8;

        x *= 32;
        y *= 32;
        x += 16;
        y += 16;

        if (ride->type == RIDE_TYPE_ENTERPRISE){
            x = vehicle->x;
            y = vehicle->y;
        }

        rct_ride_entry* ride_entry = get_ride_entry(vehicle->ride_subtype);
        rct_ride_entry_vehicle* vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

        x += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 1];
        y += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 2];

        peep->destination_x = x;
        peep->destination_y = y;
        return;
    }

    peep->var_37 |= 3;

    x = ride->exits[peep->current_ride_station] & 0xFF;
    y = ride->exits[peep->current_ride_station] >> 8;
    sint16 z = ride->station_heights[peep->current_ride_station];

    rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

    uint8 exit_direction = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);
    exit_direction ^= (1 << 1);

    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    sint16 x_shift = word_981D6C[exit_direction].x;
    sint16 y_shift = word_981D6C[exit_direction].y;

    sint16 shift_multiplier = 20;

    rct_ride_entry* ride_type = get_ride_entry(ride->subtype);
    rct_ride_entry_vehicle* vehicle_entry = &ride_type->vehicles[ride_type->default_vehicle];
    if (vehicle_entry->flags_b & (VEHICLE_ENTRY_FLAG_B_12 | VEHICLE_ENTRY_FLAG_B_14)){
        shift_multiplier = 32;
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    peep->destination_x = x;
    peep->destination_y = y;
}

/**
 *
 *  rct2: 0x006927B3
 */
static void peep_update_ride_sub_state_14(rct_peep* peep){
    sint16 x, y, xy_distance;
    rct_ride* ride = get_ride(peep->current_ride);

    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    if ((peep->var_37 & 3) == 3){
        peep->sub_state = 15;
        peep->destination_x = 0;
        peep->destination_y = 0;
        peep->var_37 = (peep->var_37 / 4) & 0xC;
        sprite_move(0x8000, y, peep->z, (rct_sprite*)peep);
        return;
    }
    else if ((peep->var_37 & 3) == 2){
        uint8 last_ride = 0;
        if (ride->status != RIDE_STATUS_OPEN)
            last_ride = 1;
        else if (peep->current_car++ != 0){
            if (ride->mode == RIDE_MODE_SINGLE_RIDE_PER_ADMISSION)
                last_ride = 1;
            if ((uint8)(peep->current_car - 1) > (peep_rand() & 0xF))
                last_ride = 1;
        }

        if (last_ride){
            x = ride->exits[peep->current_ride_station] & 0xFF;
            y = ride->exits[peep->current_ride_station] >> 8;
            sint16 z = ride->station_heights[peep->current_ride_station];

            rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

            uint8 exit_direction = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

            peep->var_37 = (exit_direction * 4) | (peep->var_37 & 0x30) |  1;
            x = ride->station_starts[peep->current_ride_station] & 0xFF;
            y = ride->station_starts[peep->current_ride_station] >> 8;

            x *= 32;
            y *= 32;
            assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
            const rct_xy16 slidePlatformDestination  = SpiralSlideWalkingPath[peep->var_37];

            x += slidePlatformDestination.x;
            y += slidePlatformDestination.y;

            peep->destination_x = x;
            peep->destination_y = y;
            peep->sub_state = 16;
            return;
        }
    }
    peep->var_37++;

    x = ride->station_starts[peep->current_ride_station] & 0xFF;
    y = ride->station_starts[peep->current_ride_station] >> 8;

    x *= 32;
    y *= 32;

    assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
    const rct_xy16 slidePlatformDestination  = SpiralSlideWalkingPath[peep->var_37];

    x += slidePlatformDestination.x;
    y += slidePlatformDestination.y;

    peep->destination_x = x;
    peep->destination_y = y;
}

/** rct2: 0x00981F0C, 0x00981F0E */
static const rct_xy16 _981F0C[] = {
    {25,  56},
    {56,  7},
    {7,   -24},
    {-24, 25},
};

/** rct2: 0x00981F1C, 0x00981F1E */
static const rct_xy16 _981F1C[] = {
    {8,   56},
    {56,  24},
    {24,  -24},
    {-24, 8},
};

/**
 *
 *  rct2: 0x00692D83
 */
static void peep_update_ride_sub_state_15(rct_peep* peep){
    rct_ride* ride = get_ride(peep->current_ride);

    if (ride->type != RIDE_TYPE_SPIRAL_SLIDE)
        return;

    if ((peep->var_37 & 3) == 0){
        switch (peep->destination_x){
        case 0:
            peep->destination_y++;
            if (peep->destination_y >= 30)
                peep->destination_x++;
            return;
        case 1:
            if (ride->slide_in_use != 0)
                return;

            ride->slide_in_use++;
            ride->slide_peep = peep->sprite_index;
            ride->slide_peep_t_shirt_colour = peep->tshirt_colour;
            ride->spiral_slide_progress = 0;
            peep->destination_x++;
            return;
        case 2:
            return;
        case 3:
        {
            sint16 x = ride->station_starts[peep->current_ride_station] & 0xFF;
            sint16 y = ride->station_starts[peep->current_ride_station] >> 8;

            x *= 32;
            y *= 32;

            uint8 direction = (peep->var_37 / 4) & 3;
            sint16 dest_x = x + _981F1C[direction].x;
            sint16 dest_y = y + _981F1C[direction].y;

            peep->destination_x = dest_x;
            peep->destination_y = dest_y;

            x += _981F0C[direction].x;
            y += _981F0C[direction].y;

            sprite_move(x, y, peep->z, (rct_sprite*)peep);

            peep->sprite_direction = (peep->var_37 & 0xC) * 2;

            invalidate_sprite_2((rct_sprite*)peep);

            peep->var_37++;
            return;
        }
        default:
            return;
        }
    }

    sint16 x, y, xy_distance;

    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    peep->var_37 = (peep->var_37 * 4 & 0x30) + 2;

    x = ride->station_starts[peep->current_ride_station] & 0xFF;
    y = ride->station_starts[peep->current_ride_station] >> 8;

    x *= 32;
    y *= 32;

    assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
    const rct_xy16 slidePlatformDestination  = SpiralSlideWalkingPath[peep->var_37];

    x += slidePlatformDestination.x;
    y += slidePlatformDestination.y;

    peep->destination_x = x;
    peep->destination_y = y;
    peep->sub_state = 14;
}

/**
 *
 *  rct2: 0x00692C6B
 */
static void peep_update_ride_sub_state_16(rct_peep* peep){
    sint16 x, y, xy_distance;

    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    rct_ride* ride = get_ride(peep->current_ride);

    if ((peep->var_37 & 0x3) != 0){
        if ((peep->var_37 & 0x3) == 3){
            peep_update_ride_prepare_for_state_9(peep);
            return;
        }

        peep->var_37--;
        x = ride->station_starts[peep->current_ride_station] & 0xFF;
        y = ride->station_starts[peep->current_ride_station] >> 8;

        x *= 32;
        y *= 32;

        assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
        const rct_xy16 slidePlatformDestination  = SpiralSlideWalkingPath[peep->var_37];

        x += slidePlatformDestination.x;
        y += slidePlatformDestination.y;

        peep->destination_x = x;
        peep->destination_y = y;
        return;
    }

    peep->var_37 |= 3;

    x = ride->exits[peep->current_ride_station] & 0xFF;
    y = ride->exits[peep->current_ride_station] >> 8;
    sint16 z = ride->station_heights[peep->current_ride_station];

    rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

    uint8 exit_direction = (map_element == NULL ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);
    exit_direction ^= (1 << 1);

    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    sint16 x_shift = word_981D6C[exit_direction].x;
    sint16 y_shift = word_981D6C[exit_direction].y;

    sint16 shift_multiplier = 20;

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    peep->destination_x = x;
    peep->destination_y = y;
}

/** rct2: 0x00981FE4 */
static const uint8 _981FE4[][4] = {
    { 15,  7, 15,  7 },
    { 11,  3, 11,  3 },
    {  7, 15,  7, 15 },
    {  3, 11,  3, 11 },
};

/** rct2: 0x00981FF4 */
static const uint8 _981FF4[][4] = {
    {  1,  2, 14,  0 },
    {  4,  5,  6,  2 },
    {  6,  8,  9, 10 },
    { 14, 10, 12, 13 },
};

/**
 *
 *  rct2: 0x00692A83
 */
static void peep_update_ride_sub_state_17(rct_peep* peep){
    sint16 x, y, xy_distance;

    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    rct_ride* ride = get_ride(peep->current_ride);
    if (peep->var_37 == 16){
        peep_update_ride_prepare_for_state_9(peep);
        return;
    }

    if (peep->action >= PEEP_ACTION_NONE_1){
        if (peep->energy > 64 &&
            (peep_rand() & 0xFFFF) <= 2427){

            peep->action = PEEP_ACTION_JUMP;
            peep->action_frame = 0;
            peep->action_sprite_image_offset = 0;
            peep_update_current_action_sprite_type(peep);
            invalidate_sprite_2((rct_sprite*)peep);
        }
    }

    x = peep->destination_x & 0xFFE0;
    y = peep->destination_y & 0xFFE0;
    sint16 z = ride->station_heights[0];

    // Find the station track element
    rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);
    do {
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK && z == mapElement->base_height)
            break;

    } while (!map_element_is_last_for_tile(mapElement++));

    uint16 maze_entry = mapElement->properties.track.maze_entry;
    uint16 open_hedges = 0;
    uint8 var_37 = peep->var_37;
    // var_37 is 3, 7, 11 or 15

    if (maze_entry & (1 << _981FF4[var_37 / 4][3])) {
        open_hedges = 1;
    }
    open_hedges <<= 1;
    if (maze_entry & (1 << _981FF4[var_37 / 4][2])) {
        open_hedges |= 1;
    }
    open_hedges <<= 1;
    if (maze_entry & (1 << _981FF4[var_37 / 4][1])) {
        open_hedges |= 1;
    }
    open_hedges <<= 1;
    if (maze_entry & (1 << _981FF4[var_37 / 4][0])) {
        open_hedges |= 1;
    }

    open_hedges ^= 0xF;
    if (open_hedges == 0)
        return;

    uint8 maze_last_edge = peep->maze_last_edge ^ (1 << 1);
    open_hedges &= ~(1 << maze_last_edge);
    if (open_hedges == 0)
        open_hedges |= (1 << maze_last_edge);

    uint8 chosen_edge = peep_rand() & 0x3;
    while (!(open_hedges & (1 << chosen_edge))){
        chosen_edge = (chosen_edge + 1) & 3;
    }

    x = TileDirectionDelta[chosen_edge].x / 2;
    y = TileDirectionDelta[chosen_edge].y / 2;

    x += peep->destination_x;
    y += peep->destination_y;

    uint8 type = 0;

    mapElement = map_get_first_element_at(x / 32, y / 32);
    do {
        if (z != mapElement->base_height)
            continue;

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK){
            type = 1;
            break;
        }

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE &&
            mapElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_EXIT){
            type = 2;
            break;
        }
    } while (!map_element_is_last_for_tile(mapElement++));

    switch (type){
    case 0:
        peep->maze_last_edge++;
        peep->maze_last_edge &= 3;
        return;
    case 1:
        peep->destination_x = x;
        peep->destination_y = y;

        peep->var_37 = _981FE4[peep->var_37 / 4][chosen_edge];
        peep->maze_last_edge = chosen_edge;
        break;
    case 2:
        x = peep->destination_x;
        y = peep->destination_y;
        if (chosen_edge & 1){
            x &= 0xFFE0;
            x += 16;
        }
        else{
            y &= 0xFFE0;
            y += 16;
        }
        peep->destination_x = x;
        peep->destination_y = y;
        peep->var_37 = 16;
        peep->maze_last_edge = chosen_edge;
        break;
    }

    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }
}

/**
 *
 *  rct2: 0x006938D2
 */
static void peep_update_ride_sub_state_18(rct_peep* peep){
    sint16 x, y, xy_distance;
    rct_ride* ride = get_ride(peep->current_ride);

    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, ride->station_heights[peep->current_ride_station] * 8, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    peep_on_enter_or_exit_ride(peep, peep->current_ride, 1);

    if (peep->peep_flags & PEEP_FLAGS_TRACKING){
        set_format_arg(0, rct_string_id, peep->name_string_idx);
        set_format_arg(2, uint32, peep->id);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);

        if (gConfigNotifications.guest_left_ride) {
            news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_LEFT_RIDE_X, peep->sprite_index);
        }
    }

    peep->interaction_ride_index = 0xFF;
    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_FALLING;
    peep_window_state_update(peep);

    x = peep->x & 0xFFE0;
    y = peep->y & 0xFFE0;

    // Find the station track element
    rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);
    do {
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
            continue;

        sint16 z = map_height_from_slope(peep->x, peep->y, mapElement->properties.path.type);
        z += mapElement->base_height * 8;

        sint16 z_diff = peep->z - z;
        if (z_diff > 0 || z_diff < -16)
            continue;

        sprite_move(peep->x, peep->y, z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    } while (!map_element_is_last_for_tile(mapElement++));
}

/**
 *
 *  rct2: 0x0069299C
 */
static void peep_update_ride_sub_state_19(rct_peep* peep){
    sint16 x, y, xy_distance;

    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    peep->sub_state++;
}

/**
 *
 *  rct2: 0x006929BB
 */
static void peep_update_ride_sub_state_20(rct_peep* peep){
    sint16 x, y;
    rct_ride* ride = get_ride(peep->current_ride);

    if (ride->type == RIDE_TYPE_FIRST_AID){
        if (peep->nausea <= 35){
            peep->sub_state++;

            x = peep->next_x + 16;
            y = peep->next_y + 16;
            peep->destination_x = x;
            peep->destination_y = y;
            peep->destination_tolerence = 3;
            peep->happiness_growth_rate = min(peep->happiness_growth_rate + 30, 0xFF);
            peep->happiness = peep->happiness_growth_rate;
        }
        else{
            peep->nausea--;
            peep->nausea_growth_rate = peep->nausea;
        }
        return;
    }

    if (peep->bathroom != 0){
        peep->bathroom--;
        return;
    }

    // Do not play toilet flush sound on title screen as it's considered loud and annoying
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) {
        audio_play_sound_at_location(SOUND_TOILET_FLUSH, peep->x, peep->y, peep->z);
    }

    peep->sub_state++;

    x = peep->next_x + 16;
    y = peep->next_y + 16;
    peep->destination_x = x;
    peep->destination_y = y;
    peep->destination_tolerence = 3;

    peep->happiness_growth_rate = min(peep->happiness_growth_rate + 30, 0xFF);
    peep->happiness = peep->happiness_growth_rate;

    peep_stop_purchase_thought(peep, ride->type);
}

/**
 *
 *  rct2: 0x00692935
 */
static void peep_update_ride_sub_state_21(rct_peep* peep){
    sint16 x, y, xy_distance;

    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);

        x = peep->x & 0xFFE0;
        y = peep->y & 0xFFE0;
        if (x != peep->next_x)
            return;
        if (y != peep->next_y)
            return;
    }

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_WALKING;
    peep_window_state_update(peep);

    rct_ride* ride = get_ride(peep->current_ride);
    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

    ride_update_satisfaction(ride, peep->happiness / 64);
}

/**
 *
 *  rct2: 0x691A30
 * Used by entering_ride and queueing_front */
static void peep_update_ride(rct_peep* peep){
    switch (peep->sub_state){
    case 0:
        peep_update_ride_sub_state_0(peep);
        break;
    case 1:
        peep_update_ride_sub_state_1(peep);
        break;
    case 2:
        peep_update_ride_sub_state_2(peep);
        break;
    case 3:
        peep_update_ride_sub_state_1(peep);
        break;
    case 4:
    {
        sint16 x, y, xy_distance;
        if (!peep_update_action(&x, &y, &xy_distance, peep))
        {
            peep->sub_state = 5;
            break;
        }

        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        break;
    }
    case 5:
        peep_update_ride_sub_state_5(peep);
        break;
    case 6:
        // No action, on ride.
        break;
    case 7:
        peep_update_ride_sub_state_7(peep);
        break;
    case 8:
        peep_update_ride_sub_state_8(peep);
        break;
    case 9:
        peep_update_ride_sub_state_9(peep);
        break;
    case 10:
    case 11:
        assert(false);
        break;
    case 12:
        peep_update_ride_sub_state_12(peep);
        break;
    case 13:
        peep_update_ride_sub_state_13(peep);
        break;
    case 14:
        peep_update_ride_sub_state_14(peep);
        break;
    case 15:
        peep_update_ride_sub_state_15(peep);
        break;
    case 16:
        peep_update_ride_sub_state_16(peep);
        break;
    case 17:
        peep_update_ride_sub_state_17(peep);
        break;
    case 18:
        peep_update_ride_sub_state_18(peep);
        break;
    case 19:
        peep_update_ride_sub_state_19(peep);
        break;
    case 20:
        peep_update_ride_sub_state_20(peep);
        break;
    case 21:
        peep_update_ride_sub_state_21(peep);
        break;
    default:
        // Invalid peep sub-state
        assert(false);
        break;
    }
}

static const uint32 loc_992A18[9] = {
    (1 << 14) | (1 << 13) | (1 << 12) | (1 << 10) | (1 << 9) | (1 << 8) | (1 << 7),
    (1 << 14) | (1 << 13) | (1 << 12) | (1 << 2) | (1 << 1),
    (1 << 14) | (1 << 13) | (1 << 12) | (1 << 4) | (1 << 1),
    (1 << 14) | (1 << 13) | (1 << 12) | (1 << 3) | (1 << 1),
    (1 << 14) | (1 << 13) | (1 << 12) | (1 << 5) | (1 << 1),
    (1 << 14) | (1 << 13) | (1 << 12) | (1 << 6) | (1 << 1),
    (1 << 14) | (1 << 13) | (1 << 12) | (1 << 11)| (1 << 9),
    (1 << 14) | (1 << 13) | (1 << 12) | (1 << 10) | (1 << 9) | (1 << 8) | (1 << 7),
    (1 << 14) | (1 << 13) | (1 << 12) | (1 << 10) | (1 << 9) | (1 << 8) | (1 << 7),
};

/**
 *
 *  rct2: 0x006C0E8B
 * Also used by inspecting.
 */
static void peep_update_fixing(sint32 steps, rct_peep* peep){
    rct_ride* ride = get_ride(peep->current_ride);

    if (ride->type == RIDE_TYPE_NULL)
    {
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_FALLING;
        peep_window_state_update(peep);
        return;
    }

    bool progressToNextSubstate = true;
    bool firstRun = true;

    while (progressToNextSubstate) {
        switch (peep->sub_state) {
            case 0:
                progressToNextSubstate = peep_update_fixing_sub_state_0(ride);
                break;

            case 1:
                progressToNextSubstate = peep_update_fixing_sub_state_1(firstRun, peep, ride);
                break;

            case 2:
            case 3:
            case 4:
            case 5:
                progressToNextSubstate = peep_update_fixing_sub_state_2345(firstRun, peep, ride);
                break;

            case 6:
                progressToNextSubstate = peep_update_fixing_sub_state_6(firstRun, peep, ride);
                break;

            case 7:
                progressToNextSubstate = peep_update_fixing_sub_state_7(firstRun, peep, ride);
                break;

            case 8:
                progressToNextSubstate = peep_update_fixing_sub_state_8(firstRun, peep);
                break;

            case 9:
                progressToNextSubstate = peep_update_fixing_sub_state_9(firstRun, peep, ride);
                break;

            case 10:
                progressToNextSubstate = peep_update_fixing_sub_state_10(firstRun, peep, ride);
                break;

            case 11:
                progressToNextSubstate = peep_update_fixing_sub_state_11(firstRun, peep, ride);
                break;

            case 12:
                progressToNextSubstate = peep_update_fixing_sub_state_12(firstRun, peep, ride);
                break;

            case 13:
                progressToNextSubstate = peep_update_fixing_sub_state_13(firstRun, steps, peep, ride);
                break;

            case 14:
                progressToNextSubstate = peep_update_fixing_sub_state_14(firstRun, peep, ride);
                break;

            default:
                log_error("Invalid substate");
                progressToNextSubstate = false;
        }

        firstRun = false;

        if (!progressToNextSubstate) {
            break;
        }

        sint32 subState = peep->sub_state;
        uint32 ebp = loc_992A18[8];

        if (peep->state != PEEP_STATE_INSPECTING) {
            ebp = loc_992A18[ride->breakdown_reason_pending];
        }

        do {
            subState++;
        } while ((ebp & (1 << subState)) == 0);

        peep->sub_state = subState & 0xFF;
    }
}

/**
 * rct2: 0x006C0EEC
 */
static bool peep_update_fixing_sub_state_0(rct_ride *ride) {
    ride->mechanic_status = RIDE_MECHANIC_STATUS_FIXING;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;

    return true;
}

/**
 * rct2: 0x006C0F09
 */
static bool peep_update_fixing_sub_state_1(bool firstRun, rct_peep *peep, rct_ride *ride) {
    sint16 x, y, tmp_xy_distance;

    if (!firstRun) {
        rct_vehicle *vehicle = ride_get_broken_vehicle(ride);
        if (vehicle == NULL) {
            return true;
        }

        while (true) {
            if (vehicle->is_child == 0) {
                break;
            }

            uint8 trackType = vehicle->track_type >> 2;
            if (trackType == TRACK_ELEM_END_STATION) {
                break;
            }

            if (trackType == TRACK_ELEM_BEGIN_STATION) {
                break;
            }

            if (trackType == TRACK_ELEM_MIDDLE_STATION) {
                break;
            }

            vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
        }

        rct_xy16 offset = word_981D6C[peep->direction];
        peep->destination_x = (offset.x * -12) + vehicle->x;
        peep->destination_y = (offset.y * -12) + vehicle->y;
        peep->destination_tolerence = 2;
    }

    invalidate_sprite_2((rct_sprite *) peep);
    if (peep_update_action(&x, &y, &tmp_xy_distance, peep)) {
        sprite_move(x, y, peep->z, (rct_sprite *) peep);
        invalidate_sprite_2((rct_sprite *) peep);
        return false;
    }

    return true;
}

/**
 * rct2: 0x006C0FD3
 */
static bool peep_update_fixing_sub_state_2345(bool firstRun, rct_peep *peep, rct_ride *ride) {
    sint16 tmp_x, tmp_y, tmp_distance;

    if (!firstRun) {
        peep->sprite_direction = peep->direction << 3;

        peep->action = (peep_rand() & 1) ? PEEP_ACTION_STAFF_FIX_2 : PEEP_ACTION_STAFF_FIX;
        peep->action_sprite_image_offset = 0;
        peep->action_frame = 0;
        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite *) peep);
    }

    if (peep->action == PEEP_ACTION_NONE_2) {
        return true;
    }

    peep_update_action(&tmp_x, &tmp_y, &tmp_distance, peep);

    uint8 actionFrame = (peep->action == PEEP_ACTION_STAFF_FIX) ? 0x25 : 0x50;
    if (peep->action_frame != actionFrame) {
        return false;
    }

    rct_vehicle *vehicle = ride_get_broken_vehicle(ride);
    if (vehicle == NULL) {
        return true;
    }

    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_BROKEN_CAR;

    return false;
}

/**
 * rct2: 0x006C107B
 */
static bool peep_update_fixing_sub_state_6(bool firstRun, rct_peep *peep, rct_ride *ride) {
    sint16 tmp_x, tmp_y, tmp_distance;

    if (!firstRun) {
        peep->sprite_direction = peep->direction << 3;
        peep->action = PEEP_ACTION_STAFF_FIX_3;
        peep->action_sprite_image_offset = 0;
        peep->action_frame = 0;

        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite *) peep);
    }

    if (peep->action == PEEP_ACTION_NONE_2) {
        return true;
    }

    peep_update_action(&tmp_x, &tmp_y, &tmp_distance, peep);
    if (peep->action_frame != 0x65) {
        return false;
    }

    rct_vehicle *vehicle = ride_get_broken_vehicle(ride);
    if (vehicle == NULL) {
        return true;
    }

    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_BROKEN_TRAIN;

    return false;
}

/** rct2: 0x00992A3C */
static const rct_xy16 _992A3C[] = {
    { -12,   0 },
    {   0,  12 },
    {  12,   0 },
    {   0, -12 },
};

/**
 * rct2: 0x006C1114
 */
static bool peep_update_fixing_sub_state_7(bool firstRun, rct_peep *peep, rct_ride *ride) {
    sint16 x, y, tmp_distance;

    if (!firstRun) {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3 | RIDE_TYPE_FLAG_HAS_NO_TRACK)) {
            return true;
        }

        uint16 stationPosition = ride->station_starts[peep->current_ride_station];
        if (stationPosition == 0xFFFF) {
            return true;
        }

        uint8 stationZ = ride->station_heights[peep->current_ride_station];
        uint16 stationX = (stationPosition & 0xFF) * 32;
        uint16 stationY = (stationPosition >> 8) * 32;

        rct_map_element *mapElement = map_get_track_element_at(stationX, stationY, stationZ);
        if (mapElement == NULL) {
            log_error("Couldn't find map_element");
            return false;
        }

        sint32 direction = map_element_get_direction(mapElement);
        rct_xy16 offset = _992A3C[direction];

        stationX += 16 + offset.x;
        if (offset.x == 0) {
            stationX = peep->destination_x;
        }

        stationY += 16 + offset.y;
        if (offset.y == 0) {
            stationY = peep->destination_y;
        }

        peep->destination_x = stationX;
        peep->destination_y = stationY;
        peep->destination_tolerence = 2;
    }

    invalidate_sprite_2((rct_sprite *) peep);
    if (!peep_update_action(&x, &y, &tmp_distance, peep)) {
        return true;
    }

    sprite_move(x, y, peep->z, (rct_sprite *) peep);
    invalidate_sprite_2((rct_sprite *) peep);

    return false;
}

/**
 * rct2: 0x006C11F5
 */
static bool peep_update_fixing_sub_state_8(bool firstRun, rct_peep *peep) {
    sint16 tmp_x, tmp_y, tmp_xy_distance;

    if (!firstRun) {
        peep->sprite_direction = peep->direction << 3;
        peep->action = PEEP_ACTION_STAFF_CHECKBOARD;
        peep->action_frame = 0;
        peep->action_sprite_image_offset = 0;

        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite *) peep);
    }

    if (peep->action == PEEP_ACTION_NONE_2) {
        return true;
    }

    peep_update_action(&tmp_x, &tmp_y, &tmp_xy_distance, peep);

    return false;
}

/**
 * rct2: 0x006C1239
 */
static bool peep_update_fixing_sub_state_9(bool firstRun, rct_peep *peep, rct_ride *ride) {
    sint16 x, y, tmp_xy_distance;

    if (!firstRun) {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3 | RIDE_TYPE_FLAG_HAS_NO_TRACK)) {
            return true;
        }

        uint16 stationPosition = ride->station_starts[peep->current_ride_station];
        if (stationPosition == 0xFFFF) {
            return true;
        }

        uint8 stationZ = ride->station_heights[peep->current_ride_station];

        rct_xy_element input;
        input.x = (stationPosition & 0xFF) * 32;
        input.y = (stationPosition >> 8) * 32;
        input.element = map_get_track_element_at_from_ride(input.x, input.y, stationZ, peep->current_ride);
        if (input.element == NULL) {
            return true;
        }

        uint8 direction = 0;
        track_begin_end trackBeginEnd;
        while (track_block_get_previous(input.x, input.y, input.element, &trackBeginEnd)) {
            uint8 trackType = trackBeginEnd.begin_element->properties.track.type;
            if (trackType == TRACK_ELEM_BEGIN_STATION || trackType == TRACK_ELEM_MIDDLE_STATION || trackType == TRACK_ELEM_END_STATION) {
                input.x = trackBeginEnd.begin_x;
                input.y = trackBeginEnd.begin_y;
                input.element = trackBeginEnd.begin_element;

                direction = trackBeginEnd.begin_element->type & 3;
                continue;
            }

            break;
        }

        // loc_6C12ED:
        uint16 destinationX = input.x + 16;
        uint16 destinationY = input.y + 16;

        rct_xy16 offset = _992A3C[direction];

        destinationX -= offset.x;
        if (offset.x == 0) {
            destinationX = peep->destination_x;
        }

        destinationY -= offset.y;
        if (offset.y == 0) {
            destinationY = peep->destination_y;
        }

        peep->destination_x = destinationX;
        peep->destination_y = destinationY;
        peep->destination_tolerence = 2;
    }

    invalidate_sprite_2((rct_sprite *) peep);

    if (!peep_update_action(&x, &y, &tmp_xy_distance, peep)) {
        return true;
    }

    sprite_move(x, y, peep->z, (rct_sprite *) peep);
    invalidate_sprite_2((rct_sprite *) peep);

    return false;
}

/**
 * rct2: 0x006C1368
 */
static bool peep_update_fixing_sub_state_10(bool firstRun, rct_peep *peep, rct_ride *ride) {
    sint16 tmp_x, tmp_y, tmp_xy_distance;

    if (!firstRun) {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3 | RIDE_TYPE_FLAG_HAS_NO_TRACK)) {
            return true;
        }

        peep->sprite_direction = peep->direction << 3;

        peep->action = PEEP_ACTION_STAFF_FIX;
        peep->action_frame = 0;
        peep->action_sprite_image_offset = 0;

        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite *) peep);
    }

    if (peep->action == PEEP_ACTION_NONE_2) {
        return true;
    }

    peep_update_action(&tmp_x, &tmp_y, &tmp_xy_distance, peep);

    return false;
}

/**
 * rct2: 0x006C13CE
 */
static bool peep_update_fixing_sub_state_11(bool firstRun, rct_peep *peep, rct_ride *ride) {
    sint16 tmp_x, tmp_y, tmp_xy_distance;

    if (!firstRun) {
        peep->sprite_direction = peep->direction << 3;

        peep->action = PEEP_ACTION_STAFF_FIX_GROUND;
        peep->action_frame = 0;
        peep->action_sprite_image_offset = 0;

        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite *) peep);
    }

    if (peep->action == PEEP_ACTION_NONE_2) {
        return true;
    }

    peep_update_action(&tmp_x, &tmp_y, &tmp_xy_distance, peep);
    if (peep->action_frame == 0x28) {
        ride->mechanic_status = RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
    }

    if (peep->action_frame == 0x13 ||
        peep->action_frame == 0x19 ||
        peep->action_frame == 0x1F ||
        peep->action_frame == 0x25 ||
        peep->action_frame == 0x2B) {
        audio_play_sound_at_location(SOUND_MECHANIC_FIX, peep->x, peep->y, peep->z);
    }

    return false;
}

/**
 * rct2: 0x006C1474
 */
static bool peep_update_fixing_sub_state_12(bool firstRun, rct_peep *peep, rct_ride *ride) {
    sint16 x, y, tmp_xy_distance;

    if (!firstRun) {
        uint16 stationPosition = ride->exits[peep->current_ride_station];
        if (stationPosition == 0xFFFF) {
            stationPosition = ride->entrances[peep->current_ride_station];

            if (stationPosition == 0xFFFF) {
                return true;
            }
        }

        uint16 stationX = (stationPosition & 0xFF) * 32;
        uint16 stationY = (stationPosition >> 8) * 32;

        stationX += 16;
        stationY += 16;

        rct_xy16 direction = word_981D6C[peep->direction];

        stationX += direction.x * 20;
        stationY += direction.y * 20;

        peep->destination_x = stationX;
        peep->destination_y = stationY;
        peep->destination_tolerence = 2;
    }

    invalidate_sprite_2((rct_sprite *) peep);
    if (peep_update_action(&x, &y, &tmp_xy_distance, peep) == 0) {
        return true;
    } else {
        sprite_move(x, y, peep->z, (rct_sprite *) peep);
        invalidate_sprite_2((rct_sprite *) peep);
    }

    return false;
}

/**
 * rct2: 0x006C1504
 */
static bool peep_update_fixing_sub_state_13(bool firstRun, sint32 steps, rct_peep *peep, rct_ride *ride) {
    sint16 tmp_x, tmp_y, tmp_xy_distance;

    if (!firstRun) {
        if (peep->state == PEEP_STATE_INSPECTING) {
            peep_update_ride_inspected(peep->current_ride);

            peep->staff_rides_inspected++;
            peep->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME | RIDE_INVALIDATE_RIDE_LIST;

            return true;
        }

        peep->staff_rides_fixed++;
        peep->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME | RIDE_INVALIDATE_RIDE_LIST;

        peep->sprite_direction = peep->direction << 3;
        peep->action = PEEP_ACTION_STAFF_ANSWER_CALL_2;
        peep->action_frame = 0;
        peep->action_sprite_image_offset = 0;

        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite *) peep);
    }

    if (peep->action != 0xFF) {
        peep_update_action(&tmp_x, &tmp_y, &tmp_xy_distance, peep);
        return false;
    }

    ride_fix_breakdown(peep->current_ride, steps);

    return true;
}

/**
 * rct2: 0x006C157E
 */
static bool peep_update_fixing_sub_state_14(bool firstRun, rct_peep *peep, rct_ride *ride) {
    sint16 x, y, xy_distance;

    if (!firstRun) {
        uint16 exitPosition = ride->exits[peep->current_ride_station];
        if (exitPosition == 0xFFFF) {
            exitPosition = ride->entrances[peep->current_ride_station];

            if (exitPosition == 0xFFFF) {
                peep_decrement_num_riders(peep);
                peep->state = 0;
                peep_window_state_update(peep);
                return false;
            }
        }

        uint16 exitX = (exitPosition & 0xFF) * 32;
        uint16 exitY = (exitPosition >> 8) * 32;

        exitX += 16;
        exitY += 16;

        rct_xy16 ebx_direction = word_981D6C[peep->direction];
        exitX -= ebx_direction.x * 19;
        exitY -= ebx_direction.y * 19;

        peep->destination_x = exitX;
        peep->destination_y = exitY;
        peep->destination_tolerence = 2;
    }

    invalidate_sprite_2((rct_sprite *) peep);
    if (peep_update_action(&x, &y, &xy_distance, peep) == 0) {
        peep_decrement_num_riders(peep);
        peep->state = 0;
        peep_window_state_update(peep);

        return false;
    }

    uint16 z = ride->station_heights[peep->current_ride_station] * 8;

    if (xy_distance >= 16) {
        z += RideData5[ride->type].z;
    }

    sprite_move(x, y, z, (rct_sprite *) peep);
    invalidate_sprite_2((rct_sprite *) peep);

    return false;
}

/**
 * rct2: 0x6B7588
 */
static void peep_update_ride_inspected(sint32 rideIndex) {
    rct_ride *ride = get_ride(rideIndex);
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;

    ride->reliability += ((100 - (ride->reliability >> 8)) >> 2) * (scenario_rand() & 0xFF);
    ride->last_inspection = 0;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE | RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
}

/**
 *
 *  rct2: 0x69185D
 */
static void peep_update_queuing(rct_peep* peep){
    if (!checkForPath(peep)){
        remove_peep_from_queue(peep);
        return;
    }
    rct_ride* ride = get_ride(peep->current_ride);
    if (ride->status == RIDE_STATUS_CLOSED || ride->status == RIDE_STATUS_TESTING){
        remove_peep_from_queue(peep);
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_1;
        peep_window_state_update(peep);
        return;
    }

    if (peep->sub_state != 10){
        bool is_front = true;
        if (peep->next_in_queue != SPRITE_INDEX_NULL) {
            // Fix #4819: Occasionally the peep->next_in_queue is incorrectly set
            // to prevent this from causing the peeps to enter a loop
            // first check if the next in queue is actually nearby
            // if they are not then it's safe to assume that this is
            // the front of the queue.
            rct_peep* next_peep = GET_PEEP(peep->next_in_queue);
            if (abs(next_peep->x - peep->x) < 32 &&
                abs(next_peep->y - peep->y) < 32) {
                is_front = false;
            }
        }
        if (is_front){
            //Happens every time peep goes onto ride.
            peep->destination_tolerence = 0;
            peep_decrement_num_riders(peep);
            peep->state = PEEP_STATE_QUEUING_FRONT;
            peep_window_state_update(peep);
            peep->sub_state = 0;
            return;
        }
        //Give up queueing for the ride
        peep->sprite_direction ^= (1 << 4);
        invalidate_sprite_2((rct_sprite*)peep);
        remove_peep_from_queue(peep);
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_1;
        peep_window_state_update(peep);
        return;
    }

    peep_perform_next_action(peep);
    if (peep->action < 0xFE)return;
    if (peep->sprite_type == PEEP_SPRITE_TYPE_NORMAL) {
        if (peep->time_in_queue >= 2000 && (0xFFFF & peep_rand()) <= 119){
            // Eat Food/Look at watch
            peep->action = PEEP_ACTION_EAT_FOOD;
            peep->action_frame = 0;
            peep->action_sprite_image_offset = 0;
            peep_update_current_action_sprite_type(peep);
            invalidate_sprite_2((rct_sprite*)peep);
        }
        if (peep->time_in_queue >= 3500 && (0xFFFF & peep_rand()) <= 93)
        {
            //Create the I have been waiting in line ages thought
            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_QUEUING_AGES, peep->current_ride);
        }
    }
    else{
        if (!(peep->time_in_queue & 0x3F) && peep->action == 0xFE && peep->next_action_sprite_type == 2){
            switch (peep->sprite_type) {
            case PEEP_SPRITE_TYPE_ICE_CREAM:
            case PEEP_SPRITE_TYPE_CHIPS:
            case PEEP_SPRITE_TYPE_BURGER:
            case PEEP_SPRITE_TYPE_DRINK:
            case PEEP_SPRITE_TYPE_CANDYFLOSS:
            case PEEP_SPRITE_TYPE_PIZZA:
            case PEEP_SPRITE_TYPE_POPCORN:
            case PEEP_SPRITE_TYPE_HOT_DOG:
            case PEEP_SPRITE_TYPE_TENTACLE:
            case PEEP_SPRITE_TYPE_TOFFEE_APPLE:
            case PEEP_SPRITE_TYPE_DONUT:
            case PEEP_SPRITE_TYPE_COFFEE:
            case PEEP_SPRITE_TYPE_CHICKEN:
            case PEEP_SPRITE_TYPE_LEMONADE:
            case PEEP_SPRITE_TYPE_PRETZEL:
            case PEEP_SPRITE_TYPE_SU_JONGKWA:
            case PEEP_SPRITE_TYPE_JUICE:
            case PEEP_SPRITE_TYPE_FUNNEL_CAKE:
            case PEEP_SPRITE_TYPE_NOODLES:
            case PEEP_SPRITE_TYPE_SAUSAGE:
            case PEEP_SPRITE_TYPE_SOUP:
            case PEEP_SPRITE_TYPE_SANDWICH:
                // Eat food
                peep->action = PEEP_ACTION_EAT_FOOD;
                peep->action_frame = 0;
                peep->action_sprite_image_offset = 0;
                peep_update_current_action_sprite_type(peep);
                invalidate_sprite_2((rct_sprite*)peep);
                break;
            }
        }
    }
    if (peep->time_in_queue < 4300) return;

    if (peep->happiness <= 65 && (0xFFFF & peep_rand()) < 2184){
        //Give up queueing for the ride
        peep->sprite_direction ^= (1 << 4);
        invalidate_sprite_2((rct_sprite*)peep);
        remove_peep_from_queue(peep);
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_1;
        peep_window_state_update(peep);
    }
}

/** rct2: 0x009929C8 */
static const rct_xy16 _9929C8[] = {
    { 28, 28 },
    { 28,  4 },
    { 20,  4 },
    { 20, 28 },
    { 12, 28 },
    { 12,  4 },
    {  4,  4 },
    {  4, 28 },
};

/**
 *
 *  rct2: 0x006BF567
 */
static void peep_update_mowing(rct_peep* peep){
    if (!checkForPath(peep))return;

    invalidate_sprite_2((rct_sprite*)peep);
    while (1){
        sint16 x = 0, y = 0, z, xy_distance;
        if (peep_update_action(&x, &y, &xy_distance, peep)){
            z = map_element_height(x, y) & 0xFFFF;
            sprite_move(x, y, z, (rct_sprite*)peep);
            invalidate_sprite_2((rct_sprite*)peep);
            return;
        }

        peep->var_37++;

        if (peep->var_37 == 1){
            peep_switch_to_special_sprite(peep, 2);
        }

        if (peep->var_37 == countof(_9929C8)) {
            peep_state_reset(peep);
            return;
        }

        peep->destination_x = _9929C8[peep->var_37].x + peep->next_x;
        peep->destination_y = _9929C8[peep->var_37].y + peep->next_y;

        if (peep->var_37 != 7)continue;

        rct_map_element *map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

        for (; (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_SURFACE); map_element++);

        if ((map_element->properties.surface.terrain & MAP_ELEMENT_SURFACE_TERRAIN_MASK) == (TERRAIN_GRASS << 5)){
            map_element->properties.surface.grass_length = GRASS_LENGTH_MOWED;
            map_invalidate_tile_zoom0(peep->next_x, peep->next_y, map_element->base_height * 8, map_element->base_height * 8 + 16);
        }
        peep->staff_lawns_mown++;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
}

/**
 *
 *  rct2: 0x006BF7E6
 */
static void peep_update_watering(rct_peep* peep){
    peep->var_E2 = 0;
    if (peep->sub_state == 0){
        if (!checkForPath(peep))return;

        peep_perform_next_action(peep);
        if (!(_unk_F1EE18 & F1EE18_DESTINATION_REACHED)) return;

        peep->sprite_direction = (peep->var_37 & 3) << 3;
        peep->action = PEEP_ACTION_STAFF_WATERING;
        peep->action_frame = 0;
        peep->action_sprite_image_offset = 0;
        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite*)peep);

        peep->sub_state = 1;
    }
    else if (peep->sub_state == 1){
        if (peep->action != PEEP_ACTION_NONE_2){
            sint16 x, y, xy_distance;
            peep_update_action(&x, &y, &xy_distance, peep);
            return;
        }

        sint32 x = peep->next_x + TileDirectionDelta[peep->var_37].x;
        sint32 y = peep->next_y + TileDirectionDelta[peep->var_37].y;

        rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

        do{
            if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_SCENERY)
                continue;

            if (abs(((sint32)peep->next_z) - map_element->base_height) > 4)
                continue;

            rct_scenery_entry* scenery_entry = get_small_scenery_entry(map_element->properties.scenery.type);

            if (!(scenery_entry->small_scenery.flags & SMALL_SCENERY_FLAG_CAN_BE_WATERED))
                continue;

            map_element->properties.scenery.age = 0;
            map_invalidate_tile_zoom0(x, y, map_element->base_height * 8, map_element->clearance_height * 8);
            peep->staff_gardens_watered++;
            peep->window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
        } while (!map_element_is_last_for_tile(map_element++));

        peep_state_reset(peep);
    }
}

/**
 *
 *  rct2: 0x006BF6C9
 */
static void peep_update_emptying_bin(rct_peep* peep){
    peep->var_E2 = 0;

    if (peep->sub_state == 0){
        if (!checkForPath(peep))return;

        peep_perform_next_action(peep);
        if (!(_unk_F1EE18 & F1EE18_DESTINATION_REACHED)) return;

        peep->sprite_direction = (peep->var_37 & 3) << 3;
        peep->action = PEEP_ACTION_STAFF_EMPTY_BIN;
        peep->action_frame = 0;
        peep->action_sprite_image_offset = 0;
        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite*)peep);

        peep->sub_state = 1;
    }
    else if (peep->sub_state == 1){

        if (peep->action == PEEP_ACTION_NONE_2){
            peep_state_reset(peep);
            return;
        }

        sint16 x = 0, y = 0, xy_distance;
        peep_update_action(&x, &y, &xy_distance, peep);

        if (peep->action_frame != 11)return;

        rct_map_element* map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

        for (;; map_element++) {
            if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_PATH) {
                if (peep->next_z == map_element->base_height)
                    break;
            }
            if (map_element_is_last_for_tile(map_element)) {
                peep_state_reset(peep);
                return;
            }
        }

        if (!footpath_element_has_path_scenery(map_element)) {
            peep_state_reset(peep);
            return;
        }

        rct_scenery_entry* scenery_entry = get_footpath_item_entry(footpath_element_get_path_scenery_index(map_element));
        if (
            !(scenery_entry->path_bit.flags & PATH_BIT_FLAG_IS_BIN)
            || map_element->flags & (1 << 5)
            || footpath_element_path_scenery_is_ghost(map_element)
        ) {
            peep_state_reset(peep);
            return;
        }

        map_element->properties.path.addition_status |= ((3 << peep->var_37) << peep->var_37);

        map_invalidate_tile_zoom0(peep->next_x, peep->next_y, map_element->base_height * 8, map_element->clearance_height * 8);

        peep->staff_bins_emptied++;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
}

/**
 *
 *  rct2: 0x6BF641
 */
static void peep_update_sweeping(rct_peep* peep){
    peep->var_E2 = 0;
    if (!checkForPath(peep))return;

    invalidate_sprite_2((rct_sprite*)peep);

    if (peep->action == PEEP_ACTION_STAFF_SWEEP && peep->action_frame == 8){
        // Remove sick at this location
        litter_remove_at(peep->x, peep->y, peep->z);
        peep->staff_litter_swept++;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
    sint16 x = 0, y = 0, z, xy_distance;
    if (peep_update_action(&x, &y, &xy_distance, peep)){
        z = peep_get_height_on_slope(peep, x, y);
        sprite_move(x, y, z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    peep->var_37++;
    if (peep->var_37 != 2){
        peep->action = PEEP_ACTION_STAFF_SWEEP;
        peep->action_frame = 0;
        peep->action_sprite_image_offset = 0;
        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }
    peep_state_reset(peep);
}

/**
 *
 *  rct2: 0x6902A2
 */
static void peep_update_1(rct_peep* peep){
    if (!checkForPath(peep))return;

    peep_decrement_num_riders(peep);

    if (peep->type == PEEP_TYPE_GUEST){
        peep->state = PEEP_STATE_WALKING;
    }
    else{
        peep->state = PEEP_STATE_PATROLLING;
    }
    peep_window_state_update(peep);
    peep->destination_x = peep->x;
    peep->destination_y = peep->y;
    peep->destination_tolerence = 10;
    peep->var_76 = 0;
    peep->direction = peep->sprite_direction >> 3;
}

/**
 *
 *  rct2: 0x690009
 */
static void peep_update_picked(rct_peep* peep){
    if (gCurrentTicks & 0x1F) return;
    peep->sub_state++;
    if (peep->sub_state == 13){
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_HELP, 0xFF);
    }
}

/**
 *
 *  rct2: 0x6914CD
 */
static void peep_update_leaving_park(rct_peep* peep){
    if (peep->var_37 != 0){
        peep_perform_next_action(peep);
        if (!(_unk_F1EE18 & F1EE18_OUTSIDE_PARK)) return;
        peep_sprite_remove(peep);
        return;
    }

    sint16 x = 0, y = 0, xy_distance;
    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }

    peep->outside_of_park = 1;
    peep->destination_tolerence = 5;
    decrement_guests_in_park();
    gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PEEP_COUNT;
    peep->var_37 = 1;

    window_invalidate_by_class(WC_GUEST_LIST);

    peep_perform_next_action(peep);
    if (!(_unk_F1EE18 & F1EE18_OUTSIDE_PARK)) return;
    peep_sprite_remove(peep);
}

/**
 *
 *  rct2: 0x6916D6
 */
static void peep_update_watching(rct_peep* peep){
    if (peep->sub_state == 0){
        if (!checkForPath(peep))return;

        peep_perform_next_action(peep);
        if (!(_unk_F1EE18 & F1EE18_DESTINATION_REACHED)) return;

        peep->destination_x = peep->x;
        peep->destination_y = peep->y;

        peep->sprite_direction = (peep->var_37 & 3) * 8;
        invalidate_sprite_2((rct_sprite*)peep);

        peep->action = 0xFE;
        peep->next_action_sprite_type = 2;

        peep_switch_to_next_action_sprite_type(peep);

        peep->sub_state++;

        peep->time_to_stand = clamp(0, ((129 - peep->energy) * 16 + 50) / 2, 255);
        peep_update_sprite_type(peep);
    }
    else if (peep->sub_state == 1){
        if (peep->action < 0xFE){
            //6917F6
            sint16 x = 0, y = 0, xy_distance;
            peep_update_action(&x, &y, &xy_distance, peep);

            if (peep->action != 0xFF)return;
            peep->action = 0xFE;
        }
        else{
            if (peep_has_food(peep)){
                if ((peep_rand() & 0xFFFF) <= 1310){
                    peep->action = PEEP_ACTION_EAT_FOOD;
                    peep->action_frame = 0;
                    peep->action_sprite_image_offset = 0;
                    peep_update_current_action_sprite_type(peep);
                    invalidate_sprite_2((rct_sprite*)peep);
                    return;
                }
            }

            if ((peep_rand() & 0xFFFF) <= 655){
                peep->action = PEEP_ACTION_TAKE_PHOTO;
                peep->action_frame = 0;
                peep->action_sprite_image_offset = 0;
                peep_update_current_action_sprite_type(peep);
                invalidate_sprite_2((rct_sprite*)peep);
                return;
            }

            if ((peep->standing_flags & 1)){
                if ((peep_rand() & 0xFFFF) <= 655){
                    peep->action = PEEP_ACTION_WAVE;
                    peep->action_frame = 0;
                    peep->action_sprite_image_offset = 0;
                    peep_update_current_action_sprite_type(peep);
                    invalidate_sprite_2((rct_sprite*)peep);
                    return;
                }
            }
        }

        peep->standing_flags ^= (1 << 7);
        if (!(peep->standing_flags & (1 << 7)))return;

        peep->time_to_stand--;
        if (peep->time_to_stand != 0)return;

        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_WALKING;
        peep_window_state_update(peep);
        peep_update_sprite_type(peep);
        // Send peep to the center of current tile.
        peep->destination_x = (peep->x & 0xFFE0) + 16;
        peep->destination_y = (peep->y & 0xFFE0) + 16;
        peep->destination_tolerence = 5;
        peep_update_current_action_sprite_type(peep);
    }
}

/**
* rct2: 0x691451
*/
static void peep_update_entering_park(rct_peep* peep){
    if (peep->var_37 != 1){
        peep_perform_next_action(peep);
        if ((_unk_F1EE18 & F1EE18_OUTSIDE_PARK)) {
            decrement_guests_heading_for_park();
            peep_sprite_remove(peep);
        }
        return;
    }
    sint16 x = 0, y = 0, xy_distance;
    if (peep_update_action(&x, &y, &xy_distance, peep)){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return;
    }
    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_FALLING;
    peep_window_state_update(peep);

    peep->outside_of_park = 0;
    peep->time_in_park = gScenarioTicks;
    increment_guests_in_park();
    decrement_guests_heading_for_park();
    gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PEEP_COUNT;
    window_invalidate_by_class(WC_GUEST_LIST);
}

/**
 *
 *  rct2: 0x00690582
 */
static sint32 peep_update_walking_find_bench(rct_peep* peep){
    if (!peep_should_find_bench(peep))return 0;

    rct_map_element* map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

    for (;; map_element++){
        if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_PATH) {
            if (peep->next_z == map_element->base_height)break;
        }
        if (map_element_is_last_for_tile(map_element)){
            return 0;
        }
    }

    if (!footpath_element_has_path_scenery(map_element)) return 0;
    rct_scenery_entry* sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(map_element));

    if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BENCH))return 0;

    if (map_element->flags & MAP_ELEMENT_FLAG_BROKEN)return 0;

    if (footpath_element_path_scenery_is_ghost(map_element)) return 0;

    sint32 edges = (map_element->properties.path.edges & 0xF) ^ 0xF;
    if (edges == 0) return 0;

    uint8 chosen_edge = peep_rand() & 0x3;

    for (; !(edges & (1 << chosen_edge));)chosen_edge = (chosen_edge + 1) & 0x3;

    uint16 sprite_id = sprite_get_first_in_quadrant(peep->x, peep->y);
    uint8 free_edge = 3;

    for (rct_sprite* sprite; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant){
        sprite = get_sprite(sprite_id);

        if (sprite->unknown.linked_list_type_offset != SPRITE_LIST_PEEP * 2) continue;

        if (sprite->peep.state != PEEP_STATE_SITTING)continue;

        if (peep->z != sprite->peep.z)continue;

        if ((sprite->peep.var_37 & 0x3) != chosen_edge)continue;

        free_edge &= ~(1 << ((sprite->peep.var_37 & 0x4) >> 2));
    }

    if (!free_edge) return 0;

    free_edge ^= 0x3;
    if (!free_edge){
        if (peep_rand() & 0x8000000) free_edge = 1;
    }

    peep->var_37 = ((free_edge & 1) << 2) | chosen_edge;

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_SITTING;
    peep_window_state_update(peep);

    peep->sub_state = 0;

    sint32 ebx = peep->var_37 & 0x7;
    sint32 x = (peep->x & 0xFFE0) + _981F2C[ebx].x;
    sint32 y = (peep->y & 0xFFE0) + _981F2C[ebx].y;

    peep->destination_x = x;
    peep->destination_y = y;
    peep->destination_tolerence = 3;

    return 1;
}

/** rct2: 0x00992A4C */
static const rct_xy16 _992A4C[] = {
    { 11, 16 },
    { 16, 21 },
    { 21, 16 },
    { 16, 11 },
};

static sint32 peep_update_walking_find_bin(rct_peep* peep){
    if (!peep_has_empty_container(peep)) return 0;

    if (peep->next_var_29 & 0x18)return 0;

    rct_map_element* map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

    for (;; map_element++){
        if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_PATH){
            if (peep->next_z == map_element->base_height)break;
        }
        if (map_element_is_last_for_tile(map_element)){
            return 0;
        }
    }

    if (!footpath_element_has_path_scenery(map_element)) return 0;
    rct_scenery_entry* sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(map_element));

    if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BIN))return 0;

    if (map_element->flags & MAP_ELEMENT_FLAG_BROKEN)return 0;

    if (footpath_element_path_scenery_is_ghost(map_element)) return 0;

    sint32 edges = (map_element->properties.path.edges & 0xF) ^ 0xF;
    if (edges == 0) return 0;

    uint8 chosen_edge = peep_rand() & 0x3;

    // Note: Bin quantity is inverted 0 = full, 3 = empty
    uint8 bin_quantities = map_element->properties.path.addition_status;

    // Rotate the bin to the correct edge. Makes it easier for next calc.
    bin_quantities = ror8(ror8(bin_quantities, chosen_edge), chosen_edge);

    for (uint8 free_edge = 4; free_edge != 0; free_edge--){
        // If not full
        if (bin_quantities & 0x3){
            if (edges&(1 << chosen_edge))break;
        }
        chosen_edge = (chosen_edge + 1) & 0x3;
        bin_quantities = ror8(bin_quantities, 2);
        if ((free_edge - 1) == 0) return 0;
    }

    peep->var_37 = chosen_edge;

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_USING_BIN;
    peep_window_state_update(peep);

    peep->sub_state = 0;

    sint32 ebx = peep->var_37 & 0x3;
    sint32 x = (peep->x & 0xFFE0) + _992A4C[ebx].x;
    sint32 y = (peep->y & 0xFFE0) + _992A4C[ebx].y;

    peep->destination_x = x;
    peep->destination_y = y;
    peep->destination_tolerence = 3;

    return 1;
}

/**
 *
 *  rct2: 0x00690848
 */
static void peep_update_walking_break_scenery(rct_peep* peep){
    if(gCheatsDisableVandalism)
        return;

    if (!(peep->peep_flags & PEEP_FLAGS_ANGRY)){
        if (peep->happiness >= 48) return;
        if (peep->energy < 85) return;
        if (peep->state != PEEP_STATE_WALKING) return;

        if ((peep->litter_count & 0xC0) != 0xC0 &&
            (peep->disgusting_count & 0xC0) != 0xC0) return;

        if ((peep_rand() & 0xFFFF) > 3276) return;
    }

    if (peep->next_var_29 & 0x18) return;

    rct_map_element* map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

    for (;; map_element++){
        if ( map_element_get_type(map_element) == MAP_ELEMENT_TYPE_PATH){
            if (peep->next_z == map_element->base_height)break;
        }
        if (map_element_is_last_for_tile(map_element)){
            return;
        }
    }

    if (!footpath_element_has_path_scenery(map_element)) return;
    rct_scenery_entry* sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(map_element));

    if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_BREAKABLE))return;

    if (map_element->flags & MAP_ELEMENT_FLAG_BROKEN)return;

    if (footpath_element_path_scenery_is_ghost(map_element))return;

    sint32 edges = map_element->properties.path.edges & 0xF;
    if (edges == 0xF) return;

    rct_peep* inner_peep;
    uint16 sprite_index;

    FOR_ALL_STAFF(sprite_index, inner_peep){
        if (inner_peep->staff_type != STAFF_TYPE_SECURITY) continue;

        if (inner_peep->x == SPRITE_LOCATION_NULL) continue;

        sint32 x_diff = abs(inner_peep->x - peep->x);
        sint32 y_diff = abs(inner_peep->y - peep->y);

        if (max(x_diff, y_diff) < 224)return;
    }

    map_element->flags |= MAP_ELEMENT_FLAG_BROKEN;

    map_invalidate_tile_zoom1(
        peep->next_x,
        peep->next_y,
        (map_element->base_height << 3) + 32,
        map_element->base_height << 3);

    peep->angriness = 16;

    return;
}

/**
 *
 *  rct2: 0x006912A3
 */
static void peep_update_buying(rct_peep* peep)
{
    if (!checkForPath(peep))return;

    rct_ride* ride = get_ride(peep->current_ride);
    if (ride->type == RIDE_TYPE_NULL || ride->status != RIDE_STATUS_OPEN){
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_FALLING;
        peep_window_state_update(peep);
        return;
    }

    if (peep->sub_state == 1){
        if (peep->action != 0xFF){
            sint16 x, y, xy_distance;
            peep_update_action(&x, &y, &xy_distance, peep);
            return;
        }

        if (ride->type == RIDE_TYPE_CASH_MACHINE){
            if (peep->current_ride != peep->previous_ride){
                peep->cash_in_pocket += MONEY(50,00);
            }
            window_invalidate_by_number(WC_PEEP, peep->sprite_index);
        }
        peep->sprite_direction ^= 0x10;
        peep->destination_x = peep->next_x + 16;
        peep->destination_y = peep->next_y + 16;
        peep->direction ^= 2;

        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_WALKING;
        peep_window_state_update(peep);
        return;
    }

    uint8 item_bought = 0;

    if (peep->current_ride != peep->previous_ride){
        if (ride->type == RIDE_TYPE_CASH_MACHINE){
            item_bought = peep_should_use_cash_machine(peep, peep->current_ride);
            if (!item_bought) {
                peep->previous_ride = peep->current_ride;
                peep->previous_ride_time_out = 0;
            } else {
                peep->action = PEEP_ACTION_WITHDRAW_MONEY;
                peep->action_frame = 0;
                peep->action_sprite_image_offset = 0;

                peep_update_current_action_sprite_type(peep);
                invalidate_sprite_2((rct_sprite*)peep);

                ride->no_primary_items_sold++;
            }
        }
        else{
            rct_ride_entry* ride_type = get_ride_entry(ride->subtype);
            if (ride_type->shop_item_secondary != 0xFF){
                money16 price = ride->price_secondary;

                item_bought = peep_decide_and_buy_item(peep, peep->current_ride, ride_type->shop_item_secondary, price);
                if (item_bought) {
                    ride->no_secondary_items_sold++;
                }
            }

            if (!item_bought && ride_type->shop_item != 0xFF){
                money16 price = ride->price;

                item_bought = peep_decide_and_buy_item(peep, peep->current_ride, ride_type->shop_item, price);
                if (item_bought) {
                    ride->no_primary_items_sold++;
                }
            }
        }
    }

    if (item_bought){
        ride_update_popularity(ride, 1);

        peep_stop_purchase_thought(peep, ride->type);
    }
    else{
        ride_update_popularity(ride, 0);
    }
    peep->sub_state = 1;
    return;
}

/** rct2: 0x0097EFCC */
static const uint8 item_standard_litter[32] = {
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_BALLOON
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_TOY
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_MAP
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_PHOTO
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_UMBRELLA
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_DRINK
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_BURGER
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_CHIPS
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_ICE_CREAM
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_CANDYFLOSS
    LITTER_TYPE_EMPTY_CAN,          // PEEP_ITEM_EMPTY_CAN
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_RUBBISH
    LITTER_TYPE_EMPTY_BURGER_BOX,   // PEEP_ITEM_EMPTY_BURGER_BOX
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_PIZZA
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_VOUCHER
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_POPCORN
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_HOT_DOG
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_TENTACLE
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_HAT
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_CANDY_APPLE
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_TSHIRT
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_DONUT
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_COFFEE
    LITTER_TYPE_EMPTY_CUP,          // PEEP_ITEM_EMPTY_CUP
    LITTER_TYPE_EMPTY_BOX,          // PEEP_ITEM_CHICKEN
    LITTER_TYPE_EMPTY_BOTTLE,       // PEEP_ITEM_LEMONADE
    LITTER_TYPE_EMPTY_BOX,          // PEEP_ITEM_EMPTY_BOX
    LITTER_TYPE_EMPTY_BOTTLE,       // PEEP_ITEM_EMPTY_BOTTLE
};

/** rct2: 0x0097EFE8 */
static const uint8 item_extra_litter[32] = {
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_PHOTO2
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_PHOTO3
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_PHOTO4
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_PRETZEL
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_CHOCOLATE
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_ICED_TEA
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_FUNNEL_CAKE
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_SUNGLASSES
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_BEEF_NOODLES
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_FRIED_RICE_NOODLES
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_WONTON_SOUP
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_MEATBALL_SOUP
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_FRUIT_JUICE
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_SOYBEAN_MILK
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_SU_JONGKWA
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_SUB_SANDWICH
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_COOKIE
    LITTER_TYPE_EMPTY_BOWL_RED,     // PEEP_ITEM_EMPTY_BOWL_RED
    LITTER_TYPE_EMPTY_DRINK_CARTON, // PEEP_ITEM_EMPTY_DRINK_CARTON
    LITTER_TYPE_EMPTY_JUICE_CUP,    // PEEP_ITEM_EMPTY_JUICE_CUP
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_ROAST_SAUSAGE
    LITTER_TYPE_EMPTY_BOWL_BLUE,    // PEEP_ITEM_EMPTY_BOWL_BLUE
};

/**
 *
 *  rct2: 0x00691089
 */
static void peep_update_using_bin(rct_peep* peep){
    if (peep->sub_state == 0){
        if (!checkForPath(peep))return;

        peep_perform_next_action(peep);
        if (!(_unk_F1EE18 & F1EE18_DESTINATION_REACHED)) return;

        peep->sub_state = 1;
    }
    else if (peep->sub_state == 1){

        if (peep->action != PEEP_ACTION_NONE_2){
            sint16 x, y, xy_distance;
            peep_update_action(&x, &y, &xy_distance, peep);
            return;
        }

        rct_map_element* map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

        for (;;map_element++){
            if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_PATH){
                continue;
            }

            if (map_element->base_height == peep->next_z)break;

            if (map_element_is_last_for_tile(map_element)){
                peep_state_reset(peep);
                return;
            }
        }

        if (!footpath_element_has_path_scenery(map_element)){
            peep_state_reset(peep);
            return;
        }

        rct_scenery_entry* sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(map_element));
        if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BIN)){
            peep_state_reset(peep);
            return;
        }

        if (map_element->flags & MAP_ELEMENT_FLAG_BROKEN){
            peep_state_reset(peep);
            return;
        }

        if (footpath_element_path_scenery_is_ghost(map_element)){
            peep_state_reset(peep);
            return;
        }

        // Bin selection is one of 4 corners
        uint8 selected_bin = peep->var_37 * 2;

        // This counts down 2 = No rubbish, 0 = full
        uint8 space_left_in_bin = 0x3 & (map_element->properties.path.addition_status >> selected_bin);
        uint32 empty_containers = peep_empty_container_standard_flag(peep);

        for (uint8 cur_container = 0; cur_container < 32; cur_container++){
            if (!(empty_containers & (1u << cur_container))) continue;

            if (space_left_in_bin != 0){
                // OpenRCT2 modification: This previously used
                // the tick count as a simple random function
                // switched to scenario_rand as it is more reliable
                if ((peep_rand() & 7) == 0) space_left_in_bin--;
                peep->item_standard_flags &= ~(1 << cur_container);
                peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                peep_update_sprite_type(peep);
                continue;
            }
            uint8 bp = item_standard_litter[cur_container];

            sint32 x, y;
            x = peep->x + (peep_rand() & 7) - 3;
            y = peep->y + (peep_rand() & 7) - 3;

            litter_create(x, y, peep->z, peep_rand() & 3, bp);
            peep->item_standard_flags &= ~(1 << cur_container);
            peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;

            peep_update_sprite_type(peep);
        }

        // Original bug: This would clear any rubbish placed by the previous function
        //space_left_in_bin = 0x3 & (map_element->properties.path.addition_status >> selected_bin);
        empty_containers = peep_empty_container_extra_flag(peep);

        for (uint8 cur_container = 0; cur_container < 32; cur_container++){
            if (!(empty_containers & (1u << cur_container))) continue;

            if (space_left_in_bin != 0){
                // OpenRCT2 modification: This previously used
                // the tick count as a simple random function
                // switched to scenario_rand as it is more reliable
                if ((peep_rand() & 7) == 0) space_left_in_bin--;
                peep->item_extra_flags &= ~(1 << cur_container);
                peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;

                peep_update_sprite_type(peep);
                continue;
            }
            uint8 bp = item_extra_litter[cur_container];

            sint32 x, y;
            x = peep->x + (peep_rand() & 7) - 3;
            y = peep->y + (peep_rand() & 7) - 3;

            litter_create(x, y, peep->z, peep_rand() & 3, bp);
            peep->item_extra_flags &= ~(1 << cur_container);
            peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;

            peep_update_sprite_type(peep);
        }

        // Place new amount in bin by first clearing the value
        map_element->properties.path.addition_status &= ~(3 << selected_bin);
        // Then placing the new value.
        map_element->properties.path.addition_status |= space_left_in_bin << selected_bin;

        map_invalidate_tile_zoom0(peep->next_x, peep->next_y, map_element->base_height << 3, map_element->clearance_height << 3);
        peep_state_reset(peep);
    }
}

/**
 *
 *  rct2: 0x006C16D7
 */
static void peep_update_heading_to_inspect(rct_peep* peep){
    rct_ride* ride = get_ride(peep->current_ride);

    if (ride->type == RIDE_TYPE_NULL){
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_FALLING;
        peep_window_state_update(peep);
        return;
    }

    if (ride->exits[peep->current_ride_station] == 0xFFFF){
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_FALLING;
        peep_window_state_update(peep);
        return;
    }

    if (ride->mechanic_status != RIDE_MECHANIC_STATUS_HEADING ||
        !(ride->lifecycle_flags & RIDE_LIFECYCLE_DUE_INSPECTION)){
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_FALLING;
        peep_window_state_update(peep);
        return;
    }

    if (peep->sub_state == 0){
        peep->var_74 = 0;
        peep_reset_pathfind_goal(peep);
        peep->sub_state = 2;
    }

    if (peep->sub_state <= 3){
        peep->var_74++;
        if (peep->var_74 > 2500){
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_DUE_INSPECTION&&
                ride->mechanic_status == RIDE_MECHANIC_STATUS_HEADING){
                ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            }
            peep_decrement_num_riders(peep);
            peep->state = PEEP_STATE_FALLING;
            peep_window_state_update(peep);
            return;
        }

        if (!checkForPath(peep))return;

        peep_perform_next_action(peep);

        if (!(_unk_F1EE18 & F1EE18_RIDE_EXIT) &&
            !(_unk_F1EE18 & F1EE18_RIDE_ENTRANCE)
        ) {
            return;
        }

        rct_map_element* map_element = _peepRideEntranceExitElement;

        if (peep->current_ride !=
            map_element->properties.entrance.ride_index)
            return;

        uint8 exit_index = ((map_element->properties.entrance.index & 0x70) >> 4);

        if (peep->current_ride_station != exit_index)
            return;

        if (_unk_F1EE18 & F1EE18_RIDE_ENTRANCE) {
            if (ride->exits[exit_index] != 0xFFFF)return;
        }

        uint8 direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
        peep->direction = direction;

        sint32 x = peep->next_x + 16 + word_981D6C[direction].x * 53;
        sint32 y = peep->next_y + 16 + word_981D6C[direction].y * 53;

        peep->destination_x = x;
        peep->destination_y = y;
        peep->destination_tolerence = 2;
        peep->sprite_direction = direction << 3;

        peep->z = map_element->base_height * 4;
        peep->sub_state = 4;
        // Falls through into sub_state 4
    }

    invalidate_sprite_2((rct_sprite*)peep);

    sint16 delta_y = abs(peep->y - peep->destination_y);

    sint16 x, y, xy_distance;
    if (!peep_update_action(&x, &y, &xy_distance, peep)){
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_INSPECTING;
        peep->sub_state = 0;
        peep_window_state_update(peep);
        return;
    }

    sint32 z = ride->station_heights[peep->current_ride_station] * 8;

    if (delta_y < 20){
        z += RideData5[ride->type].z;
    }

    sprite_move(x, y, z, (rct_sprite*)peep);
    invalidate_sprite_2((rct_sprite*)peep);
}

/**
 *
 *  rct2: 0x006C0CB8
 */
static void peep_update_answering(rct_peep* peep){
    rct_ride* ride = get_ride(peep->current_ride);

    if (ride->type == RIDE_TYPE_NULL ||
        ride->mechanic_status != RIDE_MECHANIC_STATUS_HEADING){

        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_FALLING;
        peep_window_state_update(peep);
        return;
    }

    if (peep->sub_state == 0){
        peep->action = PEEP_ACTION_STAFF_ANSWER_CALL;
        peep->action_frame = 0;
        peep->action_sprite_image_offset = 0;

        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite*)peep);

        peep->sub_state = 1;
        peep_window_state_update(peep);
        return;
    }
    else if (peep->sub_state == 1){
        if (peep->action == PEEP_ACTION_NONE_2){
            peep->sub_state = 2;
            peep_window_state_update(peep);
            peep->var_74 = 0;
            peep_reset_pathfind_goal(peep);
            return;
        }
        sint16 x, y, xy_distance;
        peep_update_action(&x, &y, &xy_distance, peep);
        return;
    }
    else if (peep->sub_state <= 3){
        peep->var_74++;
        if (peep->var_74 > 2500){
            if (ride->mechanic_status == RIDE_MECHANIC_STATUS_HEADING){
                ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
            }
            peep_decrement_num_riders(peep);
            peep->state = PEEP_STATE_FALLING;
            peep_window_state_update(peep);
            return;
        }

        if (!checkForPath(peep))return;

        peep_perform_next_action(peep);

        if (!(_unk_F1EE18 & F1EE18_RIDE_EXIT) &&
            !(_unk_F1EE18 & F1EE18_RIDE_ENTRANCE)
        ) {
            return;
        }

        rct_map_element* map_element = _peepRideEntranceExitElement;

        if (peep->current_ride !=
            map_element->properties.entrance.ride_index)
            return;

        uint8 exit_index = ((map_element->properties.entrance.index & 0x70) >> 4);

        if (peep->current_ride_station != exit_index)
            return;

        if (_unk_F1EE18 & F1EE18_RIDE_ENTRANCE) {
            if (ride->exits[exit_index] != 0xFFFF)return;
        }

        uint8 direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
        peep->direction = direction;

        sint32 x = peep->next_x + 16 + word_981D6C[direction].x * 53;
        sint32 y = peep->next_y + 16 + word_981D6C[direction].y * 53;

        peep->destination_x = x;
        peep->destination_y = y;
        peep->destination_tolerence = 2;
        peep->sprite_direction = direction << 3;

        peep->z = map_element->base_height * 4;
        peep->sub_state = 4;
        // Falls through into sub_state 4
    }

    invalidate_sprite_2((rct_sprite*)peep);

    sint16 delta_y = abs(peep->y - peep->destination_y);

    sint16 x, y, xy_distance;
    if (!peep_update_action(&x, &y, &xy_distance, peep)){
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_FIXING;
        peep->sub_state = 0;
        peep_window_state_update(peep);
        return;
    }

    sint32 z = ride->station_heights[peep->current_ride_station] * 8;

    if (delta_y < 20){
        z += RideData5[ride->type].z;
    }

    sprite_move(x, y, z, (rct_sprite*)peep);
    invalidate_sprite_2((rct_sprite*)peep);
}

/** rct2: 0x00992A5C */
static const rct_xy16 _992A5C[] = {
    {  3, 16 },
    { 16, 29 },
    { 29, 16 },
    { 16,  3 },
    {  3, 29 },
    { 29, 29 },
    { 29,  3 },
    {  3,  3 },
};

/**
 *
 *  rct2: 0x006BF483
 */
static sint32 peep_update_patrolling_find_watering(rct_peep* peep){
    if (!(peep->staff_orders & STAFF_ORDERS_WATER_FLOWERS))
        return 0;

    uint8 chosen_position = peep_rand() & 7;
    for (sint32 i = 0; i < 8; ++i, ++chosen_position){
        chosen_position &= 7;

        sint32 x = peep->next_x + TileDirectionDelta[chosen_position].x;
        sint32 y = peep->next_y + TileDirectionDelta[chosen_position].y;

        rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

        // This seems to happen in some SV4 files.
        if (map_element == NULL) {
            continue;
        }

        do {
            if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_SCENERY){
                continue;
            }

            uint8 z_diff = abs(peep->next_z - map_element->base_height);

            if (z_diff >= 4){
                continue;
            }

            rct_scenery_entry* sceneryEntry = get_small_scenery_entry(map_element->properties.scenery.type);

            if (!(sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_CAN_BE_WATERED)){
                continue;
            }

            if (map_element->properties.scenery.age < 55){
                if (chosen_position >= 4){
                    continue;
                }

                if (map_element->properties.scenery.age < 40){
                    continue;
                }
            }

            peep_decrement_num_riders(peep);
            peep->state = PEEP_STATE_WATERING;
            peep->var_37 = chosen_position;
            peep_window_state_update(peep);

            peep->sub_state = 0;
            peep->destination_x = (peep->x & 0xFFE0) + _992A5C[chosen_position].x;
            peep->destination_y = (peep->y & 0xFFE0) + _992A5C[chosen_position].y;
            peep->destination_tolerence = 3;

            return 1;
        } while (!map_element_is_last_for_tile(map_element++));
    }
    return 0;
}

/**
 *
 *  rct2: 0x006BF3A1
 */
static sint32 peep_update_patrolling_find_bin(rct_peep* peep){
    if (!(peep->staff_orders & STAFF_ORDERS_EMPTY_BINS))
        return 0;

    if ((peep->next_var_29 & 0x18) != 0) return 0;

    rct_map_element* map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);
    if (map_element == NULL)return 0;

    for (;; map_element++){

        if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_PATH
            && (map_element->base_height == peep->next_z))
            break;

        if (map_element_is_last_for_tile(map_element))
            return 0;
    }

    if (!footpath_element_has_path_scenery(map_element)) return 0;
    rct_scenery_entry* sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(map_element));

    if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BIN))
        return 0;

    if (map_element->flags & MAP_ELEMENT_FLAG_BROKEN)
        return 0;

    if (footpath_element_path_scenery_is_ghost(map_element))
        return 0;

    uint8 bin_positions = map_element->properties.path.edges & 0xF;
    uint8 bin_quantity = map_element->properties.path.addition_status;
    uint8 chosen_position = 0;

    for (; chosen_position < 4; ++chosen_position){
        if (!(bin_positions & 1) &&
            !(bin_quantity & 3))
            break;
        bin_positions >>= 1;
        bin_quantity >>= 2;
    }

    if (chosen_position == 4)return 0;

    peep->var_37 = chosen_position;
    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_EMPTYING_BIN;
    peep_window_state_update(peep);

    peep->sub_state = 0;
    peep->destination_x = (peep->x & 0xFFE0) + _992A4C[chosen_position].x;
    peep->destination_y = (peep->y & 0xFFE0) + _992A4C[chosen_position].y;
    peep->destination_tolerence = 3;
    return 1;
}

/**
 *
 *  rct2: 0x006BF322
 */
static sint32 peep_update_patrolling_find_grass(rct_peep* peep){
    if (!(peep->staff_orders & STAFF_ORDERS_MOWING))
        return 0;

    if (peep->var_E2 < 12)return 0;

    if ((peep->next_var_29 & 0x18) != 8) return 0;

    rct_map_element* map_element = map_get_surface_element_at(peep->next_x / 32, peep->next_y / 32);

    if ((map_element->properties.surface.terrain & MAP_ELEMENT_SURFACE_TERRAIN_MASK) != TERRAIN_GRASS)
        return 0;

    if ((map_element->properties.surface.grass_length & 0x7) < GRASS_LENGTH_CLEAR_1)
        return 0;

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_MOWING;
    peep_window_state_update(peep);
    peep->var_37 = 0;
    // Original code used .y for both x and y. Changed to .x to make more sense (both x and y are 28)
    peep->destination_x = peep->next_x + _9929C8[0].x;
    peep->destination_y = peep->next_y + _9929C8[0].y;
    peep->destination_tolerence = 3;
    return 1;
}

/**
 *
 *  rct2: 0x006BF295
 */
static sint32 peep_update_patrolling_find_sweeping(rct_peep* peep){
    if (!(peep->staff_orders & STAFF_ORDERS_SWEEPING))
        return 0;

    uint16 sprite_id = sprite_get_first_in_quadrant(peep->x, peep->y);

    for (rct_sprite* sprite = NULL;
        sprite_id != SPRITE_INDEX_NULL;
        sprite_id = sprite->unknown.next_in_quadrant){

        sprite = get_sprite(sprite_id);

        if (sprite->unknown.linked_list_type_offset != SPRITE_LIST_LITTER * 2) continue;

        uint16 z_diff = abs(peep->z - sprite->litter.z);

        if (z_diff >= 16)continue;

        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_SWEEPING;
        peep_window_state_update(peep);
        peep->var_37 = 0;
        peep->destination_x = sprite->litter.x;
        peep->destination_y = sprite->litter.y;
        peep->destination_tolerence = 5;
        return 1;
    }

    return 0;
}

/**
 *
 *  rct2: 0x006BF1FD
 */
static void peep_update_patrolling(rct_peep* peep){

    if (!checkForPath(peep))return;

    peep_perform_next_action(peep);
    if (!(_unk_F1EE18 & F1EE18_DESTINATION_REACHED)) return;

    if ((peep->next_var_29 & 0x18) == 8){
        rct_map_element* map_element = map_get_surface_element_at(peep->next_x / 32, peep->next_y / 32);

        if (map_element != NULL){
            sint32 water_height = map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
            if (water_height){
                invalidate_sprite_2((rct_sprite*)peep);
                water_height *= 16;
                sprite_move(peep->x, peep->y, water_height, (rct_sprite*)peep);
                invalidate_sprite_2((rct_sprite*)peep);

                peep_decrement_num_riders(peep);
                peep->state = PEEP_STATE_FALLING;
                peep_window_state_update(peep);
                return;
            }
        }
    }

    if (peep->staff_type != STAFF_TYPE_HANDYMAN) return;

    if (peep_update_patrolling_find_sweeping(peep))return;

    if (peep_update_patrolling_find_grass(peep))return;

    if (peep_update_patrolling_find_bin(peep))return;

    peep_update_patrolling_find_watering(peep);
}

/** rct2: 0x00981F4C, 0x00981F4E */
static const rct_xy16 _981F4C[] = {
    {  7,  5 },
    {  5, 25 },
    { 25,  5 },
    {  5,  7 },
    {  7,  9 },
    {  9, 25 },
    { 25,  9 },
    {  9,  7 },
    {  7, 23 },
    { 23, 25 },
    { 25, 23 },
    { 23,  7 },
    {  7, 27 },
    { 27, 25 },
    { 25, 27 },
    { 27,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
};

/**
 *
 *  rct2: 0x0069030A
 */
static void peep_update_walking(rct_peep* peep){
    if (!checkForPath(peep))return;

    if (peep->peep_flags & PEEP_FLAGS_WAVING){
        if (peep->action >= PEEP_ACTION_NONE_1){
            if ((0xFFFF & peep_rand()) < 936){
                invalidate_sprite_2((rct_sprite*)peep);

                peep->action = PEEP_ACTION_WAVE_2;
                peep->action_frame = 0;
                peep->action_sprite_image_offset = 0;

                peep_update_current_action_sprite_type(peep);
                invalidate_sprite_2((rct_sprite*)peep);
            }
        }
    }

    if (peep->peep_flags & PEEP_FLAGS_PHOTO){
        if (peep->action >= PEEP_ACTION_NONE_1){
            if ((0xFFFF & peep_rand()) < 936){
                invalidate_sprite_2((rct_sprite*)peep);

                peep->action = PEEP_ACTION_TAKE_PHOTO;
                peep->action_frame = 0;
                peep->action_sprite_image_offset = 0;

                peep_update_current_action_sprite_type(peep);
                invalidate_sprite_2((rct_sprite*)peep);
            }
        }
    }

    if (peep->peep_flags & PEEP_FLAGS_PAINTING){
        if (peep->action >= PEEP_ACTION_NONE_1){
            if ((0xFFFF & peep_rand()) < 936){
                invalidate_sprite_2((rct_sprite*)peep);

                peep->action = PEEP_ACTION_DRAW_PICTURE;
                peep->action_frame = 0;
                peep->action_sprite_image_offset = 0;

                peep_update_current_action_sprite_type(peep);
                invalidate_sprite_2((rct_sprite*)peep);
            }
        }
    }

    if (peep->peep_flags & PEEP_FLAGS_LITTER){
        if (!(peep->next_var_29 & 0x18)){
            if ((0xFFFF & peep_rand()) <= 4096){
                static const uint8 litter_types[] = {
                    LITTER_TYPE_EMPTY_CAN,
                    LITTER_TYPE_RUBBISH,
                    LITTER_TYPE_EMPTY_BURGER_BOX,
                    LITTER_TYPE_EMPTY_CUP,
                };
                sint32 ebp = litter_types[peep_rand() & 0x3];
                sint32 x = peep->x + (peep_rand() & 0x7) - 3;
                sint32 y = peep->y + (peep_rand() & 0x7) - 3;
                sint32 direction = (peep_rand() & 0x3);

                litter_create(x, y, peep->z, direction, ebp);
            }
        }
    }
    else if (peep_has_empty_container(peep)){
        if ((!(peep->next_var_29 & 0x18)) &&
            ((uint32)(peep->sprite_index & 0x1FF) == (gCurrentTicks & 0x1FF))&&
            ((0xFFFF & peep_rand()) <= 4096)){

            uint8 pos_stnd = 0;
            for (sint32 container = peep_empty_container_standard_flag(peep); pos_stnd < 32; pos_stnd++)
                if (container & (1u << pos_stnd))
                    break;

            sint32 bp = 0;

            if (pos_stnd != 32){
                peep->item_standard_flags &= ~(1u << pos_stnd);
                bp = item_standard_litter[pos_stnd];
            }
            else{
                uint8 pos_extr = 0;
                for (sint32 container = peep_empty_container_extra_flag(peep); pos_extr < 32; pos_extr++)
                    if (container & (1u << pos_extr))
                        break;
                peep->item_extra_flags &= ~(1u << pos_extr);
                bp = item_extra_litter[pos_extr];
            }

            peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            peep_update_sprite_type(peep);

            sint32 x = peep->x + (peep_rand() & 0x7) - 3;
            sint32 y = peep->y + (peep_rand() & 0x7) - 3;
            sint32 direction = (peep_rand() & 0x3);

            litter_create(x, y, peep->z, direction, bp);
        }
    }

    peep_perform_next_action(peep);
    if (!(_unk_F1EE18 & F1EE18_DESTINATION_REACHED)) return;

    if ((peep->next_var_29 & 0x18) == 8){
        rct_map_element* map_element = map_get_surface_element_at(peep->next_x / 32, peep->next_y / 32);

        sint32 water_height = map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
        if (water_height){
            invalidate_sprite_2((rct_sprite*)peep);
            water_height *= 16;
            sprite_move(peep->x, peep->y, water_height, (rct_sprite*)peep);
            invalidate_sprite_2((rct_sprite*)peep);

            peep_decrement_num_riders(peep);
            peep->state = PEEP_STATE_FALLING;
            peep_window_state_update(peep);
            return;
        }
    }

    peep_check_if_lost(peep);
    peep_check_cant_find_ride(peep);
    peep_check_cant_find_exit(peep);

    if (peep_update_walking_find_bench(peep))return;

    if (peep_update_walking_find_bin(peep))return;

    peep_update_walking_break_scenery(peep);

    if (peep->state != PEEP_STATE_WALKING)return;

    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)return;

    if (peep->nausea > 140)return;

    if (peep->happiness < 120)return;

    if (peep->bathroom > 140)return;

    uint16 chance = peep_has_food(peep) ? 13107 : 2849;

    if ((peep_rand() & 0xFFFF) > chance)return;

    if (peep->next_var_29 & 0x1C)return;

    rct_map_element* map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

    for (;; map_element++){
        if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_PATH){
            if (peep->next_z == map_element->base_height)break;
        }
        if (map_element_is_last_for_tile(map_element)){
            return;
        }
    }

    sint32 ebp = 15;

    if (footpath_element_has_path_scenery(map_element)) {
        if (!footpath_element_path_scenery_is_ghost(map_element)) {
            rct_scenery_entry* sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(map_element));

            if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BENCH)) ebp = 9;
        }
    }

    sint32 edges = (map_element->properties.path.edges & 0xF) ^ 0xF;
    if (edges == 0) return;

    uint8 chosen_edge = peep_rand() & 0x3;

    for (; !(edges & (1 << chosen_edge));)chosen_edge = (chosen_edge + 1) & 3;

    uint8 ride_to_view, ride_seat_to_view;
    if (!peep_find_ride_to_look_at(peep, chosen_edge, &ride_to_view, &ride_seat_to_view))
        return;

    uint16 sprite_id = sprite_get_first_in_quadrant(peep->x, peep->y);
    for (rct_sprite* sprite; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant){
        sprite = get_sprite(sprite_id);

        if (sprite->unknown.linked_list_type_offset != SPRITE_LIST_PEEP * 2) continue;

        if (sprite->peep.state != PEEP_STATE_WATCHING)continue;

        if (peep->z != sprite->peep.z)continue;

        if ((sprite->peep.var_37 & 0x3) != chosen_edge)continue;

        ebp &= ~(1 << ((sprite->peep.var_37 & 0x1C) >> 2));
    }

    if (!ebp)return;

    uint8 chosen_position = peep_rand() & 0x3;

    for (; !(ebp & (1 << chosen_position));)chosen_position = (chosen_position + 1) & 3;

    peep->current_ride = ride_to_view;
    peep->current_seat = ride_seat_to_view;
    peep->var_37 = chosen_edge | (chosen_position << 2);

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_WATCHING;
    peep_window_state_update(peep);

    peep->sub_state = 0;

    sint32 ebx = peep->var_37 & 0x1F;
    sint32 x = (peep->x & 0xFFE0) + _981F4C[ebx].x;
    sint32 y = (peep->y & 0xFFE0) + _981F4C[ebx].y;

    peep->destination_x = x;
    peep->destination_y = y;
    peep->destination_tolerence = 3;

    if (peep->current_seat&1){
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_NEW_RIDE, 0xFF);
    }
    if (peep->current_ride == 0xFF){
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_SCENERY, 0xFF);
    }
}

/* From peep_update */
static void peep_update_thoughts(rct_peep* peep){
    // Thoughts must always have a gap of at least
    // 220 ticks in age between them. In order to
    // allow this when a thought is new it enters
    // a holding zone. Before it becomes fresh.
    sint32 add_fresh = 1;
    sint32 fresh_thought = -1;
    for (sint32 i = 0; i < PEEP_MAX_THOUGHTS; i++) {
        if (peep->thoughts[i].type == PEEP_THOUGHT_TYPE_NONE)
            break;


        if (peep->thoughts[i].var_2 == 1) {
            add_fresh = 0;
            // If thought is fresh we wait 220 ticks
            // before allowing a new thought to become fresh.
            if (++peep->thoughts[i].var_3 >= 220) {
                peep->thoughts[i].var_3 = 0;
                // Thought is no longer fresh
                peep->thoughts[i].var_2++;
                add_fresh = 1;
            }
        }
        else if (peep->thoughts[i].var_2 > 1) {
            if (++peep->thoughts[i].var_3 == 0) {
                // When thought is older than ~6900 ticks remove it
                if (++peep->thoughts[i].var_2 >= 28) {
                    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;

                    // Clear top thought, push others up
                    if (i < PEEP_MAX_THOUGHTS - 2) {
                        memmove(&peep->thoughts[i], &peep->thoughts[i + 1], sizeof(rct_peep_thought)*(PEEP_MAX_THOUGHTS - i - 1));
                    }
                    peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;
                }
            }
        }
        else {
            fresh_thought = i;
        }
    }
    // If there are no fresh thoughts
    // a previously new thought can become
    // fresh.
    if (add_fresh && fresh_thought != -1) {
        peep->thoughts[fresh_thought].var_2 = 1;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
    }
}

/**
 *
 *  rct2: 0x0068FC1E
 */
static void peep_update(rct_peep *peep)
{
    if (peep->type == PEEP_TYPE_GUEST) {
        if (peep->previous_ride != 255)
            if (++peep->previous_ride_time_out >= 720)
                peep->previous_ride = 255;

        peep_update_thoughts(peep);
    }

    // Walking speed logic
    uint32 stepsToTake = peep->energy;
    if (stepsToTake < 95 && peep->state == PEEP_STATE_QUEUING)
        stepsToTake = 95;
    if ((peep->peep_flags & PEEP_FLAGS_SLOW_WALK) && peep->state != PEEP_STATE_QUEUING)
        stepsToTake /= 2;
    if (peep->action == 255 && (peep->next_var_29 & 4)) {
        stepsToTake /= 2;
        if (peep->state == PEEP_STATE_QUEUING)
            stepsToTake += stepsToTake / 2;
    }

    uint32 carryCheck = peep->var_73 + stepsToTake;
    peep->var_73 = carryCheck;
    if (carryCheck <= 255) {
        peep_easter_egg_peep_interactions(peep);
    } else {
        // loc_68FD2F
        switch (peep->state) {
        case PEEP_STATE_FALLING:
            peep_update_falling(peep);
            break;
        case PEEP_STATE_1:
            peep_update_1(peep);
            break;
        case PEEP_STATE_QUEUING_FRONT:
            peep_update_ride(peep);
            break;
        case PEEP_STATE_ON_RIDE:
            // No action
            break;
        case PEEP_STATE_LEAVING_RIDE:
            peep_update_ride(peep);
            break;
        case PEEP_STATE_WALKING:
            peep_update_walking(peep);
            break;
        case PEEP_STATE_QUEUING:
            peep_update_queuing(peep);
            break;
        case PEEP_STATE_ENTERING_RIDE:
            peep_update_ride(peep);
            break;
        case PEEP_STATE_SITTING:
            peep_update_sitting(peep);
            break;
        case PEEP_STATE_PICKED:
            peep_update_picked(peep);
            break;
        case PEEP_STATE_PATROLLING:
            peep_update_patrolling(peep);
            break;
        case PEEP_STATE_MOWING:
            peep_update_mowing(peep);
            break;
        case PEEP_STATE_SWEEPING:
            peep_update_sweeping(peep);
            break;
        case PEEP_STATE_ENTERING_PARK:
            peep_update_entering_park(peep);
            break;
        case PEEP_STATE_LEAVING_PARK:
            peep_update_leaving_park(peep);
            break;
        case PEEP_STATE_ANSWERING:
            peep_update_answering(peep);
            break;
        case PEEP_STATE_FIXING:
            peep_update_fixing(stepsToTake, peep);
            break;
        case PEEP_STATE_BUYING:
            peep_update_buying(peep);
            break;
        case PEEP_STATE_WATCHING:
            peep_update_watching(peep);
            break;
        case PEEP_STATE_EMPTYING_BIN:
            peep_update_emptying_bin(peep);
            break;
        case PEEP_STATE_USING_BIN:
            peep_update_using_bin(peep);
            break;
        case PEEP_STATE_WATERING:
            peep_update_watering(peep);
            break;
        case PEEP_STATE_HEADING_TO_INSPECTION:
            peep_update_heading_to_inspect(peep);
            break;
        case PEEP_STATE_INSPECTING:
            peep_update_fixing(stepsToTake, peep);
            break;
            //There shouldn't be any more
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
    rct_peep* peep;
    rct_ride* ride;
    uint16 spriteIndex;
    uint16 guests_in_park = gNumGuestsInPark;
    sint32 hunger_counter = 0, lost_counter = 0, noexit_counter = 0, thirst_counter = 0,
        litter_counter = 0, disgust_counter = 0, bathroom_counter = 0 ,vandalism_counter = 0;
    uint8 *warning_throttle = gPeepWarningThrottle;

    gRideCount = ride_get_count(); // refactor this to somewhere else

    FOR_ALL_GUESTS(spriteIndex, peep) {
        if (peep->outside_of_park != 0 || peep->thoughts[0].var_2 > 5)
            continue;

        switch (peep->thoughts[0].type) {
        case PEEP_THOUGHT_TYPE_LOST: //0x10
            lost_counter++;
            break;

        case PEEP_THOUGHT_TYPE_HUNGRY: // 0x14
            if (peep->guest_heading_to_ride_id == 0xFF){
                hunger_counter++;
                break;
            }
            ride = get_ride(peep->guest_heading_to_ride_id);
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
                hunger_counter++;
            break;

        case PEEP_THOUGHT_TYPE_THIRSTY:
            if (peep->guest_heading_to_ride_id == 0xFF){
                thirst_counter++;
                break;
            }
            ride = get_ride(peep->guest_heading_to_ride_id);
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_SELLS_DRINKS))
                thirst_counter++;
            break;

        case PEEP_THOUGHT_TYPE_BATHROOM:
            if (peep->guest_heading_to_ride_id == 0xFF){
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
        case PEEP_THOUGHT_TYPE_VANDALISM: //0x21
            vandalism_counter++;
            break;
        default:
            break;
        }
    }
    // could maybe be packed into a loop, would lose a lot of clarity though
    if (warning_throttle[0])
        --warning_throttle[0];
    else if ( hunger_counter >= PEEP_HUNGER_WARNING_THRESHOLD && hunger_counter >= guests_in_park / 16) {
        warning_throttle[0] = 4;
        if (gConfigNotifications.guest_warnings) {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_ARE_HUNGRY, 20);
        }
    }

    if (warning_throttle[1])
        --warning_throttle[1];
    else if (thirst_counter >= PEEP_THIRST_WARNING_THRESHOLD && thirst_counter >= guests_in_park / 16) {
        warning_throttle[1] = 4;
        if (gConfigNotifications.guest_warnings) {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_ARE_THIRSTY, 21);
        }
    }

    if (warning_throttle[2])
        --warning_throttle[2];
    else if (bathroom_counter >= PEEP_BATHROOM_WARNING_THRESHOLD && bathroom_counter >= guests_in_park / 16) {
        warning_throttle[2] = 4;
        if (gConfigNotifications.guest_warnings) {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_CANT_FIND_BATHROOM, 22);
        }
    }

    if (warning_throttle[3])
        --warning_throttle[3];
    else if (litter_counter >= PEEP_LITTER_WARNING_THRESHOLD && litter_counter >= guests_in_park / 32) {
        warning_throttle[3] = 4;
        if (gConfigNotifications.guest_warnings) {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISLIKE_LITTER, 26);
        }
    }

    if (warning_throttle[4])
        --warning_throttle[4];
    else if (disgust_counter >= PEEP_DISGUST_WARNING_THRESHOLD && disgust_counter >= guests_in_park / 32) {
        warning_throttle[4] = 4;
        if (gConfigNotifications.guest_warnings) {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISGUSTED_BY_PATHS, 31);
        }
    }

    if (warning_throttle[5])
        --warning_throttle[5];
    else if (vandalism_counter >= PEEP_VANDALISM_WARNING_THRESHOLD && vandalism_counter >= guests_in_park / 32) {
        warning_throttle[5] = 4;
        if (gConfigNotifications.guest_warnings) {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISLIKE_VANDALISM, 33);
        }
    }

    if (warning_throttle[6])
        --warning_throttle[6];
    else if (noexit_counter >= PEEP_NOEXIT_WARNING_THRESHOLD) {
        warning_throttle[6] = 4;
        if (gConfigNotifications.guest_warnings) {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_GETTING_LOST_OR_STUCK, 27);
        }
    } else if (lost_counter >= PEEP_LOST_WARNING_THRESHOLD) {
        warning_throttle[6] = 4;
        if (gConfigNotifications.guest_warnings) {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_GETTING_LOST_OR_STUCK, 16);
        }
    }
}

void peep_stop_crowd_noise()
{
    if (_crowdSoundChannel != NULL) {
        Mixer_Stop_Channel(_crowdSoundChannel);
        _crowdSoundChannel = NULL;
    }
}

/**
 *
 *  rct2: 0x006BD18A
 */
void peep_update_crowd_noise()
{
    rct_viewport *viewport;
    uint16 spriteIndex;
    rct_peep *peep;
    sint32 visiblePeeps;

    if (gGameSoundsOff)
        return;

    if (!gConfigSound.sound_enabled)
        return;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return;

    viewport = g_music_tracking_viewport;
    if (viewport == (rct_viewport*)-1)
        return;

    // Count the number of peeps visible
    visiblePeeps = 0;

    FOR_ALL_GUESTS(spriteIndex, peep) {
        if (peep->sprite_left == (sint16)(uint16)0x8000)
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
    if (visiblePeeps < 0) {
        // Mute crowd noise
        if (_crowdSoundChannel != NULL) {
            Mixer_Stop_Channel(_crowdSoundChannel);
            _crowdSoundChannel = NULL;
        }
    } else {
        sint32 volume;

        // Formula to scale peeps to dB where peeps [0, 120] scales approximately logarithmically to [-3314, -150] dB/100
        // 207360000 maybe related to DSBVOLUME_MIN which is -10,000 (dB/100)
        volume = 120 - min(visiblePeeps, 120);
        volume = volume * volume * volume * volume;
        volume = (((207360000 - volume) >> viewport->zoom) - 207360000) / 65536 - 150;

        // Load and play crowd noise if needed and set volume
        if (_crowdSoundChannel == NULL) {
            _crowdSoundChannel = Mixer_Play_Music(PATH_ID_CSS2, MIXER_LOOP_INFINITE, false);
            if (_crowdSoundChannel != NULL) {
                Mixer_Channel_SetGroup(_crowdSoundChannel, MIXER_GROUP_SOUND);
            }
        }
        if (_crowdSoundChannel != NULL) {
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
    uint16 spriteIndex;
    rct_peep* peep;

    FOR_ALL_GUESTS(spriteIndex, peep) {
        if (peep->outside_of_park != 0)
            continue;

        // Release balloon
        peep_release_balloon(peep, peep->z + 9);

        // Clap
        if ((peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_QUEUING) && peep->action >= 254) {
            peep->action = PEEP_ACTION_CLAP;
            peep->action_frame = 0;
            peep->action_sprite_image_offset = 0;
            peep_update_current_action_sprite_type(peep);
            invalidate_sprite_2((rct_sprite*)peep);
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
    uint16 sprite_index;
    rct_peep *peep;

    FOR_ALL_GUESTS(sprite_index, peep) {
        if (peep->outside_of_park == 0 && peep->state == PEEP_STATE_QUEUING) {
            if (peep->days_in_queue < 255) {
                peep->days_in_queue += 1;
            }
        }
    }
}

/** rct2: 0x009823A0 */
static const enum PEEP_NAUSEA_TOLERANCE nausea_tolerance_distribution[] = {
    PEEP_NAUSEA_TOLERANCE_NONE,
    PEEP_NAUSEA_TOLERANCE_LOW, PEEP_NAUSEA_TOLERANCE_LOW,
    PEEP_NAUSEA_TOLERANCE_AVERAGE, PEEP_NAUSEA_TOLERANCE_AVERAGE, PEEP_NAUSEA_TOLERANCE_AVERAGE,
    PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH,
};

/** rct2: 0x009823BC */
static const uint8 trouser_colours[] = {
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
static const uint8 tshirt_colours[] = {
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

/**
 *
 *  rct2: 0x0069A05D
 */
rct_peep *peep_generate(sint32 x, sint32 y, sint32 z)
{
    if (gSpriteListCount[SPRITE_LIST_NULL] < 400)
        return NULL;

    rct_peep* peep = (rct_peep*)create_sprite(1);

    move_sprite_to_list((rct_sprite*)peep, SPRITE_LIST_PEEP * 2);

    peep->sprite_identifier = SPRITE_IDENTIFIER_PEEP;
    peep->sprite_type = PEEP_SPRITE_TYPE_NORMAL;
    peep->outside_of_park = 1;
    peep->state = PEEP_STATE_FALLING;
    peep->action = PEEP_ACTION_NONE_2;
    peep->special_sprite = 0;
    peep->action_sprite_image_offset = 0;
    peep->no_action_frame_no = 0;
    peep->action_sprite_type = 0;
    peep->peep_flags = 0;
    peep->favourite_ride = 0xFF;
    peep->favourite_ride_rating = 0;

    const rct_sprite_bounds* spriteBounds = g_peep_animation_entries[peep->sprite_type].sprite_bounds;
    peep->sprite_width = spriteBounds[peep->action_sprite_type].sprite_width;
    peep->sprite_height_negative = spriteBounds[peep->action_sprite_type].sprite_height_negative;
    peep->sprite_height_positive = spriteBounds[peep->action_sprite_type].sprite_height_positive;

    peep->sprite_direction = 0;

    sprite_move(x, y, z, (rct_sprite*)peep);
    invalidate_sprite_2((rct_sprite*)peep);

    peep->var_41 = (peep_rand() & 0x1F) + 45;
    peep->var_C4 = 0;
    peep->interaction_ride_index = 0xFF;
    peep->type = PEEP_TYPE_GUEST;
    peep->previous_ride = 0xFF;
    peep->thoughts->type = PEEP_THOUGHT_TYPE_NONE;
    peep->window_invalidate_flags = 0;

    uint8 al = (peep_rand() & 0x7) + 3;
    uint8 ah = min(al, 7) - 3;

    if (al >= 7) al = 15;

    if (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES){
        ah = 0;
        al = 4;
    }

    if (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES){
        ah = 9;
        al = 15;
    }

    peep->intensity = (al << 4) | ah;

    uint8 nausea_tolerance = peep_rand() & 0x7;
    if (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES){
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
    sint8 happiness_delta = (peep_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->happiness = clamp(0, peep->happiness + happiness_delta, 255);
    peep->happiness_growth_rate = peep->happiness;
    peep->nausea = 0;
    peep->nausea_growth_rate = 0;

    /* Scenario editor limits initial guest hunger to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->hunger = gGuestInitialHunger;
    /* Initial value will vary by -15..16 */
    sint8 hunger_delta = (peep_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->hunger = clamp(0, peep->hunger + hunger_delta, 255);

    /* Scenario editor limits initial guest thirst to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->thirst = gGuestInitialThirst;
    /* Initial value will vary by -15..16 */
    sint8 thirst_delta = (peep_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->thirst = clamp(0, peep->thirst + thirst_delta, 0xFF);

    peep->bathroom = 0;
    peep->var_42 = 0;
    memset(&peep->rides_been_on, 0, 32);

    peep->no_of_rides = 0;
    memset(&peep->ride_types_been_on, 0, 16);
    peep->id = gNextGuestNumber++;
    peep->name_string_idx = STR_GUEST_X;

    money32 cash = (peep_rand() & 0x3) * 100 - 100 + gGuestInitialCash;
    if (cash < 0) cash = 0;

    if (gGuestInitialCash == 0){
        cash = 500;
    }

    if (gParkFlags & PARK_FLAGS_NO_MONEY){
        cash = 0;
    }

    if (gGuestInitialCash == (money16)(uint16)0xFFFF){
        cash = 0;
    }

    peep->cash_in_pocket = cash;
    peep->cash_spent = 0;
    peep->time_in_park = -1;
    peep->pathfind_goal.x = 0xFF;
    peep->pathfind_goal.y = 0xFF;
    peep->pathfind_goal.z = 0xFF;
    peep->pathfind_goal.direction = 0xFF;
    peep->item_standard_flags = 0;
    peep->item_extra_flags = 0;
    peep->guest_heading_to_ride_id = 0xFF;
    peep->litter_count = 0;
    peep->disgusting_count = 0;
    peep->var_EF = 0;
    peep->paid_to_enter = 0;
    peep->paid_on_rides = 0;
    peep->paid_on_food = 0;
    peep->paid_on_drink = 0;
    peep->paid_on_souvenirs = 0;
    peep->no_of_food = 0;
    peep->no_of_drinks = 0;
    peep->no_of_souvenirs = 0;
    peep->var_F2 = 0;
    peep->angriness = 0;
    peep->var_F4 = 0;

    uint8 tshirt_colour = peep_rand() % countof(tshirt_colours);
    peep->tshirt_colour = tshirt_colours[tshirt_colour];

    uint8 trousers_colour = peep_rand() % countof(trouser_colours);
    peep->trousers_colour = trouser_colours[trousers_colour];

    /* It looks like 65 is about 50% energy level, so this initialises
     * a peep with approx 50%-100% energy. */
    uint8 energy = (peep_rand() & 0x3F) + 65;
    peep->energy = energy;
    peep->energy_growth_rate = energy;

    if (gParkFlags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES){
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
void get_arguments_from_action(rct_peep* peep, uint32 *argument_1, uint32* argument_2){
    rct_ride *ride;

    switch (peep->state){
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
        ride = get_ride(peep->current_ride);
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE))
            *argument_1 = STR_IN_RIDE;
        *argument_1 |= ((uint32)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_BUYING:
        ride = get_ride(peep->current_ride);
        *argument_1 = STR_AT_RIDE | ((uint32)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_WALKING:
    case PEEP_STATE_USING_BIN:
        if (peep->guest_heading_to_ride_id != 0xFF){
            ride = get_ride(peep->guest_heading_to_ride_id);
            *argument_1 = STR_HEADING_FOR | ((uint32)ride->name << 16);
            *argument_2 = ride->name_arguments;
        }
        else{
            *argument_1 = (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) ? STR_LEAVING_PARK : STR_WALKING;
            *argument_2 = 0;
        }
        break;
    case PEEP_STATE_QUEUING_FRONT:
    case PEEP_STATE_QUEUING:
        ride = get_ride(peep->current_ride);
        *argument_1 = STR_QUEUING_FOR | ((uint32)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_SITTING:
        *argument_1 = STR_SITTING;
        *argument_2 = 0;
        break;
    case PEEP_STATE_WATCHING:
        if (peep->current_ride != 0xFF){
            ride = get_ride(peep->current_ride);
            *argument_1 = STR_WATCHING_RIDE | ((uint32)ride->name << 16);
            *argument_2 = ride->name_arguments;
            if (peep->current_seat & 0x1)
                *argument_1 = STR_WATCHING_CONSTRUCTION_OF | ((uint32)ride->name << 16);
            else
                *argument_1 = STR_WATCHING_RIDE | ((uint32)ride->name << 16);
        }
        else{
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
        if (peep->sub_state == 0){
            *argument_1 = STR_WALKING;
            *argument_2 = 0;
        }
        else if (peep->sub_state == 1){
            *argument_1 = STR_ANSWERING_RADIO_CALL;
            *argument_2 = 0;
        }
        else{
            ride = get_ride(peep->current_ride);
            *argument_1 = STR_RESPONDING_TO_RIDE_BREAKDOWN_CALL | ((uint32)ride->name << 16);
            *argument_2 = ride->name_arguments;
        }
        break;
    case PEEP_STATE_FIXING:
        ride = get_ride(peep->current_ride);
        *argument_1 = STR_FIXING_RIDE | ((uint32)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_HEADING_TO_INSPECTION:
        ride = get_ride(peep->current_ride);
        *argument_1 = STR_HEADING_TO_RIDE_FOR_INSPECTION | ((uint32)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_INSPECTING:
        ride = get_ride(peep->current_ride);
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
void peep_thought_set_format_args(rct_peep_thought *thought)
{
    set_format_arg(0, rct_string_id, PeepThoughts[thought->type]);

    uint8 flags = PeepThoughtToActionMap[thought->type].flags;
    if (flags & 1) {
        rct_ride *ride = get_ride(thought->item);
        set_format_arg(2, rct_string_id, ride->name);
        set_format_arg(4, uint32, ride->name_arguments);
    } else if (flags & 2) {
        set_format_arg(2, rct_string_id, ShopItemStringIds[thought->item].singular);
    } else if (flags & 4) {
        set_format_arg(2, rct_string_id, ShopItemStringIds[thought->item].indefinite);
    } else {
    }
}

/** rct2: 0x00982004 */
static const bool peep_allow_pick_up[] = {
    true,   // PEEP_STATE_FALLING
    false,  // PEEP_STATE_1
    false,  // PEEP_STATE_QUEUING_FRONT
    false,  // PEEP_STATE_ON_RIDE
    false,  // PEEP_STATE_LEAVING_RIDE
    true,   // PEEP_STATE_WALKING
    true,   // PEEP_STATE_QUEUING
    false,  // PEEP_STATE_ENTERING_RIDE
    true,   // PEEP_STATE_SITTING
    true,   // PEEP_STATE_PICKED
    true,   // PEEP_STATE_PATROLLING
    true,   // PEEP_STATE_MOWING
    true,   // PEEP_STATE_SWEEPING
    false,  // PEEP_STATE_ENTERING_PARK
    false,  // PEEP_STATE_LEAVING_PARK
    true,   // PEEP_STATE_ANSWERING
    false,  // PEEP_STATE_FIXING
    false,  // PEEP_STATE_BUYING
    true,   // PEEP_STATE_WATCHING
    true,   // PEEP_STATE_EMPTYING_BIN
    true,   // PEEP_STATE_USING_BIN
    true,   // PEEP_STATE_WATERING
    true,   // PEEP_STATE_HEADING_TO_INSPECTION
    false,  // PEEP_STATE_INSPECTING
};

/**
 *
 *  rct2: 0x00698827
 * returns 1 on pickup (CF not set)
 */
sint32 peep_can_be_picked_up(rct_peep* peep){
    return peep_allow_pick_up[peep->state];
}

enum{
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

const sint32 face_sprite_small[] = {
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

const sint32 face_sprite_large[] = {
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

static sint32 get_face_sprite_offset(rct_peep *peep){

    // ANGRY
    if (peep->angriness > 0) return PEEP_FACE_OFFSET_ANGRY;

    // VERY_VERY_SICK
    if (peep->nausea > 200) return PEEP_FACE_OFFSET_VERY_VERY_SICK;

    // VERY_SICK
    if (peep->nausea > 170) return PEEP_FACE_OFFSET_VERY_SICK;

    // SICK
    if (peep->nausea > 140) return PEEP_FACE_OFFSET_SICK;

    // VERY_TIRED
    if (peep->energy < 46) return PEEP_FACE_OFFSET_VERY_TIRED;

    // TIRED
    if (peep->energy < 70) return PEEP_FACE_OFFSET_TIRED;

    sint32 offset = PEEP_FACE_OFFSET_VERY_VERY_UNHAPPY;
    //There are 7 different happiness based faces
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
sint32 get_peep_face_sprite_small(rct_peep *peep){
    return face_sprite_small[get_face_sprite_offset(peep)];
}

/**
 * Function split into large and small sprite
 *  rct2: 0x00698721
 */
sint32 get_peep_face_sprite_large(rct_peep *peep){
    return face_sprite_large[get_face_sprite_offset(peep)];
}

/**
 *
 *  rct2: 0x0069A5A0
 * tests if a peep's name matches a cheat code, normally returns using a register flag
 * @param index (eax)
 * @param ride (esi)
 */
sint32 peep_check_easteregg_name(sint32 index, rct_peep *peep)
{
    char buffer[256];

    format_string(buffer, 256, peep->name_string_idx, &peep->id);
    return _stricmp(buffer, gPeepEasterEggNames[index]) == 0;
}

sint32 peep_get_easteregg_name_id(rct_peep *peep)
{
    char buffer[256];
    sint32 i;

    format_string(buffer, 256, peep->name_string_idx, &peep->id);

    for (i = 0; i < countof(gPeepEasterEggNames); i++)
        if (_stricmp(buffer, gPeepEasterEggNames[i]) == 0)
            return i;

    return -1;

}

sint32 peep_is_mechanic(rct_peep *peep)
{
    return (
        peep->sprite_identifier == SPRITE_IDENTIFIER_PEEP &&
        peep->type == PEEP_TYPE_STAFF &&
        peep->staff_type == STAFF_TYPE_MECHANIC
    );
}

bool peep_has_item(rct_peep *peep, sint32 peepItem)
{
    if (peepItem < 32) {
        return peep->item_standard_flags & (1u << peepItem);
    } else {
        return peep->item_extra_flags & (1u << (peepItem - 32));
    }
}

static sint32 peep_has_food_standard_flag(rct_peep* peep){
    return peep->item_standard_flags &(
        PEEP_ITEM_DRINK |
        PEEP_ITEM_BURGER |
        PEEP_ITEM_CHIPS |
        PEEP_ITEM_ICE_CREAM |
        PEEP_ITEM_CANDYFLOSS |
        PEEP_ITEM_PIZZA |
        PEEP_ITEM_POPCORN |
        PEEP_ITEM_HOT_DOG |
        PEEP_ITEM_TENTACLE |
        PEEP_ITEM_CANDY_APPLE |
        PEEP_ITEM_DONUT |
        PEEP_ITEM_COFFEE |
        PEEP_ITEM_CHICKEN |
        PEEP_ITEM_LEMONADE);
}

static sint32 peep_has_food_extra_flag(rct_peep* peep){
    return peep->item_extra_flags &(
        PEEP_ITEM_PRETZEL |
        PEEP_ITEM_CHOCOLATE |
        PEEP_ITEM_ICED_TEA |
        PEEP_ITEM_FUNNEL_CAKE |
        PEEP_ITEM_BEEF_NOODLES |
        PEEP_ITEM_FRIED_RICE_NOODLES |
        PEEP_ITEM_WONTON_SOUP |
        PEEP_ITEM_MEATBALL_SOUP |
        PEEP_ITEM_FRUIT_JUICE |
        PEEP_ITEM_SOYBEAN_MILK |
        PEEP_ITEM_SU_JONGKWA |
        PEEP_ITEM_SUB_SANDWICH |
        PEEP_ITEM_COOKIE |
        PEEP_ITEM_ROAST_SAUSAGE
        );
}

/**
 * To simplify check of 0x36BA3E0 and 0x11FF78
 * returns 0 on no food.
 */
sint32 peep_has_food(rct_peep* peep){
    return peep_has_food_standard_flag(peep) ||
        peep_has_food_extra_flag(peep);
}

static sint32 peep_has_drink_standard_flag(rct_peep* peep){
    return peep->item_standard_flags &(
        PEEP_ITEM_DRINK |
        PEEP_ITEM_COFFEE |
        PEEP_ITEM_LEMONADE);
}

static sint32 peep_has_drink_extra_flag(rct_peep* peep){
    return peep->item_extra_flags &(
        PEEP_ITEM_CHOCOLATE |
        PEEP_ITEM_ICED_TEA |
        PEEP_ITEM_FRUIT_JUICE |
        PEEP_ITEM_SOYBEAN_MILK |
        PEEP_ITEM_SU_JONGKWA
        );
}

/**
 * To simplify check of NOT(0x12BA3C0 and 0x118F48)
 * returns 0 on no food.
 */
static sint32 peep_has_drink(rct_peep* peep){
    return peep_has_drink_standard_flag(peep) ||
        peep_has_drink_extra_flag(peep);
}

static sint32 peep_empty_container_standard_flag(rct_peep* peep){
    return peep->item_standard_flags &(
        PEEP_ITEM_EMPTY_CAN |
        PEEP_ITEM_EMPTY_BURGER_BOX |
        PEEP_ITEM_EMPTY_CUP |
        PEEP_ITEM_RUBBISH |
        PEEP_ITEM_EMPTY_BOX |
        PEEP_ITEM_EMPTY_BOTTLE
        );
}

static sint32 peep_empty_container_extra_flag(rct_peep* peep){
    return peep->item_extra_flags &(
        PEEP_ITEM_EMPTY_BOWL_RED |
        PEEP_ITEM_EMPTY_DRINK_CARTON |
        PEEP_ITEM_EMPTY_JUICE_CUP |
        PEEP_ITEM_EMPTY_BOWL_BLUE
        );
}

static sint32 peep_has_empty_container(rct_peep* peep){
    return peep_empty_container_standard_flag(peep) ||
        peep_empty_container_extra_flag(peep);
}

/* Simplifies 0x690582. Returns 1 if should find bench*/
static sint32 peep_should_find_bench(rct_peep* peep){
    if (!(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)){
        if (peep_has_food(peep)){
            if (peep->hunger < 128 || peep->happiness < 128){
                if (!(peep->next_var_29 & 0x1C)){
                    return 1;
                }
            }
        }
        if (peep->nausea <= 170 && peep->energy > 50){
            return 0;
        }

        if (!(peep->next_var_29 & 0x1C)){
            return 1;
        }
    }
    return 0;
}

/**
 *
 *  rct2: 0x699F5A
 * al:thought_type
 * ah:thought_arguments
 * esi: peep
 */
void peep_insert_new_thought(rct_peep *peep, uint8 thought_type, uint8 thought_arguments)
{
    uint8 action = PeepThoughtToActionMap[thought_type].action;
    if (action != 0xFF && peep->action >= 254){
            peep->action = action;
            peep->action_frame = 0;
            peep->action_sprite_image_offset = 0;
            peep_update_current_action_sprite_type(peep);
            invalidate_sprite_2((rct_sprite*)peep);
    }

    for (sint32 i = 0; i < PEEP_MAX_THOUGHTS; ++i){
        rct_peep_thought* thought = &peep->thoughts[i];
        // Remove the oldest thought by setting it to NONE.
        if (thought->type == PEEP_THOUGHT_TYPE_NONE) break;

        if (thought->type == thought_type && thought->item == thought_arguments){
            // If the thought type has not changed then we need to move
            // it to the top of the thought list. This is done by first removing the
            // existing thought and placing it at the top.
            if (i < PEEP_MAX_THOUGHTS - 2) {
                memmove(thought, thought + 1, sizeof(rct_peep_thought)*(PEEP_MAX_THOUGHTS - i - 1));
            }
            break;
        }
    }

    memmove(&peep->thoughts[1], &peep->thoughts[0], sizeof(rct_peep_thought)*(PEEP_MAX_THOUGHTS - 1));

    peep->thoughts[0].type = thought_type;
    peep->thoughts[0].item = thought_arguments;
    peep->thoughts[0].var_2 = 0;
    peep->thoughts[0].var_3 = 0;

    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
}

/**
 *
 *  rct2: 0x00699FE3
 * Stops peeps that are having thoughts
 * such as "I'm hungry" after visiting a food shop.
 * Works for Thirst/Hungry/Low Money/Bathroom
 */
static void peep_stop_purchase_thought(rct_peep* peep, uint8 ride_type){

    uint8 thought_type = PEEP_THOUGHT_TYPE_HUNGRY;

    if (!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_SELLS_FOOD)){
        thought_type = PEEP_THOUGHT_TYPE_THIRSTY;
        if (!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_SELLS_DRINKS)){
            thought_type = PEEP_THOUGHT_TYPE_RUNNING_OUT;
            if (ride_type != RIDE_TYPE_CASH_MACHINE){
                thought_type = PEEP_THOUGHT_TYPE_BATHROOM;
                if (!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_IS_BATHROOM)){
                    return;
                }
            }
        }
    }

    //Remove the related thought
    for (sint32 i = 0; i < PEEP_MAX_THOUGHTS; ++i){
        rct_peep_thought* thought = &peep->thoughts[i];

        if (thought->type == PEEP_THOUGHT_TYPE_NONE) break;

        if (thought->type != thought_type)continue;


        if (i < PEEP_MAX_THOUGHTS - 1) {
            memmove(thought, thought + 1, sizeof(rct_peep_thought)*(PEEP_MAX_THOUGHTS - i - 1));
        }

        peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;

        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
        i--;
    }
}

void peep_set_map_tooltip(rct_peep *peep)
{
    if (peep->type == PEEP_TYPE_GUEST) {
        set_map_tooltip_format_arg(0, rct_string_id, (peep->peep_flags & PEEP_FLAGS_TRACKING) ? STR_TRACKED_GUEST_MAP_TIP : STR_GUEST_MAP_TIP);
        set_map_tooltip_format_arg(2, uint32, get_peep_face_sprite_small(peep));
        set_map_tooltip_format_arg(6, rct_string_id, peep->name_string_idx);
        set_map_tooltip_format_arg(8, uint32, peep->id);

        uint32 arg0, arg1;
        get_arguments_from_action(peep, &arg0, &arg1);
        set_map_tooltip_format_arg(12, uint32, arg0);
        set_map_tooltip_format_arg(16, uint32, arg1);
    } else {
        set_map_tooltip_format_arg(0, rct_string_id, STR_STAFF_MAP_TIP);
        set_map_tooltip_format_arg(2, rct_string_id, peep->name_string_idx);
        set_map_tooltip_format_arg(4, uint32, peep->id);

        uint32 arg0, arg1;
        get_arguments_from_action(peep, &arg0, &arg1);
        set_map_tooltip_format_arg(8, uint32, arg0);
        set_map_tooltip_format_arg(12, uint32, arg1);
    }
}

/**
*  rct2: 0x00693BAB
*/
void peep_switch_to_next_action_sprite_type(rct_peep* peep) {
    // TBD: Add nextActionSpriteType as function parameter and make peep->next_action_sprite_type obsolete?
    uint8 nextActionSpriteType = peep->next_action_sprite_type;
    if (nextActionSpriteType != peep->action_sprite_type) {
        invalidate_sprite_2((rct_sprite*)peep);
        peep->action_sprite_type = nextActionSpriteType;
        const rct_sprite_bounds* spriteBounds = g_peep_animation_entries[peep->sprite_type].sprite_bounds;
        peep->sprite_width = spriteBounds[nextActionSpriteType].sprite_width;
        peep->sprite_height_negative = spriteBounds[nextActionSpriteType].sprite_height_negative;
        peep->sprite_height_positive = spriteBounds[nextActionSpriteType].sprite_height_positive;
        invalidate_sprite_2((rct_sprite*)peep);
    }
}

/**
 *
 *  rct2: 0x00693CBB
 */
static sint32 peep_update_queue_position(rct_peep* peep, uint8 previous_action){
    peep->time_in_queue++;
    if (peep->next_in_queue == SPRITE_INDEX_NULL)
        return 0;

    rct_peep* peep_next = GET_PEEP(peep->next_in_queue);

    sint16 x_diff = abs(peep_next->x - peep->x);
    sint16 y_diff = abs(peep_next->y - peep->y);
    sint16 z_diff = abs(peep_next->z - peep->z);

    if (z_diff > 10)
        return 0;

    if (x_diff < y_diff){
        sint16 temp_x = x_diff;
        x_diff = y_diff;
        y_diff = temp_x;
    }

    x_diff += y_diff / 2;
    if (x_diff > 7){
        if (x_diff > 13){
            if ((peep->x & 0xFFE0) != (peep_next->x & 0xFFE0) ||
                (peep->y & 0xFFE0) != (peep_next->y & 0xFFE0))
                return 0;
        }

        if (peep->sprite_direction != peep_next->sprite_direction)
            return 0;

        switch (peep_next->sprite_direction / 8){
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
        peep_update_action(&x, &y, &xy_dist, peep);

    if (peep->action != PEEP_ACTION_NONE_2)
        return 1;

    peep->action = PEEP_ACTION_NONE_1;
    peep->next_action_sprite_type = 2;
    if (previous_action != PEEP_ACTION_NONE_1)
        invalidate_sprite_2((rct_sprite*)peep);
    return 1;
}

/**
 *
 *  rct2: 0x00693EF2
 */
static sint32 peep_return_to_center_of_tile(rct_peep* peep){
    peep->direction ^= (1 << 1);
    peep->destination_x = (peep->x & 0xFFE0) + 16;
    peep->destination_y = (peep->y & 0xFFE0) + 16;
    peep->destination_tolerence = 5;
    return 1;
}

/**
 *
 *  rct2: 0x00693f2C
 */
static sint32 peep_interact_with_entrance(rct_peep* peep, sint16 x, sint16 y, rct_map_element* map_element){
    uint8 entranceType = map_element->properties.entrance.type;
    uint8 rideIndex = map_element->properties.entrance.ride_index;

    // Store some details to determine when to override the default
    // behaviour (defined below) for when staff attempt to enter a ride
    // to fix/inspect it.
    if (entranceType == ENTRANCE_TYPE_RIDE_EXIT){
        _unk_F1EE18 |= F1EE18_RIDE_EXIT;
        _peepRideEntranceExitElement = map_element;
    }
    else if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE){
        _unk_F1EE18 |= F1EE18_RIDE_ENTRANCE;
        _peepRideEntranceExitElement = map_element;
    }

    if (entranceType == ENTRANCE_TYPE_RIDE_EXIT){
        // Default guest/staff behaviour attempting to enter a
        // ride exit is to turn around.
        peep->interaction_ride_index = 0xFF;
        return peep_return_to_center_of_tile(peep);
    }

    if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE){
        if (peep->type == PEEP_TYPE_STAFF){
            // Default staff behaviour attempting to enter a
            // ride entrance is to turn around.
            peep->interaction_ride_index = 0xFF;
            return peep_return_to_center_of_tile(peep);
        }

        if (peep->state == PEEP_STATE_QUEUING){
            // Guest is in the ride queue.
            peep->sub_state = 11;
            peep->action_sprite_image_offset = _unk_F1AEF0;
            return 1;
        }

        // Guest is on a normal path, i.e. ride has no queue.
        if (peep->interaction_ride_index == rideIndex)
            // Peep is retrying the ride entrance without leaving
            // the path tile and without trying any other ride
            // attached to this path tile. i.e. stick with the
            // peeps previous decision not to go on the ride.
            return peep_return_to_center_of_tile(peep);

        peep->var_F4 = 0;
        uint8 stationNum = (map_element->properties.entrance.index >> 4) & 0x7;
        // Guest walks up to the ride for the first time since entering
        // the path tile or since considering another ride attached to
        // the path tile.
        if (!peep_should_go_on_ride(peep, rideIndex, stationNum, 0)){
            // Peep remembers that this is the last ride they
            // considered while on this path tile.
            peep->interaction_ride_index = rideIndex;
            return peep_return_to_center_of_tile(peep);
        }

        // Guest has decided to go on the ride.
        peep->action_sprite_image_offset = _unk_F1AEF0;
        peep->interaction_ride_index = rideIndex;

        rct_ride* ride = get_ride(rideIndex);
        uint16 previous_last = ride->last_peep_in_queue[stationNum];
        ride->last_peep_in_queue[stationNum] = peep->sprite_index;
        peep->next_in_queue = previous_last;
        ride->queue_length[stationNum]++;

        peep_decrement_num_riders(peep);
        peep->current_ride = rideIndex;
        peep->current_ride_station = stationNum;
        peep->state = PEEP_STATE_QUEUING;
        peep->days_in_queue = 0;
        peep_window_state_update(peep);
        peep->sub_state = 11;
        peep->time_in_queue = 0;
        if (peep->peep_flags & PEEP_FLAGS_TRACKING){
            set_format_arg(0, rct_string_id, peep->name_string_idx);
            set_format_arg(2, uint32, peep->id);
            set_format_arg(6, rct_string_id, ride->name);
            set_format_arg(8, uint32, ride->name_arguments);
            if (gConfigNotifications.guest_queuing_for_ride) {
                news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, peep->sprite_index);
            }
        }
        return 1;
    }
    else{
        // PARK_ENTRANCE
        if (peep->type == PEEP_TYPE_STAFF)
            // Staff cannot leave the park, so go back.
            return  peep_return_to_center_of_tile(peep);

        // If not the center of the entrance arch
        if (map_element->properties.entrance.index & 0xF)
            return peep_return_to_center_of_tile(peep);

        uint8 entranceDirection = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
        if (entranceDirection != peep->direction){
            if ((entranceDirection ^ (1 << 1)) != peep->direction)
                return peep_return_to_center_of_tile(peep);
            // Peep is leaving the park.
            if (peep->state != PEEP_STATE_WALKING)
                return peep_return_to_center_of_tile(peep);

            if (!(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)){
                // If the park is open and leaving flag isn't set return to center
                if (gParkFlags & PARK_FLAGS_PARK_OPEN)
                    return peep_return_to_center_of_tile(peep);
            }

            peep->destination_x += TileDirectionDelta[peep->direction].x;
            peep->destination_y += TileDirectionDelta[peep->direction].y;
            peep->destination_tolerence = 9;
            invalidate_sprite_2((rct_sprite*)peep);
            sprite_move(x, y, peep->z, (rct_sprite*)peep);
            invalidate_sprite_2((rct_sprite*)peep);

            peep_decrement_num_riders(peep);
            peep->state = PEEP_STATE_LEAVING_PARK;
            peep_window_state_update(peep);

            peep->var_37 = 0;
            if (peep->peep_flags & PEEP_FLAGS_TRACKING){
                set_format_arg(0, rct_string_id, peep->name_string_idx);
                set_format_arg(2, uint32, peep->id);
                if (gConfigNotifications.guest_left_park) {
                    news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_LEFT_PARK, peep->sprite_index);
                }
            }
            return 1;
        }

        // Peep is entering the park.

        if (peep->state != PEEP_STATE_ENTERING_PARK)
            return peep_return_to_center_of_tile(peep);

        if (!(gParkFlags & PARK_FLAGS_PARK_OPEN)){
            peep->state = PEEP_STATE_LEAVING_PARK;
            peep->var_37 = 1;
            decrement_guests_heading_for_park();
            peep_window_state_update(peep);
            return peep_return_to_center_of_tile(peep);
        }

        uint8 entranceIndex = 0;
        for (entranceIndex = 0; entranceIndex < MAX_PARK_ENTRANCES; entranceIndex++) {
            if (gParkEntrances[entranceIndex].x == (x & 0xFFE0) &&
                gParkEntrances[entranceIndex].y == (y & 0xFFE0))
                break;
        }

        sint16 z = gParkEntrances[entranceIndex].z / 8;
        entranceDirection = gParkEntrances[entranceIndex].direction;

        sint16 next_x = (x & 0xFFE0) + TileDirectionDelta[entranceDirection].x;
        sint16 next_y = (y & 0xFFE0) + TileDirectionDelta[entranceDirection].y;

        uint8 found = 0;
        rct_map_element* nextMapElement = map_get_first_element_at(next_x / 32, next_y / 32);
        do{
            if (map_element_get_type(nextMapElement) != MAP_ELEMENT_TYPE_PATH)
                continue;

            if (nextMapElement->type & 1)
                continue;

            if (footpath_element_is_sloped(nextMapElement)){
                uint8 slopeDirection = footpath_element_get_slope_direction(nextMapElement);
                if (slopeDirection == entranceDirection){
                    if (z != nextMapElement->base_height){
                        continue;
                    }
                    found = 1;
                    break;
                }

                if ((slopeDirection ^ (1 << 1)) != entranceDirection)
                    continue;

                if (z - 2 != nextMapElement->base_height)
                    continue;
                found = 1;
                break;
            }
            else{
                if (z != nextMapElement->base_height){
                    continue;
                }
                found = 1;
                break;
            }
        } while (!map_element_is_last_for_tile(nextMapElement++));

        if (!found){
            peep->state = PEEP_STATE_LEAVING_PARK;
            peep->var_37 = 1;
            decrement_guests_heading_for_park();
            peep_window_state_update(peep);
            return peep_return_to_center_of_tile(peep);
        }

        money16 entranceFee = park_get_entrance_fee();
        if (entranceFee != 0) {
            if (peep->item_standard_flags & PEEP_ITEM_VOUCHER){
                if (peep->voucher_type == VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE){
                    entranceFee /= 2;
                    peep->item_standard_flags &= ~PEEP_ITEM_VOUCHER;
                    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                }
                else if (peep->voucher_type == VOUCHER_TYPE_PARK_ENTRY_FREE){
                    entranceFee = 0;
                    peep->item_standard_flags &= ~PEEP_ITEM_VOUCHER;
                    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                }
            }
            if (entranceFee > peep->cash_in_pocket){
                peep->state = PEEP_STATE_LEAVING_PARK;
                peep->var_37 = 1;
                decrement_guests_heading_for_park();
                peep_window_state_update(peep);
                return peep_return_to_center_of_tile(peep);
            }

            gTotalIncomeFromAdmissions += entranceFee;
            gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_ENTRANCE_TICKETS;
            peep_spend_money(peep, &peep->paid_to_enter, entranceFee);
            peep->peep_flags |= PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY;
        }

        gTotalAdmissions++;
        window_invalidate_by_number(WC_PARK_INFORMATION, 0);

        peep->var_37 = 1;
        peep->destination_x += TileDirectionDelta[peep->direction].x;
        peep->destination_y += TileDirectionDelta[peep->direction].y;
        peep->destination_tolerence = 7;

        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, peep->z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);

        return 1;
    }
}

/**
 *
 *  rct2: 0x006946D8
 */
static sint32 peep_footpath_move_forward(rct_peep* peep, sint16 x, sint16 y, rct_map_element* map_element, bool vandalism){
    peep->next_x = (x & 0xFFE0);
    peep->next_y = (y & 0xFFE0);
    peep->next_z = map_element->base_height;
    peep->next_var_29 = map_element->properties.path.type & 7;

    sint16 z = peep_get_height_on_slope(peep, x, y);

    if (peep->type == PEEP_TYPE_STAFF){
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return 1;
    }

    uint8 var_EF = (peep->var_EF * 2) & 0x3F;
    peep->var_EF &= 0xC0;
    peep->var_EF |= var_EF;

    if (vandalism == true){
        peep->var_EF |= 1;
        if (peep->var_EF & 0x3E &&
            !(peep->var_EF & 0xC0)){

            if ((peep_rand() & 0xFFFF) <= 10922){
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_VANDALISM, 0xFF);
                peep->happiness_growth_rate = max(0, peep->happiness_growth_rate - 17);
            }
            peep->var_EF |= 0xC0;
        }
    }

    if (peep->var_EF & 0xC0 &&
        (peep_rand()&0xFFFF) <= 4369){
        peep->var_EF -= 0x40;
    }

    uint16 crowded = 0;
    uint8 litter_count = 0;
    uint8 sick_count = 0;
    uint16 sprite_id = sprite_get_first_in_quadrant(x, y);
    for (rct_sprite* sprite; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant){
        sprite = get_sprite(sprite_id);
        if (sprite->unknown.sprite_identifier == SPRITE_IDENTIFIER_PEEP){
            rct_peep* other_peep = (rct_peep*)sprite;
            if (other_peep->state != PEEP_STATE_WALKING)
                continue;

            if (abs(other_peep->z - peep->next_z * 8) > 16)
                continue;
            crowded++;
            continue;
        }
        else if (sprite->unknown.sprite_identifier == SPRITE_IDENTIFIER_LITTER){
            rct_litter* litter = (rct_litter*)sprite;
            if (abs(litter->z - peep->next_z * 8) > 16)
                continue;

            litter_count++;
            if (litter->type != LITTER_TYPE_SICK && litter->type != LITTER_TYPE_SICK_ALT)
                continue;

            litter_count--;
            sick_count++;
        }
    }

    if (crowded >= 10 &&
        peep->state == PEEP_STATE_WALKING &&
        (peep_rand() & 0xFFFF) <= 21845){

        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CROWDED, 0xFF);
        peep->happiness_growth_rate = max(0, peep->happiness_growth_rate - 14);
    }

    litter_count = min(3, litter_count);
    sick_count = min(3, sick_count);

    uint8 disgusting_time = peep->disgusting_count & 0xC0;
    uint8 disgusting_count = ((peep->disgusting_count & 0xF) << 2) | sick_count;
    peep->disgusting_count = disgusting_count | disgusting_time;

    if (disgusting_time & 0xC0 &&
        (peep_rand() & 0xFFFF) <= 4369){
        // Reduce the disgusting time
        peep->disgusting_count -= 0x40;
    }
    else{
        uint8 total_sick = 0;
        for (uint8 time = 0; time < 3; time++){
            total_sick += (disgusting_count >> (2 * time)) & 0x3;
        }

        if (total_sick >= 3 &&
            (peep_rand() & 0xFFFF) <= 10922){
            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_PATH_DISGUSTING, 0xFF);
            peep->happiness_growth_rate = max(0, peep->happiness_growth_rate - 17);
            // Reset disgusting time
            peep->disgusting_count |= 0xC0;
        }
    }

    uint8 litter_time = peep->litter_count & 0xC0;
    litter_count = ((peep->litter_count & 0xF) << 2) | litter_count;
    peep->litter_count = litter_count | litter_time;

    if (litter_time & 0xC0 &&
        (peep_rand() & 0xFFFF) <= 4369){
        // Reduce the litter time
        peep->litter_count -= 0x40;
    }
    else{
        uint8 total_litter = 0;
        for (uint8 time = 0; time < 3; time++){
            total_litter += (litter_count >> (2 * time)) & 0x3;
        }

        if (total_litter >= 3 &&
            (peep_rand() & 0xFFFF) <= 10922){
            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_BAD_LITTER, 0xFF);
            peep->happiness_growth_rate = max(0, peep->happiness_growth_rate - 17);
            // Reset litter time
            peep->litter_count |= 0xC0;
        }
    }

    invalidate_sprite_2((rct_sprite*)peep);
    sprite_move(x, y, z, (rct_sprite*)peep);
    invalidate_sprite_2((rct_sprite*)peep);
    return 1;
}

/**
 *
 *  rct2: 0x0069455E
 */
static sint32 peep_interact_with_path(rct_peep* peep, sint16 x, sint16 y, rct_map_element* map_element){

    // 0x00F1AEE2
    bool vandalism_present = false;
    if (footpath_element_has_path_scenery(map_element) &&
        (map_element->flags & MAP_ELEMENT_FLAG_BROKEN) &&
        (map_element->properties.path.edges & 0xF) != 0xF){
        vandalism_present = 1;
    }

    sint16 z = map_element->base_height * 8;
    if (!map_is_location_owned(x, y, z)){
        if (peep->outside_of_park == 0)
            return peep_return_to_center_of_tile(peep);
    }
    else{
        if (peep->outside_of_park == 1)
            return peep_return_to_center_of_tile(peep);
    }

    if (peep->type == PEEP_TYPE_GUEST &&
        footpath_element_is_queue(map_element)){

        uint8 rideIndex = map_element->properties.path.ride_index;

        // Check if this queue path tile is connected to a ride.
        if (rideIndex == 0xFF){
            // Queue is not connected to a ride.
            peep->interaction_ride_index = 0xFF;
            return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
        }

        // Queue is connected to a ride.
        if (peep->state == PEEP_STATE_QUEUING){
            // Check if this queue is connected to the ride the
            // peep is queuing for, i.e. the player hasn't edited
            // the queue.
            if (peep->current_ride == rideIndex){
                return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
            }
            // Queue got disconnected from the ride.
            remove_peep_from_queue(peep);
            peep_decrement_num_riders(peep);
            peep->state = PEEP_STATE_1;
            peep_window_state_update(peep);
            return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
        }

        // One of two cases applies here:
        // 1. Peep is in the ride queue but no longer queuing.
        //    This usually means the ride was closed.
        // 2. Peep is walking up to the queue entrance from
        //    a normal path tile.

        if (peep->interaction_ride_index == rideIndex){
            // Case 1 above applies, so walk the queue.
            return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
        }

        // Case 2 above applies - decide whether to go on the ride.
        peep->var_F4 = 0;
        uint8 stationNum = (map_element->properties.path.additions & 0x70) >> 4;
        if (!peep_should_go_on_ride(peep, rideIndex, stationNum, PEEP_RIDE_DECISION_AT_QUEUE)){
            // Peep has decided not to go on the ride.
            return peep_return_to_center_of_tile(peep);
        }

        // Peep has decided to go on the ride at the queue.
        // Set the following so the peep will correctly walk up
        // and back down the queue if the ride is closed
        // while they are queuing.
        peep->interaction_ride_index = rideIndex;
        rct_ride* ride = get_ride(rideIndex);

        // Add the peep to the ride queue.
        uint16 old_last_peep = ride->last_peep_in_queue[stationNum];
        ride->last_peep_in_queue[stationNum] = peep->sprite_index;
        peep->next_in_queue = old_last_peep;
        ride->queue_length[stationNum]++;

        peep_decrement_num_riders(peep);
        peep->current_ride = rideIndex;
        peep->current_ride_station = stationNum;
        peep->state = PEEP_STATE_QUEUING;
        peep->days_in_queue = 0;
        peep_window_state_update(peep);

        peep->sub_state = 10;
        peep->destination_tolerence = 2;
        peep->time_in_queue = 0;
        if (peep->peep_flags & PEEP_FLAGS_TRACKING){
            set_format_arg(0, rct_string_id, peep->name_string_idx);
            set_format_arg(2, uint32, peep->id);
            set_format_arg(6, rct_string_id, ride->name);
            set_format_arg(8, uint32, ride->name_arguments);
            if (gConfigNotifications.guest_queuing_for_ride) {
                news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, peep->sprite_index);
            }
        }

        return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
    }
    else{
        peep->interaction_ride_index = 0xFF;
        if (peep->state == PEEP_STATE_QUEUING){
            remove_peep_from_queue(peep);
            peep_decrement_num_riders(peep);
            peep->state = PEEP_STATE_1;
            peep_window_state_update(peep);
        }
        return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
    }
}

/**
 *
 *  rct2: 0x00693F70
 */
static sint32 peep_interact_with_shop(rct_peep* peep, sint16 x, sint16 y, rct_map_element* map_element){
    uint8 rideIndex = map_element->properties.track.ride_index;
    rct_ride* ride = get_ride(rideIndex);

    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
        return 0;

    if (peep->type == PEEP_TYPE_STAFF)
        return peep_return_to_center_of_tile(peep);

    peep->var_F4 = 0;

    if (ride->status != RIDE_STATUS_OPEN)
        return peep_return_to_center_of_tile(peep);

    if (peep->interaction_ride_index == rideIndex)
        return peep_return_to_center_of_tile(peep);

    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
        return peep_return_to_center_of_tile(peep);

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY)){
        peep->var_F4 = 0;
        if (!peep_should_go_on_ride(peep, rideIndex, 0, 0))
            return peep_return_to_center_of_tile(peep);

        money16 cost = ride->price;
        if (cost != 0 && !(gParkFlags & PARK_FLAGS_NO_MONEY)) {
            ride->total_profit += cost;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
            gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS;
            peep_spend_money(peep, NULL, cost);
        }
        peep->destination_x = (x & 0xFFE0) + 16;
        peep->destination_y = (y & 0xFFE0) + 16;
        peep->destination_tolerence = 3;

        peep_decrement_num_riders(peep);
        peep->current_ride = rideIndex;
        peep->state = PEEP_STATE_ENTERING_RIDE;
        peep->sub_state = 19;
        peep_window_state_update(peep);

        peep->time_on_ride = 0;
        ride->cur_num_customers++;
        if (peep->peep_flags & PEEP_FLAGS_TRACKING){
            set_format_arg(0, rct_string_id, peep->name_string_idx);
            set_format_arg(2, uint32, peep->id);
            set_format_arg(6, rct_string_id, ride->name);
            set_format_arg(8, uint32, ride->name_arguments);
            rct_string_id string_id = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE) ? STR_PEEP_TRACKING_PEEP_IS_IN_X : STR_PEEP_TRACKING_PEEP_IS_ON_X;
            if (gConfigNotifications.guest_used_facility) {
                news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, string_id, peep->sprite_index);
            }
        }
        return 1;
    }
    else{
        if (peep->guest_heading_to_ride_id == rideIndex)
            peep->guest_heading_to_ride_id = 0xFF;
        peep->action_sprite_image_offset = _unk_F1AEF0;
        peep_decrement_num_riders(peep);
        peep->current_ride = rideIndex;
        peep->state = PEEP_STATE_BUYING;
        peep->sub_state = 0;
        peep_window_state_update(peep);
        return 1;
    }
}

/**
 *
 *  rct2: 0x0069524E
 */
static sint32 peep_move_one_tile(uint8 direction, rct_peep* peep){
    assert(direction <= 3);
    sint16 x = peep->next_x;
    sint16 y = peep->next_y;
    x += TileDirectionDelta[direction].x;
    y += TileDirectionDelta[direction].y;

    if (x >= 8192 || y >= 8192){
        // This could loop!
        return guest_surface_path_finding(peep);
    }

    peep->direction = direction;
    peep->destination_x = x + 16;
    peep->destination_y = y + 16;
    peep->destination_tolerence = 2;
    if (peep->state != PEEP_STATE_QUEUING){
        peep->destination_tolerence = (peep_rand() & 7) + 2;
    }
    return 0;
}

/**
 *
 *  rct2: 0x00694C41
 */
static sint32 guest_surface_path_finding(rct_peep* peep){
    sint16 x = peep->next_x;
    sint16 y = peep->next_y;
    sint16 z = peep->next_z;
    uint8 randDirection = peep_rand() & 3;

    if (!fence_in_the_way(x, y, z, z + 4, randDirection)){
        x += TileDirectionDelta[randDirection].x;
        y += TileDirectionDelta[randDirection].y;
        uint8 backwardsDirection = randDirection ^ (1 << 1);

        if (!fence_in_the_way(x, y, z, z + 4, backwardsDirection)){
            if (!map_surface_is_blocked(x, y)){
                return peep_move_one_tile(randDirection, peep);
            }
        }
    }

    randDirection++;
    uint8 rand_backwards = peep_rand() & 1;
    if (rand_backwards){
        randDirection -= 2;
    }
    randDirection &= 3;

    x = peep->next_x;
    y = peep->next_y;
    if (!fence_in_the_way(x, y, z, z + 4, randDirection)){
        x += TileDirectionDelta[randDirection].x;
        y += TileDirectionDelta[randDirection].y;
        uint8 backwardsDirection = randDirection ^ (1 << 1);

        if (!fence_in_the_way(x, y, z, z + 4, backwardsDirection)){
            if (!map_surface_is_blocked(x, y)){
                return peep_move_one_tile(randDirection, peep);
            }
        }
    }

    randDirection -= 2;
    randDirection &= 3;

    x = peep->next_x;
    y = peep->next_y;
    if (!fence_in_the_way(x, y, z, z + 4, randDirection)){
        x += TileDirectionDelta[randDirection].x;
        y += TileDirectionDelta[randDirection].y;
        uint8 backwardsDirection = randDirection ^ (1 << 1);

        if (!fence_in_the_way(x, y, z, z + 4, backwardsDirection)){
            if (!map_surface_is_blocked(x, y)){
                return peep_move_one_tile(randDirection, peep);
            }
        }
    }

    randDirection--;
    if (rand_backwards){
        randDirection += 2;
    }
    randDirection &= 3;
    return peep_move_one_tile(randDirection, peep);
}

static rct_map_element* get_banner_on_path(rct_map_element *path_element)
{
    // This is an improved version of original.
    // That only checked for one fence in the way.
    if (map_element_is_last_for_tile(path_element))
        return NULL;

    rct_map_element *bannerElement = path_element + 1;
    do {
        // Path on top, so no banners
        if (map_element_get_type(bannerElement) == MAP_ELEMENT_TYPE_PATH)
            return NULL;
        // Found a banner
        if (map_element_get_type(bannerElement) == MAP_ELEMENT_TYPE_BANNER)
            return bannerElement;
        // Last element so there cant be any other banners
        if (map_element_is_last_for_tile(bannerElement))
            return NULL;

    } while (bannerElement++);

    return NULL;
}

static sint32 banner_clear_path_edges(rct_map_element *mapElement, sint32 edges)
{
    if (_peepPathFindIsStaff) return edges;
    rct_map_element *bannerElement = get_banner_on_path(mapElement);
    if (bannerElement != NULL) {
        do {
            edges &= bannerElement->properties.banner.flags;
        } while ((bannerElement = get_banner_on_path(bannerElement)) != NULL);
    }
    return edges;
}

/**
 * Gets the connected edges of a path that are permitted (i.e. no 'no entry' signs)
 */
static sint32 path_get_permitted_edges(rct_map_element *mapElement)
{
    return banner_clear_path_edges(mapElement, mapElement->properties.path.edges) & 0x0F;
}

static bool is_valid_path_z_and_direction(rct_map_element *mapElement, sint32 currentZ, sint32 currentDirection)
{
    if (footpath_element_is_sloped(mapElement)) {
        sint32 slopeDirection = footpath_element_get_slope_direction(mapElement);
        if (slopeDirection == currentDirection) {
            if (currentZ != mapElement->base_height) return false;
        } else {
            slopeDirection ^= 2;
            if (slopeDirection != currentDirection) return false;
            if (currentZ != mapElement->base_height + 2) return false;
        }
    } else {
        if (currentZ != mapElement->base_height) return false;
    }
    return true;
}

/**
 *
 * Returns:
 *   1 - PATH_SEARCH_WIDE (path with wide flag set)
 *   4 - PATH_SEARCH_RIDE_QUEUE (queue path connected to a ride)
 *   11 - PATH_SEARCH_OTHER (other path than the above)
 *   12 - PATH_SEARCH_FAILED (no path element found)
 *
 *  rct2: 0x00694BAE
 *
 * Returns the type of the next footpath tile a peep can get to from x,y,z /
 * inputMapElement in the given direction.
 */
static uint8 footpath_element_next_in_direction(sint16 x, sint16 y, sint16 z, rct_map_element *mapElement, uint8 chosenDirection)
{
    rct_map_element *nextMapElement;

    if (footpath_element_is_sloped(mapElement)) {
        if (footpath_element_get_slope_direction(mapElement) == chosenDirection) {
            z += 2;
        }
    }

    x += TileDirectionDelta[chosenDirection].x;
    y += TileDirectionDelta[chosenDirection].y;
    nextMapElement = map_get_first_element_at(x / 32, y / 32);
    do {
        if (nextMapElement->flags & MAP_ELEMENT_FLAG_GHOST) continue;
        if (map_element_get_type(nextMapElement) != MAP_ELEMENT_TYPE_PATH) continue;
        if (!is_valid_path_z_and_direction(nextMapElement, z, chosenDirection)) continue;
        if (footpath_element_is_wide(nextMapElement)) return PATH_SEARCH_WIDE;
        // Only queue tiles that are connected to a ride are returned as ride queues.
        if (footpath_element_is_queue(nextMapElement) && nextMapElement->properties.path.ride_index != 0xFF) return PATH_SEARCH_RIDE_QUEUE;

        return PATH_SEARCH_OTHER;
    } while (!map_element_is_last_for_tile(nextMapElement++));

    return PATH_SEARCH_FAILED;
}

/**
 *
 * Returns:
 *   0 - PATH_SEARCH_DEAD_END (path is a dead end, i.e. < 2 edges)
 *   1 - PATH_SEARCH_WIDE (path with wide flag set)
 *   3 - PATH_SEARCH_JUNCTION (path is a junction, i.e. > 2 edges)
 *   5 - PATH_SEARCH_RIDE_ENTRANCE (map element is a ride entrance)
 *   6 - PATH_SEARCH_RIDE_EXIT (map element is a ride exit)
 *   7 - PATH_SEARCH_PARK_EXIT park entrance / exit (map element is a park entrance/exit)
 *   8 - PATH_SEARCH_SHOP_ENTRANCE (map element is a shop entrance)
 *   9 - PATH_SEARCH_LIMIT_REACHED (search limit reached without reaching path end)
 *   12 - PATH_SEARCH_FAILED (no path element found)
 * For return values 5, 6 & 8 the rideIndex is stored in outRideIndex.
 *
 *  rct2: 0x006949B9
 *
 * This is the recursive portion of footpath_element_destination_in_direction().
 */
static uint8 footpath_element_dest_in_dir(
    sint16 x, sint16 y, sint16 z, rct_map_element *inputMapElement, uint8 chosenDirection, uint8 *outRideIndex,
    sint32 level
) {
    rct_map_element *mapElement;
    sint32 direction;

    if (level > 25) return PATH_SEARCH_LIMIT_REACHED;

    x += TileDirectionDelta[chosenDirection].x;
    y += TileDirectionDelta[chosenDirection].y;
    mapElement = map_get_first_element_at(x / 32, y / 32);
    do {
        if (mapElement->flags & MAP_ELEMENT_FLAG_GHOST) continue;

        switch (map_element_get_type(mapElement)) {
        case MAP_ELEMENT_TYPE_TRACK:
            if (z != mapElement->base_height) continue;
            sint32 rideIndex = mapElement->properties.track.ride_index;
            rct_ride *ride = get_ride(rideIndex);
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP)) {
                *outRideIndex = rideIndex;
                return PATH_SEARCH_SHOP_ENTRANCE;
            }
            break;
        case MAP_ELEMENT_TYPE_ENTRANCE:
            if (z != mapElement->base_height) continue;
            switch (mapElement->properties.entrance.type) {
            case ENTRANCE_TYPE_RIDE_ENTRANCE:
                direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
                if (direction == chosenDirection) {
                    *outRideIndex = mapElement->properties.entrance.ride_index;
                    return PATH_SEARCH_RIDE_ENTRANCE;
                }
                break;
            case ENTRANCE_TYPE_RIDE_EXIT:
                direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
                if (direction == chosenDirection) {
                    *outRideIndex = mapElement->properties.entrance.ride_index;
                    return PATH_SEARCH_RIDE_EXIT;
                }
                break;
            case ENTRANCE_TYPE_PARK_ENTRANCE:
                return PATH_SEARCH_PARK_EXIT;
            }
            break;
        case MAP_ELEMENT_TYPE_PATH:
            if (!is_valid_path_z_and_direction(mapElement, z, chosenDirection)) continue;
            if (footpath_element_is_wide(mapElement)) return PATH_SEARCH_WIDE;

            uint8 edges = path_get_permitted_edges(mapElement);
            edges &= ~(1 << (chosenDirection ^ 2));
            z = mapElement->base_height;

            for (direction = 0; direction < 4; direction++) {
                if (!(edges & (1 << direction))) continue;

                edges &= ~(1 << direction);
                if (edges != 0) return PATH_SEARCH_JUNCTION;

                if (footpath_element_is_sloped(mapElement)) {
                    if (footpath_element_get_slope_direction(mapElement) == direction) {
                        z += 2;
                    }
                }
                return footpath_element_dest_in_dir(x, y, z, mapElement, direction, outRideIndex, level + 1);
            }
            return PATH_SEARCH_DEAD_END;
        }
    } while (!map_element_is_last_for_tile(mapElement++));

    return PATH_SEARCH_FAILED;
}

/**
 * Returns:
 *   0 - PATH_SEARCH_DEAD_END (path is a dead end, i.e. < 2 edges)
 *   1 - PATH_SEARCH_WIDE (path with wide flag set)
 *   3 - PATH_SEARCH_JUNCTION (path is a junction, i.e. > 2 edges)
 *   5 - PATH_SEARCH_RIDE_ENTRANCE (map element is a ride entrance)
 *   6 - PATH_SEARCH_RIDE_EXIT (map element is a ride exit)
 *   7 - PATH_SEARCH_PARK_EXIT park entrance / exit (map element is a park entrance/exit)
 *   8 - PATH_SEARCH_SHOP_ENTRANCE (map element is a shop entrance)
 *   9 - PATH_SEARCH_LIMIT_REACHED (search limit reached without reaching path end)
 *   12 - PATH_SEARCH_FAILED (no path element found)
 * For return values 5, 6 & 8 the rideIndex is stored in outRideIndex.
 *
 *  rct2: 0x006949A4
 *
 * Returns the destination tile type a peep can get to from x,y,z /
 * inputMapElement in the given direction following single width paths only
 * and stopping as soon as a path junction is encountered.
 * Note that a junction is a path with > 2 reachable neighbouring path tiles,
 * so wide paths have LOTS of junctions.
 * This is useful for finding out what is at the end of a short single
 * width path, for example that leads from a ride exit back to the main path.
 */
static uint8 footpath_element_destination_in_direction(sint16 x, sint16 y, sint16 z, rct_map_element *inputMapElement, uint8 chosenDirection, uint8 *outRideIndex)
{
    if (footpath_element_is_sloped(inputMapElement)) {
        if (footpath_element_get_slope_direction(inputMapElement) == chosenDirection) {
            z += 2;
        }
    }

    return footpath_element_dest_in_dir(x, y, z, inputMapElement, chosenDirection, outRideIndex, 0);
}

/**
 *
 *  rct2: 0x00695225
 */
static sint32 guest_path_find_aimless(rct_peep* peep, uint8 edges){
    if (peep_rand() & 1){
        // If possible go straight
        if (edges & (1 << peep->direction)){
            return peep_move_one_tile(peep->direction, peep);
        }
    }

    while (1){
        uint8 direction = peep_rand() & 3;
        // Otherwise go in a random direction allowed from the tile.
        if (edges & (1 << direction)){
            return peep_move_one_tile(direction, peep);
        }
    }
}

/**
 *
 *  rct2: 0x0069A60A
 */
static uint8 peep_pathfind_get_max_number_junctions(rct_peep* peep){
    if (peep->type == PEEP_TYPE_STAFF)
        return 8;

    // PEEP_FLAGS_2? It's cleared here but not set anywhere!
    if ((peep->peep_flags & PEEP_FLAGS_2)){
        if ((peep_rand() & 0xFFFF) <= 7281)
            peep->peep_flags &= ~PEEP_FLAGS_2;

        return 8;
    }

    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK &&
        peep->peep_is_lost_countdown < 90){
        return 8;
    }

    if (peep->item_standard_flags & PEEP_ITEM_MAP)
        return 7;

    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
        return 7;

    return 5;
}

/**
 * Returns if the path as xzy is a 'thin' junction.
 * A junction is considered 'thin' if it has more than 2 edges
 * leading to non-wide path elements; edges leading to non-path elements
 * (e.g. ride/shop entrances) or ride queues are not counted, since entrances
 * and ride queues coming off a path should not result in the path being
 * considered a junction.
 */
static bool path_is_thin_junction(rct_map_element *path, sint16 x, sint16 y, uint8 z) {
    uint8 edges = path_get_permitted_edges(path);

    sint32 test_edge = bitscanforward(edges);
    if (test_edge == -1) return false;

    bool thin_junction = false;
    sint32 thin_count = 0;
    do
    {
        sint32 fp_result = footpath_element_next_in_direction(x, y, z, path, test_edge);

        /* Ignore non-paths (e.g. ride entrances, shops), wide paths
         * and ride queues (per ignoreQueues) when counting
         * neighbouring tiles. */
        if (fp_result != PATH_SEARCH_FAILED &&
            fp_result != PATH_SEARCH_WIDE &&
            fp_result != PATH_SEARCH_RIDE_QUEUE) {
            thin_count++;
        }

        if (thin_count > 2) {
            thin_junction = true;
            break;
        }
        edges &= ~(1 << test_edge);
    } while ((test_edge = bitscanforward(edges)) != -1);
    return thin_junction;
}

/**
 * Searches for the tile with the best heuristic score within the search limits
 * starting from the given tile x,y,z and going in the given direction test_edge.
 * The best heuristic score is tracked and returned in the call parameters
 * along with the corresponding tile location and search path telemetry
 * (junctions passed through and directions taken).
 *
 * The primary heuristic used is distance from the goal; the secondary
 * heuristic used (when the primary heuristic gives equal scores) is the number
 * of steps. i.e. the search gets as close as possible to the goal in as few
 * steps as possible.
 *
 * Each tile is checked to determine if the goal is reached.
 * When the goal is not reached the search result is only updated at the END
 * of each search path (some map element that is not a path or a path at which
 * a search limit is reached), NOT at each step along the way.
 * This means that the search ignores thin paths that are "no through paths"
 * no matter how close to the goal they get, but will follow possible "through
 * paths".
 *
 * The implementation is a depth first search of the path layout in xyz
 * according to the search limits.
 * Unlike an A* search, which tracks for each tile a heuristic score (a
 * function of the xyz distances to the goal) and cost of reaching that tile
 * (steps to the tile), a single best result "so far" (best heuristic score
 * with least cost) is tracked via the score parameter.
 * With this approach, explicit loop detection is necessary to limit the
 * search space, and each alternate route through the same tile can be
 * returned as the best result, rather than only the shortest route with A*.
 *
 * The parameters that hold the best search result so far are:
 *   - score - the least heuristic distance from the goal
 *   - endSteps - the least number of steps that achieve the score.
 *
 * The following parameters provide telemetry information on best search path so far:
 *   - endXYZ tracks the end location of the search path.
 *   - endSteps tracks the number of steps to the end of the search path.
 *   - endJunctions tracks the number of junctions passed through in the
 *     search path.
 *   - junctionList[] and directionList[] track the junctions and
 *     corresponding directions of the search path.
 * Other than debugging purposes, these could potentially be used to visualise
 * the pathfinding on the map.
 *
 * The parameters/variables that limit the search space are:
 *   - counter (param) - number of steps walked in the current search path;
 *   - _peepPathFindTilesChecked (variable) - cumulative number of tiles that can be
 *     checked in the entire search;
 *   - _peepPathFindNumJunctions (variable) - number of thin junctions that can be
 *     checked in a single search path;
 *
 * Other global variables/state that affect the search space are:
 *   - Wide paths - to handle broad paths (> 1 tile wide), the search navigates
 *     along non-wide (or 'thin' paths) and stops as soon as it encounters a
 *     wide path. This means peeps heading for a destination will only leave
 *     thin paths if walking 1 tile onto a wide path is closer than following
 *     non-wide paths;
 *   - gPeepPathFindIgnoreForeignQueues
 *   - gPeepPathFindQueueRideIndex - the ride the peep is heading for
 *   - _peepPathFindHistory - the search path telemetry consisting of the
 *     starting point and all thin junctions with directions navigated
 *     in the current search path - also used to detect path loops.
 *
 * The score is only updated when:
 *   - the goal is reached;
 *   - a wide tile is encountered with a better search result - the goal may
 *     still be reachable from here (only if the current tile is also wide);
 *   - a junction is encountered with a better search result and
 *     maxNumJunctions is exceeded - the goal may still be reachable from here;
 *   - returning from a recursive call if a search limit (i.e. either
 *     maxNumStep or maxTilesChecked) was reached and the current tile has a
 *     better search result and the goal may still be reachable from here
 *     (i.e. not a dead end path tile).
 *
 *  rct2: 0x0069A997
 */
static void peep_pathfind_heuristic_search(sint16 x, sint16 y, uint8 z, rct_peep *peep, rct_map_element *currentMapElement, bool inPatrolArea, uint8 counter, uint16 *endScore, sint32 test_edge, uint8 *endJunctions, rct_xyz8 junctionList[16], uint8 directionList[16], rct_xyz8 *endXYZ, uint8 *endSteps) {
    uint8 searchResult = PATH_SEARCH_FAILED;

    x += TileDirectionDelta[test_edge].x;
    y += TileDirectionDelta[test_edge].y;

    ++counter;
    _peepPathFindTilesChecked--;

    /* If this is where the search started this is a search loop and the
     * current search path ends here.
     * Return without updating the parameters (best result so far). */
    if ((_peepPathFindHistory[0].location.x == (uint8)(x >> 5)) &&
        (_peepPathFindHistory[0].location.y == (uint8)(y >> 5)) &&
        (_peepPathFindHistory[0].location.z == (uint8)z)) {
        #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug) {
            log_info("[%03d] Return from %d,%d,%d; At start", counter, x >> 5, y >> 5, z);
        }
        #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        return;
    }


    bool nextInPatrolArea = inPatrolArea;
    if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == STAFF_TYPE_MECHANIC) {
        nextInPatrolArea = staff_is_location_in_patrol(peep, x, y);
        if (inPatrolArea && !nextInPatrolArea) {
            /* The mechanic will leave his patrol area by taking
             * the test_edge so the current search path ends here.
             * Return without updating the parameters (best result so far). */
            #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug) {
                log_info("[%03d] Return from %d,%d,%d; Left patrol area", counter, x >> 5, y >> 5, z);
            }
            #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            return;
        }
    }

    /* Get the next map element of interest in the direction of test_edge. */
    bool found = false;
    rct_map_element *mapElement = map_get_first_element_at(x / 32, y / 32);
    do {
        /* Look for all map elements that the peep could walk onto while
         * navigating to the goal, including the goal tile. */

        if (mapElement->flags & MAP_ELEMENT_FLAG_GHOST) continue;

        uint8 rideIndex = 0xFF;
        switch (map_element_get_type(mapElement)) {
        case MAP_ELEMENT_TYPE_TRACK:
            if (z != mapElement->base_height) continue;
            /* For peeps heading for a shop, the goal is the shop
             * tile. */
            rideIndex = mapElement->properties.track.ride_index;
            rct_ride *ride = get_ride(rideIndex);
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP)) {
                found = true;
                searchResult = PATH_SEARCH_SHOP_ENTRANCE;
                break;
            } else {
                continue;
            }
        case MAP_ELEMENT_TYPE_ENTRANCE:
            if (z != mapElement->base_height) continue;
            sint32 direction;
            searchResult = PATH_SEARCH_OTHER;
            switch (mapElement->properties.entrance.type) {
            case ENTRANCE_TYPE_RIDE_ENTRANCE:
                /* For peeps heading for a ride without a queue, the
                 * goal is the ride entrance tile.
                 * For mechanics heading for the ride entrance
                 * (in the case when the station has no exit),
                 * the goal is the ride entrance tile. */
                direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
                if (direction == test_edge) {
                    /* The rideIndex will be useful for
                     * adding transport rides later. */
                    rideIndex = mapElement->properties.entrance.ride_index;
                    searchResult = PATH_SEARCH_RIDE_ENTRANCE;
                    found = true;
                    break;
                }
                continue; // Ride entrance is not facing the right direction.
            case ENTRANCE_TYPE_PARK_ENTRANCE:
                /* For peeps leaving the park, the goal is the park
                 * entrance/exit tile. */
                searchResult = PATH_SEARCH_PARK_EXIT;
                found = true;
                break;
            case ENTRANCE_TYPE_RIDE_EXIT:
                /* For mechanics heading for the ride exit, the
                 * goal is the ride exit tile. */
                direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
                if (direction == test_edge) {
                    searchResult = PATH_SEARCH_RIDE_EXIT;
                    found = true;
                    break;
                }
                continue; // Ride exit is not facing the right direction.
            default: continue;
            }
            break;
        case MAP_ELEMENT_TYPE_PATH:
            /* For peeps heading for a ride with a queue, the goal is the last
             * queue path.
             * Otherwise, peeps walk on path tiles to get to the goal. */

            if (!is_valid_path_z_and_direction(mapElement, z, test_edge)) continue;

            // Path may be sloped, so set z to path base height.
            z = mapElement->base_height;

            if (footpath_element_is_wide(mapElement)) {
                if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == STAFF_TYPE_MECHANIC) {
                    // Check whether the tile is not on the
                    // edge of the mechanic patrol zone.
                    bool onZoneEdge = false;
                    int neighbourDir = 0;
                    while (!onZoneEdge && neighbourDir <= 7) {
                        int neighbourX = x + TileDirectionDelta[neighbourDir].x;
                        int neighbourY = y + TileDirectionDelta[neighbourDir].y;
                        onZoneEdge = !staff_is_location_in_patrol(peep, neighbourX, neighbourY);
                        neighbourDir++;
                    }
                    // Wide paths not on the edge of the
                    // mechanic patrol zone are observed.
                    if (!onZoneEdge) {
                        searchResult = PATH_SEARCH_WIDE;
                        found = true;
                        break;
                    }
                    // Wide path flag for path tiles on the
                    // edge of the mechanic patrol zone are
                    // ignored.
                } else {
                    searchResult = PATH_SEARCH_WIDE;
                    found = true;
                    break;
                }
            }

            searchResult = PATH_SEARCH_THIN;

            uint8 numEdges = bitcount(path_get_permitted_edges(mapElement));

            if (numEdges < 2) {
                searchResult = PATH_SEARCH_DEAD_END;
            } else if (numEdges > 2) {
                searchResult = PATH_SEARCH_JUNCTION;
            } else { // numEdges == 2
                if (footpath_element_is_queue(mapElement) && mapElement->properties.path.ride_index != gPeepPathFindQueueRideIndex) {
                    if (gPeepPathFindIgnoreForeignQueues && (mapElement->properties.path.ride_index != 0xFF)) {
                        // Path is a queue we aren't interested in
                        /* The rideIndex will be useful for
                        * adding transport rides later. */
                        rideIndex = mapElement->properties.path.ride_index;
                        searchResult = PATH_SEARCH_RIDE_QUEUE;
                    }
                }
            }
            found = true;
            break;
        default:
            continue;
        }

        #
        #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug) {
            log_info("[%03d] Checking map element at %d,%d,%d; Type: %s", counter, x >> 5, y >> 5, z, gPathFindSearchText[searchResult]);
        }
        #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

        /* At this point mapElement is of interest to the pathfinding. */

        /* Should we check that this mapElement is connected in the
         * reverse direction? For some mapElement types this was
         * already done above (e.g. ride entrances), but for others not.
         * Ignore for now. */

        // Calculate the heuristic score of this map element.
        uint16 x_delta = abs(gPeepPathFindGoalPosition.x - x);
        uint16 y_delta = abs(gPeepPathFindGoalPosition.y - y);
        if (x_delta < y_delta) x_delta >>= 4;
        else y_delta >>= 4;
        uint16 new_score = x_delta + y_delta;
        uint16 z_delta = abs(gPeepPathFindGoalPosition.z - z);
        z_delta <<= 1;
        new_score += z_delta;

        /* If this map element is the search goal the current search path ends here. */
        if (new_score == 0) {
            /* If the search result is better than the best so far (in the parameters),
             * then update the parameters with this search before continuing to the next map element. */
            if (new_score < *endScore || (new_score == *endScore && counter < *endSteps )) {
                // Update the search results
                *endScore = new_score;
                *endSteps = counter;
                // Update the end x,y,z
                endXYZ->x = x >> 5;
                endXYZ->y = y >> 5;
                endXYZ->z = z;
                // Update the telemetry
                *endJunctions = _peepPathFindMaxJunctions - _peepPathFindNumJunctions;
                for (uint8 junctInd = 0; junctInd < *endJunctions; junctInd++) {
                    uint8 histIdx = _peepPathFindMaxJunctions - junctInd;
                    junctionList[junctInd].x = _peepPathFindHistory[histIdx].location.x;
                    junctionList[junctInd].y = _peepPathFindHistory[histIdx].location.y;
                    junctionList[junctInd].z = _peepPathFindHistory[histIdx].location.z;
                    directionList[junctInd] = _peepPathFindHistory[histIdx].direction;
                }
            }
            #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug) {
                log_info("[%03d] Search path ends at %d,%d,%d; At goal; Score: %d", counter, x >> 5, y >> 5, z, new_score);
            }
            #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        /* At this point the map element tile is not the goal. */

        /* If this map element is not a path, the search cannot be continued.
         * Continue to the next map element without updating the parameters (best result so far). */
        if (searchResult != PATH_SEARCH_DEAD_END &&
            searchResult != PATH_SEARCH_THIN &&
            searchResult != PATH_SEARCH_JUNCTION &&
            searchResult != PATH_SEARCH_WIDE) {
            #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug) {
                log_info("[%03d] Search path ends at %d,%d,%d; Not a path", counter, x >> 5, y >> 5, z);
            }
            #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        /* At this point the map element is a path. */

        /* If this is a wide path the search ends here. */
        if (searchResult == PATH_SEARCH_WIDE) {
            /* Ignore Wide paths as continuing paths UNLESS the current path is also Wide.
             * i.e. search across wide paths from a wide path to get onto a thin path,
             * thereafter stay on thin paths. */
            /* So, if the current path is also wide the goal could still
             * be reachable from here.
             * If the search result is better than the best so far (in the parameters),
             * then update the parameters with this search before continuing to the next map element. */
            if (footpath_element_is_wide(currentMapElement) &&
                (new_score < *endScore || (new_score == *endScore && counter < *endSteps ))) {
                // Update the search results
                *endScore = new_score;
                *endSteps = counter;
                // Update the end x,y,z
                endXYZ->x = x >> 5;
                endXYZ->y = y >> 5;
                endXYZ->z = z;
                // Update the telemetry
                *endJunctions = _peepPathFindMaxJunctions - _peepPathFindNumJunctions;
                for (uint8 junctInd = 0; junctInd < *endJunctions; junctInd++) {
                    uint8 histIdx = _peepPathFindMaxJunctions - junctInd;
                    junctionList[junctInd].x = _peepPathFindHistory[histIdx].location.x;
                    junctionList[junctInd].y = _peepPathFindHistory[histIdx].location.y;
                    junctionList[junctInd].z = _peepPathFindHistory[histIdx].location.z;
                    directionList[junctInd] = _peepPathFindHistory[histIdx].direction;
                }
            }
            #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug) {
                log_info("[%03d] Search path ends at %d,%d,%d; Wide path; Score: %d", counter, x >> 5, y >> 5, z, new_score);
            }
            #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        /* At this point the map element is a non-wide path.*/

        /* Get all the permitted_edges of the map element. */
        uint8 edges = path_get_permitted_edges(mapElement);

        #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug) {
            log_info("[%03d] Path element at %d,%d,%d; Edges (0123):%d%d%d%d; Reverse: %d", counter, x >> 5, y >> 5, z, edges & 1, (edges & 2) >> 1, (edges & 4) >> 2, (edges & 8) >> 3, test_edge ^ 2);
        }
        #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

        /* Remove the reverse edge (i.e. the edge back to the previous map element.) */
        edges &= ~(1 << (test_edge ^ 2));

        sint32 next_test_edge = bitscanforward(edges);

        /* If there are no other edges the current search ends here.
         * Continue to the next map element without updating the parameters (best result so far). */
        if (next_test_edge == -1) {
            #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug) {
                log_info("[%03d] Search path ends at %d,%d,%d; No more edges/dead end", counter, x >> 5, y >> 5, z);
            }
            #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        /* Check if either of the search limits has been reached:
         * - max number of steps or max tiles checked. */
        if (counter >= 200 || _peepPathFindTilesChecked <= 0) {
            /* The current search ends here.
             * The path continues, so the goal could still be reachable from here.
             * If the search result is better than the best so far (in the parameters),
             * then update the parameters with this search before continuing to the next map element. */
            if (new_score < *endScore || (new_score == *endScore && counter < *endSteps )) {
                // Update the search results
                *endScore = new_score;
                *endSteps = counter;
                // Update the end x,y,z
                endXYZ->x = x >> 5;
                endXYZ->y = y >> 5;
                endXYZ->z = z;
                // Update the telemetry
                *endJunctions = _peepPathFindMaxJunctions - _peepPathFindNumJunctions;
                for (uint8 junctInd = 0; junctInd < *endJunctions; junctInd++) {
                    uint8 histIdx = _peepPathFindMaxJunctions - junctInd;
                    junctionList[junctInd].x = _peepPathFindHistory[histIdx].location.x;
                    junctionList[junctInd].y = _peepPathFindHistory[histIdx].location.y;
                    junctionList[junctInd].z = _peepPathFindHistory[histIdx].location.z;
                    directionList[junctInd] = _peepPathFindHistory[histIdx].direction;
                }
            }
            #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug) {
                log_info("[%03d] Search path ends at %d,%d,%d; Search limit reached; Score: %d", counter, x >> 5, y >> 5, z, new_score);
            }
            #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        bool thin_junction = false;
        if (searchResult == PATH_SEARCH_JUNCTION) {
            /* Check if this is a thin junction. And perform additional
             * necessary checks. */
            thin_junction = path_is_thin_junction(mapElement, x, y, z);

            if (thin_junction) {
                /* The current search path is passing through a thin
                 * junction on this map element. Only 'thin' junctions
                 * are counted towards the junction search limit. */

                /* First check if going through the junction would be
                 * a loop.  If so, the current search path ends here.
                 * Path finding loop detection can take advantage of both the
                 * peep->pathfind_history - loops through remembered junctions
                 *     the peep has already passed through getting to its
                 *     current position while on the way to its current goal;
                 * _peepPathFindHistory - loops in the current search path. */
                bool pathLoop = false;
                /* Check the peep->pathfind_history to see if this junction has
                 * already been visited by the peep while heading for this goal. */
                for (sint32 i = 0; i < 4; ++i) {
                    if (peep->pathfind_history[i].x == x >> 5 &&
                        peep->pathfind_history[i].y == y >> 5 &&
                        peep->pathfind_history[i].z == z) {
                        if (peep->pathfind_history[i].direction == 0) {
                            /* If all directions have already been tried while
                             * heading to this goal, this is a loop. */
                            pathLoop = true;
                        }
                        else {
                            /* The peep remembers walking through this junction
                             * before, but has not yet tried all directions.
                             * Limit the edges to search to those not yet tried. */
                            edges &= peep->pathfind_history[i].direction;
                        }
                        break;
                    }
                }

                if (!pathLoop) {
                    /* Check the _peepPathFindHistory to see if this junction has been
                     * previously passed through in the current search path.
                     * i.e. this is a loop in the current search path. */
                    for (sint32 junctionNum = _peepPathFindNumJunctions + 1; junctionNum <= _peepPathFindMaxJunctions; junctionNum++) {
                        if ((_peepPathFindHistory[junctionNum].location.x == (uint8)(x >> 5)) &&
                            (_peepPathFindHistory[junctionNum].location.y == (uint8)(y >> 5)) &&
                            (_peepPathFindHistory[junctionNum].location.z == (uint8)z)) {
                                pathLoop = true;
                                break;
                        }
                    }
                }
                if (pathLoop) {
                    /* Loop detected.  The current search path ends here.
                     * Continue to the next map element without updating the parameters (best result so far). */
                    #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
                    if (gPathFindDebug) {
                        log_info("[%03d] Search path ends at %d,%d,%d; Loop", counter, x >> 5, y >> 5, z);
                    }
                    #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
                    continue;
                }

                /* If the junction search limit is reached, the
                 * current search path ends here. The goal may still
                 * be reachable from here.
                 * If the search result is better than the best so far (in the parameters),
                 * then update the parameters with this search before continuing to the next map element. */
                if (_peepPathFindNumJunctions <= 0) {
                    if (new_score < *endScore || (new_score == *endScore && counter < *endSteps )) {
                        // Update the search results
                        *endScore = new_score;
                        *endSteps = counter;
                        // Update the end x,y,z
                        endXYZ->x = x >> 5;
                        endXYZ->y = y >> 5;
                        endXYZ->z = z;
                        // Update the telemetry
                        *endJunctions = _peepPathFindMaxJunctions; // - _peepPathFindNumJunctions;
                        for (uint8 junctInd = 0; junctInd < *endJunctions; junctInd++) {
                            uint8 histIdx = _peepPathFindMaxJunctions - junctInd;
                            junctionList[junctInd].x = _peepPathFindHistory[histIdx].location.x;
                            junctionList[junctInd].y = _peepPathFindHistory[histIdx].location.y;
                            junctionList[junctInd].z = _peepPathFindHistory[histIdx].location.z;
                            directionList[junctInd] = _peepPathFindHistory[histIdx].direction;
                        }
                    }
                    #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
                    if (gPathFindDebug) {
                        log_info("[%03d] Search path ends at %d,%d,%d; NumJunctions < 0; Score: %d", counter, x >> 5, y >> 5, z, new_score);
                    }
                    #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
                    continue;
                }


                /* This junction was NOT previously visited in the current
                 * search path, so add the junction to the history. */
                _peepPathFindHistory[_peepPathFindNumJunctions].location.x = (uint8)(x >> 5);
                _peepPathFindHistory[_peepPathFindNumJunctions].location.y = (uint8)(y >> 5);
                _peepPathFindHistory[_peepPathFindNumJunctions].location.z = (uint8)z;
                // .direction take is added below.

                _peepPathFindNumJunctions--;
            }
        }

        /* Continue searching down each remaining edge of the path
         * (recursive call). */
        do {
            edges &= ~(1 << next_test_edge);
            uint8 savedNumJunctions = _peepPathFindNumJunctions;

            uint8 height = z;
            if (footpath_element_is_sloped(mapElement) &&
                footpath_element_get_slope_direction(mapElement) == next_test_edge) {
                height += 2;
            }
            #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug) {
                if (searchResult == PATH_SEARCH_JUNCTION) {
                    if (thin_junction)
                        log_info("[%03d] Recurse from %d,%d,%d edge: %d; Thin-Junction", counter, x >> 5, y >> 5, z, next_test_edge);
                    else
                        log_info("[%03d] Recurse from %d,%d,%d edge: %d; Wide-Junction", counter, x >> 5, y >> 5, z, next_test_edge);
                } else {
                    log_info("[%03d] Recurse from %d,%d,%d edge: %d; Segment", counter, x >> 5, y >> 5, z, next_test_edge);
                }
            }
            #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

            if (thin_junction) {
                /* Add the current test_edge to the history. */
                _peepPathFindHistory[_peepPathFindNumJunctions + 1].direction = next_test_edge;
            }

            peep_pathfind_heuristic_search(x, y, height, peep, mapElement, nextInPatrolArea, counter, endScore, next_test_edge, endJunctions, junctionList, directionList, endXYZ, endSteps);
            _peepPathFindNumJunctions = savedNumJunctions;

            #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug) {
                log_info("[%03d] Returned to %d,%d,%d edge: %d; Score: %d", counter, x >> 5, y >> 5, z, next_test_edge, *endScore);
            }
            #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        } while ((next_test_edge = bitscanforward(edges)) != -1);

    } while (!map_element_is_last_for_tile(mapElement++));

    if (!found) {
        /* No map element could be found.
        * Return without updating the parameters (best result so far). */
        #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug) {
            log_info("[%03d] Returning from %d,%d,%d; No relevant map element found", counter, x >> 5, y >> 5, z);
        }
        #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
    } else {
        #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug) {
            log_info("[%03d] Returning from %d,%d,%d; All map elements checked", counter, x >> 5, y >> 5, z);
        }
        #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
    }

    return;
}

/**
 * Returns:
 *   -1   - no direction chosen
 *   0..3 - chosen direction
 *
 *  rct2: 0x0069A5F0
 */
sint32 peep_pathfind_choose_direction(sint16 x, sint16 y, uint8 z, rct_peep *peep)
{
    // The max number of thin junctions searched - a per-search-path limit.
    _peepPathFindMaxJunctions = peep_pathfind_get_max_number_junctions(peep);

    /* The max number of tiles to check - a whole-search limit.
     * Mainly to limit the performance impact of the path finding. */
    sint32 maxTilesChecked = (peep->type == PEEP_TYPE_STAFF) ? 50000 : 15000;
    // Used to allow walking through no entry banners
    _peepPathFindIsStaff = (peep->type == PEEP_TYPE_STAFF);

    rct_xyz8 goal = {
        .x = (uint8)(gPeepPathFindGoalPosition.x >> 5),
        .y = (uint8)(gPeepPathFindGoalPosition.y >> 5),
        .z = (uint8)(gPeepPathFindGoalPosition.z)
    };

    #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (gPathFindDebug) {
        log_verbose("Choose direction for %s for goal %d,%d,%d from %d,%d,%d", gPathFindDebugPeepName, goal.x, goal.y, goal.z, x >> 5, y >> 5, z);
    }
    #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    // Get the path element at this location
    rct_map_element *dest_map_element = map_get_first_element_at(x / 32, y / 32);
    /* Where there are multiple matching map elements placed with zero
     * clearance, save the first one for later use to determine the path
     * slope - this maintains the original behaviour (which only processes
     * the first matching map element found) and is consistent with peep
     * placement (i.e. height) on such paths with differing slopes.
     *
     * I cannot see a legitimate reason for building overlaid paths with
     * differing slopes and do not recall ever seeing this in practise.
     * Normal cases I have seen in practise are overlaid paths with the
     * same slope (flat) in order to place scenery (e.g. benches) in the
     * middle of a wide path that can still be walked through.
     * Anyone attempting to overlay paths with different slopes should
     * EXPECT to experience path finding irregularities due to those paths!
     * In particular common edges at different heights will not work
     * in a useful way. Simply do not do it! :-) */
    rct_map_element *first_map_element = NULL;

    bool found = false;
    uint8 permitted_edges = 0;
    bool isThin = false;
    do {
        if (dest_map_element->base_height != z) continue;
        if (map_element_get_type(dest_map_element) != MAP_ELEMENT_TYPE_PATH) continue;
        found = true;
        if (first_map_element == NULL) {
            first_map_element = dest_map_element;
        }

        /* Check if this path element is a thin junction.
         * Only 'thin' junctions are remembered in peep->pathfind_history.
         * NO attempt is made to merge the overlaid path elements and
         * check if the combination is 'thin'!
         * The junction is considered 'thin' simply if any of the
         * overlaid path elements there is a 'thin junction'. */
        isThin = isThin || path_is_thin_junction(dest_map_element, x, y, z);

        // Collect the permitted edges of ALL matching path elements at this location.
        permitted_edges |= path_get_permitted_edges(dest_map_element);
    } while (!map_element_is_last_for_tile(dest_map_element++));
    // Peep is not on a path.
    if (!found) return -1;

    permitted_edges &= 0xF;
    uint8 edges = permitted_edges;
    if (isThin && peep->pathfind_goal.x == goal.x &&
        peep->pathfind_goal.y == goal.y &&
        peep->pathfind_goal.z == goal.z
    ) {
        /* Use of peep->pathfind_history[]:
         * When walking to a goal, the peep pathfind_history stores
         * the last 4 thin junctions that the peep walked through.
         * For each of these 4 thin junctions the peep remembers
         * those edges it has not yet taken.
                 * If a peep returns to one of the 4 thin junctions that it
         * remembers, it will only choose from the directions that it
         * did not try yet.
         * This forces to the peep pathfinding to try the "next best"
         * direction after trying the "best" direction(s) and finding
         * that the goal could not be reached. */

        /* If the peep remembers walking through this junction
         * previously while heading for its goal, retrieve the
         * directions it has not yet tried. */
        for (sint32 i = 0; i < 4; ++i) {
            if (peep->pathfind_history[i].x == x / 32 &&
                    peep->pathfind_history[i].y == y / 32 &&
                    peep->pathfind_history[i].z == z) {

                /* Fix broken pathfind_history[i].direction
                 * which have untried directions that are not
                 * currently possible - could be due to pathing
                 * changes or in earlier code .directions was
                 * initialised to 0xF rather than the permitted
                 * edges. */
                peep->pathfind_history[i].direction &= permitted_edges;

                edges = peep->pathfind_history[i].direction;

                #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                if (gPathFindDebug) {
                    log_verbose("Getting untried edges from pf_history for %d,%d,%d:  %s,%s,%s,%s", x >> 5, y >> 5, z, (edges & 1) ? "0" : "-", (edges & 2) ? "1" : "-", (edges & 4) ? "2" : "-", (edges & 8) ? "3" : "-");
                }
                #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

                if (edges == 0) {
                    /* If peep has tried all edges, reset to
                     * all edges are untried.
                     * This permits the pathfinding to try
                     * again, which is good for getting
                     * unstuck when the player has edited
                     * the paths or the pathfinding itself
                     * has changed (been fixed) since
                     * the game was saved. */
                    peep->pathfind_history[i].direction = permitted_edges;
                    edges = peep->pathfind_history[i].direction;

                    #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                    if (gPathFindDebug) {
                        log_verbose("All edges tried for %d,%d,%d - resetting to all untried", x >> 5, y >> 5, z);
                    }
                    #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                }
                break;
            }
        }
    }

    /* If this is a new goal for the peep. Store it and reset the peep's
     * pathfind_history. */
    if (peep->pathfind_goal.direction > 3 ||
        peep->pathfind_goal.x != goal.x ||
        peep->pathfind_goal.y != goal.y ||
        peep->pathfind_goal.z != goal.z
    ) {
        peep->pathfind_goal.x = goal.x;
        peep->pathfind_goal.y = goal.y;
        peep->pathfind_goal.z = goal.z;
        peep->pathfind_goal.direction = 0;

        // Clear pathfinding history
        memset(peep->pathfind_history, 0xFF, sizeof(peep->pathfind_history));
        #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug) {
            log_verbose("New goal; clearing pf_history.");
        }
        #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    }

    // Peep has tried all edges.
    if (edges == 0) return -1;

    sint32 chosen_edge = bitscanforward(edges);

    // Peep has multiple edges still to try.
    if (edges & ~(1 << chosen_edge)) {
        uint16 best_score = 0xFFFF;
        uint8 best_sub = 0xFF;

        #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        uint8 bestJunctions = 0;
        rct_xyz8 bestJunctionList[16] = { 0 };
        uint8 bestDirectionList[16] = { 0 };
        rct_xyz8 bestXYZ = { 0, 0, 0 };

        if (gPathFindDebug) {
            log_verbose("Pathfind start for goal %d,%d,%d from %d,%d,%d", goal.x, goal.y, goal.z, x >> 5, y >> 5, z);
        }
        #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        /* Call the search heuristic on each edge, keeping track of the
         * edge that gives the best (i.e. smallest) value (best_score)
         * or for different edges with equal value, the edge with the
         * least steps (best_sub). */
        sint32 numEdges = bitcount(edges);
        for (sint32 test_edge = chosen_edge; test_edge != -1; test_edge = bitscanforward(edges)) {
            edges &= ~(1 << test_edge);
            uint8 height = z;

            if (footpath_element_is_sloped(first_map_element) &&
                footpath_element_get_slope_direction(first_map_element) == test_edge
            ) {
                height += 0x2;
            }

            _peepPathFindFewestNumSteps = 255;
            /* Divide the maxTilesChecked global search limit
             * between the remaining edges to ensure the search
             * covers all of the remaining edges. */
            _peepPathFindTilesChecked = maxTilesChecked / numEdges;
            _peepPathFindNumJunctions = _peepPathFindMaxJunctions;

            // Initialise _peepPathFindHistory.
            memset(_peepPathFindHistory, 0xFF, sizeof(_peepPathFindHistory));

            /* The pathfinding will only use elements
             * 1.._peepPathFindMaxJunctions, so the starting point
             * is placed in element 0 */
            _peepPathFindHistory[0].location.x = (uint8)(x >> 5);
            _peepPathFindHistory[0].location.y = (uint8)(y >> 5);
            _peepPathFindHistory[0].location.z = (uint8)z;
            _peepPathFindHistory[0].direction = 0xF;

            uint16 score = 0xFFFF;
            /* Variable endXYZ contains the end location of the
             * search path. */
            rct_xyz8 endXYZ;
            endXYZ.x = 0;
            endXYZ.y = 0;
            endXYZ.z = 0;

            uint8 endSteps = 255;

            /* Variable endJunctions is the number of junctions
             * passed through in the search path.
             * Variables endJunctionList and endDirectionList
             * contain the junctions and corresponding directions
             * of the search path.
             * In the future these could be used to visualise the
             * pathfinding on the map. */
            uint8 endJunctions = 0;
            rct_xyz8 endJunctionList[16] = { 0 };
            uint8 endDirectionList[16] = { 0 };

            bool inPatrolArea = false;
            if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == STAFF_TYPE_MECHANIC) {
                /* Mechanics are the only staff type that
                 * pathfind to a destination. Determine if the
                 * mechanic is in their patrol area. */
                inPatrolArea = staff_is_location_in_patrol(peep, peep->next_x, peep->next_y);
            }

            #if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug) {
                log_verbose("Pathfind searching in direction: %d from %d,%d,%d", test_edge, x >> 5, y >> 5, z);
            }
            #endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

            peep_pathfind_heuristic_search(x, y, height, peep, first_map_element, inPatrolArea, 0, &score, test_edge, &endJunctions, endJunctionList, endDirectionList, &endXYZ, &endSteps);

            #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
            if (gPathFindDebug) {
                log_verbose("Pathfind test edge: %d score: %d steps: %d end: %d,%d,%d junctions: %d", test_edge, score, endSteps, endXYZ.x, endXYZ.y, endXYZ.z, endJunctions);
                for (uint8 listIdx = 0; listIdx < endJunctions; listIdx++) {
                    log_info("Junction#%d %d,%d,%d Direction %d", listIdx + 1, endJunctionList[listIdx].x, endJunctionList[listIdx].y, endJunctionList[listIdx].z, endDirectionList[listIdx]);
                }
            }
            #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

            if (score < best_score || (score == best_score && endSteps < best_sub)) {
                chosen_edge = test_edge;
                best_score = score;
                best_sub = endSteps;
                #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                bestJunctions = endJunctions;
                for (uint8 index = 0; index < endJunctions; index++) {
                    bestJunctionList[index].x = endJunctionList[index].x;
                    bestJunctionList[index].y = endJunctionList[index].y;
                    bestJunctionList[index].z = endJunctionList[index].z;
                    bestDirectionList[index] = endDirectionList[index];
                }
                bestXYZ.x = endXYZ.x;
                bestXYZ.y = endXYZ.y;
                bestXYZ.z = endXYZ.z;
                #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
            }
        }

        /* Check if the heuristic search failed. e.g. all connected
         * paths are within the search limits and none reaches the
         * goal. */
        if (best_score == 0xFFFF) {
            #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
            if (gPathFindDebug) {
                log_verbose("Pathfind heuristic search failed.");
            }
            #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
            return -1;
        }
        #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug) {
            log_verbose("Pathfind best edge %d with score %d steps %d", chosen_edge, best_score, best_sub);
            for (uint8 listIdx = 0; listIdx < bestJunctions; listIdx++) {
                log_verbose("Junction#%d %d,%d,%d Direction %d", listIdx + 1, bestJunctionList[listIdx].x, bestJunctionList[listIdx].y, bestJunctionList[listIdx].z, bestDirectionList[listIdx]);
            }
            log_verbose("End at %d,%d,%d", bestXYZ.x, bestXYZ.y, bestXYZ.z);
        }
        #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    }

    if (isThin) {
        for (sint32 i = 0; i < 4; ++i) {
            if (peep->pathfind_history[i].x == x >> 5 &&
                peep->pathfind_history[i].y == y >> 5 &&
                peep->pathfind_history[i].z == z
            ) {
                /* Peep remembers this junction, so remove the
                 * chosen_edge from those left to try. */
                peep->pathfind_history[i].direction &= ~(1 << chosen_edge);
                /* Also remove the edge through which the peep
                 * entered the junction from those left to try. */
                peep->pathfind_history[i].direction &= ~(1 << (peep->direction ^ 2));
                #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                if (gPathFindDebug) {
                    log_verbose("Updating existing pf_history (in index: %d) for %d,%d,%d without entry edge %d & exit edge %d.", i, x >> 5, y >> 5, z, peep->direction ^ 2, chosen_edge);
                }
                #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                return chosen_edge;
            }
        }

        /* Peep does not remember this junction, so forget a junction
         * and remember this junction. */
        sint32 i = peep->pathfind_goal.direction++;
        peep->pathfind_goal.direction &= 3;
        peep->pathfind_history[i].x = x >> 5;
        peep->pathfind_history[i].y = y >> 5;
        peep->pathfind_history[i].z = z;
        peep->pathfind_history[i].direction = permitted_edges;
        /* Remove the chosen_edge from those left to try. */
        peep->pathfind_history[i].direction &= ~(1 << chosen_edge);
        /* Also remove the edge through which the peep
         * entered the junction from those left to try. */
        peep->pathfind_history[i].direction &= ~(1 << (peep->direction ^ 2));
        #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug) {
            log_verbose("Storing new pf_history (in index: %d) for %d,%d,%d without entry edge %d & exit edge %d.", i, x >> 5, y >> 5, z, peep->direction ^ 2, chosen_edge);
        }
        #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    }

    return chosen_edge;
}

/**
 * Gets the nearest park entrance relative to point, by using Manhattan distance.
 * @param x x coordinate of location
 * @param y y coordinate of location
 * @return Index of gParkEntrance (or 0xFF if no park entrances exist).
 */
static uint8 get_nearest_park_entrance_index(uint16 x, uint16 y)
{
    uint8 chosenEntrance = 0xFF;
    uint16 nearestDist = 0xFFFF;
    for (uint8 i = 0; i < MAX_PARK_ENTRANCES; i++) {
        if (gParkEntrances[i].x == MAP_LOCATION_NULL)
            continue;

        uint16 dist = abs(gParkEntrances[i].x - x) + abs(gParkEntrances[i].y - y);

        if (dist >= nearestDist)
            continue;

        nearestDist = dist;
        chosenEntrance = i;
    }
    return chosenEntrance;
}

/**
 *
 *  rct2: 0x006952C0
 */
static sint32 guest_path_find_entering_park(rct_peep *peep, rct_map_element *map_element, uint8 edges)
{
    // Send peeps to the nearest park entrance.
    uint8 chosenEntrance = get_nearest_park_entrance_index(peep->next_x, peep->next_y);

    // If no defined park entrances are found, walk aimlessly.
    if (chosenEntrance == 0xFF)
        return guest_path_find_aimless(peep, edges);

    sint16 x = gParkEntrances[chosenEntrance].x;
    sint16 y = gParkEntrances[chosenEntrance].y;
    sint16 z = gParkEntrances[chosenEntrance].z;

    gPeepPathFindGoalPosition = (rct_xyz16){ x, y, z >> 3 };
    gPeepPathFindIgnoreForeignQueues = true;
    gPeepPathFindQueueRideIndex = 255;

    sint32 chosenDirection = peep_pathfind_choose_direction(peep->next_x, peep->next_y, peep->next_z, peep);

    if (chosenDirection == -1)
        return guest_path_find_aimless(peep, edges);
    else
        return peep_move_one_tile(chosenDirection, peep);
}

/**
 * Gets the nearest peep spawn relative to point, by using Manhattan distance.
 * @param x x coordinate of location
 * @param y y coordinate of location
 * @return Index of gPeepSpawns (or 0xFF if no peep spawns exist).
 */
static uint8 get_nearest_peep_spawn_index(uint16 x, uint16 y)
{
    uint8 chosenSpawn = 0xFF;
    uint16 nearestDist = 0xFFFF;
    for (uint8 i = 0; i < MAX_PEEP_SPAWNS; ++i) {
        if (gPeepSpawns[i].x == PEEP_SPAWN_UNDEFINED)
            continue;

        uint16 dist = abs(gPeepSpawns[i].x - x) + abs(gPeepSpawns[i].y - y);

        if (dist >= nearestDist)
            continue;

        nearestDist = dist;
        chosenSpawn = i;
    }
    return chosenSpawn;
}

/**
 *
 *  rct2: 0x0069536C
 */
static sint32 guest_path_find_leaving_park(rct_peep *peep, rct_map_element *map_element, uint8 edges){
    // Send peeps to the nearest spawn point.
    uint8 chosenSpawn = get_nearest_peep_spawn_index(peep->next_x, peep->next_y);

    // If no defined spawns were found, walk aimlessly.
    if (chosenSpawn == 0xFF)
        return guest_path_find_aimless(peep, edges);

    rct2_peep_spawn* peepSpawn = &gPeepSpawns[chosenSpawn];

    sint16 x = peepSpawn->x & 0xFFE0;
    sint16 y = peepSpawn->y & 0xFFE0;
    uint8 z = peepSpawn->z * 2;
    uint8 direction = peepSpawn->direction;

    gPeepPathFindGoalPosition = (rct_xyz16){ x, y, z };
    if (x == peep->next_x && y == peep->next_y){
        return peep_move_one_tile(direction, peep);
    }

    gPeepPathFindIgnoreForeignQueues = true;
    gPeepPathFindQueueRideIndex = 255;
    direction = peep_pathfind_choose_direction(peep->next_x, peep->next_y, peep->next_z, peep);
    if (direction == 0xFF)
        return guest_path_find_aimless(peep, edges);
    else
        return peep_move_one_tile(direction, peep);
}

/**
 *
 *  rct2: 0x00695161
 */
static sint32 guest_path_find_park_entrance(rct_peep* peep, rct_map_element *map_element, uint8 edges){
    uint8 entranceNum;

    // Resolves already-corrupt guests (e.g. loaded from save)
    if (peep->peep_flags & PEEP_FLAGS_PARK_ENTRANCE_CHOSEN &&
        (peep->current_ride >= 4 || gParkEntrances[peep->current_ride].x == MAP_LOCATION_NULL)
    ) {
        peep->peep_flags &= ~(PEEP_FLAGS_PARK_ENTRANCE_CHOSEN);
    }

    if (!(peep->peep_flags & PEEP_FLAGS_PARK_ENTRANCE_CHOSEN)){
        uint8 chosenEntrance = 0xFF;
        uint16 nearestDist = 0xFFFF;
        for (entranceNum = 0; entranceNum < MAX_PARK_ENTRANCES; ++entranceNum){
            if (gParkEntrances[entranceNum].x == MAP_LOCATION_NULL)
                continue;

            uint16 dist = abs(gParkEntrances[entranceNum].x - peep->next_x) +
                abs(gParkEntrances[entranceNum].y - peep->next_y);

            if (dist >= nearestDist)
                continue;

            nearestDist = dist;
            chosenEntrance = entranceNum;
        }

        if (chosenEntrance == 0xFF)
            return guest_path_find_aimless(peep, edges);

        peep->current_ride = chosenEntrance;
        peep->peep_flags |= PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    entranceNum = peep->current_ride;
    sint16 x = gParkEntrances[entranceNum].x;
    sint16 y = gParkEntrances[entranceNum].y;
    sint16 z = gParkEntrances[entranceNum].z;

    gPeepPathFindGoalPosition = (rct_xyz16) { x, y, z >> 3 };
    gPeepPathFindIgnoreForeignQueues = true;
    gPeepPathFindQueueRideIndex = 255;

    #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    pathfind_logging_enable(peep);
    #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    sint32 chosenDirection = peep_pathfind_choose_direction(peep->next_x, peep->next_y, peep->next_z, peep);

    #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    pathfind_logging_disable();
    #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    if (chosenDirection == -1)
        return guest_path_find_aimless(peep, edges);
    else
        return peep_move_one_tile(chosenDirection, peep);
}

/**
 *
 *  rct2: 0x006A72C5
 *  param dist is not used.
 *
 * In case where the map element at (x, y) is invalid or there is no entrance
 * or queue leading to it the function will not update its arguments.
 */
static void get_ride_queue_end(sint16 *x, sint16 *y, sint16 *z){
    rct_xy16 result = { 0, 0 };
    rct_map_element *mapElement = map_get_first_element_at(*x / 32, *y / 32);

    if (mapElement == NULL) {
        return;
    }

    bool found = false;
    do{
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE)
            continue;

        if (*z != mapElement->base_height)
            continue;

        found = true;
        break;
    } while (!map_element_is_last_for_tile(mapElement++));

    if (!found)
        return;

    uint8 direction = (mapElement->type & 3) ^ (1 << 1);
    rct_map_element* lastPathElement = NULL;
    rct_map_element* firstPathElement = NULL;

    sint16 baseZ = mapElement->base_height;
    sint16 nextX = *x;
    sint16 nextY = *y;
    while (1){
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH){
            lastPathElement = mapElement;
            result.x = nextX;
            result.y = nextY;
            // result.direction = direction;
            if (footpath_element_is_sloped(mapElement)){
                if (footpath_element_get_slope_direction(mapElement) == direction){
                    baseZ += 2;
                }
            }
        }
        nextX += TileDirectionDelta[direction].x;
        nextY += TileDirectionDelta[direction].y;

        mapElement = map_get_first_element_at(nextX / 32, nextY / 32);
        found = false;
        do{
            if (mapElement == firstPathElement)
                continue;

            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
                continue;

            if (baseZ == mapElement->base_height){
                if (footpath_element_is_sloped(mapElement)){
                    if (footpath_element_get_slope_direction(mapElement) != direction){
                        break;
                    }
                }
                found = true;
                break;
            }

            if (baseZ - 2 == mapElement->base_height){
                if (!footpath_element_is_sloped(mapElement))
                    break;

                if (footpath_element_get_slope_direction(mapElement) != (direction ^ 2))
                    break;

                baseZ -= 2;
                found = true;
                break;
            }
        } while (!map_element_is_last_for_tile(mapElement++));

        if (found == false)
            break;

        if (!footpath_element_is_queue(mapElement))
            break;

        if (!(mapElement->properties.path.edges & (1 << (direction ^ (1 << 1)))))
            break;

        if (firstPathElement == NULL)
            firstPathElement = mapElement;

        // More queue to go.
        if (mapElement->properties.path.edges & (1 << (direction)))
            continue;

        direction++;
        direction &= 3;
        // More queue to go.
        if (mapElement->properties.path.edges & (1 << (direction)))
            continue;

        direction ^= (1 << 1);
        // More queue to go.
        if (mapElement->properties.path.edges & (1 << (direction)))
            continue;

        break;
    }

    if ((uint8)*z == 0xFF)
        return;

    mapElement = lastPathElement;
    if (mapElement == NULL)
        return;

    if (!footpath_element_is_queue(mapElement))
        return;

    *x = result.x;
    *y = result.y;
    *z = mapElement->base_height;
}

/**
 *
 *  rct2: 0x00694C35
 */
static sint32 guest_path_finding(rct_peep* peep)
{
    sint16 x, y, z;

    #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    pathfind_logging_enable(peep);
    if (gPathFindDebug) {
        log_info("Starting guest_path_finding for %s", gPathFindDebugPeepName);
    }
    #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    if (peep->next_var_29 & 0x18) {
        return guest_surface_path_finding(peep);
    }

    x = peep->next_x;
    y = peep->next_y;
    z = peep->next_z;

    rct_map_element *mapElement = map_get_path_element_at(x / 32, y / 32, z);
    if (mapElement == NULL) {
        return 1;
    }

    _peepPathFindIsStaff = false;
    uint8 edges = path_get_permitted_edges(mapElement);

    if (edges == 0) {
        return guest_surface_path_finding(peep);
    }

    if (peep->outside_of_park == 0 && peep_heading_for_ride_or_park_exit(peep)) {
        /* If this mapElement is adjacent to any non-wide paths,
         * remove all of the edges to wide paths. */
        uint8 adjustedEdges = edges;
        for (sint32 chosenDirection = 0; chosenDirection < 4; chosenDirection++) {
            // If there is no path in that direction try another
            if (!(adjustedEdges & (1 << chosenDirection)))
                continue;

            /* If there is a wide path in that direction,
                remove that edge and try another */
            if (footpath_element_next_in_direction(peep->next_x, peep->next_y, peep->next_z, mapElement, chosenDirection) == PATH_SEARCH_WIDE) {
                adjustedEdges &= ~(1 << chosenDirection);
            }
        }
        if (adjustedEdges != 0)
            edges = adjustedEdges;
    }

    sint8 direction = peep->direction ^ (1 << 1);
    // Check if in a dead end (i.e. only edge is where the peep came from)
    if (!(edges & ~(1 << direction))) {
        // In a dead end.  Check if peep is lost, etc.
        peep_check_if_lost(peep);
        peep_check_cant_find_ride(peep);
        peep_check_cant_find_exit(peep);
    } else {
        /* Not a dead end. Remove edge peep came from so peep will
         * continue on rather than going back where it came from */
        edges &= ~(1 << direction);
    }

    direction = bitscanforward(edges);
    // IF only one edge to choose from
    if ((edges & ~(1 << direction)) == 0) {
        #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug) {
            log_info("Completed guest_path_finding for %s - taking only direction available: %d.", gPathFindDebugPeepName, direction);
        }
        pathfind_logging_disable();
        #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return peep_move_one_tile(direction, peep);
    }

    // Peep still has multiple edges to choose from.

    // Peep is outside the park.
    // loc_694F19:
    if (peep->outside_of_park != 0){
        #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug) {
            log_info("Completed guest_path_finding for %s - peep is outside the park.", gPathFindDebugPeepName);
        }
        pathfind_logging_disable();
        #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        switch (peep->state) {
        case PEEP_STATE_ENTERING_PARK:
            return guest_path_find_entering_park(peep, mapElement, edges);
        case PEEP_STATE_LEAVING_PARK:
            return guest_path_find_leaving_park(peep, mapElement, edges);
        default:
            return guest_path_find_aimless(peep, edges);
        }
    }

    /* Peep is inside the park.
     * If the peep does not have food, randomly cull the useless directions
     * (dead ends, ride exits, wide paths) from the edges.
     * In principle, peeps with food are not paying as much attention to
     * where they are going and are consequently more like to walk up
     * dead end paths, paths to ride exits, etc. */
    if (!peep_has_food(peep) && (peep_rand() & 0xFFFF) >= 2184) {
        uint8 adjustedEdges = edges;
        for (sint32 chosenDirection = 0; chosenDirection < 4; chosenDirection++) {
            // If there is no path in that direction try another
            if (!(adjustedEdges & (1 << chosenDirection)))
                continue;

            uint8 rideIndex, pathSearchResult;
            pathSearchResult = footpath_element_destination_in_direction(peep->next_x, peep->next_y, peep->next_z, mapElement, chosenDirection, &rideIndex);
            switch (pathSearchResult) {
            case PATH_SEARCH_DEAD_END:
            case PATH_SEARCH_RIDE_EXIT:
            case PATH_SEARCH_WIDE:
                adjustedEdges &= ~(1 << chosenDirection);
                break;
            }
        }
        if (adjustedEdges != 0)
            edges = adjustedEdges;
    }

    /* If there are still multiple directions to choose from,
         * peeps with maps will randomly read the map: probability of doing so
     * is much higher when heading for a ride or the park exit. */
    if (peep->item_standard_flags & PEEP_ITEM_MAP) {
        // If at least 2 directions consult map
        if (bitcount(edges) >= 2) {
            uint16 probability = 1638;
            if (peep_heading_for_ride_or_park_exit(peep)) {
                probability = 9362;
            }
            if ((peep_rand() & 0xFFFF) < probability) {
                peep_read_map(peep);
            }
        }
    }

    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) {
        #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug) {
            log_info("Completed guest_path_finding for %s - peep is leaving the park.", gPathFindDebugPeepName);
        }
        pathfind_logging_disable();
        #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return guest_path_find_park_entrance(peep, mapElement, edges);
    }

    if (peep->guest_heading_to_ride_id == 0xFF) {
        #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug) {
            log_info("Completed guest_path_finding for %s - peep is aimless.", gPathFindDebugPeepName);
        }
        pathfind_logging_disable();
        #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return guest_path_find_aimless(peep, edges);
    }

    // Peep is heading for a ride.
    uint8 rideIndex = peep->guest_heading_to_ride_id;
    rct_ride* ride = get_ride(rideIndex);

    if (ride->status != RIDE_STATUS_OPEN) {
        #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug) {
            log_info("Completed guest_path_finding for %s - peep is heading to closed ride == aimless.", gPathFindDebugPeepName);
        }
        pathfind_logging_disable();
        #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return guest_path_find_aimless(peep, edges);
    }

    // The ride is open.
    gPeepPathFindQueueRideIndex = rideIndex;

    /* Find the ride's closest entrance station to the peep.
     * At the same time, count how many entrance stations there are and
     * which stations are entrance stations. */
    uint16 closestDist = 0xFFFF;
    uint8 closestStationNum = 0;

    sint32 numEntranceStations = 0;
    uint8 entranceStations = 0;

    for (uint8 stationNum = 0; stationNum < RCT12_MAX_STATIONS_PER_RIDE; ++stationNum){
        if (ride->entrances[stationNum] == 0xFFFF) // stationNum has no entrance (so presumably an exit only station).
            continue;

        numEntranceStations++;
        entranceStations |= (1 << stationNum);

        sint16 stationX = (ride->entrances[stationNum] & 0xFF) * 32;
        sint16 stationY = (ride->entrances[stationNum] & 0xFF00) / 8;
        uint16 dist = abs(stationX - peep->next_x) + abs(stationY - peep->next_y);

        if (dist < closestDist){
            closestDist = dist;
            closestStationNum = stationNum;
            continue;
        }
    }

    // Ride has no stations with an entrance, so head to station 0.
    if (numEntranceStations == 0)
        closestStationNum = 0;

    /* If a ride has multiple entrance stations and is set to sync with
     * adjacent stations, cycle through the entrance stations (based on
     * number of rides the peep has been on) so the peep will try the
     * different sections of the ride.
     * In this case, the ride's various entrance stations will typically,
     * though not necessarily, be adjacent to one another and consequently
     * not too far for the peep to walk when cycling between them.
     * Note: the same choice of station must made while the peep navigates
     * to the station. Consequently a random station selection here is not
     * appropriate. */
    if (numEntranceStations > 1 &&
        (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)) {
        sint32 select = peep->no_of_rides % numEntranceStations;
        while (select > 0) {
            closestStationNum = bitscanforward(entranceStations);
            entranceStations &= ~(1 << closestStationNum);
            select--;
        }
        closestStationNum = bitscanforward(entranceStations);
    }

    uint16 entranceXY;
    if (numEntranceStations == 0)
        entranceXY = ride->station_starts[closestStationNum]; // closestStationNum is always 0 here.
    else
        entranceXY = ride->entrances[closestStationNum];

    x = (entranceXY & 0xFF) * 32;
    y = (entranceXY & 0xFF00) / 8;
    z = ride->station_heights[closestStationNum];

    get_ride_queue_end(&x, &y, &z);

    gPeepPathFindGoalPosition = (rct_xyz16) { x, y, z };
    gPeepPathFindIgnoreForeignQueues = true;

    direction = peep_pathfind_choose_direction(peep->next_x, peep->next_y, peep->next_z, peep);

    if (direction == -1){
        /* Heuristic search failed for all directions.
         * Reset the pathfind_goal - this means that the pathfind_history
         * will be reset in the next call to peep_pathfind_choose_direction().
         * This lets the heuristic search "try again" in case the player has
         * edited the path layout or the mechanic was already stuck in the
         * save game (e.g. with a worse version of the pathfinding). */
        peep_reset_pathfind_goal(peep);

        #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug) {
            log_info("Completed guest_path_finding for %s - failed to choose a direction == aimless.", gPathFindDebugPeepName);
        }
        pathfind_logging_disable();
        #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        return guest_path_find_aimless(peep, edges);
    }
    #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (gPathFindDebug) {
        log_info("Completed guest_path_finding for %s - direction chosen: %d.", gPathFindDebugPeepName, direction);
    }
    pathfind_logging_disable();
    #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    return peep_move_one_tile(direction, peep);
}

/**
 *
 *  rct2: 0x00693C9E
 */
static sint32 peep_perform_next_action(rct_peep *peep)
{
    _unk_F1EE18 = 0;
    uint8 previousAction = peep->action;

    if (peep->action == PEEP_ACTION_NONE_1)
        peep->action = PEEP_ACTION_NONE_2;

    if (peep->state == PEEP_STATE_QUEUING){
        if (peep_update_queue_position(peep, previousAction))
            return 1;
    }

    sint16 x, y, xy_dist;
    if (!peep_update_action(&x, &y, &xy_dist, peep)){
        _unk_F1EE18 |= F1EE18_DESTINATION_REACHED;
        uint8 result = 0;
        if (peep->type == PEEP_TYPE_GUEST){
            result = guest_path_finding(peep);
        }
        else{
            result = staff_path_finding(peep);
        }

        if (result != 0)
            return 1;

        if (!peep_update_action(&x, &y, &xy_dist, peep))
            return 1;
    }

    if ((x & 0xFFE0) == peep->next_x && (y & 0xFFE0) == peep->next_y){
        sint16 z = peep_get_height_on_slope(peep, x, y);
        invalidate_sprite_2((rct_sprite*)peep);
        sprite_move(x, y, z, (rct_sprite*)peep);
        invalidate_sprite_2((rct_sprite*)peep);
        return 1;
    }

    if (x < 32 || y < 32 || x >= gMapSizeUnits || y >= gMapSizeUnits){
        if (peep->outside_of_park == 1){
            _unk_F1EE18 |= F1EE18_OUTSIDE_PARK;
        }
        return peep_return_to_center_of_tile(peep);
    }

    rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);
    sint16 base_z = max(0, (peep->z / 8) - 2);
    sint16 top_z = (peep->z / 8) + 1;

    do{
        if (base_z > mapElement->base_height)
            continue;
        if (top_z < mapElement->base_height)
            continue;
        if (mapElement->flags & MAP_ELEMENT_FLAG_GHOST)
            continue;

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH){
            if (peep_interact_with_path(peep, x, y, mapElement))
                return 1;
        }
        else if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK){
            if (peep_interact_with_shop(peep, x, y, mapElement))
                return 1;
        }
        else if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE){
            if (peep_interact_with_entrance(peep, x, y, mapElement))
                return 1;
        }
    } while (!map_element_is_last_for_tile(mapElement++));

    if (peep->type == PEEP_TYPE_STAFF || (peep->next_var_29 & 0x18)){
        sint16 z = abs(map_element_height(x, y) - peep->z);

        if (z <= 3 || (peep->type == PEEP_TYPE_STAFF && z <= 32)){
            peep->interaction_ride_index = 0xFF;
            if (peep->state == PEEP_STATE_QUEUING){
                remove_peep_from_queue(peep);
                peep_decrement_num_riders(peep);
                peep->state = PEEP_STATE_1;
                peep_window_state_update(peep);
            }

            if (!map_is_location_in_park(x & 0xFFE0, y & 0xFFE0)){
                return peep_return_to_center_of_tile(peep);
            }

            mapElement = map_get_surface_element_at(x / 32, y / 32);
            if (mapElement == NULL)
                return peep_return_to_center_of_tile(peep);

            sint16 water_height = mapElement->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
            if (water_height)
                return peep_return_to_center_of_tile(peep);

            peep->next_x = x & 0xFFE0;
            peep->next_y = y & 0xFFE0;
            peep->next_z = mapElement->base_height;
            peep->next_var_29 = 8;

            z = peep_get_height_on_slope(peep, x, y);
            invalidate_sprite_2((rct_sprite*)peep);
            sprite_move(x, y, z, (rct_sprite*)peep);
            invalidate_sprite_2((rct_sprite*)peep);
            return 1;
        }
    }
    return peep_return_to_center_of_tile(peep);
}

/**
 *
 *  rct2: 0x0069926C
 * Expend type was previously an offset saved in 0x00F1AEC0
 */
static void peep_spend_money(rct_peep *peep, money16 *peep_expend_type, money32 amount)
{
    assert(!(gParkFlags & PARK_FLAGS_NO_MONEY));

    peep->cash_in_pocket = max(0, peep->cash_in_pocket - amount);
    peep->cash_spent += amount;
    if (peep_expend_type != NULL) {
        *peep_expend_type += (money16)amount;
    }
    window_invalidate_by_number(WC_PEEP, peep->sprite_index);

    gUnk141F568 = gUnk13CA740;
    finance_payment(-amount, gCommandExpenditureType);

    if (gConfigGeneral.show_guest_purchases && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) {
        // HACK Currently disabled for multiplayer due to limitation of all sprites
        //      needing to be synchronised
        if (network_get_mode() == NETWORK_MODE_NONE) {
            money_effect_create_at(amount, peep->x, peep->y, peep->z, true);
        }
    }

    audio_play_sound_at_location(SOUND_PURCHASE, peep->x, peep->y, peep->z);
}

static void peep_set_has_ridden(rct_peep *peep, sint32 rideIndex)
{
    peep->rides_been_on[rideIndex / 8] |= 1 << (rideIndex % 8);
    rct_ride *ride = get_ride(rideIndex);
    peep_set_has_ridden_ride_type(peep, ride->type);
}

static bool peep_has_ridden(rct_peep *peep, sint32 rideIndex)
{
    return peep->rides_been_on[rideIndex / 8] & (1 << (rideIndex % 8));
}

static void peep_set_has_ridden_ride_type(rct_peep *peep, sint32 rideType)
{
    peep->ride_types_been_on[rideType / 8] |= 1 << (rideType % 8);
}

static bool peep_has_ridden_ride_type(rct_peep *peep, sint32 rideType)
{
    return peep->ride_types_been_on[rideType / 8] & (1 << (rideType % 8));
}

/**
 * Updates various peep stats upon entering a ride, as well as updating the
 * ride's satisfaction value.
 *  rct2: 0x0069545B
 */
static void peep_on_enter_ride(rct_peep *peep, sint32 rideIndex)
{
    rct_ride *ride = get_ride(rideIndex);

    // Calculate how satisfying the ride is for the peep. Can range from -140 to +105.
    sint16 satisfaction = peep_calculate_ride_satisfaction(peep, ride);

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
    if (peep->no_of_rides < 255)
        peep->no_of_rides++;

    peep_set_has_ridden(peep, peep->current_ride);
    peep_update_favourite_ride(peep, ride);
    peep->happiness_growth_rate = clamp(0, peep->happiness_growth_rate + satisfaction, 255);
    peep_update_ride_nausea_growth(peep, ride);
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
static void peep_update_favourite_ride(rct_peep *peep, rct_ride *ride)
{
    peep->peep_flags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
    uint8 peepRideRating = clamp(0, (ride->excitement / 4) + peep->happiness, 255);
    if (peepRideRating >= peep->favourite_ride_rating) {
        if (peep->happiness >= 160 && peep->happiness_growth_rate >= 160) {
            peep->favourite_ride_rating = peepRideRating;
            peep->peep_flags |= PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        }
    }
}

/* rct2: 0x00695555 */
static sint16 peep_calculate_ride_value_satisfaction(rct_peep* peep, rct_ride* ride) {
    if (gParkFlags & PARK_FLAGS_NO_MONEY){
        return -30;
    }

    if (ride->value == 0xFFFF) {
        return -30;
    }

    money16 ridePrice = ride_get_price(ride);
    if (ride->value >= ridePrice) {
        return -5;
    }

    if ((ride->value + ((ride->value * peep->happiness) / 256)) >= ridePrice) {
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
static sint16 peep_calculate_ride_intensity_nausea_satisfaction(rct_peep * peep, rct_ride * ride)
{
    if (!ride_has_ratings(ride)) {
        return 70;
    }

    uint8 intensitySatisfaction = 3;
    uint8 nauseaSatisfaction = 3;
    ride_rating maxIntensity = (peep->intensity >> 4) * 100;
    ride_rating minIntensity = (peep->intensity & 0xF) * 100;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity) {
        intensitySatisfaction--;
    }
    minIntensity -= peep->happiness * 2;
    maxIntensity += peep->happiness;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity) {
        intensitySatisfaction--;
    }
    minIntensity -= peep->happiness * 2;
    maxIntensity += peep->happiness;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity) {
        intensitySatisfaction--;
    }

    // Although it's not shown in the interface, a peep with Average or High nausea tolerance
    // has a minimum preferred nausea value. (For peeps with None or Low, this is set to zero.)
    ride_rating minNausea = NauseaMinimumThresholds[(peep->nausea_tolerance & 3)];
    ride_rating maxNausea = NauseaMaximumThresholds[(peep->nausea_tolerance & 3)];
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea) {
        nauseaSatisfaction--;
    }
    minNausea -= peep->happiness * 2;
    maxNausea += peep->happiness;
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea) {
        nauseaSatisfaction--;
    }
    minNausea -= peep->happiness * 2;
    maxNausea += peep->happiness;
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea) {
        nauseaSatisfaction--;
    }

    uint8 highestSatisfaction = max(intensitySatisfaction, nauseaSatisfaction);
    uint8 lowestSatisfaction = min(intensitySatisfaction, nauseaSatisfaction);

    switch (highestSatisfaction) {
    default:
    case 0:
        return 70;
    case 1:
        switch (lowestSatisfaction) {
        default:
        case 0: return 50;
        case 1: return 35;
        }
    case 2:
        switch (lowestSatisfaction) {
        default:
        case 0: return 35;
        case 1: return 20;
        case 2: return 10;
        }
    case 3:
        switch (lowestSatisfaction) {
        default:
        case 0: return -35;
        case 1: return -50;
        case 2: return -60;
        case 3: return -60;
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
static sint16 peep_calculate_ride_satisfaction(rct_peep *peep, rct_ride *ride)
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
    if (peep_has_ridden_ride_type(peep, ride->type))
        satisfaction += 10;

    if (peep_has_ridden(peep, peep->current_ride))
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
static void peep_update_ride_nausea_growth(rct_peep *peep, rct_ride *ride)
{
    uint32 nauseaMultiplier = clamp(64, 256 - peep->happiness_growth_rate, 200);
    uint32 nauseaGrowthRateChange = (ride->nausea * nauseaMultiplier) / 512;
    nauseaGrowthRateChange *= max(128, peep->hunger) / 64;
    nauseaGrowthRateChange >>= (peep->nausea_tolerance & 3);
    peep->nausea_growth_rate = (uint8)clamp(0, peep->nausea_growth_rate + nauseaGrowthRateChange, 255);
}

static bool peep_should_go_on_ride_again(rct_peep *peep, rct_ride *ride)
{
    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN)) return false;
    if (!ride_has_ratings(ride)) return false;
    if (ride->intensity > RIDE_RATING(10,00) && !gCheatsIgnoreRideIntensity) return false;
    if (peep->happiness < 180) return false;
    if (peep->energy < 100) return false;
    if (peep->nausea > 160) return false;
    if (peep->hunger < 30) return false;
    if (peep->thirst < 20) return false;
    if (peep->balloon_colour > 170) return false;

    uint8 r = (peep_rand() & 0xFF);
    if (r <= 128) {
        if (peep->no_of_rides > 7) return false;
        if (r > 64) return false;
    }

    return true;
}

static bool peep_should_preferred_intensity_increase(rct_peep *peep)
{
    if (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES) return false;
    if (peep->happiness < 200) return false;

    return (peep_rand() & 0xFF) >= peep->intensity;
}

static bool peep_really_liked_ride(rct_peep *peep, rct_ride *ride)
{
    if (peep->happiness < 215) return false;
    if (peep->nausea > 120) return false;
    if (!ride_has_ratings(ride)) return false;
    if (ride->intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity) return false;
    return true;
}

/**
 *
 *  rct2: 0x0069576E
 */
static void peep_on_exit_ride(rct_peep *peep, sint32 rideIndex)
{
    rct_ride *ride = get_ride(rideIndex);

    if (peep->peep_flags & PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE) {
        peep->peep_flags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        peep->favourite_ride = rideIndex;
        // TODO fix this flag name or add another one
        peep->window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
    peep->happiness = peep->happiness_growth_rate;
    peep->nausea = peep->nausea_growth_rate;
    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_STATS;

    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
        peep->peep_flags &= ~(PEEP_FLAGS_PARK_ENTRANCE_CHOSEN);

    if (peep_should_go_on_ride_again(peep, ride)) {
        peep->guest_heading_to_ride_id = rideIndex;
        peep->peep_is_lost_countdown = 200;
        peep_reset_pathfind_goal(peep);

        rct_window *w = window_find_by_number(WC_PEEP, peep->sprite_index);
        if (w != NULL) {
            window_event_invalidate_call(w);
            widget_invalidate(w, WC_PEEP__WIDX_ACTION_LBL);
        }
    }

    if (peep_should_preferred_intensity_increase(peep)) {
        if (peep->intensity <= 255 - 16) {
            peep->intensity += 16;
        }
    }

    if (peep_really_liked_ride(peep, ride)) {
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_WAS_GREAT, rideIndex);

        sint32 laugh = peep_rand() & 7;
        if (laugh < 3) {
            audio_play_sound_at_location(SOUND_LAUGH_1 + laugh, peep->x, peep->y, peep->z);
        }
    }

    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
}

/**
 *
 *  rct2: 0x00695444
 */
static void peep_on_enter_or_exit_ride(rct_peep *peep, sint32 rideIndex, sint32 flags)
{
    if (flags & 1) {
        peep_on_exit_ride(peep, rideIndex);
    } else {
        peep_on_enter_ride(peep, rideIndex);
    }
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
static bool peep_decide_and_buy_item(rct_peep *peep, sint32 rideIndex, sint32 shopItem, money32 price)
{
    rct_ride* ride = get_ride(rideIndex);
    money32 value;

    bool has_voucher = false;

    if ((peep->item_standard_flags & PEEP_ITEM_VOUCHER) &&
        (peep->voucher_type == VOUCHER_TYPE_FOOD_OR_DRINK_FREE) &&
        (peep->voucher_arguments == shopItem)) {
        has_voucher = true;
    }

    if (peep_has_item(peep, shopItem)) {
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_ALREADY_GOT, shopItem);
        return 0;
    }

    if (shop_item_is_food_or_drink(shopItem)) {
        sint32 food = -1;
        if ((food = peep_has_food_standard_flag(peep)) != 0) {
            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_HAVENT_FINISHED, bitscanforward(food));
            return 0;
        } else if ((food = peep_has_food_extra_flag(peep)) != 0) {
            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_HAVENT_FINISHED, bitscanforward(food) + 32);
            return 0;
        } else if (peep->nausea >= 145)
            return 0;
    }

    if ((shopItem == SHOP_ITEM_BALLOON) || (shopItem == SHOP_ITEM_ICE_CREAM)
        || (shopItem == SHOP_ITEM_CANDYFLOSS) || (shopItem == SHOP_ITEM_SUNGLASSES)) {
        if (gClimateCurrentRainLevel != 0)
            return 0;
    }

    if ((shopItem == SHOP_ITEM_SUNGLASSES) || (shopItem == SHOP_ITEM_ICE_CREAM)) {
        if (gClimateCurrentTemperature < 12)
            return 0;
    }

    if (shop_item_is_food(shopItem) && (peep->hunger > 75)) {
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_NOT_HUNGRY, 0xFF);
        return 0;
    }

    if (shop_item_is_drink(shopItem) && (peep->thirst > 75)) {
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_NOT_THIRSTY, 0xFF);
        return 0;
    }

    if ((shopItem == SHOP_ITEM_UMBRELLA) && (gClimateCurrentRainLevel != 0))
            goto loc_69B119;

    if ((shopItem != SHOP_ITEM_MAP) && shop_item_is_souvenir(shopItem) && !has_voucher) {
        if (((peep_rand() & 0x7F) + 0x73) > peep->happiness)
            return 0;
        else if (peep->no_of_rides < 3)
            return 0;
    }

loc_69B119:
    if (!has_voucher) {
        if (price != 0) {
            if (peep->cash_in_pocket == 0) {
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_SPENT_MONEY, 0xFF);
                return 0;
            }
            if (price > peep->cash_in_pocket) {
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CANT_AFFORD, shopItem);
                return 0;
            }
        }

        if (gClimateCurrentTemperature >= 21)
            value = get_shop_hot_value(shopItem);
        else if (gClimateCurrentTemperature <= 11)
            value = get_shop_cold_value(shopItem);
        else
            value = get_shop_base_value(shopItem);

        if (value < price) {
            value -= price;
            if (shopItem == SHOP_ITEM_UMBRELLA) {
                if (gClimateCurrentRainLevel != 0)
                    goto loc_69B221;
            }

            value = -value;
            if (peep->happiness >= 128)
                value /= 2;

            if (peep->happiness >= 180)
                value /= 2;

            if (value > ((money16)(peep_rand() & 0x07))) {
                // "I'm not paying that much for x"
                uint8 thought_type = (shopItem >= 32 ? (PEEP_THOUGHT_TYPE_PHOTO2_MUCH + (shopItem - 32)) : (PEEP_THOUGHT_TYPE_BALLOON_MUCH + shopItem));
                peep_insert_new_thought(peep, thought_type, rideIndex);
                return 0;
            }
        }
        else {
            value -= price;
            value = max(8, value);

            if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
                if (value >= (money32)(peep_rand() & 0x07)) {
                    // "This x is a really good value"
                    uint8 thought_item = (shopItem >= 32 ? (PEEP_THOUGHT_TYPE_PHOTO2 + (shopItem - 32)) : (PEEP_THOUGHT_TYPE_BALLOON + shopItem));
                    peep_insert_new_thought(peep, thought_item, rideIndex);
                }
            }

            sint32 happinessGrowth = value * 4;
            peep->happiness_growth_rate = min((peep->happiness_growth_rate + happinessGrowth), 255);
            peep->happiness = min((peep->happiness + happinessGrowth), 255);
        }
    }

loc_69B221:
    if (!has_voucher) {
        if (gClimateCurrentTemperature >= 21)
            value = get_shop_hot_value(shopItem);
        else if (gClimateCurrentTemperature <= 11)
            value = get_shop_cold_value(shopItem);
        else
            value = get_shop_base_value(shopItem);

        value -= price;
        uint8 satisfaction = 0;
        if (value > -8) {
            satisfaction++;
            if (value > -3) {
                satisfaction++;
                if (value > 3)
                    satisfaction++;
            }
        }

        ride_update_satisfaction(ride, satisfaction);
    }

    // The peep has now decided to buy the item (or, specifically, has not been
    // dissuaded so far).
    if (shopItem >= 32)
        peep->item_extra_flags |= (1u << (shopItem - 32));
    else
        peep->item_standard_flags |= (1u << shopItem);

    if (shopItem == SHOP_ITEM_TSHIRT)
        peep->tshirt_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_HAT)
        peep->hat_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_BALLOON)
        peep->balloon_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_UMBRELLA)
        peep->umbrella_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_MAP)
        peep_reset_pathfind_goal(peep);

    uint16 dl = byte_9822F4[shopItem];
    peep->var_42 = min((peep->var_42 + dl), 255);

    if (shopItem == SHOP_ITEM_PHOTO)
        peep->photo1_ride_ref = rideIndex;

    if (shopItem == SHOP_ITEM_PHOTO2)
        peep->photo2_ride_ref = rideIndex;

    if (shopItem == SHOP_ITEM_PHOTO3)
        peep->photo3_ride_ref = rideIndex;

    if (shopItem == SHOP_ITEM_PHOTO4)
        peep->photo4_ride_ref = rideIndex;

    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    peep_update_sprite_type(peep);
    if (peep->peep_flags & PEEP_FLAGS_TRACKING) {
        set_format_arg(0, rct_string_id, peep->name_string_idx);
        set_format_arg(2, uint32, peep->id);
        set_format_arg(6, rct_string_id, ShopItemStringIds[shopItem].indefinite);
        if (gConfigNotifications.guest_bought_item) {
            news_item_add_to_queue(2, STR_PEEP_TRACKING_NOTIFICATION_BOUGHT_X, peep->sprite_index);
        }
    }

    if (shop_item_is_food(shopItem))
        peep->no_of_food++;

    if (shop_item_is_drink(shopItem))
        peep->no_of_drinks++;

    if (shop_item_is_souvenir(shopItem))
        peep->no_of_souvenirs++;

    money16* expend_type = &peep->paid_on_souvenirs;
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_SHOP_STOCK;

    if (shop_item_is_food(shopItem)) {
        expend_type = &peep->paid_on_food;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_FOODDRINK_STOCK;
    }

    if (shop_item_is_drink(shopItem)) {
        expend_type = &peep->paid_on_drink;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_FOODDRINK_STOCK;
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        finance_payment(get_shop_item_cost(shopItem), gCommandExpenditureType);

    // Sets the expenditure type to *_FOODDRINK_SALES or *_SHOP_SALES appropriately.
    gCommandExpenditureType--;
    if (has_voucher) {
        peep->item_standard_flags &= ~PEEP_ITEM_VOUCHER;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }
    else if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
        peep_spend_money(peep, expend_type, price);
    }
    ride->total_profit += (price - get_shop_item_cost(shopItem));
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
    ride->cur_num_customers++;
    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

    return 1;
}

/**
 *
 *  rct2: 0x0069AEB7
 */
static bool peep_should_use_cash_machine(rct_peep *peep, sint32 rideIndex)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY) return false;
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) return false;
    if (peep->cash_in_pocket > MONEY(20,00)) return false;
    if (115 + (peep_rand() % 128) > peep->happiness) return false;
    if (peep->energy < 80) return false;

    rct_ride *ride = get_ride(rideIndex);
    ride_update_satisfaction(ride, peep->happiness >> 6);
    ride->cur_num_customers++;
    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
    return true;
}

/**
 *
 *  rct2: 0x0069A98C
 */
void peep_reset_pathfind_goal(rct_peep *peep)
{

    #if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (gPathFindDebug) {
        log_info("Resetting pathfind_goal for %s", gPathFindDebugPeepName);
    }
    #endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    peep->pathfind_goal.x = 0xFF;
    peep->pathfind_goal.y = 0xFF;
    peep->pathfind_goal.z = 0xFF;
    peep->pathfind_goal.direction = 0xFF;
}

static bool peep_has_valid_xy(rct_peep *peep)
{
    if (peep->x != MAP_LOCATION_NULL) {
        if (peep->x < (256 * 32) && peep->y < (256 * 32)) {
            return true;
        }
    }

    return false;
}

typedef void(*easter_egg_function)(rct_peep* peep, rct_peep* otherPeep);

static void peep_apply_easter_egg_to_nearby_guests(rct_peep *peep, easter_egg_function easter_egg)
{
    if (!peep_has_valid_xy(peep))
        return;

    uint16 spriteIndex = sprite_get_first_in_quadrant(peep->x, peep->y);
    if (spriteIndex == SPRITE_INDEX_NULL)
        return;

    rct_peep * otherPeep = GET_PEEP(spriteIndex);
    for (; spriteIndex != SPRITE_INDEX_NULL; spriteIndex = otherPeep->next_in_quadrant) {
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

static void peep_give_passing_peeps_purple_clothes(rct_peep *peep, rct_peep *otherPeep)
{
    otherPeep->tshirt_colour = COLOUR_BRIGHT_PURPLE;
    otherPeep->trousers_colour = COLOUR_BRIGHT_PURPLE;
    invalidate_sprite_2((rct_sprite*)otherPeep);
}

static void peep_give_passing_peeps_pizza(rct_peep *peep, rct_peep *otherPeep)
{
    if ((otherPeep->item_standard_flags & PEEP_ITEM_PIZZA))
        return;

    otherPeep->item_standard_flags |= PEEP_ITEM_PIZZA;

    sint32 peepDirection = (peep->sprite_direction >> 3) ^ 2;
    sint32 otherPeepOppositeDirection = otherPeep->sprite_direction >> 3;
    if (peepDirection == otherPeepOppositeDirection) {
        if (otherPeep->action == PEEP_ACTION_NONE_1 || otherPeep->action == PEEP_ACTION_NONE_2) {
            invalidate_sprite_2((rct_sprite*)peep);
            otherPeep->action = PEEP_ACTION_WAVE_2;
            otherPeep->action_frame = 0;
            otherPeep->action_sprite_image_offset = 0;
            peep_update_current_action_sprite_type(otherPeep);
            invalidate_sprite_2((rct_sprite*)otherPeep);
        }
    }
    invalidate_sprite_2((rct_sprite*)otherPeep);
}

static void peep_make_passing_peeps_sick(rct_peep *peep, rct_peep *otherPeep)
{
    if (peep == otherPeep)
        return;
    if (otherPeep->state == PEEP_STATE_QUEUING)
        return;

    if (otherPeep->action == PEEP_ACTION_NONE_1 || otherPeep->action == PEEP_ACTION_NONE_2) {
        otherPeep->action = PEEP_ACTION_THROW_UP;
        otherPeep->action_frame = 0;
        otherPeep->action_sprite_image_offset = 0;
        peep_update_current_action_sprite_type(otherPeep);
        invalidate_sprite_2((rct_sprite*)otherPeep);
    }
}

static void peep_give_passing_peeps_ice_cream(rct_peep *peep, rct_peep *otherPeep)
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
static void peep_easter_egg_peep_interactions(rct_peep *peep)
{
    if (peep->peep_flags & PEEP_FLAGS_PURPLE) {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_give_passing_peeps_purple_clothes);
    }

    if (peep->peep_flags & PEEP_FLAGS_PIZZA) {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_give_passing_peeps_pizza);
    }

    if (peep->peep_flags & PEEP_FLAGS_CONTAGIOUS) {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_make_passing_peeps_sick);
    }

    if (peep->peep_flags & PEEP_FLAGS_JOY) {
        if (peep_rand() <= 1456) {
            if (peep->action == PEEP_ACTION_NONE_1 || peep->action == PEEP_ACTION_NONE_2) {
                peep->action = PEEP_ACTION_JOY;
                peep->action_frame = 0;
                peep->action_sprite_image_offset = 0;
                peep_update_current_action_sprite_type(peep);
                invalidate_sprite_2((rct_sprite*)peep);
            }
        }
    }

    if (peep->peep_flags & PEEP_FLAGS_ICE_CREAM) {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_give_passing_peeps_ice_cream);
    }
}

/**
 * rct2: 0x0069101A
 *
 * @return (CF)
 */
static bool peep_should_watch_ride(rct_map_element *mapElement) {
    rct_ride *ride = get_ride(mapElement->properties.track.ride_index);

    // Ghosts are purely this-client-side and should not cause any interaction,
    // as that may lead to a desync.
    if (network_get_mode() != NETWORK_MODE_NONE) {
        if (map_element_is_ghost(mapElement)) return false;
    }

    if (gRideClassifications[ride->type] != RIDE_CLASS_RIDE) {
        return false;
    }

    // This is most likely to have peeps watch new rides
    if (ride->excitement == RIDE_RATING_UNDEFINED) {
        return true;
    }

    if (ride->excitement >= RIDE_RATING(4, 70)) {
        return true;
    }

    if (ride->intensity >= RIDE_RATING(4, 50)) {
        return true;
    }

    if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_INTERESTING_TO_LOOK_AT) {
        if ((scenario_rand() & 0xFFFF) > 0x3333) {
            return false;
        }
    } else if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_SLIGHTLY_INTERESTING_TO_LOOK_AT) {
        if ((scenario_rand() & 0xFFFF) > 0x1000) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

/**
 *
 *  rct2: 0x00690B99
 *
 * @param edge (eax)
 * @param peep (esi)
 * @param[out] rideToView (cl)
 * @param[out] rideSeatToView (ch)
 * @return !CF
 */
static bool peep_find_ride_to_look_at(rct_peep *peep, uint8 edge, uint8 *rideToView, uint8 *rideSeatToView)
{
    rct_map_element *mapElement, *surfaceElement;

    surfaceElement = map_get_surface_element_at(peep->next_x / 32, peep->next_y / 32);

    mapElement = surfaceElement;
    do {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE) {
            if (map_element_is_ghost(mapElement)) continue;
        }
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_WALL) continue;
        if (map_element_get_direction(mapElement) != edge) continue;
        if (get_wall_entry(mapElement->properties.wall.type)->wall.flags2 & WALL_SCENERY_2_FLAG4) continue;
        if (peep->next_z + 4 <= mapElement->base_height) continue;
        if (peep->next_z + 1 >= mapElement->clearance_height) continue;

        return false;
    } while (!map_element_is_last_for_tile(mapElement++));


    uint16 x = peep->next_x + TileDirectionDelta[edge].x;
    uint16 y = peep->next_y + TileDirectionDelta[edge].y;
    if (x > 255 * 32 || y > 255 * 32) {
        return false;
    }

    surfaceElement = map_get_surface_element_at(x / 32, y / 32);


    mapElement = surfaceElement;
    do {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE) {
            if (map_element_is_ghost(mapElement)) continue;
        }
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_WALL) continue;
        if ((map_element_get_direction(mapElement) ^ 0x2) != edge) continue;
        if (get_wall_entry(mapElement->properties.wall.type)->wall.flags2 & WALL_SCENERY_2_FLAG4) continue;
        // TODO: Check whether this shouldn't be <=, as the other loops use. If so, also extract as loop A.
        if (peep->next_z + 4 >= mapElement->base_height) continue;
        if (peep->next_z + 1 >= mapElement->clearance_height) continue;

        return false;
    } while (!map_element_is_last_for_tile(mapElement++));


    // TODO: Extract loop B
    mapElement = surfaceElement;
    do {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE) {
            if (map_element_is_ghost(mapElement)) continue;
        }

        if (mapElement->clearance_height + 1 < peep->next_z) continue;
        if (peep->next_z + 6 < mapElement->base_height) continue;

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK) {
            if (peep_should_watch_ride(mapElement)) {
                return loc_690FD0(peep, rideToView, rideSeatToView, mapElement);
            }
        }

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE) {
            if (!(get_large_scenery_entry(mapElement->properties.scenerymultiple.type & 0x3FF)->large_scenery.flags & LARGE_SCENERY_FLAG5)) {
                continue;
            }

            *rideSeatToView = 0;
            if (mapElement->clearance_height >= peep->next_z + 8) {
                *rideSeatToView = 0x02;
            }

            *rideToView = 0xFF;

            return true;
        }
    } while (!map_element_is_last_for_tile(mapElement++));


    // TODO: Extract loop C
    mapElement = surfaceElement;
    do {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE) {
            if (map_element_is_ghost(mapElement)) continue;
        }
        if (mapElement->clearance_height + 1 < peep->next_z) continue;
        if (peep->next_z + 6 < mapElement->base_height) continue;
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SURFACE) continue;
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH) continue;

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_WALL) {
            if (get_wall_entry(mapElement->properties.wall.type)->wall.flags2 & WALL_SCENERY_2_FLAG4) {
                continue;
            }
        }

        return false;
    } while (!map_element_is_last_for_tile(mapElement++));


    x += TileDirectionDelta[edge].x;
    y += TileDirectionDelta[edge].y;
    if (x > 255 * 32 || y > 255 * 32) {
        return false;
    }

    surfaceElement = map_get_surface_element_at(x / 32, y / 32);


    // TODO: extract loop A
    mapElement = surfaceElement;
    do {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE) {
            if (map_element_is_ghost(mapElement)) continue;
        }
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_WALL) continue;
        if ((map_element_get_direction(mapElement) ^ 0x2) != edge) continue;
        if (get_wall_entry(mapElement->properties.wall.type)->wall.flags2 & WALL_SCENERY_2_FLAG4) continue;
        if (peep->next_z + 6 <= mapElement->base_height) continue;
        if (peep->next_z >= mapElement->clearance_height) continue;

        return false;
    } while (!map_element_is_last_for_tile(mapElement++));


    // TODO: Extract loop B
    mapElement = surfaceElement;
    do {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE) {
            if (map_element_is_ghost(mapElement)) continue;
        }
        if (mapElement->clearance_height + 1 < peep->next_z) continue;
        if (peep->next_z + 8 < mapElement->base_height) continue;

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK) {
            if (peep_should_watch_ride(mapElement)) {
                return loc_690FD0(peep, rideToView, rideSeatToView, mapElement);
            }
        }

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE) {
            if (!(get_large_scenery_entry(mapElement->properties.scenerymultiple.type & 0x3FF)->large_scenery.flags & LARGE_SCENERY_FLAG5)) {
                continue;
            }

            *rideSeatToView = 0;
            if (mapElement->clearance_height >= peep->next_z + 8) {
                *rideSeatToView = 0x02;
            }

            *rideToView = 0xFF;

            return true;
        }
    } while (!map_element_is_last_for_tile(mapElement++));


    // TODO: Extract loop C
    mapElement = surfaceElement;
    do {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE) {
            if (map_element_is_ghost(mapElement)) continue;
        }
        if (mapElement->clearance_height + 1 < peep->next_z) continue;
        if (peep->next_z + 8 < mapElement->base_height) continue;
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SURFACE) continue;
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH) continue;

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_WALL) {
            if (get_wall_entry(mapElement->properties.wall.type)->wall.flags2 & WALL_SCENERY_2_FLAG4) {
                continue;
            }
        }

        return false;
    } while (!map_element_is_last_for_tile(mapElement++));

    x += TileDirectionDelta[edge].x;
    y += TileDirectionDelta[edge].y;
    if (x > 255 * 32 || y > 255 * 32) {
        return false;
    }

    surfaceElement = map_get_surface_element_at(x / 32, y / 32);


    // TODO: extract loop A
    mapElement = surfaceElement;
    do {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE) {
            if (map_element_is_ghost(mapElement)) continue;
        }
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_WALL) continue;
        if ((map_element_get_direction(mapElement) ^ 0x2) != edge) continue;
        if (get_wall_entry(mapElement->properties.wall.type)->wall.flags2 & WALL_SCENERY_2_FLAG4) continue;
        if (peep->next_z + 8 <= mapElement->base_height) continue;
        if (peep->next_z >= mapElement->clearance_height) continue;

        return false;
    } while (!map_element_is_last_for_tile(mapElement++));


    // TODO: Extract loop B
    mapElement = surfaceElement;
    do {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE) {
            if (map_element_is_ghost(mapElement)) continue;
        }
        if (mapElement->clearance_height + 1 < peep->next_z) continue;
        if (peep->next_z + 10 < mapElement->base_height) continue;

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK) {
            if (peep_should_watch_ride(mapElement)) {
                return loc_690FD0(peep, rideToView, rideSeatToView, mapElement);
            }
        }

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE) {
            if (!(get_large_scenery_entry(mapElement->properties.scenerymultiple.type & 0x3FF)->large_scenery.flags & LARGE_SCENERY_FLAG5)) {
                continue;
            }

            *rideSeatToView = 0;
            if (mapElement->clearance_height >= peep->next_z + 8) {
                *rideSeatToView = 0x02;
            }

            *rideToView = 0xFF;

            return true;
        }
    } while (!map_element_is_last_for_tile(mapElement++));

    return false;
}

bool loc_690FD0(rct_peep *peep, uint8 *rideToView, uint8 *rideSeatToView, rct_map_element *esi) {
    rct_ride *ride = get_ride(esi->properties.track.ride_index);

    *rideToView = esi->properties.track.ride_index;
    if (ride->excitement == RIDE_RATING_UNDEFINED) {
        *rideSeatToView = 1;
        if (ride->status != RIDE_STATUS_OPEN) {
            if (esi->clearance_height > peep->next_z + 8) {
                *rideSeatToView |= (1 << 1);
            }

            return true;
        }
    } else {
        *rideSeatToView = 0;
        if (ride->status == RIDE_STATUS_OPEN && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)) {
            if (esi->clearance_height > peep->next_z + 8) {
                *rideSeatToView = 0x02;
            }

            return true;
        }
    }

    return false;
}

/**
 * Gets the height including the bit depending on how far up the slope the peep
 * is.
 *  rct2: 0x00694921
 */
static sint32 peep_get_height_on_slope(rct_peep *peep, sint32 x, sint32 y)
{
    if (x == MAP_LOCATION_NULL)
        return 0;

    if (peep->next_var_29 & 0x18){
        return map_element_height(x, y) & 0xFFFF;
    }

    sint32 z = peep->next_z * 8;

    return z + map_height_from_slope(x, y, peep->next_var_29);
}

static bool peep_has_voucher_for_free_ride(rct_peep *peep, sint32 rideIndex)
{
    return
        peep->item_standard_flags & PEEP_ITEM_VOUCHER &&
        peep->voucher_type == VOUCHER_TYPE_RIDE_FREE &&
        peep->voucher_arguments == rideIndex;
}

static void peep_reset_ride_heading(rct_peep *peep)
{
    rct_window *w;

    peep->guest_heading_to_ride_id = 255;
    w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != NULL) {
        window_event_invalidate_call(w);
        widget_invalidate(w, WC_PEEP__WIDX_ACTION_LBL);
    }
}

/**
 * This function is called whenever a peep is deciding whether or not they want
 * to go on a ride or visit a shop. They may be physically present at the
 * ride/shop, or they may just be thinking about it.
 *  rct2: 0x006960AB
 */
static bool peep_should_go_on_ride(rct_peep *peep, sint32 rideIndex, sint32 entranceNum, sint32 flags)
{
    rct_ride *ride = get_ride(rideIndex);

    // Indicates if the peep is about to enter a queue (as opposed to entering an entrance directly from a path)
    bool peepAtQueue = flags & PEEP_RIDE_DECISION_AT_QUEUE;

    // Indicates whether a peep is physically at the ride, or is just thinking about going on the ride.
    bool peepAtRide = !(flags & PEEP_RIDE_DECISION_THINKING);

    if (ride->status == RIDE_STATUS_OPEN && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)) {

        // Peeps that are leaving the park will refuse to go on any rides, with the exception of free transport rides.
        if (!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_TRANSPORT_RIDE) ||
            ride->value == 0xFFFF ||
            ride_get_price(ride) != 0
        ) {
            if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) {
                peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, false);
                return false;
            }
        }

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP)) {
            return peep_should_go_to_shop(peep, rideIndex, peepAtRide);
        }

        // This used to check !(flags & 2), but the function is only ever called with flags = 0, 1 or 6.
        // This means we can use the existing !(flags & 4) check.
        if (peepAtRide) {
            // Peeps won't join a queue that has 1000 peeps already in it.
            if (ride->queue_length[entranceNum] >= 1000) {
                peep_tried_to_enter_full_queue(peep, rideIndex);
                return false;
            }

            // Rides without queues can only have one peep waiting at a time.
            if (!peepAtQueue) {
                if (ride->last_peep_in_queue[entranceNum] != 0xFFFF) {
                    peep_tried_to_enter_full_queue(peep, rideIndex);
                    return false;
                }
            }
            else {
                // Check if there's room in the queue for the peep to enter.
                if (ride->last_peep_in_queue[entranceNum] != 0xFFFF) {
                    rct_peep *lastPeepInQueue = GET_PEEP(ride->last_peep_in_queue[entranceNum]);
                    if (abs(lastPeepInQueue->z - peep->z) <= 6) {
                        sint32 dx = abs(lastPeepInQueue->x - peep->x);
                        sint32 dy = abs(lastPeepInQueue->y - peep->y);
                        sint32 maxD = max(dx, dy);

                        // Unlike normal paths, peeps cannot overlap when queueing for a ride.
                        // This check enforces a minimum distance between peeps entering the queue.
                        if (maxD < 8) {
                            peep_tried_to_enter_full_queue(peep, rideIndex);
                            return false;
                        }

                        // This checks if there's a peep standing still at the very end of the queue.
                        if (maxD <= 13
                            && lastPeepInQueue->time_in_queue > 10) {
                            peep_tried_to_enter_full_queue(peep, rideIndex);
                            return false;
                        }
                    }
                }
            }
        }

        // Assuming the queue conditions are met, peeps will always go on free transport rides.
        // Ride ratings, recent crashes and weather will all be ignored.
        money16 ridePrice = ride_get_price(ride);
        if (!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_TRANSPORT_RIDE) || ride->value == 0xFFFF || ridePrice != 0) {
            if (peep->previous_ride == rideIndex) {
                peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, false);
                return false;
            }

            // Basic price checks
            if (ridePrice != 0 && !peep_has_voucher_for_free_ride(peep, rideIndex)) {

                if (ridePrice > peep->cash_in_pocket) {
                    if (peepAtRide) {
                        if (peep->cash_in_pocket <= 0) {
                            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_SPENT_MONEY, 255);
                        }
                        else {
                            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CANT_AFFORD_0, rideIndex);
                        }
                    }
                    peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, true);
                    return false;
                }
            }

            // If happy enough, peeps will ignore the fact that a ride has recently crashed.
            if (ride->last_crash_type != RIDE_CRASH_TYPE_NONE && peep->happiness < 225) {
                if (peepAtRide) {
                    peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_NOT_SAFE, rideIndex);
                    if (peep->happiness_growth_rate >= 64) {
                        peep->happiness_growth_rate -= 8;
                    }
                    ride_update_popularity(ride, 0);
                }
                peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, true);
                return false;
            }


            if (ride_has_ratings(ride)) {
                // If a peep has already decided that they're going to go on a ride, they'll skip the weather and
                // excitement check and will only do a basic intensity check when they arrive at the ride itself.
                if (rideIndex == peep->guest_heading_to_ride_id) {
                    if (ride->intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity) {
                        peep_ride_is_too_intense(peep, rideIndex, peepAtRide);
                        return false;
                    }
                }

                // Peeps won't go on rides that aren't sufficiently undercover while it's raining.
                // The threshold is fairly low and only requires about 10-15% of the ride to be undercover.
                if (gClimateCurrentRainLevel != 0 && (ride->sheltered_eighths >> 5) < 3) {
                    if (peepAtRide) {
                        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_NOT_WHILE_RAINING, rideIndex);
                        if (peep->happiness_growth_rate >= 64) {
                            peep->happiness_growth_rate -= 8;
                        }
                        ride_update_popularity(ride, 0);
                    }
                    peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, true);
                    return false;
                }

                if (!gCheatsIgnoreRideIntensity) {
                    // Intensity calculations. Even though the max intensity can go up to 15, it's capped
                    // at 10.0 (before happiness calculations). A full happiness bar will increase the max
                    // intensity and decrease the min intensity by about 2.5.
                    ride_rating maxIntensity = min((peep->intensity >> 4) * 100, 1000) + peep->happiness;
                    ride_rating minIntensity = ((peep->intensity & 0x0F) * 100) - peep->happiness;
                    if (ride->intensity < minIntensity) {
                        if (peepAtRide) {
                            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_MORE_THRILLING, rideIndex);
                            if (peep->happiness_growth_rate >= 64) {
                                peep->happiness_growth_rate -= 8;
                            }
                            ride_update_popularity(ride, 0);
                        }
                        peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, true);
                        return false;
                    }
                    if (ride->intensity > maxIntensity) {
                        peep_ride_is_too_intense(peep, rideIndex, peepAtRide);
                        return false;
                    }

                    // Nausea calculations.
                    ride_rating maxNausea = NauseaMaximumThresholds[(peep->nausea_tolerance & 3)] + peep->happiness;

                    if (ride->nausea > maxNausea) {
                        if (peepAtRide) {
                            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_SICKENING, rideIndex);
                            if (peep->happiness_growth_rate >= 64) {
                                peep->happiness_growth_rate -= 8;
                            }
                            ride_update_popularity(ride, 0);
                        }
                        peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, true);
                        return false;
                    }

                    // Very nauseous peeps will only go on very gentle rides.
                    if (ride->nausea >= FIXED_2DP(1, 40) && peep->nausea > 160) {
                        peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, false);
                        return false;
                    }
                }
            }

            // If the ride has not yet been rated and is capable of having g-forces,
            // there's a 90% chance that the peep will ignore it.
            if (!ride_has_ratings(ride) && (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_PEEP_CHECK_GFORCES)) {
                if ((peep_rand() & 0xFFFF) > 0x1999U) {
                    peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, false);
                    return false;
                }

                if (!gCheatsIgnoreRideIntensity) {
                    if (ride->max_positive_vertical_g > FIXED_2DP(5, 00)
                        || ride->max_negative_vertical_g < FIXED_2DP(-4, 00)
                        || ride->max_lateral_g > FIXED_2DP(4, 00)) {
                        peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, false);
                        return false;
                    }
                }
            }

            uint32 value = ride->value;

            // If the value of the ride hasn't yet been calculated, peeps will be willing to pay any amount for the ride.
            if (value != 0xFFFF && !peep_has_voucher_for_free_ride(peep, rideIndex) && !(gParkFlags & PARK_FLAGS_NO_MONEY)) {

                // The amount peeps are willing to pay is decreased by 75% if they had to pay to enter the park.
                if (peep->peep_flags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY)
                    value /= 4;

                // Peeps won't pay more than twice the value of the ride.
                ridePrice = ride_get_price(ride);
                if (ridePrice > (money16)(value * 2)) {
                    if (peepAtRide) {
                        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_BAD_VALUE, rideIndex);
                        if (peep->happiness_growth_rate < 60) {
                            peep->happiness_growth_rate -= 16;
                        }
                        ride_update_popularity(ride, 0);
                    }
                    peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, true);
                    return false;
                }

                // A ride is good value if the price is 50% or less of the ride value and the peep didn't pay to enter the park.
                if (ridePrice <= (money16)(value / 2) && peepAtRide) {
                    if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
                        if (!(peep->peep_flags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY)) {
                            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_GOOD_VALUE, rideIndex);
                        }
                    }
                }
            }
        }

        // At this point, the peep has decided to go on the ride.
        if (peepAtRide) {
            ride_update_popularity(ride, 1);
        }

        if (rideIndex == peep->guest_heading_to_ride_id) {
            peep_reset_ride_heading(peep);
        }

        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_QUEUE_FULL;
        return true;
    }

    peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, false);
    return false;
}

static void peep_ride_is_too_intense(rct_peep *peep, sint32 rideIndex, bool peepAtRide)
{
    rct_ride *ride = get_ride(rideIndex);

    if (peepAtRide) {
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_INTENSE, rideIndex);
        if (peep->happiness_growth_rate >= 64) {
            peep->happiness_growth_rate -= 8;
        }
        ride_update_popularity(ride, 0);
    }
    peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtRide, true);
}

static void peep_chose_not_to_go_on_ride(rct_peep *peep, sint32 rideIndex, bool peepAtRide, bool updateLastRide)
{
    if (peepAtRide && updateLastRide) {
        peep->previous_ride = rideIndex;
        peep->previous_ride_time_out = 0;
    }

    if (rideIndex == peep->guest_heading_to_ride_id) {
        peep_reset_ride_heading(peep);
    }
}

/**
 * When the queue is full, peeps will ignore the ride when thinking about what to go on next.
 * Does not effect peeps that walk up to the queue entrance.
 * This flag is reset the next time a peep successfully joins the queue.
 */
static void peep_tried_to_enter_full_queue(rct_peep *peep, sint32 rideIndex)
{
    rct_ride *ride = get_ride(rideIndex);

    ride->lifecycle_flags |= RIDE_LIFECYCLE_QUEUE_FULL;
    peep->previous_ride = rideIndex;
    peep->previous_ride_time_out = 0;
    //Change status "Heading to" to "Walking" if queue is full
    if (rideIndex == peep->guest_heading_to_ride_id) {
        peep_reset_ride_heading(peep);
    }
}

static bool peep_should_go_to_shop(rct_peep *peep, sint32 rideIndex, bool peepAtShop)
{
    rct_ride *ride = get_ride(rideIndex);

    // Peeps won't go to the same shop twice in a row.
    if (rideIndex == peep->previous_ride) {
        peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtShop, true);
        return false;
    }

    if (ride->type == RIDE_TYPE_TOILETS) {
        if (peep->bathroom < 70) {
            peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtShop, true);
            return false;
        }

        // The amount that peeps are willing to pay to use the Toilets scales with their bathroom stat.
        // It effectively has a minimum of $0.10 (due to the check above) and a maximum of $0.60.
        if (ride->price * 40 > peep->bathroom) {
            if (peepAtShop) {
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_NOT_PAYING, rideIndex);
                if (peep->happiness_growth_rate >= 60) {
                    peep->happiness_growth_rate -= 16;
                }
                ride_update_popularity(ride, 0);
            }
            peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtShop, true);
            return false;
        }
    }

    if (ride->type == RIDE_TYPE_FIRST_AID) {
        if (peep->nausea < 128) {
            peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtShop, true);
            return false;
        }
    }

    // Basic price checks
    if (ride->price != 0 && ride->price > peep->cash_in_pocket) {
        if (peepAtShop) {
            if (peep->cash_in_pocket <= 0) {
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_SPENT_MONEY, 255);
            }
            else {
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CANT_AFFORD_0, rideIndex);
            }
        }
        peep_chose_not_to_go_on_ride(peep, rideIndex, peepAtShop, true);
        return false;
    }

    if (peepAtShop) {
        ride_update_popularity(ride, 1);
        if (rideIndex == peep->guest_heading_to_ride_id) {
            peep_reset_ride_heading(peep);
        }
    }
    return true;
}

/**
 *
 *  rct2: 0x00695DD2
 */
static void peep_pick_ride_to_go_on(rct_peep *peep)
{
    rct_ride *ride;

    if (peep->state != PEEP_STATE_WALKING) return;
    if (peep->guest_heading_to_ride_id != 255) return;
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) return;
    if (peep_has_food(peep)) return;
    if (peep->x == MAP_LOCATION_NULL) return;

    for (sint32 i = 0; i < countof(_peepRideConsideration); i++) {
        _peepRideConsideration[i] = 0;
    }

    // FIX  Originally checked for a toy, likely a mistake and should be a map,
    //      but then again this seems to only allow the peep to go on
    //      rides they haven't been on before.
    if (peep->item_standard_flags & PEEP_ITEM_MAP) {
        // Consider rides that peep hasn't been on yet
        sint32 i;
        FOR_ALL_RIDES(i, ride) {
            if (!peep_has_ridden(peep, i)) {
                _peepRideConsideration[i >> 5] |= (1u << (i & 0x1F));
            }
        }
    } else {
        // Take nearby rides into consideration
        sint32 cx = floor2(peep->x, 32);
        sint32 cy = floor2(peep->y, 32);
        for (sint32 x = cx - 320; x <= cx + 320; x += 32) {
            for (sint32 y = cy - 320; y <= cy + 320; y += 32) {
                if (x >= 0 && y >= 0 && x < (256 * 32) && y < (256 * 32)) {
                    rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
                    do {
                        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;

                        sint32 rideIndex = mapElement->properties.track.ride_index;
                        _peepRideConsideration[rideIndex >> 5] |= (1u << (rideIndex & 0x1F));
                    } while (!map_element_is_last_for_tile(mapElement++));
                }
            }
        }

        // Always take the tall rides into consideration (realistic as you can usually see them from anywhere in the park)
        sint32 i;
        FOR_ALL_RIDES(i, ride) {
            if (ride->status != RIDE_STATUS_OPEN) continue;
            if (!ride_has_ratings(ride)) continue;
            if (ride->highest_drop_height <= 66 && ride->excitement < RIDE_RATING(8,00)) continue;

            _peepRideConsideration[i >> 5] |= (1u << (i & 0x1F));
        }
    }

    // Filter the considered rides
    uint8 *potentialRides = _peepPotentialRides;
    uint8 *nextPotentialRide = potentialRides;
    sint32 numPotentialRides = 0;
    for (sint32 i = 0; i < MAX_RIDES; i++) {
        if (!(_peepRideConsideration[i >> 5] & (1u << (i & 0x1F))))
            continue;

        ride = get_ride(i);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL)) {
            if (peep_should_go_on_ride(peep, i, 0, PEEP_RIDE_DECISION_THINKING)) {
                *nextPotentialRide++ = i;
                numPotentialRides++;
            }
        }
    }

    // Pick the most exciting ride
    sint32 mostExcitingRideIndex = -1;
    ride_rating mostExcitingRideRating = 0;
    for (sint32 i = 0; i < numPotentialRides; i++) {
        ride = get_ride(potentialRides[i]);
        if (!ride_has_ratings(ride)) continue;
        if (ride->excitement > mostExcitingRideRating) {
            mostExcitingRideIndex = potentialRides[i];
            mostExcitingRideRating = ride->excitement;
        }
    }
    if (mostExcitingRideIndex == -1)
        return;

    // Head to that ride
    peep->guest_heading_to_ride_id = mostExcitingRideIndex;
    peep->peep_is_lost_countdown = 200;
    peep_reset_pathfind_goal(peep);

    // Invalidate windows
    rct_window *w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != NULL) {
        window_event_invalidate_call(w);
        widget_invalidate(w, WC_PEEP__WIDX_ACTION_LBL);
    }

    // Make peep look at their map if they have one
    if (peep->item_standard_flags & PEEP_ITEM_MAP) {
        peep_read_map(peep);
    }
}

/**
 *
 *  rct2: 0x00695B70
 */
static void peep_head_for_nearest_ride_type(rct_peep *peep, sint32 rideType)
{
    rct_ride *ride;

    if (peep->state != PEEP_STATE_SITTING && peep->state != PEEP_STATE_WATCHING && peep->state != PEEP_STATE_WALKING) {
        return;
    }
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) return;
    if (peep->x == MAP_LOCATION_NULL) return;
    if (peep->guest_heading_to_ride_id != 255) {
        ride = get_ride(peep->guest_heading_to_ride_id);
        if (ride->type == rideType) {
            return;
        }
    }

    for (sint32 i = 0; i < countof(_peepRideConsideration); i++) {
        _peepRideConsideration[i] = 0;
    }

    // FIX Originally checked for a toy,.likely a mistake and should be a map
    if ((peep->item_standard_flags & PEEP_ITEM_MAP) && rideType != RIDE_TYPE_FIRST_AID) {
        // Consider all rides in the park
        sint32 i;
        FOR_ALL_RIDES(i, ride) {
            if (ride->type == rideType) {
                _peepRideConsideration[i >> 5] |= (1u << (i & 0x1F));
            }
        }
    } else {
        // Take nearby rides into consideration
        sint32 cx = floor2(peep->x, 32);
        sint32 cy = floor2(peep->y, 32);
        for (sint32 x = cx - 320; x <= cx + 320; x += 32) {
            for (sint32 y = cy - 320; y <= cy + 320; y += 32) {
                if (x >= 0 && y >= 0 && x < (256 * 32) && y < (256 * 32)) {
                    rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
                    do {
                        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;

                        sint32 rideIndex = mapElement->properties.track.ride_index;
                        ride = get_ride(rideIndex);
                        if (ride->type == rideType) {
                            _peepRideConsideration[rideIndex >> 5] |= (1u << (rideIndex & 0x1F));
                        }
                    } while (!map_element_is_last_for_tile(mapElement++));
                }
            }
        }
    }

    // Filter the considered rides
    uint8 *potentialRides = _peepPotentialRides;
    uint8 *nextPotentialRide = potentialRides;
    sint32 numPotentialRides = 0;
    for (sint32 i = 0; i < MAX_RIDES; i++) {
        if (!(_peepRideConsideration[i >> 5] & (1u << (i & 0x1F))))
            continue;

        ride = get_ride(i);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL)) {
            if (peep_should_go_on_ride(peep, i, 0, PEEP_RIDE_DECISION_THINKING)) {
                *nextPotentialRide++ = i;
                numPotentialRides++;
            }
        }
    }

    // Pick the closest ride
    sint32 closestRideIndex = -1;
    sint32 closestRideDistance = INT_MAX;
    for (sint32 i = 0; i < numPotentialRides; i++) {
        ride = get_ride(potentialRides[i]);
        sint32 rideX = (ride->station_starts[0] & 0xFF) * 32;
        sint32 rideY = (ride->station_starts[0] >> 8) * 32;
        sint32 distance = abs(rideX - peep->x) + abs(rideY - peep->y);
        if (distance < closestRideDistance) {
            closestRideIndex = potentialRides[i];
            closestRideDistance = distance;
        }
    }
    if (closestRideIndex == -1)
        return;

    // Head to that ride
    peep->guest_heading_to_ride_id = closestRideIndex;
    peep->peep_is_lost_countdown = 200;
    peep_reset_pathfind_goal(peep);

    // Invalidate windows
    rct_window *w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != NULL) {
        window_event_invalidate_call(w);
        widget_invalidate(w, WC_PEEP__WIDX_ACTION_LBL);
    }

    peep->var_F4 = 0;
}

/**
 *
 *  rct2: 0x006958D0
 */
static void peep_head_for_nearest_ride_with_flags(rct_peep *peep, sint32 rideTypeFlags)
{
    rct_ride *ride;

    if (peep->state != PEEP_STATE_SITTING && peep->state != PEEP_STATE_WATCHING && peep->state != PEEP_STATE_WALKING) {
        return;
    }
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) return;
    if (peep->x == MAP_LOCATION_NULL) return;
    if (peep->guest_heading_to_ride_id != 255) {
        ride = get_ride(peep->guest_heading_to_ride_id);
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_BATHROOM | RIDE_TYPE_FLAG_SELLS_DRINKS | RIDE_TYPE_FLAG_SELLS_FOOD)) {
            return;
        }
    }

    if ((rideTypeFlags & RIDE_TYPE_FLAG_IS_BATHROOM) && peep_has_food(peep)) {
        return;
    }

    for (sint32 i = 0; i < countof(_peepRideConsideration); i++) {
        _peepRideConsideration[i] = 0;
    }

    // FIX Originally checked for a toy,.likely a mistake and should be a map
    if (peep->item_standard_flags & PEEP_ITEM_MAP) {
        // Consider all rides in the park
        sint32 i;
        FOR_ALL_RIDES(i, ride) {
            if (ride_type_has_flag(ride->type, rideTypeFlags)) {
                _peepRideConsideration[i >> 5] |= (1u << (i & 0x1F));
            }
        }
    } else {
        // Take nearby rides into consideration
        sint32 cx = floor2(peep->x, 32);
        sint32 cy = floor2(peep->y, 32);
        for (sint32 x = cx - 320; x <= cx + 320; x += 32) {
            for (sint32 y = cy - 320; y <= cy + 320; y += 32) {
                if (x >= 0 && y >= 0 && x < (256 * 32) && y < (256 * 32)) {
                    rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
                    do {
                        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;

                        sint32 rideIndex = mapElement->properties.track.ride_index;
                        ride = get_ride(rideIndex);
                        if (ride_type_has_flag(ride->type, rideTypeFlags)) {
                            _peepRideConsideration[rideIndex >> 5] |= (1u << (rideIndex & 0x1F));
                        }
                    } while (!map_element_is_last_for_tile(mapElement++));
                }
            }
        }
    }

    // Filter the considered rides
    uint8 *potentialRides = _peepPotentialRides;
    uint8 *nextPotentialRide = potentialRides;
    sint32 numPotentialRides = 0;
    for (sint32 i = 0; i < MAX_RIDES; i++) {
        if (!(_peepRideConsideration[i >> 5] & (1u << (i & 0x1F))))
            continue;

        ride = get_ride(i);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL)) {
            if (peep_should_go_on_ride(peep, i, 0, PEEP_RIDE_DECISION_THINKING)) {
                *nextPotentialRide++ = i;
                numPotentialRides++;
            }
        }
    }

    // Pick the closest ride
    sint32 closestRideIndex = -1;
    sint32 closestRideDistance = INT_MAX;
    for (sint32 i = 0; i < numPotentialRides; i++) {
        ride = get_ride(potentialRides[i]);
        sint32 rideX = (ride->station_starts[0] & 0xFF) * 32;
        sint32 rideY = (ride->station_starts[0] >> 8) * 32;
        sint32 distance = abs(rideX - peep->x) + abs(rideY - peep->y);
        if (distance < closestRideDistance) {
            closestRideIndex = potentialRides[i];
            closestRideDistance = distance;
        }
    }
    if (closestRideIndex == -1)
        return;

    // Head to that ride
    peep->guest_heading_to_ride_id = closestRideIndex;
    peep->peep_is_lost_countdown = 200;
    peep_reset_pathfind_goal(peep);

    // Invalidate windows
    rct_window *w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != NULL) {
        window_event_invalidate_call(w);
        window_invalidate(w);
    }

    peep->var_F4 = 0;
}

/**
 *
 *  rct2: 0x0069C483
 */
static void peep_give_real_name(rct_peep *peep)
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
    if (dx < ax) {
        dx += 0x1000;
    }
    dx /= 4;
    dx += 0xA000;
    peep->name_string_idx = dx;
}

static sint32 peep_compare(const void *sprite_index_a, const void *sprite_index_b)
{
    rct_peep const *peep_a = GET_PEEP(*(uint16*)sprite_index_a);
    rct_peep const *peep_b = GET_PEEP(*(uint16*)sprite_index_b);

    // Compare types
    if (peep_a->type != peep_b->type) {
        return peep_a->type - peep_b->type;
    }

    // Simple ID comparison for when both peeps use a number or a generated name
    const bool both_numbers = (
        peep_a->name_string_idx >= 767 && peep_a->name_string_idx <= 771 &&
        peep_b->name_string_idx >= 767 && peep_b->name_string_idx <= 771
    );
    if (both_numbers) {
        return peep_a->id - peep_b->id;
    }
    const bool both_have_generated_names = (
        peep_a->name_string_idx >= 0xA000 && peep_a->name_string_idx < 0xE000 &&
        peep_b->name_string_idx >= 0xA000 && peep_b->name_string_idx < 0xE000
    );
    if (both_have_generated_names) {
        rct_string_id peep_a_format = peep_a->name_string_idx + 0xA000;
        rct_string_id peep_b_format = peep_b->name_string_idx + 0xA000;

        uint16 peep_a_name = (peep_a_format % countof(real_names));
        uint16 peep_b_name = (peep_b_format % countof(real_names));

        if (peep_a_name == peep_b_name) {
            uint16 peep_a_initial = ((peep_a_format >> 10) % countof(real_name_initials));
            uint16 peep_b_initial = ((peep_b_format >> 10) % countof(real_name_initials));
            return peep_a_initial - peep_b_initial;
        } else {
            return peep_a_name - peep_b_name;
        }
    }

    // At least one of them has a custom name assigned
    // Compare their names as strings
    utf8 name_a[256];
    utf8 name_b[256];
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
void peep_update_name_sort(rct_peep *peep)
{
    // Remove peep from sprite list
    uint16 nextSpriteIndex = peep->next;
    uint16 prevSpriteIndex = peep->previous;
    if (prevSpriteIndex != SPRITE_INDEX_NULL) {
        rct_peep *prevPeep = GET_PEEP(prevSpriteIndex);
        prevPeep->next = nextSpriteIndex;
    } else {
        gSpriteListHead[SPRITE_LIST_PEEP] = nextSpriteIndex;
    }

    if (nextSpriteIndex != SPRITE_INDEX_NULL) {
        rct_peep *nextPeep = GET_PEEP(nextSpriteIndex);
        nextPeep->previous = prevSpriteIndex;
    }

    rct_peep *otherPeep;
    uint16 spriteIndex;
    FOR_ALL_PEEPS(spriteIndex, otherPeep) {
        // Check if peep should go before this one
        if (peep_compare(&peep->sprite_index, &otherPeep->sprite_index) >= 0) {
            continue;
        }

        // Place peep before this one
        peep->previous = otherPeep->previous;
        otherPeep->previous = peep->sprite_index;
        if (peep->previous != SPRITE_INDEX_NULL) {
            rct_peep *prevPeep = GET_PEEP(peep->previous);
            peep->next = prevPeep->next;
            prevPeep->next = peep->sprite_index;
        } else {
            peep->next = gSpriteListHead[SPRITE_LIST_PEEP];
            gSpriteListHead[SPRITE_LIST_PEEP] = peep->sprite_index;
        }
        goto finish_peep_sort;
    }

    // Place peep at the end
    FOR_ALL_PEEPS(spriteIndex, otherPeep) {
        if (otherPeep->next == SPRITE_INDEX_NULL) {
            otherPeep->next = peep->sprite_index;
            peep->previous = otherPeep->sprite_index;
            peep->next = SPRITE_INDEX_NULL;
            goto finish_peep_sort;
        }
    }

    gSpriteListHead[SPRITE_LIST_PEEP] = peep->sprite_index;
    peep->next = SPRITE_INDEX_NULL;
    peep->previous = SPRITE_INDEX_NULL;

finish_peep_sort:
    // This is required at the moment because this function reorders peeps in the sprite list
    sprite_position_tween_reset();
}

void peep_sort()
{
    // Count number of peeps
    uint16 sprite_index, num_peeps = 0;
    rct_peep *peep;
    FOR_ALL_PEEPS(sprite_index, peep) {
        num_peeps++;
    }

    // No need to sort
    if (num_peeps < 2)
        return;

    // Create a copy of the peep list and sort it using peep_compare
    uint16 *peep_list = (uint16*)malloc(num_peeps * sizeof(uint16));
    sint32 i = 0;
    FOR_ALL_PEEPS(sprite_index, peep) {
        peep_list[i++] = peep->sprite_index;
    }
    qsort(peep_list, num_peeps, sizeof(uint16), peep_compare);

    // Set the correct peep->next and peep->previous using the sorted list
    for (i = 0; i < num_peeps; i++) {
        peep = GET_PEEP(peep_list[i]);
        peep->previous = (i > 0) ? peep_list[i - 1] : SPRITE_INDEX_NULL;
        peep->next = (i + 1 < num_peeps) ? peep_list[i + 1] : SPRITE_INDEX_NULL;
    }
    // Make sure the first peep is set
    gSpriteListHead[SPRITE_LIST_PEEP] = peep_list[0];

    free(peep_list);

    i = 0;
    FOR_ALL_PEEPS(sprite_index, peep) {
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
    if (realNames) {
        gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        rct_peep *peep;
        uint16 spriteIndex;
        FOR_ALL_GUESTS(spriteIndex, peep) {
            if (peep->name_string_idx == STR_GUEST_X) {
                peep_give_real_name(peep);
            }
        }
    } else {
        gParkFlags &= ~PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        rct_peep *peep;
        uint16 spriteIndex;
        FOR_ALL_GUESTS(spriteIndex, peep) {
            if (peep->name_string_idx >= 0xA000 && peep->name_string_idx < 0xE000) {
                peep->name_string_idx = STR_GUEST_X;
            }
        }
    }

    peep_sort();
    gfx_invalidate_screen();
}

static void peep_read_map(rct_peep *peep)
{
    if (peep->action == PEEP_ACTION_NONE_1 || peep->action == PEEP_ACTION_NONE_2) {
        peep->action = PEEP_ACTION_READ_MAP;
        peep->action_frame = 0;
        peep->action_sprite_image_offset = 0;
        peep_update_current_action_sprite_type(peep);
        invalidate_sprite_2((rct_sprite*)peep);
    }
}

static bool peep_heading_for_ride_or_park_exit(rct_peep *peep)
{
    return (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) || peep->guest_heading_to_ride_id != 0xFF;
}

money32 set_peep_name(sint32 flags, sint32 state, uint16 sprite_index, uint8* text_1, uint8* text_2, uint8* text_3) {
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;

    static char newName[128];
    //if (flags & GAME_COMMAND_FLAG_APPLY) { // this check seems to be useless and causes problems in multiplayer
        uint8 position = (state - 1) & 3;
        memcpy(newName + position * 12, text_1, 4);
        memcpy(newName + 4 + position * 12, text_2, 4);
        memcpy(newName + 8 + position * 12, text_3, 4);
    //}

    if (state != 0)
        return 0;

    rct_peep* peep = GET_PEEP(sprite_index);
    set_format_arg(0, uint32, peep->id);
    utf8* curName = gCommonStringFormatBuffer;
    rct_string_id curId = peep->name_string_idx;
    format_string(curName, 256, curId, gCommonFormatArgs);

    if (strcmp(curName, newName) == 0)
        return 0;

    if (*newName == '\0') {
        gGameCommandErrorText = STR_ERR_INVALID_NAME_FOR_GUEST;
        return MONEY32_UNDEFINED;
    }

    rct_string_id newId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER, newName);
    if (newId == 0) {
        return MONEY32_UNDEFINED;
    }

    if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
        user_string_free(newId);
        return 0;
    }

    user_string_free(curId);
    peep->name_string_idx = newId;

    peep_update_name_sort(peep);

    peep->peep_flags &= ~PEEP_FLAGS_WAVING;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATIE_BRAYSHAW, peep)) {
        peep->peep_flags |= PEEP_FLAGS_WAVING;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PHOTO;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_CHRIS_SAWYER, peep)) {
        peep->peep_flags |= PEEP_FLAGS_PHOTO;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PAINTING;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_SIMON_FOSTER, peep)) {
        peep->peep_flags |= PEEP_FLAGS_PAINTING;
    }

    peep->peep_flags &= ~PEEP_FLAGS_WOW;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_JOHN_WARDLEY, peep)) {
        peep->peep_flags |= PEEP_FLAGS_WOW;
    }

    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_MELANIE_WARN, peep)) {
        peep->happiness = 250;
        peep->happiness_growth_rate = 250;
        peep->energy = 127;
        peep->energy_growth_rate = 127;
        peep->nausea = 0;
        peep->nausea_growth_rate = 0;
    }

    peep->peep_flags &= ~PEEP_FLAGS_LITTER;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_LISA_STIRLING, peep)) {
        peep->peep_flags |= PEEP_FLAGS_LITTER;
    }

    peep->peep_flags &= ~PEEP_FLAGS_LOST;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_DONALD_MACRAE, peep)) {
        peep->peep_flags |= PEEP_FLAGS_LOST;
    }

    peep->peep_flags &= ~PEEP_FLAGS_HUNGER;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATHERINE_MCGOWAN, peep)) {
        peep->peep_flags |= PEEP_FLAGS_HUNGER;
    }

    peep->peep_flags &= ~PEEP_FLAGS_BATHROOM;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_FRANCES_MCGOWAN, peep)) {
        peep->peep_flags |= PEEP_FLAGS_BATHROOM;
    }

    peep->peep_flags &= ~PEEP_FLAGS_CROWDED;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_CORINA_MASSOURA, peep)) {
        peep->peep_flags |= PEEP_FLAGS_CROWDED;
    }

    peep->peep_flags &= ~PEEP_FLAGS_HAPPINESS;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_CAROL_YOUNG, peep)) {
        peep->peep_flags |= PEEP_FLAGS_HAPPINESS;
    }

    peep->peep_flags &= ~PEEP_FLAGS_NAUSEA;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_MIA_SHERIDAN, peep)) {
        peep->peep_flags |= PEEP_FLAGS_NAUSEA;
    }

    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATIE_RODGER, peep)) {
        peep->peep_flags |= PEEP_FLAGS_LEAVING_PARK;
        peep->peep_flags &= ~PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PURPLE;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_EMMA_GARRELL, peep)) {
        peep->peep_flags |= PEEP_FLAGS_PURPLE;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PIZZA;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_JOANNE_BARTON, peep)) {
        peep->peep_flags |= PEEP_FLAGS_PIZZA;
    }

    peep->peep_flags &= ~PEEP_FLAGS_CONTAGIOUS;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_FELICITY_ANDERSON, peep)) {
        peep->peep_flags |= PEEP_FLAGS_CONTAGIOUS;
    }

    peep->peep_flags &= ~PEEP_FLAGS_JOY;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATIE_SMITH, peep)) {
        peep->peep_flags |= PEEP_FLAGS_JOY;
    }

    peep->peep_flags &= ~PEEP_FLAGS_ANGRY;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_EILIDH_BELL, peep)) {
        peep->peep_flags |= PEEP_FLAGS_ANGRY;
    }

    peep->peep_flags &= ~PEEP_FLAGS_ICE_CREAM;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_NANCY_STILLWAGON, peep)) {
        peep->peep_flags |= PEEP_FLAGS_ICE_CREAM;
    }

    peep->peep_flags &= ~PEEP_FLAGS_HERE_WE_ARE;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_DAVID_ELLIS, peep)) {
        peep->peep_flags |= PEEP_FLAGS_HERE_WE_ARE;
    }

    gfx_invalidate_screen();
    return 0;
}

/**
 *
 *  rct2: 0x00698D6C
 */
void game_command_set_guest_name(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp) {
    uint16 sprite_index = *ecx & 0xFFFF;

    if (sprite_index >= MAX_SPRITES) {
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    rct_peep *peep = GET_PEEP(sprite_index);
    if (peep->type != PEEP_TYPE_GUEST) {
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    *ebx = set_peep_name(
        *ebx & 0xFF,
        *eax & 0xFFFF,
        sprite_index,
        (uint8*)edx,
        (uint8*)ebp,
        (uint8*)edi
        );
}

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
void pathfind_logging_enable(rct_peep* peep) {
    #if defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
    /* Determine if the pathfinding debugging is wanted for this peep. */
    format_string(gPathFindDebugPeepName, sizeof(gPathFindDebugPeepName), peep->name_string_idx, &(peep->id));

    /* For guests, use the existing PEEP_FLAGS_TRACKING flag to
     * determine for which guest(s) the pathfinding debugging will
     * be output for. */
    if (peep->type == PEEP_TYPE_GUEST){
        gPathFindDebug = peep->peep_flags & PEEP_FLAGS_TRACKING;
    }
    /* For staff, there is no tracking button (any other similar
     * suitable existing mechanism?), so fall back to a crude
     * string comparison with a compile time hardcoded name. */
    else {
        gPathFindDebug = strcmp(gPathFindDebugPeepName, "Mechanic Debug") == 0;
    }
    #endif // defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
}

void pathfind_logging_disable() {
    #if defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
    gPathFindDebug = false;
    #endif // defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
}
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

void peep_autoposition(rct_peep *newPeep)
{
    // Find a location to place new staff member

    newPeep->state = PEEP_STATE_FALLING;

    sint16 x, y, z;
    uint32 count = 0;
    uint16 sprite_index;
    rct_peep *guest = NULL;

    // Count number of walking guests
    FOR_ALL_GUESTS(sprite_index, guest) {
                if (guest->state == PEEP_STATE_WALKING)
                    ++count;
            }

    if (count > 0) {
        // Place staff at a random guest
        uint32 rand = scenario_rand_max(count);
        FOR_ALL_GUESTS(sprite_index, guest) {
                    if (guest->state == PEEP_STATE_WALKING) {
                        if (rand == 0)
                            break;
                        --rand;
                    }
                }

        x = guest->x;
        y = guest->y;
        z = guest->z;
    } else {
        // No walking guests; pick random park entrance
        count = 0;
        uint8 i;
        for (i = 0; i < MAX_PARK_ENTRANCES; ++i) {
            if (gParkEntrances[i].x != SPRITE_LOCATION_NULL)
                ++count;
        }

        if (count > 0) {
            uint32 rand = scenario_rand_max(count);
            for (i = 0; i < MAX_PARK_ENTRANCES; ++i) {
                if (gParkEntrances[i].x != SPRITE_LOCATION_NULL) {
                    if (rand == 0)
                        break;
                    --rand;
                }
            }

            uint8 dir = gParkEntrances[i].direction;
            x = gParkEntrances[i].x;
            y = gParkEntrances[i].y;
            z = gParkEntrances[i].z;
            x += 16 + ((dir & 1) == 0 ? ((dir & 2) ? 32 : -32) : 0);
            y += 16 + ((dir & 1) == 1 ? ((dir & 2) ? -32 : 32) : 0);
        } else {
            // No more options; user must pick a location
            newPeep->state = PEEP_STATE_PICKED;
            x = newPeep->x;
            y = newPeep->y;
            z = newPeep->z;
        }
    }

    sprite_move(x, y, z + 16, (rct_sprite *)newPeep);
    invalidate_sprite_2((rct_sprite *)newPeep);
}

void increment_guests_in_park(){
    if (gNumGuestsInPark < UINT16_MAX){
        gNumGuestsInPark++;
    } else {
        openrct2_assert(false, "Attempt to increment guests in park above max value (65535).");
    }
}

void increment_guests_heading_for_park(){
    if (gNumGuestsHeadingForPark < UINT16_MAX){
        gNumGuestsHeadingForPark++;
    } else {
        openrct2_assert(false, "Attempt to increment guests heading for park above max value (65535).");
    }
}

void decrement_guests_in_park(){
    if (gNumGuestsInPark > 0){
        gNumGuestsInPark--;
    } else {
        log_error("Attempt to decrement guests in park below zero.");
    }
}

void decrement_guests_heading_for_park(){
    if (gNumGuestsHeadingForPark > 0){
        gNumGuestsHeadingForPark--;
    } else {
        log_error("Attempt to decrement guests heading for park below zero.");
    }
}

static void peep_release_balloon(rct_peep *peep, sint16 spawn_height) {

    if (peep->item_standard_flags & PEEP_ITEM_BALLOON)
    {
        peep->item_standard_flags &= ~PEEP_ITEM_BALLOON;

        if (peep->sprite_type == PEEP_SPRITE_TYPE_BALLOON && peep->x != MAP_LOCATION_NULL)
        {
            create_balloon(peep->x, peep->y, spawn_height, peep->balloon_colour, false);
            peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            peep_update_sprite_type(peep);
        }
    }
}
