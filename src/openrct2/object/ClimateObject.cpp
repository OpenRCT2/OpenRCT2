/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

using namespace OpenRCT2;

struct RawClimateMonth
{
    int8_t baseTemperature;
    int8_t randomBias;
    uint8_t distribution[kNumWeatherTypes]{};
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

void ClimateObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "ClimateObject::ReadJson expects parameter root to be an object");
    PopulateTablesFromJson(context, root);

    Guard::Assert(root["weather"].is_object(), "ClimateObject::ReadJson expects weather key to be an object");
    auto rawClimate = readWeatherTable(root["weather"]);
    _climate = convertRawClimate(rawClimate);
}

static Climate convertRawClimate(const RawClimate& rawClimate)
{
    Climate climate{};

    for (auto m = 0; m < kNumClimateMonths; m++)
    {
        auto& srcMonth = rawClimate[m];
        auto& dstMonth = climate[m];

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
