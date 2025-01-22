/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../management/Finance.h"
#include "Map.h"

#include <string>

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

struct Guest;

namespace OpenRCT2
{
    struct Date;

    namespace Park
    {
        struct ParkData final
        {
            std::string Name;
            uint64_t Flags;
            uint16_t Rating;
            uint16_t RatingHistory[kParkRatingHistorySize];
            int16_t RatingCasualtyPenalty;
            money64 EntranceFee;
            std::vector<CoordsXYZD> Entrances;
            uint32_t Size;
            money64 Value;
            money64 ValueHistory[kFinanceHistorySize];

            bool IsOpen() const;
        };

        void Initialise(OpenRCT2::GameState_t& gameState);
        void Update(OpenRCT2::GameState_t& gameState, const OpenRCT2::Date& date);

        uint32_t CalculateParkSize();
        int32_t CalculateParkRating();
        money64 CalculateParkValue();
        money64 CalculateCompanyValue();

        Guest* GenerateGuest();

        void ResetHistories(OpenRCT2::GameState_t& gameState);
        void UpdateHistories(OpenRCT2::GameState_t& gameState);
        void SetForcedRating(int32_t rating);
        int32_t GetForcedRating();

        uint32_t UpdateSize(OpenRCT2::GameState_t& gameState);

        void UpdateFences(const CoordsXY& coords);
        void UpdateFencesAroundTile(const CoordsXY& coords);

        uint8_t CalculateGuestInitialHappiness(uint8_t percentage);

        void SetOpen(bool open);
        money64 GetEntranceFee();

        bool RidePricesUnlocked();
        bool EntranceFeeUnlocked();
    } // namespace Park

} // namespace OpenRCT2
