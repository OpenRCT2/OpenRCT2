/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Vehicle.h"

#include "../GameState.h"
#include "../audio/Audio.h"
#include "../config/Config.h"
#include "../core/Speed.hpp"
#include "../entity/EntityRegistry.h"
#include "../entity/Guest.h"
#include "../entity/Peep.h"
#include "../localisation/Formatter.h"
#include "../management/NewsItem.h"
#include "../world/Map.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

struct SynchronisedVehicle
{
    RideId ride_id;
    StationIndex stationIndex;
    EntityId vehicle_id;
};

constexpr int32_t SYNCHRONISED_VEHICLE_COUNT = 16;

// Synchronised vehicle info
static SynchronisedVehicle _synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT] = {};

static SynchronisedVehicle* _lastSynchronisedVehicle = nullptr;

/**
 * Checks if a map position contains a synchronised ride station and adds the vehicle
 * to synchronise to the vehicle synchronisation list.
 *  rct2: 0x006DE1A4
 */
static bool try_add_synchronised_station(const CoordsXYZ& coords)
{
    // make sure we are in map bounds
    if (!MapIsLocationValid(coords))
    {
        return false;
    }

    TileElement* tileElement = GetStationPlatform({ coords, coords.z + 2 * kCoordsZStep });
    if (tileElement == nullptr)
    {
        /* No station platform element found,
         * so no station to synchronise */
        return false;
    }

    auto rideIndex = tileElement->AsTrack()->GetRideIndex();
    auto ride = GetRide(rideIndex);
    if (ride == nullptr || !(ride->departFlags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS))
    {
        /* Ride is not set to synchronise with adjacent stations. */
        return false;
    }

    /* From this point on, the ride of the map element is one that is set
     * to sync with adjacent stations, so it will return true.
     * Still to determine if a vehicle to sync can be identified. */

    auto stationIndex = tileElement->AsTrack()->GetStationIndex();

    SynchronisedVehicle* sv = _lastSynchronisedVehicle;
    sv->ride_id = rideIndex;
    sv->stationIndex = stationIndex;
    sv->vehicle_id = EntityId::GetNull();
    _lastSynchronisedVehicle++;

    /* Ride vehicles are not on the track (e.g. ride is/was under
     * construction), so just return; vehicle_id for this station
     * is SPRITE_INDEX_NULL. */
    if (!ride->flags.has(RideFlag::onTrack))
    {
        return true;
    }

    /* Station is not ready to depart, so just return;
     * vehicle_id for this station is SPRITE_INDEX_NULL. */
    if (!(ride->getStation(stationIndex).Depart & kStationDepartFlag))
    {
        return true;
    }

    // Look for a vehicle on this station waiting to depart.
    for (int32_t i = 0; i < ride->numTrains; i++)
    {
        auto* vehicle = getGameState().entities.GetEntity<Vehicle>(ride->vehicles[i]);
        if (vehicle == nullptr)
        {
            continue;
        }

        if (vehicle->status != Vehicle::Status::waitingToDepart)
        {
            continue;
        }
        if (vehicle->sub_state != 0)
        {
            continue;
        }
        if (!vehicle->flags.has(VehicleFlag::waitingOnAdjacentStation))
        {
            continue;
        }
        if (vehicle->current_station != stationIndex)
        {
            continue;
        }

        sv->vehicle_id = vehicle->Id;
        return true;
    }

    /* No vehicle found waiting to depart (with sync adjacent) at the
     * station, so just return; vehicle_id for this station is
     * SPRITE_INDEX_NULL. */
    return true;
}

/**
 * Checks whether a vehicle can depart a station when set to synchronise with adjacent stations.
 *  rct2: 0x006DE287
 * @param vehicle The vehicle waiting to depart.
 * @returns true if the vehicle can depart (all adjacent trains are ready or broken down), otherwise false.
 *
 * Permits vehicles to depart in two ways:
 *  Returns true, permitting the vehicle in the param to depart immediately;
 *  The vehicle flag VehicleFlags::WaitingOnAdjacentStation is cleared for those
 *  vehicles that depart in sync with the vehicle in the param.
 */
