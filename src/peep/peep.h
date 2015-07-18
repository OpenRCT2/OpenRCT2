/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill
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

#ifndef _PEEP_H_
#define _PEEP_H_

#include "../common.h"

#define PEEP_MAX_THOUGHTS 5

#define PEEP_HUNGER_WARNING_THRESHOLD 25
#define PEEP_THIRST_WARNING_THRESHOLD 25
#define PEEP_BATHROOM_WARNING_THRESHOLD 28
#define PEEP_LITTER_WARNING_THRESHOLD 23
#define PEEP_DISGUST_WARNING_THRESHOLD 22
#define PEEP_VANDALISM_WARNING_THRESHOLD 15
#define PEEP_NOEXIT_WARNING_THRESHOLD 8
#define PEEP_LOST_WARNING_THRESHOLD 8

enum PEEP_TYPE {
	PEEP_TYPE_GUEST,
	PEEP_TYPE_STAFF
};

enum PEEP_THOUGHT_TYPE {
	PEEP_THOUGHT_TYPE_SPENT_MONEY = 1, // "I've spent all my money"
	PEEP_THOUGHT_TYPE_SICK = 2, // "I feel sick"
	PEEP_THOUGHT_TYPE_VERY_SICK = 3, // "I feel very sick"
	PEEP_THOUGHT_TYPE_MORE_THRILLING = 4, // "I want to go on something more thrilling than X"
	PEEP_THOUGHT_TYPE_INTENSE = 5, // "X looks too intense for me"
	PEEP_THOUGHT_TYPE_HAVENT_FINISHED = 6, // "I haven't finished my X yet"
	PEEP_THOUGHT_TYPE_SICKENING = 7, // "Just looking at X makes me feel sick"
	PEEP_THOUGHT_TYPE_BAD_VALUE = 8, // "I'm not paying that much to go on X"
	PEEP_THOUGHT_TYPE_GO_HOME = 9, // "I want to go home"
	PEEP_THOUGHT_TYPE_GOOD_VALUE = 10, // "X is really good value"
	PEEP_THOUGHT_TYPE_ALREADY_GOT = 11, // "I've already got"
	PEEP_THOUGHT_TYPE_CANT_AFFORD = 12, // "I can't afford"
	PEEP_THOUGHT_TYPE_NOT_HUNGRY = 13, // "I'm not not hungry"
	PEEP_THOUGHT_TYPE_NOT_THIRSTY = 14, // "I'm not not thirsty"
	PEEP_THOUGHT_TYPE_DROWNING = 15, // "Help! I'm drowning!"
	PEEP_THOUGHT_TYPE_LOST = 16, // "I'm lost!"
	PEEP_THOUGHT_TYPE_WAS_GREAT = 17, // "X was great"
	PEEP_THOUGHT_TYPE_QUEUING_AGES = 18, // "I've been queuing for X for ages"
	PEEP_THOUGHT_TYPE_TIRED = 19, // "I'm tired"
	PEEP_THOUGHT_TYPE_HUNGRY = 20, // "I'm hungry"
	PEEP_THOUGHT_TYPE_THIRSTY = 21, // "I'm thirsty"
	PEEP_THOUGHT_TYPE_BATHROOM = 22, // "I need to go to the bathroom"
	PEEP_THOUGHT_TYPE_CANT_FIND = 23, // "I can't find X"
	PEEP_THOUGHT_TYPE_NOT_PAYING = 24, // "I'm not paying that much to use X"
	PEEP_THOUGHT_TYPE_NOT_WHILE_RAINING = 25, // "I'm not going on X while it's raining"
	PEEP_THOUGHT_TYPE_BAD_LITTER = 26, // "The litter here is really bad"
	PEEP_THOUGHT_TYPE_CANT_FIND_EXIT = 27, // "I can't find the exit"
	PEEP_THOUGHT_TYPE_GET_OFF = 28, // "I want to get off X"
	PEEP_THOUGHT_TYPE_GET_OUT = 29, // "I want to get out of X"
	PEEP_THOUGHT_TYPE_NOT_SAFE = 30, // "I'm not going on X - it isn't safe"
	PEEP_THOUGHT_TYPE_PATH_DISGUSTING = 31, // "This path is disgusting"
	PEEP_THOUGHT_TYPE_CROWDED = 32, // "It's too crowded here"
	PEEP_THOUGHT_TYPE_VANDALISM = 33, // "The vandalism here is really bad"
	PEEP_THOUGHT_TYPE_SCENERY = 34, // "Great scenery!"
	PEEP_THOUGHT_VERY_CLEAN = 35, // "This park is very clean and tidy"
	PEEP_THOUGHT_TYPE_FOUNTAINS = 36, // "The jumping fountains are great"
	PEEP_THOUGHT_TYPE_MUSIC = 37, // "The music is nice here"
	PEEP_THOUGHT_TYPE_BALLOON = 38, // "This baloon from X is really good value"
	PEEP_THOUGHT_TYPE_TOY = 39, // "This cuddly toy from X is really good value"
	PEEP_THOUGHT_TYPE_MAP = 40,
	PEEP_THOUGHT_TYPE_PHOTO = 41, // "This on ride photo from X is really good value"
	PEEP_THOUGHT_TYPE_UMBRELLA = 42,
	PEEP_THOUGHT_TYPE_DRINK = 43,
	PEEP_THOUGHT_TYPE_BURGER = 44,
	PEEP_THOUGHT_TYPE_FRIES = 45,
	PEEP_THOUGHT_TYPE_ICE_CREAM = 46,
	PEEP_THOUGHT_TYPE_COTTON_CANDY = 47,

