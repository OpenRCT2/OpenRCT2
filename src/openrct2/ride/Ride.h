/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _RIDE_H_
#define _RIDE_H_

#include "../common.h"
#include "../rct12/RCT12.h"
#include "../rct2/RCT2.h"
#include "../world/Map.h"
#include "RideRatings.h"
#include "RideTypes.h"
#include "Vehicle.h"

interface IObjectManager;
class StationObject;
struct rct_peep;

#define MAX_RIDE_TYPES_PER_RIDE_ENTRY 3
// The max number of different types of vehicle.
// Examples of vehicles here are the locomotive, tender and carriage of the Miniature Railway.
#define MAX_VEHICLES_PER_RIDE_ENTRY 4
#define MAX_VEHICLES_PER_RIDE 32
#define RIDE_ENTRY_INDEX_NULL 255
#define NUM_COLOUR_SCHEMES 4
#define MAX_CATEGORIES_PER_RIDE 2
#define DOWNTIME_HISTORY_SIZE 8
#define CUSTOMER_HISTORY_SIZE 10
#define MAX_CARS_PER_TRAIN 255
#define MAX_STATIONS 4
#define RIDE_MEASUREMENT_MAX_ITEMS 4800
#define MAX_RIDES 255
#define RIDE_ID_NULL 255
#define RIDE_ADJACENCY_CHECK_DISTANCE 5

#pragma pack(push, 1)

/**
 * Couples a ride type and subtype together.
 */
struct ride_list_item
{
    union
    {
        struct
        {
            uint8_t type;
            uint8_t entry_index;
        };
        uint16_t ride_type_and_entry;
    };
};
assert_struct_size(ride_list_item, 2);

struct TrackColour
{
    uint8_t main;
    uint8_t additional;
    uint8_t supports;
};
assert_struct_size(TrackColour, 3);

struct vehicle_colour
{
    uint8_t main;
    uint8_t additional_1;
    uint8_t additional_2;
};
assert_struct_size(vehicle_colour, 3);

struct track_colour_preset_list
{
    uint8_t count;
    TrackColour list[256];
};
assert_struct_size(track_colour_preset_list, (1 + 256 * 3));

struct vehicle_colour_preset_list
{
    uint8_t count;
    vehicle_colour list[256];
};
assert_struct_size(vehicle_colour_preset_list, (1 + 256 * 3));

struct rct_ride_name
{
    rct_string_id name;
    rct_string_id description;
};
assert_struct_size(rct_ride_name, 4);

#pragma pack(pop)

/**
 * Ride type structure.
 * size: unknown
 */
struct rct_ride_entry
{
    rct_ride_name naming;
    uint32_t images_offset; // 0x004. The first three images are previews. They correspond to the ride_type[] array.
    uint32_t flags;         // 0x008
    uint8_t ride_type[RCT2_MAX_RIDE_TYPES_PER_RIDE_ENTRY]; // 0x00C
    uint8_t min_cars_in_train;                             // 0x00F
    uint8_t max_cars_in_train;                             // 0x010
    uint8_t cars_per_flat_ride;                            // 0x011
    // Number of cars that can't hold passengers
    uint8_t zero_cars; // 0x012
    // The index to the vehicle type displayed in
    // the vehicle tab.
    uint8_t tab_vehicle;     // 0x013
    uint8_t default_vehicle; // 0x014
    // Convert from first - fourth vehicle to
    // vehicle structure
    uint8_t front_vehicle;                                             // 0x015
    uint8_t second_vehicle;                                            // 0x016
    uint8_t rear_vehicle;                                              // 0x017
    uint8_t third_vehicle;                                             // 0x018
    uint8_t pad_019;                                                   // 0x019
    rct_ride_entry_vehicle vehicles[RCT2_MAX_VEHICLES_PER_RIDE_ENTRY]; // 0x01A
    vehicle_colour_preset_list* vehicle_preset_list;                   // 0x1AE
    int8_t excitement_multiplier;                                      // 0x1B2
    int8_t intensity_multiplier;                                       // 0x1B3
    int8_t nausea_multiplier;                                          // 0x1B4
    uint8_t max_height;                                                // 0x1B5
    uint64_t enabledTrackPieces;                                       // 0x1B6
    uint8_t category[RCT2_MAX_CATEGORIES_PER_RIDE];                    // 0x1BE
    uint8_t shop_item;                                                 // 0x1C0
    uint8_t shop_item_secondary;                                       // 0x1C1
    rct_string_id capacity;
    void* obj;
};

struct RideStation
{
    LocationXY8 Start;
    uint8_t Height;
    uint8_t Length;
    uint8_t Depart;
    uint8_t TrainAtStation;
    TileCoordsXYZD Entrance;
    TileCoordsXYZD Exit;
    int32_t SegmentLength; // Length of track between this station and the next.
    uint16_t SegmentTime;  // Time for train to reach the next station from this station.
    uint8_t QueueTime;
    uint16_t QueueLength;
    uint16_t LastPeepInQueue;

    static constexpr uint8_t NO_TRAIN = std::numeric_limits<uint8_t>::max();
};

/**
 * Ride structure.
 *
 * This is based on RCT2's ride structure.
 * Padding and no longer used fields have been removed.
 */
struct Ride
{
    ride_id_t id;
    uint8_t type;
    // pointer to static info. for example, wild mouse type is 0x36, subtype is
    // 0x4c.
    uint8_t subtype;
    uint8_t mode;
    uint8_t colour_scheme_type;
    VehicleColour vehicle_colours[MAX_CARS_PER_TRAIN];
    // 0 = closed, 1 = open, 2 = test
    uint8_t status;
    rct_string_id name;
    union
    {
        uint32_t name_arguments;
        struct
        {
            rct_string_id name_arguments_type_name;
            uint16_t name_arguments_number;
        };
    };
    LocationXY8 overall_view;
    uint16_t vehicles[MAX_VEHICLES_PER_RIDE]; // Points to the first car in the train
    uint8_t depart_flags;
    uint8_t num_stations;
    uint8_t num_vehicles;
    uint8_t num_cars_per_train;
    uint8_t proposed_num_vehicles;
    uint8_t proposed_num_cars_per_train;
    uint8_t max_trains;
    uint8_t min_max_cars_per_train;
    uint8_t min_waiting_time;
    uint8_t max_waiting_time;
    union
    {
        uint8_t operation_option;
        uint8_t time_limit;
        uint8_t num_laps;
        uint8_t launch_speed;
        uint8_t speed;
        uint8_t rotations;
    };

