/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "../audio/audio.h"
#include "../interface/Colour.h"
#include "../localisation/Localisation.h"
#include "../management/Research.h"
#include "../sprites.h"
#include "../core/Json.hpp"
#include "Ride.h"
#include "ShopItem.h"
#include "Track.h"
#include "Vehicle.h"
#include "coaster/meta/AirPoweredVerticalCoaster.h"
#include "coaster/meta/BobsleighCoaster.h"
#include "coaster/meta/ClassicMiniRollerCoaster.h"
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

// clang-format off

const rct_ride_entry_vehicle CableLiftVehicle = {
    /* .rotation_frame_mask = */ 31,
    /* .num_vertical_frames = */ 0,
    /* .num_horizontal_frames = */ 0,
    /* .spacing = */ 0,
    /* .car_mass = */ 0,
    /* .tab_height = */ 0,
    /* .num_seats = */ 0,
    /* .sprite_flags = */ VEHICLE_SPRITE_FLAG_FLAT | VEHICLE_SPRITE_FLAG_GENTLE_SLOPES | VEHICLE_SPRITE_FLAG_STEEP_SLOPES,
    /* .sprite_width = */ 0,
    /* .sprite_height_negative = */ 0,
    /* .sprite_height_positive = */ 0,
    /* .animation = */ 0,
    /* .flags = */ 0,
    /* .base_num_frames = */ 1,
    /* .base_image_id = */ 29110,
    /* .restraint_image_id = */ 0,
    /* .gentle_slope_image_id = */ 29142,
    /* .steep_slope_image_id = */ 29214,
    /* .vertical_slope_image_id = */ 0,
    /* .diagonal_slope_image_id = */ 0,
    /* .banked_image_id = */ 0,
    /* .inline_twist_image_id = */ 0,
    /* .flat_to_gentle_bank_image_id = */ 0,
    /* .diagonal_to_gentle_slope_bank_image_id = */ 0,
    /* .gentle_slope_to_bank_image_id = */ 0,
    /* .gentle_slope_bank_turn_image_id = */ 0,
    /* .flat_bank_to_gentle_slope_image_id = */ 0,
    /* .corkscrew_image_id = */ 0,
    /* .no_vehicle_images = */ 0,
    /* .no_seating_rows = */ 0,
    /* .spinning_inertia = */ 0,
    /* .spinning_friction = */ 255,
    /* .friction_sound_id = */ OpenRCT2::Audio::SoundId::LiftClassic,
    /* .log_flume_reverser_vehicle_type = */ 0,
    /* .sound_range = */ 0,
    /* .double_sound_frequency = */ 0,
    /* .powered_acceleration = */ 0,
    /* .powered_max_speed = */ 0,
    /* .car_visual = */ 0,
    /* .effect_visual = */ 1,
    /* .draw_order = */ 14,
    /* .num_vertical_frames_override = */ 0,
    /* .peep_loading_positions = */ 0
};

/* rct2: 0x009A0AA0 */
const uint16_t RideFilmLength[3] = {
    5000, // MOUSE_TAILS
    6000, // STORM_CHASERS
    7000, // SPACE_RAIDERS
};