static bool ride_station_can_depart_synchronised(const Ride& ride, StationIndex stationIndex)
{
    const auto& station = ride.getStation(stationIndex);
    auto location = station.GetStart();

    auto tileElement = MapGetTrackElementAt(location);
    if (tileElement == nullptr)
    {
        return false;
    }

    // Reset the list of synchronised vehicles to empty.
    _lastSynchronisedVehicle = _synchronisedVehicles;

    /* Search for stations to sync in both directions from the current tile.
     * We allow for some space between stations, and every time a station
     *  is found we allow for space between that and the next.
     */

    int32_t direction = tileElement->GetDirectionWithOffset(1);
    constexpr uint8_t maxCheckDistance = kRideAdjacencyCheckDistance;
    uint8_t spaceBetween = maxCheckDistance;

    while (_lastSynchronisedVehicle < &_synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT - 1])
    {
        location += CoordsXYZ{ CoordsDirectionDelta[direction], 0 };
        if (try_add_synchronised_station(location))
        {
            spaceBetween = maxCheckDistance;
            continue;
        }
        if (spaceBetween-- == 0)
        {
            break;
        }
    }

    // Other search direction.
    location = station.GetStart();
    direction = DirectionReverse(direction) & 3;
    spaceBetween = maxCheckDistance;
    while (_lastSynchronisedVehicle < &_synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT - 1])
    {
        location += CoordsXYZ{ CoordsDirectionDelta[direction], 0 };
        if (try_add_synchronised_station(location))
        {
            spaceBetween = maxCheckDistance;
            continue;
        }
        if (spaceBetween-- == 0)
        {
            break;
        }
    }

    if (_lastSynchronisedVehicle == _synchronisedVehicles)
    {
        // No adjacent stations, allow depart
        return true;
    }

    for (SynchronisedVehicle* sv = _synchronisedVehicles; sv < _lastSynchronisedVehicle; sv++)
    {
        Ride* sv_ride = GetRide(sv->ride_id);

        if (!sv_ride->flags.has(RideFlag::brokenDown))
        {
            if (sv_ride->status != RideStatus::closed)
            {
                if (sv_ride->isBlockSectioned())
                {
                    if (!(sv_ride->getStation(sv->stationIndex).Depart & kStationDepartFlag))
                    {
                        sv = _synchronisedVehicles;
                        RideId rideId = RideId::GetNull();
                        for (; sv < _lastSynchronisedVehicle; sv++)
                        {
                            if (rideId.IsNull())
                            {
                                rideId = sv->ride_id;
                            }
                            if (rideId != sv->ride_id)
                            {
                                // Here the sync-ed stations are not all from the same ride.
                                return false;
                            }
                        }

                        /* Here all the of sync-ed stations are from the same ride */
                        auto curRide = GetRide(rideId);
                        if (curRide != nullptr)
                        {
                            for (int32_t i = 0; i < curRide->numTrains; i++)
                            {
                                Vehicle* v = getGameState().entities.GetEntity<Vehicle>(curRide->vehicles[i]);
                                if (v == nullptr)
                                {
                                    continue;
                                }
                                if (v->status != Vehicle::Status::waitingToDepart && v->velocity != 0)
                                {
                                    // Here at least one vehicle on the ride is moving.
                                    return false;
                                }
                            }
                        }

                        // UNCERTAIN: is the return desired here, or rather continue on with the general checks?
                        return true;
                    }
                }
                // There is no vehicle waiting at this station to sync with.
                if (sv->vehicle_id.IsNull())
                {
                    // Check conditions for departing without all stations being in sync.
                    if (_lastSynchronisedVehicle > &_synchronisedVehicles[1])
                    {
                        // Sync condition: there are at least 3 stations to sync
                        return false;
                    }
                    RideId someRideIndex = _synchronisedVehicles[0].ride_id;
                    if (someRideIndex != ride.id)
                    {
                        // Sync condition: the first station to sync is a different ride
                        return false;
                    }

                    int32_t numTrainsAtStation = 0;
                    int32_t numTravelingTrains = 0;
                    auto currentStation = sv->stationIndex;
                    for (int32_t i = 0; i < sv_ride->numTrains; i++)
                    {
                        auto* otherVehicle = getGameState().entities.GetEntity<Vehicle>(sv_ride->vehicles[i]);
                        if (otherVehicle == nullptr)
                        {
                            continue;
                        }
                        if (otherVehicle->status != Vehicle::Status::travelling)
                        {
                            if (currentStation == otherVehicle->current_station)
                            {
                                if (otherVehicle->status == Vehicle::Status::waitingToDepart
                                    || otherVehicle->status == Vehicle::Status::movingToEndOfStation)
                                {
                                    numTrainsAtStation++;
                                }
                            }
                        }
                        else
                        {
                            numTravelingTrains++;
                        }
                    }

                    int32_t totalTrains = numTrainsAtStation + numTravelingTrains;
                    if (totalTrains != sv_ride->numTrains || numTravelingTrains >= sv_ride->numTrains / 2)
                    {
                        // if (numArrivingTrains > 0 || numTravelingTrains >= sv_ride->numTrains / 2) {
                        /* Sync condition: If there are trains arriving at the
                         * station or half or more of the ride trains are
                         * travelling, this station must be sync-ed before the
                         * trains can depart! */
                        return false;
                    }

                    /* Sync exception - train is not arriving at the station
                     * and there are less than half the trains for the ride
                     * travelling. */
                }
            }
        }
    }

    // At this point all vehicles in _snychronisedVehicles can depart.
    for (SynchronisedVehicle* sv = _synchronisedVehicles; sv < _lastSynchronisedVehicle; sv++)
    {
        auto v = getGameState().entities.GetEntity<Vehicle>(sv->vehicle_id);
        if (v != nullptr)
        {
            v->flags.unset(VehicleFlag::waitingOnAdjacentStation);
        }
    }

    return true;
}

bool Vehicle::CanDepartSynchronised() const
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return false;
    return ride_station_can_depart_synchronised(*curRide, current_station);
}

/**
 *
 *  rct2: 0x006D7BCC
 */
void Vehicle::UpdateMovingToEndOfStation()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    int32_t curFlags = 0;
    int32_t station = 0;

    switch (curRide->mode)
    {
        case RideMode::upwardLaunch:
        case RideMode::rotatingLift:
        case RideMode::downwardLaunch:
        case RideMode::freefallDrop:
            if (velocity >= -131940)
            {
                acceleration = -3298;
            }
            else
            {
                velocity -= velocity / 16;
                acceleration = 0;
            }
            curFlags = UpdateTrackMotion(&station);
            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_5))
                break;
            [[fallthrough]];
        case RideMode::dodgems:
        case RideMode::swing:
        case RideMode::rotation:
        case RideMode::forwardRotation:
        case RideMode::backwardRotation:
        case RideMode::filmAvengingAviators:
        case RideMode::filmThrillRiders:
        case RideMode::beginners:
        case RideMode::intense:
        case RideMode::berserk:
        case RideMode::mouseTails3DFilm:
        case RideMode::stormChasers3DFilm:
        case RideMode::spaceRaiders3DFilm:
        case RideMode::spaceRings:
        case RideMode::hauntedHouse:
        case RideMode::crookedHouse:
        case RideMode::circus:
            current_station = StationIndex::FromUnderlying(0);
            velocity = 0;
            acceleration = 0;
            SetState(Status::waitingForPassengers);
            break;
        default:
        {
            const auto* rideEntry = GetRideEntry();
            if (rideEntry == nullptr)
            {
                return;
            }

            const auto& carEntry = rideEntry->Cars[vehicle_type];

            if (!carEntry.flags.has(CarEntryFlag::isPowered))
            {
                if (velocity <= 131940)
                {
                    acceleration = 3298;
                }
            }
            if (velocity > 131940)
            {
                velocity -= velocity / 16;
                acceleration = 0;
            }

            curFlags = UpdateTrackMotion(&station);

            if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_1)
            {
                velocity = 0;
                acceleration = 0;
                sub_state++;

                if (curRide->mode == RideMode::race && sub_state >= 40)
                {
                    SetState(Status::waitingForPassengers);
                    break;
                }
            }
            else
            {
                if (velocity > 98955)
                {
                    sub_state = 0;
                }
            }

            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION))
                break;

            current_station = StationIndex::FromUnderlying(station);
            velocity = 0;
            acceleration = 0;
            SetState(Status::waitingForPassengers);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006D7FB4
 */
