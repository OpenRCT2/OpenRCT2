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

#include "../addresses.h"
#include "../util/util.h"
#include "../audio/audio.h"
#include "../audio/mixer.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../management/finance.h"
#include "../management/news_item.h"
#include "../config.h"
#include "../openrct2.h"
#include "../ride/ride.h"
#include "../scenario.h"
#include "../sprites.h"
#include "../world/sprite.h"
#include "../world/scenery.h"
#include "../world/footpath.h"
#include "../management/marketing.h"
#include "peep.h"
#include "staff.h"

static void sub_68F41A(rct_peep *peep, int index);
static void peep_update(rct_peep *peep);
static int peep_has_empty_container(rct_peep* peep);
static int peep_has_drink(rct_peep* peep);
static int peep_has_food_standard_flag(rct_peep* peep);
static int peep_has_food_extra_flag(rct_peep* peep);
static int peep_empty_container_standard_flag(rct_peep* peep);
static int peep_empty_container_extra_flag(rct_peep* peep);
static int peep_should_find_bench(rct_peep* peep);
static void peep_stop_purchase_thought(rct_peep* peep, uint8 ride_type);
static void sub_693BAB(rct_peep* peep);
static int sub_693C9E(rct_peep *peep);
static void peep_spend_money(rct_peep *peep, money16 *peep_expend_type, money32 amount);
static void peep_set_has_ridden(rct_peep *peep, int rideIndex);
static bool peep_has_ridden(rct_peep *peep, int rideIndex);
static void peep_set_has_ridden_ride_type(rct_peep *peep, int rideType);
static bool peep_has_ridden_ride_type(rct_peep *peep, int rideType);
static void peep_on_enter_or_exit_ride(rct_peep *peep, int rideIndex, int flags);
static bool sub_69AF1E(rct_peep *peep, int rideIndex, int shopItem, money32 price);
static bool peep_should_use_cash_machine(rct_peep *peep, int rideIndex);
static bool sub_6960AB(rct_peep *peep, int rideIndex, int dh, int bp);
static void sub_69A98C(rct_peep *peep);
static void sub_68FD3A(rct_peep *peep);
static bool sub_690B99(rct_peep *peep, int edge, uint8 *rideToView, uint8 *rideSeatToView);
static int peep_get_height_on_slope(rct_peep *peep, int x, int y);
static void peep_pick_ride_to_go_on(rct_peep *peep);
static void peep_head_for_nearest_ride_type(rct_peep *peep, int rideType);
static void peep_head_for_nearest_ride_with_flags(rct_peep *peep, int rideTypeFlags);
static void peep_give_real_name(rct_peep *peep);
static int guest_surface_path_finding(rct_peep* peep);

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
	"ANDY HINE",
	"ELISSA WHITE",
	"DAVID ELLIS"
};

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
			sub_68F41A(peep, i);
			if (peep->linked_list_type_offset == SPRITE_LINKEDLIST_OFFSET_PEEP)
				peep_update(peep);
		}

		i++;
	}
}

/* rct2: 0x0069BC9A */
static uint8 peep_assess_surroundings(sint16 center_x, sint16 center_y, sint16 center_z){
	if ((map_element_height(center_x, center_y) & 0xFFFF) > center_z)
		return 0;

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
					if ((mapElement->properties.path.additions & 0xF) == 0)
						break;

					scenery = g_pathBitSceneryEntries[(mapElement->properties.path.additions & 0x0F) - 1];
					if (mapElement->properties.path.additions & (1 << 7))
						break;

					if (scenery->path_bit.var_06 & 
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
					ride = GET_RIDE(mapElement->properties.track.ride_index);
					if (ride->type == RIDE_TYPE_MERRY_GO_ROUND &&
						ride->music_tune_id != 0xFF){
						nearby_music |= 1;
						break;
					}

					if (ride->music_tune_id == MUSIC_STYLE_ORGAN){
						nearby_music |= 1;
						break;
					}

					if (ride->type == RIDE_TYPE_DODGEMS &&
						ride->music_tune_id != 0xFF){
						// Dodgems drown out music?
						nearby_music |= 2;
					}
					break;
				}
			} while (!map_element_is_last_for_tile(mapElement++));
		}
	}


	short num_litter;

	num_litter = 0;
	rct_litter* litter;
	for (uint16 sprite_idx = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_LITTER, uint16); sprite_idx != SPRITE_INDEX_NULL; sprite_idx = litter->next) {
		litter = &(g_sprite_list[sprite_idx].litter);

		sint16 dist_x = abs(litter->x - center_x);
		sint16 dist_y = abs(litter->y - center_y);
		if (max(dist_x, dist_y) <= 160){
			num_rubbish++;
		}
	}

	if (num_fountains >= 5){
		if (num_rubbish < 20)
			return 3;
	}

	if (num_scenery >= 40){
		if (num_rubbish < 8)
			return 1;
	}

	if (nearby_music == 1){
		if (num_rubbish < 20)
			return 4;
	}

	if (num_rubbish < 2)
		return 2;

	return 0;
}

/* rct2: 0x0068F9A9*/
static void peep_update_hunger(rct_peep *peep){
	if (peep->hunger >= 3){
		peep->hunger -= 2;
		peep->energy_growth_rate = min(peep->energy_growth_rate + 2, 255);
		peep->bathroom = min(peep->bathroom + 1, 255);
	}
}

/* rct2: 0x0068F93E */
static void peep_leave_park(rct_peep* peep){
	peep->guest_heading_to_ride_id = 0xFF;
	if (peep->flags & PEEP_FLAGS_LEAVING_PARK){
		if (peep->var_C6 < 60){
			return;
		}
	}
	else{
		peep->var_C6 = 254;
		peep->flags |= PEEP_FLAGS_LEAVING_PARK;
		peep->flags &= ~PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
	}

	peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_GO_HOME, 0xFF);

	rct_window* w = window_find_by_number(WC_PEEP, peep->sprite_index);
	if (w != NULL) window_event_invalidate_call(w);
	window_invalidate_by_number(WC_PEEP, peep->sprite_index);
}

/* rct2: 0x0068f8CD*/
static void sub_68F8CD(rct_peep *peep){
	if (peep->energy_growth_rate >= 33)
		peep->energy_growth_rate -= 2;

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TEMPERATURE, uint8) >= 21){
		if (peep->thirst >= 5)
			peep->thirst--;
	}

	if (peep->var_2A != 0)
		return;
	
	if (!(peep->flags & PEEP_FLAGS_LEAVING_PARK)){
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)){
			if (peep->energy >= 55)
				return;
			
			if (peep->happiness >= 45)
				return;
			
			if (peep->cash_in_pocket >= MONEY(5, 00))
				return;
		}
		else{
			if (peep->energy >= 70)
				return;

			if (peep->happiness >= 60)
				return;
		}			
		if ((scenario_rand() & 0xFFFF) > 3276)
			return;
	}
	peep_leave_park(peep);
}

/**
 *
 *  rct2: 0x0068F41A
 */
static void sub_68F41A(rct_peep *peep, int index)
{
	if (peep->type == PEEP_TYPE_STAFF){
		if (peep->staff_type != STAFF_TYPE_SECURITY)
			return;

		uint8 sprite_type = 23;
		if (peep->state != PEEP_STATE_PATROLLING)
			sprite_type = 3;

		if (peep->sprite_type == sprite_type)
			return;

		peep->sprite_type = sprite_type;
		peep->action_sprite_image_offset = 0;
		peep->no_action_frame_no = 0;
		if (peep->action < PEEP_ACTION_NONE_1)
			peep->action = PEEP_ACTION_NONE_2;

		peep->flags &= ~PEEP_FLAGS_SLOW_WALK;
		if (RCT2_ADDRESS(0x00982134, uint8)[sprite_type] & 1){
			peep->flags |= PEEP_FLAGS_SLOW_WALK;
		}

		peep->action_sprite_type = 0xFF;
		sub_693B58(peep);
		return;
	}

	if ((index & 0x1FF) == (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 0x1FF)){
		//RCT2_GLOBAL(0x00F1EDFE, uint32) = index; not needed all cases accounted for

		if (peep->flags & PEEP_FLAGS_CROWDED){
			uint8 thought_type = RCT2_ADDRESS(0x009823AC, uint8)[scenario_rand() & 0xF];
			if (thought_type != PEEP_THOUGHT_TYPE_NONE){
				peep_insert_new_thought(peep, thought_type, 0xFF);
			}
		}

		if (peep->flags & PEEP_FLAGS_EXPLODE && peep->x != (sint16)0x8000){
			sound_play_panned(SOUND_CRASH, 0x8001, peep->x, peep->y, peep->z);

			sprite_misc_3_create(peep->x, peep->y, peep->z + 16);
			sprite_misc_5_create(peep->x, peep->y, peep->z + 16);

			peep_remove(peep);
			return;
		}

		if (peep->flags & PEEP_FLAGS_HUNGER){
			if (peep->hunger >= 15)peep->hunger -= 15;
		}

		if (peep->flags & PEEP_FLAGS_BATHROOM){
			if (peep->bathroom <= 180)peep->bathroom += 50;
		}

		if (peep->flags & PEEP_FLAGS_HAPPINESS){
			peep->happiness_growth_rate = 5;
		}

		if (peep->flags & PEEP_FLAGS_NAUSEA){
			peep->nausea_growth_rate = 200;
			if (peep->nausea <= 130)peep->nausea = 130;
		}

		if (peep->var_F3 != 0)
			peep->var_F3--;

		if (peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_SITTING){
			peep->var_F2++;
			if (peep->var_F2 >= 18){
				peep->var_F2 = 0;
				if (peep->x != (sint16)0x8000){

					uint8 bl = peep_assess_surroundings(peep->x & 0xFFE0, peep->y & 0xFFE0, peep->z);

					if (bl != 0){
						peep->happiness_growth_rate = min(255, peep->happiness_growth_rate + 45);

						switch (bl){
						case 1:
							peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_SCENERY, 0xFF);
							break;
						case 2:
							peep_insert_new_thought(peep, PEEP_THOUGHT_VERY_CLEAN, 0xFF);
							break;
						case 3:
							peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_FOUNTAINS, 0xFF);
							break;
						default:
							peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_MUSIC, 0xFF);
							break;
						}
					}
				}
			}
		}

		peep_update_sprite_type(peep);

		if (peep->state == PEEP_STATE_ON_RIDE || peep->state == PEEP_STATE_ENTERING_RIDE){
			peep->time_on_ride = min(255, peep->time_on_ride + 1);

			if (peep->flags & PEEP_FLAGS_WOW){
				peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_WOW2, 0xFF);
			}

			if (peep->time_on_ride > 15){
				peep->happiness_growth_rate = min(0, peep->happiness_growth_rate - 5);

				if (peep->time_on_ride > 22){
					rct_ride* ride = GET_RIDE(peep->current_ride);

					uint8 thought_type = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE) ?
						PEEP_THOUGHT_TYPE_GET_OUT :
						PEEP_THOUGHT_TYPE_GET_OFF;

					peep_insert_new_thought(peep, thought_type, peep->current_ride);
				}
			}
		}
		
		if (peep->state == PEEP_STATE_WALKING &&
			peep->var_2A == 0 &&
			!(peep->flags & PEEP_FLAGS_LEAVING_PARK) &&
			peep->no_of_rides == 0 &&
			peep->guest_heading_to_ride_id == 0xFF){

			uint32 time_duration = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32) - peep->time_in_park;
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
		
		if ((scenario_rand() & 0xFFFF) <= (peep->item_standard_flags & PEEP_ITEM_MAP ? 8192U : 2184U)){
			peep_pick_ride_to_go_on(peep);
		}

		if ((index & 0x3FF) == (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 0x3FF)){

			if (peep->var_2A == 0 &&
				(peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_SITTING)){

				uint8 num_thoughts = 0;
				uint8 possible_thoughts[5] = { 0 };

				if (peep->flags & PEEP_FLAGS_LEAVING_PARK){
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

					// Not sure why the happiness check is like that seems wrong to me
					if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY) &&
						peep->cash_in_pocket <= MONEY(9, 00) &&
						peep->happiness >= 105 &&
						peep->happiness >= 70){
						possible_thoughts[num_thoughts++] = PEEP_THOUGHT_RUNNING_OUT;
					}
				}

				if (num_thoughts != 0){
					uint8 chosen_thought = possible_thoughts[scenario_rand() % num_thoughts];

					peep_insert_new_thought(peep, chosen_thought, 0xFF);

					switch (chosen_thought){
					case PEEP_THOUGHT_TYPE_HUNGRY:
						peep_head_for_nearest_ride_with_flags(peep, 0x00800000);
						break;
					case PEEP_THOUGHT_TYPE_THIRSTY:
						peep_head_for_nearest_ride_with_flags(peep, 0x01000000);
						break;
					case PEEP_THOUGHT_TYPE_BATHROOM:
						peep_head_for_nearest_ride_with_flags(peep, 0x00200000);
						break;
					case PEEP_THOUGHT_RUNNING_OUT:
						peep_head_for_nearest_ride_type(peep, RIDE_TYPE_CASH_MACHINE);
						break;
					}
				}
			}
		}
		else{
			if (peep->nausea >= 140){
				uint8 thought = PEEP_THOUGHT_TYPE_SICK;
				if (peep->nausea >= 200){
					thought = PEEP_THOUGHT_TYPE_VERY_SICK;
					peep_head_for_nearest_ride_type(peep, RIDE_TYPE_FIRST_AID);
				}
				peep_insert_new_thought(peep, thought, 0xFF);
			}
		}


		switch (peep->state){
		case PEEP_STATE_WALKING:
		case PEEP_STATE_LEAVING_PARK:
		case PEEP_STATE_ENTERING_PARK:
			sub_68F8CD(peep);
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
				rct_map_element* mapElement = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);
				uint8 found = 0;
				do {
					if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
						continue;
					if (mapElement->base_height != peep->next_z)
						continue;

					uint8 additions = mapElement->properties.path.additions & 0xF;
					if (additions != 0 && mapElement->properties.path.additions & (1 << 7)){
						rct_scenery_entry *sceneryEntry = g_pathBitSceneryEntries[additions - 1];
						if (sceneryEntry->path_bit.var_06 & (1 << 8)){
							found = 1;
						}
					}
					break;
				} while (!map_element_is_last_for_tile(mapElement++));

				if (found){
					if (peep->happiness_growth_rate < 90)
						peep->happiness_growth_rate = 90;

					if (peep->happiness_growth_rate < 165)
						peep->happiness_growth_rate += 2;
				}
				else{
					peep->happiness_growth_rate = max(peep->happiness_growth_rate - 4, 0);
				}
			}
			peep_update_hunger(peep);
			break;
		case PEEP_STATE_ENTERING_RIDE:
			if (peep->sub_state == 17 ||
				peep->sub_state == 15){
				sub_68F8CD(peep);
			}
			peep_update_hunger(peep);
			break;
		}

	loc_68F9F3:
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

			if ((scenario_rand() & 0xFF) <= (uint8)((peep->nausea - 128) / 2)){
				if (peep->action >= PEEP_ACTION_NONE_1){
					peep->action = PEEP_ACTION_THROW_UP;
					peep->action_frame = 0;
					peep->action_sprite_image_offset = 0;
					sub_693B58(peep);
					invalidate_sprite((rct_sprite*)peep);
				}
			}
		}
	}		
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
			int chosen_food = bitscanforward(peep_has_food_standard_flag(peep));
			if (chosen_food != -1){
				peep->item_standard_flags &= ~(1 << chosen_food);

				uint8 discard_container = RCT2_ADDRESS(0x00982326, uint8)[chosen_food];
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
					uint8 discard_container = RCT2_ADDRESS(0x00982342, uint8)[chosen_food];
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

/* some sort of check to see if peep is connected to the ground?? */
int sub_68F3AE(rct_peep* peep){
	peep->var_C4++;
	if ((peep->var_C4 & 0xF) != (peep->sprite_index & 0xF))return 1;

	rct_map_element* map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

	uint8 map_type = MAP_ELEMENT_TYPE_PATH;
	if (peep->next_var_29 & ((1 << 4) | (1 << 3))){
		map_type = MAP_ELEMENT_TYPE_SURFACE;
	}

	int z = peep->next_z;

	do {
		if (map_element_get_type(map_element) == map_type){
			if (z == map_element->base_height)return 1;
		}
	} while (!map_element_is_last_for_tile(map_element++));
	
	peep_decrement_num_riders(peep);
	peep->state = PEEP_STATE_FALLING;
	peep_window_state_update(peep);
	return 0;
}

void sub_693B58(rct_peep* peep){
	int ebx;
	if (peep->action >= 0xFE){
		ebx = RCT2_ADDRESS(0x981D8C, uint8)[peep->var_6D];
	}
	else{
		ebx = RCT2_ADDRESS(0x981D8F, uint8)[peep->action];
	}
	if (ebx == peep->action_sprite_type)return;

	invalidate_sprite((rct_sprite*)peep);
	peep->action_sprite_type = ebx;

	uint8* edx = RCT2_ADDRESS(0x98270C, uint8*)[peep->sprite_type * 2];
	peep->sprite_width = edx[ebx * 4];
	peep->sprite_height_negative = edx[ebx * 4 + 1];
	peep->sprite_height_positive = edx[ebx * 4 + 2];
	// This is pointless as nothing will have changed.
	invalidate_sprite((rct_sprite*)peep);
}

/* 0x00693BE5 */
void sub_693BE5(rct_peep* peep, uint8 al){
	if (al == peep->var_6D)return;

	peep->var_6D = al;

	// If NONE_1 or NONE_2
	if (peep->action >= PEEP_ACTION_NONE_1){
		peep->action_sprite_image_offset = 0;
	}
	sub_693B58(peep);
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
	sub_693BE5(peep, 0);
}

static void peep_state_reset(rct_peep* peep){
	peep_decrement_num_riders(peep);
	peep->state = PEEP_STATE_1;
	peep_window_state_update(peep);

	sub_693BE5(peep, 0);
}

/* rct2: 0x69C308 
 * Check if lost.
 */
void peep_check_if_lost(rct_peep* peep){
	if (!(peep->flags & PEEP_FLAGS_LOST)){
		if (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_COUNT, uint16) < 2)return;
		peep->flags ^= PEEP_FLAGS_21;

		if (!(peep->flags & PEEP_FLAGS_21)) return;

		peep->var_F4++;
		if (peep->var_F4 != 254)return;
		peep->var_F4 = 230;
	}
	peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_LOST, 0xFF);

	peep->happiness_growth_rate = max(peep->happiness_growth_rate - 30, 0);
}

/* rct2: 0x69C26B
* Check if cant find ride.
*/
void peep_check_cant_find_ride(rct_peep* peep){
	if (peep->guest_heading_to_ride_id == 0xFF) return;

	if (peep->var_C6 == 30 || peep->var_C6 == 60){
		peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CANT_FIND, peep->guest_heading_to_ride_id);

		peep->happiness_growth_rate = max(peep->happiness_growth_rate - 30, 0);
	}

	peep->var_C6--;
	if (peep->var_C6 != 0)return;

	peep->guest_heading_to_ride_id = 0xFF;
	rct_window* w = window_find_by_number(WC_PEEP, peep->sprite_index);

	if (w){
		window_event_invalidate_call(w);
	}

	widget_invalidate_by_number(WC_PEEP, peep->sprite_index, 12);
}

/* rct2: 0x69C2D0
* Check if cant find exit.
*/
void peep_check_cant_find_exit(rct_peep* peep){
	if (!(peep->flags & PEEP_FLAGS_LEAVING_PARK))return;

	if (peep->var_C6 == 1){
		peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CANT_FIND_EXIT, 0xFF);

		peep->happiness_growth_rate = max(peep->happiness_growth_rate - 30, 0);
	}

	if (--peep->var_C6 == 0) peep->var_C6 = 90;
}

/* rct2: 0x6939EB 
 * Also used to move peeps to the correct position to
 * start an action. Returns 1 if the correct destination 
 * has not yet been reached. xy_distance is how close the
 * peep is to the target.
 */
int peep_update_action(sint16* x, sint16* y, sint16* xy_distance, rct_peep* peep){
	RCT2_GLOBAL(0xF1AEF0, uint8) = peep->action_sprite_image_offset;
	if (peep->action == 0xFE){
		peep->action = 0xFF;
	}

	*x = peep->x - peep->destination_x;
	*y = peep->y - peep->destination_y;

	int x_delta = abs(*x);
	int y_delta = abs(*y);
	
	*xy_distance = x_delta + y_delta;
	
	if (peep->action >= 0xFE){
		if (*xy_distance <= peep->destination_tolerence){
			return 0;
		}
		int direction = 0;
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
		*x = peep->x + RCT2_ADDRESS(0x981D7C, uint16)[direction / 4];
		*y = peep->y + RCT2_ADDRESS(0x981D7E, uint16)[direction / 4];
		peep->no_action_frame_no++;
		uint32* edi = RCT2_ADDRESS(0x982708, uint32*)[peep->sprite_type * 2];
		uint8* _edi = (uint8*)(edi[peep->action_sprite_type * 2 + 1]);
		if (peep->no_action_frame_no >= *_edi){
			peep->no_action_frame_no = 0;
		}
		peep->action_sprite_image_offset = _edi[peep->no_action_frame_no + 1];
		return 1;
	}
	
	uint32* edi = RCT2_ADDRESS(0x982708, uint32*)[peep->sprite_type * 2];
	uint8* _edi = (uint8*)(edi[peep->action_sprite_type * 2 + 1]);
	peep->action_frame++;
	int ebx = _edi[peep->action_frame + 1];

	// If last frame of action
	if (ebx == 0xFF){
		peep->action_sprite_image_offset = 0;
		peep->action = 0xFF;
		sub_693B58(peep);
		invalidate_sprite((rct_sprite*)peep);
		*x = peep->x;
		*y = peep->y;
		return 1;
	}
	peep->action_sprite_image_offset = ebx;

	// If not throwing up and not at the frame where sick appears.
	if (peep->action != PEEP_ACTION_THROW_UP || peep->action_frame != 15){
		invalidate_sprite((rct_sprite*)peep);
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
	litter_create(peep->x, peep->y, peep->z, peep->sprite_direction, peep->sprite_index & 1);

	int sound_id = SOUND_COUGH_1 + (scenario_rand() & 3);
	sound_play_panned(sound_id, 0x8001, peep->x, peep->y, peep->z);

	invalidate_sprite((rct_sprite*)peep);
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

		rct_ride* ride = GET_RIDE(peep->current_ride);
		ride->num_riders--;
		ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
	}
}

/* Part of 0x0069B8CC rct2:0x0069BC31 */
void set_sprite_type(rct_peep* peep, uint8 type){
	if (peep->sprite_type == type)return;

	peep->sprite_type = type;
	peep->action_sprite_image_offset = 0;
	peep->no_action_frame_no = 0;
	
	if (peep->action >= PEEP_ACTION_NONE_1)
		peep->action = PEEP_ACTION_NONE_2;

	peep->flags &= ~PEEP_FLAGS_SLOW_WALK;
	if (RCT2_ADDRESS(0x00982134, uint8)[type] & 1){
		peep->flags |= PEEP_FLAGS_SLOW_WALK;
	}

	peep->action_sprite_type = 0xFF;
	sub_693B58(peep);

	if (peep->state == PEEP_STATE_SITTING){
		peep->action = PEEP_ACTION_NONE_1;
		peep->var_6F = 7;
		sub_693BAB(peep);
	}
	if (peep->state == PEEP_STATE_WATCHING){
		peep->action = PEEP_ACTION_NONE_1;
		peep->var_6F = 2;
		sub_693BAB(peep);
	}
}

typedef struct{
	uint8 type; // 0 for standard, 1 for extra
	uint32 item; // And this with the relevant flags
	uint8 sprite_type;
} item_pref;

item_pref item_order_preference[] = {
		{ 0, PEEP_ITEM_ICE_CREAM, 15 },
		{ 0, PEEP_ITEM_FRIES, 16},
		{ 0, PEEP_ITEM_PIZZA, 22 },
		{ 0, PEEP_ITEM_BURGER, 17 },
		{ 0, PEEP_ITEM_DRINK, 18 },
		{ 0, PEEP_ITEM_COFFEE, 35 },
		{ 0, PEEP_ITEM_CHICKEN, 34 },
		{ 0, PEEP_ITEM_LEMONADE, 37 },
		{ 0, PEEP_ITEM_COTTON_CANDY, 20 },
		{ 0, PEEP_ITEM_POPCORN, 22 },
		{ 0, PEEP_ITEM_HOT_DOG, 31 },
		{ 0, PEEP_ITEM_TENTACLE, 32 },
		{ 0, PEEP_ITEM_CANDY_APPLE, 33 },
		{ 0, PEEP_ITEM_DONUT, 34 },
		{ 1, PEEP_ITEM_PRETZEL, 39 },
		{ 1, PEEP_ITEM_COOKIE, 39 },
		{ 1, PEEP_ITEM_CHOCOLATE, 35 },
		{ 1, PEEP_ITEM_ICED_TEA, 35 },
		{ 1, PEEP_ITEM_FUNNEL_CAKE, 43 },
		{ 1, PEEP_ITEM_BEEF_NOODLES, 44 },
		{ 1, PEEP_ITEM_FRIED_RICE_NOODLES, 44 },
		{ 1, PEEP_ITEM_WONTON_SOUP, 46 },
		{ 1, PEEP_ITEM_MEATBALL_SOUP, 46 },
		{ 1, PEEP_ITEM_FRUIT_JUICE, 43 },
		{ 1, PEEP_ITEM_SOYBEAN_MILK, 41 },
		{ 1, PEEP_ITEM_SU_JONGKWA, 41 },
		{ 1, PEEP_ITEM_SUB_SANDWICH, 47 },
		{ 1, PEEP_ITEM_ROAST_SAUSAGE, 45 },
		{ 0, PEEP_ITEM_BALLOON, 19 },
		{ 0, PEEP_ITEM_HAT, 30},
		{ 1, PEEP_ITEM_SUNGLASSES, 40},
		{ 0xFF, 0xFFFFFFFF, 0xFF}
};

