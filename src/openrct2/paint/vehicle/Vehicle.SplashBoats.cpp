/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../ride/Vehicle.h"

#include "../../GameState.h"
#include "../../entity/EntityRegistry.h"
#include "../../ride/Ride.h"
#include "../Paint.h"
#include "../entity/Paint.Vehicle.h"
#include "VehiclePaint.h"

#include <cstdint>

namespace OpenRCT2
{
    /**
     *
     *  rct2: 0x006D4295
     */
    void vehicleVisualSplashBoatsOrWaterCoaster(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry)
    {
        // TODO: pass as parameter?
        auto& entityRegistry = getGameState().entities;

        auto* vehicleToPaint = vehicle->IsHead() ? entityRegistry.GetEntity<Vehicle>(vehicle->next_vehicle_on_ride)
                                                 : entityRegistry.GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
        if (vehicleToPaint == nullptr)
        {
            return;
        }

        session.currentlyDrawnEntity = vehicleToPaint;
        imageDirection = Entity::Yaw::Add(Entity::Yaw::YawFrom4(session.currentRotation), vehicleToPaint->orientation);
        session.spritePosition.x = vehicleToPaint->x;
        session.spritePosition.y = vehicleToPaint->y;
        PaintVehicle(session, *vehicleToPaint, imageDirection);
    }
} // namespace OpenRCT2
