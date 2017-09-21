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

#include "cheats.h"
#include "config/Config.h"
#include "Editor.h"
#include "game.h"
#include "interface/window.h"
#include "localisation/localisation.h"
#include "management/finance.h"
#include "network/network.h"
#include "ride/station.h"
#include "util/util.h"
#include "world/Climate.h"
#include "world/footpath.h"
#include "world/map.h"
#include "world/park.h"
#include "world/scenery.h"
#include "world/sprite.h"

bool gCheatsSandboxMode = false;
bool gCheatsDisableClearanceChecks = false;
bool gCheatsDisableSupportLimits = false;
bool gCheatsShowAllOperatingModes = false;
bool gCheatsShowVehiclesFromOtherTrackTypes = false;
bool gCheatsFastLiftHill = false;
bool gCheatsDisableBrakesFailure = false;
bool gCheatsDisableAllBreakdowns = false;
bool gCheatsUnlockAllPrices = false;
bool gCheatsBuildInPauseMode = false;
bool gCheatsIgnoreRideIntensity = false;
bool gCheatsDisableVandalism = false;
bool gCheatsDisableLittering = false;
bool gCheatsNeverendingMarketing = false;
bool gCheatsFreezeClimate = false;
bool gCheatsDisableTrainLengthLimit = false;
bool gCheatsDisablePlantAging = false;
bool gCheatsEnableChainLiftOnAllTrack = false;
bool gCheatsAllowArbitraryRideTypeChanges = false;
bool gCheatsDisableRideValueAging = false;
bool gCheatsIgnoreResearchStatus = false;
bool gCheatsEnableAllDrawableTrackPieces = false;

sint32 park_rating_spinner_value;

#pragma region Cheat functions

static void cheat_set_grass_length(sint32 length)
{
    sint32 x, y;
    rct_map_element *mapElement;

    for (y = 0; y < 256; y++) {
        for (x = 0; x < 256; x++) {
            mapElement = map_get_surface_element_at(x, y);
            if (!(mapElement->properties.surface.ownership & OWNERSHIP_OWNED))
                continue;

            if (map_element_get_terrain(mapElement) != TERRAIN_GRASS)
                continue;

            if (map_get_water_height(mapElement) > 0)
                continue;

            mapElement->properties.surface.grass_length = length;
        }
    }

    gfx_invalidate_screen();
}

static void cheat_water_plants()
{
    map_element_iterator it;

    map_element_iterator_begin(&it);
    do {
        if (map_element_get_type(it.element) == MAP_ELEMENT_TYPE_SCENERY) {
            it.element->properties.scenery.age = 0;
        }
    } while (map_element_iterator_next(&it));

    gfx_invalidate_screen();
}

static void cheat_fix_vandalism()
{
    map_element_iterator it;

    map_element_iterator_begin(&it);
    do {
        if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_PATH)
            continue;

        if (!footpath_element_has_path_scenery(it.element))
            continue;

        it.element->flags &= ~MAP_ELEMENT_FLAG_BROKEN;
    } while (map_element_iterator_next(&it));

    gfx_invalidate_screen();
}

static void cheat_remove_litter()
{
    rct_litter* litter;
    uint16 spriteIndex, nextSpriteIndex;

    for (spriteIndex = gSpriteListHead[SPRITE_LIST_LITTER]; spriteIndex != SPRITE_INDEX_NULL; spriteIndex = nextSpriteIndex) {
        litter = &(get_sprite(spriteIndex)->litter);
        nextSpriteIndex = litter->next;
        sprite_remove((rct_sprite*)litter);
    }

    map_element_iterator it;
    rct_scenery_entry *sceneryEntry;

    map_element_iterator_begin(&it);
    do {
        if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_PATH)
            continue;

        if (!footpath_element_has_path_scenery(it.element))
            continue;

        sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(it.element));
        if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BIN)
            it.element->properties.path.addition_status = 0xFF;

    } while (map_element_iterator_next(&it));

    gfx_invalidate_screen();
}

static void cheat_fix_rides()
{
    sint32 rideIndex;
    Ride *ride;
    rct_peep *mechanic;

    FOR_ALL_RIDES(rideIndex, ride)
    {
        if ((ride->mechanic_status != RIDE_MECHANIC_STATUS_FIXING) && (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)))
        {
            mechanic = ride_get_assigned_mechanic(ride);

            if (mechanic != NULL){
                remove_peep_from_ride(mechanic);
            }

            ride_fix_breakdown(rideIndex, 0);
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
        }
    }
}