void Vehicle::TrainReadyToDepart(uint8_t num_peeps_on_train, uint8_t num_used_seats)
{
    if (num_peeps_on_train != num_used_seats)
        return;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RideStatus::open && !curRide->flags.has(RideFlag::brokenDown)
        && !flags.has(VehicleFlag::readyToDepart))
    {
        return;
    }

    if (!curRide->flags.has(RideFlag::brokenDown))
    {
        const auto& rtd = curRide->getRideTypeDescriptor();
        // Original code did not check if the ride was a boat hire, causing empty boats to leave the platform when closing a
        // Boat Hire with passengers on it.
        if (curRide->status != RideStatus::closed || (curRide->numRiders != 0 && rtd.specialType != RtdSpecialType::boatHire))
        {
            curRide->getStation(current_station).TrainAtStation = RideStation::kNoTrain;
            sub_state = 2;
            return;
        }
    }

    if (curRide->mode == RideMode::forwardRotation || curRide->mode == RideMode::backwardRotation)
    {
        uint8_t seat = ((-flatRideAnimationFrame) / 8) & 0xF;
        if (!peep[seat].IsNull())
        {
            curRide->getStation(current_station).TrainAtStation = RideStation::kNoTrain;
            SetState(Status::unloadingPassengers);
            return;
        }

        if (num_peeps == 0)
            return;

        curRide->getStation(current_station).TrainAtStation = RideStation::kNoTrain;
        sub_state = 2;
        return;
    }

    if (num_peeps_on_train == 0)
        return;

    curRide->getStation(current_station).TrainAtStation = RideStation::kNoTrain;
    SetState(Status::waitingForPassengers);
}

/**
 *
 *  rct2: 0x006D7DA1
 */
void Vehicle::UpdateWaitingForPassengers()
{
    velocity = 0;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (sub_state == 0)
    {
        if (!OpenRestraints())
            return;

        auto& station = curRide->getStation(current_station);
        if (station.Entrance.IsNull())
        {
            station.TrainAtStation = RideStation::kNoTrain;
            sub_state = 2;
            return;
        }

        auto trainIndex = ride_get_train_index_from_vehicle(*curRide, Id);
        if (!trainIndex.has_value())
        {
            return;
        }

        if (station.TrainAtStation != RideStation::kNoTrain)
            return;

        station.TrainAtStation = trainIndex.value();
        sub_state = 1;
        time_waiting = 0;

        Invalidate();
        return;
    }
    if (sub_state == 1)
    {
        if (time_waiting != 0xFFFF)
            time_waiting++;

        flags.unset(VehicleFlag::readyToDepart);

        // 0xF64E31, 0xF64E32, 0xF64E33
        uint8_t num_peeps_on_train = 0, num_used_seats_on_train = 0, num_seats_on_train = 0;

        for (const Vehicle* trainCar = getGameState().entities.GetEntity<Vehicle>(Id); trainCar != nullptr;
             trainCar = getGameState().entities.GetEntity<Vehicle>(trainCar->next_vehicle_on_train))
        {
            num_peeps_on_train += trainCar->num_peeps;
            num_used_seats_on_train += trainCar->next_free_seat;
            num_seats_on_train += trainCar->num_seats;
        }

        num_seats_on_train &= 0x7F;

        if (curRide->supportsStatus(RideStatus::testing))
        {
            if (time_waiting < 20)
            {
                TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                return;
            }
        }
        else
        {
            if (num_peeps_on_train == 0)
            {
                TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                return;
            }
        }

        if (curRide->getRideTypeDescriptor().flags.has(RtdFlag::hasLoadOptions))
        {
            if (curRide->departFlags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH)
            {
                if (curRide->minWaitingTime * 32 > time_waiting)
                {
                    TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                    return;
                }
            }
            if (curRide->departFlags & RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH)
            {
                if (curRide->maxWaitingTime * 32 < time_waiting)
                {
                    flags.set(VehicleFlag::readyToDepart);
                    TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                    return;
                }
            }
        }

        if (curRide->departFlags & RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES)
        {
            for (auto train_id : curRide->vehicles)
            {
                if (train_id == Id)
                    continue;

                Vehicle* train = getGameState().entities.GetEntity<Vehicle>(train_id);
                if (train == nullptr)
                    continue;

                if (train->status == Status::unloadingPassengers || train->status == Status::movingToEndOfStation)
                {
                    if (train->current_station == current_station)
                    {
                        flags.set(VehicleFlag::readyToDepart);
                        TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                        return;
                    }
                }
            }
        }

        if (curRide->getRideTypeDescriptor().flags.has(RtdFlag::hasLoadOptions)
            && curRide->departFlags & RIDE_DEPART_WAIT_FOR_LOAD)
        {
            if (num_peeps_on_train == num_seats_on_train)
            {
                flags.set(VehicleFlag::readyToDepart);
                TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                return;
            }

            // any load: load=4 , full: load=3 , 3/4s: load=2 , half: load=1 , quarter: load=0
            uint8_t load = curRide->departFlags & RIDE_DEPART_WAIT_FOR_LOAD_MASK;

            // We want to wait for ceiling((load+1)/4 * num_seats_on_train) peeps, the +3 below is used instead of
            // ceil() to prevent issues on different cpus/platforms with floats. Note that vanilla RCT1/2 rounded
            // down here; our change reflects the expected behaviour for waiting for a minimum load target (see #9987)
            uint8_t peepTarget = ((load + 1) * num_seats_on_train + 3) / 4;

            if (load == 4) // take care of "any load" special case
                peepTarget = 1;

            if (num_peeps_on_train >= peepTarget)
                flags.set(VehicleFlag::readyToDepart);

            TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
            return;
        }

        flags.set(VehicleFlag::readyToDepart);
        TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
        return;
    }

    if (!CloseRestraints())
        return;

    velocity = 0;
    flags.unset(VehicleFlag::waitingOnAdjacentStation);

    if (curRide->departFlags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)
    {
        flags.set(VehicleFlag::waitingOnAdjacentStation);
    }

    SetState(Status::waitingToDepart);
}

