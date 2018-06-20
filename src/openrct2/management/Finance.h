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
#include "../peep/Staff.h"
#include "Research.h"

using rct_expenditure_type = int32_t;

enum {
    RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION,
    RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS,
    RCT_EXPENDITURE_TYPE_LAND_PURCHASE,
    RCT_EXPENDITURE_TYPE_LANDSCAPING,
    RCT_EXPENDITURE_TYPE_PARK_ENTRANCE_TICKETS,
    RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS,
    RCT_EXPENDITURE_TYPE_SHOP_SHOP_SALES,
    RCT_EXPENDITURE_TYPE_SHOP_STOCK,
    RCT_EXPENDITURE_TYPE_FOODDRINK_SALES,
    RCT_EXPENDITURE_TYPE_FOODDRINK_STOCK,
    RCT_EXPENDITURE_TYPE_WAGES,
    RCT_EXPENDITURE_TYPE_MARKETING,
    RCT_EXPENDITURE_TYPE_RESEARCH,
    RCT_EXPENDITURE_TYPE_INTEREST,
    RCT_EXPENDITURE_TYPE_COUNT
};

#define EXPENDITURE_TABLE_MONTH_COUNT 16
#define FINANCE_GRAPH_SIZE 128

extern const money32 wage_table[STAFF_TYPE_COUNT];
extern const money32 research_cost_table[RESEARCH_FUNDING_COUNT];

extern money32 gInitialCash;
extern money32 gCash;
extern money32 gBankLoan;
extern uint8_t gBankLoanInterestRate;
extern money32 gMaxBankLoan;
extern money32 gCurrentExpenditure;
extern money32 gCurrentProfit;

/**
 * The total profit for the entire scenario that precedes
 * the current financial table.
 */
extern money32 gHistoricalProfit;

extern money32 gWeeklyProfitAverageDividend;
extern uint16_t gWeeklyProfitAverageDivisor;
extern money32 gCashHistory[FINANCE_GRAPH_SIZE];
extern money32 gWeeklyProfitHistory[FINANCE_GRAPH_SIZE];
extern money32 gParkValueHistory[FINANCE_GRAPH_SIZE];
extern money32 gExpenditureTable[EXPENDITURE_TABLE_MONTH_COUNT][RCT_EXPENDITURE_TYPE_COUNT];

extern uint8_t gCommandExpenditureType;

void finance_payment(money32 amount, rct_expenditure_type type);
void finance_pay_wages();
void finance_pay_research();
void finance_pay_interest();
void finance_pay_ride_upkeep();
void finance_reset_history();
void finance_init();
void finance_update_daily_profit();
void finance_shift_expenditure_table();
void finance_reset_cash_to_initial();

money32 finance_get_initial_cash();
money32 finance_get_current_loan();
money32 finance_get_maximum_loan();
money32 finance_get_current_cash();

money32 finance_get_last_month_shop_profit();