static void cheat_renew_rides()
{
    sint32 i;
    Ride *ride;

    FOR_ALL_RIDES(i, ride)
    {
        // Set build date to current date (so the ride is brand new)
        ride->build_date = gDateMonthsElapsed;
        // Set reliability to 100
        ride->reliability = RIDE_INITIAL_RELIABILITY;
    }
    window_invalidate_by_class(WC_RIDE);
}

static void cheat_make_destructible()
{
    sint32 i;
    Ride *ride;
    FOR_ALL_RIDES(i, ride)
    {
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
            ride->lifecycle_flags&=~RIDE_LIFECYCLE_INDESTRUCTIBLE;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
            ride->lifecycle_flags&=~RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
    }
    window_invalidate_by_class(WC_RIDE);
}

static void cheat_reset_crash_status()
{
    sint32 i;
    Ride *ride;

    FOR_ALL_RIDES(i, ride){
        // Reset crash status
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CRASHED;
        // Reset crash history
        ride->last_crash_type = RIDE_CRASH_TYPE_NONE;
    }
    window_invalidate_by_class(WC_RIDE);
}

static void cheat_10_minute_inspections()
{
    sint32 i;
    Ride *ride;

    FOR_ALL_RIDES(i, ride) {
        // Set inspection interval to 10 minutes
        ride->inspection_interval = RIDE_INSPECTION_EVERY_10_MINUTES;
    }
    window_invalidate_by_class(WC_RIDE);
}

static void cheat_no_money(bool enabled)
{
    if (enabled) {
        gParkFlags |= PARK_FLAGS_NO_MONEY;
    }
    else {
        gParkFlags &= ~PARK_FLAGS_NO_MONEY;
    }
    // Invalidate all windows that have anything to do with finance
    window_invalidate_by_class(WC_RIDE);
    window_invalidate_by_class(WC_PEEP);
    window_invalidate_by_class(WC_PARK_INFORMATION);
    window_invalidate_by_class(WC_FINANCES);
    window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
    window_invalidate_by_class(WC_TOP_TOOLBAR);
    window_invalidate_by_class(WC_CHEATS);
}

static void cheat_set_money(money32 amount)
{
    gCashEncrypted = ENCRYPT_MONEY(amount);

    window_invalidate_by_class(WC_FINANCES);
    window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
}

static void cheat_add_money(money32 amount)
{
    money32 currentMoney = DECRYPT_MONEY(gCashEncrypted);
    currentMoney = add_clamp_money32(currentMoney, amount);

    gCashEncrypted = ENCRYPT_MONEY(currentMoney);

    window_invalidate_by_class(WC_FINANCES);
    window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
}

static void cheat_clear_loan()
{
    // First give money
    cheat_add_money(gBankLoan);

    // Then pay the loan
    money32 newLoan;
    newLoan = MONEY(0, 00);
    game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, newLoan, GAME_COMMAND_SET_CURRENT_LOAN, 0, 0);
}

static void cheat_generate_guests(sint32 count)
{
    for (sint32 i = 0; i < count; i++)
        park_generate_new_guest();

    window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
}

static void cheat_set_guest_parameter(sint32 parameter, sint32 value)
{
    sint32 spriteIndex;
    rct_peep *peep;

    FOR_ALL_GUESTS(spriteIndex, peep) {
        switch(parameter) {
        case GUEST_PARAMETER_HAPPINESS:
            peep->happiness = value;
            peep->happiness_target = value;
            // Clear the 'red-faced with anger' status if we're making the guest happy
            if (value > 0)
            {
                peep->peep_flags &= ~PEEP_FLAGS_ANGRY;
                peep->angriness = 0;
            }
            break;
        case GUEST_PARAMETER_ENERGY:
            peep->energy = value;
            peep->energy_target = value;
            break;
        case GUEST_PARAMETER_HUNGER:
            peep->hunger = value;
            break;
        case GUEST_PARAMETER_THIRST:
            peep->thirst = value;
            break;
        case GUEST_PARAMETER_NAUSEA:
            peep->nausea = value;
            peep->nausea_target = value;
            break;
        case GUEST_PARAMETER_NAUSEA_TOLERANCE:
            peep->nausea_tolerance = value;
            break;
        case GUEST_PARAMETER_BATHROOM:
            peep->bathroom = value;
            break;
        case GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY:
            peep->intensity = (15 << 4) | value;
            break;
        }
        peep_update_sprite_type(peep);
    }

}

