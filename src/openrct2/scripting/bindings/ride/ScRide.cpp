/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScRide.hpp"

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../ride/Ride.h"
#    include "../../../ride/RideData.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"
#    include "../object/ScObject.hpp"

namespace OpenRCT2::Scripting
{
    ScRide::ScRide(RideId rideId)
        : _rideId(rideId)
    {
    }

    int32_t ScRide::id_get() const
    {
        return _rideId.ToUnderlying();
    }

    std::shared_ptr<ScRideObject> ScRide::object_get()
    {
        auto ride = GetRide();
        if (ride != nullptr)
        {
            auto rideObject = GetContext()->GetObjectManager().GetLoadedObject(ObjectType::Ride, ride->subtype);
            if (rideObject != nullptr)
            {
                return std::make_shared<ScRideObject>(ObjectType::Ride, ride->subtype);
            }
        }
        return nullptr;
    }

    int32_t ScRide::type_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->type : 0;
    }

    std::string ScRide::classification_get() const
    {
        auto ride = GetRide();
        if (ride != nullptr)
        {
            switch (ride->GetClassification())
            {
                case RideClassification::Ride:
                    return "ride";
                case RideClassification::ShopOrStall:
                    return "stall";
                case RideClassification::KioskOrFacility:
                    return "facility";
            }
        }
        return "";
    }

    std::string ScRide::name_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->GetName() : std::string();
    }
    void ScRide::name_set(std::string value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->custom_name = value;
        }
    }

    std::string ScRide::status_get() const
    {
        auto ride = GetRide();
        if (ride != nullptr)
        {
            switch (ride->status)
            {
                case RideStatus::Closed:
                    return "closed";
                case RideStatus::Open:
                    return "open";
                case RideStatus::Testing:
                    return "testing";
                case RideStatus::Simulating:
                    return "simulating";
                case RideStatus::Count: // Meaningless but necessary to satisfy -Wswitch
                    return "count";
            }
        }
        return "";
    }

    uint32_t ScRide::lifecycleFlags_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->lifecycle_flags : 0;
    }

    void ScRide::lifecycleFlags_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->lifecycle_flags = value;
        }
    }

    uint8_t ScRide::mode_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? static_cast<uint8_t>(ride->mode) : 0;
    }

    void ScRide::mode_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->mode = static_cast<RideMode>(value);
        }
    }

    uint8_t ScRide::departFlags_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->depart_flags : 0;
    }

    void ScRide::departFlags_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->depart_flags = value;
        }
    }

    uint8_t ScRide::minimumWaitingTime_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->min_waiting_time : 0;
    }

    void ScRide::minimumWaitingTime_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->min_waiting_time = value;
        }
    }

    uint8_t ScRide::maximumWaitingTime_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->max_waiting_time : 0;
    }

    void ScRide::maximumWaitingTime_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->max_waiting_time = value;
        }
    }

    std::vector<uint16_t> ScRide::vehicles_get() const
    {
        std::vector<uint16_t> result;
        auto ride = GetRide();
        if (ride != nullptr)
        {
            std::for_each(std::begin(ride->vehicles), std::begin(ride->vehicles) + ride->num_vehicles, [&](auto& veh) {
                result.push_back(veh.ToUnderlying());
            });
        }
        return result;
    }

    std::vector<DukValue> ScRide::vehicleColours_get() const
    {
        std::vector<DukValue> result;
        auto ride = GetRide();
        if (ride != nullptr)
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            for (const auto& vehicleColour : ride->vehicle_colours)
            {
                result.push_back(ToDuk(ctx, vehicleColour));
            }
        }
        return result;
    }

    void ScRide::vehicleColours_set(const std::vector<DukValue>& value)
    {
        auto ride = GetRide();
        if (ride != nullptr)
        {
            auto count = std::min(value.size(), std::size(ride->vehicle_colours));
            for (size_t i = 0; i < count; i++)
            {
                ride->vehicle_colours[i] = FromDuk<VehicleColour>(value[i]);
            }
        }
    }

    std::vector<DukValue> ScRide::colourSchemes_get() const
    {
        std::vector<DukValue> result;
        auto ride = GetRide();
        if (ride != nullptr)
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            for (const auto& trackColour : ride->track_colour)
            {
                result.push_back(ToDuk(ctx, trackColour));
            }
        }
        return result;
    }

    void ScRide::colourSchemes_set(const std::vector<DukValue>& value)
    {
        auto ride = GetRide();
        if (ride != nullptr)
        {
            auto count = std::min(value.size(), std::size(ride->track_colour));
            for (size_t i = 0; i < count; i++)
            {
                ride->track_colour[i] = FromDuk<TrackColour>(value[i]);
            }
        }
    }

    ObjectEntryIndex ScRide::stationStyle_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->entrance_style : 0;
    }

    void ScRide::stationStyle_set(ObjectEntryIndex value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->entrance_style = value;
        }
    }

    ObjectEntryIndex ScRide::music_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->music : 0;
    }

    void ScRide::music_set(ObjectEntryIndex value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->music = value;
        }
    }

    std::vector<std::shared_ptr<ScRideStation>> ScRide::stations_get() const
    {
        std::vector<std::shared_ptr<ScRideStation>> result;
        auto ride = GetRide();
        if (ride != nullptr)
        {
            for (const auto& station : ride->GetStations())
            {
                result.push_back(std::make_shared<ScRideStation>(ride->id, ride->GetStationIndex(&station)));
            }
        }
        return result;
    }

    std::vector<int32_t> ScRide::price_get() const
    {
        std::vector<int32_t> result;
        auto ride = GetRide();
        if (ride != nullptr)
        {
            auto numPrices = ride->GetNumPrices();
            for (size_t i = 0; i < numPrices; i++)
            {
                result.push_back(ride->price[i]);
            };
        }
        return result;
    }

    void ScRide::price_set(const std::vector<int32_t>& value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            auto numPrices = std::min(value.size(), ride->GetNumPrices());
            for (size_t i = 0; i < numPrices; i++)
            {
                ride->price[i] = static_cast<money16>(value[i]);
            }
        }
    }

    int32_t ScRide::excitement_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->excitement : 0;
    }
    void ScRide::excitement_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->excitement = value;
        }
    }

    int32_t ScRide::intensity_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->intensity : 0;
    }
    void ScRide::intensity_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->intensity = value;
        }
    }

    int32_t ScRide::nausea_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->nausea : 0;
    }
    void ScRide::nausea_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->nausea = value;
        }
    }

    int32_t ScRide::totalCustomers_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->total_customers : 0;
    }
    void ScRide::totalCustomers_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->total_customers = value;
        }
    }

    int32_t ScRide::buildDate_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->build_date : 0;
    }
    void ScRide::buildDate_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->build_date = value;
        }
    }

    int32_t ScRide::age_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->GetAge() : 0;
    }

    int16_t ScRide::runningCost_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->upkeep_cost : 0;
    }
    void ScRide::runningCost_set(int16_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->upkeep_cost = value;
        }
    }

    int32_t ScRide::totalProfit_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->total_profit : 0;
    }
    void ScRide::totalProfit_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->total_profit = value;
        }
    }

    uint8_t ScRide::inspectionInterval_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->inspection_interval : 0;
    }
    void ScRide::inspectionInterval_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->inspection_interval = std::clamp<uint8_t>(value, RIDE_INSPECTION_EVERY_10_MINUTES, RIDE_INSPECTION_NEVER);
        }
    }

    DukValue ScRide::value_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto ride = GetRide();
        if (ride != nullptr && ride->value != RIDE_VALUE_UNDEFINED)
        {
            return ToDuk<int32_t>(ctx, ride->value);
        }
        return ToDuk(ctx, nullptr);
    }

    void ScRide::value_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            if (value.type() == DukValue::Type::NUMBER)
            {
                ride->value = value.as_int();
            }
            else
            {
                ride->value = RIDE_VALUE_UNDEFINED;
            }
        }
    }

    Ride* ScRide::GetRide() const
    {
        return get_ride(_rideId);
    }

    uint8_t ScRide::downtime_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->downtime : 0;
    }

    uint8_t ScRide::liftHillSpeed_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->lift_hill_speed : 0;
    }

    void ScRide::lifthillSpeed_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();

        auto ride = GetRide();
        if (ride)
        {
            ride->lift_hill_speed = value;
        }
    }

    uint8_t ScRide::maxLiftHillSpeed_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->GetRideTypeDescriptor().LiftData.maximum_speed : 0;
    }

    uint8_t ScRide::minLiftHillSpeed_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->GetRideTypeDescriptor().LiftData.minimum_speed : 0;
    }

    void ScRide::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScRide::id_get, nullptr, "id");
        dukglue_register_property(ctx, &ScRide::object_get, nullptr, "object");
        dukglue_register_property(ctx, &ScRide::type_get, nullptr, "type");
        dukglue_register_property(ctx, &ScRide::classification_get, nullptr, "classification");
        dukglue_register_property(ctx, &ScRide::name_get, &ScRide::name_set, "name");
        dukglue_register_property(ctx, &ScRide::status_get, nullptr, "status");
        dukglue_register_property(ctx, &ScRide::lifecycleFlags_get, &ScRide::lifecycleFlags_set, "lifecycleFlags");
        dukglue_register_property(ctx, &ScRide::mode_get, &ScRide::mode_set, "mode");
        dukglue_register_property(ctx, &ScRide::departFlags_get, &ScRide::departFlags_set, "departFlags");
        dukglue_register_property(ctx, &ScRide::minimumWaitingTime_get, &ScRide::minimumWaitingTime_set, "minimumWaitingTime");
        dukglue_register_property(ctx, &ScRide::maximumWaitingTime_get, &ScRide::maximumWaitingTime_set, "maximumWaitingTime");
        dukglue_register_property(ctx, &ScRide::vehicles_get, nullptr, "vehicles");
        dukglue_register_property(ctx, &ScRide::vehicleColours_get, &ScRide::vehicleColours_set, "vehicleColours");
        dukglue_register_property(ctx, &ScRide::colourSchemes_get, &ScRide::colourSchemes_set, "colourSchemes");
        dukglue_register_property(ctx, &ScRide::stationStyle_get, &ScRide::stationStyle_set, "stationStyle");
        dukglue_register_property(ctx, &ScRide::music_get, &ScRide::music_set, "music");
        dukglue_register_property(ctx, &ScRide::stations_get, nullptr, "stations");
        dukglue_register_property(ctx, &ScRide::price_get, &ScRide::price_set, "price");
        dukglue_register_property(ctx, &ScRide::excitement_get, &ScRide::excitement_set, "excitement");
        dukglue_register_property(ctx, &ScRide::intensity_get, &ScRide::intensity_set, "intensity");
        dukglue_register_property(ctx, &ScRide::nausea_get, &ScRide::nausea_set, "nausea");
        dukglue_register_property(ctx, &ScRide::totalCustomers_get, &ScRide::totalCustomers_set, "totalCustomers");
        dukglue_register_property(ctx, &ScRide::buildDate_get, &ScRide::buildDate_set, "buildDate");
        dukglue_register_property(ctx, &ScRide::age_get, nullptr, "age");
        dukglue_register_property(ctx, &ScRide::runningCost_get, &ScRide::runningCost_set, "runningCost");
        dukglue_register_property(ctx, &ScRide::totalProfit_get, &ScRide::totalProfit_set, "totalProfit");
        dukglue_register_property(ctx, &ScRide::inspectionInterval_get, &ScRide::inspectionInterval_set, "inspectionInterval");
        dukglue_register_property(ctx, &ScRide::value_get, &ScRide::value_set, "value");
        dukglue_register_property(ctx, &ScRide::downtime_get, nullptr, "downtime");
        dukglue_register_property(ctx, &ScRide::liftHillSpeed_get, &ScRide::lifthillSpeed_set, "liftHillSpeed");
        dukglue_register_property(ctx, &ScRide::maxLiftHillSpeed_get, nullptr, "maxLiftHillSpeed");
        dukglue_register_property(ctx, &ScRide::minLiftHillSpeed_get, nullptr, "minLiftHillSpeed");
    }

} // namespace OpenRCT2::Scripting

#endif
