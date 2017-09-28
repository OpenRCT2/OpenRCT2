#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <memory>
#include <vector>
#include "RideGroupManager.h"
#include "../config/Config.h"
#include "../core/String.hpp"

#include "../localisation/string_ids.h"
#include "../management/research.h"
#include "ride.h"
#include "ride_data.h"
#include "track.h"
#include "track_data.h"

class RideGroupManager final : public IRideGroupManager
{
public:
    ride_group ride_group_corkscrew_rc = {
        /*.ride_type =*/ RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
        /*.maximum_height =*/ 28,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_VERTICAL_LOOP) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HALF_LOOP) | (1ULL << TRACK_CORKSCREW) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_BOOSTER),
        /*.naming =*/ { STR_CORKSCREW_RC_GROUP, STR_CORKSCREW_RC_GROUP_DESC },
    };

    ride_group ride_group_hypercoaster = {
        /*.ride_type =*/ RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
        /*.maximum_height =*/ 45,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_STEEP_LONG),
        /*.naming =*/ { STR_HYPERCOASTER_GROUP, STR_HYPERCOASTER_GROUP_DESC },
    };

    ride_group ride_group_car_ride = {
        /*.ride_type =*/ RIDE_TYPE_CAR_RIDE,
        /*.maximum_height =*/ 6,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_SPINNING_TUNNEL),
        /*.naming =*/ { STR_CAR_RIDE_GROUP, STR_CAR_RIDE_GROUP_DESC },
    };

    ride_group ride_group_monster_trucks = {
        /*.ride_type =*/ RIDE_TYPE_CAR_RIDE,
        /*.maximum_height =*/ 18,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_RAPIDS),
        /*.naming =*/ { STR_MONSTER_TRUCKS_GROUP, STR_MONSTER_TRUCKS_GROUP_DESC },
    };

    ride_group ride_group_steel_twister_rc = {
        /*.ride_type =*/ RIDE_TYPE_TWISTER_ROLLER_COASTER,
        /*.maximum_height =*/ 34,
        /*.available_track_pieces =*/ (1ULL << TRACK_FLAT) | (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_VERTICAL_LOOP) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HALF_LOOP) | (1ULL << TRACK_CORKSCREW) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_SLOPE_VERTICAL) | (1ULL << TRACK_BARREL_ROLL) | (1ULL << TRACK_POWERED_LIFT) | (1ULL << TRACK_HALF_LOOP_LARGE) | (1ULL << TRACK_SLOPE_CURVE_BANKED) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_ROLL_BANKING) | (1ULL << TRACK_SLOPE_STEEP_LONG) | (1ULL << TRACK_CURVE_VERTICAL) | (1ULL << TRACK_QUARTER_LOOP) | (1ULL << TRACK_BOOSTER),
        /*.naming =*/ { STR_STEEL_TWISTER_GROUP, STR_STEEL_TWISTER_GROUP_DESC },
    };

    ride_group ride_group_hyper_twister = {
        /*.ride_type =*/ RIDE_TYPE_TWISTER_ROLLER_COASTER,
        /*.maximum_height =*/ 54,
        /*.available_track_pieces =*/ (1ULL << TRACK_FLAT) | (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_POWERED_LIFT) | (1ULL << TRACK_SLOPE_CURVE_BANKED) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_ROLL_BANKING) | (1ULL << TRACK_SLOPE_STEEP_LONG),
        /*.naming =*/ { STR_HYPER_TWISTER_GROUP, STR_HYPER_TWISTER_GROUP_DESC },
    };

    ride_group ride_group_junior_rc = {
        /*.ride_type =*/ RIDE_TYPE_JUNIOR_ROLLER_COASTER,
        /*.maximum_height =*/ 12,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_LIFT_HILL_CURVE) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_LONG) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_BOOSTER),
        /*.naming =*/ { STR_JUNIOR_RC_GROUP, STR_JUNIOR_RC_GROUP_DESC },
    };

    ride_group ride_group_midi_coaster = {
        /*.ride_type =*/ RIDE_TYPE_JUNIOR_ROLLER_COASTER,
        /*.maximum_height =*/ 15,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_LIFT_HILL_CURVE) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_LONG) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_BOOSTER),
        /*.naming =*/ { STR_MIDI_COASTER_GROUP, STR_MIDI_COASTER_GROUP_DESC },
    };

    ride_group corkscrew_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_corkscrew_rc, ride_group_hypercoaster };
    ride_group junior_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_junior_rc, ride_group_midi_coaster };
    ride_group car_ride_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_car_ride, ride_group_monster_trucks };
    ride_group twister_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_steel_twister_rc, ride_group_hyper_twister };

    const ride_group * GetRideGroup(uint8 rideType, rct_ride_entry * rideEntry) const override
    {
        switch (rideType) {
            case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
                if (rideEntry->enabledTrackPieces & (1ULL << TRACK_VERTICAL_LOOP))
                    return (ride_group *) &ride_group_corkscrew_rc;
                else
                    return (ride_group *) &ride_group_hypercoaster;
            case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
                if (ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP))
                    return (ride_group *) &ride_group_midi_coaster;
                else
                    return (ride_group *) &ride_group_junior_rc;
            case RIDE_TYPE_CAR_RIDE:
                if (rideEntry->enabledTrackPieces & (1ULL << TRACK_SLOPE_STEEP))
                    return (ride_group *) &ride_group_monster_trucks;
                else
                    return (ride_group *) &ride_group_car_ride;
            case RIDE_TYPE_TWISTER_ROLLER_COASTER:
                if (rideEntry->enabledTrackPieces & (1ULL << TRACK_VERTICAL_LOOP))
                    return (ride_group *) &ride_group_steel_twister_rc;
                else
                    return (ride_group *) &ride_group_hyper_twister;
            default:
                return nullptr;
        }
    }

    bool RideTypeHasRideGroups(uint8 rideType) const override
    {
        if (!gConfigInterface.select_by_track_type) {
            return false;
        }

        switch (rideType)
        {
            case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
            case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
            case RIDE_TYPE_CAR_RIDE:
            case RIDE_TYPE_TWISTER_ROLLER_COASTER:
                return true;
            default:
                return false;
        }
    }

    ride_group * RideGroupFind(uint8 rideType, uint8 index) const override
    {
        ride_group * rideGroup;

        switch(rideType)
        {
            case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
                rideGroup = (ride_group *) &corkscrew_rc_groups[index];
                break;
            case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
                rideGroup = (ride_group *) &junior_rc_groups[index];
                break;
            case RIDE_TYPE_CAR_RIDE:
                rideGroup = (ride_group *) &car_ride_groups[index];
                break;
            case RIDE_TYPE_TWISTER_ROLLER_COASTER:
                rideGroup = (ride_group *) &twister_rc_groups[index];
                break;
            default:
                return nullptr;
        }

        return rideGroup;
    }

    bool RideGroupsAreEqual(const ride_group * a, const ride_group * b) const override
    {
        if (a != nullptr && b != nullptr && (a->naming.name == b->naming.name && a->naming.description == b->naming.description))
        {
            return true;
        }
        return false;
    }

    bool RideGroupIsInvented(const ride_group * rideGroup) const override
    {
        if (!ride_type_is_invented(rideGroup->ride_type))
            return false;

        uint8 *rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(rideGroup->ride_type);

        while (*rideEntryIndexPtr != RIDE_ENTRY_INDEX_NULL)
        {
            uint8 rideEntryIndex = *rideEntryIndexPtr++;

            if (!ride_entry_is_invented(rideEntryIndex))
                continue;

            rct_ride_entry *rideEntry = get_ride_entry(rideEntryIndex);
            const ride_group * rideEntryRideGroup = GetRideGroup(rideGroup->ride_type, rideEntry);

            if (!RideGroupsAreEqual(rideGroup, rideEntryRideGroup))
                continue;

            // The ride entry is invented and belongs to the same ride group. This means the ride group is invented.
            return true;
        }

        return false;
    }

    const std::vector<const char *> GetPreferedRideEntryOrder(uint8 rideType) const override
    {
        static const std::vector<const char *> preferedRideEntryOrder[] =
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
            { },                                                                        // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT
            { "BMAIR   " },                                                             // RIDE_TYPE_FLYING_ROLLER_COASTER
            { },                                                                        // RIDE_TYPE_FLYING_ROLLER_COASTER_ALT
            { "VREEL   " },                                                             // RIDE_TYPE_VIRGINIA_REEL
            { "SPBOAT  " },                                                             // RIDE_TYPE_SPLASH_BOATS
            { "HELICAR " },                                                             // RIDE_TYPE_MINI_HELICOPTERS
            { "VEKST   " },                                                             // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
            { "SMONO   " },                                                             // RIDE_TYPE_SUSPENDED_MONORAIL
            { },                                                                        // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT
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
            { },                                                                        // RIDE_TYPE_59
            { "PREMT1  " },                                                             // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
       };
        return preferedRideEntryOrder[rideType];
    }
};

