/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Limits.h"
#include "../core/BitSet.hpp"
#include "../core/FixedPoint.hpp"
#include "../core/FlagHolder.hpp"
#include "RideColour.h"
#include "RideEntry.h"
#include "RideRatings.h"
#include "RideTypes.h"
#include "Track.h"
#include "VehicleColour.h"

#include <array>
#include <limits>
#include <memory>
#include <span>
#include <string_view>

struct IObjectManager;
struct Ride;
struct RideTypeDescriptor;
struct Guest;
struct OpenRCT2String;
struct Staff;
struct Vehicle;
struct RideObjectEntry;
struct ResultWithMessage;

namespace OpenRCT2
{
    class Formatter;
    class MusicObject;
    class StationObject;

    struct TileElement;
    struct TrackElement;
} // namespace OpenRCT2

constexpr uint8_t kRideAdjacencyCheckDistance = 5;

constexpr uint8_t kTuneIDNull = 0xFF;

constexpr uint16_t kMaxStationLocations = OpenRCT2::Limits::kMaxStationsPerRide * 2; // Entrance and exit per station

constexpr uint16_t kMazeClearanceHeight = 4 * kCoordsZStep;

constexpr money64 kRideMinPrice = 0.00_GBP;
constexpr money64 kRideMaxPrice = 20.00_GBP;

extern const StringId kRideInspectionIntervalNames[];

enum class RideInspection : uint8_t
{
    every10Minutes,
    every20Minutes,
    every30Minutes,
    every45Minutes,
    everyHour,
    every2Hours,
    never,
};

// Flags used by ride->windowInvalidateFlags
enum class RideInvalidateFlag : uint8_t
{
    customers,
    income,
    main,
    list,
    operatingSettings,
    maintenance,
    music,
    ratings,
};
using RideInvalidateFlags = FlagHolder<uint8_t, RideInvalidateFlag>;

enum class RideMeasurementFlag : uint8_t
{
    running,
    unloading,
    gForces
};
using RideMeasurementFlags = FlagHolder<uint8_t, RideMeasurementFlag>;

enum class RideTestingFlag : uint8_t
{
    sheltered,
    turnLeft,
    turnRight,
    turnBanked,
    turnSloped,
    dropDown,
    poweredLift,
    dropUp,
};
using RideTestingFlags = FlagHolder<uint32_t, RideTestingFlag>;

struct RideStation
{
    static constexpr uint8_t kNoTrain = std::numeric_limits<uint8_t>::max();

    CoordsXY Start;
    uint8_t Height{};
    uint8_t Length{};
    uint8_t Depart{};
    uint8_t TrainAtStation{ kNoTrain };
    TileCoordsXYZD Entrance;
    TileCoordsXYZD Exit;
    int32_t SegmentLength{}; // Length of track between this station and the next.
    uint16_t SegmentTime{};  // Time for train to reach the next station from this station.
    uint8_t QueueTime{};
    uint16_t QueueLength{};
    EntityId LastPeepInQueue{ EntityId::GetNull() };

    int32_t GetBaseZ() const;
    void SetBaseZ(int32_t newZ);
    CoordsXYZ GetStart() const;
};

struct RideMeasurement
{
    static constexpr size_t kMaxItems = 4800;

    RideMeasurementFlags flags{};
    uint32_t last_use_tick{};
    uint16_t num_items{};
    uint16_t current_item{};
    uint8_t vehicle_index{};
    StationIndex current_station{};
    int8_t vertical[kMaxItems]{};
    int8_t lateral[kMaxItems]{};
    uint8_t velocity[kMaxItems]{};
    uint8_t altitude[kMaxItems]{};
};

enum class RideClassification
{
    ride,
    shopOrStall,
    kioskOrFacility
};

enum class MechanicStatus : uint8_t
{
    undefined,
    calling,
    heading,
    fixing,
    hasFixedStationBrakes
};