const rct_string_id RideModeNames[] = {
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

constexpr const RideTypeDescriptor RideTypeDescriptors[RIDE_TYPE_COUNT] = {
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
    /* RIDE_TYPE_1D                                 */ DummyRTD,
    /* RIDE_TYPE_DRINK_STALL                        */ DrinkStallRTD,
    /* RIDE_TYPE_1F                                 */ DummyRTD,
    /* RIDE_TYPE_SHOP                               */ ShopRTD,
    /* RIDE_TYPE_MERRY_GO_ROUND                     */ MerryGoRoundRTD,
    /* RIDE_TYPE_22                                 */ DummyRTD,
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
    /* RIDE_TYPE_50                                 */ DummyRTD,
    /* RIDE_TYPE_ENTERPRISE                         */ EnterpriseRTD,
    /* RIDE_TYPE_52                                 */ DummyRTD,
    /* RIDE_TYPE_53                                 */ DummyRTD,
    /* RIDE_TYPE_54                                 */ DummyRTD,
    /* RIDE_TYPE_55                                 */ DummyRTD,
    /* RIDE_TYPE_INVERTED_IMPULSE_COASTER           */ InvertedImpulseCoasterRTD,
    /* RIDE_TYPE_MINI_ROLLER_COASTER                */ MiniRollerCoasterRTD,
    /* RIDE_TYPE_MINE_RIDE                          */ MineRideRTD,
    /* RIDE_TYPE_59                                 */ DummyRTD,
    /* RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER        */ LIMLaunchedRollerCoasterRTD,
    /* RIDE_TYPE_HYPERCOASTER,                      */ HypercoasterRTD,
    /* RIDE_TYPE_HYPER_TWISTER,                     */ HyperTwisterRTD,
    /* RIDE_TYPE_MONSTER_TRUCKS,                    */ MonsterTrucksRTD,
    /* RIDE_TYPE_SPINNING_WILD_MOUSE,               */ SpinningWildMouseRTD,
    /* RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER,       */ ClassicMiniRollerCoasterRTD,
    /* RIDE_TYPE_HYBRID_COASTER                     */ HybridCoasterRTD,
    /* RIDE_TYPE_SINGLE_RAIL_ROLLER_COASTER         */ SingleRailRollerCoasterRTD,
};

bool RideTypeDescriptor::HasFlag(uint64_t flag) const
{
    return Flags & flag;
}

void RideTypeDescriptor::GetAvailableTrackPieces(RideTrackGroup& res) const
{
    res = EnabledTrackPieces;
    if (gCheatsEnableAllDrawableTrackPieces)
        res |= ExtraTrackPieces;
}

bool RideTypeDescriptor::SupportsTrackPiece(const uint64_t trackPiece) const
{
    return EnabledTrackPieces.get(trackPiece) || (gCheatsEnableAllDrawableTrackPieces && ExtraTrackPieces.get(trackPiece));
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
    log_error("Cannot get Research Category of invalid RideCategory");
    return ResearchCategory::Transport;
}

static RideTrackGroup _enabledRidePieces = {};

bool IsTrackEnabled(int32_t trackFlagIndex)
{
    return _enabledRidePieces.get(trackFlagIndex);
}

void UpdateEnabledRidePieces(ride_type_t rideType)
{
    GetRideTypeDescriptor(rideType).GetAvailableTrackPieces(_enabledRidePieces);
}

// object export/import functions
static std::string GetRideTypeId(ObjectEntryIndex rideType)
{
    switch (rideType)
    {
        case RIDE_TYPE_CIRCUS:
            return "openrct2.ride_type.circus";
        default:
            return "";
    }
}

static std::string GetRideTypeString(ObjectEntryIndex rideType)
{
    switch (rideType)
    {
        case RIDE_TYPE_CIRCUS:
            return "circus";
        default:
            return "";
    }
}

static std::string GetRideTypeCategoryString(uint8_t category)
{
    switch (category)
    {
        case RIDE_CATEGORY_GENTLE:
            return "gentle";
        default:
            return "";
    }
}

static std::string GetTrackElementString(uint64_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::FlatTrack3x3:
            return "flat_track_3x3";
        default:
            return "";
    }
}

static std::string GetTrackPaintFunctionString(TRACK_PAINT_FUNCTION_GETTER paintFunction)
{
    if (paintFunction == get_track_paint_function_circus)
    {
        return "circus";
    }
    return "";
}

static json_t GetRideFlagsJson(const uint64_t flags)
{
    json_t result;
    if (flags & RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION)
        result.push_back("has_single_piece_station");
    if (flags & RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS)
        result.push_back("cannot_have_gaps");
    if (flags & RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS)
        result.push_back("has_load_options");
    if (flags & RIDE_TYPE_FLAG_HAS_NO_TRACK)
        result.push_back("has_no_track");
    if (flags & RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL)
        result.push_back("vehicle_is_integral");
    if (flags & RIDE_TYPE_FLAG_TRACK_NO_WALLS)
        result.push_back("track_no_walls");
    if (flags & RIDE_TYPE_FLAG_FLAT_RIDE)
        result.push_back("flat_ride");
    if (flags & RIDE_TYPE_FLAG_IN_RIDE)
        result.push_back("in_ride");
    if (flags & RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS)
        result.push_back("has_vehicle_colours");
    if (flags & RIDE_TYPE_FLAG_MUSIC_ON_DEFAULT)
        result.push_back("music_on_default");
    if (flags & RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT)
        result.push_back("has_entrance_exit");
    if (flags & RIDE_TYPE_FLAG_SINGLE_SESSION)
        result.push_back("single_session");
    if (flags & RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY)
        result.push_back("list_vehicles_separately");
    return result;
}