	PEEP_THOUGHT_TYPE_PIZZA = 51,

	PEEP_THOUGHT_TYPE_POPCORN = 53,
	PEEP_THOUGHT_TYPE_HOT_DOG = 54,
	PEEP_THOUGHT_TYPE_TENTACLE = 55,
	PEEP_THOUGHT_TYPE_HAT = 56,
	PEEP_THOUGHT_TYPE_CANDY_APPLE = 57,
	PEEP_THOUGHT_TYPE_TSHIRT = 58,
	PEEP_THOUGHT_TYPE_DONUT = 59,
	PEEP_THOUGHT_TYPE_COFFEE = 60,

	PEEP_THOUGHT_TYPE_CHICKEN = 62,
	PEEP_THOUGHT_TYPE_LEMONADE = 63,

	PEEP_THOUGHT_TYPE_WOW = 67, // "Wow!"

	PEEP_THOUGHT_TYPE_WOW2 = 70,// "Wow!"
	PEEP_THOUGHT_TYPE_WATCHED = 71, // "I have the strangest feeling someone is watching me"
	PEEP_THOUGHT_TYPE_BALLOON_MUCH = 72, // "I'm not paying that much to get a balloon from X"
	PEEP_THOUGHT_TYPE_TOY_MUCH = 73,
	PEEP_THOUGHT_TYPE_MAP_MUCH = 74,
	PEEP_THOUGHT_TYPE_PHOTO_MUCH = 75,
	PEEP_THOUGHT_TYPE_UMBRELLA_MUCH = 76,
	PEEP_THOUGHT_TYPE_DRINK_MUCH = 77,
	PEEP_THOUGHT_TYPE_BURGER_MUCH = 78,
	PEEP_THOUGHT_TYPE_FRIES_MUCH = 79,
	PEEP_THOUGHT_TYPE_ICE_CREAM_MUCH = 80,
	PEEP_THOUGHT_TYPE_COTTON_CANDY_MUCH = 81,

	PEEP_THOUGHT_TYPE_PIZZA_MUCH = 85,

	PEEP_THOUGHT_TYPE_POPCORN_MUCH = 87,
	PEEP_THOUGHT_TYPE_HOT_DOG_MUCH = 88,
	PEEP_THOUGHT_TYPE_TENTACLE_MUCH = 89,
	PEEP_THOUGHT_TYPE_HAT_MUCH = 90,
	PEEP_THOUGHT_TYPE_CANDY_APPLE_MUCH = 91,
	PEEP_THOUGHT_TYPE_TSHIRT_MUCH = 92,
	PEEP_THOUGHT_TYPE_DONUT_MUCH = 93,
	PEEP_THOUGHT_TYPE_COFFEE_MUCH = 94,

