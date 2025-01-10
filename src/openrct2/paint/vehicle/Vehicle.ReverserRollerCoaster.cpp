/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../ride/Vehicle.h"

#include "../../entity/EntityRegistry.h"
#include "../Paint.h"
#include "VehiclePaint.h"

#include <cstdint>

namespace OpenRCT2
{
    /**
     *
     *  rct2: 0x006D4453
     */
    void VehicleVisualReverser(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry)
    {
        Vehicle* v1 = GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
        Vehicle* v2 = GetEntity<Vehicle>(vehicle->next_vehicle_on_ride);
        if (v1 == nullptr || v2 == nullptr)
        {
            return;
        }
        x = (v1->x + v2->x) / 2;
        y = (v1->y + v2->y) / 2;
        z = (v1->z + v2->z) / 2;
        session.SpritePosition.x = x;
        session.SpritePosition.y = y;
        VehicleVisualDefault(session, imageDirection, z, vehicle, carEntry);
    }
} // namespace OpenRCT2
