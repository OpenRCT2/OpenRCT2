/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScVehicle.hpp"

#include "../ride/ScRide.hpp"

#ifdef ENABLE_SCRIPTING

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<Vehicle::Status> VehicleStatusMap({
        { "moving_to_end_of_station", Vehicle::Status::MovingToEndOfStation },
        { "waiting_for_passengers", Vehicle::Status::WaitingForPassengers },
        { "waiting_to_depart", Vehicle::Status::WaitingToDepart },
        { "departing", Vehicle::Status::Departing },
        { "travelling", Vehicle::Status::Travelling },
        { "arriving", Vehicle::Status::Arriving },
        { "unloading_passengers", Vehicle::Status::UnloadingPassengers },
        { "travelling_boat", Vehicle::Status::TravellingBoat },
        { "crashing", Vehicle::Status::Crashing },
        { "crashed", Vehicle::Status::Crashed },
        { "travelling_dodgems", Vehicle::Status::TravellingDodgems },
        { "swinging", Vehicle::Status::Swinging },
        { "rotating", Vehicle::Status::Rotating },
        { "ferris_wheel_rotating", Vehicle::Status::FerrisWheelRotating },
        { "simulator_operating", Vehicle::Status::SimulatorOperating },
        { "showing_film", Vehicle::Status::ShowingFilm },
        { "space_rings_operating", Vehicle::Status::SpaceRingsOperating },
        { "top_spin_operating", Vehicle::Status::TopSpinOperating },
        { "haunted_house_operating", Vehicle::Status::HauntedHouseOperating },
        { "doing_circus_show", Vehicle::Status::DoingCircusShow },
        { "crooked_house_operating", Vehicle::Status::CrookedHouseOperating },
        { "waiting_for_cable_lift", Vehicle::Status::WaitingForCableLift },
        { "travelling_cable_lift", Vehicle::Status::TravellingCableLift },
        { "stopping", Vehicle::Status::Stopping },
        { "waiting_for_passengers_17", Vehicle::Status::WaitingForPassengers17 },
        { "waiting_to_start", Vehicle::Status::WaitingToStart },
        { "starting", Vehicle::Status::Starting },
        { "operating_1a", Vehicle::Status::Operating1A },
        { "stopping_1b", Vehicle::Status::Stopping1B },
        { "unloading_passengers_1c", Vehicle::Status::UnloadingPassengers1C },
        { "stopped_by_block_brake", Vehicle::Status::StoppedByBlockBrakes },
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
        dukglue_register_property(ctx, &ScVehicle::colours_get, &ScVehicle::colours_set, "colours");
        dukglue_register_property(ctx, &ScVehicle::trackLocation_get, &ScVehicle::trackLocation_set, "trackLocation");
        dukglue_register_property(ctx, &ScVehicle::trackProgress_get, nullptr, "trackProgress");
        dukglue_register_property(ctx, &ScVehicle::remainingDistance_get, nullptr, "remainingDistance");
        dukglue_register_property(
            ctx, &ScVehicle::poweredAcceleration_get, &ScVehicle::poweredAcceleration_set, "poweredAcceleration");
        dukglue_register_property(ctx, &ScVehicle::poweredMaxSpeed_get, &ScVehicle::poweredMaxSpeed_set, "poweredMaxSpeed");
        dukglue_register_property(ctx, &ScVehicle::status_get, &ScVehicle::status_set, "status");
        dukglue_register_property(ctx, &ScVehicle::guests_get, nullptr, "peeps");
        dukglue_register_property(ctx, &ScVehicle::guests_get, nullptr, "guests");
        dukglue_register_property(ctx, &ScVehicle::gForces_get, nullptr, "gForces");
        dukglue_register_method(ctx, &ScVehicle::travelBy, "travelBy");
    }

    Vehicle* ScVehicle::GetVehicle() const
    {
        return ::GetEntity<Vehicle>(_id);
    }

    uint8_t ScVehicle::rideObject_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->ride_subtype : 0;
    }
    void ScVehicle::rideObject_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->ride_subtype = value;
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
        }
    }

    uint8_t ScVehicle::spriteType_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->Pitch : 0;
    }
    void ScVehicle::spriteType_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->Pitch = value;
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
        return vehicle != nullptr ? vehicle->num_seats & VEHICLE_SEAT_NUM_MASK : 0;
    }
    void ScVehicle::numSeats_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->num_seats &= ~VEHICLE_SEAT_NUM_MASK;
            vehicle->num_seats |= value & VEHICLE_SEAT_NUM_MASK;
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
                vehicle->next_vehicle_on_train = EntityId::FromUnderlying(value.as_int());
            }
            else
            {
                vehicle->next_vehicle_on_train = EntityId::GetNull();
            }
        }
    }

    uint16_t ScVehicle::previousCarOnRide_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->prev_vehicle_on_ride.ToUnderlying() : EntityId::GetNull().ToUnderlying();
    }
    void ScVehicle::previousCarOnRide_set(uint16_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->prev_vehicle_on_ride = EntityId::FromUnderlying(value);
        }
    }

    uint16_t ScVehicle::nextCarOnRide_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->next_vehicle_on_ride.ToUnderlying() : EntityId::GetNull().ToUnderlying();
    }
    void ScVehicle::nextCarOnRide_set(uint16_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->next_vehicle_on_ride = EntityId::FromUnderlying(value);
        }
    }

    StationIndex ScVehicle::currentStation_get() const
    {
        auto vehicle = GetVehicle();
        return vehicle != nullptr ? vehicle->current_station : 0;
    }
    void ScVehicle::currentStation_set(StationIndex value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->current_station = value;
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
        return vehicle != nullptr ? vehicle->bank_rotation : 0;
    }
    void ScVehicle::bankRotation_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            vehicle->bank_rotation = value;
        }
    }

    DukValue ScVehicle::colours_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            VehicleColour colours;
            colours.Body = vehicle->colours.body_colour;
            colours.Trim = vehicle->colours.trim_colour;
            colours.Ternary = vehicle->colours_extended;
            return ToDuk<VehicleColour>(ctx, colours);
        }
        return ToDuk(ctx, nullptr);
    }
    void ScVehicle::colours_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            auto colours = FromDuk<VehicleColour>(value);
            vehicle->colours.body_colour = colours.Body;
            vehicle->colours.trim_colour = colours.Trim;
            vehicle->colours_extended = colours.Ternary;
        }
    }

    DukValue ScVehicle::trackLocation_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            auto coords = CoordsXYZD(vehicle->TrackLocation, vehicle->GetTrackDirection());
            return ToDuk<CoordsXYZD>(ctx, coords);
        }
        return ToDuk(ctx, nullptr);
    }
    void ScVehicle::trackLocation_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();
        auto vehicle = GetVehicle();
        if (vehicle != nullptr)
        {
            auto coords = FromDuk<CoordsXYZD>(value);
            vehicle->TrackLocation = CoordsXYZ(coords.x, coords.y, coords.z);
            vehicle->SetTrackDirection(coords.direction);
        }
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
        }
    }

} // namespace OpenRCT2::Scripting

#endif