	PEEP_THOUGHT_TYPE_CHICKEN_MUCH = 96,
	PEEP_THOUGHT_TYPE_LEMONADE_MUCH = 97,

	PEEP_THOUGHT_TYPE_PHOTO2 = 104, // "This on-ride photo from X is really good value"
	PEEP_THOUGHT_TYPE_PHOTO3 = 105,
	PEEP_THOUGHT_TYPE_PHOTO4 = 106,
	PEEP_THOUGHT_TYPE_PRETZEL = 107,
	PEEP_THOUGHT_TYPE_HOT_CHOCOLATE = 108,
	PEEP_THOUGHT_TYPE_ICED_TEA = 109,
	PEEP_THOUGHT_TYPE_FUNNEL_CAKE = 110,
	PEEP_THOUGHT_TYPE_SUNGLASSES = 111,
	PEEP_THOUGHT_TYPE_BEEF_NOODLES = 112,
	PEEP_THOUGHT_TYPE_FRIED_RICE_NOODLES = 113,
	PEEP_THOUGHT_TYPE_WONTON_SOUP = 114,
	PEEP_THOUGHT_TYPE_MEATBALL_SOUP = 115,
	PEEP_THOUGHT_TYPE_FRUIT_JUICE = 116,
	PEEP_THOUGHT_TYPE_SOYBEAN_MILK = 117,
	PEEP_THOUGHT_TYPE_SU_JONGKWA = 118,
	PEEP_THOUGHT_TYPE_SUB_SANDWICH = 119,
	PEEP_THOUGHT_TYPE_COOKIE = 120,

	PEEP_THOUGH_ROAST_SAUSAGE = 124,

	PEEP_THOUGHT_TYPE_PHOTO2_MUCH = 136,
	PEEP_THOUGHT_TYPE_PHOTO3_MUCH = 137,
	PEEP_THOUGHT_TYPE_PHOTO4_MUCH = 138,
	PEEP_THOUGHT_TYPE_PRETZEL_MUCH = 139,
	PEEP_THOUGHT_TYPE_HOT_CHOCOLATE_MUCH = 140,
	PEEP_THOUGHT_TYPE_ICED_TEA_MUCH = 141,
	PEEP_THOUGHT_TYPE_FUNNEL_CAKE_MUCH = 142,
	PEEP_THOUGHT_TYPE_SUNGLASSES_MUCH = 143,
	PEEP_THOUGHT_TYPE_BEEF_NOODLES_MUCH = 144,
	PEEP_THOUGHT_TYPE_FRIED_RICE_NOODLES_MUCH = 145,
	PEEP_THOUGHT_TYPE_WONTON_SOUP_MUCH = 146,
	PEEP_THOUGHT_TYPE_MEATBALL_SOUP_MUCH = 147,
	PEEP_THOUGHT_TYPE_FRUIT_JUICE_MUCH = 148,
	PEEP_THOUGHT_TYPE_SOYBEAN_MILK_MUCH = 149,
	PEEP_THOUGHT_TYPE_SU_JONGKWA_MUCH = 150,
	PEEP_THOUGHT_TYPE_SUB_SANDWICH_MUCH = 151,
	PEEP_THOUGHT_TYPE_COOKIE_MUCH = 152,

	PEEP_THOUGHT_ROAST_SAUSAGE_MUCH = 156,

	PEEP_THOUGHT_HELP = 168, // "Help! Put me down!"
	PEEP_THOUGHT_RUNNING_OUT = 169, // I'm running out of cash!"
	PEEP_THOUGHT_NEW_RIDE = 170, // "Wow! A new ride being built!"
	PEEP_THOUGHT_NICE_RIDE = 171, // "Nice ride! But not as good as the Phoenix..."
	PEEP_THOUGHT_EXCITED = 172, // "I'm so excited - It's an Intamin ride!"
	PEEP_THOUGHT_HERE_WE_ARE = 173, // "...and here we are on X!"

