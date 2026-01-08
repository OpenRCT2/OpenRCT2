/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Station.h"

#include "../Game.h"
#include "../GameState.h"
#include "../entity/Guest.h"
#include "../scenario/Scenario.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "RideEntry.h"
#include "Track.h"
#include "Vehicle.h"

using namespace OpenRCT2;

static void RideUpdateStationBlockSection(Ride& ride, StationIndex stationIndex);
static void RideUpdateStationDodgems(Ride& ride, StationIndex stationIndex);
static void RideUpdateStationNormal(Ride& ride, StationIndex stationIndex);
static void RideUpdateStationRace(Ride& ride, StationIndex stationIndex);
static void RideRaceInitVehicleSpeeds(const Ride& ride);
static void RideInvalidateStationStart(Ride& ride, StationIndex stationIndex, bool greenLight);

/**
 *
 *  rct2: 0x006ABFFB
 */
void RideUpdateStation(Ride& ride, StationIndex stationIndex)
{
    if (ride.getStation(stationIndex).Start.IsNull())
        return;

    switch (ride.mode)
    {
        case RideMode::race:
            RideUpdateStationRace(ride, stationIndex);
            break;
        case RideMode::dodgems:
            RideUpdateStationDodgems(ride, stationIndex);
            break;
        case RideMode::continuousCircuitBlockSectioned:
        case RideMode::poweredLaunchBlockSectioned:
            RideUpdateStationBlockSection(ride, stationIndex);
            break;
        default:
            RideUpdateStationNormal(ride, stationIndex);
            break;
    }
}

/**
 *
 *  rct2: 0x006AC0A1
 */
static void RideUpdateStationBlockSection(Ride& ride, StationIndex stationIndex)
{
    TileElement* tileElement = RideGetStationStartTrackElement(ride, stationIndex);
    auto& station = ride.getStation(stationIndex);

    if ((ride.status == RideStatus::closed && ride.numRiders == 0)
        || (tileElement != nullptr && tileElement->AsTrack()->IsBrakeClosed()))
    {
        station.Depart &= ~kStationDepartFlag;

        if ((station.Depart & kStationDepartFlag) || (tileElement != nullptr && tileElement->AsTrack()->HasGreenLight()))
            RideInvalidateStationStart(ride, stationIndex, false);
    }
    else
    {
        if (!(station.Depart & kStationDepartFlag))
        {
            station.Depart |= kStationDepartFlag;
            RideInvalidateStationStart(ride, stationIndex, true);
        }
        else if (tileElement != nullptr && tileElement->AsTrack()->HasGreenLight())
        {
            RideInvalidateStationStart(ride, stationIndex, true);
        }
    }
}

/**
 *
 *  rct2: 0x006AC12B
 */
static void RideUpdateStationDodgems(Ride& ride, StationIndex stationIndex)
{
    auto& station = ride.getStation(stationIndex);

    // Change of station depart flag should really call invalidate_station_start
    // but since dodgems do not have station lights there is no point.
    if (ride.status == RideStatus::closed || (ride.lifecycleFlags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)))
    {
        station.Depart &= ~kStationDepartFlag;
        return;
    }

    if (ride.lifecycleFlags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
    {
        int32_t dx = ride.timeLimit * 32;
        int32_t dh = (dx >> 8) & 0xFF;
        for (size_t i = 0; i < ride.numTrains; i++)
        {
            Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[i]);
            if (vehicle == nullptr)
                continue;

            if (vehicle->NumLaps < dh)
                continue;

            // End match
            ride.lifecycleFlags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
            station.Depart &= ~kStationDepartFlag;
            return;
        }

        // Continue match
        station.Depart |= kStationDepartFlag;
    }
    else
    {
        // Check if all vehicles are ready to go
        for (size_t i = 0; i < ride.numTrains; i++)
        {
            Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[i]);
            if (vehicle == nullptr)
                continue;

            if (vehicle->status != Vehicle::Status::waitingToDepart)
            {
                station.Depart &= ~kStationDepartFlag;
                return;
            }
        }

        // Begin the match
        ride.lifecycleFlags |= RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
        station.Depart |= kStationDepartFlag;
        ride.windowInvalidateFlags.set(RideInvalidateFlag::main, RideInvalidateFlag::list);
    }
}

