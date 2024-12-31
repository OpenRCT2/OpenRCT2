/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
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

constexpr uint8_t kExpenditureTableMonthCount = 16;
constexpr uint8_t kFinanceHistorySize = 128;

constexpr uint8_t MaxBankLoanInterestRate = 255;

extern const money64 research_cost_table[RESEARCH_FUNDING_COUNT];

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