namespace OpenRCT2::ShelteredSectionsBits
{
    constexpr uint8_t kNumShelteredSectionsMask = 0b00011111;
    constexpr uint8_t kRotatingWhileSheltered = 0b00100000;
    constexpr uint8_t kBankingWhileSheltered = 0b01000000;
} // namespace OpenRCT2::ShelteredSectionsBits

struct TrackDesign;
struct TrackDesignState;
enum class RideMode : uint8_t;
enum class RideStatus : uint8_t;

enum class SpecialElement : uint8_t
{
    spinningTunnel = 5,
    splash = 5,
    rapids = 5,
    reverser = 6,
    waterfall = 6,
    whirlpool = 7,
};
using SpecialElements = FlagHolder<uint8_t, SpecialElement>;

/**
 * Ride structure.
 *
 * This is based on RCT2's ride structure.
 * Padding and no longer used fields have been removed.
 */
struct Ride
{
    RideId id{ RideId::GetNull() };
    ride_type_t type{ kRideTypeNull };
    // pointer to static info. for example, wild mouse type is 0x36, subtype is
    // 0x4c.
    OpenRCT2::ObjectEntryIndex subtype{ OpenRCT2::kObjectEntryIndexNull };
    RideMode mode{};
    VehicleColourSettings vehicleColourSettings{};
    VehicleColour vehicleColours[OpenRCT2::Limits::kMaxVehicleColours]{};
    // 0 = closed, 1 = open, 2 = test
    RideStatus status{};
    std::string customName;
    uint16_t defaultNameNumber{};
    CoordsXY overallView;
    EntityId vehicles[OpenRCT2::Limits::kMaxTrainsPerRide + 1]{}; // Points to the first car in the train
    uint8_t departFlags{};
    uint8_t numStations{};
    uint8_t numTrains{};
    uint8_t numCarsPerTrain{};
    uint8_t proposedNumTrains{};
    uint8_t proposedNumCarsPerTrain{};
    uint8_t maxTrains{};
    uint8_t minCarsPerTrain{};
    uint8_t maxCarsPerTrain{};
    uint8_t minWaitingTime{};
    uint8_t maxWaitingTime{};
    union
    {
        uint8_t operationOption;
        uint8_t timeLimit;
        uint8_t numLaps;
        uint8_t launchSpeed;
        uint8_t speed;
        uint8_t rotations{};
    };