/**
 *
 *  rct2: 0x006AC02C
 */
static void RideUpdateStationNormal(Ride& ride, StationIndex stationIndex)
{
    auto& station = ride.getStation(stationIndex);
    int32_t time = station.Depart & kStationDepartMask;
    const auto currentTicks = getGameState().currentTicks;
    if ((ride.lifecycleFlags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        || (ride.status == RideStatus::closed && ride.numRiders == 0))
    {
        if (time != 0 && time != 127 && !(currentTicks & 7))
            time--;

        station.Depart = time;
        RideInvalidateStationStart(ride, stationIndex, false);
    }
    else
    {
        if (time == 0)
        {
            station.Depart |= kStationDepartFlag;
            RideInvalidateStationStart(ride, stationIndex, true);
        }
        else
        {
            if (time != 127 && !(currentTicks & 31))
                time--;

            station.Depart = time;
            RideInvalidateStationStart(ride, stationIndex, false);
        }
    }
}

/**
 *
 *  rct2: 0x006AC1DF
 */
static void RideUpdateStationRace(Ride& ride, StationIndex stationIndex)
{
    auto& station = ride.getStation(stationIndex);
    if (ride.status == RideStatus::closed || (ride.lifecycleFlags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)))
    {
        if (station.Depart & kStationDepartFlag)
        {
            station.Depart &= ~kStationDepartFlag;
            RideInvalidateStationStart(ride, stationIndex, false);
        }
        return;
    }

    if (ride.lifecycleFlags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
    {
        int32_t numLaps = ride.numLaps;

        for (size_t i = 0; i < ride.numTrains; i++)
        {
            Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[i]);
            if (vehicle == nullptr)
                continue;

            if (vehicle->status != Vehicle::Status::waitingToDepart && vehicle->NumLaps >= numLaps)
            {
                // Found a winner
                if (vehicle->num_peeps != 0)
                {
                    auto* peep = getGameState().entities.GetEntity<Guest>(vehicle->peep[0]);
                    if (peep != nullptr)
                    {
                        ride.raceWinner = peep->Id;
                        ride.windowInvalidateFlags.set(RideInvalidateFlag::main, RideInvalidateFlag::list);
                    }
                }

                // Race is over
                ride.lifecycleFlags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
                if (station.Depart & kStationDepartFlag)
                {
                    station.Depart &= ~kStationDepartFlag;
                    RideInvalidateStationStart(ride, stationIndex, false);
                }
                return;
            }
        }

        // Continue racing
        station.Depart |= kStationDepartFlag;
    }
    else
    {
        // Check if all vehicles are ready to go
        for (size_t i = 0; i < ride.numTrains; i++)
        {
            Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[i]);
            if (vehicle == nullptr)
                continue;

            if (vehicle->status != Vehicle::Status::waitingToDepart && vehicle->status != Vehicle::Status::departing)
            {
                if (station.Depart & kStationDepartFlag)
                {
                    station.Depart &= ~kStationDepartFlag;
                    RideInvalidateStationStart(ride, stationIndex, false);
                }
                return;
            }
        }

        // Begin the race
        RideRaceInitVehicleSpeeds(ride);
        ride.lifecycleFlags |= RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
        if (!(station.Depart & kStationDepartFlag))
        {
            station.Depart |= kStationDepartFlag;
            RideInvalidateStationStart(ride, stationIndex, true);
        }
        ride.windowInvalidateFlags.set(RideInvalidateFlag::main, RideInvalidateFlag::list);
    }
}

/**
 *
 *  rct2: 0x006AC988
 * set the speed of the go kart type vehicle at the start to a random value or alter if peep name is an easter egg
 * @param ride (esi)
 */
