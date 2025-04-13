/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScRide.hpp"

    #include "../../../Context.h"
    #include "../../../core/UnitConversion.h"
    #include "../../../ride/Ride.h"
    #include "../../../ride/RideData.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"
    #include "../object/ScObject.hpp"

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<uint8_t> BreakdownMap // The types of breakdowns.
        ({ { "safety_cut_out", BREAKDOWN_SAFETY_CUT_OUT },
           { "restraints_stuck_closed", BREAKDOWN_RESTRAINTS_STUCK_CLOSED },
           { "restraints_stuck_open", BREAKDOWN_RESTRAINTS_STUCK_OPEN },
           { "doors_stuck_closed", BREAKDOWN_DOORS_STUCK_CLOSED },
           { "doors_stuck_open", BREAKDOWN_DOORS_STUCK_OPEN },
           { "vehicle_malfunction", BREAKDOWN_VEHICLE_MALFUNCTION },
           { "brakes_failure", BREAKDOWN_BRAKES_FAILURE },
           { "control_failure", BREAKDOWN_CONTROL_FAILURE } });

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
            auto rideObject = GetContext()->GetObjectManager().GetLoadedObject<RideObject>(ride->subtype);
            if (rideObject != nullptr)
            {
                return std::make_shared<ScRideObject>(ObjectType::ride, ride->subtype);
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
            switch (ride->getClassification())
            {
                case RideClassification::ride:
                    return "ride";
                case RideClassification::shopOrStall:
                    return "stall";
                case RideClassification::kioskOrFacility:
                    return "facility";
            }
        }
        return "";
    }

    std::string ScRide::name_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->getName() : std::string();
    }
    void ScRide::name_set(std::string value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->customName = std::move(value);
        }
    }

    std::string ScRide::status_get() const
    {
        auto ride = GetRide();
        if (ride != nullptr)
        {
            switch (ride->status)
            {
                case RideStatus::closed:
                    return "closed";
                case RideStatus::open:
                    return "open";
                case RideStatus::testing:
                    return "testing";
                case RideStatus::simulating:
                    return "simulating";
                case RideStatus::count: // Meaningless but necessary to satisfy -Wswitch
                    return "count";
            }
        }
        return "";
    }

    uint32_t ScRide::lifecycleFlags_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->lifecycleFlags : 0;
    }

    void ScRide::lifecycleFlags_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->lifecycleFlags = value;
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
        return ride != nullptr ? ride->departFlags : 0;
    }

    void ScRide::departFlags_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->departFlags = value;
        }
    }

    uint8_t ScRide::minimumWaitingTime_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->minWaitingTime : 0;
    }

    void ScRide::minimumWaitingTime_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->minWaitingTime = value;
        }
    }

    uint8_t ScRide::maximumWaitingTime_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->maxWaitingTime : 0;
    }

    void ScRide::maximumWaitingTime_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->maxWaitingTime = value;
        }
    }

    std::vector<uint16_t> ScRide::vehicles_get() const
    {
        std::vector<uint16_t> result;
        auto ride = GetRide();
        if (ride != nullptr)
        {
            std::for_each(std::begin(ride->vehicles), std::begin(ride->vehicles) + ride->numTrains, [&](auto& veh) {
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
            for (const auto& vehicleColour : ride->vehicleColours)
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
            auto count = std::min(value.size(), std::size(ride->vehicleColours));
            for (size_t i = 0; i < count; i++)
            {
                ride->vehicleColours[i] = FromDuk<VehicleColour>(value[i]);
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
            for (const auto& trackColour : ride->trackColours)
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
            auto count = std::min(value.size(), std::size(ride->trackColours));
            for (size_t i = 0; i < count; i++)
            {
                ride->trackColours[i] = FromDuk<TrackColour>(value[i]);
            }
        }
    }

    ObjectEntryIndex ScRide::stationStyle_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->entranceStyle : 0;
    }

    void ScRide::stationStyle_set(ObjectEntryIndex value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->entranceStyle = value;
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
            for (const auto& station : ride->getStations())
            {
                result.push_back(std::make_shared<ScRideStation>(ride->id, ride->getStationIndex(&station)));
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
            auto numPrices = ride->getNumPrices();
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
            auto numPrices = std::min(value.size(), ride->getNumPrices());
            for (size_t i = 0; i < numPrices; i++)
            {
                ride->price[i] = std::clamp<money64>(value[i], kRideMinPrice, kRideMaxPrice);
            }
        }
    }

    int32_t ScRide::excitement_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->ratings.excitement : 0;
    }
    void ScRide::excitement_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->ratings.excitement = value;
        }
    }

    int32_t ScRide::intensity_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->ratings.intensity : 0;
    }
    void ScRide::intensity_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->ratings.intensity = value;
        }
    }

    int32_t ScRide::nausea_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->ratings.nausea : 0;
    }
    void ScRide::nausea_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->ratings.nausea = value;
        }
    }

    int32_t ScRide::totalCustomers_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->totalCustomers : 0;
    }
    void ScRide::totalCustomers_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->totalCustomers = value;
        }
    }

    int32_t ScRide::buildDate_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->buildDate : 0;
    }
    void ScRide::buildDate_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->buildDate = value;
        }
    }

    int32_t ScRide::age_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->getAge() : 0;
    }

    money64 ScRide::runningCost_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->upkeepCost : 0;
    }
    void ScRide::runningCost_set(money64 value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->upkeepCost = value;
        }
    }

    int32_t ScRide::totalProfit_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->totalProfit : 0;
    }
    void ScRide::totalProfit_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->totalProfit = value;
        }
    }

    uint8_t ScRide::inspectionInterval_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->inspectionInterval : 0;
    }
    void ScRide::inspectionInterval_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr)
        {
            ride->inspectionInterval = std::clamp<uint8_t>(value, RIDE_INSPECTION_EVERY_10_MINUTES, RIDE_INSPECTION_NEVER);
        }
    }

    DukValue ScRide::value_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto ride = GetRide();
        if (ride != nullptr && ride->value != kRideValueUndefined)
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
                ride->value = kRideValueUndefined;
            }
        }
    }

    Ride* ScRide::GetRide() const
    {
        return ::GetRide(_rideId);
    }

    void ScRide::SetBreakdown(const std::string& breakDown)
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr && ride->canBreakDown() && ride->status == RideStatus::open)
        {
            auto it = BreakdownMap.find(breakDown);
            if (it == BreakdownMap.end())
                return;
            RidePrepareBreakdown(*ride, it->second);
        }
    }

    void ScRide::FixBreakdown()
    {
        ThrowIfGameStateNotMutable();
        auto ride = GetRide();
        if (ride != nullptr && ride->canBreakDown() && ride->status == RideStatus::open)
        {
            RideFixBreakdown(*ride, 0);
        }
    }

    uint8_t ScRide::downtime_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->downtime : 0;
    }

    uint8_t ScRide::liftHillSpeed_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->liftHillSpeed : 0;
    }

    void ScRide::lifthillSpeed_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();

        auto ride = GetRide();
        if (ride)
        {
            ride->liftHillSpeed = value;
        }
    }

    uint8_t ScRide::maxLiftHillSpeed_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->getRideTypeDescriptor().LiftData.maximum_speed : 0;
    }

    uint8_t ScRide::minLiftHillSpeed_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->getRideTypeDescriptor().LiftData.minimum_speed : 0;
    }

    uint8_t ScRide::satisfaction_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->satisfaction * 5 : 0;
    }

    double ScRide::maxSpeed_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ToHumanReadableSpeed(ride->maxSpeed) : 0;
    }

    double ScRide::averageSpeed_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ToHumanReadableSpeed(ride->averageSpeed) : 0;
    }

    int32_t ScRide::rideTime_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->getTotalTime() : 0;
    }

    double ScRide::rideLength_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ToHumanReadableRideLength(ride->getTotalLength()) : 0;
    }

    double ScRide::maxPositiveVerticalGs_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->maxPositiveVerticalG / 100.0 : 0;
    }

    double ScRide::maxNegativeVerticalGs_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->maxNegativeVerticalG / 100.0 : 0;
    }

    double ScRide::maxLateralGs_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->maxLateralG / 100.0 : 0;
    }

    double ScRide::totalAirTime_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ToHumanReadableAirTime(ride->totalAirTime) / 100.0 : 0;
    }

    uint8_t ScRide::numDrops_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->getNumDrops() : 0;
    }

    uint8_t ScRide::numLiftHills_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->getNumPoweredLifts() : 0;
    }

    double ScRide::highestDropHeight_get() const
    {
        auto ride = GetRide();
        return ride != nullptr ? ride->highestDropHeight : 0;
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
        dukglue_register_property(ctx, &ScRide::satisfaction_get, nullptr, "satisfaction");
        dukglue_register_property(ctx, &ScRide::maxSpeed_get, nullptr, "maxSpeed");
        dukglue_register_property(ctx, &ScRide::averageSpeed_get, nullptr, "averageSpeed");
        dukglue_register_property(ctx, &ScRide::rideTime_get, nullptr, "rideTime");
        dukglue_register_property(ctx, &ScRide::rideLength_get, nullptr, "rideLength");
        dukglue_register_property(ctx, &ScRide::maxPositiveVerticalGs_get, nullptr, "maxPositiveVerticalGs");
        dukglue_register_property(ctx, &ScRide::maxNegativeVerticalGs_get, nullptr, "maxNegativeVerticalGs");
        dukglue_register_property(ctx, &ScRide::maxLateralGs_get, nullptr, "maxLateralGs");
        dukglue_register_property(ctx, &ScRide::totalAirTime_get, nullptr, "totalAirTime");
        dukglue_register_property(ctx, &ScRide::numDrops_get, nullptr, "numDrops");
        dukglue_register_property(ctx, &ScRide::numLiftHills_get, nullptr, "numLiftHills");
        dukglue_register_property(ctx, &ScRide::highestDropHeight_get, nullptr, "highestDropHeight");
        dukglue_register_method(ctx, &ScRide::SetBreakdown, "setBreakdown");
        dukglue_register_method(ctx, &ScRide::FixBreakdown, "fixBreakdown");
    }

} // namespace OpenRCT2::Scripting

#endif
