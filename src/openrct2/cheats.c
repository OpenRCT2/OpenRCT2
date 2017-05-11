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

#include "cheats.h"
#include "config/Config.h"
#include "game.h"
#include "interface/window.h"
#include "localisation/date.h"
#include "management/finance.h"
#include "network/network.h"
#include "util/util.h"
#include "world/Climate.h"
#include "world/footpath.h"
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

			if ((mapElement->properties.surface.terrain & 0x1F) > 0)
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
	rct_ride *ride;
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
	rct_ride *ride;

	FOR_ALL_RIDES(i, ride)
	{
		// Set build date to current date (so the ride is brand new)
		ride->build_date = gDateMonthsElapsed;
		// Set reliability to 100
		ride->reliability = (100 << 8);
	}
	window_invalidate_by_class(WC_RIDE);
}

static void cheat_make_destructible()
{
	sint32 i;
	rct_ride *ride;
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
	rct_ride *ride;

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
	rct_ride *ride;

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
				// Clear the 'red-faced with anger' status if we're making the guest happy
				if (value > 0)
				{
					peep->peep_flags &= ~PEEP_FLAGS_ANGRY;
					peep->angriness = 0;
				}
				break;
			case GUEST_PARAMETER_ENERGY:
				peep->energy = value;
				break;
			case GUEST_PARAMETER_HUNGER:
				peep->hunger = value;
				break;
			case GUEST_PARAMETER_THIRST:
				peep->thirst = value;
				break;
			case GUEST_PARAMETER_NAUSEA:
				peep->nausea = value;
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
	rct_ride *ride;

	FOR_ALL_RIDES(rideIndex, ride)
	{
		ride->num_riders = 0;

		for (size_t stationIndex = 0; stationIndex < RCT12_MAX_STATIONS_PER_RIDE; stationIndex++)
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
		peep->energy_growth_rate = value;
	}
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
}