    uint8_t boatHireReturnDirection{};
    TileCoordsXY boatHireReturnPosition;
    uint8_t numHelices{};
    SpecialElements specialTrackElements{};
    // Use ToHumanReadableSpeed if converting to display
    int32_t maxSpeed{};
    int32_t averageSpeed{};
    uint8_t currentTestSegment{};
    uint8_t averageSpeedTestTimeout{};
    fixed16_2dp maxPositiveVerticalG{};
    fixed16_2dp maxNegativeVerticalG{};
    fixed16_2dp maxLateralG{};
    fixed16_2dp previousVerticalG{};
    fixed16_2dp previousLateralG{};
    RideTestingFlags testingFlags{};
    // x y z map location of the current track piece during a test
    // this is to prevent counting special tracks multiple times
    TileCoordsXYZ curTestTrackLocation;
    // Next 3 variables are related (XXXX XYYY ZZZa aaaa)
    uint16_t turnCountDefault{}; // X = current turn count
    uint16_t turnCountBanked{};
    uint16_t turnCountSloped{}; // X = number turns > 3 elements
    // Y is number of powered lifts, X is drops
    uint8_t numDrops{};
    uint8_t numPoweredLifts{};
    uint8_t startDropHeight{};
    uint8_t highestDropHeight{};
    int32_t shelteredLength{};
    // Unused always 0? Should affect nausea
    uint16_t var11C{};
    uint8_t numShelteredSections{}; // (?abY YYYY)
    // Customer counter in the current 960 game tick (about 30 seconds) interval
    uint16_t curNumCustomers{};
    // Counts ticks to update customer intervals, resets each 960 game ticks.
    uint16_t numCustomersTimeout{};
    // Customer count in the last 10 * 960 game ticks (sliding window)
    uint16_t numCustomers[OpenRCT2::Limits::kCustomerHistorySize]{};
    money64 price[OpenRCT2::RCT2::ObjectLimits::kMaxShopItemsPerRideEntry]{};
    TileCoordsXYZ chairliftBullwheelLocation[2];
    OpenRCT2::RideRating::Tuple ratings{};
    money64 value{};
    uint16_t chairliftBullwheelRotation{};
    uint8_t satisfaction{};
    uint8_t satisfactionTimeout{};
    uint8_t satisfactionNext{};
    // Various flags stating whether a ride window needs to be refreshed
    RideInvalidateFlags windowInvalidateFlags{};
    uint32_t totalCustomers{};
    money64 totalProfit{};
    uint8_t popularity{};
    uint8_t popularityTimeout{}; // Updated every purchase and ?possibly by time?
    uint8_t popularityNext{};    // When timeout reached this will be the next popularity
    uint16_t numRiders{};
    uint8_t musicTuneId{};
    uint8_t slideInUse{};
    union
    {
        EntityId slidePeep;
        uint16_t mazeTiles{};
    };
    OpenRCT2::Drawing::Colour slidePeepTShirtColour{};
    uint8_t spiralSlideProgress{};
    int32_t buildDate{};
    money64 upkeepCost{};
    EntityId raceWinner{};
    uint32_t musicPosition{};
    uint8_t breakdownReasonPending{};
    MechanicStatus mechanicStatus{};
    EntityId mechanic{ EntityId::GetNull() };
    StationIndex inspectionStation{ StationIndex::GetNull() };
    uint8_t brokenTrain{};
    uint8_t brokenCar{};
    uint8_t breakdownReason{};
    union
    {
        struct
        {
            uint8_t reliabilitySubvalue;   // 0 - 255, acts like the decimals for reliabilityPercentage
            uint8_t reliabilityPercentage; // Starts at 100 and decreases from there.
        };
        uint16_t reliability{};
    };
    // Small constant used to increase the unreliability as the game continues,
    // making breakdowns more and more likely.
    uint8_t unreliabilityFactor{};
    // Range from [0, 100]
    uint8_t downtime{};
    RideInspection inspectionInterval{};
    uint8_t lastInspection{};
    uint8_t downtimeHistory[OpenRCT2::Limits::kDowntimeHistorySize]{};
    uint32_t numPrimaryItemsSold{};
    uint32_t numSecondaryItemsSold{};
    // Used to oscillate the sound when ride breaks down.
    // 0 = no change, 255 = max change
    uint8_t breakdownSoundModifier{};
    uint8_t notFixedTimeout{};
    uint8_t lastCrashType{};
    uint8_t connectedMessageThrottle{};
    money64 incomePerHour{};
    money64 profit{};
    TrackColour trackColours[kNumRideColourSchemes]{};
    OpenRCT2::ObjectEntryIndex music{ OpenRCT2::kObjectEntryIndexNull };
    OpenRCT2::ObjectEntryIndex entranceStyle{ OpenRCT2::kObjectEntryIndexNull };
    uint16_t vehicleChangeTimeout{};
    uint8_t numBlockBrakes{};
    uint8_t liftHillSpeed{};
    uint32_t guestsFavourite{};
    uint32_t lifecycleFlags{};
    uint16_t totalAirTime{};
    StationIndex currentTestStation{ StationIndex::GetNull() };
    uint8_t numCircuits{};
    CoordsXYZ cableLiftLoc{};
    EntityId cableLift{ EntityId::GetNull() };

    // These two fields are used to warn users about issues.
    // Such issue can be hacked rides with incompatible options set.
    // They don't require export/import.
    uint8_t currentIssues{};
    uint32_t lastIssueTime{};

private:
    std::array<RideStation, OpenRCT2::Limits::kMaxStationsPerRide> stations{};

public:
    uint8_t numInversions{};
    uint8_t numHoles{};
    uint8_t shelteredEighths{};

