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

#pragma clang diagnostic ignored "-Wunused-variable"

extern "C"
{
#include "./peepwatch.h"
}

#ifndef STOUT_PEEPWATCH_EXPERIMENT

void peepwatch_update() { }

#else

#include "../core/Math.hpp"
#include "../core/String.hpp"
#include "../config/Config.h"
#include <string> 
#include <sstream> 
#include <locale> 
#include <codecvt>

extern "C"
{
#include "../drawing/drawing.h"
#include "../game.h"
#include "../interface/console.h"
#include "../interface/window.h"
#include "../interface/viewport.h"
#include "../localisation/localisation.h"
#include "../management/news_item.h"
#include "../peep/peep.h"
#include "../world/sprite.h"
#include "../util/util.h"
#include "../ride/ride_data.h"
#include "../management/marketing.h"
#include "../cheats.h"
#include "../network/twitch.h"
#include "../platform/platform.h"
}

#include "../core/FileStream.hpp"

bool gPeepwatchEnable = false;

static rct_string_id window_guest_inventory_format_item(rct_peep *peep, int item)
{
	rct_ride *ride;

	// Default arguments
	set_format_arg(0, uint16, ShopItemStringIds[item].display);
	set_format_arg(2, uint16, gParkName);
	set_format_arg(4, uint32, gParkNameArgs);

	// Special overrides
	switch (item) {
	case SHOP_ITEM_BALLOON:
		break;
	case SHOP_ITEM_PHOTO:
		ride = get_ride(peep->photo1_ride_ref);
		set_format_arg(2, uint16, ride->name);
		set_format_arg(4, uint32, ride->name_arguments);
		break;
	case SHOP_ITEM_UMBRELLA:
		break;
	case SHOP_ITEM_VOUCHER:
		switch (peep->voucher_type) {
		case VOUCHER_TYPE_PARK_ENTRY_FREE:
			set_format_arg(2, uint16, 2418);
			set_format_arg(4, uint16, gParkName);
			set_format_arg(6, uint32, gParkNameArgs);
			break;
		case VOUCHER_TYPE_RIDE_FREE:
			ride = get_ride(peep->voucher_arguments);
			set_format_arg(2, uint16, 2419);
			set_format_arg(4, uint16, ride->name);
			set_format_arg(6, uint32, ride->name_arguments);
			break;
		case VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE:
			set_format_arg(2, uint16, 2420);
			set_format_arg(4, uint16, gParkName);
			set_format_arg(6, uint32, gParkNameArgs);
			break;
		case VOUCHER_TYPE_FOOD_OR_DRINK_FREE:
			set_format_arg(2, uint16, 2421);
			set_format_arg(4, uint16, ShopItemStringIds[peep->voucher_arguments].singular);
			break;
		}
		break;
	case SHOP_ITEM_HAT:
		break;
	case SHOP_ITEM_TSHIRT:
		break;
	case SHOP_ITEM_PHOTO2:
		ride = get_ride(peep->photo2_ride_ref);
		set_format_arg(2, uint16, ride->name);
		set_format_arg(4, uint32, ride->name_arguments);
		break;
	case SHOP_ITEM_PHOTO3:
		ride = get_ride(peep->photo3_ride_ref);
		set_format_arg(2, uint16, ride->name);
		set_format_arg(4, uint32, ride->name_arguments);
		break;
	case SHOP_ITEM_PHOTO4:
		ride = get_ride(peep->photo4_ride_ref);
		set_format_arg(2, uint16, ride->name);
		set_format_arg(4, uint32, ride->name_arguments);
		break;
	}

	return STR_GUEST_ITEM_FORMAT;
}

namespace Peepwatch
{

	struct WatchedPeep {
		uint16		SpriteId;
		uint8		DefaultTShirtColour;
		uint8		DefaultTrouserColour;
		uint8		DefaultUmbrellaColour;

		uint8		LastState;
		uint8		LastRide;
		uint8		LastHeadingTo;
		uint32		LastItemStandardFlags;
		uint32		LastItemExtraFlags;

		uint32		GreetTimeout;

		uint32		PeopleAheadInCue;
		float		DistanceToTarget;

		float		FollowWeight;

		float		LastPosition[3];

		float		ExcitementContinuous;
		float		ExcitementEffective;
		float		ExcitementSort;
		float		ExcitementAudienceParticipation;
		float		ExcitementBonusFast;
		float		ExcitementBonusSlow;
		float		ExcitementBonusRide;
		float		ExcitementGoodwill;

		void Reset() {
			Memory::Set(this, 0, sizeof(WatchedPeep));
		}
	};

	utf8						_ioCurrentName[MAX_PATH];
	utf8						_ioCurrentThought[MAX_PATH];
	utf8						_ioAllPeeps[MAX_PATH];
	utf8						_ioCurrentState[MAX_PATH];

	static uint32				_updateLastTick = 0;
	static uint32				_updateStepTickLeft = 0;
	static uint32				_updateTickNumber = 0;
	static uint16				_effectiveFollowSprite = 300;

	static int					_totalTrackCount = 12;

	static float				_watchingPositionPrevious[3];
	static float				_watchingPositionPreviousTarget[3];
	static float				_watchingPositionDelta[3];

	static std::vector<WatchedPeep>	_watchedPeeps;

	static const uint8			RidePenaltyCount = 128;

	static uint8				_rideBasePenalties[RidePenaltyCount];
	static uint8				_rideBasePenaltyIndex = 0;
	static uint8				_rideTypePenalties[RidePenaltyCount];
	static uint8				_rideTypePenaltyIndex = 0;

	static bool					_maintainPark;

	static bool IsPeepwatchEnabled()
	{
		if (!gPeepwatchEnable) return false;
		return true;
	}

	/**
	* Copied over from the twitch implement
	*/
	static char * strchrm(const char * str, const char * find)
	{
		do
		{
			const char * fch = find;
			while (*fch != '\0')
			{
				if (*str == *fch)
				{
					return (char *)str;
				}
				fch++;
			}
		} while (*str++ != '\0');
		return nullptr;
	}

	static char * strskipwhitespace(const char * str)
	{
		while (*str == ' ' || *str == '\t')
		{
			str++;
		}
		return (char *)str;
	}

	static void AddNewWatchedPeep();
	static void UpdateStepTrack();
	static void UpdateStepExcitement();
	static void UpdateStepSort();
	static void UpdateStepMaintain();
	static void UpdatePeepFlash();
	static void UpdateFileCurrentPeep();
	static void UpdateFileCurrentThought();
	static void UpdateFileCurrentState();
	static void UpdateFileAllPeeps();
	static void AddRidePenalty(uint8);
	static uint32 CountRidePenalty(uint8);
	static void RichPeepState(char*, rct_peep*);