    uint8_t boat_hire_return_direction;
    LocationXY8 boat_hire_return_position;
    uint8_t measurement_index;
    // bits 0 through 4 are the number of helix sections
    // bit 5: spinning tunnel, water splash, or rapids
    // bit 6: log reverser, waterfall
    // bit 7: whirlpool
    uint8_t special_track_elements;
    // Divide this value by 29127 to get the human-readable max speed
    // (in RCT2, display_speed = (max_speed * 9) >> 18)
    int32_t max_speed;
    int32_t average_speed;
    uint8_t current_test_segment;
    uint8_t average_speed_test_timeout;
    fixed16_2dp max_positive_vertical_g;
    fixed16_2dp max_negative_vertical_g;
    fixed16_2dp max_lateral_g;
    fixed16_2dp previous_vertical_g;
    fixed16_2dp previous_lateral_g;
    uint32_t testing_flags;
    // x y map location of the current track piece during a test
    // this is to prevent counting special tracks multiple times
    LocationXY8 cur_test_track_location;
    // Next 3 variables are related (XXXX XYYY ZZZa aaaa)
    uint16_t turn_count_default; // X = current turn count
    uint16_t turn_count_banked;
    uint16_t turn_count_sloped; // X = number turns > 3 elements
    union
    {
        uint8_t inversions; // (???X XXXX)
        uint8_t holes;      // (???X XXXX)
        // This is a very rough approximation of how much of the ride is undercover.
        // It reaches the maximum value of 7 at about 50% undercover and doesn't increase beyond that.
        uint8_t sheltered_eighths; // (XXX?-????)
    };
    // Y is number of powered lifts, X is drops
    uint8_t drops; // (YYXX XXXX)
    uint8_t start_drop_height;
    uint8_t highest_drop_height;
    int32_t sheltered_length;
    // Unused always 0? Should affect nausea
    uint16_t var_11C;
    uint8_t num_sheltered_sections; // (?abY YYYY)
    // see cur_test_track_location
    uint8_t cur_test_track_z;
    // Customer counter in the current 960 game tick (about 30 seconds) interval
    uint16_t cur_num_customers;
    // Counts ticks to update customer intervals, resets each 960 game ticks.
    uint16_t num_customers_timeout;
    // Customer count in the last 10 * 960 game ticks (sliding window)
    uint16_t num_customers[CUSTOMER_HISTORY_SIZE];
    money16 price;
    LocationXY8 chairlift_bullwheel_location[2];
    uint8_t chairlift_bullwheel_z[2];
    union
    {
        rating_tuple ratings;
        struct
        {
            ride_rating excitement;
            ride_rating intensity;
            ride_rating nausea;
        };
    };
    uint16_t value;
    uint16_t chairlift_bullwheel_rotation;
    uint8_t satisfaction;
    uint8_t satisfaction_time_out;
    uint8_t satisfaction_next;
    // Various flags stating whether a window needs to be refreshed
    uint8_t window_invalidate_flags;
    uint32_t total_customers;
    money32 total_profit;
    uint8_t popularity;
    uint8_t popularity_time_out; // Updated every purchase and ?possibly by time?
    uint8_t popularity_next;     // When timeout reached this will be the next popularity
    uint16_t num_riders;
    uint8_t music_tune_id;
    uint8_t slide_in_use;
    union
    {
        uint16_t slide_peep;
        uint16_t maze_tiles;
    };
    uint8_t slide_peep_t_shirt_colour;
    uint8_t spiral_slide_progress;
    int16_t build_date;
    money16 upkeep_cost;
    uint16_t race_winner;
    uint32_t music_position;
    uint8_t breakdown_reason_pending;
    uint8_t mechanic_status;
    uint16_t mechanic;
    uint8_t inspection_station;
    uint8_t broken_vehicle;
    uint8_t broken_car;
    uint8_t breakdown_reason;
    money16 price_secondary;
    union
    {
        struct
        {
            uint8_t reliability_subvalue;   // 0 - 255, acts like the decimals for reliability_percentage
            uint8_t reliability_percentage; // Starts at 100 and decreases from there.
        };
        uint16_t reliability;
    };
    // Small constant used to increase the unreliability as the game continues,
    // making breakdowns more and more likely.
    uint8_t unreliability_factor;
    // Range from [0, 100]
    uint8_t downtime;
    uint8_t inspection_interval;
    uint8_t last_inspection;
    uint8_t downtime_history[DOWNTIME_HISTORY_SIZE];
    uint32_t no_primary_items_sold;
    uint32_t no_secondary_items_sold;
    uint8_t breakdown_sound_modifier;
    // Used to oscillate the sound when ride breaks down.
    // 0 = no change, 255 = max change
    uint8_t not_fixed_timeout;
    uint8_t last_crash_type;
    uint8_t connected_message_throttle;
    money32 income_per_hour;
    money32 profit;
    TrackColour track_colour[NUM_COLOUR_SCHEMES];
    uint8_t music;
    uint8_t entrance_style;
    uint16_t vehicle_change_timeout;
    uint8_t num_block_brakes;
    uint8_t lift_hill_speed;
    uint16_t guests_favourite;
    uint32_t lifecycle_flags;
    uint16_t total_air_time;
    uint8_t current_test_station;
    uint8_t num_circuits;
    int16_t cable_lift_x;
    int16_t cable_lift_y;
    uint8_t cable_lift_z;
    uint16_t cable_lift;
    // These fields are used to warn users about issues.
    // Such issue can be hacked rides with incompatible options set.
    // They don't require export/import.
    uint8_t current_issues;
    uint32_t last_issue_time;
    RideStation stations[MAX_STATIONS];

    bool CanBreakDown() const;
};

#pragma pack(push, 1)

/**
 * Ride measurement structure.
 * size: 0x04B0C
 */
struct rct_ride_measurement
{
    ride_id_t ride_index;                         // 0x0000
    uint8_t flags;                                // 0x0001
    uint32_t last_use_tick;                       // 0x0002
    uint16_t num_items;                           // 0x0006
    uint16_t current_item;                        // 0x0008
    uint8_t vehicle_index;                        // 0x000A
    uint8_t current_station;                      // 0x000B
    int8_t vertical[RIDE_MEASUREMENT_MAX_ITEMS];  // 0x000C
    int8_t lateral[RIDE_MEASUREMENT_MAX_ITEMS];   // 0x12CC
    uint8_t velocity[RIDE_MEASUREMENT_MAX_ITEMS]; // 0x258C
    uint8_t altitude[RIDE_MEASUREMENT_MAX_ITEMS]; // 0x384C
};
assert_struct_size(rct_ride_measurement, 0x4b0c);