    std::unique_ptr<RideMeasurement> measurement;

public:
    RideStation& getStation(StationIndex stationIndex = StationIndex::FromUnderlying(0));
    const RideStation& getStation(StationIndex stationIndex = StationIndex::FromUnderlying(0)) const;
    std::span<RideStation> getStations();
    std::span<const RideStation> getStations() const;
    StationIndex getStationIndex(const RideStation* station) const;

    // Returns the logical station number from the given station. Index 0 = station 1, index 1 = station 2. It accounts for gaps
    // in the station array. e.g. if only slot 0 and 2 are in use, index 2 returns 2 instead of 3.
    StationIndex::UnderlyingType getStationNumber(StationIndex in) const;

    void chainQueues() const;

private:
    void update();
    void updateQueueLength(StationIndex stationIndex);
    ResultWithMessage createVehicles(const CoordsXYE& element, bool isApplying, bool isSimulating);
    void moveTrainsToBlockBrakes(const CoordsXYZ& firstBlockPosition, OpenRCT2::TrackElement& firstBlock);
    money64 calculateIncomePerHour() const;
    void constructMissingEntranceOrExit() const;

    ResultWithMessage changeStatusDoStationChecks(StationIndex& stationIndex);
    ResultWithMessage changeStatusGetStartElement(StationIndex stationIndex, CoordsXYE& trackElement);
    ResultWithMessage changeStatusCheckCompleteCircuit(const CoordsXYE& trackElement);
    ResultWithMessage changeStatusCheckTrackValidity(const CoordsXYE& trackElement, bool isSimulating);
    ResultWithMessage changeStatusCreateVehicles(bool isApplying, const CoordsXYE& trackElement, bool isSimulating);

public:
    bool canBreakDown() const;
    RideClassification getClassification() const;
    bool isRide() const;
    void renew();
    void remove();
    void crash(uint8_t vehicleIndex);
    void setRideEntry(OpenRCT2::ObjectEntryIndex entryIndex);

    void setNumTrains(int32_t newNumTrains);
    void setNumCarsPerTrain(int32_t numCarsPerVehicle);
    void setReversedTrains(bool reversedTrains);
    void updateMaxVehicles();
    void updateNumberOfCircuits();

    bool hasSpinningTunnel() const;
    bool hasWaterSplash() const;
    bool hasRapids() const;
    bool hasLogReverser() const;
    bool hasWaterfall() const;
    bool hasWhirlpool() const;

    bool isPoweredLaunched() const;
    bool isBlockSectioned() const;
    bool canHaveMultipleCircuits() const;
    bool supportsStatus(RideStatus s) const;

    void stopGuestsQueuing();
    void validateStations();

    ResultWithMessage open(bool isApplying);
    ResultWithMessage test(bool isApplying);
    ResultWithMessage simulate(bool isApplying);

    RideMode getDefaultMode() const;

    void setColourPreset(uint8_t trackColourPreset, uint8_t vehicleColourPreset);

    const RideObjectEntry* getRideEntry() const;

    size_t getNumPrices() const;
    int32_t getAge() const;
    int32_t getTotalQueueLength() const;
    int32_t getMaxQueueTime() const;

    void queueInsertGuestAtFront(StationIndex stationIndex, Guest* peep);
    Guest* getQueueHeadGuest(StationIndex stationIndex) const;

    void setNameToDefault();
    std::string getName() const;
    void formatNameTo(OpenRCT2::Formatter&) const;
    void formatStatusTo(OpenRCT2::Formatter&) const;

    static void updateAll();
    static bool nameExists(std::string_view name, RideId excludeRideId = RideId::GetNull());

    [[nodiscard]] std::unique_ptr<TrackDesign> saveToTrackDesign(TrackDesignState& tds) const;

    uint64_t getAvailableModes() const;
    const RideTypeDescriptor& getRideTypeDescriptor() const;
    RideNaming getTypeNaming() const;
    OpenRCT2::TrackElement* getOriginElement(StationIndex stationIndex) const;

