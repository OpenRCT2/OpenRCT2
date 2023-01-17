/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Award.h"

#include "../config/Config.h"
#include "../entity/Guest.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../scenario/Scenario.h"
#include "../world/Park.h"
#include "NewsItem.h"

#include <algorithm>

constexpr uint8_t NEGATIVE = 0;
constexpr uint8_t POSITIVE = 1;

static constexpr const uint8_t AwardPositiveMap[] = {
    NEGATIVE, // AwardType::MostUntidy
    POSITIVE, // AwardType::MostTidy
    POSITIVE, // AwardType::BestRollerCoasters
    POSITIVE, // AwardType::BestValue
    POSITIVE, // AwardType::MostBeautiful
    NEGATIVE, // AwardType::WorstValue
    POSITIVE, // AwardType::Safest
    POSITIVE, // AwardType::BestStaff
    POSITIVE, // AwardType::BestFood
    NEGATIVE, // AwardType::WorstFood
    POSITIVE, // AwardType::BestToilets
    NEGATIVE, // AwardType::MostDisappointing
    POSITIVE, // AwardType::BestWaterRides
    POSITIVE, // AwardType::BestCustomDesignedRides
    POSITIVE, // AwardType::MostDazzlingRideColours
    NEGATIVE, // AwardType::MostConfusingLayout
    POSITIVE, // AwardType::BestGentleRides
};

static constexpr const StringId AwardNewsStrings[] = {
    STR_NEWS_ITEM_AWARD_MOST_UNTIDY,
    STR_NEWS_ITEM_MOST_TIDY,
    STR_NEWS_ITEM_BEST_ROLLERCOASTERS,
    STR_NEWS_ITEM_BEST_VALUE,
    STR_NEWS_ITEM_MOST_BEAUTIFUL,
    STR_NEWS_ITEM_WORST_VALUE,
    STR_NEWS_ITEM_SAFEST,
    STR_NEWS_ITEM_BEST_STAFF,
    STR_NEWS_ITEM_BEST_FOOD,
    STR_NEWS_ITEM_WORST_FOOD,
    STR_NEWS_ITEM_BEST_TOILETS,
    STR_NEWS_ITEM_MOST_DISAPPOINTING,
    STR_NEWS_ITEM_BEST_WATER_RIDES,
    STR_NEWS_ITEM_BEST_CUSTOM_DESIGNED_RIDES,
    STR_NEWS_ITEM_MOST_DAZZLING_RIDE_COLOURS,
    STR_NEWS_ITEM_MOST_CONFUSING_LAYOUT,
    STR_NEWS_ITEM_BEST_GENTLE_RIDES,
};

static std::vector<Award> _currentAwards;

std::vector<Award>& GetAwards()
{
    return _currentAwards;
}

bool award_is_positive(AwardType type)
{
    return AwardPositiveMap[EnumValue(type)];
}

#pragma region Award checks

/** More than 1/16 of the total guests must be thinking untidy thoughts. */
static bool award_is_deserved_most_untidy(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::MostBeautiful))
        return false;
    if (activeAwardTypes & EnumToFlag(AwardType::BestStaff))
        return false;
    if (activeAwardTypes & EnumToFlag(AwardType::MostTidy))
        return false;

    uint32_t negativeCount = 0;
    for (auto peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark)
            continue;

        const auto& thought = std::get<0>(peep->Thoughts);
        if (thought.freshness > 5)
            continue;

        if (thought.type == PeepThoughtType::BadLitter || thought.type == PeepThoughtType::PathDisgusting
            || thought.type == PeepThoughtType::Vandalism)
        {
            negativeCount++;
        }
    }

    return (negativeCount > gNumGuestsInPark / 16);
}

/** More than 1/64 of the total guests must be thinking tidy thoughts and less than 6 guests thinking untidy thoughts. */
static bool award_is_deserved_most_tidy(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::MostUntidy))
        return false;
    if (activeAwardTypes & EnumToFlag(AwardType::MostDisappointing))
        return false;

    uint32_t positiveCount = 0;
    uint32_t negativeCount = 0;
    for (auto peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark)
            continue;

        const auto& thought = std::get<0>(peep->Thoughts);
        if (thought.freshness > 5)
            continue;

        if (thought.type == PeepThoughtType::VeryClean)
            positiveCount++;

        if (thought.type == PeepThoughtType::BadLitter || thought.type == PeepThoughtType::PathDisgusting
            || thought.type == PeepThoughtType::Vandalism)
        {
            negativeCount++;
        }
    }

    return (negativeCount <= 5 && positiveCount > gNumGuestsInPark / 64);
}

