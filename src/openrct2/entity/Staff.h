/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "../core/Money.hpp"
#include "Peep.h"

#include <cstdint>
#include <vector>

struct Ride;

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

namespace OpenRCT2
{
    class DataSerialiser;

    struct TileElement;
    struct PathElement;

    class PatrolArea;

    enum class StaffType : uint8_t
    {
        handyman,
        mechanic,
        security,
        entertainer,

        count
    };

    struct Staff : Peep
    {
        static constexpr auto cEntityType = EntityType::staff;

    public:
        PatrolArea* patrolInfo;
        StaffType assignedStaffType;
        uint16_t mechanicTimeSinceCall; // time getting to ride to fix
        int32_t hireDate;
        uint8_t staffOrders;
        uint8_t staffMowingTimeout;

        union // 1st statistic
        {
            uint32_t staffLawnsMown;
            uint32_t staffRidesFixed;
            uint32_t staffGuestsEntertained;
        };
        union // 2nd statistic
        {
            uint32_t staffGardensWatered;
            uint32_t staffRidesInspected;
        };
        union // 3rd statistic
        {
            uint32_t staffLitterSwept;
            uint32_t staffVandalsStopped;
        };
        union // 4th statistic
        {
            uint32_t staffBinsEmptied;
        };

        void Update();
        void tick128UpdateStaff();
        bool isMechanic() const;
        bool isEntertainer() const;
        bool isPatrolAreaSet(const CoordsXY& coords) const;
        bool isLocationInPatrol(const CoordsXY& loc) const;
        bool isLocationOnPatrolEdge(const CoordsXY& loc) const;
        bool doPathFinding();
        void setHireDate(int32_t value);
        int32_t getHireDate() const;

        bool canIgnoreWideFlag(const CoordsXYZ& staffPos, TileElement* path) const;

        static void resetStats();
        void serialise(DataSerialiser& stream);

        void clearPatrolArea();
        void setPatrolArea(const CoordsXY& coords, bool value);
        void setPatrolArea(const MapRange& range, bool value);
        bool hasPatrolArea() const;

    private:
        void updatePatrolling();
        void updateMowing();
        void updateSweeping();
        void updateEmptyingBin();
        void updateWatering();
        void updateAnswering();
        void updateFixing(int32_t steps);
        bool updateFixingEnterStation(Ride& ride) const;
        bool updateFixingMoveToBrokenDownVehicle(bool firstRun, const Ride& ride);
        bool updateFixingFixVehicle(bool firstRun, const Ride& ride);
        bool updateFixingFixVehicleMalfunction(bool firstRun, const Ride& ride);
        bool updateFixingMoveToStationEnd(bool firstRun, const Ride& ride);
        bool updateFixingFixStationEnd(bool firstRun);
        bool updateFixingMoveToStationStart(bool firstRun, const Ride& ride);
        bool updateFixingFixStationStart(bool firstRun, const Ride& ride);
        bool updateFixingFixStationBrakes(bool firstRun, Ride& ride);
        bool updateFixingMoveToStationExit(bool firstRun, const Ride& ride);
        bool updateFixingFinishFixOrInspect(bool firstRun, int32_t steps, Ride& ride);
        bool updateFixingLeaveByEntranceExit(bool firstRun, const Ride& ride);
        void updateRideInspected(RideId rideIndex);
        void updateHeadingToInspect();

        bool doHandymanPathFinding();
        bool doMechanicPathFinding();
        bool doEntertainerPathFinding();
        bool doMiscPathFinding();
        bool isMechanicHeadingToFixRideBlockingPath();

        Direction handymanDirectionRandSurface(uint8_t validDirections) const;

        void entertainerUpdateNearbyPeeps();
        bool securityGuardPathIsCrowded() const;

        uint8_t getValidPatrolDirections(const CoordsXY& loc) const;
        Direction handymanDirectionToNearestLitter() const;
        uint8_t handymanDirectionToUncutGrass(uint8_t valid_directions) const;
        Direction directionSurface(Direction initialDirection) const;
        Direction directionPath(uint8_t validDirections, PathElement* pathElement) const;
        Direction mechanicDirectionSurface() const;
        Direction mechanicDirectionPathRand(uint8_t pathDirections) const;
        Direction mechanicDirectionPath(uint8_t validDirections, PathElement* pathElement);
        bool updatePatrollingFindWatering();
        bool updatePatrollingFindBin();
        bool updatePatrollingFindSweeping();
        bool updatePatrollingFindGrass();
    };
    static_assert(sizeof(Staff) <= 512);

    enum STAFF_ORDERS
    {
        STAFF_ORDERS_SWEEPING = (1 << 0),
        STAFF_ORDERS_WATER_FLOWERS = (1 << 1),
        STAFF_ORDERS_EMPTY_BINS = (1 << 2),
        STAFF_ORDERS_MOWING = (1 << 3),
        STAFF_ORDERS_INSPECT_RIDES = (1 << 0),
        STAFF_ORDERS_FIX_RIDES = (1 << 1)
    };

    Drawing::Colour StaffGetColour(StaffType staffType);
    GameActions::Result StaffSetColour(StaffType staffType, Drawing::Colour value);

    money64 GetStaffWage(StaffType type);

    const PatrolArea& GetMergedPatrolArea(StaffType type);
} // namespace OpenRCT2
