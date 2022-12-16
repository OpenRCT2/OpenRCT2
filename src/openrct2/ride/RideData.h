/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#define DEFAULT_FLAT_RIDE_COLOUR_PRESET TRACK_COLOUR_PRESETS({ COLOUR_BRIGHT_RED, COLOUR_LIGHT_BLUE, COLOUR_YELLOW })
#define DEFAULT_STALL_COLOUR_PRESET TRACK_COLOUR_PRESETS({ COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED })

#include "../audio/audio.h"
#include "../common.h"
#include "../core/BitSet.hpp"
#include "../entity/Guest.h"
#include "../localisation/StringIds.h"
#include "../sprites.h"
#include "../util/Util.h"
#include "Ride.h"
#include "RideAudio.h"
#include "RideConstruction.h"
#include "RideEntry.h"
#include "RideRatings.h"
#include "ShopItem.h"
#include "Track.h"
#include "TrackPaint.h"
#include "Vehicle.h"

#include <bitset>

enum class ResearchCategory : uint8_t;

using ride_ratings_calculation = void (*)(Ride* ride, RideRatingUpdateState& state);

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

struct rct_ride_lift_data
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
    uint8_t MaxBrakesSpeed;
    uint8_t PoweredLiftAcceleration;
    uint8_t BoosterAcceleration;
    int8_t BoosterSpeedFactor; // The factor to shift the raw booster speed with
    uint16_t AccelerationFactor = 12;
    uint8_t OperatingSettingMultiplier = 1; // Used for the Ride window, cosmetic only.
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

using RideTrackGroup = OpenRCT2::BitSet<TRACK_GROUP_COUNT>;
using UpdateRideApproachVehicleWaypointsFunction = void (*)(Guest&, const CoordsXY&, int16_t&);
using RideMusicUpdateFunction = void (*)(Ride*);
using PeepUpdateRideLeaveEntranceFunc = void (*)(Guest*, Ride*, CoordsXYZD&);
using StartRideMusicFunction = void (*)(const OpenRCT2::RideAudio::ViewportRideMusicInstance&);
using LightFXAddLightsMagicVehicleFunction = void (*)(const Vehicle* vehicle);
using RideLocationFunction = CoordsXY (*)(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation);
using RideUpdateFunction = void (*)(Ride& ride);
using RideUpdateMeasurementsSpecialElementsFunc = void (*)(Ride* ride, const track_type_t trackType);
using MusicTrackOffsetLengthFunc = std::pair<size_t, size_t> (*)(const Ride& ride);
using SpecialElementRatingAdjustmentFunc = void (*)(const Ride* ride, int32_t& excitement, int32_t& intensity, int32_t& nausea);

using UpdateRotatingFunction = void (*)(Vehicle& vehicle);
enum class RideConstructionWindowContext : uint8_t
{
    Default,
    Maze,
};

enum class RideTypeFlags : uint64_t
{
    HasTrackColourMain = 0,
    HasTrackColourAdditional,
    HasTrackColourSupports,
    HasSinglePieceStation, // Set by flat rides, tower rides and shops/stalls.
    HasLeaveWhenAnotherVehicleArrivesAtStation,
    CanSynchroniseAdjacentStations,
    TrackMustBeOnWater, // used only by boat Hire and submarine ride
    HasGForces,
    CannotHaveGaps, // used by rides that can't have gaps, like those with a vertical tower, such
                    // as the observation tower
    HasDataLogging,
    HasDrops,
    NoTestMode,
    TrackElementsHaveTwoVarieties, // used by rides with two varieties, like the u and o
                                   // shapes of the dinghy slide and the dry and submerged
                                   // track of the water coaster
    NoVehicles,                    // used only by maze, spiral slide and shops
    HasLoadOptions,
    HasNoTrack,
    VehicleIsIntegral, // Set by flat rides where the vehicle is integral to the structure, like
    // Merry-go-round and swinging ships. (Contrast with rides like dodgems.)
    IsShopOrFacility,
    TrackNoWalls, // if set, wall scenery can not share a tile with the ride's track
    FlatRide,
    PeepWillRideAgain, // whether or not guests will go on the ride again if they liked it
                       // (this is
    // usually applied to everything apart from transport rides)
    PeepShouldGoInsideFacility, // used by toilets and first aid to mark that peep should go
                                // inside the building (rather than 'buying' at the counter)
    InRide,                     // peeps are "IN" (ride) rather than "ON" (ride)
    SellsFood,
    SellsDrinks,
    IsToilet,
    HasVehicleColours, // whether or not vehicle colours can be set
    CheckForStalling,
    HasTrack,
    AllowExtraTowerBases, // Only set by lift
    HasLargeCurves,       // whether the ride supports large (45 degree turn) curves
    SupportsMultipleTrackColour,