/* rct2: 0x0069B8CC */
void peep_update_sprite_type(rct_peep* peep){
	if (peep->sprite_type == 19 &&
		(scenario_rand() & 0xFFFF) <= 327){
		uint8 bl = 0;

		if ((scenario_rand() & 0xFFFF) <= 13107 &&
			peep->x != SPRITE_LOCATION_NULL){
				
			bl = 1;
			sound_play_panned(SOUND_BALLOON_POP, 0x8001, peep->x, peep->y, peep->z);
		}

		if (peep->x != SPRITE_LOCATION_NULL){
			create_balloon(peep->x, peep->y, peep->z + 9, peep->balloon_colour, bl);
		}

		peep->item_standard_flags &= ~PEEP_ITEM_BALLOON;

		peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RAIN_LEVEL, uint8) != 0 &&
		peep->item_standard_flags & PEEP_ITEM_UMBRELLA &&
		peep->x != SPRITE_LOCATION_NULL){
		int x = peep->x & 0xFFE0;
		int y = peep->y & 0xFFE0;

		if (x < 0x1FFF && y < 0x1FFF){
			rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);
			while (1) {
				if ((peep->z / 32) < map_element->base_height)break;

				if (map_element_is_last_for_tile(map_element)){
					set_sprite_type(peep, 21);
					return;
				}
				map_element++;
			}
		}	
	}

	for (item_pref* item_pref = item_order_preference; item_pref->type != 0xFF; item_pref++){
		if (item_pref->type == 0){
			if (peep->item_standard_flags & item_pref->item){
				set_sprite_type(peep, item_pref->sprite_type);
				return;
			}
		}
		else{
			if (peep->item_extra_flags & item_pref->item){
				set_sprite_type(peep, item_pref->sprite_type);
				return;
			}
		}
	}

	if (peep->state == PEEP_STATE_WATCHING &&
		peep->standing_flags & (1<<1)){
		set_sprite_type(peep, 38);
		return;
	}
	
	if (peep->nausea > 170){
		set_sprite_type(peep, 28);
		return;
	}
	
	if (peep->nausea > 140){
		set_sprite_type(peep, 27);
		return;
	}
	
	if (peep->energy <= 64 &&
		peep->happiness < 128){
		set_sprite_type(peep, 26);
		return;
	}
	
	if (peep->energy <= 80 &&
		peep->happiness < 128){
		set_sprite_type(peep, 25);
		return;
	}
	
	if (peep->bathroom > 220){
		set_sprite_type(peep, 29);
		return;
	}
	
	set_sprite_type(peep, 0);
}

/**
 *  rct2: 0x0069A42F
 * Call after changing a peeps state to insure that
 * all relevant windows update. Note also increase ride
 * count if on/entering a ride.
 */
void peep_window_state_update(rct_peep* peep){
	
	rct_window* w = window_find_by_number(WC_PEEP, peep->sprite_index);
	if (w != NULL)
		window_event_invalidate_call(w);

	if (peep->type == PEEP_TYPE_GUEST){
		// Update action label
		widget_invalidate_by_number(WC_PEEP, peep->sprite_index, 12);

		if (peep->state == PEEP_STATE_ON_RIDE || peep->state == PEEP_STATE_ENTERING_RIDE){
			rct_ride* ride = GET_RIDE(peep->current_ride);
			ride->num_riders++;
			ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
		}

		window_invalidate_by_class(WC_GUEST_LIST);
	}
	else{
		// Update action label
		widget_invalidate_by_number(WC_PEEP, peep->sprite_index, 9);
		window_invalidate_by_class(WC_STAFF_LIST);
	}
}

/* rct2: 0x0069A535*/
void peep_sprite_remove(rct_peep* peep){
	remove_peep_from_ride(peep);
	invalidate_sprite((rct_sprite*)peep);

	window_close_by_number(WC_PEEP, peep->sprite_index);

	window_close_by_number(WC_FIRE_PROMPT, peep->sprite_identifier);

	if (peep->type == PEEP_TYPE_GUEST){
		window_invalidate_by_class(WC_GUEST_LIST);

		news_item_disable_news(NEWS_ITEM_PEEP_ON_RIDE, peep->sprite_index);
	}
	else{
		window_invalidate_by_class(WC_STAFF_LIST);

		RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[peep->staff_id] = 0;
		peep->type = 0xFF;
		staff_update_greyed_patrol_areas();
		peep->type = PEEP_TYPE_STAFF;

		news_item_disable_news(NEWS_ITEM_PEEP, peep->sprite_index);
	}
	sprite_remove((rct_sprite*)peep);
}

/** New function removes peep from 
 * park existance. Works with staff.
 */
void peep_remove(rct_peep* peep){
	if (peep->type == PEEP_TYPE_GUEST){
		if (peep->var_2A == 0){
			RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16)--;
			RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) |= BTM_TB_DIRTY_FLAG_PEEP_COUNT;
		}
		if (peep->state == PEEP_STATE_ENTERING_PARK){
			RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_HEADING_FOR_PARK, uint16)--;
		}
	}
	peep_sprite_remove(peep);
}

/**
 * rct2: 0x690028
 * Falling and its subset drowning
 */
void peep_update_falling(rct_peep* peep){
	if (peep->action == PEEP_ACTION_DROWNING){
		// Check to see if we are ready to drown.
		sint16 x, y, xy_distance;

		peep_update_action(&x, &y, &xy_distance, peep);
		if (peep->action == PEEP_ACTION_DROWNING) return;
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & 0x80000)){
			RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
			RCT2_GLOBAL(0x13CE954, uint32) = peep->id;
			news_item_add_to_queue(NEWS_ITEM_BLANK, 2347, peep->x | (peep->y << 16));
		}
		RCT2_GLOBAL(0x135882E, uint16) += 25;
		if (RCT2_GLOBAL(0x135882E, uint16) > 1000){
			RCT2_GLOBAL(0x135882E, uint16) = 1000;
		}
		peep_remove(peep);
		return;
	}

	// If not drowning then falling. Note: peeps 'fall' after leaving a ride/enter the park.
	rct_map_element *map_element = map_get_first_element_at(peep->x / 32, peep->y / 32);
	rct_map_element *saved_map = NULL;
	int saved_height = 0;

	do {
		// If a path check if we are on it
		if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_PATH){
			int height = map_height_from_slope(peep->x, peep->y, map_element->properties.surface.slope)
				+ map_element->base_height * 8;

			if (height < peep->z - 1 || height > peep->z + 4) continue;

			saved_height = height;
			saved_map = map_element;
			break;
		} // If a surface get the height and see if we are on it
		else if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_SURFACE){
			// If the surface is water check to see if we could be drowning
			if (map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK){
				int height = (map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK) * 16;

				if (height - 4 >= peep->z && height < peep->z + 20){
					// Looks like we are drowning!
					invalidate_sprite((rct_sprite*)peep);
					sprite_move(peep->x, peep->y, height, (rct_sprite*)peep);
					// Drop balloon if held
					if (peep->item_standard_flags & PEEP_ITEM_BALLOON){
						peep->item_standard_flags &= ~PEEP_ITEM_BALLOON;

						if (peep->sprite_type == 19 && peep->x != (sint16)0x8000){
							create_balloon(peep->x, peep->y, height, peep->balloon_colour, 0);
							peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
							peep_update_sprite_type(peep);
						}
					}

					peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_DROWNING, -1);

					peep->action = PEEP_ACTION_DROWNING;
					peep->action_frame = 0;
					peep->action_sprite_image_offset = 0;

					sub_693B58(peep);
					invalidate_sprite((rct_sprite*)peep);
					peep_window_state_update(peep);
					return;
				}
			}
			int map_height = map_element_height(0xFFFF & peep->x, 0xFFFF & peep->y) & 0xFFFF;
			if (map_height < peep->z || map_height - 4 > peep->z) continue;
			saved_height = map_height;
			saved_map = map_element;
		} // If not a path or surface go see next element
		else continue;
	} while (!map_element_is_last_for_tile(map_element++));

	// This will be null if peep is falling
	if (saved_map == NULL){
		invalidate_sprite((rct_sprite*)peep);
		if (peep->z <= 1){
			// Remove peep if it has gone to the void
			peep_remove(peep);
			return;
		}
		sprite_move(peep->x, peep->y, peep->z - 2, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}
	
	invalidate_sprite((rct_sprite*)peep);
	sprite_move(peep->x, peep->y, saved_height, (rct_sprite*)peep);
	invalidate_sprite((rct_sprite*)peep);

	peep->next_x = peep->x & 0xFFE0;
	peep->next_y = peep->y & 0xFFE0;
	peep->next_z = saved_map->base_height;

	int edx = saved_map->properties.surface.slope & 0x7;
	if (map_element_get_type(saved_map) != MAP_ELEMENT_TYPE_PATH){
		edx = 8;
	}
	peep->next_var_29 = edx;
	peep_decrement_num_riders(peep);
	peep->state = PEEP_STATE_1;
	peep_window_state_update(peep);
}

/**
 * rct2: 0x00691677
 */
void peep_try_get_up_from_sitting(rct_peep* peep){
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
	sub_693B58(peep);
}

/**
 * rct2: 0x0069152B 
 */
void peep_update_sitting(rct_peep* peep){
	if (peep->sub_state == 0){
		if (!sub_68F3AE(peep))return;
		//691541

		sub_693C9E(peep);
		if (!(RCT2_GLOBAL(0xF1EE18, uint16) & 1))return;

		int ebx = peep->var_37 & 0x7;
		int x = (peep->x & 0xFFE0) + RCT2_ADDRESS(0x981F2C, uint16)[ebx * 2];
		int y = (peep->y & 0xFFE0) + RCT2_ADDRESS(0x981F2E, uint16)[ebx * 2];
		int z = peep->z;		
		
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, z, (rct_sprite*)peep);

		peep->sprite_direction = ((peep->var_37 + 2) & 3) * 8;
		invalidate_sprite((rct_sprite*)peep);
		peep->action = 254;
		peep->var_6F = 7;
		sub_693BAB(peep);

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
		
		if ((peep->flags & PEEP_FLAGS_LEAVING_PARK)){
			peep_decrement_num_riders(peep);
			peep->state = PEEP_STATE_WALKING;
			peep_window_state_update(peep);

			// Set destination to the center of the tile
			peep->destination_x = (peep->x & 0xFFE0) + 16;
			peep->destination_y = (peep->y & 0xFFE0) + 16;
			peep->destination_tolerence = 5;
			sub_693B58(peep);
			return;
		}
		
		if (peep->sprite_type == 0x15){
			peep_try_get_up_from_sitting(peep);
			return;
		}

		if (peep_has_food(peep)){
			if ((scenario_rand() & 0xFFFF) > 1310){
				peep_try_get_up_from_sitting(peep);
				return;
			}
			peep->action = PEEP_ACTION_SITTING_EAT_FOOD;
			peep->action_frame = 0;
			peep->action_sprite_image_offset = 0;
			sub_693B58(peep);
			invalidate_sprite((rct_sprite*)peep);
			return;
		}

		int rand = scenario_rand();
		if ((rand & 0xFFFF) > 131){
			peep_try_get_up_from_sitting(peep);
			return;
		}
		if (peep->sprite_type == 0x13 || peep->sprite_type == 0x1E){
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
		sub_693B58(peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}
}

/**
 *
 * rct2: 0x006966A9 
 */
void remove_peep_from_queue(rct_peep* peep)
{
	rct_ride* ride = GET_RIDE(peep->current_ride);

	uint8 cur_station = peep->current_ride_station;
	ride->queue_length[cur_station]--;
	if (peep->sprite_index == ride->first_peep_in_queue[cur_station])
	{
		ride->first_peep_in_queue[cur_station] = peep->next_in_queue;
		return;
	}

	for (rct_peep* other_peep = GET_PEEP(ride->first_peep_in_queue[cur_station]);;
		other_peep = GET_PEEP(other_peep->next_in_queue)){
		if (peep->sprite_index == other_peep->next_in_queue){
			other_peep->next_in_queue = peep->next_in_queue;
			return;
		}
	}
}

/* rct2: 0x00691C6E */
static rct_vehicle* peep_choose_car_from_ride(rct_peep* peep, rct_ride* ride, uint8* car_array, uint8 car_array_size){
	uint8 chosen_car = scenario_rand();
	if (RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2] & RIDE_TYPE_FLAG_HAS_G_FORCES
		&& ((chosen_car & 0xC) != 0xC)){
		chosen_car = (scenario_rand() & 1) ? 0 : car_array_size - 1;
	}
	else{
		chosen_car = (chosen_car * (uint16)car_array_size) >> 8;
	}

	peep->current_car = car_array[chosen_car];

	rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);

	for (int i = peep->current_car; i > 0; --i){
		vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
	}

	return vehicle;
}

/* rct2: 0x00691CD1 */
static void peep_choose_seat_from_car(rct_peep* peep, rct_ride* ride, rct_vehicle* vehicle){
	uint8 chosen_seat = vehicle->next_free_seat;

	if (ride->mode == RIDE_MODE_FORWARD_ROTATION ||
		ride->mode == RIDE_MODE_BACKWARD_ROTATION){

		chosen_seat = (((~vehicle->var_1F + 1) >> 3) & 0xF) * 2;
		if (vehicle->next_free_seat & 1){
			chosen_seat++;
		}
	}
	peep->current_seat = chosen_seat;
	vehicle->next_free_seat++;

	vehicle->peep[peep->current_seat] = peep->sprite_index;
	vehicle->peep_tshirt_colours[peep->current_seat] = peep->tshirt_colour;
}

