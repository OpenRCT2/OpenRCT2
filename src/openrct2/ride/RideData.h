/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _RIDE_DATA_H_
#define _RIDE_DATA_H_

#define TRACK_COLOUR_PRESETS(...)                                                                                              \
    {                                                                                                                          \
        static_cast<uint8_t>(std::size<TrackColour>({ __VA_ARGS__ })),                                                         \
        {                                                                                                                      \
            __VA_ARGS__                                                                                                        \
        }                                                                                                                      \
    }
#define DEFAULT_FLAT_RIDE_COLOUR_PRESET TRACK_COLOUR_PRESETS({ COLOUR_BRIGHT_RED, COLOUR_LIGHT_BLUE, COLOUR_YELLOW })
#define DEFAULT_STALL_COLOUR_PRESET TRACK_COLOUR_PRESETS({ COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED })

#include "../common.h"
#include "../localisation/StringIds.h"
#include "../sprites.h"
#include "Ride.h"
#include "ShopItem.h"
#include "Track.h"
#include "TrackPaint.h"

using ride_ratings_calculation = void (*)(Ride* ride);
struct RideComponentName
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

enum class RideColourKey : uint8_t
{
    Ride,
    Food,
    Drink,
    Shop,
    InfoKiosk,
    FirstAid,
    CashMachine,
    Toilets
};

struct RideNameConvention
{
    RIDE_COMPONENT_TYPE vehicle;
    RIDE_COMPONENT_TYPE structure;
    RIDE_COMPONENT_TYPE station;
};

struct RideBuildCost
{
    uint16_t TrackPrice;
    uint16_t SupportPrice;
    uint8_t PriceEstimateMultiplier;
};

struct RideHeights
{
    uint8_t MaxHeight;
    uint8_t ClearanceHeight;
    int8_t VehicleZOffset;
    uint8_t PlatformHeight;
};

struct rct_ride_lift_data
{
    SoundId sound_id;
    uint8_t minimum_speed;
    uint8_t maximum_speed;
};

struct RideColourPreview
{
    uint32_t Track;
    uint32_t Supports;
};

struct UpkeepCostsDescriptor
{
    /**
     * Data about ride running costs. This is widely adjusted by the upkeep
     * function, so values that don't make much sense here (a roller coaster having
     * cheaper upkeep than a car ride) are fixed later on.
     *
     * Data generation script: https://gist.github.com/kevinburke/6bcf4a8fcc95faad7bac
     */
    uint8_t BaseCost;
    /** rct2: 0x0097E3AC */
    uint8_t TrackLengthMultiplier;
    uint8_t CostPerTrackPiece;
    /** rct2: 0x0097E3B4 */
    uint8_t CostPerTrain;
    /** rct2: 0x0097E3B6 */
    uint8_t CostPerCar;
    /** rct2: 0x0097E3B8 */
    uint8_t CostPerStation;
};

struct RideTypeDescriptor
{
    uint8_t AlternateType;
    uint8_t Category;
    /** rct2: 0x0097C468 (0 - 31) and 0x0097C5D4 (32 - 63) */
    uint64_t EnabledTrackPieces;
    // Pieces that this ride type _can_ draw, but are disabled because their vehicles lack the relevant sprites,
    // or because they are not realistic for the ride type (e.g. LIM boosters in Mini Roller Coasters).
    uint64_t ExtraTrackPieces;
    uint64_t CoveredTrackPieces;
    /** rct2: 0x0097CC68 */
    uint64_t StartTrackPiece;
    TRACK_PAINT_FUNCTION_GETTER TrackPaintFunction;
    uint64_t Flags;
    /** rct2: 0x0097C8AC */
    uint64_t RideModes;
    uint8_t DefaultMode;
    /** rct2: 0x0097CF40 */
    RideOperatingSettings OperatingSettings;
    RideNaming Naming;
    RideNameConvention NameConvention;
    const char* EnumName;
    uint8_t AvailableBreakdowns;
    /** rct2: 0x0097D218 */
    RideHeights Heights;
    uint8_t MaxMass;
    /** rct2: 0x0097D7C8, 0x0097D7C9, 0x0097D7CA */
    rct_ride_lift_data LiftData;
    // rct2: 0x0097E050
    ride_ratings_calculation RatingsCalculationFunction;
    // rct2: 0x0097CD1E
    RatingTuple RatingsMultipliers;
    UpkeepCostsDescriptor UpkeepCosts;
    // rct2: 0x0097DD78
    RideBuildCost BuildCosts;
    money16 DefaultPrices[NUM_SHOP_ITEMS_PER_RIDE];
    uint8_t DefaultMusic;
    /** rct2: 0x0097D7CB */
    ShopItemIndex PhotoItem;
    /** rct2: 0x0097D21E */
    uint8_t BonusValue;
    track_colour_preset_list ColourPresets;
    RideColourPreview ColourPreview;
    RideColourKey ColourKey;

