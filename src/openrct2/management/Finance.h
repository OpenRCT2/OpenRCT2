/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

bool finance_check_money_required(uint32_t flags);
bool finance_check_affordability(money32 cost, uint32_t flags);
void finance_payment(money32 amount, ExpenditureType type);
void finance_pay_wages();
void finance_pay_research();
void finance_pay_interest();
void finance_pay_ride_upkeep();
void finance_reset_history();
void finance_init();
void finance_update_daily_profit();
void finance_shift_expenditure_table();
void finance_reset_cash_to_initial();

money64 finance_get_initial_cash();
money64 finance_get_current_loan();
money64 finance_get_maximum_loan();
money64 finance_get_current_cash();

money64 finance_get_last_month_shop_profit();
