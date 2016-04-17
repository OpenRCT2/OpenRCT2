#include "../common.h"
#include "../core/List.hpp"
#include "import.h"

extern "C"
{
    #include "../interface/colour.h"
    #include "../rct1.h"
    #include "../ride/ride.h"
}

namespace RCT1
{
    const char * DefaultParkEntranceObject = "PKENT1  ";

    colour_t GetColour(colour_t colour)
    {
        static const uint8 map[] =
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
        return map[colour];
    }

    uint8 GetTerrain(uint8 terrain)
    {
        static const uint8 map[] =
        {
            TERRAIN_GRASS,
            TERRAIN_SAND,
            TERRAIN_DIRT,
            TERRAIN_ROCK,
            TERRAIN_MARTIAN,
            TERRAIN_CHECKERBOARD,
            TERRAIN_GRASS_CLUMPS,
            TERRAIN_DIRT,                 // Originally TERRAIN_ROOF_BROWN
            TERRAIN_ICE,
            TERRAIN_DIRT,                 // Originally TERRAIN_ROOF_LOG
            TERRAIN_DIRT,                 // Originally TERRAIN_ROOF_IRON
            TERRAIN_ROCK,                 // Originally TERRAIN_ROOF_GREY
            TERRAIN_GRID_RED,
            TERRAIN_GRID_YELLOW,
            TERRAIN_GRID_BLUE,
            TERRAIN_GRID_GREEN
        };
        return map[terrain];
    }

    uint8 GetTerrainEdge(uint8 terrainEdge)
    {
        static const uint8 map[] =
        {
            TERRAIN_EDGE_ROCK,
            TERRAIN_EDGE_ROCK,            // Originally TERRAIN_EDGE_BRICK
            TERRAIN_EDGE_ROCK,            // Originally TERRAIN_EDGE_IRON
            TERRAIN_EDGE_WOOD_RED,
            TERRAIN_EDGE_ROCK,            // Originally TERRAIN_EDGE_GREY
            TERRAIN_EDGE_ROCK,            // Originally TERRAIN_EDGE_YELLOW
            TERRAIN_EDGE_WOOD_BLACK,
            TERRAIN_EDGE_ROCK,            // Originally TERRAIN_EDGE_RED
            TERRAIN_EDGE_ICE,
            TERRAIN_EDGE_ROCK,            // Originally TERRAIN_EDGE_PURPLE
            TERRAIN_EDGE_ROCK,            // Originally TERRAIN_EDGE_GREEN
            TERRAIN_EDGE_ROCK,            // Originally TERRAIN_EDGE_STONE_BROWN
            TERRAIN_EDGE_ROCK,            // Originally TERRAIN_EDGE_STONE_GREY
            TERRAIN_EDGE_ROCK,            // Originally TERRAIN_EDGE_SKYSCRAPER_A
            TERRAIN_EDGE_ROCK,            // Originally TERRAIN_EDGE_SKYSCRAPER_B
            TERRAIN_EDGE_ROCK             // Unused
        };
        return map[terrainEdge];
    }

    uint8 GetRideType(uint8 rideType)
    {
        static uint8 map[] =
        {
            RIDE_TYPE_WOODEN_ROLLER_COASTER,
            RIDE_TYPE_STAND_UP_ROLLER_COASTER,
            RIDE_TYPE_SUSPENDED_SWINGING_COASTER,
            RIDE_TYPE_INVERTED_ROLLER_COASTER,
            RIDE_TYPE_JUNIOR_ROLLER_COASTER,
            RIDE_TYPE_MINIATURE_RAILWAY,
            RIDE_TYPE_MONORAIL,
            RIDE_TYPE_MINI_SUSPENDED_COASTER,
            RIDE_TYPE_BOAT_RIDE,
            RIDE_TYPE_WOODEN_WILD_MOUSE,
            RIDE_TYPE_STEEPLECHASE,
            RIDE_TYPE_CAR_RIDE,
            RIDE_TYPE_LAUNCHED_FREEFALL,
            RIDE_TYPE_BOBSLEIGH_COASTER,
            RIDE_TYPE_OBSERVATION_TOWER,
            RIDE_TYPE_LOOPING_ROLLER_COASTER,
            RIDE_TYPE_DINGHY_SLIDE,
            RIDE_TYPE_MINE_TRAIN_COASTER,
            RIDE_TYPE_CHAIRLIFT,
            RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
            RIDE_TYPE_MAZE,
            RIDE_TYPE_SPIRAL_SLIDE,
            RIDE_TYPE_GO_KARTS,
            RIDE_TYPE_LOG_FLUME,
            RIDE_TYPE_RIVER_RAPIDS,
            RIDE_TYPE_DODGEMS,
            RIDE_TYPE_PIRATE_SHIP,
            RIDE_TYPE_SWINGING_INVERTER_SHIP,
            RIDE_TYPE_FOOD_STALL,
            RIDE_TYPE_FOOD_STALL,
            RIDE_TYPE_DRINK_STALL,
            RIDE_TYPE_FOOD_STALL,
            RIDE_TYPE_SHOP,
            RIDE_TYPE_MERRY_GO_ROUND,
            RIDE_TYPE_SHOP,
            RIDE_TYPE_INFORMATION_KIOSK,
            RIDE_TYPE_TOILETS,
            RIDE_TYPE_FERRIS_WHEEL,
            RIDE_TYPE_MOTION_SIMULATOR,
            RIDE_TYPE_3D_CINEMA,
            RIDE_TYPE_TOP_SPIN,
            RIDE_TYPE_SPACE_RINGS,
            RIDE_TYPE_REVERSE_FREEFALL_COASTER,
            RIDE_TYPE_SHOP,
            RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER,
            RIDE_TYPE_FOOD_STALL,
            RIDE_TYPE_TWIST,
            RIDE_TYPE_HAUNTED_HOUSE,
            RIDE_TYPE_FOOD_STALL,
            RIDE_TYPE_CIRCUS_SHOW,
            RIDE_TYPE_GHOST_TRAIN,
            RIDE_TYPE_TWISTER_ROLLER_COASTER,
            RIDE_TYPE_WOODEN_ROLLER_COASTER,
            RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER,
            RIDE_TYPE_WILD_MOUSE,
            RIDE_TYPE_FOOD_STALL,
            RIDE_TYPE_FOOD_STALL,
            RIDE_TYPE_SHOP,
            RIDE_TYPE_FOOD_STALL,
            RIDE_TYPE_VIRGINIA_REEL,
            RIDE_TYPE_SPLASH_BOATS,
            RIDE_TYPE_MINI_HELICOPTERS,
            RIDE_TYPE_LAY_DOWN_ROLLER_COASTER,
            RIDE_TYPE_SUSPENDED_MONORAIL,
            RIDE_TYPE_NULL,
            RIDE_TYPE_REVERSER_ROLLER_COASTER,
            RIDE_TYPE_HEARTLINE_TWISTER_COASTER,
            RIDE_TYPE_MINI_GOLF,
            RIDE_TYPE_NULL,
            RIDE_TYPE_ROTO_DROP,
            RIDE_TYPE_FLYING_SAUCERS,
            RIDE_TYPE_CROOKED_HOUSE,
            RIDE_TYPE_MONORAIL_CYCLES,
            RIDE_TYPE_COMPACT_INVERTED_COASTER,
            RIDE_TYPE_WATER_COASTER,
            RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER,
            RIDE_TYPE_INVERTED_HAIRPIN_COASTER,
            RIDE_TYPE_BOAT_RIDE,
            RIDE_TYPE_SHOP,
            RIDE_TYPE_RIVER_RAFTS,
            RIDE_TYPE_FOOD_STALL,
            RIDE_TYPE_ENTERPRISE,
            RIDE_TYPE_DRINK_STALL,
            RIDE_TYPE_FOOD_STALL,
            RIDE_TYPE_DRINK_STALL
        };
        return map[rideType];
    }

