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

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../ride/Ride.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"
#    include "../object/ScObject.hpp"
#    include "ScRideStation.hpp"

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

    template<> inline TrackColour FromDuk(const DukValue& s)
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
        obj.Set("ternary", value.Tertiary);
        return obj.Take();
    }

    template<> inline VehicleColour FromDuk(const DukValue& s)
    {
        VehicleColour result{};
        result.Body = AsOrDefault(s["body"], 0);
        result.Trim = AsOrDefault(s["trim"], 0);
        result.Tertiary = AsOrDefault(s["ternary"], 0);
        return result;
    }

    class ScRide
    {
    private:
        RideId _rideId = RideId::GetNull();

    public:
        ScRide(RideId rideId);

    private:
        int32_t id_get() const;

        std::shared_ptr<ScRideObject> object_get();

        int32_t type_get() const;

        std::string classification_get() const;

        std::string name_get() const;
        void name_set(std::string value);

        std::string status_get() const;

        uint32_t lifecycleFlags_get() const;

        void lifecycleFlags_set(uint32_t value);

        uint8_t mode_get() const;

        void mode_set(uint8_t value);

        uint8_t departFlags_get() const;

        void departFlags_set(uint8_t value);

        uint8_t minimumWaitingTime_get() const;

        void minimumWaitingTime_set(uint8_t value);

        uint8_t maximumWaitingTime_get() const;

        void maximumWaitingTime_set(uint8_t value);

        std::vector<uint16_t> vehicles_get() const;

        std::vector<DukValue> vehicleColours_get() const;

        void vehicleColours_set(const std::vector<DukValue>& value);

        std::vector<DukValue> colourSchemes_get() const;

        void colourSchemes_set(const std::vector<DukValue>& value);

        ObjectEntryIndex stationStyle_get() const;

        void stationStyle_set(ObjectEntryIndex value);

        ObjectEntryIndex music_get() const;

        void music_set(ObjectEntryIndex value);

        std::vector<std::shared_ptr<ScRideStation>> stations_get() const;

        std::vector<int32_t> price_get() const;

        void price_set(const std::vector<int32_t>& value);

        int32_t excitement_get() const;
        void excitement_set(int32_t value);

        int32_t intensity_get() const;
        void intensity_set(int32_t value);

        int32_t nausea_get() const;
        void nausea_set(int32_t value);

        int32_t totalCustomers_get() const;
        void totalCustomers_set(int32_t value);

        int32_t buildDate_get() const;
        void buildDate_set(int32_t value);

        int32_t age_get() const;

        int16_t runningCost_get() const;
        void runningCost_set(int16_t value);

        int32_t totalProfit_get() const;
        void totalProfit_set(int32_t value);

        uint8_t inspectionInterval_get() const;
        void inspectionInterval_set(uint8_t value);

        DukValue value_get() const;

        void value_set(const DukValue& value);

        uint8_t downtime_get() const;

        uint8_t liftHillSpeed_get() const;
        void lifthillSpeed_set(uint8_t value);

        uint8_t maxLiftHillSpeed_get() const;
        uint8_t minLiftHillSpeed_get() const;

        Ride* GetRide() const;

    public:
        static void Register(duk_context* ctx);
    };
} // namespace OpenRCT2::Scripting

#endif
