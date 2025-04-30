/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Tables.h"

#include "../Diagnostic.h"
#include "../core/Guard.hpp"
#include "../interface/Colour.h"
#include "../object/ObjectManager.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "RCT1.h"

#include <iterator>

// clang-format off
namespace OpenRCT2::RCT1
{
    colour_t GetColour(colour_t colour)
    {
        static constexpr uint8_t map[] =
        {
            COLOUR_BLACK,
            COLOUR_GREY,
            COLOUR_WHITE,
            COLOUR_LIGHT_PURPLE,
            COLOUR_BRIGHT_PURPLE,
            COLOUR_DARK_BLUE,
            COLOUR_LIGHT_BLUE,
            COLOUR_TEAL,
            COLOUR_SATURATED_GREEN,
            COLOUR_DARK_GREEN,
            COLOUR_MOSS_GREEN,
            COLOUR_BRIGHT_GREEN,
            COLOUR_OLIVE_GREEN,
            COLOUR_DARK_OLIVE_GREEN,
            COLOUR_YELLOW,
            COLOUR_DARK_YELLOW,
            COLOUR_LIGHT_ORANGE,
            COLOUR_DARK_ORANGE,
            COLOUR_LIGHT_BROWN,
            COLOUR_SATURATED_BROWN,
            COLOUR_DARK_BROWN,
            COLOUR_SALMON_PINK,
            COLOUR_BORDEAUX_RED,
            COLOUR_SATURATED_RED,
            COLOUR_BRIGHT_RED,
            COLOUR_BRIGHT_PINK,
            COLOUR_LIGHT_PINK,
            COLOUR_DARK_PINK,
            COLOUR_DARK_PURPLE,
            COLOUR_AQUAMARINE,
            COLOUR_BRIGHT_YELLOW,
            COLOUR_ICY_BLUE
        };
        if (colour >= std::size(map))
        {
            LOG_WARNING("Unsupported RCT1 colour.");
            return COLOUR_BLACK;
        }
        return map[colour];
    }

    RCT12PeepAnimationGroup GetPeepAnimationGroup(PeepAnimationGroup rct1AnimationGroup)
    {
        static constexpr RCT12PeepAnimationGroup map[] =
        {
            RCT12PeepAnimationGroup::Normal,               // 0x00
            RCT12PeepAnimationGroup::Handyman,             // 0x01
            RCT12PeepAnimationGroup::Mechanic,             // 0x02
            RCT12PeepAnimationGroup::Security,             // 0x03
            RCT12PeepAnimationGroup::EntertainerPanda,     // 0x04
            RCT12PeepAnimationGroup::EntertainerTiger,     // 0x05
            RCT12PeepAnimationGroup::EntertainerElephant,  // 0x06
            RCT12PeepAnimationGroup::EntertainerRoman,     // 0x07
            RCT12PeepAnimationGroup::EntertainerGorilla,   // 0x08
            RCT12PeepAnimationGroup::EntertainerSnowman,   // 0x09
            RCT12PeepAnimationGroup::EntertainerKnight,    // 0x0A
            RCT12PeepAnimationGroup::EntertainerAstronaut, // 0x0B
            RCT12PeepAnimationGroup::IceCream,             // 0x0C
            RCT12PeepAnimationGroup::Chips,                // 0x0D
            RCT12PeepAnimationGroup::Burger,               // 0x0E
            RCT12PeepAnimationGroup::Drink,                // 0x0F
            RCT12PeepAnimationGroup::Balloon,              // 0x10
            RCT12PeepAnimationGroup::Candyfloss,           // 0x11
            RCT12PeepAnimationGroup::Umbrella,             // 0x12
            RCT12PeepAnimationGroup::Pizza,                // 0x13
            RCT12PeepAnimationGroup::SecurityAlt,          // 0x14
            RCT12PeepAnimationGroup::Popcorn,              // 0x15
            RCT12PeepAnimationGroup::ArmsCrossed,          // 0x16
            RCT12PeepAnimationGroup::HeadDown,             // 0x17
            RCT12PeepAnimationGroup::Nauseous,             // 0x18
            RCT12PeepAnimationGroup::VeryNauseous,         // 0x19
            RCT12PeepAnimationGroup::RequireToilet,        // 0x1A
            RCT12PeepAnimationGroup::Hat,                  // 0x1B
            RCT12PeepAnimationGroup::HotDog,               // 0x1C
            RCT12PeepAnimationGroup::Tentacle,             // 0x1D
            RCT12PeepAnimationGroup::ToffeeApple,          // 0x1E
            RCT12PeepAnimationGroup::Doughnut,             // 0x1F
            RCT12PeepAnimationGroup::Coffee,               // 0x20
            RCT12PeepAnimationGroup::Chicken,              // 0x21
            RCT12PeepAnimationGroup::Lemonade,             // 0x22
        };
        if (EnumValue(rct1AnimationGroup) >= std::size(map))
        {
            LOG_WARNING("Unsupported RCT1 peep sprite type: %d.", EnumValue(rct1AnimationGroup));
            return RCT12PeepAnimationGroup::Normal;
        }
        return map[EnumValue(rct1AnimationGroup)];
    }

    std::string_view GetTerrainSurfaceObject(uint8_t terrainSurface)
    {
        static constexpr std::string_view map[Limits::kNumTerrainSurfaces] =
        {
            "rct2.terrain_surface.grass",
            "rct2.terrain_surface.sand",
            "rct2.terrain_surface.dirt",
            "rct2.terrain_surface.rock",
            "rct2.terrain_surface.martian",
            "rct2.terrain_surface.chequerboard",
            "rct2.terrain_surface.grass_clumps",
            "rct1aa.terrain_surface.roof_red",
            "rct2.terrain_surface.ice",
            "rct1ll.terrain_surface.wood",
            "rct1ll.terrain_surface.rust",
            "rct1ll.terrain_surface.roof_grey",
            "rct2.terrain_surface.grid_red",
            "rct2.terrain_surface.grid_yellow",
            "rct2.terrain_surface.grid_purple",
            "rct2.terrain_surface.grid_green",
        };
        return terrainSurface < std::size(map) ? map[terrainSurface] : map[0];
    }

    std::string_view GetTerrainEdgeObject(uint8_t terrainEdge)
    {
        static constexpr std::string_view map[Limits::kNumTerrainEdges] =
        {
            "rct2.terrain_edge.rock",
            "rct1.terrain_edge.brick",
            "rct1.terrain_edge.iron",
            "rct2.terrain_edge.wood_red",
            "rct1aa.terrain_edge.grey",
            "rct1aa.terrain_edge.yellow",
            "rct2.terrain_edge.wood_black",
            "rct1aa.terrain_edge.red",
            "rct2.terrain_edge.ice",
            "rct1ll.terrain_edge.purple",
            "rct1ll.terrain_edge.green",
            "rct1ll.terrain_edge.stone_brown",
            "rct1ll.terrain_edge.stone_grey",
            "rct1ll.terrain_edge.skyscraper_a",
            "rct1ll.terrain_edge.skyscraper_b",
        };
        return terrainEdge < std::size(map) ? map[terrainEdge] : map[0];
    }