/** At least 6 open roller coasters. */
static bool award_is_deserved_best_rollercoasters([[maybe_unused]] int32_t activeAwardTypes)
{
    auto rollerCoasters = 0;
    for (const auto& ride : GetRideManager())
    {
        auto rideEntry = ride.GetRideEntry();
        if (rideEntry == nullptr)
        {
            continue;
        }

        if (ride.status != RideStatus::Open || (ride.lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
        {
            continue;
        }

        if (!RideEntryHasCategory(*rideEntry, RIDE_CATEGORY_ROLLERCOASTER))
        {
            continue;
        }

        rollerCoasters++;
    }
    return (rollerCoasters >= 6);
}

/** Entrance fee is 0.10 less than half of the total ride value. */
static bool award_is_deserved_best_value(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::WorstValue))
        return false;

    if (activeAwardTypes & EnumToFlag(AwardType::MostDisappointing))
        return false;

    if ((gParkFlags & PARK_FLAGS_NO_MONEY) || !ParkEntranceFeeUnlocked())
        return false;

    if (gTotalRideValueForMoney < 10.00_GBP)
        return false;

    if (ParkGetEntranceFee() + 0.10_GBP >= gTotalRideValueForMoney / 2)
        return false;

    return true;
}

/** More than 1/128 of the total guests must be thinking scenic thoughts and fewer than 16 untidy thoughts. */
static bool award_is_deserved_most_beautiful(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::MostUntidy))
        return false;
    if (activeAwardTypes & EnumToFlag(AwardType::MostDisappointing))
        return false;

    uint32_t positiveCount = 0;
    uint32_t negativeCount = 0;
    auto list = EntityList<Guest>();
    for (auto peep : list)
    {
        if (peep->OutsideOfPark)
            continue;

        const auto& thought = std::get<0>(peep->Thoughts);
        if (thought.freshness > 5)
            continue;

        if (thought.type == PeepThoughtType::Scenery)
            positiveCount++;

        if (thought.type == PeepThoughtType::BadLitter || thought.type == PeepThoughtType::PathDisgusting
            || thought.type == PeepThoughtType::Vandalism)
        {
            negativeCount++;
        }
    }

    return (negativeCount <= 15 && positiveCount > gNumGuestsInPark / 128);
}

/** Entrance fee is more than total ride value. */
static bool award_is_deserved_worst_value(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::BestValue))
        return false;
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
        return false;

    const auto parkEntranceFee = ParkGetEntranceFee();
    if (parkEntranceFee == 0.00_GBP)
        return false;
    if (parkEntranceFee <= gTotalRideValueForMoney)
        return false;
    return true;
}

/** No more than 2 people who think the vandalism is bad and no crashes. */
static bool award_is_deserved_safest([[maybe_unused]] int32_t activeAwardTypes)
{
    auto peepsWhoDislikeVandalism = 0;
    for (auto peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark)
            continue;

        const auto& thought = std::get<0>(peep->Thoughts);
        if (thought.freshness <= 5 && thought.type == PeepThoughtType::Vandalism)
            peepsWhoDislikeVandalism++;
    }

    if (peepsWhoDislikeVandalism > 2)
        return false;

    // Check for rides that have crashed maybe?
    const auto& rideManager = GetRideManager();
    if (std::any_of(rideManager.begin(), rideManager.end(), [](const Ride& ride) {
            return ride.last_crash_type != RIDE_CRASH_TYPE_NONE;
        }))
    {
        return false;
    }

    return true;
}

/** All staff types, at least 20 staff, one staff per 32 peeps. */
static bool award_is_deserved_best_staff(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::MostUntidy))
        return false;

    auto staffCount = GetEntityListCount(EntityType::Staff);
    auto peepCount = GetEntityListCount(EntityType::Guest);

    return ((staffCount != 0) && staffCount >= 20 && staffCount >= peepCount / 32);
}

/** At least 7 shops, 4 unique, one shop per 128 guests and no more than 12 hungry guests. */
static bool award_is_deserved_best_food(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::WorstFood))
        return false;

    uint32_t shops = 0;
    uint32_t uniqueShops = 0;
    uint64_t shopTypes = 0;
    for (const auto& ride : GetRideManager())
    {
        if (ride.status != RideStatus::Open)
            continue;
        if (!ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SELLS_FOOD))
            continue;

        shops++;
        auto rideEntry = GetRideEntryByIndex(ride.subtype);
        if (rideEntry != nullptr)
        {
            if (!(shopTypes & EnumToFlag(rideEntry->shop_item[0])))
            {
                shopTypes |= EnumToFlag(rideEntry->shop_item[0]);
                uniqueShops++;
            }
        }
    }

    if (shops < 7 || uniqueShops < 4 || shops < gNumGuestsInPark / 128)
        return false;

    // Count hungry peeps
    auto hungryPeeps = 0;
    for (auto peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark)
            continue;

        const auto& thought = std::get<0>(peep->Thoughts);
        if (thought.freshness <= 5 && thought.type == PeepThoughtType::Hungry)
            hungryPeeps++;
    }
    return (hungryPeeps <= 12);
}

