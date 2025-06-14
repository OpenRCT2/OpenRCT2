/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../core/EnumUtils.hpp"
#include "../entity/EntityList.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideManager.hpp"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Park.h"

#include <numeric>

using namespace OpenRCT2;

// Monthly research funding costs
const money64 research_cost_table[RESEARCH_FUNDING_COUNT] = {
    0.00_GBP,   // No funding
    100.00_GBP, // Minimum funding
    200.00_GBP, // Normal funding
    400.00_GBP, // Maximum funding
};

static constexpr int32_t dword_988E60[EnumValue(ExpenditureType::Count)] = {
    1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0,
};

/**
 * Checks the condition if the game is required to use money.
 * @param flags game command flags.
 */
bool FinanceCheckMoneyRequired(uint32_t flags)
{
    if (getGameState().park.Flags & PARK_FLAGS_NO_MONEY)
        return false;
    if (isInEditorMode())
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
    return !FinanceCheckMoneyRequired(flags) || cost <= 0 || cost <= getGameState().cash;
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
    auto& gameState = getGameState();
    gameState.cash = AddClamp<money64>(gameState.cash, -amount);

    gameState.expenditureTable[0][EnumValue(type)] -= amount;
    if (dword_988E60[EnumValue(type)] & 1)
    {
        // Cumulative amount of money spent this day
        gameState.currentExpenditure -= amount;
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

    if (getGameState().park.Flags & PARK_FLAGS_NO_MONEY)
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
    const auto& gameState = getGameState();
    if (gameState.park.Flags & PARK_FLAGS_NO_MONEY)
    {
        return;
    }

    const uint8_t level = gameState.researchFundingLevel;
    FinancePayment(research_cost_table[level] / 4, ExpenditureType::Research);
}

/**
 * Pay interest on current loans.
 *  rct2: 0x0069E092
 */
void FinancePayInterest()
{
    const auto& gameState = getGameState();

    if (gameState.park.Flags & PARK_FLAGS_NO_MONEY)
    {
        return;
    }

    // This variable uses the 64-bit type as the computation below can involve multiplying very large numbers
    // that will overflow money64 if the loan is greater than (1 << 31) / (5 * current_interest_rate)
    const money64 current_loan = gameState.bankLoan;
    const auto current_interest_rate = gameState.bankLoanInterestRate;
    const money64 interest_to_pay = (gameState.park.Flags & PARK_FLAGS_RCT1_INTEREST)
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
        if (!(ride.lifecycleFlags & RIDE_LIFECYCLE_EVER_BEEN_OPENED))
        {
            ride.renew();
        }

        if (ride.status != RideStatus::closed && !(getGameState().park.Flags & PARK_FLAGS_NO_MONEY))
        {
            auto upkeep = ride.upkeepCost;
            if (upkeep != kMoney64Undefined)
            {
                ride.totalProfit = AddClamp(ride.totalProfit, -upkeep);
                ride.windowInvalidateFlags |= RIDE_INVALIDATE_RIDE_INCOME;
                FinancePayment(upkeep, ExpenditureType::RideRunningCosts);
            }
        }

        if (ride.lastCrashType != RIDE_CRASH_TYPE_NONE)
        {
            ride.lastCrashType--;
        }
    }
}

void FinanceResetHistory()
{
    auto& gameState = getGameState();
    for (auto i = 0; i < kFinanceHistorySize; i++)
    {
        gameState.cashHistory[i] = kMoney64Undefined;
        gameState.weeklyProfitHistory[i] = kMoney64Undefined;
        gameState.park.ValueHistory[i] = kMoney64Undefined;
    }

    for (uint32_t i = 0; i < kExpenditureTableMonthCount; ++i)
    {
        for (uint32_t j = 0; j < static_cast<int32_t>(ExpenditureType::Count); ++j)
        {
            gameState.expenditureTable[i][j] = 0;
        }
    }
}

/**
 *
 *  rct2: 0x0069DEFB
 */
void FinanceInit()
{
    auto& gameState = getGameState();

    // It only initialises the first month
    for (uint32_t i = 0; i < static_cast<int32_t>(ExpenditureType::Count); i++)
    {
        gameState.expenditureTable[0][i] = 0;
    }

    gameState.currentExpenditure = 0;
    gameState.currentProfit = 0;

    gameState.weeklyProfitAverageDividend = 0;
    gameState.weeklyProfitAverageDivisor = 0;

    gameState.initialCash = 10000.00_GBP; // Cheat detection

    gameState.cash = 10000.00_GBP;
    gameState.bankLoan = 10000.00_GBP;
    gameState.maxBankLoan = 20000.00_GBP;

    gameState.bankLoanInterestRate = 10;
    gameState.park.Value = 0;
    gameState.companyValue = 0;
    gameState.historicalProfit = 0;
    gameState.scenarioCompletedCompanyValue = kMoney64Undefined;
    gameState.totalAdmissions = 0;
    gameState.totalIncomeFromAdmissions = 0;
    gameState.scenarioCompletedBy = std::string("?");
}