/**
 *
 *  rct2: 0x006D80BE
 */
void Vehicle::UpdateWaitingToDepart()
{
    auto* curRide = GetRide();
    if (curRide == nullptr)
        return;

    const auto& currentStation = curRide->getStation(current_station);

    bool shouldBreak = false;
    if (curRide->flags.has(RideFlag::brokenDown))
    {
        switch (curRide->breakdownReasonPending)
        {
            case BREAKDOWN_RESTRAINTS_STUCK_CLOSED:
            case BREAKDOWN_RESTRAINTS_STUCK_OPEN:
            case BREAKDOWN_DOORS_STUCK_CLOSED:
            case BREAKDOWN_DOORS_STUCK_OPEN:
                break;
            default:
                shouldBreak = true;
                break;
        }
    }

    bool skipCheck = false;
    if (shouldBreak || curRide->status != RideStatus::open)
    {
        if (curRide->mode == RideMode::forwardRotation || curRide->mode == RideMode::backwardRotation)
        {
            uint8_t seat = ((-flatRideAnimationFrame) >> 3) & 0xF;
            if (peep[seat * 2].IsNull())
            {
                if (num_peeps == 0)
                {
                    skipCheck = true;
                }
            }
            else
            {
                if (!currentStation.Exit.IsNull())
                {
                    SetState(Status::unloadingPassengers);
                    return;
                }
            }
        }
        else
        {
            for (const Vehicle* trainCar = getGameState().entities.GetEntity<Vehicle>(Id); trainCar != nullptr;
                 trainCar = getGameState().entities.GetEntity<Vehicle>(trainCar->next_vehicle_on_train))
            {
                if (trainCar->num_peeps != 0)
                {
                    if (!currentStation.Exit.IsNull())
                    {
                        SetState(Status::unloadingPassengers);
                        return;
                    }
                    break;
                }
            }
        }
    }

    if (!skipCheck)
    {
        if (!(currentStation.Depart & kStationDepartFlag))
            return;
    }

    if (curRide->getRideTypeDescriptor().flags.has(RtdFlag::canSynchroniseWithAdjacentStations))
    {
        if (curRide->departFlags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)
        {
            if (flags.has(VehicleFlag::waitingOnAdjacentStation))
            {
                if (!CanDepartSynchronised())
                {
                    return;
                }
            }
        }
    }

    SetState(Status::departing);

    if (curRide->flags.has(RideFlag::cableLift))
    {
        CoordsXYE track;
        int32_t zUnused;
        int32_t direction;

        uint8_t trackDirection = GetTrackDirection();
        if (TrackBlockGetNextFromZero(TrackLocation, *curRide, trackDirection, &track, &zUnused, &direction, false))
        {
            if (track.element->AsTrack()->HasCableLift())
            {
                SetState(Status::waitingForCableLift, sub_state);
            }
        }
    }

    switch (curRide->mode)
    {
        case RideMode::dodgems:
            // Dodgems mode uses sub_state and TimeActive to tell how long
            // the vehicle has been ridden.
            SetState(Status::travellingDodgems);
            TimeActive = 0;
            UpdateDodgemsMode();
            break;
        case RideMode::swing:
            SetState(Status::swinging);
            NumSwings = 0;
            current_time = -1;
            UpdateSwinging();
            break;
        case RideMode::rotation:
            SetState(Status::rotating);
            NumRotations = 0;
            current_time = -1;
            UpdateRotating();
            break;
        case RideMode::filmAvengingAviators:
            SetState(Status::simulatorOperating);
            current_time = -1;
            UpdateSimulatorOperating();
            break;
        case RideMode::filmThrillRiders:
            SetState(Status::simulatorOperating, 1);
            current_time = -1;
            UpdateSimulatorOperating();
            break;
        case RideMode::beginners:
        case RideMode::intense:
        case RideMode::berserk:
            SetState(Status::topSpinOperating, sub_state);
            switch (curRide->mode)
            {
                case RideMode::beginners:
                    sub_state = 0;
                    break;
                case RideMode::intense:
                    sub_state = 1;
                    break;
                case RideMode::berserk:
                    sub_state = 2;
                    break;
                default:
                {
                    // This is workaround for multiple compilation errors of type "enumeration value 'RIDE_MODE_*' not handled
                    // in switch [-Werror=switch]"
                }
            }
            current_time = -1;
            flatRideAnimationFrame = 0;
            flatRideSecondaryAnimationFrame = 0;
            UpdateTopSpinOperating();
            break;
        case RideMode::forwardRotation:
        case RideMode::backwardRotation:
            SetState(Status::ferrisWheelRotating, flatRideAnimationFrame);
            NumRotations = 0;
            ferris_wheel_var_0 = 8;
            ferris_wheel_var_1 = 8;
            UpdateFerrisWheelRotating();
            break;
        case RideMode::mouseTails3DFilm:
        case RideMode::stormChasers3DFilm:
        case RideMode::spaceRaiders3DFilm:
            SetState(Status::showingFilm, sub_state);
            switch (curRide->mode)
            {
                case RideMode::mouseTails3DFilm:
                    sub_state = 0;
                    break;
                case RideMode::stormChasers3DFilm:
                    sub_state = 1;
                    break;
                case RideMode::spaceRaiders3DFilm:
                    sub_state = 2;
                    break;
                default:
                {
                    // This is workaround for multiple compilation errors of type "enumeration value 'RIDE_MODE_*' not handled
                    // in switch [-Werror=switch]"
                }
            }
            current_time = -1;
            UpdateShowingFilm();
            break;
        case RideMode::circus:
            SetState(Status::doingCircusShow);
            current_time = -1;
            UpdateDoingCircusShow();
            break;
        case RideMode::spaceRings:
            SetState(Status::spaceRingsOperating);
            flatRideAnimationFrame = 0;
            current_time = -1;
            UpdateSpaceRingsOperating();
            break;
        case RideMode::hauntedHouse:
            SetState(Status::hauntedHouseOperating);
            flatRideAnimationFrame = 0;
            current_time = -1;
            UpdateHauntedHouseOperating();
            break;
        case RideMode::crookedHouse:
            SetState(Status::crookedHouseOperating);
            flatRideAnimationFrame = 0;
            current_time = -1;
            UpdateCrookedHouseOperating();
            break;
        default:
            SetState(status);
            NumLaps = 0;
            break;
    }
}

