/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScVehicle.hpp"

#include "../../../ride/TrackData.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../ride/ScRide.hpp"

#ifdef ENABLE_SCRIPTING

using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::TrackMetaData;

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<Vehicle::Status> VehicleStatusMap(
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

    ScVehicle::ScVehicle(EntityId id)
        : ScEntity(id)
    {
    }

    void ScVehicle::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScEntity, ScVehicle>(ctx);
        dukglue_register_property(ctx, &ScVehicle::ride_get, &ScVehicle::ride_set, "ride");
        dukglue_register_property(ctx, &ScVehicle::rideObject_get, &ScVehicle::rideObject_set, "rideObject");
        dukglue_register_property(ctx, &ScVehicle::vehicleObject_get, &ScVehicle::vehicleObject_set, "vehicleObject");
        dukglue_register_property(ctx, &ScVehicle::spriteType_get, &ScVehicle::spriteType_set, "spriteType");
        dukglue_register_property(ctx, &ScVehicle::numSeats_get, &ScVehicle::numSeats_set, "numSeats");
        dukglue_register_property(ctx, &ScVehicle::nextCarOnTrain_get, &ScVehicle::nextCarOnTrain_set, "nextCarOnTrain");
        dukglue_register_property(
            ctx, &ScVehicle::previousCarOnRide_get, &ScVehicle::previousCarOnRide_set, "previousCarOnRide");
        dukglue_register_property(ctx, &ScVehicle::nextCarOnRide_get, &ScVehicle::nextCarOnRide_set, "nextCarOnRide");
        dukglue_register_property(ctx, &ScVehicle::currentStation_get, &ScVehicle::currentStation_set, "currentStation");
        dukglue_register_property(ctx, &ScVehicle::mass_get, &ScVehicle::mass_set, "mass");
        dukglue_register_property(ctx, &ScVehicle::acceleration_get, &ScVehicle::acceleration_set, "acceleration");
        dukglue_register_property(ctx, &ScVehicle::velocity_get, &ScVehicle::velocity_set, "velocity");
        dukglue_register_property(ctx, &ScVehicle::bankRotation_get, &ScVehicle::bankRotation_set, "bankRotation");
        dukglue_register_property(
            ctx, &ScVehicle::flag_get<VehicleFlags::CarIsReversed>, &ScVehicle::flag_set<VehicleFlags::CarIsReversed>,
            "isReversed");
        dukglue_register_property(
            ctx, &ScVehicle::flag_get<VehicleFlags::Crashed>, &ScVehicle::flag_set<VehicleFlags::Crashed>, "isCrashed");
        dukglue_register_property(ctx, &ScVehicle::colours_get, &ScVehicle::colours_set, "colours");
        dukglue_register_property(ctx, &ScVehicle::trackLocation_get, nullptr, "trackLocation");
        dukglue_register_property(ctx, &ScVehicle::trackProgress_get, nullptr, "trackProgress");
        dukglue_register_property(ctx, &ScVehicle::remainingDistance_get, nullptr, "remainingDistance");
        dukglue_register_property(ctx, &ScVehicle::subposition_get, nullptr, "subposition");
        dukglue_register_property(
            ctx, &ScVehicle::poweredAcceleration_get, &ScVehicle::poweredAcceleration_set, "poweredAcceleration");
        dukglue_register_property(ctx, &ScVehicle::poweredMaxSpeed_get, &ScVehicle::poweredMaxSpeed_set, "poweredMaxSpeed");
        dukglue_register_property(ctx, &ScVehicle::status_get, &ScVehicle::status_set, "status");
        dukglue_register_property(ctx, &ScVehicle::spin_get, &ScVehicle::spin_set, "spin");
        dukglue_register_property(ctx, &ScVehicle::guests_get, nullptr, "peeps");
        dukglue_register_property(ctx, &ScVehicle::guests_get, nullptr, "guests");
        dukglue_register_property(ctx, &ScVehicle::gForces_get, nullptr, "gForces");
        dukglue_register_method(ctx, &ScVehicle::travelBy, "travelBy");
        dukglue_register_method(ctx, &ScVehicle::moveToTrack, "moveToTrack");
    }

    Vehicle* ScVehicle::GetVehicle() const
    {
        return getGameState().entities.GetEntity<Vehicle>(_id);
    }

    ObjectEntryIndex ScVehicle::rideObject_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->ride_subtype : 0;
    }
    void ScVehicle::rideObject_set(ObjectEntryIndex value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->ride_subtype = value;
            vehicle->Invalidate();
        }
    }

    uint8_t ScVehicle::vehicleObject_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->vehicle_type : 0;
    }
    void ScVehicle::vehicleObject_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->vehicle_type = value;
            vehicle->Invalidate();
        }
    }

    uint8_t ScVehicle::spriteType_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? EnumValue(vehicle->pitch) : 0;
    }
    void ScVehicle::spriteType_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->pitch = static_cast<VehiclePitch>(value);
            vehicle->Invalidate();
        }
    }

    int32_t ScVehicle::ride_get() const
    {
        auto vehicle = GetVehicle();
        return (vehicle != nullptr ? vehicle->ride : RideId::GetNull()).ToUnderlying();
    }
    void ScVehicle::ride_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->ride = RideId::FromUnderlying(value);
        }
    }

    uint8_t ScVehicle::numSeats_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->num_seats & kVehicleSeatNumMask : 0;
    }
    void ScVehicle::numSeats_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->num_seats &= ~kVehicleSeatNumMask;
            vehicle->num_seats |= value & kVehicleSeatNumMask;
        }
    }

    DukValue ScVehicle::nextCarOnTrain_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            if (!vehicle->next_vehicle_on_train.IsNull())
            {
                return ToDuk<int32_t>(ctx, vehicle->next_vehicle_on_train.ToUnderlying());
            }
        }
        return ToDuk(ctx, nullptr);
    }
    void ScVehicle::nextCarOnTrain_set(DukValue value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            if (value.type() == DukValue::Type::NUMBER)
            {
                vehicle->next_vehicle_on_train = EntityId::FromUnderlying(value.as_uint());
            }
            else
            {
                vehicle->next_vehicle_on_train = EntityId::GetNull();
            }
        }
    }

    DukValue ScVehicle::previousCarOnRide_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        const auto* vehicle = GetVehicle();
        if (vehicle == nullptr)
            return ToDuk(ctx, nullptr);

        if (vehicle->prev_vehicle_on_ride.IsNull())
            return ToDuk(ctx, nullptr);

        return ToDuk(ctx, vehicle->prev_vehicle_on_ride.ToUnderlying());
    }
    void ScVehicle::previousCarOnRide_set(DukValue value)
    {
        ThrowIfGameStateNotMutable();
        auto* vehicle = GetVehicle();
        if (vehicle == nullptr)
            return;

        if (value.type() == DukValue::Type::NUMBER)
        {
            vehicle->prev_vehicle_on_ride = EntityId::FromUnderlying(value.as_uint());
        }
        else
        {
            vehicle->prev_vehicle_on_ride = EntityId::GetNull();
        }
    }

    DukValue ScVehicle::nextCarOnRide_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        const auto* vehicle = GetVehicle();
        if (vehicle == nullptr)
            return ToDuk(ctx, nullptr);

        if (vehicle->next_vehicle_on_ride.IsNull())
            return ToDuk(ctx, nullptr);

        return ToDuk(ctx, vehicle->next_vehicle_on_ride.ToUnderlying());
    }
    void ScVehicle::nextCarOnRide_set(DukValue value)
    {
        ThrowIfGameStateNotMutable();
        auto* vehicle = GetVehicle();
        if (vehicle == nullptr)
            return;

        if (value.type() == DukValue::Type::NUMBER)
        {
            vehicle->next_vehicle_on_ride = EntityId::FromUnderlying(value.as_uint());
        }
        else
        {
            vehicle->next_vehicle_on_ride = EntityId::GetNull();
        }
    }

    StationIndex::UnderlyingType ScVehicle::currentStation_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->current_station.ToUnderlying() : 0;
    }
    void ScVehicle::currentStation_set(StationIndex::UnderlyingType value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->current_station = StationIndex::FromUnderlying(value);
        }
    }

    uint16_t ScVehicle::mass_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->mass : 0;
    }
    void ScVehicle::mass_set(uint16_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->mass = value;
        }
    }

    int32_t ScVehicle::acceleration_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->acceleration : 0;
    }
    void ScVehicle::acceleration_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->acceleration = value;
        }
    }

    int32_t ScVehicle::velocity_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->velocity : 0;
    }
    void ScVehicle::velocity_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->velocity = value;
        }
    }

    uint8_t ScVehicle::bankRotation_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? EnumValue(vehicle->roll) : 0;
    }
    void ScVehicle::bankRotation_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->roll = static_cast<VehicleRoll>(value);
            vehicle->Invalidate();
        }
    }

    template<uint32_t flag>
    bool ScVehicle::flag_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->HasFlag(flag) : false;
    }

    template<uint32_t flag>
    void ScVehicle::flag_set(bool value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
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
    }

    DukValue ScVehicle::colours_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            return ToDuk<VehicleColour>(ctx, vehicle->colours);
        }
        return ToDuk(ctx, nullptr);
    }
    void ScVehicle::colours_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->colours = FromDuk<VehicleColour>(value);
            vehicle->Invalidate();
        }
    }

    DukValue ScVehicle::trackLocation_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            DukObject dukCoords(ctx);
            dukCoords.Set("x", vehicle->TrackLocation.x);
            dukCoords.Set("y", vehicle->TrackLocation.y);
            dukCoords.Set("z", vehicle->TrackLocation.z);
            dukCoords.Set("direction", vehicle->GetTrackDirection());
            dukCoords.Set("trackType", EnumValue(vehicle->GetTrackType()));
            return dukCoords.Take();
        }
        return ToDuk(ctx, nullptr);
    }

    uint16_t ScVehicle::trackProgress_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->track_progress : 0;
    }

    int32_t ScVehicle::remainingDistance_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->remaining_distance : 0;
    }

    uint8_t ScVehicle::subposition_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? static_cast<uint8_t>(vehicle->TrackSubposition) : 0;
    }

    uint8_t ScVehicle::poweredAcceleration_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->powered_acceleration : 0;
    }
    void ScVehicle::poweredAcceleration_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->powered_acceleration = value;
        }
    }

    uint8_t ScVehicle::poweredMaxSpeed_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->speed : 0;
    }
    void ScVehicle::poweredMaxSpeed_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->speed = value;
        }
    }

    std::string ScVehicle::status_get() const
    {
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            return std::string(VehicleStatusMap[vehicle->status]);
        }
        return {};
    }
    void ScVehicle::status_set(const std::string& value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->status = VehicleStatusMap[value];
        }
    }

    uint8_t ScVehicle::spin_get() const
    {
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            return vehicle->spin_sprite;
        }
        return 0;
    }
    void ScVehicle::spin_set(const uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->spin_sprite = value;
            vehicle->Invalidate();
        }
    }

    std::vector<DukValue> ScVehicle::guests_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        std::vector<DukValue> result;
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            size_t len = 0;
            for (size_t i = 0; i < std::size(vehicle->peep); i++)
            {
                auto peep = vehicle->peep[i];
                if (peep.IsNull())
                {
                    result.push_back(ToDuk(ctx, nullptr));
                }
                else
                {
                    result.push_back(ToDuk<int32_t>(ctx, peep.ToUnderlying()));
                    len = i + 1;
                }
            }
            result.resize(len);
        }
        return result;
    }

    DukValue ScVehicle::gForces_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            GForces gForces = vehicle->GetGForces();
            return ToDuk<GForces>(ctx, gForces);
        }
        return ToDuk(ctx, nullptr);
    }

    void ScVehicle::travelBy(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->MoveRelativeDistance(value);
            EntityTweener::Get().RemoveEntity(vehicle);
        }
    }

    void ScVehicle::moveToTrack(int32_t x, int32_t y, int32_t elementIndex)
    {
        auto vehicle = GetVehicle();
        if (vehicle == nullptr)
            return;

        CoordsXY coords = TileCoordsXY(x, y).ToCoordsXY();
        auto el = MapGetNthElementAt(coords, elementIndex);
        if (el == nullptr)
            return;

        auto origin = GetTrackSegmentOrigin(CoordsXYE(coords, el));
        if (!origin)
            return;

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
    }
} // namespace OpenRCT2::Scripting

#endif
