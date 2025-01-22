/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#define TRACK_COLOUR_PRESETS(...)                                                                                              \
    {                                                                                                                          \
        static_cast<uint8_t>(std::size<TrackColour>({ __VA_ARGS__ })),                                                         \
        {                                                                                                                      \
            __VA_ARGS__                                                                                                        \
        }                                                                                                                      \
    }
#define kDefaultFlatRideColourPreset TRACK_COLOUR_PRESETS({ COLOUR_BRIGHT_RED, COLOUR_LIGHT_BLUE, COLOUR_YELLOW })
#define kDefaultStallColourPreset TRACK_COLOUR_PRESETS({ COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED })

#include "../audio/audio.h"
#include "../core/BitSet.hpp"
#include "../core/EnumUtils.hpp"
#include "../drawing/ImageIndexType.h"
#include "../entity/Guest.h"
#include "../localisation/StringIds.h"
#include "../paint/track/Support.h"
#include "Ride.h"
#include "RideAudio.h"
#include "RideConstruction.h"
#include "RideEntry.h"
#include "RideRatings.h"
#include "RideStringIds.h"
#include "ShopItem.h"
#include "Track.h"
#include "TrackStyle.h"
#include "Vehicle.h"

enum class ResearchCategory : uint8_t;

constexpr uint8_t kDefaultFoodStallHeight = 8 * kCoordsZStep;
constexpr uint8_t kDefaultDrinksStallHeight = 8 * kCoordsZStep;
constexpr uint8_t kDefaultShopHeight = 8 * kCoordsZStep;
constexpr uint8_t kDefaultToiletHeight = 4 * kCoordsZStep;
constexpr uint8_t kDefaultInformationKioskHeight = 6 * kCoordsZStep;
constexpr uint8_t kDefaultFirstAidHeight = 6 * kCoordsZStep;
constexpr uint8_t kDefaultCashMachineHeight = 8 * kCoordsZStep;

struct RideComponentName
{
    StringId singular;
    StringId plural;
    StringId capitalised;
    StringId capitalised_plural;
    StringId count;
    StringId count_plural;
    StringId number;
};