static void cheat_give_all_guests(sint32 object)
{
    sint32 spriteIndex;
    rct_peep *peep;

    FOR_ALL_GUESTS(spriteIndex, peep) {
        switch(object)
        {
            case OBJECT_MONEY:
                peep->cash_in_pocket = MONEY(1000,00);
                break;
            case OBJECT_PARK_MAP:
                peep->item_standard_flags |= PEEP_ITEM_MAP;
                break;
            case OBJECT_BALLOON:
                peep->item_standard_flags |= PEEP_ITEM_BALLOON;
                peep->balloon_colour = scenario_rand_max(COLOUR_COUNT - 1);
                peep_update_sprite_type(peep);
                break;
            case OBJECT_UMBRELLA:
                peep->item_standard_flags |= PEEP_ITEM_UMBRELLA;
                peep->umbrella_colour = scenario_rand_max(COLOUR_COUNT - 1);
                peep_update_sprite_type(peep);
                break;
        }
    }
    window_invalidate_by_class(WC_PEEP);
}

static void cheat_remove_all_guests()
{
    rct_peep *peep;
    rct_vehicle *vehicle;
    uint16 spriteIndex, nextSpriteIndex;

    for (spriteIndex = gSpriteListHead[SPRITE_LIST_PEEP]; spriteIndex != SPRITE_INDEX_NULL; spriteIndex = nextSpriteIndex) {
        peep = &(get_sprite(spriteIndex)->peep);
        nextSpriteIndex = peep->next;
        if (peep->type == PEEP_TYPE_GUEST) {
            peep_remove(peep);
        }
    }

    sint32 rideIndex;
    Ride *ride;

    FOR_ALL_RIDES(rideIndex, ride)
    {
        ride->num_riders = 0;

        for (size_t stationIndex = 0; stationIndex < MAX_STATIONS; stationIndex++)
        {
            ride->queue_length[stationIndex] = 0;
            ride->last_peep_in_queue[stationIndex] = SPRITE_INDEX_NULL;
        }

        for (size_t trainIndex = 0; trainIndex < 32; trainIndex++)
        {
            spriteIndex = ride->vehicles[trainIndex];
            while (spriteIndex != SPRITE_INDEX_NULL)
            {
                vehicle = GET_VEHICLE(spriteIndex);

                vehicle->num_peeps = 0;
                vehicle->next_free_seat = 0;

                for (size_t peepInTrainIndex = 0; peepInTrainIndex < 32; peepInTrainIndex++)
                {
                    vehicle->peep[peepInTrainIndex] = SPRITE_INDEX_NULL;
                }

                spriteIndex = vehicle->next_vehicle_on_train;
            }
        }
    }

    window_invalidate_by_class(WC_RIDE);
    gfx_invalidate_screen();
}

static void cheat_explode_guests()
{
    sint32 sprite_index;
    rct_peep *peep;

    FOR_ALL_GUESTS(sprite_index, peep) {
        // To prevent blowing up peeps that will break
        // ride vehicle logic.
        if (peep->state == PEEP_STATE_ENTERING_RIDE ||
            peep->state == PEEP_STATE_QUEUING_FRONT ||
            peep->state == PEEP_STATE_LEAVING_RIDE ||
            peep->state == PEEP_STATE_ON_RIDE ||
            peep->state == PEEP_STATE_QUEUING) {
            continue;
        }

        if (scenario_rand_max(6) == 0) {
            peep->peep_flags |= PEEP_FLAGS_EXPLODE;
        }
    }
}

static void cheat_set_staff_speed(uint8 value)
{
    uint16 spriteIndex;
    rct_peep *peep;

    FOR_ALL_STAFF(spriteIndex, peep) {
        peep->energy = value;
        peep->energy_target = value;
    }
}