	PEEP_THOUGHT_TYPE_NONE = 255
};

enum PEEP_STATE {
	PEEP_STATE_FALLING = 0, //Drowning is part of falling
	PEEP_STATE_1 = 1,
	PEEP_STATE_QUEUING_FRONT = 2,
	PEEP_STATE_ON_RIDE = 3,
	PEEP_STATE_LEAVING_RIDE = 4,
	PEEP_STATE_WALKING = 5,
	PEEP_STATE_QUEUING = 6,
	PEEP_STATE_ENTERING_RIDE = 7,
	PEEP_STATE_SITTING = 8,
	PEEP_STATE_PICKED = 9,
	PEEP_STATE_PATROLLING = 10, // Not sure
	PEEP_STATE_MOWING = 11,
	PEEP_STATE_SWEEPING = 12,
	PEEP_STATE_ENTERING_PARK = 13,
	PEEP_STATE_LEAVING_PARK = 14,
	PEEP_STATE_ANSWERING = 15,
	PEEP_STATE_FIXING = 16,
	PEEP_STATE_BUYING = 17,
	PEEP_STATE_WATCHING = 18,
	PEEP_STATE_EMPTYING_BIN = 19,
	PEEP_STATE_USING_BIN = 20,
	PEEP_STATE_WATERING = 21,
	PEEP_STATE_HEADING_TO_INSPECTION = 22,
	PEEP_STATE_INSPECTING = 23
};

enum PEEP_ACTION_EVENTS {	
	PEEP_ACTION_CHECK_TIME = 0,
	// If no food then check watch
	PEEP_ACTION_EAT_FOOD = 1,
	PEEP_ACTION_SHAKE_HEAD = 2,
	PEEP_ACTION_EMPTY_POCKETS = 3,	
	PEEP_ACTION_SITTING_EAT_FOOD = 4,
	PEEP_ACTION_SITTING_CHECK_WATCH = 4,
	PEEP_ACTION_SITTING_LOOK_AROUND_LEFT = 5,
	PEEP_ACTION_SITTING_LOOK_AROUND_RIGHT = 6,	
	PEEP_ACTION_WOW = 7,
	PEEP_ACTION_THROW_UP = 8,
	PEEP_ACTION_JUMP = 9,
	PEEP_ACTION_STAFF_SWEEP = 10,
	PEEP_ACTION_DROWNING = 11,
	PEEP_ACTION_STAFF_ANSWER_CALL = 12,
	PEEP_ACTION_STAFF_ANSWER_CALL_2 = 13,
	PEEP_ACTION_STAFF_CHECKBOARD = 14,
	PEEP_ACTION_STAFF_FIX = 15,
	PEEP_ACTION_STAFF_FIX_2 = 16,
	PEEP_ACTION_STAFF_FIX_GROUND = 17,
	PEEP_ACTION_STAFF_WATERING = 19,
	PEEP_ACTION_READ_MAP = 21,
	PEEP_ACTION_WAVE = 22,
	PEEP_ACTION_STAFF_EMPTY_BIN = 23,
	PEEP_ACTION_WAVE_2 = 24,
	PEEP_ACTION_TAKE_PHOTO = 25,
	PEEP_ACTION_CLAP = 26,

	PEEP_ACTION_DRAW_PICTURE = 28,

	PEEP_ACTION_WITHDRAW_MONEY = 30,

	PEEP_ACTION_NONE_1 = 254,
	PEEP_ACTION_NONE_2 = 255
};

enum PEEP_FLAGS {
	PEEP_FLAGS_LEAVING_PARK = (1 << 0),
	PEEP_FLAGS_SLOW_WALK = (1 << 1),

	PEEP_FLAGS_TRACKING = (1 << 3),
	PEEP_FLAGS_WAVING = (1 << 4), // Makes the peep wave