static void RideRaceInitVehicleSpeeds(const Ride& ride)
{
    for (size_t i = 0; i < ride.numTrains; i++)
    {
        Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[i]);
        if (vehicle == nullptr)
            continue;

        vehicle->ClearFlag(VehicleFlags::CurrentlyColliding);

        const auto* rideEntry = vehicle->GetRideEntry();

        vehicle->speed = (ScenarioRand() & 15) - 8 + rideEntry->Cars[vehicle->vehicle_type].powered_max_speed;

        if (vehicle->num_peeps != 0)
        {
            auto* guest = getGameState().entities.GetEntity<Guest>(vehicle->peep[0]);

            // Easter egg names should only work on guests
            if (guest != nullptr)
            {
                switch (guest->GetEasterEggNameId())
                {
                    case EASTEREGG_PEEP_NAME_MICHAEL_SCHUMACHER:
                        vehicle->speed += 35;
                        break;
                    case EASTEREGG_PEEP_NAME_JACQUES_VILLENEUVE:
                        vehicle->speed += 25;
                        break;
                    case EASTEREGG_PEEP_NAME_DAMON_HILL:
                        vehicle->speed += 55;
                        break;
                    case EASTEREGG_PEEP_NAME_CHRIS_SAWYER:
                        vehicle->speed += 14;
                        break;
                    case EASTEREGG_PEEP_NAME_MR_BEAN:
                        vehicle->speed = 9;
                        break;
                }
            }
        }
    }
}

/**
 *
 *  rct2: 0x006AC2C7
 */
static void RideInvalidateStationStart(Ride& ride, StationIndex stationIndex, bool greenLight)
{
    auto startPos = ride.getStation(stationIndex).Start;
    TileElement* tileElement = RideGetStationStartTrackElement(ride, stationIndex);

    // If no station track found return
    if (tileElement == nullptr)
        return;

    TrackElement* const trackElement = tileElement->AsTrack();
    if (trackElement->HasGreenLight() != greenLight)
    {
        trackElement->SetHasGreenLight(greenLight);
        MapInvalidateTileZoom1({ startPos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
    }
}

TileElement* RideGetStationStartTrackElement(const Ride& ride, StationIndex stationIndex)
{
    auto stationStart = ride.getStation(stationIndex).GetStart();

    // Find the station track element
    TileElement* tileElement = MapGetFirstElementAt(stationStart);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() == TileElementType::Track && stationStart.z == tileElement->GetBaseZ())
            return tileElement;

    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

TileElement* RideGetStationExitElement(const CoordsXYZ& elementPos)
{
    // Find the station track element
    TileElement* tileElement = MapGetFirstElementAt(elementPos);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement == nullptr)
            break;
        if (tileElement->GetType() == TileElementType::Entrance && elementPos.z == tileElement->GetBaseZ())
            return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

StationIndex RideGetFirstValidStationExit(const Ride& ride)
{
    for (const auto& station : ride.getStations())
    {
        if (!station.Exit.IsNull())
        {
            return ride.getStationIndex(&station);
        }
    }
    return StationIndex::GetNull();
}

StationIndex RideGetFirstValidStationStart(const Ride& ride)
{
    for (const auto& station : ride.getStations())
    {
        if (!station.Start.IsNull())
        {
            return ride.getStationIndex(&station);
        }
    }
    return StationIndex::GetNull();
}

StationIndex RideGetFirstEmptyStationStart(const Ride& ride)
{
    for (const auto& station : ride.getStations())
    {
        if (station.Start.IsNull())
        {
            return ride.getStationIndex(&station);
        }
    }
    return StationIndex::GetNull();
}

int32_t RideStation::GetBaseZ() const
{
    return Height * kCoordsZStep;
}

void RideStation::SetBaseZ(int32_t newZ)
{
    Height = newZ / kCoordsZStep;
}

CoordsXYZ RideStation::GetStart() const
{
    return { Start, GetBaseZ() };
}
