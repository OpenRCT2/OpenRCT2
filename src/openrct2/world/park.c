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

#include "../cheats.h"
#include "../config/Config.h"
#include "../game.h"
#include "../interface/colour.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../management/award.h"
#include "../management/finance.h"
#include "../management/marketing.h"
#include "../management/news_item.h"
#include "../management/research.h"
#include "../network/network.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../rct2.h"
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../scenario/scenario.h"
#include "../world/map.h"
#include "entrance.h"
#include "park.h"
#include "sprite.h"

rct_string_id gParkName;
uint32 gParkNameArgs;
uint32 gParkFlags;
uint16 gParkRating;
money16 gParkEntranceFee;
uint16 gParkSize;
money16 gLandPrice;
money16 gConstructionRightsPrice;

uint32 gTotalAdmissions;
money32 gTotalIncomeFromAdmissions;

money32 gParkValue;
money32 gCompanyValue;

sint16 gParkRatingCasualtyPenalty;
uint8 gParkRatingHistory[32];
uint8 gGuestsInParkHistory[32];

// If this value is more than or equal to 0, the park rating is forced to this value. Used for cheat
sint32 gForcedParkRating = -1;

/**
 * In a difficult guest generation scenario, no guests will be generated if over this value.
 */
sint32 _suggestedGuestMaximum;

/**
 * Probability out of 65535, of gaining a new guest per game tick.
 * new guests per second = 40 * (probability / 65535)
 * With a full park rating, non-overpriced entrance fee, less guests than the suggested maximum and four positive awards,
 * approximately 1 guest per second can be generated (+60 guests in one minute).
 */
sint32 _guestGenerationProbability;

sint32 park_is_open()
{
    return (gParkFlags & PARK_FLAGS_PARK_OPEN) != 0;
}

/**
 *
 *  rct2: 0x00667132
 */
void park_init()
{
    sint32 i;

    gUnk13CA740 = 0;
    gParkName = STR_UNNAMED_PARK;
    gStaffHandymanColour = COLOUR_BRIGHT_RED;
    gStaffMechanicColour = COLOUR_LIGHT_BLUE;
    gStaffSecurityColour = COLOUR_YELLOW;
    gNumGuestsInPark = 0;
    gNumGuestsInParkLastWeek = 0;
    gNumGuestsHeadingForPark = 0;
    gGuestChangeModifier = 0;
    gParkRating = 0;
    _guestGenerationProbability = 0;
    gTotalRideValue = 0;
    gResearchLastItemSubject = (uint32)-1;

    for (i = 0; i < 20; i++)
        gMarketingCampaignDaysLeft[i] = 0;

    research_reset_items();
    finance_init();

    for (i = 0; i < 2; i++)
        gResearchedRideTypes[i] = 0;

    reset_researched_scenery_items();

    gParkEntranceFee = MONEY(10, 00);
    gPeepSpawns[0].x = PEEP_SPAWN_UNDEFINED;
    gPeepSpawns[1].x = PEEP_SPAWN_UNDEFINED;
    gResearchPriorities =
        (1 << RESEARCH_CATEGORY_TRANSPORT) |
        (1 << RESEARCH_CATEGORY_GENTLE) |
        (1 << RESEARCH_CATEGORY_ROLLERCOASTER) |
        (1 << RESEARCH_CATEGORY_THRILL) |
        (1 << RESEARCH_CATEGORY_WATER) |
        (1 << RESEARCH_CATEGORY_SHOP) |
        (1 << RESEARCH_CATEGORY_SCENERYSET);
    gResearchFundingLevel = RESEARCH_FUNDING_NORMAL;

    gGuestInitialCash = MONEY(50,00); // Cash per guest (average)
    gGuestInitialHappiness = calculate_guest_initial_happiness(50); // 50%
    gGuestInitialHunger = 200;
    gGuestInitialThirst = 200;
    gScenarioObjectiveType = OBJECTIVE_GUESTS_BY;
    gScenarioObjectiveYear = 4;
    gScenarioObjectiveNumGuests = 1000;
    gLandPrice = MONEY(90, 00);
    gConstructionRightsPrice = MONEY(40,00);
    gParkFlags = PARK_FLAGS_NO_MONEY | PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
    park_reset_history();
    finance_reset_history();
    award_reset();

    gS6Info.name[0] = '\0';
    format_string(gS6Info.details, 256, STR_NO_DETAILS_YET, NULL);
}