enum class RideComponentType
{
    Train,
    Boat,
    Track,
    DockingPlatform,
    Station,
    Car,
    Building,
    Structure,
    Ship,
    Cabin,
    Wheel,
    Ring,
    Player,
    Course,
    Count
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

enum class TrackDesignCreateMode : uint_fast8_t
{
    Default,
    Maze
};

enum class RatingsCalculationType : uint8_t
{
    Normal,
    FlatRide,
    Stall,
};

enum class RatingsModifierType : uint8_t
{
    NoModifier,
    // General Rating Bonuses
    BonusLength,
    BonusSynchronisation,
    BonusTrainLength,
    BonusMaxSpeed,
    BonusAverageSpeed,
    BonusDuration,
    BonusGForces,
    BonusTurns,
    BonusDrops,
    BonusSheltered,
    BonusProximity,
    BonusScenery,
    BonusRotations,
    BonusOperationOption,
    BonusReversedTrains,
    // Ride-specific Rating Bonuses
    BonusGoKartRace,
    BonusTowerRide,
    BonusRotoDrop,
    BonusMazeSize,
    BonusBoatHireNoCircuit,
    BonusSlideUnlimitedRides,
    BonusMotionSimulatorMode,
    Bonus3DCinemaMode,
    BonusTopSpinMode,
    // Number of reversals BONUS for reverser coaster
    BonusReversals,
    // Number of hole BONUS for mini golf
    BonusHoles,
    // Number of cars bonus for dodgems/flying saucers
    BonusNumTrains,
    // Bonus for launched freefall in downward launch mode
    BonusDownwardLaunch,
    // Bonus with further mode-dependent logic for LF
    BonusLaunchedFreefallSpecial,
    // General Rating Requirements
    RequirementLength,
    RequirementDropHeight,
    RequirementNumDrops,
    RequirementMaxSpeed,
    RequirementNegativeGs,
    RequirementLateralGs,
    RequirementInversions,
    RequirementUnsheltered,
    // Number of reversals REQUIREMENT for reverser coaster
    RequirementReversals,
    // Number of hole REQUIREMENT for mini golf
    RequirementHoles,
    // 2 Station requirement for Chairlift
    RequirementStations,
    // Water section requirement for Water Coaster
    RequirementSplashdown,
    PenaltyLateralGs,
};

struct RideNameConvention
{
    RideComponentType vehicle;
    RideComponentType structure;
    RideComponentType station;
};

struct RideBuildCost
{
    money64 TrackPrice; // Cost of a single straight piece of track
    money64 SupportPrice;
    uint8_t PriceEstimateMultiplier;
};

struct RideHeights
{
    uint8_t MaxHeight;
    uint8_t ClearanceHeight;
    int8_t VehicleZOffset;
    uint8_t PlatformHeight;
};

struct RideLiftData
{
    OpenRCT2::Audio::SoundId sound_id;
    uint8_t minimum_speed;
    uint8_t maximum_speed;
};

struct RideColourPreview
{
    uint32_t Track;
    uint32_t Supports;
};

struct RideOperatingSettings
{
    uint8_t MinValue;
    uint8_t MaxValue;
    uint8_t OperatingSettingMultiplier = 1; // Used for the Ride window, cosmetic only.
};

struct RideTrackSpeedSettings
{
    int8_t BrakesMaxSpeed = 30;
    int8_t BoosterMaxSpeed = 30;
};

struct RideBoosterSettings
{
    uint8_t PoweredLiftAcceleration = 0;
    uint8_t BoosterAcceleration = 0;
    uint8_t AccelerationFactor = 12; // the amount to right-shift the launch speed for powered launch from a station
};

struct RideLegacyBoosterSettings
// These values that must be kept for backwards compatibility. New ride types should set the acceleration values equal to
// BoosterSettings' and leave BoosterSpeedFactor at default.
{
    uint8_t PoweredLiftAcceleration = 0; // PoweredLiftAcceleration value before unified-speed update
    uint8_t BoosterAcceleration = 0;     // BoosterAcceleration value before unified-speed update
    int8_t BoosterSpeedFactor = 2; // Multiplier representing how much to multiply booster speed by, scaled to 2x the final
                                   // multiplier.
};

struct RatingsModifier
{
    RatingsModifierType type;
    int32_t threshold;
    int32_t excitement;
    int32_t intensity;
    int32_t nausea;
};

struct RideRatingsDescriptor
{
    RatingsCalculationType Type;
    RatingTuple BaseRatings;
    uint8_t Unreliability;
    // Used for rides with a set sheltered 8ths value (-1 = normal calculation)
    int8_t RideShelter;
    bool RelaxRequirementsIfInversions;
    RatingsModifier Modifiers[32];
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
    money64 BaseCost;
    /** rct2: 0x0097E3AC */
    uint8_t TrackLengthMultiplier;
    money64 CostPerTrackPiece;
    /** rct2: 0x0097E3B4 */
    money64 CostPerTrain;
    /** rct2: 0x0097E3B6 */
    money64 CostPerCar;
    /** rct2: 0x0097E3B8 */
    money64 CostPerStation;
};

using RideTrackGroups = OpenRCT2::BitSet<EnumValue(TrackGroup::count)>;
using UpdateRideApproachVehicleWaypointsFunction = void (*)(Guest&, const CoordsXY&, int16_t&);
using RideMusicUpdateFunction = void (*)(Ride&);
using PeepUpdateRideLeaveEntranceFunc = void (*)(Guest*, Ride&, CoordsXYZD&);
using StartRideMusicFunction = void (*)(const OpenRCT2::RideAudio::ViewportRideMusicInstance&);
using LightFXAddLightsMagicVehicleFunction = void (*)(const Vehicle* vehicle);
using RideLocationFunction = CoordsXY (*)(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation);
using RideUpdateFunction = void (*)(Ride& ride);
using RideUpdateMeasurementsSpecialElementsFunc = void (*)(Ride& ride, const OpenRCT2::TrackElemType trackType);
using MusicTrackOffsetLengthFunc = std::pair<size_t, size_t> (*)(const Ride& ride);
using SpecialElementRatingAdjustmentFunc = void (*)(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea);

using UpdateRotatingFunction = void (*)(Vehicle& vehicle);
enum class RideConstructionWindowContext : uint8_t
{
    Default,
    Maze,
};

struct TrackDrawerEntry
{
    TrackStyle trackStyle = TrackStyle::null;
    SupportType supportType{};
    /** rct2: 0x0097C468 (0 - 31) and 0x0097C5D4 (32 - 63) */
    RideTrackGroups enabledTrackGroups{};
    // Pieces that this ride type _can_ draw, but are disabled because their vehicles lack the relevant sprites,
    // or because they are not realistic for the ride type (e.g. LIM boosters in Mini Roller Coasters).
    RideTrackGroups extraTrackGroups{};

    ImageIndex icon = kSpriteIdNull;
    StringId tooltip = kStringIdNone;

