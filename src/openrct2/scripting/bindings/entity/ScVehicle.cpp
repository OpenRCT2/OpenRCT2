/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScVehicle.hpp"

#include "../../../core/EnumMap.hpp"
#include "../../../ride/TrackData.h"
#include "../../../ride/Vehicle.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../ride/ScRide.hpp"

#ifdef ENABLE_SCRIPTING_REFACTOR

using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::TrackMetaData;

namespace OpenRCT2::Scripting
{
    static const EnumMap<Vehicle::Status> VehicleStatusMap(
        {
            { "moving_to_end_of_station", Vehicle::Status::movingToEndOfStation },
            { "waiting_for_passengers", Vehicle::Status::waitingForPassengers },
            { "waiting_to_depart", Vehicle::Status::waitingToDepart },
            { "departing", Vehicle::Status::departing },
            { "travelling", Vehicle::Status::travelling },
            { "arriving", Vehicle::Status::arriving },
            { "unloading_passengers", Vehicle::Status::unloadingPassengers },
            { "travelling_boat", Vehicle::Status::travellingBoat },
            { "crashing", Vehicle::Status::crashing },
            { "crashed", Vehicle::Status::crashed },
            { "travelling_dodgems", Vehicle::Status::travellingDodgems },
            { "swinging", Vehicle::Status::swinging },
            { "rotating", Vehicle::Status::rotating },
            { "ferris_wheel_rotating", Vehicle::Status::ferrisWheelRotating },
            { "simulator_operating", Vehicle::Status::simulatorOperating },
            { "showing_film", Vehicle::Status::showingFilm },
            { "space_rings_operating", Vehicle::Status::spaceRingsOperating },
            { "top_spin_operating", Vehicle::Status::topSpinOperating },
            { "haunted_house_operating", Vehicle::Status::hauntedHouseOperating },
            { "doing_circus_show", Vehicle::Status::doingCircusShow },
            { "crooked_house_operating", Vehicle::Status::crookedHouseOperating },
            { "waiting_for_cable_lift", Vehicle::Status::waitingForCableLift },
            { "travelling_cable_lift", Vehicle::Status::travellingCableLift },
            { "stopping", Vehicle::Status::stopping },
            { "waiting_for_passengers_17", Vehicle::Status::waitingForPassengers17 },
            { "waiting_to_start", Vehicle::Status::waitingToStart },
            { "starting", Vehicle::Status::starting },
            { "operating_1a", Vehicle::Status::operating1A },
            { "stopping_1b", Vehicle::Status::stopping1B },
            { "unloading_passengers_1c", Vehicle::Status::unloadingPassengers1C },
            { "stopped_by_block_brake", Vehicle::Status::stoppedByBlockBrakes },
        });

    JSValue ScVehicle::New(JSContext* ctx, EntityId entityId)
    {
        JSValue obj = gScEntity.New(ctx, entityId);
        AddFuncs(ctx, obj);
        return obj;
    }