/**
 *
 *  rct2: 0x0066729F
 */
void park_reset_history()
{
    for (sint32 i = 0; i < 32; i++) {
        gParkRatingHistory[i] = 255;
        gGuestsInParkHistory[i] = 255;
    }
}

/**
 *
 *  rct2: 0x0066A348
 */
sint32 park_calculate_size()
{
    sint32 tiles;
    map_element_iterator it;

    tiles = 0;
    map_element_iterator_begin(&it);
    do {
        if (map_element_get_type(it.element) == MAP_ELEMENT_TYPE_SURFACE) {
            if (it.element->properties.surface.ownership & (OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED | OWNERSHIP_OWNED)) {
                tiles++;
            }
        }
    } while (map_element_iterator_next(&it));

    if (tiles != gParkSize) {
        gParkSize = tiles;
        window_invalidate_by_class(WC_PARK_INFORMATION);
    }

    return tiles;
}

/**
 *
 *  rct2: 0x00669EAA
 */
sint32 calculate_park_rating()
{
    if (gForcedParkRating >= 0)
        return gForcedParkRating;

    sint32 result;

    result = 1150;
    if (gParkFlags & PARK_FLAGS_DIFFICULT_PARK_RATING)
        result = 1050;

    // Guests
    {
        rct_peep* peep;
        uint16 spriteIndex;
        sint32 num_happy_peeps;
        sint32 num_lost_guests;

        // -150 to +3 based on a range of guests from 0 to 2000
        result -= 150 - (min(2000, gNumGuestsInPark) / 13);

        // Find the number of happy peeps and the number of peeps who can't find the park exit
        num_happy_peeps = 0;
        num_lost_guests = 0;
        FOR_ALL_GUESTS(spriteIndex, peep) {
            if (peep->outside_of_park != 0)
                continue;
            if (peep->happiness > 128)
                num_happy_peeps++;
            if ((peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) && (peep->peep_is_lost_countdown < 90))
                num_lost_guests++;
        }

        // Peep happiness -500 to +0
        result -= 500;

        if (gNumGuestsInPark > 0)
            result += 2 * min(250, (num_happy_peeps * 300) / gNumGuestsInPark);

        // Up to 25 guests can be lost without affecting the park rating.
        if (num_lost_guests > 25)
            result -= (num_lost_guests - 25) * 7;
    }

    // Rides
    {
        sint32 i;
        sint16 total_ride_uptime = 0, total_ride_intensity = 0, total_ride_excitement = 0;
        sint32 num_rides, num_exciting_rides = 0;
        rct_ride* ride;

        num_rides = 0;
        FOR_ALL_RIDES(i, ride) {
            total_ride_uptime += 100 - ride->downtime;

            if (ride->excitement != RIDE_RATING_UNDEFINED){
                total_ride_excitement += ride->excitement / 8;
                total_ride_intensity += ride->intensity / 8;
                num_exciting_rides++;
            }
            num_rides++;
        }
        result -= 200;
        if (num_rides > 0)
            result += (total_ride_uptime / num_rides) * 2;

        result -= 100;

        if (num_exciting_rides>0){
            sint16 average_excitement = total_ride_excitement / num_exciting_rides;
            sint16 average_intensity = total_ride_intensity / num_exciting_rides;

            average_excitement -= 46;
            if (average_excitement < 0){
                average_excitement = -average_excitement;
            }

            average_intensity -= 65;
            if (average_intensity < 0){
                average_intensity = -average_intensity;
            }

            average_excitement = min(average_excitement / 2, 50);
            average_intensity = min(average_intensity / 2, 50);
            result += 100 - average_excitement - average_intensity;
        }

        total_ride_excitement = min(1000, total_ride_excitement);
        total_ride_intensity = min(1000, total_ride_intensity);
        result -= 200 - ((total_ride_excitement + total_ride_intensity) / 10);
    }

    // Litter
    {
        rct_litter* litter;
        uint16 sprite_idx;
        sint16 num_litter;

        num_litter = 0;
        for (sprite_idx = gSpriteListHead[SPRITE_LIST_LITTER]; sprite_idx != SPRITE_INDEX_NULL; sprite_idx = litter->next) {
            litter = &(get_sprite(sprite_idx)->litter);

            // Ignore recently dropped litter
            if (litter->creationTick - gScenarioTicks >= 7680)
                num_litter++;
        }
        result -= 600 - (4 * (150 - min(150, num_litter)));
    }

    result -= gParkRatingCasualtyPenalty;
    result = clamp(0, result, 999);
    return result;
}

