/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../audio/Audio.h"
#include "../management/Research.h"
#include "Ride.h"
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

namespace OpenRCT2
{
    using namespace OpenRCT2::Entity::Yaw;

    // clang-format off

    const CarEntry kCableLiftVehicle = {
        .TabRotationMask = 31,
        .spacing = 0,
        .car_mass = 0,
        .tab_height = 0,
        .num_seats = 0,
        .spriteWidth = 0,
        .spriteHeightNegative = 0,
        .spriteHeightPositive = 0,
        .animation = CarEntryAnimation::none,
        .flags = {},
        .base_num_frames = 1,
        .base_image_id = 29110,
        .SpriteGroups = {
            /* SpriteGroupType::slopeFlat */            { 29110, SpritePrecision::sprites32},
            /* SpriteGroupType::slopes12 */             { 29142, SpritePrecision::sprites4},
            /* SpriteGroupType::slopes25 */             { 29150, SpritePrecision::sprites32},
            /* SpriteGroupType::slopes42 */             { 29214, SpritePrecision::sprites8},
            /* SpriteGroupType::slopes60 */             { 29230, SpritePrecision::sprites32},
            /* SpriteGroupType::slopes75 */             { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes90 */             { 0, SpritePrecision::none},
            /* SpriteGroupType::slopesLoop */           { 0, SpritePrecision::none},
            /* SpriteGroupType::slopeInverted */        { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes8 */              { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes16 */             { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes50 */             { 0, SpritePrecision::none},
            /* SpriteGroupType::flatBanked22 */         { 0, SpritePrecision::none},
            /* SpriteGroupType::flatBanked45 */         { 0, SpritePrecision::none},
            /* SpriteGroupType::flatBanked67 */         { 0, SpritePrecision::none},
            /* SpriteGroupType::flatBanked90 */         { 0, SpritePrecision::none},
            /* SpriteGroupType::inlineTwists */         { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes12Banked22 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes8Banked22 */      { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes25Banked22 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes8Banked22 */      { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes16Banked22 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes16Banked45 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes25Banked45 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes12Banked45 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes25Banked67 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes25Banked90 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes25InlineTwists */ { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes42Banked22 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes42Banked45 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes42Banked67 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes42Banked90 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes60Banked22 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes60Banked45 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes60Banked67 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::slopes60Banked90 */     { 0, SpritePrecision::none},
            /* SpriteGroupType::corkscrews */           { 0, SpritePrecision::none},
            /* SpriteGroupType::restraintAnimation */   { 0, SpritePrecision::none},
            /* SpriteGroupType::curvedLiftHillUp */     { 0, SpritePrecision::none},
            /* SpriteGroupType::curvedLiftHillDown */   { 0, SpritePrecision::none},
        },
        .NumCarImages = 0,
        .no_seating_rows = 0,
        .spinning_inertia = 0,
        .spinning_friction = 255,
        .friction_sound_id = Audio::SoundId::liftClassic,
        .ReversedCarIndex = 0,
        .soundRange = SoundRange::screamsMisc,
        .double_sound_frequency = 0,
        .powered_acceleration = 0,
        .powered_max_speed = 0,
        .paintStyle = VehiclePaintStyle::standard,
        .effectVisual = EffectVisual::unknown1,
        .draw_order = 14,
        .num_vertical_frames_override = 0,
        .peep_loading_waypoint_segments = 0,
        .AnimationSpeed = 0,
        .AnimationFrames = 0,
        .SteamEffect = {
            .Longitudinal = 0,
            .Vertical = 0,
        },
        .spinningNumFrames = 0,
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
            STR_RIDE_MODE_POWERED_LAUNCH_PASSTHROUGH,
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
        /* RIDE_TYPE_LSM_LAUNCHED_ROLLER_COASTER        */ kLSMLaunchedRollerCoasterRTD,
        /* RIDE_TYPE_CLASSIC_WOODEN_TWISTER_ROLLER_COASTER */ kClassicWoodenTwisterRollerCoasterRTD,
    };

    bool RideTypeDescriptor::SupportsTrackGroup(const TrackGroup trackGroup) const
    {
        return TrackPaintFunctions.Regular.SupportsTrackGroup(trackGroup);
    }

    ResearchCategory RideTypeDescriptor::GetResearchCategory() const
    {
        switch (Category)
        {
            case RideCategory::transport:
                return ResearchCategory::transport;
            case RideCategory::gentle:
                return ResearchCategory::gentle;
            case RideCategory::rollerCoaster:
                return ResearchCategory::rollercoaster;
            case RideCategory::thrill:
                return ResearchCategory::thrill;
            case RideCategory::water:
                return ResearchCategory::water;
            case RideCategory::shop:
                return ResearchCategory::shop;
            case RideCategory::none:
                break;
        }
        LOG_ERROR("Cannot get Research Category of invalid RideCategory");
        return ResearchCategory::transport;
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

        if (!getGameState().cheats.enableAllDrawableTrackPieces)
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
        if (getGameState().cheats.enableAllDrawableTrackPieces)
            res |= extraTrackGroups;
    }

    bool TrackDrawerEntry::SupportsTrackGroup(const TrackGroup trackGroup) const
    {
        return enabledTrackGroups.get(EnumValue(trackGroup))
            || (getGameState().cheats.enableAllDrawableTrackPieces && extraTrackGroups.get(EnumValue(trackGroup)));
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

    int32_t RideTypeDescriptor::GetUnifiedBoosterSpeed(int32_t relativeSpeed) const
    {
        // BoosterSpeedFactor has valid values of 1, 2, 4 representing a 1/2, 1, and 2 multiplier of legacy speed to unified
        // speed.
        return relativeSpeed * LegacyBoosterSettings.BoosterSpeedFactor / 2;
    }
} // namespace OpenRCT2