    std::pair<RideMeasurement*, OpenRCT2String> getMeasurement();

    uint8_t getNumShelteredSections() const;
    void increaseNumShelteredSections();

    void removeVehicles();
    /**
     * Updates all pieces of the ride to match the internal ride type. (Track pieces can have different ride types from the ride
     * they belong to, to enable “merging”.)
     */
    void updateRideTypeForAllPieces();

    void updateSatisfaction(uint8_t happiness);
    void updatePopularity(uint8_t pop_amount);
    void removePeeps();

    int32_t getTotalLength() const;
    int32_t getTotalTime() const;

    const OpenRCT2::StationObject* getStationObject() const;
    const OpenRCT2::MusicObject* getMusicObject() const;

    bool hasLifecycleFlag(uint32_t flag) const;
    void setLifecycleFlag(uint32_t flag, bool on);

    bool hasRecolourableShopItems() const;
    bool hasStation() const;

    bool findTrackGap(const CoordsXYE& input, CoordsXYE* output) const;

    // TO-DO: those friend functions are temporary, find a way to not access the private fields
    friend void updateSpiralSlide(Ride& ride);
    friend void updateChairlift(Ride& ride);
};
void updateSpiralSlide(Ride& ride);
void updateChairlift(Ride& ride);

#pragma pack(push, 1)

struct TrackBeginEnd
{
    int32_t begin_x;
    int32_t begin_y;
    int32_t begin_z;
    int32_t begin_direction;
    OpenRCT2::TileElement* begin_element;
    int32_t end_x;
    int32_t end_y;
    int32_t end_direction;
    OpenRCT2::TileElement* end_element;
};
#ifdef PLATFORM_32BIT
static_assert(sizeof(TrackBeginEnd) == 36);
#endif

#pragma pack(pop)

// Constants used by the lifecycleFlags property at 0x1D0
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
    RIDE_LIFECYCLE_REVERSED_TRAINS = 1 << 22,
};

// Constants used by the ride_type->flags property at 0x008
enum
{
    RIDE_ENTRY_FLAG_VEHICLE_TAB_SCALE_HALF = 1 << 0,
    RIDE_ENTRY_FLAG_NO_INVERSIONS = 1 << 1,
    RIDE_ENTRY_FLAG_NO_BANKED_TRACK = 1 << 2,
    RIDE_ENTRY_FLAG_PLAY_DEPART_SOUND = 1 << 3,
    RIDE_ENTRY_FLAG_INVERTER_SHIP_SWING_MODE = 1 << 4,
    RIDE_ENTRY_FLAG_TWIST_ROTATION_TYPE = 1 << 5,
    RIDE_ENTRY_FLAG_ENTERPRISE_ROTATION_TYPE = 1 << 6,
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
    // Must be set with Inverter Ship swing mode as well.
    RIDE_ENTRY_FLAG_MAGIC_CARPET_SWING_MODE = 1 << 20,
    RIDE_ENTRY_FLAG_RIDER_CONTROLS_SPEED = 1 << 21,
    RIDE_ENTRY_FLAG_HIDE_EMPTY_TRAINS = 1 << 22,
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
    RIDE_TYPE_CLASSIC_STAND_UP_ROLLER_COASTER,
    RIDE_TYPE_LSM_LAUNCHED_ROLLER_COASTER,
    RIDE_TYPE_CLASSIC_WOODEN_TWISTER_ROLLER_COASTER,

    RIDE_TYPE_COUNT
};

enum class RideStatus : uint8_t
{
    closed,
    open,
    testing,
    simulating,
    count,
};