    void GetAvailableTrackGroups(RideTrackGroups& res) const;
    bool SupportsTrackGroup(const TrackGroup trackGroup) const;
};

struct TrackDrawerDescriptor
{
    TrackDrawerEntry Regular{};
    TrackDrawerEntry Covered{};

    constexpr TrackDrawerDescriptor()
    {
    }

    constexpr TrackDrawerDescriptor(TrackDrawerEntry function)
        : Regular(function)
    {
    }

    constexpr TrackDrawerDescriptor(TrackDrawerEntry entryRegular, TrackDrawerEntry entryCovered)
        : Regular(entryRegular)
        , Covered(entryCovered)
    {
    }

    bool HasCoveredPieces() const;
};

enum class RtdFlag : uint8_t
{
    hasTrackColourMain,
    hasTrackColourAdditional,
    hasTrackColourSupports,

    // Set by flat rides, tower rides and shops/stalls.
    hasSinglePieceStation,

    hasLeaveWhenAnotherVehicleArrivesAtStation,
    canSynchroniseWithAdjacentStations,

    // Used only by boat Hire and submarine ride
    trackMustBeOnWater,

    hasGForces,

    // Used by rides that can't have gaps, like those with a vertical tower,
    // such as the observation tower.
    cannotHaveGaps,

    hasDataLogging,
    hasDrops,

    noTestMode,

    // Set on rides with two varieties, like the u and o shapes of the dinghy slide
    // and the dry and submerged track of the water coaster.
    hasCoveredPieces,

    // Used only by maze, spiral slide and shops
    noVehicles,

    hasLoadOptions,
    hasLsmBehaviourOnFlat,

    // Set by flat rides where the vehicle is integral to the structure, like
    // Merry-go-round and swinging ships. (Contrast with rides like dodgems.)
    vehicleIsIntegral,

    isShopOrFacility,

    // If set, wall scenery can not share a tile with the ride's track
    noWallsAroundTrack,

    isFlatRide,

    // Whether or not guests will go on the ride again if they liked it
    // (this is usually applied to everything apart from transport rides).
    guestsWillRideAgain,

    // Used by Toilets and First Aid to mark that guest should go
    // inside the building (rather than 'buying' at the counter)
    guestsShouldGoInsideFacility,

    // Guests are "IN" (ride) rather than "ON" (ride)
    describeAsInside,

    sellsFood,
    sellsDrinks,

    // Whether or not vehicle colours can be set
    hasVehicleColours,

    checkForStalling,
    hasTrack,

    // Only set by lift
    allowExtraTowerBases,

    // Only set by reverser coaster
    layeredVehiclePreview,

    supportsMultipleColourSchemes,
    allowDoorsOnTrack,
    hasMusicByDefault,
    allowMusic,

    // Used by the Flying RC, Lay-down RC, Multi-dimension RC
    hasInvertedVariant,

    checkGForces,
    hasEntranceAndExit,
    allowMoreVehiclesThanStationFits,
    hasAirTime,
    singleSession,
    allowMultipleCircuits,
    allowCableLiftHill,
    showInTrackDesigner,
    isTransportRide,
    interestingToLookAt,
    slightlyInterestingToLookAt,

    // This is only set on the Flying RC and its alternative type.
    startConstructionInverted,