    bool RideTypeHasVehicle(uint8 rideType)
    {
        switch (rideType) {
        case RCT1_RIDE_TYPE_HEDGE_MAZE:
        case RCT1_RIDE_TYPE_SPIRAL_SLIDE:
        case RCT1_RIDE_TYPE_ICE_CREAM_STALL:
        case RCT1_RIDE_TYPE_FRIES_STALL:
        case RCT1_RIDE_TYPE_DRINK_STALL:
        case RCT1_RIDE_TYPE_COTTON_CANDY_STALL:
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
        case RCT1_RIDE_TYPE_CANDY_APPLE_STAND:
        case RCT1_RIDE_TYPE_40:
        case RCT1_RIDE_TYPE_44:
        case RCT1_RIDE_TYPE_T_SHIRT_STALL:
        case RCT1_RIDE_TYPE_DOUGHNUT_SHOP:
        case RCT1_RIDE_TYPE_COFFEE_SHOP:
        case RCT1_RIDE_TYPE_FRIED_CHICKEN_STALL:
        case RCT1_RIDE_TYPE_LEMONADE_STALL:
            return true;
        default:
            return false;
        }
    }

    const char * GetRideTypeObject(uint8 rideType)
    {
        static const char * map[] =
        {
            "PTCT1   ",  // RCT1_RIDE_TYPE_WOODEN_ROLLER_COASTER
            "TOGST   ",  // RCT1_RIDE_TYPE_STAND_UP_STEEL_ROLLER_COASTER
            "ARRSW1  ",  // RCT1_RIDE_TYPE_SUSPENDED_ROLLER_COASTER
            "NEMT    ",  // RCT1_RIDE_TYPE_INVERTED_ROLLER_COASTER
            "ZLDB    ",  // RCT1_RIDE_TYPE_STEEL_MINI_ROLLER_COASTER
            "NRL     ",  // RCT1_RIDE_TYPE_MINIATURE_RAILROAD
            "MONO2   ",  // RCT1_RIDE_TYPE_MONORAIL
            "BATFL   ",  // RCT1_RIDE_TYPE_SUSPENDED_SINGLE_RAIL_ROLLER_COASTER
            "RBOAT   ",  // RCT1_RIDE_TYPE_BOAT_HIRE
            "WMOUSE  ",  // RCT1_RIDE_TYPE_WOODEN_CRAZY_RODENT_ROLLER_COASTER
            "STEEP1  ",  // RCT1_RIDE_TYPE_SINGLE_RAIL_ROLLER_COASTER
            "SPCAR   ",  // RCT1_RIDE_TYPE_CAR_RIDE
            "SSC1    ",  // RCT1_RIDE_TYPE_LAUNCHED_FREEFALL
            "BOB1    ",  // RCT1_RIDE_TYPE_BOBSLED_ROLLER_COASTER
            "OBS1    ",  // RCT1_RIDE_TYPE_OBSERVATION_TOWER
            "SCHT1   ",  // RCT1_RIDE_TYPE_STEEL_ROLLER_COASTER
            "DING1   ",  // RCT1_RIDE_TYPE_WATER_SLIDE
            "AMT1    ",  // RCT1_RIDE_TYPE_MINE_TRAIN_ROLLER_COASTER
            "CLIFT1  ",  // RCT1_RIDE_TYPE_CHAIRLIFT
            "ARRT1   ",  // RCT1_RIDE_TYPE_STEEL_CORKSCREW_ROLLER_COASTER
            "HMAZE   ",  // RCT1_RIDE_TYPE_HEDGE_MAZE
            "HSKELT  ",  // RCT1_RIDE_TYPE_SPIRAL_SLIDE
            "KART1   ",  // RCT1_RIDE_TYPE_GO_KARTS
            "LFB1    ",  // RCT1_RIDE_TYPE_LOG_FLUME
            "RAPBOAT ",  // RCT1_RIDE_TYPE_RIVER_RAPIDS
            "DODG1   ",  // RCT1_RIDE_TYPE_DODGEMS
            "SWSH1   ",  // RCT1_RIDE_TYPE_SWINGING_SHIP
            "SWSH2   ",  // RCT1_RIDE_TYPE_SWINGING_INVERTER_SHIP
            "ICECR1  ",  // RCT1_RIDE_TYPE_ICE_CREAM_STALL
            "CHPSH   ",  // RCT1_RIDE_TYPE_FRIES_STALL
            "DRNKS   ",  // RCT1_RIDE_TYPE_DRINK_STALL
            "CNDYF   ",  // RCT1_RIDE_TYPE_COTTON_CANDY_STALL
            "BURGB   ",  // RCT1_RIDE_TYPE_BURGER_BAR
            "MGR1    ",  // RCT1_RIDE_TYPE_MERRY_GO_ROUND
            "BALLN   ",  // RCT1_RIDE_TYPE_BALLOON_STALL
            "INFOK   ",  // RCT1_RIDE_TYPE_INFORMATION_KIOSK
            "TLT1    ",  // RCT1_RIDE_TYPE_TOILETS
            "FWH1    ",  // RCT1_RIDE_TYPE_FERRIS_WHEEL
            "SIMPOD  ",  // RCT1_RIDE_TYPE_MOTION_SIMULATOR
            "C3D     ",  // RCT1_RIDE_TYPE_3D_CINEMA
            "TOPSP1  ",  // RCT1_RIDE_TYPE_GRAVITRON
            "SRINGS  ",  // RCT1_RIDE_TYPE_SPACE_RINGS
            "REVF1   ",  // RCT1_RIDE_TYPE_REVERSE_WHOA_BELLY_ROLLER_COASTER
            "SOUVS   ",  // RCT1_RIDE_TYPE_SOUVENIR_STALL
            "BMVD    ",  // RCT1_RIDE_TYPE_VERTICAL_ROLLER_COASTER
            "PIZZS   ",  // RCT1_RIDE_TYPE_PIZZA_STALL
            "TWIST1  ",  // RCT1_RIDE_TYPE_SCRAMBLED_EGGS
            "HHBUILD ",  // RCT1_RIDE_TYPE_HAUNTED_HOUSE
            "POPCS   ",  // RCT1_RIDE_TYPE_POPCORN_STALL
            "CIRCUS1 ",  // RCT1_RIDE_TYPE_CIRCUS_SHOW
            "GTC     ",  // RCT1_RIDE_TYPE_GHOST_TRAIN
            "BMSD    ",  // RCT1_RIDE_TYPE_STEEL_TWISTER_ROLLER_COASTER
            "MFT     ",  // RCT1_RIDE_TYPE_WOODEN_TWISTER_ROLLER_COASTER
            "SFRIC1  ",  // RCT1_RIDE_TYPE_WOODEN_SIDE_FRICTION_ROLLER_COASTER
            "SMC1    ",  // RCT1_RIDE_TYPE_STEEL_WILD_MOUSE_ROLLER_COASTER
            "HOTDS   ",  // RCT1_RIDE_TYPE_HOT_DOG_STALL
            "SQDST   ",  // RCT1_RIDE_TYPE_EXOTIC_SEA_FOOD_STALL
            "HATST   ",  // RCT1_RIDE_TYPE_HAT_STALL
            "TOFFS   ",  // RCT1_RIDE_TYPE_CANDY_APPLE_STAND
            "VREEL   ",  // RCT1_RIDE_TYPE_VIRGINIA_REEL
            "SPBOAT  ",  // RCT1_RIDE_TYPE_RIVER_RIDE
            "MONBK   ",  // RCT1_RIDE_TYPE_CYCLE_MONORAIL
            "BMAIR   ",  // RCT1_RIDE_TYPE_FLYING_ROLLER_COASTER
            "SMONO   ",  // RCT1_RIDE_TYPE_SUSPENDED_MONORAIL
            "        ",  // RCT1_RIDE_TYPE_40
            "REVCAR  ",  // RCT1_RIDE_TYPE_WOODEN_REVERSER_ROLLER_COASTER
            "UTCAR   ",  // RCT1_RIDE_TYPE_HEARTLINE_TWISTER_ROLLER_COASTER
            "GOLF1   ",  // RCT1_RIDE_TYPE_MINIATURE_GOLF
            "        ",  // RCT1_RIDE_TYPE_44
            "GDROP1  ",  // RCT1_RIDE_TYPE_ROTO_DROP
            "FSAUC   ",  // RCT1_RIDE_TYPE_FLYING_SAUCERS
            "CHBUILD ",  // RCT1_RIDE_TYPE_CROOKED_HOUSE
            "HELICAR ",  // RCT1_RIDE_TYPE_CYCLE_RAILWAY
            "SLCT    ",  // RCT1_RIDE_TYPE_SUSPENDED_LOOPING_ROLLER_COASTER
            "CSTBOAT ",  // RCT1_RIDE_TYPE_WATER_COASTER
            "THCAR   ",  // RCT1_RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
            "IVMC1   ",  // RCT1_RIDE_TYPE_INVERTED_WILD_MOUSE_COASTER
            "JSKI    ",  // RCT1_RIDE_TYPE_JET_SKIS
            "TSHRT   ",  // RCT1_RIDE_TYPE_T_SHIRT_STALL
            "RFTBOAT ",  // RCT1_RIDE_TYPE_RAFT_RIDE
            "DOUGH   ",  // RCT1_RIDE_TYPE_DOUGHNUT_SHOP
            "ENTERP  ",  // RCT1_RIDE_TYPE_ENTERPRISE
            "COFFS   ",  // RCT1_RIDE_TYPE_COFFEE_SHOP
            "CHCKS   ",  // RCT1_RIDE_TYPE_FRIED_CHICKEN_STALL
            "LEMST   ",  // RCT1_RIDE_TYPE_LEMONADE_STALL
        };
        return map[rideType];
    }

