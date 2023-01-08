/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Identifiers.h"
#include "../../Limits.h"
#include "../../core/BitSet.hpp"
#include "../../ride/VehicleEntry.h"
#include "../Peep.h"

#include <cstdint>
#include <vector>

struct CarEntry;
struct Guest;
struct Ride;
struct Vehicle;

class GuestRideHelper
{
    friend struct Guest;

private:
    Guest& _guest;

public:
    GuestRideHelper(Guest& guest);

    void HeadForNearestRideWithFlags(bool considerOnlyCloseRides, int64_t rideTypeFlags);
    bool ShouldRideWhileRaining(const Ride& ride);
    void ChoseNotToGoOnRide(const Ride& ride, bool peepAtRide, bool updateLastRide);
    void PickRideToGoOn();
    bool ShouldGoOnRide(Ride& ride, StationIndex entranceNum, bool atQueue, bool thinking);

    bool UpdateQueuePosition(PeepActionType previous_action);
    void RemoveFromQueue();

    void OnEnterRide(Ride& ride);
    void OnExitRide(Ride& ride);

    void SetHasRidden(const Ride& ride);
    bool HasRidden(const Ride& ride) const;
    void SetHasRiddenRideType(int32_t rideType);
    bool HasRiddenRideType(int32_t rideType) const;
    // Removes the ride from the guests memory, this includes
    // the history, thoughts, etc.
    void RemoveRideFromMemory(RideId rideId);

private:
    void UpdateRide();
    void UpdateQueuing();

    void UpdateRideAtEntrance();
    void UpdateRideAdvanceThroughEntrance();
    void UpdateRideLeaveEntranceWaypoints(const Ride& ride);
    uint8_t GetWaypointedSeatLocation(const Ride& ride, CarEntry* vehicle_type, uint8_t track_direction) const;
    void UpdateRideFreeVehicleCheck();
    void UpdateRideFreeVehicleEnterRide(Ride& ride);
    void UpdateRideApproachVehicle();
    void UpdateRideEnterVehicle();
    void UpdateRideLeaveVehicle();
    void UpdateRideApproachExit();
    void UpdateRideInExit();

    void UpdateRideApproachVehicleWaypoints();

    void UpdateRideApproachExitWaypoints();
    void UpdateRideApproachSpiralSlide();
    void UpdateRideOnSpiralSlide();
    void UpdateRideLeaveSpiralSlide();
    void UpdateRideMazePathfinding();
    void UpdateRideLeaveExit();
    void UpdateRideShopApproach();
    void UpdateRideShopInteract();
    void UpdateRideShopLeave();
    void UpdateRidePrepareForExit();

    Ride* FindBestRideToGoOn();
    OpenRCT2::BitSet<OpenRCT2::Limits::MaxRidesInPark> FindRidesToGoOn();
    bool FindVehicleToEnter(const Ride& ride, std::vector<uint8_t>& car_array);
    void GoToRideEntrance(const Ride& ride);
    bool HasVoucherForFreeRide(const Ride& ride) const;
    void TriedToEnterFullQueue(Ride& ride);
    void ResetRideHeading();
    void RideIsTooIntense(Ride& ride, bool peepAtRide);
    int16_t CalculateRideSatisfaction(const Ride& ride) const;
    int16_t CalculateRideValueSatisfaction(const Ride& ride) const;
    int16_t CalculateRideIntensityNauseaSatisfaction(const Ride& ride) const;
    void UpdateFavouriteRide(const Ride& ride);

    bool ShouldGoOnRideAgain(const Ride& ride) const;
    void UpdateRideNauseaGrowth(const Ride& ride);
    bool ShouldPreferredIntensityIncrease() const;
    bool ReallyLikedRide(const Ride& ride) const;
    bool FindRideToLookAt(uint8_t edge, RideId* rideToView, uint8_t* rideSeatToView) const;
    bool Loc690FD0(RideId* rideToView, uint8_t* rideSeatToView, TileElement* tileElement) const;
    bool CheckRidePriceAtEntrance(const Ride& ride, money32 ridePrice);
    void UpdateRideAtEntranceTryLeave();
    Vehicle* ChooseCarFromRide(const Ride& ride, std::vector<uint8_t>& car_array);
    void ChooseSeatFromCar(const Ride& ride, Vehicle& vehicle);
    void GoToRideExit(const Ride& ride, int16_t x, int16_t y, int16_t z, uint8_t exit_direction);
    void UpdateRideNoFreeVehicleRejoinQueue(Ride& ride);

    bool ShouldGoToShop(Ride& ride, bool peepAtShop);
};

void PeepUpdateRideLeaveEntranceMaze(Guest& guest, Ride& ride, CoordsXYZD& entrance_loc);
void PeepUpdateRideLeaveEntranceSpiralSlide(Guest& guest, Ride& ride, CoordsXYZD& entrance_loc);
void PeepUpdateRideLeaveEntranceDefault(Guest& guest, Ride& ride, CoordsXYZD& entrance_loc);

CoordsXY GetGuestWaypointLocationDefault(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation);
CoordsXY GetGuestWaypointLocationEnterprise(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation);