/** No more than 2 unique shops, less than one shop per 256 guests and more than 15 hungry guests. */
static bool award_is_deserved_worst_food(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::BestFood))
        return false;

    uint32_t shops = 0;
    uint32_t uniqueShops = 0;
    uint64_t shopTypes = 0;
    for (const auto& ride : GetRideManager())
    {
        if (ride.status != RideStatus::Open)
            continue;
        if (!ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SELLS_FOOD))
            continue;

        shops++;
        auto rideEntry = ride.GetRideEntry();
        if (rideEntry != nullptr)
        {
            if (!(shopTypes & EnumToFlag(rideEntry->shop_item[0])))
            {
                shopTypes |= EnumToFlag(rideEntry->shop_item[0]);
                uniqueShops++;
            }
        }
    }

    if (uniqueShops > 2 || shops > gNumGuestsInPark / 256)
        return false;

    // Count hungry peeps
    auto hungryPeeps = 0;
    for (auto peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark)
            continue;

        const auto& thought = std::get<0>(peep->Thoughts);
        if (thought.freshness <= 5 && thought.type == PeepThoughtType::Hungry)
            hungryPeeps++;
    }
    return (hungryPeeps > 15);
}

/** At least 4 toilets, 1 toilet per 128 guests and no more than 16 guests who think they need the toilet. */
static bool award_is_deserved_best_toilets([[maybe_unused]] int32_t activeAwardTypes)
{
    // Count open toilets
    const auto& rideManager = GetRideManager();
    auto numToilets = static_cast<size_t>(std::count_if(rideManager.begin(), rideManager.end(), [](const Ride& ride) {
        const auto& rtd = ride.GetRideTypeDescriptor();
        return rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET) && ride.status == RideStatus::Open;
    }));

    // At least 4 open toilets
    if (numToilets < 4)
        return false;

    // At least one open toilet for every 128 guests
    if (numToilets < gNumGuestsInPark / 128u)
        return false;

    // Count number of guests who are thinking they need the toilet
    auto guestsWhoNeedToilet = 0;
    for (auto peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark)
            continue;

        const auto& thought = std::get<0>(peep->Thoughts);
        if (thought.freshness <= 5 && thought.type == PeepThoughtType::Toilet)
            guestsWhoNeedToilet++;
    }
    return (guestsWhoNeedToilet <= 16);
}

/** More than half of the rides have satisfaction <= 6 and park rating <= 650. */
static bool award_is_deserved_most_disappointing(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::BestValue))
        return false;
    if (gParkRating > 650)
        return false;

    // Count the number of disappointing rides
    auto countedRides = 0;
    auto disappointingRides = 0;
    for (const auto& ride : GetRideManager())
    {
        if (RideHasRatings(ride) && ride.popularity != 0xFF)
        {
            countedRides++;
            if (ride.popularity <= 6)
            {
                disappointingRides++;
            }
        }
    }

    // Half of the rides are disappointing
    return (disappointingRides >= countedRides / 2);
}