	PEEP_FLAGS_PHOTO = (1 << 6), // Makes the peep take a picture
	PEEP_FLAGS_PAINTING = (1 << 7),
	PEEP_FLAGS_WOW = (1 << 8), // Makes a peep WOW2
	PEEP_FLAGS_LITTER = (1 << 9), // Makes the peep throw litter
	PEEP_FLAGS_LOST = (1 << 10), // Makes the peep feel lost (animation trigerred)
	PEEP_FLAGS_HUNGER = (1 << 11), // Makes the peep become hungry quicker
	PEEP_FLAGS_BATHROOM = (1 << 12), // Makes the peep want to go to the bathroom
	PEEP_FLAGS_CROWDED = (1 << 13), // The peep will start feeling crowded
	PEEP_FLAGS_HAPPINESS = (1 << 14), // The peep will start increasing happiness
	PEEP_FLAGS_NAUSEA = (1 << 15), // Makes the peep feel sick (e.g. after an extreme ride)

	PEEP_FLAGS_EATING = (1 << 17), // Reduces hunger
	PEEP_FLAGS_EXPLODE = (1 << 18),
	PEEP_FLAGS_20 = (1 << 20),
	PEEP_FLAGS_21 = (1 << 21),

	PEEP_FLAGS_JOY = (1 << 23), // Makes the peep jump in joy
	PEEP_FLAGS_ANGRY = (1 << 24),
	PEEP_FLAGS_ICE_CREAM = (1 << 25), // Unconfirmed

	PEEP_FLAGS_TWITCH = (1 << 31)		// Added for twitch integration
};

enum PEEP_NAUSEA_TOLERANCE {
	PEEP_NAUSEA_TOLERANCE_NONE,
	PEEP_NAUSEA_TOLERANCE_LOW,
	PEEP_NAUSEA_TOLERANCE_AVERAGE,
	PEEP_NAUSEA_TOLERANCE_HIGH
};

enum PEEP_ITEM {
	// item_standard_flags
	PEEP_ITEM_BALLOON = (1 << 0),
	PEEP_ITEM_TOY = (1 << 1),
	PEEP_ITEM_MAP = (1 << 2),
	PEEP_ITEM_PHOTO = (1 << 3),
	PEEP_ITEM_UMBRELLA = (1 << 4),
	PEEP_ITEM_DRINK = (1 << 5),
	PEEP_ITEM_BURGER = (1 << 6),
	PEEP_ITEM_FRIES = (1 << 7),
	PEEP_ITEM_ICE_CREAM = (1 << 8),
	PEEP_ITEM_COTTON_CANDY = (1 << 9),
	PEEP_ITEM_EMPTY_CAN = (1 << 10),
	PEEP_ITEM_RUBBISH = (1 << 11),
	PEEP_ITEM_EMPTY_BURGER_BOX = (1 << 12),
	PEEP_ITEM_PIZZA = (1 << 13),
	PEEP_ITEM_VOUCHER = (1 << 14),
	PEEP_ITEM_POPCORN = (1 << 15),
	PEEP_ITEM_HOT_DOG = (1 << 16),
	PEEP_ITEM_TENTACLE = (1 << 17),
	PEEP_ITEM_HAT = (1 << 18),
	PEEP_ITEM_CANDY_APPLE = (1 << 19),
	PEEP_ITEM_TSHIRT = (1 << 20),
	PEEP_ITEM_DONUT = (1 << 21),
	PEEP_ITEM_COFFEE = (1 << 22),
	PEEP_ITEM_EMPTY_CUP = (1 << 23),
	PEEP_ITEM_CHICKEN = (1 << 24),
	PEEP_ITEM_LEMONADE = (1 << 25),
	PEEP_ITEM_EMPTY_BOX = (1 << 26),
	PEEP_ITEM_EMPTY_BOTTLE = (1 << 27),

