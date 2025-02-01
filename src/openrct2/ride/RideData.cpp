/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

/**
 * Whether a particular ride has a running track or not. Will probably end up
 * being used in various places in the game.
 *
 * Data source is 0x0097E3AC
 *
 * Generating function is here
 * https://gist.github.com/kevinburke/eaeb1d8149a6eef0dcc1
 */

#include "RideData.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../audio/audio.h"
#include "../interface/Colour.h"
#include "../management/Research.h"
#include "../sprites.h"
#include "Ride.h"
#include "ShopItem.h"
#include "Track.h"
#include "Vehicle.h"
#include "rtd/coaster/AirPoweredVerticalCoaster.h"
#include "rtd/coaster/AlpineCoaster.h"
#include "rtd/coaster/BobsleighCoaster.h"
#include "rtd/coaster/ClassicMiniRollerCoaster.h"
#include "rtd/coaster/ClassicStandUpRollerCoaster.h"
#include "rtd/coaster/ClassicWoodenRollerCoaster.h"
#include "rtd/coaster/ClassicWoodenTwisterRollerCoaster.h"
#include "rtd/coaster/CompactInvertedCoaster.h"
#include "rtd/coaster/CorkscrewRollerCoaster.h"
#include "rtd/coaster/FlyingRollerCoaster.h"
#include "rtd/coaster/GigaCoaster.h"
#include "rtd/coaster/HeartlineTwisterCoaster.h"
#include "rtd/coaster/HybridCoaster.h"
#include "rtd/coaster/HyperTwister.h"
#include "rtd/coaster/Hypercoaster.h"
#include "rtd/coaster/InvertedHairpinCoaster.h"
#include "rtd/coaster/InvertedImpulseCoaster.h"
#include "rtd/coaster/InvertedRollerCoaster.h"
#include "rtd/coaster/JuniorRollerCoaster.h"
#include "rtd/coaster/LIMLaunchedRollerCoaster.h"
#include "rtd/coaster/LSMLaunchedRollerCoaster.h"
#include "rtd/coaster/LayDownRollerCoaster.h"
#include "rtd/coaster/LoopingRollerCoaster.h"
#include "rtd/coaster/MineRide.h"
#include "rtd/coaster/MineTrainCoaster.h"
#include "rtd/coaster/MiniRollerCoaster.h"
#include "rtd/coaster/MiniSuspendedCoaster.h"
#include "rtd/coaster/MultiDimensionRollerCoaster.h"
#include "rtd/coaster/ReverseFreefallCoaster.h"
#include "rtd/coaster/ReverserRollerCoaster.h"
#include "rtd/coaster/SideFrictionRollerCoaster.h"
#include "rtd/coaster/SingleRailRollerCoaster.h"
#include "rtd/coaster/SpinningWildMouse.h"
#include "rtd/coaster/SpiralRollerCoaster.h"
#include "rtd/coaster/StandUpRollerCoaster.h"
#include "rtd/coaster/SteelWildMouse.h"
#include "rtd/coaster/Steeplechase.h"
#include "rtd/coaster/SuspendedSwingingCoaster.h"
#include "rtd/coaster/TwisterRollerCoaster.h"
#include "rtd/coaster/VerticalDropCoaster.h"
#include "rtd/coaster/VirginiaReel.h"
#include "rtd/coaster/WaterCoaster.h"
#include "rtd/coaster/WoodenRollerCoaster.h"
#include "rtd/coaster/WoodenWildMouse.h"
#include "rtd/gentle/CarRide.h"
#include "rtd/gentle/Circus.h"
#include "rtd/gentle/CrookedHouse.h"
#include "rtd/gentle/Dodgems.h"
#include "rtd/gentle/FerrisWheel.h"
#include "rtd/gentle/FlyingSaucers.h"
#include "rtd/gentle/GhostTrain.h"
#include "rtd/gentle/HauntedHouse.h"
#include "rtd/gentle/Maze.h"
#include "rtd/gentle/MerryGoRound.h"
#include "rtd/gentle/MiniGolf.h"
#include "rtd/gentle/MiniHelicopters.h"
#include "rtd/gentle/MonorailCycles.h"
#include "rtd/gentle/MonsterTrucks.h"
#include "rtd/gentle/ObservationTower.h"
#include "rtd/gentle/SpaceRings.h"
#include "rtd/gentle/SpiralSlide.h"
#include "rtd/shops/CashMachine.h"
#include "rtd/shops/DrinkStall.h"
#include "rtd/shops/FirstAid.h"
#include "rtd/shops/FoodStall.h"
#include "rtd/shops/InformationKiosk.h"
#include "rtd/shops/Shop.h"
#include "rtd/shops/Toilets.h"
#include "rtd/thrill/3DCinema.h"
#include "rtd/thrill/Enterprise.h"
#include "rtd/thrill/GoKarts.h"
#include "rtd/thrill/LaunchedFreefall.h"
#include "rtd/thrill/MagicCarpet.h"
#include "rtd/thrill/MotionSimulator.h"
#include "rtd/thrill/RotoDrop.h"
#include "rtd/thrill/SwingingInverterShip.h"
#include "rtd/thrill/SwingingShip.h"
#include "rtd/thrill/TopSpin.h"
#include "rtd/thrill/Twist.h"
#include "rtd/transport/Chairlift.h"
#include "rtd/transport/Lift.h"
#include "rtd/transport/MiniatureRailway.h"
#include "rtd/transport/Monorail.h"
#include "rtd/transport/SuspendedMonorail.h"
#include "rtd/water/BoatHire.h"
#include "rtd/water/DinghySlide.h"
#include "rtd/water/LogFlume.h"
#include "rtd/water/RiverRafts.h"
#include "rtd/water/RiverRapids.h"
#include "rtd/water/SplashBoats.h"
#include "rtd/water/SubmarineRide.h"

