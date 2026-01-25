/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Award.h"

#include "../GameState.h"
#include "../config/Config.h"
#include "../entity/EntityList.h"
#include "../entity/Guest.h"
#include "../localisation/Formatter.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../scenario/Scenario.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"
#include "NewsItem.h"

using namespace OpenRCT2;

enum class AwardEffect : uint8_t
{
    negative,
    positive
};

struct AwardData_t
{
    StringId text;
    StringId news;
    ImageIndex sprite;
    AwardEffect effect;
};

// clang-format off
static constexpr AwardData_t AwardData[] = {
    { STR_AWARD_MOST_UNTIDY,                STR_NEWS_ITEM_AWARD_MOST_UNTIDY,          SPR_AWARD_MOST_UNTIDY,                AwardEffect::negative },
    { STR_AWARD_MOST_TIDY,                  STR_NEWS_ITEM_MOST_TIDY,                  SPR_AWARD_MOST_TIDY,                  AwardEffect::positive },
    { STR_AWARD_BEST_ROLLERCOASTERS,        STR_NEWS_ITEM_BEST_ROLLERCOASTERS,        SPR_AWARD_BEST_ROLLERCOASTERS,        AwardEffect::positive },
    { STR_AWARD_BEST_VALUE,                 STR_NEWS_ITEM_BEST_VALUE,                 SPR_AWARD_BEST_VALUE,                 AwardEffect::positive },
    { STR_AWARD_MOST_BEAUTIFUL,             STR_NEWS_ITEM_MOST_BEAUTIFUL,             SPR_AWARD_MOST_BEAUTIFUL,             AwardEffect::positive },
    { STR_AWARD_WORST_VALUE,                STR_NEWS_ITEM_WORST_VALUE,                SPR_AWARD_WORST_VALUE,                AwardEffect::negative },
    { STR_AWARD_SAFEST,                     STR_NEWS_ITEM_SAFEST,                     SPR_AWARD_SAFEST,                     AwardEffect::positive },
    { STR_AWARD_BEST_STAFF,                 STR_NEWS_ITEM_BEST_STAFF,                 SPR_AWARD_BEST_STAFF,                 AwardEffect::positive },
    { STR_AWARD_BEST_FOOD,                  STR_NEWS_ITEM_BEST_FOOD,                  SPR_AWARD_BEST_FOOD,                  AwardEffect::positive },
    { STR_AWARD_WORST_FOOD,                 STR_NEWS_ITEM_WORST_FOOD,                 SPR_AWARD_WORST_FOOD,                 AwardEffect::negative },
    { STR_AWARD_BEST_TOILETS,               STR_NEWS_ITEM_BEST_TOILETS,               SPR_AWARD_BEST_TOILETS,               AwardEffect::positive },
    { STR_AWARD_MOST_DISAPPOINTING,         STR_NEWS_ITEM_MOST_DISAPPOINTING,         SPR_AWARD_MOST_DISAPPOINTING,         AwardEffect::negative },
    { STR_AWARD_BEST_WATER_RIDES,           STR_NEWS_ITEM_BEST_WATER_RIDES,           SPR_AWARD_BEST_WATER_RIDES,           AwardEffect::positive },
    { STR_AWARD_BEST_CUSTOM_DESIGNED_RIDES, STR_NEWS_ITEM_BEST_CUSTOM_DESIGNED_RIDES, SPR_AWARD_BEST_CUSTOM_DESIGNED_RIDES, AwardEffect::positive },
    { STR_AWARD_MOST_DAZZLING_RIDE_COLOURS, STR_NEWS_ITEM_MOST_DAZZLING_RIDE_COLOURS, SPR_AWARD_MOST_DAZZLING_RIDE_COLOURS, AwardEffect::positive },
    { STR_AWARD_MOST_CONFUSING_LAYOUT,      STR_NEWS_ITEM_MOST_CONFUSING_LAYOUT,      SPR_AWARD_MOST_CONFUSING_LAYOUT,      AwardEffect::negative },
    { STR_AWARD_BEST_GENTLE_RIDES,          STR_NEWS_ITEM_BEST_GENTLE_RIDES,          SPR_AWARD_BEST_GENTLE_RIDES,          AwardEffect::positive },
};
// clang-format on