    uint8_t GetRideType(RideType rideType, VehicleType vehicleType)
    {
        if (rideType == RideType::SteelTwisterRollerCoaster && vehicleType == VehicleType::NonLoopingSteelTwisterRollerCoasterTrain)
            return RIDE_TYPE_HYPER_TWISTER;
        if (rideType == RideType::SteelCorkscrewRollerCoaster && vehicleType == VehicleType::HypercoasterTrain)
            return RIDE_TYPE_HYPERCOASTER;

        static uint8_t map[] =
        {
            RIDE_TYPE_CLASSIC_WOODEN_ROLLER_COASTER,         // RCT1_RIDE_TYPE_WOODEN_ROLLER_COASTER
            RIDE_TYPE_CLASSIC_STAND_UP_ROLLER_COASTER,       // RCT1_RIDE_TYPE_STAND_UP_STEEL_ROLLER_COASTER
            RIDE_TYPE_SUSPENDED_SWINGING_COASTER,            // RCT1_RIDE_TYPE_SUSPENDED_ROLLER_COASTER
            RIDE_TYPE_INVERTED_ROLLER_COASTER,               // RCT1_RIDE_TYPE_INVERTED_ROLLER_COASTER
            RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER,           // RCT1_RIDE_TYPE_STEEL_MINI_ROLLER_COASTER
            RIDE_TYPE_MINIATURE_RAILWAY,                     // RCT1_RIDE_TYPE_MINIATURE_RAILWAY
            RIDE_TYPE_MONORAIL,                              // RCT1_RIDE_TYPE_MONORAIL
            RIDE_TYPE_MINI_SUSPENDED_COASTER,                // RCT1_RIDE_TYPE_SUSPENDED_SINGLE_RAIL_ROLLER_COASTER
            RIDE_TYPE_BOAT_HIRE,                             // RCT1_RIDE_TYPE_BOAT_HIRE
            RIDE_TYPE_WOODEN_WILD_MOUSE,                     // RCT1_RIDE_TYPE_WOODEN_CRAZY_RODENT_ROLLER_COASTER
            RIDE_TYPE_STEEPLECHASE,                          // RCT1_RIDE_TYPE_SINGLE_RAIL_ROLLER_COASTER
            RIDE_TYPE_CAR_RIDE,                              // RCT1_RIDE_TYPE_CAR_RIDE
            RIDE_TYPE_LAUNCHED_FREEFALL,                     // RCT1_RIDE_TYPE_LAUNCHED_FREEFALL
            RIDE_TYPE_BOBSLEIGH_COASTER,                     // RCT1_RIDE_TYPE_BOBSLED_ROLLER_COASTER
            RIDE_TYPE_OBSERVATION_TOWER,                     // RCT1_RIDE_TYPE_OBSERVATION_TOWER
            RIDE_TYPE_LOOPING_ROLLER_COASTER,                // RCT1_RIDE_TYPE_STEEL_ROLLER_COASTER
            RIDE_TYPE_DINGHY_SLIDE,                          // RCT1_RIDE_TYPE_WATER_SLIDE
            RIDE_TYPE_MINE_TRAIN_COASTER,                    // RCT1_RIDE_TYPE_MINE_TRAIN_ROLLER_COASTER
            RIDE_TYPE_CHAIRLIFT,                             // RCT1_RIDE_TYPE_CHAIRLIFT
            RIDE_TYPE_CORKSCREW_ROLLER_COASTER,              // RCT1_RIDE_TYPE_STEEL_CORKSCREW_ROLLER_COASTER
            RIDE_TYPE_MAZE,                                  // RCT1_RIDE_TYPE_HEDGE_MAZE
            RIDE_TYPE_SPIRAL_SLIDE,                          // RCT1_RIDE_TYPE_SPIRAL_SLIDE
            RIDE_TYPE_GO_KARTS,                              // RCT1_RIDE_TYPE_GO_KARTS
            RIDE_TYPE_LOG_FLUME,                             // RCT1_RIDE_TYPE_LOG_FLUME
            RIDE_TYPE_RIVER_RAPIDS,                          // RCT1_RIDE_TYPE_RIVER_RAPIDS
            RIDE_TYPE_DODGEMS,                               // RCT1_RIDE_TYPE_DODGEMS
            RIDE_TYPE_SWINGING_SHIP,                         // RCT1_RIDE_TYPE_SWINGING_SHIP
            RIDE_TYPE_SWINGING_INVERTER_SHIP,                // RCT1_RIDE_TYPE_SWINGING_INVERTER_SHIP
            RIDE_TYPE_FOOD_STALL,                            // RCT1_RIDE_TYPE_ICE_CREAM_STALL
            RIDE_TYPE_FOOD_STALL,                            // RCT1_RIDE_TYPE_CHIPS_STALL
            RIDE_TYPE_DRINK_STALL,                           // RCT1_RIDE_TYPE_DRINK_STALL
            RIDE_TYPE_FOOD_STALL,                            // RCT1_RIDE_TYPE_CANDYFLOSS_STALL
            RIDE_TYPE_SHOP,                                  // RCT1_RIDE_TYPE_BURGER_BAR
            RIDE_TYPE_MERRY_GO_ROUND,                        // RCT1_RIDE_TYPE_MERRY_GO_ROUND
            RIDE_TYPE_SHOP,                                  // RCT1_RIDE_TYPE_BALLOON_STALL
            RIDE_TYPE_INFORMATION_KIOSK,                     // RCT1_RIDE_TYPE_INFORMATION_KIOSK
            RIDE_TYPE_TOILETS,                               // RCT1_RIDE_TYPE_TOILETS
            RIDE_TYPE_FERRIS_WHEEL,                          // RCT1_RIDE_TYPE_FERRIS_WHEEL
            RIDE_TYPE_MOTION_SIMULATOR,                      // RCT1_RIDE_TYPE_MOTION_SIMULATOR
            RIDE_TYPE_3D_CINEMA,                             // RCT1_RIDE_TYPE_3D_CINEMA
            RIDE_TYPE_TOP_SPIN,                              // RCT1_RIDE_TYPE_TOP_SPIN
            RIDE_TYPE_SPACE_RINGS,                           // RCT1_RIDE_TYPE_SPACE_RINGS
            RIDE_TYPE_REVERSE_FREEFALL_COASTER,              // RCT1_RIDE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER
            RIDE_TYPE_SHOP,                                  // RCT1_RIDE_TYPE_SOUVENIR_STALL
            RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER,          // RCT1_RIDE_TYPE_VERTICAL_ROLLER_COASTER
            RIDE_TYPE_FOOD_STALL,                            // RCT1_RIDE_TYPE_PIZZA_STALL
            RIDE_TYPE_TWIST,                                 // RCT1_RIDE_TYPE_TWIST
            RIDE_TYPE_HAUNTED_HOUSE,                         // RCT1_RIDE_TYPE_HAUNTED_HOUSE
            RIDE_TYPE_FOOD_STALL,                            // RCT1_RIDE_TYPE_POPCORN_STALL
            RIDE_TYPE_CIRCUS,                                // RCT1_RIDE_TYPE_CIRCUS
            RIDE_TYPE_GHOST_TRAIN,                           // RCT1_RIDE_TYPE_GHOST_TRAIN
            RIDE_TYPE_TWISTER_ROLLER_COASTER,                // RCT1_RIDE_TYPE_STEEL_TWISTER_ROLLER_COASTER
            RIDE_TYPE_CLASSIC_WOODEN_TWISTER_ROLLER_COASTER, // RCT1_RIDE_TYPE_WOODEN_TWISTER_ROLLER_COASTER
            RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER,          // RCT1_RIDE_TYPE_WOODEN_SIDE_FRICTION_ROLLER_COASTER
            RIDE_TYPE_STEEL_WILD_MOUSE,                      // RCT1_RIDE_TYPE_STEEL_WILD_MOUSE_ROLLER_COASTER
            RIDE_TYPE_FOOD_STALL,                            // RCT1_RIDE_TYPE_HOT_DOG_STALL
            RIDE_TYPE_FOOD_STALL,                            // RCT1_RIDE_TYPE_EXOTIC_SEA_FOOD_STALL
            RIDE_TYPE_SHOP,                                  // RCT1_RIDE_TYPE_HAT_STALL
            RIDE_TYPE_FOOD_STALL,                            // RCT1_RIDE_TYPE_TOFFEE_APPLE_STALL
            RIDE_TYPE_VIRGINIA_REEL,                         // RCT1_RIDE_TYPE_VIRGINIA_REEL
            RIDE_TYPE_SPLASH_BOATS,                          // RCT1_RIDE_TYPE_RIVER_RIDE
            RIDE_TYPE_MINI_HELICOPTERS,                      // RCT1_RIDE_TYPE_CYCLE_MONORAIL
            RIDE_TYPE_LAY_DOWN_ROLLER_COASTER,               // RCT1_RIDE_TYPE_FLYING_ROLLER_COASTER
            RIDE_TYPE_SUSPENDED_MONORAIL,                    // RCT1_RIDE_TYPE_SUSPENDED_MONORAIL
            kRideTypeNull,                                  // RCT1_RIDE_TYPE_40
            RIDE_TYPE_REVERSER_ROLLER_COASTER,               // RCT1_RIDE_TYPE_WOODEN_REVERSER_ROLLER_COASTER
            RIDE_TYPE_HEARTLINE_TWISTER_COASTER,             // RCT1_RIDE_TYPE_HEARTLINE_TWISTER_ROLLER_COASTER
            RIDE_TYPE_MINI_GOLF,                             // RCT1_RIDE_TYPE_MINIATURE_GOLF
            kRideTypeNull,                                  // RCT1_RIDE_TYPE_44
            RIDE_TYPE_ROTO_DROP,                             // RCT1_RIDE_TYPE_ROTO_DROP
            RIDE_TYPE_FLYING_SAUCERS,                        // RCT1_RIDE_TYPE_FLYING_SAUCERS
            RIDE_TYPE_CROOKED_HOUSE,                         // RCT1_RIDE_TYPE_CROOKED_HOUSE
            RIDE_TYPE_MONORAIL_CYCLES,                       // RCT1_RIDE_TYPE_CYCLE_RAILWAY
            RIDE_TYPE_COMPACT_INVERTED_COASTER,              // RCT1_RIDE_TYPE_SUSPENDED_LOOPING_ROLLER_COASTER
            RIDE_TYPE_WATER_COASTER,                         // RCT1_RIDE_TYPE_WATER_COASTER
            RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER,          // RCT1_RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
            RIDE_TYPE_INVERTED_HAIRPIN_COASTER,              // RCT1_RIDE_TYPE_INVERTED_WILD_MOUSE_COASTER
            RIDE_TYPE_BOAT_HIRE,                             // RCT1_RIDE_TYPE_JET_SKIS
            RIDE_TYPE_SHOP,                                  // RCT1_RIDE_TYPE_T_SHIRT_STALL
            RIDE_TYPE_RIVER_RAFTS,                           // RCT1_RIDE_TYPE_RAFT_RIDE
            RIDE_TYPE_FOOD_STALL,                            // RCT1_RIDE_TYPE_DOUGHNUT_SHOP
            RIDE_TYPE_ENTERPRISE,                            // RCT1_RIDE_TYPE_ENTERPRISE
            RIDE_TYPE_DRINK_STALL,                           // RCT1_RIDE_TYPE_COFFEE_SHOP
            RIDE_TYPE_FOOD_STALL,                            // RCT1_RIDE_TYPE_FRIED_CHICKEN_STALL
            RIDE_TYPE_DRINK_STALL,                           // RCT1_RIDE_TYPE_LEMONADE_STALL
        };

        const auto index = EnumValue(rideType);
        Guard::ArgumentInRange<size_t>(index, 0, std::size(map) - 1, "Unsupported RCT1 ride type.");

        return map[index];
    }

