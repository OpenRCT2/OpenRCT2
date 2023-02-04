/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Map.h"

constexpr auto MAX_ENTRANCE_FEE = 999.00_GBP;

constexpr const uint8_t ParkRatingHistoryUndefined = std::numeric_limits<uint8_t>::max();
constexpr const uint32_t GuestsInParkHistoryUndefined = std::numeric_limits<uint32_t>::max();
constexpr const uint8_t ParkNameMaxLength = 128;
constexpr const uint8_t ScenarioNameMaxLength = 128;
constexpr const uint16_t ScenarioDetailsNameMaxLength = 256;

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
    PARK_FLAGS_NO_MONEY = (1 << 11),
    PARK_FLAGS_DIFFICULT_GUEST_GENERATION = (1 << 12),
    PARK_FLAGS_PARK_FREE_ENTRY = (1 << 13),
    PARK_FLAGS_DIFFICULT_PARK_RATING = (1 << 14),
    PARK_FLAGS_LOCK_REAL_NAMES_OPTION_DEPRECATED = (1 << 15), // Deprecated now we use a persistent 'real names' setting
    PARK_FLAGS_NO_MONEY_SCENARIO = (1 << 17),                 // Deprecated, originally used in scenario editor
    PARK_FLAGS_SPRITES_INITIALISED = (1 << 18),  // After a scenario is loaded this prevents edits in the scenario editor
    PARK_FLAGS_SIX_FLAGS_DEPRECATED = (1 << 19), // Not used anymore
    PARK_FLAGS_UNLOCK_ALL_PRICES = (1u << 31),   // OpenRCT2 only!
};

struct Guest;
struct rct_ride;

namespace OpenRCT2
{
    class Date;

    class Park final
    {
    public:
        std::string Name;
        std::string PluginStorage;

        Park() = default;
        Park(const Park&) = delete;

        bool IsOpen() const;

        uint16_t GetParkRating() const;
        money64 GetParkValue() const;
        money64 GetCompanyValue() const;

        void Initialise();
        void Update(const Date& date);

        uint32_t CalculateParkSize() const;
        int32_t CalculateParkRating() const;
        money64 CalculateParkValue() const;
        money64 CalculateCompanyValue() const;
        static uint8_t CalculateGuestInitialHappiness(uint8_t percentage);

        Guest* GenerateGuest();

        void ResetHistories();
        void UpdateHistories();

    private:
        money64 CalculateRideValue(const Ride& ride) const;
        money64 CalculateTotalRideValueForMoney() const;
        uint32_t CalculateSuggestedMaxGuests() const;
        uint32_t CalculateGuestGenerationProbability() const;

        void GenerateGuests();
        Guest* GenerateGuestFromCampaign(int32_t campaign);
    };
} // namespace OpenRCT2

extern uint64_t gParkFlags;
extern uint16_t gParkRating;
extern money16 gParkEntranceFee;
extern uint32_t gParkSize;
extern money16 gLandPrice;
extern money16 gConstructionRightsPrice;

extern uint64_t gTotalAdmissions;
extern money64 gTotalIncomeFromAdmissions;

extern money64 gParkValue;
extern money64 gCompanyValue;

extern int16_t gParkRatingCasualtyPenalty;
extern uint8_t gParkRatingHistory[32];
extern uint32_t gGuestsInParkHistory[32];
extern int32_t _guestGenerationProbability;
extern uint32_t _suggestedGuestMaximum;

void ParkSetForcedRating(int32_t rating);
int32_t ParkGetForcedRating();

int32_t ParkIsOpen();
uint32_t ParkCalculateSize();

void ParkUpdateFences(const CoordsXY& coords);
void ParkUpdateFencesAroundTile(const CoordsXY& coords);

uint8_t CalculateGuestInitialHappiness(uint8_t percentage);

void ParkSetOpen(bool open);
int32_t ParkEntranceGetIndex(const CoordsXYZ& entrancePos);
money16 ParkGetEntranceFee();

bool ParkRidePricesUnlocked();
bool ParkEntranceFeeUnlocked();