    AllowDoorsOnTrack,
    MusicOnDefault,
    AllowMusic,
    HasAlternativeTrackType, // Used by the Flying RC, Lay-down RC, Multi-dimension RC
    PeepCheckGForces,
    HasEntranceExit,
    AllowMoreVehiclesThanStationFits,
    HasAirTime,
    SingleSession,
    AllowMultipleCircuits,
    AllowCableLiftHill,
    ShowInTrackDesigner,
    TransportRide,
    InterestingToLookAt,
    SlightlyInterestingToLookAt,
    StartConstructionInverted, // This is only set on the Flying RC and its alternative type.

    ListVehiclesSeparately,
    SupportsLevelCrossings,
    IsSuspended,
    HasLandscapeDoors,
    UpInclineRequiresLift,
    PeepCanUseUmbrella,
    IsCashMachine,
    HasOneStation,
    HasSeatRotation,
    IsFirstAid,
    IsMaze,
    IsSpiralSlide,

    Count,
};

class RideTypeFlagsBitSet : public OpenRCT2::BitSet<static_cast<size_t>(RideTypeFlags::Count)>
{
public:
    constexpr RideTypeFlagsBitSet() = default;
    constexpr RideTypeFlagsBitSet(const std::initializer_list<RideTypeFlags> flags)
    {
        for (auto flag : flags)
        {
            set(static_cast<size_t>(flag), true);
        }
    }

    constexpr RideTypeFlagsBitSet(const std::initializer_list<RideTypeFlagsBitSet>& bitsets)
    {
        for (size_t i = 0; i < size(); i++)
        {
            for (const auto& bitset : bitsets)
            {
                const auto val = bitset.get(i);
                if (val)
                {
                    set(i, true);
                    break;
                }
            }
        }
    }
};

struct RideTypeDescriptor
{
    uint8_t AlternateType;
    uint8_t Category;
    /** rct2: 0x0097C468 (0 - 31) and 0x0097C5D4 (32 - 63) */
    RideTrackGroup EnabledTrackPieces;
    // Pieces that this ride type _can_ draw, but are disabled because their vehicles lack the relevant sprites,
    // or because they are not realistic for the ride type (e.g. LIM boosters in Mini Roller Coasters).
    RideTrackGroup ExtraTrackPieces;
    RideTrackGroup CoveredTrackPieces;
    /** rct2: 0x0097CC68 */
    track_type_t StartTrackPiece;
    TRACK_PAINT_FUNCTION_GETTER TrackPaintFunction;
    RideTypeFlagsBitSet Flags;
    /** rct2: 0x0097C8AC */
    uint64_t RideModes;
    RideMode DefaultMode;
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
    money16 DefaultPrices[RCT2::ObjectLimits::MaxShopItemsPerRideEntry];
    std::string_view DefaultMusic;
    /** rct2: 0x0097D7CB */
    ShopItemIndex PhotoItem;
    /** rct2: 0x0097D21E */
    uint8_t BonusValue;
    track_colour_preset_list ColourPresets;
    RideColourPreview ColourPreview;
    RideColourKey ColourKey;

    // json name lookup
    std::string_view Name;

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