    const char * GetVehicleObject(uint8 vehicleType)
    {
        static const char * map[] =
        {
            "SCHT1   ",  // RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN
            "SCHT1   ",  // RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN_BACKWARDS
            "PTCT1   ",  // RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN
            "SLCT    ",  // RCT1_VEHICLE_TYPE_INVERTED_COASTER_TRAIN (Not in RCT2)
            "ARRSW1  ",  // RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_CARS
            "ZLDB    ",  // RCT1_VEHICLE_TYPE_LADYBIRD_CARS
            "TOGST   ",  // RCT1_VEHICLE_TYPE_STANDUP_ROLLER_COASTER_CARS
            "WMSPIN  ",  // RCT1_VEHICLE_TYPE_SPINNING_CARS
            "BATFL   ",  // RCT1_VEHICLE_TYPE_SINGLE_PERSON_SWINGING_CHAIRS
            "SWANS   ",  // RCT1_VEHICLE_TYPE_SWANS_PEDAL_BOATS
            "MONO1   ",  // RCT1_VEHICLE_TYPE_LARGE_MONORAIL_TRAIN
            "CBOAT   ",  // RCT1_VEHICLE_TYPE_CANOES
            "RBOAT   ",  // RCT1_VEHICLE_TYPE_ROWING_BOATS
            "NRL     ",  // RCT1_VEHICLE_TYPE_STEAM_TRAIN
            "WMOUSE  ",  // RCT1_VEHICLE_TYPE_WOODEN_MOUSE_CARS
            "BBOAT   ",  // RCT1_VEHICLE_TYPE_BUMPER_BOATS
            "PTCT1   ",  // RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN_BACKWARDS
            "RCKC    ",  // RCT1_VEHICLE_TYPE_ROCKET_CARS
            "STEEP1  ",  // RCT1_VEHICLE_TYPE_HORSES // Steeplechase
            "SPCAR   ",  // RCT1_VEHICLE_TYPE_SPORTSCARS
            "SKYTR   ",  // RCT1_VEHICLE_TYPE_LYING_DOWN_SWINGING_CARS (Inverted single-rail)
            "WMMINE  ",  // RCT1_VEHICLE_TYPE_WOODEN_MINE_CARS
            "ARRSW2  ",  // RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_AIRPLANE_CARS
            "MONO2   ",  // RCT1_VEHICLE_TYPE_SMALL_MONORAIL_CARS
            "TRIKE   ",  // RCT1_VEHICLE_TYPE_WATER_TRICYCLES
            "SSC1    ",  // RCT1_VEHICLE_TYPE_LAUNCHED_FREEFALL_CAR
            "BOB1    ",  // RCT1_VEHICLE_TYPE_BOBSLEIGH_CARS
            "DING1   ",  // RCT1_VEHICLE_TYPE_DINGHIES
            "OBS1    ",  // RCT1_VEHICLE_TYPE_ROTATING_CABIN
            "AMT1    ",  // RCT1_VEHICLE_TYPE_MINE_TRAIN
            "CLIFT1  ",  // RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS
            "ARRT1   ",  // RCT1_VEHICLE_TYPE_CORKSCREW_ROLLER_COASTER_TRAIN
            "STEEP2  ",  // RCT1_VEHICLE_TYPE_MOTORBIKES
            "RCR     ",  // RCT1_VEHICLE_TYPE_RACING_CARS
            "TRUCK1  ",  // RCT1_VEHICLE_TYPE_TRUCKS
            "KART1   ",  // RCT1_VEHICLE_TYPE_GO_KARTS
            "RAPBOAT ",  // RCT1_VEHICLE_TYPE_RAPIDS_BOATS
            "LFB1    ",  // RCT1_VEHICLE_TYPE_LOG_FLUME_BOATS
            "DODG1   ",  // RCT1_VEHICLE_TYPE_DODGEMS
            "SWSH1   ",  // RCT1_VEHICLE_TYPE_SWINGING_SHIP
            "SWSH2   ",  // RCT1_VEHICLE_TYPE_SWINGING_INVERTER_SHIP
            "MGR1    ",  // RCT1_VEHICLE_TYPE_MERRY_GO_ROUND
            "FWH1    ",  // RCT1_VEHICLE_TYPE_FERRIS_WHEEL
            "SIMPOD  ",  // RCT1_VEHICLE_TYPE_SIMULATOR_POD
            "C3D     ",  // RCT1_VEHICLE_TYPE_CINEMA_BUILDING
            "TOPSP1  ",  // RCT1_VEHICLE_TYPE_TOPSPIN_CAR
            "SRINGS  ",  // RCT1_VEHICLE_TYPE_SPACE_RINGS
            "REVF1   ",  // RCT1_VEHICLE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER_CAR
            "BMVD    ",  // RCT1_VEHICLE_TYPE_VERTICAL_ROLLER_COASTER_CARS
            "CTCAR   ",  // RCT1_VEHICLE_TYPE_CAT_CARS
            "TWIST1  ",  // RCT1_VEHICLE_TYPE_TWIST_ARMS_AND_CARS
            "HHBUILD ",  // RCT1_VEHICLE_TYPE_HAUNTED_HOUSE_BUILDING
            "ZLOG    ",  // RCT1_VEHICLE_TYPE_LOG_CARS
            "CIRCUS1 ",  // RCT1_VEHICLE_TYPE_CIRCUS_TENT
            "GTC     ",  // RCT1_VEHICLE_TYPE_GHOST_TRAIN_CARS
            "BMSD    ",  // RCT1_VEHICLE_TYPE_STEEL_TWISTER_ROLLER_COASTER_TRAIN
            "MFT     ",  // RCT1_VEHICLE_TYPE_WOODEN_TWISTER_ROLLER_COASTER_TRAIN
            "SFRIC1  ",  // RCT1_VEHICLE_TYPE_WOODEN_SIDE_FRICTION_CARS
            "VCR     ",  // RCT1_VEHICLE_TYPE_VINTAGE_CARS
            "NRL2    ",  // RCT1_VEHICLE_TYPE_STEAM_TRAIN_COVERED_CARS
            "BMSU    ",  // RCT1_VEHICLE_TYPE_STAND_UP_STEEL_TWISTER_ROLLER_COASTER_TRAIN
            "BMFL    ",  // RCT1_VEHICLE_TYPE_FLOORLESS_STEEL_TWISTER_ROLLER_COASTER_TRAIN
            "SMC1    ",  // RCT1_VEHICLE_TYPE_STEEL_MOUSE_CARS
            "CLIFT2  ",  // RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS_ALTERNATIVE
            "SMONO   ",  // RCT1_VEHICLE_TYPE_SUSPENDED_MONORAIL_TRAIN
            "HELICAR ",  // RCT1_VEHICLE_TYPE_HELICOPTER_CARS
            "VREEL   ",  // RCT1_VEHICLE_TYPE_VIRGINIA_REEL_TUBS
            "REVCAR  ",  // RCT1_VEHICLE_TYPE_REVERSER_CARS
            "GOLF1   ",  // RCT1_VEHICLE_TYPE_GOLFERS
            "SPBOAT  ",  // RCT1_VEHICLE_TYPE_RIVER_RIDE_BOATS
            "BMAIR   ",  // RCT1_VEHICLE_TYPE_FLYING_ROLLER_COASTER_TRAIN
            "BMRB    ",  // RCT1_VEHICLE_TYPE_NON_LOOPING_STEEL_TWISTER_ROLLER_COASTER_TRAIN
            "UTCAR   ",  // RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS
            "UTCARR  ",  // RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS_REVERSED
            "        ",  // RCT1_VEHICLE_TYPE_RESERVED
            "GDROP1  ",  // RCT1_VEHICLE_TYPE_ROTODROP_CAR
            "FSAUC   ",  // RCT1_VEHICLE_TYPE_FLYING_SAUCERS
            "CHBUILD ",  // RCT1_VEHICLE_TYPE_CROOKED_HOUSE_BUILDING
            "MONBK   ",  // RCT1_VEHICLE_TYPE_BICYCLES
            "ARRT2   ",  // RCT1_VEHICLE_TYPE_HYPERCOASTER_TRAIN
            "NEMT    ",  // RCT1_VEHICLE_TYPE_4_ACROSS_INVERTED_COASTER_TRAIN
            "CSTBOAT ",  // RCT1_VEHICLE_TYPE_WATER_COASTER_BOATS
            "SLCFO   ",  // RCT1_VEHICLE_TYPE_FACEOFF_CARS
            "JSKI    ",  // RCT1_VEHICLE_TYPE_JET_SKIS
            "RFTBOAT ",  // RCT1_VEHICLE_TYPE_RAFT_BOATS
            "AML1    ",  // RCT1_VEHICLE_TYPE_AMERICAN_STYLE_STEAM_TRAIN
            "THCAR   ",  // RCT1_VEHICLE_TYPE_AIR_POWERED_COASTER_TRAIN
            "IVMC1   ",  // RCT1_VEHICLE_TYPE_SUSPENDED_WILD_MOUSE_CARS (Inverted Hairpin in RCT2)
            "ENTERP  ",  // RCT1_VEHICLE_TYPE_ENTERPRISE_WHEEL
        };
        return map[vehicleType];
    }