    bool HasFlag(uint64_t flag) const;
    uint64_t GetAvailableTrackPieces() const;
    bool SupportsTrackPiece(const uint64_t trackPiece) const;
};

#ifdef _WIN32
#    define SET_FIELD(fieldname, ...) __VA_ARGS__
#else
#    define SET_FIELD(fieldname, ...) .fieldname = __VA_ARGS__
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

    RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY = (1ULL << 48),
    RIDE_TYPE_FLAG_SUPPORTS_LEVEL_CROSSINGS = (1ULL << 49),
    RIDE_TYPE_FLAG_IS_SUSPENDED = (1ULL << 50),
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

// clang-format off
constexpr const RideComponentName RideComponentNames[] = 
{
    { STR_RIDE_COMPONENT_TRAIN,             STR_RIDE_COMPONENT_TRAIN_PLURAL,            STR_RIDE_COMPONENT_TRAIN_CAPITALISED,               STR_RIDE_COMPONENT_TRAIN_CAPITALISED_PLURAL,            STR_RIDE_COMPONENT_TRAIN_COUNT,             STR_RIDE_COMPONENT_TRAIN_COUNT_PLURAL,              STR_RIDE_COMPONENT_TRAIN_NO },
    { STR_RIDE_COMPONENT_BOAT,              STR_RIDE_COMPONENT_BOAT_PLURAL,             STR_RIDE_COMPONENT_BOAT_CAPITALISED,                STR_RIDE_COMPONENT_BOAT_CAPITALISED_PLURAL,             STR_RIDE_COMPONENT_BOAT_COUNT,              STR_RIDE_COMPONENT_BOAT_COUNT_PLURAL,               STR_RIDE_COMPONENT_BOAT_NO },
    { STR_RIDE_COMPONENT_TRACK,             STR_RIDE_COMPONENT_TRACK_PLURAL,            STR_RIDE_COMPONENT_TRACK_CAPITALISED,               STR_RIDE_COMPONENT_TRACK_CAPITALISED_PLURAL,            STR_RIDE_COMPONENT_TRACK_COUNT,             STR_RIDE_COMPONENT_TRACK_COUNT_PLURAL,              STR_RIDE_COMPONENT_TRACK_NO },
    { STR_RIDE_COMPONENT_DOCKING_PLATFORM,  STR_RIDE_COMPONENT_DOCKING_PLATFORM_PLURAL, STR_RIDE_COMPONENT_DOCKING_PLATFORM_CAPITALISED,    STR_RIDE_COMPONENT_DOCKING_PLATFORM_CAPITALISED_PLURAL, STR_RIDE_COMPONENT_DOCKING_PLATFORM_COUNT,  STR_RIDE_COMPONENT_DOCKING_PLATFORM_COUNT_PLURAL,   STR_RIDE_COMPONENT_DOCKING_PLATFORM_NO },
    { STR_RIDE_COMPONENT_STATION,           STR_RIDE_COMPONENT_STATION_PLURAL,          STR_RIDE_COMPONENT_STATION_CAPITALISED,             STR_RIDE_COMPONENT_STATION_CAPITALISED_PLURAL,          STR_RIDE_COMPONENT_STATION_COUNT,           STR_RIDE_COMPONENT_STATION_COUNT_PLURAL,            STR_RIDE_COMPONENT_STATION_NO },
    { STR_RIDE_COMPONENT_CAR,               STR_RIDE_COMPONENT_CAR_PLURAL,              STR_RIDE_COMPONENT_CAR_CAPITALISED,                 STR_RIDE_COMPONENT_CAR_CAPITALISED_PLURAL,              STR_RIDE_COMPONENT_CAR_COUNT,               STR_RIDE_COMPONENT_CAR_COUNT_PLURAL,                STR_RIDE_COMPONENT_CAR_NO },
    { STR_RIDE_COMPONENT_BUILDING,          STR_RIDE_COMPONENT_BUILDING_PLURAL,         STR_RIDE_COMPONENT_BUILDING_CAPITALISED,            STR_RIDE_COMPONENT_BUILDING_CAPITALISED_PLURAL,         STR_RIDE_COMPONENT_BUILDING_COUNT,          STR_RIDE_COMPONENT_BUILDING_COUNT_PLURAL,           STR_RIDE_COMPONENT_BUILDING_NO },
    { STR_RIDE_COMPONENT_STRUCTURE,         STR_RIDE_COMPONENT_STRUCTURE_PLURAL,        STR_RIDE_COMPONENT_STRUCTURE_CAPITALISED,           STR_RIDE_COMPONENT_STRUCTURE_CAPITALISED_PLURAL,        STR_RIDE_COMPONENT_STRUCTURE_COUNT,         STR_RIDE_COMPONENT_STRUCTURE_COUNT_PLURAL,          STR_RIDE_COMPONENT_STRUCTURE_NO },
    { STR_RIDE_COMPONENT_SHIP,              STR_RIDE_COMPONENT_SHIP_PLURAL,             STR_RIDE_COMPONENT_SHIP_CAPITALISED,                STR_RIDE_COMPONENT_SHIP_CAPITALISED_PLURAL,             STR_RIDE_COMPONENT_SHIP_COUNT,              STR_RIDE_COMPONENT_SHIP_COUNT_PLURAL,               STR_RIDE_COMPONENT_SHIP_NO },
    { STR_RIDE_COMPONENT_CABIN,             STR_RIDE_COMPONENT_CABIN_PLURAL,            STR_RIDE_COMPONENT_CABIN_CAPITALISED,               STR_RIDE_COMPONENT_CABIN_CAPITALISED_PLURAL,            STR_RIDE_COMPONENT_CABIN_COUNT,             STR_RIDE_COMPONENT_CABIN_COUNT_PLURAL,              STR_RIDE_COMPONENT_CABIN_NO },
    { STR_RIDE_COMPONENT_WHEEL,             STR_RIDE_COMPONENT_WHEEL_PLURAL,            STR_RIDE_COMPONENT_WHEEL_CAPITALISED,               STR_RIDE_COMPONENT_WHEEL_CAPITALISED_PLURAL,            STR_RIDE_COMPONENT_WHEEL_COUNT,             STR_RIDE_COMPONENT_WHEEL_COUNT_PLURAL,              STR_RIDE_COMPONENT_WHEEL_NO },
    { STR_RIDE_COMPONENT_RING,              STR_RIDE_COMPONENT_RING_PLURAL,             STR_RIDE_COMPONENT_RING_CAPITALISED,                STR_RIDE_COMPONENT_RING_CAPITALISED_PLURAL,             STR_RIDE_COMPONENT_RING_COUNT,              STR_RIDE_COMPONENT_RING_COUNT_PLURAL,               STR_RIDE_COMPONENT_RING_NO },
    { STR_RIDE_COMPONENT_PLAYER,            STR_RIDE_COMPONENT_PLAYER_PLURAL,           STR_RIDE_COMPONENT_PLAYER_CAPITALISED,              STR_RIDE_COMPONENT_PLAYER_CAPITALISED_PLURAL,           STR_RIDE_COMPONENT_PLAYER_COUNT,            STR_RIDE_COMPONENT_PLAYER_COUNT_PLURAL,             STR_RIDE_COMPONENT_PLAYER_NO },
    { STR_RIDE_COMPONENT_COURSE,            STR_RIDE_COMPONENT_COURSE_PLURAL,           STR_RIDE_COMPONENT_COURSE_CAPITALISED,              STR_RIDE_COMPONENT_COURSE_CAPITALISED_PLURAL,           STR_RIDE_COMPONENT_COURSE_COUNT,            STR_RIDE_COMPONENT_COURSE_COUNT_PLURAL,             STR_RIDE_COMPONENT_COURSE_NO },
};
// clang-format on