static money32 calculate_ride_value(rct_ride *ride)
{
    if (ride->type == RIDE_TYPE_NULL)
        return 0;
    if (ride->value == RIDE_VALUE_UNDEFINED)
        return 0;

    // Fair value * (...)
    return (ride->value * 10) * (ride_customers_in_last_5_minutes(ride) + rideBonusValue[ride->type] * 4);
}

/**
 *
 *  rct2: 0x0066A3F6
 */
money32 calculate_park_value()
{

    // Sum ride values
    money32 result = 0;
    for (sint32 i = 0; i < 255; i++) {
        rct_ride* ride = get_ride(i);
        result += calculate_ride_value(ride);
    }

    // +7.00 per guest
    result += gNumGuestsInPark * MONEY(7, 00);

    return result;
}

/**
 * Calculate the company value.
 * Cash + Park Value - Loan
 *
 *  rct2: 0x0066A498
 */
money32 calculate_company_value()
{
    return
        DECRYPT_MONEY(gCashEncrypted) +
        gParkValue -
        gBankLoan;
}

/**
 *
 *  rct2: 0x00667104
 */
void reset_park_entry()
{
    gParkName = 0;
    reset_park_entrance();
    for (sint32 i = 0; i < MAX_PEEP_SPAWNS; i++) {
        gPeepSpawns[i].x = PEEP_SPAWN_UNDEFINED;
    }
}

/**
 * Calculates the probability of a new guest. Also sets total ride value and suggested guest maximum.
 * Total ride value should probably be set elsewhere, as it's not just used for guest generation.
 * Suggested guest maximum should probably be an output result, not a global.
 * @returns A probability out of 65535
 *  rct2: 0x0066730A
 */
static sint32 park_calculate_guest_generation_probability()
{
    uint32 probability;
    sint32 i, suggestedMaxGuests;
    money16 totalRideValue;
    rct_ride *ride;

    // Calculate suggested guest maximum (based on ride type) and total ride value
    suggestedMaxGuests = 0;
    totalRideValue = 0;
    FOR_ALL_RIDES(i, ride) {
        if (ride->status != RIDE_STATUS_OPEN)
            continue;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
            continue;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)
            continue;

        // Add guest score for ride type
        suggestedMaxGuests += rideBonusValue[ride->type];

        // Add ride value
        if (ride->value != RIDE_VALUE_UNDEFINED) {
            money16 rideValue = (money16)(ride->value - ride->price);
            if (rideValue > 0) {
                totalRideValue += rideValue * 2;
            }
        }
    }

    // If difficult guest generation, extra guests are available for good rides
    if (gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION) {
        suggestedMaxGuests = min(suggestedMaxGuests, 1000);
        FOR_ALL_RIDES(i, ride) {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
                continue;
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)
                continue;

            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK))
                continue;
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_DATA_LOGGING))
                continue;
            if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
                continue;
            if (ride->length[0] < (600 << 16))
                continue;
            if (ride->excitement < RIDE_RATING(6,00))
                continue;

            // Bonus guests for good ride
            suggestedMaxGuests += rideBonusValue[ride->type] * 2;
        }
    }

    suggestedMaxGuests = min(suggestedMaxGuests, 65535);
    gTotalRideValue = totalRideValue;
    _suggestedGuestMaximum = suggestedMaxGuests;

    // Begin with 50 + park rating
    probability = 50 + clamp(0, gParkRating - 200, 650);

    // The more guests, the lower the chance of a new one
    sint32 numGuests = gNumGuestsInPark + gNumGuestsHeadingForPark;
    if (numGuests > suggestedMaxGuests) {
        probability /= 4;

        // Even lower for difficult guest generation
        if (gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION)
            probability /= 4;
    }

    // Reduces chance for any more than 7000 guests
    if (numGuests > 7000)
        probability /= 4;

    // Penalty for overpriced entrance fee relative to total ride value
    money16 entranceFee = park_get_entrance_fee();
    if (entranceFee > totalRideValue) {
        probability /= 4;

        // Extra penalty for very overpriced entrance fee
        if (entranceFee / 2 > totalRideValue)
            probability /= 4;
    }

    // Reward or penalties for park awards
    for (i = 0; i < MAX_AWARDS; i++) {
        Award *award = &gCurrentAwards[i];
        if (award->Time == 0)
            continue;

        // +/- 0.25% of the probability
        if (award_is_positive(award->Type))
            probability += probability / 4;
        else
            probability -= probability / 4;
    }

    return probability;
}