	// item_extra_flags
	PEEP_ITEM_PHOTO2 = (1 << 0),
	PEEP_ITEM_PHOTO3 = (1 << 1),
	PEEP_ITEM_PHOTO4 = (1 << 2),
	PEEP_ITEM_PRETZEL = (1 << 3),
	PEEP_ITEM_CHOCOLATE = (1 << 4),
	PEEP_ITEM_ICED_TEA = (1 << 5),
	PEEP_ITEM_FUNNEL_CAKE = (1 << 6),
	PEEP_ITEM_SUNGLASSES = (1 << 7),
	PEEP_ITEM_BEEF_NOODLES = (1 << 8),
	PEEP_ITEM_FRIED_RICE_NOODLES = (1 << 9),
	PEEP_ITEM_WONTON_SOUP = (1 << 10),
	PEEP_ITEM_MEATBALL_SOUP = (1 << 11),
	PEEP_ITEM_FRUIT_JUICE = (1 << 12),
	PEEP_ITEM_SOYBEAN_MILK = (1 << 13),
	PEEP_ITEM_SU_JONGKWA = (1 << 14),
	PEEP_ITEM_SUB_SANDWICH = (1 << 15),
	PEEP_ITEM_COOKIE = (1 << 16),
	PEEP_ITEM_EMPTY_BOWL_RED = (1 << 17),
	PEEP_ITEM_EMPTY_DRINK_CARTON = (1 << 18),
	PEEP_ITEM_EMPTY_JUICE_CUP = (1 << 19),
	PEEP_ITEM_ROAST_SAUSAGE = (1 << 20),
	PEEP_ITEM_EMPTY_BOWL_BLUE = (1 << 21)
};

