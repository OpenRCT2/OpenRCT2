/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Finance.h"

#include "../Context.h"
#include "../Game.h"
#include "../GameState.h"
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

using namespace OpenRCT2;

// Monthly research funding costs
const money64 research_cost_table[RESEARCH_FUNDING_COUNT] = {
    0.00_GBP,   // No funding
    100.00_GBP, // Minimum funding
    200.00_GBP, // Normal funding
    400.00_GBP, // Maximum funding
};

static constexpr int32_t dword_988E60[static_cast<int32_t>(ExpenditureType::Count)] = {
    1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0,
};

money64 gBankLoan;
uint8_t gBankLoanInterestRate;
money64 gMaxBankLoan;
money64 gCurrentExpenditure;
money64 gCurrentProfit;
money64 gHistoricalProfit;
money64 gCashHistory[FINANCE_GRAPH_SIZE];
money64 gParkValueHistory[FINANCE_GRAPH_SIZE];
money64 gExpenditureTable[EXPENDITURE_TABLE_MONTH_COUNT][static_cast<int32_t>(ExpenditureType::Count)];

/**
 * Checks the condition if the game is required to use money.
 * @param flags game command flags.
 */
bool FinanceCheckMoneyRequired(uint32_t flags)
{
    if (GetGameState().ParkFlags & PARK_FLAGS_NO_MONEY)
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
bool FinanceCheckAffordability(money64 cost, uint32_t flags)
{
    return !FinanceCheckMoneyRequired(flags) || cost <= 0 || cost <= GetGameState().Cash;
}

/**
 * Pay an amount of money.
 *  rct2: 0x069C674
 * @param amount (eax)
 * @param type passed via global var 0x0141F56C (RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE), our type is that var/4.
 */
void FinancePayment(money64 amount, ExpenditureType type)
{
    // overflow check
    auto& gameState = GetGameState();
    gameState.Cash = AddClamp_money64(gameState.Cash, -amount);

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
void FinancePayWages()
{
    PROFILED_FUNCTION();

    if (GetGameState().ParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return;
    }

    for (auto peep : EntityList<Staff>())
    {
        FinancePayment(GetStaffWage(peep->AssignedStaffType) / 4, ExpenditureType::Wages);
    }
}

/**
 * Pays the current research level's cost.
 * rct2: 0x00684DA5
 **/
void FinancePayResearch()
{
    if (GetGameState().ParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return;
    }

    const uint8_t level = gResearchFundingLevel;
    FinancePayment(research_cost_table[level] / 4, ExpenditureType::Research);
}

/**
 * Pay interest on current loans.
 *  rct2: 0x0069E092
 */
void FinancePayInterest()
{
    if (GetGameState().ParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return;
    }

    // This variable uses the 64-bit type as the computation below can involve multiplying very large numbers
    // that will overflow money64 if the loan is greater than (1 << 31) / (5 * current_interest_rate)
    const money64 current_loan = gBankLoan;
    const auto current_interest_rate = gBankLoanInterestRate;
    const money64 interest_to_pay = (GetGameState().ParkFlags & PARK_FLAGS_RCT1_INTEREST)
        ? (current_loan / 2400)
        : (current_loan * 5 * current_interest_rate) >> 14;

    FinancePayment(interest_to_pay, ExpenditureType::Interest);
}

/**
 *
 *  rct2: 0x006AC885
 */
void FinancePayRideUpkeep()
{
    PROFILED_FUNCTION();

    for (auto& ride : GetRideManager())
    {
        if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED))
        {
            ride.Renew();
        }

        if (ride.status != RideStatus::Closed && !(GetGameState().ParkFlags & PARK_FLAGS_NO_MONEY))
        {
            auto upkeep = ride.upkeep_cost;
            if (upkeep != MONEY64_UNDEFINED)
            {
                ride.total_profit -= upkeep;
                ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
                FinancePayment(upkeep, ExpenditureType::RideRunningCosts);
            }
        }

        if (ride.last_crash_type != RIDE_CRASH_TYPE_NONE)
        {
            ride.last_crash_type--;
        }
    }
}