struct track_begin_end
{
    int32_t begin_x;
    int32_t begin_y;
    int32_t begin_z;
    int32_t begin_direction;
    TileElement* begin_element;
    int32_t end_x;
    int32_t end_y;
    int32_t end_direction;
    TileElement* end_element;
};
#ifdef PLATFORM_32BIT
assert_struct_size(track_begin_end, 36);
#endif

struct ride_name_args
{
    uint16_t type_name;
    uint16_t number;
};
assert_struct_size(ride_name_args, 4);

#pragma pack(pop)

/*
 * This array should probably be only 91 + 128 * 3 = 475 bytes long.
 * It was originally stored at address 0x009E32F8 and continued until 0x009E34E3
 * (inclusive). 0x009E34E4 is the address of s6 header, so it's likely it had
 * some padding at the end as well.
 */
#define TYPE_TO_RIDE_ENTRY_SLOTS 492
extern uint8_t gTypeToRideEntryIndexMap[TYPE_TO_RIDE_ENTRY_SLOTS];

enum
{
    RIDE_CLASS_RIDE,
    RIDE_CLASS_SHOP_OR_STALL,
    RIDE_CLASS_KIOSK_OR_FACILITY
};

// Constants used by the lifecycle_flags property at 0x1D0
enum
{
    RIDE_LIFECYCLE_ON_TRACK = 1 << 0,
    RIDE_LIFECYCLE_TESTED = 1 << 1,
    RIDE_LIFECYCLE_TEST_IN_PROGRESS = 1 << 2,
    RIDE_LIFECYCLE_NO_RAW_STATS = 1 << 3,
    RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING = 1 << 4,
    RIDE_LIFECYCLE_ON_RIDE_PHOTO = 1 << 5,
    RIDE_LIFECYCLE_BREAKDOWN_PENDING = 1 << 6,
    RIDE_LIFECYCLE_BROKEN_DOWN = 1 << 7,
    RIDE_LIFECYCLE_DUE_INSPECTION = 1 << 8,
    RIDE_LIFECYCLE_QUEUE_FULL = 1 << 9,
    RIDE_LIFECYCLE_CRASHED = 1 << 10,
    RIDE_LIFECYCLE_HAS_STALLED_VEHICLE = 1 << 11,
    RIDE_LIFECYCLE_EVER_BEEN_OPENED = 1 << 12,
    RIDE_LIFECYCLE_MUSIC = 1 << 13,
    RIDE_LIFECYCLE_INDESTRUCTIBLE = 1 << 14,
    RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK = 1 << 15,
    RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED = 1 << 16,
    RIDE_LIFECYCLE_CABLE_LIFT = 1 << 17,
    RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN = 1 << 18,   // Used for the Award for Best Custom-designed Rides
    RIDE_LIFECYCLE_SIX_FLAGS_DEPRECATED = 1 << 19 // Not used anymore
};

// Constants used by the ride_type->flags property at 0x008
enum
{
    RIDE_ENTRY_FLAG_VEHICLE_TAB_SCALE_HALF = 1 << 0,
    RIDE_ENTRY_FLAG_NO_INVERSIONS = 1 << 1,
    RIDE_ENTRY_FLAG_NO_BANKED_TRACK = 1 << 2,
    RIDE_ENTRY_FLAG_PLAY_DEPART_SOUND = 1 << 3,
    RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_1 = 1 << 4,
    // Twist type rotation ride
    RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_1 = 1 << 5,
    // Lifting arm rotation ride (enterprise)
    RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_2 = 1 << 6,
    RIDE_ENTRY_FLAG_DISABLE_WANDERING_DEPRECATED = 1 << 7,
    RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND = 1 << 8,
    RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND_SLIDE = 1 << 9,
    RIDE_ENTRY_FLAG_COVERED_RIDE = 1 << 10,
    RIDE_ENTRY_FLAG_LIMIT_AIRTIME_BONUS = 1 << 11,
    RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME_DEPRECATED = 1 << 12, // Always set with SEPARATE_RIDE, and deprecated in favour of it.
    RIDE_ENTRY_FLAG_SEPARATE_RIDE_DEPRECATED = 1 << 13,      // Made redundant by ride groups
    RIDE_ENTRY_FLAG_CANNOT_BREAK_DOWN = 1 << 14,
    RIDE_ENTRY_DISABLE_LAST_OPERATING_MODE = 1 << 15,
    RIDE_ENTRY_FLAG_DISABLE_DOOR_CONSTRUCTION_DEPRECATED = 1 << 16,
    RIDE_ENTRY_DISABLE_FIRST_TWO_OPERATING_MODES = 1 << 17,
    RIDE_ENTRY_FLAG_DISABLE_COLLISION_CRASHES = 1 << 18,
    RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB = 1 << 19,
    // Must be set with swing mode 1 as well.
    RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_2 = 1 << 20,
};

enum
{
    RIDE_TESTING_SHELTERED = (1 << 0),
    RIDE_TESTING_TURN_LEFT = (1 << 1),
    RIDE_TESTING_TURN_RIGHT = (1 << 2),
    RIDE_TESTING_TURN_BANKED = (1 << 3),
    RIDE_TESTING_TURN_SLOPED = (1 << 4),
    RIDE_TESTING_DROP_DOWN = (1 << 5),
    RIDE_TESTING_POWERED_LIFT = (1 << 6),
    RIDE_TESTING_DROP_UP = (1 << 7),
};