typedef struct {
	uint8 type;		//0
	uint8 item;		//1
	uint8 var_2;	//2
	uint8 var_3;	//3
} rct_peep_thought;

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 var_01;
	uint16 next_in_quadrant;		// 0x02
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
	// Height from center of sprite to bottom
	uint8 sprite_height_negative;	// 0x09
	uint16 sprite_index;			// 0x0A
	uint16 var_0C;
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	// Width from center of sprite to edge
	uint8 sprite_width;				// 0x14
	// Height from center of sprite to top
	uint8 sprite_height_positive;	// 0x15
	sint16 sprite_left;				// 0x16
	sint16 sprite_top;				// 0x18
	sint16 sprite_right;			// 0x1A
	sint16 sprite_bottom;			// 0x1C
	uint8 sprite_direction;			// 0x1E
	uint8 pad_1F[3];
	uint16 name_string_idx;			// 0x22
	uint16 next_x;					// 0x24
	uint16 next_y;					// 0x26
	uint8 next_z;					// 0x28
	uint8 next_var_29;				// 0x29
	uint8 var_2A;
	uint8 state;					// 0x2B
	uint8 sub_state;				// 0x2C
	uint8 sprite_type;				// 0x2D
	uint8 type;						// 0x2E
	union{							
		uint8 staff_type;			// 0x2F
		uint8 no_of_rides;			// 0x2F
	};
	uint8 tshirt_colour;			// 0x30
	uint8 trousers_colour;			// 0x31
	uint16 destination_x;			// 0x32 Location that the peep is trying to get to
	uint16 destination_y;			// 0x34 
	uint8 destination_tolerence;	// 0x36 How close to destination before next action/state 0 = exact
	uint8 var_37;
	uint8 energy;					// 0x38
	uint8 energy_growth_rate;		// 0x39
	uint8 happiness;				// 0x3A
	uint8 happiness_growth_rate;	// 0x3B
	uint8 nausea;					// 0x3C
	uint8 nausea_growth_rate;		// 0x3D
	uint8 hunger;					// 0x3E
	uint8 thirst;					// 0x3F
	uint8 bathroom;					// 0x40
	uint8 var_41;
	uint8 var_42;
	uint8 intensity;				// 0x43
	uint8 nausea_tolerance;			// 0x44
	uint8 var_45;					//		Some sort of flags?
	money16 paid_on_drink;			// 0x46
	uint8 var_48[16];
	uint32 item_extra_flags;		// 0x58
	uint8 photo2_ride_ref;			// 0x5C
	uint8 photo3_ride_ref;			// 0x5D
	uint8 photo4_ride_ref;			// 0x5E
	uint8 pad_5F[0x09];				// 0x5C
	uint8 current_ride;				// 0x68
	uint8 current_ride_station;		// 0x69
	uint8 current_train;   	        // 0x6A
	union{
		struct{
			uint8 current_car;		// 0x6B
			uint8 current_seat;		// 0x6C
		};
		uint16 time_to_sitdown;		//0x6B
		struct{
			uint8 time_to_stand;	//0x6B
			uint8 standing_flags;	//0x6C
		};
	};
	uint8 var_6D;					// 0x6D
	uint8 action_sprite_type;		// 0x6E
	uint8 var_6F;
	uint8 action_sprite_image_offset; // 0x70
	uint8 action;					// 0x71
	uint8 action_frame;				// 0x72
	uint8 var_73;
	union {
		uint16 var_74; // time getting to ride to fix
		uint16 next_in_queue;		// 0x74
	};
	uint8 var_76;
	uint8 pad_77;
	union{
		uint8 maze_last_edge;			// 0x78
		uint8 var_78;
	};
	uint8 var_79;
	uint16 time_in_queue;			// 0x7A
	uint8 rides_been_on[32];		// 0x7C 
	// 255 bit bitmap of every ride the peep has been on see
	// window_peep_rides_update for how to use.
	uint32 id;						// 0x9C
	money32 cash_in_pocket;			// 0xA0
	money32 cash_spent;				// 0xA4
	sint32 time_in_park;			// 0xA8
	sint8 var_AC;					// 0xAC
	uint8 previous_ride;			// 0xAD
	uint16 previous_ride_time_out;	// 0xAE
	rct_peep_thought thoughts[PEEP_MAX_THOUGHTS];	// 0xB0
	uint8 var_C4;					// 0xC4
	union							// 0xC5
	{
		uint8 staff_id;
		uint8 guest_heading_to_ride_id;
	};
	union{
		uint8 staff_orders;			// 0xC6
		uint8 var_C6;
	};
	uint8 photo1_ride_ref;			// 0xC7
	uint32 flags;					// 0xC8
	uint32 var_CC;
	uint8 pad_D0[0x10];
	uint8 no_action_frame_no;		// 0xE0
	uint8 var_E1;
	union{
		uint8 time_on_ride;			// 0xE2
		uint8 var_E2;				// 0xE2
	};
	uint8 var_E3;
	union{
		money16 paid_to_enter;			// 0xE4
		uint16 staff_lawns_mown;		// 0xE4
		uint16 staff_rides_fixed;		// 0xE4
	};
	union{
		money16 paid_on_rides;			// 0xE6
		uint16 staff_gardens_watered;	// 0xE6
		uint16 staff_rides_inspected;	// 0xE6
	};
	union {
		money16 paid_on_food;			// 0xE8
		uint16 staff_litter_swept;		// 0xE8
	};
	union{
		money16 paid_on_souvenirs;		// 0xEA
		uint16 staff_bins_emptied;		// 0xEA
	};
	uint8 no_of_food;				// 0xEC
	uint8 no_of_drinks;				// 0xED
	uint8 no_of_souvenirs;			// 0xEE
	uint8 var_EF;
	uint8 voucher_type;				// 0xF0
	uint8 voucher_arguments;		// 0xF1 ride_id or string_offset_id
	uint8 var_F2;
	uint8 var_F3;
	uint8 var_F4;
	uint8 days_in_queue;			// 0xF5
	uint8 balloon_colour;			// 0xF6
	uint8 umbrella_colour;			// 0xF7
	uint8 hat_colour;				// 0xF8
	uint8 favourite_ride;			// 0xF9
	uint8 var_FA;
	uint8 pad_FB;
	uint32 item_standard_flags;		// 0xFC
} rct_peep;