bool AwardIsPositive(AwardType type)
{
    return AwardData[EnumValue(type)].effect == AwardEffect::positive;
}

ImageIndex AwardGetSprite(AwardType type)
{
    return AwardData[EnumValue(type)].sprite;
}

StringId AwardGetText(AwardType type)
{
    return AwardData[EnumValue(type)].text;
}

StringId AwardGetNews(AwardType type)
{
    return AwardData[EnumValue(type)].news;
}

#pragma region Award checks

/** More than 1/16 of the total guests must be thinking untidy thoughts. */
static bool AwardIsDeservedMostUntidy(GameState_t& gameState, int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::mostBeautiful))
        return false;
    if (activeAwardTypes & EnumToFlag(AwardType::bestStaff))
        return false;
    if (activeAwardTypes & EnumToFlag(AwardType::mostTidy))
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

    return (negativeCount > gameState.park.numGuestsInPark / 16);
}

/** More than 1/64 of the total guests must be thinking tidy thoughts and less than 6 guests thinking untidy thoughts. */
static bool AwardIsDeservedMostTidy(GameState_t& gameState, int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::mostUntidy))
        return false;
    if (activeAwardTypes & EnumToFlag(AwardType::mostDisappointing))
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

    return (negativeCount <= 5 && positiveCount > gameState.park.numGuestsInPark / 64);
}

/** At least 6 open roller coasters. */
static bool AwardIsDeservedBestRollercoasters(GameState_t& gameState, [[maybe_unused]] int32_t activeAwardTypes)
{
    auto rollerCoasters = 0;
    for (const auto& ride : RideManager(gameState))
    {
        auto rideEntry = ride.getRideEntry();
        if (rideEntry == nullptr)
        {
            continue;
        }

        if (ride.status != RideStatus::open || (ride.lifecycleFlags & RIDE_LIFECYCLE_CRASHED))
        {
            continue;
        }

        if (!RideEntryHasCategory(*rideEntry, RideCategory::rollerCoaster))
        {
            continue;
        }

        rollerCoasters++;
    }
    return (rollerCoasters >= 6);
}

/** Entrance fee is 0.10 less than half of the total ride value. */
static bool AwardIsDeservedBestValue(GameState_t& gameState, int32_t activeAwardTypes)
{
    auto& park = gameState.park;

    if (activeAwardTypes & EnumToFlag(AwardType::worstValue))
        return false;

    if (activeAwardTypes & EnumToFlag(AwardType::mostDisappointing))
        return false;

    if ((park.flags & PARK_FLAGS_NO_MONEY) || !Park::EntranceFeeUnlocked())
        return false;

    if (park.totalRideValueForMoney < 10.00_GBP)
        return false;

    if (Park::GetEntranceFee() + 0.10_GBP >= park.totalRideValueForMoney / 2)
        return false;

    return true;
}

/** More than 1/128 of the total guests must be thinking scenic thoughts and fewer than 16 untidy thoughts. */
static bool AwardIsDeservedMostBeautiful(GameState_t& gameState, int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::mostUntidy))
        return false;
    if (activeAwardTypes & EnumToFlag(AwardType::mostDisappointing))
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

    return (negativeCount <= 15 && positiveCount > getGameState().park.numGuestsInPark / 128);
}

/** Entrance fee is more than total ride value. */
static bool AwardIsDeservedWorstValue(GameState_t& gameState, int32_t activeAwardTypes)
{
    auto& park = gameState.park;

    if (activeAwardTypes & EnumToFlag(AwardType::bestValue))
        return false;
    if (park.flags & PARK_FLAGS_NO_MONEY)
        return false;

    const auto parkEntranceFee = Park::GetEntranceFee();
    if (parkEntranceFee == 0.00_GBP)
        return false;
    if (parkEntranceFee <= park.totalRideValueForMoney)
        return false;
    return true;
}

/** No more than 2 people who think the vandalism is bad and no crashes. */
static bool AwardIsDeservedSafest(GameState_t& gameState, [[maybe_unused]] int32_t activeAwardTypes)
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
    const auto& rideManager = RideManager(gameState);
    if (std::any_of(rideManager.begin(), rideManager.end(), [](const Ride& ride) {
            return ride.lastCrashType != RIDE_CRASH_TYPE_NONE;
        }))
    {
        return false;
    }

    return true;
}