    const char * GetSmallSceneryObject(uint8 smallSceneryType)
    {
        static const char * map[] =
        {
            "TL0     ",
            "TL1     ",
            "TL2     ",
            "TL3     ",
            "TM0     ",
            "TM1     ",
            "TM2     ",
            "TM3     ",
            "TS0     ",
            "TS1     ",
            "TS2     ",
            "TS3     ",
            "TS4     ",
            "TS5     ",
            "TS6     ",
            "TIC     ",
            "TLC     ",
            "TMC     ",
            "TMP     ",
            "TITC    ",
            "TGHC    ",
            "TAC     ",
            "TGHC2   ",
            "TCJ     ",
            "TMBJ    ",
            "TCF     ",
            "TCL     ",
            "TRF     ",
            "TRF2    ",
            "TEL     ",
            "TAP     ",
            "TSP     ",
            "TMZP    ",
            "TCRP    ",
            "TBP     ",
            "TLP     ",
            "TWP     ",
            "TAS     ",
            "TMG     ",
            "TWW     ",
            "TSB     ",
            "TVL     ",
            "TCT     ",
            "TEF     ",
            "TAL     ",
            "TSQ     ",
            "THT     ",
            "TCB     ",
            "TDM     ",
            "TSD     ",
            "TGS     ",
            "TUS     ",
            "TH1     ",
            "TBC     ",
            "TH2     ",
            "TPM     ",
            "TSC     ",
            "TG1     ",
            "TWF     ",
            "TSH0    ",
            "TSH1    ",
            "TSH2    ",
            "TSH3    ",
            "TSH4    ",
            "TSH5    ",
            "TG2     ",
            "TG3     ",
            "TG4     ",
            "TG5     ",
            "TG6     ",
            "TG7     ",
            "TG8     ",
            "TG9     ",
            "TG10    ",
            "TG11    ",
            "TG12    ",
            "TG13    ",
            "TG14    ",
            "TT1     ",
            "TDF     ",
            "TSH     ",
            "THRS    ",
            "TSTD    ",
            "TRMS    ",
            "TRWS    ",
            "TRC     ",
            "TQF     ",
            "TES1    ",
            "TEN     ",
            "TERS    ",
            "TERB    ",
            "TEP     ",
            "TST1    ",
            "TST2    ",
            "TMS1    ",
            "TAS1    ",
            "TAS2    ",
            "TAS3    ",
            "TST3    ",
            "TST4    ",
            "TST5    ",
            "TAS4    ",
            "TCY     ",
            "TBW     ",
            "TBR1    ",
            "TBR2    ",
            "TML     ",
            "TMW     ",
            "TBR3    ",
            "TBR4    ",
            "TMJ     ",
            "TBR     ",
            "TMO1    ",
            "TMO2    ",
            "TMO3    ",
            "TMO4    ",
            "TMO5    ",
            "TWH1    ",
            "TWH2    ",
            "TNS     ",
            "TP1     ",
            "TP2     ",
            "TK1     ",
            "TK2     ",
            "TR1     ",
            "TR2     ",
            "TQ1     ",
            "TQ2     ",
            "TWN     ",
            "TCE     ",
            "TCO     ",
            "THL     ",
            "TCC     ",
            "TB1     ",
            "TB2     ",
            "TK3     ",
            "TK4     ",
            "TBN     ",
            "TBN1    ",
            "TDT1    ",
            "TDT2    ",
            "TDT3    ",
            "TMM1    ",
            "TMM2    ",
            "TMM3    ",
            "TGS1    ",
            "TGS2    ",
            "TGS3    ",
            "TGS4    ",
            "TDN4    ",
            "TDN5    ",
            "TJT1    ",
            "TJT2    ",
            "TJB1    ",
            "TTF     ",
            "TF1     ",
            "TF2     ",
            "TGE1    ",
            "TJT3    ",
            "TJT4    ",
            "TJP1    ",
            "TJB2    ",
            "TGE2    ",
            "TJT5    ",
            "TJB3    ",
            "TJB4    ",
            "TJT6    ",
            "TJP2    ",
            "TGE3    ",
            "TCK     ",
            "TGE4    ",
            "TGE5    ",
            "TG15    ",
            "TG16    ",
            "TG17    ",
            "TG18    ",
            "TG19    ",
            "TG20    ",
            "TG21    ",
            "TSM     ",
            "TIG     ",
            "TCFS    ",
            "TRFS    ",
            "TRF3    ",
            "TNSS    ",
            "TCT1    ",
            "TCT2    ",
            "TSF1    ",
            "TSF2    ",
            "TSF3    ",
            "TCN     ",
            "TTG     ",
            "TSNC    ",
            "TSNB    ",
            "TSCP    ",
            "TCD     ",
            "TSG     ",
            "TSK     ",
            "TGH1    ",
            "TGH2    ",
            "TSMP    ",
            "TJF     ",
            "TLY     ",
            "TGC1    ",
            "TGC2    ",
            "TGG     ",
            "TSPH    ",
            "TOH1    ",
            "TOH2    ",
            "TOT1    ",
            "TOT2    ",
            "TOS     ",
            "TOT3    ",
            "TOT4    ",
            "TSC2    ",
            "TSP1    ",
            "TOH3    ",
            "TSP2    ",
            "ROMROOF1",
            "GEOROOF1",
            "TNTROOF1",
            "JNGROOF1",
            "MINROOF1",
            "ROMROOF2",
            "GEOROOF2",
            "PAGROOF1",
            "SPCROOF1",
            "ROOF1   ",
            "ROOF2   ",
            "ROOF3   ",
            "ROOF4   ",
            "ROOF5   ",
            "ROOF6   ",
            "ROOF7   ",
            "ROOF8   ",
            "ROOF9   ",
            "ROOF10  ",
            "ROOF11  ",
            "ROOF12  ",
            "ROOF13  ",
            "ROOF14  ",
            "IGROOF  ",
            "CORROOF ",
            "CORROOF2",
        };
        return map[smallSceneryType];
    }