/* rct2: 0x00691D27 */
static void peep_go_to_ride_entrance(rct_peep* peep, rct_ride* ride){
	int x = ride->entrances[peep->current_ride_station] & 0xFF;
	int y = ride->entrances[peep->current_ride_station] >> 8;
	int z = ride->station_heights[peep->current_ride_station];
	
	rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

	uint8 direction = !map_element ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK;

	x *= 32;
	y *= 32;
	x += 16;
	y += 16;

	sint16 x_shift = RCT2_ADDRESS(0x00981D6C, sint16)[direction * 2];
	sint16 y_shift = RCT2_ADDRESS(0x00981D6E, sint16)[direction * 2];

	uint8 shift_multiplier = 21;
	rct_ride_type* ride_type = GET_RIDE_ENTRY(ride->subtype);
	if (ride_type->vehicles[ride_type->default_vehicle].var_12 & (1 << 3) ||
		ride_type->vehicles[ride_type->default_vehicle].var_14 & 0x5000){
		shift_multiplier = 32;
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

/* rct2: 0x00691A3B */
static void peep_update_ride_sub_state_0(rct_peep* peep){
	rct_ride* ride = GET_RIDE(peep->current_ride);

	if (peep->destination_tolerence != 0){
		invalidate_sprite((rct_sprite*)peep);

		sint16 x, y, xy_distance;

		if (peep_update_action(&x, &y, &xy_distance, peep)){
			sint16 z = peep->z;
			if (xy_distance < 16){
				z = ride->station_heights[peep->current_ride_station] * 8 + 2;
			}
			sprite_move(x, y, z, (rct_sprite*)peep);
			invalidate_sprite((rct_sprite*)peep);
		}
		else{
			peep->destination_tolerence = 0;
			peep->sprite_direction ^= (1 << 4);
		}
	}

	uint8 car_array_size = 0xFF;
	uint8* car_array = RCT2_ADDRESS(0xF1AD78, uint8);

	if (RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2] & RIDE_TYPE_FLAG_13){
		if (ride->num_riders >= ride->operation_option)
			return;
	}
	else{
		uint8 chosen_train = 0xFF;

		if (ride->mode == RIDE_MODE_BUMPERCAR || ride->mode == RIDE_MODE_RACE){
			if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
				return;

			for (int i = 0; i < ride->num_vehicles; ++i){
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
			chosen_train = ride->var_066[peep->current_ride_station];
		}	
		if (chosen_train == 0xFF){
			return;
		}

		peep->current_train = chosen_train;
		uint8* car_array_pointer = car_array;

		int i = 0;

		uint16 vehicle_id = ride->vehicles[chosen_train];
		rct_vehicle* vehicle = GET_VEHICLE(vehicle_id);

		for (; vehicle_id != 0xFFFF;
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
				uint8 position = (((~vehicle->var_1F + 1) >> 3) & 0xF) * 2;
				if (vehicle->peep[position] != 0xFFFF)
					continue;
			}

			*car_array_pointer++ = i;
		}

		car_array_size = car_array_pointer - car_array;

		if (car_array_size == 0)return;
	}

	if (ride->status != RIDE_STATUS_OPEN || 
		ride->var_1CA != 0){
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

	if (ride->price != 0){
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

			if (ride->price > peep->cash_in_pocket){
				peep_insert_new_thought(peep, 0, peep->current_ride);
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
				if (value * 2 < ride->price){
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

	if (!(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2] & RIDE_TYPE_FLAG_13)){
		rct_vehicle* vehicle = peep_choose_car_from_ride(peep, ride, car_array, car_array_size);
		peep_choose_seat_from_car(peep, ride, vehicle);
	}
	peep_go_to_ride_entrance(peep, ride);
}

/* rct2: 0x006921D3 */
void peep_update_ride_sub_state_1(rct_peep* peep){
	sint16 x, y, xy_distance;

	rct_ride* ride = GET_RIDE(peep->current_ride);
	rct_ride_type* ride_entry = gRideTypeList[ride->subtype];

	if (peep_update_action(&x, &y, &xy_distance, peep))
	{
		uint8 vehicle = ride_entry->default_vehicle;

		if (ride_entry->vehicles[vehicle].var_12 & (1 << 3) || 
			ride_entry->vehicles[vehicle].var_14 & ((1 << 14) | (1<<12)))
			RCT2_GLOBAL(0xF1AECA, uint16) = 0x1C;
		else
			RCT2_GLOBAL(0xF1AECA, uint16) = 0x10;

		if (peep->sub_state == 1 &&
			xy_distance < RCT2_GLOBAL(0xF1AECA, uint16))
				peep->sub_state = 2;
		
		invalidate_sprite((rct_sprite*)peep);

		sint16 z = ride->station_heights[peep->current_ride_station] * 8;

		RCT2_GLOBAL(0xF1AECA, uint16) += 4;

		if (xy_distance < RCT2_GLOBAL(0xF1AECA, uint16)){
			z += RCT2_ADDRESS(0x0097D21C, uint8)[ride->type * 8];
		}

		sprite_move(x, y, z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}

	if (RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2] &RIDE_TYPE_FLAG_13)
	{
		sint16 x, y, z;
		x = ride->entrances[peep->current_ride_station] & 0xFF;
		y = ride->entrances[peep->current_ride_station] >> 8;
		z = ride->station_heights[peep->current_ride_station];

		rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

		uint8 direction_entrance = (map_element->type & MAP_ELEMENT_DIRECTION_MASK);

		if (ride->type == RIDE_TYPE_MAZE){	
			peep->maze_last_edge = direction_entrance + 1;
			x *= 32;
			y *= 32;

			x += TileDirectionDelta[direction_entrance].x;
			y += TileDirectionDelta[direction_entrance].y;

			uint8 direction = direction_entrance * 4 + 11;
			if (scenario_rand() & 0x40){
				direction += 4;
				peep->maze_last_edge += 2;
			}

			direction &= 0xF;
			peep->var_37 = direction;
			peep->maze_last_edge &= 3;

			x += RCT2_GLOBAL(0x981FD1 + direction, sint16);
			y += RCT2_GLOBAL(0x981FD3 + direction, sint16);

			peep->destination_x = x;
			peep->destination_y = y;
			peep->destination_tolerence = 3;

			ride->var_120++;
			peep_on_enter_or_exit_ride(peep, peep->current_ride, 0);
			peep->sub_state = 17;
			return;
		}

		x = ride->station_starts[peep->current_ride_station] & 0xFF;
		y = ride->station_starts[peep->current_ride_station] >> 8;

		map_element = ride_get_station_start_track_element(ride, peep->current_ride_station);

		uint8 direction_track = (!map_element ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

		peep->var_37 = (direction_entrance << 2) | (direction_track << 4);

		x *= 32;
		y *= 32;

		sint8* edx = peep->var_37 * 2 + RCT2_ADDRESS(0x97E1BC, sint8*)[ride->type];

		x += edx[0];
		y += edx[1];

		peep->destination_x = x;
		peep->destination_y = y;
		peep->current_car = 0;

		ride->var_120++;
		peep_on_enter_or_exit_ride(peep, peep->current_ride, 0);
		peep->sub_state = 14;
		return;
	}

	rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
	for (int i = peep->current_car; i != 0; --i){
		vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
	}

	ride_entry = GET_RIDE_ENTRY(vehicle->ride_subtype);
	rct_ride_type_vehicle* vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

	if (vehicle_type->var_14 & (1 << 10)){
		sint16 x, y, z;
		x = ride->entrances[peep->current_ride_station] & 0xFF;
		y = ride->entrances[peep->current_ride_station] >> 8;
		z = ride->station_heights[peep->current_ride_station];

		rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

		uint8 direction_entrance = !map_element ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK;

		x = ride->station_starts[peep->current_ride_station] & 0xFF;
		y = ride->station_starts[peep->current_ride_station] >> 8;

		map_element = ride_get_station_start_track_element(ride, peep->current_ride_station);

		uint8 direction_track = (!map_element ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

		vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
		ride_entry = GET_RIDE_ENTRY(vehicle->ride_subtype);
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

	if (vehicle_type->var_14 & (1 << 15)){
		peep->destination_x = vehicle->x;
		peep->destination_y = vehicle->y;
		peep->destination_tolerence = 15;
		peep->sub_state = 4;
		return;
	}

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

/* rct2: 0x0069321D */
static void peep_go_to_ride_exit(rct_peep* peep, rct_ride* ride, sint16 x, sint16 y, sint16 z, uint8 exit_direction){
	z += RCT2_ADDRESS(0x0097D21C, uint8)[ride->type * 8];

	sprite_move(x, y, z, (rct_sprite*)peep);
	invalidate_sprite((rct_sprite*)peep);

	x = ride->exits[peep->current_ride_station] & 0xFF;
	y = ride->exits[peep->current_ride_station] >> 8;
	x *= 32;
	y *= 32;
	x += 16;
	y += 16;

	sint16 x_shift = RCT2_ADDRESS(0x00981D6C, sint16)[exit_direction * 2];
	sint16 y_shift = RCT2_ADDRESS(0x00981D6E, sint16)[exit_direction * 2];

	sint16 shift_multiplier = 20;

	rct_ride_type* ride_type = GET_RIDE_ENTRY(ride->subtype);
	rct_ride_type_vehicle* vehicle_entry = &ride_type->vehicles[ride_type->default_vehicle];
	if (vehicle_entry->var_12 & (1 << 3) ||
		vehicle_entry->var_14 & 0x5000){
		shift_multiplier = 32;
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

/* rct2: 0x006920B4 */
static void peep_update_ride_sub_state_2_enter_ride(rct_peep* peep, rct_ride* ride){
	if (ride->price != 0){
		if ((peep->item_standard_flags & PEEP_ITEM_VOUCHER) &&
			(peep->voucher_type == VOUCHER_TYPE_RIDE_FREE) &&
			(peep->voucher_arguments == peep->current_ride)){

			peep->item_standard_flags &= ~PEEP_ITEM_VOUCHER;
			peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
		}
		else{
			ride->total_profit += ride->price;
			ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
			RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS * 4;
			peep_spend_money(peep, &peep->paid_on_rides, ride->price);
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

	if (peep->flags & PEEP_FLAGS_TRACKING){
		RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
		RCT2_GLOBAL(0x13CE954, uint32) = peep->id;
		RCT2_GLOBAL(0x13CE958, uint16) = ride->name;
		RCT2_GLOBAL(0x13CE95A, uint32) = ride->name_arguments;

		rct_string_id msg_string;
		if (RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2] & RIDE_TYPE_FLAG_IN_RIDE)
			msg_string = 1932;
		else
			msg_string = 1933;

		news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, msg_string, peep->sprite_index);
	}

	if (ride->type == RIDE_TYPE_SPIRAL_SLIDE){
		sub_693BE5(peep, 1);
	}

	peep_update_ride_sub_state_1(peep);
}

/* rct2: 0x00691FD4 */
static void peep_update_ride_sub_state_2_rejoin_queue(rct_peep* peep, rct_ride* ride){
	sint16 x, y, z;
	x = ride->entrances[peep->current_ride_station] & 0xFF;
	y = ride->entrances[peep->current_ride_station] >> 8;
	z = ride->station_heights[peep->current_ride_station];

	rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

	uint8 direction_entrance = !map_element ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK;

	x *= 32;
	y *= 32;
	x += 16 - RCT2_ADDRESS(0x981D6C, sint16)[direction_entrance * 2] * 20;
	y += 16 - RCT2_ADDRESS(0x981D6E, sint16)[direction_entrance * 2] * 20;

	peep->destination_x = x;
	peep->destination_y = y;
	peep->destination_tolerence = 2;

	peep_decrement_num_riders(peep);
	peep->state = PEEP_STATE_QUEUING_FRONT;
	peep->sub_state = 0;
	peep_window_state_update(peep);

	peep->next_in_queue = 0xFFFF;

	ride->queue_length[peep->current_ride_station]++;

	uint16 current_first = ride->first_peep_in_queue[peep->current_ride_station];
	if (current_first == 0xFFFF){
		ride->first_peep_in_queue[peep->current_ride_station] = peep->sprite_index;
		return;
	}

	rct_peep* queue_peep;
	for (queue_peep = GET_PEEP(current_first);
		queue_peep->next_in_queue != 0xFFFF;
		queue_peep = GET_PEEP(queue_peep->next_in_queue));

	queue_peep->next_in_queue = peep->sprite_index;
}
/* rct2: 0x00691E42 
 * Note: Before this was the entry
 * point for sub state 1 and 3. The
 * check has been removed that would
 * branch it out to 1 and 3. Now uses
 * separate functions. 
 */
static void peep_update_ride_sub_state_2(rct_peep* peep){
	rct_ride* ride = GET_RIDE(peep->current_ride);

	if (RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2] & RIDE_TYPE_FLAG_13){
		if (ride->status != RIDE_STATUS_OPEN ||
			ride->var_1CA != 0 || 
			(++peep->var_AC) == 0){

			peep_update_ride_sub_state_2_rejoin_queue(peep, ride);
			return;
		}

		peep_update_ride_sub_state_2_enter_ride(peep, ride);
		return;
	}

	rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
	for (int i = peep->current_car; i != 0; --i){
		vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
	}

	rct_ride_type* ride_entry = GET_RIDE_ENTRY(vehicle->ride_subtype);

	if (ride_entry->vehicles[0].var_12 & (1 << 3)){
		vehicle->var_D5 &= ~(1 << 5);


		for (int i = 0; i < ride->num_vehicles; ++i){
			if (ride->vehicles[i] == 0xFFFF)
				continue;

			rct_vehicle* train = GET_VEHICLE(ride->vehicles[i]);
			rct_vehicle* second_vehicle = GET_VEHICLE(train->next_vehicle_on_train);

			if (second_vehicle->num_peeps == 0)
				continue;

			if (second_vehicle->var_D5 & (1 << 5))
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

	if (ride->status == RIDE_STATUS_OPEN &&
		++peep->var_AC != 0 &&
		!((GET_VEHICLE(ride->vehicles[peep->current_train]))->var_48 & (1 << 4)))
		return;
	
	if (ride->mode != RIDE_MODE_FORWARD_ROTATION &&
		ride->mode != RIDE_MODE_BACKWARD_ROTATION){
		if (vehicle->next_free_seat - 1 != peep->current_seat)
			return;
	}

	vehicle->next_free_seat--;
	vehicle->peep[peep->current_seat] = 0xFFFF;

	peep_update_ride_sub_state_2_rejoin_queue(peep, ride);
}

static void peep_update_ride_sub_state_5(rct_peep* peep){
	rct_ride* ride = GET_RIDE(peep->current_ride);

	rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
	for (int i = peep->current_car; i != 0; --i){
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
		ride->var_120++;

		vehicle->var_46 += seated_peep->var_41;
		invalidate_sprite((rct_sprite*)seated_peep);
		sprite_move(0x8000, 0, 0, (rct_sprite*)seated_peep);

		peep_decrement_num_riders(seated_peep);
		seated_peep->state = PEEP_STATE_ON_RIDE;
		peep_window_state_update(seated_peep);
		seated_peep->time_on_ride = 0;
		seated_peep->sub_state = 6;
		peep_on_enter_or_exit_ride(seated_peep, peep->current_ride, 0);
	}

	vehicle->num_peeps++;
	ride->var_120++;

	vehicle->var_46 += peep->var_41;
	invalidate_sprite((rct_sprite*)vehicle);

	invalidate_sprite((rct_sprite*)peep);
	sprite_move(0x8000, 0, 0, (rct_sprite*)peep);

	peep_decrement_num_riders(peep);
	peep->state = PEEP_STATE_ON_RIDE;
	peep_window_state_update(peep);

	peep->time_on_ride = 0;
	peep->sub_state = 6;

	peep_on_enter_or_exit_ride(peep, peep->current_ride, 0);
}

/* rct2: 0x00693028*/
void peep_update_ride_sub_state_7(rct_peep* peep){
	rct_ride* ride = GET_RIDE(peep->current_ride);

	rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
	uint8 ride_station = vehicle->current_station;

	for (int i = peep->current_car; i != 0; --i){
		vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
	}

	// Unsure why backward rotation is missing.
	if (ride->mode != RIDE_MODE_FORWARD_ROTATION){
		if (vehicle->num_peeps - 1 != peep->current_seat)
			return;
	}

	peep->action_sprite_image_offset++;
	if (peep->action_sprite_image_offset & 3)
		return;

	peep->action_sprite_image_offset = 0;

	vehicle->num_peeps--;
	vehicle->var_46 -= peep->var_41;
	invalidate_sprite((rct_sprite*)vehicle);

	peep->current_ride_station = ride_station;

	rct_ride_type* ride_entry = GET_RIDE_ENTRY(vehicle->ride_subtype);
	rct_ride_type_vehicle* vehicle_entry = &ride_entry->vehicles[vehicle->vehicle_type];

	if (!(vehicle_entry->var_14 & (1 << 10))){
		sint16 x, y, z;
		x = ride->exits[peep->current_ride_station] & 0xFF;
		y = ride->exits[peep->current_ride_station] >> 8;
		z = ride->station_heights[peep->current_ride_station];

		rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

		uint8 exit_direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
		exit_direction ^= (1 << 1);

		if (!(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2] & RIDE_TYPE_FLAG_16)){
			
			for (; vehicle->var_01 != 0; vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_train)){
				uint16 eax = vehicle->var_36 / 4;
				if (eax == 0 || eax > 3)
					continue;

				rct_map_element* inner_map = map_get_first_element_at(vehicle->var_38 / 32, vehicle->var_3A / 32);
				for (;; inner_map++){
					if (map_element_get_type(inner_map) != MAP_ELEMENT_TYPE_TRACK)
						continue;
					if (inner_map->base_height == vehicle->var_3C / 8)
						break;
				}

				uint8 al = (inner_map->properties.track.sequence & 0x70) >> 4;
				if (al == peep->current_ride_station)
					break;
			}

			ride_entry = GET_RIDE_ENTRY(ride->subtype);
			vehicle_entry = &ride_entry->vehicles[ride_entry->default_vehicle];

			uint8 shift_multiplier = 12;
			if (vehicle_entry->var_14 & (1 << 14)){
				shift_multiplier = 9;
			}

			uint8 direction = exit_direction;
			if (vehicle_entry->var_14 & ((1 << 14) | (1 << 12))){
				direction = ((vehicle->sprite_direction + 3) / 8) + 1;
				direction &= 3;

				if (vehicle->var_CD == 6)
					direction ^= (1 << 1);
			}

			sint16 x_shift = RCT2_ADDRESS(0x00981D6C, sint16)[direction * 2];
			sint16 y_shift = RCT2_ADDRESS(0x00981D6E, sint16)[direction * 2];

			x = vehicle->x + x_shift * shift_multiplier;
			y = vehicle->y + y_shift * shift_multiplier;
			z *= 8;

			peep_go_to_ride_exit(peep, ride, x, y, z, exit_direction);
			return;
		}

		x = vehicle->x + RCT2_ADDRESS(0x00981D6C, sint16)[exit_direction * 2] * 12;
		y = vehicle->y + RCT2_ADDRESS(0x00981D6E, sint16)[exit_direction * 2] * 12;

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

	uint8 exit_direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;

	x = ride->station_starts[peep->current_ride_station] & 0xFF;
	y = ride->station_starts[peep->current_ride_station] >> 8;

	map_element = ride_get_station_start_track_element(ride, peep->current_ride_station);

	uint8 station_direction = (!map_element ? 0 : map_element->type & MAP_ELEMENT_DIRECTION_MASK);

	vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
	
	ride_entry = GET_RIDE_ENTRY(vehicle->ride_subtype);
	rct_ride_type_vehicle* vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

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
	z += RCT2_ADDRESS(0x0097D21C, uint8)[ride->type * 8];

	if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
		z += 15;

	sprite_move(exit_x, exit_y, z, (rct_sprite*)peep);
	invalidate_sprite((rct_sprite*)peep);

	x += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 1];
	y += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 2];

	peep->destination_x = x;
	peep->destination_y = y;
	peep->destination_tolerence = 2;
	peep->sub_state = 13;
}

/* rct2: 0x0069376A */
static void peep_update_ride_prepare_for_state_9(rct_peep* peep){
	rct_ride* ride = GET_RIDE(peep->current_ride);
	
	sint16 x = ride->exits[peep->current_ride_station] & 0xFF;
	sint16 y = ride->exits[peep->current_ride_station] >> 8;
	sint16 z = ride->station_heights[peep->current_ride_station];

	rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

	uint8 exit_direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;

	x *= 32;
	y *= 32;
	x += 16;
	y += 16;

	sint16 x_shift = RCT2_ADDRESS(0x00981D6C, sint16)[exit_direction * 2];
	sint16 y_shift = RCT2_ADDRESS(0x00981D6E, sint16)[exit_direction * 2];

	sint16 shift_multiplier = 20;

	rct_ride_type* ride_type = GET_RIDE_ENTRY(ride->subtype);
	rct_ride_type_vehicle* vehicle_entry = &ride_type->vehicles[ride_type->default_vehicle];
	if (vehicle_entry->var_14 & 0x5000){
		shift_multiplier = 32;
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

/* rct2: 0x0069374F */
static void peep_update_ride_sub_state_8(rct_peep* peep){
	sint16 x, y, xy_distance;
	if (peep_update_action(&x, &y, &xy_distance, peep)){
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}

	peep_update_ride_prepare_for_state_9(peep);
}

/* rct2: 0x0069382E */
static void peep_update_ride_sub_state_9(rct_peep* peep){
	sint16 x, y, xy_distance;
	rct_ride* ride = GET_RIDE(peep->current_ride);

	if (peep_update_action(&x, &y, &xy_distance, peep)){
		invalidate_sprite((rct_sprite*)peep);
		
		if (xy_distance >= 16){
			sint16 z = ride->station_heights[peep->current_ride_station] * 8;

			z += RCT2_ADDRESS(0x97D21C, uint8)[ride->type * 8];
			sprite_move(x, y, z, (rct_sprite*)peep);
			invalidate_sprite((rct_sprite*)peep);
			return;
		}
		
		sub_693BE5(peep, 0);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
	}

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO){
		uint8 secondaryItem = RCT2_ADDRESS(0x0097D7CB, uint8)[ride->type * 4];
		if (sub_69AF1E(peep, peep->current_ride, secondaryItem, ride->price_secondary)) {
			ride->no_secondary_items_sold++;
		}
	}
	peep->sub_state = 18;
}

/* rct2: 0x006926AD */
static void peep_update_ride_sub_state_12(rct_peep* peep){
	sint16 x, y, xy_distance;
	rct_ride* ride = GET_RIDE(peep->current_ride);

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
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
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

	rct_ride_type* ride_entry = GET_RIDE_ENTRY(vehicle->ride_subtype);
	rct_ride_type_vehicle* vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

	x += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 1];
	y += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 2];

	peep->destination_x = x;
	peep->destination_y = y;
}

/* rct2: 0x0069357D */
static void peep_udpate_ride_sub_state_13(rct_peep* peep){
	sint16 x, y, xy_distance;
	rct_ride* ride = GET_RIDE(peep->current_ride);

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
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
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

		rct_ride_type* ride_entry = GET_RIDE_ENTRY(vehicle->ride_subtype);
		rct_ride_type_vehicle* vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

		x += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 1];
		y += vehicle_type->peep_loading_positions[peep->var_37 * 2 + 2];

		peep->destination_x = x;
		peep->destination_y = y;
	}

	peep->var_37 |= 3;

	x = ride->exits[peep->current_ride_station] & 0xFF;
	y = ride->exits[peep->current_ride_station] >> 8;
	sint16 z = ride->station_heights[peep->current_ride_station];

	rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

	uint8 exit_direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
	exit_direction ^= (1 << 1);

	x *= 32;
	y *= 32;
	x += 16;
	y += 16;

	sint16 x_shift = RCT2_ADDRESS(0x00981D6C, sint16)[exit_direction * 2];
	sint16 y_shift = RCT2_ADDRESS(0x00981D6E, sint16)[exit_direction * 2];

	sint16 shift_multiplier = 20;

	rct_ride_type* ride_type = GET_RIDE_ENTRY(ride->subtype);
	rct_ride_type_vehicle* vehicle_entry = &ride_type->vehicles[ride_type->default_vehicle];
	if (vehicle_entry->var_14 & 0x5000){
		shift_multiplier = 32;
	}

	x_shift *= shift_multiplier;
	y_shift *= shift_multiplier;

	x -= x_shift;
	y -= y_shift;

	peep->destination_x = x;
	peep->destination_y = y;
}

/* rct2: 0x006927B3 */
static void peep_update_ride_sub_state_14(rct_peep* peep){
	sint16 x, y, xy_distance;
	rct_ride* ride = GET_RIDE(peep->current_ride);

	if (peep_update_action(&x, &y, &xy_distance, peep)){
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
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
			if ((uint8)(peep->current_car - 1) > (scenario_rand() & 0xF))
				last_ride = 1;
		}

		if (last_ride){
			x = ride->exits[peep->current_ride_station] & 0xFF;
			y = ride->exits[peep->current_ride_station] >> 8;
			sint16 z = ride->station_heights[peep->current_ride_station];

			rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

			uint8 exit_direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
			
			peep->var_37 = (exit_direction * 4) | (peep->var_37 & 0x30) |  1;
			x = ride->station_starts[peep->current_ride_station] & 0xFF;
			y = ride->station_starts[peep->current_ride_station] >> 8;

			x *= 32;
			y *= 32;
			sint8* edx = peep->var_37 * 2 + RCT2_ADDRESS(0x97E1BC, sint8*)[ride->type];

			x += edx[0];
			y += edx[1];

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
	sint8* edx = peep->var_37 * 2 + RCT2_ADDRESS(0x97E1BC, sint8*)[ride->type];

	x += edx[0];
	y += edx[1];

	peep->destination_x = x;
	peep->destination_y = y;
}

/* rct2: 0x00692D83 */
static void peep_update_ride_sub_state_15(rct_peep* peep){
	rct_ride* ride = GET_RIDE(peep->current_ride);

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
			if (ride->var_15D != 0)
				return;

			ride->var_15D++;
			ride->slide_peep = peep->sprite_index;
			ride->slide_peep_t_shirt_colour = peep->tshirt_colour;
			ride->var_176 = 0;
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
			sint16 dest_x = x + RCT2_ADDRESS(0x981F1C, sint16)[direction * 2];
			sint16 dest_y = y + RCT2_ADDRESS(0x981F1E, sint16)[direction * 2];

			peep->destination_x = dest_x;
			peep->destination_y = dest_y;

			x += RCT2_ADDRESS(0x981F0C, sint16)[direction * 2];
			y += RCT2_ADDRESS(0x981F0E, sint16)[direction * 2];

			sprite_move(x, y, peep->z, (rct_sprite*)peep);

			peep->sprite_direction = (peep->var_37 & 0xC) * 2;

			invalidate_sprite((rct_sprite*)peep);

			peep->var_37++;
			return; 
		}
		default:
			return;
		}
	}

	sint16 x, y, xy_distance;

	if (peep_update_action(&x, &y, &xy_distance, peep)){
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}

	peep->var_37 = (peep->var_37 * 4 & 0x30) + 2;

	x = ride->station_starts[peep->current_ride_station] & 0xFF;
	y = ride->station_starts[peep->current_ride_station] >> 8;

	x *= 32;
	y *= 32;
	sint8* edx = peep->var_37 * 2 + RCT2_ADDRESS(0x97E1BC, sint8*)[ride->type];

	x += edx[0];
	y += edx[1];

	peep->destination_x = x;
	peep->destination_y = y;
	peep->sub_state = 14;
}

/* rct2: 0x00692C6B */
static void peep_update_ride_sub_state_16(rct_peep* peep){
	sint16 x, y, xy_distance;

	if (peep_update_action(&x, &y, &xy_distance, peep)){
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}

	rct_ride* ride = GET_RIDE(peep->current_ride);

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
		sint8* edx = peep->var_37 * 2 + RCT2_ADDRESS(0x97E1BC, sint8*)[ride->type];

		x += edx[0];
		y += edx[1];

		peep->destination_x = x;
		peep->destination_y = y;
		return;
	}

	peep->var_37 |= 3;

	x = ride->exits[peep->current_ride_station] & 0xFF;
	y = ride->exits[peep->current_ride_station] >> 8;
	sint16 z = ride->station_heights[peep->current_ride_station];

	rct_map_element* map_element = ride_get_station_exit_element(ride, x, y, z);

	uint8 exit_direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
	exit_direction ^= (1 << 1);

	x *= 32;
	y *= 32;
	x += 16;
	y += 16;

	sint16 x_shift = RCT2_ADDRESS(0x00981D6C, sint16)[exit_direction * 2];
	sint16 y_shift = RCT2_ADDRESS(0x00981D6E, sint16)[exit_direction * 2];

	sint16 shift_multiplier = 20;

	x_shift *= shift_multiplier;
	y_shift *= shift_multiplier;

	x -= x_shift;
	y -= y_shift;

	peep->destination_x = x;
	peep->destination_y = y;
}

/* rct2: 0x00692A83 */
static void peep_update_ride_sub_state_17(rct_peep* peep){
	sint16 x, y, xy_distance;

	if (peep_update_action(&x, &y, &xy_distance, peep)){
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}

	rct_ride* ride = GET_RIDE(peep->current_ride);
	if (peep->var_37 == 16){
		peep_update_ride_prepare_for_state_9(peep);
		return;
	}

	if (peep->action >= PEEP_ACTION_NONE_1){
		if (peep->energy > 64 &&
			(scenario_rand() & 0xFFFF) <= 2427){

			peep->action = PEEP_ACTION_JUMP;
			peep->action_frame = 0;
			peep->action_sprite_image_offset = 0;
			sub_693B58(peep);
			invalidate_sprite((rct_sprite*)peep);
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

	if (maze_entry & (1 << RCT2_ADDRESS(0x981FF4, uint8)[var_37])){
		open_hedges = 1;
	}
	open_hedges <<= 1;
	if (maze_entry & (1 << RCT2_ADDRESS(0x981FF3, uint8)[var_37])){
		open_hedges |= 1;
	}
	open_hedges <<= 1;
	if (maze_entry & (1 << RCT2_ADDRESS(0x981FF2, uint8)[var_37])){
		open_hedges |= 1;
	}
	open_hedges <<= 1;
	if (maze_entry & (1 << RCT2_ADDRESS(0x981FF1, uint8)[var_37])){
		open_hedges |= 1;
	}

	open_hedges ^= 0xF;
	if (open_hedges == 0)
		return;

	uint8 maze_last_edge = peep->maze_last_edge ^ (1 << 1);
	open_hedges &= ~(1 << maze_last_edge);
	if (open_hedges == 0)
		open_hedges |= (1 << maze_last_edge);

	uint8 chosen_edge = scenario_rand() & 0x3;
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

		peep->var_37 = RCT2_ADDRESS(0x981FE1, uint8)[peep->var_37 + chosen_edge];
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
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}
}

/* rct2: 0x006938D2 */
static void peep_update_ride_sub_state_18(rct_peep* peep){
	sint16 x, y, xy_distance;
	rct_ride* ride = GET_RIDE(peep->current_ride);

	if (peep_update_action(&x, &y, &xy_distance, peep)){
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, ride->station_heights[peep->current_ride_station] * 8, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}

	peep_on_enter_or_exit_ride(peep, peep->current_ride, 1);

	if (peep->flags & PEEP_FLAGS_TRACKING){
		RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
		RCT2_GLOBAL(0x13CE954, uint32) = peep->id;
		RCT2_GLOBAL(0x13CE958, uint16) = ride->name;
		RCT2_GLOBAL(0x13CE95A, uint32) = ride->name_arguments;

		news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, 1934, peep->sprite_index);
	}

	peep->var_79 = 0xFF;
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
		invalidate_sprite((rct_sprite*)peep);
		return;
	} while (!map_element_is_last_for_tile(mapElement++));
}

/* rct2: 0x0069299C */
static void peep_update_ride_sub_state_19(rct_peep* peep){
	sint16 x, y, xy_distance;

	if (peep_update_action(&x, &y, &xy_distance, peep)){
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}

	peep->sub_state++;
}

/* rct2: 0x006929BB */
static void peep_update_ride_sub_state_20(rct_peep* peep){
	sint16 x, y;
	rct_ride* ride = GET_RIDE(peep->current_ride);

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

	sound_play_panned(SOUND_TOILET_FLUSH, 0x8001, peep->x, peep->y, peep->z);

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

/* rct2: 0x00692935 */
static void peep_update_ride_sub_state_21(rct_peep* peep){
	sint16 x, y, xy_distance;

	if (peep_update_action(&x, &y, &xy_distance, peep)){
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);

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

	rct_ride* ride = GET_RIDE(peep->current_ride);
	ride->total_customers++;
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

	ride_update_satisfaction(ride, peep->happiness / 64);
}

/* rct2: 0x691A30 
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

		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
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
		peep_udpate_ride_sub_state_13(peep);
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
		RCT2_CALLPROC_X(RCT2_ADDRESS(0x9820DC, int)[peep->sub_state], 0, 0, 0, 0, (int)peep, 0, 0);
	}
}

/* rct2: 0x006C0E8B 
 * Also used by inspecting.
 */
static void peep_update_fixing(int steps, rct_peep* peep){
	rct_ride* ride = GET_RIDE(peep->current_ride);

	if (ride->type == RIDE_TYPE_NULL)
	{
		peep_decrement_num_riders(peep);
		peep->state = PEEP_STATE_FALLING;
		peep_window_state_update(peep);
		return;
	}

	RCT2_CALLPROC_X(RCT2_ADDRESS(0x006C0EB0, uint32)[peep->sub_state], steps, 0, 0, 0, (int)peep, (int)ride, 0);
}

/**
 * rct2: 0x69185D
 */
static void peep_update_queuing(rct_peep* peep){
	if (!sub_68F3AE(peep)){
		remove_peep_from_queue(peep);
		return;
	}
	rct_ride* ride = GET_RIDE(peep->current_ride);
	if (ride->status == RIDE_STATUS_CLOSED || ride->status == RIDE_STATUS_TESTING){
		remove_peep_from_queue(peep);
		peep_decrement_num_riders(peep);
		peep->state = PEEP_STATE_1;
		peep_window_state_update(peep);
		return;
	}

	if (peep->sub_state != 10){
		if (peep->next_in_queue == 0xFFFF){
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
		invalidate_sprite((rct_sprite*)peep);
		remove_peep_from_queue(peep);
		peep_decrement_num_riders(peep);
		peep->state = PEEP_STATE_1;
		peep_window_state_update(peep);
		return;
	}

	sub_693C9E(peep);
	if (peep->action < 0xFE)return;
	if (peep->sprite_type == 0){
		if (peep->time_in_queue >= 2000 && (0xFFFF & scenario_rand()) <= 119){
			// Eat Food/Look at watch
			peep->action = PEEP_ACTION_EAT_FOOD;
			peep->action_frame = 0;
			peep->action_sprite_image_offset = 0;
			sub_693B58(peep);
			invalidate_sprite((rct_sprite*)peep);
		}
		if (peep->time_in_queue >= 3500 && (0xFFFF & scenario_rand()) <= 93)
		{
			//Create the ive been waiting in line ages thought
			peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_QUEUING_AGES, peep->current_ride);
		}
	}
	else{
		if (!(peep->time_in_queue & 0x3F) && peep->action == 0xFE && peep->var_6F == 2){
			switch (peep->sprite_type){
			case 0xF:
			case 0x10:
			case 0x11:
			case 0x12:
			case 0x14:
			case 0x16:
			case 0x18:
			case 0x1F:
			case 0x20:
			case 0x21:
			case 0x22:
			case 0x23:
			case 0x24:
			case 0x25:
			case 0x27:
			case 0x29:
			case 0x2A:
			case 0x2B:
			case 0x2C:
			case 0x2D:
			case 0x2E:
			case 0x2F:
				// Eat food/Look at watch
				peep->action = PEEP_ACTION_EAT_FOOD;
				peep->action_frame = 0;
				peep->action_sprite_image_offset = 0;
				sub_693B58(peep);
				invalidate_sprite((rct_sprite*)peep);
				break;
			}
		}
	}
	if (peep->time_in_queue < 4300) return;

	if (peep->happiness <= 65 && (0xFFFF & scenario_rand()) < 2184){
		//Give up queueing for the ride
		peep->sprite_direction ^= (1 << 4);
		invalidate_sprite((rct_sprite*)peep);
		remove_peep_from_queue(peep);
		peep_decrement_num_riders(peep);
		peep->state = PEEP_STATE_1;
		peep_window_state_update(peep);
	}
}

/* rct2: 0x006BF567 */
static void peep_update_mowing(rct_peep* peep){
	peep->var_E2 = 0;
	if (!sub_68F3AE(peep))return;

	invalidate_sprite((rct_sprite*)peep);
	while (1){
		sint16 x = 0, y = 0, z, xy_distance;
		if (peep_update_action(&x, &y, &xy_distance, peep)){
			z = map_element_height(x, y) & 0xFFFF;
			sprite_move(x, y, z, (rct_sprite*)peep);
			invalidate_sprite((rct_sprite*)peep);
			return;
		}

		peep->var_37++;

		if (peep->var_37 == 1){
			sub_693BE5(peep, 2);
		}

		if (RCT2_ADDRESS(0x9929C8, uint16)[peep->var_37 * 2] == 0xFFFF){
			peep_state_reset(peep);
			return;
		}

		peep->destination_x = RCT2_ADDRESS(0x9929C8, uint16)[peep->var_37 * 2] + peep->next_x;
		peep->destination_y = RCT2_ADDRESS(0x9929CA, uint16)[peep->var_37 * 2] + peep->next_y;

		if (peep->var_37 != 7)continue;

		rct_map_element *map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

		for (; (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_SURFACE); map_element++);

		if ((map_element->properties.surface.terrain & MAP_ELEMENT_SURFACE_TERRAIN_MASK) == (TERRAIN_GRASS << 5)){
			map_element->properties.surface.grass_length = 0;
			map_invalidate_tile_zoom0(peep->next_x, peep->next_y, map_element->base_height * 8, map_element->base_height * 8 + 16);
		}
		peep->staff_lawns_mown++;
		peep->window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
	}
}

/* rct2: 0x006BF7E6 */
static void peep_update_watering(rct_peep* peep){
	peep->var_E2 = 0;
	if (peep->sub_state == 0){
		if (!sub_68F3AE(peep))return;

		sub_693C9E(peep);
		if (!(RCT2_GLOBAL(0xF1EE18, uint16) & 1))return;

		peep->sprite_direction = (peep->var_37 & 3) << 3;
		peep->action = PEEP_ACTION_STAFF_WATERING;
		peep->action_frame = 0;
		peep->action_sprite_image_offset = 0;
		sub_693B58(peep);
		invalidate_sprite((rct_sprite*)peep);		
		
		peep->sub_state = 1;
	}
	else if (peep->sub_state == 1){
		if (peep->action != PEEP_ACTION_NONE_2){
			sint16 x, y, xy_distance;
			peep_update_action(&x, &y, &xy_distance, peep);
			return;
		}

		int x = peep->next_x + TileDirectionDelta[peep->var_37].x;
		int y = peep->next_y + TileDirectionDelta[peep->var_37].y;

		rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

		do{
			if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_SCENERY)
				continue;
			
			if (abs(((int)peep->next_z) - map_element->base_height) > 4)
				continue;
			
			rct_scenery_entry* scenery_entry = g_smallSceneryEntries[map_element->properties.scenery.type];

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

/* rct2: 0x006BF6C9 */
static void peep_update_emptying_bin(rct_peep* peep){
	peep->var_E2 = 0;

	if (peep->sub_state == 0){
		if (!sub_68F3AE(peep))return;

		sub_693C9E(peep);
		if (!(RCT2_GLOBAL(0xF1EE18, uint16) & 1))return;

		peep->sprite_direction = (peep->var_37 & 3) << 3;
		peep->action = PEEP_ACTION_STAFF_EMPTY_BIN;
		peep->action_frame = 0;
		peep->action_sprite_image_offset = 0;
		sub_693B58(peep);
		invalidate_sprite((rct_sprite*)peep);

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

		if ((map_element->properties.path.additions & 0x0F) == 0) {
			peep_state_reset(peep);
			return;
		}

		rct_scenery_entry* scenery_entry = g_pathBitSceneryEntries[(map_element->properties.path.additions & 0xF) - 1];
		if (
			!(scenery_entry->path_bit.var_06 & 1)
			|| map_element->flags & (1 << 5)
			|| map_element->properties.path.additions & (1 << 7)
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

/* rct2: 0x6BF641 */
static void peep_update_sweeping(rct_peep* peep){
	peep->var_E2 = 0;
	if (!sub_68F3AE(peep))return;
	
	invalidate_sprite((rct_sprite*)peep);

	if (peep->action == PEEP_ACTION_STAFF_SWEEP && peep->action_frame == 8){
		// Remove sick at this location
		sub_6738E1(peep->x, peep->y, peep->z);
		peep->staff_litter_swept++;
		peep->window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
	}
	sint16 x = 0, y = 0, z, xy_distance;
	if (peep_update_action(&x, &y, &xy_distance, peep)){
		z = peep_get_height_on_slope(peep, x, y);
		sprite_move(x, y, z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}

	peep->var_37++;
	if (peep->var_37 != 2){
		peep->action = PEEP_ACTION_STAFF_SWEEP;
		peep->action_frame = 0;
		peep->action_sprite_image_offset = 0;
		sub_693B58(peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}
	peep_state_reset(peep);
}

/* rct2: 0x6902A2 */
static void peep_update_1(rct_peep* peep){
	if (!sub_68F3AE(peep))return;

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
	peep->var_78 = peep->sprite_direction >> 3;
}

/**
 * rct2: 0x690009
 */
static void peep_update_picked(rct_peep* peep){
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 0x1F) return;
	peep->sub_state++;
	if (peep->sub_state == 13){
		peep_insert_new_thought(peep, PEEP_THOUGHT_HELP, 0xFF);
	}
}

/* rct2: 0x6914CD */
static void peep_update_leaving_park(rct_peep* peep){
	if (peep->var_37 != 0){
		sub_693C9E(peep);
		if (!(RCT2_GLOBAL(0xF1EE18, uint16) & 2))return;
		peep_sprite_remove(peep);
		return;
	}

	sint16 x = 0, y = 0, xy_distance;
	if (peep_update_action(&x, &y, &xy_distance, peep)){
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}

	peep->var_2A = 1;
	peep->destination_tolerence = 5;
	RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16)--;
	RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) |= BTM_TB_DIRTY_FLAG_PEEP_COUNT;
	peep->var_37 = 1;

	window_invalidate_by_class(WC_GUEST_LIST);

	sub_693C9E(peep);
	if (!(RCT2_GLOBAL(0xF1EE18, uint16) & 2))return;
	peep_sprite_remove(peep);
}

/* rct2: 0x6916D6 */
static void peep_update_watching(rct_peep* peep){
	if (peep->sub_state == 0){
		if (!sub_68F3AE(peep))return;

		sub_693C9E(peep);
		if (!(RCT2_GLOBAL(0xF1EE18, uint16) & 1))return;

		peep->destination_x = peep->x;
		peep->destination_y = peep->y;

		peep->sprite_direction = (peep->var_37 & 3) * 8;
		invalidate_sprite((rct_sprite*)peep);

		peep->action = 0xFE;
		peep->var_6F = 2;

		sub_693BAB(peep);

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
				if ((scenario_rand() & 0xFFFF) <= 1310){
					peep->action = PEEP_ACTION_EAT_FOOD;
					peep->action_frame = 0;
					peep->action_sprite_image_offset = 0;
					sub_693B58(peep);
					invalidate_sprite((rct_sprite*)peep);
					return;
				}
			}
			
			if ((scenario_rand() & 0xFFFF) <= 655){
				peep->action = PEEP_ACTION_TAKE_PHOTO;
				peep->action_frame = 0;
				peep->action_sprite_image_offset = 0;
				sub_693B58(peep);
				invalidate_sprite((rct_sprite*)peep);
				return;
			}
				
			if ((peep->standing_flags & 1)){
				if ((scenario_rand() & 0xFFFF) <= 655){
					peep->action = PEEP_ACTION_WAVE;
					peep->action_frame = 0;
					peep->action_sprite_image_offset = 0;
					sub_693B58(peep);
					invalidate_sprite((rct_sprite*)peep);
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
		sub_693B58(peep);
	}
}

/**
* rct2: 0x691451
*/
static void peep_update_entering_park(rct_peep* peep){
	if (peep->var_37 != 1){
		sub_693C9E(peep);
		if ((RCT2_GLOBAL(0xF1EE18, uint16) & 2)){
			RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_HEADING_FOR_PARK, uint16)--;
			peep_sprite_remove(peep);
		}
		return;
	}
	sint16 x = 0, y = 0, xy_distance;
	if (peep_update_action(&x, &y, &xy_distance, peep)){
		invalidate_sprite((rct_sprite*)peep); 
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return;
	}
	peep_decrement_num_riders(peep);
	peep->state = PEEP_STATE_FALLING;
	peep_window_state_update(peep);

	peep->var_2A = 0;
	peep->time_in_park = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, sint32);
	RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16)++;
	RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_HEADING_FOR_PARK, uint16)--;
	RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) |= BTM_TB_DIRTY_FLAG_PEEP_COUNT;
	window_invalidate_by_class(WC_GUEST_LIST);
}

/* rct2: 0x00690582*/
static int peep_update_walking_find_bench(rct_peep* peep){
	if (!peep_should_find_bench(peep))return 0;

	rct_map_element* map_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

	for (;; map_element++){
		if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_PATH){
			if (peep->next_z == map_element->base_height)break;
		}
		if (map_element_is_last_for_tile(map_element)){
			return 0;
		}
	}

	uint8 additions = map_element->properties.path.additions & 0xF;

	if (!additions) return 0;
	rct_scenery_entry* sceneryEntry = g_pathBitSceneryEntries[additions - 1];

	if (!(sceneryEntry->path_bit.var_06 & 0x2))return 0;

	if (map_element->flags & MAP_ELEMENT_FLAG_BROKEN)return 0;

	if (map_element->properties.path.additions & 0x80)return 0;

	int edges = (map_element->properties.path.edges & 0xF) ^ 0xF;
	if (edges == 0) return 0;

	uint8 chosen_edge = scenario_rand() & 0x3;

	for (; !(edges & (1 << chosen_edge));)chosen_edge = (chosen_edge + 1) & 0x3;

	uint16 sprite_id = RCT2_ADDRESS(0xF1EF60, uint16)[((peep->x & 0x1FE0) << 3) | (peep->y >> 5)];
	uint8 free_edge = 3;

	for (rct_sprite* sprite; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant){
		sprite = &g_sprite_list[sprite_id];

		if (sprite->unknown.linked_list_type_offset != SPRITE_LINKEDLIST_OFFSET_PEEP)continue;

		if (sprite->peep.state != PEEP_STATE_SITTING)continue;

		if (peep->z != sprite->peep.z)continue;

		if ((sprite->peep.var_37 & 0x3) != chosen_edge)continue;

		free_edge &= ~(1 << ((sprite->peep.var_37 & 0x4) >> 2));
	}
	
	if (!free_edge) return 0;

	free_edge ^= 0x3;
	if (!free_edge){
		if (scenario_rand() & 0x8000000) free_edge = 1;
	}

	peep->var_37 = ((free_edge & 1) << 2) | chosen_edge;

	peep_decrement_num_riders(peep);
	peep->state = PEEP_STATE_SITTING;
	peep_window_state_update(peep);

	peep->sub_state = 0;

	int ebx = peep->var_37 & 0x7;
	int x = (peep->x & 0xFFE0) + RCT2_ADDRESS(0x981F2C, uint16)[ebx * 2];
	int y = (peep->y & 0xFFE0) + RCT2_ADDRESS(0x981F2E, uint16)[ebx * 2];
	
	peep->destination_x = x;
	peep->destination_y = y;
	peep->destination_tolerence = 3;

	return 1;
}

static int peep_update_walking_find_bin(rct_peep* peep){
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

	uint8 additions = map_element->properties.path.additions & 0xF;

	if (!additions) return 0;
	rct_scenery_entry* sceneryEntry = g_pathBitSceneryEntries[additions - 1];

	if (!(sceneryEntry->path_bit.var_06 & 0x1))return 0;

	if (map_element->flags & MAP_ELEMENT_FLAG_BROKEN)return 0;

	if (map_element->properties.path.additions & 0x80)return 0;

	int edges = (map_element->properties.path.edges & 0xF) ^ 0xF;
	if (edges == 0) return 0;

	uint8 chosen_edge = scenario_rand() & 0x3;

	// Note: Bin qunatity is inverted 0 = full, 3 = empty
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

	int ebx = peep->var_37 & 0x3;
	int x = (peep->x & 0xFFE0) + RCT2_ADDRESS(0x992A4C, uint16)[ebx * 2];
	int y = (peep->y & 0xFFE0) + RCT2_ADDRESS(0x992A4E, uint16)[ebx * 2];

	peep->destination_x = x;
	peep->destination_y = y;
	peep->destination_tolerence = 3;

	return 1;
}

/* rct2: 0x00690848*/
static void peep_update_walking_break_scenery(rct_peep* peep){
	if (!(peep->flags & PEEP_FLAGS_ANGRY)){
		if (peep->happiness >= 48) return;
		if (peep->energy < 85) return;
		if (peep->state != PEEP_STATE_WALKING) return;

		if ((peep->litter_count & 0xC0) != 0xC0 &&
			(peep->disgusting_count & 0xC0) != 0xC0) return;

		if ((scenario_rand() & 0xFFFF) > 3276) return;
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

	uint8 additions = map_element->properties.path.additions & 0xF;

	if (!additions) return;
	rct_scenery_entry* sceneryEntry = g_pathBitSceneryEntries[additions - 1];

	if (!(sceneryEntry->path_bit.var_06 & 0x4))return;

	if (map_element->flags & MAP_ELEMENT_FLAG_BROKEN)return;

	if (map_element->properties.path.additions & 0x80)return;

	int edges = map_element->properties.path.edges & 0xF;
	if (edges == 0xF) return;

	rct_peep* inner_peep;
	uint16 sprite_index;

	FOR_ALL_STAFF(sprite_index, inner_peep){
		if (inner_peep->staff_type != STAFF_TYPE_SECURITY) continue;

		if (inner_peep->x == SPRITE_LOCATION_NULL) continue;

		int x_diff = abs(inner_peep->x - peep->x);
		int y_diff = abs(inner_peep->y - peep->y);

		if (max(x_diff, y_diff) < 224)return;
	}

	map_element->flags |= MAP_ELEMENT_FLAG_BROKEN;

	map_invalidate_tile_zoom1(
		peep->next_x, 
		peep->next_y, 
		(map_element->base_height << 3) + 32, 
		map_element->base_height << 3);

	peep->var_F3 = 0x10;

	return;
}

/* rct2: 0x006912A3 */
static void peep_update_buying(rct_peep* peep)
{
	if (!sub_68F3AE(peep))return;
	
	rct_ride* ride = GET_RIDE(peep->current_ride);
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
		peep->var_78 ^= 2;

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

				sub_693B58(peep);
				invalidate_sprite((rct_sprite*)peep);

				ride->no_primary_items_sold++;
			}
		}
		else{
			rct_ride_type* ride_type = gRideTypeList[ride->subtype];
			if (ride_type->shop_item_secondary != 0xFF){
				money16 price = ride->price_secondary;

				item_bought = sub_69AF1E(peep, peep->current_ride, ride_type->shop_item_secondary, price);
				if (item_bought) {
					ride->no_secondary_items_sold++;
				}
			}

			if (!item_bought && ride_type->shop_item != 0xFF){
				money16 price = ride->price;

				item_bought = sub_69AF1E(peep, peep->current_ride, ride_type->shop_item, price);
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

/* rct2: 0x00691089 */
static void peep_update_using_bin(rct_peep* peep){
	if (peep->sub_state == 0){
		if (!sub_68F3AE(peep))return;

		sub_693C9E(peep);
		if (!(RCT2_GLOBAL(0xF1EE18, uint16) & 1))return;

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

		uint8 additions = map_element->properties.path.additions & 0x0F;
		if (!additions){
			peep_state_reset(peep);
			return;
		}

		rct_scenery_entry* sceneryEntry = g_pathBitSceneryEntries[additions - 1];
		if (!(sceneryEntry->path_bit.var_06 & 1)){
			peep_state_reset(peep);
			return;
		}

		if (map_element->flags & MAP_ELEMENT_FLAG_BROKEN){
			peep_state_reset(peep);
			return;
		}

		if (map_element->properties.path.additions & 0x80){
			peep_state_reset(peep);
			return;
		}

		// Bin selection is one of 4 corners
		uint8 selected_bin = peep->var_37 * 2;

		// This counts down 2 = No rubbish, 0 = full
		uint8 rubbish_in_bin = 0x3 & (map_element->properties.path.addition_status >> selected_bin);
		uint32 empty_containers = peep_empty_container_standard_flag(peep);

		for (uint8 cur_container = 0; cur_container < 32; cur_container++){
			if (!(empty_containers & (1 << cur_container))) continue;

			if (rubbish_in_bin != 0){
				// OpenRCT2 modification: This previously used
				// the tick count as a simple random function
				// switched to scenario_rand as it is more reliable
				if (scenario_rand() & 7) rubbish_in_bin--;
				peep->item_standard_flags &= ~(1 << cur_container);
				peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
				peep_update_sprite_type(peep);
				continue;
			}
			uint8 bp = RCT2_ADDRESS(0x97EFCC, uint8)[cur_container];

			int x, y;
			x = peep->x + (scenario_rand() & 7) - 3;
			y = peep->y + (scenario_rand() & 7) - 3;

			litter_create(x, y, peep->z, scenario_rand() & 3, bp);
			peep->item_standard_flags &= ~(1 << cur_container);
			peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;

			peep_update_sprite_type(peep);
		}

		// Original bug: This would clear any rubbish placed by the previous function
		//rubbish_in_bin = 0x3 & (map_element->properties.path.addition_status >> selected_bin);
		empty_containers = peep_empty_container_extra_flag(peep);

		for (uint8 cur_container = 0; cur_container < 32; cur_container++){
			if (!(empty_containers & (1 << cur_container))) continue;

			if (rubbish_in_bin != 0){
				// OpenRCT2 modification: This previously used
				// the tick count as a simple random function
				// switched to scenario_rand as it is more reliable
				if (scenario_rand() & 7) rubbish_in_bin--;
				peep->item_extra_flags &= ~(1 << cur_container);
				peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;

				peep_update_sprite_type(peep);
				continue;
			}
			uint8 bp = RCT2_ADDRESS(0x97EFE8, uint8)[cur_container];

			int x, y;
			x = peep->x + (scenario_rand() & 7) - 3;
			y = peep->y + (scenario_rand() & 7) - 3;

			litter_create(x, y, peep->z, scenario_rand() & 3, bp);
			peep->item_extra_flags &= ~(1 << cur_container);
			peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;

			peep_update_sprite_type(peep);
		}

		// Place new amount in bin by first clearing the value
		map_element->properties.path.addition_status &= ~(3 << selected_bin);
		// Then placeing the new value.
		map_element->properties.path.addition_status |= rubbish_in_bin << selected_bin;

		map_invalidate_tile_zoom0(peep->next_x, peep->next_y, map_element->base_height << 3, map_element->clearance_height << 3);
		peep_state_reset(peep);
	}
}

/* rct2: 0x006C16D7 */
static void peep_update_heading_to_inspect(rct_peep* peep){
	rct_ride* ride = GET_RIDE(peep->current_ride);

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
		sub_69A98C(peep);
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

		if (!sub_68F3AE(peep))return;

		sub_693C9E(peep);

		if (!(RCT2_GLOBAL(0xF1EE18, uint16) & 0xC))return;

		rct_map_element* map_element = RCT2_GLOBAL(0x00F1EE1A, rct_map_element*);

		if (peep->current_ride !=
			map_element->properties.entrance.ride_index)
			return;

		uint8 exit_index = ((map_element->properties.entrance.index & 0x70) >> 4);

		if (peep->current_ride_station != exit_index)
			return;

		if (RCT2_GLOBAL(0xF1EE18, uint16)&(1 << 3)){
			if (ride->exits[exit_index] != 0xFFFF)return;
		}

		uint8 direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
		peep->var_78 = direction;

		int x = peep->next_x + 16 + RCT2_ADDRESS(0x00981D6C, sint16)[direction * 2] * 53;
		int y = peep->next_y + 16 + RCT2_ADDRESS(0x00981D6E, sint16)[direction * 2] * 53;

		peep->destination_x = x;
		peep->destination_y = y;
		peep->destination_tolerence = 2;
		peep->sprite_direction = direction << 3;

		peep->z = map_element->base_height * 4;
		peep->sub_state = 4;
		// Falls through into sub_state 4
	}

	invalidate_sprite((rct_sprite*)peep);

	sint16 delta_y = abs(peep->y - peep->destination_y);

	sint16 x, y, xy_distance;
	if (!peep_update_action(&x, &y, &xy_distance, peep)){
		peep_decrement_num_riders(peep);
		peep->state = PEEP_STATE_INSPECTING;
		peep->sub_state = 0;
		peep_window_state_update(peep);
		return;
	}

	int z = ride->station_heights[peep->current_ride_station] * 8;

	if (delta_y < 20){
		z += RCT2_ADDRESS(0x0097D21C, uint8)[ride->type * 8];
	}

	sprite_move(x, y, z, (rct_sprite*)peep);
	invalidate_sprite((rct_sprite*)peep);
}

/* rct2: 0x006C0CB8 */
static void peep_update_answering(rct_peep* peep){
	rct_ride* ride = GET_RIDE(peep->current_ride);

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

		sub_693B58(peep);
		invalidate_sprite((rct_sprite*)peep);

		peep->sub_state = 1;
		peep_window_state_update(peep);
		return;
	}
	else if (peep->sub_state == 1){
		if (peep->action == PEEP_ACTION_NONE_2){
			peep->sub_state = 2;
			peep_window_state_update(peep);
			peep->var_74 = 0;
			sub_69A98C(peep);
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

		if (!sub_68F3AE(peep))return;

		sub_693C9E(peep);

		if (!(RCT2_GLOBAL(0xF1EE18, uint16) & 0xC))return;

		rct_map_element* map_element = RCT2_GLOBAL(0x00F1EE1A, rct_map_element*);

		if (peep->current_ride != 
			map_element->properties.entrance.ride_index)
			return;

		uint8 exit_index = ((map_element->properties.entrance.index & 0x70) >> 4);

		if (peep->current_ride_station != exit_index)
			return;

		if (RCT2_GLOBAL(0xF1EE18, uint16)&(1 << 3)){
			if (ride->exits[exit_index] != 0xFFFF)return;
		}

		uint8 direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
		peep->var_78 = direction;

		int x = peep->next_x + 16 + RCT2_ADDRESS(0x00981D6C, sint16)[direction * 2] * 53;
		int y = peep->next_y + 16 + RCT2_ADDRESS(0x00981D6E, sint16)[direction * 2] * 53;

		peep->destination_x = x;
		peep->destination_y = y;
		peep->destination_tolerence = 2;
		peep->sprite_direction = direction << 3;

		peep->z = map_element->base_height * 4;
		peep->sub_state = 4;
		// Falls through into sub_state 4
	}

	invalidate_sprite((rct_sprite*)peep);

	sint16 delta_y = abs(peep->y - peep->destination_y);

	sint16 x, y, xy_distance;
	if (!peep_update_action(&x, &y, &xy_distance, peep)){
		peep_decrement_num_riders(peep);
		peep->state = PEEP_STATE_FIXING;
		peep->sub_state = 0;
		peep_window_state_update(peep);
		return;
	}

	int z = ride->station_heights[peep->current_ride_station] * 8;

	if (delta_y < 20){
		z += RCT2_ADDRESS(0x0097D21C, uint8)[ride->type * 8];
	}

	sprite_move(x, y, z, (rct_sprite*)peep);
	invalidate_sprite((rct_sprite*)peep);
}

/* rct2: 0x006BF483 */
static int peep_update_patrolling_find_watering(rct_peep* peep){
	if (!(peep->staff_orders & STAFF_ORDERS_WATER_FLOWERS))
		return 0;

	uint8 chosen_position = scenario_rand() & 7;
	for (int i = 0; i < 8; ++i, ++chosen_position){
		chosen_position &= 7;

		int x = peep->next_x + TileDirectionDelta[chosen_position].x;
		int y = peep->next_y + TileDirectionDelta[chosen_position].y;

		rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

		do {
			if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_SCENERY){
				continue;
			}

			uint8 z_diff = abs(peep->next_z - map_element->base_height);

			if (z_diff >= 4){
				continue;
			}

			rct_scenery_entry* sceneryEntry = g_smallSceneryEntries[map_element->properties.scenery.type];

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
			peep->destination_x = (peep->x & 0xFFE0) + RCT2_ADDRESS(0x992A5C, uint16)[chosen_position * 2];
			peep->destination_y = (peep->y & 0xFFE0) + RCT2_ADDRESS(0x992A5E, uint16)[chosen_position * 2];
			peep->destination_tolerence = 3;

			return 1;
		} while (!map_element_is_last_for_tile(map_element++));
	}
	return 0;
}

/* rct2: 0x006BF3A1 */
static int peep_update_patrolling_find_bin(rct_peep* peep){
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

	uint8 additions = map_element->properties.path.additions & 0xF;

	if (additions == 0)return 0;

	rct_scenery_entry* sceneryEntry = g_pathBitSceneryEntries[additions - 1];

	if (!(sceneryEntry->path_bit.var_06 & 1))
		return 0;

	if (map_element->flags & MAP_ELEMENT_FLAG_BROKEN)
		return 0;

	if (map_element->properties.path.additions & 0x80)
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
	peep->destination_x = (peep->x & 0xFFE0) + RCT2_ADDRESS(0x992A4C, uint16)[chosen_position * 2];
	peep->destination_y = (peep->y & 0xFFE0) + RCT2_ADDRESS(0x992A4E, uint16)[chosen_position * 2];
	peep->destination_tolerence = 3;
	return 1;
}

/* rct2: 0x006BF322 */
static int peep_update_patrolling_find_grass(rct_peep* peep){
	if (!(peep->staff_orders & STAFF_ORDERS_MOWING))
		return 0;

	if (peep->var_E2 < 12)return 0;

	if ((peep->next_var_29 & 0x18) != 8) return 0;

	rct_map_element* map_element = map_get_surface_element_at(peep->next_x / 32, peep->next_y / 32);

	if ((map_element->properties.surface.terrain & MAP_ELEMENT_SURFACE_TERRAIN_MASK) != TERRAIN_GRASS)
		return 0;

	if (map_element->properties.surface.grass_length < GRASS_LENGTH_CLEAR_1)
		return 0;

	peep_decrement_num_riders(peep);
	peep->state = PEEP_STATE_MOWING;
	peep_window_state_update(peep);
	peep->var_37 = 0;
	peep->destination_x = peep->next_x + RCT2_ADDRESS(0x9929CA, uint16)[0 * 2];
	peep->destination_y = peep->next_y + RCT2_ADDRESS(0x9929CA, uint16)[0 * 2];
	peep->destination_tolerence = 3;
	return 1;
}

/* rct2: 0x006BF295 */
static int peep_update_patrolling_find_sweeping(rct_peep* peep){
	if (!(peep->staff_orders & STAFF_ORDERS_SWEEPING))
		return 0;

	uint16 sprite_id = RCT2_ADDRESS(0xF1EF60, uint16)[((peep->x & 0x1FE0) << 3) | (peep->y >> 5)];

	for (rct_sprite* sprite = NULL;
		sprite_id != 0xFFFF;
		sprite_id = sprite->unknown.next_in_quadrant){

		sprite = &g_sprite_list[sprite_id];

		if (sprite->unknown.linked_list_type_offset != SPRITE_LINKEDLIST_OFFSET_LITTER)continue;

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

/* rct2: 0x006BF1FD */
static void peep_update_patrolling(rct_peep* peep){

	if (!sub_68F3AE(peep))return;

	sub_693C9E(peep);
	if (!(RCT2_GLOBAL(0xF1EE18, uint16) & 1))return;

	if ((peep->next_var_29 & 0x18) == 8){
		rct_map_element* map_element = map_get_surface_element_at(peep->next_x / 32, peep->next_y / 32);

		if (map_element != NULL){
			int water_height = map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
			if (water_height){
				invalidate_sprite((rct_sprite*)peep);
				water_height *= 16;
				sprite_move(peep->x, peep->y, water_height, (rct_sprite*)peep);
				invalidate_sprite((rct_sprite*)peep);

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

/* rct2: 0x0069030A */
static void peep_update_walking(rct_peep* peep){
	if (!sub_68F3AE(peep))return;

	if (peep->flags & PEEP_FLAGS_WAVING){
		if (peep->action >= PEEP_ACTION_NONE_1){
			if ((0xFFFF & scenario_rand()) < 936){
				invalidate_sprite((rct_sprite*)peep);

				peep->action = PEEP_ACTION_WAVE_2;
				peep->action_frame = 0;
				peep->action_sprite_image_offset = 0;

				sub_693B58(peep);
				invalidate_sprite((rct_sprite*)peep);
			}
		}
	}

	if (peep->flags & PEEP_FLAGS_PHOTO){
		if (peep->action >= PEEP_ACTION_NONE_1){
			if ((0xFFFF & scenario_rand()) < 936){
				invalidate_sprite((rct_sprite*)peep);

				peep->action = PEEP_ACTION_TAKE_PHOTO;
				peep->action_frame = 0;
				peep->action_sprite_image_offset = 0;

				sub_693B58(peep);
				invalidate_sprite((rct_sprite*)peep);
			}
		}
	}

	if (peep->flags & PEEP_FLAGS_PAINTING){
		if (peep->action >= PEEP_ACTION_NONE_1){
			if ((0xFFFF & scenario_rand()) < 936){
				invalidate_sprite((rct_sprite*)peep);

				peep->action = PEEP_ACTION_DRAW_PICTURE;
				peep->action_frame = 0;
				peep->action_sprite_image_offset = 0;

				sub_693B58(peep);
				invalidate_sprite((rct_sprite*)peep);
			}
		}
	}

	if (peep->flags & PEEP_FLAGS_LITTER){
		if (!(peep->next_var_29 & 0x18)){
			if ((0xFFFF & scenario_rand()) <= 4096){
				int ebp = (scenario_rand() & 0x3) + 2;
				int x = peep->x + (scenario_rand() & 0x7) - 3;
				int y = peep->y + (scenario_rand() & 0x7) - 3;
				int direction = (scenario_rand() & 0x3);

				litter_create(x, y, peep->z, direction, ebp);
			}
		}
	}
	else if (peep_has_empty_container(peep)){
		if ((!(peep->next_var_29 & 0x18)) && 
			((peep->sprite_index & 0x1FF) == (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 0x1FF))&&
			((0xFFFF & scenario_rand()) <= 4096)){
			
			uint8 pos_stnd = 0;
			for (int container = peep_empty_container_standard_flag(peep); pos_stnd < 32; pos_stnd++)if (container&(1<<pos_stnd))break;

			int bp = 0;

			if (pos_stnd != 32){
				peep->item_standard_flags &= ~(1 << pos_stnd);
				bp = RCT2_ADDRESS(0x97EFCC, uint8)[pos_stnd];
			}
			else{
				uint8 pos_extr = 0;
				for (int container = peep_empty_container_extra_flag(peep); pos_extr < 32; pos_extr++)if (container&(1 << pos_extr))break;
				peep->item_extra_flags &= ~(1 << pos_extr);
				bp = RCT2_ADDRESS(0x97EFE8, uint8)[pos_extr];
			}

			peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
			peep_update_sprite_type(peep);

			int x = peep->x + (scenario_rand() & 0x7) - 3;
			int y = peep->y + (scenario_rand() & 0x7) - 3;
			int direction = (scenario_rand() & 0x3);

			litter_create(x, y, peep->z, direction, bp);
		}
	}

	sub_693C9E(peep);
	if (!(RCT2_GLOBAL(0xF1EE18, uint16) & 1))return;

	if ((peep->next_var_29 & 0x18) == 8){
		rct_map_element* map_element = map_get_surface_element_at(peep->next_x / 32, peep->next_y / 32);
		
		int water_height = map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
		if (water_height){
			invalidate_sprite((rct_sprite*)peep);
			water_height *= 16;
			sprite_move(peep->x, peep->y, water_height, (rct_sprite*)peep);
			invalidate_sprite((rct_sprite*)peep);

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

	if (peep->flags & PEEP_FLAGS_LEAVING_PARK)return;

	if (peep->nausea > 140)return;

	if (peep->happiness < 120)return;

	if (peep->bathroom > 140)return;

	uint16 chance = peep_has_food(peep) ? 13107 : 2849;

	if ((scenario_rand() & 0xFFFF) > chance)return;

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

	uint8 additions = map_element->properties.path.additions & 0xF;

	int ebp = 15;

	if (additions){
		if (!(map_element->properties.path.additions & 0x80)){
			rct_scenery_entry* sceneryEntry = g_pathBitSceneryEntries[additions - 1];

			if (!(sceneryEntry->path_bit.var_06 & 0x2)) ebp = 9;
		}
	}

	int edges = (map_element->properties.path.edges & 0xF) ^ 0xF;
	if (edges == 0) return;

	uint8 chosen_edge = scenario_rand() & 0x3;

	for (; !(edges & (1 << chosen_edge));)chosen_edge = (chosen_edge + 1) & 3;

	uint8 ride_to_view, ride_seat_to_view;
	if (!sub_690B99(peep, chosen_edge, &ride_to_view, &ride_seat_to_view))
		return;

	uint16 sprite_id = RCT2_ADDRESS(0xF1EF60, uint16)[((peep->x & 0x1FE0) << 3) | (peep->y >> 5)];
	for (rct_sprite* sprite; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant){
		sprite = &g_sprite_list[sprite_id];

		if (sprite->unknown.linked_list_type_offset != SPRITE_LINKEDLIST_OFFSET_PEEP)continue;

		if (sprite->peep.state != PEEP_STATE_WATCHING)continue;

		if (peep->z != sprite->peep.z)continue;

		if ((sprite->peep.var_37 & 0x3) != chosen_edge)continue;

		ebp &= ~(1 << ((sprite->peep.var_37 & 0x1C) >> 2));
	}

	if (!ebp)return;

	uint8 chosen_position = scenario_rand() & 0x3;

	for (; !(ebp & (1 << chosen_position));)chosen_position = (chosen_position + 1) & 3;

	peep->current_ride = ride_to_view;
	peep->current_seat = ride_seat_to_view;
	peep->var_37 = chosen_edge | (chosen_position << 2);

	peep_decrement_num_riders(peep);
	peep->state = PEEP_STATE_WATCHING;
	peep_window_state_update(peep);

	peep->sub_state = 0;

	int ebx = peep->var_37 & 0x1F;
	int x = (peep->x & 0xFFE0) + RCT2_ADDRESS(0x981F4C, uint16)[ebx * 2];
	int y = (peep->y & 0xFFE0) + RCT2_ADDRESS(0x981F4E, uint16)[ebx * 2];

	peep->destination_x = x;
	peep->destination_y = y;
	peep->destination_tolerence = 3;

	if (peep->current_seat&1){
		peep_insert_new_thought(peep, PEEP_THOUGHT_NEW_RIDE, 0xFF);
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
	int add_fresh = 1;
	int fresh_thought = -1;
	for (int i = 0; i < PEEP_MAX_THOUGHTS; i++) {
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
					memmove(&peep->thoughts[i], &peep->thoughts[i + 1], sizeof(rct_peep_thought)*(PEEP_MAX_THOUGHTS - i - 1));
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
	unsigned int stepsToTake = peep->energy;
	if (stepsToTake < 95 && peep->state == PEEP_STATE_QUEUING)
		stepsToTake = 95;
	if ((peep->flags & PEEP_FLAGS_SLOW_WALK) && peep->state != PEEP_STATE_QUEUING)
		stepsToTake /= 2;
	if (peep->action == 255 && (peep->next_var_29 & 4)) {
		stepsToTake /= 2;
		if (peep->state == PEEP_STATE_QUEUING)
			stepsToTake += stepsToTake / 2;
	}

	unsigned int carryCheck = peep->var_73 + stepsToTake;
	peep->var_73 = carryCheck;
	if (carryCheck <= 255) {
		// loc_68FD3A
		sub_68FD3A(peep);
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
			//There shouldnt be any more
		default:		
			assert(0);
			break;
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
	uint8* warning_throttle = RCT2_ADDRESS(0x01358750, uint8);

	RCT2_GLOBAL(RCT2_ADDRESS_RIDE_COUNT, sint16) = ride_get_count(); // refactor this to somewhere else

	FOR_ALL_GUESTS(spriteIndex, peep) {
		if (peep->var_2A != 0 || peep->thoughts[0].var_2 > 5)
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
			ride = &g_ride_list[peep->guest_heading_to_ride_id];
			if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
				hunger_counter++;
			break;

		case PEEP_THOUGHT_TYPE_THIRSTY:
			if (peep->guest_heading_to_ride_id == 0xFF){
				thirst_counter++;
				break;
			}
			ride = &g_ride_list[peep->guest_heading_to_ride_id];
			if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_SELLS_DRINKS))
				thirst_counter++;
			break;

		case PEEP_THOUGHT_TYPE_BATHROOM:
			if (peep->guest_heading_to_ride_id == 0xFF){
				bathroom_counter++;
				break;
			}
			ride = &g_ride_list[peep->guest_heading_to_ride_id];
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

	if (gGameSoundsOff)
		return;

	if (!gConfigSound.sound)
		return;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
		return;

	viewport = RCT2_GLOBAL(0x00F438A4, rct_viewport*);
	if (viewport == (rct_viewport*)-1)
		return;

	// Count the number of peeps visible
	visiblePeeps = 0;

	FOR_ALL_GUESTS(spriteIndex, peep) {
		if (peep->sprite_left == (sint16)0x8000)
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
		if (RCT2_GLOBAL(0x009AF5FC, uint32) != 1) {
#ifdef USE_MIXER
			Mixer_Stop_Channel(gCrowdSoundChannel);
			gCrowdSoundChannel = 0;
#else
			sound_channel_stop(2); //RCT2_CALLPROC_1(0x00401A05, int, 2);
#endif
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
#ifdef USE_MIXER
			if (!gCrowdSoundChannel) {
				gCrowdSoundChannel = Mixer_Play_Music(PATH_ID_CSS2, MIXER_LOOP_INFINITE, false);
				if (gCrowdSoundChannel) {
					Mixer_Channel_SetGroup(gCrowdSoundChannel, MIXER_GROUP_NONE);
				}
			}
			if (gCrowdSoundChannel) {
				Mixer_Channel_Volume(gCrowdSoundChannel, DStoMixerVolume(volume));
				RCT2_GLOBAL(0x009AF5FC, uint32) = volume;
			}
#else
			if (sound_channel_load_file2(2, (char*)get_file_path(PATH_ID_CSS2), 0)) {
				sound_channel_play(2, 1, volume, 0, 0);
				RCT2_GLOBAL(0x009AF5FC, uint32) = volume;
			}
#endif
		} else {
			// Alter crowd noise volume
			if (RCT2_GLOBAL(0x009AF5FC, uint32) != volume) {
#ifdef USE_MIXER
				Mixer_Channel_Volume(gCrowdSoundChannel, DStoMixerVolume(volume));
#else
				sound_channel_set_volume(2, volume);//RCT2_CALLPROC_2(0x00401AD3, int, int, 2, volume);
#endif
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
			if (peep->x != (sint16)0x8000) {
				create_balloon(peep->x, peep->y, peep->z + 9, peep->balloon_colour, 0);
				peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
				peep_update_sprite_type(peep);
			}
		}

		// Clap
		if ((peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_QUEUING) && peep->action >= 254) {
			peep->action = PEEP_ACTION_CLAP;
			peep->action_frame = 0;
			peep->action_sprite_image_offset = 0;
			sub_693B58(peep);
			invalidate_sprite((rct_sprite*)peep);
		}
	}

	// Play applause noise
	sound_play_panned(SOUND_APPLAUSE, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) / 2, 0, 0, 0);
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
		if (peep->var_2A == 0 && peep->state == PEEP_STATE_QUEUING) {
			if (peep->days_in_queue < 255) {
				peep->days_in_queue += 1;
			}
		}
	}
}

/**
 *
 *  rct2: 0x0069A05D
 */
rct_peep *peep_generate(int x, int y, int z)
{
	if (RCT2_GLOBAL(0x13573C8, uint16) < 400)
		return NULL;

	rct_peep* peep = (rct_peep*)create_sprite(1);

	move_sprite_to_list((rct_sprite*)peep, SPRITE_LINKEDLIST_OFFSET_PEEP);

	peep->sprite_identifier = 1;
	peep->sprite_type = 0;
	peep->var_2A = 1;
	peep->state = PEEP_STATE_FALLING;
	peep->action = PEEP_ACTION_NONE_2;
	peep->var_6D = 0;
	peep->action_sprite_image_offset = 0;
	peep->no_action_frame_no = 0;
	peep->action_sprite_type = 0;
	peep->flags = 0;
	peep->favourite_ride = 0xFF;
	peep->var_FA = 0;

	uint8* edx = RCT2_ADDRESS(0x98270C, uint8*)[peep->sprite_type * 2];
	peep->sprite_width = edx[peep->action_sprite_type * 4];
	peep->sprite_height_negative = edx[peep->action_sprite_type * 4 + 1];
	peep->sprite_height_positive = edx[peep->action_sprite_type * 4 + 2];

	peep->sprite_direction = 0;

	sprite_move(x, y, z, (rct_sprite*)peep);
	invalidate_sprite((rct_sprite*)peep);

	peep->var_41 = (scenario_rand() & 0x1F) + 45;
	peep->var_C4 = 0;
	peep->var_79 = 0xFF;
	peep->type = PEEP_TYPE_GUEST;
	peep->previous_ride = 0xFF;
	peep->thoughts->type = PEEP_THOUGHT_TYPE_NONE;
	peep->window_invalidate_flags = 0;

	uint8 al = (scenario_rand() & 0x7) + 3;
	uint8 ah = min(al, 7) - 3;

	if (al >= 7) al = 15;

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PREF_LESS_INTENSE_RIDES){
		ah = 0;
		al = 4;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PREF_MORE_INTENSE_RIDES){
		ah = 9;
		al = 15;
	}

	peep->intensity = (al << 4) | ah;

	uint8 nausea_tolerance = scenario_rand() & 0x7;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PREF_MORE_INTENSE_RIDES){
		nausea_tolerance += 4;
	}

	peep->nausea_tolerance = RCT2_ADDRESS(0x009823A0, uint8)[nausea_tolerance];

	sint8 happiness = (scenario_rand() & 0x1F) - 15 + RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_HAPPINESS, uint8);

	if (RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_HAPPINESS, uint8) == 0)
		happiness += 0x80;

	peep->happiness = happiness;
	peep->happiness_growth_rate = happiness;
	peep->nausea = 0;
	peep->nausea_growth_rate = 0;

	sint8 hunger = (scenario_rand() & 0x1F) - 15 + RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_HUNGER, uint8);

	peep->hunger = hunger;

	sint8 thirst = (scenario_rand() & 0x1F) - 15 + RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_THIRST, uint8);

	peep->thirst = thirst;

	peep->bathroom = 0;
	peep->var_42 = 0;
	memset(&peep->rides_been_on, 0, 32);

	peep->no_of_rides = 0;
	memset(&peep->ride_types_been_on, 0, 16);
	peep->id = RCT2_GLOBAL(0x013B0E6C, uint32)++;
	peep->name_string_idx = 767;

	money32 cash = (scenario_rand() & 0x3) * 100 - 100 + RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_CASH, money16);
	if (cash < 0) cash = 0;

	if (RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_CASH, money16) == 0){
		cash = 500;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY){
		cash = 0;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_GUEST_INITIAL_CASH, money16) == 0xFFFF){
		cash = 0;
	}

	peep->cash_in_pocket = cash;
	peep->cash_spent = 0;
	peep->time_in_park = -1;
	peep->var_CC.x = 0xFF;
	peep->var_CC.y = 0xFF;
	peep->var_CC.z = 0xFF;
	peep->var_CC.direction = 0xFF;
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
	peep->var_F3 = 0;
	peep->var_F4 = 0;

	uint8 tshirt_colour = scenario_rand() % 33;
	peep->tshirt_colour = RCT2_ADDRESS(0x009823D5, uint8)[tshirt_colour];

	uint8 trousers_colour = scenario_rand() % 25;
	peep->trousers_colour = RCT2_ADDRESS(0x009823BC, uint8)[trousers_colour];

	uint8 energy = (scenario_rand() & 0x3F) + 65;
	peep->energy = energy;
	peep->energy_growth_rate = energy;

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_SHOW_REAL_GUEST_NAMES){
		peep_give_real_name(peep);
	}
	peep_update_name_sort(peep);

	RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_HEADING_FOR_PARK, uint16)++;

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
	rct_ride ride;

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
		ride = g_ride_list[peep->current_ride];
		if (ride_type_has_flag(ride.type, RIDE_TYPE_FLAG_IN_RIDE))
			*argument_1 = STR_IN_RIDE;
		*argument_1 |= (ride.name << 16);
		*argument_2 = ride.name_arguments;
		break;
	case PEEP_STATE_BUYING:
		ride = g_ride_list[peep->current_ride];
		*argument_1 = STR_AT_RIDE | (ride.name << 16);
		*argument_2 = ride.name_arguments;
		break;
	case PEEP_STATE_WALKING:
	case PEEP_STATE_USING_BIN:
		if (peep->guest_heading_to_ride_id != 0xFF){
			ride = g_ride_list[peep->guest_heading_to_ride_id];
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
		if (peep->sub_state == 0){
			*argument_1 = STR_WALKING;
			*argument_2 = 0;
		}
		else if (peep->sub_state == 1){
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

const int face_sprite_small[] = {
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

const int face_sprite_large[] = {
	SPR_PEEP_LARGE_FACE_ANGRY,
	SPR_PEEP_LARGE_FACE_VERY_VERY_SICK,
	SPR_PEEP_LARGE_FACE_VERY_SICK,
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

int get_face_sprite_offset(rct_peep *peep){

	// ANGRY
	if (peep->var_F3) return PEEP_FACE_OFFSET_ANGRY;

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

	int offset = PEEP_FACE_OFFSET_VERY_VERY_UNHAPPY;
	//There are 7 different happiness based faces
	for (int i = 37; peep->happiness >= i; i += 37)
	{
		offset++;
	}

	return offset;
}

/**
 *  Function split into large and small sprite
 *  rct2: 0x00698721
 */
int get_peep_face_sprite_small(rct_peep *peep){
	return face_sprite_small[get_face_sprite_offset(peep)];
}

/**
 *  Function split into large and small sprite
 *  rct2: 0x00698721
 */
int get_peep_face_sprite_large(rct_peep *peep){
	return face_sprite_large[get_face_sprite_offset(peep)];
}

/**
 *
 *  rct2: 0x0069A5A0
 * tests if a peep's name matches a cheat code, normally returns using a register flag
 * @param index (eax)
 * @param ride (esi)
 */
int peep_check_easteregg_name(int index, rct_peep *peep)
{
	char buffer[256];

	format_string(buffer, peep->name_string_idx, &peep->id);
	return _stricmp(buffer, gPeepEasterEggNames[index]) == 0;
}

int peep_get_easteregg_name_id(rct_peep *peep)
{
	char buffer[256];
	int i;
	
	format_string(buffer, peep->name_string_idx, &peep->id);

	for (i = 0; i < countof(gPeepEasterEggNames); i++)
		if (_stricmp(buffer, gPeepEasterEggNames[i]) == 0)
			return i;

	return -1;

}

int peep_is_mechanic(rct_peep *peep)
{
	return (
		peep->sprite_identifier == SPRITE_IDENTIFIER_PEEP &&
		peep->type == PEEP_TYPE_STAFF &&
		peep->staff_type == STAFF_TYPE_MECHANIC
	);
}

static int peep_has_food_standard_flag(rct_peep* peep){
	return peep->item_standard_flags &(
		PEEP_ITEM_DRINK |
		PEEP_ITEM_BURGER |
		PEEP_ITEM_FRIES |
		PEEP_ITEM_ICE_CREAM |
		PEEP_ITEM_COTTON_CANDY |
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

static int peep_has_food_extra_flag(rct_peep* peep){
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

/* To simplify check of 0x36BA3E0 and 0x11FF78 
 * returns 0 on no food.
 */
int peep_has_food(rct_peep* peep){
	return peep_has_food_standard_flag(peep) ||
		peep_has_food_extra_flag(peep);
}

static int peep_has_drink_standard_flag(rct_peep* peep){
	return peep->item_standard_flags &(
		PEEP_ITEM_DRINK |
		PEEP_ITEM_COFFEE |
		PEEP_ITEM_LEMONADE);
}

static int peep_has_drink_extra_flag(rct_peep* peep){
	return peep->item_extra_flags &(
		PEEP_ITEM_CHOCOLATE |
		PEEP_ITEM_ICED_TEA |
		PEEP_ITEM_FRUIT_JUICE |
		PEEP_ITEM_SOYBEAN_MILK |
		PEEP_ITEM_SU_JONGKWA
		);
}

/* To simplify check of NOT(0x12BA3C0 and 0x118F48)
 * returns 0 on no food.
 */
static int peep_has_drink(rct_peep* peep){
	return peep_has_drink_standard_flag(peep) ||
		peep_has_drink_extra_flag(peep);
}

static int peep_empty_container_standard_flag(rct_peep* peep){
	return peep->item_standard_flags &(
		PEEP_ITEM_EMPTY_CAN |
		PEEP_ITEM_EMPTY_BURGER_BOX |
		PEEP_ITEM_EMPTY_CUP |
		PEEP_ITEM_RUBBISH |
		PEEP_ITEM_EMPTY_BOX |
		PEEP_ITEM_EMPTY_BOTTLE
		);
}

static int peep_empty_container_extra_flag(rct_peep* peep){
	return peep->item_extra_flags &(
		PEEP_ITEM_EMPTY_BOWL_RED |
		PEEP_ITEM_EMPTY_DRINK_CARTON |
		PEEP_ITEM_EMPTY_JUICE_CUP |
		PEEP_ITEM_EMPTY_BOWL_BLUE
		);
}

static int peep_has_empty_container(rct_peep* peep){
	return peep_empty_container_standard_flag(peep) ||
		peep_empty_container_extra_flag(peep);
}

/* Simplifies 0x690582. Returns 1 if should find bench*/
static int peep_should_find_bench(rct_peep* peep){
	if (!(peep->flags & PEEP_FLAGS_LEAVING_PARK)){
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
 * rct2: 0x699F5A
 * al:thought_type
 * ah:thought_arguments
 * esi: peep
 */
void peep_insert_new_thought(rct_peep *peep, uint8 thought_type, uint8 thought_arguments){
	uint8 action = RCT2_ADDRESS(0x981DB0, uint16)[thought_type] & 0xFF;

	if (action != 0xFF && peep->action >= 254){
			peep->action = action;
			peep->action_frame = 0;
			peep->action_sprite_image_offset = 0;
			sub_693B58(peep);
			invalidate_sprite((rct_sprite*)peep);
	}
	
	for (int i = 0; i < PEEP_MAX_THOUGHTS; ++i){
		rct_peep_thought* thought = &peep->thoughts[i];
		// Remove the oldest thought by setting it to NONE.
		if (thought->type == PEEP_THOUGHT_TYPE_NONE) break;

		if (thought->type == thought_type && thought->item == thought_arguments){
			// If the thought type has not changed then we need to move
			// it to the top of the thought list. This is done by first removing the
			// existing thought and placing it at the top.
			memmove(thought, thought + 1, sizeof(rct_peep_thought)*(PEEP_MAX_THOUGHTS - i - 1));
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

/* rct2: 0x00699FE3 
 * Stops peeps that are having thoughts
 * such as "I'm hungry" after visiting a food shop.
 * Works for Thirst/Hungry/Low Money/Bathroom
 */
static void peep_stop_purchase_thought(rct_peep* peep, uint8 ride_type){

	uint8 thought_type = PEEP_THOUGHT_TYPE_HUNGRY;

	if (!(RCT2_ADDRESS(0x97CF40, uint32)[ride_type * 2] & 0x800000)){
		thought_type = PEEP_THOUGHT_TYPE_THIRSTY;
		if (!(RCT2_ADDRESS(0x97CF40, uint32)[ride_type * 2] & 0x1000000)){
			thought_type = PEEP_THOUGHT_RUNNING_OUT;
			if (ride_type != RIDE_TYPE_CASH_MACHINE){
				thought_type = PEEP_THOUGHT_TYPE_BATHROOM;
				if (!(RCT2_ADDRESS(0x97CF40, uint32)[ride_type * 2] & 0x2000000)){
					return;
				}
			}
		}
	}

	//Remove the related thought
	for (int i = 0; i < PEEP_MAX_THOUGHTS; ++i){
		rct_peep_thought* thought = &peep->thoughts[i];

		if (thought->type == PEEP_THOUGHT_TYPE_NONE) break;

		if (thought->type != thought_type)continue;

		memmove(thought, thought + 1, sizeof(rct_peep_thought)*(PEEP_MAX_THOUGHTS - i - 1));

		peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;

		peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
		i--;
	}
}

void peep_set_map_tooltip(rct_peep *peep)
{
	if (peep->type == PEEP_TYPE_GUEST) {
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = peep->flags & PEEP_FLAGS_TRACKING ? 1450 : 1449;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint32) = get_peep_face_sprite_small(peep);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 6, uint16) = peep->name_string_idx;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 8, uint32) = peep->id;

		uint32 arg0, arg1;
		get_arguments_from_action(peep, &arg0, &arg1);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 12, uint32) = arg0;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 16, uint32) = arg1;
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1451;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = peep->name_string_idx;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 4, uint32) = peep->id;

		uint32 arg0, arg1;
		get_arguments_from_action(peep, &arg0, &arg1);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 8, uint32) = arg0;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 12, uint32) = arg1;
	}
}


void sub_693BAB(rct_peep* peep) {
	uint8 bl = peep->var_6F;
	if (bl != peep->action_sprite_type) {
		invalidate_sprite((rct_sprite*)peep);
		peep->action_sprite_type = bl;
		uint8* edx = RCT2_ADDRESS(0x98270C, uint8*)[peep->sprite_type * 2];
		peep->sprite_width = edx[bl * 4];
		peep->sprite_height_negative = edx[bl * 4 + 1];
		peep->sprite_height_positive = edx[bl * 4 + 2];
		invalidate_sprite((rct_sprite*)peep);
	}
}

/**
 * 
 *  rct2: 0x00693CBB
 */
static int peep_update_queue_position(rct_peep* peep){
	peep->time_in_queue++;
	if (peep->next_in_queue == 0xFFFF)
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

		switch (peep->sprite_direction / 8){
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
	peep->var_6F = 2;
	if (RCT2_GLOBAL(0x00F1AEF1, uint8) != 0xFE)
		invalidate_sprite((rct_sprite*)peep);
	return 1;
}

/* rct2: 0x00693EF2 */
static int peep_return_to_center_of_tile(rct_peep* peep){
	peep->var_78 ^= (1 << 1);
	peep->destination_x = (peep->x & 0xFFE0) + 16;
	peep->destination_y = (peep->y & 0xFFE0) + 16;
	peep->destination_tolerence = 5;
	return 1;
}

/* rct2: 0x00693f2C*/
static int peep_interact_with_entrance(rct_peep* peep, sint16 x, sint16 y, rct_map_element* map_element){
	uint8 entranceType = map_element->properties.entrance.type;
	uint8 rideIndex = map_element->properties.entrance.ride_index;

	if (entranceType == ENTRANCE_TYPE_RIDE_EXIT){
		RCT2_GLOBAL(0x00F1EE18, uint8) |= (1 << 2);
		RCT2_GLOBAL(0x00F1EE1A, rct_map_element*) = map_element;
	}
	else if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE){
		RCT2_GLOBAL(0x00F1EE18, uint8) |= (1 << 3);
		RCT2_GLOBAL(0x00F1EE1A, rct_map_element*) = map_element;
	}

	if (entranceType == ENTRANCE_TYPE_RIDE_EXIT){
		peep->var_79 = 0xFF;
		return peep_return_to_center_of_tile(peep);
	}

	if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE){
		if (peep->type == PEEP_TYPE_STAFF){
			peep->var_79 = 0xFF;
			return peep_return_to_center_of_tile(peep);
		}

		if (peep->state == PEEP_STATE_QUEUING){
			peep->sub_state = 11;
			peep->action_sprite_image_offset = RCT2_GLOBAL(0x00F1AEF0, uint8);
			return 1;
		}

		if (peep->var_79 == rideIndex)
			return peep_return_to_center_of_tile(peep);

		peep->var_F4 = 0;
		uint8 stationNum = (map_element->properties.entrance.index >> 4) & 0x7;
		if (!sub_6960AB(peep, rideIndex, stationNum, 0)){
			peep->var_79 = rideIndex;
			return peep_return_to_center_of_tile(peep);
		}

		peep->action_sprite_image_offset = RCT2_GLOBAL(0x00F1AEF0, uint8);
		peep->var_79 = rideIndex;

		rct_ride* ride = GET_RIDE(rideIndex);
		uint16 previous_first = ride->first_peep_in_queue[stationNum];
		ride->first_peep_in_queue[stationNum] = peep->sprite_index;
		peep->next_in_queue = previous_first;
		ride->queue_length[stationNum]++;
		
		peep_decrement_num_riders(peep);
		peep->current_ride = rideIndex;
		peep->current_ride_station = stationNum;
		peep->state = PEEP_STATE_QUEUING;
		peep->days_in_queue = 0;
		peep_window_state_update(peep);
		peep->sub_state = 11;
		peep->time_in_queue = 0;
		if (peep->flags & PEEP_FLAGS_TRACKING){
			RCT2_GLOBAL(0x0013CE952, rct_string_id) = peep->name_string_idx;
			RCT2_GLOBAL(0x0013CE954, uint32) = peep->id;
			RCT2_GLOBAL(0x0013CE958, rct_string_id) = ride->name;
			RCT2_GLOBAL(0x0013CE95A, uint32) = ride->name_arguments;
			news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, 1931, peep->sprite_index);
		}
		return 1;
	}
	else{
		// PARK_ENTRANCE
		if (peep->type == PEEP_TYPE_STAFF)
			return  peep_return_to_center_of_tile(peep);

		// If not the center of the entrance arch
		if (map_element->properties.entrance.index & 0xF)
			return peep_return_to_center_of_tile(peep);

		uint8 entranceDirection = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
		if (entranceDirection != peep->var_78){
			if ((entranceDirection ^ (1 << 1)) != peep->var_78)
				return peep_return_to_center_of_tile(peep);
			// Peep is leaving the park.
			if (peep->state != PEEP_STATE_WALKING)
				return peep_return_to_center_of_tile(peep);

			if (!(peep->flags & PEEP_FLAGS_LEAVING_PARK)){
				// If the park is open and leaving flag isnt set return to center
				if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_OPEN)
					return peep_return_to_center_of_tile(peep);
			}

			peep->destination_x += TileDirectionDelta[peep->var_78].x;
			peep->destination_y += TileDirectionDelta[peep->var_78].y;
			peep->destination_tolerence = 9;
			invalidate_sprite((rct_sprite*)peep);
			sprite_move(x, y, peep->z, (rct_sprite*)peep);
			invalidate_sprite((rct_sprite*)peep);

			peep_decrement_num_riders(peep);
			peep->state = PEEP_STATE_LEAVING_PARK;
			peep_window_state_update(peep);

			peep->var_37 = 0;
			if (peep->flags & PEEP_FLAGS_TRACKING){
				RCT2_GLOBAL(0x0013CE952, rct_string_id) = peep->name_string_idx;
				RCT2_GLOBAL(0x0013CE954, uint32) = peep->id;
				news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, 1935, peep->sprite_index);
			}
			return 1;
		}

		// Peep is entering the park.

		if (peep->state != PEEP_STATE_ENTERING_PARK)
			return peep_return_to_center_of_tile(peep);

		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_OPEN)){
			peep->state = PEEP_STATE_LEAVING_PARK;
			peep->var_37 = 1;
			RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_HEADING_FOR_PARK, uint16)--;
			peep_window_state_update(peep);
			return peep_return_to_center_of_tile(peep);
		}

		uint8 entranceIndex = 0;
		while (1){
			if (RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, sint16)[entranceIndex] == (x & 0xFFE0) &&
				RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Y, sint16)[entranceIndex] == (y & 0xFFE0))
				break;
			entranceIndex++;
		}

		sint16 z = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Z, sint16)[entranceIndex] / 8;
		entranceDirection = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_DIRECTION, uint8)[entranceIndex];

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
			RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_HEADING_FOR_PARK, uint16)--;
			peep_window_state_update(peep);
			return peep_return_to_center_of_tile(peep);
		}

		money16 entranceFee = RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, money16);
		if (entranceFee != 0 && !(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)){
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
				RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_HEADING_FOR_PARK, uint16)--;
				peep_window_state_update(peep);
				return peep_return_to_center_of_tile(peep);
			}

			RCT2_GLOBAL(RCT2_ADDRESS_INCOME_FROM_ADMISSIONS, money32) += entranceFee;
			RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_PARK_ENTRANCE_TICKETS * 4;
			peep_spend_money(peep, &peep->paid_to_enter, entranceFee);
			peep->flags |= PEEP_FLAGS_5;
		}

		RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_ADMISSIONS, uint32)++;
		window_invalidate_by_number(WC_PARK_INFORMATION, 0);

		peep->var_37 = 1;
		peep->destination_x += TileDirectionDelta[peep->var_78].x;
		peep->destination_y += TileDirectionDelta[peep->var_78].y;
		peep->destination_tolerence = 7;

		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, peep->z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);

		return 1;
	}
}