/**
 *
 *  rct2: 0x006D9002
 */
void Vehicle::UpdateUnloadingPassengers()
{
    if (sub_state == 0)
    {
        if (OpenRestraints())
        {
            sub_state = 1;
        }
    }

    const auto* curRide = GetRide();
    if (curRide == nullptr)
        return;

    const auto& currentStation = curRide->getStation(current_station);

    if (curRide->mode == RideMode::forwardRotation || curRide->mode == RideMode::backwardRotation)
    {
        uint8_t seat = ((-flatRideAnimationFrame) >> 3) & 0xF;
        if (restraints_position == 255 && !peep[seat * 2].IsNull())
        {
            next_free_seat -= 2;

            auto firstGuest = getGameState().entities.GetEntity<Guest>(peep[seat * 2]);
            peep[seat * 2] = EntityId::GetNull();

            if (firstGuest != nullptr)
            {
                firstGuest->SetState(PeepState::leavingRide);
                firstGuest->RideSubState = PeepRideSubState::leaveVehicle;
            }

            auto secondGuest = getGameState().entities.GetEntity<Guest>(peep[seat * 2 + 1]);
            peep[seat * 2 + 1] = EntityId::GetNull();

            if (secondGuest != nullptr)
            {
                secondGuest->SetState(PeepState::leavingRide);
                secondGuest->RideSubState = PeepRideSubState::leaveVehicle;
            }
        }
    }
    else
    {
        if (currentStation.Exit.IsNull())
        {
            if (sub_state != 1)
                return;

            if (!curRide->flags.has(RideFlag::tested) && flags.has(VehicleFlag::testing)
                && curRide->currentTestSegment + 1 >= curRide->numStations)
            {
                UpdateTestFinish();
            }
            SetState(Status::movingToEndOfStation);
            return;
        }

        for (Vehicle* train = getGameState().entities.GetEntity<Vehicle>(Id); train != nullptr;
             train = getGameState().entities.GetEntity<Vehicle>(train->next_vehicle_on_train))
        {
            if (train->restraints_position != 255)
                continue;

            if (train->next_free_seat == 0)
                continue;

            train->next_free_seat = 0;
            for (uint8_t peepIndex = 0; peepIndex < train->num_peeps; peepIndex++)
            {
                Peep* curPeep = getGameState().entities.GetEntity<Guest>(train->peep[peepIndex]);
                if (curPeep != nullptr)
                {
                    curPeep->SetState(PeepState::leavingRide);
                    curPeep->RideSubState = PeepRideSubState::leaveVehicle;
                }
            }
        }
    }

    if (sub_state != 1)
        return;

    for (Vehicle* train = getGameState().entities.GetEntity<Vehicle>(Id); train != nullptr;
         train = getGameState().entities.GetEntity<Vehicle>(train->next_vehicle_on_train))
    {
        if (train->num_peeps != train->next_free_seat)
            return;
    }

    if (!curRide->flags.has(RideFlag::tested) && flags.has(VehicleFlag::testing)
        && curRide->currentTestSegment + 1 >= curRide->numStations)
    {
        UpdateTestFinish();
    }
    SetState(Status::movingToEndOfStation);
}

/**
 *
 *  rct2: 0x006D845B
 */