	static void UpdateStep() {
		std::vector<WatchedPeep>::iterator it = _watchedPeeps.begin();
		while (it != _watchedPeeps.end()) {
			rct_peep *peep = GET_PEEP(it->SpriteId);

			if (peep->type != PEEP_TYPE_GUEST) {
				it = _watchedPeeps.erase(it);
				continue;
			}
			if (peep->x == SPRITE_LOCATION_NULL) {
				if (!(peep->state == PEEP_STATE_ON_RIDE
					|| peep->state == PEEP_STATE_ENTERING_RIDE
					|| peep->state == PEEP_STATE_LEAVING_RIDE)) {
					it = _watchedPeeps.erase(it);
					continue;
				}
			}
			++it;
		}

		if ((_updateTickNumber % 500) == 0) {
			if (_watchedPeeps.size() >= (uint32)_totalTrackCount) {
				_watchedPeeps.pop_back();
			}
		}
		if ((_updateTickNumber % 25) == 0 && _watchedPeeps.size() < (uint32)_totalTrackCount) {
			AddNewWatchedPeep();
		}

		for (WatchedPeep &m : _watchedPeeps) {
			if (m.GreetTimeout > 0)
				m.GreetTimeout -= 1;
			m.FollowWeight = 0.0f;
		}

		if (_watchedPeeps.size() > 0) {
			_watchedPeeps[0].FollowWeight = 1.0f;
		}

		UpdateStepTrack();
		UpdateStepExcitement();

		if ((_updateTickNumber % 5) == 0) {
			UpdateStepSort();
		}

		if ((_updateTickNumber % 5) == 0) {
			UpdateFileCurrentPeep();
			UpdateFileCurrentThought();
			UpdateFileCurrentState();
			UpdateFileAllPeeps();
		}

		if ((_updateTickNumber % 100) == 0) {
			if (_maintainPark) {
				UpdateStepMaintain();
			}
		}
	}