    void ScVehicle::AddFuncs(JSContext* ctx, JSValue obj)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("ride", &ScVehicle::ride_get, &ScVehicle::ride_set),
            JS_CGETSET_DEF("rideObject", &ScVehicle::rideObject_get, &ScVehicle::rideObject_set),
            JS_CGETSET_DEF("vehicleObject", &ScVehicle::vehicleObject_get, &ScVehicle::vehicleObject_set),
            JS_CGETSET_DEF("spriteType", &ScVehicle::spriteType_get, &ScVehicle::spriteType_set),
            JS_CGETSET_DEF("numSeats", &ScVehicle::numSeats_get, &ScVehicle::numSeats_set),
            JS_CGETSET_DEF("nextCarOnTrain", &ScVehicle::nextCarOnTrain_get, &ScVehicle::nextCarOnTrain_set),
            JS_CGETSET_DEF("previousCarOnRide", &ScVehicle::previousCarOnRide_get, &ScVehicle::previousCarOnRide_set),
            JS_CGETSET_DEF("nextCarOnRide", &ScVehicle::nextCarOnRide_get, &ScVehicle::nextCarOnRide_set),
            JS_CGETSET_DEF("currentStation", &ScVehicle::currentStation_get, &ScVehicle::currentStation_set),
            JS_CGETSET_DEF("mass", &ScVehicle::mass_get, &ScVehicle::mass_set),
            JS_CGETSET_DEF("acceleration", &ScVehicle::acceleration_get, &ScVehicle::acceleration_set),
            JS_CGETSET_DEF("velocity", &ScVehicle::velocity_get, &ScVehicle::velocity_set),
            JS_CGETSET_DEF("bankRotation", &ScVehicle::bankRotation_get, &ScVehicle::bankRotation_set),
            JS_CGETSET_DEF(
                "isReversed", &ScVehicle::flag_get<VehicleFlags::CarIsReversed>,
                &ScVehicle::flag_set<VehicleFlags::CarIsReversed>),
            JS_CGETSET_DEF(
                "isCrashed", &ScVehicle::flag_get<VehicleFlags::Crashed>, &ScVehicle::flag_set<VehicleFlags::Crashed>),
            JS_CGETSET_DEF("colours", &ScVehicle::colours_get, &ScVehicle::colours_set),
            JS_CGETSET_DEF("trackLocation", &ScVehicle::trackLocation_get, nullptr),
            JS_CGETSET_DEF("trackProgress", &ScVehicle::trackProgress_get, nullptr),
            JS_CGETSET_DEF("remainingDistance", &ScVehicle::remainingDistance_get, nullptr),
            JS_CGETSET_DEF("subposition", &ScVehicle::subposition_get, nullptr),
            JS_CGETSET_DEF("poweredAcceleration", &ScVehicle::poweredAcceleration_get, &ScVehicle::poweredAcceleration_set),
            JS_CGETSET_DEF("poweredMaxSpeed", &ScVehicle::poweredMaxSpeed_get, &ScVehicle::poweredMaxSpeed_set),
            JS_CGETSET_DEF("status", &ScVehicle::status_get, &ScVehicle::status_set),
            JS_CGETSET_DEF("spin", &ScVehicle::spin_get, &ScVehicle::spin_set),
            JS_CGETSET_DEF("peeps", &ScVehicle::guests_get, nullptr),
            JS_CGETSET_DEF("guests", &ScVehicle::guests_get, nullptr),
            JS_CGETSET_DEF("gForces", &ScVehicle::gForces_get, nullptr),
            JS_CFUNC_DEF("travelBy", 1, &ScVehicle::travelBy),
            JS_CFUNC_DEF("moveToTrack", 3, &ScVehicle::moveToTrack),
        };
        JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
    }

    Vehicle* ScVehicle::GetVehicle(JSValue thisVal)
    {
        auto id = GetEntityId(thisVal);
        return getGameState().entities.GetEntity<Vehicle>(id);
    }

    JSValue ScVehicle::rideObject_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? vehicle->ride_subtype : 0);
    }
    JSValue ScVehicle::rideObject_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->ride_subtype = value;
            vehicle->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::vehicleObject_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? vehicle->vehicle_type : 0);
    }
    JSValue ScVehicle::vehicleObject_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->vehicle_type = value;
            vehicle->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::spriteType_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? EnumValue(vehicle->pitch) : 0);
    }
    JSValue ScVehicle::spriteType_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->pitch = static_cast<VehiclePitch>(value);
            vehicle->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::ride_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        auto rideId = vehicle != nullptr ? vehicle->ride : RideId::GetNull();
        return JS_NewUint32(ctx, rideId.ToUnderlying());
    }
    JSValue ScVehicle::ride_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_INT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->ride = RideId::FromUnderlying(value);
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::numSeats_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? vehicle->num_seats & kVehicleSeatNumMask : 0);
    }
    JSValue ScVehicle::numSeats_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->num_seats &= ~kVehicleSeatNumMask;
            vehicle->num_seats |= value & kVehicleSeatNumMask;
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::nextCarOnTrain_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            if (!vehicle->next_vehicle_on_train.IsNull())
            {
                return JS_NewUint32(ctx, vehicle->next_vehicle_on_train.ToUnderlying());
            }
        }
        return JS_NULL;
    }
    JSValue ScVehicle::nextCarOnTrain_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            if (JS_IsNull(jsValue))
            {
                vehicle->next_vehicle_on_train = EntityId::GetNull();
            }
            else
            {
                JS_UNPACK_UINT32(entityId, ctx, jsValue);
                vehicle->next_vehicle_on_train = EntityId::FromUnderlying(entityId);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::previousCarOnRide_get(JSContext* ctx, JSValue thisVal)
    {
        const auto* vehicle = GetVehicle(thisVal);
        if (vehicle == nullptr)
            return JS_NULL;

        if (vehicle->prev_vehicle_on_ride.IsNull())
            return JS_NULL;

        return JS_NewUint32(ctx, vehicle->prev_vehicle_on_ride.ToUnderlying());
    }
    JSValue ScVehicle::previousCarOnRide_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto* vehicle = GetVehicle(thisVal);
        if (vehicle == nullptr)
            return JS_UNDEFINED;

        if (JS_IsNull(jsValue))
        {
            vehicle->prev_vehicle_on_ride = EntityId::GetNull();
        }
        else
        {
            JS_UNPACK_UINT32(entityId, ctx, jsValue);
            vehicle->prev_vehicle_on_ride = EntityId::FromUnderlying(entityId);
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::nextCarOnRide_get(JSContext* ctx, JSValue thisVal)
    {
        const auto* vehicle = GetVehicle(thisVal);
        if (vehicle == nullptr)
            return JS_NULL;

        if (vehicle->next_vehicle_on_ride.IsNull())
            return JS_NULL;

        return JS_NewUint32(ctx, vehicle->next_vehicle_on_ride.ToUnderlying());
    }
    JSValue ScVehicle::nextCarOnRide_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto* vehicle = GetVehicle(thisVal);
        if (vehicle == nullptr)
            return JS_UNDEFINED;

        if (JS_IsNull(jsValue))
        {
            vehicle->next_vehicle_on_ride = EntityId::GetNull();
        }
        else
        {
            JS_UNPACK_UINT32(entityId, ctx, jsValue);
            vehicle->next_vehicle_on_ride = EntityId::FromUnderlying(entityId);
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::currentStation_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? vehicle->current_station.ToUnderlying() : 0);
    }
    JSValue ScVehicle::currentStation_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->current_station = StationIndex::FromUnderlying(value);
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::mass_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? vehicle->mass : 0);
    }
    JSValue ScVehicle::mass_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->mass = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::acceleration_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewInt32(ctx, vehicle != nullptr ? vehicle->acceleration : 0);
    }
    JSValue ScVehicle::acceleration_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_INT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->acceleration = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::velocity_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewInt32(ctx, vehicle != nullptr ? vehicle->velocity : 0);
    }
    JSValue ScVehicle::velocity_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_INT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->velocity = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::bankRotation_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? EnumValue(vehicle->roll) : 0);
    }
    JSValue ScVehicle::bankRotation_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->roll = static_cast<VehicleRoll>(value);
            vehicle->Invalidate();
        }
        return JS_UNDEFINED;
    }

    template<uint32_t flag>
    JSValue ScVehicle::flag_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? vehicle->HasFlag(flag) : false);
    }

    template<uint32_t flag>
    JSValue ScVehicle::flag_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            if (value)
            {
                vehicle->SetFlag(flag);
            }
            else
            {
                vehicle->ClearFlag(flag);
            }
            vehicle->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::colours_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            return ToJSValue(ctx, vehicle->colours);
        }
        return JS_NULL;
    }
    JSValue ScVehicle::colours_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_OBJECT(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->colours = JSToVehicleColours(ctx, value);
            vehicle->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::trackLocation_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            JSValue obj = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, obj, "x", JS_NewInt32(ctx, vehicle->TrackLocation.x));
            JS_SetPropertyStr(ctx, obj, "y", JS_NewInt32(ctx, vehicle->TrackLocation.y));
            JS_SetPropertyStr(ctx, obj, "z", JS_NewInt32(ctx, vehicle->TrackLocation.z));
            JS_SetPropertyStr(ctx, obj, "direction", JS_NewUint32(ctx, vehicle->GetTrackDirection()));
            JS_SetPropertyStr(ctx, obj, "trackType", JS_NewUint32(ctx, EnumValue(vehicle->GetTrackType())));
            return obj;
        }
        return JS_NULL;
    }

    JSValue ScVehicle::trackProgress_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? vehicle->track_progress : 0);
    }

    JSValue ScVehicle::remainingDistance_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewInt32(ctx, vehicle != nullptr ? vehicle->remaining_distance : 0);
    }

    JSValue ScVehicle::subposition_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? static_cast<uint8_t>(vehicle->TrackSubposition) : 0);
    }

    JSValue ScVehicle::poweredAcceleration_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? vehicle->powered_acceleration : 0);
    }
    JSValue ScVehicle::poweredAcceleration_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->powered_acceleration = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::poweredMaxSpeed_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? vehicle->speed : 0);
    }
    JSValue ScVehicle::poweredMaxSpeed_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->speed = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::status_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            return JSFromStdString(ctx, VehicleStatusMap[vehicle->status]);
        }
        return JS_UNDEFINED;
    }
    JSValue ScVehicle::status_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_STR(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->status = VehicleStatusMap[value];
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::spin_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        return JS_NewUint32(ctx, vehicle != nullptr ? vehicle->spin_sprite : 0);
    }
    JSValue ScVehicle::spin_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->spin_sprite = value;
            vehicle->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::guests_get(JSContext* ctx, JSValue thisVal)
    {
        auto result = JS_NewArray(ctx);
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            size_t len = 0;
            for (size_t i = 0; i < std::size(vehicle->peep); i++)
            {
                auto peep = vehicle->peep[i];
                if (!peep.IsNull())
                {
                    // Set all peep slots between last valid peep and current to NULL (if there were any null peeps).
                    for (size_t j = len; j < i; j++)
                    {
                        JS_SetPropertyInt64(ctx, result, i, JS_NULL);
                    }

                    JS_SetPropertyInt64(ctx, result, i, JS_NewUint32(ctx, peep.ToUnderlying()));
                    len = i + 1;
                }
            }
        }
        return result;
    }

    JSValue ScVehicle::gForces_get(JSContext* ctx, JSValue thisVal)
    {
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            GForces gForces = vehicle->GetGForces();
            JSValue obj = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, obj, "lateralG", JS_NewInt32(ctx, gForces.LateralG));
            JS_SetPropertyStr(ctx, obj, "verticalG", JS_NewInt32(ctx, gForces.VerticalG));
            return obj;
        }
        return JS_NULL;
    }

    JSValue ScVehicle::travelBy(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_INT32(value, ctx, argv[0]);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle != nullptr)
        {
            vehicle->MoveRelativeDistance(value);
            EntityTweener::Get().RemoveEntity(vehicle);
        }
        return JS_UNDEFINED;
    }

    JSValue ScVehicle::moveToTrack(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_INT32(x, ctx, argv[0]);
        JS_UNPACK_INT32(y, ctx, argv[1]);
        JS_UNPACK_INT32(elementIndex, ctx, argv[2]);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto vehicle = GetVehicle(thisVal);
        if (vehicle == nullptr)
            return JS_UNDEFINED;

        CoordsXY coords = TileCoordsXY(x, y).ToCoordsXY();
        auto el = MapGetNthElementAt(coords, elementIndex);
        if (el == nullptr)
            return JS_UNDEFINED;

        auto origin = GetTrackSegmentOrigin(CoordsXYE(coords, el));
        if (!origin)
            return JS_UNDEFINED;

        const auto& trackType = el->AsTrack()->GetTrackType();
        const auto& ted = GetTrackElementDescriptor(trackType);
        const auto& seq0 = ted.sequences[0].clearance;
        const auto trackLoc = CoordsXYZ(origin->x + seq0.x, origin->y + seq0.y, origin->z + seq0.z);

        vehicle->TrackLocation.x = trackLoc.x;
        vehicle->TrackLocation.y = trackLoc.y;
        vehicle->TrackLocation.z = trackLoc.z;
        vehicle->SetTrackDirection(origin->direction);
        vehicle->SetTrackType(trackType);

        // Clip track progress to avoid being out of bounds of current piece
        uint16_t trackTotalProgress = vehicle->GetTrackProgress();
        if (trackTotalProgress && vehicle->track_progress >= trackTotalProgress)
            vehicle->track_progress = trackTotalProgress - 1;

        vehicle->UpdateTrackChange();
        EntityTweener::Get().RemoveEntity(vehicle);
        return JS_UNDEFINED;
    }
} // namespace OpenRCT2::Scripting

#endif