void FinanceResetHistory()
{
    auto& gameState = GetGameState();
    for (int32_t i = 0; i < FINANCE_GRAPH_SIZE; i++)
    {
        gCashHistory[i] = MONEY64_UNDEFINED;
        gameState.WeeklyProfitHistory[i] = MONEY64_UNDEFINED;
        gParkValueHistory[i] = MONEY64_UNDEFINED;
    }

    for (uint32_t i = 0; i < EXPENDITURE_TABLE_MONTH_COUNT; ++i)
    {
        for (uint32_t j = 0; j < static_cast<int32_t>(ExpenditureType::Count); ++j)
        {
            gExpenditureTable[i][j] = 0;
        }
    }
}

/**
 *
 *  rct2: 0x0069DEFB
 */
void FinanceInit()
{
    auto& gameState = GetGameState();

    // It only initialises the first month
    for (uint32_t i = 0; i < static_cast<int32_t>(ExpenditureType::Count); i++)
    {
        gExpenditureTable[0][i] = 0;
    }

    gCurrentExpenditure = 0;
    gCurrentProfit = 0;

    gameState.WeeklyProfitAverageDividend = 0;
    gameState.WeeklyProfitAverageDivisor = 0;

    gameState.InitialCash = 10000.00_GBP; // Cheat detection

    gameState.Cash = 10000.00_GBP;
    gBankLoan = 10000.00_GBP;
    gMaxBankLoan = 20000.00_GBP;

    gHistoricalProfit = 0;

    gBankLoanInterestRate = 10;
    gParkValue = 0;
    gCompanyValue = 0;
    gameState.ScenarioCompletedCompanyValue = MONEY64_UNDEFINED;
    gTotalAdmissions = 0;
    gTotalIncomeFromAdmissions = 0;
    gameState.ScenarioCompletedBy = "?";
}

/**
 *
 *  rct2: 0x0069E79A
 */
void FinanceUpdateDailyProfit()
{
    PROFILED_FUNCTION();

    gCurrentProfit = 7 * gCurrentExpenditure;
    gCurrentExpenditure = 0; // Reset daily expenditure

    money64 current_profit = 0;
    auto& gameState = GetGameState();

    if (!(gameState.ParkFlags & PARK_FLAGS_NO_MONEY))
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
        auto current_loan = gBankLoan;
        current_profit -= current_loan / 600;

        // Ride costs
        for (auto& ride : GetRideManager())
        {
            if (ride.status != RideStatus::Closed && ride.upkeep_cost != MONEY64_UNDEFINED)
            {
                current_profit -= 2 * ride.upkeep_cost;
            }
        }
    }

    // This is not equivalent to / 4 due to rounding of negative numbers
    current_profit = current_profit >> 2;

    gCurrentProfit += current_profit;

    // These are related to weekly profit graph
    gameState.WeeklyProfitAverageDividend += gCurrentProfit;
    gameState.WeeklyProfitAverageDivisor += 1;

    WindowInvalidateByClass(WindowClass::Finances);
}

money64 FinanceGetInitialCash()
{
    return GetGameState().InitialCash;
}

money64 FinanceGetCurrentLoan()
{
    return gBankLoan;
}

money64 FinanceGetMaximumLoan()
{
    return gMaxBankLoan;
}

money64 FinanceGetCurrentCash()
{
    return GetGameState().Cash;
}

/**
 * Shift the expenditure table history one month to the left
 * If the table is full, accumulate the sum of the oldest month first
 * rct2: 0x0069DEAD
 */
void FinanceShiftExpenditureTable()
{
    // If EXPENDITURE_TABLE_MONTH_COUNT months have passed then is full, sum the oldest month
    if (GetDate().GetMonthsElapsed() >= EXPENDITURE_TABLE_MONTH_COUNT)
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

    WindowInvalidateByClass(WindowClass::Finances);
}

/**
 *
 *  rct2: 0x0069E89B
 */
void FinanceResetCashToInitial()
{
    auto& gameState = GetGameState();
    gameState.Cash = gameState.InitialCash;
}

/**
 * Gets the last month's profit from food, drink and merchandise.
 */
money64 FinanceGetLastMonthShopProfit()
{
    money64 profit = 0;
    if (GetDate().GetMonthsElapsed() != 0)
    {
        const auto* lastMonthExpenditure = gExpenditureTable[1];

        profit += lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::ShopSales)];
        profit += lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::ShopStock)];
        profit += lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::FoodDrinkSales)];
        profit += lastMonthExpenditure[static_cast<int32_t>(ExpenditureType::FoodDrinkStock)];
    }
    return profit;
}