/**
 * Choose a random peep spawn and iterates through until defined spawn is found.
 */
static uint32 get_random_peep_spawn_index()
{
    uint32 spawnIndexList[MAX_PEEP_SPAWNS];
    uint32 numSpawns = map_get_available_peep_spawn_index_list(spawnIndexList);
    if (numSpawns > 0) {
        return spawnIndexList[scenario_rand() % numSpawns];
    }
    else {
        return 0;
    }
}

rct_peep *park_generate_new_guest()
{
    rct_peep *peep = NULL;
    rct2_peep_spawn spawn = gPeepSpawns[get_random_peep_spawn_index()];

    if (spawn.x != 0xFFFF) {
        spawn.direction ^= 2;
        peep = peep_generate(spawn.x, spawn.y, spawn.z * 16);
        if (peep != NULL) {
            peep->sprite_direction = spawn.direction << 3;

            // Get the centre point of the tile the peep is on
            peep->destination_x = (peep->x & 0xFFE0) + 16;
            peep->destination_y = (peep->y & 0xFFE0) + 16;

            peep->destination_tolerence = 5;
            peep->var_76 = 0;
            peep->direction = spawn.direction;
            peep->var_37 = 0;
            peep->state = PEEP_STATE_ENTERING_PARK;
        }
    }

    return peep;
}

static rct_peep *park_generate_new_guest_due_to_campaign(sint32 campaign)
{
    rct_peep *peep = park_generate_new_guest();
    if (peep != NULL)
        marketing_set_guest_campaign(peep, campaign);
    return peep;
}

static void park_generate_new_guests()
{
    // Generate a new guest for some probability
    if ((sint32)(scenario_rand() & 0xFFFF) < _guestGenerationProbability) {
        sint32 difficultGeneration = (gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION) != 0;
        if (!difficultGeneration || _suggestedGuestMaximum + 150 >= gNumGuestsInPark)
            park_generate_new_guest();
    }

    // Extra guests generated by advertising campaigns
    sint32 campaign;
    for (campaign = 0; campaign < ADVERTISING_CAMPAIGN_COUNT; campaign++) {
        if (gMarketingCampaignDaysLeft[campaign] != 0) {
            // Random chance of guest generation
            if ((sint32)(scenario_rand() & 0xFFFF) < marketing_get_campaign_guest_generation_probability(campaign))
                park_generate_new_guest_due_to_campaign(campaign);
        }
    }
}

/**
 *
 *  rct2: 0x006674F7
 */
