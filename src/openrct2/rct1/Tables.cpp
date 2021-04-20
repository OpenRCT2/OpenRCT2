/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Tables.h"

#include "../common.h"
#include "../core/Guard.hpp"
#include "../interface/Colour.h"
#include "../object/ObjectManager.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../world/Surface.h"
#include "RCT1.h"

#include <iterator>

// clang-format off
namespace RCT1
{
    colour_t GetColour(colour_t colour)
    {
        static constexpr const uint8_t map[] =
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
            log_warning("Unsupported RCT1 colour.");
            return COLOUR_BLACK;
        }
        return map[colour];
    }

    PeepSpriteType GetPeepSpriteType(uint8_t rct1SpriteType)
    {
        static constexpr const PeepSpriteType map[] =
        {
            PeepSpriteType::Normal, // 0x00
            PeepSpriteType::Handyman, // 0x01
            PeepSpriteType::Mechanic, // 0x02
            PeepSpriteType::Security, // 0x03
            PeepSpriteType::EntertainerPanda, // 0x04
            PeepSpriteType::EntertainerTiger, // 0x05
            PeepSpriteType::EntertainerElephant, // 0x06
            PeepSpriteType::EntertainerRoman, // 0x07
            PeepSpriteType::EntertainerGorilla, // 0x08
            PeepSpriteType::EntertainerSnowman, // 0x09
            PeepSpriteType::EntertainerKnight, // 0x0A
            PeepSpriteType::EntertainerAstronaut, // 0x0B
            PeepSpriteType::IceCream, // 0x0C
            PeepSpriteType::Chips, // 0x0D
            PeepSpriteType::Burger, // 0x0E
            PeepSpriteType::Drink, // 0x0F
            PeepSpriteType::Balloon, // 0x10
            PeepSpriteType::Candyfloss, // 0x11
            PeepSpriteType::Umbrella, // 0x12
            PeepSpriteType::Pizza, // 0x13
            PeepSpriteType::SecurityAlt, // 0x14
            PeepSpriteType::Popcorn, // 0x15
            PeepSpriteType::ArmsCrossed, // 0x16
            PeepSpriteType::HeadDown, // 0x17
            PeepSpriteType::Nauseous, // 0x18
            PeepSpriteType::VeryNauseous, // 0x19
            PeepSpriteType::RequireToilet, // 0x1A
            PeepSpriteType::Hat, // 0x1B
            PeepSpriteType::HotDog, // 0x1C
            PeepSpriteType::Tentacle, // 0x1D
            PeepSpriteType::ToffeeApple, // 0x1E
            PeepSpriteType::Doughnut, // 0x1F
            PeepSpriteType::Coffee, // 0x20
            PeepSpriteType::Chicken, // 0x21
            PeepSpriteType::Lemonade, // 0x22
        };
        if (rct1SpriteType >= std::size(map))
        {
            log_warning("Unsupported RCT1 peep sprite type: %d.", rct1SpriteType);
            return PeepSpriteType::Normal;
        }
        return map[rct1SpriteType];
    }

    ObjectEntryIndex GetTerrain(uint8_t terrainSurface)
    {
        static constexpr std::string_view map[RCT1_NUM_TERRAIN_SURFACES] =
        {
            "rct2.surface.grass",
            "rct2.surface.sand",
            "rct2.surface.dirt",
            "rct2.surface.rock",
            "rct2.surface.martian",
            "rct2.surface.chequerboard",
            "rct2.surface.grassclumps",
            "rct1.aa.surface.roofred",
            "rct2.surface.ice",
            "rct1.ll.surface.wood",
            "rct1.ll.surface.rust",
            "rct1.ll.surface.roofgrey",
            "rct2.surface.gridred",
            "rct2.surface.gridyellow",
            "rct2.surface.gridpurple",
            "rct2.surface.gridgreen",
        };
        std::string selectedSurface = "rct2.surface.grass";
        if (terrainSurface < std::size(map))
        {
            selectedSurface = map[terrainSurface];
        }

        return object_manager_get_loaded_object_entry_index(ObjectEntryDescriptor(selectedSurface));
    }

    ObjectEntryIndex GetTerrainEdge(uint8_t terrainEdge)
    {
        static constexpr std::string_view map[RCT1_NUM_TERRAIN_EDGES] =
        {
            "rct2.edge.rock",
            "rct1.edge.brick",
            "rct1.edge.iron",
            "rct2.edge.woodred",
            "rct1.aa.edge.grey",
            "rct1.aa.edge.yellow",
            "rct2.edge.woodblack",
            "rct1.aa.edge.red",
            "rct2.edge.ice",
            "rct1.ll.edge.purple",
            "rct1.ll.edge.green",
            "rct1.ll.edge.stonebrown",
            "rct1.ll.edge.stonegrey",
            "rct1.ll.edge.skyscrapera",
            "rct1.ll.edge.skyscraperb",
        };
        std::string selectedEdge = "rct2.edge.rock";
        if (terrainEdge < std::size(map))
        {
            selectedEdge = map[terrainEdge];
        }

        return object_manager_get_loaded_object_entry_index(ObjectEntryDescriptor(selectedEdge));
    }

    uint8_t GetRideType(uint8_t rideType, uint8_t vehicleType)
    {
        if (rideType == RCT1_RIDE_TYPE_STEEL_TWISTER_ROLLER_COASTER && vehicleType == RCT1_VEHICLE_TYPE_NON_LOOPING_STEEL_TWISTER_ROLLER_COASTER_TRAIN)
            return RIDE_TYPE_HYPER_TWISTER;
        if (rideType == RCT1_RIDE_TYPE_STEEL_CORKSCREW_ROLLER_COASTER && vehicleType == RCT1_VEHICLE_TYPE_HYPERCOASTER_TRAIN)
            return RIDE_TYPE_HYPERCOASTER;

        static uint8_t map[] =
        {
            RIDE_TYPE_WOODEN_ROLLER_COASTER,        // RCT1_RIDE_TYPE_WOODEN_ROLLER_COASTER
            RIDE_TYPE_STAND_UP_ROLLER_COASTER,      // RCT1_RIDE_TYPE_STAND_UP_STEEL_ROLLER_COASTER
            RIDE_TYPE_SUSPENDED_SWINGING_COASTER,   // RCT1_RIDE_TYPE_SUSPENDED_ROLLER_COASTER
            RIDE_TYPE_INVERTED_ROLLER_COASTER,      // RCT1_RIDE_TYPE_INVERTED_ROLLER_COASTER
            RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER,  // RCT1_RIDE_TYPE_STEEL_MINI_ROLLER_COASTER
            RIDE_TYPE_MINIATURE_RAILWAY,            // RCT1_RIDE_TYPE_MINIATURE_RAILWAY
            RIDE_TYPE_MONORAIL,                     // RCT1_RIDE_TYPE_MONORAIL
            RIDE_TYPE_MINI_SUSPENDED_COASTER,       // RCT1_RIDE_TYPE_SUSPENDED_SINGLE_RAIL_ROLLER_COASTER
            RIDE_TYPE_BOAT_HIRE,                    // RCT1_RIDE_TYPE_BOAT_HIRE
            RIDE_TYPE_WOODEN_WILD_MOUSE,            // RCT1_RIDE_TYPE_WOODEN_CRAZY_RODENT_ROLLER_COASTER
            RIDE_TYPE_STEEPLECHASE,                 // RCT1_RIDE_TYPE_SINGLE_RAIL_ROLLER_COASTER
            RIDE_TYPE_CAR_RIDE,                     // RCT1_RIDE_TYPE_CAR_RIDE
            RIDE_TYPE_LAUNCHED_FREEFALL,            // RCT1_RIDE_TYPE_LAUNCHED_FREEFALL
            RIDE_TYPE_BOBSLEIGH_COASTER,            // RCT1_RIDE_TYPE_BOBSLED_ROLLER_COASTER
            RIDE_TYPE_OBSERVATION_TOWER,            // RCT1_RIDE_TYPE_OBSERVATION_TOWER
            RIDE_TYPE_LOOPING_ROLLER_COASTER,       // RCT1_RIDE_TYPE_STEEL_ROLLER_COASTER
            RIDE_TYPE_DINGHY_SLIDE,                 // RCT1_RIDE_TYPE_WATER_SLIDE
            RIDE_TYPE_MINE_TRAIN_COASTER,           // RCT1_RIDE_TYPE_MINE_TRAIN_ROLLER_COASTER
            RIDE_TYPE_CHAIRLIFT,                    // RCT1_RIDE_TYPE_CHAIRLIFT
            RIDE_TYPE_CORKSCREW_ROLLER_COASTER,     // RCT1_RIDE_TYPE_STEEL_CORKSCREW_ROLLER_COASTER
            RIDE_TYPE_MAZE,                         // RCT1_RIDE_TYPE_HEDGE_MAZE
            RIDE_TYPE_SPIRAL_SLIDE,                 // RCT1_RIDE_TYPE_SPIRAL_SLIDE
            RIDE_TYPE_GO_KARTS,                     // RCT1_RIDE_TYPE_GO_KARTS
            RIDE_TYPE_LOG_FLUME,                    // RCT1_RIDE_TYPE_LOG_FLUME
            RIDE_TYPE_RIVER_RAPIDS,                 // RCT1_RIDE_TYPE_RIVER_RAPIDS
            RIDE_TYPE_DODGEMS,                      // RCT1_RIDE_TYPE_DODGEMS
            RIDE_TYPE_SWINGING_SHIP,                // RCT1_RIDE_TYPE_SWINGING_SHIP
            RIDE_TYPE_SWINGING_INVERTER_SHIP,       // RCT1_RIDE_TYPE_SWINGING_INVERTER_SHIP
            RIDE_TYPE_FOOD_STALL,                   // RCT1_RIDE_TYPE_ICE_CREAM_STALL
            RIDE_TYPE_FOOD_STALL,                   // RCT1_RIDE_TYPE_CHIPS_STALL
            RIDE_TYPE_DRINK_STALL,                  // RCT1_RIDE_TYPE_DRINK_STALL
            RIDE_TYPE_FOOD_STALL,                   // RCT1_RIDE_TYPE_CANDYFLOSS_STALL
            RIDE_TYPE_SHOP,                         // RCT1_RIDE_TYPE_BURGER_BAR
            RIDE_TYPE_MERRY_GO_ROUND,               // RCT1_RIDE_TYPE_MERRY_GO_ROUND
            RIDE_TYPE_SHOP,                         // RCT1_RIDE_TYPE_BALLOON_STALL
            RIDE_TYPE_INFORMATION_KIOSK,            // RCT1_RIDE_TYPE_INFORMATION_KIOSK
            RIDE_TYPE_TOILETS,                      // RCT1_RIDE_TYPE_TOILETS
            RIDE_TYPE_FERRIS_WHEEL,                 // RCT1_RIDE_TYPE_FERRIS_WHEEL
            RIDE_TYPE_MOTION_SIMULATOR,             // RCT1_RIDE_TYPE_MOTION_SIMULATOR
            RIDE_TYPE_3D_CINEMA,                    // RCT1_RIDE_TYPE_3D_CINEMA
            RIDE_TYPE_TOP_SPIN,                     // RCT1_RIDE_TYPE_TOP_SPIN
            RIDE_TYPE_SPACE_RINGS,                  // RCT1_RIDE_TYPE_SPACE_RINGS
            RIDE_TYPE_REVERSE_FREEFALL_COASTER,     // RCT1_RIDE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER
            RIDE_TYPE_SHOP,                         // RCT1_RIDE_TYPE_SOUVENIR_STALL
            RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER, // RCT1_RIDE_TYPE_VERTICAL_ROLLER_COASTER
            RIDE_TYPE_FOOD_STALL,                   // RCT1_RIDE_TYPE_PIZZA_STALL
            RIDE_TYPE_TWIST,                        // RCT1_RIDE_TYPE_TWIST
            RIDE_TYPE_HAUNTED_HOUSE,                // RCT1_RIDE_TYPE_HAUNTED_HOUSE
            RIDE_TYPE_FOOD_STALL,                   // RCT1_RIDE_TYPE_POPCORN_STALL
            RIDE_TYPE_CIRCUS,                       // RCT1_RIDE_TYPE_CIRCUS
            RIDE_TYPE_GHOST_TRAIN,                  // RCT1_RIDE_TYPE_GHOST_TRAIN
            RIDE_TYPE_TWISTER_ROLLER_COASTER,       // RCT1_RIDE_TYPE_STEEL_TWISTER_ROLLER_COASTER
            RIDE_TYPE_WOODEN_ROLLER_COASTER,        // RCT1_RIDE_TYPE_WOODEN_TWISTER_ROLLER_COASTER
            RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER, // RCT1_RIDE_TYPE_WOODEN_SIDE_FRICTION_ROLLER_COASTER
            RIDE_TYPE_STEEL_WILD_MOUSE,             // RCT1_RIDE_TYPE_STEEL_WILD_MOUSE_ROLLER_COASTER
            RIDE_TYPE_FOOD_STALL,                   // RCT1_RIDE_TYPE_HOT_DOG_STALL
            RIDE_TYPE_FOOD_STALL,                   // RCT1_RIDE_TYPE_EXOTIC_SEA_FOOD_STALL
            RIDE_TYPE_SHOP,                         // RCT1_RIDE_TYPE_HAT_STALL
            RIDE_TYPE_FOOD_STALL,                   // RCT1_RIDE_TYPE_TOFFEE_APPLE_STALL
            RIDE_TYPE_VIRGINIA_REEL,                // RCT1_RIDE_TYPE_VIRGINIA_REEL
            RIDE_TYPE_SPLASH_BOATS,                 // RCT1_RIDE_TYPE_RIVER_RIDE
            RIDE_TYPE_MINI_HELICOPTERS,             // RCT1_RIDE_TYPE_CYCLE_MONORAIL
            RIDE_TYPE_LAY_DOWN_ROLLER_COASTER,      // RCT1_RIDE_TYPE_FLYING_ROLLER_COASTER
            RIDE_TYPE_SUSPENDED_MONORAIL,           // RCT1_RIDE_TYPE_SUSPENDED_MONORAIL
            RIDE_TYPE_NULL,                         // RCT1_RIDE_TYPE_40
            RIDE_TYPE_REVERSER_ROLLER_COASTER,      // RCT1_RIDE_TYPE_WOODEN_REVERSER_ROLLER_COASTER
            RIDE_TYPE_HEARTLINE_TWISTER_COASTER,    // RCT1_RIDE_TYPE_HEARTLINE_TWISTER_ROLLER_COASTER
            RIDE_TYPE_MINI_GOLF,                    // RCT1_RIDE_TYPE_MINIATURE_GOLF
            RIDE_TYPE_NULL,                         // RCT1_RIDE_TYPE_44
            RIDE_TYPE_ROTO_DROP,                    // RCT1_RIDE_TYPE_ROTO_DROP
            RIDE_TYPE_FLYING_SAUCERS,               // RCT1_RIDE_TYPE_FLYING_SAUCERS
            RIDE_TYPE_CROOKED_HOUSE,                // RCT1_RIDE_TYPE_CROOKED_HOUSE
            RIDE_TYPE_MONORAIL_CYCLES,              // RCT1_RIDE_TYPE_CYCLE_RAILWAY
            RIDE_TYPE_COMPACT_INVERTED_COASTER,     // RCT1_RIDE_TYPE_SUSPENDED_LOOPING_ROLLER_COASTER
            RIDE_TYPE_WATER_COASTER,                // RCT1_RIDE_TYPE_WATER_COASTER
            RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER, // RCT1_RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
            RIDE_TYPE_INVERTED_HAIRPIN_COASTER,     // RCT1_RIDE_TYPE_INVERTED_WILD_MOUSE_COASTER
            RIDE_TYPE_BOAT_HIRE,                    // RCT1_RIDE_TYPE_JET_SKIS
            RIDE_TYPE_SHOP,                         // RCT1_RIDE_TYPE_T_SHIRT_STALL
            RIDE_TYPE_RIVER_RAFTS,                  // RCT1_RIDE_TYPE_RAFT_RIDE
            RIDE_TYPE_FOOD_STALL,                   // RCT1_RIDE_TYPE_DOUGHNUT_SHOP
            RIDE_TYPE_ENTERPRISE,                   // RCT1_RIDE_TYPE_ENTERPRISE
            RIDE_TYPE_DRINK_STALL,                  // RCT1_RIDE_TYPE_COFFEE_SHOP
            RIDE_TYPE_FOOD_STALL,                   // RCT1_RIDE_TYPE_FRIED_CHICKEN_STALL
            RIDE_TYPE_DRINK_STALL,                  // RCT1_RIDE_TYPE_LEMONADE_STALL
        };

        Guard::ArgumentInRange<size_t>(rideType, 0, std::size(map), "Unsupported RCT1 ride type.");
        return map[rideType];
    }

    RCT1VehicleColourSchemeCopyDescriptor GetColourSchemeCopyDescriptor(uint8_t vehicleType)
    {
        static RCT1VehicleColourSchemeCopyDescriptor map[89] =
        {
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN = 0,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN_BACKWARDS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 }, // RCT1_VEHICLE_TYPE_INVERTED_COASTER_TRAIN, // Not in RCT2
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 }, // RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_LADYBIRD_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_1, COPY_COLOUR_2 }, // RCT1_VEHICLE_TYPE_STANDUP_ROLLER_COASTER_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SPINNING_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SINGLE_PERSON_SWINGING_CHAIRS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SWANS_PEDAL_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_DARK_BLUE }, // RCT1_VEHICLE_TYPE_LARGE_MONORAIL_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_CANOES,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_ROWING_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_STEAM_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COPY_COLOUR_2 }, // RCT1_VEHICLE_TYPE_WOODEN_MOUSE_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_BUMPER_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN_BACKWARDS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_ROCKET_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_HORSES, // Steeplechase
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SPORTSCARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_LYING_DOWN_SWINGING_CARS, // Inverted single-rail
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_WOODEN_MINE_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_AIRPLANE_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SMALL_MONORAIL_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_WATER_TRICYCLES,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_LAUNCHED_FREEFALL_CAR,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_BOBSLEIGH_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_DINGHIES,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_ROTATING_CABIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_MINE_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_CORKSCREW_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_MOTORBIKES,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_RACING_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_TRUCKS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_GO_KARTS,
            { COPY_COLOUR_1, COLOUR_BLACK, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_RAPIDS_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_LOG_FLUME_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_DODGEMS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SWINGING_SHIP,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SWINGING_INVERTER_SHIP,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_MERRY_GO_ROUND,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_FERRIS_WHEEL,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SIMULATOR_POD,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_CINEMA_BUILDING,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_TOPSPIN_CAR,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SPACE_RINGS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER_CAR,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_VERTICAL_ROLLER_COASTER_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_CAT_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_TWIST_ARMS_AND_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_HAUNTED_HOUSE_BUILDING,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_LOG_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_CIRCUS_TENT,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_GHOST_TRAIN_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_WOODEN_TWISTER_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_WOODEN_SIDE_FRICTION_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_1, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_VINTAGE_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_STEAM_TRAIN_COVERED_CARS,
            { COPY_COLOUR_1, COLOUR_BLACK, COPY_COLOUR_2 }, // RCT1_VEHICLE_TYPE_STAND_UP_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_FLOORLESS_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_STEEL_MOUSE_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS_ALTERNATIVE,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SUSPENDED_MONORAIL_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_HELICOPTER_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_VIRGINIA_REEL_TUBS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_REVERSER_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_GOLFERS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_RIVER_RIDE_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_FLYING_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_NON_LOOPING_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS_REVERSED,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_RESERVED,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_ROTODROP_CAR,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_FLYING_SAUCERS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_CROOKED_HOUSE_BUILDING,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_BICYCLES,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_HYPERCOASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_4_ACROSS_INVERTED_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_WATER_COASTER_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_FACEOFF_CARS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_JET_SKIS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_RAFT_BOATS,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_AMERICAN_STYLE_STEAM_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_AIR_POWERED_COASTER_TRAIN,
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_SUSPENDED_WILD_MOUSE_CARS, // Inverted Hairpin in RCT2
            { COPY_COLOUR_1, COPY_COLOUR_2, COLOUR_BLACK }, // RCT1_VEHICLE_TYPE_ENTERPRISE_WHEEL
        };

        Guard::ArgumentInRange<size_t>(vehicleType, 0, std::size(map), "Unsupported RCT1 vehicle type.");
        return map[vehicleType];
    }

    bool RideTypeUsesVehicles(uint8_t rideType)
    {
        switch (rideType) {
        case RCT1_RIDE_TYPE_HEDGE_MAZE:
        case RCT1_RIDE_TYPE_SPIRAL_SLIDE:
        case RCT1_RIDE_TYPE_ICE_CREAM_STALL:
        case RCT1_RIDE_TYPE_CHIPS_STALL:
        case RCT1_RIDE_TYPE_DRINK_STALL:
        case RCT1_RIDE_TYPE_CANDYFLOSS_STALL:
        case RCT1_RIDE_TYPE_BURGER_BAR:
        case RCT1_RIDE_TYPE_BALLOON_STALL:
        case RCT1_RIDE_TYPE_INFORMATION_KIOSK:
        case RCT1_RIDE_TYPE_TOILETS:
        case RCT1_RIDE_TYPE_SOUVENIR_STALL:
        case RCT1_RIDE_TYPE_PIZZA_STALL:
        case RCT1_RIDE_TYPE_POPCORN_STALL:
        case RCT1_RIDE_TYPE_HOT_DOG_STALL:
        case RCT1_RIDE_TYPE_EXOTIC_SEA_FOOD_STALL:
        case RCT1_RIDE_TYPE_HAT_STALL:
        case RCT1_RIDE_TYPE_TOFFEE_APPLE_STALL:
        case RCT1_RIDE_TYPE_40:
        case RCT1_RIDE_TYPE_44:
        case RCT1_RIDE_TYPE_T_SHIRT_STALL:
        case RCT1_RIDE_TYPE_DOUGHNUT_SHOP:
        case RCT1_RIDE_TYPE_COFFEE_SHOP:
        case RCT1_RIDE_TYPE_FRIED_CHICKEN_STALL:
        case RCT1_RIDE_TYPE_LEMONADE_STALL:
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

    uint8_t GetVehicleSubEntryIndex(uint8_t vehicleSubEntry)
    {
        static constexpr const uint8_t map[] =
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
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0, // HYPERCOASTER_FRONT
            1, // HYPERCOASTER_CARRIAGE
            0, // INVERTED_4_ACROSS_CARRIAGE
            0, // WATER_COASTER_BOAT
            0,
            1, // WATER_COASTER_INVISIBLE
            0,
            0, // RIVER_RAFT
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
            0,
            0,
            0,
            0,
        };
        return map[vehicleSubEntry];
    }

    std::string_view GetRideTypeObject(uint8_t rideType)
    {
        static constexpr const char * map[] =
        {
            "rct2.ptct1",   // RCT1_RIDE_TYPE_WOODEN_ROLLER_COASTER
            "rct2.togst",   // RCT1_RIDE_TYPE_STAND_UP_STEEL_ROLLER_COASTER
            "rct2.arrsw1",  // RCT1_RIDE_TYPE_SUSPENDED_ROLLER_COASTER
            "rct2.nemt",    // RCT1_RIDE_TYPE_INVERTED_ROLLER_COASTER
            "rct2.zldb",    // RCT1_RIDE_TYPE_STEEL_MINI_ROLLER_COASTER
            "rct2.nrl",     // RCT1_RIDE_TYPE_MINIATURE_RAILWAY
            "rct2.mono2",   // RCT1_RIDE_TYPE_MONORAIL
            "rct2.batfl",   // RCT1_RIDE_TYPE_SUSPENDED_SINGLE_RAIL_ROLLER_COASTER
            "rct2.rboat",   // RCT1_RIDE_TYPE_BOAT_HIRE
            "rct2.wmouse",  // RCT1_RIDE_TYPE_WOODEN_CRAZY_RODENT_ROLLER_COASTER
            "rct2.steep1",  // RCT1_RIDE_TYPE_SINGLE_RAIL_ROLLER_COASTER
            "rct2.spcar",   // RCT1_RIDE_TYPE_CAR_RIDE
            "rct2.ssc1",    // RCT1_RIDE_TYPE_LAUNCHED_FREEFALL
            "rct2.bob1",    // RCT1_RIDE_TYPE_BOBSLED_ROLLER_COASTER
            "rct2.obs1",    // RCT1_RIDE_TYPE_OBSERVATION_TOWER
            "rct2.scht1",   // RCT1_RIDE_TYPE_STEEL_ROLLER_COASTER
            "rct2.ding1",   // RCT1_RIDE_TYPE_WATER_SLIDE
            "rct2.amt1",    // RCT1_RIDE_TYPE_MINE_TRAIN_ROLLER_COASTER
            "rct2.clift1",  // RCT1_RIDE_TYPE_CHAIRLIFT
            "rct2.arrt1",   // RCT1_RIDE_TYPE_STEEL_CORKSCREW_ROLLER_COASTER
            "rct2.hmaze",   // RCT1_RIDE_TYPE_HEDGE_MAZE
            "rct2.hskelt",  // RCT1_RIDE_TYPE_SPIRAL_SLIDE
            "rct2.kart1",   // RCT1_RIDE_TYPE_GO_KARTS
            "rct2.lfb1",    // RCT1_RIDE_TYPE_LOG_FLUME
            "rct2.rapboat", // RCT1_RIDE_TYPE_RIVER_RAPIDS
            "rct2.dodg1",   // RCT1_RIDE_TYPE_DODGEMS
            "rct2.swsh1",   // RCT1_RIDE_TYPE_SWINGING_SHIP
            "rct2.swsh2",   // RCT1_RIDE_TYPE_SWINGING_INVERTER_SHIP
            "rct2.icecr1",  // RCT1_RIDE_TYPE_ICE_CREAM_STALL
            "rct2.chpsh",   // RCT1_RIDE_TYPE_CHIPS_STALL
            "rct2.drnks",   // RCT1_RIDE_TYPE_DRINK_STALL
            "rct2.cndyf",   // RCT1_RIDE_TYPE_CANDYFLOSS_STALL
            "rct2.burgb",   // RCT1_RIDE_TYPE_BURGER_BAR
            "rct2.mgr1",    // RCT1_RIDE_TYPE_MERRY_GO_ROUND
            "rct2.balln",   // RCT1_RIDE_TYPE_BALLOON_STALL
            "rct2.infok",   // RCT1_RIDE_TYPE_INFORMATION_KIOSK
            "rct1.ride.toilets",    // RCT1_RIDE_TYPE_TOILETS
            "rct2.fwh1",    // RCT1_RIDE_TYPE_FERRIS_WHEEL
            "rct2.simpod",  // RCT1_RIDE_TYPE_MOTION_SIMULATOR
            "rct2.c3d",     // RCT1_RIDE_TYPE_3D_CINEMA
            "rct2.topsp1",  // RCT1_RIDE_TYPE_TOP_SPIN
            "rct2.srings",  // RCT1_RIDE_TYPE_SPACE_RINGS
            "rct2.revf1",   // RCT1_RIDE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER
            "rct2.souvs",   // RCT1_RIDE_TYPE_SOUVENIR_STALL
            "rct2.bmvd",    // RCT1_RIDE_TYPE_VERTICAL_ROLLER_COASTER
            "rct2.pizzs",   // RCT1_RIDE_TYPE_PIZZA_STALL
            "rct2.twist1",  // RCT1_RIDE_TYPE_TWIST
            "rct2.hhbuild", // RCT1_RIDE_TYPE_HAUNTED_HOUSE
            "rct2.popcs",   // RCT1_RIDE_TYPE_POPCORN_STALL
            "rct2.circus1", // RCT1_RIDE_TYPE_CIRCUS
            "rct2.gtc",     // RCT1_RIDE_TYPE_GHOST_TRAIN
            "rct2.bmsd",    // RCT1_RIDE_TYPE_STEEL_TWISTER_ROLLER_COASTER
            "rct2.mft",     // RCT1_RIDE_TYPE_WOODEN_TWISTER_ROLLER_COASTER
            "rct2.sfric1",  // RCT1_RIDE_TYPE_WOODEN_SIDE_FRICTION_ROLLER_COASTER
            "rct2.smc1",    // RCT1_RIDE_TYPE_STEEL_WILD_MOUSE_ROLLER_COASTER
            "rct2.hotds",   // RCT1_RIDE_TYPE_HOT_DOG_STALL
            "rct2.sqdst",   // RCT1_RIDE_TYPE_EXOTIC_SEA_FOOD_STALL
            "rct2.hatst",   // RCT1_RIDE_TYPE_HAT_STALL
            "rct2.toffs",   // RCT1_RIDE_TYPE_TOFFEE_APPLE_STALL
            "rct2.vreel",   // RCT1_RIDE_TYPE_VIRGINIA_REEL
            "rct2.spboat",  // RCT1_RIDE_TYPE_RIVER_RIDE
            "rct2.monbk",   // RCT1_RIDE_TYPE_CYCLE_MONORAIL
            "rct2.vekst",   // RCT1_RIDE_TYPE_FLYING_ROLLER_COASTER
            "rct2.smono",   // RCT1_RIDE_TYPE_SUSPENDED_MONORAIL
            "",             // RCT1_RIDE_TYPE_40
            "rct2.revcar",  // RCT1_RIDE_TYPE_WOODEN_REVERSER_ROLLER_COASTER
            "rct2.utcar",   // RCT1_RIDE_TYPE_HEARTLINE_TWISTER_ROLLER_COASTER
            "rct2.golf1",   // RCT1_RIDE_TYPE_MINIATURE_GOLF
            "",             // RCT1_RIDE_TYPE_44
            "rct2.gdrop1",  // RCT1_RIDE_TYPE_ROTO_DROP
            "rct2.fsauc",   // RCT1_RIDE_TYPE_FLYING_SAUCERS
            "rct2.chbuild", // RCT1_RIDE_TYPE_CROOKED_HOUSE
            "rct2.helicar", // RCT1_RIDE_TYPE_CYCLE_RAILWAY
            "rct2.slct",    // RCT1_RIDE_TYPE_SUSPENDED_LOOPING_ROLLER_COASTER
            "rct2.cstboat", // RCT1_RIDE_TYPE_WATER_COASTER
            "rct2.thcar",   // RCT1_RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
            "rct2.ivmc1",   // RCT1_RIDE_TYPE_INVERTED_WILD_MOUSE_COASTER
            "rct2.jski",    // RCT1_RIDE_TYPE_JET_SKIS
            "rct2.tshrt",   // RCT1_RIDE_TYPE_T_SHIRT_STALL
            "rct2.rftboat", // RCT1_RIDE_TYPE_RAFT_RIDE
            "rct2.dough",   // RCT1_RIDE_TYPE_DOUGHNUT_SHOP
            "rct2.enterp",  // RCT1_RIDE_TYPE_ENTERPRISE
            "rct2.coffs",   // RCT1_RIDE_TYPE_COFFEE_SHOP
            "rct2.chcks",   // RCT1_RIDE_TYPE_FRIED_CHICKEN_STALL
            "rct2.lemst",   // RCT1_RIDE_TYPE_LEMONADE_STALL
        };

        Guard::ArgumentInRange<size_t>(rideType, 0, std::size(map), "Unsupported RCT1 ride type.");
        return map[rideType];
    }

    std::string_view GetVehicleObject(uint8_t vehicleType)
    {
        static constexpr const char * map[] =
        {
            "rct2.scht1",       // RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN
            "rct2.scht1",       // RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN_BACKWARDS
            "rct2.ptct1",       // RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN
            "rct2.slct",        // RCT1_VEHICLE_TYPE_INVERTED_COASTER_TRAIN (Not in RCT2)
            "rct2.arrsw1",      // RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_CARS
            "rct2.zldb",        // RCT1_VEHICLE_TYPE_LADYBIRD_CARS
            "rct2.togst",       // RCT1_VEHICLE_TYPE_STANDUP_ROLLER_COASTER_CARS
            "rct2.wmspin",      // RCT1_VEHICLE_TYPE_SPINNING_CARS
            "rct2.batfl",       // RCT1_VEHICLE_TYPE_SINGLE_PERSON_SWINGING_CHAIRS
            "rct2.swans",       // RCT1_VEHICLE_TYPE_SWANS_PEDAL_BOATS
            "rct2.mono1",       // RCT1_VEHICLE_TYPE_LARGE_MONORAIL_TRAIN
            "rct2.cboat",       // RCT1_VEHICLE_TYPE_CANOES
            "rct2.rboat",       // RCT1_VEHICLE_TYPE_ROWING_BOATS
            "rct2.nrl",         // RCT1_VEHICLE_TYPE_STEAM_TRAIN
            "rct2.wmouse",      // RCT1_VEHICLE_TYPE_WOODEN_MOUSE_CARS
            "rct2.bboat",       // RCT1_VEHICLE_TYPE_BUMPER_BOATS
            "rct2.ptct1",       // RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN_BACKWARDS
            "rct2.rckc",        // RCT1_VEHICLE_TYPE_ROCKET_CARS
            "rct2.steep1",      // RCT1_VEHICLE_TYPE_HORSES // Steeplechase
            "rct2.spcar",       // RCT1_VEHICLE_TYPE_SPORTSCARS
            "rct2.skytr",       // RCT1_VEHICLE_TYPE_LYING_DOWN_SWINGING_CARS (Inverted single-rail)
            "rct2.wmmine",      // RCT1_VEHICLE_TYPE_WOODEN_MINE_CARS
            "rct2.arrsw2",      // RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_AIRPLANE_CARS
            "rct2.mono2",       // RCT1_VEHICLE_TYPE_SMALL_MONORAIL_CARS
            "rct2.trike",       // RCT1_VEHICLE_TYPE_WATER_TRICYCLES
            "rct2.ssc1",        // RCT1_VEHICLE_TYPE_LAUNCHED_FREEFALL_CAR
            "rct2.bob1",        // RCT1_VEHICLE_TYPE_BOBSLEIGH_CARS
            "rct2.ding1",       // RCT1_VEHICLE_TYPE_DINGHIES
            "rct2.obs1",        // RCT1_VEHICLE_TYPE_ROTATING_CABIN
            "rct2.amt1",        // RCT1_VEHICLE_TYPE_MINE_TRAIN
            "rct2.clift1",      // RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS
            "rct2.arrt1",       // RCT1_VEHICLE_TYPE_CORKSCREW_ROLLER_COASTER_TRAIN
            "rct2.steep2",      // RCT1_VEHICLE_TYPE_MOTORBIKES
            "rct2.rcr",         // RCT1_VEHICLE_TYPE_RACING_CARS
            "rct2.truck1",      // RCT1_VEHICLE_TYPE_TRUCKS
            "rct2.kart1",       // RCT1_VEHICLE_TYPE_GO_KARTS
            "rct2.rapboat",     // RCT1_VEHICLE_TYPE_RAPIDS_BOATS
            "rct2.lfb1",        // RCT1_VEHICLE_TYPE_LOG_FLUME_BOATS
            "rct2.dodg1",       // RCT1_VEHICLE_TYPE_DODGEMS
            "rct2.swsh1",       // RCT1_VEHICLE_TYPE_SWINGING_SHIP
            "rct2.swsh2",       // RCT1_VEHICLE_TYPE_SWINGING_INVERTER_SHIP
            "rct2.mgr1",        // RCT1_VEHICLE_TYPE_MERRY_GO_ROUND
            "rct2.fwh1",        // RCT1_VEHICLE_TYPE_FERRIS_WHEEL
            "rct2.simpod",      // RCT1_VEHICLE_TYPE_SIMULATOR_POD
            "rct2.c3d",         // RCT1_VEHICLE_TYPE_CINEMA_BUILDING
            "rct2.topsp1",      // RCT1_VEHICLE_TYPE_TOPSPIN_CAR
            "rct2.srings",      // RCT1_VEHICLE_TYPE_SPACE_RINGS
            "rct2.revf1",       // RCT1_VEHICLE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER_CAR
            "rct2.bmvd",        // RCT1_VEHICLE_TYPE_VERTICAL_ROLLER_COASTER_CARS
            "rct2.ctcar",       // RCT1_VEHICLE_TYPE_CAT_CARS
            "rct2.twist1",      // RCT1_VEHICLE_TYPE_TWIST_ARMS_AND_CARS
            "rct2.hhbuild",     // RCT1_VEHICLE_TYPE_HAUNTED_HOUSE_BUILDING
            "rct2.zlog",        // RCT1_VEHICLE_TYPE_LOG_CARS
            "rct2.circus1",     // RCT1_VEHICLE_TYPE_CIRCUS_TENT
            "rct2.gtc",         // RCT1_VEHICLE_TYPE_GHOST_TRAIN_CARS
            "rct2.bmsd",        // RCT1_VEHICLE_TYPE_STEEL_TWISTER_ROLLER_COASTER_TRAIN
            "rct2.mft",         // RCT1_VEHICLE_TYPE_WOODEN_TWISTER_ROLLER_COASTER_TRAIN
            "rct2.sfric1",      // RCT1_VEHICLE_TYPE_WOODEN_SIDE_FRICTION_CARS
            "rct2.vcr",         // RCT1_VEHICLE_TYPE_VINTAGE_CARS
            "rct2.nrl2",        // RCT1_VEHICLE_TYPE_STEAM_TRAIN_COVERED_CARS
            "rct2.bmsu",        // RCT1_VEHICLE_TYPE_STAND_UP_STEEL_TWISTER_ROLLER_COASTER_TRAIN
            "rct2.bmfl",        // RCT1_VEHICLE_TYPE_FLOORLESS_STEEL_TWISTER_ROLLER_COASTER_TRAIN
            "rct2.smc1",        // RCT1_VEHICLE_TYPE_STEEL_MOUSE_CARS
            "rct2.clift2",      // RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS_ALTERNATIVE
            "rct2.smono",       // RCT1_VEHICLE_TYPE_SUSPENDED_MONORAIL_TRAIN
            "rct2.helicar",     // RCT1_VEHICLE_TYPE_HELICOPTER_CARS
            "rct2.vreel",       // RCT1_VEHICLE_TYPE_VIRGINIA_REEL_TUBS
            "rct2.revcar",      // RCT1_VEHICLE_TYPE_REVERSER_CARS
            "rct2.golf1",       // RCT1_VEHICLE_TYPE_GOLFERS
            "rct2.spboat",      // RCT1_VEHICLE_TYPE_RIVER_RIDE_BOATS
            "rct2.vekst",       // RCT1_VEHICLE_TYPE_FLYING_ROLLER_COASTER_TRAIN
            "rct2.bmrb",        // RCT1_VEHICLE_TYPE_NON_LOOPING_STEEL_TWISTER_ROLLER_COASTER_TRAIN
            "rct2.utcar",       // RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS
            "rct2.utcarr",      // RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS_REVERSED
            "rct2.",            // RCT1_VEHICLE_TYPE_RESERVED
            "rct2.gdrop1",      // RCT1_VEHICLE_TYPE_ROTODROP_CAR
            "rct2.fsauc",       // RCT1_VEHICLE_TYPE_FLYING_SAUCERS
            "rct2.chbuild",     // RCT1_VEHICLE_TYPE_CROOKED_HOUSE_BUILDING
            "rct2.monbk",       // RCT1_VEHICLE_TYPE_BICYCLES
            "rct2.arrt2",       // RCT1_VEHICLE_TYPE_HYPERCOASTER_TRAIN
            "rct2.nemt",        // RCT1_VEHICLE_TYPE_4_ACROSS_INVERTED_COASTER_TRAIN
            "rct2.cstboat",     // RCT1_VEHICLE_TYPE_WATER_COASTER_BOATS
            "rct2.slcfo",       // RCT1_VEHICLE_TYPE_FACEOFF_CARS
            "rct2.jski",        // RCT1_VEHICLE_TYPE_JET_SKIS
            "rct2.rftboat",     // RCT1_VEHICLE_TYPE_RAFT_BOATS
            "rct2.aml1",        // RCT1_VEHICLE_TYPE_AMERICAN_STYLE_STEAM_TRAIN
            "rct2.thcar",       // RCT1_VEHICLE_TYPE_AIR_POWERED_COASTER_TRAIN
            "rct2.ivmc1",       // RCT1_VEHICLE_TYPE_SUSPENDED_WILD_MOUSE_CARS (Inverted Hairpin in RCT2)
            "rct2.enterp",      // RCT1_VEHICLE_TYPE_ENTERPRISE_WHEEL
        };

        Guard::ArgumentInRange<size_t>(vehicleType, 0, std::size(map), "Unsupported RCT1 vehicle type.");
        return map[vehicleType];
    }

    std::string_view GetSmallSceneryObject(uint8_t smallSceneryType)
    {
        static constexpr const char * map[] =
        {
            "rct2.tl0",
            "rct2.tl1",
            "rct2.tl2",
            "rct2.tl3",
            "rct2.tm0",
            "rct2.tm1",
            "rct2.tm2",
            "rct2.tm3",
            "rct2.ts0",
            "rct2.ts1",
            "rct2.ts2",
            "rct2.ts3",
            "rct2.ts4",
            "rct2.ts5",
            "rct2.ts6",
            "rct2.tic",
            "rct2.tlc",
            "rct2.tmc",
            "rct2.tmp",
            "rct2.titc",
            "rct2.tghc",
            "rct2.tac",
            "rct2.tghc2",
            "rct2.tcj",
            "rct2.tmbj",
            "rct2.tcf",
            "rct2.tcl",
            "rct2.trf",
            "rct2.trf2",
            "rct2.tel",
            "rct2.tap",
            "rct2.tsp",
            "rct2.tmzp",
            "rct2.tcrp",
            "rct2.tbp",
            "rct2.tlp",
            "rct2.twp",
            "rct2.tas",
            "rct2.tmg",
            "rct2.tww",
            "rct2.tsb",
            "rct2.tvl",
            "rct2.tct",
            "rct2.tef",
            "rct2.tal",
            "rct2.tsq",
            "rct2.tht",
            "rct2.tcb",
            "rct2.tdm",
            "rct2.tsd",
            "rct2.tgs",
            "rct2.tus",
            "rct2.th1",
            "rct2.tbc",
            "rct2.th2",
            "rct2.tpm",
            "rct2.tsc",
            "rct2.tg1",
            "rct2.twf",
            "rct2.tsh0",
            "rct2.tsh1",
            "rct2.tsh2",
            "rct2.tsh3",
            "rct2.tsh4",
            "rct2.tsh5",
            "rct2.tg2",
            "rct2.tg3",
            "rct2.tg4",
            "rct2.tg5",
            "rct2.tg6",
            "rct2.tg7",
            "rct2.tg8",
            "rct2.tg9",
            "rct2.tg10",
            "rct2.tg11",
            "rct2.tg12",
            "rct2.tg13",
            "rct2.tg14",
            "rct2.tt1",
            "rct2.tdf",
            "rct2.tsh",
            "rct2.thrs",
            "rct2.tstd",
            "rct2.trms",
            "rct2.trws",
            "rct2.trc",
            "rct2.tqf",
            "rct2.tes1",
            "rct2.ten",
            "rct2.ters",
            "rct2.terb",
            "rct2.tep",
            "rct2.tst1",
            "rct2.tst2",
            "rct2.tms1",
            "rct2.tas1",
            "rct2.tas2",
            "rct2.tas3",
            "rct2.tst3",
            "rct2.tst4",
            "rct2.tst5",
            "rct2.tas4",
            "rct2.tcy",
            "rct2.tbw",
            "rct2.tbr1",
            "rct2.tbr2",
            "rct2.tml",
            "rct2.tmw",
            "rct2.tbr3",
            "rct2.tbr4",
            "rct2.tmj",
            "rct2.tbr",
            "rct2.tmo1",
            "rct2.tmo2",
            "rct2.tmo3",
            "rct2.tmo4",
            "rct2.tmo5",
            "rct2.twh1",
            "rct2.twh2",
            "rct2.tns",
            "rct2.tp1",
            "rct2.tp2",
            "rct2.tk1",
            "rct2.tk2",
            "rct2.tr1",
            "rct2.tr2",
            "rct2.tq1",
            "rct2.tq2",
            "rct2.twn",
            "rct2.tce",
            "rct2.tco",
            "rct2.thl",
            "rct2.tcc",
            "rct2.tb1",
            "rct2.tb2",
            "rct2.tk3",
            "rct2.tk4",
            "rct2.tbn",
            "rct2.tbn1",
            "rct2.tdt1",
            "rct2.tdt2",
            "rct2.tdt3",
            "rct2.tmm1",
            "rct2.tmm2",
            "rct2.tmm3",
            "rct2.tgs1",
            "rct2.tgs2",
            "rct2.tgs3",
            "rct2.tgs4",
            "rct2.tdn4",
            "rct2.tdn5",
            "rct2.tjt1",
            "rct2.tjt2",
            "rct2.tjb1",
            "rct2.ttf",
            "rct2.tf1",
            "rct2.tf2",
            "rct2.tge1",
            "rct2.tjt3",
            "rct2.tjt4",
            "rct2.tjp1",
            "rct2.tjb2",
            "rct2.tge2",
            "rct2.tjt5",
            "rct2.tjb3",
            "rct2.tjb4",
            "rct2.tjt6",
            "rct2.tjp2",
            "rct2.tge3",
            "rct2.tck",
            "rct2.tge4",
            "rct2.tge5",
            "rct2.tg15",
            "rct2.tg16",
            "rct2.tg17",
            "rct2.tg18",
            "rct2.tg19",
            "rct2.tg20",
            "rct2.tg21",
            "rct2.tsm",
            "rct2.tig",
            "rct2.tcfs",
            "rct2.trfs",
            "rct2.trf3",
            "rct2.tnss",
            "rct2.tct1",
            "rct2.tct2",
            "rct2.tsf1",
            "rct2.tsf2",
            "rct2.tsf3",
            "rct2.tcn",
            "rct2.ttg",
            "rct2.tsnc",
            "rct2.tsnb",
            "rct2.tscp",
            "rct2.tcd",
            "rct2.tsg",
            "rct2.tsk",
            "rct2.tgh1",
            "rct2.tgh2",
            "rct2.tsmp",
            "rct2.tjf",
            "rct2.tly",
            "rct2.tgc1",
            "rct2.tgc2",
            "rct2.tgg",
            "rct2.tsph",
            "rct2.toh1",
            "rct2.toh2",
            "rct2.tot1",
            "rct2.tot2",
            "rct2.tos",
            "rct2.tot3",
            "rct2.tot4",
            "rct2.tsc2",
            "rct2.tsp1",
            "rct2.toh3",
            "rct2.tsp2",
            "rct2.romroof1",
            "rct2.georoof1",
            "rct2.tntroof1",
            "rct2.jngroof1",
            "rct2.minroof1",
            "rct2.romroof2",
            "rct2.georoof2",
            "rct2.pagroof1",
            "rct2.spcroof1",
            "rct2.roof1",
            "rct2.roof2",
            "rct2.roof3",
            "rct2.roof4",
            "rct2.roof5",
            "rct2.roof6",
            "rct2.roof7",
            "rct2.roof8",
            "rct2.roof9",
            "rct2.roof10",
            "rct2.roof11",
            "rct2.roof12",
            "rct2.roof13",
            "rct2.roof14",
            "rct2.igroof",
            "rct2.corroof",
            "rct2.corroof2",
        };
        return map[smallSceneryType];
    }

    std::string_view GetLargeSceneryObject(uint8_t largeSceneryType)
    {
        static constexpr const char * map[] =
        {
            "rct2.scol",
            "rct2.shs1",
            "rct2.sspx",
            "rct2.shs2",
            "rct2.scln",
            "rct2.smh1",
            "rct2.smh2",
            "rct2.svlc",
            "rct2.spyr",
            "rct2.smn1",
            "rct2.smb",
            "rct2.ssk1",
            "rct2.sdn1",
            "rct2.sdn2",
            "rct2.sdn3",
            "rct2.sip",
            "rct2.stb1",
            "rct2.stb2",
            "rct2.stg1",
            "rct2.stg2",
            "rct2.sct",
            "rct2.soh1",
            "rct2.soh2",
            "rct2.soh3",
            "rct2.sgp",
            "rct2.ssr",
            "rct2.sth",
            "rct2.sah",
            "rct2.sps",
            "rct2.spg",
            "rct2.sob",
            "rct2.sah2",
            "rct2.sst",
            "rct2.ssh",
            "rct2.sah3",
            "rct2.ssig1",
            "rct2.ssig2",
            "rct2.ssig3",
            "rct2.ssig4",
        };
        return map[largeSceneryType];
    }

    std::string_view GetWallObject(uint8_t wallType)
    {
        static constexpr const char * map[] =
        {
            "rct2.wmf",         // RCT1_WALL_TYPE_MESH_FENCE
            "rct2.wmfg",        // RCT1_WALL_TYPE_MESH_FENCE_WITH_GATE
            "rct2.wrw",         // RCT1_WALL_TYPE_ROMAN
            "rct2.wew",         // RCT1_WALL_TYPE_EGYPTIAN
            "rct2.whg",         // RCT1_WALL_TYPE_HEDGE
            "rct2.whgg",        // RCT1_WALL_TYPE_HEDGE_WITH_GATE
            "rct2.wcw1",        // RCT1_WALL_TYPE_BLUE_PLAYING_CARDS
            "rct2.wcw2",        // RCT1_WALL_TYPE_RED_PLAYING_CARDS
            "rct2.wsw",         // RCT1_WALL_TYPE_WHITE_RAILING
            "rct2.wswg",        // RCT1_WALL_TYPE_WHITE_RAILING_WITH_GATE
            "rct2.wmw",         // RCT1_WALL_TYPE_MARTIAN
            "rct2.wallgl16",    // RCT1_WALL_TYPE_GLASS_SMOOTH
            "rct2.wfw1",        // RCT1_WALL_TYPE_WOODEN_PANEL_FENCE
            "rct2.wfwg",        // RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_GATE
            "rct2.wpw1",        // RCT1_WALL_TYPE_WOODEN_POST_FENCE
            "rct2.wpw2",        // RCT1_WALL_TYPE_RED_WOODEN_POST_FENCE
            "rct2.wpf",         // RCT1_WALL_TYPE_BARBED_WIRE
            "rct2.wpfg",        // RCT1_WALL_TYPE_BARBED_WIRE_WITH_GATE
            "rct2.wwtw",        // RCT1_WALL_TYPE_PRIMITIVE_TALL_WOOD_FENCE
            "rct2.wmww",        // RCT1_WALL_TYPE_PRIMITIVE_SHORT_WOOD_FENCE
            "rct2.wsw1",        // RCT1_WALL_TYPE_IRON_RAILING
            "rct2.wsw2",        // RCT1_WALL_TYPE_IRON_RAILING_WITH_GATE
            "rct2.wgw2",        // RCT1_WALL_TYPE_GLASS_PANELS
            "rct2.wbw",         // RCT1_WALL_TYPE_BONE_FENCE
            "rct2.wbr1",        // RCT1_WALL_TYPE_BRICK
            "rct2.wbrg",        // RCT1_WALL_TYPE_BRICK_WITH_GATE
            "rct2.wfw1",        // RCT1_WALL_TYPE_WHITE_WOODEN_PANEL_FENCE
            "rct2.wfw1",        // RCT1_WALL_TYPE_RED_WOODEN_PANEL_FENCE
            "rct2.wbr2",        // RCT1_WALL_TYPE_STONE
            "rct2.wbr3",        // RCT1_WALL_TYPE_STONE_WITH_GATE
            "rct2.wpw3",        // RCT1_WALL_TYPE_WOODEN_FENCE
            "rct2.wjf",         // RCT1_WALL_TYPE_JUNGLE
            "rct2.wch",         // RCT1_WALL_TYPE_CONIFER_HEDGE
            "rct2.wchg",        // RCT1_WALL_TYPE_CONIFER_HEDGE_WITH_GATE
            "rct2.wc1",         // RCT1_WALL_TYPE_SMALL_BROWN_CASTLE
            "rct2.wc2",         // RCT1_WALL_TYPE_SMALL_GREY_CASTLE
            "rct2.wc3",         // RCT1_WALL_TYPE_ROMAN_COLUMN
            "rct2.wc4",         // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE
            "rct2.wc5",         // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_CROSS
            "rct2.wc6",         // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_GATE
            "rct2.wc7",         // RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_WINDOW
            "rct2.wc8",         // RCT1_WALL_TYPE_MEDIUM_BROWN_CASTLE
            "rct2.wc9",         // RCT1_WALL_TYPE_LARGE_GREY_CASTLE
            "rct2.wc10",        // RCT1_WALL_TYPE_LARGE_GREY_CASTLE_CROSS
            "rct2.wc11",        // RCT1_WALL_TYPE_LARGE_GREY_CASTLE_GATE
            "rct2.wc12",        // RCT1_WALL_TYPE_LARGE_GREY_CASTLE_WINDOW
            "rct2.wc13",        // RCT1_WALL_TYPE_MEDIUM_GREY_CASTLE
            "rct2.wc14",        // RCT1_WALL_TYPE_CREEPY
            "rct2.wc15",        // RCT1_WALL_TYPE_CREEPY_GATE
            "rct2.wc16",        // RCT1_WALL_TYPE_BARBED_WIRE_WITH_SNOW
            "rct2.wc17",        // RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_SNOW
            "rct2.wc18",        // RCT1_WALL_TYPE_WOODEN_POST_FENCE_WITH_SNOW
        };
        if (wallType < std::size(map))
            return map[wallType];
        else
            return map[0];
    }

    std::string_view GetPathSurfaceObject(uint8_t pathType)
    {
        static constexpr const char * map[] =
        {
            "rct1.pathsurface.queue.blue",         // RCT1_FOOTPATH_TYPE_QUEUE_BLUE
            "rct1.pathsurface.queue.red",          // RCT1_FOOTPATH_TYPE_QUEUE_RED
            "rct1.pathsurface.queue.yellow",       // RCT1_FOOTPATH_TYPE_QUEUE_YELLOW
            "rct1.pathsurface.queue.green",        // RCT1_FOOTPATH_TYPE_QUEUE_GREEN

            "rct1.pathsurface.tarmac",             // RCT1_FOOTPATH_TYPE_TARMAC_GRAY
            "rct1.aa.pathsurface.space",           // RCT1_FOOTPATH_TYPE_TARMAC_RED
            "rct1.aa.pathsurface.tarmac.brown",    // RCT1_FOOTPATH_TYPE_TARMAC_BROWN
            "rct1.aa.pathsurface.tarmac.green",    // RCT1_FOOTPATH_TYPE_TARMAC_GREEN

            "rct1.pathsurface.dirt",               // RCT1_FOOTPATH_TYPE_DIRT_RED
            "rct1.aa.pathsurface.ash",             // RCT1_FOOTPATH_TYPE_DIRT_BLACK
            "",
            "",

            "rct1.pathsurface.crazy",              // RCT1_FOOTPATH_TYPE_CRAZY_PAVING
            "",
            "",
            "",

            "rct2.pathsurface.road",               // RCT1_FOOTPATH_TYPE_ROADS
            "",
            "",
            "",

            "rct1.pathsurface.tile.pink",          // RCT1_FOOTPATH_TYPE_TILE_PINK
            "rct1.aa.pathsurface.tile.grey",       // RCT1_FOOTPATH_TYPE_TILE_GRAY
            "rct1.ll.pathsurface.tile.red",        // RCT1_FOOTPATH_TYPE_TILE_RED
            "rct1.ll.pathsurface.tile.green",      // RCT1_FOOTPATH_TYPE_TILE_GREEN
        };
        return map[pathType];
    }

    std::string_view GetPathAddtionObject(uint8_t pathAdditionType)
    {
        static constexpr const char * map[] =
        {
            "",                 // RCT1_PATH_ADDITION_NONE
            "rct2.lamp1",       // RCT1_PATH_ADDITION_LAMP_1
            "rct2.lamp2",       // RCT1_PATH_ADDITION_LAMP_2
            "rct2.litter1",     // RCT1_PATH_ADDITION_BIN
            "rct2.bench1",      // RCT1_PATH_ADDITION_BENCH
            "rct2.jumpfnt1",    // RCT1_PATH_ADDITION_JUMPING_FOUNTAIN
            "rct2.lamp3",       // RCT1_PATH_ADDITION_LAMP_3
            "rct2.lamp4",       // RCT1_PATH_ADDITION_LAMP_4
            "rct2.lamp1",       // RCT1_PATH_ADDITION_BROKEN_LAMP_1
            "rct2.lamp2",       // RCT1_PATH_ADDITION_BROKEN_LAMP_2
            "rct2.litter1",     // RCT1_PATH_ADDITION_BROKEN_BIN
            "rct2.bench1",      // RCT1_PATH_ADDITION_BROKEN_BENCH
            "rct2.lamp3",       // RCT1_PATH_ADDITION_BROKEN_LAMP_3
            "rct2.lamp4",       // RCT1_PATH_ADDITION_BROKEN_LAMP_4
            "rct2.jumpsnw1",    // RCT1_PATH_ADDITION_JUMPING_SNOW
        };
        return map[pathAdditionType];
    }

    std::string_view GetFootpathRailingsObject(uint8_t footpathRailingsType)
    {
        static constexpr const char * map[] =
        {
            "rct2.railings.wood",       // RCT1_PATH_SUPPORT_TYPE_TRUSS
            "rct2.railings.concrete",   // RCT1_PATH_SUPPORT_TYPE_COATED_WOOD
            "rct1.railings.space",      // RCT1_PATH_SUPPORT_TYPE_SPACE
            "rct1.railings.bamboo",     // RCT1_PATH_SUPPORT_TYPE_BAMBOO
        };
        return map[footpathRailingsType];
    }

    std::string_view GetSceneryGroupObject(uint8_t sceneryGroupType)
    {
        static constexpr const char * map[] =
        {
            "",                 // RCT1_SCENERY_THEME_GENERAL
            "rct2.scgmine",     // RCT1_SCENERY_THEME_MINE
            "rct2.scgclass",    // RCT1_SCENERY_THEME_CLASSICAL_ROMAN
            "rct2.scgegypt",    // RCT1_SCENERY_THEME_EGYPTIAN
            "rct2.scgmart",     // RCT1_SCENERY_THEME_MARTIAN
            "",                 // RCT1_SCENERY_THEME_JUMPING_FOUNTAINS
            "rct2.scgwond",     // RCT1_SCENERY_THEME_WONDERLAND
            "rct2.scgjuras",    // RCT1_SCENERY_THEME_JURASSIC
            "rct2.scgspook",    // RCT1_SCENERY_THEME_SPOOKY
            "rct2.scgjungl",    // RCT1_SCENERY_THEME_JUNGLE
            "rct2.scgabstr",    // RCT1_SCENERY_THEME_ABSTRACT
            "",                 // RCT1_SCENERY_THEME_GARDEN_CLOCK
            "rct2.scgsnow",     // RCT1_SCENERY_THEME_SNOW_ICE
            "rct2.scgmedie",    // RCT1_SCENERY_THEME_MEDIEVAL
            "rct2.scgspace",    // RCT1_SCENERY_THEME_SPACE
            "rct2.scghallo",    // RCT1_SCENERY_THEME_CREEPY
            "rct2.scgurban",    // RCT1_SCENERY_THEME_URBAN
            "rct2.scgorien",    // RCT1_SCENERY_THEME_PAGODA
        };
        return map[sceneryGroupType];
    }

    std::string_view GetWaterObject(uint8_t waterType)
    {
        static constexpr const char * map[] =
        {
            "rct2.wtrcyan",
            "rct2.wtrorng",
        };
        return map[waterType];
    }

    const std::vector<const char *> GetSceneryObjects(uint8_t sceneryType)
    {
        static const std::vector<const char *> map[] =
        {
            // RCT1_SCENERY_THEME_GENERAL (trees, shrubs, garden, walls, fence, path accessories)
            { "rct2.tic", "rct2.tlc", "rct2.tmc", "rct2.tmp", "rct2.titc", "rct2.tghc", "rct2.tac", "rct2.tghc2", "rct2.tcj", "rct2.tmbj", "rct2.tcf", "rct2.tcl", "rct2.trf", "rct2.trf2", "rct2.tel", "rct2.tap", "rct2.tsp", "rct2.tmzp", "rct2.tcrp", "rct2.tbp", "rct2.tlp", "rct2.twp", "rct2.tas", "rct2.tmg", "rct2.tww", "rct2.tsb", "rct2.tvl", "rct2.tcy", "rct2.tns", "rct2.twn", "rct2.tce", "rct2.tco", "rct2.thl", "rct2.tcc", "rct2.tf1", "rct2.tf2", "rct2.tct", "rct2.th1", "rct2.th2", "rct2.tpm", "rct2.tropt1",
              "rct2.ts0", "rct2.ts1", "rct2.ts2", "rct2.ts3", "rct2.ts4", "rct2.ts5", "rct2.ts6", "rct2.tef", "rct2.tal", "rct2.tsq", "rct2.tht", "rct2.tcb", "rct2.tdm", "rct2.tsd", "rct2.torn1", "rct2.torn2", "rct2.tgs", "rct2.tus", "rct2.tbc", "rct2.tsc", "rct2.twf", "rct2.tsh0", "rct2.tsh1", "rct2.tsh2", "rct2.tsh3", "rct2.tsh4", "rct2.tsh5", "rct2.tdf", "rct2.tsh", "rct2.thrs", "rct2.tstd", "rct2.tbr", "rct2.ttf", "rct2.whg", "rct2.whgg", "rct2.wch", "rct2.wchg",
              "rct2.tg1", "rct2.tg2", "rct2.tg3", "rct2.tg4", "rct2.tg5", "rct2.tg6", "rct2.tg7", "rct2.tg8", "rct2.tg9", "rct2.tg10", "rct2.tg11", "rct2.tg12", "rct2.tg13", "rct2.tg14", "rct2.tg15", "rct2.tg16", "rct2.tg17", "rct2.tg18", "rct2.tg19", "rct2.tg20", "rct2.tg21",
              "rct2.wbr1a", "rct2.wbr2a", "rct2.wallbb34", "rct2.walltn32", "rct2.tntroof1", "rct2.wallbb33", "rct2.wallbb32", "rct2.wallbb16", "rct2.wallbb8", "rct2.roof5", "rct2.roof7", "rct2.wallrs32", "rct2.wallrs16", "rct2.wallrs8", "rct2.wallbr32", "rct2.wallbr16", "rct2.wallbr8", "rct2.wallbrdr", "rct2.wallbrwn", "rct2.brbase", "rct2.roof1", "rct2.roof2", "rct2.roof3", "rct2.roof4", "rct2.wallcb32", "rct2.wallcb16", "rct2.wallcb8", "rct2.wallcbdr", "rct2.wallcbwn", "rct2.brbase2", "rct2.cwbcrv33", "rct2.cwbcrv32", "rct2.brcrrf1", "rct2.roof6", "rct2.roof8", "rct2.wallcf32", "rct2.wallcf16", "rct2.wallcf8", "rct2.wallcfdr", "rct2.wallcfwn", "rct2.wallcfar", "rct2.brbase3", "rct2.cwfcrv33", "rct2.cwfcrv32", "rct2.brcrrf2", "rct2.roof9", "rct2.roof11", "rct2.roof10", "rct2.roof12", "rct2.corroof2", "rct2.wallco16", "rct2.corroof", "rct2.walllt32", "rct2.wallsk16", "rct2.wallsk32", "rct2.sktdw2", "rct2.sktdw", "rct2.sktbase", "rct2.sktbaset", "rct2.suppw2", "rct2.suppw1", "rct2.suppw3", "rct2.suppleg1", "rct2.suppleg2", "rct2.sumrf", "rct2.wallrh32",
              "rct2.wmf", "rct2.wmfg", "rct2.wsw", "rct2.wswg", "rct2.wfw1", "rct2.wfwg", "rct2.wpf", "rct2.wpfg", "rct2.wsw1", "rct2.wsw2", "rct2.wbr1", "rct2.wbrg", "rct2.wbr2", "rct2.wbr3", "rct2.wallmm16", "rct2.wallmm17",
              "rct2.lamp1", "rct2.lamp2", "rct2.litter1", "rct2.bench1", "rct2.qtv1", "rct2.bn1", "rct2.wallpost", "rct2.wallsign", "rct2.ssig1", "rct2.ssig2", "rct2.ssig3", "rct2.ssig4" },
            // RCT1_SCENERY_THEME_MINE
            { "rct2.smh1", "rct2.smh2", "rct2.smn1", "rct2.tbw", "rct2.tbr1", "rct2.tbr2", "rct2.tml", "rct2.tmw", "rct2.tbr3", "rct2.tbr4", "rct2.tmj", "rct2.bn5", "rct2.wallwd8", "rct2.wallwd16", "rct2.wallwd32", "rct2.wallwd33", "rct2.wallmn32", "rct2.wdbase", "rct2.minroof1", "rct2.roof13", "rct2.littermn" },
            // RCT1_SCENERY_THEME_CLASSICAL_ROMAN
            { "rct2.scol", "rct2.tt1", "rct2.trms", "rct2.trws", "rct2.trc", "rct2.tqf", "rct2.wrw", "rct2.wrwa", "rct2.romroof2", "rct2.wc3", "rct2.romroof1", "rct2.bn3" },
            // RCT1_SCENERY_THEME_EGYPTIAN
            { "rct2.sspx", "rct2.scln", "rct2.spyr", "rct2.tes1", "rct2.ten", "rct2.ters", "rct2.terb", "rct2.tep", "rct2.wew", "rct2.lamp3", "rct2.bn4", "rct2.benchstn" },
            // RCT1_SCENERY_THEME_MARTIAN
            { "rct2.smb", "rct2.tmo1", "rct2.tmo2", "rct2.tmo3", "rct2.tmo4", "rct2.tmo5", "rct2.svlc", "rct2.wmw", "rct2.lamp4" },
            // RCT1_SCENERY_THEME_JUMPING_FOUNTAINS (Single researchable scenery item)
            { "rct2.jumpfnt1" },
            // RCT1_SCENERY_THEME_WONDERLAND
            { "rct2.twh1", "rct2.twh2", "rct2.tst1", "rct2.tst2", "rct2.tms1", "rct2.tst3", "rct2.tst4", "rct2.tst5", "rct2.tas1", "rct2.tas2", "rct2.tas3", "rct2.tas4", "rct2.chbbase", "rct2.tp1", "rct2.tp2", "rct2.tk1", "rct2.tk2", "rct2.tr1", "rct2.tr2", "rct2.tq1", "rct2.tq2", "rct2.tb1", "rct2.tb2", "rct2.tk3", "rct2.tk4", "rct2.wcw1", "rct2.wcw2" },
            // RCT1_SCENERY_THEME_JURASSIC
            { "rct2.tbn", "rct2.tbn1", "rct2.tdn4", "rct2.tdn5", "rct2.sdn1", "rct2.sdn2", "rct2.sdn3", "rct2.wwtw", "rct2.wmww", "rct2.wwtwa", "rct2.wbw", "rct2.bn6" },
            // RCT1_SCENERY_THEME_SPOOKY,
            { "rct2.ssk1", "rct2.tdt1", "rct2.tdt2", "rct2.tdt3", "rct2.tmm1", "rct2.tmm2", "rct2.tmm3", "rct2.tgs1", "rct2.tgs2", "rct2.tgs3", "rct2.tgs4", "rct2.smskull", "rct2.wallrk32" },
            // RCT1_SCENERY_THEME_JUNGLE
            { "rct2.tjt1", "rct2.tjt2", "rct2.tjb1", "rct2.tjt3", "rct2.tjt4", "rct2.tjp1", "rct2.tjb2", "rct2.tjt5", "rct2.tjb3", "rct2.tjb4", "rct2.tjt6", "rct2.tjp2", "rct2.tjf", "rct2.wpw1", "rct2.wpw2", "rct2.wjf", "rct2.bn2", "rct2.walljn32", "rct2.jngroof1", "rct2.roof14", "rct2.benchlog" },
            // RCT1_SCENERY_THEME_ABSTRACT
            { "rct2.tge1", "rct2.tge2", "rct2.tge3", "rct2.tge4", "rct2.tge5", "rct2.tgc1", "rct2.tgc2", "rct2.wallgl8", "rct2.wallgl16", "rct2.wallgl32", "rct2.georoof1", "rct2.wgw2", "rct2.georoof2" },
            // RCT1_SCENERY_THEME_GARDEN_CLOCK (Single researchable scenery item)
            { "rct2.tck" },
            // RCT1_SCENERY_THEME_SNOW_ICE
            { "rct2.sip", "rct2.tsm", "rct2.tig", "rct2.tsf1", "rct2.tsf2", "rct2.tsf3", "rct2.tsnc", "rct2.tsnb", "rct2.wc16", "rct2.wc17", "rct2.wc18", "rct2.jumpsnw1", "rct2.tcfs", "rct2.trfs", "rct2.trf3", "rct2.tnss", "rct2.bn8", "rct2.wallig16", "rct2.wallig24", "rct2.igroof" },
            // RCT1_SCENERY_THEME_MEDIEVAL
            { "rct2.tct1", "rct2.stb1", "rct2.stb2", "rct2.wc1", "rct2.wc4", "rct2.wc5", "rct2.wc6", "rct2.wc7", "rct2.wc8", "rct2.wallcz32", "rct2.wallcy32", "rct2.tct2", "rct2.stg1", "rct2.stg2", "rct2.wc2", "rct2.wc9", "rct2.wc10", "rct2.wc11", "rct2.wc12", "rct2.wc13", "rct2.wallcw32", "rct2.wallcx32", "rct2.tcn", "rct2.ttg", "rct2.sct", "rct2.soh1", "rct2.soh2", "rct2.soh3", "rct2.wpw3", "rct2.wallcfpc", "rct2.wallcbpc" },
            // RCT1_SCENERY_THEME_SPACE
            { "rct2.ssr", "rct2.sst", "rct2.ssh", "rct2.tscp", "rct2.tsph", "rct2.tsc2", "rct2.tsp1", "rct2.tsp2", "rct2.wallsp32", "rct2.spcroof1", "rct2.bn9", "rct2.benchspc", "rct2.littersp" },
            // RCT1_SCENERY_THEME_CREEPY
            { "rct2.tcd", "rct2.tsg", "rct2.tsk", "rct2.tgh1", "rct2.tgh2", "rct2.tsmp", "rct2.sgp", "rct2.wc14", "rct2.wc15", "rct2.tl0", "rct2.tl1", "rct2.tl2", "rct2.tl3", "rct2.tm0", "rct2.tm1", "rct2.tm2", "rct2.tm3" },
            // RCT1_SCENERY_THEME_URBAN
            { "rct2.shs1", "rct2.shs2", "rct2.sth", "rct2.sah", "rct2.sps", "rct2.sah2", "rct2.sah3", "rct2.sob", "rct2.wallu132", "rct2.wallu232" },
            // RCT1_SCENERY_THEME_PAGODA
            { "rct2.spg", "rct2.tly", "rct2.tgg", "rct2.toh1", "rct2.toh2", "rct2.tot1", "rct2.tot2", "rct2.tos", "rct2.tot3", "rct2.tot4", "rct2.toh3", "rct2.wallpg32", "rct2.pagroof1", "rct2.bn7" }
        };
        return map[sceneryType];
    }
} // namespace RCT1

track_type_t RCT1TrackTypeToOpenRCT2(RCT12TrackType origTrackType, uint8_t rideType)
{
    if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
        return RCT12FlatTrackTypeToOpenRCT2(origTrackType);

    return origTrackType;
}
// clang-format on
