/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../Context.h"
#    include "../common.h"
#    include "../peep/Peep.h"
#    include "../peep/Staff.h"
#    include "../util/Util.h"
#    include "../world/EntityList.h"
#    include "../world/Sprite.h"
#    include "Duktape.hpp"
#    include "ScRide.hpp"
#    include "ScriptEngine.h"

#    include <algorithm>
#    include <string_view>
#    include <unordered_map>

namespace OpenRCT2::Scripting
{
    class ScEntity
    {
    protected:
        uint16_t _id = SPRITE_INDEX_NULL;

    public:
        ScEntity(uint16_t id)
            : _id(id)
        {
        }

    private:
        int32_t id_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->sprite_index : 0;
        }

        std::string type_get() const
        {
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                switch (entity->sprite_identifier)
                {
                    case SpriteIdentifier::Vehicle:
                        return "car";
                    case SpriteIdentifier::Peep:
                        return "peep";
                    case SpriteIdentifier::Misc:
                    {
                        auto misc = entity->As<MiscEntity>();
                        if (misc == nullptr)
                        {
                            return "unknown";
                        }
                        switch (misc->SubType)
                        {
                            case MiscEntityType::SteamParticle:
                                return "steam_particle";
                            case MiscEntityType::MoneyEffect:
                                return "money_effect";
                            case MiscEntityType::CrashedVehicleParticle:
                                return "crashed_vehicle_particle";
                            case MiscEntityType::ExplosionCloud:
                                return "explosion_cloud";
                            case MiscEntityType::CrashSplash:
                                return "crash_splash";
                            case MiscEntityType::ExplosionFlare:
                                return "explosion_flare";
                            case MiscEntityType::JumpingFountainWater:
                                return "jumping_fountain_water";
                            case MiscEntityType::Balloon:
                                return "balloon";
                            case MiscEntityType::Duck:
                                return "duck";
                            case MiscEntityType::JumpingFountainSnow:
                                return "jumping_fountain_snow";
                            default:
                                break;
                        }
                    }
                    break;
                    case SpriteIdentifier::Litter:
                        return "litter";
                    case SpriteIdentifier::Null:
                        return "unknown";
                }
            }
            return "unknown";
        }

        // x getter and setter
        int32_t x_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->x : 0;
        }
        void x_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                entity->MoveTo({ value, entity->y, entity->z });
            }
        }

        // y getter and setter
        int32_t y_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->y : 0;
        }
        void y_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                entity->MoveTo({ entity->x, value, entity->z });
            }
        }

        // z getter and setter
        int16_t z_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->z : 0;
        }
        void z_set(int16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                entity->MoveTo({ entity->x, entity->y, value });
            }
        }

        void remove()
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                entity->Invalidate();
                switch (entity->sprite_identifier)
                {
                    case SpriteIdentifier::Vehicle:
                        duk_error(ctx, DUK_ERR_ERROR, "Removing a vehicle is currently unsupported.");
                        break;
                    case SpriteIdentifier::Peep:
                    {
                        auto peep = static_cast<Peep*>(entity);
                        // We can't remove a single peep from a ride at the moment as this can cause complications with the
                        // vehicle car having an unsupported peep capacity.
                        if (peep->State == PeepState::OnRide || peep->State == PeepState::EnteringRide)
                        {
                            duk_error(ctx, DUK_ERR_ERROR, "Removing a peep that is on a ride is currently unsupported.");
                        }
                        else
                        {
                            peep->Remove();
                        }
                        break;
                    }
                    case SpriteIdentifier::Misc:
                    case SpriteIdentifier::Litter:
                        sprite_remove(entity);
                        break;
                    case SpriteIdentifier::Null:
                        break;
                }
            }
        }

        SpriteBase* GetEntity() const
        {
            return ::GetEntity(_id);
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScEntity::id_get, nullptr, "id");
            dukglue_register_property(ctx, &ScEntity::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScEntity::x_get, &ScEntity::x_set, "x");
            dukglue_register_property(ctx, &ScEntity::y_get, &ScEntity::y_set, "y");
            dukglue_register_property(ctx, &ScEntity::z_get, &ScEntity::z_set, "z");
            dukglue_register_method(ctx, &ScEntity::remove, "remove");
        }
    };

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

    class ScVehicle : public ScEntity
    {
    public:
        ScVehicle(uint16_t id)
            : ScEntity(id)
        {
        }

        static void Register(duk_context* ctx)
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
            dukglue_register_property(ctx, &ScVehicle::peeps_get, nullptr, "peeps");
            dukglue_register_property(ctx, &ScVehicle::gForces_get, nullptr, "gForces");
            dukglue_register_method(ctx, &ScVehicle::travelBy, "travelBy");
        }

    private:
        Vehicle* GetVehicle() const
        {
            return ::GetEntity<Vehicle>(_id);
        }

        uint8_t rideObject_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->ride_subtype : 0;
        }
        void rideObject_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->ride_subtype = value;
            }
        }

        uint8_t vehicleObject_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->vehicle_type : 0;
        }
        void vehicleObject_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->vehicle_type = value;
            }
        }

        uint8_t spriteType_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->vehicle_sprite_type : 0;
        }
        void spriteType_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->vehicle_sprite_type = value;
            }
        }

        ride_id_t ride_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->ride : 0;
        }
        void ride_set(ride_id_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->ride = value;
            }
        }

        uint8_t numSeats_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->num_seats & VEHICLE_SEAT_NUM_MASK : 0;
        }
        void numSeats_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->num_seats &= ~VEHICLE_SEAT_NUM_MASK;
                vehicle->num_seats |= value & VEHICLE_SEAT_NUM_MASK;
            }
        }

        DukValue nextCarOnTrain_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                if (vehicle->next_vehicle_on_train != SPRITE_INDEX_NULL)
                {
                    return ToDuk<int32_t>(ctx, vehicle->next_vehicle_on_train);
                }
            }
            return ToDuk(ctx, nullptr);
        }
        void nextCarOnTrain_set(DukValue value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                if (value.type() == DukValue::Type::NUMBER)
                {
                    vehicle->next_vehicle_on_train = static_cast<uint16_t>(value.as_int());
                }
                else
                {
                    vehicle->next_vehicle_on_train = SPRITE_INDEX_NULL;
                }
            }
        }

        uint16_t previousCarOnRide_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->prev_vehicle_on_ride : 0;
        }
        void previousCarOnRide_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->prev_vehicle_on_ride = value;
            }
        }

        uint16_t nextCarOnRide_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->next_vehicle_on_ride : 0;
        }
        void nextCarOnRide_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->next_vehicle_on_ride = value;
            }
        }

        StationIndex currentStation_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->current_station : 0;
        }
        void currentStation_set(StationIndex value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->current_station = value;
            }
        }

        uint16_t mass_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->mass : 0;
        }
        void mass_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->mass = value;
            }
        }

        int32_t acceleration_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->acceleration : 0;
        }
        void acceleration_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->acceleration = value;
            }
        }

        int32_t velocity_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->velocity : 0;
        }
        void velocity_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->velocity = value;
            }
        }

        uint8_t bankRotation_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->bank_rotation : 0;
        }
        void bankRotation_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->bank_rotation = value;
            }
        }

        DukValue colours_get() const
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
        void colours_set(const DukValue& value)
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

        DukValue trackLocation_get() const
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
        void trackLocation_set(const DukValue& value)
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

        uint16_t trackProgress_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->track_progress : 0;
        }

        int32_t remainingDistance_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->remaining_distance : 0;
        }

        uint8_t poweredAcceleration_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->powered_acceleration : 0;
        }
        void poweredAcceleration_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->powered_acceleration = value;
            }
        }

        uint8_t poweredMaxSpeed_get() const
        {
            auto vehicle = GetVehicle();
            return vehicle != nullptr ? vehicle->speed : 0;
        }
        void poweredMaxSpeed_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->speed = value;
            }
        }

        std::string status_get() const
        {
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                return std::string(VehicleStatusMap[vehicle->status]);
            }
            return {};
        }
        void status_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->status = VehicleStatusMap[value];
            }
        }

        std::vector<DukValue> peeps_get() const
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
                    if (peep == SPRITE_INDEX_NULL)
                    {
                        result.push_back(ToDuk(ctx, nullptr));
                    }
                    else
                    {
                        result.push_back(ToDuk<int32_t>(ctx, peep));
                        len = i + 1;
                    }
                }
                result.resize(len);
            }
            return result;
        }

        DukValue gForces_get() const
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

        void travelBy(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto vehicle = GetVehicle();
            if (vehicle != nullptr)
            {
                vehicle->MoveRelativeDistance(value);
            }
        }
    };

    static const DukEnumMap<uint32_t> PeepFlagMap({
        { "leavingPark", PEEP_FLAGS_LEAVING_PARK },
        { "slowWalk", PEEP_FLAGS_SLOW_WALK },
        { "tracking", PEEP_FLAGS_TRACKING },
        { "waving", PEEP_FLAGS_WAVING },
        { "hasPaidForParkEntry", PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY },
        { "photo", PEEP_FLAGS_PHOTO },
        { "painting", PEEP_FLAGS_PAINTING },
        { "wow", PEEP_FLAGS_WOW },
        { "litter", PEEP_FLAGS_LITTER },
        { "lost", PEEP_FLAGS_LOST },
        { "hunger", PEEP_FLAGS_HUNGER },
        { "toilet", PEEP_FLAGS_TOILET },
        { "crowded", PEEP_FLAGS_CROWDED },
        { "happiness", PEEP_FLAGS_HAPPINESS },
        { "nausea", PEEP_FLAGS_NAUSEA },
        { "purple", PEEP_FLAGS_PURPLE },
        { "pizza", PEEP_FLAGS_PIZZA },
        { "explode", PEEP_FLAGS_EXPLODE },
        { "rideShouldBeMarkedAsFavourite", PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE },
        { "parkEntranceChosen", PEEP_FLAGS_PARK_ENTRANCE_CHOSEN },
        { "contagious", PEEP_FLAGS_CONTAGIOUS },
        { "joy", PEEP_FLAGS_JOY },
        { "angry", PEEP_FLAGS_ANGRY },
        { "iceCream", PEEP_FLAGS_ICE_CREAM },
        { "hereWeAre", PEEP_FLAGS_HERE_WE_ARE },
    });

    class ScPeep : public ScEntity
    {
    public:
        ScPeep(uint16_t id)
            : ScEntity(id)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScEntity, ScPeep>(ctx);
            dukglue_register_property(ctx, &ScPeep::peepType_get, nullptr, "peepType");
            dukglue_register_property(ctx, &ScPeep::name_get, &ScPeep::name_set, "name");
            dukglue_register_property(ctx, &ScPeep::destination_get, &ScPeep::destination_set, "destination");
            dukglue_register_property(ctx, &ScPeep::energy_get, &ScPeep::energy_set, "energy");
            dukglue_register_property(ctx, &ScPeep::energyTarget_get, &ScPeep::energyTarget_set, "energyTarget");
            dukglue_register_method(ctx, &ScPeep::getFlag, "getFlag");
            dukglue_register_method(ctx, &ScPeep::setFlag, "setFlag");
        }

    private:
        std::string peepType_get() const
        {
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                return peep->AssignedPeepType == PeepType::Staff ? "staff" : "guest";
            }
            return "";
        }

        std::string name_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->GetName() : std::string();
        }
        void name_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->SetName(value);
            }
        }

        bool getFlag(const std::string& key) const
        {
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                auto mask = PeepFlagMap[key];
                return (peep->PeepFlags & mask) != 0;
            }
            return false;
        }

        void setFlag(const std::string& key, bool value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                auto mask = PeepFlagMap[key];
                if (value)
                    peep->PeepFlags |= mask;
                else
                    peep->PeepFlags &= ~mask;
                peep->Invalidate();
            }
        }

        DukValue destination_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                return ToDuk(ctx, peep->GetDestination());
            }
            return ToDuk(ctx, nullptr);
        }

        void destination_set(const DukValue& value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                auto pos = FromDuk<CoordsXY>(value);
                peep->SetDestination(pos);
                peep->Invalidate();
            }
        }

        uint8_t energy_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Energy : 0;
        }
        void energy_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Energy = value;
            }
        }

        uint8_t energyTarget_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->EnergyTarget : 0;
        }
        void energyTarget_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->EnergyTarget = value;
            }
        }

    protected:
        Peep* GetPeep() const
        {
            return ::GetEntity<Peep>(_id);
        }
    };

    class ScGuest : public ScPeep
    {
    public:
        ScGuest(uint16_t id)
            : ScPeep(id)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScPeep, ScGuest>(ctx);
            dukglue_register_property(ctx, &ScGuest::tshirtColour_get, &ScGuest::tshirtColour_set, "tshirtColour");
            dukglue_register_property(ctx, &ScGuest::trousersColour_get, &ScGuest::trousersColour_set, "trousersColour");
            dukglue_register_property(ctx, &ScGuest::balloonColour_get, &ScGuest::balloonColour_set, "balloonColour");
            dukglue_register_property(ctx, &ScGuest::hatColour_get, &ScGuest::hatColour_set, "hatColour");
            dukglue_register_property(ctx, &ScGuest::umbrellaColour_get, &ScGuest::umbrellaColour_set, "umbrellaColour");
            dukglue_register_property(ctx, &ScGuest::happiness_get, &ScGuest::happiness_set, "happiness");
            dukglue_register_property(ctx, &ScGuest::happinessTarget_get, &ScGuest::happinessTarget_set, "happinessTarget");
            dukglue_register_property(ctx, &ScGuest::nausea_get, &ScGuest::nausea_set, "nausea");
            dukglue_register_property(ctx, &ScGuest::nauseaTarget_get, &ScGuest::nauseaTarget_set, "nauseaTarget");
            dukglue_register_property(ctx, &ScGuest::hunger_get, &ScGuest::hunger_set, "hunger");
            dukglue_register_property(ctx, &ScGuest::thirst_get, &ScGuest::thirst_set, "thirst");
            dukglue_register_property(ctx, &ScGuest::toilet_get, &ScGuest::toilet_set, "toilet");
            dukglue_register_property(ctx, &ScGuest::mass_get, &ScGuest::mass_set, "mass");
            dukglue_register_property(ctx, &ScGuest::minIntensity_get, &ScGuest::minIntensity_set, "minIntensity");
            dukglue_register_property(ctx, &ScGuest::maxIntensity_get, &ScGuest::maxIntensity_set, "maxIntensity");
            dukglue_register_property(ctx, &ScGuest::nauseaTolerance_get, &ScGuest::nauseaTolerance_set, "nauseaTolerance");
            dukglue_register_property(ctx, &ScGuest::cash_get, &ScGuest::cash_set, "cash");
        }

    private:
        Guest* GetGuest() const
        {
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                return peep->As<Guest>();
            }
            return nullptr;
        }

        uint8_t tshirtColour_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->TshirtColour : 0;
        }
        void tshirtColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->TshirtColour = value;
                peep->Invalidate();
            }
        }

        uint8_t trousersColour_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->TrousersColour : 0;
        }
        void trousersColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->TrousersColour = value;
                peep->Invalidate();
            }
        }

        uint8_t balloonColour_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->BalloonColour : 0;
        }
        void balloonColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->BalloonColour = value;
                peep->Invalidate();
            }
        }

        uint8_t hatColour_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->HatColour : 0;
        }
        void hatColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->HatColour = value;
                peep->Invalidate();
            }
        }

        uint8_t umbrellaColour_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->UmbrellaColour : 0;
        }
        void umbrellaColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->UmbrellaColour = value;
                peep->Invalidate();
            }
        }

        uint8_t happiness_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Happiness : 0;
        }
        void happiness_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Happiness = value;
            }
        }

        uint8_t happinessTarget_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->HappinessTarget : 0;
        }
        void happinessTarget_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->HappinessTarget = value;
            }
        }

        uint8_t nausea_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Nausea : 0;
        }
        void nausea_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Nausea = value;
            }
        }

        uint8_t nauseaTarget_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->NauseaTarget : 0;
        }
        void nauseaTarget_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->NauseaTarget = value;
            }
        }

        uint8_t hunger_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Hunger : 0;
        }
        void hunger_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Hunger = value;
            }
        }

        uint8_t thirst_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Thirst : 0;
        }
        void thirst_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Thirst = value;
            }
        }

        uint8_t toilet_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Toilet : 0;
        }
        void toilet_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Toilet = value;
            }
        }

        uint8_t mass_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Mass : 0;
        }
        void mass_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Mass = value;
            }
        }

        uint8_t minIntensity_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Intensity.GetMinimum() : 0;
        }
        void minIntensity_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Intensity = peep->Intensity.WithMinimum(value);
            }
        }

        uint8_t maxIntensity_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Intensity.GetMaximum() : 0;
        }
        void maxIntensity_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Intensity = peep->Intensity.WithMaximum(value);
            }
        }

        uint8_t nauseaTolerance_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? EnumValue(peep->NauseaTolerance) : 0;
        }
        void nauseaTolerance_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->NauseaTolerance = static_cast<PeepNauseaTolerance>(std::min<uint8_t>(value, 3));
            }
        }

        int32_t cash_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->CashInPocket : 0;
        }
        void cash_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->CashInPocket = std::max(0, value);
            }
        }
    };

    class ScStaff : public ScPeep
    {
    public:
        ScStaff(uint16_t Id)
            : ScPeep(Id)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScPeep, ScStaff>(ctx);
            dukglue_register_property(ctx, &ScStaff::staffType_get, &ScStaff::staffType_set, "staffType");
            dukglue_register_property(ctx, &ScStaff::colour_get, &ScStaff::colour_set, "colour");
            dukglue_register_property(ctx, &ScStaff::costume_get, &ScStaff::costume_set, "costume");
            dukglue_register_property(ctx, &ScStaff::orders_get, &ScStaff::orders_set, "orders");
        }

    private:
        Staff* GetStaff() const
        {
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                return peep->As<Staff>();
            }
            return nullptr;
        }

        std::string staffType_get() const
        {
            auto peep = GetStaff();
            if (peep != nullptr)
            {
                switch (peep->AssignedStaffType)
                {
                    case StaffType::Handyman:
                        return "handyman";
                    case StaffType::Mechanic:
                        return "mechanic";
                    case StaffType::Security:
                        return "security";
                    case StaffType::Entertainer:
                        return "entertainer";
                    case StaffType::Count:
                        break;
                }
            }
            return "";
        }

        void staffType_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetStaff();
            if (peep != nullptr)
            {
                if (value == "handyman" && peep->AssignedStaffType != StaffType::Handyman)
                {
                    peep->AssignedStaffType = StaffType::Handyman;
                    peep->SpriteType = PeepSpriteType::Handyman;
                }
                else if (value == "mechanic" && peep->AssignedStaffType != StaffType::Mechanic)
                {
                    peep->AssignedStaffType = StaffType::Mechanic;
                    peep->SpriteType = PeepSpriteType::Mechanic;
                }
                else if (value == "security" && peep->AssignedStaffType != StaffType::Security)
                {
                    peep->AssignedStaffType = StaffType::Security;
                    peep->SpriteType = PeepSpriteType::Security;
                }
                else if (value == "entertainer" && peep->AssignedStaffType != StaffType::Entertainer)
                {
                    peep->AssignedStaffType = StaffType::Entertainer;
                    peep->SpriteType = PeepSpriteType::EntertainerPanda;
                }
            }
        }

        uint8_t colour_get() const
        {
            auto peep = GetStaff();
            return peep != nullptr ? peep->TshirtColour : 0;
        }

        void colour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetStaff();
            if (peep != nullptr)
            {
                peep->TshirtColour = value;
                peep->TrousersColour = value;
            }
        }

        uint8_t costume_get() const
        {
            auto peep = GetStaff();
            if (peep != nullptr && peep->AssignedStaffType == StaffType::Entertainer)
            {
                return peep->GetCostume();
            }
            return 0;
        }

        void costume_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetStaff();
            if (peep != nullptr)
            {
                peep->SetCostume(value);
            }
        }

        uint8_t orders_get() const
        {
            auto peep = GetStaff();
            return peep != nullptr ? peep->StaffOrders : 0;
        }

        void orders_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetStaff();
            if (peep != nullptr)
            {
                peep->StaffOrders = value;
            }
        }
    };

} // namespace OpenRCT2::Scripting

#endif
