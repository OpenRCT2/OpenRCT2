/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
using OpenRCT2::GameActions::CommandFlag;
using OpenRCT2::GameActions::CommandFlags;

// Monthly research funding costs
const money64 kResearchCosts[RESEARCH_FUNDING_COUNT] = {
    0.00_GBP,   // No funding
    100.00_GBP, // Minimum funding
    200.00_GBP, // Normal funding
    400.00_GBP, // Maximum funding
};

static constexpr bool kCountTowardsCurrentExpenditure[EnumValue(ExpenditureType::count)] = {
    true,  // ExpenditureType::rideConstruction
    false, // ExpenditureType::rideRunningCosts
    true,  // ExpenditureType::landPurchase
    true,  // ExpenditureType::landscaping
    true,  // ExpenditureType::parkEntranceTickets
    true,  // ExpenditureType::parkRideTickets
    true,  // ExpenditureType::shopSales
    true,  // ExpenditureType::shopStock
    true,  // ExpenditureType::foodDrinkSales
    true,  // ExpenditureType::foodDrinkStock
    false, // ExpenditureType::wages
    true,  // ExpenditureType::marketing
    false, // ExpenditureType::research
    false, // ExpenditureType::interest
};

/**
 * Checks the condition if the game is required to use money.
 * @param flags game command flags.
 */
bool FinanceCheckMoneyRequired(CommandFlags flags)
{
    if (getGameState().park.flags & PARK_FLAGS_NO_MONEY)
        return false;
    if (isInEditorMode())
        return false;
    if (flags.has(CommandFlag::noSpend))
        return false;
    if (flags.has(CommandFlag::ghost))
        return false;
    return true;
}

/**
 * Checks if enough money is available.
 * @param cost.
 * @param flags game command flags.
 */
bool FinanceCheckAffordability(money64 cost, CommandFlags flags)
{
    return !FinanceCheckMoneyRequired(flags) || cost <= 0 || cost <= getGameState().park.cash;
}

/**
 * Pay an amount of money.
 *  rct2: 0x069C674
 * @param amount (eax)
 * @param type passed via global var 0x0141F56C (RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE), our type is that var/4.
 */