enum class RideMode : uint8_t
{
    normal,
    continuousCircuit,
    reverseInclineLaunchedShuttle,
    poweredLaunchPasstrough, // RCT2 style, pass through station
    shuttle,
    boatHire,
    upwardLaunch,
    rotatingLift,
    stationToStation,
    singleRidePerAdmission,
    unlimitedRidesPerAdmission = 10,
    maze,
    race,
    dodgems,
    swing,
    shopStall,
    rotation,
    forwardRotation,
    backwardRotation,
    filmAvengingAviators,
    mouseTails3DFilm = 20,
    spaceRings,
    beginners,
    limPoweredLaunch,
    filmThrillRiders,
    stormChasers3DFilm,
    spaceRaiders3DFilm,
    intense,
    berserk,
    hauntedHouse,
    circus = 30,
    downwardLaunch,
    crookedHouse,
    freefallDrop,
    continuousCircuitBlockSectioned,
    poweredLaunch, // RCT1 style, don't pass through station
    poweredLaunchBlockSectioned,

    count,
    nullMode = 255,
};

RideMode& operator++(RideMode& d, int);

enum class RideCategory : uint8_t
{
    transport,
    gentle,
    rollerCoaster,
    thrill,
    water,
    shop,

    none = 255,
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
    RIDE_CRASH_TYPE_NONE = 0,
    RIDE_CRASH_TYPE_NO_FATALITIES = 2,
    RIDE_CRASH_TYPE_FATALITIES = 8
};

enum MazeWallType : uint8_t
{
    brick,
    hedges,
    ice,
    wooden,
};

enum
{
    RIDE_ISSUE_NONE = 0,
    RIDE_ISSUE_GUESTS_STUCK = (1 << 0),
};

constexpr uint8_t kMaxRideMeasurements = 8;
constexpr money64 kRideValueUndefined = kMoney64Undefined;
constexpr uint16_t kRideInitialReliability = ((100 << 8) | 0xFF); // Upper byte is percentage, lower byte is "decimal".

constexpr uint8_t kStationDepartFlag = (1 << 7);
constexpr uint8_t kStationDepartMask = static_cast<uint8_t>(~kStationDepartFlag);

constexpr uint16_t kCurrentTurnCountMask = 0xF800;
constexpr uint16_t kTurnMask1Element = 0x001F;
constexpr uint16_t kTurnMask2Elements = 0x00E0;
constexpr uint16_t kTurnMask3Elements = 0x0700;
constexpr uint16_t kTurnMask4PlusElements = 0xF800;

Ride* GetRide(RideId index);

RideId GetNextFreeRideId();
Ride* RideAllocateAtIndex(RideId index);
Ride& RideGetTemporaryForPreview();
void RideDelete(RideId id);

const RideObjectEntry* GetRideEntryByIndex(OpenRCT2::ObjectEntryIndex index);
std::string_view GetRideEntryName(OpenRCT2::ObjectEntryIndex index);

int32_t RideGetCount();
void RideInitAll();
void ResetAllRideBuildDates();
void RideUpdateFavouritedStat();
void RideCheckAllReachable();

bool RideTryGetOriginElement(const Ride& ride, CoordsXYE* output);
void RideClearBlockedTiles(const Ride& ride);
Staff* RideGetMechanic(const Ride& ride);
Staff* RideGetAssignedMechanic(const Ride& ride);
VehicleColour RideGetVehicleColour(const Ride& ride, int32_t vehicleIndex);
int32_t RideGetUnusedPresetVehicleColour(OpenRCT2::ObjectEntryIndex subType);
void RideSetVehicleColoursToRandomPreset(Ride& ride, uint8_t preset_index);
void RideMeasurementsUpdate();
void RideBreakdownAddNewsItem(const Ride& ride);
Staff* RideFindClosestMechanic(const Ride& ride, int32_t forInspection);
int32_t RideInitialiseConstructionWindow(Ride& ride);
void RideSetMapTooltip(const OpenRCT2::TileElement& tileElement);
void RidePrepareBreakdown(Ride& ride, int32_t breakdownReason);
OpenRCT2::TileElement* RideGetStationStartTrackElement(const Ride& ride, StationIndex stationIndex);
OpenRCT2::TileElement* RideGetStationExitElement(const CoordsXYZ& elementPos);
money64 RideGetRefundPrice(const Ride& ride);
int32_t RideGetRandomColourPresetIndex(ride_type_t rideType);
money64 RideGetCommonPrice(const Ride& forRide);

