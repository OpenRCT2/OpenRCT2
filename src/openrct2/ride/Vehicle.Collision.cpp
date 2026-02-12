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
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Yaw.hpp"
#include "../math/Trigonometry.hpp"

using namespace OpenRCT2;

/** rct2: 0x009A37C4 */
static constexpr CoordsXY kSurroundingTiles[] = {
    { 0, 0 },
    { 0, +kCoordsXYStep },
    { +kCoordsXYStep, 0 },
    { 0, -kCoordsXYStep },
    { 0, -kCoordsXYStep },
    { -kCoordsXYStep, 0 },
    { -kCoordsXYStep, 0 },
    { 0, +kCoordsXYStep },
    { 0, +kCoordsXYStep },
};

/**
 *
 *  rct2: 0x006DB807
 */
void Vehicle::UpdateReverserCarBogies()
{
    const auto moveInfo = GetMoveInfo();
    MoveTo({ TrackLocation.x + moveInfo->x, TrackLocation.y + moveInfo->y, z });
}

/**
 * Collision Detection
 *  rct2: 0x006DD078
 * @param vehicle (esi)
 * @param x (ax)
 * @param y (cx)
 * @param z (dx)
 * @param otherVehicleIndex (bp)
 */
bool Vehicle::UpdateMotionCollisionDetection(const CoordsXYZ& loc, EntityId* otherVehicleIndex)
{
    if (flags.has(VehicleFlag::collisionDisabled))
        return false;

    auto carEntry = Entry();
    if (carEntry == nullptr)
    {
        return false;
    }

    if (!carEntry->flags.has(CarEntryFlag::useBoatHireCollisionDetection))
    {
        CollisionDetectionTimer = 0;

        // If hacking boat hire rides you can end up here
        if (otherVehicleIndex == nullptr)
            return false;

        Vehicle* collideVehicle = getGameState().entities.GetEntity<Vehicle>(*otherVehicleIndex);
        if (collideVehicle == nullptr)
            return false;

        if (this == collideVehicle)
            return false;

        int32_t x_diff = abs(loc.x - collideVehicle->x);
        if (x_diff > 0x7FFF)
            return false;

        int32_t y_diff = abs(loc.y - collideVehicle->y);
        if (y_diff > 0x7FFF)
            return false;

        int32_t z_diff = abs(loc.z - collideVehicle->z);
        if (x_diff + y_diff + z_diff > 0xFFFF)
            return false;

        uint16_t ecx = std::min(var_44 + collideVehicle->var_44, 560);
        ecx = ((ecx >> 1) * 30) >> 8;

        if (x_diff + y_diff + z_diff >= ecx)
            return false;

        uint8_t direction = (Orientation - collideVehicle->Orientation + 7) & 0x1F;
        return direction < 0xF;
    }

    CoordsXY location = loc;

    bool mayCollide = false;
    Vehicle* collideVehicle = nullptr;
    for (auto xy_offset : kSurroundingTiles)
    {
        location += xy_offset;

        for (auto vehicle2 : EntityTileList<Vehicle>(location))
        {
            if (vehicle2 == this)
                continue;

            int32_t z_diff = abs(vehicle2->z - loc.z);

            if (z_diff > 16)
                continue;

            if (vehicle2->IsCableLift())
                continue;

            auto collideCarEntry = vehicle2->Entry();
            if (collideCarEntry == nullptr)
                continue;

            if (!collideCarEntry->flags.has(CarEntryFlag::useBoatHireCollisionDetection))
                continue;

            uint32_t x_diff = abs(vehicle2->x - loc.x);
            if (x_diff > 0x7FFF)
                continue;

            uint32_t y_diff = abs(vehicle2->y - loc.y);
            if (y_diff > 0x7FFF)
                continue;

            VehicleTrackSubposition cl = std::min(TrackSubposition, vehicle2->TrackSubposition);
            VehicleTrackSubposition ch = std::max(TrackSubposition, vehicle2->TrackSubposition);
            if (cl != ch)
            {
                if (cl == VehicleTrackSubposition::GoKartsLeftLane && ch == VehicleTrackSubposition::GoKartsRightLane)
                    continue;
            }

            uint32_t ecx = var_44 + vehicle2->var_44;
            ecx = ((ecx >> 1) * 30) >> 8;

            if (x_diff + y_diff >= ecx)
                continue;

            if (!collideCarEntry->flags.has(CarEntryFlag::isGoKart))
            {
                collideVehicle = vehicle2;
                mayCollide = true;
                break;
            }

            uint8_t direction = (Orientation - vehicle2->Orientation - 6) & 0x1F;

            if (direction < 0x14)
                continue;

            const CoordsXY directionVector = Math::Trigonometry::YawToDirectionVector[Entity::Yaw::YawTo64(Orientation)];

            const CoordsXY directionVectorToVehicle2 = { vehicle2->x - loc.x, vehicle2->y - loc.y };
            const int32_t directionVectorToVehicle2Length = (directionVectorToVehicle2.x * directionVectorToVehicle2.x)
                + (directionVectorToVehicle2.y * directionVectorToVehicle2.y);

            const int32_t dotProduct = (directionVector.x * directionVectorToVehicle2.x)
                + (directionVector.y * directionVectorToVehicle2.y);

            static constexpr int32_t threshold = []() consteval {
                const constexpr float originalThreshold = 0.35f;
                const constexpr float directionVectorLength = 256.0f;
                const constexpr float thresholdLength = originalThreshold * directionVectorLength;
                return static_cast<int32_t>(thresholdLength * thresholdLength);
            }();
            static_assert(threshold == 8028);

            if (dotProduct > 0 && dotProduct * dotProduct > threshold * directionVectorToVehicle2Length)
            {
                collideVehicle = vehicle2;
                mayCollide = true;
                break;
            }
        }
        if (mayCollide)
        {
            break;
        }
    }

    if (!mayCollide || collideVehicle == nullptr)
    {
        CollisionDetectionTimer = 0;
        return false;
    }

    if (collideVehicle->status == Status::travellingBoat && sub_state == BoatHireSubState::EnteringReturnPosition)
    {
        return false;
    }

    CollisionDetectionTimer++;
    if (CollisionDetectionTimer < 200)
    {
        flags.set(VehicleFlag::currentlyColliding);
        if (otherVehicleIndex != nullptr)
            *otherVehicleIndex = collideVehicle->Id;
        return true;
    }

    if (status == Status::movingToEndOfStation)
    {
        if (Orientation == 0)
        {
            if (x <= collideVehicle->x)
            {
                return false;
            }
        }
        else if (Orientation == 8)
        {
            if (y >= collideVehicle->y)
            {
                return false;
            }
        }
        else if (Orientation == 16)
        {
            if (x >= collideVehicle->x)
            {
                return false;
            }
        }
        else if (Orientation == 24)
        {
            if (y <= collideVehicle->y)
            {
                return false;
            }
        }
    }

    if ((collideVehicle->status == Status::travellingBoat) && (status != Status::arriving) && (status != Status::travelling))
    {
        return false;
    }

    flags.set(VehicleFlag::currentlyColliding);
    if (otherVehicleIndex != nullptr)
        *otherVehicleIndex = collideVehicle->Id;
    return true;
}

/**
 *
 *  rct2: 0x006DB7D6
 */
void Vehicle::ReverseReverserCar()
{
    Vehicle* previousVehicle = getGameState().entities.GetEntity<Vehicle>(prev_vehicle_on_ride);
    Vehicle* nextVehicle = getGameState().entities.GetEntity<Vehicle>(next_vehicle_on_ride);
    if (previousVehicle == nullptr || nextVehicle == nullptr)
    {
        return;
    }

    track_progress = 168;
    vehicle_type ^= 1;

    previousVehicle->track_progress = 86;
    nextVehicle->track_progress = 158;

    nextVehicle->UpdateReverserCarBogies();
    previousVehicle->UpdateReverserCarBogies();
}
