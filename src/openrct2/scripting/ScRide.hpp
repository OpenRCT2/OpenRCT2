/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
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
#    include "../ride/Ride.h"
#    include "Duktape.hpp"
#    include "ScObject.hpp"
#    include "ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    template<> inline DukValue ToDuk(duk_context* ctx, const TrackColour& value)
    {
        DukObject obj(ctx);
        obj.Set("main", value.main);
        obj.Set("additional", value.additional);
        obj.Set("supports", value.supports);
        return obj.Take();
    }

    template<> TrackColour FromDuk(const DukValue& s)
    {
        TrackColour result{};
        result.main = AsOrDefault(s["main"], 0);
        result.additional = AsOrDefault(s["additional"], 0);
        result.supports = AsOrDefault(s["supports"], 0);
        return result;
    }

    template<> inline DukValue ToDuk(duk_context* ctx, const VehicleColour& value)
    {
        DukObject obj(ctx);
        obj.Set("body", value.Body);
        obj.Set("trim", value.Trim);
        obj.Set("ternary", value.Ternary);
        return obj.Take();
    }

    template<> VehicleColour FromDuk(const DukValue& s)
    {
        VehicleColour result{};
        result.Body = AsOrDefault(s["body"], 0);
        result.Trim = AsOrDefault(s["trim"], 0);
        result.Ternary = AsOrDefault(s["ternary"], 0);
        return result;
    }

    class ScRideStation
    {
    private:
        ride_id_t _rideId = RIDE_ID_NULL;
        StationIndex _stationIndex{};

    public:
        ScRideStation(ride_id_t rideId, StationIndex stationIndex)
            : _rideId(rideId)
            , _stationIndex(stationIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScRideStation::start_get, &ScRideStation::start_set, "start");
            dukglue_register_property(ctx, &ScRideStation::length_get, &ScRideStation::length_set, "length");
            dukglue_register_property(ctx, &ScRideStation::entrance_get, &ScRideStation::entrance_set, "entrance");
            dukglue_register_property(ctx, &ScRideStation::exit_get, &ScRideStation::exit_set, "exit");
        }

    private:
        DukValue start_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto station = GetRideStation();
            if (station != nullptr)
            {
                auto start = CoordsXYZ(station->Start, station->GetBaseZ());
                return ToDuk(ctx, start);
            }
            return ToDuk(ctx, nullptr);
        }

        void start_set(const DukValue& value)
        {
            auto station = GetRideStation();
            if (station != nullptr)
            {
                auto start = FromDuk<CoordsXYZ>(value);
                station->Start = { start.x, start.y };
                station->SetBaseZ(start.z);
            }
        }

        int32_t length_get() const
        {
            auto station = GetRideStation();
            if (station != nullptr)
            {
                return station->Length;
            }
            return 0;
        }

        void length_set(int32_t value)
        {
            auto station = GetRideStation();
            if (station != nullptr)
            {
                station->Length = value;
            }
        }

        DukValue entrance_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto station = GetRideStation();
            if (station != nullptr)
            {
                return ToDuk(ctx, station->Entrance.ToCoordsXYZD());
            }
            return ToDuk(ctx, nullptr);
        }

        void entrance_set(const DukValue& value)
        {
            auto station = GetRideStation();
            if (station != nullptr)
            {
                station->Entrance = FromDuk<CoordsXYZD>(value);
            }
        }

        DukValue exit_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto station = GetRideStation();
            if (station != nullptr)
            {
                return ToDuk(ctx, station->Exit.ToCoordsXYZD());
            }
            return ToDuk(ctx, nullptr);
        }

        void exit_set(const DukValue& value)
        {
            auto station = GetRideStation();
            if (station != nullptr)
            {
                station->Exit = FromDuk<CoordsXYZD>(value);
            }
        }

        RideStation* GetRideStation() const
        {
            auto ride = get_ride(_rideId);
            if (ride != nullptr)
            {
                if (_stationIndex < std::size(ride->stations))
                {
                    return &ride->stations[_stationIndex];
                }
            }
            return nullptr;
        }
    };

    class ScRide
    {
    private:
        ride_id_t _rideId = RIDE_ID_NULL;

    public:
        ScRide(ride_id_t rideId)
            : _rideId(rideId)
        {
        }

    private:
        int32_t id_get() const
        {
            return _rideId;
        }

        std::shared_ptr<ScRideObject> object_get()
        {
            auto ride = GetRide();
            if (ride != nullptr)
            {
                auto rideObject = GetContext()->GetObjectManager().GetLoadedObject(OBJECT_TYPE_RIDE, ride->subtype);
                if (rideObject != nullptr)
                {
                    return std::make_shared<ScRideObject>(OBJECT_TYPE_RIDE, ride->subtype);
                }
            }
            return nullptr;
        }

        int32_t type_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->type : 0;
        }

        std::string classification_get() const
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

        std::string name_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->GetName() : std::string();
        }
        void name_set(std::string value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->custom_name = value;
            }
        }

        std::string status_get() const
        {
            auto ride = GetRide();
            if (ride != nullptr)
            {
                switch (ride->status)
                {
                    case RIDE_STATUS_CLOSED:
                        return "closed";
                    case RIDE_STATUS_OPEN:
                        return "open";
                    case RIDE_STATUS_TESTING:
                        return "testing";
                    case RIDE_STATUS_SIMULATING:
                        return "simulating";
                }
            }
            return "";
        }

        uint32_t lifecycleFlags_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->lifecycle_flags : 0;
        }

        void lifecycleFlags_set(uint32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->lifecycle_flags = value;
            }
        }

        uint8_t mode_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->mode : 0;
        }

        void mode_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->mode = value;
            }
        }

        uint8_t departFlags_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->depart_flags : 0;
        }

        void departFlags_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->depart_flags = value;
            }
        }

        uint8_t minimumWaitingTime_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->min_waiting_time : 0;
        }

        void minimumWaitingTime_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->min_waiting_time = value;
            }
        }

        uint8_t maximumWaitingTime_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->max_waiting_time : 0;
        }

        void maximumWaitingTime_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->max_waiting_time = value;
            }
        }

        std::vector<uint16_t> vehicles_get() const
        {
            std::vector<uint16_t> result;
            auto ride = GetRide();
            if (ride != nullptr)
            {
                result.insert(result.begin(), std::begin(ride->vehicles), std::begin(ride->vehicles) + ride->num_vehicles);
            }
            return result;
        }

        std::vector<DukValue> vehicleColours_get() const
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

        void vehicleColours_set(const std::vector<DukValue>& value)
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

        std::vector<DukValue> colourSchemes_get() const
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

        void colourSchemes_set(const std::vector<DukValue>& value)
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

        uint8_t stationStyle_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->entrance_style : 0;
        }

        void stationStyle_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->entrance_style = value;
            }
        }

        uint8_t music_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->music : 0;
        }

        void music_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->music = value;
            }
        }

        std::vector<std::shared_ptr<ScRideStation>> stations_get() const
        {
            std::vector<std::shared_ptr<ScRideStation>> result;
            auto ride = GetRide();
            if (ride != nullptr)
            {
                for (size_t i = 0; i < std::size(ride->stations); i++)
                {
                    result.push_back(std::make_shared<ScRideStation>(ride->id, static_cast<StationIndex>(i)));
                }
            }
            return result;
        }

        std::vector<int32_t> price_get() const
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

        void price_set(const std::vector<int32_t>& value)
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

        int32_t excitement_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->excitement : 0;
        }
        void excitement_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->excitement = value;
            }
        }

        int32_t intensity_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->intensity : 0;
        }
        void intensity_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->intensity = value;
            }
        }

        int32_t nausea_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->nausea : 0;
        }
        void nausea_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->nausea = value;
            }
        }

        int32_t totalCustomers_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->total_customers : 0;
        }
        void totalCustomers_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->total_customers = value;
            }
        }

        int32_t buildDate_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->build_date : 0;
        }
        void buildDate_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->build_date = value;
            }
        }

        int32_t age_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->GetAge() : 0;
        }

        int16_t runningCost_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->upkeep_cost : 0;
        }
        void runningCost_set(int16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->upkeep_cost = value;
            }
        }

        int32_t totalProfit_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->total_profit : 0;
        }
        void totalProfit_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->total_profit = value;
            }
        }

        uint8_t inspectionInterval_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->inspection_interval : 0;
        }
        void inspectionInterval_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->inspection_interval = std::clamp<uint8_t>(value, RIDE_INSPECTION_EVERY_10_MINUTES, RIDE_INSPECTION_NEVER);
            }
        }

        DukValue value_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto ride = GetRide();
            if (ride != nullptr && ride->value != RIDE_VALUE_UNDEFINED)
            {
                return ToDuk<int32_t>(ctx, ride->value);
            }
            return ToDuk(ctx, nullptr);
        }

        void value_set(const DukValue& value)
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

        Ride* GetRide() const
        {
            return get_ride(_rideId);
        }

    public:
        static void Register(duk_context* ctx)
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
            dukglue_register_property(
                ctx, &ScRide::minimumWaitingTime_get, &ScRide::minimumWaitingTime_set, "minimumWaitingTime");
            dukglue_register_property(
                ctx, &ScRide::maximumWaitingTime_get, &ScRide::maximumWaitingTime_set, "maximumWaitingTime");
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
            dukglue_register_property(
                ctx, &ScRide::inspectionInterval_get, &ScRide::inspectionInterval_set, "inspectionInterval");
            dukglue_register_property(ctx, &ScRide::value_get, &ScRide::value_set, "value");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