void park_update()
{
    if (gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        return;

    // Every 5 seconds approximately
    if (gCurrentTicks % 512 == 0) {
        gParkRating = calculate_park_rating();
        gParkValue = calculate_park_value();
        gCompanyValue = calculate_company_value();
        window_invalidate_by_class(WC_FINANCES);
        _guestGenerationProbability = park_calculate_guest_generation_probability();
        gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PARK_RATING;
        window_invalidate_by_class(WC_PARK_INFORMATION);
    }

    // Generate new guests
    park_generate_new_guests();
}

uint8 calculate_guest_initial_happiness(uint8 percentage) {
    if (percentage < 15) {
        // There is a minimum of 15% happiness
        percentage = 15;
    }
    else if (percentage > 98) {
        // There is a maximum of 98% happiness
        percentage = 98;
    }

    /* The percentages follow this sequence:
        15 17 18 20 21 23 25 26 28 29 31 32 34 36 37 39 40 42 43 45 47 48 50 51 53...

        This sequence can be defined as PI*(9+n)/2 (the value is floored)
        */
    uint8 n;
    for (n = 1; n < 55; n++) {
        if ((3.14159*(9 + n)) / 2 >= percentage) {
            return (9 + n) * 4;
        }
    }
    return 40; // This is the lowest possible value
}

/**
 *
 *  rct2: 0x0066A231
 */
void park_update_histories()
{
    sint32 guestsInPark = gNumGuestsInPark;
    sint32 lastGuestsInPark = gNumGuestsInParkLastWeek;
    gNumGuestsInParkLastWeek = guestsInPark;
    gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PEEP_COUNT;

    sint32 changeInGuestsInPark = guestsInPark - lastGuestsInPark;
    sint32 guestChangeModifier = 1;
    if (changeInGuestsInPark > -20) {
        guestChangeModifier++;
        if (changeInGuestsInPark < 20)
            guestChangeModifier = 0;
    }
    gGuestChangeModifier = guestChangeModifier;

    // Update park rating history
    for (sint32 i = 31; i > 0; i--)
        gParkRatingHistory[i] = gParkRatingHistory[i - 1];
    gParkRatingHistory[0] = calculate_park_rating() / 4;
    window_invalidate_by_class(WC_PARK_INFORMATION);

    // Update guests in park history
    for (sint32 i = 31; i > 0; i--)
        gGuestsInParkHistory[i] = gGuestsInParkHistory[i - 1];
    gGuestsInParkHistory[0] = min(guestsInPark, 5000) / 20;
    window_invalidate_by_class(WC_PARK_INFORMATION);

    // Update current cash history
    for (sint32 i = 127; i > 0; i--)
        gCashHistory[i] = gCashHistory[i - 1];
    gCashHistory[0] = DECRYPT_MONEY(gCashEncrypted) - gBankLoan;
    window_invalidate_by_class(WC_FINANCES);

    // Update weekly profit history
    money32 currentWeeklyProfit = gWeeklyProfitAverageDividend;
    if (gWeeklyProfitAverageDivisor != 0) {
        currentWeeklyProfit /= gWeeklyProfitAverageDivisor;
    }

    for (sint32 i = 127; i > 0; i--)
        gWeeklyProfitHistory[i] = gWeeklyProfitHistory[i - 1];
    gWeeklyProfitHistory[0] = currentWeeklyProfit;

    gWeeklyProfitAverageDividend = 0;
    gWeeklyProfitAverageDivisor = 0;
    window_invalidate_by_class(WC_FINANCES);

    // Update park value history
    for (sint32 i = 127; i > 0; i--)
        gParkValueHistory[i] = gParkValueHistory[i - 1];
    gParkValueHistory[0] = gParkValue;
}

void park_set_open(sint32 open)
{
    game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, open << 8, GAME_COMMAND_SET_PARK_OPEN, 0, 0);
}

/**
 *
 *  rct2: 0x00669D4A
 */
