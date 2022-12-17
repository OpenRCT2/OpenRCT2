/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Limits.h"
#include "../actions/ResultWithMessage.h"
#include "../common.h"
#include "../core/BitSet.hpp"
#include "../object/MusicObject.h"
#include "../rct2/DATLimits.h"
#include "../rct2/Limits.h"
#include "../world/Map.h"
#include "RideColour.h"
#include "RideEntry.h"
#include "RideRatings.h"
#include "RideTypes.h"
#include "Track.h"
#include "VehicleColour.h"

#include <array>
#include <limits>
#include <memory>
#include <string_view>

struct IObjectManager;
class Formatter;
class StationObject;
struct Ride;
struct RideTypeDescriptor;
struct Guest;
struct Staff;
struct Vehicle;
struct rct_ride_entry;
struct ResultWithMessage;

#define RIDE_ADJACENCY_CHECK_DISTANCE 5
constexpr uint8_t TUNE_ID_NULL = 0xFF;

constexpr uint16_t const MAX_STATION_LOCATIONS = OpenRCT2::Limits::MaxStationsPerRide * 2; // Entrance and exit per station

constexpr uint16_t const MAZE_CLEARANCE_HEIGHT = 4 * COORDS_Z_STEP;

struct RideStation
{
    CoordsXY Start;
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
    EntityId LastPeepInQueue;

    static constexpr uint8_t NO_TRAIN = std::numeric_limits<uint8_t>::max();

    int32_t GetBaseZ() const;
    void SetBaseZ(int32_t newZ);
    CoordsXYZ GetStart() const;
};

struct RideMeasurement
{
    static constexpr size_t MAX_ITEMS = 4800;

    uint8_t flags{};
    uint32_t last_use_tick{};
    uint16_t num_items{};
    uint16_t current_item{};
    uint8_t vehicle_index{};
    StationIndex current_station{};
    int8_t vertical[MAX_ITEMS]{};
    int8_t lateral[MAX_ITEMS]{};
    uint8_t velocity[MAX_ITEMS]{};
    uint8_t altitude[MAX_ITEMS]{};
};

enum class RideClassification
{
    Ride,
    ShopOrStall,
    KioskOrFacility
};

namespace ShelteredSectionsBits
{
    constexpr const uint8_t NumShelteredSectionsMask = 0b00011111;
    constexpr const uint8_t RotatingWhileSheltered = 0b00100000;
    constexpr const uint8_t BankingWhileSheltered = 0b01000000;
}; // namespace ShelteredSectionsBits

struct TrackDesign;
struct TrackDesignState;
enum class RideMode : uint8_t;
enum class RideStatus : uint8_t;

/**
 * Ride structure.
 *
 * This is based on RCT2's ride structure.
 * Padding and no longer used fields have been removed.
 */
struct Ride
{
    RideId id = RideId::GetNull();
    ride_type_t type = RIDE_TYPE_NULL;
    // pointer to static info. for example, wild mouse type is 0x36, subtype is
    // 0x4c.
    ObjectEntryIndex subtype;
    RideMode mode;
    uint8_t colour_scheme_type;
    VehicleColour vehicle_colours[OpenRCT2::Limits::MaxTrainsPerRide + 1];
    // 0 = closed, 1 = open, 2 = test
    RideStatus status;
    std::string custom_name;
    uint16_t default_name_number;
    CoordsXY overall_view;
    EntityId vehicles[OpenRCT2::Limits::MaxTrainsPerRide + 1]; // Points to the first car in the train
    uint8_t depart_flags;
    uint8_t num_stations;
    uint8_t NumTrains;
    uint8_t num_cars_per_train;
    uint8_t ProposedNumTrains;
    uint8_t proposed_num_cars_per_train;
    uint8_t max_trains;
    uint8_t MinCarsPerTrain;
    uint8_t MaxCarsPerTrain;
    uint8_t min_waiting_time;
    uint8_t max_waiting_time;
    union
    {
        uint8_t operation_option;
        uint8_t time_limit;
        uint8_t NumLaps;
        uint8_t launch_speed;
        uint8_t speed;
        uint8_t rotations;
    };