/**
 *
 *  rct2: 0x0069E79A
 */
void FinanceUpdateDailyProfit()
{
    PROFILED_FUNCTION();
    auto& gameState = getGameState();

    gameState.currentProfit = 7 * gameState.currentExpenditure;
    gameState.currentExpenditure = 0; // Reset daily expenditure

    money64 current_profit = 0;

    if (!(gameState.park.Flags & PARK_FLAGS_NO_MONEY))
    {
        // Staff costs
        for (auto peep : EntityList<Staff>())
        {
            current_profit -= GetStaffWage(peep->AssignedStaffType);
        }

        // Research costs
        uint8_t level = gameState.researchFundingLevel;
        current_profit -= research_cost_table[level];

        // Loan costs
        auto current_loan = gameState.bankLoan;
        current_profit -= current_loan / 600;

        // Ride costs
        for (auto& ride : GetRideManager())
        {
            if (ride.status != RideStatus::closed && ride.upkeepCost != kMoney64Undefined)
            {
                current_profit -= 2 * ride.upkeepCost;
            }
        }
    }

    // This is not equivalent to / 4 due to rounding of negative numbers
    current_profit = current_profit >> 2;

    gameState.currentProfit += current_profit;

    // These are related to weekly profit graph
    gameState.weeklyProfitAverageDividend += gameState.currentProfit;
    gameState.weeklyProfitAverageDivisor += 1;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::Finances);
}

money64 FinanceGetInitialCash()
{
    return getGameState().initialCash;
}

money64 FinanceGetCurrentLoan()
{
    return getGameState().bankLoan;
}

money64 FinanceGetMaximumLoan()
{
    return getGameState().maxBankLoan;
}

money64 FinanceGetCurrentCash()
{
    return getGameState().cash;
}

/**
 * Shift the expenditure table history one month to the left
 * If the table is full, accumulate the sum of the oldest month first
 * rct2: 0x0069DEAD
 */
void FinanceShiftExpenditureTable()
{
    auto& gameState = getGameState();
    // If kExpenditureTableMonthCount months have passed then is full, sum the oldest month
    if (GetDate().GetMonthsElapsed() >= kExpenditureTableMonthCount)
    {
        const money64 sum = std::accumulate(
            std::cbegin(gameState.expenditureTable[kExpenditureTableMonthCount - 1]),
            std::cend(gameState.expenditureTable[kExpenditureTableMonthCount - 1]), money64{});

        gameState.historicalProfit += sum;
    }

    // Shift the table
    for (size_t i = kExpenditureTableMonthCount - 1; i >= 1; i--)
    {
        for (size_t j = 0; j < static_cast<int32_t>(ExpenditureType::Count); j++)
        {
            gameState.expenditureTable[i][j] = gameState.expenditureTable[i - 1][j];
        }
    }

    // Zero the beginning of the table, which is the new month
    for (uint32_t i = 0; i < static_cast<int32_t>(ExpenditureType::Count); i++)
    {
        gameState.expenditureTable[0][i] = 0;
    }

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::Finances);
}

/**
 *
 *  rct2: 0x0069E89B
 */
void FinanceResetCashToInitial()
{
    auto& gameState = getGameState();
    gameState.cash = gameState.initialCash;
}

/**
 * Gets the last month's profit from food, drink and merchandise.
 */
money64 FinanceGetLastMonthShopProfit()
{
    money64 profit = 0;
    if (GetDate().GetMonthsElapsed() != 0)
    {
        const auto* lastMonthExpenditure = getGameState().expenditureTable[1];

        profit += lastMonthExpenditure[EnumValue(ExpenditureType::ShopSales)];
        profit += lastMonthExpenditure[EnumValue(ExpenditureType::ShopStock)];
        profit += lastMonthExpenditure[EnumValue(ExpenditureType::FoodDrinkSales)];
        profit += lastMonthExpenditure[EnumValue(ExpenditureType::FoodDrinkStock)];
    }
    return profit;
}