void RideClearForConstruction(Ride& ride);
void InvalidateTestResults(Ride& ride);

void IncrementTurnCount1Element(Ride& ride, uint8_t type);
void IncrementTurnCount2Elements(Ride& ride, uint8_t type);
void IncrementTurnCount3Elements(Ride& ride, uint8_t type);
void IncrementTurnCount4PlusElements(Ride& ride, uint8_t type);
int32_t GetTurnCount1Element(const Ride& ride, uint8_t type);
int32_t GetTurnCount2Elements(const Ride& ride, uint8_t type);
int32_t GetTurnCount3Elements(const Ride& ride, uint8_t type);
int32_t GetTurnCount4PlusElements(const Ride& ride, uint8_t type);

bool RideHasAnyTrackElements(const Ride& ride);

bool TrackBlockGetNext(CoordsXYE* input, CoordsXYE* output, int32_t* z, int32_t* direction);
bool TrackBlockGetNextFromZero(
    const CoordsXYZ& startPos, const Ride& ride, uint8_t direction_start, CoordsXYE* output, int32_t* z, int32_t* direction,
    bool isGhost);

bool TrackBlockGetPrevious(const CoordsXYE& trackPos, TrackBeginEnd* outTrackBeginEnd);
bool TrackBlockGetPreviousFromZero(
    const CoordsXYZ& startPos, const Ride& ride, uint8_t direction, TrackBeginEnd* outTrackBeginEnd);

void RideGetStartOfTrack(CoordsXYE* output);

money64 RideEntranceExitPlaceGhost(
    const Ride& ride, const CoordsXY& entranceExitCoords, Direction direction, int32_t placeType, StationIndex stationNum);

ResultWithMessage RideAreAllPossibleEntrancesAndExitsBuilt(const Ride& ride);
void RideFixBreakdown(Ride& ride, int32_t reliabilityIncreaseFactor);

void BlockBrakeSetLinkedBrakesClosed(const CoordsXYZ& vehicleTrackLocation, OpenRCT2::TrackElement& tileElement, bool isOpen);

uint8_t RideEntryGetVehicleAtPosition(int32_t rideEntryIndex, int32_t numCarsPerTrain, int32_t position);
void RideUpdateVehicleColours(const Ride& ride);

OpenRCT2::BitSet<EnumValue(TrackGroup::count)> RideEntryGetSupportedTrackPieces(const RideObjectEntry& rideEntry);

uint32_t RideCustomersPerHour(const Ride& ride);
uint32_t RideCustomersInLast5Minutes(const Ride& ride);

Vehicle* RideGetBrokenVehicle(const Ride& ride);

money64 RideGetPrice(const Ride& ride);

OpenRCT2::TileElement* GetStationPlatform(const CoordsXYRangedZ& coords);
bool RideHasAdjacentStation(const Ride& ride);
bool RideHasStationShelter(const Ride& ride);
bool RideHasRatings(const Ride& ride);

int32_t GetUnifiedBoosterSpeed(ride_type_t rideType, int32_t relativeSpeed);
void FixInvalidVehicleSpriteSizes();
bool RideEntryHasCategory(const RideObjectEntry& rideEntry, RideCategory category);

OpenRCT2::ObjectEntryIndex RideGetEntryIndex(ride_type_t rideType, OpenRCT2::ObjectEntryIndex rideSubType);

void DetermineRideEntranceAndExitLocations();
void RideClearLeftoverEntrances(const Ride& ride);

void SetBrakeClosedMultiTile(OpenRCT2::TrackElement& trackElement, const CoordsXY& trackLocation, bool isClosed);

std::vector<RideId> GetTracklessRides();

void CircusMusicUpdate(Ride& ride);
void DefaultMusicUpdate(Ride& ride);

RideMode RideModeGetBlockSectionedCounterpart(RideMode originalMode);
