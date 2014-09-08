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
#include "sprites.h"
#include "window.h"

static void peep_update(rct_peep *peep);

int peep_get_staff_count()
{
	uint16 spriteIndex;
	rct_peep *peep;
	int count = 0;

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
	int i;
	uint16 spriteIndex;
	rct_peep* peep;	

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)
		return;

	spriteIndex = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16);
	i = 0;
	while (spriteIndex != SPRITE_INDEX_NULL) {
		peep = &(g_sprite_list[spriteIndex].peep);
		spriteIndex = peep->next;

		if ((i & 0x7F) != (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 0x7F)) {
			peep_update(peep);
		} else {
			RCT2_CALLPROC_X(0x0068F41A, 0, 0, 0, i, (int)peep, 0, 0);
			if (peep->linked_list_type_offset == SPRITE_LINKEDLIST_OFFSET_PEEP)
				peep_update(peep);
		}

		i++;
	}
}

/**
 *
 *  rct2: 0x0068FC1E
 */
static void peep_update(rct_peep *peep)
{
	// RCT2_CALLPROC_X(0x0068FC1E, 0, 0, 0, 0, (int)peep, 0, 0); return;

	int i, j;

	if (peep->type == PEEP_TYPE_GUEST) {
		if (peep->var_AD != 255)
			if (++peep->var_AE < 720)
				peep->var_AD = 255;

		// Update thoughts
		i = 0;
		int ebp = 0;
		int edi = -1;
		for (i = 0; i < PEEP_MAX_THOUGHTS; i++) {
			if (peep->thoughts[i].type == PEEP_THOUGHT_TYPE_NONE)
				break;

			if (peep->thoughts[i].var_2 == 1) {
				ebp++;
				if (++peep->thoughts[i].var_3 >= 220) {
					peep->thoughts[i].var_3 = 0;
					peep->thoughts[i].var_2++;
					ebp--;
				}
			} else if (peep->thoughts[i].var_2 > 1) {
				if (++peep->thoughts[i].var_3 > 255) {
					if (++peep->thoughts[i].var_3 >= 28) {
						peep->var_45 |= 1;

						// Clear top thought, push others up
						for (j = i; j < PEEP_MAX_THOUGHTS - 1; j++)
							peep->thoughts[j].type = peep->thoughts[j + 1].type;
						peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;
					}
				}
			} else {
				edi = i;
			}
		}
		if (ebp == 0 && edi != -1) {
			peep->thoughts[edi].var_2 = 1;
			peep->var_45 |= 1;
		}
	}

	// Walking speed logic
	unsigned int stepsToTake = peep->energy;
	if (stepsToTake < 95 && peep->state == PEEP_STATE_QUEUING)
		stepsToTake = 95;
	if ((peep->flags & PEEP_FLAGS_SLOW_WALK) && peep->state != PEEP_STATE_QUEUING)
		stepsToTake /= 2;
	if (peep->var_71 == 255 && (RCT2_GLOBAL((int)peep + 0x29, uint8) & 4)) {
		stepsToTake /= 2;
		if (peep->state == PEEP_STATE_QUEUING)
			stepsToTake += stepsToTake / 2;
	}

	unsigned int carryCheck = peep->var_73 + stepsToTake;
	peep->var_73 = carryCheck;
	if (carryCheck <= 255) {
		// loc_68FD3A
		RCT2_CALLPROC_X(0x0068FD3A, 0, 0, 0, 0, (int)peep, 0, 0);
	} else {
		// loc_68FD2F
		RCT2_CALLPROC_X(0x0068FD2F, 0, 0, 0, 0, (int)peep, 0, 0);
		switch (peep->state) {

		}
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
	uint16 spriteIndex;
	uint16 guests_in_park = RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16);
	int hunger_counter = 0, lost_counter = 0, noexit_counter = 0, thirst_counter = 0,
		litter_counter = 0, disgust_counter = 0, bathroom_counter = 0 ,vandalism_counter = 0;
	static int warning_throttle[7] = { 0, 0, 0, 0, 0, 0, 0 };

	RCT2_GLOBAL(RCT2_ADDRESS_RIDE_COUNT, sint16) = ride_get_count(); // refactor this to somewhere else

	FOR_ALL_GUESTS(spriteIndex, peep) {
		if (peep->var_2A != 0 || peep->thoughts[0].var_2 > 5)
			continue;

		switch (peep->thoughts[0].type) {
		case PEEP_THOUGHT_TYPE_LOST: //0x10
			lost_counter++;
			break;

		case PEEP_THOUGHT_TYPE_HUNGRY: // 0x14
			if (peep->staff_id == -1){
				hunger_counter++;
				break;
			}
			ride = &g_ride_list[peep->staff_id];
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride->type * 8, uint32) & 0x80000))
				hunger_counter++;
			break;

		case PEEP_THOUGHT_TYPE_THIRSTY:
			if (peep->staff_id == -1){
				thirst_counter++;
				break;
			}
			ride = &g_ride_list[peep->staff_id];
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride->type * 8, uint32) & 0x1000000))
				thirst_counter++;
			break;

		case PEEP_THOUGHT_TYPE_BATHROOM:
			if (peep->staff_id == -1){
				bathroom_counter++;
				break;
			}
			ride = &g_ride_list[peep->staff_id];
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
	uint16 spriteIndex;
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

	FOR_ALL_GUESTS(spriteIndex, peep) {
		if (peep->var_16 == 0x8000)
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
			sound_channel_stop(2); //RCT2_CALLPROC_1(0x00401A05, int, 2);
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
			if (sound_channel_load_file2(2, (char*)get_file_path(PATH_ID_CSS2), 0)) {
				sound_channel_play(2, 1, volume, 0, 0);
				RCT2_GLOBAL(0x009AF5FC, uint32) = volume;
			}
		} else {
			// Alter crowd noise volume
			if (RCT2_GLOBAL(0x009AF5FC, uint32) != volume) {
				sound_channel_set_volume(2, volume);//RCT2_CALLPROC_2(0x00401AD3, int, int, 2, volume);
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
	uint16 spriteIndex;
	rct_peep* peep;	

	FOR_ALL_GUESTS(spriteIndex, peep) {
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
	sound_play_panned(SOUND_APPLAUSE, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) / 2);
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

/**
* rct2: 0x00698B0D
* peep.sprite_index (eax)
* thought.type (ebx)
* argument_1 (ecx & ebx)
* argument_2 (edx)
*/
void get_arguments_from_action(rct_peep* peep, uint32 *argument_1, uint32* argument_2){
	rct_ride ride;

	switch (peep->state){
	case 0:
		*argument_1 = peep->var_71 == 0xB ? STR_DROWNING : STR_WALKING;
		*argument_2 = 0;
		break;
	case 1:
		*argument_1 = STR_WALKING;
		*argument_2 = 0;
		break;
	case PEEP_STATE_ON_RIDE:
	case PEEP_STATE_LEAVING_RIDE:
	case PEEP_STATE_ENTERING_RIDE:
		*argument_1 = STR_ON_RIDE;
		ride = g_ride_list[peep->current_ride];
		if (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride.type * 8, uint32) & 0x400000){
			*argument_1 = STR_IN_RIDE;
		}
		*argument_1 |= (ride.name << 16);
		*argument_2 = ride.name_arguments;
		break;
	case PEEP_STATE_BUYING:
		ride = g_ride_list[peep->current_ride];
		*argument_1 = STR_AT_RIDE | (ride.name << 16);
		*argument_2 = ride.name_arguments;
		break;
	case PEEP_STATE_WALKING:
	case 0x14:
		if (peep->staff_id != 0xFF){
			ride = g_ride_list[peep->staff_id];
			*argument_1 = STR_HEADING_FOR | (ride.name << 16);
			*argument_2 = ride.name_arguments;
		}
		else{
			*argument_1 = peep->flags & PEEP_FLAGS_LEAVING_PARK ? STR_LEAVING_PARK : STR_WALKING;
			*argument_2 = 0;
		}
		break;
	case PEEP_STATE_QUEUING_FRONT:
	case PEEP_STATE_QUEUING:
		ride = g_ride_list[peep->current_ride];
		*argument_1 = STR_QUEUING_FOR | (ride.name << 16);
		*argument_2 = ride.name_arguments;
		break;
	case PEEP_STATE_SITTING:
		*argument_1 = STR_SITTING;
		*argument_2 = 0;
		break;
	case PEEP_STATE_WATCHING:
		if (peep->current_ride != 0xFF){
			ride = g_ride_list[peep->current_ride];
			*argument_1 = STR_WATCHING_RIDE | (ride.name << 16);
			*argument_2 = ride.name_arguments;
			if (peep->current_seat & 0x1)
				*argument_1 = STR_WATCHING_CONSTRUCTION_OF | (ride.name << 16);
			else
				*argument_1 = STR_WATCHING_RIDE | (ride.name << 16);
		}
		else{
			*argument_1 = peep->current_seat & 0x1 ? STR_WATCHING_NEW_RIDE_BEING_CONSTRUCTED : STR_LOOKING_AT_SCENERY;
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
		if (peep->pad_2C == 0){
			*argument_1 = STR_WALKING;
			*argument_2 = 0;
		}
		else if (peep->pad_2C == 1){
			*argument_1 = STR_ANSWERING_RADIO_CALL;
			*argument_2 = 0;
		}
		else{
			ride = g_ride_list[peep->current_ride];
			*argument_1 = STR_RESPONDING_TO_RIDE_BREAKDOWN_CALL | (ride.name << 16);
			*argument_2 = ride.name_arguments;
		}
		break;
	case PEEP_STATE_FIXING:
		ride = g_ride_list[peep->current_ride];
		*argument_1 = STR_FIXING_RIDE | (ride.name << 16);
		*argument_2 = ride.name_arguments;
		break;
	case PEEP_STATE_HEADING_TO_INSPECTION:
		ride = g_ride_list[peep->current_ride];
		*argument_1 = STR_HEADING_TO_RIDE_FOR_INSPECTION | (ride.name << 16);
		*argument_2 = ride.name_arguments;
		break;
	case PEEP_STATE_INSPECTING:
		ride = g_ride_list[peep->current_ride];
		*argument_1 = STR_INSPECTING_RIDE | (ride.name << 16);
		*argument_2 = ride.name_arguments;
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
void get_arguments_from_thought(rct_peep_thought thought, uint32* argument_1, uint32* argument_2){
	int esi = 0x9AC86C;

	if ((RCT2_ADDRESS(0x981DB1, uint16)[thought.type] & 0xFF) & 1){
		rct_ride* ride = &g_ride_list[thought.item];
		esi = (int)(&(ride->name));
	}
	else if ((RCT2_ADDRESS(0x981DB1, uint16)[thought.type] & 0xFF) & 2){
		if (thought.item < 0x20){
			RCT2_GLOBAL(0x9AC86C, uint16) = thought.item + STR_ITEM_START;
		}
		else{
			RCT2_GLOBAL(0x9AC86C, uint16) = thought.item + STR_ITEM2_START;
		}
	}
	else if ((RCT2_ADDRESS(0x981DB1, uint16)[thought.type] & 0xFF) & 4){
		if (thought.item < 0x20){
			RCT2_GLOBAL(0x9AC86C, uint16) = thought.item + STR_ITEM_SINGULAR_START;
		}
		else
		{
			RCT2_GLOBAL(0x9AC86C, uint16) = thought.item + STR_ITEM2_SINGULAR_START;
		}
	}
	else{
		esi = 0x9AC864; //No thought?
	}
	*argument_1 = ((thought.type + STR_THOUGHT_START) & 0xFFFF) | (*((uint16*)esi) << 16);
	*argument_2 = *((uint32*)(esi + 2)); //Always 0 apart from on rides?
}

/**
 * rct2: 0x00698827 
 * returns 1 on pickup (CF not set)
 */
int peep_can_be_picked_up(rct_peep* peep){
	return RCT2_ADDRESS(0x982004, uint8)[peep->state] & 1;
}

/**
*  Function split into large and small sprite
*  rct2: 0x00698721
*/
int get_peep_face_sprite_small(rct_peep *peep)
{
	int sprite;
	sprite = SPR_PEEP_SMALL_FACE_ANGRY;

	if (peep->var_F3) return sprite;
	sprite = SPR_PEEP_SMALL_FACE_VERY_VERY_SICK;

	if (peep->nausea > 200) return sprite;
	sprite--; //VERY_SICK

	if (peep->nausea > 170) return sprite;
	sprite--; //SICK

	if (peep->nausea > 140) return sprite;
	sprite = SPR_PEEP_SMALL_FACE_VERY_TIRED;

	if (peep->energy < 46) return sprite;
	sprite--; //TIRED

	if (peep->energy < 70) return sprite;
	sprite = SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY;

	//There are 7 different happiness based faces
	for (int i = 37; peep->happiness >= i; i += 37)
	{
		sprite++;
	}

	return sprite;
}

/**
*  Function split into large and small sprite
*  rct2: 0x00698721
*/
int get_peep_face_sprite_large(rct_peep* peep){
	int sprite;
	sprite = SPR_PEEP_LARGE_FACE_ANGRY;

	if (peep->var_F3) return sprite;
	sprite = SPR_PEEP_LARGE_FACE_VERY_VERY_SICK;

	if (peep->nausea > 200) return sprite;
	sprite = SPR_PEEP_LARGE_FACE_VERY_SICK;

	if (peep->nausea > 170) return sprite;
	sprite = SPR_PEEP_LARGE_FACE_SICK;

	if (peep->nausea > 140) return sprite;
	sprite = SPR_PEEP_LARGE_FACE_VERY_TIRED;

	if (peep->energy < 46) return sprite;
	sprite--; //TIRED

	if (peep->energy < 70) return sprite;
	sprite = SPR_PEEP_LARGE_FACE_VERY_VERY_UNHAPPY;

	//There are 7 different happiness based faces
	for (int i = 37; peep->happiness >= i; i += 37)
	{
		sprite++;
	}

	return sprite;
}
