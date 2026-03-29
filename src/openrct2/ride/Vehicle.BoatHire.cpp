/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Vehicle.h"

#include "../scenario/Scenario.h"
#include "../world/Map.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Ride.h"
#include "VehicleGeometry.h"

using namespace OpenRCT2;
using namespace OpenRCT2::RideVehicle;

static bool vehicle_boat_is_location_accessible(const CoordsXYZ& location);

void Vehicle::UpdateTravellingBoatHireSetup()
{
    var_34 = Orientation;
    TrackLocation.x = x;
    TrackLocation.y = y;
    TrackLocation = TrackLocation.ToTileStart();

    CoordsXY location = CoordsXY(TrackLocation) + CoordsDirectionDelta[Orientation >> 3];

    BoatLocation = location;
    var_35 = 0;
    // No longer on a track so reset to 0 for import/export
    SetTrackDirection(0);
    SetTrackType(TrackElemType::flat);
    SetState(Status::travellingBoat);
    remaining_distance += 27924;

    UpdateTravellingBoat();
}

/**
 *
 *  rct2: 0x006D982F
 */
void Vehicle::UpdateDepartingBoatHire()
{
    lost_time_out = 0;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    auto& station = curRide->getStation(current_station);
    station.Depart &= kStationDepartFlag;
    uint8_t waitingTime = std::max(curRide->minWaitingTime, static_cast<uint8_t>(3));
    waitingTime = std::min(waitingTime, static_cast<uint8_t>(127));
    station.Depart |= waitingTime;
    UpdateTravellingBoatHireSetup();
}

/**
 *
 *  rct2: 0x006D845B
 */
void Vehicle::UpdateTravellingBoat()
{
    CheckIfMissing();
    UpdateMotionBoatHire();
}

void Vehicle::TryReconnectBoatToTrack(const CoordsXY& currentBoatLocation, const CoordsXY& trackCoords)
{
    remaining_distance = 0;
    if (!UpdateMotionCollisionDetection({ currentBoatLocation, z }, nullptr))
    {
        TrackLocation.x = trackCoords.x;
        TrackLocation.y = trackCoords.y;

        auto curRide = GetRide();
        if (curRide != nullptr)
        {
            auto trackElement = MapGetTrackElementAt(TrackLocation);
            if (trackElement != nullptr)
                SetTrackType(trackElement->GetTrackType());

            SetTrackDirection(curRide->boatHireReturnDirection);
            BoatLocation.SetNull();
        }

        track_progress = 0;
        SetState(Status::travelling, sub_state);
        _vehicleCurPosition.x = currentBoatLocation.x;
        _vehicleCurPosition.y = currentBoatLocation.y;
    }
}

/**
 *
 *  rct2: 0x006DA717
 */
