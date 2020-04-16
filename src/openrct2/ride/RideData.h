/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
#include "TrackPaint.h"

struct ride_component_name
{
    rct_string_id singular;
    rct_string_id plural;
    rct_string_id capitalised;
    rct_string_id capitalised_plural;
    rct_string_id count;
    rct_string_id count_plural;
    rct_string_id number;
};

enum RIDE_COMPONENT_TYPE
{
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

struct rct_ride_name_convention
{
    RIDE_COMPONENT_TYPE vehicle;
    RIDE_COMPONENT_TYPE structure;
    RIDE_COMPONENT_TYPE station;
};

struct rct_ride_data_4
{
    uint8_t price;
    uint8_t price_secondary;
    uint8_t default_music;
    uint8_t alternate_type;
};

struct ride_cost
{
    uint16_t track_price;
    uint16_t support_price;
};

struct rct_ride_data_5
{
    uint8_t max_height;
    uint8_t clearance_height;
    int8_t z_offset;
    uint8_t max_mass;
    uint8_t z;
    uint8_t price;
    uint8_t bonus_value; // Deprecated. Use rideBonusValue instead
    uint8_t pad = 0;
};

struct rct_ride_lift_data
{
    SoundId sound_id;
    uint8_t minimum_speed;
    uint8_t maximum_speed;
};

struct RideTypeDescriptor
{
    // rct2: 0x0097C468 (0 - 31) and 0x0097C5D4 (32 - 63)
    uint64_t EnabledTrackPieces;
    // Pieces that this ride type _can_ draw, but are disabled because their vehicles lack the relevant sprites,
    // or because they are not realistic for the ride type (e.g. LIM boosters in Mini Roller Coasters).
    uint64_t ExtraTrackPieces;
    TRACK_PAINT_FUNCTION_GETTER TrackPaintFunction;
    uint64_t Flags;
};

#ifdef _WIN32
#    define SET_FIELD(fieldname, value) value
#else
#    define SET_FIELD(fieldname, value) .fieldname = value
#endif

extern const RideTypeDescriptor RideTypeDescriptors[RIDE_TYPE_COUNT];

enum
{
    RIDE_TYPE_NO_ALTERNATIVES = 0,
    RIDE_TYPE_ALTERNATIVE_TRACK_PIECES = 1, // Dinghy slide and Water Coaster
    RIDE_TYPE_ALTERNATIVE_TRACK_TYPE = 2,   // Flying RC, Lay-down RC, Multi-dimension RC
};

enum ride_type_flags : uint64_t
{
    RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN = (1ULL << 0),
    RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL = (1ULL << 1),
    RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS = (1ULL << 2),
    RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION = (1ULL << 3), // Set by flat rides, tower rides and shops/stalls.
    RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION = (1ULL << 4),
    RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS = (1ULL << 5),
    RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER = (1ULL << 6), // used only by boat Hire and submarine ride
    RIDE_TYPE_FLAG_HAS_G_FORCES = (1ULL << 7),
    RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS = (1ULL << 8), // used by rides that can't have gaps, like those with a vertical tower, such
                                                   // as the observation tower
    RIDE_TYPE_FLAG_HAS_DATA_LOGGING = (1ULL << 9),
    RIDE_TYPE_FLAG_HAS_DROPS = (1ULL << 10),
    RIDE_TYPE_FLAG_NO_TEST_MODE = (1ULL << 11),
    RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES = (1ULL << 12), // used by rides with two varieties, like the u and o
                                                                     // shapes of the dinghy slide and the dry and submerged
                                                                     // track of the water coaster
    RIDE_TYPE_FLAG_NO_VEHICLES = (1ULL << 13),                       // used only by maze, spiral slide and shops
    RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS = (1ULL << 14),
    RIDE_TYPE_FLAG_HAS_NO_TRACK = (1ULL << 15),
    RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL = (1ULL << 16), // Set by flat rides where the vehicle is integral to the structure, like
    // Merry-go-round and swinging ships. (Contrast with rides like dodgems.)
    RIDE_TYPE_FLAG_IS_SHOP = (1ULL << 17),
    RIDE_TYPE_FLAG_TRACK_NO_WALLS = (1ULL << 18), // if set, wall scenery can not share a tile with the ride's track
    RIDE_TYPE_FLAG_FLAT_RIDE = (1ULL << 19),
    RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN = (1ULL << 20), // whether or not guests will go on the ride again if they liked it
                                                        // (this is
    // usually applied to everything apart from transport rides)
    RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY = (1ULL << 21), // used by toilets and first aid to mark that peep should go
                                                                  // inside the building (rather than 'buying' at the counter)
    RIDE_TYPE_FLAG_IN_RIDE = (1ULL << 22),                        // peeps are "IN" (ride) rather than "ON" (ride)
    RIDE_TYPE_FLAG_SELLS_FOOD = (1ULL << 23),
    RIDE_TYPE_FLAG_SELLS_DRINKS = (1ULL << 24),
    RIDE_TYPE_FLAG_IS_TOILET = (1ULL << 25),
    RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS = (1ULL << 26), // whether or not vehicle colours can be set
    RIDE_TYPE_FLAG_CHECK_FOR_STALLING = (1ULL << 27),
    RIDE_TYPE_FLAG_HAS_TRACK = (1ULL << 28),
    RIDE_TYPE_FLAG_ALLOW_EXTRA_TOWER_BASES = (1ULL << 29), // Only set by lift
    RIDE_TYPE_FLAG_HAS_LARGE_CURVES = (1ULL << 30),        // whether the ride supports large (45 degree turn) curves
    RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR = (1ULL << 31),

