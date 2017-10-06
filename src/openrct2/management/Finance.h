#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _FINANCE_H_
#define _FINANCE_H_

#include "../common.h"
#include "../peep/staff.h"
#include "Research.h"

typedef sint32 rct_expenditure_type;

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
#define EXPENDITURE_TABLE_TOTAL_COUNT (EXPENDITURE_TABLE_MONTH_COUNT * RCT_EXPENDITURE_TYPE_COUNT)
#define FINANCE_GRAPH_SIZE 128

#ifdef __cplusplus
extern "C" {
#endif

extern const money32 wage_table[STAFF_TYPE_COUNT];
extern const money32 research_cost_table[RESEARCH_FUNDING_COUNT];

extern money32 gInitialCash;
extern money32 gCashEncrypted;
extern money32 gBankLoan;
extern uint8 gBankLoanInterestRate;
extern money32 gMaxBankLoan;
extern money32 gCurrentExpenditure;
extern money32 gCurrentProfit;

/**
 * The total profit for the entire scenario that precedes
 * the current financial table.
 */
extern money32 gHistoricalProfit;

extern money32 gWeeklyProfitAverageDividend;
extern uint16 gWeeklyProfitAverageDivisor;
extern money32 gCashHistory[FINANCE_GRAPH_SIZE];
extern money32 gWeeklyProfitHistory[FINANCE_GRAPH_SIZE];
extern money32 gParkValueHistory[FINANCE_GRAPH_SIZE];
extern money32 gExpenditureTable[EXPENDITURE_TABLE_TOTAL_COUNT];

extern uint8 gCommandExpenditureType;

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

void finance_set_loan(money32 loan);
money32 finance_get_initial_cash();
money32 finance_get_current_loan();
money32 finance_get_maximum_loan();
money32 finance_get_current_cash();
void game_command_set_current_loan(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);

money32 finance_get_last_month_shop_profit();

#ifdef __cplusplus
}
#endif

#endif