static std::unique_ptr<RideGroupManager> _rideGroupManager = std::unique_ptr<RideGroupManager>(new RideGroupManager());
IRideGroupManager * GetRideGroupManager()
{
    return _rideGroupManager.get();
}

extern "C"
{
    const ride_group * get_ride_group(uint8 rideType, rct_ride_entry * rideEntry)
    {
        const IRideGroupManager * rideGroupManager = GetRideGroupManager();
        return rideGroupManager->GetRideGroup(rideType, rideEntry);
    }

    bool ride_type_has_ride_groups(uint8 rideType)
    {
        const IRideGroupManager * rideGroupManager = GetRideGroupManager();
        return rideGroupManager->RideTypeHasRideGroups(rideType);
    }

    ride_group * ride_group_find(uint8 rideType, uint8 index)
    {
        const IRideGroupManager * rideGroupManager = GetRideGroupManager();
        return rideGroupManager->RideGroupFind(rideType, index);
    }

    bool ride_groups_are_equal(const ride_group * a, const ride_group * b)
    {
        const IRideGroupManager * rideGroupManager = GetRideGroupManager();
        return rideGroupManager->RideGroupsAreEqual(a, b);
    }
    bool ride_group_is_invented(const ride_group * rideGroup)
    {
        const IRideGroupManager * rideGroupManager = GetRideGroupManager();
        return rideGroupManager->RideGroupIsInvented(rideGroup);
    }

    /**
     * This function keeps a list of the preferred vehicle for every generic track
     * type, out of the available vehicle types in the current game. It determines
     * which picture is shown on the new ride tab and which train type is selected
     * by default.
     */
    sint32 vehicle_preference_compare(uint8 rideType, const char * a, const char * b)
    {
        const IRideGroupManager * rideGroupManager = GetRideGroupManager();
        std::vector<const char *> rideEntryOrder = rideGroupManager->GetPreferedRideEntryOrder(rideType);
        for (const char * object : rideEntryOrder)
        {
            if (String::Equals(object, a, true))
            {
                return -1;
            }
            if (String::Equals(object, b, true))
            {
                return  1;
            }
        }
        return 0;
    }
}