/** All staff types, at least 20 staff, one staff per 32 peeps. */
static bool AwardIsDeservedBestStaff(GameState_t& gameState, int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::mostUntidy))
        return false;

    auto staffCount = gameState.entities.GetEntityListCount(EntityType::staff);
    auto peepCount = gameState.entities.GetEntityListCount(EntityType::guest);

    return ((staffCount != 0) && staffCount >= 20 && staffCount >= peepCount / 32);
}

/** At least 7 shops, 4 unique, one shop per 128 guests and no more than 12 hungry guests. */
static bool AwardIsDeservedBestFood(GameState_t& gameState, int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::worstFood))
        return false;

    uint32_t shops = 0;
    uint32_t uniqueShops = 0;
    uint64_t shopTypes = 0;
    for (const auto& ride : RideManager(gameState))
    {
        if (ride.status != RideStatus::open)
            continue;
        if (!ride.getRideTypeDescriptor().HasFlag(RtdFlag::sellsFood))
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

    if (shops < 7 || uniqueShops < 4 || shops < getGameState().park.numGuestsInPark / 128)
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
static bool AwardIsDeservedWorstFood(GameState_t& gameState, int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::bestFood))
        return false;

    uint32_t shops = 0;
    uint32_t uniqueShops = 0;
    uint64_t shopTypes = 0;
    for (const auto& ride : RideManager(gameState))
    {
        if (ride.status != RideStatus::open)
            continue;
        if (!ride.getRideTypeDescriptor().HasFlag(RtdFlag::sellsFood))
            continue;

        shops++;
        auto rideEntry = ride.getRideEntry();
        if (rideEntry != nullptr)
        {
            if (!(shopTypes & EnumToFlag(rideEntry->shop_item[0])))
            {
                shopTypes |= EnumToFlag(rideEntry->shop_item[0]);
                uniqueShops++;
            }
        }
    }

    if (uniqueShops > 2 || shops > getGameState().park.numGuestsInPark / 256)
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
static bool AwardIsDeservedBestToilets(GameState_t& gameState, [[maybe_unused]] int32_t activeAwardTypes)
{
    // Count open toilets
    const auto& rideManager = RideManager(gameState);
    auto numToilets = static_cast<size_t>(std::count_if(rideManager.begin(), rideManager.end(), [](const Ride& ride) {
        const auto& rtd = ride.getRideTypeDescriptor();
        return rtd.specialType == RtdSpecialType::toilet && ride.status == RideStatus::open;
    }));

    // At least 4 open toilets
    if (numToilets < 4)
        return false;

    // At least one open toilet for every 128 guests
    if (numToilets < getGameState().park.numGuestsInPark / 128u)
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
static bool AwardIsDeservedMostDisappointing(GameState_t& gameState, int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::bestValue))
        return false;
    if (gameState.park.rating > 650)
        return false;

    // Count the number of disappointing rides
    auto countedRides = 0;
    auto disappointingRides = 0;
    for (const auto& ride : RideManager(gameState))
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
static bool AwardIsDeservedBestWaterRides(GameState_t& gameState, [[maybe_unused]] int32_t activeAwardTypes)
{
    auto waterRides = 0;
    for (const auto& ride : RideManager(gameState))
    {
        auto rideEntry = ride.getRideEntry();
        if (rideEntry == nullptr)
        {
            continue;
        }

        if (ride.status != RideStatus::open || (ride.lifecycleFlags & RIDE_LIFECYCLE_CRASHED))
        {
            continue;
        }

        if (!RideEntryHasCategory(*rideEntry, RideCategory::water))
        {
            continue;
        }

        waterRides++;
    }

    return (waterRides >= 6);
}

