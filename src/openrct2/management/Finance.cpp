/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Finance.h"

#include "../Context.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Park.h"

// Monthly research funding costs
const money32 research_cost_table[RESEARCH_FUNDING_COUNT] = {
    0.00_GBP,   // No funding
    100.00_GBP, // Minimum funding
    200.00_GBP, // Normal funding
    400.00_GBP, // Maximum funding
};

static constexpr const int32_t dword_988E60[static_cast<int32_t>(ExpenditureType::Count)] = {
    1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0,
};

money64 gInitialCash;
money64 gCash;
money64 gBankLoan;
uint8_t gBankLoanInterestRate;
money64 gMaxBankLoan;
money64 gCurrentExpenditure;
money64 gCurrentProfit;
money64 gHistoricalProfit;
money64 gWeeklyProfitAverageDividend;
uint16_t gWeeklyProfitAverageDivisor;
money64 gCashHistory[FINANCE_GRAPH_SIZE];
money64 gWeeklyProfitHistory[FINANCE_GRAPH_SIZE];
money64 gParkValueHistory[FINANCE_GRAPH_SIZE];
money64 gExpenditureTable[EXPENDITURE_TABLE_MONTH_COUNT][static_cast<int32_t>(ExpenditureType::Count)];

/**
 * Checks the condition if the game is required to use money.
 * @param flags game command flags.
 */
bool finance_check_money_required(uint32_t flags)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
        return false;
    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
        return false;
    if (flags & GAME_COMMAND_FLAG_NO_SPEND)
        return false;
    if (flags & GAME_COMMAND_FLAG_GHOST)
        return false;
    return true;
}

/**
 * Checks if enough money is available.
 * @param cost.
 * @param flags game command flags.
 */
bool finance_check_affordability(money64 cost, uint32_t flags)
{
    return !finance_check_money_required(flags) || cost <= 0 || cost <= gCash;
}

/**
 * Pay an amount of money.
 *  rct2: 0x069C674
 * @param amount (eax)
 * @param type passed via global var 0x0141F56C (RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE), our type is that var/4.
 */
void finance_payment(money64 amount, ExpenditureType type)
{
    // overflow check
    gCash = add_clamp_money64(gCash, -amount);

    gExpenditureTable[0][static_cast<int32_t>(type)] -= amount;
    if (dword_988E60[static_cast<int32_t>(type)] & 1)
    {
        // Cumulative amount of money spent this day
        gCurrentExpenditure -= amount;
    }

    auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
    ContextBroadcastIntent(&intent);
}

/**
 * Pays the wages of all active staff members in the park.
 *  rct2: 0x006C18A9
 */
void finance_pay_wages()
{
    PROFILED_FUNCTION();

    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return;
    }

    for (auto peep : EntityList<Staff>())
    {
        finance_payment(GetStaffWage(peep->AssignedStaffType) / 4, ExpenditureType::Wages);
    }
}

/**
 * Pays the current research level's cost.
 * rct2: 0x00684DA5
 **/
void finance_pay_research()
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return;
    }

    const uint8_t level = gResearchFundingLevel;
    finance_payment(research_cost_table[level] / 4, ExpenditureType::Research);
}

/**
 * Pay interest on current loans.
 *  rct2: 0x0069E092
 */
void finance_pay_interest()
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return;
    }

    // This variable uses the 64-bit type as the computation below can involve multiplying very large numbers
    // that will overflow money32 if the loan is greater than (1 << 31) / (5 * current_interest_rate)
    const money64 current_loan = gBankLoan;
    const uint8_t current_interest_rate = gBankLoanInterestRate;
    const money32 interest_to_pay = (current_loan * 5 * current_interest_rate) >> 14;

    finance_payment(interest_to_pay, ExpenditureType::Interest);
}

/**
 *
 *  rct2: 0x006AC885
 */
void finance_pay_ride_upkeep()
{
    PROFILED_FUNCTION();

    for (auto& ride : GetRideManager())
    {
        if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED))
        {
            ride.Renew();
        }

        if (ride.status != RideStatus::Closed && !(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            int16_t upkeep = ride.upkeep_cost;
            if (upkeep != -1)
            {
                ride.total_profit -= upkeep;
                ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
                finance_payment(upkeep, ExpenditureType::RideRunningCosts);
            }
        }

        if (ride.last_crash_type != RIDE_CRASH_TYPE_NONE)
        {
            ride.last_crash_type--;
        }
    }
}

void finance_reset_history()
{
    for (int32_t i = 0; i < FINANCE_GRAPH_SIZE; i++)
    {
        gCashHistory[i] = MONEY64_UNDEFINED;
        gWeeklyProfitHistory[i] = MONEY64_UNDEFINED;
        gParkValueHistory[i] = MONEY64_UNDEFINED;
    }
}