static json_t GetRideModeFlagsJson(uint64_t mode)
{
    json_t result;
    if (mode & (1 << static_cast<uint64_t>(RideMode::Circus)))
        result.push_back("circus");
    return result;
}

static std::string GetRideModeString(RideMode mode)
{
    switch (mode)
    {
        case RideMode::Circus:
            return "circus";
        default:
            return "";
    }
}

static json_t GetRideTypeNamingJson(const RideNaming& naming)
{
    std::string name;
    std::string description;

    switch (naming.Name)
    {
        case STR_RIDE_NAME_CIRCUS:
            name = "circus";
            break;
        default:
            name = "";
            break;
    }

    switch (naming.Description)
    {
        case STR_RIDE_DESCRIPTION_CIRCUS:
            description = "circus";
            break;
        default:
            description = "";
            break;
    }

    json_t result;
    result["Name"] = name;
    result["Description"] = description;
    return result;
}

static json_t GetRideTypeNamingConventionJson(const RideNameConvention& nameConvention)
{
    json_t result;
    std::string vehicle, structure, station;

    switch (nameConvention.station)
    {
        case RideComponentType::Station:
            station = "station";
            break;
        default:
            station = "";
            break;
    }

    switch (nameConvention.structure)
    {
        case RideComponentType::Structure:
            structure = "structure";
            break;
        default:
            structure = "";
            break;
    }

    switch (nameConvention.vehicle)
    {
        case RideComponentType::Building:
            vehicle = "building";
            break;
        default:
            vehicle = "";
            break;
    }
    result["Vehicle"] = vehicle;
    result["Structure"] = structure;
    result["Station"] = station;
    return result;
}

static json_t GetRideTypeAvailableBreakdownsJson(uint8_t breakdowns)
{
    json_t result;
    if (breakdowns & (1 << BREAKDOWN_SAFETY_CUT_OUT))
        result.push_back("safety_cut_out");
    return result;
}

static std::string GetSoundIdString(OpenRCT2::Audio::SoundId sound)
{
    using namespace OpenRCT2::Audio;
    switch (sound)
    {
        case SoundId::LiftClassic:
            return "lift_classic";
        default:
            return "";
    }
}

static std::string GetRatingsCalculationFunctionString(ride_ratings_calculation func)
{
    if (func == ride_ratings_calculate_circus)
        return "circus";
    else
        return "";
}

static json_t GetDefaultPricesJson(const money16* prices)
{
    json_t res;
    res.push_back(prices[0]);
    res.push_back(prices[1]);
    return res;
}

static std::string GetShopItemString(ShopItem item)
{
    switch (item)
    {
        case ShopItem::Photo:
            return "photo";
        default:
            return "";
    }
}

static std::string GetColourString(const uint8_t colour)
{
    switch (colour)
    {
        case COLOUR_BRIGHT_RED:
            return "bright_red";
        case COLOUR_LIGHT_BLUE:
            return "light_blue";
        case COLOUR_YELLOW:
            return "yellow";
        default:
            return "";
    }
}

static json_t GetColourPresetsJson(const track_colour_preset_list& colours)
{
    json_t res;
    for (int i = 0; i < colours.count; i++)
    {
        auto trackColour = colours.list[i];
        json_t colourSet;
        colourSet["Main"] = GetColourString(trackColour.main);
        colourSet["Additional"] = GetColourString(trackColour.additional);
        colourSet["Supports"] = GetColourString(trackColour.supports);
        res.push_back(colourSet);
    }
    return res;
}

static json_t GetRideColourPreviewJson(const RideColourPreview& colourPreview)
{
    json_t res;
    res["Track"] = colourPreview.Track;
    res["Supports"] = colourPreview.Supports;
    return res;
}

static std::string GetRideColourKeyString(const RideColourKey& key)
{
    switch (key)
    {
        case RideColourKey::Ride:
            return "ride";
        default:
            return "";
    }
}

static json_t GetRatingsMultipliersJson(const RatingTuple& ratings)
{
    json_t res;
    res["Excitement"] = ratings.Excitement;
    res["Intensity"] = ratings.Intensity;
    res["Nausea"] = ratings.Nausea;
    return res;
}

