/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Award.h"

#include "../GameState.h"
#include "../config/Config.h"
#include "../entity/Guest.h"
#include "../localisation/StringIds.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../scenario/Scenario.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"
#include "NewsItem.h"

using namespace OpenRCT2;

constexpr uint8_t NEGATIVE = 0;
constexpr uint8_t POSITIVE = 1;

static constexpr uint8_t AwardPositiveMap[] = {
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

static constexpr StringId AwardNewsStrings[] = {
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

bool AwardIsPositive(AwardType type)
{
    return AwardPositiveMap[EnumValue(type)];
}

#pragma region Award checks

/** More than 1/16 of the total guests must be thinking untidy thoughts. */
static bool AwardIsDeservedMostUntidy(int32_t activeAwardTypes)
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

    return (negativeCount > GetGameState().NumGuestsInPark / 16);
}

/** More than 1/64 of the total guests must be thinking tidy thoughts and less than 6 guests thinking untidy thoughts. */
static bool AwardIsDeservedMostTidy(int32_t activeAwardTypes)
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

    return (negativeCount <= 5 && positiveCount > GetGameState().NumGuestsInPark / 64);
}

/** At least 6 open roller coasters. */
static bool AwardIsDeservedBestRollercoasters([[maybe_unused]] int32_t activeAwardTypes)
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
static bool AwardIsDeservedBestValue(int32_t activeAwardTypes)
{
    auto& gameState = GetGameState();

    if (activeAwardTypes & EnumToFlag(AwardType::WorstValue))
        return false;

    if (activeAwardTypes & EnumToFlag(AwardType::MostDisappointing))
        return false;

    if ((gameState.Park.Flags & PARK_FLAGS_NO_MONEY) || !Park::EntranceFeeUnlocked())
        return false;

    if (gameState.TotalRideValueForMoney < 10.00_GBP)
        return false;

    if (Park::GetEntranceFee() + 0.10_GBP >= gameState.TotalRideValueForMoney / 2)
        return false;

    return true;
}

/** More than 1/128 of the total guests must be thinking scenic thoughts and fewer than 16 untidy thoughts. */
static bool AwardIsDeservedMostBeautiful(int32_t activeAwardTypes)
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

    return (negativeCount <= 15 && positiveCount > GetGameState().NumGuestsInPark / 128);
}

/** Entrance fee is more than total ride value. */
static bool AwardIsDeservedWorstValue(int32_t activeAwardTypes)
{
    auto& gameState = GetGameState();

    if (activeAwardTypes & EnumToFlag(AwardType::BestValue))
        return false;
    if (gameState.Park.Flags & PARK_FLAGS_NO_MONEY)
        return false;

    const auto parkEntranceFee = Park::GetEntranceFee();
    if (parkEntranceFee == 0.00_GBP)
        return false;
    if (parkEntranceFee <= gameState.TotalRideValueForMoney)
        return false;
    return true;
}

/** No more than 2 people who think the vandalism is bad and no crashes. */
static bool AwardIsDeservedSafest([[maybe_unused]] int32_t activeAwardTypes)
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
static bool AwardIsDeservedBestStaff(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::MostUntidy))
        return false;

    auto staffCount = GetEntityListCount(EntityType::Staff);
    auto peepCount = GetEntityListCount(EntityType::Guest);

    return ((staffCount != 0) && staffCount >= 20 && staffCount >= peepCount / 32);
}