enum {
	EASTEREGG_PEEP_NAME_MICHAEL_SCHUMACHER,
	EASTEREGG_PEEP_NAME_JACQUES_VILLENEUVE,
	EASTEREGG_PEEP_NAME_DAMON_HILL,
	EASTEREGG_PEEP_NAME_MR_BEAN,
	EASTEREGG_PEEP_NAME_CHRIS_SAWYER,
	EASTEREGG_PEEP_NAME_KATIE_BRAYSHAW,
	EASTEREGG_PEEP_NAME_MELANIE_WARN,
	EASTEREGG_PEEP_NAME_SIMON_FOSTER,
	EASTEREGG_PEEP_NAME_JOHN_WARDLEY,
	EASTEREGG_PEEP_NAME_LISA_STIRLING,
	EASTEREGG_PEEP_NAME_DONALD_MACRAE,
	EASTEREGG_PEEP_NAME_KATHERINE_MCGOWAN,
	EASTEREGG_PEEP_NAME_FRANCES_MCGOWAN,
	EASTEREGG_PEEP_NAME_CORINA_MASSOURA,
	EASTEREGG_PEEP_NAME_CAROL_YOUNG,
	EASTEREGG_PEEP_NAME_MIA_SHERIDAN,
	EASTEREGG_PEEP_NAME_KATIE_RODGER,
	EASTEREGG_PEEP_NAME_EMMA_GARRELL,
	EASTEREGG_PEEP_NAME_JOANNE_BARTON,
	EASTEREGG_PEEP_NAME_FELICITY_ANDERSON,
	EASTEREGG_PEEP_NAME_KATIE_SMITH,
	EASTEREGG_PEEP_NAME_EILIDH_BELL,
	EASTEREGG_PEEP_NAME_NANCY_STILLWAGON,
	EASTEREGG_PEEP_NAME_ANDY_HINE,
	EASTEREGG_PEEP_NAME_ELISSA_WHITE,
	EASTEREGG_PEEP_NAME_DAVID_ELLIS
};

/** Helper macro until rides are stored in this module. */
#define GET_PEEP(sprite_index) &(g_sprite_list[sprite_index].peep)

/**
 * Helper macro loop for enumerating through all the non null rides. To avoid needing a end loop counterpart, statements are
 * applied in tautology if statements.
 */
#define FOR_ALL_PEEPS(sprite_index, peep) \
	for (sprite_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16); sprite_index != SPRITE_INDEX_NULL; sprite_index = peep->next) \
		if ((peep = GET_PEEP(sprite_index)) || 1)

#define FOR_ALL_GUESTS(sprite_index, peep) \
	FOR_ALL_PEEPS(sprite_index, peep) \
		if (peep->type == PEEP_TYPE_GUEST)

#define FOR_ALL_STAFF(sprite_index, peep) \
	FOR_ALL_PEEPS(sprite_index, peep) \
		if (peep->type == PEEP_TYPE_STAFF)

int peep_get_staff_count();
int peep_can_be_picked_up(rct_peep* peep);
void peep_update_all();
void peep_problem_warnings_update();
void peep_update_crowd_noise();
void peep_update_days_in_queue();
void peep_applause();
rct_peep *peep_generate(int x, int y, int z);
void get_arguments_from_action(rct_peep* peep, uint32 *argument_1, uint32* argument_2);
void get_arguments_from_thought(rct_peep_thought thought, uint32* argument_1, uint32* argument_2);
int get_peep_face_sprite_small(rct_peep *peep);
int get_peep_face_sprite_large(rct_peep *peep);
int peep_check_easteregg_name(int index, rct_peep *peep);
int peep_get_easteregg_name_id(rct_peep *peep);
int peep_is_mechanic(rct_peep *peep);
int peep_has_food(rct_peep* peep);
void peep_sprite_remove(rct_peep* peep);
void peep_remove(rct_peep* peep);
void peep_update_sprite_type(rct_peep* peep);

void peep_window_state_update(rct_peep* peep);
void peep_decrement_num_riders(rct_peep* peep);
/**
* rct2: 0x699F5A
* al:thought_type
* ah:thought_arguments
* esi: peep
*/
void peep_insert_new_thought(rct_peep *peep, uint8 thought_type, uint8 thought_arguments);

void peep_set_map_tooltip(rct_peep *peep);
void sub_693B58(rct_peep* peep);
void remove_peep_from_ride(rct_peep* peep);
void remove_peep_from_queue(rct_peep* peep);

void sub_693BE5(rct_peep* peep, uint8 al);
void peep_update_name_sort(rct_peep *peep);
void peep_update_names(bool realNames);

#endif
