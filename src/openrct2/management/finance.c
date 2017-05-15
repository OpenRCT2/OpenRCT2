#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../game.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../peep/peep.h"
#include "../ride/ride.h"
#include "../util/util.h"
#include "../world/park.h"
#include "../world/sprite.h"
#include "finance.h"

/**
 * Monthly staff wages
 *
 * rct2: 0x00992A00
 */
const money32 wage_table[4] = {
	MONEY(50,00),		// Handyman
	MONEY(80,00),		// Mechanic
	MONEY(60,00),		// Security guard
	MONEY(55,00)		// Entertainer
};

// Monthly research funding costs
const money32 research_cost_table[4] = {
	MONEY(  0,00),		// No funding
	MONEY(100,00),		// Minimum funding
	MONEY(200,00),		// Normal funding
	MONEY(400,00)		// Maximum funding
};

sint32 dword_988E60[] = { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0 };

money32 gInitialCash;
money32 gCashEncrypted;
money32 gBankLoan;
uint8 gBankLoanInterestRate;
money32 gMaxBankLoan;
money32 gCurrentExpenditure;
money32 gCurrentProfit;
money32 gHistoricalProfit;
money32 gWeeklyProfitAverageDividend;
uint16 gWeeklyProfitAverageDivisor;
money32 gCashHistory[128];
money32 gWeeklyProfitHistory[128];
money32 gParkValueHistory[128];
money32 gExpenditureTable[EXPENDITURE_TABLE_TOTAL_COUNT];

uint8 gCommandExpenditureType;

/**
 * Pay an amount of money.
 *  rct2: 0x069C674
 * @param amount (eax)
 * @param type passed via global var 0x0141F56C (RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE), our type is that var/4.
 */
void finance_payment(money32 amount, rct_expenditure_type type)
{
	money32 cur_money = DECRYPT_MONEY(gCashEncrypted);
	money32 new_money;

	//overflow check
	new_money = add_clamp_money32(cur_money, -amount);

	gCashEncrypted = ENCRYPT_MONEY(new_money);
	gExpenditureTable[type] -= amount;
	if (dword_988E60[type] & 1)
		gCurrentExpenditure -= amount; // Cumulative amount of money spent this day


	gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_MONEY;
	window_invalidate_by_class(WC_FINANCES);
}

/**
 * Pays the wages of all active staff members in the park.
 *  rct2: 0x006C18A9
 */
void finance_pay_wages()
{
	rct_peep* peep;
	uint16 spriteIndex;

	if (gParkFlags & PARK_FLAGS_NO_MONEY)
		return;

	FOR_ALL_STAFF(spriteIndex, peep)
		finance_payment(wage_table[peep->staff_type] / 4, RCT_EXPENDITURE_TYPE_WAGES);
}

/**
* Pays the current research level's cost.
* rct2: 0x00684DA5
**/
void finance_pay_research()
{
	uint8 level;

	if (gParkFlags & PARK_FLAGS_NO_MONEY)
		return;

	level = gResearchFundingLevel;
	finance_payment(research_cost_table[level] / 4, RCT_EXPENDITURE_TYPE_RESEARCH);
}

/**
 * Pay interest on current loans.
 *  rct2: 0x0069E092
 */
void finance_pay_interest()
{
	money32 current_loan = gBankLoan;
	sint16 current_interest = gBankLoanInterestRate;
	money32 tempcost = (current_loan * 5 * current_interest) >> 14; // (5 * interest) / 2^14 is pretty close to

	if (gParkFlags & PARK_FLAGS_NO_MONEY)
		return;

	finance_payment(tempcost, RCT_EXPENDITURE_TYPE_INTEREST);
}

/**
 *
 *  rct2: 0x006AC885
 */
void finance_pay_ride_upkeep()
{
	sint32 i;
	rct_ride* ride;

	FOR_ALL_RIDES(i, ride) {
		if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)) {
			ride->build_date = gDateMonthsElapsed;
			ride->reliability = RIDE_INITIAL_RELIABILITY;
		}

		if (ride->status != RIDE_STATUS_CLOSED && !(gParkFlags & PARK_FLAGS_NO_MONEY)) {
			sint16 upkeep = ride->upkeep_cost;
			if (upkeep != -1) {
				ride->total_profit -= upkeep;
				ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
				finance_payment(upkeep, RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS);
			}
		}

		if (ride->last_crash_type != RIDE_CRASH_TYPE_NONE) {
			ride->last_crash_type--;
		}
	}
}

void finance_reset_history()
{
	for (sint32 i = 0; i < 128; i++) {
		gCashHistory[i] = MONEY32_UNDEFINED;
		gWeeklyProfitHistory[i] = MONEY32_UNDEFINED;
		gParkValueHistory[i] = MONEY32_UNDEFINED;
	}
}

/**
*
*  rct2: 0x0069DEFB
*/
void finance_init() {

	// It only initialises the first month
	for (uint32 i = 0; i < RCT_EXPENDITURE_TYPE_COUNT; i++) {
		gExpenditureTable[i] = 0;
	}

	gCurrentExpenditure = 0;
	gCurrentProfit = 0;

	gWeeklyProfitAverageDividend = 0;
	gWeeklyProfitAverageDivisor = 0;

	gInitialCash = MONEY(10000,00); // Cheat detection

	gCashEncrypted = ENCRYPT_MONEY(MONEY(10000,00));
	gBankLoan = MONEY(10000,00);
	gMaxBankLoan = MONEY(20000,00);

	gHistoricalProfit = 0;

	gBankLoanInterestRate = 10;
	gParkValue = 0;
	gCompanyValue = 0;
	gScenarioCompletedCompanyValue = MONEY32_UNDEFINED;
	gTotalAdmissions = 0;
	gTotalIncomeFromAdmissions = 0;
	safe_strcpy(gScenarioCompletedBy, "?", sizeof(gScenarioCompletedBy));
}