/* rct2: 0x006946D8 */
static int peep_footpath_move_forward(rct_peep* peep, sint16 x, sint16 y, rct_map_element* map_element, bool vandalism){
	peep->next_x = (x & 0xFFE0);
	peep->next_y = (y & 0xFFE0);
	peep->next_z = map_element->base_height;
	peep->next_var_29 = map_element->properties.path.type & 7;
	
	sint16 z = peep_get_height_on_slope(peep, x, y);

	if (peep->type == PEEP_TYPE_STAFF){
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return 1;
	}

	uint8 var_EF = (peep->var_EF * 2) & 0x3F;
	peep->var_EF &= 0xC0;
	peep->var_EF |= var_EF;

	if (vandalism == true){
		peep->var_EF |= 1;
		if (peep->var_EF & 0x3E && 
			!(peep->var_EF & 0xC0)){

			if ((scenario_rand() & 0xFFFF) <= 10922){
				peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_VANDALISM, 0xFF);
				peep->happiness_growth_rate = max(0, peep->happiness_growth_rate - 17);
			}
			peep->var_EF |= 0xC0;
		}
	}

	if (peep->var_EF & 0xC0 &&
		(scenario_rand()&0xFFFF) <= 4369){
		peep->var_EF -= 0x40;
	}
	
	uint16 crowded = 0;
	uint8 litter_count = 0;
	uint8 sick_count = 0;
	uint16 sprite_id = RCT2_ADDRESS(0xF1EF60, uint16)[((x & 0x1FE0) << 3) | (y >> 5)];
	for (rct_sprite* sprite; sprite_id != 0xFFFF; sprite_id = sprite->unknown.next_in_quadrant){
		sprite = &g_sprite_list[sprite_id];
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
			if (litter->type > 1)
				continue;

			litter_count--;
			sick_count++;
		}
	}

	if (crowded >= 10 && 
		peep->state == PEEP_STATE_WALKING &&
		(scenario_rand() & 0xFFFF) <= 21845){

		peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CROWDED, 0xFF);
		peep->happiness_growth_rate = max(0, peep->happiness_growth_rate - 14);
	}

	litter_count = min(3, litter_count);
	sick_count = min(3, sick_count);

	uint8 disgusting_time = peep->disgusting_count & 0xC0;
	uint8 disgusting_count = ((peep->disgusting_count & 0xF) << 2) | sick_count;
	peep->disgusting_count = disgusting_count | disgusting_time;

	if (disgusting_time & 0xC0 &&
		(scenario_rand() & 0xFFFF) <= 4369){
		// Reduce the disgusting time
		peep->disgusting_count -= 0x40;
	}
	else{
		uint8 total_sick = 0;
		for (uint8 time = 0; time < 3; time++){
			total_sick += (disgusting_count >> (2 * time)) & 0x3;
		}
		
		if (total_sick >= 3 &&
			(scenario_rand() & 0xFFFF) <= 10922){
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
		(scenario_rand() & 0xFFFF) <= 4369){
		// Reduce the litter time
		peep->litter_count -= 0x40;
	}
	else{
		uint8 total_litter = 0;
		for (uint8 time = 0; time < 3; time++){
			total_litter += (litter_count >> (2 * time)) & 0x3;
		}

		if (total_litter >= 3 &&
			(scenario_rand() & 0xFFFF) <= 10922){
			peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_BAD_LITTER, 0xFF);
			peep->happiness_growth_rate = max(0, peep->happiness_growth_rate - 17);
			// Reset litter time
			peep->litter_count |= 0xC0;
		}
	}

	invalidate_sprite((rct_sprite*)peep);
	sprite_move(x, y, z, (rct_sprite*)peep);
	invalidate_sprite((rct_sprite*)peep);
	return 1;
}