void Vehicle::UpdateDeparting()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    const auto* rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
        return;

    if (sub_state == 0)
    {
        if (flags.has(VehicleFlag::trainIsBroken))
        {
            if (curRide->flags.has(RideFlag::brokenDown))
                return;

            curRide->flags.set(RideFlag::brokenDown);
            RideBreakdownAddNewsItem(*curRide);

            curRide->windowInvalidateFlags.set(
                RideInvalidateFlag::main, RideInvalidateFlag::list, RideInvalidateFlag::maintenance);
            curRide->mechanicStatus = MechanicStatus::calling;
            curRide->inspectionStation = current_station;
            curRide->breakdownReason = curRide->breakdownReasonPending;
            velocity = 0;
            return;
        }

        sub_state = 1;
        PeepEasterEggHereWeAre();

        if (rideEntry->flags.has(RideEntryFlag::playDepartSound))
        {
            auto soundId = (rideEntry->Cars[0].soundRange == SoundRange::tramBell) ? SoundId::tram : SoundId::trainDeparting;
            Play3D(soundId, GetLocation());
        }

        if (curRide->mode == RideMode::upwardLaunch || (curRide->mode == RideMode::downwardLaunch && NumLaunches > 1))
        {
            Play3D(SoundId::rideLaunch2, GetLocation());
        }

        if (!curRide->flags.has(RideFlag::tested))
        {
            if (flags.has(VehicleFlag::testing))
            {
                if (curRide->currentTestSegment + 1 < curRide->numStations)
                {
                    curRide->currentTestSegment++;
                    curRide->currentTestStation = current_station;
                }
                else
                {
                    UpdateTestFinish();
                }
            }
            else if (!curRide->flags.has(RideFlag::testInProgress) && !IsGhost())
            {
                TestReset();
            }
        }
    }

    const auto& carEntry = rideEntry->Cars[vehicle_type];
    const auto& rtd = curRide->getRideTypeDescriptor();
    switch (curRide->mode)
    {
        case RideMode::reverseInclineLaunchedShuttle:
            if (velocity >= -131940)
                acceleration = -3298;
            break;
        case RideMode::poweredLaunchPasstrough:
        case RideMode::poweredLaunch:
        case RideMode::poweredLaunchBlockSectioned:
        case RideMode::limPoweredLaunch:
        case RideMode::upwardLaunch:
            if ((curRide->launchSpeed << 16) > velocity)
            {
                acceleration = curRide->launchSpeed << rtd.BoosterSettings.AccelerationFactor;
            }
            break;
        case RideMode::downwardLaunch:
            if (NumLaunches >= 1)
            {
                if ((14 << 16) > velocity)
                    acceleration = 14 << 12;
                break;
            }
            [[fallthrough]];
        case RideMode::continuousCircuit:
        case RideMode::continuousCircuitBlockSectioned:
        case RideMode::rotatingLift:
        case RideMode::freefallDrop:
        case RideMode::boatHire:
            if (carEntry.flags.has(CarEntryFlag::isPowered))
                break;

            if (velocity <= 131940)
                acceleration = 3298;
            break;
        default:
        {
            // This is workaround for multiple compilation errors of type "enumeration value ‘RIDE_MODE_*' not handled
            // in switch [-Werror=switch]"
        }
    }

    uint32_t curFlags = UpdateTrackMotion(nullptr);

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_8)
    {
        if (curRide->mode == RideMode::reverseInclineLaunchedShuttle)
        {
            velocity = -velocity;
            FinishDeparting();
            return;
        }
    }

    if (curFlags & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_5 | VEHICLE_UPDATE_MOTION_TRACK_FLAG_12))
    {
        if (curRide->mode == RideMode::boatHire)
        {
            UpdateDepartingBoatHire();
            return;
        }
        if (curRide->mode == RideMode::reverseInclineLaunchedShuttle)
        {
            velocity = -velocity;
            FinishDeparting();
            return;
        }
        if (curRide->mode == RideMode::shuttle)
        {
            flags.flip(VehicleFlag::poweredCarInReverse);
            velocity = 0;

            // We have turned, so treat it like entering a new tile
            UpdateCrossings();
        }
    }

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL)
    {
        sound2_flags |= VEHICLE_SOUND2_FLAGS_LIFT_HILL;
        if (curRide->mode != RideMode::reverseInclineLaunchedShuttle)
        {
            int32_t curSpeed = curRide->liftHillSpeed * 31079;
            if (velocity <= curSpeed)
            {
                acceleration = 15539;
                if (velocity != 0)
                {
                    if (_vehicleBreakdown == BREAKDOWN_SAFETY_CUT_OUT)
                    {
                        flags.set(VehicleFlag::stoppedBySafetyCutout);
                        flags.unset(VehicleFlag::collisionDisabled);
                    }
                }
                else
                    flags.unset(VehicleFlag::collisionDisabled);
            }
        }
        else
        {
            int32_t curSpeed = curRide->liftHillSpeed * -31079;
            if (velocity >= curSpeed)
            {
                acceleration = -15539;
                if (velocity != 0)
                {
                    if (_vehicleBreakdown == BREAKDOWN_SAFETY_CUT_OUT)
                    {
                        flags.set(VehicleFlag::stoppedBySafetyCutout);
                        flags.unset(VehicleFlag::collisionDisabled);
                    }
                }
                else
                    flags.unset(VehicleFlag::collisionDisabled);
            }
        }
    }

    if (curRide->mode == RideMode::freefallDrop)
    {
        animation_frame++;
    }
    else
    {
        bool shouldLaunch = true;
        if (curRide->mode == RideMode::downwardLaunch)
        {
            if (NumLaunches < 1)
                shouldLaunch = false;
        }

        if (shouldLaunch)
        {
            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_3) || _vehicleStationIndex != current_station)
            {
                FinishDeparting();
                return;
            }

            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_5))
                return;
            if (curRide->mode == RideMode::boatHire || curRide->mode == RideMode::rotatingLift
                || curRide->mode == RideMode::shuttle)
                return;

            UpdateCrashSetup();
            return;
        }
    }

    if (!CurrentTowerElementIsTop())
    {
        if (curRide->mode == RideMode::freefallDrop)
            Invalidate();
        return;
    }

    FinishDeparting();
}

/**
 * Part of UpdateDeparting
 * Called after finishing departing sequence to enter
 * traveling state.
 * Vertical rides class the lift to the top of the tower
 * as the departing sequence. After this point station
 * boosters do not affect the ride.
 *  rct2: 0x006D8858
 */