#include <iterator>

using namespace OpenRCT2;
using namespace OpenRCT2::Entity::Yaw;

// clang-format off

const CarEntry kCableLiftVehicle = {
    .TabRotationMask = 31,
    .spacing = 0,
    .car_mass = 0,
    .tab_height = 0,
    .num_seats = 0,
    .sprite_width = 0,
    .sprite_height_negative = 0,
    .sprite_height_positive = 0,
    .animation = CarEntryAnimation::None,
    .flags = 0,
    .base_num_frames = 1,
    .base_image_id = 29110,
    .SpriteGroups = {
        /* SpriteGroupType::SlopeFlat */            { 29110, SpritePrecision::Sprites32},
        /* SpriteGroupType::Slopes12 */             { 29142, SpritePrecision::Sprites4},
        /* SpriteGroupType::Slopes25 */             { 29150, SpritePrecision::Sprites32},
        /* SpriteGroupType::Slopes42 */             { 29214, SpritePrecision::Sprites8},
        /* SpriteGroupType::Slopes60 */             { 29230, SpritePrecision::Sprites32},
        /* SpriteGroupType::Slopes75 */             { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes90 */             { 0, SpritePrecision::None},
        /* SpriteGroupType::SlopesLoop */           { 0, SpritePrecision::None},
        /* SpriteGroupType::SlopeInverted */        { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes8 */              { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes16 */             { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes50 */             { 0, SpritePrecision::None},
        /* SpriteGroupType::FlatBanked22 */         { 0, SpritePrecision::None},
        /* SpriteGroupType::FlatBanked45 */         { 0, SpritePrecision::None},
        /* SpriteGroupType::FlatBanked67 */         { 0, SpritePrecision::None},
        /* SpriteGroupType::FlatBanked90 */         { 0, SpritePrecision::None},
        /* SpriteGroupType::InlineTwists */         { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes12Banked22 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes8Banked22 */      { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes25Banked22 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes8Banked22 */      { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes16Banked22 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes16Banked45 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes25Banked45 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes12Banked45 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes25Banked67 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes25Banked90 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes25InlineTwists */ { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes42Banked22 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes42Banked45 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes42Banked67 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes42Banked90 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes60Banked22 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes60Banked45 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes60Banked67 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Slopes60Banked90 */     { 0, SpritePrecision::None},
        /* SpriteGroupType::Corkscrews */           { 0, SpritePrecision::None},
        /* SpriteGroupType::RestraintAnimation */   { 0, SpritePrecision::None},
        /* SpriteGroupType::CurvedLiftHillUp */     { 0, SpritePrecision::None},
        /* SpriteGroupType::CurvedLiftHillDown */   { 0, SpritePrecision::None},
    },
    .NumCarImages = 0,
    .no_seating_rows = 0,
    .spinning_inertia = 0,
    .spinning_friction = 255,
    .friction_sound_id = Audio::SoundId::LiftClassic,
    .ReversedCarIndex = 0,
    .sound_range = 0,
    .double_sound_frequency = 0,
    .powered_acceleration = 0,
    .powered_max_speed = 0,
    .PaintStyle = 0,
    .effect_visual = 1,
    .draw_order = 14,
    .num_vertical_frames_override = 0,
    .peep_loading_waypoint_segments = 0,
    .AnimationSpeed = 0,
    .AnimationFrames = 0,
    .SteamEffect = {
        .Longitudinal = 0,
        .Vertical = 0,
    },
};

/* rct2: 0x009A0AA0 */
const uint16_t kRideFilmLength[3] = {
    5000, // MOUSE_TAILS
    6000, // STORM_CHASERS
    7000, // SPACE_RAIDERS
};

const StringId kRideModeNames[] = {
        STR_RIDE_MODE_NORMAL,
        STR_RIDE_MODE_CONTINUOUS_CIRCUIT,
        STR_RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE,
        STR_RIDE_MODE_POWERED_LAUNCH_PASSTROUGH,
        STR_RIDE_MODE_SHUTTLE,
        STR_RIDE_MODE_BOAT_HIRE,
        STR_RIDE_MODE_UPWARD_LAUNCH,
        STR_RIDE_MODE_ROTATING_LIFT,
        STR_RIDE_MODE_STATION_TO_STATION,
        STR_RIDE_MODE_SINGLE_RIDE_PER_ADMISSION,
        STR_RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION,
        STR_RIDE_MODE_MAZE,
        STR_RIDE_MODE_RACE,
        STR_RIDE_MODE_DODGEMS,
        STR_RIDE_MODE_SWING,
        STR_RIDE_MODE_SHOP_STALL,
        STR_RIDE_MODE_ROTATION,
        STR_RIDE_MODE_FORWARD_ROTATION,
        STR_RIDE_MODE_BACKWARD_ROTATION,
        STR_RIDE_MODE_FILM_AVENGING_AVIATORS,
        STR_RIDE_MODE_3D_FILM_MOUSE_TAILS,
        STR_RIDE_MODE_SPACE_RINGS,
        STR_RIDE_MODE_BEGINNERS,
        STR_RIDE_MODE_LIM_POWERED_LAUNCH,
        STR_RIDE_MODE_FILM_THRILL_RIDERS,
        STR_RIDE_MODE_3D_FILM_STORM_CHASERS,
        STR_RIDE_MODE_3D_FILM_SPACE_RAIDERS,
        STR_RIDE_MODE_INTENSE,
        STR_RIDE_MODE_BERSERK,
        STR_RIDE_MODE_HAUNTED_HOUSE,
        STR_RIDE_MODE_CIRCUS_SHOW,
        STR_RIDE_MODE_DOWNWARD_LAUNCH,
        STR_RIDE_MODE_CROOKED_HOUSE,
        STR_RIDE_MODE_FREEFALL_DROP,
        STR_RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED,
        STR_RIDE_MODE_POWERED_LAUNCH,
        STR_RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED_MODE,
};
// clang-format on

constexpr RideTypeDescriptor kRideTypeDescriptors[RIDE_TYPE_COUNT] = {
    /* RIDE_TYPE_SPIRAL_ROLLER_COASTER              */ SpiralRollerCoasterRTD,
    /* RIDE_TYPE_STAND_UP_ROLLER_COASTER            */ StandUpRollerCoasterRTD,
    /* RIDE_TYPE_SUSPENDED_SWINGING_COASTER         */ SuspendedSwingingCoasterRTD,
    /* RIDE_TYPE_INVERTED_ROLLER_COASTER            */ InvertedRollerCoasterRTD,
    /* RIDE_TYPE_JUNIOR_ROLLER_COASTER              */ JuniorRollerCoasterRTD,
    /* RIDE_TYPE_MINIATURE_RAILWAY                  */ MiniatureRailwayRTD,
    /* RIDE_TYPE_MONORAIL                           */ MonorailRTD,
    /* RIDE_TYPE_MINI_SUSPENDED_COASTER             */ MiniSuspendedCoasterRTD,
    /* RIDE_TYPE_BOAT_HIRE                          */ BoatHireRTD,
    /* RIDE_TYPE_WOODEN_WILD_MOUSE                  */ WoodenWildMouseRTD,
    /* RIDE_TYPE_STEEPLECHASE                       */ SteeplechaseRTD,
    /* RIDE_TYPE_CAR_RIDE                           */ CarRideRTD,
    /* RIDE_TYPE_LAUNCHED_FREEFALL                  */ LaunchedFreefallRTD,
    /* RIDE_TYPE_BOBSLEIGH_COASTER                  */ BobsleighCoasterRTD,
    /* RIDE_TYPE_OBSERVATION_TOWER                  */ ObservationTowerRTD,
    /* RIDE_TYPE_LOOPING_ROLLER_COASTER             */ LoopingRollerCoasterRTD,
    /* RIDE_TYPE_DINGHY_SLIDE                       */ DinghySlideRTD,
    /* RIDE_TYPE_MINE_TRAIN_COASTER                 */ MineTrainCoasterRTD,
    /* RIDE_TYPE_CHAIRLIFT                          */ ChairliftRTD,
    /* RIDE_TYPE_CORKSCREW_ROLLER_COASTER           */ CorkscrewRollerCoasterRTD,
    /* RIDE_TYPE_MAZE                               */ MazeRTD,
    /* RIDE_TYPE_SPIRAL_SLIDE                       */ SpiralSlideRTD,
    /* RIDE_TYPE_GO_KARTS                           */ GoKartsRTD,
    /* RIDE_TYPE_LOG_FLUME                          */ LogFlumeRTD,
    /* RIDE_TYPE_RIVER_RAPIDS                       */ RiverRapidsRTD,
    /* RIDE_TYPE_DODGEMS                            */ DodgemsRTD,
    /* RIDE_TYPE_SWINGING_SHIP                      */ SwingingShipRTD,
    /* RIDE_TYPE_SWINGING_INVERTER_SHIP             */ SwingingInverterShipRTD,
    /* RIDE_TYPE_FOOD_STALL                         */ FoodStallRTD,
    /* RIDE_TYPE_1D                                 */ kDummyRTD,
    /* RIDE_TYPE_DRINK_STALL                        */ DrinkStallRTD,
    /* RIDE_TYPE_1F                                 */ kDummyRTD,
    /* RIDE_TYPE_SHOP                               */ ShopRTD,
    /* RIDE_TYPE_MERRY_GO_ROUND                     */ MerryGoRoundRTD,
    /* RIDE_TYPE_22                                 */ kDummyRTD,
    /* RIDE_TYPE_INFORMATION_KIOSK                  */ InformationKioskRTD,
    /* RIDE_TYPE_TOILETS                            */ ToiletsRTD,
    /* RIDE_TYPE_FERRIS_WHEEL                       */ FerrisWheelRTD,
    /* RIDE_TYPE_MOTION_SIMULATOR                   */ MotionSimulatorRTD,
    /* RIDE_TYPE_3D_CINEMA                          */ CinemaRTD,
    /* RIDE_TYPE_TOP_SPIN                           */ TopSpinRTD,
    /* RIDE_TYPE_SPACE_RINGS                        */ SpaceRingsRTD,
    /* RIDE_TYPE_REVERSE_FREEFALL_COASTER           */ ReverseFreefallCoasterRTD,
    /* RIDE_TYPE_LIFT                               */ LiftRTD,
    /* RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER       */ VerticalDropCoasterRTD,
    /* RIDE_TYPE_CASH_MACHINE                       */ CashMachineRTD,
    /* RIDE_TYPE_TWIST                              */ TwistRTD,
    /* RIDE_TYPE_HAUNTED_HOUSE                      */ HauntedHouseRTD,
    /* RIDE_TYPE_FIRST_AID                          */ FirstAidRTD,
    /* RIDE_TYPE_CIRCUS                             */ CircusRTD,
    /* RIDE_TYPE_GHOST_TRAIN                        */ GhostTrainRTD,
    /* RIDE_TYPE_TWISTER_ROLLER_COASTER             */ TwisterRollerCoasterRTD,
    /* RIDE_TYPE_WOODEN_ROLLER_COASTER              */ WoodenRollerCoasterRTD,
    /* RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER       */ SideFrictionRollerCoasterRTD,
    /* RIDE_TYPE_STEEL_WILD_MOUSE                   */ SteelWildMouseRTD,
    /* RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER     */ MultiDimensionRollerCoasterRTD,
    /* RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT */ MultiDimensionRollerCoasterAltRTD,
    /* RIDE_TYPE_FLYING_ROLLER_COASTER              */ FlyingRollerCoasterRTD,
    /* RIDE_TYPE_FLYING_ROLLER_COASTER_ALT          */ FlyingRollerCoasterAltRTD,
    /* RIDE_TYPE_VIRGINIA_REEL                      */ VirginiaReelRTD,
    /* RIDE_TYPE_SPLASH_BOATS                       */ SplashBoatsRTD,
    /* RIDE_TYPE_MINI_HELICOPTERS                   */ MiniHelicoptersRTD,
    /* RIDE_TYPE_LAY_DOWN_ROLLER_COASTER            */ LayDownRollerCoasterRTD,
    /* RIDE_TYPE_SUSPENDED_MONORAIL                 */ SuspendedMonorailRTD,
    /* RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT        */ LayDownRollerCoasterAltRTD,
    /* RIDE_TYPE_REVERSER_ROLLER_COASTER            */ ReverserRollerCoasterRTD,
    /* RIDE_TYPE_HEARTLINE_TWISTER_COASTER          */ HeartlineTwisterCoasterRTD,
    /* RIDE_TYPE_MINI_GOLF                          */ MiniGolfRTD,
    /* RIDE_TYPE_GIGA_COASTER                       */ GigaCoasterRTD,
    /* RIDE_TYPE_ROTO_DROP                          */ RotoDropRTD,
    /* RIDE_TYPE_FLYING_SAUCERS                     */ FlyingSaucersRTD,
    /* RIDE_TYPE_CROOKED_HOUSE                      */ CrookedHouseRTD,
    /* RIDE_TYPE_MONORAIL_CYCLES                    */ MonorailCyclesRTD,
    /* RIDE_TYPE_COMPACT_INVERTED_COASTER           */ CompactInvertedCoasterRTD,
    /* RIDE_TYPE_WATER_COASTER                      */ WaterCoasterRTD,
    /* RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER       */ AirPoweredVerticalCoasterRTD,
    /* RIDE_TYPE_INVERTED_HAIRPIN_COASTER           */ InvertedHairpinCoasterRTD,
    /* RIDE_TYPE_MAGIC_CARPET                       */ MagicCarpetRTD,
    /* RIDE_TYPE_SUBMARINE_RIDE                     */ SubmarineRideRTD,
    /* RIDE_TYPE_RIVER_RAFTS                        */ RiverRaftsRTD,
    /* RIDE_TYPE_50                                 */ kDummyRTD,
    /* RIDE_TYPE_ENTERPRISE                         */ EnterpriseRTD,
    /* RIDE_TYPE_52                                 */ kDummyRTD,
    /* RIDE_TYPE_53                                 */ kDummyRTD,
    /* RIDE_TYPE_54                                 */ kDummyRTD,
    /* RIDE_TYPE_55                                 */ kDummyRTD,
    /* RIDE_TYPE_INVERTED_IMPULSE_COASTER           */ InvertedImpulseCoasterRTD,
    /* RIDE_TYPE_MINI_ROLLER_COASTER                */ MiniRollerCoasterRTD,
    /* RIDE_TYPE_MINE_RIDE                          */ MineRideRTD,
    /* RIDE_TYPE_59                                 */ kDummyRTD,
    /* RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER        */ LIMLaunchedRollerCoasterRTD,
    /* RIDE_TYPE_HYPERCOASTER,                      */ HypercoasterRTD,
    /* RIDE_TYPE_HYPER_TWISTER,                     */ HyperTwisterRTD,
    /* RIDE_TYPE_MONSTER_TRUCKS,                    */ MonsterTrucksRTD,
    /* RIDE_TYPE_SPINNING_WILD_MOUSE,               */ SpinningWildMouseRTD,
    /* RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER,       */ ClassicMiniRollerCoasterRTD,
    /* RIDE_TYPE_HYBRID_COASTER                     */ HybridCoasterRTD,
    /* RIDE_TYPE_SINGLE_RAIL_ROLLER_COASTER         */ SingleRailRollerCoasterRTD,
    /* RIDE_TYPE_ALPINE_COASTER                     */ AlpineCoasterRTD,
    /* RIDE_TYPE_CLASSIC_WOODEN_ROLLER_COASTER      */ ClassicWoodenRollerCoasterRTD,
    /* RIDE_TYPE_CLASSIC_STAND_UP_ROLLER_COASTER    */ ClassicStandUpRollerCoasterRTD,
    /* RIDE_TYPE_LSM_LAUNCHED_ROLLER_COASTER        */ LSMLaunchedRollerCoasterRTD,
    /* RIDE_TYPE_CLASSIC_WOODEN_TWISTER_ROLLER_COASTER */ ClassicWoodenTwisterRollerCoasterRTD,
};

bool RideTypeDescriptor::HasFlag(RtdFlag flag) const
{
    return ::HasFlag(Flags, flag);
}

bool RideTypeDescriptor::SupportsTrackGroup(const TrackGroup trackGroup) const
{
    return TrackPaintFunctions.Regular.SupportsTrackGroup(trackGroup);
}

ResearchCategory RideTypeDescriptor::GetResearchCategory() const
{
    switch (Category)
    {
        case RIDE_CATEGORY_TRANSPORT:
            return ResearchCategory::Transport;
        case RIDE_CATEGORY_GENTLE:
            return ResearchCategory::Gentle;
        case RIDE_CATEGORY_ROLLERCOASTER:
            return ResearchCategory::Rollercoaster;
        case RIDE_CATEGORY_THRILL:
            return ResearchCategory::Thrill;
        case RIDE_CATEGORY_WATER:
            return ResearchCategory::Water;
        case RIDE_CATEGORY_SHOP:
            return ResearchCategory::Shop;
        case RIDE_CATEGORY_NONE:
            break;
    }
    LOG_ERROR("Cannot get Research Category of invalid RideCategory");
    return ResearchCategory::Transport;
}

bool RideTypeDescriptor::SupportsRideMode(RideMode rideMode) const
{
    return RideModes & EnumToFlag(rideMode);
}

static RideTrackGroups _enabledRideGroups = {};
static RideTrackGroups _disabledRideGroups = {};

bool IsTrackEnabled(TrackGroup trackGroup)
{
    return _enabledRideGroups.get(EnumValue(trackGroup));
}

void UpdateEnabledRideGroups(TrackDrawerDescriptor trackDrawerDescriptor)
{
    trackDrawerDescriptor.Regular.GetAvailableTrackGroups(_enabledRideGroups);

    if (!GetGameState().Cheats.enableAllDrawableTrackPieces)
    {
        _enabledRideGroups &= ~_disabledRideGroups;
    }
}

void UpdateDisabledRideGroups(const RideTrackGroups& res)
{
    _disabledRideGroups = res;
}

void TrackDrawerEntry::GetAvailableTrackGroups(RideTrackGroups& res) const
{
    res = enabledTrackGroups;
    if (GetGameState().Cheats.enableAllDrawableTrackPieces)
        res |= extraTrackGroups;
}

bool TrackDrawerEntry::SupportsTrackGroup(const TrackGroup trackGroup) const
{
    return enabledTrackGroups.get(EnumValue(trackGroup))
        || (GetGameState().Cheats.enableAllDrawableTrackPieces && extraTrackGroups.get(EnumValue(trackGroup)));
}

bool TrackDrawerDescriptor::HasCoveredPieces() const
{
    return Covered.enabledTrackGroups.count() > 0;
}

TrackDrawerDescriptor getTrackDrawerDescriptor(const RideTypeDescriptor& rtd, bool isInverted)
{
    return isInverted ? rtd.InvertedTrackPaintFunctions : rtd.TrackPaintFunctions;
}

TrackDrawerEntry getTrackDrawerEntry(const RideTypeDescriptor& rtd, bool isInverted, bool isCovered)
{
    auto descriptor = getTrackDrawerDescriptor(rtd, isInverted);

    if (isCovered)
    {
        return descriptor.Covered;
    }

    return descriptor.Regular;
}

int32_t RideTypeDescriptor::GetUnifiedBoosterSpeed(int32_t compressedSpeed) const
{
    // BoosterSpeedFactor has valid values of 1, 2, 4 representing a 1/2, 1, and 2 multiplier of legacy speed to unified
    // speed.
    return compressedSpeed * LegacyBoosterSettings.BoosterSpeedFactor / 2;
}