/** At least 7 shops, 4 unique, one shop per 128 guests and no more than 12 hungry guests. */
static bool AwardIsDeservedBestFood(int32_t activeAwardTypes)
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
        if (!ride.GetRideTypeDescriptor().HasFlag(RtdFlag::sellsFood))
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

    if (shops < 7 || uniqueShops < 4 || shops < GetGameState().NumGuestsInPark / 128)
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
static bool AwardIsDeservedWorstFood(int32_t activeAwardTypes)
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
        if (!ride.GetRideTypeDescriptor().HasFlag(RtdFlag::sellsFood))
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

    if (uniqueShops > 2 || shops > GetGameState().NumGuestsInPark / 256)
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
static bool AwardIsDeservedBestToilets([[maybe_unused]] int32_t activeAwardTypes)
{
    // Count open toilets
    const auto& rideManager = GetRideManager();
    auto numToilets = static_cast<size_t>(std::count_if(rideManager.begin(), rideManager.end(), [](const Ride& ride) {
        const auto& rtd = ride.GetRideTypeDescriptor();
        return rtd.specialType == RtdSpecialType::toilet && ride.status == RideStatus::Open;
    }));

    // At least 4 open toilets
    if (numToilets < 4)
        return false;

    // At least one open toilet for every 128 guests
    if (numToilets < GetGameState().NumGuestsInPark / 128u)
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
static bool AwardIsDeservedMostDisappointing(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::BestValue))
        return false;
    if (GetGameState().Park.Rating > 650)
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
static bool AwardIsDeservedBestWaterRides([[maybe_unused]] int32_t activeAwardTypes)
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
static bool AwardIsDeservedBestCustomDesignedRides(int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::MostDisappointing))
        return false;

    auto customDesignedRides = 0;
    for (const auto& ride : GetRideManager())
    {
        if (!ride.GetRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
            continue;
        if (ride.lifecycle_flags & RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN)
            continue;
        if (ride.ratings.excitement < MakeRideRating(5, 50))
            continue;
        if (ride.status != RideStatus::Open || (ride.lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
            continue;

        customDesignedRides++;
    }

    return (customDesignedRides >= 6);
}

static bool AwardIsDeservedMostDazzlingRideColours(int32_t activeAwardTypes)
{
    /** At least 5 colourful rides and more than half of the rides are colourful. */
    static constexpr colour_t dazzling_ride_colours[] = {
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
        if (!ride.GetRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
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
static bool AwardIsDeservedMostConfusingLayout([[maybe_unused]] int32_t activeAwardTypes)
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
static bool AwardIsDeservedBestGentleRides([[maybe_unused]] int32_t activeAwardTypes)
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

static constexpr award_deserved_check _awardChecks[] = {
    AwardIsDeservedMostUntidy,
    AwardIsDeservedMostTidy,
    AwardIsDeservedBestRollercoasters,
    AwardIsDeservedBestValue,
    AwardIsDeservedMostBeautiful,
    AwardIsDeservedWorstValue,
    AwardIsDeservedSafest,
    AwardIsDeservedBestStaff,
    AwardIsDeservedBestFood,
    AwardIsDeservedWorstFood,
    AwardIsDeservedBestToilets,
    AwardIsDeservedMostDisappointing,
    AwardIsDeservedBestWaterRides,
    AwardIsDeservedBestCustomDesignedRides,
    AwardIsDeservedMostDazzlingRideColours,
    AwardIsDeservedMostConfusingLayout,
    AwardIsDeservedBestGentleRides,
};

static bool AwardIsDeserved(AwardType awardType, int32_t activeAwardTypes)
{
    return _awardChecks[EnumValue(awardType)](activeAwardTypes);
}

#pragma endregion

void AwardReset()
{
    GetGameState().CurrentAwards.clear();
}

/**
 *
 *  rct2: 0x0066A86C
 */
void AwardUpdateAll()
{
    PROFILED_FUNCTION();

    auto& gameState = GetGameState();
    auto& currentAwards = gameState.CurrentAwards;
    auto* windowMgr = Ui::GetWindowManager();

    // Decrease award times
    for (auto& award : currentAwards)
    {
        --award.Time;
    }

    // Remove any 0 time awards
    auto res = std::remove_if(
        std::begin(currentAwards), std::end(currentAwards), [](const Award& award) { return award.Time == 0; });
    if (res != std::end(currentAwards))
    {
        currentAwards.erase(res, std::end(currentAwards));
        windowMgr->InvalidateByClass(WindowClass::ParkInformation);
    }

    // Only add new awards if park is open
    if (gameState.Park.Flags & PARK_FLAGS_PARK_OPEN)
    {
        // Set active award types as flags
        int32_t activeAwardTypes = 0;
        for (auto& award : currentAwards)
        {
            activeAwardTypes |= (1 << EnumValue(award.Type));
        }

        // Check if there was a free award entry
        if (currentAwards.size() < OpenRCT2::Limits::kMaxAwards)
        {
            // Get a random award type not already active
            AwardType awardType;
            do
            {
                awardType = static_cast<AwardType>((((ScenarioRand() & 0xFF) * EnumValue(AwardType::Count)) >> 8) & 0xFF);
            } while (activeAwardTypes & (1 << EnumValue(awardType)));

            // Check if award is deserved
            if (AwardIsDeserved(awardType, activeAwardTypes))
            {
                // Add award
                currentAwards.push_back(Award{ 5u, awardType });
                if (Config::Get().notifications.ParkAward)
                {
                    News::AddItemToQueue(News::ItemType::Award, AwardNewsStrings[EnumValue(awardType)], 0, {});
                }
                windowMgr->InvalidateByClass(WindowClass::ParkInformation);
            }
        }
    }
}
