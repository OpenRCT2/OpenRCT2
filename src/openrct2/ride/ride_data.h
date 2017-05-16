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

#ifndef _RIDE_DATA_H_
#define _RIDE_DATA_H_

#include "../common.h"
#include "ride.h"

typedef struct ride_component_name {
	rct_string_id singular;
	rct_string_id plural;
	rct_string_id capitalised;
	rct_string_id capitalised_plural;
	rct_string_id count;
	rct_string_id count_plural;
	rct_string_id number;
} ride_component_name;

typedef enum  {
	RIDE_COMPONENT_TYPE_TRAIN,
	RIDE_COMPONENT_TYPE_BOAT,
	RIDE_COMPONENT_TYPE_TRACK,
	RIDE_COMPONENT_TYPE_DOCKING_PLATFORM,
	RIDE_COMPONENT_TYPE_STATION,
	RIDE_COMPONENT_TYPE_CAR,
	RIDE_COMPONENT_TYPE_BUILDING,
	RIDE_COMPONENT_TYPE_STRUCTURE,
	RIDE_COMPONENT_TYPE_SHIP,
	RIDE_COMPONENT_TYPE_CABIN,
	RIDE_COMPONENT_TYPE_WHEEL,
	RIDE_COMPONENT_TYPE_RING,
	RIDE_COMPONENT_TYPE_PLAYER,
	RIDE_COMPONENT_TYPE_COURSE,
	RIDE_COMPONENT_TYPE_COUNT
} ride_component_type;

typedef struct rct_ride_name_convention {
	ride_component_type vehicle;
	ride_component_type structure;
	ride_component_type station;
} rct_ride_name_convention;

typedef struct rct_ride_name {
	rct_string_id name;
	rct_string_id description;
} rct_ride_name;

typedef struct rct_ride_entrance_definition {
	uint32 sprite_index;
	uint16 height;
	uint16 scrolling_mode;
	uint32 base_image_id;
	rct_string_id string_id;
	uint16 colour_use_flags;
} rct_ride_entrance_definition;

typedef struct rct_shop_item_string_types {
	rct_string_id price_label;		// Balloon price:
	rct_string_id singular;			// Balloon
	rct_string_id plural;			// Balloons
	rct_string_id indefinite;		// a Balloon
	rct_string_id display;			// "Diamond Heights" Balloon
} rct_shop_item_string_types;

typedef struct rct_ride_data_4 {
	uint8 price;
	uint8 price_secondary;
	uint16 flags;
	uint8 default_music;
	uint8 alternate_type;
	uint8 pad[2];
} rct_ride_data_4;

typedef struct ride_cost {
	uint16 track_price;
	uint16 support_price;
} ride_cost;

typedef struct rct_ride_data_5 {
	uint8 max_height;
	uint8 clearance_height;
	sint8 z_offset;
	uint8 max_friction;
	uint8 z;
	uint8 price;
	uint8 bonus_value;		// Deprecated. Use rideBonusValue instead
	uint8 pad;
} rct_ride_data_5;

typedef struct rct_ride_lift_data {
	uint8 sound_id;
	uint8 minimum_speed;
	uint8 maximum_speed;
} rct_ride_lift_data;

