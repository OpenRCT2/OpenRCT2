/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScLocalisation.hpp"
//#    include "ScLanguage.hpp"
#    include "../../../Context.h"
#    include "../../../config/Config.h"
#    include "../../ScriptEngine.h"
#    include "../../Duktape.hpp"

namespace OpenRCT2::Scripting
{
    ScLocalisation::ScLocalisation()
    {
    }

/* std::shared_ptr<ScLanguage> language_get()
    {
        return std::make_shared<ScLanguage>();
    }*/

    bool ScLocalisation::showHeightAsUnits_get() const
    {
        return gConfigGeneral.ShowHeightAsUnits;
    }

    void ScLocalisation::showHeightAsUnits_set(bool value)
    {
        gConfigGeneral.ShowHeightAsUnits = value;
    }

    std::string ScLocalisation::measurementFormat_get() const
    {
        const MeasurementFormat format = gConfigGeneral.MeasurementFormat;
        switch (format)
        {
            case MeasurementFormat::Imperial:
                return "imperial";
            case MeasurementFormat::Metric:
                return "metric";
            case MeasurementFormat::SI:
                return "si";
            default:
                return "unknown";
        }
    }

    void ScLocalisation::measurementFormat_set(std::string value)
    {
        if (value == "imperial")
            gConfigGeneral.MeasurementFormat = MeasurementFormat::Imperial;
        else if (value == "metric")
            gConfigGeneral.MeasurementFormat = MeasurementFormat::Metric;
        else if (value == "si")
            gConfigGeneral.MeasurementFormat = MeasurementFormat::SI;
    }

    std::string ScLocalisation::temperatureFormat_get() const
    {
        const TemperatureUnit unit = gConfigGeneral.TemperatureFormat;
        switch (unit)
        {
            case TemperatureUnit::Celsius:
                return "celsius";
            case TemperatureUnit::Fahrenheit:
                return "fahrenheit";
            default:
                return "unknown";
        }
    }

    void ScLocalisation::temperatureFormat_set(std::string value)
    {
        if (value == "fahrenheit")
            gConfigGeneral.TemperatureFormat = TemperatureUnit::Fahrenheit;
        else if (value == "celsius")
            gConfigGeneral.TemperatureFormat = TemperatureUnit::Celsius;
    }

    std::string ScLocalisation::dateFormat_get() const
    {
        int32_t format = gConfigGeneral.DateFormat;
        switch (format)
        {
            case 0:
                return "DMY";
            case 1:
                return "MDY";
            case 2:
                return "YMD";
            case 3:
                return "YDM";
            default:
                return "unknown";
        }
    }

    void ScLocalisation::dateFormat_set(std::string value)
    {
        if (value == "DMY")
            gConfigGeneral.DateFormat = 0;
        else if (value == "MDY")
            gConfigGeneral.DateFormat = 1;
        else if (value == "YMD")
            gConfigGeneral.DateFormat = 2;
        else if (value == "YDM")
            gConfigGeneral.DateFormat = 3;
    }

    void ScLocalisation::Register(duk_context* ctx)
    {
        //dukglue_register_property(ctx, &ScLocalisation::language_get, nullptr, "language");
        dukglue_register_property(
            ctx, &ScLocalisation::showHeightAsUnits_get, &ScLocalisation::showHeightAsUnits_set, "showHeightAsUnits");
        dukglue_register_property(
            ctx, &ScLocalisation::measurementFormat_get, &ScLocalisation::measurementFormat_set, "measurementFormat");
        dukglue_register_property(
            ctx, &ScLocalisation::temperatureFormat_get, &ScLocalisation::temperatureFormat_set, "temperatureFormat");
        dukglue_register_property(ctx, &ScLocalisation::dateFormat_get, &ScLocalisation::dateFormat_set, "dateFormat");
    }
} // namespace OpenRCT2::Scripting

#endif
