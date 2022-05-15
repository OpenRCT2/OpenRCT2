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
            virtual void PeepChooseSeatFromCar(Peep* peep, Ride* ride, Vehicle* vehicle);
            virtual bool FindVehicleToEnter(Guest* guest, Ride* ride, std::vector<uint8_t>& car_array);
            virtual void UpdateRideFreeVehicleCheck(Guest* guest);
            virtual void UpdateRideEnterVehicle(Guest* guest);
            virtual void UpdateRideLeaveVehicle(Guest* guest);
            virtual void UpdateRideApproachSpiralSlide(Guest* guest);
            virtual StationIndex CheckValidStationNumbers(Ride* ride);
            virtual bool CreateVehicles(Ride* ride, const CoordsXYE& element, bool isApplying);
            virtual bool CreateCableLift(RideId rideIndex, bool isApplying);
            virtual void Test(Ride* ride, RideStatus newStatus, bool isApplying);
            virtual void Open(Ride* ride, bool isApplying);
            virtual bool IsPoweredLaunched(Ride* ride) const;
            virtual bool IsBlockSectioned(Ride* ride) const;
            virtual void UpdateMaxVehicles(Ride* ride);
            virtual uint16_t ComputeUpkeep(RideRatingUpdateState& state, Ride* ride);
            virtual void RideRatingsCalculateLaunchedFreefall(Ride* ride, RideRatingUpdateState& state);
            virtual void RideRatingsCalculateSpiralSlide(Ride* ride, RideRatingUpdateState& state);
            virtual void RideRatingsCalculateGoKarts(Ride* ride, RideRatingUpdateState& state);
            virtual void RideRatingsCalculateMotionSimulator(Ride* ride, RideRatingUpdateState& state);
            virtual void RideRatingsCalculate3dCinema(Ride* ride, RideRatingUpdateState& state);
            virtual void RideRatingsCalculateTopSpin(Ride* ride, RideRatingUpdateState& state);
            virtual void RideRatingsCalculateCompactInvertedCoaster(Ride* ride, RideRatingUpdateState& state);
            virtual void RideUpdateStation(Ride* ride, StationIndex stationIndex);
            virtual void UpdateMovingToEndOfStation(Vehicle* vehicle);
            virtual void TrainReadyToDepart(Vehicle* vehicle, uint8_t num_peeps_on_train, uint8_t num_used_seats);
            virtual void UpdateWaitingToDepart(Vehicle* vehicle);
            virtual void UpdateDeparting(Vehicle* vehicle);
            virtual void FinishDeparting(Vehicle* vehicle);
            virtual void UpdateTravelling(Vehicle* vehicle);
            virtual void UpdateArrivingPassThroughStation(
                Vehicle* vehicle, const Ride& curRide, const rct_ride_entry_vehicle& vehicleEntry, bool stationBrakesWork);
            virtual void UpdateArriving(Vehicle* vehicle);
            virtual void UpdateUnloadingPassengers(Vehicle* vehicle);
            virtual void UpdateFerrisWheelRotating(Vehicle* vehicle);
            virtual uint8_t WindowRideModeTweakStep();
            virtual void WindowRideOperatingInvalidate(
                Formatter& ft, rct_string_id& format, rct_string_id& caption, rct_string_id& tooltip);
            virtual uint8_t WindowRideOperatingPaintOffset();
        };
    } // namespace RideModes
} // namespace OpenRCT2
