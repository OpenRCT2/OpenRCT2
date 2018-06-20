/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../ride/Ride.h"
#include "Map.h"

#define DECRYPT_MONEY(money) ((money32)rol32((money) ^ 0xF4EC9621, 13))
#define ENCRYPT_MONEY(money) ((money32)(ror32((money), 13) ^ 0xF4EC9621))

#define MAX_ENTRANCE_FEE MONEY(200,00)

struct rct_peep;

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
    PARK_FLAGS_NO_MONEY_SCENARIO = (1 << 17),  // equivalent to PARK_FLAGS_NO_MONEY, but used in scenario editor
    PARK_FLAGS_SPRITES_INITIALISED = (1 << 18), // After a scenario is loaded this prevents edits in the scenario editor
    PARK_FLAGS_SIX_FLAGS_DEPRECATED = (1 << 19), // Not used anymore
    PARK_FLAGS_UNLOCK_ALL_PRICES = (1u << 31), // OpenRCT2 only!
};

struct rct_peep;
struct rct_ride;

namespace OpenRCT2
{
    class Date;

    class Park final
    {
    public:
        Park() = default;
        Park(const Park&) = delete;

        bool IsOpen() const;

        uint16_t  GetParkRating() const;
        money32 GetParkValue() const;
        money32 GetCompanyValue() const;

        void Initialise();
        void Update(const Date &date);

        int32_t          CalculateParkSize() const;
        int32_t          CalculateParkRating() const;
        money32         CalculateParkValue() const;
        money32         CalculateCompanyValue() const;
        static uint8_t    CalculateGuestInitialHappiness(uint8_t percentage);

        rct_peep *  GenerateGuest();

        void ResetHistories();
        void UpdateHistories();

    private:
        money32     CalculateRideValue(const Ride * ride) const;
        money16     CalculateTotalRideValueForMoney() const;
        uint32_t      CalculateSuggestedMaxGuests() const;
        uint32_t      CalculateGuestGenerationProbability() const;

        void        GenerateGuests();
        rct_peep *  GenerateGuestFromCampaign(int32_t campaign);

    };
}

enum
{
    BUY_LAND_RIGHTS_FLAG_BUY_LAND,
    BUY_LAND_RIGHTS_FLAG_UNOWN_TILE,
    BUY_LAND_RIGHTS_FLAG_BUY_CONSTRUCTION_RIGHTS,
    BUY_LAND_RIGHTS_FLAG_UNOWN_CONSTRUCTION_RIGHTS,
    BUY_LAND_RIGHTS_FLAG_SET_FOR_SALE,
    BUY_LAND_RIGHTS_FLAG_SET_CONSTRUCTION_RIGHTS_FOR_SALE,
    BUY_LAND_RIGHTS_FLAG_SET_OWNERSHIP_WITH_CHECKS, // Used in scenario editor
};

extern rct_string_id gParkName;
extern uint32_t gParkNameArgs;
extern uint32_t gParkFlags;
extern uint16_t gParkRating;
extern money16 gParkEntranceFee;
extern uint16_t gParkSize;
extern money16 gLandPrice;
extern money16 gConstructionRightsPrice;

extern uint32_t gTotalAdmissions;
extern money32 gTotalIncomeFromAdmissions;

extern money32 gParkValue;
extern money32 gCompanyValue;

extern int16_t gParkRatingCasualtyPenalty;
extern uint8_t gParkRatingHistory[32];
extern uint8_t gGuestsInParkHistory[32];
extern int32_t _guestGenerationProbability;
extern int32_t _suggestedGuestMaximum;

void set_forced_park_rating(int32_t rating);
int32_t get_forced_park_rating();

int32_t park_is_open();
int32_t park_calculate_size();

void reset_park_entry();

void update_park_fences(CoordsXY coords);
void update_park_fences_around_tile(CoordsXY coords);

uint8_t calculate_guest_initial_happiness(uint8_t percentage);

void park_set_open(int32_t open);
int32_t park_entrance_get_index(int32_t x, int32_t y, int32_t z);
void park_set_name(const char *name);
void park_set_entrance_fee(money32 value);

int32_t map_buy_land_rights(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t setting, int32_t flags);

void game_command_set_park_entrance_fee(int32_t *eax, int32_t *ebx, int32_t *ecx, int32_t *edx, int32_t *esi, int32_t *edi, int32_t *ebp);
void game_command_set_park_open(int32_t *eax, int32_t *ebx, int32_t *ecx, int32_t *edx, int32_t *esi, int32_t *edi, int32_t *ebp);
void game_command_buy_land_rights(int32_t *eax, int32_t *ebx, int32_t *ecx, int32_t *edx, int32_t *esi, int32_t *edi, int32_t *ebp);

money16 park_get_entrance_fee();

bool park_ride_prices_unlocked();
bool park_entry_price_unlocked();