    const char * GetLargeSceneryObject(uint8 largeSceneryType)
    {
        static const char * map[] =
        {
            "SCOL    ",
            "SHS1    ",
            "SSPX    ",
            "SHS2    ",
            "SCLN    ",
            "SMH1    ",
            "SMH2    ",
            "SVLC    ",
            "SPYR    ",
            "SMN1    ",
            "SMB     ",
            "SSK1    ",
            "SDN1    ",
            "SDN2    ",
            "SDN3    ",
            "SIP     ",
            "STB1    ",
            "STB2    ",
            "STG1    ",
            "STG2    ",
            "SCT     ",
            "SOH1    ",
            "SOH2    ",
            "SOH3    ",
            "SGP     ",
            "SSR     ",
            "STH     ",
            "SAH     ",
            "SPS     ",
            "SPG     ",
            "SOB     ",
            "SAH2    ",
            "SST     ",
            "SSH     ",
            "SAH3    ",
            "SSIG1   ",
            "SSIG2   ",
            "SSIG3   ",
            "SSIG4   ",
        };
        return map[largeSceneryType];
    }

    const char * GetWallObject(uint8 wallType)
    {
        static const char * map[] =
        {
            "WMF     ",
            "WMFG    ",
            "WRW     ",
            "WEW     ",
            "WHG     ",
            "WHGG    ",
            "WCW1    ",
            "WCW2    ",
            "WSW     ",
            "WSWG    ",
            "WMW     ",
            "WALLGL16",
            "WFW1    ",
            "WFWG    ",
            "WPW1    ",
            "WPW2    ",
            "WPF     ",
            "WPFG    ",
            "WWTW    ",
            "WMWW    ",
            "WSW1    ",
            "WSW2    ",
            "WGW2    ",
            "WBW     ",
            "WBR1    ",
            "WBRG    ",
            "WALLCFAR",
            "WALLPOST",
            "WBR2    ",
            "WBR3    ",
            "WPW3    ",
            "WJF     ",
            "WCH     ",
            "WCHG    ",
            "WC1     ",
            "WC2     ",
            "WC3     ",
            "WC4     ",
            "WC5     ",
            "WC6     ",
            "WC7     ",
            "WC8     ",
            "WC9     ",
            "WC10    ",
            "WC11    ",
            "WC12    ",
            "WC13    ",
            "WC14    ",
            "WC15    ",
            "WC16    ",
            "WC17    ",
            "WC18    ",
            "WALLBRDR",
            "WALLBR32",
            "WALLBR16",
            "WALLBR8 ",
            "WALLCF8 ",
            "WALLCF16",
            "WALLCF32",
            "WALLBB8 ",
            "WALLBB16",
            "WALLBB32",
            "WALLRS8 ",
            "WALLRS16",
            "WALLRS32",
            "WALLCB8 ",
            "WALLCB16",
            "WALLCB32",
            "WALLGL8 ",
            "WALLGL32",
            "WALLWD8 ",
            "WALLWD16",
            "WALLWD32",
            "WALLTN32",
            "WALLJN32",
            "WALLMN32",
            "WALLSP32",
            "WALLPG32",
            "WALLU132",
            "WALLU232",
            "WALLCZ32",
            "WALLCW32",
            "WALLCY32",
            "WALLCX32",
            "WBR1A   ",
            "WBR2A   ",
            "WRWA    ",
            "WWTWA   ",
            "WALLIG16",
            "WALLIG24",
            "WALLCO16",
            "WALLCFDR",
            "WALLCBDR",
            "WALLBRWN",
            "WALLCFWN",
            "WALLCBWN",
        };
        return map[wallType];
    }