void game_command_set_park_open(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
{
    if (!(*ebx & GAME_COMMAND_FLAG_APPLY)) {
        *ebx = 0;
        return;
    }

    sint32 dh = (*edx >> 8) & 0xFF;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_ENTRANCE_TICKETS;
    switch (dh) {
    case 0:
        if (gParkFlags & PARK_FLAGS_PARK_OPEN) {
            gParkFlags &= ~PARK_FLAGS_PARK_OPEN;
            window_invalidate_by_class(WC_PARK_INFORMATION);
        }
        break;
    case 1:
        if (!(gParkFlags & PARK_FLAGS_PARK_OPEN)) {
            gParkFlags |= PARK_FLAGS_PARK_OPEN;
            window_invalidate_by_class(WC_PARK_INFORMATION);
        }
        break;
    case 2:
        gSamePriceThroughoutParkA = *edi;
        window_invalidate_by_class(WC_RIDE);
        break;
    case 3:
        gSamePriceThroughoutParkB = *edi;
        window_invalidate_by_class(WC_RIDE);
        break;
    }

    *ebx = 0;
}

/**
*
*  rct2: 0x00664D05
*/
void update_park_fences(sint32 x, sint32 y)
{
    if (x > 0x1FFF)
        return;
    if (y > 0x1FFF)
        return;

    // When setting the ownership of map edges
    if (x <= 0 || x >= gMapSizeUnits)
        return;
    if (y <= 0 || y >= gMapSizeUnits)
        return;

    rct_map_element* sufaceElement = map_get_surface_element_at(x / 32, y / 32);
    if (sufaceElement == NULL)return;

    uint8 newOwnership = sufaceElement->properties.surface.ownership & 0xF0;
    if ((sufaceElement->properties.surface.ownership & OWNERSHIP_OWNED) == 0) {
        uint8 fence_required = 1;

        rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);
        // If an entrance element do not place flags around surface
        do {
            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE)
                continue;

            if (mapElement->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE)
                continue;

            if (!(mapElement->flags & MAP_ELEMENT_FLAG_GHOST)) {
                fence_required = 0;
                break;
            }
        } while (!map_element_is_last_for_tile(mapElement++));

        if (fence_required) {
            // As map_is_location_in_park sets the error text
            // will require to back it up.
            rct_string_id previous_error = gGameCommandErrorText;
            if (map_is_location_in_park(x - 32, y)){
                newOwnership |= 0x8;
            }

            if (map_is_location_in_park(x, y - 32)){
                newOwnership |= 0x4;
            }

            if (map_is_location_in_park(x + 32, y)){
                newOwnership |= 0x2;
            }

            if (map_is_location_in_park(x, y + 32)){
                newOwnership |= 0x1;
            }

            gGameCommandErrorText = previous_error;
        }
    }

    if (sufaceElement->properties.surface.ownership != newOwnership) {
        sint32 z0 = sufaceElement->base_height * 8;
        sint32 z1 = z0 + 16;
        map_invalidate_tile(x, y, z0, z1);
        sufaceElement->properties.surface.ownership = newOwnership;
    }
}

void update_park_fences_around_tile(sint32 x, sint32 y)
{
    update_park_fences(x, y);
    update_park_fences(x + 32, y);
    update_park_fences(x - 32, y);
    update_park_fences(x, y + 32);
    update_park_fences(x, y - 32);
}

void park_set_name(const char *name)
{
    gGameCommandErrorTitle = STR_CANT_RENAME_PARK;
    game_do_command(1, GAME_COMMAND_FLAG_APPLY, 0, *((sint32*)(name + 0)), GAME_COMMAND_SET_PARK_NAME, *((sint32*)(name + 8)), *((sint32*)(name + 4)));
    game_do_command(2, GAME_COMMAND_FLAG_APPLY, 0, *((sint32*)(name + 12)), GAME_COMMAND_SET_PARK_NAME, *((sint32*)(name + 20)), *((sint32*)(name + 16)));
    game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, *((sint32*)(name + 24)), GAME_COMMAND_SET_PARK_NAME, *((sint32*)(name + 32)), *((sint32*)(name + 28)));
}

/**
 *
 *  rct2: 0x00669C6D
 */
