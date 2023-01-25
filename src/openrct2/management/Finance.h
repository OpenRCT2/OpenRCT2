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
#include "Research.h"

enum class ExpenditureType : int32_t
{
    RideConstruction = 0,
    RideRunningCosts,
    LandPurchase,
    Landscaping,
    ParkEntranceTickets,
    ParkRideTickets,
    ShopSales,
    ShopStock,
    FoodDrinkSales,
    FoodDrinkStock,
    Wages,
    Marketing,
    Research,
    Interest,
    Count
};

#define EXPENDITURE_TABLE_MONTH_COUNT 16
#define FINANCE_GRAPH_SIZE 128

constexpr const uint8_t MaxBankLoanInterestRate = 255;

extern const money32 research_cost_table[RESEARCH_FUNDING_COUNT];

extern money64 gInitialCash;
extern money64 gCash;
extern money64 gBankLoan;
extern uint8_t gBankLoanInterestRate;
extern money64 gMaxBankLoan;
extern money64 gCurrentExpenditure;
extern money64 gCurrentProfit;

/**
 * The total profit for the entire scenario that precedes
 * the current financial table.
 */
extern money64 gHistoricalProfit;

extern money64 gWeeklyProfitAverageDividend;
extern uint16_t gWeeklyProfitAverageDivisor;
extern money64 gCashHistory[FINANCE_GRAPH_SIZE];
extern money64 gWeeklyProfitHistory[FINANCE_GRAPH_SIZE];
extern money64 gParkValueHistory[FINANCE_GRAPH_SIZE];
extern money64 gExpenditureTable[EXPENDITURE_TABLE_MONTH_COUNT][static_cast<int32_t>(ExpenditureType::Count)];

bool FinanceCheckMoneyRequired(uint32_t flags);
bool FinanceCheckAffordability(money64 cost, uint32_t flags);
void FinancePayment(money64 amount, ExpenditureType type);
void FinancePayWages();
void FinancePayResearch();
void FinancePayInterest();
void FinancePayRideUpkeep();
void FinanceResetHistory();
void FinanceInit();
void FinanceUpdateDailyProfit();
void FinanceShiftExpenditureTable();
void FinanceResetCashToInitial();

money64 FinanceGetInitialCash();
money64 FinanceGetCurrentLoan();
money64 FinanceGetMaximumLoan();
money64 FinanceGetCurrentCash();

money64 FinanceGetLastMonthShopProfit();