    RIDE_TYPE_FLAG_ALLOW_DOORS_ON_TRACK = (1ULL << 32),
    RIDE_TYPE_FLAG_MUSIC_ON_DEFAULT = (1ULL << 33),
    RIDE_TYPE_FLAG_ALLOW_MUSIC = (1ULL << 34),
    RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE = (1ULL << 35), // Used by the Flying RC, Lay-down RC, Multi-dimension RC
    RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES = (1ULL << 36),
    RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT = (1ULL << 37),
    RIDE_TYPE_FLAG_ALLOW_MORE_VEHICLES_THAN_STATION_FITS = (1ULL << 38),
    RIDE_TYPE_FLAG_HAS_AIR_TIME = (1ULL << 39),
    RIDE_TYPE_FLAG_SINGLE_SESSION = (1ULL << 40),
    RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS = (1ULL << 41),
    RIDE_TYPE_FLAG_ALLOW_CABLE_LIFT_HILL = (1ULL << 42),
    RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER = (1ULL << 43),
    RIDE_TYPE_FLAG_TRANSPORT_RIDE = (1ULL << 44),
    RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT = (1ULL << 45),
    RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT = (1ULL << 46),
    RIDE_TYPE_FLAG_START_CONSTRUCTION_INVERTED = (1ULL << 47), // This is only set on the Flying RC and its alternative type.
};

// Set on ride types that have a main colour, additional colour and support colour.
constexpr const uint64_t RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS = RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN
    | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS;
// Set on _all_ roller coaster ride types, including the _ALT types used for constructing upside down.
constexpr const uint64_t RIDE_TYPE_FLAGS_COMMON_COASTER = RIDE_TYPE_FLAG_HAS_G_FORCES | RIDE_TYPE_FLAG_HAS_DATA_LOGGING
    | RIDE_TYPE_FLAG_HAS_DROPS | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN
    | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_CHECK_FOR_STALLING | RIDE_TYPE_FLAG_HAS_TRACK
    | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT
    | RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS;
// Set on all roller coaster ride types, excluding the _ALT types used for constructing upside down.
constexpr const uint64_t RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT = RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER
    | RIDE_TYPE_FLAG_HAS_AIR_TIME | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT;

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
