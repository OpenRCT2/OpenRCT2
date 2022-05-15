#include "DefaultMode.h"
/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

rct_string_id OpenRCT2::RideModes::Default::GetOperationErrorMessage(Ride* ride) const
{
    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        return STR_CANT_CHANGE_THIS;
    }
    return STR_CANT_CHANGE_LAUNCH_SPEED;
}

void OpenRCT2::RideModes::Default::PeepChooseSeatFromCar(Peep* peep, Ride* ride, Vehicle* vehicle)
{
}

bool OpenRCT2::RideModes::Default::FindVehicleToEnter(Guest* guest, Ride* ride, std::vector<uint8_t>& car_array)
{
    return false;
}

void OpenRCT2::RideModes::Default::UpdateRideFreeVehicleCheck(Guest* guest)
{
}

void OpenRCT2::RideModes::Default::UpdateRideEnterVehicle(Guest* guest)
{
}

void OpenRCT2::RideModes::Default::UpdateRideLeaveVehicle(Guest* guest)
{
}

void OpenRCT2::RideModes::Default::UpdateRideApproachSpiralSlide(Guest* guest)
{
}

StationIndex OpenRCT2::RideModes::Default::CheckValidStationNumbers(Ride* ride)
{
    return StationIndex();
}

bool OpenRCT2::RideModes::Default::CreateVehicles(Ride* ride, const CoordsXYE& element, bool isApplying)
{
    return false;
}

bool OpenRCT2::RideModes::Default::CreateCableLift(RideId rideIndex, bool isApplying)
{
    return false;
}

void OpenRCT2::RideModes::Default::Test(Ride* ride, RideStatus newStatus, bool isApplying)
{
}

void OpenRCT2::RideModes::Default::Open(Ride* ride, bool isApplying)
{
}

bool OpenRCT2::RideModes::Default::IsPoweredLaunched(Ride* ride) const
{
    return false;
}

bool OpenRCT2::RideModes::Default::IsBlockSectioned(Ride* ride) const
{
    return false;
}

void OpenRCT2::RideModes::Default::UpdateMaxVehicles(Ride* ride)
{
}

uint16_t OpenRCT2::RideModes::Default::ComputeUpkeep(RideRatingUpdateState& state, Ride* ride)
{
    return uint16_t();
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateLaunchedFreefall(Ride* ride, RideRatingUpdateState& state)
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateSpiralSlide(Ride* ride, RideRatingUpdateState& state)
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateGoKarts(Ride* ride, RideRatingUpdateState& state)
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateMotionSimulator(Ride* ride, RideRatingUpdateState& state)
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculate3dCinema(Ride* ride, RideRatingUpdateState& state)
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateTopSpin(Ride* ride, RideRatingUpdateState& state)
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateCompactInvertedCoaster(Ride* ride, RideRatingUpdateState& state)
{
}

void OpenRCT2::RideModes::Default::RideUpdateStation(Ride* ride, StationIndex stationIndex)
{
}

void OpenRCT2::RideModes::Default::UpdateMovingToEndOfStation(Vehicle* vehicle)
{
}

void OpenRCT2::RideModes::Default::TrainReadyToDepart(Vehicle* vehicle, uint8_t num_peeps_on_train, uint8_t num_used_seats)
{
}

void OpenRCT2::RideModes::Default::UpdateWaitingToDepart(Vehicle* vehicle)
{
}

void OpenRCT2::RideModes::Default::UpdateDeparting(Vehicle* vehicle)
{
}

void OpenRCT2::RideModes::Default::FinishDeparting(Vehicle* vehicle)
{
}

void OpenRCT2::RideModes::Default::UpdateTravelling(Vehicle* vehicle)
{
}

void OpenRCT2::RideModes::Default::UpdateArrivingPassThroughStation(
    Vehicle* vehicle, const Ride& curRide, const rct_ride_entry_vehicle& vehicleEntry, bool stationBrakesWork)
{
}

void OpenRCT2::RideModes::Default::UpdateArriving(Vehicle* vehicle)
{
}

void OpenRCT2::RideModes::Default::UpdateUnloadingPassengers(Vehicle* vehicle)
{
}

void OpenRCT2::RideModes::Default::UpdateFerrisWheelRotating(Vehicle* vehicle)
{
}

uint8_t OpenRCT2::RideModes::Default::WindowRideModeTweakStep()
{
    return uint8_t();
}

void OpenRCT2::RideModes::Default::WindowRideOperatingInvalidate(
    Formatter& ft, rct_string_id& format, rct_string_id& caption, rct_string_id& tooltip)
{
}

uint8_t OpenRCT2::RideModes::Default::WindowRideOperatingPaintOffset()
{
    return uint8_t();
}