/** At least 6 custom designed rides. */
static bool AwardIsDeservedBestCustomDesignedRides(GameState_t& gameState, int32_t activeAwardTypes)
{
    if (activeAwardTypes & EnumToFlag(AwardType::mostDisappointing))
        return false;

    auto customDesignedRides = 0;
    for (const auto& ride : RideManager(gameState))
    {
        if (!ride.getRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
            continue;
        if (ride.lifecycleFlags & RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN)
            continue;
        if (ride.ratings.excitement < RideRating::make(5, 50))
            continue;
        if (ride.status != RideStatus::open || (ride.lifecycleFlags & RIDE_LIFECYCLE_CRASHED))
            continue;

        customDesignedRides++;
    }

    return (customDesignedRides >= 6);
}

static bool AwardIsDeservedMostDazzlingRideColours(GameState_t& gameState, int32_t activeAwardTypes)
{
    /** At least 5 colourful rides and more than half of the rides are colourful. */
    static constexpr OpenRCT2::Drawing::Colour dazzling_ride_colours[] = {
        OpenRCT2::Drawing::Colour::brightPurple,
        OpenRCT2::Drawing::Colour::brightGreen,
        OpenRCT2::Drawing::Colour::lightOrange,
        OpenRCT2::Drawing::Colour::brightPink,
    };

    if (activeAwardTypes & EnumToFlag(AwardType::mostDisappointing))
        return false;

    auto countedRides = 0;
    auto colourfulRides = 0;
    for (const auto& ride : RideManager(gameState))
    {
        if (!ride.getRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
            continue;

        countedRides++;

        auto mainTrackColour = ride.trackColours[0].main;
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
static bool AwardIsDeservedMostConfusingLayout(GameState_t& gameState, [[maybe_unused]] int32_t activeAwardTypes)
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
static bool AwardIsDeservedBestGentleRides(GameState_t& gameState, [[maybe_unused]] int32_t activeAwardTypes)
{
    auto gentleRides = 0;
    for (const auto& ride : RideManager(gameState))
    {
        auto rideEntry = ride.getRideEntry();
        if (rideEntry == nullptr)
        {
            continue;
        }

        if (ride.status != RideStatus::open || (ride.lifecycleFlags & RIDE_LIFECYCLE_CRASHED))
        {
            continue;
        }

        if (!RideEntryHasCategory(*rideEntry, RideCategory::gentle))
        {
            continue;
        }

        gentleRides++;
    }

    return (gentleRides >= 10);
}

using award_deserved_check = bool (*)(GameState_t& gameState, int32_t);

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

static bool AwardIsDeserved(GameState_t& gameState, AwardType awardType, int32_t activeAwardTypes)
{
    return _awardChecks[EnumValue(awardType)](gameState, activeAwardTypes);
}

#pragma endregion

void AwardReset()
{
    getGameState().park.currentAwards.clear();
}

static void AwardAdd(AwardType type)
{
    getGameState().park.currentAwards.push_back(Award{ 5u, type });
    if (Config::Get().notifications.parkAward)
    {
        News::AddItemToQueue(News::ItemType::award, AwardGetNews(type), 0, {});
    }
    Ui::GetWindowManager()->InvalidateByClass(WindowClass::parkInformation);
}

/**
 *
 *  rct2: 0x0066A86C
 */
void AwardUpdateAll()
{
    PROFILED_FUNCTION();

    auto& currentAwards = getGameState().park.currentAwards;
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
        windowMgr->InvalidateByClass(WindowClass::parkInformation);
    }

    // Only add new awards if park is open
    auto& gameState = getGameState();
    if (gameState.park.flags & PARK_FLAGS_PARK_OPEN)
    {
        // Set active award types as flags
        int32_t activeAwardTypes = 0;
        for (auto& award : currentAwards)
        {
            activeAwardTypes |= (1 << EnumValue(award.Type));
        }

        // Check if there was a free award entry
        if (currentAwards.size() < Limits::kMaxAwards)
        {
            // Get a random award type not already active
            AwardType awardType;
            do
            {
                awardType = static_cast<AwardType>((((ScenarioRand() & 0xFF) * EnumValue(AwardType::count)) >> 8) & 0xFF);
            } while (activeAwardTypes & (1 << EnumValue(awardType)));

            // Check if award is deserved
            if (AwardIsDeserved(gameState, awardType, activeAwardTypes))
            {
                AwardAdd(awardType);
            }
        }
    }
}

void AwardGrant(AwardType type)
{
    auto& currentAwards = getGameState().park.currentAwards;

    // Remove award type if already granted
    std::erase_if(currentAwards, [type](const Award& award) { return award.Type == type; });

    // Ensure there is space for the award
    if (currentAwards.size() >= Limits::kMaxAwards)
    {
        currentAwards.erase(currentAwards.begin());
    }

    AwardAdd(type);
}