constexpr const uint64_t AllRideModesAvailable = (1ULL << RIDE_MODE_CONTINUOUS_CIRCUIT)
    | (1ULL << RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED) | (1ULL << RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE)
    | (1ULL << RIDE_MODE_POWERED_LAUNCH_PASSTROUGH) | (1ULL << RIDE_MODE_SHUTTLE) | (1ULL << RIDE_MODE_NORMAL)
    | (1ULL << RIDE_MODE_BOAT_HIRE) | (1ULL << RIDE_MODE_UPWARD_LAUNCH) | (1ULL << RIDE_MODE_ROTATING_LIFT)
    | (1ULL << RIDE_MODE_STATION_TO_STATION) | (1ULL << RIDE_MODE_SINGLE_RIDE_PER_ADMISSION)
    | (1ULL << RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION) | (1ULL << RIDE_MODE_MAZE) | (1ULL << RIDE_MODE_RACE)
    | (1ULL << RIDE_MODE_DODGEMS) | (1ULL << RIDE_MODE_SWING) | (1ULL << RIDE_MODE_SHOP_STALL) | (1ULL << RIDE_MODE_ROTATION)
    | (1ULL << RIDE_MODE_FORWARD_ROTATION) | (1ULL << RIDE_MODE_BACKWARD_ROTATION) | (1ULL << RIDE_MODE_FILM_AVENGING_AVIATORS)
    | (1ULL << RIDE_MODE_3D_FILM_MOUSE_TAILS) | (1ULL << RIDE_MODE_SPACE_RINGS) | (1ULL << RIDE_MODE_BEGINNERS)
    | (1ULL << RIDE_MODE_LIM_POWERED_LAUNCH) | (1ULL << RIDE_MODE_FILM_THRILL_RIDERS)
    | (1ULL << RIDE_MODE_3D_FILM_STORM_CHASERS) | (1ULL << RIDE_MODE_3D_FILM_SPACE_RAIDERS) | (1ULL << RIDE_MODE_INTENSE)
    | (1ULL << RIDE_MODE_BERSERK) | (1ULL << RIDE_MODE_HAUNTED_HOUSE) | (1ULL << RIDE_MODE_CIRCUS_SHOW)
    | (1ULL << RIDE_MODE_DOWNWARD_LAUNCH) | (1ULL << RIDE_MODE_CROOKED_HOUSE) | (1ULL << RIDE_MODE_FREEFALL_DROP)
    | (1ULL << RIDE_MODE_POWERED_LAUNCH) | (1ULL << RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED);