static void cheat_own_all_land()
{
    sint32 min = 32;
    sint32 max = gMapSizeUnits - 32;
    for (sint32 y = min; y <= max; y += 32) {
        for (sint32 x = min; x <= max; x += 32) {
            rct_map_element * surfaceElement = map_get_surface_element_at(x >> 5, y >> 5);

            // Ignore already owned tiles.
            if (surfaceElement->properties.surface.ownership & OWNERSHIP_OWNED)
                continue;

            sint32 base_z = surfaceElement->base_height;
            sint32 destOwnership = check_max_allowable_land_rights_for_tile(x >> 5, y >> 5, base_z);

            // only own tiles that were not set to 0
            if (destOwnership != OWNERSHIP_UNOWNED) {
                surfaceElement->properties.surface.ownership |= destOwnership;
                update_park_fences_around_tile(x, y);
                uint16 baseHeight = surfaceElement->base_height * 8;
                map_invalidate_tile(x, y, baseHeight, baseHeight + 16);
            }
        }
    }

    // Completely unown peep spawn points
    for (sint32 i = 0; i < MAX_PEEP_SPAWNS; i++) {
        sint32 x = gPeepSpawns[i].x;
        sint32 y = gPeepSpawns[i].y;
        if (x != PEEP_SPAWN_UNDEFINED) {
            rct_map_element * surfaceElement = map_get_surface_element_at(x >> 5, y >> 5);
            surfaceElement->properties.surface.ownership = OWNERSHIP_UNOWNED;
            update_park_fences_around_tile(x, y);
            uint16 baseHeight = surfaceElement->base_height * 8;
            map_invalidate_tile(x, y, baseHeight, baseHeight + 16);
        }
    }

    map_count_remaining_land_rights();
}

#pragma endregion