void game_command_set_park_name(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    rct_string_id newUserStringId;
    char oldName[128];
    static char newName[128];

    sint32 nameChunkIndex = *eax & 0xFFFF;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    //if (*ebx & GAME_COMMAND_FLAG_APPLY) { // this check seems to be useless and causes problems in multiplayer
        sint32 nameChunkOffset = nameChunkIndex - 1;
        if (nameChunkOffset < 0)
            nameChunkOffset = 2;
        nameChunkOffset *= 12;
        nameChunkOffset = min(nameChunkOffset, countof(newName) - 12);
        memcpy(newName + nameChunkOffset + 0, edx, 4);
        memcpy(newName + nameChunkOffset + 4, ebp, 4);
        memcpy(newName + nameChunkOffset + 8, edi, 4);
    //}

    if (nameChunkIndex != 0) {
        *ebx = 0;
        return;
    }

    format_string(oldName, 128, gParkName, &gParkNameArgs);
    if (strcmp(oldName, newName) == 0) {
        *ebx = 0;
        return;
    }

    if (newName[0] == 0) {
        gGameCommandErrorText = STR_INVALID_RIDE_ATTRACTION_NAME;
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    newUserStringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER, newName);
    if (newUserStringId == 0) {
        gGameCommandErrorText = STR_INVALID_NAME_FOR_PARK;
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    if (*ebx & GAME_COMMAND_FLAG_APPLY) {
        // Log park rename command if we are in multiplayer and logging is enabled
        if ((network_get_mode() == NETWORK_MODE_CLIENT || network_get_mode() == NETWORK_MODE_SERVER) && gConfigNetwork.log_server_actions) {
            // Get player name
            int player_index = network_get_player_index(game_command_playerid);
            const char* player_name = network_get_player_name(player_index);

            char log_msg[256];
            char* args[3] = {
                (char *) player_name,
                oldName,
                newName
            };
            format_string(log_msg, 256, STR_LOG_PARK_NAME, args);
            network_append_server_log(log_msg);
        }

        // Free the old ride name
        user_string_free(gParkName);
        gParkName = newUserStringId;

        gfx_invalidate_screen();
    } else {
        user_string_free(newUserStringId);
    }

    *ebx = 0;
}

static money32 map_buy_land_rights_for_tile(sint32 x, sint32 y, sint32 setting, sint32 flags) {
    rct_map_element* surfaceElement = map_get_surface_element_at(x / 32, y / 32);
    if (surfaceElement == NULL)
        return MONEY32_UNDEFINED;

    switch (setting) {
    case BUY_LAND_RIGHTS_FLAG_BUY_LAND: // 0
        if ((surfaceElement->properties.surface.ownership & OWNERSHIP_OWNED) != 0) { // If the land is already owned
            return 0;
        }

        if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) != 0 || (surfaceElement->properties.surface.ownership & OWNERSHIP_AVAILABLE) == 0) {
            gGameCommandErrorText = STR_LAND_NOT_FOR_SALE;
            return MONEY32_UNDEFINED;
        }
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership |= OWNERSHIP_OWNED;
            update_park_fences_around_tile(x, y);
        }
        return gLandPrice;
    case BUY_LAND_RIGHTS_FLAG_UNOWN_TILE: // 1
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership &= ~(OWNERSHIP_OWNED | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED);
            update_park_fences_around_tile(x, y);
        }
        return 0;
    case BUY_LAND_RIGHTS_FLAG_BUY_CONSTRUCTION_RIGHTS: // 2
        if ((surfaceElement->properties.surface.ownership & (OWNERSHIP_OWNED | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)) != 0) { // If the land or construction rights are already owned
            return 0;
        }

        if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) != 0 || (surfaceElement->properties.surface.ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE) == 0) {
            gGameCommandErrorText = STR_CONSTRUCTION_RIGHTS_NOT_FOR_SALE;
            return MONEY32_UNDEFINED;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership |= OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED;
            uint16 baseHeight = surfaceElement->base_height * 8;
            map_invalidate_tile(x, y, baseHeight, baseHeight + 16);
        }
        return gConstructionRightsPrice;
    case BUY_LAND_RIGHTS_FLAG_UNOWN_CONSTRUCTION_RIGHTS: // 3
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership &= ~OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED;
            uint16 baseHeight = surfaceElement->base_height * 8;
            map_invalidate_tile(x, y, baseHeight, baseHeight + 16);
        }
        return 0;
    case BUY_LAND_RIGHTS_FLAG_SET_FOR_SALE: // 4
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership |= OWNERSHIP_AVAILABLE;
            uint16 baseHeight = surfaceElement->base_height * 8;
            map_invalidate_tile(x, y, baseHeight, baseHeight + 16);
        }
        return 0;
    case BUY_LAND_RIGHTS_FLAG_SET_CONSTRUCTION_RIGHTS_FOR_SALE: // 5
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership |= OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE;
            uint16 baseHeight = surfaceElement->base_height * 8;
            map_invalidate_tile(x, y, baseHeight, baseHeight + 16);
        }
        return 0;
    case BUY_LAND_RIGHTS_FLAG_SET_OWNERSHIP_WITH_CHECKS:
        if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode) {
            return MONEY32_UNDEFINED;
        }

        if (x <= 0 || y <= 0) {
            gGameCommandErrorText = STR_TOO_CLOSE_TO_EDGE_OF_MAP;
            return MONEY32_UNDEFINED;
        }

        if (x >= gMapSizeUnits || y >= gMapSizeUnits) {
            gGameCommandErrorText = STR_TOO_CLOSE_TO_EDGE_OF_MAP;
            return MONEY32_UNDEFINED;
        }

        uint8 newOwnership = (flags & 0xFF00) >> 4;
        if (newOwnership == (surfaceElement->properties.surface.ownership & 0xF0)) {
            return 0;
        }

        rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);
        do {
            if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE) {
                // Do not allow ownership of park entrance.
                if (newOwnership == OWNERSHIP_OWNED || newOwnership == OWNERSHIP_AVAILABLE)
                    return 0;
                // Allow construction rights available / for sale on park entrances on surface.
                // There is no need to check the height if newOwnership is 0 (unowned and no rights available).
                if ((newOwnership == OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED ||
                     newOwnership == OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE) &&
                    (mapElement->base_height - 3 > surfaceElement->base_height ||
                     mapElement->base_height < surfaceElement->base_height))
                    return 0;
            }
        } while (!map_element_is_last_for_tile(mapElement++));

        if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
            return gLandPrice;
        }

        if ((newOwnership & 0xF0) != 0) {
            rct2_peep_spawn *peepSpawns = gPeepSpawns;

            for (uint8 i = 0; i < MAX_PEEP_SPAWNS; ++i) {
                if (x == (peepSpawns[i].x & 0xFFE0)) {
                    if (y == (peepSpawns[i].y & 0xFFE0)) {
                        peepSpawns[i].x = PEEP_SPAWN_UNDEFINED;
                    }
                }
            }
        }
        surfaceElement->properties.surface.ownership &= 0x0F;
        surfaceElement->properties.surface.ownership |= newOwnership;
        update_park_fences_around_tile(x, y);
        gMapLandRightsUpdateSuccess = true;
        return 0;
    default:
        log_warning("Tried calling map_buy_land_rights_for_tile() with an incorrect setting!");
        assert(false);
        return MONEY32_UNDEFINED;
    }
}