enum
{
    RIDE_TYPE_NULL = 255,
    RIDE_TYPE_SPIRAL_ROLLER_COASTER = 0,
    RIDE_TYPE_STAND_UP_ROLLER_COASTER,
    RIDE_TYPE_SUSPENDED_SWINGING_COASTER,
    RIDE_TYPE_INVERTED_ROLLER_COASTER,
    RIDE_TYPE_JUNIOR_ROLLER_COASTER,
    RIDE_TYPE_MINIATURE_RAILWAY,
    RIDE_TYPE_MONORAIL,
    RIDE_TYPE_MINI_SUSPENDED_COASTER,
    RIDE_TYPE_BOAT_HIRE,
    RIDE_TYPE_WOODEN_WILD_MOUSE,
    RIDE_TYPE_STEEPLECHASE = 10,
    RIDE_TYPE_CAR_RIDE,
    RIDE_TYPE_LAUNCHED_FREEFALL,
    RIDE_TYPE_BOBSLEIGH_COASTER,
    RIDE_TYPE_OBSERVATION_TOWER,
    RIDE_TYPE_LOOPING_ROLLER_COASTER,
    RIDE_TYPE_DINGHY_SLIDE,
    RIDE_TYPE_MINE_TRAIN_COASTER,
    RIDE_TYPE_CHAIRLIFT,
    RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
    RIDE_TYPE_MAZE = 20,
    RIDE_TYPE_SPIRAL_SLIDE,
    RIDE_TYPE_GO_KARTS,
    RIDE_TYPE_LOG_FLUME,
    RIDE_TYPE_RIVER_RAPIDS,
    RIDE_TYPE_DODGEMS,
    RIDE_TYPE_SWINGING_SHIP,
    RIDE_TYPE_SWINGING_INVERTER_SHIP,
    RIDE_TYPE_FOOD_STALL,
    RIDE_TYPE_1D,
    RIDE_TYPE_DRINK_STALL = 30,
    RIDE_TYPE_1F,
    RIDE_TYPE_SHOP,
    RIDE_TYPE_MERRY_GO_ROUND,
    RIDE_TYPE_22,
    RIDE_TYPE_INFORMATION_KIOSK,
    RIDE_TYPE_TOILETS,
    RIDE_TYPE_FERRIS_WHEEL,
    RIDE_TYPE_MOTION_SIMULATOR,
    RIDE_TYPE_3D_CINEMA,
    RIDE_TYPE_TOP_SPIN = 40,
    RIDE_TYPE_SPACE_RINGS,
    RIDE_TYPE_REVERSE_FREEFALL_COASTER,
    RIDE_TYPE_LIFT,
    RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER,
    RIDE_TYPE_CASH_MACHINE,
    RIDE_TYPE_TWIST,
    RIDE_TYPE_HAUNTED_HOUSE,
    RIDE_TYPE_FIRST_AID,
    RIDE_TYPE_CIRCUS,
    RIDE_TYPE_GHOST_TRAIN = 50,
    RIDE_TYPE_TWISTER_ROLLER_COASTER,
    RIDE_TYPE_WOODEN_ROLLER_COASTER,
    RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER,
    RIDE_TYPE_STEEL_WILD_MOUSE,
    RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER,
    RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT,
    RIDE_TYPE_FLYING_ROLLER_COASTER,
    RIDE_TYPE_FLYING_ROLLER_COASTER_ALT,
    RIDE_TYPE_VIRGINIA_REEL,
    RIDE_TYPE_SPLASH_BOATS = 60,
    RIDE_TYPE_MINI_HELICOPTERS,
    RIDE_TYPE_LAY_DOWN_ROLLER_COASTER,
    RIDE_TYPE_SUSPENDED_MONORAIL,
    RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT,
    RIDE_TYPE_REVERSER_ROLLER_COASTER,
    RIDE_TYPE_HEARTLINE_TWISTER_COASTER,
    RIDE_TYPE_MINI_GOLF,
    RIDE_TYPE_GIGA_COASTER,
    RIDE_TYPE_ROTO_DROP,
    RIDE_TYPE_FLYING_SAUCERS = 70,
    RIDE_TYPE_CROOKED_HOUSE,
    RIDE_TYPE_MONORAIL_CYCLES,
    RIDE_TYPE_COMPACT_INVERTED_COASTER,
    RIDE_TYPE_WATER_COASTER,
    RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER,
    RIDE_TYPE_INVERTED_HAIRPIN_COASTER,
    RIDE_TYPE_MAGIC_CARPET,
    RIDE_TYPE_SUBMARINE_RIDE,
    RIDE_TYPE_RIVER_RAFTS,
    RIDE_TYPE_50 = 80,
    RIDE_TYPE_ENTERPRISE,
    RIDE_TYPE_52,
    RIDE_TYPE_53,
    RIDE_TYPE_54,
    RIDE_TYPE_55,
    RIDE_TYPE_INVERTED_IMPULSE_COASTER,
    RIDE_TYPE_MINI_ROLLER_COASTER,
    RIDE_TYPE_MINE_RIDE,
    RIDE_TYPE_59,
    RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER = 90,

    RIDE_TYPE_COUNT
};

enum
{
    RIDE_STATUS_CLOSED,
    RIDE_STATUS_OPEN,
    RIDE_STATUS_TESTING
};

enum
{
    RIDE_MODE_NORMAL,
    RIDE_MODE_CONTINUOUS_CIRCUIT,
    RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE,
    RIDE_MODE_POWERED_LAUNCH_PASSTROUGH, // RCT2 style, pass through station
    RIDE_MODE_SHUTTLE,
    RIDE_MODE_BOAT_HIRE,
    RIDE_MODE_UPWARD_LAUNCH,
    RIDE_MODE_ROTATING_LIFT,
    RIDE_MODE_STATION_TO_STATION,
    RIDE_MODE_SINGLE_RIDE_PER_ADMISSION,
    RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION = 10,
    RIDE_MODE_MAZE,
    RIDE_MODE_RACE,
    RIDE_MODE_BUMPERCAR,
    RIDE_MODE_SWING,
    RIDE_MODE_SHOP_STALL,
    RIDE_MODE_ROTATION,
    RIDE_MODE_FORWARD_ROTATION,
    RIDE_MODE_BACKWARD_ROTATION,
    RIDE_MODE_FILM_AVENGING_AVIATORS,
    RIDE_MODE_3D_FILM_MOUSE_TAILS = 20,
    RIDE_MODE_SPACE_RINGS,
    RIDE_MODE_BEGINNERS,
    RIDE_MODE_LIM_POWERED_LAUNCH,
    RIDE_MODE_FILM_THRILL_RIDERS,
    RIDE_MODE_3D_FILM_STORM_CHASERS,
    RIDE_MODE_3D_FILM_SPACE_RAIDERS,
    RIDE_MODE_INTENSE,
    RIDE_MODE_BERSERK,
    RIDE_MODE_HAUNTED_HOUSE,
    RIDE_MODE_CIRCUS_SHOW = 30,
    RIDE_MODE_DOWNWARD_LAUNCH,
    RIDE_MODE_CROOKED_HOUSE,
    RIDE_MODE_FREEFALL_DROP,
    RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED,
    RIDE_MODE_POWERED_LAUNCH, // RCT1 style, don't pass through station
    RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
};

enum
{
    RIDE_COLOUR_SCHEME_ALL_SAME,
    RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN,
    RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR
};