void game_command_cheat(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
{
    sint32 cheat = *ecx;
    if (*ebx & GAME_COMMAND_FLAG_APPLY) {
        switch (cheat) {
            case CHEAT_SANDBOXMODE: gCheatsSandboxMode = *edx != 0; window_invalidate_by_class(WC_MAP); window_invalidate_by_class(WC_FOOTPATH); break;
            case CHEAT_DISABLECLEARANCECHECKS: gCheatsDisableClearanceChecks = *edx != 0; break;
            case CHEAT_DISABLESUPPORTLIMITS: gCheatsDisableSupportLimits = *edx != 0; break;
            case CHEAT_SHOWALLOPERATINGMODES: gCheatsShowAllOperatingModes = *edx != 0; break;
            case CHEAT_SHOWVEHICLESFROMOTHERTRACKTYPES: gCheatsShowVehiclesFromOtherTrackTypes = *edx != 0; break;
            case CHEAT_FASTLIFTHILL: gCheatsFastLiftHill = *edx != 0; break;
            case CHEAT_DISABLEBRAKESFAILURE: gCheatsDisableBrakesFailure = *edx != 0; break;
            case CHEAT_DISABLEALLBREAKDOWNS: gCheatsDisableAllBreakdowns = *edx != 0; break;
            case CHEAT_DISABLETRAINLENGTHLIMIT: gCheatsDisableTrainLengthLimit = *edx != 0; break;
            case CHEAT_ENABLECHAINLIFTONALLTRACK: gCheatsEnableChainLiftOnAllTrack = *edx != 0; break;
            case CHEAT_UNLOCKALLPRICES: gCheatsUnlockAllPrices = *edx != 0; window_invalidate_by_class(WC_RIDE); window_invalidate_by_class(WC_PARK_INFORMATION); break;
            case CHEAT_BUILDINPAUSEMODE: gCheatsBuildInPauseMode = *edx != 0; break;
            case CHEAT_IGNORERIDEINTENSITY: gCheatsIgnoreRideIntensity = *edx != 0; break;
            case CHEAT_DISABLEVANDALISM: gCheatsDisableVandalism = *edx != 0; break;
            case CHEAT_DISABLELITTERING: gCheatsDisableLittering = *edx != 0; break;
            case CHEAT_NOMONEY: cheat_no_money(*edx != 0); break;
            case CHEAT_ADDMONEY: cheat_add_money(*edx); break;
            case CHEAT_SETMONEY: cheat_set_money(*edx); break;
            case CHEAT_CLEARLOAN: cheat_clear_loan(); break;
            case CHEAT_SETGUESTPARAMETER: cheat_set_guest_parameter(*edx, *edi); break;
            case CHEAT_GENERATEGUESTS: cheat_generate_guests(*edx); break;
            case CHEAT_REMOVEALLGUESTS: cheat_remove_all_guests(); break;
            case CHEAT_EXPLODEGUESTS: cheat_explode_guests(); break;
            case CHEAT_GIVEALLGUESTS: cheat_give_all_guests(*edx); break;
            case CHEAT_SETGRASSLENGTH: cheat_set_grass_length(*edx); break;
            case CHEAT_WATERPLANTS: cheat_water_plants(); break;
            case CHEAT_FIXVANDALISM: cheat_fix_vandalism(); break;
            case CHEAT_REMOVELITTER: cheat_remove_litter(); break;
            case CHEAT_DISABLEPLANTAGING: gCheatsDisablePlantAging = *edx != 0; break;
            case CHEAT_SETSTAFFSPEED: cheat_set_staff_speed(*edx); break;
            case CHEAT_RENEWRIDES: cheat_renew_rides(); break;
            case CHEAT_MAKEDESTRUCTIBLE: cheat_make_destructible(); break;
            case CHEAT_FIXRIDES: cheat_fix_rides(); break;
            case CHEAT_RESETCRASHSTATUS: cheat_reset_crash_status(); break;
            case CHEAT_10MINUTEINSPECTIONS: cheat_10_minute_inspections(); break;
            case CHEAT_WINSCENARIO: scenario_success(); break;
            case CHEAT_FORCEWEATHER: climate_force_weather(*edx); break;
            case CHEAT_FREEZECLIMATE: gCheatsFreezeClimate = *edx != 0; break;
            case CHEAT_NEVERENDINGMARKETING: gCheatsNeverendingMarketing = *edx != 0; break;
            case CHEAT_OPENCLOSEPARK: park_set_open(park_is_open() ? 0 : 1); break;
            case CHEAT_HAVEFUN: gScenarioObjectiveType = OBJECTIVE_HAVE_FUN; break;
            case CHEAT_SETFORCEDPARKRATING: if(*edx > -1) { park_rating_spinner_value = *edx; } set_forced_park_rating(*edx); break;
            case CHEAT_RESETDATE: date_reset(); window_invalidate_by_class(WC_BOTTOM_TOOLBAR); break;
            case CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES: gCheatsAllowArbitraryRideTypeChanges = *edx != 0; window_invalidate_by_class(WC_RIDE); break;
            case CHEAT_OWNALLLAND: cheat_own_all_land(); break;
            case CHEAT_DISABLERIDEVALUEAGING: gCheatsDisableRideValueAging = *edx != 0; break;
            case CHEAT_IGNORERESEARCHSTATUS: gCheatsIgnoreResearchStatus = *edx != 0; break;
            case CHEAT_ENABLEALLDRAWABLETRACKPIECES: gCheatsEnableAllDrawableTrackPieces = *edx != 0; break;
        }
        if (network_get_mode() == NETWORK_MODE_NONE) {
            config_save_default();
        }
        window_invalidate_by_class(WC_CHEATS);
    }
    *ebx = 0;
}

void cheats_reset()
{
    gCheatsSandboxMode = false;
    gCheatsDisableClearanceChecks = false;
    gCheatsDisableSupportLimits = false;
    gCheatsShowAllOperatingModes = false;
    gCheatsShowVehiclesFromOtherTrackTypes = false;
    gCheatsDisableTrainLengthLimit = false;
    gCheatsEnableChainLiftOnAllTrack = false;
    gCheatsFastLiftHill = false;
    gCheatsDisableBrakesFailure = false;
    gCheatsDisableAllBreakdowns = false;
    gCheatsUnlockAllPrices = false;
    gCheatsBuildInPauseMode = false;
    gCheatsIgnoreRideIntensity = false;
    gCheatsDisableVandalism = false;
    gCheatsDisableLittering = false;
    gCheatsNeverendingMarketing = false;
    gCheatsFreezeClimate = false;
    gCheatsDisablePlantAging = false;
    gCheatsAllowArbitraryRideTypeChanges = false;
    gCheatsDisableRideValueAging = false;
    gCheatsIgnoreResearchStatus = false;
}

//Generates the string to print for the server log when a cheat is used.
const char* cheats_get_cheat_string(int cheat, int edx, int edi) {
    switch (cheat) {
        case CHEAT_SANDBOXMODE:
            if (gCheatsSandboxMode) {
                return language_get_string(STR_CHEAT_SANDBOX_MODE_DISABLE);
            } else {
                return language_get_string(STR_CHEAT_SANDBOX_MODE);
            }
        case CHEAT_DISABLECLEARANCECHECKS: return language_get_string(STR_DISABLE_CLEARANCE_CHECKS);
        case CHEAT_DISABLESUPPORTLIMITS: return language_get_string(STR_DISABLE_SUPPORT_LIMITS);
        case CHEAT_SHOWALLOPERATINGMODES: return language_get_string(STR_CHEAT_SHOW_ALL_OPERATING_MODES);
        case CHEAT_SHOWVEHICLESFROMOTHERTRACKTYPES: return language_get_string(STR_CHEAT_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES);
        case CHEAT_FASTLIFTHILL: return language_get_string(STR_CHEAT_UNLOCK_OPERATING_LIMITS);
        case CHEAT_DISABLEBRAKESFAILURE: return language_get_string(STR_CHEAT_DISABLE_BRAKES_FAILURE);
        case CHEAT_DISABLEALLBREAKDOWNS: return language_get_string(STR_CHEAT_DISABLE_BREAKDOWNS);
        case CHEAT_DISABLETRAINLENGTHLIMIT: return language_get_string(STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT);
        case CHEAT_ENABLECHAINLIFTONALLTRACK: return language_get_string(STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK);
        case CHEAT_UNLOCKALLPRICES: return language_get_string(STR_CHEAT_UNLOCK_PRICES);
        case CHEAT_BUILDINPAUSEMODE: return language_get_string(STR_CHEAT_BUILD_IN_PAUSE_MODE);
        case CHEAT_IGNORERIDEINTENSITY: return language_get_string(STR_CHEAT_IGNORE_INTENSITY);
        case CHEAT_DISABLEVANDALISM: return language_get_string(STR_CHEAT_DISABLE_VANDALISM);
        case CHEAT_DISABLELITTERING: return language_get_string(STR_CHEAT_DISABLE_LITTERING);
        case CHEAT_ADDMONEY: return language_get_string(STR_LOG_CHEAT_ADD_MONEY);
        case CHEAT_CLEARLOAN: return language_get_string(STR_CHEAT_CLEAR_LOAN);
        case CHEAT_SETGUESTPARAMETER:
        {
            static char cheat_string[128];
            safe_strcpy(cheat_string, language_get_string(STR_CHEAT_SET_GUESTS_PARAMETERS), 128);
            safe_strcat(cheat_string, " ", 128);
            switch (edx) {
                case GUEST_PARAMETER_HAPPINESS:
                    safe_strcat(cheat_string, language_get_string(STR_CHEAT_GUEST_HAPPINESS), 128);
                    safe_strcat(cheat_string, " ", 128);
                    if (edi == 255) {
                        safe_strcat(cheat_string, language_get_string(STR_MAX), 128);
                    } else if (edi == 0) {
                        safe_strcat(cheat_string, language_get_string(STR_MIN), 128);
                    }
                    break;
                case GUEST_PARAMETER_ENERGY:
                    safe_strcat(cheat_string, language_get_string(STR_CHEAT_GUEST_ENERGY), 128);
                    safe_strcat(cheat_string, " ", 128);
                    if (edi == 127) {
                        safe_strcat(cheat_string, language_get_string(STR_MAX), 128);
                    } else if (edi == 0) {
                        safe_strcat(cheat_string, language_get_string(STR_MIN), 128);
                    }
                    break;
                case GUEST_PARAMETER_HUNGER:
                    safe_strcat(cheat_string, language_get_string(STR_CHEAT_GUEST_HUNGER), 128);
                    safe_strcat(cheat_string, " ", 128);
                    if (edi == 255) {
                        safe_strcat(cheat_string, language_get_string(STR_MIN), 128);
                    } else if (edi == 0) {
                        safe_strcat(cheat_string, language_get_string(STR_MAX), 128);
                    }
                    break;
                case GUEST_PARAMETER_THIRST:
                    safe_strcat(cheat_string, language_get_string(STR_CHEAT_GUEST_THIRST), 128);
                    safe_strcat(cheat_string, " ", 128);
                    if (edi == 255) {
                        safe_strcat(cheat_string, language_get_string(STR_MIN), 128);
                    } else if (edi == 0) {
                        safe_strcat(cheat_string, language_get_string(STR_MAX), 128);
                    }
                    break;
                case GUEST_PARAMETER_NAUSEA:
                    safe_strcat(cheat_string, language_get_string(STR_CHEAT_GUEST_NAUSEA), 128);
                    safe_strcat(cheat_string, " ", 128);
                    if (edi == 255) {
                        safe_strcat(cheat_string, language_get_string(STR_MAX), 128);
                    } else if (edi == 0) {
                        safe_strcat(cheat_string, language_get_string(STR_MIN), 128);
                    }
                    break;
                case GUEST_PARAMETER_NAUSEA_TOLERANCE:
                    safe_strcat(cheat_string, language_get_string(STR_CHEAT_GUEST_NAUSEA_TOLERANCE), 128);
                    safe_strcat(cheat_string, " ", 128);
                    if (edi == PEEP_NAUSEA_TOLERANCE_HIGH) {
                        safe_strcat(cheat_string, language_get_string(STR_MAX), 128);
                    } else if (edi == PEEP_NAUSEA_TOLERANCE_NONE) {
                        safe_strcat(cheat_string, language_get_string(STR_MIN), 128);
                    }
                    break;
                case GUEST_PARAMETER_BATHROOM:
                    safe_strcat(cheat_string, language_get_string(STR_CHEAT_GUEST_BATHROOM), 128);
                    safe_strcat(cheat_string, " ", 128);
                    if (edi == 255) {
                        safe_strcat(cheat_string, language_get_string(STR_MAX), 128);
                    } else if (edi == 0) {
                        safe_strcat(cheat_string, language_get_string(STR_MIN), 128);
                    }
                    break;
                case GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY:
                    safe_strcat(cheat_string, language_get_string(STR_CHEAT_GUEST_PREFERRED_INTENSITY), 128);
                    safe_strcat(cheat_string, " ", 128);
                    if (edi == 1) {
                        safe_strcat(cheat_string, language_get_string(STR_CHEAT_MORE_THAN_1), 128);
                    } else if (edi == 0) {
                        safe_strcat(cheat_string, language_get_string(STR_CHEAT_LESS_THAN_15), 128);
                    }
                    break;
            }
            return cheat_string;
        }
        case CHEAT_GENERATEGUESTS: return language_get_string(STR_CHEAT_LARGE_TRAM_GUESTS);
        case CHEAT_REMOVEALLGUESTS: return language_get_string(STR_CHEAT_REMOVE_ALL_GUESTS);
        case CHEAT_EXPLODEGUESTS: return language_get_string(STR_CHEAT_EXPLODE);
        case CHEAT_GIVEALLGUESTS:
        {
            static char cheat_string[64];
            safe_strcpy(cheat_string, language_get_string(STR_CHEAT_GIVE_ALL_GUESTS), 64);
            safe_strcat(cheat_string, " ", 64);
            switch (edx) {
                case OBJECT_MONEY:
                {
                    char money[16];
                    set_format_arg(0, money32, CHEATS_GIVE_GUESTS_MONEY);
                    format_string(money, 16, STR_CHEAT_CURRENCY_FORMAT, gCommonFormatArgs);
                    safe_strcat(cheat_string, money, 64);
                    break;
                }
                case OBJECT_PARK_MAP: safe_strcat(cheat_string, language_get_string(STR_SHOP_ITEM_PLURAL_PARK_MAP), 64); break;
                case OBJECT_BALLOON: safe_strcat(cheat_string, language_get_string(STR_SHOP_ITEM_PLURAL_BALLOON), 64); break;
                case OBJECT_UMBRELLA: safe_strcat(cheat_string, language_get_string(STR_SHOP_ITEM_PLURAL_UMBRELLA), 64); break;
            }
            return cheat_string;
        }
        case CHEAT_SETGRASSLENGTH:
            if (edx == 0) {
                return language_get_string(STR_CHEAT_MOWED_GRASS);
            } else if (edx == 1) {
                return language_get_string(STR_CHEAT_CLEAR_GRASS);
            }
        case CHEAT_WATERPLANTS: return language_get_string(STR_CHEAT_WATER_PLANTS);
        case CHEAT_FIXVANDALISM: return language_get_string(STR_CHEAT_FIX_VANDALISM);
        case CHEAT_REMOVELITTER: return language_get_string(STR_CHEAT_REMOVE_LITTER);
        case CHEAT_DISABLEPLANTAGING: return language_get_string(STR_CHEAT_DISABLE_PLANT_AGING);
        case CHEAT_SETSTAFFSPEED:
        {
            static char cheat_string[64];
            safe_strcpy(cheat_string, language_get_string(STR_CHEAT_STAFF_SPEED), 64);
            safe_strcat(cheat_string, " ", 64);
            if (edx == CHEATS_STAFF_FAST_SPEED) {
                safe_strcat(cheat_string, language_get_string(STR_FAST), 64);
            } else if (edx == CHEATS_STAFF_NORMAL_SPEED) {
                safe_strcat(cheat_string, language_get_string(STR_NORMAL), 64);
            }
            return cheat_string;
        }
        case CHEAT_RENEWRIDES: return language_get_string(STR_CHEAT_RENEW_RIDES);
        case CHEAT_MAKEDESTRUCTIBLE: return language_get_string(STR_CHEAT_MAKE_DESTRUCTABLE);
        case CHEAT_FIXRIDES: return language_get_string(STR_CHEAT_FIX_ALL_RIDES);
        case CHEAT_RESETCRASHSTATUS: return language_get_string(STR_CHEAT_RESET_CRASH_STATUS);
        case CHEAT_10MINUTEINSPECTIONS: return language_get_string(STR_CHEAT_10_MINUTE_INSPECTIONS);
        case CHEAT_WINSCENARIO: return language_get_string(STR_CHEAT_WIN_SCENARIO);
        case CHEAT_FORCEWEATHER:
        {
            static char cheat_string[64];
            safe_strcpy(cheat_string, language_get_string(STR_FORCE_WEATHER), 64);
            safe_strcat(cheat_string, " ", 64);
            switch (edx) {
                case 0: safe_strcat(cheat_string, language_get_string(STR_SUNNY), 64); break;
                case 1: safe_strcat(cheat_string, language_get_string(STR_PARTIALLY_CLOUDY), 64); break;
                case 2: safe_strcat(cheat_string, language_get_string(STR_CLOUDY), 64); break;
                case 3: safe_strcat(cheat_string, language_get_string(STR_RAIN), 64); break;
                case 4: safe_strcat(cheat_string, language_get_string(STR_HEAVY_RAIN), 64); break;
                case 5: safe_strcat(cheat_string, language_get_string(STR_THUNDERSTORM), 64); break;
            }
            return cheat_string;
        }
        case CHEAT_FREEZECLIMATE:
            if (gCheatsFreezeClimate) {
                return language_get_string(STR_CHEAT_UNFREEZE_CLIMATE);
            } else {
                return language_get_string(STR_CHEAT_FREEZE_CLIMATE);
            }
        case CHEAT_NEVERENDINGMARKETING: return language_get_string(STR_CHEAT_NEVERENDING_MARKETING);
        case CHEAT_OPENCLOSEPARK:
            if (park_is_open()) {
                return language_get_string(STR_CHEAT_CLOSE_PARK);
            } else {
                return language_get_string(STR_CHEAT_OPEN_PARK);
            }
        case CHEAT_HAVEFUN: return language_get_string(STR_CHEAT_HAVE_FUN);
        case CHEAT_SETFORCEDPARKRATING:
        {
            static char cheat_string[64];
            safe_strcpy(cheat_string, language_get_string(STR_FORCE_PARK_RATING), 64);
            safe_strcat(cheat_string, " ", 64);

            char buffer[8];
            snprintf(buffer, sizeof(buffer), "%d", park_rating_spinner_value);
            char* park_rating = buffer;
            safe_strcat(cheat_string, park_rating, 64);

            return cheat_string;
        }
        case CHEAT_RESETDATE: return language_get_string(STR_CHEAT_RESET_DATE);
        case CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES: return language_get_string(STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES);
        case CHEAT_SETMONEY: return language_get_string(STR_SET_MONEY);
        case CHEAT_OWNALLLAND: return language_get_string(STR_CHEAT_OWN_ALL_LAND);
        case CHEAT_DISABLERIDEVALUEAGING: return language_get_string(STR_CHEAT_DISABLE_RIDE_VALUE_AGING);
        case CHEAT_IGNORERESEARCHSTATUS: return language_get_string(STR_CHEAT_IGNORE_RESEARCH_STATUS);
        case CHEAT_ENABLEALLDRAWABLETRACKPIECES: return language_get_string(STR_CHEAT_ENABLE_ALL_DRAWABLE_TRACK_PIECES);
    }

    return "";
}
