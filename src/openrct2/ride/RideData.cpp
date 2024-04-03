/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
#include "../GameState.h"
#include "../audio/audio.h"
#include "../interface/Colour.h"
#include "../localisation/Localisation.h"
#include "../management/Research.h"
#include "../sprites.h"
#include "Ride.h"
#include "ShopItem.h"
#include "Track.h"
#include "Vehicle.h"
#include "coaster/meta/AirPoweredVerticalCoaster.h"
#include "coaster/meta/AlpineCoaster.h"
#include "coaster/meta/BobsleighCoaster.h"
#include "coaster/meta/ClassicMiniRollerCoaster.h"
#include "coaster/meta/ClassicStandUpRollerCoaster.h"
#include "coaster/meta/ClassicWoodenRollerCoaster.h"
#include "coaster/meta/CompactInvertedCoaster.h"
#include "coaster/meta/CorkscrewRollerCoaster.h"
#include "coaster/meta/FlyingRollerCoaster.h"
#include "coaster/meta/GigaCoaster.h"
#include "coaster/meta/HeartlineTwisterCoaster.h"
#include "coaster/meta/HybridCoaster.h"
#include "coaster/meta/HyperTwister.h"
#include "coaster/meta/Hypercoaster.h"
#include "coaster/meta/InvertedHairpinCoaster.h"
#include "coaster/meta/InvertedImpulseCoaster.h"
#include "coaster/meta/InvertedRollerCoaster.h"
#include "coaster/meta/JuniorRollerCoaster.h"
#include "coaster/meta/LIMLaunchedRollerCoaster.h"
#include "coaster/meta/LayDownRollerCoaster.h"
#include "coaster/meta/LoopingRollerCoaster.h"
#include "coaster/meta/MineRide.h"
#include "coaster/meta/MineTrainCoaster.h"
#include "coaster/meta/MiniRollerCoaster.h"
#include "coaster/meta/MiniSuspendedCoaster.h"
#include "coaster/meta/MultiDimensionRollerCoaster.h"
#include "coaster/meta/ReverseFreefallCoaster.h"
#include "coaster/meta/ReverserRollerCoaster.h"
#include "coaster/meta/SideFrictionRollerCoaster.h"
#include "coaster/meta/SingleRailRollerCoaster.h"
#include "coaster/meta/SpinningWildMouse.h"
#include "coaster/meta/SpiralRollerCoaster.h"
#include "coaster/meta/StandUpRollerCoaster.h"
#include "coaster/meta/SteelWildMouse.h"
#include "coaster/meta/Steeplechase.h"
#include "coaster/meta/SuspendedSwingingCoaster.h"
#include "coaster/meta/TwisterRollerCoaster.h"
#include "coaster/meta/VerticalDropCoaster.h"
#include "coaster/meta/VirginiaReel.h"
#include "coaster/meta/WaterCoaster.h"
#include "coaster/meta/WoodenRollerCoaster.h"
#include "coaster/meta/WoodenWildMouse.h"
#include "gentle/meta/CarRide.h"
#include "gentle/meta/Circus.h"
#include "gentle/meta/CrookedHouse.h"
#include "gentle/meta/Dodgems.h"
#include "gentle/meta/FerrisWheel.h"
#include "gentle/meta/FlyingSaucers.h"
#include "gentle/meta/GhostTrain.h"
#include "gentle/meta/HauntedHouse.h"
#include "gentle/meta/Maze.h"
#include "gentle/meta/MerryGoRound.h"
#include "gentle/meta/MiniGolf.h"
#include "gentle/meta/MiniHelicopters.h"
#include "gentle/meta/MonorailCycles.h"
#include "gentle/meta/MonsterTrucks.h"
#include "gentle/meta/ObservationTower.h"
#include "gentle/meta/SpaceRings.h"
#include "gentle/meta/SpiralSlide.h"
#include "shops/meta/CashMachine.h"
#include "shops/meta/DrinkStall.h"
#include "shops/meta/FirstAid.h"
#include "shops/meta/FoodStall.h"
#include "shops/meta/InformationKiosk.h"
#include "shops/meta/Shop.h"
#include "shops/meta/Toilets.h"
#include "thrill/meta/3DCinema.h"
#include "thrill/meta/Enterprise.h"
#include "thrill/meta/GoKarts.h"
#include "thrill/meta/LaunchedFreefall.h"
#include "thrill/meta/MagicCarpet.h"
#include "thrill/meta/MotionSimulator.h"
#include "thrill/meta/RotoDrop.h"
#include "thrill/meta/SwingingInverterShip.h"
#include "thrill/meta/SwingingShip.h"
#include "thrill/meta/TopSpin.h"
#include "thrill/meta/Twist.h"
#include "transport/meta/Chairlift.h"
#include "transport/meta/Lift.h"
#include "transport/meta/MiniatureRailway.h"
#include "transport/meta/Monorail.h"
#include "transport/meta/SuspendedMonorail.h"
#include "water/meta/BoatHire.h"
#include "water/meta/DinghySlide.h"
#include "water/meta/LogFlume.h"
#include "water/meta/RiverRafts.h"
#include "water/meta/RiverRapids.h"
#include "water/meta/SplashBoats.h"
#include "water/meta/SubmarineRide.h"

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
    /* RIDE_TYPE_SPIRAL_ROLLER_COASTER              */ kSpiralRollerCoasterRTD,
    /* RIDE_TYPE_STAND_UP_ROLLER_COASTER            */ kStandUpRollerCoasterRTD,
    /* RIDE_TYPE_SUSPENDED_SWINGING_COASTER         */ kSuspendedSwingingCoasterRTD,
    /* RIDE_TYPE_INVERTED_ROLLER_COASTER            */ kInvertedRollerCoasterRTD,
    /* RIDE_TYPE_JUNIOR_ROLLER_COASTER              */ kJuniorRollerCoasterRTD,
    /* RIDE_TYPE_MINIATURE_RAILWAY                  */ kMiniatureRailwayRTD,
    /* RIDE_TYPE_MONORAIL                           */ kMonorailRTD,
    /* RIDE_TYPE_MINI_SUSPENDED_COASTER             */ kMiniSuspendedCoasterRTD,
    /* RIDE_TYPE_BOAT_HIRE                          */ kBoatHireRTD,
    /* RIDE_TYPE_WOODEN_WILD_MOUSE                  */ kWoodenWildMouseRTD,
    /* RIDE_TYPE_STEEPLECHASE                       */ kSteeplechaseRTD,
    /* RIDE_TYPE_CAR_RIDE                           */ kCarRideRTD,
    /* RIDE_TYPE_LAUNCHED_FREEFALL                  */ kLaunchedFreefallRTD,
    /* RIDE_TYPE_BOBSLEIGH_COASTER                  */ kBobsleighCoasterRTD,
    /* RIDE_TYPE_OBSERVATION_TOWER                  */ kObservationTowerRTD,
    /* RIDE_TYPE_LOOPING_ROLLER_COASTER             */ kLoopingRollerCoasterRTD,
    /* RIDE_TYPE_DINGHY_SLIDE                       */ kDinghySlideRTD,
    /* RIDE_TYPE_MINE_TRAIN_COASTER                 */ kMineTrainCoasterRTD,
    /* RIDE_TYPE_CHAIRLIFT                          */ kChairliftRTD,
    /* RIDE_TYPE_CORKSCREW_ROLLER_COASTER           */ kCorkscrewRollerCoasterRTD,
    /* RIDE_TYPE_MAZE                               */ kMazeRTD,
    /* RIDE_TYPE_SPIRAL_SLIDE                       */ kSpiralSlideRTD,
    /* RIDE_TYPE_GO_KARTS                           */ kGoKartsRTD,
    /* RIDE_TYPE_LOG_FLUME                          */ kLogFlumeRTD,
    /* RIDE_TYPE_RIVER_RAPIDS                       */ kRiverRapidsRTD,
    /* RIDE_TYPE_DODGEMS                            */ kDodgemsRTD,
    /* RIDE_TYPE_SWINGING_SHIP                      */ kSwingingShipRTD,
    /* RIDE_TYPE_SWINGING_INVERTER_SHIP             */ kSwingingInverterShipRTD,
    /* RIDE_TYPE_FOOD_STALL                         */ kFoodStallRTD,
    /* RIDE_TYPE_1D                                 */ kDummyRTD,
    /* RIDE_TYPE_DRINK_STALL                        */ kDrinkStallRTD,
    /* RIDE_TYPE_1F                                 */ kDummyRTD,
    /* RIDE_TYPE_SHOP                               */ kShopRTD,
    /* RIDE_TYPE_MERRY_GO_ROUND                     */ kMerryGoRoundRTD,
    /* RIDE_TYPE_22                                 */ kDummyRTD,
    /* RIDE_TYPE_INFORMATION_KIOSK                  */ kInformationKioskRTD,
    /* RIDE_TYPE_TOILETS                            */ kToiletsRTD,
    /* RIDE_TYPE_FERRIS_WHEEL                       */ kFerrisWheelRTD,
    /* RIDE_TYPE_MOTION_SIMULATOR                   */ kMotionSimulatorRTD,
    /* RIDE_TYPE_3D_CINEMA                          */ kCinemaRTD,
    /* RIDE_TYPE_TOP_SPIN                           */ kTopSpinRTD,
    /* RIDE_TYPE_SPACE_RINGS                        */ kSpaceRingsRTD,
    /* RIDE_TYPE_REVERSE_FREEFALL_COASTER           */ kReverseFreefallCoasterRTD,
    /* RIDE_TYPE_LIFT                               */ kLiftRTD,
    /* RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER       */ kVerticalDropCoasterRTD,
    /* RIDE_TYPE_CASH_MACHINE                       */ kCashMachineRTD,
    /* RIDE_TYPE_TWIST                              */ kTwistRTD,
    /* RIDE_TYPE_HAUNTED_HOUSE                      */ kHauntedHouseRTD,
    /* RIDE_TYPE_FIRST_AID                          */ kFirstAidRTD,
    /* RIDE_TYPE_CIRCUS                             */ kCircusRTD,
    /* RIDE_TYPE_GHOST_TRAIN                        */ kGhostTrainRTD,
    /* RIDE_TYPE_TWISTER_ROLLER_COASTER             */ kTwisterRollerCoasterRTD,
    /* RIDE_TYPE_WOODEN_ROLLER_COASTER              */ kWoodenRollerCoasterRTD,
    /* RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER       */ kSideFrictionRollerCoasterRTD,
    /* RIDE_TYPE_STEEL_WILD_MOUSE                   */ kSteelWildMouseRTD,
    /* RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER     */ kMultiDimensionRollerCoasterRTD,
    /* RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT */ kMultiDimensionRollerCoasterAltRTD,
    /* RIDE_TYPE_FLYING_ROLLER_COASTER              */ kFlyingRollerCoasterRTD,
    /* RIDE_TYPE_FLYING_ROLLER_COASTER_ALT          */ kFlyingRollerCoasterAltRTD,
    /* RIDE_TYPE_VIRGINIA_REEL                      */ kVirginiaReelRTD,
    /* RIDE_TYPE_SPLASH_BOATS                       */ kSplashBoatsRTD,
    /* RIDE_TYPE_MINI_HELICOPTERS                   */ kMiniHelicoptersRTD,
    /* RIDE_TYPE_LAY_DOWN_ROLLER_COASTER            */ kLayDownRollerCoasterRTD,
    /* RIDE_TYPE_SUSPENDED_MONORAIL                 */ kSuspendedMonorailRTD,
    /* RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT        */ kLayDownRollerCoasterAltRTD,
    /* RIDE_TYPE_REVERSER_ROLLER_COASTER            */ kReverserRollerCoasterRTD,
    /* RIDE_TYPE_HEARTLINE_TWISTER_COASTER          */ kHeartlineTwisterCoasterRTD,
    /* RIDE_TYPE_MINI_GOLF                          */ kMiniGolfRTD,
    /* RIDE_TYPE_GIGA_COASTER                       */ kGigaCoasterRTD,
    /* RIDE_TYPE_ROTO_DROP                          */ kRotoDropRTD,
    /* RIDE_TYPE_FLYING_SAUCERS                     */ kFlyingSaucersRTD,
    /* RIDE_TYPE_CROOKED_HOUSE                      */ kCrookedHouseRTD,
    /* RIDE_TYPE_MONORAIL_CYCLES                    */ kMonorailCyclesRTD,
    /* RIDE_TYPE_COMPACT_INVERTED_COASTER           */ kCompactInvertedCoasterRTD,
    /* RIDE_TYPE_WATER_COASTER                      */ kWaterCoasterRTD,
    /* RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER       */ kAirPoweredVerticalCoasterRTD,
    /* RIDE_TYPE_INVERTED_HAIRPIN_COASTER           */ kInvertedHairpinCoasterRTD,
    /* RIDE_TYPE_MAGIC_CARPET                       */ kMagicCarpetRTD,
    /* RIDE_TYPE_SUBMARINE_RIDE                     */ kSubmarineRideRTD,
    /* RIDE_TYPE_RIVER_RAFTS                        */ kRiverRaftsRTD,
    /* RIDE_TYPE_50                                 */ kDummyRTD,
    /* RIDE_TYPE_ENTERPRISE                         */ kEnterpriseRTD,
    /* RIDE_TYPE_52                                 */ kDummyRTD,
    /* RIDE_TYPE_53                                 */ kDummyRTD,
    /* RIDE_TYPE_54                                 */ kDummyRTD,
    /* RIDE_TYPE_55                                 */ kDummyRTD,
    /* RIDE_TYPE_INVERTED_IMPULSE_COASTER           */ kInvertedImpulseCoasterRTD,
    /* RIDE_TYPE_MINI_ROLLER_COASTER                */ kMiniRollerCoasterRTD,
    /* RIDE_TYPE_MINE_RIDE                          */ kMineRideRTD,
    /* RIDE_TYPE_59                                 */ kDummyRTD,
    /* RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER        */ kLIMLaunchedRollerCoasterRTD,
    /* RIDE_TYPE_HYPERCOASTER,                      */ kHypercoasterRTD,
    /* RIDE_TYPE_HYPER_TWISTER,                     */ kHyperTwisterRTD,
    /* RIDE_TYPE_MONSTER_TRUCKS,                    */ kMonsterTrucksRTD,
    /* RIDE_TYPE_SPINNING_WILD_MOUSE,               */ kSpinningWildMouseRTD,
    /* RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER,       */ kClassicMiniRollerCoasterRTD,
    /* RIDE_TYPE_HYBRID_COASTER                     */ kHybridCoasterRTD,
    /* RIDE_TYPE_SINGLE_RAIL_ROLLER_COASTER         */ kSingleRailRollerCoasterRTD,
    /* RIDE_TYPE_ALPINE_COASTER                     */ kAlpineCoasterRTD,
    /* RIDE_TYPE_CLASSIC_WOODEN_ROLLER_COASTER      */ kClassicWoodenRollerCoasterRTD,
    /* RIDE_TYPE_CLASSIC_STAND_UP_ROLLER_COASTER    */ kClassicStandUpRollerCoasterRTD,
};