    VehicleColourSchemeCopyDescriptor GetColourSchemeCopyDescriptor(VehicleType vehicleType)
    {
        static VehicleColourSchemeCopyDescriptor map[89] =
        {
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN = 0,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN_BACKWARDS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 },    // RCT1_VEHICLE_TYPE_INVERTED_COASTER_TRAIN, // Not in RCT2
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 },    // RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_LADYBIRD_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 },    // RCT1_VEHICLE_TYPE_STANDUP_ROLLER_COASTER_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_SPINNING_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_SINGLE_PERSON_SWINGING_CHAIRS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_YELLOW },    // RCT1_VEHICLE_TYPE_SWANS_PEDAL_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_DARK_BLUE }, // RCT1_VEHICLE_TYPE_LARGE_MONORAIL_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_CANOES,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_ROWING_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_YELLOW },    // RCT1_VEHICLE_TYPE_STEAM_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 },    // RCT1_VEHICLE_TYPE_WOODEN_MOUSE_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_BUMPER_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN_BACKWARDS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_ROCKET_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_HORSES, // Steeplechase
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_SPORTSCARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_LYING_DOWN_SWINGING_CARS, // Inverted single-rail
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_WOODEN_MINE_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 },    // RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_AIRPLANE_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_SMALL_MONORAIL_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_WATER_TRICYCLES,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_LAUNCHED_FREEFALL_CAR,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_BOBSLEIGH_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_DINGHIES,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_ROTATING_CABIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_MINE_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_CORKSCREW_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_MOTORBIKES,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_RACING_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_TRUCKS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_GO_KARTS,
            { COPY_COLOUR_1, COLOUR_BLACK, COLOUR_BLACK },      // RCT1_VEHICLE_TYPE_RAPIDS_BOATS,
            { COPY_COLOUR_1, COLOUR_BLACK, COLOUR_BLACK },      // RCT1_VEHICLE_TYPE_LOG_FLUME_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_DODGEMS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_SWINGING_SHIP,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_SWINGING_INVERTER_SHIP,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_MERRY_GO_ROUND,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_FERRIS_WHEEL,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_SIMULATOR_POD,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_CINEMA_BUILDING,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_TOPSPIN_CAR,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_SPACE_RINGS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_1 },    // RCT1_VEHICLE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER_CAR,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_1 },    // RCT1_VEHICLE_TYPE_VERTICAL_ROLLER_COASTER_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_CAT_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_TWIST_ARMS_AND_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_HAUNTED_HOUSE_BUILDING,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_LOG_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_CIRCUS_TENT,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_GHOST_TRAIN_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 },    // RCT1_VEHICLE_TYPE_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_WOODEN_TWISTER_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_WOODEN_SIDE_FRICTION_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_1, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_VINTAGE_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_YELLOW },    // RCT1_VEHICLE_TYPE_STEAM_TRAIN_COVERED_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 },    // RCT1_VEHICLE_TYPE_STAND_UP_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 },    // RCT1_VEHICLE_TYPE_FLOORLESS_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_STEEL_MOUSE_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS_ALTERNATIVE,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_SUSPENDED_MONORAIL_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_HELICOPTER_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_VIRGINIA_REEL_TUBS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_REVERSER_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_GOLFERS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_RIVER_RIDE_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 },    // RCT1_VEHICLE_TYPE_FLYING_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_1 },    // RCT1_VEHICLE_TYPE_NON_LOOPING_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS_REVERSED,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_RESERVED,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_ROTODROP_CAR,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_FLYING_SAUCERS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_CROOKED_HOUSE_BUILDING,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_BICYCLES,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_HYPERCOASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_4_ACROSS_INVERTED_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_WATER_COASTER_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 },    // RCT1_VEHICLE_TYPE_FACEOFF_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_JET_SKIS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 },    // RCT1_VEHICLE_TYPE_RAFT_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_YELLOW },    // RCT1_VEHICLE_TYPE_AMERICAN_STYLE_STEAM_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_AIR_POWERED_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_SUSPENDED_WILD_MOUSE_CARS, // Inverted Hairpin in RCT2
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK },     // RCT1_VEHICLE_TYPE_ENTERPRISE_WHEEL
        };

        Guard::ArgumentInRange<size_t>(EnumValue(vehicleType), 0, std::size(map) - 1, "Unsupported RCT1 vehicle type.");
        return map[EnumValue(vehicleType)];
    }

    bool RideTypeUsesVehicles(RideType rideType)
    {
        switch (rideType) {
        case RideType::HedgeMaze:
        case RideType::SpiralSlide:
        case RideType::IceCreamStall:
        case RideType::ChipsStall:
        case RideType::DrinkStall:
        case RideType::CandyflossStall:
        case RideType::BurgerBar:
        case RideType::BalloonStall:
        case RideType::InformationKiosk:
        case RideType::Toilets:
        case RideType::SouvenirStall:
        case RideType::PizzaStall:
        case RideType::PopcornStall:
        case RideType::HotDogStall:
        case RideType::ExoticSeaFoodStall:
        case RideType::HatStall:
        case RideType::ToffeeAppleStall:
        case RideType::_40:
        case RideType::_44:
        case RideType::TShirtStall:
        case RideType::DoughnutShop:
        case RideType::CoffeeShop:
        case RideType::FriedChickenStall:
        case RideType::LemonadeStall:
            return false;
        default:
            return true;
        }
    }

    bool PathIsQueue(uint8_t pathType)
    {
        switch (pathType) {
        case RCT1_FOOTPATH_TYPE_QUEUE_BLUE:
        case RCT1_FOOTPATH_TYPE_QUEUE_RED:
        case RCT1_FOOTPATH_TYPE_QUEUE_YELLOW:
        case RCT1_FOOTPATH_TYPE_QUEUE_GREEN:
            return true;
        }
        return false;
    }

    uint8_t NormalisePathAddition(uint8_t pathAdditionType)
    {
        switch (pathAdditionType) {
        case RCT1_PATH_ADDITION_BROKEN_LAMP_1: return RCT1_PATH_ADDITION_LAMP_1;
        case RCT1_PATH_ADDITION_BROKEN_LAMP_2: return RCT1_PATH_ADDITION_LAMP_2;
        case RCT1_PATH_ADDITION_BROKEN_BIN:    return RCT1_PATH_ADDITION_BIN;
        case RCT1_PATH_ADDITION_BROKEN_BENCH:  return RCT1_PATH_ADDITION_BENCH;
        case RCT1_PATH_ADDITION_BROKEN_LAMP_3: return RCT1_PATH_ADDITION_LAMP_3;
        case RCT1_PATH_ADDITION_BROKEN_LAMP_4: return RCT1_PATH_ADDITION_LAMP_4;
        }
        return pathAdditionType;
    }

    uint8_t GetVehicleSubEntryIndex(VehicleType rct1VehicleType, uint8_t vehicleSubEntry)
    {
        static constexpr uint8_t map[] =
        {
            0, // STEEL_RC_FRONT
            1, // STEEL_RC_CARRIAGE
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0, // MONORAIL_CARRIAGE
            1, // MONORAIL_FRONT
            2, // MONORAIL_BACK
            0,
            0,
            1, // MINIATURE_RAILWAY_TENDER
            0, // MINIATURE_RAILWAY_LOCOMOTIVE
            2, // MINIATURE_RAILWAY_CARRIAGE
            0,
            0,
            0, // WOODEN_RC_TRAIN_BACKWARDS
            0,
            0,
            0, // STEEL_RC_REVERSED_FRONT
            1, // STEEL_RC_REVERSED_CARRIAGE
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0, // MINE_TRAIN_FRONT
            1, // MINE_TRAIN_CARRIAGE
            0,
            0, // CORKSCREW_RC_FRONT
            1, // CORKSCREW_RC_CARRIAGE
            0,
            0,
            0,
            0,
            0,
            0, // LOG_FLUME_BOAT
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1, // LOG_FLUME_BOAT_REVERSED
            0,
            0, // GHOST_TRAIN_CAR
            1, // TWISTER_RC_SPOILER
            0, // TWISTER_RC_CARRIAGE
            1, // GHOST_TRAIN_INVISIBLE
            0, // ARTICULATED_RC_FRONT
            1, // ARTICULATED_RC_CARRIAGE
            0,
            0,
            2, // MINIATURE_RAILWAY_CARRIAGE_COVERED
            0, // STANDUP_TWISTER_RC_CARRIAGE
            0,
            0,
            0,
            0,
            0,
            0,
            0, // REVERSER_RC_CAR
            2, // REVERSER_RC_BOGIE
            1, // MINIGOLF_PLAYER
            0, // MINIGOLF_BALL
            0, // SPLASH_BOAT
            1, // SPLASH_BOAT_INVISIBLE
            0,
            0,
            0,
            0, // HEARTLINE_TWISTER_FORWARDS
            0, // HEARTLINE_TWISTER_BACKWARDS
            0,
            0,
            0,
            0,
            0,
            1, // REVERSER_RC_CAR_REVERSED
            0, // HYPERCOASTER_FRONT
            1, // HYPERCOASTER_CARRIAGE
            0, // INVERTED_4_ACROSS_CARRIAGE
            0, // WATER_COASTER_BOAT
            0,
            1, // WATER_COASTER_INVISIBLE
            0,
            0, // RIVER_RAFT
            1, // MINIATURE_RAILWAY_AMERICAN_TENDER
            0, // MINIATURE_RAILWAY_AMERICAN_LOCOMOTIVE
            1, // AIR_POWERED_VERTICAL_COASTER_TRAIN_FRONT
            0, // AIR_POWERED_VERTICAL_COASTER_TRAIN_CAR
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        };

        if (rct1VehicleType == VehicleType::HeartlineTwisterCars)
        {
            return vehicleSubEntry == HEARTLINE_TWISTER_FORWARDS ? 0 : 1;
        }
        if (rct1VehicleType == VehicleType::HeartlineTwisterCarsReversed)
        {
            return vehicleSubEntry == HEARTLINE_TWISTER_BACKWARDS ? 0 : 1;
        }

        return map[vehicleSubEntry];
    }

    std::string_view GetRideTypeObject(RideType rideType, bool isLL)
    {
        if (rideType == RideType::InvertedRollerCoaster && !isLL) {
            return "rct1.ride.inverted_trains";
        }

        static constexpr const char * map[] =
        {
            "rct1.ride.wooden_rc_trains",            // RCT1_RIDE_TYPE_WOODEN_ROLLER_COASTER
            "rct1.ride.stand_up_trains",             // RCT1_RIDE_TYPE_STAND_UP_STEEL_ROLLER_COASTER
            "rct1.ride.suspended_swinging_cars",     // RCT1_RIDE_TYPE_SUSPENDED_ROLLER_COASTER
            "rct1ll.ride.4_across_inverted_trains",  // RCT1_RIDE_TYPE_INVERTED_ROLLER_COASTER
            "rct1.ride.ladybird_trains",             // RCT1_RIDE_TYPE_STEEL_MINI_ROLLER_COASTER
            "rct1.ride.steam_trains",                // RCT1_RIDE_TYPE_MINIATURE_RAILWAY
            "rct1.ride.small_monorail_cars",         // RCT1_RIDE_TYPE_MONORAIL
            "rct1.ride.single_person_swinging_cars", // RCT1_RIDE_TYPE_SUSPENDED_SINGLE_RAIL_ROLLER_COASTER
            "rct2.ride.rboat",                       // RCT1_RIDE_TYPE_BOAT_HIRE
            "rct1.ride.mouse_cars",                  // RCT1_RIDE_TYPE_WOODEN_CRAZY_RODENT_ROLLER_COASTER
            "rct1.ride.horses",                      // RCT1_RIDE_TYPE_SINGLE_RAIL_ROLLER_COASTER
            "rct1.ride.sports_cars",                 // RCT1_RIDE_TYPE_CAR_RIDE
            "rct2.ride.ssc1",                        // RCT1_RIDE_TYPE_LAUNCHED_FREEFALL
            "rct1.ride.bobsleigh_trains",            // RCT1_RIDE_TYPE_BOBSLED_ROLLER_COASTER
            "rct2.ride.obs1",                        // RCT1_RIDE_TYPE_OBSERVATION_TOWER
            "rct1.ride.steel_rc_trains",             // RCT1_RIDE_TYPE_STEEL_ROLLER_COASTER
            "rct1.ride.dinghies",                    // RCT1_RIDE_TYPE_WATER_SLIDE
            "rct1.ride.mine_trains",                 // RCT1_RIDE_TYPE_MINE_TRAIN_ROLLER_COASTER
            "rct1.ride.chairlift_cars",              // RCT1_RIDE_TYPE_CHAIRLIFT
            "rct1.ride.corkscrew_trains",            // RCT1_RIDE_TYPE_STEEL_CORKSCREW_ROLLER_COASTER
            "rct2.ride.hmaze",                       // RCT1_RIDE_TYPE_HEDGE_MAZE
            "rct2.ride.hskelt",                      // RCT1_RIDE_TYPE_SPIRAL_SLIDE
            "rct1.ride.go_karts",                    // RCT1_RIDE_TYPE_GO_KARTS
            "rct1.ride.logs",                        // RCT1_RIDE_TYPE_LOG_FLUME
            "rct1.ride.river_rapids_boats",          // RCT1_RIDE_TYPE_RIVER_RAPIDS
            "rct1.ride.dodgems",                     // RCT1_RIDE_TYPE_DODGEMS
            "rct2.ride.swsh1",                       // RCT1_RIDE_TYPE_SWINGING_SHIP
            "rct2.ride.swsh2",                       // RCT1_RIDE_TYPE_SWINGING_INVERTER_SHIP
            "rct1.ride.fruity_ices_stall",           // RCT1_RIDE_TYPE_ICE_CREAM_STALL
            "rct2.ride.chpsh",                       // RCT1_RIDE_TYPE_CHIPS_STALL
            "rct2.ride.drnks",                       // RCT1_RIDE_TYPE_DRINK_STALL
            "rct2.ride.cndyf",                       // RCT1_RIDE_TYPE_CANDYFLOSS_STALL
            "rct2.ride.burgb",                       // RCT1_RIDE_TYPE_BURGER_BAR
            "rct2.ride.mgr1",                        // RCT1_RIDE_TYPE_MERRY_GO_ROUND
            "rct2.ride.balln",                       // RCT1_RIDE_TYPE_BALLOON_STALL
            "rct2.ride.infok",                       // RCT1_RIDE_TYPE_INFORMATION_KIOSK
            "rct1.ride.toilets",                     // RCT1_RIDE_TYPE_TOILETS
            "rct2.ride.fwh1",                        // RCT1_RIDE_TYPE_FERRIS_WHEEL
            "rct2.ride.simpod",                      // RCT1_RIDE_TYPE_MOTION_SIMULATOR
            "rct2.ride.c3d",                         // RCT1_RIDE_TYPE_3D_CINEMA
            "rct2.ride.topsp1",                      // RCT1_RIDE_TYPE_TOP_SPIN
            "rct2.ride.srings",                      // RCT1_RIDE_TYPE_SPACE_RINGS
            "rct1.ride.reverse_freefall_car",        // RCT1_RIDE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER
            "rct2.ride.souvs",                       // RCT1_RIDE_TYPE_SOUVENIR_STALL
            "rct1.ride.vertical_drop_trains",        // RCT1_RIDE_TYPE_VERTICAL_ROLLER_COASTER
            "rct2.ride.pizzs",                       // RCT1_RIDE_TYPE_PIZZA_STALL
            "rct2.ride.twist1",                      // RCT1_RIDE_TYPE_TWIST
            "rct2.ride.hhbuild",                     // RCT1_RIDE_TYPE_HAUNTED_HOUSE
            "rct2.ride.popcs",                       // RCT1_RIDE_TYPE_POPCORN_STALL
            "rct2.ride.circus1",                     // RCT1_RIDE_TYPE_CIRCUS
            "rct1aa.ride.ghost_train_cars",          // RCT1_RIDE_TYPE_GHOST_TRAIN
            "rct1aa.ride.twister_trains",            // RCT1_RIDE_TYPE_STEEL_TWISTER_ROLLER_COASTER
            "rct1aa.ride.wooden_articulated_trains", // RCT1_RIDE_TYPE_WOODEN_TWISTER_ROLLER_COASTER
            "rct1aa.ride.side_friction_cars",        // RCT1_RIDE_TYPE_WOODEN_SIDE_FRICTION_ROLLER_COASTER
            "rct1aa.ride.steel_wild_mouse_cars",     // RCT1_RIDE_TYPE_STEEL_WILD_MOUSE_ROLLER_COASTER
            "rct2.ride.hotds",                       // RCT1_RIDE_TYPE_HOT_DOG_STALL
            "rct2.ride.sqdst",                       // RCT1_RIDE_TYPE_EXOTIC_SEA_FOOD_STALL
            "rct2.ride.hatst",                       // RCT1_RIDE_TYPE_HAT_STALL
            "rct2.ride.toffs",                       // RCT1_RIDE_TYPE_TOFFEE_APPLE_STALL
            "rct1aa.ride.virginia_reel_tubs",        // RCT1_RIDE_TYPE_VIRGINIA_REEL
            "rct1aa.ride.splash_boats",              // RCT1_RIDE_TYPE_RIVER_RIDE
            "rct1aa.ride.mini_helicopters",          // RCT1_RIDE_TYPE_CYCLE_MONORAIL
            "rct1aa.ride.lay_down_trains",           // RCT1_RIDE_TYPE_FLYING_ROLLER_COASTER
            "rct1aa.ride.suspended_monorail_trains", // RCT1_RIDE_TYPE_SUSPENDED_MONORAIL
            "",                                      // RCT1_RIDE_TYPE_40
            "rct1aa.ride.reverser_cars",             // RCT1_RIDE_TYPE_WOODEN_REVERSER_ROLLER_COASTER
            "rct1aa.ride.heartline_twister_cars",    // RCT1_RIDE_TYPE_HEARTLINE_TWISTER_ROLLER_COASTER
            "rct2.ride.golf1",                       // RCT1_RIDE_TYPE_MINIATURE_GOLF
            "",                                      // RCT1_RIDE_TYPE_44
            "rct2.ride.gdrop1",                      // RCT1_RIDE_TYPE_ROTO_DROP
            "rct1aa.ride.flying_saucers",            // RCT1_RIDE_TYPE_FLYING_SAUCERS
            "rct2.ride.chbuild",                     // RCT1_RIDE_TYPE_CROOKED_HOUSE
            "rct1aa.ride.bicycles",                  // RCT1_RIDE_TYPE_CYCLE_RAILWAY
            "rct1.ride.inverted_trains",             // RCT1_RIDE_TYPE_SUSPENDED_LOOPING_ROLLER_COASTER
            "rct1ll.ride.coaster_boats",             // RCT1_RIDE_TYPE_WATER_COASTER
            "rct1ll.ride.air_powered_trains",        // RCT1_RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
            "rct1ll.ride.inverted_hairpin_cars",     // RCT1_RIDE_TYPE_INVERTED_WILD_MOUSE_COASTER
            "rct1ll.ride.jet_skis",                  // RCT1_RIDE_TYPE_JET_SKIS
            "rct2.ride.tshrt",                       // RCT1_RIDE_TYPE_T_SHIRT_STALL
            "rct1ll.ride.rafts",                     // RCT1_RIDE_TYPE_RAFT_RIDE
            "rct2.ride.dough",                       // RCT1_RIDE_TYPE_DOUGHNUT_SHOP
            "rct2.ride.enterp",                      // RCT1_RIDE_TYPE_ENTERPRISE
            "rct2.ride.coffs",                       // RCT1_RIDE_TYPE_COFFEE_SHOP
            "rct2.ride.chcks",                       // RCT1_RIDE_TYPE_FRIED_CHICKEN_STALL
            "rct2.ride.lemst",                       // RCT1_RIDE_TYPE_LEMONADE_STALL
        };

        const auto index = EnumValue(rideType);
        Guard::ArgumentInRange<size_t>(index, 0, std::size(map) - 1, "Unsupported RCT1 ride type.");

        return map[index];
    }

    std::string_view GetVehicleObject(VehicleType vehicleType)
    {
        static constexpr const char * map[] =
        {
            "rct1.ride.steel_rc_trains",                   //  VehicleType::SteelRollerCoasterTrain
            "rct1.ride.steel_rc_trains",                   //  VehicleType::SteelRollerCoasterTrainBackwards
            "rct1.ride.wooden_rc_trains",                  //  VehicleType::WoodenRollerCoasterTrain
            "rct1.ride.inverted_trains",                   //  VehicleType::InvertedCoasterTrain//NotinRCT2
            "rct1.ride.suspended_swinging_cars",           //  VehicleType::SuspendedSwingingCars
            "rct1.ride.ladybird_trains",                   //  VehicleType::LadybirdCars
            "rct1.ride.stand_up_trains",                   //  VehicleType::StandupRollerCoasterCars
            "rct2.ride.wmspin",                            //  VehicleType::SpinningCars
            "rct1.ride.single_person_swinging_cars",       //  VehicleType::SinglePersonSwingingChairs
            "rct2.ride.swans",                             //  VehicleType::SwansPedalBoats
            "rct1.ride.streamlined_monorail_trains",       //  VehicleType::LargeMonorailTrain
            "rct2.ride.cboat",                             //  VehicleType::Canoes
            "rct2.ride.rboat",                             //  VehicleType::RowingBoats
            "rct1.ride.steam_trains",                      //  VehicleType::SteamTrain
            "rct1.ride.mouse_cars",                        //  VehicleType::WoodenMouseCars
            "rct1.ride.bumper_boats",                      //  VehicleType::BumperBoats
            "rct1.ride.wooden_rc_trains",                  //  VehicleType::WoodenRollerCoasterTrainBackwards
            "rct1.ride.rocket_cars",                       //  VehicleType::RocketCars
            "rct1.ride.horses",                            //  VehicleType::Horses//Steeplechase
            "rct1.ride.sports_cars",                       //  VehicleType::Sportscars
            "rct1.ride.swinging_lay_down_cars",            //  VehicleType::LyingDownSwingingCars//Invertedsingle-rail
            "rct1.ride.mine_cars",                         //  VehicleType::WoodenMineCars
            "rct1.ride.suspended_swinging_aeroplane_cars", //  VehicleType::SuspendedSwingingAirplaneCars
            "rct1.ride.small_monorail_cars",               //  VehicleType::SmallMonorailCars
            "rct2.ride.trike",                             //  VehicleType::WaterTricycles
            "rct2.ride.ssc1",                              //  VehicleType::LaunchedFreefallCar
            "rct1.ride.bobsleigh_trains",                  //  VehicleType::BobsleighCars
            "rct1.ride.dinghies",                          //  VehicleType::Dinghies
            "rct2.ride.obs1",                              //  VehicleType::RotatingCabin
            "rct1.ride.mine_trains",                       //  VehicleType::MineTrain
            "rct1.ride.chairlift_cars",                    //  VehicleType::ChairliftCars
            "rct1.ride.corkscrew_trains",                  //  VehicleType::CorkscrewRollerCoasterTrain
            "rct1.ride.motorbikes",                        //  VehicleType::Motorbikes
            "rct1.ride.racing_cars",                       //  VehicleType::RacingCars
            "rct1.ride.pickup_trucks",                     //  VehicleType::Trucks
            "rct1.ride.go_karts",                          //  VehicleType::GoKarts
            "rct1.ride.river_rapids_boats",                //  VehicleType::RapidsBoats
            "rct1.ride.logs",                              //  VehicleType::LogFlumeBoats
            "rct1.ride.dodgems",                           //  VehicleType::Dodgems
            "rct2.ride.swsh1",                             //  VehicleType::SwingingShip
            "rct2.ride.swsh2",                             //  VehicleType::SwingingInverterShip
            "rct2.ride.mgr1",                              //  VehicleType::MerryGoRound
            "rct2.ride.fwh1",                              //  VehicleType::FerrisWheel
            "rct2.ride.simpod",                            //  VehicleType::SimulatorPod
            "rct2.ride.c3d",                               //  VehicleType::CinemaBuilding
            "rct2.ride.topsp1",                            //  VehicleType::TopspinCar
            "rct2.ride.srings",                            //  VehicleType::SpaceRings
            "rct1.ride.reverse_freefall_car",              //  VehicleType::ReverseFreefallRollerCoasterCar
            "rct1.ride.vertical_drop_trains",              //  VehicleType::VerticalRollerCoasterCars
            "rct1.ride.cat_cars",                          //  VehicleType::CatCars
            "rct2.ride.twist1",                            //  VehicleType::TwistArmsAndCars
            "rct2.ride.hhbuild",                           //  VehicleType::HauntedHouseBuilding
            "rct1.ride.log_trains",                        //  VehicleType::LogCars
            "rct2.ride.circus1",                           //  VehicleType::CircusTent
            "rct1aa.ride.ghost_train_cars",                //  VehicleType::GhostTrainCars
            "rct1aa.ride.twister_trains",                  //  VehicleType::SteelTwisterRollerCoasterTrain
            "rct1aa.ride.wooden_articulated_trains",       //  VehicleType::WoodenTwisterRollerCoasterTrain
            "rct1aa.ride.side_friction_cars",              //  VehicleType::WoodenSideFrictionCars
            "rct1aa.ride.vintage_cars",                    //  VehicleType::VintageCars
            "rct1aa.ride.steam_trains_covered",            //  VehicleType::SteamTrainCoveredCars
            "rct1aa.ride.stand_up_twister_trains",         //  VehicleType::StandUpSteelTwisterRollerCoasterTrain
            "rct1aa.ride.floorless_twister_trains",        //  VehicleType::FloorlessSteelTwisterRollerCoasterTrain
            "rct1aa.ride.steel_wild_mouse_cars",           //  VehicleType::SteelMouseCars
            "rct1aa.ride.ski_lift_cars",                   //  VehicleType::ChairliftCarsAlternative
            "rct1aa.ride.suspended_monorail_trains",       //  VehicleType::SuspendedMonorailTrain
            "rct1aa.ride.mini_helicopters",                //  VehicleType::HelicopterCars
            "rct1aa.ride.virginia_reel_tubs",              //  VehicleType::VirginiaReelTubs
            "rct1aa.ride.reverser_cars",                   //  VehicleType::ReverserCars
            "rct2.ride.golf1",                             //  VehicleType::Golfers
            "rct1aa.ride.splash_boats",                    //  VehicleType::RiverRideBoats
            "rct1aa.ride.lay_down_trains",                 //  VehicleType::FlyingRollerCoasterTrain
            "rct1aa.ride.hyper_twister_trains",            //  VehicleType::NonLoopingSteelTwisterRollerCoasterTrain
            "rct1aa.ride.heartline_twister_cars",          //  VehicleType::HeartlineTwisterCars
            "rct1aa.ride.heartline_twister_cars",          //  VehicleType::HeartlineTwisterCarsReversed
            "",                                            //  VehicleType::Reserved
            "rct2.ride.gdrop1",                            //  VehicleType::RotodropCar
            "rct1aa.ride.flying_saucers",                  //  VehicleType::FlyingSaucers
            "rct2.ride.chbuild",                           //  VehicleType::CrookedHouseBuilding
            "rct1aa.ride.bicycles",                        //  VehicleType::Bicycles
            "rct1ll.ride.hypercoaster_trains",             //  VehicleType::HypercoasterTrain
            "rct1ll.ride.4_across_inverted_trains",        //  VehicleType::_4_Across_Inverted_Coaster_Train
            "rct1ll.ride.coaster_boats",                   //  VehicleType::WaterCoasterBoats
            "rct1ll.ride.face_off_cars",                   //  VehicleType::FaceoffCars
            "rct1ll.ride.jet_skis",                        //  VehicleType::JetSkis
            "rct1ll.ride.rafts",                           //  VehicleType::RaftBoats
            "rct1ll.ride.steam_trains_american",           //  VehicleType::AmericanStyleSteamTrain
            "rct1ll.ride.air_powered_trains",              //  VehicleType::AirPoweredCoasterTrain
            "rct1ll.ride.inverted_hairpin_cars",           //  VehicleType::SuspendedWildMouseCars
            "rct2.ride.enterp",                            //  VehicleType::EnterpriseWheel
        };

        Guard::ArgumentInRange<size_t>(EnumValue(vehicleType), 0, std::size(map) - 1, "Unsupported RCT1 vehicle type.");
        return map[EnumValue(vehicleType)];
    }

    std::string_view GetSmallSceneryObject(uint8_t smallSceneryType)
    {
        static constexpr const char * map[] =
        {
            "rct2.scenery_small.tl0",
            "rct2.scenery_small.tl1",
            "rct2.scenery_small.tl2",
            "rct2.scenery_small.tl3",
            "rct2.scenery_small.tm0",
            "rct2.scenery_small.tm1",
            "rct2.scenery_small.tm2",
            "rct2.scenery_small.tm3",
            "rct2.scenery_small.ts0",
            "rct2.scenery_small.ts1",
            "rct2.scenery_small.ts2",
            "rct2.scenery_small.ts3",
            "rct2.scenery_small.ts4",
            "rct2.scenery_small.ts5",
            "rct2.scenery_small.ts6",
            "rct2.scenery_small.tic",
            "rct2.scenery_small.tlc",
            "rct2.scenery_small.tmc",
            "rct2.scenery_small.tmp",
            "rct2.scenery_small.titc",
            "rct2.scenery_small.tghc",
            "rct2.scenery_small.tac",
            "rct2.scenery_small.tghc2",
            "rct2.scenery_small.tcj",
            "rct2.scenery_small.tmbj",
            "rct2.scenery_small.tcf",
            "rct2.scenery_small.tcl",
            "rct2.scenery_small.trf",
            "rct2.scenery_small.trf2",
            "rct2.scenery_small.tel",
            "rct2.scenery_small.tap",
            "rct2.scenery_small.tsp",
            "rct2.scenery_small.tmzp",
            "rct2.scenery_small.tcrp",
            "rct2.scenery_small.tbp",
            "rct2.scenery_small.tlp",
            "rct2.scenery_small.twp",
            "rct2.scenery_small.tas",
            "rct2.scenery_small.tmg",
            "rct2.scenery_small.tww",
            "rct2.scenery_small.tsb",
            "rct2.scenery_small.tvl",
            "rct2.scenery_small.tct",
            "rct2.scenery_small.tef",
            "rct2.scenery_small.tal",
            "rct2.scenery_small.tsq",
            "rct2.scenery_small.tht",
            "rct2.scenery_small.tcb",
            "rct2.scenery_small.tdm",
            "rct2.scenery_small.tsd",
            "rct2.scenery_small.tgs",
            "rct2.scenery_small.tus",
            "rct2.scenery_small.th1",
            "rct2.scenery_small.tbc",
            "rct2.scenery_small.th2",
            "rct2.scenery_small.tpm",
            "rct2.scenery_small.tsc",
            "rct2.scenery_small.tg1",
            "rct2.scenery_small.twf",
            "rct2.scenery_small.tsh0",
            "rct2.scenery_small.tsh1",
            "rct2.scenery_small.tsh2",
            "rct2.scenery_small.tsh3",
            "rct2.scenery_small.tsh4",
            "rct2.scenery_small.tsh5",
            "rct2.scenery_small.tg2",
            "rct2.scenery_small.tg3",
            "rct2.scenery_small.tg4",
            "rct2.scenery_small.tg5",
            "rct2.scenery_small.tg6",
            "rct2.scenery_small.tg7",
            "rct2.scenery_small.tg8",
            "rct2.scenery_small.tg9",
            "rct2.scenery_small.tg10",
            "rct2.scenery_small.tg11",
            "rct2.scenery_small.tg12",
            "rct2.scenery_small.tg13",
            "rct2.scenery_small.tg14",
            "rct2.scenery_small.tt1",
            "rct2.scenery_small.tdf",
            "rct2.scenery_small.tsh",
            "rct2.scenery_small.thrs",
            "rct2.scenery_small.tstd",
            "rct2.scenery_small.trms",
            "rct2.scenery_small.trws",
            "rct2.scenery_small.trc",
            "rct2.scenery_small.tqf",
            "rct2.scenery_small.tes1",
            "rct2.scenery_small.ten",
            "rct2.scenery_small.ters",
            "rct2.scenery_small.terb",
            "rct2.scenery_small.tep",
            "rct2.scenery_small.tst1",
            "rct2.scenery_small.tst2",
            "rct2.scenery_small.tms1",
            "rct2.scenery_small.tas1",
            "rct2.scenery_small.tas2",
            "rct2.scenery_small.tas3",
            "rct2.scenery_small.tst3",
            "rct2.scenery_small.tst4",
            "rct2.scenery_small.tst5",
            "rct2.scenery_small.tas4",
            "rct2.scenery_small.tcy",
            "rct2.scenery_small.tbw",
            "rct2.scenery_small.tbr1",
            "rct2.scenery_small.tbr2",
            "rct2.scenery_small.tml",
            "rct2.scenery_small.tmw",
            "rct2.scenery_small.tbr3",
            "rct2.scenery_small.tbr4",
            "rct2.scenery_small.tmj",
            "rct2.scenery_small.tbr",
            "rct2.scenery_small.tmo1",
            "rct2.scenery_small.tmo2",
            "rct2.scenery_small.tmo3",
            "rct2.scenery_small.tmo4",
            "rct2.scenery_small.tmo5",
            "rct2.scenery_small.twh1",
            "rct2.scenery_small.twh2",
            "rct2.scenery_small.tns",
            "rct2.scenery_small.tp1",
            "rct2.scenery_small.tp2",
            "rct2.scenery_small.tk1",
            "rct2.scenery_small.tk2",
            "rct2.scenery_small.tr1",
            "rct2.scenery_small.tr2",
            "rct2.scenery_small.tq1",
            "rct2.scenery_small.tq2",
            "rct2.scenery_small.twn",
            "rct2.scenery_small.tce",
            "rct2.scenery_small.tco",
            "rct2.scenery_small.thl",
            "rct2.scenery_small.tcc",
            "rct2.scenery_small.tb1",
            "rct2.scenery_small.tb2",
            "rct2.scenery_small.tk3",
            "rct2.scenery_small.tk4",
            "rct2.scenery_small.tbn",
            "rct2.scenery_small.tbn1",
            "rct2.scenery_small.tdt1",
            "rct2.scenery_small.tdt2",
            "rct2.scenery_small.tdt3",
            "rct2.scenery_small.tmm1",
            "rct2.scenery_small.tmm2",
            "rct2.scenery_small.tmm3",
            "rct2.scenery_small.tgs1",
            "rct2.scenery_small.tgs2",
            "rct2.scenery_small.tgs3",
            "rct2.scenery_small.tgs4",
            "rct2.scenery_small.tdn4",
            "rct2.scenery_small.tdn5",
            "rct2.scenery_small.tjt1",
            "rct2.scenery_small.tjt2",
            "rct2.scenery_small.tjb1",
            "rct2.scenery_small.ttf",
            "rct2.scenery_small.tf1",
            "rct2.scenery_small.tf2",
            "rct2.scenery_small.tge1",
            "rct2.scenery_small.tjt3",
            "rct2.scenery_small.tjt4",
            "rct2.scenery_small.tjp1",
            "rct2.scenery_small.tjb2",
            "rct2.scenery_small.tge2",
            "rct2.scenery_small.tjt5",
            "rct2.scenery_small.tjb3",
            "rct2.scenery_small.tjb4",
            "rct2.scenery_small.tjt6",
            "rct2.scenery_small.tjp2",
            "rct2.scenery_small.tge3",
            "rct2.scenery_small.tck",
            "rct2.scenery_small.tge4",
            "rct2.scenery_small.tge5",
            "rct2.scenery_small.tg15",
            "rct2.scenery_small.tg16",
            "rct2.scenery_small.tg17",
            "rct2.scenery_small.tg18",
            "rct2.scenery_small.tg19",
            "rct2.scenery_small.tg20",
            "rct2.scenery_small.tg21",
            "rct2.scenery_small.tsm",
            "rct2.scenery_small.tig",
            "rct2.scenery_small.tcfs",
            "rct2.scenery_small.trfs",
            "rct2.scenery_small.trf3",
            "rct2.scenery_small.tnss",
            "rct2.scenery_small.tct1",
            "rct2.scenery_small.tct2",
            "rct2.scenery_small.tsf1",
            "rct2.scenery_small.tsf2",
            "rct2.scenery_small.tsf3",
            "rct2.scenery_small.tcn",
            "rct2.scenery_small.ttg",
            "rct2.scenery_small.tsnc",
            "rct2.scenery_small.tsnb",
            "rct2.scenery_small.tscp",
            "rct2.scenery_small.tcd",
            "rct2.scenery_small.tsg",
            "rct2.scenery_small.tsk",
            "rct2.scenery_small.tgh1",
            "rct2.scenery_small.tgh2",
            "rct2.scenery_small.tsmp",
            "rct2.scenery_small.tjf",
            "rct2.scenery_small.tly",
            "rct2.scenery_small.tgc1",
            "rct2.scenery_small.tgc2",
            "rct2.scenery_small.tgg",
            "rct2.scenery_small.tsph",
            "rct2.scenery_small.toh1",
            "rct2.scenery_small.toh2",
            "rct2.scenery_small.tot1",
            "rct2.scenery_small.tot2",
            "rct2.scenery_small.tos",
            "rct2.scenery_small.tot3",
            "rct2.scenery_small.tot4",
            "rct2.scenery_small.tsc2",
            "rct2.scenery_small.tsp1",
            "rct2.scenery_small.toh3",
            "rct2.scenery_small.tsp2",
            "rct2.scenery_small.romroof1",
            "rct2.scenery_small.georoof1",
            "rct2.scenery_small.tntroof1",
            "rct2.scenery_small.jngroof1",
            "rct2.scenery_small.minroof1",
            "rct2.scenery_small.romroof2",
            "rct2.scenery_small.georoof2",
            "rct2.scenery_small.pagroof1",
            "rct2.scenery_small.spcroof1",
            "rct2.scenery_small.roof1",
            "rct2.scenery_small.roof2",
            "rct2.scenery_small.roof3",
            "rct2.scenery_small.roof4",
            "rct2.scenery_small.roof5",
            "rct2.scenery_small.roof6",
            "rct2.scenery_small.roof7",
            "rct2.scenery_small.roof8",
            "rct2.scenery_small.roof9",
            "rct2.scenery_small.roof10",
            "rct2.scenery_small.roof11",
            "rct2.scenery_small.roof12",
            "rct2.scenery_small.roof13",
            "rct2.scenery_small.roof14",
            "rct2.scenery_small.igroof",
            "rct2.scenery_small.corroof",
            "rct2.scenery_small.corroof2",
        };
        return map[smallSceneryType];
    }

    std::string_view GetLargeSceneryObject(uint8_t largeSceneryType)
    {
        static constexpr const char * map[] =
        {
            "rct2.scenery_large.scol",
            "rct2.scenery_large.shs1",
            "rct2.scenery_large.sspx",
            "rct2.scenery_large.shs2",
            "rct2.scenery_large.scln",
            "rct2.scenery_large.smh1",
            "rct2.scenery_large.smh2",
            "rct2.scenery_large.svlc",
            "rct2.scenery_large.spyr",
            "rct2.scenery_large.smn1",
            "rct2.scenery_large.smb",
            "rct2.scenery_large.ssk1",
            "rct2.scenery_large.sdn1",
            "rct2.scenery_large.sdn2",
            "rct2.scenery_large.sdn3",
            "rct2.scenery_large.sip",
            "rct2.scenery_large.stb1",
            "rct2.scenery_large.stb2",
            "rct2.scenery_large.stg1",
            "rct2.scenery_large.stg2",
            "rct2.scenery_large.sct",
            "rct2.scenery_large.soh1",
            "rct2.scenery_large.soh2",
            "rct2.scenery_large.soh3",
            "rct2.scenery_large.sgp",
            "rct2.scenery_large.ssr",
            "rct2.scenery_large.sth",
            "rct2.scenery_large.sah",
            "rct2.scenery_large.sps",
            "rct2.scenery_large.spg",
            "rct2.scenery_large.sob",
            "rct2.scenery_large.sah2",
            "rct2.scenery_large.sst",
            "rct2.scenery_large.ssh",
            "rct2.scenery_large.sah3",
            "rct2.scenery_large.ssig1",
            "rct2.scenery_large.ssig2",
            "rct2.scenery_large.ssig3",
            "rct2.scenery_large.ssig4",
        };
        return map[largeSceneryType];
    }

    int32_t MapSlopedWall(uint8_t wallType)
    {
        static constexpr uint8_t map[] =
        {
            RCT1_WALL_TYPE_MESH_FENCE,                   // RCT1_WALL_TYPE_MESH_FENCE,
            RCT1_WALL_TYPE_MESH_FENCE,                   // RCT1_WALL_TYPE_MESH_FENCE_WITH_GATE,
            RCT1_WALL_TYPE_ROMAN,                        // RCT1_WALL_TYPE_ROMAN,
            RCT1_WALL_TYPE_EGYPTIAN,                     // RCT1_WALL_TYPE_EGYPTIAN,
            RCT1_WALL_TYPE_HEDGE,                        // RCT1_WALL_TYPE_HEDGE,
            RCT1_WALL_TYPE_HEDGE,                        // RCT1_WALL_TYPE_HEDGE_WITH_GATE,
            RCT1_WALL_TYPE_BLUE_PLAYING_CARDS,           // RCT1_WALL_TYPE_BLUE_PLAYING_CARDS,
            RCT1_WALL_TYPE_RED_PLAYING_CARDS,            // RCT1_WALL_TYPE_RED_PLAYING_CARDS,
            RCT1_WALL_TYPE_WHITE_RAILING,                // RCT1_WALL_TYPE_WHITE_RAILING,
            RCT1_WALL_TYPE_WHITE_RAILING,                // RCT1_WALL_TYPE_WHITE_RAILING_WITH_GATE,
            RCT1_WALL_TYPE_MARTIAN,                      // RCT1_WALL_TYPE_MARTIAN,
            RCT1_WALL_TYPE_GLASS_SMOOTH,                 // RCT1_WALL_TYPE_GLASS_SMOOTH,
            RCT1_WALL_TYPE_WOODEN_PANEL_FENCE,           // RCT1_WALL_TYPE_WOODEN_PANEL_FENCE,
            RCT1_WALL_TYPE_WOODEN_PANEL_FENCE,           // RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_GATE,
            RCT1_WALL_TYPE_WOODEN_POST_FENCE,            // RCT1_WALL_TYPE_WOODEN_POST_FENCE,
            RCT1_WALL_TYPE_RED_WOODEN_POST_FENCE,        // RCT1_WALL_TYPE_RED_WOODEN_POST_FENCE,
            RCT1_WALL_TYPE_BARBED_WIRE,                  // RCT1_WALL_TYPE_BARBED_WIRE,
            RCT1_WALL_TYPE_BARBED_WIRE,                  // RCT1_WALL_TYPE_BARBED_WIRE_WITH_GATE,
            RCT1_WALL_TYPE_PRIMITIVE_TALL_WOOD_FENCE,    // RCT1_WALL_TYPE_PRIMITIVE_TALL_WOOD_FENCE,
            RCT1_WALL_TYPE_PRIMITIVE_SHORT_WOOD_FENCE,   // RCT1_WALL_TYPE_PRIMITIVE_SHORT_WOOD_FENCE,
            RCT1_WALL_TYPE_IRON_RAILING,                 // RCT1_WALL_TYPE_IRON_RAILING,
            RCT1_WALL_TYPE_IRON_RAILING,                 // RCT1_WALL_TYPE_IRON_RAILING_WITH_GATE,
            RCT1_WALL_TYPE_GLASS_PANELS,                 // RCT1_WALL_TYPE_GLASS_PANELS,
            RCT1_WALL_TYPE_BONE_FENCE,                   // RCT1_WALL_TYPE_BONE_FENCE,
            RCT1_WALL_TYPE_BRICK,                        // RCT1_WALL_TYPE_BRICK,
            RCT1_WALL_TYPE_BRICK,                        // RCT1_WALL_TYPE_BRICK_WITH_GATE,
            RCT1_WALL_TYPE_WHITE_WOODEN_PANEL_FENCE,     // RCT1_WALL_TYPE_WHITE_WOODEN_PANEL_FENCE,
            RCT1_WALL_TYPE_RED_WOODEN_PANEL_FENCE,       // RCT1_WALL_TYPE_RED_WOODEN_PANEL_FENCE,
            RCT1_WALL_TYPE_STONE,                        // RCT1_WALL_TYPE_STONE,
            RCT1_WALL_TYPE_STONE,                        // RCT1_WALL_TYPE_STONE_WITH_GATE,
            RCT1_WALL_TYPE_WOODEN_FENCE,                 // RCT1_WALL_TYPE_WOODEN_FENCE,
            RCT1_WALL_TYPE_JUNGLE,                       // RCT1_WALL_TYPE_JUNGLE,
            RCT1_WALL_TYPE_CONIFER_HEDGE,                // RCT1_WALL_TYPE_CONIFER_HEDGE,
            RCT1_WALL_TYPE_CONIFER_HEDGE,                // RCT1_WALL_TYPE_CONIFER_HEDGE_WITH_GATE,
            RCT1_WALL_TYPE_SMALL_BROWN_CASTLE,           // RCT1_WALL_TYPE_SMALL_BROWN_CASTLE,
            RCT1_WALL_TYPE_SMALL_GREY_CASTLE,            // RCT1_WALL_TYPE_SMALL_GREY_CASTLE,
            RCT1_WALL_TYPE_ROMAN_COLUMN,                 // RCT1_WALL_TYPE_ROMAN_COLUMN,
            RCT1_WALL_TYPE_LARGE_BROWN_CASTLE,           // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE,
            RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_CROSS,     // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_CROSS,
            RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_GATE,      // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_GATE,
            RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_WINDOW,    // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_WINDOW,
            RCT1_WALL_TYPE_MEDIUM_BROWN_CASTLE,          // RCT1_WALL_TYPE_MEDIUM_BROWN_CASTLE,
            RCT1_WALL_TYPE_LARGE_GREY_CASTLE,            // RCT1_WALL_TYPE_LARGE_GREY_CASTLE,
            RCT1_WALL_TYPE_LARGE_GREY_CASTLE_CROSS,      // RCT1_WALL_TYPE_LARGE_GREY_CASTLE_CROSS,
            RCT1_WALL_TYPE_LARGE_GREY_CASTLE_GATE,       // RCT1_WALL_TYPE_LARGE_GREY_CASTLE_GATE,
            RCT1_WALL_TYPE_LARGE_GREY_CASTLE_WINDOW,     // RCT1_WALL_TYPE_LARGE_GREY_CASTLE_WINDOW,
            RCT1_WALL_TYPE_MEDIUM_GREY_CASTLE,           // RCT1_WALL_TYPE_MEDIUM_GREY_CASTLE,
            RCT1_WALL_TYPE_CREEPY,                       // RCT1_WALL_TYPE_CREEPY,
            RCT1_WALL_TYPE_CREEPY_GATE,                  // RCT1_WALL_TYPE_CREEPY_GATE,
            RCT1_WALL_TYPE_BARBED_WIRE_WITH_SNOW,        // RCT1_WALL_TYPE_BARBED_WIRE_WITH_SNOW,
            RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_SNOW, // RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_SNOW,
            RCT1_WALL_TYPE_WOODEN_POST_FENCE_WITH_SNOW,  // RCT1_WALL_TYPE_WOODEN_POST_FENCE_WITH_SNOW,
        };

        if (wallType < std::size(map))
            return map[wallType];

        return -1;
    }

    std::string_view GetWallObject(uint8_t wallType)
    {
        static constexpr const char * map[] =
        {
            "rct2.scenery_wall.wmf",                       // RCT1_WALL_TYPE_MESH_FENCE
            "rct2.scenery_wall.wmfg",                      // RCT1_WALL_TYPE_MESH_FENCE_WITH_GATE
            "rct2.scenery_wall.wrw",                       // RCT1_WALL_TYPE_ROMAN
            "rct2.scenery_wall.wew",                       // RCT1_WALL_TYPE_EGYPTIAN
            "rct2.scenery_wall.whg",                       // RCT1_WALL_TYPE_HEDGE
            "rct2.scenery_wall.whgg",                      // RCT1_WALL_TYPE_HEDGE_WITH_GATE
            "rct1.scenery_wall.playing_card_wall_1",       // RCT1_WALL_TYPE_BLUE_PLAYING_CARDS
            "rct1.scenery_wall.playing_card_wall_2",       // RCT1_WALL_TYPE_RED_PLAYING_CARDS
            "rct2.scenery_wall.wsw",                       // RCT1_WALL_TYPE_WHITE_RAILING
            "rct2.scenery_wall.wswg",                      // RCT1_WALL_TYPE_WHITE_RAILING_WITH_GATE
            "rct2.scenery_wall.wmw",                       // RCT1_WALL_TYPE_MARTIAN
            "rct1aa.scenery_wall.glass_wall",              // RCT1_WALL_TYPE_GLASS_SMOOTH
            "rct1.scenery_wall.wooden_fence_brown",        // RCT1_WALL_TYPE_WOODEN_PANEL_FENCE
            "rct1.scenery_wall.wooden_fence_brown_gate",   // RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_GATE
            "rct1aa.scenery_wall.wooden_post_wall_1",      // RCT1_WALL_TYPE_WOODEN_POST_FENCE
            "rct1aa.scenery_wall.wooden_post_wall_2",      // RCT1_WALL_TYPE_RED_WOODEN_POST_FENCE
            "rct2.scenery_wall.wpf",                       // RCT1_WALL_TYPE_BARBED_WIRE
            "rct2.scenery_wall.wpfg",                      // RCT1_WALL_TYPE_BARBED_WIRE_WITH_GATE
            "rct2.scenery_wall.wwtw",                      // RCT1_WALL_TYPE_PRIMITIVE_TALL_WOOD_FENCE
            "rct2.scenery_wall.wmww",                      // RCT1_WALL_TYPE_PRIMITIVE_SHORT_WOOD_FENCE
            "rct2.scenery_wall.wsw1",                      // RCT1_WALL_TYPE_IRON_RAILING
            "rct2.scenery_wall.wsw2",                      // RCT1_WALL_TYPE_IRON_RAILING_WITH_GATE
            "rct2.scenery_wall.wgw2",                      // RCT1_WALL_TYPE_GLASS_PANELS
            "rct2.scenery_wall.wbw",                       // RCT1_WALL_TYPE_BONE_FENCE
            "rct2.scenery_wall.wbr1",                      // RCT1_WALL_TYPE_BRICK
            "rct2.scenery_wall.wbrg",                      // RCT1_WALL_TYPE_BRICK_WITH_GATE
            "rct1.scenery_wall.wooden_fence_white",        // RCT1_WALL_TYPE_WHITE_WOODEN_PANEL_FENCE
            "rct1.scenery_wall.wooden_fence_red",          // RCT1_WALL_TYPE_RED_WOODEN_PANEL_FENCE
            "rct2.scenery_wall.wbr2",                      // RCT1_WALL_TYPE_STONE
            "rct2.scenery_wall.wbr3",                      // RCT1_WALL_TYPE_STONE_WITH_GATE
            "rct1ll.scenery_wall.medieval_wooden_fence",   // RCT1_WALL_TYPE_WOODEN_FENCE
            "rct2.scenery_wall.wjf",                       // RCT1_WALL_TYPE_JUNGLE
            "rct2.scenery_wall.wch",                       // RCT1_WALL_TYPE_CONIFER_HEDGE
            "rct2.scenery_wall.wchg",                      // RCT1_WALL_TYPE_CONIFER_HEDGE_WITH_GATE
            "rct2.scenery_wall.wc1",                       // RCT1_WALL_TYPE_SMALL_BROWN_CASTLE
            "rct2.scenery_wall.wc2",                       // RCT1_WALL_TYPE_SMALL_GREY_CASTLE
            "rct1.scenery_wall.roman_column_wall",         // RCT1_WALL_TYPE_ROMAN_COLUMN
            "rct2.scenery_wall.wc4",                       // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE
            "rct2.scenery_wall.wc5",                       // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_CROSS
            "rct2.scenery_wall.wc6",                       // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_GATE
            "rct2.scenery_wall.wc7",                       // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_WINDOW
            "rct2.scenery_wall.wc8",                       // RCT1_WALL_TYPE_MEDIUM_BROWN_CASTLE
            "rct2.scenery_wall.wc9",                       // RCT1_WALL_TYPE_LARGE_GREY_CASTLE
            "rct2.scenery_wall.wc10",                      // RCT1_WALL_TYPE_LARGE_GREY_CASTLE_CROSS
            "rct2.scenery_wall.wc11",                      // RCT1_WALL_TYPE_LARGE_GREY_CASTLE_GATE
            "rct2.scenery_wall.wc12",                      // RCT1_WALL_TYPE_LARGE_GREY_CASTLE_WINDOW
            "rct2.scenery_wall.wc13",                      // RCT1_WALL_TYPE_MEDIUM_GREY_CASTLE
            "rct2.scenery_wall.wc14",                      // RCT1_WALL_TYPE_CREEPY
            "rct2.scenery_wall.wc15",                      // RCT1_WALL_TYPE_CREEPY_GATE
            "rct2.scenery_wall.wc16",                      // RCT1_WALL_TYPE_BARBED_WIRE_WITH_SNOW
            "rct1ll.scenery_wall.wooden_fence_brown_snow", // RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_SNOW
            "rct2.scenery_wall.wc18",                      // RCT1_WALL_TYPE_WOODEN_POST_FENCE_WITH_SNOW
        };
        if (wallType < std::size(map))
            return map[wallType];

        return map[0];
    }

    std::string_view GetBannerObject(BannerType bannerType)
    {
        static constexpr const char * map[] =
        {
            "rct2.footpath_banner.bn1", // BannerType::Plain
            "rct2.footpath_banner.bn2", // BannerType::Jungle
            "rct2.footpath_banner.bn3", // BannerType::Roman
            "rct2.footpath_banner.bn4", // BannerType::Egyptian
            "rct2.footpath_banner.bn5", // BannerType::Mine
            "rct2.footpath_banner.bn6", // BannerType::Jurassic
            "rct2.footpath_banner.bn7", // BannerType::Oriental
            "rct2.footpath_banner.bn8", // BannerType::Snow
            "rct2.footpath_banner.bn9", // BannerType::Space
        };
        return map[EnumValue(bannerType)];
    }

    std::string_view GetPathSurfaceObject(uint8_t pathType)
    {
        static constexpr const char * map[] =
        {
            "rct1.footpath_surface.queue_blue",     // RCT1_FOOTPATH_TYPE_QUEUE_BLUE
            "rct1aa.footpath_surface.queue_red",    // RCT1_FOOTPATH_TYPE_QUEUE_RED
            "rct1aa.footpath_surface.queue_yellow", // RCT1_FOOTPATH_TYPE_QUEUE_YELLOW
            "rct1aa.footpath_surface.queue_green",  // RCT1_FOOTPATH_TYPE_QUEUE_GREEN

            "rct1.footpath_surface.tarmac",         // RCT1_FOOTPATH_TYPE_TARMAC_GREY
            "rct1aa.footpath_surface.tarmac_red",   // RCT1_FOOTPATH_TYPE_TARMAC_RED
            "rct1aa.footpath_surface.tarmac_brown", // RCT1_FOOTPATH_TYPE_TARMAC_BROWN
            "rct1aa.footpath_surface.tarmac_green", // RCT1_FOOTPATH_TYPE_TARMAC_GREEN

            "rct1.footpath_surface.dirt",           // RCT1_FOOTPATH_TYPE_DIRT_BROWN
            "rct1aa.footpath_surface.ash",          // RCT1_FOOTPATH_TYPE_DIRT_BLACK
            "",
            "",

            "rct1.footpath_surface.crazy_paving",   // RCT1_FOOTPATH_TYPE_CRAZY_PAVING
            "",
            "",
            "",

            "rct1.footpath_surface.road",           // RCT1_FOOTPATH_TYPE_ROAD
            "",
            "",
            "",

            "rct1.footpath_surface.tiles_brown",    // RCT1_FOOTPATH_TYPE_TILE_BROWN
            "rct1aa.footpath_surface.tiles_grey",   // RCT1_FOOTPATH_TYPE_TILE_GREY
            "rct1ll.footpath_surface.tiles_red",    // RCT1_FOOTPATH_TYPE_TILE_RED
            "rct1ll.footpath_surface.tiles_green",  // RCT1_FOOTPATH_TYPE_TILE_GREEN
        };
        return map[pathType];
    }

    std::string_view GetPathAddtionObject(uint8_t pathAdditionType)
    {
        static constexpr const char * map[] =
        {
            "",                 // RCT1_PATH_ADDITION_NONE
            "rct2.footpath_item.lamp1",       // RCT1_PATH_ADDITION_LAMP_1
            "rct2.footpath_item.lamp2",       // RCT1_PATH_ADDITION_LAMP_2
            "rct2.footpath_item.litter1",     // RCT1_PATH_ADDITION_BIN
            "rct2.footpath_item.bench1",      // RCT1_PATH_ADDITION_BENCH
            "rct2.footpath_item.jumpfnt1",    // RCT1_PATH_ADDITION_JUMPING_FOUNTAIN
            "rct2.footpath_item.lamp3",       // RCT1_PATH_ADDITION_LAMP_3
            "rct2.footpath_item.lamp4",       // RCT1_PATH_ADDITION_LAMP_4
            "rct2.footpath_item.lamp1",       // RCT1_PATH_ADDITION_BROKEN_LAMP_1
            "rct2.footpath_item.lamp2",       // RCT1_PATH_ADDITION_BROKEN_LAMP_2
            "rct2.footpath_item.litter1",     // RCT1_PATH_ADDITION_BROKEN_BIN
            "rct2.footpath_item.bench1",      // RCT1_PATH_ADDITION_BROKEN_BENCH
            "rct2.footpath_item.lamp3",       // RCT1_PATH_ADDITION_BROKEN_LAMP_3
            "rct2.footpath_item.lamp4",       // RCT1_PATH_ADDITION_BROKEN_LAMP_4
            "rct2.footpath_item.jumpsnw1",    // RCT1_PATH_ADDITION_JUMPING_SNOW
        };
        return map[pathAdditionType];
    }

    std::string_view GetFootpathRailingsObject(uint8_t footpathRailingsType)
    {
        static constexpr const char * map[] =
        {
            "rct2.footpath_railings.wood",       // RCT1_PATH_SUPPORT_TYPE_TRUSS
            "rct2.footpath_railings.concrete",   // RCT1_PATH_SUPPORT_TYPE_COATED_WOOD
            "rct1ll.footpath_railings.space",      // RCT1_PATH_SUPPORT_TYPE_SPACE
            "rct1ll.footpath_railings.bamboo",     // RCT1_PATH_SUPPORT_TYPE_BAMBOO
        };
        return map[footpathRailingsType];
    }

    std::string_view GetSceneryGroupObject(uint8_t sceneryGroupType)
    {
        static constexpr const char * map[] =
        {
            "",                 // RCT1_SCENERY_THEME_GENERAL
            "rct2.scenery_group.scgmine",     // RCT1_SCENERY_THEME_MINE
            "rct2.scenery_group.scgclass",    // RCT1_SCENERY_THEME_CLASSICAL_ROMAN
            "rct2.scenery_group.scgegypt",    // RCT1_SCENERY_THEME_EGYPTIAN
            "rct2.scenery_group.scgmart",     // RCT1_SCENERY_THEME_MARTIAN
            "",                 // RCT1_SCENERY_THEME_JUMPING_FOUNTAINS
            "rct2.scenery_group.scgwond",     // RCT1_SCENERY_THEME_WONDERLAND
            "rct2.scenery_group.scgjuras",    // RCT1_SCENERY_THEME_JURASSIC
            "rct2.scenery_group.scgspook",    // RCT1_SCENERY_THEME_SPOOKY
            "rct2.scenery_group.scgjungl",    // RCT1_SCENERY_THEME_JUNGLE
            "rct2.scenery_group.scgabstr",    // RCT1_SCENERY_THEME_ABSTRACT
            "",                 // RCT1_SCENERY_THEME_GARDEN_CLOCK
            "rct2.scenery_group.scgsnow",     // RCT1_SCENERY_THEME_SNOW_ICE
            "rct2.scenery_group.scgmedie",    // RCT1_SCENERY_THEME_MEDIEVAL
            "rct2.scenery_group.scgspace",    // RCT1_SCENERY_THEME_SPACE
            "rct2.scenery_group.scghallo",    // RCT1_SCENERY_THEME_CREEPY
            "rct2.scenery_group.scgurban",    // RCT1_SCENERY_THEME_URBAN
            "rct2.scenery_group.scgorien",    // RCT1_SCENERY_THEME_PAGODA
        };
        return map[sceneryGroupType];
    }

    std::string_view GetWaterObject(uint8_t waterType)
    {
        static constexpr const char * map[] =
        {
            "rct2.water.wtrcyan",
            "rct2.water.wtrorng",
        };
        return map[waterType];
    }

    const std::vector<const char *> GetSceneryObjects(uint8_t sceneryType)
    {
        static const std::vector<const char *> map[] =
        {
            // RCT1_SCENERY_THEME_GENERAL (trees, shrubs, garden, walls, fence, path accessories)
            { "rct2.scenery_small.tic", "rct2.scenery_small.tlc", "rct2.scenery_small.tmc", "rct2.scenery_small.tmp", "rct2.scenery_small.titc", "rct2.scenery_small.tghc", "rct2.scenery_small.tac", "rct2.scenery_small.tghc2", "rct2.scenery_small.tcj", "rct2.scenery_small.tmbj", "rct2.scenery_small.tcf", "rct2.scenery_small.tcl", "rct2.scenery_small.trf", "rct2.scenery_small.trf2", "rct2.scenery_small.tel", "rct2.scenery_small.tap", "rct2.scenery_small.tsp", "rct2.scenery_small.tmzp", "rct2.scenery_small.tcrp", "rct2.scenery_small.tbp", "rct2.scenery_small.tlp", "rct2.scenery_small.twp", "rct2.scenery_small.tas", "rct2.scenery_small.tmg", "rct2.scenery_small.tww", "rct2.scenery_small.tsb", "rct2.scenery_small.tvl", "rct2.scenery_small.tcy", "rct2.scenery_small.tns", "rct2.scenery_small.twn", "rct2.scenery_small.tce", "rct2.scenery_small.tco", "rct2.scenery_small.thl", "rct2.scenery_small.tcc", "rct2.scenery_small.tf1", "rct2.scenery_small.tf2", "rct2.scenery_small.tct", "rct2.scenery_small.th1", "rct2.scenery_small.th2", "rct2.scenery_small.tpm", "rct2.scenery_small.tropt1",
              "rct2.scenery_small.ts0", "rct2.scenery_small.ts1", "rct2.scenery_small.ts2", "rct2.scenery_small.ts3", "rct2.scenery_small.ts4", "rct2.scenery_small.ts5", "rct2.scenery_small.ts6", "rct2.scenery_small.tef", "rct2.scenery_small.tal", "rct2.scenery_small.tsq", "rct2.scenery_small.tht", "rct2.scenery_small.tcb", "rct2.scenery_small.tdm", "rct2.scenery_small.tsd", "rct2.scenery_small.torn1", "rct2.scenery_small.torn2", "rct2.scenery_small.tgs", "rct2.scenery_small.tus", "rct2.scenery_small.tbc", "rct2.scenery_small.tsc", "rct2.scenery_small.twf", "rct2.scenery_small.tsh0", "rct2.scenery_small.tsh1", "rct2.scenery_small.tsh2", "rct2.scenery_small.tsh3", "rct2.scenery_small.tsh4", "rct2.scenery_small.tsh5", "rct2.scenery_small.tdf", "rct2.scenery_small.tsh", "rct2.scenery_small.thrs", "rct2.scenery_small.tstd", "rct2.scenery_small.tbr", "rct2.scenery_small.ttf", "rct2.scenery_wall.whg", "rct2.scenery_wall.whgg", "rct2.scenery_wall.wch", "rct2.scenery_wall.wchg",
              "rct2.scenery_small.tg1", "rct2.scenery_small.tg2", "rct2.scenery_small.tg3", "rct2.scenery_small.tg4", "rct2.scenery_small.tg5", "rct2.scenery_small.tg6", "rct2.scenery_small.tg7", "rct2.scenery_small.tg8", "rct2.scenery_small.tg9", "rct2.scenery_small.tg10", "rct2.scenery_small.tg11", "rct2.scenery_small.tg12", "rct2.scenery_small.tg13", "rct2.scenery_small.tg14", "rct2.scenery_small.tg15", "rct2.scenery_small.tg16", "rct2.scenery_small.tg17", "rct2.scenery_small.tg18", "rct2.scenery_small.tg19", "rct2.scenery_small.tg20", "rct2.scenery_small.tg21",
              "rct2.scenery_wall.wbr1a", "rct2.scenery_wall.wbr2a", "rct2.scenery_wall.wallbb34", "rct2.scenery_wall.walltn32", "rct2.scenery_small.tntroof1", "rct2.scenery_wall.wallbb33", "toontowner.scenery_small.xxbbbr01_fix", "rct2.scenery_wall.wallbb32", "rct2.scenery_wall.wallbb16", "rct2.scenery_wall.wallbb8", "rct2.scenery_small.roof5", "rct2.scenery_small.roof7", "rct2.scenery_wall.wallrs32", "rct2.scenery_wall.wallrs16", "rct2.scenery_wall.wallrs8", "rct2.scenery_wall.wallbr32", "rct2.scenery_wall.wallbr16", "rct2.scenery_wall.wallbr8", "rct2.scenery_wall.wallbrdr", "rct2.scenery_wall.wallbrwn", "rct2.scenery_small.brbase", "rct2.scenery_small.roof1", "toontowner.scenery_small.ttrftl02", "toontowner.scenery_small.ttrftl03", "toontowner.scenery_small.ttrftl04", "rct2.scenery_small.roof2", "rct2.scenery_small.roof3", "toontowner.scenery_small.ttrftl07", "toontowner.scenery_small.ttrftl08", "rct2.scenery_small.roof4", "rct2.scenery_wall.wallcb32", "rct2.scenery_wall.wallcb16", "rct2.scenery_wall.wallcb8", "rct2.scenery_wall.wallcbdr", "rct2.scenery_wall.wallcbwn", "rct2.scenery_small.brbase2", "rct2.scenery_small.cwbcrv33", "rct2.scenery_small.cwbcrv32", "rct2.scenery_small.brcrrf1", "rct2.scenery_small.roof6", "rct2.scenery_small.roof8", "rct2.scenery_wall.wallcf32", "rct2.scenery_wall.wallcf16", "rct2.scenery_wall.wallcf8", "rct2.scenery_wall.wallcfdr", "rct2.scenery_wall.wallcfwn", "rct2.scenery_wall.wallcfar", "rct2.scenery_small.brbase3", "rct2.scenery_small.cwfcrv33", "rct2.scenery_small.cwfcrv32", "rct2.scenery_small.brcrrf2", "rct2.scenery_small.roof9", "rct2.scenery_small.roof11", "rct2.scenery_small.roof10", "rct2.scenery_small.roof12", "rct2.scenery_small.corroof2", "rct2.scenery_wall.wallco16", "rct2.scenery_small.corroof", "rct2.scenery_wall.walllt32", "rct2.scenery_wall.wallsk16", "rct2.scenery_wall.wallsk32", "rct2.scenery_small.sktdw2", "rct2.scenery_small.sktdw", "rct2.scenery_small.sktbase", "rct2.scenery_small.sktbaset", "official.scenery_wall.support_structure_full", "official.scenery_wall.support_structure_half", "rct2.scenery_small.suppw2", "official.scenery_small.support_structure_half", "rct2.scenery_small.suppw1", "rct2.scenery_small.suppw3", "rct2.scenery_small.suppleg1", "rct2.scenery_small.suppleg2", "rct2.scenery_small.sumrf", "rct2.scenery_wall.wallrh32",
              "rct2.scenery_wall.wmf", "rct2.scenery_wall.wmfg", "rct2.scenery_wall.wsw", "rct2.scenery_wall.wswg", "rct2.scenery_wall.wfw1", "rct2.scenery_wall.wfwg", "rct1.scenery_wall.wooden_fence_brown", "rct1.scenery_wall.wooden_fence_brown_gate", "rct1.scenery_wall.wooden_fence_red", "rct1.scenery_wall.wooden_fence_white", "rct2.scenery_wall.wpf", "rct2.scenery_wall.wpfg", "rct2.scenery_wall.wsw1", "rct2.scenery_wall.wsw2", "rct2.scenery_wall.wbr1", "rct2.scenery_wall.wbrg", "rct2.scenery_wall.wbr2", "rct2.scenery_wall.wbr3", "rct2.scenery_wall.wallmm16", "rct2.scenery_wall.wallmm17",
              "rct2.footpath_item.lamp1", "rct2.footpath_item.lamp2", "rct2.footpath_item.litter1", "rct2.footpath_item.bench1", "rct2.footpath_item.qtv1", "rct2.footpath_banner.bn1", "rct2.scenery_wall.wallpost", "official.scenery_wall.post_flipped", "rct2.scenery_wall.wallsign", "rct2.scenery_large.ssig1", "rct2.scenery_large.ssig2", "rct2.scenery_large.ssig3", "rct2.scenery_large.ssig4" },
            // RCT1_SCENERY_THEME_MINE
            { "rct2.scenery_large.smh1", "rct2.scenery_large.smh2", "rct2.scenery_large.smn1", "rct2.scenery_small.tbw", "rct2.scenery_small.tbr1", "rct2.scenery_small.tbr2", "rct2.scenery_small.tml", "rct2.scenery_small.tmw", "rct2.scenery_small.tbr3", "rct2.scenery_small.tbr4", "rct2.scenery_small.tmj", "rct2.footpath_banner.bn5", "rct2.scenery_wall.wallwd8", "rct2.scenery_wall.wallwd16", "rct2.scenery_wall.wallwd32", "rct2.scenery_wall.wallwd33", "rct2.scenery_wall.wallmn32", "rct2.scenery_small.wdbase", "rct2.scenery_small.minroof1", "rct2.scenery_small.roof13", "toontowner.scenery_small.ttrfwd01", "toontowner.scenery_small.ttrfwd02", "toontowner.scenery_small.ttrfwd03", "toontowner.scenery_small.ttrfwd04", "toontowner.scenery_small.ttrfwd06", "toontowner.scenery_small.ttrfwd07", "toontowner.scenery_small.ttrfwd08", "rct2.footpath_item.littermn" },
            // RCT1_SCENERY_THEME_CLASSICAL_ROMAN
            { "rct2.scenery_large.scol", "rct2.scenery_small.tt1", "rct2.scenery_small.trms", "rct2.scenery_small.trws", "rct2.scenery_small.trc", "rct2.scenery_small.tqf", "rct2.scenery_wall.wrw", "rct2.scenery_wall.wrwa", "rct2.scenery_small.romroof2", "rct1.scenery_wall.roman_column_wall", "rct2.scenery_wall.wc3", "rct2.scenery_small.romroof1", "rct2.footpath_banner.bn3" },
            // RCT1_SCENERY_THEME_EGYPTIAN
            { "rct2.scenery_large.sspx", "rct2.scenery_large.scln", "rct2.scenery_large.spyr", "rct2.scenery_small.tes1", "rct2.scenery_small.ten", "rct2.scenery_small.ters", "rct2.scenery_small.terb", "rct2.scenery_small.tep", "rct2.scenery_wall.wew", "rct2.footpath_item.lamp3", "rct2.footpath_banner.bn4", "rct2.footpath_item.benchstn" },
            // RCT1_SCENERY_THEME_MARTIAN
            { "rct2.scenery_large.smb", "rct2.scenery_small.tmo1", "rct2.scenery_small.tmo2", "rct2.scenery_small.tmo3", "rct2.scenery_small.tmo4", "rct2.scenery_small.tmo5", "rct2.scenery_large.svlc", "rct2.scenery_wall.wmw", "rct2.footpath_item.lamp4" },
            // RCT1_SCENERY_THEME_JUMPING_FOUNTAINS (Single researchable scenery item)
            { "rct2.footpath_item.jumpfnt1" },
            // RCT1_SCENERY_THEME_WONDERLAND
            { "rct2.scenery_small.twh1", "rct2.scenery_small.twh2", "rct2.scenery_small.tst1", "rct2.scenery_small.tst2", "rct2.scenery_small.tms1", "rct2.scenery_small.tst3", "rct2.scenery_small.tst4", "rct2.scenery_small.tst5", "rct2.scenery_small.tas1", "rct2.scenery_small.tas2", "rct2.scenery_small.tas3", "rct2.scenery_small.tas4", "rct2.scenery_small.chbbase", "rct2.scenery_small.tp1", "rct2.scenery_small.tp2", "rct2.scenery_small.tk1", "rct2.scenery_small.tk2", "rct2.scenery_small.tr1", "rct2.scenery_small.tr2", "rct2.scenery_small.tq1", "rct2.scenery_small.tq2", "rct2.scenery_small.tb1", "rct2.scenery_small.tb2", "rct2.scenery_small.tk3", "rct2.scenery_small.tk4", "rct1.scenery_wall.playing_card_wall_1", "rct1.scenery_wall.playing_card_wall_2", "rct2.scenery_wall.wcw1", "rct2.scenery_wall.wcw2" },
            // RCT1_SCENERY_THEME_JURASSIC
            { "rct2.scenery_small.tbn", "rct2.scenery_small.tbn1", "rct2.scenery_small.tdn4", "rct2.scenery_small.tdn5", "rct2.scenery_large.sdn1", "rct2.scenery_large.sdn2", "rct2.scenery_large.sdn3", "rct2.scenery_wall.wwtw", "rct2.scenery_wall.wmww", "rct2.scenery_wall.wwtwa", "rct2.scenery_wall.wbw", "rct2.footpath_banner.bn6" },
            // RCT1_SCENERY_THEME_SPOOKY,
            { "rct2.scenery_large.ssk1", "rct2.scenery_small.tdt1", "rct2.scenery_small.tdt2", "rct2.scenery_small.tdt3", "rct2.scenery_small.tmm1", "rct2.scenery_small.tmm2", "rct2.scenery_small.tmm3", "rct2.scenery_small.tgs1", "rct2.scenery_small.tgs2", "rct2.scenery_small.tgs3", "rct2.scenery_small.tgs4", "rct2.scenery_small.smskull", "rct2.scenery_wall.wallrk32" },
            // RCT1_SCENERY_THEME_JUNGLE
            { "rct2.scenery_small.tjt1", "rct2.scenery_small.tjt2", "rct2.scenery_small.tjb1", "rct2.scenery_small.tjt3", "rct2.scenery_small.tjt4", "rct2.scenery_small.tjp1", "rct2.scenery_small.tjb2", "rct2.scenery_small.tjt5", "rct2.scenery_small.tjb3", "rct2.scenery_small.tjb4", "rct2.scenery_small.tjt6", "rct2.scenery_small.tjp2", "rct2.scenery_small.tjf", "rct1aa.scenery_wall.wooden_post_wall_1", "rct1aa.scenery_wall.wooden_post_wall_2", "rct2.scenery_wall.wpw1", "rct2.scenery_wall.wpw2", "rct2.scenery_wall.wjf", "couger.scenery_wall.acwwf32", "couger.scenery_wall.acww33", "rct2.footpath_banner.bn2", "rct2.scenery_wall.walljn32", "rct2.scenery_small.jngroof1", "rct2.scenery_small.roof14", "rct2.footpath_item.benchlog" },
            // RCT1_SCENERY_THEME_ABSTRACT
            { "rct2.scenery_small.tge1", "rct2.scenery_small.tge2", "rct2.scenery_small.tge3", "rct2.scenery_small.tge4", "rct2.scenery_small.tge5", "rct2.scenery_small.tgc1", "rct2.scenery_small.tgc2", "rct2.scenery_wall.wallgl8", "rct2.scenery_wall.wallgl16", "rct2.scenery_wall.wallgl32", "rct2.scenery_small.georoof1", "rct2.scenery_wall.wgw2", "rct1aa.scenery_wall.glass_wall", "toontowner.scenery_small.ttrfgl01", "toontowner.scenery_small.ttrfgl02", "rct2.scenery_small.georoof2", "toontowner.scenery_small.ttrfgl03" },
            // RCT1_SCENERY_THEME_GARDEN_CLOCK (Single researchable scenery item)
            { "rct2.scenery_small.tck" },
            // RCT1_SCENERY_THEME_SNOW_ICE
            { "rct2.scenery_large.sip", "rct2.scenery_small.tsm", "rct2.scenery_small.tig", "rct2.scenery_small.tsf1", "rct2.scenery_small.tsf2", "rct2.scenery_small.tsf3", "rct2.scenery_small.tsnc", "rct2.scenery_small.tsnb", "rct2.scenery_wall.wc16", "rct1ll.scenery_wall.wooden_fence_brown_snow", "rct2.scenery_wall.wc17", "rct2.scenery_wall.wc18", "rct2.footpath_item.jumpsnw1", "rct2.scenery_small.tcfs", "rct2.scenery_small.trfs", "rct2.scenery_small.trf3", "rct2.scenery_small.tnss", "rct2.footpath_banner.bn8", "rct2.scenery_wall.wallig16", "rct2.scenery_wall.wallig24", "rct2.scenery_small.igroof" },
            // RCT1_SCENERY_THEME_MEDIEVAL
            { "rct2.scenery_small.tct1", "rct2.scenery_large.stb1", "rct2.scenery_large.stb2", "rct2.scenery_wall.wc1", "rct2.scenery_wall.wc4", "rct2.scenery_wall.wc5", "rct2.scenery_wall.wc6", "rct2.scenery_wall.wc7", "rct2.scenery_wall.wc8", "rct2.scenery_wall.wallcz32", "rct2.scenery_wall.wallcy32", "rct2.scenery_small.tct2", "rct2.scenery_large.stg1", "rct2.scenery_large.stg2", "rct2.scenery_wall.wc2", "rct2.scenery_wall.wc9", "rct2.scenery_wall.wc10", "rct2.scenery_wall.wc11", "rct2.scenery_wall.wc12", "rct2.scenery_wall.wc13", "rct2.scenery_wall.wallcw32", "rct2.scenery_wall.wallcx32", "rct2.scenery_small.tcn", "rct2.scenery_small.ttg", "rct2.scenery_large.sct", "rct2.scenery_large.soh1", "rct2.scenery_large.soh2", "rct2.scenery_large.soh3", "rct1ll.scenery_wall.medieval_wooden_fence", "rct2.scenery_wall.wpw3", "rct2.scenery_wall.wallcfpc", "rct2.scenery_wall.wallcbpc" },
            // RCT1_SCENERY_THEME_SPACE
            { "rct2.scenery_large.ssr", "rct2.scenery_large.sst", "rct2.scenery_large.ssh", "rct2.scenery_small.tscp", "rct2.scenery_small.tsph", "rct2.scenery_small.tsc2", "rct2.scenery_small.tsp1", "rct2.scenery_small.tsp2", "rct2.scenery_wall.wallsp32", "rct2.scenery_small.spcroof1", "rct2.footpath_banner.bn9", "rct2.footpath_item.benchspc", "rct2.footpath_item.littersp" },
            // RCT1_SCENERY_THEME_CREEPY
            { "rct2.scenery_small.tcd", "rct2.scenery_small.tsg", "rct2.scenery_small.tsk", "rct2.scenery_small.tgh1", "rct2.scenery_small.tgh2", "rct2.scenery_small.tsmp", "rct2.scenery_large.sgp", "rct2.scenery_wall.wc14", "rct2.scenery_wall.wc15", "rct2.scenery_small.tl0", "rct2.scenery_small.tl1", "rct2.scenery_small.tl2", "rct2.scenery_small.tl3", "rct2.scenery_small.tm0", "rct2.scenery_small.tm1", "rct2.scenery_small.tm2", "rct2.scenery_small.tm3" },
            // RCT1_SCENERY_THEME_URBAN
            { "rct2.scenery_large.shs1", "rct2.scenery_large.shs2", "rct2.scenery_large.sth", "rct2.scenery_large.sah", "rct2.scenery_large.sps", "rct2.scenery_large.sah2", "rct2.scenery_large.sah3", "rct2.scenery_large.sob", "rct2.scenery_wall.wallu132", "rct2.scenery_wall.wallu232" },
            // RCT1_SCENERY_THEME_PAGODA
            { "rct2.scenery_large.spg", "rct2.scenery_small.tly", "rct2.scenery_small.tgg", "rct2.scenery_small.toh1", "rct2.scenery_small.toh2", "rct2.scenery_small.tot1", "rct2.scenery_small.tot2", "rct2.scenery_small.tos", "rct2.scenery_small.tot3", "rct2.scenery_small.tot4", "rct2.scenery_small.toh3", "rct2.scenery_wall.wallpg32", "rct2.scenery_small.pagroof1", "rct2.footpath_banner.bn7" }
        };
        return map[sceneryType];
    }
    // clang-format on

    OpenRCT2::TrackElemType RCT1TrackTypeToOpenRCT2(OpenRCT2::RCT12::TrackElemType origTrackType, ride_type_t rideType)
    {
        if (GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::isFlatRide))
            return RCT12FlatTrackTypeToOpenRCT2(origTrackType);

        return static_cast<OpenRCT2::TrackElemType>(origTrackType);
    }

    bool VehicleTypeIsReversed(const VehicleType vehicleType)
    {
        switch (vehicleType)
        {
            case VehicleType::SteelRollerCoasterTrainBackwards:
            case VehicleType::WoodenRollerCoasterTrainBackwards:
            case VehicleType::HeartlineTwisterCarsReversed:
                return true;
            default:
                return false;
        }
    }
} // namespace OpenRCT2::RCT1