/* rct2: 0x0069455E */
static int peep_interact_with_path(rct_peep* peep, sint16 x, sint16 y, rct_map_element* map_element){

	// 0x00F1AEE2
	bool vandalism_present = false;
	if ((map_element->properties.path.additions & 0xF) != 0 &&
		(map_element->flags & MAP_ELEMENT_FLAG_BROKEN) &&
		(map_element->properties.path.edges & 0xF) != 0xF){
		vandalism_present = 1;
	}

	sint16 z = map_element->base_height * 8;
	if (!map_is_location_owned(x, y, z)){
		if (peep->var_2A == 0)
			return peep_return_to_center_of_tile(peep);
	}
	else{
		if (peep->var_2A == 1)
			return peep_return_to_center_of_tile(peep);
	}

	if (peep->type == PEEP_TYPE_GUEST &&
		footpath_element_is_queue(map_element)){

		uint8 rideIndex = map_element->properties.path.ride_index;

		if (rideIndex == 0xFF){
			peep->var_79 = 0xFF;
			return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
		}

		if (peep->state == PEEP_STATE_QUEUING){
			if (peep->current_ride == rideIndex){
				return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
			}
			remove_peep_from_queue(peep);
			peep_decrement_num_riders(peep);
			peep->state = PEEP_STATE_1;
			peep_window_state_update(peep);
			return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
		}

		if (peep->var_79 == rideIndex){
			return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
		}

		peep->var_F4 = 0;
		uint8 stationNum = (map_element->properties.path.additions & 0x70) >> 4;
		if (!sub_6960AB(peep, rideIndex, stationNum, 1)){
			peep->var_79 = rideIndex;
			return peep_return_to_center_of_tile(peep);
		}

		peep->var_79 = rideIndex;
		rct_ride* ride = GET_RIDE(rideIndex);

		uint16 old_first_peep = ride->first_peep_in_queue[stationNum];
		ride->first_peep_in_queue[stationNum] = peep->sprite_index;		
		peep->next_in_queue = old_first_peep;
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
		if (peep->flags & PEEP_FLAGS_TRACKING){
			RCT2_GLOBAL(0x0013CE952, rct_string_id) = peep->name_string_idx;
			RCT2_GLOBAL(0x0013CE954, uint32) = peep->id;
			RCT2_GLOBAL(0x0013CE958, rct_string_id) = ride->name;
			RCT2_GLOBAL(0x0013CE95A, uint32) = ride->name_arguments;
			news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, 1931, peep->sprite_index);
		}

		return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
	}
	else{
		peep->var_79 = 0xFF;
		if (peep->state == PEEP_STATE_QUEUING){
			remove_peep_from_queue(peep);
			peep_decrement_num_riders(peep);
			peep->state = PEEP_STATE_1;
			peep_window_state_update(peep);
		}
		return peep_footpath_move_forward(peep, x, y, map_element, vandalism_present);
	}
}