    uint8_t boat_hire_return_direction;
    TileCoordsXY boat_hire_return_position;
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
    // x y z map location of the current track piece during a test
    // this is to prevent counting special tracks multiple times
    TileCoordsXYZ CurTestTrackLocation;
    // Next 3 variables are related (XXXX XYYY ZZZa aaaa)
    uint16_t turn_count_default; // X = current turn count
    uint16_t turn_count_banked;
    uint16_t turn_count_sloped; // X = number turns > 3 elements
    // Y is number of powered lifts, X is drops
    uint8_t drops; // (YYXX XXXX)
    uint8_t start_drop_height;
    uint8_t highest_drop_height;
    int32_t sheltered_length;
    // Unused always 0? Should affect nausea
    uint16_t var_11C;
    uint8_t num_sheltered_sections; // (?abY YYYY)
    // Customer counter in the current 960 game tick (about 30 seconds) interval
    uint16_t cur_num_customers;
    // Counts ticks to update customer intervals, resets each 960 game ticks.
    uint16_t num_customers_timeout;
    // Customer count in the last 10 * 960 game ticks (sliding window)
    uint16_t num_customers[OpenRCT2::Limits::CustomerHistorySize];
    money16 price[RCT2::ObjectLimits::MaxShopItemsPerRideEntry];
    TileCoordsXYZ ChairliftBullwheelLocation[2];
    union
    {
        RatingTuple ratings;
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
    money64 total_profit;
    uint8_t popularity;
    uint8_t popularity_time_out; // Updated every purchase and ?possibly by time?
    uint8_t popularity_next;     // When timeout reached this will be the next popularity
    uint16_t num_riders;
    uint8_t music_tune_id;
    uint8_t slide_in_use;
    union
    {
        EntityId slide_peep;
        uint16_t maze_tiles;
    };
    uint8_t slide_peep_t_shirt_colour;
    uint8_t spiral_slide_progress;
    int32_t build_date;
    money16 upkeep_cost;
    EntityId race_winner;
    uint32_t music_position;
    uint8_t breakdown_reason_pending;
    uint8_t mechanic_status;
    EntityId mechanic;
    StationIndex inspection_station;
    uint8_t broken_vehicle;
    uint8_t broken_car;
    uint8_t breakdown_reason;
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
    uint8_t downtime_history[OpenRCT2::Limits::DowntimeHistorySize];
    uint32_t no_primary_items_sold;
    uint32_t no_secondary_items_sold;
    uint8_t breakdown_sound_modifier;
    // Used to oscillate the sound when ride breaks down.
    // 0 = no change, 255 = max change
    uint8_t not_fixed_timeout;
    uint8_t last_crash_type;
    uint8_t connected_message_throttle;
    money64 income_per_hour;
    money64 profit;
    TrackColour track_colour[OpenRCT2::Limits::NumColourSchemes];
    ObjectEntryIndex music;
    ObjectEntryIndex entrance_style;
    uint16_t vehicle_change_timeout;
    uint8_t num_block_brakes;
    uint8_t lift_hill_speed;
    uint16_t guests_favourite;
    uint32_t lifecycle_flags;
    uint16_t total_air_time;
    StationIndex current_test_station;
    uint8_t num_circuits;
    CoordsXYZ CableLiftLoc;
    EntityId cable_lift;

    // These two fields are used to warn users about issues.
    // Such issue can be hacked rides with incompatible options set.
    // They don't require export/import.
    uint8_t current_issues;
    uint32_t last_issue_time;

    // TO-DO: those friend functions are temporary, find a way to not access the private fields
    friend void UpdateSpiralSlide(Ride& ride);
    friend void UpdateChairlift(Ride& ride);

private:
    std::array<RideStation, OpenRCT2::Limits::MaxStationsPerRide> stations;

public:
    RideStation& GetStation(StationIndex stationIndex = StationIndex::FromUnderlying(0));
    const RideStation& GetStation(StationIndex stationIndex = StationIndex::FromUnderlying(0)) const;
    std::array<RideStation, OpenRCT2::Limits::MaxStationsPerRide>& GetStations();
    const std::array<RideStation, OpenRCT2::Limits::MaxStationsPerRide>& GetStations() const;
    StationIndex GetStationIndex(const RideStation* station) const;

public:
    uint16_t inversions;
    uint16_t holes;
    uint8_t sheltered_eighths;

