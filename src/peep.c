/*****************************************************************************
* Copyright (c) 2014 Ted John
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* This file is part of OpenRCT2.
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include <windows.h>
#include "addresses.h"
#include "audio.h"
#include "news_item.h"
#include "peep.h"
#include "rct2.h"
#include "ride.h"
#include "sprite.h"
#include "window.h"

int peep_get_staff_count()
{
	uint16 sprite_index;
	rct_peep *peep;
	int count = 0;

	sprite_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16);
	while (sprite_index != SPRITE_INDEX_NULL) {
		peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_index].peep);
		sprite_index = peep->next;

		if (peep->type == PEEP_TYPE_STAFF)
			count++;
	}

	return count;
}

/**
 *
 *  rct2: 0x0068F0A9
 */
void peep_update_all()
{
	int i;
	uint16 sprite_index;
	rct_peep* peep;	

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)
		return;

	sprite_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16);
	i = 0;
	while (sprite_index != 0xFFFF) {
		peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_index].peep);
		sprite_index = peep->next;

		if ((i & 0x7F) != (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 0x7F)) {
			RCT2_CALLPROC_X(0x0068FC1E, 0, 0, 0, 0, (int)peep, 0, 0);
		} else {
			RCT2_CALLPROC_X(0x0068F41A, 0, 0, 0, i, (int)peep, 0, 0);
			if (peep->var_08 == 4)
				RCT2_CALLPROC_X(0x0068FC1E, 0, 0, 0, 0, (int)peep, 0, 0);
		}

		i++;
	}
}


/**
 *
 * rct2: 0x0069BF41
 **/
void peep_problem_warnings_update()
{
	rct_peep* peep;
	rct_ride* ride;
	uint16 sprite_idx;
	uint16 guests_in_park = RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16);
	int hunger_counter = 0, lost_counter = 0, noexit_counter = 0, thirst_counter = 0,
		litter_counter = 0, disgust_counter = 0, bathroom_counter = 0 ,vandalism_counter = 0;
	static int warning_throttle[7] = { 0, 0, 0, 0, 0, 0, 0 };

	RCT2_GLOBAL(RCT2_ADDRESS_RIDE_COUNT, sint16) = ride_get_count(); // refactor this to somewhere else


	for (sprite_idx = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16); sprite_idx != SPRITE_INDEX_NULL; sprite_idx = peep->next) {
		peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_idx].peep);

		if (peep->type != PEEP_TYPE_GUEST || peep->var_2A != 0 || peep->thoughts[0].pad_3 > 5)
			continue;

		switch (peep->thoughts[0].type) {
		case PEEP_THOUGHT_TYPE_LOST: //0x10
			lost_counter++;
			break;

		case PEEP_THOUGHT_TYPE_HUNGRY: // 0x14
			if (peep->var_C5 == -1){
				hunger_counter++;
				break;
			}
			ride = &RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[peep->var_C5];
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride->type * 8, uint32) & 0x80000))
				hunger_counter++;
			break;

		case PEEP_THOUGHT_TYPE_THIRSTY:
			if (peep->var_C5 == -1){
				thirst_counter++;
				break;
			}
			ride = &RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[peep->var_C5];
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride->type * 8, uint32) & 0x1000000))
				thirst_counter++;
			break;

		case PEEP_THOUGHT_TYPE_BATHROOM:
			if (peep->var_C5 == -1){
				bathroom_counter++;
				break;
			}
			ride = &RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[peep->var_C5];
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride->type * 8, uint32) & 0x2000000))
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
		news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_ARE_HUNGRY, 20);
	}

	if (warning_throttle[1])
		--warning_throttle[1];
	else if (thirst_counter >= PEEP_THIRST_WARNING_THRESHOLD && thirst_counter >= guests_in_park / 16) {
		warning_throttle[1] = 4;
		news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_ARE_THIRSTY, 21);
	}

	if (warning_throttle[2])
		--warning_throttle[2];
	else if (bathroom_counter >= PEEP_BATHROOM_WARNING_THRESHOLD && bathroom_counter >= guests_in_park / 16) {
		warning_throttle[2] = 4;
		news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_CANT_FIND_BATHROOM, 22);
	}

	if (warning_throttle[3])
		--warning_throttle[3];
	else if (litter_counter >= PEEP_LITTER_WARNING_THRESHOLD && litter_counter >= guests_in_park / 32) {
		warning_throttle[3] = 4;
		news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISLIKE_LITTER, 26);
	}

	if (warning_throttle[4])
		--warning_throttle[4];
	else if (disgust_counter >= PEEP_DISGUST_WARNING_THRESHOLD && disgust_counter >= guests_in_park / 32) {
		warning_throttle[4] = 4;
		news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISGUSTED_BY_PATHS, 31);
	}

	if (warning_throttle[5])
		--warning_throttle[5];
	else if (vandalism_counter >= PEEP_VANDALISM_WARNING_THRESHOLD && vandalism_counter >= guests_in_park / 32) {
		warning_throttle[5] = 4;
		news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISLIKE_VANDALISM, 33);
	}

	if (warning_throttle[6])
		--warning_throttle[6];
	else if (noexit_counter >= PEEP_NOEXIT_WARNING_THRESHOLD) {
		warning_throttle[6] = 4;
		news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_GETTING_LOST_OR_STUCK, 27);
	} else if (lost_counter >= PEEP_LOST_WARNING_THRESHOLD) {
		warning_throttle[6] = 4;
		news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_GETTING_LOST_OR_STUCK, 16);
	}
}