enum
{
    RIDE_CATEGORY_TRANSPORT,
    RIDE_CATEGORY_GENTLE,
    RIDE_CATEGORY_ROLLERCOASTER,
    RIDE_CATEGORY_THRILL,
    RIDE_CATEGORY_WATER,
    RIDE_CATEGORY_SHOP
};

enum
{
    MUSIC_STYLE_DODGEMS_BEAT,
    MUSIC_STYLE_FAIRGROUND_ORGAN,
    MUSIC_STYLE_ROMAN_FANFARE,
    MUSIC_STYLE_ORIENTAL,
    MUSIC_STYLE_MARTIAN,
    MUSIC_STYLE_JUNGLE_DRUMS,
    MUSIC_STYLE_EGYPTIAN,
    MUSIC_STYLE_TOYLAND,
    MUSIC_STYLE_CIRCUS_SHOW,
    MUSIC_STYLE_SPACE,
    MUSIC_STYLE_HORROR,
    MUSIC_STYLE_TECHNO,
    MUSIC_STYLE_GENTLE,
    MUSIC_STYLE_SUMMER,
    MUSIC_STYLE_WATER,
    MUSIC_STYLE_WILD_WEST,
    MUSIC_STYLE_JURASSIC,
    MUSIC_STYLE_ROCK,
    MUSIC_STYLE_RAGTIME,
    MUSIC_STYLE_FANTASY,
    MUSIC_STYLE_ROCK_STYLE_2,
    MUSIC_STYLE_ICE,
    MUSIC_STYLE_SNOW,
    MUSIC_STYLE_CUSTOM_MUSIC_1,
    MUSIC_STYLE_CUSTOM_MUSIC_2,
    MUSIC_STYLE_MEDIEVAL,
    MUSIC_STYLE_URBAN,
    MUSIC_STYLE_ORGAN,
    MUSIC_STYLE_MECHANICAL,
    MUSIC_STYLE_MODERN,
    MUSIC_STYLE_PIRATES,
    MUSIC_STYLE_ROCK_STYLE_3,
    MUSIC_STYLE_CANDY_STYLE,
    MUSIC_STYLE_COUNT
};

enum
{
    BREAKDOWN_NONE = 255,
    BREAKDOWN_SAFETY_CUT_OUT = 0,
    BREAKDOWN_RESTRAINTS_STUCK_CLOSED,
    BREAKDOWN_RESTRAINTS_STUCK_OPEN,
    BREAKDOWN_DOORS_STUCK_CLOSED,
    BREAKDOWN_DOORS_STUCK_OPEN,
    BREAKDOWN_VEHICLE_MALFUNCTION,
    BREAKDOWN_BRAKES_FAILURE,
    BREAKDOWN_CONTROL_FAILURE,

    BREAKDOWN_COUNT
};

enum
{
    RIDE_MECHANIC_STATUS_UNDEFINED,
    RIDE_MECHANIC_STATUS_CALLING,
    RIDE_MECHANIC_STATUS_HEADING,
    RIDE_MECHANIC_STATUS_FIXING,
    RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES
};

enum
{
    RIDE_DEPART_WAIT_FOR_LOAD_MASK = 7,
    RIDE_DEPART_WAIT_FOR_LOAD = 1 << 3,
    RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES = 1 << 4,
    RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS = 1 << 5,
    RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH = 1 << 6,
    RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH = 1 << 7
};

enum
{
    WAIT_FOR_LOAD_QUARTER,
    WAIT_FOR_LOAD_HALF,
    WAIT_FOR_LOAD_THREE_QUARTER,
    WAIT_FOR_LOAD_FULL,
    WAIT_FOR_LOAD_ANY,
};

enum
{
    RIDE_COLOUR_SCHEME_MAIN,
    RIDE_COLOUR_SCHEME_ADDITIONAL_1,
    RIDE_COLOUR_SCHEME_ADDITIONAL_2,
    RIDE_COLOUR_SCHEME_ADDITIONAL_3
};

enum
{
    VEHICLE_COLOUR_SCHEME_SAME,
    VEHICLE_COLOUR_SCHEME_PER_TRAIN,
    VEHICLE_COLOUR_SCHEME_PER_VEHICLE
};

enum
{
    RIDE_INSPECTION_EVERY_10_MINUTES,
    RIDE_INSPECTION_EVERY_20_MINUTES,
    RIDE_INSPECTION_EVERY_30_MINUTES,
    RIDE_INSPECTION_EVERY_45_MINUTES,
    RIDE_INSPECTION_EVERY_HOUR,
    RIDE_INSPECTION_EVERY_2_HOURS,
    RIDE_INSPECTION_NEVER
};

// Flags used by ride->window_invalidate_flags
enum
{
    RIDE_INVALIDATE_RIDE_CUSTOMER = 1,
    RIDE_INVALIDATE_RIDE_INCOME = 1 << 1,
    RIDE_INVALIDATE_RIDE_MAIN = 1 << 2,
    RIDE_INVALIDATE_RIDE_LIST = 1 << 3,
    RIDE_INVALIDATE_RIDE_OPERATING = 1 << 4,
    RIDE_INVALIDATE_RIDE_MAINTENANCE = 1 << 5,
};

enum
{
    RIDE_MEASUREMENT_FLAG_RUNNING = 1 << 0,
    RIDE_MEASUREMENT_FLAG_UNLOADING = 1 << 1,
    RIDE_MEASUREMENT_FLAG_G_FORCES = 1 << 2
};

// Constants for ride->special_track_elements
enum
{
    RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS = 1 << 5,
    RIDE_ELEMENT_REVERSER_OR_WATERFALL = 1 << 6,
    RIDE_ELEMENT_WHIRLPOOL = 1 << 7
};