sint32 map_buy_land_rights(sint32 x0, sint32 y0, sint32 x1, sint32 y1, sint32 setting, sint32 flags)
{
    sint32 x, y, z;
    money32 totalCost, cost;
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

    if (x1 == 0 && y1 == 0) {
        x1 = x0;
        y1 = y0;
    }

    x = (x0 + x1) / 2 + 16;
    y = (y0 + y1) / 2 + 16;
    z = map_element_height(x, y);
    gCommandPosition.x = x;
    gCommandPosition.y = y;
    gCommandPosition.z = z;

    // Game command modified to accept selection size
    totalCost = 0;
    gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
    if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) != 0 || game_is_not_paused() || gCheatsBuildInPauseMode) {
        for (y = y0; y <= y1; y += 32) {
            for (x = x0; x <= x1; x += 32) {
                cost = map_buy_land_rights_for_tile(x, y, setting, flags);
                if (cost == MONEY32_UNDEFINED)
                    return MONEY32_UNDEFINED;

                totalCost += cost;
            }
        }
    }

    return totalCost;
}

/**
*
*  rct2: 0x006649BD
*/
void game_command_buy_land_rights(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    sint32 flags = *ebx & 0xFFFF;

    *ebx = map_buy_land_rights(
        (*eax & 0xFFFF),
        (*ecx & 0xFFFF),
        (*edi & 0xFFFF),
        (*ebp & 0xFFFF),
        (*edx & 0x00FF),
        flags
    );

    // Too expensive to always call in map_buy_land_rights.
    // It's already counted when the park is loaded, after
    // that it should only be called for user actions.
    if (flags & GAME_COMMAND_FLAG_APPLY) {
        map_count_remaining_land_rights();
    }
}


void set_forced_park_rating(sint32 rating){
    gForcedParkRating = rating;
    gParkRating = calculate_park_rating();
    gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PARK_RATING;
    window_invalidate_by_class(WC_PARK_INFORMATION);
}

sint32 get_forced_park_rating(){
    return gForcedParkRating;
}

money16 park_get_entrance_fee()
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY) return 0;
    if (!gCheatsUnlockAllPrices) {
        if (gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) return 0;
    }
    return gParkEntranceFee;
}