/**
 *
 *  rct2: 0x0069DEFB
 */
void finance_init()
{
    // It only initialises the first month
    for (uint32_t i = 0; i < static_cast<int32_t>(ExpenditureType::Count); i++)
    {
        gExpenditureTable[0][i] = 0;
    }

    gCurrentExpenditure = 0;
    gCurrentProfit = 0;

    gWeeklyProfitAverageDividend = 0;
    gWeeklyProfitAverageDivisor = 0;

    gInitialCash = 10000.00_GBP; // Cheat detection

    gCash = 10000.00_GBP;
    gBankLoan = 10000.00_GBP;
    gMaxBankLoan = 20000.00_GBP;

    gHistoricalProfit = 0;

    gBankLoanInterestRate = 10;
    gParkValue = 0;
    gCompanyValue = 0;
    gScenarioCompletedCompanyValue = MONEY64_UNDEFINED;
    gTotalAdmissions = 0;
    gTotalIncomeFromAdmissions = 0;
    gScenarioCompletedBy = "?";
}

/**
 *
 *  rct2: 0x0069E79A
 */
void finance_update_daily_profit()
{
    PROFILED_FUNCTION();

    gCurrentProfit = 7 * gCurrentExpenditure;
    gCurrentExpenditure = 0; // Reset daily expenditure

    money32 current_profit = 0;

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        // Staff costs
        for (auto peep : EntityList<Staff>())
        {
            current_profit -= GetStaffWage(peep->AssignedStaffType);
        }

        // Research costs
        uint8_t level = gResearchFundingLevel;
        current_profit -= research_cost_table[level];

        // Loan costs
        money32 current_loan = gBankLoan;
        current_profit -= current_loan / 600;

        // Ride costs
        for (auto& ride : GetRideManager())
        {
            if (ride.status != RideStatus::Closed && ride.upkeep_cost != MONEY16_UNDEFINED)
            {
                current_profit -= 2 * ride.upkeep_cost;
            }
        }
    }

    // This is not equivalent to / 4 due to rounding of negative numbers
    current_profit = current_profit >> 2;

    gCurrentProfit += current_profit;

    // These are related to weekly profit graph
    gWeeklyProfitAverageDividend += gCurrentProfit;
    gWeeklyProfitAverageDivisor += 1;

    window_invalidate_by_class(WindowClass::Finances);
}

money64 finance_get_initial_cash()
{
    return gInitialCash;
}

money64 finance_get_current_loan()
{
    return gBankLoan;
}

money64 finance_get_maximum_loan()
{
    return gMaxBankLoan;
}

money64 finance_get_current_cash()
{
    return gCash;
}

/**
 * Shift the expenditure table history one month to the left
 * If the table is full, accumulate the sum of the oldest month first
 * rct2: 0x0069DEAD
 */
void finance_shift_expenditure_table()
{
    // If EXPENDITURE_TABLE_MONTH_COUNT months have passed then is full, sum the oldest month
    if (gDateMonthsElapsed >= EXPENDITURE_TABLE_MONTH_COUNT)
    {
        money64 sum = 0;
        for (uint32_t i = 0; i < static_cast<int32_t>(ExpenditureType::Count); i++)
        {
            sum += gExpenditureTable[EXPENDITURE_TABLE_MONTH_COUNT - 1][i];
        }
        gHistoricalProfit += sum;
    }

    // Shift the table
    for (size_t i = EXPENDITURE_TABLE_MONTH_COUNT - 1; i >= 1; i--)
    {
        for (size_t j = 0; j < static_cast<int32_t>(ExpenditureType::Count); j++)
        {
            gExpenditureTable[i][j] = gExpenditureTable[i - 1][j];
        }
    }

    // Zero the beginning of the table, which is the new month
    for (uint32_t i = 0; i < static_cast<int32_t>(ExpenditureType::Count); i++)
    {
        gExpenditureTable[0][i] = 0;
    }

    window_invalidate_by_class(WindowClass::Finances);
}

/**
 *
 *  rct2: 0x0069E89B
 */
void finance_reset_cash_to_initial()
{
    gCash = gInitialCash;
}

/**
 * Gets the last month's profit from food, drink and merchandise.
 */
money64 finance_get_last_month_shop_profit()
{
    money64 profit = 0;
    if (gDateMonthsElapsed != 0)
    {
        const auto* lastMonthExpenditure = gExpenditureTable[1];

        profit += lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::ShopSales)];
        profit += lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::ShopStock)];
        profit += lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::FoodDrinkSales)];
        profit += lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::FoodDrinkStock)];
    }
    return profit;
}