/* rct2: 0x00693F70 */
static int peep_interact_with_shop(rct_peep* peep, sint16 x, sint16 y, rct_map_element* map_element){
	uint8 rideIndex = map_element->properties.track.ride_index;
	rct_ride* ride = GET_RIDE(rideIndex);

	if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
		return 0;

	if (peep->type == PEEP_TYPE_STAFF)
		return peep_return_to_center_of_tile(peep);
	
	peep->var_F4 = 0;
	
	if (ride->status != RIDE_STATUS_OPEN)
		return peep_return_to_center_of_tile(peep);

	if (peep->var_79 == rideIndex)
		return peep_return_to_center_of_tile(peep);

	if (peep->flags & PEEP_FLAGS_LEAVING_PARK)
		return peep_return_to_center_of_tile(peep);

	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY)){
		peep->var_F4 = 0;
		if (!sub_6960AB(peep, rideIndex, 0, 0))
			return peep_return_to_center_of_tile(peep);
		
		money16 cost = ride->price;
		if (cost != 0){
			ride->total_profit += cost;
			ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
			RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS * 4;
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
		ride->var_120++;
		if (peep->flags & PEEP_FLAGS_TRACKING){
			RCT2_GLOBAL(0x0013CE952, rct_string_id) = peep->name_string_idx;
			RCT2_GLOBAL(0x0013CE954, uint32) = peep->id;
			RCT2_GLOBAL(0x0013CE958, rct_string_id) = ride->name;
			RCT2_GLOBAL(0x0013CE95A, uint32) = ride->name_arguments;
			rct_string_id string_id = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE) ? 1933 : 1932;
			news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, string_id, peep->sprite_index);
		}
		return 1;
	}
	else{
		if (peep->guest_heading_to_ride_id == rideIndex)
			peep->guest_heading_to_ride_id = 0xFF;
		peep->action_sprite_image_offset = RCT2_GLOBAL(0x00F1AEF0, uint8);
		peep_decrement_num_riders(peep);
		peep->current_ride = rideIndex;
		peep->state = PEEP_STATE_BUYING;
		peep->sub_state = 0;
		peep_window_state_update(peep);
		return 1;
	}
}

/* rct2: 0x0069524E */
static int peep_move_one_tile(uint8 direction, rct_peep* peep){
	sint16 x = peep->next_x;
	sint16 y = peep->next_y;
	x += TileDirectionDelta[direction].x;
	y += TileDirectionDelta[direction].y;

	if (x >= 8192 || y >= 8192){
		// This could loop!
		return guest_surface_path_finding(peep);
	}

	peep->var_78 = direction;
	peep->destination_x = x + 16;
	peep->destination_y = y + 16;
	peep->destination_tolerence = 2;
	if (peep->state == PEEP_STATE_QUEUING){
		peep->destination_tolerence = (scenario_rand() & 7) + 2;
	}
	return 0;
}

/* rct2: 0x00694C41 */
static int guest_surface_path_finding(rct_peep* peep){
	sint16 x = peep->next_x;
	sint16 y = peep->next_y;
	sint16 z = peep->next_z;
	uint8 randDirection = scenario_rand() & 3;

	if (!fence_in_the_way(x, y, z, z + 4, randDirection)){
		x += TileDirectionDelta[randDirection].x;
		y += TileDirectionDelta[randDirection].y;
		randDirection ^= (1 << 1);

		if (!fence_in_the_way(x, y, z, z + 4, randDirection)){
			randDirection ^= (1 << 1);
			if (!map_surface_is_blocked(x, y)){
				return peep_move_one_tile(randDirection, peep);
			}
		}
	}

	randDirection++;
	uint8 rand_backwards = scenario_rand() & 1;
	if (rand_backwards){
		randDirection -= 2;
	}
	randDirection &= 3;

	if (!fence_in_the_way(x, y, z, z + 4, randDirection)){
		x += TileDirectionDelta[randDirection].x;
		y += TileDirectionDelta[randDirection].y;
		randDirection ^= (1 << 1);

		if (!fence_in_the_way(x, y, z, z + 4, randDirection)){
			randDirection ^= (1 << 1);
			if (!map_surface_is_blocked(x, y)){
				return peep_move_one_tile(randDirection, peep);
			}
		}
	}

	randDirection -= 2;
	randDirection &= 3;

	if (!fence_in_the_way(x, y, z, z + 4, randDirection)){
		x += TileDirectionDelta[randDirection].x;
		y += TileDirectionDelta[randDirection].y;
		randDirection ^= (1 << 1);

		if (!fence_in_the_way(x, y, z, z + 4, randDirection)){
			randDirection ^= (1 << 1);
			if (!map_surface_is_blocked(x, y)){
				return peep_move_one_tile(randDirection, peep);
			}
		}
	}

	randDirection--;
	if (rand_backwards){
		randDirection += 2;
	}
	return peep_move_one_tile(randDirection, peep);
}

rct_map_element* get_banner_on_path(rct_map_element *path_element){
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

static bool is_valid_path_z_and_direction(rct_map_element *mapElement, int currentZ, int currentDirection)
{
	if (footpath_element_is_sloped(mapElement)) {
		int slopeDirection = footpath_element_get_slope_direction(mapElement);
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
 *  rct2: 0x00694BAE
 */
static uint8 sub_694BAE(sint16 x, sint16 y, sint16 z, rct_map_element *mapElement, uint8 chosenDirection)
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
		if (map_element_get_type(nextMapElement) != MAP_ELEMENT_TYPE_PATH) continue;
		if (!is_valid_path_z_and_direction(nextMapElement, z, chosenDirection)) continue;

		if (nextMapElement->type & 2) return 6;

		return 0;
	} while (!map_element_is_last_for_tile(nextMapElement++));

	return 0;
}

/**
 *
 *  rct2: 0x006949B9
 */
static uint8 loc_6949B9(
	sint16 x, sint16 y, sint16 z, rct_map_element *inputMapElement, uint8 chosenDirection, uint8 *outRideIndex,
	int level
) {
	rct_map_element *mapElement;
	int direction;

	if (level > 25) return 5;

	x += TileDirectionDelta[chosenDirection].x;
	y += TileDirectionDelta[chosenDirection].y;
	mapElement = map_get_first_element_at(x / 32, y / 32);
	do {
		if (mapElement->flags & MAP_ELEMENT_FLAG_GHOST) continue;

		switch (map_element_get_type(mapElement)) {
		case MAP_ELEMENT_TYPE_TRACK:
			if (z != mapElement->base_height) continue;
			int rideIndex = inputMapElement->properties.path.ride_index;
			rct_ride *ride = GET_RIDE(rideIndex);
			if (RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x20000) {
				*outRideIndex = rideIndex;
				return 2;
			}
			break;
		case MAP_ELEMENT_TYPE_ENTRANCE:
			if (z != mapElement->base_height) continue;
			switch (mapElement->properties.entrance.type) {
			case ENTRANCE_TYPE_RIDE_ENTRANCE:
				direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
				if (direction == chosenDirection) {
					*outRideIndex = mapElement->properties.entrance.ride_index;
					return 2;
				}
				break;
			case ENTRANCE_TYPE_RIDE_EXIT:
				direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
				if (direction == chosenDirection) {
					*outRideIndex = mapElement->properties.entrance.ride_index;
					return 1;
				}
				break;
			case ENTRANCE_TYPE_PARK_ENTRANCE:
				return 4;
			}
			break;
		case MAP_ELEMENT_TYPE_PATH:
			if (!is_valid_path_z_and_direction(mapElement, z, chosenDirection)) continue;
			if (!(mapElement->type & 2)) return 6;

			uint8 edges = mapElement->properties.path.edges;
			rct_map_element *bannerElement = get_banner_on_path(mapElement);
			if (bannerElement != NULL) {
				do {
					edges &= bannerElement->properties.banner.flags;
				} while ((bannerElement = get_banner_on_path(bannerElement)) != NULL);
			}
			edges &= 0x0F;
			edges &= ~(1 << (chosenDirection ^ 2));
			z = mapElement->base_height;

			for (direction = 0; direction < 4; direction++) {
				if (!(edges & (1 << direction))) continue;

				edges &= ~(1 << direction);
				if (edges != 0) return 3;

				if (footpath_element_is_sloped(mapElement)) {
					if (footpath_element_get_slope_direction(mapElement) == direction) {
						z += 2;
					}
				}
				return loc_6949B9(x, y, z, mapElement, direction, outRideIndex, level + 1);
			}
		}
	} while (!map_element_is_last_for_tile(mapElement++));

	return 0;
}

/**
 * Returns:
 *   0 - dead end?
 *   1 - ride exit
 *   2 - ride entrance
 *   4 - park entrance / exit
 *   5 - search limit reached
 *   6 - (path->type & 2)?
 *  rct2: 0x006949A4
 */
static uint8 sub_6949A4(sint16 x, sint16 y, sint16 z, rct_map_element *inputMapElement, uint8 chosenDirection, uint8 *outRideIndex)
{
	if (footpath_element_is_sloped(inputMapElement)) {
		if (footpath_element_get_slope_direction(inputMapElement) == chosenDirection) {
			z += 2;
		}
	}

	return loc_6949B9(x, y, z, inputMapElement, chosenDirection, outRideIndex, 0);
}

/* rct2: 0x00695225 */
static int guest_path_find_aimless(rct_peep* peep, uint8 edges){
	if (scenario_rand() & 1){
		// If possible go straight
		if (edges & (1 << peep->var_78)){
			return peep_move_one_tile(peep->var_78, peep);
		}
	}

	while (1){
		uint8 direction = scenario_rand() & 3;
		if (edges & (1 << direction)){
			return peep_move_one_tile(direction, peep);
		}
	}
}

/* rct2: 0x0069A60A */
uint8 sub_69A60A(rct_peep* peep){
	RCT2_GLOBAL(0x00F1AED8, uint32) = 0xC350;
	RCT2_GLOBAL(0x00F1AEDD, uint8) = 0x80;

	if (peep->type == PEEP_TYPE_STAFF)
		return 16;

	RCT2_GLOBAL(0x00F1AED8, uint32) = 0x3A98;
	RCT2_GLOBAL(0x00F1AEDD, uint8) = 0;
	if ((peep->flags & PEEP_FLAGS_2)){
		if ((scenario_rand() & 0xFFFF) <= 7281)
			peep->flags &= ~PEEP_FLAGS_2;

		return 16;
	}

	if (peep->flags & PEEP_FLAGS_LEAVING_PARK &&
		peep->var_C6 < 90){
		return 16;
	}

	if (peep->item_standard_flags & PEEP_ITEM_MAP)
		return 14;

	if (peep->flags & PEEP_FLAGS_LEAVING_PARK)
		return 14;

	return 10;
}

/* rct2: 0x0069A5F0 */
static int sub_69A5F0(sint16 x, sint16 y, sint16 z, rct_peep *peep, rct_map_element *map_element){
	//RCT2_GLOBAL(0x00F1AEDC, uint8) = sub_69A60A(peep);

	//// Redundant check to make sure peep is not null??
	//sint16 start_x = RCT2_GLOBAL(0x00F1AECE, sint16);
	//sint16 start_y = RCT2_GLOBAL(0x00F1AED0, sint16);
	//uint8 start_z = RCT2_GLOBAL(0x00F1AED2, uint8);
	//
	//uint8 edges = 0xF;
	//if (peep->var_CC.x == (start_x / 32) && 
	//	peep->var_CC.y == (start_y / 32) &&
	//	peep->var_CC.z == start_z){

	//	uint8 index = 0;
	//	for (; index < 4; ++index){
	//		if (peep->var_D0[index].x == x &&
	//			peep->var_D0[index].y == y &&
	//			peep->var_D0[index].z == z){
	//			edges = peep->var_D0[index].direction & 0xF;	
	//			break;
	//		}
	//	}
	//}

	//bool found = false;
	//rct_map_element *destMapElement = map_get_first_element_at(x / 32, y / 32);
	//do{
	//	if (destMapElement->base_height != z)
	//		continue;

	//	if (map_element_get_type(destMapElement) != MAP_ELEMENT_TYPE_PATH)
	//		continue;

	//	found = true;
	//	break;
	//} while (!map_element_is_last_for_tile(destMapElement++));

	//sint8 chosenDirection = 0xF;
	//if (!found){
	//	chosenDirection = 0xF;
	//	//goto 69A89C
	//}

	//edges &= destMapElement->properties.path.edges & 0xF;
	//rct_map_element *bannerElement = get_banner_on_path(destMapElement);
	//if (bannerElement != NULL) {
	//	do {
	//		edges &= bannerElement->properties.banner.flags;
	//	} while ((bannerElement = get_banner_on_path(bannerElement)) != NULL);
	//}

	//if (edges == 0){
	//	chosenDirection = 0xF;
	//	// goto 69A89C
	//}

	//chosenDirection = bitscanforward(edges);
	//if (edges & ~(1 << chosenDirection) == 0){
	//	// goto 69A8A1 chosenDirection
	//}

	//for (; chosenDirection != -1; chosenDirection = bitscanforward(edges)){
	//	edges &= ~(1 << chosenDirection);
	//	//69a814
	//}
	////69a895
	int eax = x, ebx, ecx = y, edx = z, esi = (int)peep, ebp, edi = (int)map_element;
	RCT2_CALLFUNC_X(0x0069A5F0, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return ebp;
}

/* rct2: 0x006952C0 */
static int guest_path_find_entering_park(rct_peep *peep, rct_map_element *map_element, uint8 edges){
	uint8 chosenEntrance = 0xFF;
	uint16 nearestDist = 0xFFFF;
	for (uint8 entranceNum = 0; entranceNum < 4; ++entranceNum){
		if (RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, sint16)[entranceNum] == (sint16)0x8000)
			continue;

		uint16 dist = abs(RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, sint16)[entranceNum] - peep->next_x) +
			abs(RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Y, sint16)[entranceNum] - peep->next_y);

		if (dist >= nearestDist)
			continue;

		nearestDist = dist;
		chosenEntrance = entranceNum;
	}

	if (chosenEntrance == 0xFF)
		return guest_path_find_aimless(peep, edges);

	sint16 x = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, sint16)[chosenEntrance];
	sint16 y = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Y, sint16)[chosenEntrance];
	sint16 z = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Z, sint16)[chosenEntrance];
	RCT2_GLOBAL(0x00F1AECE, sint16) = x;
	RCT2_GLOBAL(0x00F1AED0, sint16) = y;
	RCT2_GLOBAL(0x00F1AED2, uint8) = z / 8;

	RCT2_GLOBAL(0x00F1AEE0, uint8) = 1;
	RCT2_GLOBAL(0x00F1AEE1, uint8) = 0xFF;

	int chosenDirection = sub_69A5F0(peep->next_x, peep->next_y, peep->next_z, peep, map_element);

	if (chosenDirection == -1)
		return guest_path_find_aimless(peep, edges);
	else
		return peep_move_one_tile(chosenDirection, peep);
}