bool RideTypeDescriptor::HasFlag(uint64_t flag) const
{
    return Flags & flag;
}

void RideTypeDescriptor::GetAvailableTrackPieces(RideTrackGroup& res) const
{
    res = EnabledTrackPieces;
    if (GetGameState().Cheats.EnableAllDrawableTrackPieces)
        res |= ExtraTrackPieces;
}

bool RideTypeDescriptor::SupportsTrackPiece(const uint64_t trackPiece) const
{
    return EnabledTrackPieces.get(trackPiece)
        || (GetGameState().Cheats.EnableAllDrawableTrackPieces && ExtraTrackPieces.get(trackPiece));
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

static RideTrackGroup _enabledRidePieces = {};
static RideTrackGroup _disabledRidePieces = {};

bool IsTrackEnabled(int32_t trackFlagIndex)
{
    return _enabledRidePieces.get(trackFlagIndex);
}

void UpdateEnabledRidePieces(ride_type_t rideType)
{
    GetRideTypeDescriptor(rideType).GetAvailableTrackPieces(_enabledRidePieces);

    if (!GetGameState().Cheats.EnableAllDrawableTrackPieces)
    {
        _enabledRidePieces &= ~_disabledRidePieces;
    }
}

void UpdateDisabledRidePieces(const RideTrackGroup& res)
{
    _disabledRidePieces = res;
}
