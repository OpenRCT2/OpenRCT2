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
#include "VehiclePaint.h"

#include <cstdint>

namespace OpenRCT2
{
    /**
     *
     *  rct2: 0x006D4295
     */
    void VehicleVisualSplashBoatsOrWaterCoaster(
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

        session.CurrentlyDrawnEntity = vehicleToPaint;
        imageDirection = Entity::Yaw::Add(Entity::Yaw::YawFrom4(session.CurrentRotation), vehicleToPaint->Orientation);
        session.SpritePosition.x = vehicleToPaint->x;
        session.SpritePosition.y = vehicleToPaint->y;
        vehicleToPaint->Paint(session, imageDirection);
    }
} // namespace OpenRCT2