    std::unique_ptr<RideMeasurement> measurement;

private:
    void Update();
    void UpdateQueueLength(StationIndex stationIndex);
    ResultWithMessage CreateVehicles(const CoordsXYE& element, bool isApplying);
    void MoveTrainsToBlockBrakes(TrackElement* firstBlock);
    money64 CalculateIncomePerHour() const;
    void ChainQueues() const;
    void ConstructMissingEntranceOrExit() const;

public:
    bool CanBreakDown() const;
    RideClassification GetClassification() const;
    bool IsRide() const;
    void Renew();
    void Delete();
    void Crash(uint8_t vehicleIndex);
    void SetToDefaultInspectionInterval();
    void SetRideEntry(ObjectEntryIndex entryIndex);

    void SetNumTrains(int32_t numTrains);
    void SetNumCarsPerVehicle(int32_t numCarsPerVehicle);
    void UpdateMaxVehicles();
    void UpdateNumberOfCircuits();

    bool HasSpinningTunnel() const;
    bool HasWaterSplash() const;
    bool HasRapids() const;
    bool HasLogReverser() const;
    bool HasWaterfall() const;
    bool HasWhirlpool() const;

    bool IsPoweredLaunched() const;
    bool IsBlockSectioned() const;
    bool CanHaveMultipleCircuits() const;
    bool SupportsStatus(RideStatus s) const;

    void StopGuestsQueuing();
    void ValidateStations();

    ResultWithMessage Open(bool isApplying);
    ResultWithMessage Test(RideStatus newStatus, bool isApplying);

    RideMode GetDefaultMode() const;

    void SetColourPreset(uint8_t index);

    rct_ride_entry* GetRideEntry() const;

    size_t GetNumPrices() const;
    int32_t GetAge() const;
    int32_t GetTotalQueueLength() const;
    int32_t GetMaxQueueTime() const;

    void QueueInsertGuestAtFront(StationIndex stationIndex, Guest* peep);
    Guest* GetQueueHeadGuest(StationIndex stationIndex) const;

    void SetNameToDefault();
    std::string GetName() const;
    void FormatNameTo(Formatter&) const;
    void FormatStatusTo(Formatter&) const;

    static void UpdateAll();
    static bool NameExists(std::string_view name, RideId excludeRideId = RideId::GetNull());

    [[nodiscard]] std::unique_ptr<TrackDesign> SaveToTrackDesign(TrackDesignState& tds) const;

    uint64_t GetAvailableModes() const;
    const RideTypeDescriptor& GetRideTypeDescriptor() const;
    TrackElement* GetOriginElement(StationIndex stationIndex) const;

    std::pair<RideMeasurement*, OpenRCT2String> GetMeasurement();

    uint8_t GetNumShelteredSections() const;
    void IncreaseNumShelteredSections();

    void RemoveVehicles();
    /**
     * Updates all pieces of the ride to match the internal ride type. (Track pieces can have different ride types from the ride
     * they belong to, to enable “merging”.)
     */
    void UpdateRideTypeForAllPieces();

    void UpdateSatisfaction(const uint8_t happiness);
    void UpdatePopularity(const uint8_t pop_amount);
    void RemovePeeps();

    int32_t GetTotalLength() const;
    int32_t GetTotalTime() const;

    const StationObject* GetStationObject() const;
    const MusicObject* GetMusicObject() const;

    bool HasLifecycleFlag(uint32_t flag) const;
    void SetLifecycleFlag(uint32_t flag, bool on);

    bool HasRecolourableShopItems() const;
    bool HasStation() const;