void Vehicle::FinishDeparting()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->mode == RideMode::downwardLaunch)
    {
        if (NumLaunches >= 1 && (14 << 16) > velocity)
            return;

        Play3D(SoundId::rideLaunch1, GetLocation());
    }

    if (curRide->mode == RideMode::upwardLaunch)
    {
        if ((curRide->launchSpeed << 16) > velocity)
            return;

        Play3D(SoundId::rideLaunch1, GetLocation());
    }

    if (curRide->mode != RideMode::race && !curRide->isBlockSectioned())
    {
        auto& currentStation = curRide->getStation(current_station);
        currentStation.Depart &= kStationDepartFlag;
        uint8_t waitingTime = 3;
        if (curRide->departFlags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH)
        {
            waitingTime = std::max(curRide->minWaitingTime, static_cast<uint8_t>(3));
            waitingTime = std::min(waitingTime, static_cast<uint8_t>(127));
        }

        currentStation.Depart |= waitingTime;
    }
    lost_time_out = 0;
    SetState(Status::travelling, 1);
    if (velocity < 0)
        sub_state = 0;
}

/**
 *
 *  rct2: 0x006DE5CB
 */
void Vehicle::CheckIfMissing()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->flags.hasAny(RideFlag::brokenDown, RideFlag::crashed))
        return;

    if (curRide->isBlockSectioned())
        return;

    if (!curRide->getRideTypeDescriptor().flags.has(RtdFlag::checkForStalling))
        return;

    lost_time_out++;
    if (curRide->flags.has(RideFlag::hasStalledVehicle))
        return;

    uint16_t limit = curRide->type == RIDE_TYPE_BOAT_HIRE ? 15360 : 9600;

    if (lost_time_out <= limit)
        return;

    curRide->flags.set(RideFlag::hasStalledVehicle);

    if (Config::Get().notifications.rideStalledVehicles)
    {
        Formatter ft;
        ft.Add<StringId>(GetRideComponentName(GetRideTypeDescriptor(curRide->type).NameConvention.vehicle).number);

        uint8_t vehicleIndex = 0;
        for (; vehicleIndex < curRide->numTrains; ++vehicleIndex)
            if (curRide->vehicles[vehicleIndex] == Id)
                break;

        vehicleIndex++;
        ft.Add<uint16_t>(vehicleIndex);
        curRide->formatNameTo(ft);
        ft.Add<StringId>(GetRideComponentName(GetRideTypeDescriptor(curRide->type).NameConvention.station).singular);

        News::AddItemToQueue(News::ItemType::ride, STR_NEWS_VEHICLE_HAS_STALLED, ride.ToUnderlying(), ft);
    }
}

/**
 *
 *  rct2: 0x006D8937
 */
void Vehicle::UpdateTravelling()
{
    CheckIfMissing();

    auto curRide = GetRide();
    if (curRide == nullptr || (_vehicleBreakdown == 0 && curRide->mode == RideMode::rotatingLift))
        return;

    if (sub_state == 2)
    {
        velocity = 0;
        acceleration = 0;
        var_C0--;
        if (var_C0 == 0)
            sub_state = 0;
    }

    if (curRide->mode == RideMode::freefallDrop && animation_frame != 0)
    {
        animation_frame++;
        velocity = 0;
        acceleration = 0;
        Invalidate();
        return;
    }

    uint32_t curFlags = UpdateTrackMotion(nullptr);

    bool skipCheck = false;
    if (curFlags & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_8 | VEHICLE_UPDATE_MOTION_TRACK_FLAG_9)
        && curRide->mode == RideMode::reverseInclineLaunchedShuttle && sub_state == 0)
    {
        sub_state = 1;
        velocity = 0;
        skipCheck = true;
    }

    if (!skipCheck)
    {
        if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_DERAILED)
        {
            UpdateCrashSetup();
            return;
        }

        if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION)
        {
            UpdateCollisionSetup();
            return;
        }

        if (curFlags & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_5 | VEHICLE_UPDATE_MOTION_TRACK_FLAG_12))
        {
            if (curRide->mode == RideMode::rotatingLift)
            {
                if (sub_state <= 1)
                {
                    SetState(Status::arriving, 1);
                    var_C0 = 0;
                    return;
                }
            }
            else if (curRide->mode == RideMode::boatHire)
            {
                UpdateTravellingBoatHireSetup();
                return;
            }
            if (curRide->mode == RideMode::shuttle)
            {
                flags.flip(VehicleFlag::poweredCarInReverse);
                velocity = 0;
            }
            else
            {
                if (sub_state != 0)
                {
                    UpdateCrashSetup();
                    return;
                }
                sub_state = 1;
                velocity = 0;
            }
        }
    }

    if (curRide->mode == RideMode::rotatingLift && sub_state <= 1)
    {
        if (sub_state == 0)
        {
            if (velocity >= -131940)
                acceleration = -3298;
            velocity = std::max(velocity, -131940);
        }
        else
        {
            if (CurrentTowerElementIsTop())
            {
                velocity = 0;
                sub_state = 2;
                var_C0 = 150;
            }
            else
            {
                if (velocity <= 131940)
                    acceleration = 3298;
            }
        }
    }

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL)
    {
        if (curRide->mode == RideMode::reverseInclineLaunchedShuttle)
        {
            if (sub_state == 0)
            {
                if (velocity != 0)
                    sound2_flags |= VEHICLE_SOUND2_FLAGS_LIFT_HILL;

                if (!flags.has(VehicleFlag::reverseInclineCompletedLap))
                {
                    if (velocity >= curRide->liftHillSpeed * -31079)
                    {
                        acceleration = -15539;

                        if (_vehicleBreakdown == 0)
                        {
                            sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
                            flags.set(VehicleFlag::stoppedBySafetyCutout);
                        }
                    }
                }
            }
        }
        else
        {
            sound2_flags |= VEHICLE_SOUND2_FLAGS_LIFT_HILL;
            if (velocity <= curRide->liftHillSpeed * 31079)
            {
                acceleration = 15539;
                if (velocity != 0)
                {
                    if (_vehicleBreakdown == 0)
                    {
                        flags.set(VehicleFlag::stoppedBySafetyCutout);
                        sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
                    }
                }
                else
                {
                    sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
                }
            }
        }
    }

    if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_3))
        return;

    if (curRide->mode == RideMode::reverseInclineLaunchedShuttle && velocity >= 0
        && !flags.has(VehicleFlag::reverseInclineCompletedLap))
    {
        return;
    }

    if (curRide->mode == RideMode::poweredLaunchPasstrough && velocity < 0)
        return;

    SetState(Status::arriving);
    current_station = _vehicleStationIndex;
    var_C0 = 0;
    if (velocity < 0)
        sub_state = 1;
}

