/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ClimateObject.h"

#include "../Diagnostic.h"
#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Formatter.h"
#include "../localisation/StringIds.h"
#include "../world/Location.hpp"

#include <algorithm>

namespace OpenRCT2
{
    struct RawClimateMonth
    {
        int8_t baseTemperature;
        int8_t randomBias;
        uint8_t distribution[kNumWeatherTypes]{};
        int8_t distRotation{};
        int16_t distributionSum{};
    };

    using RawClimate = std::array<RawClimateMonth, kNumClimateMonths>;

    static constexpr const char* kWeatherTypes[] = {
        "sunny", "partiallyCloudy", "cloudy", "rain", "heavyRain", "thunder", "snow", "heavySnow", "blizzard",
    };
    static_assert(std::size(kWeatherTypes) == kNumWeatherTypes);

    static constexpr const char* kMonthKeys[] = {
        "march", "april", "may", "june", "july", "august", "september", "october",
    };
    static_assert(std::size(kMonthKeys) == kNumClimateMonths);

    void ClimateObject::Load()
    {
    }

    void ClimateObject::Unload()
    {
    }

    static RawClimate readWeatherTable(json_t& weather);
    static Climate convertRawClimate(const RawClimate& rawClimate);

    void ClimateObject::DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const
    {
        const auto dist = getYearlyDistribution();
        const auto totalSize = kNumClimateMonths * kWeatherDistSize;

        for (auto i = 0u; i < EnumValue(WeatherType::Count); i++)
        {
            auto type = WeatherType(i);
            auto imageId = ImageId(ClimateGetWeatherSpriteId(type));
            auto coords = ScreenCoordsXY(8 + (i % 3) * 35, 3 + (i / 3) * 37);
            GfxDrawSprite(rt, imageId, coords);

            auto ft = Formatter();
            ft.Add<uint16_t>(dist[i] * 100 / totalSize);
            DrawTextEllipsised(
                rt, coords + ScreenCoordsXY{ 12, 22 }, 35, STR_CLIMATE_WEATHER_PERCENT, ft,
                { FontStyle::small, TextAlignment::centre });
        }
    }

    void ClimateObject::ReadJson(IReadObjectContext* context, json_t& root)
    {
        Guard::Assert(root.is_object(), "ClimateObject::ReadJson expects parameter root to be an object");
        PopulateTablesFromJson(context, root);

        Guard::Assert(root["weather"].is_object(), "ClimateObject::ReadJson expects weather key to be an object");
        auto rawClimate = readWeatherTable(root["weather"]);
        _climate = convertRawClimate(rawClimate);

        _scriptName = Json::GetString(root["scriptName"], std::string(GetIdentifier()));

        Guard::Assert(root["properties"].is_object(), "ClimateObject::ReadJson expects properties key to be an object");
        _climate.itemThresholds.cold = Json::GetNumber(root["properties"]["coldItemTempThreshold"], 11);
        _climate.itemThresholds.warm = Json::GetNumber(root["properties"]["warmItemTempThreshold"], 21);
    }

    const TemperatureThresholds& ClimateObject::getItemThresholds() const
    {
        return _climate.itemThresholds;
    }

    const WeatherPattern& ClimateObject::getPatternForMonth(uint8_t month) const
    {
        return _climate.patterns[month];
    }

    std::string ClimateObject::getScriptName() const
    {
        return _scriptName;
    }

    YearlyDistribution ClimateObject::getYearlyDistribution() const
    {
        auto weatherTypeCount = [](const WeatherPattern& pattern, const WeatherType target) {
            auto count = 0u;
            for (auto type : pattern.distribution)
            {
                if (type == target)
                    count++;
            }
            return count;
        };

        YearlyDistribution dist{};
        for (auto m = 0; m < kNumClimateMonths; m++)
        {
            auto& pattern = getPatternForMonth(m);
            for (auto i = 0u; i < EnumValue(WeatherType::Count); i++)
                dist[i] += weatherTypeCount(pattern, WeatherType(i));
        }

        return dist;
    }

    static Climate convertRawClimate(const RawClimate& rawClimate)
    {
        Climate climate{};

        for (auto m = 0; m < kNumClimateMonths; m++)
        {
            auto& srcMonth = rawClimate[m];
            auto& dstMonth = climate.patterns[m];

            dstMonth.baseTemperature = srcMonth.baseTemperature;
            dstMonth.randomBias = srcMonth.randomBias;

            auto i = 0;
            for (auto w = 0u; w < kNumWeatherTypes; w++)
            {
                if (i > kWeatherDistSize)
                    break;

                for (auto k = 0u; k < srcMonth.distribution[w]; k++)
                {
                    dstMonth.distribution[i] = WeatherType(w);
                    i++;
                }
            }

            // Rotate weather distribution to be less biased towards sunny weather
            auto begin = std::begin(dstMonth.distribution);
            auto end = std::end(dstMonth.distribution);
            std::rotate(begin, begin + srcMonth.distRotation, end);
        }

        return climate;
    }

    static RawClimate readWeatherTable(json_t& weather)
    {
        // First read raw climate distribution from JSON
        RawClimate rawClimate{};
        for (auto i = 0u; i < kNumClimateMonths; i++)
        {
            auto& monthKey = kMonthKeys[i];
            if (!weather.contains(monthKey))
            {
                LOG_ERROR("Climate month not defined: %s", monthKey);
                continue;
            }

            auto& month = weather[monthKey];
            for (auto j = 0u; j < kNumWeatherTypes; j++)
            {
                Guard::Assert(
                    month["baseTemperature"].is_number(),
                    "ClimateObject::ReadJson expects weather months to contain baseTemperature");
                Guard::Assert(
                    month["randomBias"].is_number(), "ClimateObject::ReadJson expects weather months to contain randomBias");
                Guard::Assert(
                    month["distribution"].is_object(),
                    "ClimateObject::ReadJson expects weather months to contain distribution object");

                auto& weatherKey = kWeatherTypes[j];
                if (!month["distribution"].contains(weatherKey))
                    continue;

                auto weight = Json::GetNumber<uint8_t>(month["distribution"][weatherKey]);
                if (weight <= 0)
                    continue;

                rawClimate[i].baseTemperature = Json::GetNumber<int8_t>(month["baseTemperature"]);
                rawClimate[i].randomBias = Json::GetNumber<int8_t>(month["randomBias"]);
                rawClimate[i].distribution[j] = weight;
                rawClimate[i].distRotation = Json::GetNumber<int8_t>(month["distRotation"]);
                rawClimate[i].distributionSum += weight;
            }

            Guard::Assert(rawClimate[i].distributionSum != 0, "Month %s has no weather defined!", monthKey);
        }

        // Adjust distribution to fit internal format
        for (auto& climateMonth : rawClimate)
        {
            auto adjustedDistSum = 0u;
            for (auto i = 0u; i < kNumWeatherTypes; i++)
            {
                auto rawWeight = climateMonth.distribution[i];
                auto adjustedWeight = rawWeight * kWeatherDistSize / climateMonth.distributionSum;

                climateMonth.distribution[i] = adjustedWeight;
                adjustedDistSum += adjustedWeight;
            }

            Guard::Assert(
                adjustedDistSum == kWeatherDistSize, "Weather distribution size mismatches: %d != %d", adjustedDistSum,
                kWeatherDistSize);
        }

        return rawClimate;
    }
} // namespace OpenRCT2