    bool FindTrackGap(const CoordsXYE& input, CoordsXYE* output) const;
};
void UpdateSpiralSlide(Ride& ride);
void UpdateChairlift(Ride& ride);

#pragma pack(push, 1)

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
    RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN = 1 << 18,    // Used for the Award for Best Custom-designed Rides
    RIDE_LIFECYCLE_SIX_FLAGS_DEPRECATED = 1 << 19, // Not used anymore
    RIDE_LIFECYCLE_FIXED_RATINGS = 1 << 20,        // When set, the ratings will not be updated (useful for hacked rides).
    RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS = 1 << 21,
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
    RIDE_ENTRY_DISABLE_LAST_OPERATING_MODE_DEPRECATED = 1 << 15,
    RIDE_ENTRY_FLAG_DISABLE_DOOR_CONSTRUCTION_DEPRECATED = 1 << 16,
    RIDE_ENTRY_DISABLE_FIRST_TWO_OPERATING_MODES_DEPRECATED = 1 << 17,
    RIDE_ENTRY_FLAG_DISABLE_COLLISION_CRASHES = 1 << 18,
    RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB = 1 << 19,
    // Must be set with swing mode 1 as well.
    RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_2 = 1 << 20,
    RIDE_ENTRY_FLAG_RIDER_CONTROLS_SPEED = 1 << 21,
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
    RIDE_TYPE_HYPERCOASTER,
    RIDE_TYPE_HYPER_TWISTER,
    RIDE_TYPE_MONSTER_TRUCKS,
    RIDE_TYPE_SPINNING_WILD_MOUSE,
    RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER,
    RIDE_TYPE_HYBRID_COASTER,
    RIDE_TYPE_SINGLE_RAIL_ROLLER_COASTER,
    RIDE_TYPE_ALPINE_COASTER,
    RIDE_TYPE_CLASSIC_WOODEN_ROLLER_COASTER,

    RIDE_TYPE_COUNT
};

enum class RideStatus : uint8_t
{
    Closed,
    Open,
    Testing,
    Simulating,
    Count,
};

enum class RideMode : uint8_t
{
    Normal,
    ContinuousCircuit,
    ReverseInclineLaunchedShuttle,
    PoweredLaunchPasstrough, // RCT2 style, pass through station
    Shuttle,
    BoatHire,
    UpwardLaunch,
    RotatingLift,
    StationToStation,
    SingleRidePerAdmission,
    UnlimitedRidesPerAdmission = 10,
    Maze,
    Race,
    Dodgems,
    Swing,
    ShopStall,
    Rotation,
    ForwardRotation,
    BackwardRotation,
    FilmAvengingAviators,
    MouseTails3DFilm = 20,
    SpaceRings,
    Beginners,
    LimPoweredLaunch,
    FilmThrillRiders,
    StormChasers3DFilm,
    SpaceRaiders3DFilm,
    Intense,
    Berserk,
    HauntedHouse,
    Circus = 30,
    DownwardLaunch,
    CrookedHouse,
    FreefallDrop,
    ContinuousCircuitBlockSectioned,
    PoweredLaunch, // RCT1 style, don't pass through station
    PoweredLaunchBlockSectioned,

    Count,
    NullMode = 255,
};

RideMode& operator++(RideMode& d, int);

enum
{
    RIDE_COLOUR_SCHEME_MODE_ALL_SAME,
    RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_TRAIN,
    RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_CAR,

    RIDE_COLOUR_SCHEME_MODE_COUNT,
};

enum
{
    RIDE_CATEGORY_TRANSPORT,
    RIDE_CATEGORY_GENTLE,
    RIDE_CATEGORY_ROLLERCOASTER,
    RIDE_CATEGORY_THRILL,
    RIDE_CATEGORY_WATER,
    RIDE_CATEGORY_SHOP,

    RIDE_CATEGORY_NONE = 255,
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

    WAIT_FOR_LOAD_COUNT,
};

enum
{
    RIDE_COLOUR_SCHEME_MAIN,
    RIDE_COLOUR_SCHEME_ADDITIONAL_1,
    RIDE_COLOUR_SCHEME_ADDITIONAL_2,
    RIDE_COLOUR_SCHEME_ADDITIONAL_3,