    const char * GetSceneryGroupObject(uint8 sceneryGroupType)
    {
        static const char * map[] =
        {
            "        ", // RCT1_SCENERY_THEME_GENERAL
            "SCGMINE ", // RCT1_SCENERY_THEME_MINE
            "SCGCLASS", // RCT1_SCENERY_THEME_CLASSICAL_ROMAN
            "SCGEGYPT", // RCT1_SCENERY_THEME_EGYPTIAN
            "SCGMART ", // RCT1_SCENERY_THEME_MARTIAN
            "        ", // RCT1_SCENERY_THEME_JUMPING_FOUNTAINS
            "SCGWOND ", // RCT1_SCENERY_THEME_WONDERLAND
            "SCGJURAS", // RCT1_SCENERY_THEME_JURASSIC
            "SCGSPOOK", // RCT1_SCENERY_THEME_SPOOKY
            "SCGJUNGL", // RCT1_SCENERY_THEME_JUNGLE
            "        ", // RCT1_SCENERY_THEME_GARDEN_CLOCK
            "SCGABSTR", // RCT1_SCENERY_THEME_ABSTRACT
            "SCGSNOW ", // RCT1_SCENERY_THEME_SNOW_ICE
            "SCGMEDIE", // RCT1_SCENERY_THEME_MEDIEVAL
            "SCGSPACE", // RCT1_SCENERY_THEME_SPACE
            "SCGHALLO", // RCT1_SCENERY_THEME_CREEPY
            "SCGURBAN", // RCT1_SCENERY_THEME_URBAN
            "SCGORIEN", // RCT1_SCENERY_THEME_PAGODA
        };
        return map[sceneryGroupType];
    }

    const char * GetWaterObject(uint8 waterType)
    {
        static const char * map[] =
        {
            "WTRCYAN ",
            "WTRORNG ",
        };
        return map[waterType];
    }

    const List<const char *> GetPreferedRideEntryOrder(uint8 rideType)
    {
        static const List<const char *> preferedRideEntryOrder[] =
        {
            { "SPDRCR  "},                                                              // RIDE_TYPE_SPIRAL_ROLLER_COASTER
            { "TOGST   "},                                                              // RIDE_TYPE_STAND_UP_ROLLER_COASTER
            { "ARRSW1  ", "VEKVAMP ", "ARRSW2 "},                                       // RIDE_TYPE_SUSPENDED_SWINGING_COASTER
            { "NEMT    "},                                                              // RIDE_TYPE_INVERTED_ROLLER_COASTER
            { "ZLDB    ", "ZLOG    "},                                                  // RIDE_TYPE_JUNIOR_ROLLER_COASTER
            { "NRL     ", "NRL2    ", "AML1    ", "TRAM1   "},                          // RIDE_TYPE_MINIATURE_RAILWAY
            { "MONO1   ", "MONO2   ", "MONO3   "},                                      // RIDE_TYPE_MONORAIL
            { "BATFL   ", "SKYTR   "},                                                  // RIDE_TYPE_MINI_SUSPENDED_COASTER
            { "RBOAT   ", "BBOAT   ", "CBOAT   ", "SWANS   ", "TRIKE   ", "JSKI    " }, // RIDE_TYPE_BOAT_RIDE
            { "WMOUSE  ", "WMMINE  "},                                                  // RIDE_TYPE_WOODEN_WILD_MOUSE
            { "STEEP1  ", "STEEP2  ", "SBOX    "},                                      // RIDE_TYPE_STEEPLECHASE
            { "SPCAR   ", "RCR     ", "TRUCK1  ", "VCR     ", "CTCAR   " },             // RIDE_TYPE_CAR_RIDE
            { "SSC1    " },                                                             // RIDE_TYPE_LAUNCHED_FREEFALL
            { "BOB1    ", "INTBOB  " },                                                 // RIDE_TYPE_BOBSLEIGH_COASTER
            { "OBS1    ", "OBS2    " },                                                 // RIDE_TYPE_OBSERVATION_TOWER
            { "SCHT1   " },                                                             // RIDE_TYPE_LOOPING_ROLLER_COASTER
            { "DING1   " },                                                             // RIDE_TYPE_DINGHY_SLIDE
            { "AMT1    " },                                                             // RIDE_TYPE_MINE_TRAIN_COASTER
            { "CLIFT1  ", "CLIFT2  " },                                                 // RIDE_TYPE_CHAIRLIFT
            { "ARRT1   ", "ARRT2   " },                                                 // RIDE_TYPE_CORKSCREW_ROLLER_COASTER
            { },                                                                        // RIDE_TYPE_MAZE
            { },                                                                        // RIDE_TYPE_SPIRAL_SLIDE
            { "KART1   " },                                                             // RIDE_TYPE_GO_KARTS
            { "LFB1    " },                                                             // RIDE_TYPE_LOG_FLUME
            { "RAPBOAT " },                                                             // RIDE_TYPE_RIVER_RAPIDS
            { },                                                                        // RIDE_TYPE_DODGEMS
            { },                                                                        // RIDE_TYPE_PIRATE_SHIP
            { },                                                                        // RIDE_TYPE_SWINGING_INVERTER_SHIP
            { },                                                                        // RIDE_TYPE_FOOD_STALL
            { },                                                                        // RIDE_TYPE_1D
            { },                                                                        // RIDE_TYPE_DRINK_STALL
            { },                                                                        // RIDE_TYPE_1F
            { },                                                                        // RIDE_TYPE_SHOP
            { },                                                                        // RIDE_TYPE_MERRY_GO_ROUND
            { },                                                                        // RIDE_TYPE_22
            { },                                                                        // RIDE_TYPE_INFORMATION_KIOSK
            { },                                                                        // RIDE_TYPE_TOILETS
            { },                                                                        // RIDE_TYPE_FERRIS_WHEEL
            { },                                                                        // RIDE_TYPE_MOTION_SIMULATOR
            { },                                                                        // RIDE_TYPE_3D_CINEMA
            { },                                                                        // RIDE_TYPE_TOP_SPIN
            { },                                                                        // RIDE_TYPE_SPACE_RINGS
            { "REVF1   " },                                                             // RIDE_TYPE_REVERSE_FREEFALL_COASTER
            { "LIFT1   " },                                                             // RIDE_TYPE_LIFT
            { "BMVD    " },                                                             // RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
            { },                                                                        // RIDE_TYPE_CASH_MACHINE
            { },                                                                        // RIDE_TYPE_TWIST
            { },                                                                        // RIDE_TYPE_HAUNTED_HOUSE
            { },                                                                        // RIDE_TYPE_FIRST_AID
            { },                                                                        // RIDE_TYPE_CIRCUS_SHOW
            { "GTC     ", "HMCAR   " },                                                 // RIDE_TYPE_GHOST_TRAIN
            { "BMSD    ", "BMSU    ", "BMFL    ", "BMRB    ", "GOLTR   " },             // RIDE_TYPE_TWISTER_ROLLER_COASTER
            { "PTCT1   ", "MFT     ", "PTCT2   " },                                     // RIDE_TYPE_WOODEN_ROLLER_COASTER
            { "SFRIC1  " },                                                             // RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
            { "SMC1    ", "SMC2    ", "WMSPIN  " },                                     // RIDE_TYPE_WILD_MOUSE
            { "ARRX    " },                                                             // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
            { },                                                                        // RIDE_TYPE_38
            { "BMAIR   " },                                                             // RIDE_TYPE_FLYING_ROLLER_COASTER
            { },                                                                        // RIDE_TYPE_3A
            { "VREEL   " },                                                             // RIDE_TYPE_VIRGINIA_REEL
            { "SPBOAT  " },                                                             // RIDE_TYPE_SPLASH_BOATS
            { "HELICAR " },                                                             // RIDE_TYPE_MINI_HELICOPTERS
            { "VEKST   " },                                                             // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
            { "SMONO   " },                                                             // RIDE_TYPE_SUSPENDED_MONORAIL
            { },                                                                        // RIDE_TYPE_40
            { "REVCAR  " },                                                             // RIDE_TYPE_REVERSER_ROLLER_COASTER
            { "UTCAR   ", "UTCARR  " },                                                 // RIDE_TYPE_HEARTLINE_TWISTER_COASTER
            { },                                                                        // RIDE_TYPE_MINI_GOLF
            { "INTST   " },                                                             // RIDE_TYPE_GIGA_COASTER
            { "GDROP1  " },                                                             // RIDE_TYPE_ROTO_DROP
            { },                                                                        // RIDE_TYPE_FLYING_SAUCERS
            { },                                                                        // RIDE_TYPE_CROOKED_HOUSE
            { "MONBK   " },                                                             // RIDE_TYPE_MONORAIL_CYCLES
            { "SLCT    ", "SLCFO    ", "VEKDV   " },                                    // RIDE_TYPE_COMPACT_INVERTED_COASTER
            { "CSTBOAT " },                                                             // RIDE_TYPE_WATER_COASTER
            { "THCAR   " },                                                             // RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
            { "IVMC1   " },                                                             // RIDE_TYPE_INVERTED_HAIRPIN_COASTER
            { },                                                                        // RIDE_TYPE_MAGIC_CARPET
            { "SUBMAR  " },                                                             // RIDE_TYPE_SUBMARINE_RIDE
            { "RFTBOAT " },                                                             // RIDE_TYPE_RIVER_RAFTS
            { },                                                                        // RIDE_TYPE_50
            { },                                                                        // RIDE_TYPE_ENTERPRISE
            { },                                                                        // RIDE_TYPE_52
            { },                                                                        // RIDE_TYPE_53
            { },                                                                        // RIDE_TYPE_54
            { },                                                                        // RIDE_TYPE_55
            { "INTINV  " },                                                             // RIDE_TYPE_INVERTED_IMPULSE_COASTER
            { "WCATC   ", "RCKC     ", "JSTAR1  " },                                    // RIDE_TYPE_MINI_ROLLER_COASTER
            { "PMT1    " },                                                             // RIDE_TYPE_MINE_RIDE
            { "PREMT1  " },                                                             // RIDE_TYPE_59
        };                                                                              // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
        return preferedRideEntryOrder[rideType];
    }