    listVehiclesSeparately,
    supportsLevelCrossings,
    isSuspended,
    hasLandscapeDoors,
    upInclineRequiresLift,
    guestsCanUseUmbrella,
    hasOneStation,
    hasSeatRotation,
    allowReversedTrains,
};

/**
 * Some rides are so different from others that they need some special code.
 * This replaces direct ride type checks.
 *
 * Note: only add to this list if behaviour cannot sufficiently be altered using flags.
 */
enum class RtdSpecialType
{
    none,
    maze,
    miniGolf,
    spiralSlide,
    toilet,
    cashMachine,
    firstAid,
};

// Set on ride types that have a main colour, additional colour and support colour.
constexpr uint64_t kRtdFlagsHasThreeColours = EnumsToFlags(
    RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourAdditional, RtdFlag::hasTrackColourSupports);
// Set on _all_ roller coaster ride types, including the _ALT types used for constructing upside down.
constexpr uint64_t kRtdFlagsCommonCoaster = EnumsToFlags(
    RtdFlag::hasGForces, RtdFlag::hasDataLogging, RtdFlag::hasDrops, RtdFlag::hasLoadOptions, RtdFlag::guestsWillRideAgain,
    RtdFlag::hasVehicleColours, RtdFlag::checkForStalling, RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes,
    RtdFlag::allowMusic, RtdFlag::interestingToLookAt, RtdFlag::canSynchroniseWithAdjacentStations);
// Set on all roller coaster ride types, excluding the _ALT types used for constructing upside down.
constexpr uint64_t kRtdFlagsCommonCoasterNonAlt = EnumsToFlags(
    RtdFlag::showInTrackDesigner, RtdFlag::hasAirTime, RtdFlag::hasEntranceAndExit);

struct RideTypeDescriptor
{
    uint8_t Category{};
    /** rct2: 0x0097CC68 */
    OpenRCT2::TrackElemType StartTrackPiece{};
    TrackDrawerDescriptor TrackPaintFunctions{};
    TrackDrawerDescriptor InvertedTrackPaintFunctions{};
    uint64_t Flags{};
    /** rct2: 0x0097C8AC */
    uint64_t RideModes{};
    RideMode DefaultMode{};
    /** rct2: 0x0097CF40 */
    RideOperatingSettings OperatingSettings{};
    RideTrackSpeedSettings TrackSpeedSettings{};
    RideBoosterSettings BoosterSettings{};
    RideLegacyBoosterSettings LegacyBoosterSettings{};
    RideNaming Naming{};
    RideNameConvention NameConvention{};
    uint8_t AvailableBreakdowns{};
    /** rct2: 0x0097D218 */
    RideHeights Heights{};
    uint8_t MaxMass{};
    /** rct2: 0x0097D7C8, 0x0097D7C9, 0x0097D7CA */
    RideLiftData LiftData{};
    // rct2: 0x0097CD1E
    RatingTuple RatingsMultipliers{};
    UpkeepCostsDescriptor UpkeepCosts{};
    // rct2: 0x0097DD78
    RideBuildCost BuildCosts{};
    money64 DefaultPrices[OpenRCT2::RCT2::ObjectLimits::kMaxShopItemsPerRideEntry]{};
    std::string_view DefaultMusic{};
    /** rct2: 0x0097D7CB */
    ShopItemIndex PhotoItem{};
    /** rct2: 0x0097D21E */
    uint8_t BonusValue{};
    TrackColourPresetList ColourPresets{};
    RideColourPreview ColourPreview{};
    RideColourKey ColourKey{};

    // json name lookup
    std::string_view Name{};

    RideRatingsDescriptor RatingsData{};

    UpdateRotatingFunction UpdateRotating = UpdateRotatingDefault;

    LightFXAddLightsMagicVehicleFunction LightFXAddLightsMagicVehicle = nullptr;
    StartRideMusicFunction StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel;

    TrackDesignCreateMode DesignCreateMode = TrackDesignCreateMode::Default;

    RideMusicUpdateFunction MusicUpdateFunction = DefaultMusicUpdate;
    RideClassification Classification = RideClassification::Ride;

    PeepUpdateRideLeaveEntranceFunc UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault;
    SpecialElementRatingAdjustmentFunc SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default;

    RideLocationFunction GetGuestWaypointLocation = GetGuestWaypointLocationDefault;

    RideConstructionWindowContext ConstructionWindowContext = RideConstructionWindowContext::Default;
    RideUpdateFunction RideUpdate = nullptr;

    RideUpdateMeasurementsSpecialElementsFunc UpdateMeasurementsSpecialElements = RideUpdateMeasurementsSpecialElements_Default;

    MusicTrackOffsetLengthFunc MusicTrackOffsetLength = OpenRCT2::RideAudio::RideMusicGetTrackOffsetLength_Default;

    UpdateRideApproachVehicleWaypointsFunction UpdateRideApproachVehicleWaypoints = UpdateRideApproachVehicleWaypointsDefault;
    RtdSpecialType specialType = RtdSpecialType::none;