	static void UpdateStepMaintain() {
			// Since nobody is attending the game, water the planets and fix vandalism...

		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_WATERPLANTS, 0, GAME_COMMAND_CHEAT, 0, 0);
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_FIXVANDALISM, 0, GAME_COMMAND_CHEAT, 0, 0);

			// Check for all rides whether they have crashed/stopped going and restart them

		uint16 sprite_index;

		sprite_index = gSpriteListHead[SPRITE_LIST_TRAIN];
		while (sprite_index != SPRITE_INDEX_NULL) {
			rct_vehicle *vehicle	= &(get_sprite(sprite_index)->vehicle);
			rct_ride	*ride		= get_ride(vehicle->ride);

			if (vehicle->status == VEHICLE_STATUS_CRASHED) {
				if (ride->status != RIDE_STATUS_TESTING) {
						// Reboot immediately, but we do not clear the crash status;
						//	if it keeps crashing, people will stop riding. Right?
					ride_set_status(vehicle->ride, RIDE_STATUS_CLOSED);
					ride_set_status(vehicle->ride, RIDE_STATUS_CLOSED);
					ride_set_status(vehicle->ride, RIDE_STATUS_OPEN);
				}
			}

			sprite_index = vehicle->next;
		}

		window_close_by_class(WC_RIDE);

		rct_peep *peep;
		FOR_ALL_GUESTS(sprite_index, peep) {
			if (peep->state != PEEP_STATE_ON_RIDE)
				continue;

			uint8 ride = peep->current_ride;

			if (ride == 0xFF)
				continue;

			if (peep->time_on_ride > 25) {
					// A little while after they start complaining, reboot the ride
				ride_set_status(ride, RIDE_STATUS_CLOSED);
				ride_set_status(ride, RIDE_STATUS_CLOSED);
				ride_set_status(ride, RIDE_STATUS_OPEN);
			}
		}
	}

	static void UpdateStepExcitement() {
		bool first = true;

		float sumAudienceBoost = 0.0f;

		for (WatchedPeep &m : _watchedPeeps) {
			rct_peep *peep = GET_PEEP(m.SpriteId);

			m.ExcitementContinuous = m.ExcitementBonusSlow + m.ExcitementBonusFast + m.ExcitementBonusRide;
			m.ExcitementBonusSlow	*= 0.970f;
			m.ExcitementBonusFast	*= 0.995f;
			m.ExcitementBonusRide	*= 0.990f;

			if (peep->peep_flags & PEEP_FLAGS_TWITCH) {
				m.ExcitementContinuous += 20.0f; // everybody on Twitch is super-exciting
			}

			if (peep->state != m.LastState) {
				m.ExcitementBonusSlow	+= 0.5f;
				m.ExcitementBonusFast	+= 0.5f;

				if (peep->state == PEEP_STATE_QUEUING) {
					m.ExcitementBonusSlow += 1.0f;
				}
				else if (peep->state == PEEP_STATE_SITTING) {
					m.ExcitementBonusSlow += 0.5f;
				}
				else if (peep->state == PEEP_STATE_ENTERING_RIDE) {
					m.ExcitementBonusFast += 0.5f;
					m.ExcitementBonusSlow += 0.5f;
					if (first) {
						AddRidePenalty(peep->current_ride);
						m.ExcitementEffective += 15.0f; // don't interrupt main peeps on-ride too often
					}
				}
			}

			if (m.LastItemExtraFlags != peep->item_extra_flags ||
				m.LastItemStandardFlags != peep->item_standard_flags) {
				m.ExcitementBonusSlow += 1.5f;
				m.ExcitementBonusFast += 1.5f;
			}
			
			for (int i = 0; i < PEEP_MAX_THOUGHTS; ++i) {
				if (peep->thoughts[i].type == PEEP_THOUGHT_TYPE_NONE) {
					i = PEEP_MAX_THOUGHTS;
					break;
				}
				if (peep->thoughts[i].var_2 == 1) { // If a fresh thought
					m.ExcitementContinuous += 1.5f;
					break;
				}
			}

			m.DistanceToTarget	= -1.0f;

			if (peep->guest_heading_to_ride_id != 0xFF &&
				!(peep->state == PEEP_STATE_ON_RIDE
					|| peep->state == PEEP_STATE_ENTERING_RIDE
					|| peep->state == PEEP_STATE_QUEUING
					|| peep->state == PEEP_STATE_LEAVING_RIDE)) {
				m.ExcitementContinuous += 0.5f;

				float	tarX = (float)peep->destination_x;
				float	tarY = (float)peep->destination_x;

				tarX -= m.LastPosition[0];
				tarY -= m.LastPosition[1];

				m.DistanceToTarget	= sqrt(tarX * tarX + tarY * tarY);

				m.ExcitementContinuous += 2.0f - log10(1.0f + (m.DistanceToTarget / 5.0f));
			}
			if (m.LastHeadingTo != peep->guest_heading_to_ride_id) {
				m.ExcitementBonusSlow += 0.75f;
				m.ExcitementBonusFast += 0.50f;
			}

			if (peep->state == PEEP_STATE_ON_RIDE
				|| peep->state == PEEP_STATE_ENTERING_RIDE
				|| peep->state == PEEP_STATE_LEAVING_RIDE) {

				m.ExcitementBonusRide = 2.0f;

				rct_ride *ride = get_ride(peep->current_ride);

				m.ExcitementBonusRide += 0.002f * (float)ride->excitement;
				m.ExcitementBonusRide += 0.002f * (float)ride->intensity;
			}
			if (peep->state == PEEP_STATE_QUEUING) {
				rct_ride *ride = get_ride(peep->current_ride);

				m.PeopleAheadInCue = 0;

				rct_peep* other_peep = peep;

				while (true) {
					uint16 nPeep = other_peep->next_in_queue;
					if (nPeep == 0xFFFF)
						break;
					other_peep = GET_PEEP(nPeep);
					m.PeopleAheadInCue++;
				}

				m.ExcitementContinuous += 1.0f - (float)(m.PeopleAheadInCue) * 0.075f;
			}
			else {
				m.PeopleAheadInCue	= 0;
			}
			if (peep->state == PEEP_STATE_SITTING) {
				m.ExcitementContinuous -= 0.5f;
			}

			uint8 checkForRide = 0xFF;

			if (peep->current_ride != 0xFF)
				checkForRide = peep->current_ride;
			if (peep->guest_heading_to_ride_id != 0xFF)
				checkForRide = peep->guest_heading_to_ride_id;
			if (checkForRide != 0xFF) {
				m.ExcitementContinuous += 3.0f - ((float)(CountRidePenalty(checkForRide)) / 15.0f);
			}

			if (scenario_rand() % 1000 == 0) {
				// are you feeling lucky?
				m.ExcitementBonusSlow += 3.5f;
			}
			if (scenario_rand() % 100 == 0) {
				m.ExcitementGoodwill = -3.0f + (float)(scenario_rand() % 6);
			}

			m.ExcitementContinuous	+= m.ExcitementGoodwill;

			m.LastState				= peep->state;
			m.LastRide				= peep->current_ride;
			m.LastHeadingTo			= peep->guest_heading_to_ride_id;
			m.LastItemStandardFlags	= peep->item_standard_flags;
			m.LastItemExtraFlags	= peep->item_extra_flags;

			sumAudienceBoost		+= m.ExcitementAudienceParticipation;
			m.ExcitementAudienceParticipation	*= 0.9995f;

			first = false;
		}

		if (sumAudienceBoost > 10.0f) {
			float div = 10.0f / sumAudienceBoost;

			for (WatchedPeep &m : _watchedPeeps) {
				m.ExcitementAudienceParticipation *= div;
			}
		}

		for (uint32 i = 0; i < _watchedPeeps.size(); i++) {
			WatchedPeep &firstPeep = _watchedPeeps[i];

			for (uint32 j = i + 1; j < _watchedPeeps.size(); j++) {
				WatchedPeep &peep = _watchedPeeps[j];

				float	offset[3];
				offset[0] = firstPeep.LastPosition[0] - peep.LastPosition[0];
				offset[1] = firstPeep.LastPosition[1] - peep.LastPosition[1];
				offset[2] = firstPeep.LastPosition[2] - peep.LastPosition[2];

				float distance = sqrt(offset[0] * offset[0] + offset[1] * offset[1] + offset[2] * offset[2]);

				float relevance = Math::Max(0.0f, 500.0f - distance) / 500.0f;

				firstPeep.ExcitementContinuous	+= relevance * relevance * 5.0f;
				peep.ExcitementContinuous		+= relevance * relevance * 5.0f;

				if (i == 0) {
					peep.FollowWeight				+= relevance;
				}

				// this code shouldn't be here, but oh well

				if (relevance > 0.95f) {
					if (firstPeep.GreetTimeout == 0) {
						rct_peep *fPeep = GET_PEEP(firstPeep.SpriteId);
						if (fPeep->action == PEEP_ACTION_NONE_1 || fPeep->action == PEEP_ACTION_NONE_2) {
							fPeep->action = PEEP_ACTION_JOY;
							fPeep->action_frame = 0;
							fPeep->action_sprite_image_offset = 0;
							peep_update_current_action_sprite_type(fPeep);
							invalidate_sprite_2((rct_sprite*)fPeep);
							firstPeep.GreetTimeout = 200;
						}
					}
					else if (peep.GreetTimeout == 0) {
						rct_peep *fPeep = GET_PEEP(peep.SpriteId);
						if (fPeep->action == PEEP_ACTION_NONE_1 || fPeep->action == PEEP_ACTION_NONE_2) {
							fPeep->action = PEEP_ACTION_JOY;
							fPeep->action_frame = 0;
							fPeep->action_sprite_image_offset = 0;
							peep_update_current_action_sprite_type(fPeep);
							invalidate_sprite_2((rct_sprite*)fPeep);
							peep.GreetTimeout = 200;
						}
					}
				}
			}
		}

		for (WatchedPeep &m : _watchedPeeps) {
			m.ExcitementEffective	+= (m.ExcitementContinuous - m.ExcitementEffective) * 0.01f;
			m.ExcitementSort		= m.ExcitementEffective + m.ExcitementAudienceParticipation;
		}

		if (_watchedPeeps.size() > 0) {
			_watchedPeeps[0].ExcitementSort		+= 4.0f;
		}
	}

	static int FindWatchedPeepIndexByVagueID(const char *ID) {
		char buffer[128];
		const char *lookup = ID;
		const char * ch = strchrm(ID, " \t");
		if (ch) {
			safe_strcpy(buffer, ID, Math::Min(sizeof(buffer), (size_t)(ch - ID + 1)));
			lookup = buffer;
		}

		int		matchBegins		= -1;
		int		matchContains	= -1;

		for (uint32 i = 0; i < _watchedPeeps.size(); i++) {
			rct_peep *peep = GET_PEEP(_watchedPeeps[i].SpriteId);

			utf8 peepNameBuf[256];
			format_string(peepNameBuf, sizeof(peepNameBuf), peep->name_string_idx, &peep->id);

			if (String::Equals(peepNameBuf, lookup, true)) {
				return (int)i;
			}
			if (matchBegins == -1) {
				if (String::StartsWith(peepNameBuf, lookup, true)) {
					matchBegins	= (int)i;
				}
			}
			else if (matchContains == -1 && strstr(peepNameBuf, lookup)) {
				matchContains	= (int)i;
			}
		}

		if (matchBegins != -1)
			return matchBegins;
		return matchContains;
	}

	static uint8 FindRideIndexByVagueID(const char *ID) {
		char buffer[128];
		const char *lookup = ID;
		const char * ch = strchrm(ID, " \t");
		if (ch) {
			safe_strcpy(buffer, ID, Math::Min(sizeof(buffer), (size_t)(ch - ID + 1)));
			lookup = buffer;
		}

		uint8	matchBegins = 0xFF;
		uint8	matchContains = 0xFF;

		int index;
		rct_ride *ride;
		FOR_ALL_RIDES(index, ride) {
			utf8 rideNameBuf[256];
			format_string(rideNameBuf, sizeof(rideNameBuf), ride->name, &ride->name_arguments);

		//	std::stringstream stream;
		//	stream << "Trying " << index << ": " << rideNameBuf;
		//	news_item_add_to_queue_raw(NEWS_ITEM_BLANK, stream.str().c_str(), 0);

			if (String::Equals(rideNameBuf, lookup, true)) {
				return (uint8)index;
			}
			if (matchBegins == 0xFF) {
				if (String::StartsWith(rideNameBuf, lookup, true)) {
					matchBegins = (uint8)index;
				}
				else if (matchContains == 0xFF && strstr(rideNameBuf, lookup)) {
					matchContains = (uint8)index;
				}
			}
		}

		if (matchBegins != 0xFF)
			return matchBegins;
		return matchContains;
	}
	
	static void AudiencePromoteSee(const char *giver, const char *seeID) {
		int index = FindWatchedPeepIndexByVagueID(seeID);

		if (index == -1) {
			return;
		}

		std::stringstream stream;
		stream << giver << " promoted " << seeID;
		news_item_add_to_queue_raw(NEWS_ITEM_BLANK, stream.str().c_str(), 0);

		_watchedPeeps[index].ExcitementAudienceParticipation	+= 4.0f;
	}

	static void AudiencePromoteGo(const char *giver, const char *goID) {
		int index = FindWatchedPeepIndexByVagueID(giver);

		if (index == -1) {
			return;
		}

		rct_peep *peep = GET_PEEP(_watchedPeeps[index].SpriteId);

		if (peep->state == PEEP_STATE_ON_RIDE
			|| peep->state == PEEP_STATE_ENTERING_RIDE
			|| peep->state == PEEP_STATE_LEAVING_RIDE
			|| peep->state == PEEP_STATE_QUEUING_FRONT) {
			std::stringstream stream;
			stream << giver << " is too busy.";
			news_item_add_to_queue_raw(NEWS_ITEM_BLANK, stream.str().c_str(), 0);
			return;
		}

		uint8 rideID = FindRideIndexByVagueID(goID);

		if (rideID == 0xFF) {
			std::stringstream stream;
			stream << giver << " cannot find " << goID;
			news_item_add_to_queue_raw(NEWS_ITEM_BLANK, stream.str().c_str(), 0);
			return;
		}

		peep->guest_heading_to_ride_id = rideID;
		peep->peep_is_lost_countdown = 200;
		peep->pathfind_goal.x = 0xFF;
		peep->pathfind_goal.y = 0xFF;
		peep->pathfind_goal.z = 0xFF;
		peep->pathfind_goal.direction = 0xFF;

		if (peep->state == PEEP_STATE_QUEUING) {
			remove_peep_from_queue(peep);
			peep_decrement_num_riders(peep);
			peep->state = PEEP_STATE_FALLING;
			peep_window_state_update(peep);
		}

		std::stringstream stream;
		stream << giver << " is now going to " << goID << " (" << (uint32)(rideID) << ")";
		news_item_add_to_queue_raw(NEWS_ITEM_BLANK, stream.str().c_str(), 0);
	}

	static void UpdateStepSort() {
		// Just a dumb sort

		for (int i = _watchedPeeps.size() - 1; i > 0; i--) {
			if (_watchedPeeps[i].ExcitementSort < _watchedPeeps[i-1].ExcitementSort)
				continue;

			WatchedPeep tmp;
			memcpy(&tmp, &_watchedPeeps[i-1], sizeof(WatchedPeep));
			memcpy(&_watchedPeeps[i-1], &_watchedPeeps[i], sizeof(WatchedPeep));
			memcpy(&_watchedPeeps[i], &tmp, sizeof(WatchedPeep));

			// small bonus to keep it ahead for a bit
			_watchedPeeps[i-1].ExcitementBonusFast += 0.5f;

			if (i == 1) {
				_watchedPeeps[i - 1].ExcitementBonusSlow += 10.0f;
				_watchedPeeps[i - 1].ExcitementEffective += 15.0f; // don't fly about too much

				gCurrentRotation = scenario_rand() & 3;
			}
		}
	}

	static void UpdateStepTrack() {
		for (WatchedPeep &m : _watchedPeeps) {
			rct_peep *peep = GET_PEEP(m.SpriteId);

			if (peep->x == SPRITE_LOCATION_NULL) {
				if (peep->state == PEEP_STATE_ON_RIDE
					|| peep->state == PEEP_STATE_ENTERING_RIDE
					|| (peep->state == PEEP_STATE_LEAVING_RIDE && peep->x == SPRITE_LOCATION_NULL)) {

					rct_ride *ride = get_ride(peep->current_ride);
					if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
						rct_vehicle* train = GET_VEHICLE(ride->vehicles[peep->current_train]);
						int car = peep->current_car;

						for (; car != 0; car--) {
							train = GET_VEHICLE(train->next_vehicle_on_train);
						}

						m.LastPosition[0] = (float)train->x;
						m.LastPosition[1] = (float)train->y;
						m.LastPosition[2] = (float)train->z;
					}
				}
			}
			else {
				m.LastPosition[0] = (float)peep->x;
				m.LastPosition[1] = (float)peep->y;
				m.LastPosition[2] = (float)peep->z;
			}
		}
	}

	static void AddRidePenalty(uint8 ride) {
		_rideBasePenalties[(_rideBasePenaltyIndex++) % RidePenaltyCount] = ride;
		_rideTypePenalties[(_rideTypePenaltyIndex++) % RidePenaltyCount] = get_ride(ride)->type;
	}

	static uint32 CountRidePenalty(uint8 ride) {
		uint32	penalty		= 0;
		uint8	type		= get_ride(ride)->type;

		for (int i = 0; i < RidePenaltyCount; i++) {
			if (_rideBasePenalties[i] == ride)
				penalty += 2;
			if (_rideTypePenalties[i] == type)
				penalty += 1;
		}

		return penalty;
	}

	static void AddNewWatchedPeep()
	{
		sint16		countOverall	= -(sint16)(_watchedPeeps.size());
		sint16		countTwitch		= 0;

		for (WatchedPeep &m : _watchedPeeps) {
			if (0 != ((GET_PEEP(m.SpriteId))->peep_flags & PEEP_FLAGS_TWITCH)) {
				countTwitch -= 1;
			}
		}

		uint16		sprite_index;
		rct_peep	*peep = 0;
		bool		selectForTwitch;

		FOR_ALL_GUESTS(sprite_index, peep) {
			if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
				continue;

			countOverall++;

			if (peep->peep_flags & PEEP_FLAGS_TWITCH) {
				countTwitch++;
			}
		}

		if (countOverall == 0)
			return; // There's nobody here...

		selectForTwitch		= (countTwitch > 0);

		uint16	selected		= scenario_rand() % (selectForTwitch? countTwitch : countOverall);
		uint16	selectedPeep	= SPRITE_INDEX_NULL;

		FOR_ALL_GUESTS(sprite_index, peep) {
			if (0 != (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK))
				continue;
			if (selectForTwitch && !(peep->peep_flags & PEEP_FLAGS_TWITCH))
				continue;

			bool found = false;

			for (WatchedPeep &m : _watchedPeeps) {
				if (m.SpriteId == sprite_index) {
					found = true;
					break;
				}
			}
			if (found)
				continue;

			if (selected == 0) {
				selectedPeep = sprite_index;
				break;
			}

			selected -= 1;
		}

		if (selectedPeep == SPRITE_INDEX_NULL) {
			// We couldn't find any?! Oh well...
			return;
		}

		WatchedPeep newPeep;

		newPeep.Reset();
		newPeep.SpriteId				= selectedPeep;
		newPeep.DefaultTShirtColour		= peep->tshirt_colour;
		newPeep.DefaultTrouserColour	= peep->trousers_colour;
		newPeep.DefaultUmbrellaColour	= peep->umbrella_colour;

		_watchedPeeps.push_back(newPeep);
	}

	static void UpdatePeepFlash()
	{
		int itt = _updateTickNumber % 10;

		if (itt == 0) {
			for (WatchedPeep &m : _watchedPeeps) {
				rct_peep *peep = GET_PEEP(m.SpriteId);
				m.DefaultTrouserColour	= peep->trousers_colour;
				m.DefaultTShirtColour	= peep->tshirt_colour;
				m.DefaultUmbrellaColour	= peep->umbrella_colour;
				peep->trousers_colour	= COLOUR_ICY_BLUE;
				peep->tshirt_colour		= COLOUR_ICY_BLUE;
				peep->umbrella_colour	= COLOUR_ICY_BLUE;
			}
		}
		else if (itt == 3) {
			for (WatchedPeep &m : _watchedPeeps) {
				rct_peep *peep = GET_PEEP(m.SpriteId);
					// peeps can purchase items; do not change back if they changed colours while flashing
				if (peep->tshirt_colour == COLOUR_ICY_BLUE)
					peep->tshirt_colour = m.DefaultTShirtColour;
				if (peep->trousers_colour == COLOUR_ICY_BLUE)
					peep->trousers_colour = m.DefaultTrouserColour;
				if (peep->umbrella_colour == COLOUR_ICY_BLUE)
					peep->umbrella_colour = m.DefaultUmbrellaColour;
			}
		}
	}

	static void UpdatePeepDoTrack()
	{
		float		TargetPosition[3] = { 0, 0, 0 };
		float		sumWeight = 0.0f;

		rct_peep *peep = GET_PEEP(_effectiveFollowSprite);

		for (WatchedPeep &m : _watchedPeeps) {
			if (m.FollowWeight == 0.0f)
				continue;

			TargetPosition[0] += m.LastPosition[0] * m.FollowWeight;
			TargetPosition[1] += m.LastPosition[1] * m.FollowWeight;
			TargetPosition[2] += m.LastPosition[2] * m.FollowWeight;
			sumWeight += m.FollowWeight;
		}

		if (sumWeight == 0.0f)
			return;

		TargetPosition[0]	/= sumWeight;
		TargetPosition[1]	/= sumWeight;
		TargetPosition[2]	/= sumWeight;

		rct_window *mainWindow;
		if ((mainWindow = window_get_main()) != NULL) {
			float addDelta[3];
			float pullTo[3];
			float pullForce[3];

			const float		clamp		= 10000.0f;
			const float		deadzone	= 5.0f;

			addDelta[0] = TargetPosition[0] - _watchingPositionPreviousTarget[0];
			addDelta[1] = TargetPosition[1] - _watchingPositionPreviousTarget[1];
			addDelta[2] = TargetPosition[2] - _watchingPositionPreviousTarget[2];

			if (_watchingPositionPrevious[0] == -99999999.0f) {
				_watchingPositionPrevious[0] = TargetPosition[0];
				_watchingPositionPrevious[1] = TargetPosition[1];
				_watchingPositionPrevious[2] = TargetPosition[2];
				addDelta[0] = 0.0f;
				addDelta[1] = 0.0f;
				addDelta[2] = 0.0f;
			}

			addDelta[0] *= 10.f;
			addDelta[1] *= 10.f;
			addDelta[2] *= 10.f;

			_watchingPositionPreviousTarget[0] = TargetPosition[0];
			_watchingPositionPreviousTarget[1] = TargetPosition[1];
			_watchingPositionPreviousTarget[2] = TargetPosition[2];

		//	_watchingPositionPrevious[0] = TargetPosition[0];
		//	_watchingPositionPrevious[1] = TargetPosition[1];
		//	_watchingPositionPrevious[2] = TargetPosition[2];

			_watchingPositionDelta[0] += addDelta[0] * 0.1f;
			_watchingPositionDelta[1] += addDelta[1] * 0.1f;
			_watchingPositionDelta[2] += addDelta[2] * 0.1f;

			_watchingPositionDelta[0] /= 1.1f;
			_watchingPositionDelta[1] /= 1.1f;
			_watchingPositionDelta[2] /= 1.1f;

			if (_watchingPositionDelta[0] > clamp)
				_watchingPositionDelta[0] = clamp;
			else if (_watchingPositionDelta[0] < -clamp)
				_watchingPositionDelta[0] = -clamp;
			if (_watchingPositionDelta[1] > clamp)
				_watchingPositionDelta[1] = clamp;
			else if (_watchingPositionDelta[1] < -clamp)
				_watchingPositionDelta[1] = -clamp;
			if (_watchingPositionDelta[2] > clamp)
				_watchingPositionDelta[2] = clamp;
			else if (_watchingPositionDelta[2] < -clamp)
				_watchingPositionDelta[2] = -clamp;

			pullTo[0] = TargetPosition[0] + _watchingPositionDelta[0];
			pullTo[1] = TargetPosition[1] + _watchingPositionDelta[1];
			pullTo[2] = TargetPosition[2] + _watchingPositionDelta[2];

			pullForce[0] = pullTo[0] - _watchingPositionPrevious[0];
			pullForce[1] = pullTo[1] - _watchingPositionPrevious[1];
			pullForce[2] = pullTo[2] - _watchingPositionPrevious[2];

			float pullForceMagCur	= sqrt(pullForce[0] * pullForce[0] + pullForce[1] * pullForce[1] + pullForce[2] * pullForce[2]);
			float pullForceMagTar	= pullForceMagCur;

			pullForceMagTar		*= 0.12f;

			if (pullForceMagTar > 300.0f)
				pullForceMagTar = 300.0f;

			if (pullForceMagTar < 15.0f)
				pullForceMagTar = 0.0f;
			else
				pullForceMagTar -= 15.0f;

			if (pullForceMagTar > 0.001f) {
				float	fac = pullForceMagTar / pullForceMagCur;
				_watchingPositionPrevious[0] += pullForce[0] * fac;
				_watchingPositionPrevious[1] += pullForce[1] * fac;
				_watchingPositionPrevious[2] += pullForce[2] * fac;
			}

			sint16 effPositionX = sint16(_watchingPositionPrevious[0]);
			sint16 effPositionY = sint16(_watchingPositionPrevious[1]);
			sint16 effPositionZ = sint16(_watchingPositionPrevious[2]);

			window_scroll_to_location(mainWindow, effPositionX, effPositionY, effPositionZ + 50);
		}
	}

	static void RichPeepState(char *out, rct_peep *peep) {
		char tmp[256];

		uint32 argument1, argument2;

		get_arguments_from_action(peep, &argument1, &argument2);
		set_format_arg(0, uint32, argument1);
		set_format_arg(4, uint32, argument2);
		set_format_arg(8, uint16, 0);

		format_string(tmp, sizeof(tmp), STR_BLACK_STRING, gCommonFormatArgs);

		std::stringstream stream;

		if (tmp[0] && tmp[1] && tmp[2])
			stream << tmp + 2;

		if (peep->state == PEEP_STATE_WALKING ||
			peep->state == PEEP_STATE_SITTING) {
			if (int drink = peep_get_drink(peep)) {
				window_guest_inventory_format_item(peep, drink);

				format_string(tmp, sizeof(tmp), STR_PEEPWATCH_DRINKING, gCommonFormatArgs);

				stream << tmp;
			}
			else if (int food = peep_get_foodstuff(peep)) {
				window_guest_inventory_format_item(peep, food);

				format_string(tmp, sizeof(tmp), STR_PEEPWATCH_EATING, gCommonFormatArgs);

				stream << tmp;
			}
		}

		safe_strcpy(out, stream.str().c_str(), 1000);
	}

	static void UpdateFileCurrentPeep()
	{
		int bufSize = 1000;

		std::stringstream out;
		utf8 tmp[1000];

		if (_watchedPeeps.size() == 0) {
			format_string(tmp, sizeof(tmp), STR_PEEPWATCH_NOBODY_IS_HERE, 0);
			out << tmp;
		}
		else {
			WatchedPeep &watch = _watchedPeeps[0];
			rct_peep	*peep = GET_PEEP(watch.SpriteId);

			set_format_arg(0, uint16, peep->name_string_idx);
			set_format_arg(2, uint32, peep->id);
			format_string(tmp, sizeof(tmp), STR_PEEPWATCH_WATCHING, gCommonFormatArgs);

			out << tmp << " -- ";

			RichPeepState(tmp, peep);
			out << tmp;
		}

		auto fs = FileStream(_ioCurrentName, FILE_MODE_WRITE);
		fs.WriteString(out.str());
	}

	static void UpdateFileCurrentThought()
	{
		int bufSize = 1000;

		utf8 tmp[1000];

		std::stringstream out;

		if (_watchedPeeps.size() == 0) {
			format_string(tmp, sizeof(tmp), STR_PEEPWATCH_NOBODY_IS_HERE, 0);
			out << tmp;
		}
		else {
			WatchedPeep &watch = _watchedPeeps[0];
			rct_peep	*peep = GET_PEEP(watch.SpriteId);

			uint32		argument1, argument2;

			int i = 0;
			for (; i < PEEP_MAX_THOUGHTS; ++i) {
				if (peep->thoughts[i].type == PEEP_THOUGHT_TYPE_NONE) {
					i = PEEP_MAX_THOUGHTS;
					break;
				}
				if (peep->thoughts[i].var_2 == 1) { // If a fresh thought
					break;
				}
			}
			if (i != PEEP_MAX_THOUGHTS) {
				peep_thought_set_format_args(&peep->thoughts[i]);
				format_string(tmp, sizeof(tmp), STR_PEEPWATCH_THOUGHT, gCommonFormatArgs);
				out << (tmp + 1);
			}
			else {
				if (watch.PeopleAheadInCue > 0) {
					set_format_arg(0, uint16, watch.PeopleAheadInCue);
					format_string(tmp, sizeof(tmp), STR_PEEPWATCH_PEEPS_AHEAD, gCommonFormatArgs);
					out << tmp;
				}
				if (watch.DistanceToTarget > 0.0f) {
					set_format_arg(0, sint16, (sint16)(watch.DistanceToTarget) >> 4);
					format_string(tmp, sizeof(tmp), STR_PEEPWATCH_DISTANCE, gCommonFormatArgs);
					out << tmp;
				}
			}
		}

		auto fs = FileStream(_ioCurrentThought, FILE_MODE_WRITE);
		fs.WriteString(out.str());
	}

	static void UpdateFileCurrentState()
	{
		const int bufSize = 1000;

		utf8 tmp[bufSize];

		std::stringstream out;

		if (_watchedPeeps.size() == 0) {
			format_string(tmp, sizeof(tmp), STR_PEEPWATCH_NOBODY_IS_HERE, 0);
			out << tmp;
		}
		else {
			WatchedPeep &watch = _watchedPeeps[0];
			rct_peep	*peep = GET_PEEP(watch.SpriteId);

			int tickNumber = ((_updateTickNumber / 70) % 3);

			switch (tickNumber) {
				case 0: {
					int happiness	= peep->happiness;
					int energy		= ((peep->energy - 32) * 85) / 32;
					int hunger		= peep->hunger;
					int thirst		= peep->thirst;
					int bathroom	= peep->bathroom;
					int nausea		= peep->nausea;

					if (happiness < 10)	happiness = 10;
					happiness *= 100;
					happiness /= 255;

					set_format_arg(0, uint16, (uint16)(happiness));
					format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_HAPPY, gCommonFormatArgs);
					out << tmp << "\r\n";

					energy *= 100;
					energy /= 255;

					set_format_arg(0, uint16, (uint16)(energy));
					format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_ENERGY, gCommonFormatArgs);
					out << tmp << "\r\n";

					if (hunger > 190)
						hunger = 190;
					hunger -= 32;
					if (hunger < 0) hunger = 0;
					hunger *= 51;
					hunger /= 32;
					hunger = 0xFF & ~hunger;
					hunger *= 100;
					hunger /= 255;

					set_format_arg(0, uint16, (uint16)(hunger));
					format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_HUNGER, gCommonFormatArgs);
					out << tmp << "\r\n";

					if (thirst > 190)
						thirst = 190;
					thirst -= 32;
					if (thirst < 0) thirst = 0;
					thirst *= 51;
					thirst /= 32;
					thirst = 0xFF & ~thirst;
					thirst *= 100;
					thirst /= 255;

					set_format_arg(0, uint16, (uint16)(thirst));
					format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_THIRST, gCommonFormatArgs);
					out << tmp << "\r\n";

					nausea -= 32;
					if (nausea < 0) nausea = 0;
					nausea *= 36;
					nausea /= 32;
					nausea *= 100;
					nausea /= 255;

					set_format_arg(0, uint16, (uint16)(nausea));
					format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_NAUSEA, gCommonFormatArgs);
					out << tmp << "\r\n";

					bathroom -= 32;
					if (bathroom < 0) bathroom = 0;
					bathroom *= 45;
					bathroom /= 32;
					bathroom *= 100;
					bathroom /= 255;

					set_format_arg(0, uint16, (uint16)(bathroom));
					format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_BATHROOM, gCommonFormatArgs);
					out << tmp;

					break;
				}
				case 1: {
					int money = peep->cash_in_pocket;
					int spent = peep->cash_spent;

					set_format_arg(0, money32, (money32)(money));
					set_format_arg(4, money32, (money32)(spent));
					format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_CASH, gCommonFormatArgs);
					out << tmp << "\r\n";

					if (peep->paid_to_enter > 0) {
						set_format_arg(0, money32, (money32)(peep->paid_to_enter));
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_ENTRANCE, gCommonFormatArgs);
						out << tmp << "\r\n";
					}

					set_format_arg(0, uint16, (uint16)(peep->no_of_rides));
					if (peep->paid_on_rides == 0) {
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_RIDES_FREE, gCommonFormatArgs);
					}
					else {
						set_format_arg(2, money32, (money32)(peep->paid_on_rides));
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_RIDES, gCommonFormatArgs);
					}
					out << tmp << "\r\n";

					set_format_arg(0, uint16, (uint16)(peep->no_of_food));
					if (peep->paid_on_souvenirs == 0) {
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_FOOD_FREE, gCommonFormatArgs);
					}
					else {
						set_format_arg(2, money32, (money32)(peep->paid_on_food));
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_FOOD, gCommonFormatArgs);
					}
					out << tmp << "\r\n";

					set_format_arg(0, uint16, (uint16)(peep->no_of_drinks));
					if (peep->paid_on_drink == 0) {
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_DRINKS_FREE, gCommonFormatArgs);
					}
					else {
						set_format_arg(2, money32, (money32)(peep->paid_on_drink));
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_DRINKS, gCommonFormatArgs);
					}
					out << tmp << "\r\n";
						
					set_format_arg(0, uint16, (uint16)(peep->no_of_souvenirs));
					if (peep->paid_on_souvenirs == 0) {
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_SOUVENIRS_FREE, gCommonFormatArgs);
					}
					else {
						set_format_arg(2, money32, (money32)(peep->paid_on_souvenirs));
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_SOUVENIRS, gCommonFormatArgs);
					}
					out << tmp;

					if (peep->time_in_park != -1) {
						out << "\r\n";

						int eax = gScenarioTicks;
						eax -= peep->time_in_park;
						eax >>= 11;

						set_format_arg(0, uint16, (uint16)(eax));
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_SHOW_TIME_IN_PARK, gCommonFormatArgs);
						out << tmp;
					}
					break;
				}
				case 2: {
					format_string(tmp, sizeof(tmp), STR_PEEPWATCH_ITEM_LIST_HEADER, gCommonFormatArgs);
					out << tmp << "\r\n";

					bool first = true;
					for (int item = 0; item < SHOP_ITEM_COUNT; item++) {
						if (!peep_has_item(peep, item)) continue;

						if (!first) {
							out << "\r\n";
						}
						else {
							first = false;
						}

						rct_string_id stringId = window_guest_inventory_format_item(peep, item);
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_ITEM_LIST_ITEM, gCommonFormatArgs);
						out << tmp;
					}

					if (first) {
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_ITEM_LIST_NOTHING, gCommonFormatArgs);
						out << tmp << "\r\n";
					}

					break;
				}
			}
		}

		auto fs = FileStream(_ioCurrentState, FILE_MODE_WRITE);
		fs.WriteString(out.str());
	}

	static void UpdateFileAllPeeps()
	{
		int bufSize = 1000;
		utf8 tmp[1000];
		char star[16];
		utf8 finger[16];

		std::stringstream out;

		if (_watchedPeeps.size() == 0) {
			format_string(tmp, sizeof(tmp), STR_PEEPWATCH_NOBODY_IS_HERE, 0);
		}
		else {
			format_string(star, sizeof(star), STR_PEEPWATCH_IMPORTANCE_STAR, 0);
			format_string(finger, sizeof(finger), STR_PEEPWATCH_IMPORTANCE_PUSH, 0);

			int index = 0;

			format_string(tmp, sizeof(tmp), STR_PEEPWATCH_GUEST_LIST_HEADER, 0);
			out << tmp << "\r\n";

			for (WatchedPeep &watch : _watchedPeeps) {
				rct_peep	*peep = GET_PEEP(watch.SpriteId);

				format_string(tmp, sizeof(tmp), peep->name_string_idx, &peep->id);
				out << tmp << " ";

				float watchSort = watch.ExcitementSort - watch.ExcitementAudienceParticipation;

				int starCount;

				starCount = int(0.89f + watch.ExcitementAudienceParticipation / 4.5f);
				for (int i = 0; i < starCount; i++)
					out << finger;
				starCount = int(0.89f + watchSort / 5.5f);
				for (int i = 0; i < starCount; i++)
					out << star;
				out <<"\r\n";

				index++;
				if (index == 1 || index > 6) {
					continue;
				}

				out << " - ";

				int i = 0;
				for (; i < PEEP_MAX_THOUGHTS; ++i) {
					if (peep->thoughts[i].type == PEEP_THOUGHT_TYPE_NONE) {
						i = PEEP_MAX_THOUGHTS;
						break;
					}
					if (peep->thoughts[i].var_2 == 1) { // If a fresh thought
						break;
					}
				}

				uint32 argument1, argument2;

				if (i != PEEP_MAX_THOUGHTS) {
					peep_thought_set_format_args(&peep->thoughts[i]);

					format_string(tmp, sizeof(tmp), STR_PEEPWATCH_THOUGHT, gCommonFormatArgs);
					out << (tmp+1);
				}
				else {
					RichPeepState(tmp, peep);
					out << tmp;

					if (watch.PeopleAheadInCue > 0) {
						set_format_arg(0, uint16, watch.PeopleAheadInCue);
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_PEEPS_AHEAD_SHORT, gCommonFormatArgs);
						out << " " << tmp;
					}
					if (watch.DistanceToTarget > 0.0f) {
						set_format_arg(0, sint16, (sint16)(watch.DistanceToTarget) >> 4);
						format_string(tmp, sizeof(tmp), STR_PEEPWATCH_DISTANCE_SHORT, gCommonFormatArgs);
						out << " " << tmp;
					}
				}
				out << "\r\n";
			}
		}

		auto fs = FileStream(_ioAllPeeps, FILE_MODE_WRITE);
		fs.WriteString(out.str());
	}

	static void Begin()
	{
		_maintainPark						= gConfigPeepwatch.enable_maintain_park != 0;
		_totalTrackCount					= gConfigPeepwatch.max_follow;

		if (_maintainPark) {
			news_item_add_to_queue(NEWS_ITEM_AWARD, STR_PEEPWATCH_ENABLE_MAINTENANCE_ON, 0);
		}
		else {
			news_item_add_to_queue(NEWS_ITEM_AWARD, STR_PEEPWATCH_ENABLE_MAINTENANCE_OFF, 0);
		}
		_updateLastTick = platform_get_ticks();

		_updateTickNumber					= 0;
		_updateStepTickLeft					= 0;
		_watchingPositionDelta[0]			= 0.0f;
		_watchingPositionDelta[1]			= 0.0f;
		_watchingPositionDelta[2]			= 0.0f;
		_watchingPositionPrevious[0]		= -99999999.0f;
		_watchingPositionPrevious[1]		= 0.0f;
		_watchingPositionPrevious[2]		= 0.0f;
		_watchingPositionPreviousTarget[0]	= 0.0f;
		_watchingPositionPreviousTarget[1]	= 0.0f;
		_watchingPositionPreviousTarget[2]	= 0.0f;
		_watchedPeeps.clear();

		_rideBasePenaltyIndex			= 0;
		memset(_rideBasePenalties, 0xFFFFFFFF, sizeof(uint8) * 128 / 4);
		_rideTypePenaltyIndex			= 0;
		memset(_rideTypePenalties, 0xFFFFFFFF, sizeof(uint8) * 128 / 4);

		platform_get_user_directory(_ioCurrentName, NULL, MAX_PATH);
		safe_strcat(_ioCurrentName, "peepwatch_current.txt", MAX_PATH);
		platform_get_user_directory(_ioCurrentThought, NULL, MAX_PATH);
		safe_strcat(_ioCurrentThought, "peepwatch_thought.txt", MAX_PATH);
		platform_get_user_directory(_ioCurrentState, NULL, MAX_PATH);
		safe_strcat(_ioCurrentState, "peepwatch_state.txt", MAX_PATH);
		platform_get_user_directory(_ioAllPeeps, NULL, MAX_PATH);
		safe_strcat(_ioAllPeeps, "peepwatch_allpeeps.txt", MAX_PATH);
	}

	static void End()
	{
		news_item_add_to_queue_raw(NEWS_ITEM_AWARD, "No longer peep-watching.", 0);
	}

	static void Update()
	{
		if (IsPeepwatchEnabled()) {
			if (game_is_paused()) return;

			if (_updateLastTick == 0) {
				Begin();
				_updateLastTick = platform_get_ticks();
				return;
			}

			uint32 currentTime = platform_get_ticks();
			uint32 timeSinceLastTick = currentTime - _updateLastTick;
			_updateStepTickLeft		+= timeSinceLastTick;
			_updateLastTick			= currentTime;

			if (_updateStepTickLeft > 2000) {
				_updateStepTickLeft = 2000;
			}

			while (_updateStepTickLeft > 100) {
				_updateStepTickLeft	-= 100;
				_updateTickNumber	+= 1;
				UpdateStep();
				UpdatePeepFlash();
			}

			UpdatePeepDoTrack();
		}
		else {
			if (_updateLastTick > 0) {
				End();
				_updateLastTick = 0;
			}
		}
	}

	// See what we need to do

	void HandleCommand(const char *giver, const char *message) {
		char buffer[32];
		const char * ch = strchrm(message, " \t");

		if (!ch)
			return;

		safe_strcpy(buffer, message, Math::Min(sizeof(buffer), (size_t)(ch - message + 1)));
		ch = strskipwhitespace(ch);

		if (String::Equals(buffer, "see", true)) {
			AudiencePromoteSee(giver, ch);
		}
		else if (String::Equals(buffer, "go", true)) {
			AudiencePromoteGo(giver, ch);
		}
	}
}

void peepwatch_update()
{
	Peepwatch::Update();
}

void peepwatch_handleCommand(const char *giver, const char *buffer)
{
	Peepwatch::HandleCommand(giver, buffer);
}

#endif