json_t RideTypeDescriptorToJson(ObjectEntryIndex rideType)
{
    const auto& rtd = GetRideTypeDescriptor(rideType);

    json_t res;
    res["id"] = GetRideTypeId(rideType);
    res["authors"] = { "OpenRCT2 developers" };
    res["version"] = "1.0";
    res["sourceGame"] = "rct2";
    res["object_type"] = "ride_type";

    json_t properties;
    properties["RideType"] = GetRideTypeString(rideType);
    properties["AlternateType"] = GetRideTypeString(rtd.AlternateType);
    properties["Category"] = GetRideTypeCategoryString(rtd.Category);
    properties["EnabledTrackPieces"] = {};
    properties["StartTrackPiece"] = GetTrackElementString(rtd.StartTrackPiece);
    properties["TrackPaintFunction"] = GetTrackPaintFunctionString(rtd.TrackPaintFunction);
    properties["Flags"] = GetRideFlagsJson(rtd.Flags);
    properties["RideModes"] = GetRideModeFlagsJson(rtd.RideModes);
    properties["DefaultMode"] = GetRideModeString(rtd.DefaultMode);

    json_t operatingSettings;
    operatingSettings["MinValue"] = rtd.OperatingSettings.MinValue;
    operatingSettings["MaxValue"] = rtd.OperatingSettings.MaxValue;
    operatingSettings["MaxBrakesSpeed"] = rtd.OperatingSettings.MaxBrakesSpeed;
    operatingSettings["PoweredLiftAcceleration"] = rtd.OperatingSettings.PoweredLiftAcceleration;
    operatingSettings["BoosterAcceleration"] = rtd.OperatingSettings.BoosterAcceleration;
    operatingSettings["BoosterSpeedFactor"] = rtd.OperatingSettings.BoosterSpeedFactor;

    properties["OperatingSettings"] = operatingSettings;
    properties["Naming"] = GetRideTypeNamingJson(rtd.Naming);
    properties["NameConvention"] = GetRideTypeNamingConventionJson(rtd.NameConvention);
    properties["AvailableBreakdowns"] = GetRideTypeAvailableBreakdownsJson(rtd.AvailableBreakdowns);

    json_t heights;
    heights["MaxHeight"] = rtd.Heights.MaxHeight;
    heights["ClearanceHeight"] = rtd.Heights.ClearanceHeight;
    heights["VehicleZOffset"] = rtd.Heights.VehicleZOffset;
    heights["PlatformHeight"] = rtd.Heights.PlatformHeight;

    properties["Heights"] = heights;
    properties["MaxMass"] = rtd.MaxMass;

    json_t liftData;
    liftData["SoundId"] = GetSoundIdString(rtd.LiftData.sound_id);
    liftData["MinimumSpeed"] = rtd.LiftData.minimum_speed;
    liftData["MaximumSpeed"] = rtd.LiftData.maximum_speed;

    properties["LiftData"] = liftData;
    properties["RatingsCalculationFunction"] = GetRatingsCalculationFunctionString(rtd.RatingsCalculationFunction);

    properties["RatingsMultipliers"] = GetRatingsMultipliersJson(rtd.RatingsMultipliers);

    json_t upkeepCosts;
    upkeepCosts["BaseCost"] = rtd.UpkeepCosts.BaseCost;
    upkeepCosts["TrackLengthMultiplier"] = rtd.UpkeepCosts.TrackLengthMultiplier;
    upkeepCosts["CostPerTrackPiece"] = rtd.UpkeepCosts.CostPerTrackPiece;
    upkeepCosts["CostPerTrain"] = rtd.UpkeepCosts.CostPerTrain;
    upkeepCosts["CostPerCar"] = rtd.UpkeepCosts.CostPerCar;
    upkeepCosts["CostPerStation"] = rtd.UpkeepCosts.CostPerStation;

    properties["UpkeepCosts"] = upkeepCosts;

    json_t buildCosts;
    buildCosts["TrackPrice"] = rtd.BuildCosts.TrackPrice;
    buildCosts["SupportPrice"] = rtd.BuildCosts.SupportPrice;
    buildCosts["PriceEstimateMultiplier"] = rtd.BuildCosts.PriceEstimateMultiplier;

    properties["BuildCosts"] = buildCosts;
    properties["DefaultPrices"] = GetDefaultPricesJson(rtd.DefaultPrices);
    properties["DefaultMusic"] = rtd.DefaultMusic;
    properties["PhotoItem"] = GetShopItemString(rtd.PhotoItem);
    properties["BonusValue"] = rtd.BonusValue;
    properties["ColourPresets"] = GetColourPresetsJson(rtd.ColourPresets);
    properties["ColourPreview"] = GetRideColourPreviewJson(rtd.ColourPreview);
    properties["ColourKey"] = GetRideColourKeyString(rtd.ColourKey);

    res["properties"] = properties;
    return res;
}