    bool HasFlag(RideTypeFlags flag) const;
    bool HasFlag(std::vector<RideTypeFlags> flags) const;
    void GetAvailableTrackPieces(RideTrackGroup& res) const;
    bool SupportsTrackPiece(const uint64_t trackPiece) const;
    ResearchCategory GetResearchCategory() const;
    bool SupportsRideMode(RideMode rideMode) const;
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

// Set on ride types that have a main colour, additional colour and support colour.
constexpr const RideTypeFlagsBitSet TrackHasThreeColours = { RideTypeFlags::HasTrackColourMain,
                                                             RideTypeFlags::HasTrackColourAdditional,
                                                             RideTypeFlags::HasTrackColourSupports };
// Set on _all_ roller coaster ride types, including the _ALT types used for constructing upside down.
constexpr const RideTypeFlagsBitSet CommonCoaster = { RideTypeFlags::HasGForces,
                                                      RideTypeFlags::HasDataLogging,
                                                      RideTypeFlags::HasDrops,
                                                      RideTypeFlags::HasLoadOptions,
                                                      RideTypeFlags::PeepWillRideAgain,
                                                      RideTypeFlags::HasVehicleColours,
                                                      RideTypeFlags::CheckForStalling,
                                                      RideTypeFlags::HasTrack,
                                                      RideTypeFlags::SupportsMultipleTrackColour,
                                                      RideTypeFlags::AllowMusic,
                                                      RideTypeFlags::InterestingToLookAt,
                                                      RideTypeFlags::CanSynchroniseAdjacentStations };

// Set on all roller coaster ride types, excluding the _ALT types used for constructing upside down.
constexpr const RideTypeFlagsBitSet CommonCoasterNonAlt = { RideTypeFlags::ShowInTrackDesigner, RideTypeFlags::HasAirTime,
                                                            RideTypeFlags::HasEntranceExit };

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

constexpr std::string_view MUSIC_OBJECT_DODGEMS = "rct2.music.dodgems";
constexpr std::string_view MUSIC_OBJECT_EGYPTIAN = "rct2.music.egyptian";
constexpr std::string_view MUSIC_OBJECT_FAIRGROUND = "rct2.music.fairground";
constexpr std::string_view MUSIC_OBJECT_GENTLE = "rct2.music.gentle";
constexpr std::string_view MUSIC_OBJECT_HORROR = "rct2.music.horror";
constexpr std::string_view MUSIC_OBJECT_PIRATE = "rct2.music.pirate";
constexpr std::string_view MUSIC_OBJECT_ROCK_1 = "rct2.music.rock1";
constexpr std::string_view MUSIC_OBJECT_ROCK_2 = "rct2.music.rock2";
constexpr std::string_view MUSIC_OBJECT_ROCK_3 = "rct2.music.rock3";
constexpr std::string_view MUSIC_OBJECT_SUMMER = "rct2.music.summer";
constexpr std::string_view MUSIC_OBJECT_TECHNO = "rct2.music.techno";
constexpr std::string_view MUSIC_OBJECT_WATER = "rct2.music.water";
constexpr std::string_view MUSIC_OBJECT_WILD_WEST = "rct2.music.wildwest";

constexpr const RideComponentName& GetRideComponentName(const RideComponentType type)
{
    return RideComponentNames[EnumValue(type)];
}

constexpr const uint64_t AllRideModesAvailable = EnumsToFlags(
    RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned, RideMode::ReverseInclineLaunchedShuttle,
    RideMode::PoweredLaunchPasstrough, RideMode::Shuttle, RideMode::Normal, RideMode::BoatHire, RideMode::UpwardLaunch,
    RideMode::RotatingLift, RideMode::StationToStation, RideMode::SingleRidePerAdmission, RideMode::UnlimitedRidesPerAdmission,
    RideMode::Maze, RideMode::Race, RideMode::Dodgems, RideMode::Swing, RideMode::ShopStall, RideMode::Rotation,
    RideMode::ForwardRotation, RideMode::BackwardRotation, RideMode::FilmAvengingAviators, RideMode::MouseTails3DFilm,
    RideMode::SpaceRings, RideMode::Beginners, RideMode::LimPoweredLaunch, RideMode::FilmThrillRiders,
    RideMode::StormChasers3DFilm, RideMode::SpaceRaiders3DFilm, RideMode::Intense, RideMode::Berserk, RideMode::HauntedHouse,
    RideMode::Circus, RideMode::DownwardLaunch, RideMode::CrookedHouse, RideMode::FreefallDrop, RideMode::PoweredLaunch,
    RideMode::PoweredLaunchBlockSectioned);

extern const CarEntry CableLiftVehicle;

extern const uint16_t RideFilmLength[3];

extern const StringId RideModeNames[static_cast<uint8_t>(RideMode::Count)];

// clang-format off
constexpr const RideTypeDescriptor DummyRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_NONE),
    SET_FIELD(EnabledTrackPieces, {}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, nullptr),
    SET_FIELD(Flags, { static_cast<RideTypeFlags>(0) }),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_UNKNOWN_RIDE, STR_RIDE_DESCRIPTION_UNKNOWN }),
    SET_FIELD(NameConvention, { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, "(INVALID)"),
    SET_FIELD(AvailableBreakdowns, 0),
    SET_FIELD(Heights, { 12, 64, 0, 0, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, nullptr),
    SET_FIELD(RatingsMultipliers, { 0, 0, 0 }),
    SET_FIELD(UpkeepCosts, { 50, 1, 0, 0, 0, 0 }),
    SET_FIELD(BuildCosts, { 0.00_GBP, 0.00_GBP, 1 }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_GENTLE),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 0),
    SET_FIELD(ColourPresets, DEFAULT_FLAT_RIDE_COLOUR_PRESET),
    SET_FIELD(ColourPreview, { static_cast<uint32_t>(SPR_NONE), static_cast<uint32_t>(SPR_NONE) }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "invalid"),
    SET_FIELD(UpdateRotating, UpdateRotatingDefault),
    SET_FIELD(LightFXAddLightsMagicVehicle, nullptr),
    SET_FIELD(StartRideMusic, OpenRCT2::RideAudio::DefaultStartRideMusicChannel),
    SET_FIELD(DesignCreateMode, TrackDesignCreateMode::Default),
    SET_FIELD(MusicUpdateFunction, DefaultMusicUpdate),
    SET_FIELD(Classification, RideClassification::Ride),
    SET_FIELD(UpdateLeaveEntrance, PeepUpdateRideLeaveEntranceDefault),
};
// clang-format on

constexpr const RideTypeDescriptor& GetRideTypeDescriptor(ObjectEntryIndex rideType)
{
    if (rideType >= std::size(RideTypeDescriptors))
        return DummyRTD;

    return RideTypeDescriptors[rideType];
}

constexpr bool RideTypeIsValid(ObjectEntryIndex rideType)
{
    return rideType < std::size(RideTypeDescriptors);
}

bool IsTrackEnabled(int32_t trackFlagIndex);
void UpdateEnabledRidePieces(ride_type_t rideType);
void UpdateDisabledRidePieces(const RideTrackGroup& res);