void FinancePayment(money64 amount, ExpenditureType type)
{
    auto& park = getGameState().park;
    park.cash = AddClamp(park.cash, -amount);

    park.expenditureTable[0][EnumValue(type)] -= amount;
    if (kCountTowardsCurrentExpenditure[EnumValue(type)])
    {
        // Cumulative amount of money spent this day
        park.currentExpenditure -= amount;
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

    if (getGameState().park.flags & PARK_FLAGS_NO_MONEY)
    {
        return;
    }

    for (auto peep : EntityList<Staff>())
    {
        FinancePayment(GetStaffWage(peep->AssignedStaffType) / 4, ExpenditureType::wages);
    }
}

/**
 * Pays the current research level's cost.
 * rct2: 0x00684DA5
 **/
void FinancePayResearch()
{
    const auto& gameState = getGameState();
    if (getGameState().park.flags & PARK_FLAGS_NO_MONEY)
    {
        return;
    }

    const uint8_t level = gameState.researchFundingLevel;
    FinancePayment(kResearchCosts[level] / 4, ExpenditureType::research);
}

/**
 * Pay interest on current loans.
 *  rct2: 0x0069E092
 */
void FinancePayInterest()
{
    const auto& park = getGameState().park;

    if (park.flags & PARK_FLAGS_NO_MONEY)
    {
        return;
    }

    // This variable uses the 64-bit type as the computation below can involve multiplying very large numbers
    // that will overflow money64 if the loan is greater than (1 << 31) / (5 * current_interest_rate)
    const money64 current_loan = park.bankLoan;
    const auto current_interest_rate = park.bankLoanInterestRate;
    const money64 interest_to_pay = (park.flags & PARK_FLAGS_RCT1_INTEREST) ? (current_loan / 2400)
                                                                            : (current_loan * 5 * current_interest_rate) >> 14;

    FinancePayment(interest_to_pay, ExpenditureType::interest);
}

/**
 *
 *  rct2: 0x006AC885
 */
void FinancePayRideUpkeep()
{
    PROFILED_FUNCTION();

    auto& gameState = getGameState();
    for (auto& ride : RideManager(gameState))
    {
        if (!(ride.lifecycleFlags & RIDE_LIFECYCLE_EVER_BEEN_OPENED))
        {
            ride.renew();
        }

        if (ride.status != RideStatus::closed && !(gameState.park.flags & PARK_FLAGS_NO_MONEY))
        {
            auto upkeep = ride.upkeepCost;
            if (upkeep != kMoney64Undefined)
            {
                ride.totalProfit = AddClamp(ride.totalProfit, -upkeep);
                ride.windowInvalidateFlags.set(RideInvalidateFlag::income);
                FinancePayment(upkeep, ExpenditureType::rideRunningCosts);
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
    auto& park = getGameState().park;
    for (auto i = 0; i < kFinanceHistorySize; i++)
    {
        park.cashHistory[i] = kMoney64Undefined;
        park.weeklyProfitHistory[i] = kMoney64Undefined;
        park.valueHistory[i] = kMoney64Undefined;
    }

    for (uint32_t i = 0; i < kExpenditureTableMonthCount; ++i)
    {
        for (uint32_t j = 0; j < static_cast<int32_t>(ExpenditureType::count); ++j)
        {
            park.expenditureTable[i][j] = 0;
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
    auto& park = gameState.park;

    // It only initialises the first month
    for (uint32_t i = 0; i < static_cast<int32_t>(ExpenditureType::count); i++)
    {
        park.expenditureTable[0][i] = 0;
    }

    park.currentExpenditure = 0;
    park.currentProfit = 0;

    park.weeklyProfitAverageDividend = 0;
    park.weeklyProfitAverageDivisor = 0;

    gameState.scenarioOptions.initialCash = 10000.00_GBP; // Cheat detection

    park.cash = 10000.00_GBP;
    park.bankLoan = 10000.00_GBP;
    park.maxBankLoan = 20000.00_GBP;

    park.bankLoanInterestRate = 10;
    park.value = 0;
    park.companyValue = 0;
    park.historicalProfit = 0;
    gameState.scenarioCompletedCompanyValue = kMoney64Undefined;
    park.totalAdmissions = 0;
    park.totalIncomeFromAdmissions = 0;
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
    auto& park = gameState.park;

    park.currentProfit = 7 * park.currentExpenditure;
    park.currentExpenditure = 0; // Reset daily expenditure

    money64 current_profit = 0;

    if (!(park.flags & PARK_FLAGS_NO_MONEY))
    {
        // Staff costs
        for (auto peep : EntityList<Staff>())
        {
            current_profit -= GetStaffWage(peep->AssignedStaffType);
        }

        // Research costs
        uint8_t level = gameState.researchFundingLevel;
        current_profit -= kResearchCosts[level];

        // Loan costs
        auto current_loan = park.bankLoan;
        current_profit -= current_loan / 600;

        // Ride costs
        for (auto& ride : RideManager(gameState))
        {
            if (ride.status != RideStatus::closed && ride.upkeepCost != kMoney64Undefined)
            {
                current_profit -= 2 * ride.upkeepCost;
            }
        }
    }

    // This is not equivalent to / 4 due to rounding of negative numbers
    current_profit = current_profit >> 2;

    park.currentProfit += current_profit;

    // These are related to weekly profit graph
    park.weeklyProfitAverageDividend += park.currentProfit;
    park.weeklyProfitAverageDivisor += 1;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::finances);
}

/**
 * Shift the expenditure table history one month to the left
 * If the table is full, accumulate the sum of the oldest month first
 * rct2: 0x0069DEAD
 */
void FinanceShiftExpenditureTable()
{
    auto& park = getGameState().park;

    // If kExpenditureTableMonthCount months have passed then is full, sum the oldest month
    if (GetDate().GetMonthsElapsed() >= kExpenditureTableMonthCount)
    {
        const money64 sum = std::accumulate(
            std::cbegin(park.expenditureTable[kExpenditureTableMonthCount - 1]),
            std::cend(park.expenditureTable[kExpenditureTableMonthCount - 1]), money64{});

        park.historicalProfit += sum;
    }

    // Shift the table
    for (size_t i = kExpenditureTableMonthCount - 1; i >= 1; i--)
    {
        for (size_t j = 0; j < static_cast<int32_t>(ExpenditureType::count); j++)
        {
            park.expenditureTable[i][j] = park.expenditureTable[i - 1][j];
        }
    }

    // Zero the beginning of the table, which is the new month
    for (uint32_t i = 0; i < static_cast<int32_t>(ExpenditureType::count); i++)
    {
        park.expenditureTable[0][i] = 0;
    }

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::finances);
}

/**
 *
 *  rct2: 0x0069E89B
 */
void FinanceResetCashToInitial()
{
    auto& gameState = getGameState();
    getGameState().park.cash = gameState.scenarioOptions.initialCash;
}

/**
 * Gets the last month's profit from food, drink and merchandise.
 */
money64 FinanceGetLastMonthShopProfit()
{
    money64 profit = 0;
    if (GetDate().GetMonthsElapsed() != 0)
    {
        const auto* lastMonthExpenditure = getGameState().park.expenditureTable[1];

        profit += lastMonthExpenditure[EnumValue(ExpenditureType::shopSales)];
        profit += lastMonthExpenditure[EnumValue(ExpenditureType::shopStock)];
        profit += lastMonthExpenditure[EnumValue(ExpenditureType::foodDrinkSales)];
        profit += lastMonthExpenditure[EnumValue(ExpenditureType::foodDrinkStock)];
    }
    return profit;
}