    bool HasFlag(RtdFlag flag) const;
    /** @deprecated */
    bool SupportsTrackGroup(const TrackGroup trackGroup) const;
    ResearchCategory GetResearchCategory() const;
    bool SupportsRideMode(RideMode rideMode) const;
    /**
     * Converts booster speed from the ride type's speed regime (Junior, Default, Giga) to to the unified values used by the
     * vehicle. See https://github.com/OpenRCT2/OpenRCT2/discussions/23119 for more information about unified speed.
     */
    int32_t GetUnifiedBoosterSpeed(int32_t relativeSpeed) const;
};

extern const RideTypeDescriptor kRideTypeDescriptors[RIDE_TYPE_COUNT];

// clang-format off
constexpr RideComponentName kRideComponentNames[] =
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

constexpr std::string_view kMusicObjectDodgems = "rct2.music.dodgems";
constexpr std::string_view kMusicObjectEgyptian = "rct2.music.egyptian";
constexpr std::string_view kMusicObjectFairground = "rct2.music.fairground";
constexpr std::string_view kMusicObjectGentle = "rct2.music.gentle";
constexpr std::string_view kMusicObjectHorror = "rct2.music.horror";
constexpr std::string_view kMusicObjectPirate = "rct2.music.pirate";
constexpr std::string_view kMusicObjectRock1 = "rct2.music.rock1";
constexpr std::string_view kMusicObjectRock2 = "rct2.music.rock2";
constexpr std::string_view kMusicObjectRock3 = "rct2.music.rock3";
constexpr std::string_view kMusicObjectSummer = "rct2.music.summer";
constexpr std::string_view kMusicObjectTechno = "rct2.music.techno";
constexpr std::string_view kMusicObjectWater = "rct2.music.water";
constexpr std::string_view kMusicObjectWildWest = "rct2.music.wildwest";
constexpr std::string_view kMusicObjectModern = "rct2.music.modern";

constexpr const RideComponentName& GetRideComponentName(const RideComponentType type)
{
    return kRideComponentNames[EnumValue(type)];
}

constexpr uint64_t kAllRideModesAvailable = EnumsToFlags(
    RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned, RideMode::ReverseInclineLaunchedShuttle,
    RideMode::PoweredLaunchPasstrough, RideMode::Shuttle, RideMode::Normal, RideMode::BoatHire, RideMode::UpwardLaunch,
    RideMode::RotatingLift, RideMode::StationToStation, RideMode::SingleRidePerAdmission, RideMode::UnlimitedRidesPerAdmission,
    RideMode::Maze, RideMode::Race, RideMode::Dodgems, RideMode::Swing, RideMode::ShopStall, RideMode::Rotation,
    RideMode::ForwardRotation, RideMode::BackwardRotation, RideMode::FilmAvengingAviators, RideMode::MouseTails3DFilm,
    RideMode::SpaceRings, RideMode::Beginners, RideMode::LimPoweredLaunch, RideMode::FilmThrillRiders,
    RideMode::StormChasers3DFilm, RideMode::SpaceRaiders3DFilm, RideMode::Intense, RideMode::Berserk, RideMode::HauntedHouse,
    RideMode::Circus, RideMode::DownwardLaunch, RideMode::CrookedHouse, RideMode::FreefallDrop, RideMode::PoweredLaunch,
    RideMode::PoweredLaunchBlockSectioned);

extern const CarEntry kCableLiftVehicle;

extern const uint16_t kRideFilmLength[3];

extern const StringId kRideModeNames[EnumValue(RideMode::Count)];

// clang-format off
constexpr RideTypeDescriptor kDummyRTD =
{
    .Category = RIDE_CATEGORY_NONE,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = {},
    .InvertedTrackPaintFunctions = {},
    .Flags = 0,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = {},
    .TrackSpeedSettings = {},
    .BoosterSettings = {},
    .LegacyBoosterSettings = {},
    .Naming = { STR_UNKNOWN_RIDE, STR_RIDE_DESCRIPTION_UNKNOWN },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = 0,
    .Heights = { 12, 64, 0, 0, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 0, 0, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 0.00_GBP, 0.00_GBP, 1 },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 0,
    .ColourPresets = kDefaultFlatRideColourPreset,
    .ColourPreview = { kSpriteIdNull, kSpriteIdNull },
    .ColourKey = RideColourKey::Ride,
    .Name = "invalid",
	.RatingsData =
    {
        RatingsCalculationType::FlatRide,
        { MakeRideRating(1, 00), MakeRideRating(1, 00), MakeRideRating(1, 00) },
        1,
        -1,
        false,
        {
            { RatingsModifierType::NoModifier, 0, 0, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
};
// clang-format on

constexpr const RideTypeDescriptor& GetRideTypeDescriptor(ride_type_t rideType)
{
    if (rideType >= std::size(kRideTypeDescriptors))
        return kDummyRTD;

    return kRideTypeDescriptors[rideType];
}

constexpr bool RideTypeIsValid(ride_type_t rideType)
{
    return rideType < std::size(kRideTypeDescriptors);
}

bool IsTrackEnabled(TrackGroup trackGroup);
void UpdateEnabledRideGroups(TrackDrawerDescriptor trackDrawerDescriptor);
void UpdateDisabledRideGroups(const RideTrackGroups& res);

TrackDrawerDescriptor getTrackDrawerDescriptor(const RideTypeDescriptor& rtd, bool isInverted);
TrackDrawerEntry getTrackDrawerEntry(const RideTypeDescriptor& rtd, bool isInverted = false, bool isCovered = false);
