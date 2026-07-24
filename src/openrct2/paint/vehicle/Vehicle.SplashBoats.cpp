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

#include <algorithm>
#include <cassert>
#include <cstdint>

namespace OpenRCT2
{
    static bool IsSplashBoatVehicleBeingPainted(const PaintSession& session, EntityId vehicleId)
    {
        return std::find(
                   session.ActiveSplashBoatPaintVehicleIds.begin(), session.ActiveSplashBoatPaintVehicleIds.end(), vehicleId)
            != session.ActiveSplashBoatPaintVehicleIds.end();
    }

    class ScopedSplashBoatVehiclePaint
    {
    public:
        ScopedSplashBoatVehiclePaint(PaintSession& session, EntityId vehicleId)
            : _vehicleId(vehicleId)
            , _session(session)
            , _shouldPaint(
                  !IsSplashBoatVehicleBeingPainted(session, vehicleId) && !session.ActiveSplashBoatPaintVehicleIds.full())
        {
            if (_shouldPaint)
            {
                _session.ActiveSplashBoatPaintVehicleIds.push_back(vehicleId);
            }
        }

        ScopedSplashBoatVehiclePaint(const ScopedSplashBoatVehiclePaint&) = delete;
        ScopedSplashBoatVehiclePaint& operator=(const ScopedSplashBoatVehiclePaint&) = delete;
        ScopedSplashBoatVehiclePaint(ScopedSplashBoatVehiclePaint&&) = delete;
        ScopedSplashBoatVehiclePaint& operator=(ScopedSplashBoatVehiclePaint&&) = delete;

        ~ScopedSplashBoatVehiclePaint()
        {
            if (_shouldPaint)
            {
                auto& activeVehicleIds = _session.ActiveSplashBoatPaintVehicleIds;
                assert(!activeVehicleIds.empty());
                assert(activeVehicleIds.back() == _vehicleId);
                if (!activeVehicleIds.empty() && activeVehicleIds.back() == _vehicleId)
                {
                    activeVehicleIds.pop_back();
                }
            }
        }

        bool ShouldPaint() const
        {
            return _shouldPaint;
        }

    private:
        EntityId _vehicleId;
        PaintSession& _session;
        bool _shouldPaint;
    };

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

        ScopedSplashBoatVehiclePaint paintScope(session, vehicle->id);
        if (!paintScope.ShouldPaint())
        {
            return;
        }

        auto* vehicleToPaint = vehicle->IsHead() ? entityRegistry.GetEntity<Vehicle>(vehicle->next_vehicle_on_ride)
                                                 : entityRegistry.GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
        if (vehicleToPaint == nullptr || IsSplashBoatVehicleBeingPainted(session, vehicleToPaint->id))
        {
            return;
        }

        session.CurrentlyDrawnEntity = vehicleToPaint;
        imageDirection = Entity::Yaw::Add(Entity::Yaw::YawFrom4(session.CurrentRotation), vehicleToPaint->orientation);
        session.SpritePosition.x = vehicleToPaint->x;
        session.SpritePosition.y = vehicleToPaint->y;
        PaintVehicle(session, *vehicleToPaint, imageDirection);
    }
} // namespace OpenRCT2
