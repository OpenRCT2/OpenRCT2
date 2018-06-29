/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _RIDE_DATA_H_
#define _RIDE_DATA_H_

#include "../common.h"
#include "Ride.h"

struct ride_component_name {
    rct_string_id singular;
    rct_string_id plural;
    rct_string_id capitalised;
    rct_string_id capitalised_plural;
    rct_string_id count;
    rct_string_id count_plural;
    rct_string_id number;
};

enum RIDE_COMPONENT_TYPE {
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
};

struct rct_ride_name_convention {
    RIDE_COMPONENT_TYPE vehicle;
    RIDE_COMPONENT_TYPE structure;
    RIDE_COMPONENT_TYPE station;
};

struct rct_ride_entrance_definition {
    uint32_t sprite_index;
    uint16_t height;
    uint16_t scrolling_mode;
    uint32_t base_image_id;
    rct_string_id string_id;
    uint16_t colour_use_flags;
};

struct rct_ride_data_4 {
    uint8_t price;
    uint8_t price_secondary;
    uint16_t flags;
    uint8_t default_music;
    uint8_t alternate_type;
    uint8_t pad[2] = {};
};

struct ride_cost {
    uint16_t track_price;
    uint16_t support_price;
};

struct rct_ride_data_5 {
    uint8_t max_height;
    uint8_t clearance_height;
    int8_t z_offset;
    uint8_t max_mass;
    uint8_t z;
    uint8_t price;
    uint8_t bonus_value;      // Deprecated. Use rideBonusValue instead
    uint8_t pad = 0;
};

struct rct_ride_lift_data {
    uint8_t sound_id;
    uint8_t minimum_speed;
    uint8_t maximum_speed;
};

enum {
    RIDE_TYPE_FLAG4_ALLOW_DOORS_ON_TRACK = (1 << 0),
    RIDE_TYPE_FLAG4_MUSIC_ON_DEFAULT = (1 << 1),
    RIDE_TYPE_FLAG4_ALLOW_MUSIC = (1 << 2),
    RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE = (1 << 3),  // Used by the Flying RC, Lay-down RC, Multi-dimension RC
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
    RIDE_TYPE_ALTERNATIVE_TRACK_PIECES = 1, // Dinghy slide and Water Coaster
    RIDE_TYPE_ALTERNATIVE_TRACK_TYPE = 2,   // Flying RC, Lay-down RC, Multi-dimension RC
};

extern const bool hasRunningTrack[RIDE_TYPE_COUNT];
extern const uint8_t initialUpkeepCosts[RIDE_TYPE_COUNT];
extern const uint8_t costPerTrackPiece[RIDE_TYPE_COUNT];

extern const uint8_t costPerVehicle[RIDE_TYPE_COUNT];
extern const bool chargeUpkeepForTrainLength[RIDE_TYPE_COUNT];
extern const uint8_t costPerStation[RIDE_TYPE_COUNT];
extern const uint8_t rideBonusValue[RIDE_TYPE_COUNT];

extern const ride_component_name RideComponentNames[RIDE_COMPONENT_TYPE_COUNT];
extern const rct_ride_name_convention RideNameConvention[RIDE_TYPE_COUNT];
extern const rct_ride_name RideNaming[RIDE_TYPE_COUNT];
extern const uint8_t RideAvailableModes[];
extern const uint8_t AllRideModesAvailable[];
extern const uint8_t RideAvailableBreakdowns[];

extern const rct_ride_entrance_definition RideEntranceDefinitions[RIDE_ENTRANCE_STYLE_COUNT];
extern const rct_ride_lift_data RideLiftData[RIDE_TYPE_COUNT];

extern const rct_ride_data_4 RideData4[RIDE_TYPE_COUNT];
extern const int32_t RidePhotoItems[RIDE_TYPE_COUNT];
extern const ride_cost RideTrackCosts[RIDE_TYPE_COUNT];
extern const rct_ride_data_5 RideData5[RIDE_TYPE_COUNT];

extern const rct_ride_entry_vehicle CableLiftVehicle;

extern const uint16_t RideFilmLength[3];
extern const uint16_t RideCrookedHouseLength[1];

extern const rating_tuple RideRatings[RIDE_TYPE_COUNT];

extern const uint8_t RideConstructionDefaultTrackType[RIDE_TYPE_COUNT];

extern const track_colour_preset_list RideColourPresets[RIDE_TYPE_COUNT];

extern const rct_string_id RideModeNames[RIDE_MODE_COUNT];

#endif