void Vehicle::UpdateMotionBoatHire()
{
    _vehicleMotionTrackFlags = 0;
    velocity += acceleration;
    _vehicleVelocityF64E08 = velocity;
    _vehicleVelocityF64E0C = (velocity >> 10) * 42;

    auto carEntry = Entry();
    if (carEntry == nullptr)
    {
        return;
    }
    if (carEntry->flags.hasAny(CarEntryFlag::hasVehicleAnimation, CarEntryFlag::hasRiderAnimation))
    {
        UpdateAdditionalAnimation();
    }

    _vehicleUnkF64E10 = 1;
    acceleration = 0;
    remaining_distance += _vehicleVelocityF64E0C;
    if (remaining_distance >= 0x368A)
    {
        sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
        _vehicleCurPosition = GetLocation();
        Invalidate();

        for (;;)
        {
            // Loc6DA7A5
            var_35++;
            auto loc = BoatLocation.ToTileCentre();
            CoordsXY loc2 = loc;
            uint8_t bl;

            loc2.x -= x;
            if (loc2.x >= 0)
            {
                loc2.y -= y;
                if (loc2.y < 0)
                {
                    // Loc6DA81A:
                    loc2.y = -loc2.y;
                    bl = 24;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 16;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 20;
                        }
                    }
                }
                else
                {
                    bl = 8;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 16;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 12;
                        }
                    }
                }
            }
            else
            {
                loc2.y -= y;
                if (loc2.y < 0)
                {
                    // Loc6DA83D:
                    loc2.x = -loc2.x;
                    loc2.y = -loc2.y;
                    bl = 24;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 0;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 28;
                        }
                    }
                }
                else
                {
                    loc2.x = -loc2.x;
                    bl = 8;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 0;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 4;
                        }
                    }
                }
            }

            // Loc6DA861:
            var_34 = bl;
            loc2.x += loc2.y;
            if (loc2.x <= 12)
            {
                UpdateBoatLocation();
            }

            if (!(var_35 & (1 << 0)))
            {
                uint8_t spriteDirection = Orientation;
                if (spriteDirection != var_34)
                {
                    uint8_t dl = (var_34 + 16 - spriteDirection) & 0x1E;
                    if (dl >= 16)
                    {
                        spriteDirection += 2;
                        if (dl > 24)
                        {
                            var_35--;
                        }
                    }
                    else
                    {
                        spriteDirection -= 2;
                        if (dl < 8)
                        {
                            var_35--;
                        }
                    }

                    Orientation = spriteDirection & 0x1E;
                }
            }

            int32_t edi = (Orientation | (var_35 & 1)) & 0x1F;
            loc2 = { x + Geometry::getFreeroamVehicleMovementData(edi).x, y + Geometry::getFreeroamVehicleMovementData(edi).y };
            if (UpdateMotionCollisionDetection({ loc2, z }, nullptr))
            {
                remaining_distance = 0;
                if (Orientation == var_34)
                {
                    Orientation ^= (1 << 4);
                    UpdateBoatLocation();
                    Orientation ^= (1 << 4);
                }
                break;
            }

            auto flooredLocation = loc2.ToTileStart();
            if (flooredLocation != TrackLocation)
            {
                if (!vehicle_boat_is_location_accessible({ loc2, TrackLocation.z }))
                {
                    // Loc6DA939:
                    auto curRide = GetRide();
                    if (curRide == nullptr)
                        return;

                    bool do_Loc6DAA97 = false;
                    if (sub_state != BoatHireSubState::EnteringReturnPosition)
                    {
                        do_Loc6DAA97 = true;
                    }
                    else
                    {
                        auto flooredTileLoc = TileCoordsXY(flooredLocation);
                        if (curRide->boatHireReturnPosition != flooredTileLoc)
                        {
                            do_Loc6DAA97 = true;
                        }
                    }

                    // Loc6DAA97:
                    if (do_Loc6DAA97)
                    {
                        remaining_distance = 0;
                        if (Orientation == var_34)
                        {
                            UpdateBoatLocation();
                        }
                        break;
                    }

                    if (!(curRide->boatHireReturnDirection & 1))
                    {
                        uint16_t tilePart = loc2.y % kCoordsXYStep;
                        if (tilePart == kCoordsXYHalfTile)
                        {
                            TryReconnectBoatToTrack(loc2, flooredLocation);
                            break;
                        }
                        loc2 = _vehicleCurPosition;
                        if (tilePart <= kCoordsXYHalfTile)
                        {
                            loc2.y += 1;
                        }
                        else
                        {
                            loc2.y -= 1;
                        }
                    }
                    else
                    {
                        // Loc6DA9A2:
                        uint16_t tilePart = loc2.x % kCoordsXYStep;
                        if (tilePart == kCoordsXYHalfTile)
                        {
                            TryReconnectBoatToTrack(loc2, flooredLocation);
                            break;
                        }
                        loc2 = _vehicleCurPosition;
                        if (tilePart <= kCoordsXYHalfTile)
                        {
                            loc2.x += 1;
                        }
                        else
                        {
                            loc2.x -= 1;
                        }
                    }

                    // Loc6DA9D1:
                    remaining_distance = 0;
                    if (!UpdateMotionCollisionDetection({ loc2, z }, nullptr))
                    {
                        _vehicleCurPosition.x = loc2.x;
                        _vehicleCurPosition.y = loc2.y;
                    }
                    break;
                }
                TrackLocation = { flooredLocation, TrackLocation.z };
            }

            remaining_distance -= Geometry::getFreeroamVehicleMovementData(edi).distance;
            _vehicleCurPosition.x = loc2.x;
            _vehicleCurPosition.y = loc2.y;
            if (remaining_distance < 0x368A)
            {
                break;
            }
            _vehicleUnkF64E10++;
        }

        MoveTo(_vehicleCurPosition);
    }

    // Loc6DAAC9:
    {
        int32_t edx = velocity >> 8;
        edx = (edx * edx);
        if (velocity < 0)
        {
            edx = -edx;
        }
        edx >>= 5;

        // Hack to fix people messing with boat hire
        int32_t curMass = mass == 0 ? 1 : mass;

        int32_t eax = ((velocity >> 1) + edx) / curMass;
        int32_t newAcceleration = -eax;
        if (carEntry->flags.has(CarEntryFlag::isPowered))
        {
            eax = speed << 14;
            int32_t ebx = (speed * curMass) >> 2;
            if (flags.has(VehicleFlag::poweredCarInReverse))
            {
                eax = -eax;
            }
            eax -= velocity;
            edx = powered_acceleration * 2;
            if (ebx != 0)
            {
                newAcceleration += (eax * edx) / ebx;
            }
        }
        acceleration = newAcceleration;
    }
    // eax = _vehicleMotionTrackFlags;
    // ebx = _vehicleStationIndex;
}

