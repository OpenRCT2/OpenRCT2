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

void ClimateObject::Load()
{
}

void ClimateObject::Unload()
{
}

struct WeatherPattern
{
    union
    {
        uint8_t weather[9];
        struct
        {
            uint8_t sunny{};
            uint8_t partiallyCloudy{};
            uint8_t cloudy{};
            uint8_t rain{};
            uint8_t heavyRain{};
            uint8_t thunder{};
            uint8_t snow{};
            uint8_t heavySnow{};
            uint8_t blizzard{};
        };
    };
};

struct RawClimateMonth
{
    int8_t baseTemperature;
    int8_t randomBias;
    WeatherPattern distribution;
    int16_t distributionSum{};
};

using RawClimate = std::array<RawClimateMonth, 8>;

static constexpr const char* kWeatherTypes[] = {
    "sunny", "partiallyCloudy", "cloudy", "rain", "heavyRain", "thunder", "snow", "heavySnow", "blizzard",
};

static constexpr const char* kMonthKeys[] = {
    "march", "april", "may", "june", "july", "august", "september", "october",
};

constexpr size_t kWeatherDistSize = 24;

void ClimateObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "ClimateObject::ReadJson expects parameter root to be an object");
    PopulateTablesFromJson(context, root);

    Guard::Assert(root["weather"].is_object(), "ClimateObject::ReadJson expects weather key to be an object");

    // First read raw climate distribution from JSON
    RawClimate rawClimate{};
    for (auto i = 0U; i < std::size(kMonthKeys); i++)
    {
        auto& monthKey = kMonthKeys[i];
        if (!root["weather"].contains(monthKey))
        {
            LOG_ERROR("Climate month not defined: %s", monthKey);
            continue;
        }

        auto& month = root["weather"][monthKey];
        for (auto j = 0U; j < std::size(kWeatherTypes); j++)
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
            {
                printf("\rmonth: %s, weather: %s, weight: N/A\n", monthKey, weatherKey);
                continue;
            }

            auto weight = Json::GetNumber<uint8_t>(month["distribution"][weatherKey]);
            if (weight <= 0)
            {
                printf("\rmonth: %s, weather: %s, weight: %d -- skipping\n", monthKey, weatherKey, weight);
                continue;
            }

            printf("\rmonth: %s, weather: %s, weight: %d\n", monthKey, weatherKey, weight);
            rawClimate[i].distribution.weather[j] = weight;
            rawClimate[i].distributionSum += weight;
        }

        Guard::Assert(rawClimate[i].distributionSum != 0, "Month %s has no weather defined!", monthKey);
    }

    // Adjust distribution to fit internal format
    for (auto& climateMonth : rawClimate)
    {
        auto adjustedDistSum = 0U;
        for (auto& weatherWeight : climateMonth.distribution.weather)
        {
            weatherWeight = weatherWeight * kWeatherDistSize / climateMonth.distributionSum;
            adjustedDistSum += weatherWeight;
        }

        Guard::Assert(
            adjustedDistSum == kWeatherDistSize - 1, "Weather distribution size mismatches: %d != %d", adjustedDistSum,
            kWeatherDistSize - 1);
    }
}