    RIDE_COLOUR_SCHEME_COUNT,
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

enum
{
    RIDE_CRASH_TYPE_NONE = 0,
    RIDE_CRASH_TYPE_NO_FATALITIES = 2,
    RIDE_CRASH_TYPE_FATALITIES = 8
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
    RIDE_MODIFY_RENEW,
};

enum
{
    RIDE_ISSUE_NONE = 0,
    RIDE_ISSUE_GUESTS_STUCK = (1 << 0),
};

enum
{
    TRACK_BLOCK_2 = (1 << 2)
};

enum
{
    TRACK_ELEMENT_SET_HIGHLIGHT_FALSE = (1 << 0),
    TRACK_ELEMENT_SET_HIGHLIGHT_TRUE = (1 << 1),
    TRACK_ELEMENT_SET_COLOUR_SCHEME = (1 << 2),
    TRACK_ELEMENT_SET_HAS_CABLE_LIFT_TRUE = (1 << 3),
    TRACK_ELEMENT_SET_HAS_CABLE_LIFT_FALSE = (1 << 4),
    TRACK_ELEMENT_SET_SEAT_ROTATION = (1 << 5)
};

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

constexpr uint32_t CONSTRUCTION_LIFT_HILL_SELECTED = 1 << 0;
constexpr uint32_t CONSTRUCTION_INVERTED_TRACK_SELECTED = 1 << 1;

Ride* get_ride(RideId index);

struct RideManager
{
    const Ride* operator[](RideId id) const
    {
        return get_ride(id);
    }

    Ride* operator[](RideId id)
    {
        return get_ride(id);
    }

    class Iterator
    {
        friend RideManager;

    private:
        RideManager* _rideManager;
        RideId::UnderlyingType _index{};
        RideId::UnderlyingType _endIndex{};

    public:
        using difference_type = intptr_t;
        using value_type = Ride;
        using pointer = const Ride*;
        using reference = const Ride&;
        using iterator_category = std::forward_iterator_tag;

    private:
        Iterator(RideManager& rideManager, size_t beginIndex, size_t endIndex)
            : _rideManager(&rideManager)
            , _index(static_cast<RideId::UnderlyingType>(beginIndex))
            , _endIndex(static_cast<RideId::UnderlyingType>(endIndex))
        {
            if (_index < _endIndex && (*_rideManager)[RideId::FromUnderlying(_index)] == nullptr)
            {
                ++(*this);
            }
        }

    public:
        Iterator& operator++()
        {
            do
            {
                _index++;
            } while (_index < _endIndex && (*_rideManager)[RideId::FromUnderlying(_index)] == nullptr);
            return *this;
        }
        Iterator operator++(int)
        {
            auto result = *this;
            ++(*this);
            return result;
        }
        bool operator==(Iterator other) const
        {
            return _index == other._index;
        }
        bool operator!=(Iterator other) const
        {
            return !(*this == other);
        }
        Ride& operator*()
        {
            return *(*_rideManager)[RideId::FromUnderlying(_index)];
        }
    };