/**
*
*  rct2: 0x0069E79A
*/
void finance_update_daily_profit()
{
	gCurrentProfit = 7 * gCurrentExpenditure;
	gCurrentExpenditure = 0; // Reset daily expenditure

	money32 current_profit = 0;

	if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
	{
		// Staff costs
		uint16 sprite_index;
		rct_peep *peep;

		FOR_ALL_STAFF(sprite_index, peep) {
			current_profit -= wage_table[peep->staff_type];
		}

		// Research costs
		uint8 level = gResearchFundingLevel;
		current_profit -= research_cost_table[level];

		// Loan costs
		money32 current_loan = gBankLoan;
		current_profit -= current_loan / 600;

		// Ride costs
		rct_ride *ride;
		sint32 i;
		FOR_ALL_RIDES(i, ride) {
			if (ride->status != RIDE_STATUS_CLOSED && ride->upkeep_cost != -1) {
				current_profit -= 2 * ride->upkeep_cost;
			}
		}
	}

	// This is not equivalent to / 4 due to rounding of negative numbers
	current_profit = current_profit >> 2;

	gCurrentProfit += current_profit;

	// These are related to weekly profit graph
	gWeeklyProfitAverageDividend += gCurrentProfit;
	gWeeklyProfitAverageDivisor += 1;

	window_invalidate_by_class(WC_FINANCES);
}

void finance_set_loan(money32 loan)
{
	game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, loan, GAME_COMMAND_SET_CURRENT_LOAN, 0, 0);
}

money32 finance_get_initial_cash()
{
	return gInitialCash;
}

money32 finance_get_current_loan()
{
	return gBankLoan;
}

money32 finance_get_maximum_loan()
{
	return gMaxBankLoan;
}

money32 finance_get_current_cash()
{
	return DECRYPT_MONEY(gCashEncrypted);
}

/**
 *
 *  rct2: 0x0069DFB3
 */
void game_command_set_current_loan(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
{
	money32 money, loanDifference, currentLoan;
	money32 newLoan = *edx;

	currentLoan = gBankLoan;
	money = DECRYPT_MONEY(gCashEncrypted);
	loanDifference = currentLoan - newLoan;

	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_INTEREST;
	if (newLoan > currentLoan) {
		if (newLoan > gMaxBankLoan) {
			gGameCommandErrorText = STR_BANK_REFUSES_TO_INCREASE_LOAN;
			*ebx = MONEY32_UNDEFINED;
			return;
		}
	} else {
		if (loanDifference > money) {
			gGameCommandErrorText = STR_NOT_ENOUGH_CASH_AVAILABLE;
			*ebx = MONEY32_UNDEFINED;
			return;
		}
	}

	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		money -= loanDifference;
		gBankLoan = newLoan;
		gInitialCash = money;
		gCashEncrypted = ENCRYPT_MONEY(money);

		window_invalidate_by_class(WC_FINANCES);
		gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_MONEY;
	}

	*ebx = 0;
}

/**
 * Shift the expenditure table history one month to the left
 * If the table is full, accumulate the sum of the oldest month first
 * rct2: 0x0069DEAD
 */
void finance_shift_expenditure_table()
{
	// If EXPENDITURE_TABLE_MONTH_COUNT months have passed then is full, sum the oldest month
	if (gDateMonthsElapsed >= EXPENDITURE_TABLE_MONTH_COUNT) {
		money32 sum = 0;
		for (uint32 i = EXPENDITURE_TABLE_TOTAL_COUNT - RCT_EXPENDITURE_TYPE_COUNT; i < EXPENDITURE_TABLE_TOTAL_COUNT; i++) {
			sum += gExpenditureTable[i];
		}
		gHistoricalProfit += sum;
	}

	// Shift the table
	for (uint32 i = EXPENDITURE_TABLE_TOTAL_COUNT - 1; i >= RCT_EXPENDITURE_TYPE_COUNT; i--) {
		gExpenditureTable[i] = gExpenditureTable[i - RCT_EXPENDITURE_TYPE_COUNT];
	}

	// Zero the beginning of the table, which is the new month
	for (uint32 i = 0; i < RCT_EXPENDITURE_TYPE_COUNT; i++) {
		gExpenditureTable[i] = 0;
	}

	window_invalidate_by_class(WC_FINANCES);
}

/**
 *
 *  rct2: 0x0069E89B
 */
void finance_reset_cash_to_initial()
{
	gCashEncrypted = ENCRYPT_MONEY(gInitialCash);
}

/**
 * Gets the last month's profit from food, drink and merchandise.
 */
money32 finance_get_last_month_shop_profit()
{
	money32 profit = 0;
	if (gDateMonthsElapsed != 0) {
		money32 * lastMonthExpenditure = &gExpenditureTable[1 * RCT_EXPENDITURE_TYPE_COUNT];
		profit += lastMonthExpenditure[RCT_EXPENDITURE_TYPE_SHOP_SHOP_SALES];
		profit += lastMonthExpenditure[RCT_EXPENDITURE_TYPE_SHOP_STOCK];
		profit += lastMonthExpenditure[RCT_EXPENDITURE_TYPE_FOODDRINK_SALES];
		profit += lastMonthExpenditure[RCT_EXPENDITURE_TYPE_FOODDRINK_STOCK];
	}
	return profit;
}