/**
 *
 *  rct2: 0x006DA280
 */
void Vehicle::UpdateBoatLocation()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    TileCoordsXY returnPosition = curRide->boatHireReturnPosition;
    uint8_t returnDirection = curRide->boatHireReturnDirection & 3;

    CoordsXY location = CoordsXY{ x, y } + CoordsDirectionDelta[returnDirection];

    if (location.ToTileStart() == returnPosition.ToCoordsXY())
    {
        sub_state = BoatHireSubState::EnteringReturnPosition;
        BoatLocation = location.ToTileStart();
        return;
    }

    sub_state = BoatHireSubState::Normal;
    uint8_t curDirection = ((Orientation + 19) >> 3) & 3;
    uint8_t randDirection = ScenarioRand() & 3;

    if (lost_time_out > 1920)
    {
        if (ScenarioRand() & 1)
        {
            CoordsXY destLocation = (returnPosition.ToCoordsXY() - CoordsDirectionDelta[returnDirection]).ToTileCentre();

            destLocation.x -= x;
            destLocation.y -= y;

            if (abs(destLocation.x) <= abs(destLocation.y))
            {
                randDirection = destLocation.y < 0 ? 3 : 1;
            }
            else
            {
                randDirection = destLocation.x < 0 ? 0 : 2;
            }
        }
    }

    static constexpr int8_t rotations[] = { 0, 1, -1, 2 };
    for (auto rotation : rotations)
    {
        if (randDirection + rotation == curDirection)
        {
            continue;
        }

        auto trackLocation = TrackLocation;
        trackLocation += CoordsDirectionDelta[(randDirection + rotation) & 3];

        if (!vehicle_boat_is_location_accessible(trackLocation))
        {
            continue;
        }

        BoatLocation = trackLocation.ToTileStart();
        return;
    }

    CoordsXY trackLocation = TrackLocation;
    trackLocation += CoordsDirectionDelta[curDirection & 3];
    BoatLocation = trackLocation.ToTileStart();
}

/**
 *
 *  rct2: 0x006DA22A
 */
static bool vehicle_boat_is_location_accessible(const CoordsXYZ& location)
{
    TileElement* tileElement = MapGetFirstElementAt(location);
    if (tileElement == nullptr)
        return false;
    do
    {
        if (tileElement->IsGhost())
            continue;

        if (tileElement->GetType() == TileElementType::Surface)
        {
            int32_t waterZ = tileElement->AsSurface()->GetWaterHeight();
            if (location.z != waterZ)
            {
                return false;
            }
        }
        else
        {
            if (location.z > (tileElement->GetBaseZ() - (2 * kCoordsZStep))
                && location.z < tileElement->GetClearanceZ() + (2 * kCoordsZStep))
            {
                return false;
            }
        }
    } while (!(tileElement++)->IsLastForTile());
    return true;
}
