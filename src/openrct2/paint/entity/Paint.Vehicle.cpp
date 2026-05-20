/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Vehicle.h"

#include "../../SpriteIds.h"
#include "../../paint/vehicle/VehiclePaint.h"
#include "../../ride/RideData.h"
#include "../Paint.h"

using namespace OpenRCT2;

void PaintVehicle(PaintSession& session, const Vehicle& vehicle, int32_t imageDirection)
{
    const CarEntry* carEntry;

    if (vehicle.flags.has(VehicleFlag::crashed))
    {
        paintAddImageAsParent(
            session, ImageId(SPR_WATER_PARTICLES_DENSE_0 + vehicle.animation_frame), { 0, 0, vehicle.z },
            { { 0, 0, vehicle.z + 2 }, { 1, 1, 0 } });
        return;
    }

    int32_t zOffset = 0;
    if (vehicle.IsCableLift())
    {
        carEntry = &kCableLiftVehicle;
    }
    else
    {
        auto rideEntry = vehicle.GetRideEntry();
        if (rideEntry == nullptr)
        {
            return;
        }

        auto carEntryIndex = vehicle.vehicle_type;
        if (vehicle.flags.has(VehicleFlag::carIsInverted))
        {
            carEntryIndex++;
            zOffset += 16;
        }

        if (carEntryIndex >= std::size(rideEntry->Cars))
        {
            return;
        }
        carEntry = &rideEntry->Cars[carEntryIndex];
    }

    switch (carEntry->PaintStyle)
    {
        case VEHICLE_VISUAL_DEFAULT:
            vehicleVisualDefault(session, imageDirection, vehicle.z + zOffset, &vehicle, carEntry);
            break;
        case VEHICLE_VISUAL_LAUNCHED_FREEFALL:
            vehicleVisualLaunchedFreefall(
                session, vehicle.x, imageDirection, vehicle.y, vehicle.z + zOffset, &vehicle, carEntry);
            break;
        case VEHICLE_VISUAL_OBSERVATION_TOWER:
            vehicleVisualObservationTower(
                session, vehicle.x, imageDirection, vehicle.y, vehicle.z + zOffset, &vehicle, carEntry);
            break;
        case VEHICLE_VISUAL_RIVER_RAPIDS:
            vehicleVisualRiverRapids(session, vehicle.x, imageDirection, vehicle.y, vehicle.z + zOffset, &vehicle, carEntry);
            break;
        case VEHICLE_VISUAL_MINI_GOLF_PLAYER:
            vehicleVisualMiniGolfPlayer(session, vehicle.x, imageDirection, vehicle.y, vehicle.z + zOffset, &vehicle);
            break;
        case VEHICLE_VISUAL_MINI_GOLF_BALL:
            vehicleVisualMiniGolfBall(session, vehicle.x, imageDirection, vehicle.y, vehicle.z + zOffset, &vehicle);
            break;
        case VEHICLE_VISUAL_REVERSER:
            vehicleVisualReverser(session, vehicle.x, imageDirection, vehicle.y, vehicle.z + zOffset, &vehicle, carEntry);
            break;
        case VEHICLE_VISUAL_SPLASH_BOATS_OR_WATER_COASTER:
            vehicleVisualSplashBoatsOrWaterCoaster(
                session, vehicle.x, imageDirection, vehicle.y, vehicle.z + zOffset, &vehicle, carEntry);
            break;
        case VEHICLE_VISUAL_ROTO_DROP:
            vehicleVisualRotoDrop(session, vehicle.x, imageDirection, vehicle.y, vehicle.z + zOffset, &vehicle, carEntry);
            break;
        case VEHICLE_VISUAL_VIRGINIA_REEL:
            vehicleVisualVirginiaReel(session, vehicle.x, imageDirection, vehicle.y, vehicle.z + zOffset, &vehicle, carEntry);
            break;
        case VEHICLE_VISUAL_SUBMARINE:
            vehicleVisualSubmarine(session, vehicle.x, imageDirection, vehicle.y, vehicle.z + zOffset, &vehicle, carEntry);
            break;
        case VEHICLE_VISUAL_SPINNING_CARS:
            vehicleVisualClassicMiniSpinning(
                session, vehicle.x, imageDirection, vehicle.y, vehicle.z + zOffset, &vehicle, carEntry);
            break;
    }
}