/* rct2: 0x0069536C */
static int guest_path_find_leaving_park(rct_peep *peep, rct_map_element *map_element, uint8 edges){
	rct2_peep_spawn* peepSpawn = &gPeepSpawns[0];
	// Peeps for whatever reason return to their original spawn point
	// this in future should look for the nearest.
	if (peep->sprite_index & 1 && gPeepSpawns[1].x != 0xFFFF){
		peepSpawn++;
	}

	sint16 x = peepSpawn->x & 0xFFE0;
	sint16 y = peepSpawn->y & 0xFFE0;
	uint8 z = peepSpawn->z * 2;
	uint8 direction = peepSpawn->direction;

	RCT2_GLOBAL(0x00F1AECE, sint16) = x;
	RCT2_GLOBAL(0x00F1AED0, sint16) = y;
	RCT2_GLOBAL(0x00F1AED2, uint8) = z;

	if (x == peep->next_x && y == peep->next_y){
		return peep_move_one_tile(direction, peep);
	}

	RCT2_GLOBAL(0x00F1AEE0, uint8) = 1;
	RCT2_GLOBAL(0x00F1AEE1, uint8) = 0xFF;
	direction = sub_69A5F0(peep->next_x, peep->next_y, peep->next_z, peep, map_element);
	if (direction == -1)
		return guest_path_find_aimless(peep, edges);
	else
		return peep_move_one_tile(direction, peep);
}

/* rct2: 0x00695161 */
static int guest_path_find_park_entrance(rct_peep* peep, rct_map_element *map_element, uint8 edges){
	uint8 entranceNum;

	if (!(peep->flags & PEEP_FLAGS_PARK_ENTRANCE_CHOSEN)){
		uint8 chosenEntrance = 0xFF;
		uint16 nearestDist = 0xFFFF;
		for (entranceNum = 0; entranceNum < 4; ++entranceNum){
			if (RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, sint16)[entranceNum] == (sint16)0x8000)
				continue;

			uint16 dist = abs(RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, sint16)[entranceNum] - peep->next_x) +
				abs(RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Y, sint16)[entranceNum] - peep->next_y);

			if (dist >= nearestDist)
				continue;

			nearestDist = dist;
			chosenEntrance = entranceNum;
		}

		if (chosenEntrance == 0xFF)
			return guest_path_find_aimless(peep, edges);

		peep->current_ride = chosenEntrance;
		peep->flags |= PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
	}

	entranceNum = peep->current_ride;
	sint16 x = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, sint16)[entranceNum];
	sint16 y = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Y, sint16)[entranceNum];
	sint16 z = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Z, sint16)[entranceNum];
	RCT2_GLOBAL(0x00F1AECE, sint16) = x;
	RCT2_GLOBAL(0x00F1AED0, sint16) = y;
	RCT2_GLOBAL(0x00F1AED2, uint8) = z / 8;

	RCT2_GLOBAL(0x00F1AEE0, uint8) = 1;
	RCT2_GLOBAL(0x00F1AEE1, uint8) = 0xFF;

	int chosenDirection = sub_69A5F0(peep->next_x, peep->next_y, peep->next_z, peep, map_element);

	if (chosenDirection == -1)
		return guest_path_find_aimless(peep, edges);
	else
		return peep_move_one_tile(chosenDirection, peep);
}

/* rct2: 0x006A72C5 */
static void get_ride_queue_end(sint16 *x, sint16 *y, sint16 *z, sint16 dist){
	rct_map_element *mapElement = map_get_first_element_at(*x / 32, *y / 32);

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
	RCT2_GLOBAL(0x00F3EFE0, rct_map_element*) = NULL;
	RCT2_GLOBAL(0x00F3EFE8, rct_map_element*) = NULL;

	sint16 baseZ = mapElement->base_height;
	sint16 nextX = *x;
	sint16 nextY = *y;
	while (1){
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH){
			RCT2_GLOBAL(0x00F3EFE0, rct_map_element*) = mapElement;
			RCT2_GLOBAL(0x00F3EFE4, sint16) = nextX;
			RCT2_GLOBAL(0x00F3EFE6, sint16) = nextY;
			RCT2_GLOBAL(0x00F3EFEC, uint32) = direction;
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
			if (mapElement == RCT2_GLOBAL(0x00F3EFE8, rct_map_element*))
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

				if (footpath_element_get_slope_direction(mapElement) != direction)
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
		
		if (RCT2_GLOBAL(0x00F3EFE8, rct_map_element*) == NULL)
			RCT2_GLOBAL(0x00F3EFE8, rct_map_element*) = mapElement;

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

	mapElement = RCT2_GLOBAL(0x00F3EFE0, rct_map_element*);
	if (mapElement == NULL)
		return;

	if (!footpath_element_is_queue(mapElement))
		return;

	*x = RCT2_GLOBAL(0x00F3EFE4, sint16);
	*y = RCT2_GLOBAL(0x00F3EFE6, sint16);
	*z = mapElement->base_height;
}

/* rct2: 0x00694C35 */
static int guest_path_finding(rct_peep* peep){		
	sint16 x, y, z;
	if (peep->next_var_29 & 0x18){
		return guest_surface_path_finding(peep);
	}
	

	x = peep->next_x;
	y = peep->next_y;
	z = peep->next_z;

	bool found = false;
	rct_map_element *mapElement = map_get_first_element_at(x / 32, y / 32);
	do{
		if (z != mapElement->base_height)
			continue;

		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;

		found = true;
		break;
	} while (!map_element_is_last_for_tile(mapElement++));

	if (!found){
		return 1;
	}

	uint8 edges = mapElement->properties.path.edges;
	rct_map_element *bannerElement = get_banner_on_path(mapElement);
	if (bannerElement != NULL){
		do{
			edges &= bannerElement->properties.banner.flags;
		} while ((bannerElement = get_banner_on_path(bannerElement)) != NULL);
	}
	edges &= 0xF;

	if (peep->var_2A == 0 && 
		(peep->guest_heading_to_ride_id == 0xFF || 
		peep->flags & PEEP_FLAGS_LEAVING_PARK)){

		uint8 adjustedEdges = edges;
		uint8 chosenDirection = 0;
		for (; chosenDirection < 4; ++chosenDirection){
			// If there is no path in that direction try another
			if (!(adjustedEdges & (1 << chosenDirection)))
				continue;

			if (sub_694BAE(peep->next_x, peep->next_y, peep->next_z, mapElement, chosenDirection) == 6){
				adjustedEdges &= ~(1 << chosenDirection);
			}
		}
		if (adjustedEdges != 0)
			edges = adjustedEdges;
	}

	if (edges == 0)
		return guest_surface_path_finding(peep);

	sint8 direction = peep->var_78 ^ (1 << 1);
	if (!(edges & ~(1 << direction))){
		peep_check_if_lost(peep);
		peep_check_cant_find_ride(peep);
		peep_check_cant_find_exit(peep);
	}
	else{
		edges &= ~(1 << direction);
	}

	direction = bitscanforward(edges);
	// IF only one edge to choose from
	if ((edges & ~(1 << direction)) == 0){
		return peep_move_one_tile(direction, peep);
	}

	//694F19
	if (peep->var_2A != 0){
		if (peep->state == PEEP_STATE_ENTERING_PARK){
			return guest_path_find_entering_park(peep, mapElement, edges);
		}
		else if (peep->state == PEEP_STATE_LEAVING_PARK){
			return guest_path_find_leaving_park(peep, mapElement, edges);
		}
		return guest_path_find_aimless(peep, edges);
	}

	
	if (!peep_has_food(peep) && 
		(scenario_rand()&0xFFFF) >= 2184){

		uint8 adjustedEdges = edges;
		uint8 chosenDirection = 0;
		for (; chosenDirection < 4; ++chosenDirection){
			// If there is no path in that direction try another
			if (!(adjustedEdges & (1 << chosenDirection)))
				continue;
				
			uint8 rideIndex;
			uint8 al = sub_6949A4(peep->next_x, peep->next_y, peep->next_z, mapElement, chosenDirection, &rideIndex);
			if (al == 6 || al <= 1){
				adjustedEdges &= ~(1 << chosenDirection);
			}
		}
		if (adjustedEdges != 0)
			edges = adjustedEdges;
	}

	if (peep->item_standard_flags & PEEP_ITEM_MAP){
		// If at least 2 directions consult map
		direction = bitscanforward(edges);
		if (direction != -1){
			uint8 edges2 = edges & ~(1 << direction);
			if (bitscanforward(edges2) != -1){

				uint16 probability = 1638;
				if ((peep->flags & PEEP_FLAGS_LEAVING_PARK) ||
					peep->guest_heading_to_ride_id == 0xFF){
					probability = 9362;
				}

				if ((scenario_rand() & 0xFFFF) < probability){
					if (peep->action >= PEEP_ACTION_NONE_1){
						peep->action = PEEP_ACTION_READ_MAP;
						peep->action_frame = 0;
						peep->action_sprite_image_offset = 0;
						sub_693B58(peep);
						invalidate_sprite((rct_sprite*)peep);
					}
				}
			}
		}
	}
		
	if (peep->flags & PEEP_FLAGS_LEAVING_PARK)
		return guest_path_find_park_entrance(peep, mapElement, edges);
		
	if (peep->guest_heading_to_ride_id == 0xFF)
		return guest_path_find_aimless(peep, edges);

	uint8 rideIndex = peep->guest_heading_to_ride_id;
	rct_ride* ride = GET_RIDE(rideIndex);

	if (ride->status != RIDE_STATUS_OPEN)
		return guest_path_find_aimless(peep, edges);

	RCT2_GLOBAL(0x00F1AEE1, uint8) = rideIndex;

	RCT2_GLOBAL(0x00F1AEBC, uint32) = 4;
		
	uint16 closestDist = 0xFFFF;
	uint8 closestStationNum = 4;

	for (uint8 stationNum = 0; stationNum < 4; ++stationNum){
		if (ride->entrances[stationNum] == 0xFFFF)
			continue;

		sint16 stationX = (ride->entrances[stationNum] & 0xFF) * 32;
		sint16 stationY = (ride->entrances[stationNum] & 0xFF00) / 8;
		uint16 dist = abs(stationX - peep->next_x) + abs(stationY - peep->next_y);

		if (dist < closestDist){
			closestDist = dist;
			RCT2_GLOBAL(0x00F1AEBC, uint32) = closestStationNum;
			closestStationNum = stationNum;
			continue;
		}

		if (RCT2_GLOBAL(0x00F1AEBC, uint32) == 4){
			RCT2_GLOBAL(0x00F1AEBC, uint32) = stationNum;
		}
	}

	if (closestStationNum == 4)
		closestStationNum = 0;

	if (RCT2_GLOBAL(0x00F1AEBC, uint32) != 4){
		if (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS &&
			ride->num_stations == 2 &&
			ride->entrances[0] != 0xFFFF &&
			ride->entrances[1] != 0xFFFF){
			closestStationNum = 0;
			if (peep->no_of_rides & 1)
				closestStationNum++;
		}
	}

	uint16 entranceXY = ride->entrances[closestStationNum];
	if (entranceXY == 0xFFFF){
		entranceXY = ride->entrances[closestStationNum + 1];
		if (entranceXY == 0xFFFF){
			entranceXY = ride->entrances[closestStationNum + 2];
		}
	}

	if (closestDist == 0xFFFF){
		entranceXY = ride->station_starts[closestStationNum];
	}

	x = (entranceXY & 0xFF) * 32;
	y = (entranceXY & 0xFF00) / 8;
	z = ride->station_heights[closestStationNum];

	get_ride_queue_end(&x, &y, &z, closestDist);
	RCT2_GLOBAL(0x00F1AECE, sint16) = x;
	RCT2_GLOBAL(0x00F1AECE, sint16) = y;
	RCT2_GLOBAL(0x00F1AECE, uint8) = (uint8)z;
	RCT2_GLOBAL(0x00F1AEE0, uint8) = 1;

	direction = sub_69A5F0(peep->next_x, peep->next_y, peep->next_z, peep, mapElement);
	if (direction == -1){
		return guest_path_find_aimless(peep, edges);
	}
	return peep_move_one_tile(direction, peep);
}

/**
 * 
 *  rct2: 0x00693C9E
 */
static int sub_693C9E(rct_peep *peep)
{
	RCT2_GLOBAL(0x00F1EE18, uint16) = 0;
	RCT2_GLOBAL(0x00F1AEF1, uint8) = peep->action;

	if (peep->action == PEEP_ACTION_NONE_1)
		peep->action = PEEP_ACTION_NONE_2;

	if (peep->state == PEEP_STATE_QUEUING){
		if (peep_update_queue_position(peep))
			return 1;
	}

	sint16 x, y, xy_dist;
	if (!peep_update_action(&x, &y, &xy_dist, peep)){
		RCT2_GLOBAL(0x00F1EE18, uint16) |= 1;
		uint8 result = 0;
		if (peep->type == PEEP_TYPE_GUEST){
			result = guest_path_finding(peep);
		}
		else{
			result = RCT2_CALLPROC_X(0x006BF926, x, 0, y, 0, (int)peep, 0, 0) & 0x100;
		}

		if (result != 0)
			return 1;

		if (!peep_update_action(&x, &y, &xy_dist, peep))
			return 1;
	}

	if ((x & 0xFFE0) == peep->next_x && (y & 0xFFE0) == peep->next_y){
		sint16 z = peep_get_height_on_slope(peep, x, y);
		invalidate_sprite((rct_sprite*)peep);
		sprite_move(x, y, z, (rct_sprite*)peep);
		invalidate_sprite((rct_sprite*)peep);
		return 1;
	}

	if (x < 32 || y < 32 || x >= RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) || y >= RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16)){
		if (peep->var_2A == 1){
			RCT2_GLOBAL(0x00F1EE18, uint16) |= (1 << 1);
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

		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH){
			if ((mapElement->flags & MAP_ELEMENT_FLAG_GHOST))
				continue;
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
			peep->var_79 = 0xFF;
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

			sint16 z = peep_get_height_on_slope(peep, x, y);
			invalidate_sprite((rct_sprite*)peep);
			sprite_move(x, y, z, (rct_sprite*)peep);
			invalidate_sprite((rct_sprite*)peep);
			return 1;
		}
	}
	return peep_return_to_center_of_tile(peep);
}

/**
 * 
 *  rct2: 0x0069926C
 *  Expend type was previously an offset saved in 0x00F1AEC0
 */
static void peep_spend_money(rct_peep *peep, money16 *peep_expend_type, money32 amount)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)
		return;

	peep->cash_in_pocket = max(0, peep->cash_in_pocket - amount);
	peep->cash_spent += amount;
	if (peep_expend_type != NULL) {
		*peep_expend_type += (money16)amount;
	}
	window_invalidate_by_number(WC_PEEP, peep->sprite_index);

	RCT2_GLOBAL(0x00141F568, uint8) = RCT2_GLOBAL(0x0013CA740, uint8);
	finance_payment(-amount, RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) / 4);

	sound_play_panned(SOUND_PURCHASE, 0x8001, peep->x, peep->y, peep->z);
}

static void peep_set_has_ridden(rct_peep *peep, int rideIndex)
{
	peep->rides_been_on[rideIndex >> 5] |= 1 << (rideIndex & 0x1F);
}

static bool peep_has_ridden(rct_peep *peep, int rideIndex)
{
	return peep->rides_been_on[rideIndex >> 5] & (1 << (rideIndex & 0x1F));
}

static void peep_set_has_ridden_ride_type(rct_peep *peep, int rideType)
{
	peep->rides_been_on[rideType >> 5] |= 1 << (rideType & 0x1F);
}

static bool peep_has_ridden_ride_type(rct_peep *peep, int rideType)
{
	return peep->rides_been_on[rideType >> 5] & (1 << (rideType & 0x1F));
}

/**
 * 
 *  rct2: 0x0069545B
 */
static void peep_on_enter_ride(rct_peep *peep, int rideIndex)
{
	rct_ride *ride;
	ride_rating minIntensity, maxIntensity;
	ride_rating minNausea, maxNausea;
	uint16 satisfactionFlags;

	ride = GET_RIDE(rideIndex);
	peep->flags &= ~PEEP_FLAGS_19;
	if (ride->excitement == (ride_rating)0xFFFF) {
		satisfactionFlags = 0x1FF;
	} else {
		satisfactionFlags = 0;
		maxIntensity = (peep->intensity & 0x0F) * 100;
		minIntensity = (peep->intensity / 16) * 100;
		if (maxIntensity <= ride->intensity || minIntensity >= ride->intensity) {
			satisfactionFlags |= (1 << 1);
		}
		maxIntensity -= peep->happiness * 2;
		minIntensity += peep->happiness;
		if (maxIntensity <= ride->intensity || minIntensity >= ride->intensity) {
			satisfactionFlags |= (1 << 4);
		}
		maxIntensity -= peep->happiness * 2;
		minIntensity += peep->happiness;
		if (maxIntensity <= ride->intensity || minIntensity >= ride->intensity) {
			satisfactionFlags |= (1 << 7);
		}

		minNausea = RCT2_ADDRESS(0x00982390, uint16)[(peep->nausea_tolerance & 3) * 2];
		maxNausea = RCT2_ADDRESS(0x00982392, uint16)[(peep->nausea_tolerance & 3) * 2];
		if (maxNausea <= ride->nausea || minNausea >= ride->nausea) {
			satisfactionFlags |= (1 << 2);
		}
		maxNausea -= peep->happiness * 2;
		minNausea += peep->happiness;
		if (maxNausea <= ride->nausea || minNausea >= ride->nausea) {
			satisfactionFlags |= (1 << 5);
		}
		maxNausea -= peep->happiness * 2;
		minNausea += peep->happiness;
		if (maxNausea <= ride->nausea || minNausea >= ride->nausea) {
			satisfactionFlags |= (1 << 8);
		}
		satisfactionFlags |= (1 << 9);
	}

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
		if (ride->value != 0xFFFF) {
			satisfactionFlags &= ~(1 << 9);
			satisfactionFlags |= (1 << 10);
			if ((money16)ride->value < ride->price) {
				satisfactionFlags &= ~(1 << 10);
				satisfactionFlags |= (1 << 9);
				if ((ride->value + ((ride->value * peep->happiness) / 256)) < ride->price) {
					satisfactionFlags &= ~(1 << 9);
				}
			}
		}
	}

	uint8 satisfactionVar = 0;
	if (satisfactionFlags & (1 << 9)) satisfactionVar += 15;
	if (satisfactionFlags & (1 << 10)) satisfactionVar += 40;
	if ((satisfactionFlags & (1 << 9)) && (satisfactionFlags & (1 << 10))) satisfactionVar -= 45;
	if ((satisfactionFlags & 6) == 6) {
		satisfactionVar += 70;
	} else {
		if (satisfactionFlags & 6) satisfactionVar += 15;
		if ((satisfactionFlags & 0x30) == 0x30) {
			satisfactionVar += 35;
		} else {
			if (satisfactionFlags & 0x30) satisfactionVar += 10;
			if ((satisfactionFlags & 0x180) == 0x180) {
				satisfactionVar += 10;
			} else {
				satisfactionVar -= 60;
			}
		}
	}

	if (peep->time_in_queue <= 750) {
		satisfactionVar += 10;
	} else if (peep->time_in_queue >= 2250) {
		satisfactionVar -= 10;
		if (peep->time_in_queue >= 4500) {
			satisfactionVar -= 25;
		}
	}

	if (peep_has_ridden(peep, peep->current_ride)) satisfactionVar += 10;
	peep_set_has_ridden(peep, peep->current_ride);
	if (peep->no_of_rides < 255) peep->no_of_rides++;

	if (peep_has_ridden_ride_type(peep, ride->type)) satisfactionVar += 10;
	peep_set_has_ridden_ride_type(peep, ride->type);

	uint8 unkExcitementValue = clamp(0, (ride->excitement / 4) + peep->happiness, 255);
	if (unkExcitementValue >= peep->var_FA) {
		if (peep->happiness >= 160 && peep->happiness_growth_rate >= 160) {
			peep->var_FA = unkExcitementValue;
			peep->flags |= PEEP_FLAGS_19;
		}
	}

	uint8 satisfaction = 0;
	if (satisfactionVar >= 0) {
		satisfaction++;
		if (satisfactionVar >= 20) {
			satisfaction++;
			if (satisfactionVar >= 40) {
				satisfaction++;
			}
		}
	}
	ride_update_satisfaction(ride, satisfaction);
	peep->happiness_growth_rate = clamp(0, satisfactionVar + peep->happiness_growth_rate, 255);

	uint32 nauseaMultiplier = clamp(64, 256 - peep->happiness_growth_rate, 200);
	uint32 nauseaGrowthRateChange = (ride->nausea * nauseaMultiplier) / 512;
	nauseaGrowthRateChange *= max(128, peep->hunger);
	nauseaGrowthRateChange /= 128;
	nauseaGrowthRateChange *= 2;
	nauseaGrowthRateChange >>= (peep->nausea_tolerance & 3);
	peep->nausea_growth_rate = (uint8)clamp(0, peep->nausea_growth_rate + nauseaGrowthRateChange, 255);
}

static bool peep_should_go_on_ride_again(rct_peep *peep, rct_ride *ride)
{
	if (!(RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x100000)) return false;
	if (ride->excitement == (ride_rating)0xFFFF) return false;
	if (ride->intensity > RIDE_RATING(10,00)) return false;
	if (peep->happiness < 180) return false;
	if (peep->energy < 100) return false;
	if (peep->nausea > 160) return false;
	if (peep->hunger < 30) return false;
	if (peep->thirst < 20) return false;
	if (peep->balloon_colour > 170) return false;
	
	uint8 r = (scenario_rand() & 0xFF);
	if (r <= 128) {
		if (peep->no_of_rides > 7) return false;
		if (r > 64) return false;
	}

	return true;
}

static bool peep_should_preferred_intensity_increase(rct_peep *peep)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PREF_LESS_INTENSE_RIDES) return false;
	if (peep->happiness < 200) return false;

	return (scenario_rand() & 0xFF) >= peep->intensity;
}

static bool peep_really_liked_ride(rct_peep *peep, rct_ride *ride)
{
	return
		peep->happiness >= 215 &&
		peep->nausea <= 120 &&
		ride->excitement != (ride_rating)0xFFFF &&
		ride->intensity <= RIDE_RATING(10,00);
}

/**
 * 
 *  rct2: 0x0069576E
 */