    size_t size() const;
    Iterator begin();
    Iterator end();
    Iterator get(RideId rideId);
    Iterator begin() const
    {
        return (const_cast<RideManager*>(this))->begin();
    }
    Iterator end() const
    {
        return (const_cast<RideManager*>(this))->end();
    }
};

RideManager GetRideManager();
RideId GetNextFreeRideId();
Ride* GetOrAllocateRide(RideId index);
rct_ride_entry* get_ride_entry(ObjectEntryIndex index);
std::string_view get_ride_entry_name(ObjectEntryIndex index);

extern money16 gTotalRideValueForMoney;

extern const StringId ColourSchemeNames[4];

extern ObjectEntryIndex gLastEntranceStyle;

int32_t ride_get_count();
void ride_init_all();
void reset_all_ride_build_dates();
void ride_update_favourited_stat();
void ride_check_all_reachable();

bool ride_try_get_origin_element(const Ride* ride, CoordsXYE* output);
void ride_clear_blocked_tiles(Ride* ride);
Staff* ride_get_mechanic(Ride* ride);
Staff* ride_get_assigned_mechanic(Ride* ride);
TrackColour ride_get_track_colour(Ride* ride, int32_t colourScheme);
VehicleColour ride_get_vehicle_colour(Ride* ride, int32_t vehicleIndex);
int32_t ride_get_unused_preset_vehicle_colour(ObjectEntryIndex subType);
void ride_set_vehicle_colours_to_random_preset(Ride* ride, uint8_t preset_index);
void ride_measurements_update();
void ride_breakdown_add_news_item(Ride* ride);
Staff* ride_find_closest_mechanic(Ride* ride, int32_t forInspection);
int32_t ride_initialise_construction_window(Ride* ride);
void ride_set_map_tooltip(TileElement* tileElement);
void ride_prepare_breakdown(Ride* ride, int32_t breakdownReason);
TileElement* ride_get_station_start_track_element(const Ride* ride, StationIndex stationIndex);
TileElement* ride_get_station_exit_element(const CoordsXYZ& elementPos);
int32_t ride_get_refund_price(const Ride* ride);
int32_t ride_get_random_colour_preset_index(ride_type_t rideType);
money32 ride_get_common_price(Ride* forRide);

void ride_clear_for_construction(Ride* ride);
void invalidate_test_results(Ride* ride);

void increment_turn_count_1_element(Ride* ride, uint8_t type);
void increment_turn_count_2_elements(Ride* ride, uint8_t type);
void increment_turn_count_3_elements(Ride* ride, uint8_t type);
void increment_turn_count_4_plus_elements(Ride* ride, uint8_t type);
int32_t get_turn_count_1_element(Ride* ride, uint8_t type);
int32_t get_turn_count_2_elements(Ride* ride, uint8_t type);
int32_t get_turn_count_3_elements(Ride* ride, uint8_t type);
int32_t get_turn_count_4_plus_elements(Ride* ride, uint8_t type);

uint8_t ride_get_helix_sections(Ride* ride);

bool ride_has_any_track_elements(const Ride* ride);

bool track_block_get_next(CoordsXYE* input, CoordsXYE* output, int32_t* z, int32_t* direction);
bool track_block_get_next_from_zero(
    const CoordsXYZ& startPos, Ride* ride, uint8_t direction_start, CoordsXYE* output, int32_t* z, int32_t* direction,
    bool isGhost);

bool track_block_get_previous(const CoordsXYE& trackPos, track_begin_end* outTrackBeginEnd);
bool track_block_get_previous_from_zero(
    const CoordsXYZ& startPos, Ride* ride, uint8_t direction, track_begin_end* outTrackBeginEnd);

void ride_get_start_of_track(CoordsXYE* output);

void window_ride_construction_update_active_elements();
money32 RideEntranceExitPlaceGhost(
    Ride* ride, const CoordsXY& entranceExitCoords, Direction direction, int32_t placeType, StationIndex stationNum);

ResultWithMessage ride_are_all_possible_entrances_and_exits_built(Ride* ride);
void ride_fix_breakdown(Ride* ride, int32_t reliabilityIncreaseFactor);

uint8_t ride_entry_get_vehicle_at_position(int32_t rideEntryIndex, int32_t numCarsPerTrain, int32_t position);
void ride_update_vehicle_colours(Ride* ride);

OpenRCT2::BitSet<TRACK_GROUP_COUNT> ride_entry_get_supported_track_pieces(const rct_ride_entry* rideEntry);

enum class RideSetSetting : uint8_t;
money32 set_operating_setting(RideId rideId, RideSetSetting setting, uint8_t value);
money32 set_operating_setting_nested(RideId rideId, RideSetSetting setting, uint8_t value, uint8_t flags);

void UpdateGhostTrackAndArrow();

uint32_t ride_customers_per_hour(const Ride* ride);
uint32_t ride_customers_in_last_5_minutes(const Ride* ride);

Vehicle* ride_get_broken_vehicle(const Ride* ride);

money16 ride_get_price(const Ride* ride);

TileElement* get_station_platform(const CoordsXYRangedZ& coords);
bool ride_has_adjacent_station(Ride* ride);
bool ride_has_station_shelter(Ride* ride);
bool ride_has_ratings(const Ride* ride);

int32_t get_booster_speed(ride_type_t rideType, int32_t rawSpeed);
void fix_invalid_vehicle_sprite_sizes();
bool ride_entry_has_category(const rct_ride_entry* rideEntry, uint8_t category);

int32_t ride_get_entry_index(int32_t rideType, int32_t rideSubType);

void determine_ride_entrance_and_exit_locations();
void ride_clear_leftover_entrances(Ride* ride);

std::vector<RideId> GetTracklessRides();

void CircusMusicUpdate(Ride* ride);
void DefaultMusicUpdate(Ride* ride);