void Vehicle::UpdateArrivingPassThroughStation(const Ride& curRide, const CarEntry& carEntry, bool stationBrakesWork)
{
    if (sub_state == 0)
    {
        if (curRide.mode == RideMode::race && curRide.flags.has(RideFlag::passStationNoStopping))
        {
            return;
        }

        if (velocity <= 131940)
        {
            acceleration = 3298;
            return;
        }

        int32_t velocity_diff = velocity;
        if (velocity_diff >= 24.0_mph)
            velocity_diff /= 8;
        else
            velocity_diff /= 16;

        if (!stationBrakesWork)
        {
            return;
        }

        if (curRide.numCircuits != 1)
        {
            if (NumLaps + 1 < curRide.numCircuits)
            {
                return;
            }
        }
        velocity -= velocity_diff;
        acceleration = 0;
    }
    else
    {
        if (!carEntry.flags.has(CarEntryFlag::isPowered) && velocity >= -131940)
        {
            acceleration = -3298;
        }

        if (velocity >= -131940)
        {
            return;
        }

        int32_t velocity_diff = velocity;
        if (velocity_diff < -24.0_mph)
            velocity_diff /= 8;
        else
            velocity_diff /= 16;

        if (!stationBrakesWork)
        {
            return;
        }

        if (NumLaps + 1 < curRide.numCircuits)
        {
            return;
        }

        if (NumLaps + 1 != curRide.numCircuits)
        {
            velocity -= velocity_diff;
            acceleration = 0;
            return;
        }

        if (GetRideTypeDescriptor(curRide.type).flags.has(RtdFlag::allowMultipleCircuits) && curRide.mode != RideMode::shuttle
            && curRide.mode != RideMode::poweredLaunch)
        {
            flags.set(VehicleFlag::reverseInclineCompletedLap);
        }
        else
        {
            velocity -= velocity_diff;
            acceleration = 0;
        }
    }
}

/**
 *
 *  rct2: 0x006D8C36
 */
void Vehicle::UpdateArriving()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    uint32_t curFlags = 0;

    switch (curRide->mode)
    {
        case RideMode::swing:
        case RideMode::rotation:
        case RideMode::forwardRotation:
        case RideMode::backwardRotation:
        case RideMode::filmAvengingAviators:
        case RideMode::filmThrillRiders:
        case RideMode::beginners:
        case RideMode::intense:
        case RideMode::berserk:
        case RideMode::mouseTails3DFilm:
        case RideMode::stormChasers3DFilm:
        case RideMode::spaceRaiders3DFilm:
        case RideMode::circus:
        case RideMode::spaceRings:
        case RideMode::hauntedHouse:
        case RideMode::crookedHouse:
            flags.unset(VehicleFlag::reverseInclineCompletedLap);
            velocity = 0;
            acceleration = 0;
            SetState(Status::unloadingPassengers);
            return;
        default:
        {
            // This is workaround for multiple compilation errors of type "enumeration value ‘RIDE_MODE_*' not handled
            // in switch [-Werror=switch]"
        }
    }

    auto stationBrakesWork = !curRide->hasFailingBrakes();

    const auto* rideEntry = GetRideEntry();
    const auto& carEntry = rideEntry->Cars[vehicle_type];

    UpdateArrivingPassThroughStation(*curRide, carEntry, stationBrakesWork);

    curFlags = UpdateTrackMotion(nullptr);
    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION && !stationBrakesWork)
    {
        UpdateCollisionSetup();
        return;
    }

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION && !stationBrakesWork)
    {
        SetState(Status::departing, 1);
        return;
    }

    if (!(curFlags
          & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION | VEHICLE_UPDATE_MOTION_TRACK_FLAG_1
             | VEHICLE_UPDATE_MOTION_TRACK_FLAG_5)))
    {
        if (velocity > 98955)
            var_C0 = 0;
        return;
    }

    var_C0++;
    if ((curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_1) && carEntry.flags.has(CarEntryFlag::isGoKart) && (var_C0 < 40))
    {
        return;
    }

    auto trackElement = MapGetTrackElementAt(TrackLocation);

    if (trackElement == nullptr)
    {
        return;
    }

    current_station = trackElement->GetStationIndex();
    NumLaps++;

    if (sub_state != 0)
    {
        if (NumLaps < curRide->numCircuits)
        {
            SetState(Status::departing, 1);
            return;
        }

        if (NumLaps == curRide->numCircuits && flags.has(VehicleFlag::reverseInclineCompletedLap))
        {
            SetState(Status::departing, 1);
            return;
        }
    }

    if (curRide->numCircuits != 1 && NumLaps < curRide->numCircuits)
    {
        SetState(Status::departing, 1);
        return;
    }

    if ((curRide->mode == RideMode::upwardLaunch || curRide->mode == RideMode::downwardLaunch) && NumLaunches < 2)
    {
        Play3D(SoundId::rideLaunch2, GetLocation());
        velocity = 0;
        acceleration = 0;
        SetState(Status::departing, 1);
        return;
    }

    if (curRide->mode == RideMode::race && curRide->flags.has(RideFlag::passStationNoStopping))
    {
        SetState(Status::departing, 1);
        return;
    }

    flags.unset(VehicleFlag::reverseInclineCompletedLap);
    velocity = 0;
    acceleration = 0;
    SetState(Status::unloadingPassengers);
}