static void peep_on_exit_ride(rct_peep *peep, int rideIndex)
{
	rct_ride *ride = GET_RIDE(rideIndex);

	if (peep->flags & PEEP_FLAGS_19) {
		peep->flags &= ~PEEP_FLAGS_19;
		peep->favourite_ride = rideIndex;
		// TODO fix this flag name or add another one
		peep->window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
	}
	peep->happiness = peep->happiness_growth_rate;
	peep->nausea = peep->nausea_growth_rate;
	peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_STATS;

	if (peep_should_go_on_ride_again(peep, ride)) {
		peep->guest_heading_to_ride_id = rideIndex;
		peep->var_C6 = 200;
		sub_69A98C(peep);

		rct_window *w = window_find_by_number(WC_PEEP, peep->sprite_index);
		if (w != NULL) {
			window_event_invalidate_call(w);
			widget_invalidate(w, 12);
		}
	}

	if (peep_should_preferred_intensity_increase(peep)) {
		if (peep->intensity <= 255 - 16) {
			peep->intensity += 16;
		}
	}

	if (peep->flags & PEEP_FLAGS_26) {
		peep_insert_new_thought(peep, PEEP_THOUGHT_NICE_RIDE, 255);
	}

	if (peep_really_liked_ride(peep, ride)) {
		peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_WAS_GREAT, rideIndex);

		int laugh = scenario_rand() & 7;
		if (laugh < 3) {
			sound_play_panned(SOUND_LAUGH_1 + laugh, 0x8001, peep->x, peep->y, peep->z);
		}
	}

	ride->total_customers++;
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
}

/**
 * 
 *  rct2: 0x00695444
 */
static void peep_on_enter_or_exit_ride(rct_peep *peep, int rideIndex, int flags)
{
	if (flags & 1) {
		peep_on_exit_ride(peep, rideIndex);
	} else {
		peep_on_enter_ride(peep, rideIndex);
	}
}

/**
 * 
 *  rct2: 0x0069AF1E
 */
static bool sub_69AF1E(rct_peep *peep, int rideIndex, int shopItem, money32 price)
{
	return !(RCT2_CALLPROC_X(0x0069AF1E, shopItem | (rideIndex << 8), 0, price, 0, (int)peep, 0, 0) & 0x100);
}

/**
 * 
 *  rct2: 0x0069AEB7
 */
static bool peep_should_use_cash_machine(rct_peep *peep, int rideIndex)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY) return false;
	if (peep->flags & PEEP_FLAGS_LEAVING_PARK) return false;
	if (peep->cash_in_pocket > MONEY(20,00)) return false;
	if (115 + (scenario_rand() % 128) > peep->happiness) return false;
	if (peep->energy < 80) return false;

	rct_ride *ride = GET_RIDE(rideIndex);
	ride_update_satisfaction(ride, peep->happiness >> 6);
	ride->var_120++;
	ride->total_customers++;
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
	return true;
}

/**
 * 
 *  rct2: 0x0069A98C
 */
static void sub_69A98C(rct_peep *peep)
{
	peep->var_CC.x = 0xFF;
	peep->var_CC.y = 0xFF;
	peep->var_CC.z = 0xFF;
	peep->var_CC.direction = 0xFF;
}

/**
 * 
 *  rct2: 0x0068FD3A
 */
static void sub_68FD3A(rct_peep *peep)
{
	RCT2_CALLPROC_X(0x0068FD3A, 0, 0, 0, 0, (int)peep, 0, 0);
}

/**
 * 
 *  rct2: 0x00690B99
 */
static bool sub_690B99(rct_peep *peep, int edge, uint8 *rideToView, uint8 *rideSeatToView)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	eax = edge;
	esi = (int)peep;
	if (RCT2_CALLFUNC_X(0x00690B99, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp) & 0x100)
		return false;

	*rideToView = ecx & 0xFF;
	*rideSeatToView = (ecx >> 8) & 0xFF;
	return true;
}

/**
 * 
 *  rct2: 0x00694921
 *  Gets the height including the bit depending
 *  on how far up the slope the peep is.
 */
static int peep_get_height_on_slope(rct_peep *peep, int x, int y)
{
	if (x == (sint16)0x8000)
		return 0;

	if (peep->next_var_29 & 0x18){
		return map_element_height(x, y) & 0xFFFF;
	}

	int z = peep->next_z * 8;

	return z + map_height_from_slope(x, y, peep->next_var_29);
}

static bool peep_has_voucher_for_free_ride(rct_peep *peep, int rideIndex)
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
		widget_invalidate(w, 12);
	}
}

/**
 * 
 *  rct2: 0x006960AB
 */
static bool sub_6960AB(rct_peep *peep, int rideIndex, int dh, int bp)
{
	// return RCT2_CALLPROC_X(0x006960AB, 0, 0, 0, rideIndex | (dh << 8), (int)peep, 0, bp) & 0x100;

	rct_ride *ride = GET_RIDE(rideIndex);
	rct_ride_type *rideEntry;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_8)) {
		if (ride->status == RIDE_STATUS_OPEN && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)) {
			if (!(RCT2_ADDRESS(0x0097D4F2, uint16)[ride->type * 8] & 0x1000) || ride->value == 0xFFFF || ride->price != 0) {
				if (peep->flags & PEEP_FLAGS_LEAVING_PARK) {
					goto loc_69666E;
				}
			}
			if (RCT2_ADDRESS(0x0097CF40, uint32)[ride->type * 2] & 0x20000) {
				if (rideIndex == peep->previous_ride) goto loc_69666E;
				if (ride->type != RIDE_TYPE_CASH_MACHINE) {
					if (ride->type == RIDE_TYPE_FIRST_AID) {
						if (peep->nausea < 128) goto loc_69666E;
						goto loc_69652C;
					}
					rideEntry = GET_RIDE_ENTRY(ride->subtype);
					if (rideEntry->shop_item == 255) {
						if (peep->bathroom < 70) goto loc_69666E;
						money16 ax = ride->price * 40;
						if ((ax >> 8) != 0 || (ax & 0xFF) > peep->bathroom) {
							if (!(bp & 4)) {
								peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_NOT_PAYING, rideIndex);
								if (peep->happiness_growth_rate >= 60) {
									peep->happiness_growth_rate -= 16;
								}
								ride_update_popularity(ride, 0);
							}
							goto loc_696658;
						}
					}

					if (ride->price != 0) {
						if (peep->cash_in_pocket <= 0) {
							if (!(bp & 4)) {
								peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_SPENT_MONEY, 255);
							}
							goto loc_696658;
						}
						if (ride->price > peep->cash_in_pocket) {
							if (!(bp & 4)) {
								peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CANT_AFFORD_0, rideIndex);
							}
							goto loc_696658;
						}
					}
				}

			loc_69652C:
				if (!(bp & 4)) {
					ride_update_popularity(ride, 1);
					if (rideIndex == peep->guest_heading_to_ride_id) {
						peep_reset_ride_heading(peep);
					}
				}
				return true;
			}
			if (!(bp & 2)) {
				if (ride->queue_length[dh] == 1000)
					goto loc_696645;
				if (!(bp & 1)) {
					if (ride->first_peep_in_queue[dh] != 0xFFFF)
						goto loc_696645;
				} else {
					if (ride->first_peep_in_queue[dh] != 0xFFFF) {
						rct_peep *firstPeepInQueue = &(g_sprite_list[ride->first_peep_in_queue[dh]].peep);
						if (abs(firstPeepInQueue->z - peep->z) <= 6) {
							int dx = abs(firstPeepInQueue->x - peep->x);
							int dy = abs(firstPeepInQueue->y - peep->y);
							int maxD = max(dx, dy);
							if (maxD <= 13) {
								if (firstPeepInQueue->time_in_queue > 10)
									goto loc_696645;
								if (maxD < 8)
									goto loc_696645;
							}
						}
					}
				}
			}
			if (!(RCT2_ADDRESS(0x0097D4F2, uint16)[ride->type * 8] & 0x1000) || ride->value == 0xFFFF || ride->price != 0) {
				if (peep->previous_ride == rideIndex)
					goto loc_69666E;
				if (ride->price != 0 && !peep_has_voucher_for_free_ride(peep, rideIndex)) {
					if (peep->cash_in_pocket <= 0) {
						if (!(bp & 4)) {
							peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_SPENT_MONEY, 255);
						}
						goto loc_696658;
					}
					if (ride->price > peep->cash_in_pocket) {
						if (!(bp & 4)) {
							peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CANT_AFFORD_0, rideIndex);
						}
						goto loc_696658;
					}
				}
				if (ride->last_crash_type != RIDE_CRASH_TYPE_NONE) {
					if (peep->happiness < 225) {
						if (!(bp & 4)) {
							peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_NOT_SAFE, rideIndex);
							if (peep->happiness_growth_rate >= 64) {
								peep->happiness_growth_rate -= 8;
							}
							ride_update_popularity(ride, 0);
						}
						goto loc_696658;
					}
				}
				if (ride->excitement != 0xFFFF) {
					if (rideIndex == peep->guest_heading_to_ride_id) {
						if (ride->intensity > RIDE_RATING(10, 00)) goto loc_6965F1;
						goto loc_696387;
					}
					if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RAIN_LEVEL, uint8) != 0) {
						if ((ride->var_114 >> 5) >= 3) goto loc_696387;
						if (!(bp & 4)) {
							peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_NOT_WHILE_RAINING, rideIndex);
							if (peep->happiness_growth_rate >= 64) {
								peep->happiness_growth_rate -= 8;
							}
							ride_update_popularity(ride, 0);
						}
						goto loc_696658;
					}
					ride_rating maxIntensity = min((peep->intensity >> 4) * 100, 1000) + peep->happiness;
					ride_rating minIntensity = ((peep->intensity & 0x0F) * 100) - peep->happiness;
					if (ride->intensity < minIntensity) {
						if (!(bp & 4)) {
							peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_MORE_THRILLING, rideIndex);
							if (peep->happiness_growth_rate >= 64) {
								peep->happiness_growth_rate -= 8;
							}
							ride_update_popularity(ride, 0);
						}
						goto loc_696658;
					}
					if (ride->intensity > maxIntensity) goto loc_6965F1;

					ride_rating minNausea = RCT2_ADDRESS(0x00982390, uint16)[(peep->nausea_tolerance & 3) * 2] - peep->happiness;
					ride_rating maxNausea = RCT2_ADDRESS(0x00982392, uint16)[(peep->nausea_tolerance & 3) * 2] + peep->happiness;
					if (ride->nausea > maxNausea) {
						if (!(bp & 4)) {
							peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_SICKENING, rideIndex);
							if (peep->happiness_growth_rate >= 64) {
								peep->happiness_growth_rate -= 8;
							}
							ride_update_popularity(ride, 0);
						}
						goto loc_696658;
					}
					if (ride->nausea >= 140 && peep->nausea > 160) goto loc_69666E;
					goto loc_696387;
				}

				if (RCT2_ADDRESS(0x0097D4F2, uint16)[ride->type * 8] & 0x10) {
					if (scenario_rand() > 0x1999U) goto loc_69666E;
					if (ride->max_positive_vertical_g > 500) goto loc_69666E;
					if (ride->max_negative_vertical_g < -400) goto loc_69666E;
					if (ride->max_lateral_g > 400) goto loc_69666E;
				}

			loc_696387:;
				uint32 value = ride->value;
				if (value != 0xFFFF && !peep_has_voucher_for_free_ride(peep, rideIndex)) {
					if (peep->flags & PEEP_FLAGS_5) value /= 4;
					if (ride->price > (money16)(value * 2)) {
						if (bp & 4) goto loc_696658;
						peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_BAD_VALUE, rideIndex);
						if (peep->happiness_growth_rate < 60) {
							peep->happiness_growth_rate -= 16;
						}
						ride_update_popularity(ride, 0);
						goto loc_696658;
					}
					if (ride->price <= (money16)(value / 2) && bp == 4) {
						if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
							if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_FREE_ENTRY) {
								peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_GOOD_VALUE, rideIndex);
							}
						}
					}
				}
			}

			if (!(bp & 4)) {
				ride_update_popularity(ride, 1);
				if ((peep->flags & PEEP_FLAGS_27) && ride_type_is_intamin(ride->type)) {
					peep_insert_new_thought(peep, PEEP_THOUGHT_EXCITED, 255);
				}
			}

			if (rideIndex == peep->guest_heading_to_ride_id) {
				peep_reset_ride_heading(peep);
			}

			ride->lifecycle_flags &= ~RIDE_LIFECYCLE_9;
			return true;
		}
	}
loc_69666E:
	if (rideIndex == peep->guest_heading_to_ride_id) {
		peep_reset_ride_heading(peep);
	}
	return false;

loc_6965F1:
	if (!(bp & 4)) {
		peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_INTENSE, rideIndex);
		if (peep->happiness_growth_rate >= 64) {
			peep->happiness_growth_rate -= 8;
		}
		ride_update_popularity(ride, 0);
	}
	goto loc_696658;

loc_696645:
	ride->lifecycle_flags |= RIDE_LIFECYCLE_9;

loc_696658:
	if (!(bp & 4)) {
		peep->previous_ride = rideIndex;
		peep->previous_ride_time_out = 0;
	}

	peep_reset_ride_heading(peep);
	return false;
}

/**
 * 
 *  rct2: 0x00695DD2
 */
static void peep_pick_ride_to_go_on(rct_peep *peep)
{
	rct_ride *ride;

	if (peep->state == PEEP_STATE_WALKING) return;
	if (peep->guest_heading_to_ride_id != 255) return;
	if (peep->flags & PEEP_FLAGS_LEAVING_PARK) return;
	if (peep_has_food(peep)) return;
	if (peep->x == (sint16)0x8000) return;

	RCT2_GLOBAL(0x00F1AD98, uint32) = 0;
	RCT2_GLOBAL(0x00F1AD9C, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADA0, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADA4, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADA8, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADAC, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADB0, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADB4, uint32) = 0;

	// TODO Check for a toy is likely a mistake and should be a map,
	//      but then again this seems to only allow the peep to go on
	//      rides they haven't been on before.
	if (peep->item_standard_flags & PEEP_ITEM_TOY) {
		// Consider rides that peep hasn't been on yet
		int i;
		FOR_ALL_RIDES(i, ride) {
			if (!peep_has_ridden(peep, i)) {
				RCT2_ADDRESS(0x00F1AD98, uint32)[i >> 5] |= i & 0x1F;
			}
		}
	} else {
		// Take nearby rides into consideration
		int cx = floor2(peep->x, 32);
		int cy = floor2(peep->y, 32);
		for (int x = cx - 320; x <= cx + 320; x++) {
			for (int y = cy - 320; y <= cy + 320; y++) {
				if (x >= 0 && y >= 0 && x < (256 * 32) && y < (256 * 32)) {
					rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
					do {
						if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;

						int rideIndex = mapElement->properties.track.ride_index;
						RCT2_ADDRESS(0x00F1AD98, uint32)[rideIndex >> 5] |= (rideIndex & 0x1F);
					} while (!map_element_is_last_for_tile(mapElement++));
				}
			}
		}

		// Always take the big rides into consideration (realistic as you can usually see them from anywhere in the park)
		int i;
		FOR_ALL_RIDES(i, ride) {
			if (ride->lifecycle_flags == RIDE_LIFECYCLE_TESTED) continue;
			if (ride->excitement == (ride_rating)0xFFFF) continue;
			if (ride->highest_drop_height <= 66 && ride->excitement < RIDE_RATING(8,00)) continue;

			RCT2_ADDRESS(0x00F1AD98, uint32)[i >> 5] |= i & 0x1F;
		}
	}

	// Filter the considered rides
	uint8 *potentialRides = (uint8*)0x00F1ADBC;
	uint8 *nextPotentialRide = potentialRides;
	int numPotentialRides = 0;
	for (int i = 0; i < MAX_RIDES; i++) {
		if (!(RCT2_ADDRESS(0x00F1AD98, uint32)[i >> 5] & (i & 0x1F)))
			continue;

		rct_ride *ride = GET_RIDE(i);
		if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_9)) {
			if (sub_6960AB(peep, i, 0, 6)) {
				*nextPotentialRide++ = i;
				numPotentialRides++;
			}
		}
	}

	// Pick the most exciting ride
	int mostExcitingRideIndex = -1;
	ride_rating mostExcitingRideRating = 0;
	for (int i = 0; i < numPotentialRides; i++) {
		ride = GET_RIDE(potentialRides[i]);
		if (ride->excitement == (ride_rating)0xFFFF) continue;
		if (ride->excitement > mostExcitingRideRating) {
			mostExcitingRideIndex = potentialRides[i];
			mostExcitingRideRating = ride->excitement;
		}
	}
	if (mostExcitingRideIndex == -1)
		return;

	// Head to that ride
	peep->guest_heading_to_ride_id = mostExcitingRideIndex;
	peep->var_C6 = 200;
	sub_69A98C(peep);

	// Invalidate windows
	rct_window *w = window_find_by_number(WC_PEEP, peep->sprite_index);
	if (w != NULL) {
		window_event_invalidate_call(w);
		widget_invalidate(w, 12);
	}

	// Make peep look at their map if they have one
	if (peep->item_standard_flags & PEEP_ITEM_MAP) {
		if (peep->action == PEEP_ACTION_NONE_1 || peep->action == PEEP_ACTION_NONE_2) {
			peep->action = PEEP_ACTION_READ_MAP;
			peep->action_frame = 0;
			peep->action_sprite_image_offset = 0;
			sub_693B58(peep);
			invalidate_sprite((rct_sprite*)peep);
		}
	}
}

/**
 * 
 *  rct2: 0x00695B70
 */
static void peep_head_for_nearest_ride_type(rct_peep *peep, int rideType)
{
	rct_ride *ride;

	if (peep->state != PEEP_STATE_SITTING && peep->state != PEEP_STATE_WATCHING && peep->state != PEEP_STATE_WALKING) {
		return;
	}
	if (peep->flags & PEEP_FLAGS_LEAVING_PARK) return;
	if (peep->x == (sint16)0x8000) return;
	if (peep->guest_heading_to_ride_id != 255) {
		ride = GET_RIDE(peep->guest_heading_to_ride_id);
		if (ride->type == rideType) {
			return;
		}
	}

	RCT2_GLOBAL(0x00F1AD98, uint32) = 0;
	RCT2_GLOBAL(0x00F1AD9C, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADA0, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADA4, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADA8, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADAC, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADB0, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADB4, uint32) = 0;

	// TODO Check for a toy is likely a mistake and should be a map
	if ((peep->item_standard_flags & PEEP_ITEM_TOY) && rideType != RIDE_TYPE_FIRST_AID) {
		// Consider all rides in the park
		int i;
		FOR_ALL_RIDES(i, ride) {
			if (ride->type == rideType) {
				RCT2_ADDRESS(0x00F1AD98, uint32)[i >> 5] |= i & 0x1F;
			}
		}
	} else {
		// Take nearby rides into consideration
		int cx = floor2(peep->x, 32);
		int cy = floor2(peep->y, 32);
		for (int x = cx - 320; x <= cx + 320; x++) {
			for (int y = cy - 320; y <= cy + 320; y++) {
				if (x >= 0 && y >= 0 && x < (256 * 32) && y < (256 * 32)) {
					rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
					do {
						if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;

						int rideIndex = mapElement->properties.track.ride_index;
						ride = GET_RIDE(rideIndex);
						if (ride->type == rideType) {
							RCT2_ADDRESS(0x00F1AD98, uint32)[rideIndex >> 5] |= (rideIndex & 0x1F);
						}
					} while (!map_element_is_last_for_tile(mapElement++));
				}
			}
		}
	}

	// Filter the considered rides
	uint8 *potentialRides = (uint8*)0x00F1ADBC;
	uint8 *nextPotentialRide = potentialRides;
	int numPotentialRides = 0;
	for (int i = 0; i < MAX_RIDES; i++) {
		if (!(RCT2_ADDRESS(0x00F1AD98, uint32)[i >> 5] & (i & 0x1F)))
			continue;

		rct_ride *ride = GET_RIDE(i);
		if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_9)) {
			if (sub_6960AB(peep, i, 0, 6)) {
				*nextPotentialRide++ = i;
				numPotentialRides++;
			}
		}
	}

	// Pick the closest ride
	int closestRideIndex = -1;
	int closestRideDistance = INT_MAX;
	for (int i = 0; i < numPotentialRides; i++) {
		ride = GET_RIDE(potentialRides[i]);
		int rideX = (ride->station_starts[0] & 0xFF) * 32;
		int rideY = (ride->station_starts[0] >> 8) * 32;
		int distance = abs(rideX - peep->x) + abs(rideY - peep->y);
		if (distance < closestRideDistance) {
			closestRideIndex = potentialRides[i];
			closestRideDistance = distance;
		}
	}
	if (closestRideIndex == -1)
		return;

	// Head to that ride
	peep->guest_heading_to_ride_id = closestRideIndex;
	peep->var_C6 = 200;
	sub_69A98C(peep);

	// Invalidate windows
	rct_window *w = window_find_by_number(WC_PEEP, peep->sprite_index);
	if (w != NULL) {
		window_event_invalidate_call(w);
		widget_invalidate(w, 12);
	}

	peep->var_F4 = 0;
}

/**
 * 
 *  rct2: 0x006958D0
 */
static void peep_head_for_nearest_ride_with_flags(rct_peep *peep, int rideTypeFlags)
{
	rct_ride *ride;

	if (peep->state != PEEP_STATE_SITTING && peep->state != PEEP_STATE_WATCHING && peep->state != PEEP_STATE_WALKING) {
		return;
	}
	if (peep->flags & PEEP_FLAGS_LEAVING_PARK) return;
	if (peep->x == (sint16)0x8000) return;
	if (peep->guest_heading_to_ride_id != 255) {
		ride = GET_RIDE(peep->guest_heading_to_ride_id);
		if (RCT2_ADDRESS(0x0097CF40, uint32)[ride->type * 2] & 0x03800000) {
			return;
		}
	}

	if ((rideTypeFlags & 0x002000000) && peep_has_food(peep)) {
		return;
	}

	RCT2_GLOBAL(0x00F1AD98, uint32) = 0;
	RCT2_GLOBAL(0x00F1AD9C, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADA0, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADA4, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADA8, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADAC, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADB0, uint32) = 0;
	RCT2_GLOBAL(0x00F1ADB4, uint32) = 0;

	// TODO Check for a toy is likely a mistake and should be a map
	if (peep->item_standard_flags & PEEP_ITEM_TOY) {
		// Consider all rides in the park
		int i;
		FOR_ALL_RIDES(i, ride) {
			if (RCT2_ADDRESS(0x0097CF40, uint32)[ride->type * 2] & rideTypeFlags) {
				RCT2_ADDRESS(0x00F1AD98, uint32)[i >> 5] |= i & 0x1F;
			}
		}
	} else {
		// Take nearby rides into consideration
		int cx = floor2(peep->x, 32);
		int cy = floor2(peep->y, 32);
		for (int x = cx - 320; x <= cx + 320; x++) {
			for (int y = cy - 320; y <= cy + 320; y++) {
				if (x >= 0 && y >= 0 && x < (256 * 32) && y < (256 * 32)) {
					rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
					do {
						if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;

						int rideIndex = mapElement->properties.track.ride_index;
						ride = GET_RIDE(rideIndex);
						if (RCT2_ADDRESS(0x0097CF40, uint32)[ride->type * 2] & rideTypeFlags) {
							RCT2_ADDRESS(0x00F1AD98, uint32)[rideIndex >> 5] |= (rideIndex & 0x1F);
						}
					} while (!map_element_is_last_for_tile(mapElement++));
				}
			}
		}
	}

	// Filter the considered rides
	uint8 *potentialRides = (uint8*)0x00F1ADBC;
	uint8 *nextPotentialRide = potentialRides;
	int numPotentialRides = 0;
	for (int i = 0; i < MAX_RIDES; i++) {
		if (!(RCT2_ADDRESS(0x00F1AD98, uint32)[i >> 5] & (i & 0x1F)))
			continue;

		rct_ride *ride = GET_RIDE(i);
		if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_9)) {
			if (sub_6960AB(peep, i, 0, 6)) {
				*nextPotentialRide++ = i;
				numPotentialRides++;
			}
		}
	}

	// Pick the closest ride
	int closestRideIndex = -1;
	int closestRideDistance = INT_MAX;
	for (int i = 0; i < numPotentialRides; i++) {
		ride = GET_RIDE(potentialRides[i]);
		int rideX = (ride->station_starts[0] & 0xFF) * 32;
		int rideY = (ride->station_starts[0] >> 8) * 32;
		int distance = abs(rideX - peep->x) + abs(rideY - peep->y);
		if (distance < closestRideDistance) {
			closestRideIndex = potentialRides[i];
			closestRideDistance = distance;
		}
	}
	if (closestRideIndex == -1)
		return;

	// Head to that ride
	peep->guest_heading_to_ride_id = closestRideIndex;
	peep->var_C6 = 200;
	sub_69A98C(peep);

	// Invalidate windows
	rct_window *w = window_find_by_number(WC_PEEP, peep->sprite_index);
	if (w != NULL) {
		window_event_invalidate_call(w);
	}
	widget_invalidate_by_number(WC_RIDE, closestRideIndex, 23);

	peep->var_F4 = 0;
}

/**
 * 
 *  rct2: 0x0069C483
 */
static void peep_give_real_name(rct_peep *peep)
{
	RCT2_CALLPROC_X(0x0069C483, 0, 0, 0, 0, (int)peep, 0, 0);
}

/**
 * 
 *  rct2: 0x00699115
 */
void peep_update_name_sort(rct_peep *peep)
{
	RCT2_CALLPROC_X(0x00699115, 0, 0, 0, 0, (int)peep, 0, 0);

	// This is required at the moment because this function reorders peeps in the sprite list
	openrct2_reset_object_tween_locations();
}

/**
 * 
 *  rct2: 0x0069926C
 */
void peep_update_names(bool realNames)
{
	rct_peep *peep;
	uint16 spriteIndex;
	bool restart;

	if (realNames) {
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
		do {
			restart = false;
			FOR_ALL_GUESTS(spriteIndex, peep) {
				if (peep->name_string_idx == 767) {
					peep_give_real_name(peep);
					peep_update_name_sort(peep);
					restart = true;
				}
			}
		} while (restart);
		gfx_invalidate_screen();
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) &= ~PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
		do {
			restart = false;
			FOR_ALL_GUESTS(spriteIndex, peep) {
				if (peep->name_string_idx < 0xA000)
					continue;
				if (peep->name_string_idx >= 0xE000)
					continue;

				peep->name_string_idx = 767;
				peep_update_name_sort(peep);
				restart = true;
			}
		} while (restart);
		gfx_invalidate_screen();
	}
}