enum {
	RIDE_TYPE_FLAG4_0 = (1 << 0),
	RIDE_TYPE_FLAG4_MUSIC_ON_DEFAULT = (1 << 1),
	RIDE_TYPE_FLAG4_ALLOW_MUSIC = (1 << 2),
	RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE = (1 << 3),	// Used by the Flying RC, Lay-down RC, Multi-dimension RC
	RIDE_TYPE_FLAG4_PEEP_CHECK_GFORCES = (1 << 4),
	RIDE_TYPE_FLAG4_HAS_ENTRANCE_EXIT = (1 << 5),
	RIDE_TYPE_FLAG4_ALLOW_MORE_VEHICLES_THAN_STATION_FITS = (1 << 6),
	RIDE_TYPE_FLAG4_HAS_AIR_TIME = (1 << 7),
	RIDE_TYPE_FLAG4_SINGLE_SESSION = (1 << 8),
	RIDE_TYPE_FLAG4_ALLOW_MULTIPLE_CIRCUITS = (1 << 9),
	RIDE_TYPE_FLAG4_ALLOW_CABLE_LIFT_HILL = (1 << 10),
	RIDE_TYPE_FLAG4_SHOW_IN_TRACK_DESIGNER = (1 << 11),
	RIDE_TYPE_FLAG4_TRANSPORT_RIDE = (1 << 12),
	RIDE_TYPE_FLAG4_INTERESTING_TO_LOOK_AT = (1 << 13),
	RIDE_TYPE_FLAG4_SLIGHTLY_INTERESTING_TO_LOOK_AT = (1 << 14),
	RIDE_TYPE_FLAG4_START_CONSTRUCTION_INVERTED = (1 << 15), // This is only set on the Flying RC and its alternative type.
};

enum {
	RIDE_TYPE_NO_ALTERNATIVES = 0,
	RIDE_TYPE_ALTERNATIVE_TRACK_PIECES = 1,	// Dinghy slide and Water Coaster
	RIDE_TYPE_ALTERNATIVE_TRACK_TYPE = 2,	// Flying RC, Lay-down RC, Multi-dimension RC
};

typedef struct rct_shop_item_stats {
	uint16 cost;
	uint16 base_value;
	uint16 hot_value;
	uint16 cold_value;
} rct_shop_item_stats;

extern const bool hasRunningTrack[RIDE_TYPE_COUNT];
extern const uint8 initialUpkeepCosts[RIDE_TYPE_COUNT];
extern const uint8 costPerTrackPiece[RIDE_TYPE_COUNT];

extern const uint8 costPerVehicle[RIDE_TYPE_COUNT];
extern const bool chargeUpkeepForTrainLength[RIDE_TYPE_COUNT];
extern const uint8 costPerStation[RIDE_TYPE_COUNT];
extern const uint8 rideBonusValue[RIDE_TYPE_COUNT];

extern const ride_component_name RideComponentNames[RIDE_COMPONENT_TYPE_COUNT];
extern const rct_ride_name_convention RideNameConvention[RIDE_TYPE_COUNT];
extern const rct_ride_name RideNaming[RIDE_TYPE_COUNT];
extern const uint8 RideAvailableModes[];
extern const uint8 AllRideModesAvailable[];
extern const uint8 RideAvailableBreakdowns[];

extern const rct_ride_entrance_definition RideEntranceDefinitions[RIDE_ENTRANCE_STYLE_COUNT];
extern const rct_ride_lift_data RideLiftData[RIDE_TYPE_COUNT];

extern const rct_ride_data_4 RideData4[RIDE_TYPE_COUNT];
extern const sint32 RidePhotoItems[RIDE_TYPE_COUNT];
extern const ride_cost RideTrackCosts[RIDE_TYPE_COUNT];
extern const rct_ride_data_5 RideData5[RIDE_TYPE_COUNT];

extern const money8 DefaultShopItemPrice[SHOP_ITEM_COUNT];
extern const rct_shop_item_string_types ShopItemStringIds[SHOP_ITEM_COUNT];
extern const uint32 ShopItemImage[SHOP_ITEM_COUNT];

extern const rct_ride_entry_vehicle CableLiftVehicle;

extern const uint16 RideFilmLength[3];
extern const uint16 RideCrookedHouseLength[1];

extern const rating_tuple RideRatings[RIDE_TYPE_COUNT];

extern const rct_shop_item_stats ShopItemStats[SHOP_ITEM_COUNT];

extern const uint8 RideConstructionDefaultTrackType[RIDE_TYPE_COUNT];

extern const track_colour_preset_list RideColourPresets[RIDE_TYPE_COUNT];

#endif