/**
 *
 *  rct2: 0x006BD18A
 */
void peep_update_crowd_noise()
{
	rct_viewport *viewport;
	uint16 sprite_index;
	rct_peep *peep;
	int visiblePeeps;

	if (!(RCT2_GLOBAL(0x009AF284, uint32) & (1 << 0)))
		return;

	if (RCT2_GLOBAL(0x009AF59C, uint8) != 0)
		return;

	if (!(RCT2_GLOBAL(0x009AF59D, uint8) & (1 << 0)))
		return;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
		return;

	viewport = RCT2_GLOBAL(0x00F438A4, rct_viewport*);
	if (viewport == (rct_viewport*)-1)
		return;

	// Count the number of peeps visible
	visiblePeeps = 0;
	sprite_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16);
	while (sprite_index != SPRITE_INDEX_NULL) {
		peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_index].peep);
		sprite_index = peep->next;

		if (peep->var_16 == 0x8000)
			continue;
		if (peep->type != PEEP_TYPE_GUEST)
			continue;
		if (viewport->view_x > peep->var_1A)
			continue;
		if (viewport->view_x + viewport->view_width < peep->var_16)
			continue;
		if (viewport->view_y > peep->var_1C)
			continue;
		if (viewport->view_y + viewport->view_height < peep->var_18)
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
		if (RCT2_GLOBAL(0x009AF5FC, uint32) != 1) {
			RCT2_CALLPROC_1(0x00401A05, int, 2);
			RCT2_GLOBAL(0x009AF5FC, uint32) = 1;
		}
	} else {
		sint32 volume;

		// Formula to scale peeps to dB where peeps [0, 120] scales approximately logarithmically to [-3314, -150] dB/100
		// 207360000 maybe related to DSBVOLUME_MIN which is -10,000 (dB/100)
		volume = 120 - min(visiblePeeps, 120);
		volume = volume * volume * volume * volume;
		volume = (((207360000 - volume) >> viewport->zoom) - 207360000) / 65536 - 150;

		// Check if crowd noise is already playing
		if (RCT2_GLOBAL(0x009AF5FC, uint32) == 1) {
			// Load and play crowd noise
			if (RCT2_CALLFUNC_3(0x0040194E, int, int, char*, int, 2, get_file_path(PATH_ID_CSS2), 0)) {
				RCT2_CALLPROC_5(0x00401999, int, int, int, int, int, 2, 1, volume, 0, 0);
				RCT2_GLOBAL(0x009AF5FC, uint32) = volume;
			}
		} else {
			// Alter crowd noise volume
			if (RCT2_GLOBAL(0x009AF5FC, uint32) != volume) {
				RCT2_CALLPROC_2(0x00401AD3, int, int, 2, volume);
				RCT2_GLOBAL(0x009AF5FC, uint32) = volume;
			}
		}
	}
}

/**
 *
 *  rct2: 0x0069BE9B
 */
void peep_applause()
{
	uint16 sprite_index;
	rct_peep* peep;	

	// For each guest
	sprite_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16);
	while (sprite_index != 0xFFFF) {
		peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_index].peep);
		sprite_index = peep->next;

		if (peep->type != PEEP_TYPE_GUEST)
			continue;
		if (peep->var_2A != 0)
			continue;

		// Release balloon
		if (peep->item_standard_flags & PEEP_ITEM_BALLOON) {
			peep->item_standard_flags &= ~PEEP_ITEM_BALLOON;
			if (peep->x != 0x8000) {
				create_balloon(peep->x, peep->y, peep->z + 9, peep->balloon_colour);
				peep->var_45 |= 8;
				RCT2_CALLPROC_X(0x0069B8CC, 0, 0, 0, 0, (int)peep, 0, 0);
			}
		}

		// Clap
		if ((peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_QUEUING) && peep->var_71 >= 254) {
			peep->var_71 = 26;
			peep->var_72 = 0;
			peep->var_70 = 0;
			RCT2_CALLPROC_X(0x00693B58, 0, 0, 0, 0, (int)peep, 0, 0);
			RCT2_CALLPROC_X(0x006EC473, 0, 0, 0, 0, (int)peep, 0, 0);
		}
	}

	// Play applause noise
	sound_play_panned(44, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) / 2);
}

/**
 *
 *  rct2: 0x0069A05D
 */
rct_peep *peep_generate(int x, int y, int z)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	eax = x;
	ecx = y;
	edx = z;
	RCT2_CALLFUNC_X(0x0069A05D, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return (rct_peep*)esi;
}