enum ride_type_flags : uint32_t
{
    RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN = 1 << 0,
    RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL = 1 << 1,
    RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS = 1 << 2,
    RIDE_TYPE_FLAG_3 = 1 << 3,
    RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION = 1 << 4,
    RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS = 1 << 5,
    RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER = 1 << 6, // used only by boat Hire and submarine ride
    RIDE_TYPE_FLAG_HAS_G_FORCES = 1 << 7,
    RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS = 1
        << 8, // used by rides that can't have gaps, like those with a vertical tower, such as the observation tower
    RIDE_TYPE_FLAG_HAS_DATA_LOGGING = 1 << 9,
    RIDE_TYPE_FLAG_HAS_DROPS = 1 << 10,
    RIDE_TYPE_FLAG_NO_TEST_MODE = 1 << 11,
    RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES = 1
        << 12, // used by rides with two varieties, like the u and o shapes of the dinghy slide and the dry and submerged track
               // of the water coaster
    RIDE_TYPE_FLAG_NO_VEHICLES = 1 << 13, // used only by maze, spiral slide and shops
    RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS = 1 << 14,
    RIDE_TYPE_FLAG_HAS_NO_TRACK = 1 << 15,
    RIDE_TYPE_FLAG_16 = 1 << 16, // something to do with vehicle colour scheme
    RIDE_TYPE_FLAG_IS_SHOP = 1 << 17,
    RIDE_TYPE_FLAG_TRACK_NO_WALLS = 1 << 18, // if set, wall scenery can not share a tile with the ride's track
    RIDE_TYPE_FLAG_FLAT_RIDE = 1 << 19,
    RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN = 1 << 20, // whether or not guests will go on the ride again if they liked it (this is
                                                   // usually applied to everything apart from transport rides)
    RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY = 1 << 21, // used by toilets and first aid to mark that peep should go inside
                                                             // the building (rather than 'buying' at the counter)
    RIDE_TYPE_FLAG_IN_RIDE = 1 << 22,                        // peeps are "IN" (ride) rather than "ON" (ride)
    RIDE_TYPE_FLAG_SELLS_FOOD = 1 << 23,
    RIDE_TYPE_FLAG_SELLS_DRINKS = 1 << 24,
    RIDE_TYPE_FLAG_IS_BATHROOM = 1 << 25,
    RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS = 1 << 26, // whether or not vehicle colours can be set
    RIDE_TYPE_FLAG_CHECK_FOR_STALLING = 1 << 27,
    RIDE_TYPE_FLAG_HAS_TRACK = 1 << 28,
    RIDE_TYPE_FLAG_29 = 1 << 29,               // used only by lift
    RIDE_TYPE_FLAG_HAS_LARGE_CURVES = 1 << 30, // whether the ride supports large (45 degree turn) curves
    RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR = 1u << 31,
};
// Hack for MSVC which thinks RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR = 1u << 31 is signed and generates narrowing
// conversion warnings
constexpr inline uint32_t operator|(ride_type_flags a, ride_type_flags b)
{
    return static_cast<ride_type_flags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

enum
{
    RIDE_CRASH_TYPE_NONE = 0,
    RIDE_CRASH_TYPE_NO_FATALITIES = 2,
    RIDE_CRASH_TYPE_FATALITIES = 8
};

enum
{
    RIDE_CONSTRUCTION_STATE_0,
    RIDE_CONSTRUCTION_STATE_FRONT,
    RIDE_CONSTRUCTION_STATE_BACK,
    RIDE_CONSTRUCTION_STATE_SELECTED,
    RIDE_CONSTRUCTION_STATE_PLACE,
    RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT,
    RIDE_CONSTRUCTION_STATE_MAZE_BUILD,
    RIDE_CONSTRUCTION_STATE_MAZE_MOVE,
    RIDE_CONSTRUCTION_STATE_MAZE_FILL
};

enum
{
    RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_TRAINS,
    RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_CARS_PER_TRAIN,
    RIDE_SET_VEHICLES_COMMAND_TYPE_RIDE_ENTRY
};

enum
{
    RIDE_SETTING_MODE,
    RIDE_SETTING_DEPARTURE,
    RIDE_SETTING_MIN_WAITING_TIME,
    RIDE_SETTING_MAX_WAITING_TIME,
    RIDE_SETTING_OPERATION_OPTION,
    RIDE_SETTING_INSPECTION_INTERVAL,
    RIDE_SETTING_MUSIC,
    RIDE_SETTING_MUSIC_TYPE,
    RIDE_SETTING_LIFT_HILL_SPEED,
    RIDE_SETTING_NUM_CIRCUITS,
    RIDE_SETTING_RIDE_TYPE,
};

enum
{
    MAZE_WALL_TYPE_BRICK,
    MAZE_WALL_TYPE_HEDGE,
    MAZE_WALL_TYPE_ICE,
    MAZE_WALL_TYPE_WOOD,
};

enum
{
    TRACK_SELECTION_FLAG_ARROW = 1 << 0,
    TRACK_SELECTION_FLAG_TRACK = 1 << 1,
    TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT = 1 << 2,
    TRACK_SELECTION_FLAG_RECHECK = 1 << 3,
    TRACK_SELECTION_FLAG_TRACK_PLACE_ACTION_QUEUED = 1 << 4,
};

enum
{
    RIDE_MODIFY_DEMOLISH,
    RIDE_MODIFY_RENEW
};

enum
{
    RIDE_ISSUE_NONE = 0,
    RIDE_ISSUE_GUESTS_STUCK = (1 << 0),
};

struct rct_ride_properties
{
    uint32_t flags;
    uint8_t min_value;
    uint8_t max_value;
    uint8_t max_brakes_speed;
    uint8_t powered_lift_acceleration;
    uint8_t booster_acceleration;
    int8_t booster_speed_factor; // The factor to shift the raw booster speed with
};

#define RIDE_MODE_COUNT 37

#define MAX_RIDE_MEASUREMENTS 8
#define RIDE_VALUE_UNDEFINED 0xFFFF
#define RIDE_INITIAL_RELIABILITY ((100 << 8) | 0xFF) // Upper byte is percentage, lower byte is "decimal".

#define STATION_DEPART_FLAG (1 << 7)
#define STATION_DEPART_MASK (~STATION_DEPART_FLAG)

#define CURRENT_TURN_COUNT_MASK 0xF800
#define TURN_MASK_1_ELEMENT 0x001F
#define TURN_MASK_2_ELEMENTS 0x00E0
#define TURN_MASK_3_ELEMENTS 0x0700
#define TURN_MASK_4_PLUS_ELEMENTS 0xF800

#define CONSTRUCTION_LIFT_HILL_SELECTED 1

extern const rct_ride_properties RideProperties[RIDE_TYPE_COUNT];

/** Helper macros until rides are stored in this module. */
Ride* get_ride(int32_t index);
rct_ride_entry* get_ride_entry(int32_t index);
void get_ride_entry_name(char* name, int32_t index);
rct_ride_measurement* get_ride_measurement(int32_t index);

/**
 * Helper macro loop for enumerating through all the non null rides.
 */
#define FOR_ALL_RIDES(i, ride)                                                                                                 \
    for (i = 0; i < MAX_RIDES; i++)                                                                                            \
        if ((ride = get_ride(i))->type != RIDE_TYPE_NULL)

extern money16 gTotalRideValueForMoney;

extern const uint8_t gRideClassifications[MAX_RIDES];

extern Ride gRideList[MAX_RIDES];

extern rct_ride_measurement gRideMeasurements[MAX_RIDE_MEASUREMENTS];
extern uint16_t gRideCount;

extern money32 _currentTrackPrice;

extern uint16_t _numCurrentPossibleRideConfigurations;
extern uint16_t _numCurrentPossibleSpecialTrackPieces;

extern uint16_t _currentTrackCurve;
extern uint8_t _rideConstructionState;
extern ride_id_t _currentRideIndex;

extern CoordsXYZ _currentTrackBegin;

extern uint8_t _currentTrackPieceDirection;
extern uint8_t _currentTrackPieceType;
extern uint8_t _currentTrackSelectionFlags;
extern int8_t _rideConstructionArrowPulseTime;
extern uint8_t _currentTrackSlopeEnd;
extern uint8_t _currentTrackBankEnd;
extern uint8_t _currentTrackLiftHill;
extern uint8_t _currentTrackAlternative;
extern uint8_t _selectedTrackType;

extern uint8_t _previousTrackBankEnd;
extern uint8_t _previousTrackSlopeEnd;

extern CoordsXYZ _previousTrackPiece;

extern uint8_t _currentBrakeSpeed2;
extern uint8_t _currentSeatRotationAngle;

extern LocationXYZ16 _unkF44188;

extern CoordsXYZD _unkF440C5;

extern uint8_t gRideEntranceExitPlaceType;
extern ride_id_t gRideEntranceExitPlaceRideIndex;
extern uint8_t gRideEntranceExitPlaceStationIndex;
extern uint8_t gRideEntranceExitPlacePreviousRideConstructionState;
extern uint8_t gRideEntranceExitPlaceDirection;

extern bool gGotoStartPlacementMode;

extern uint8_t gLastEntranceStyle;

ride_id_t ride_get_empty_slot();
int32_t ride_get_default_mode(Ride* ride);
int32_t ride_get_count();
int32_t ride_get_total_queue_length(Ride* ride);
int32_t ride_get_max_queue_time(Ride* ride);
rct_peep* ride_get_queue_head_guest(Ride* ride, int32_t stationIndex);
void ride_queue_insert_guest_at_front(Ride* ride, int32_t stationIndex, rct_peep* peep);
void ride_init_all();
void reset_all_ride_build_dates();
void ride_update_favourited_stat();
void ride_update_all();
void ride_check_all_reachable();
void ride_update_satisfaction(Ride* ride, uint8_t happiness);
void ride_update_popularity(Ride* ride, uint8_t pop_amount);
bool ride_try_get_origin_element(const Ride* ride, CoordsXYE* output);
int32_t ride_find_track_gap(const Ride* ride, CoordsXYE* input, CoordsXYE* output);
void ride_construct_new(ride_list_item listItem);
void ride_construct(Ride* ride);
int32_t ride_modify(CoordsXYE* input);
void ride_remove_peeps(Ride* ride);
void ride_clear_blocked_tiles(Ride* ride);
void ride_get_status(const Ride* ride, rct_string_id* formatSecondary, int32_t* argument);
rct_peep* ride_get_assigned_mechanic(Ride* ride);
int32_t ride_get_total_length(Ride* ride);
int32_t ride_get_total_time(Ride* ride);
int32_t ride_can_have_multiple_circuits(Ride* ride);
TrackColour ride_get_track_colour(Ride* ride, int32_t colourScheme);
vehicle_colour ride_get_vehicle_colour(Ride* ride, int32_t vehicleIndex);
int32_t ride_get_unused_preset_vehicle_colour(uint8_t ride_sub_type);
void ride_set_vehicle_colours_to_random_preset(Ride* ride, uint8_t preset_index);
rct_ride_entry* get_ride_entry_by_ride(const Ride* ride);
uint8_t* get_ride_entry_indices_for_ride_type(uint8_t rideType);
void reset_type_to_ride_entry_index_map(IObjectManager& objectManager);
void ride_measurement_clear(Ride* ride);
void ride_measurements_update();
rct_ride_measurement* ride_get_measurement(Ride* ride, rct_string_id* message);
void ride_breakdown_add_news_item(Ride* ride);
rct_peep* ride_find_closest_mechanic(Ride* ride, int32_t forInspection);
int32_t ride_is_valid_for_open(Ride* ride, int32_t goingToBeOpen, int32_t isApplying);
int32_t ride_is_valid_for_test(Ride* ride, int32_t goingToBeOpen, int32_t isApplying);
int32_t ride_initialise_construction_window(Ride* ride);
void ride_construction_invalidate_current_track();
int32_t sub_6C683D(
    int32_t* x, int32_t* y, int32_t* z, int32_t direction, int32_t type, uint16_t extra_params, TileElement** output_element,
    uint16_t flags);
void ride_set_map_tooltip(TileElement* tileElement);
int32_t ride_music_params_update(
    int16_t x, int16_t y, int16_t z, Ride* ride, uint16_t sampleRate, uint32_t position, uint8_t* tuneId);
void ride_music_update_final();
void ride_prepare_breakdown(Ride* ride, int32_t breakdownReason);
TileElement* ride_get_station_start_track_element(Ride* ride, int32_t stationIndex);
TileElement* ride_get_station_exit_element(int32_t x, int32_t y, int32_t z);
void ride_set_status(Ride* ride, int32_t status);
void game_command_set_ride_status(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void ride_set_name(Ride* ride, const char* name, uint32_t flags);
void game_command_set_ride_name(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
int32_t ride_get_refund_price(const Ride* ride);
int32_t ride_get_random_colour_preset_index(uint8_t ride_type);
void ride_set_colour_preset(Ride* ride, uint8_t index);
money32 ride_get_common_price(Ride* forRide);
rct_ride_name get_ride_naming(const uint8_t rideType, rct_ride_entry* rideEntry);
void game_command_create_ride(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_callback_ride_construct_new(
    int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, int32_t edi, int32_t ebp);
void game_command_demolish_ride(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
money32 ride_create_command(int32_t type, int32_t subType, int32_t flags, ride_id_t* outRideIndex, uint8_t* outRideColour);
void ride_set_name_to_default(Ride* ride, rct_ride_entry* rideEntry);

void ride_set_name_to_track_default(Ride* ride, rct_ride_entry* rideEntry);
void ride_clear_for_construction(Ride* ride);
void ride_entrance_exit_place_provisional_ghost();
void ride_entrance_exit_remove_ghost();
void ride_restore_provisional_track_piece();
void ride_remove_provisional_track_piece();
void set_vehicle_type_image_max_sizes(rct_ride_entry_vehicle* vehicle_type, int32_t num_images);
void invalidate_test_results(Ride* ride);

void ride_select_next_section();
void ride_select_previous_section();

void increment_turn_count_1_element(Ride* ride, uint8_t type);
void increment_turn_count_2_elements(Ride* ride, uint8_t type);
void increment_turn_count_3_elements(Ride* ride, uint8_t type);
void increment_turn_count_4_plus_elements(Ride* ride, uint8_t type);
int32_t get_turn_count_1_element(Ride* ride, uint8_t type);
int32_t get_turn_count_2_elements(Ride* ride, uint8_t type);
int32_t get_turn_count_3_elements(Ride* ride, uint8_t type);
int32_t get_turn_count_4_plus_elements(Ride* ride, uint8_t type);

uint8_t ride_get_helix_sections(Ride* ride);
bool ride_has_spinning_tunnel(Ride* ride);
bool ride_has_water_splash(Ride* ride);
bool ride_has_rapids(Ride* ride);
bool ride_has_log_reverser(Ride* ride);
bool ride_has_waterfall(Ride* ride);
bool ride_has_whirlpool(Ride* ride);

bool ride_type_has_flag(int32_t rideType, uint32_t flag);
bool ride_is_powered_launched(Ride* ride);
bool ride_is_block_sectioned(Ride* ride);
bool ride_has_any_track_elements(const Ride* ride);
void ride_all_has_any_track_elements(bool* rideIndexArray);

void ride_construction_set_default_next_piece();

bool track_block_get_next(CoordsXYE* input, CoordsXYE* output, int32_t* z, int32_t* direction);
bool track_block_get_next_from_zero(
    int16_t x, int16_t y, int16_t z_start, Ride* ride, uint8_t direction_start, CoordsXYE* output, int32_t* z,
    int32_t* direction, bool isGhost);

bool track_block_get_previous(int32_t x, int32_t y, TileElement* tileElement, track_begin_end* outTrackBeginEnd);
bool track_block_get_previous_from_zero(
    int16_t x, int16_t y, int16_t z, Ride* ride, uint8_t direction, track_begin_end* outTrackBeginEnd);

void ride_get_start_of_track(CoordsXYE* output);

void window_ride_construction_update_active_elements();
void ride_construction_remove_ghosts();
money32 ride_entrance_exit_place_ghost(
    Ride* ride, int32_t x, int32_t y, int32_t direction, int32_t placeType, int32_t stationNum);
void ride_get_entrance_or_exit_position_from_screen_position(
    int32_t x, int32_t y, int32_t* outX, int32_t* outY, int32_t* outDirection);

bool ride_select_backwards_from_front();
bool ride_select_forwards_from_back();

bool ride_are_all_possible_entrances_and_exits_built(Ride* ride);
void ride_fix_breakdown(Ride* ride, int32_t reliabilityIncreaseFactor);

void ride_entry_get_train_layout(int32_t rideEntryIndex, int32_t numCarsPerTrain, uint8_t* trainLayout);
uint8_t ride_entry_get_vehicle_at_position(int32_t rideEntryIndex, int32_t numCarsPerTrain, int32_t position);
void ride_update_max_vehicles(Ride* ride);
void ride_update_vehicle_colours(Ride* ride);
uint64_t ride_entry_get_supported_track_pieces(const rct_ride_entry* rideEntry);

void ride_set_ride_entry(Ride* ride, int32_t rideEntry);
void ride_set_num_vehicles(Ride* ride, int32_t numVehicles);
void ride_set_num_cars_per_vehicle(Ride* ride, int32_t numCarsPerVehicle);

enum class RideSetSetting : uint8_t;
money32 set_operating_setting(ride_id_t rideId, RideSetSetting setting, uint8_t value);
money32 set_operating_setting_nested(ride_id_t rideId, RideSetSetting setting, uint8_t value, uint8_t flags);

void game_command_set_ride_vehicles(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);

void game_command_place_ride_entrance_or_exit(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_remove_ride_entrance_or_exit(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);

void ride_set_to_default_inspection_interval(Ride* ride);

void sub_6CB945(Ride* ride);
void ride_crash(Ride* ride, uint8_t vehicleIndex);

void sub_6C94D8();

void ride_reset_all_names();
const uint8_t* ride_seek_available_modes(Ride* ride);

void window_ride_construction_mouseup_demolish_next_piece(int32_t x, int32_t y, int32_t z, int32_t direction, int32_t type);

uint32_t ride_customers_per_hour(const Ride* ride);
uint32_t ride_customers_in_last_5_minutes(const Ride* ride);

rct_vehicle* ride_get_broken_vehicle(Ride* ride);

void window_ride_construction_do_station_check();
void window_ride_construction_do_entrance_exit_check();
void game_command_callback_place_ride_entrance_or_exit(
    int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, int32_t edi, int32_t ebp);

void ride_delete(Ride* ride);
void ride_renew(Ride* ride);
money16 ride_get_price(Ride* ride);

TileElement* get_station_platform(int32_t x, int32_t y, int32_t z, int32_t z_tolerance);
bool ride_has_adjacent_station(Ride* ride);
bool ride_has_station_shelter(Ride* ride);
bool ride_has_ratings(const Ride* ride);

const char* ride_type_get_enum_name(int32_t rideType);

uint8_t ride_entry_get_first_non_null_ride_type(const rct_ride_entry* rideEntry);
bool ride_type_supports_boosters(uint8_t rideType);
int32_t get_booster_speed(uint8_t rideType, int32_t rawSpeed);
void fix_invalid_vehicle_sprite_sizes();
bool ride_entry_has_category(const rct_ride_entry* rideEntry, uint8_t category);

int32_t ride_get_entry_index(int32_t rideType, int32_t rideSubType);
StationObject* ride_get_station_object(const Ride* ride);

void ride_action_modify(Ride* ride, int32_t modifyType, int32_t flags);
void ride_stop_peeps_queuing(Ride* ride);

LocationXY16 ride_get_rotated_coords(int16_t x, int16_t y, int16_t z);

void determine_ride_entrance_and_exit_locations();
void ride_clear_leftover_entrances(Ride* ride);

#endif