extern const rct_ride_entry_vehicle CableLiftVehicle;

extern const uint16_t RideFilmLength[3];

extern const rct_string_id RideModeNames[RIDE_MODE_COUNT];

// clang-format off
constexpr const RideTypeDescriptor DummyRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_NONE),
    SET_FIELD(EnabledTrackPieces, 0),
    SET_FIELD(ExtraTrackPieces, 0),
    SET_FIELD(CoveredTrackPieces, 0),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, nullptr),
    SET_FIELD(Flags, 0),
    SET_FIELD(RideModes, (1ULL << RIDE_MODE_CONTINUOUS_CIRCUIT)),
    SET_FIELD(DefaultMode, RIDE_MODE_CONTINUOUS_CIRCUIT),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_UNKNOWN_RIDE, STR_RIDE_DESCRIPTION_UNKNOWN }),
    SET_FIELD(NameConvention, { RIDE_COMPONENT_TYPE_TRAIN, RIDE_COMPONENT_TYPE_TRACK, RIDE_COMPONENT_TYPE_STATION }),
    SET_FIELD(EnumName, "(INVALID)"),
    SET_FIELD(AvailableBreakdowns, 0),
    SET_FIELD(Heights, { 12, 64, 0, 0, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, nullptr),
    SET_FIELD(RatingsMultipliers, { 0, 0, 0 }),
    SET_FIELD(UpkeepCosts, { 50, 1, 0, 0, 0, 0 }),
    SET_FIELD(BuildCosts, { 0, 0, 1 }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_STYLE_GENTLE),
    SET_FIELD(PhotoItem, SHOP_ITEM_PHOTO),
    SET_FIELD(BonusValue, 0),
    SET_FIELD(ColourPresets, DEFAULT_FLAT_RIDE_COLOUR_PRESET),
    SET_FIELD(ColourPreview, { static_cast<uint32_t>(SPR_NONE), static_cast<uint32_t>(SPR_NONE) }),
    SET_FIELD(ColourKey, RideColourKey::Ride)
};
// clang-format on

#endif
