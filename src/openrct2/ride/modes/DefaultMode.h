/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <vector>
#include "../RideData.h"

struct Ride;
struct Vehicle;
struct Peep;
struct Guest;
struct rct_ride_entry_vehicle;

namespace OpenRCT2
{
    namespace RideModes
    {
        class Default : public IRideMode
        {
        public:
            virtual rct_string_id GetOperationErrorMessage(Ride* ride) const;
            virtual void PeepChooseSeatFromCar(Peep* peep, Ride* ride, Vehicle* vehicle) const;
            virtual bool FindVehicleToEnter(Guest* guest, Ride* ride, std::vector<uint8_t>& car_array) const;
            virtual void UpdateRideFreeVehicleCheck(Guest* guest) const;
            virtual void UpdateRideEnterVehicle(Guest* guest) const;
            virtual void UpdateRideLeaveVehicle(Guest* guest) const;
            virtual void UpdateRideApproachSpiralSlide(Guest* guest) const;
            virtual StationIndex CheckValidStationNumbers(Ride* ride) const;
            virtual bool CreateVehicles(Ride* ride, const CoordsXYE& element, bool isApplying) const;
            virtual bool CreateCableLift(RideId rideIndex, bool isApplying) const;
            virtual void Test(Ride* ride, RideStatus newStatus, bool isApplying) const;
            virtual void Open(Ride* ride, bool isApplying) const;
            virtual bool IsPoweredLaunched(Ride* ride) const;
            virtual bool IsBlockSectioned(Ride* ride) const;
            virtual void UpdateMaxVehicles(Ride* ride) const;
            virtual uint16_t ComputeUpkeep(RideRatingUpdateState& state, Ride* ride) const;
            virtual void RideRatingsCalculateLaunchedFreefall(Ride* ride, RideRatingUpdateState& state) const;
            virtual void RideRatingsCalculateSpiralSlide(Ride* ride, RideRatingUpdateState& state) const;
            virtual void RideRatingsCalculateGoKarts(Ride* ride, RideRatingUpdateState& state) const;
            virtual void RideRatingsCalculateMotionSimulator(Ride* ride, RideRatingUpdateState& state) const;
            virtual void RideRatingsCalculate3dCinema(Ride* ride, RideRatingUpdateState& state) const;
            virtual void RideRatingsCalculateTopSpin(Ride* ride, RideRatingUpdateState& state) const;
            virtual void RideRatingsCalculateCompactInvertedCoaster(Ride* ride, RideRatingUpdateState& state) const;
            virtual void RideUpdateStation(Ride* ride, StationIndex stationIndex) const;
            virtual void UpdateMovingToEndOfStation(Vehicle* vehicle) const;
            virtual void TrainReadyToDepart(Vehicle* vehicle, uint8_t num_peeps_on_train, uint8_t num_used_seats) const;
            virtual void UpdateWaitingToDepart(Vehicle* vehicle) const;
            virtual void UpdateDeparting(Vehicle* vehicle) const;
            virtual void FinishDeparting(Vehicle* vehicle) const;
            virtual void UpdateTravelling(Vehicle* vehicle) const;
            virtual void UpdateArrivingPassThroughStation(
                Vehicle* vehicle, const Ride& curRide, const rct_ride_entry_vehicle& vehicleEntry,
                bool stationBrakesWork) const;
            virtual void UpdateArriving(Vehicle* vehicle) const;
            virtual void UpdateUnloadingPassengers(Vehicle* vehicle) const;
            virtual void UpdateFerrisWheelRotating(Vehicle* vehicle) const;
            virtual uint8_t WindowRideModeTweakStep() const;
            virtual void WindowRideOperatingInvalidate(
                Formatter& ft, rct_string_id& format, rct_string_id& caption, rct_string_id& tooltip) const;
            virtual uint8_t WindowRideOperatingPaintOffset() const;
        };
    } // namespace RideModes
} // namespace OpenRCT2