    const List<const char *> GetSceneryObjects(uint8 sceneryType)
    {
        static const List<const char *> map[] =
        {
            // RCT1_SCENERY_THEME_GENERAL (trees, shrubs, garden, walls, fence, path accessories)
            { "TIC     ", "TLC     ", "TMC     ", "TMP     ", "TITC    ", "TGHC    ", "TAC     ", "TGHC2   ", "TCJ     ", "TMBJ    ", "TCF     ", "TCL     ", "TRF     ", "TRF2    ", "TEL     ", "TAP     ", "TSP     ", "TMZP    ", "TCRP    ", "TBP     ", "TLP     ", "TWP     ", "TAS     ", "TMG     ", "TWW     ", "TSB     ", "TVL     ", "TCY     ", "TNS     ", "TWN     ", "TCE     ", "TCO     ", "THL     ", "TCC     ", "TF1     ", "TF2     ", "TCT     ", "TH1     ", "TH2     ", "TPM     ", "TROPT1  ",
              "TS0     ", "TS1     ", "TS2     ", "TS3     ", "TS4     ", "TS5     ", "TS6     ", "TEF     ", "TAL     ", "TSQ     ", "THT     ", "TCB     ", "TDM     ", "TSD     ", "TORN1   ", "TORN2   ", "TGS     ", "TUS     ", "TBC     ", "TSC     ", "TWF     ", "TSH0    ", "TSH1    ", "TSH2    ", "TSH3    ", "TSH4    ", "TSH5    ", "TDF     ", "TSH     ", "THRS    ", "TSTD    ", "TBR     ", "TTF     ", "WHG     ", "WHGG    ", "WCH     ", "WCHG    ",
              "TG1     ", "TG2     ", "TG3     ", "TG4     ", "TG5     ", "TG6     ", "TG7     ", "TG8     ", "TG9     ", "TG10    ", "TG11    ", "TG12    ", "TG13    ", "TG14    ", "TG15    ", "TG16    ", "TG17    ", "TG18    ", "TG19    ", "TG20    ", "TG21    ",
              "WBR1A   ", "WBR2A   ", "WALLBB34", "WALLTN32", "TNTROOF1", "WALLBB33", "WALLBB32", "WALLBB16", "WALLBB8 ", "ROOF5   ", "ROOF7   ", "WALLRS32", "WALLRS16", "WALLRS8 ", "WALLBR32", "WALLBR16", "WALLBR8 ", "WALLBRDR", "WALLBRWN", "BRBASE  ", "ROOF1   ", "ROOF2   ", "ROOF3   ", "ROOF4   ", "WALLCB32", "WALLCB16", "WALLCB8 ", "WALLCBDR", "WALLCBWN", "BRBASE2 ", "CWBCRV33", "CWBCRV32", "BRCRRF1 ", "ROOF6   ", "ROOF8   ", "WALLCF32", "WALLCF16", "WALLCF8 ", "WALLCFDR", "WALLCFWN", "WALLCFAR", "BRBASE3 ", "CWFCRV33", "CWFCRV32", "BRCRRF2 ", "ROOF9   ", "ROOF11  ", "ROOF10  ", "ROOF12  ", "CORROOF2", "WALLCO16", "CORROOF ", "WALLLT32", "WALLSK16", "WALLSK32", "SKTDW2  ", "SKTDW   ", "SKTBASE ", "SKTBASET", "SUPPW2  ", "SUPPW1  ", "SUPPW3  ", "SUPPLEG1", "SUPPLEG2", "SUMRF   ", "WALLRH32"
              "WMF     ", "WMFG    ", "WSW     ", "WSWG    ", "WFW1    ", "WFWG    ", "WPF     ", "WPFG    ", "WSW1    ", "WSW2    ", "WBR1    ", "WBRG    ", "WBR2    ", "WBR3    ", "WALLMM16", "WALLMM17",
              "LAMP1   ", "LAMP2   ", "LITTER1 ", "BENCH1  ", "QTV1    ", "BN1     ", "WALLPOST", "WALLSIGN", "SSIG1   ", "SSIG2   ", "SSIG3   ", "SSIG4   " },
            // RCT1_SCENERY_THEME_MINE
            { "SMH1    ", "SMH2    ", "SMN1    ", "TBW     ", "TBR1    ", "TBR2    ", "TML     ", "TMW     ", "TBR3    ", "TBR4    ", "TMJ     ", "BN5     ", "WALLWD8 ", "WALLWD16", "WALLWD32", "WALLWD33", "WALLMN32", "WDBASE  ", "MINROOF1", "ROOF13  ", "LITTERMN" },
            // RCT1_SCENERY_THEME_CLASSICAL_ROMAN
            { "SCOL    ", "TT1     ", "TRMS    ", "TRWS    ", "TRC     ", "TQF     ", "WRW     ", "WRWA    ", "ROMROOF2", "WC3     ", "ROMROOF1", "BN3     " },
            // RCT1_SCENERY_THEME_EGYPTIAN
            { "SSPX    ", "SCLN    ", "SPYR    ", "TES1    ", "TEN     ", "TERS    ", "TERB    ", "TEP     ", "WEW     ", "LAMP3   ", "BN4     ", "BENCHSTN" },
            // RCT1_SCENERY_THEME_MARTIAN
            { "SMB     ", "TMO1    ", "TMO2    ", "TMO3    ", "TMO4    ", "TMO5    ", "SVLC    ", "WMW     ", "LAMP4   " },
            // RCT1_SCENERY_THEME_JUMPING_FOUNTAINS (Single researchable scenery item)
            { "JUMPFNT1" },
            // RCT1_SCENERY_THEME_WONDERLAND
            { "TWH1    ", "TWH2    ", "TST1    ", "TST2    ", "TMS1    ", "TST3    ", "TST4    ", "TST5    ", "TAS1    ", "TAS2    ", "TAS3    ", "TAS4    ", "CHBBASE ", "TP1     ", "TP2     ", "TK1     ", "TK2     ", "TR1     ", "TR2     ", "TQ1     ", "TQ2     ", "TB1     ", "TB2     ", "TK3     ", "TK4     ", "WCW1    ", "WCW2    " },
            // RCT1_SCENERY_THEME_JURASSIC
            { "TBN     ", "TBN1    ", "TDN4    ", "TDN5    ", "SDN1    ", "SDN2    ", "SDN3    ", "WWTW    ", "WMWW    ", "WWTWA   ", "WBW     ", "BN6     " },
            // RCT1_SCENERY_THEME_SPOOKY,
            { "SSK1    ", "TDT1    ", "TDT2    ", "TDT3    ", "TMM1    ", "TMM2    ", "TMM3    ", "TGS1    ", "TGS2    ", "TGS3    ", "TGS4    ", "SMSKULL ", "WALLRK32" },
            // RCT1_SCENERY_THEME_JUNGLE
            { "TJT1    ", "TJT2    ", "TJB1    ", "TJT3    ", "TJT4    ", "TJP1    ", "TJB2    ", "TJT5    ", "TJB3    ", "TJB4    ", "TJT6    ", "TJP2    ", "TJF     ", "WPW1    ", "WPW2    ", "WJF     ", "BN2     ", "WALLJN32", "JNGROOF1", "ROOF14  ", "BENCHLOG" },
            // RCT1_SCENERY_THEME_ABSTRACT
            { "TGE1    ", "TGE2    ", "TGE3    ", "TGE4    ", "TGE5    ", "TGC1    ", "TGC2    ", "WALLGL8 ", "WALLGL16", "WALLGL32", "GEOROOF1", "WGW2    ", "GEOROOF2" },
            // RCT1_SCENERY_THEME_GARDEN_CLOCK (Single researchable scenery item)
            { "TCK     " },
            // RCT1_SCENERY_THEME_SNOW_ICE
            { "SIP     ", "TSM     ", "TIG     ", "TSF1    ", "TSF2    ", "TSF3    ", "TSNC    ", "TSNB    ", "WC16    ", "WC17    ", "WC18    ", "JUMPSNW1", "TCFS    ", "TRFS    ", "TRF3    ", "TNSS    ", "BN8     ", "WALLIG16", "WALLIG24", "IGROOF  " },
            // RCT1_SCENERY_THEME_MEDIEVAL
            { "TCT1    ", "STB1    ", "STB2    ", "WC1     ", "WC4     ", "WC5     ", "WC6     ", "WC7     ", "WC8     ", "WALLCZ32", "WALLCY32", "TCT2    ", "STG1    ", "STG2    ", "WC2     ", "WC9     ", "WC10    ", "WC11    ", "WC12    ", "WC13    ", "WALLCW32", "WALLCX32", "TCN     ", "TTG     ", "SCT     ", "SOH1    ", "SOH2    ", "SOH3    ", "WPW3    ", "WALLCFPC", "WALLCBPC" },
            // RCT1_SCENERY_THEME_SPACE
            { "SSR     ", "SST     ", "SSH     ", "TSCP    ", "TSPH    ", "TSC2    ", "TSP1    ", "TSP2    ", "WALLSP32", "SPCROOF1", "BN9     ", "BENCHSPC", "LITTERSP" },
            // RCT1_SCENERY_THEME_CREEPY
            { "TCD     ", "TSG     ", "TSK     ", "TGH1    ", "TGH2    ", "TSMP    ", "SGP     ", "WC14    ", "WC15    ", "TL0     ", "TL1     ", "TL2     ", "TL3     ", "TM0     ", "TM1     ", "TM2     ", "TM3     " },
            // RCT1_SCENERY_THEME_URBAN
            { "SHS1    ", "SHS2    ", "STH     ", "SAH     ", "SPS     ", "SAH2    ", "SAH3    ", "SOB     ", "WALLU132", "WALLU232" },
            // RCT1_SCENERY_THEME_PAGODA
            { "SPG     ", "TLY     ", "TGG     ", "TOH1    ", "TOH2    ", "TOT1    ", "TOT2    ", "TOS     ", "TOT3    ", "TOT4    ", "TOH3    ", "WALLPG32", "PAGROOF1", "BN7     " }
        };
        return map[sceneryType];
    }
}