/** At least 6 open water rides. */
static bool award_is_deserved_best_water_rides([[maybe_unused]] int32_t activeAwardTypes)
{
    auto waterRides = 0;
    for (const auto& ride : GetRideManager())
    {
        auto rideEntry = ride.GetRideEntry();
        if (rideEntry == nullptr)
        {
            continue;
        }

        if (ride.status != RideStatus::Open || (ride.lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
        {
            continue;
        }

        if (!RideEntryHasCategory(*rideEntry, RIDE_CATEGORY_WATER))
        {
            continue;
        }

        waterRides++;
    }

    return (waterRides >= 6);
}

/** At least 6 custom designed rides. */
static bool award_is_deserved_best_custom_designed_rides(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::MostDisappointing))
        return false;

    auto customDesignedRides = 0;
    for (const auto& ride : GetRideManager())
    {
        if (!ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
            continue;
        if (ride.lifecycle_flags & RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN)
            continue;
        if (ride.excitement < RIDE_RATING(5, 50))
            continue;
        if (ride.status != RideStatus::Open || (ride.lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
            continue;

        customDesignedRides++;
    }

    return (customDesignedRides >= 6);
}

static bool award_is_deserved_most_dazzling_ride_colours(int32_t activeAwardTypes)
{
    /** At least 5 colourful rides and more than half of the rides are colourful. */
    static constexpr const colour_t dazzling_ride_colours[] = {
        COLOUR_BRIGHT_PURPLE,
        COLOUR_BRIGHT_GREEN,
        COLOUR_LIGHT_ORANGE,
        COLOUR_BRIGHT_PINK,
    };

    if (activeAwardTypes & EnumToFlag(AwardType::MostDisappointing))
        return false;

    auto countedRides = 0;
    auto colourfulRides = 0;
    for (const auto& ride : GetRideManager())
    {
        if (!ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
            continue;

        countedRides++;

        auto mainTrackColour = ride.track_colour[0].main;
        for (auto dazzling_ride_colour : dazzling_ride_colours)
        {
            if (mainTrackColour == dazzling_ride_colour)
            {
                colourfulRides++;
                break;
            }
        }
    }

    return (colourfulRides >= 5 && colourfulRides >= countedRides - colourfulRides);
}

/** At least 10 peeps and more than 1/64 of total guests are lost or can't find something. */
static bool award_is_deserved_most_confusing_layout([[maybe_unused]] int32_t activeAwardTypes)
{
    uint32_t peepsCounted = 0;
    uint32_t peepsLost = 0;
    for (auto peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark)
            continue;

        peepsCounted++;
        const auto& thought = std::get<0>(peep->Thoughts);
        if (thought.freshness <= 5 && (thought.type == PeepThoughtType::Lost || thought.type == PeepThoughtType::CantFind))
            peepsLost++;
    }

    return (peepsLost >= 10 && peepsLost >= peepsCounted / 64);
}

/** At least 10 open gentle rides. */
static bool award_is_deserved_best_gentle_rides([[maybe_unused]] int32_t activeAwardTypes)
{
    auto gentleRides = 0;
    for (const auto& ride : GetRideManager())
    {
        auto rideEntry = ride.GetRideEntry();
        if (rideEntry == nullptr)
        {
            continue;
        }

        if (ride.status != RideStatus::Open || (ride.lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
        {
            continue;
        }

        if (!RideEntryHasCategory(*rideEntry, RIDE_CATEGORY_GENTLE))
        {
            continue;
        }

        gentleRides++;
    }

    return (gentleRides >= 10);
}

using award_deserved_check = bool (*)(int32_t);

static constexpr const award_deserved_check _awardChecks[] = {
    award_is_deserved_most_untidy,
    award_is_deserved_most_tidy,
    award_is_deserved_best_rollercoasters,
    award_is_deserved_best_value,
    award_is_deserved_most_beautiful,
    award_is_deserved_worst_value,
    award_is_deserved_safest,
    award_is_deserved_best_staff,
    award_is_deserved_best_food,
    award_is_deserved_worst_food,
    award_is_deserved_best_toilets,
    award_is_deserved_most_disappointing,
    award_is_deserved_best_water_rides,
    award_is_deserved_best_custom_designed_rides,
    award_is_deserved_most_dazzling_ride_colours,
    award_is_deserved_most_confusing_layout,
    award_is_deserved_best_gentle_rides,
};

static bool award_is_deserved(AwardType awardType, int32_t activeAwardTypes)
{
    return _awardChecks[EnumValue(awardType)](activeAwardTypes);
}

#pragma endregion

void award_reset()
{
    _currentAwards.clear();
}

/**
 *
 *  rct2: 0x0066A86C
 */
void award_update_all()
{
    PROFILED_FUNCTION();

    // Decrease award times
    for (auto& award : _currentAwards)
    {
        --award.Time;
    }
    // Remove any 0 time awards
    auto res = std::remove_if(
        std::begin(_currentAwards), std::end(_currentAwards), [](const Award& award) { return award.Time == 0; });
    if (res != std::end(_currentAwards))
    {
        _currentAwards.erase(res, std::end(_currentAwards));
        WindowInvalidateByClass(WindowClass::ParkInformation);
    }

    // Only add new awards if park is open
    if (gParkFlags & PARK_FLAGS_PARK_OPEN)
    {
        // Set active award types as flags
        int32_t activeAwardTypes = 0;
        for (auto& award : _currentAwards)
        {
            activeAwardTypes |= (1 << EnumValue(award.Type));
        }

        // Check if there was a free award entry
        if (_currentAwards.size() < OpenRCT2::Limits::MaxAwards)
        {
            // Get a random award type not already active
            AwardType awardType;
            do
            {
                awardType = static_cast<AwardType>((((scenario_rand() & 0xFF) * EnumValue(AwardType::Count)) >> 8) & 0xFF);
            } while (activeAwardTypes & (1 << EnumValue(awardType)));

            // Check if award is deserved
            if (award_is_deserved(awardType, activeAwardTypes))
            {
                // Add award
                _currentAwards.push_back(Award{ 5u, awardType });
                if (gConfigNotifications.ParkAward)
                {
                    News::AddItemToQueue(News::ItemType::Award, AwardNewsStrings[EnumValue(awardType)], 0, {});
                }
                WindowInvalidateByClass(WindowClass::ParkInformation);
            }
        }
    }
}
