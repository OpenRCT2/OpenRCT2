/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "Location.hpp"

#include <string>
#include <vector>

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

constexpr auto kMaxEntranceFee = 999.00_GBP;

constexpr uint16_t kParkRatingHistoryUndefined = std::numeric_limits<uint16_t>::max();
constexpr uint32_t kGuestsInParkHistoryUndefined = std::numeric_limits<uint32_t>::max();
constexpr uint8_t kParkRatingHistorySize = 32;
constexpr uint8_t kGuestsInParkHistorySize = 32;
constexpr uint8_t kParkNameMaxLength = 128;
constexpr uint8_t kScenarioNameMaxLength = 128;
constexpr uint16_t kScenarioDetailsNameMaxLength = 256;

enum : uint32_t
{
    PARK_FLAGS_PARK_OPEN = (1 << 0),
    PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT = (1 << 1),
    PARK_FLAGS_FORBID_LANDSCAPE_CHANGES = (1 << 2),
    PARK_FLAGS_FORBID_TREE_REMOVAL = (1 << 3),
    PARK_FLAGS_SHOW_REAL_GUEST_NAMES = (1 << 4),
    PARK_FLAGS_FORBID_HIGH_CONSTRUCTION = (1 << 5), // below tree height
    PARK_FLAGS_PREF_LESS_INTENSE_RIDES = (1 << 6),
    PARK_FLAGS_FORBID_MARKETING_CAMPAIGN = (1 << 7),
    PARK_FLAGS_ANTI_CHEAT_DEPRECATED = (1 << 8), // Not used anymore, used for cheat detection
    PARK_FLAGS_PREF_MORE_INTENSE_RIDES = (1 << 9),
    PARK_FLAGS_SHOW_REAL_STAFF_NAMES = (1 << 10),
    PARK_FLAGS_NO_MONEY = (1 << 11),
    PARK_FLAGS_DIFFICULT_GUEST_GENERATION = (1 << 12),
    PARK_FLAGS_PARK_FREE_ENTRY = (1 << 13),
    PARK_FLAGS_DIFFICULT_PARK_RATING = (1 << 14),
    PARK_FLAGS_LOCK_REAL_NAMES_OPTION_DEPRECATED = (1 << 15), // Deprecated now we use a persistent 'real names' setting
    PARK_FLAGS_NO_MONEY_SCENARIO = (1 << 17),                 // Deprecated, originally used in scenario editor
    PARK_FLAGS_SPRITES_INITIALISED = (1 << 18),  // After a scenario is loaded this prevents edits in the scenario editor
    PARK_FLAGS_SIX_FLAGS_DEPRECATED = (1 << 19), // Not used anymore

    PARK_FLAGS_RCT1_INTEREST = (1u << 30),     // OpenRCT2 only
    PARK_FLAGS_UNLOCK_ALL_PRICES = (1u << 31), // OpenRCT2 only
};

namespace OpenRCT2::Park
{
    struct ParkData final
    {
        std::string name;
        uint64_t flags;
        uint16_t rating;
        uint16_t ratingHistory[kParkRatingHistorySize];
        int16_t ratingCasualtyPenalty;
        money64 entranceFee;
        std::vector<CoordsXYZD> entrances;
        uint32_t size;
        money64 value;
        money64 valueHistory[kFinanceHistorySize];

        money64 cash;
        money64 cashHistory[kFinanceHistorySize];

        uint16_t weeklyProfitAverageDivisor;
        money64 weeklyProfitAverageDividend;
        money64 weeklyProfitHistory[kFinanceHistorySize];

        // The total profit for the entire scenario that precedes the current financial table.
        money64 historicalProfit;
        money64 currentProfit;
        money64 expenditureTable[kExpenditureTableMonthCount][EnumValue(ExpenditureType::count)];
        money64 currentExpenditure;

        money64 companyValue;

        uint32_t guestsInParkHistory[kGuestsInParkHistorySize];
        uint32_t numGuestsInPark;
        uint32_t numGuestsHeadingForPark;
        uint32_t numGuestsInParkLastWeek;
        uint8_t guestChangeModifier;

        /**
         * Probability out of 65535, of gaining a new guest per game tick.
         * new guests per second = 40 * (probability / 65535)
         * With a full park rating, non-overpriced entrance fee, less guests than the suggested maximum and four positive
         * awards, approximately 1 guest per second can be generated (+60 guests in one minute).
         */
        int32_t guestGenerationProbability;

        /**
         * In a difficult guest generation scenario, no guests will be generated if over this value.
         */
        uint32_t suggestedGuestMaximum;

        uint64_t totalAdmissions;
        money64 totalIncomeFromAdmissions;

        money64 totalRideValueForMoney;

        money64 bankLoan;
        money64 maxBankLoan;
        uint8_t bankLoanInterestRate;

        uint64_t samePriceThroughoutPark{};

        Drawing::Colour staffHandymanColour;
        Drawing::Colour staffMechanicColour;
        Drawing::Colour staffSecurityColour;

        uint8_t peepWarningThrottle[16];

        std::vector<Award> currentAwards;
        std::vector<MarketingCampaign> marketingCampaigns;
    };
} // namespace OpenRCT2::Park
