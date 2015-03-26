/*****************************************************************************
* Copyright (c) 2014 Matthias Lanzinger
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* This file is part of OpenRCT2.
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "../addresses.h"
#include "../game.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../peep/peep.h"
#include "../ride/ride.h"
#include "../world/park.h"
#include "../world/sprite.h"
#include "finance.h"

// Monthly staff wages
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

int dword_988E60[] = { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0 };

/**
 * Pay an amount of money.
 * rct2: 0x069C674
 * @param amount (eax)
 * @param type passed via global var 0x0141F56C, our type is that var/4.
 **/
void finance_payment(money32 amount, rct_expenditure_type type)
{
	money32 cur_money = DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32));
	money32 new_money = cur_money - amount;

	//overflow check
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32) = ENCRYPT_MONEY(new_money);
	RCT2_ADDRESS(RCT2_ADDRESS_EXPENDITURE_TABLE, money32)[type] -= amount;
	if (dword_988E60[type] & 1)
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_EXPENDITURE, money32) -= amount; // Cumulative amount of money spent this day
	

	RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint32) |= BTM_TB_DIRTY_FLAG_MONEY;
	window_invalidate_by_class(WC_FINANCES);
}

/**
 * Pays the wages of all active staff members in the park.
 * rct2: 0x006C18A9
 **/
void finance_pay_wages()
{
	rct_peep* peep;
	uint16 spriteIndex;

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)
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

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)
		return;

	level = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RESEARCH_LEVEL, uint8);
	finance_payment(research_cost_table[level] / 4, RCT_EXPENDITURE_TYPE_RESEARCH);
}

/**
 * Pay interest on current loans.
 * rct2: 0x0069E092
 **/
void finance_pay_interest()
{
	money32 current_loan = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, sint32);
	sint16 current_interest = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_INTEREST_RATE, sint16);
	money32 tempcost = (current_loan * 5 * current_interest) >> 14; // (5 * interest) / 2^14 is pretty close to

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)
		return;

	finance_payment(tempcost, RCT_EXPENDITURE_TYPE_INTEREST);
}

/**
 *
 * rct2: 0x006AC885
 */
void finance_pay_ride_upkeep()
{
	int i;
	rct_ride* ride;

	FOR_ALL_RIDES(i, ride) {
		if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)) {
			ride->build_date = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
			ride->reliability = RIDE_INITIAL_RELIABILITY;

		}
		if (ride->status != RIDE_STATUS_CLOSED && !(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
			sint16 upkeep = ride->upkeep_cost;
			if (upkeep != -1) {
				ride->total_profit -= upkeep;
				ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
				finance_payment(upkeep, RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS);
			}
		}
	}
}

void finance_reset_history()
{
	int i;
	for (i = 0; i < 128; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_BALANCE_HISTORY, money32)[i] = MONEY32_UNDEFINED;
		RCT2_ADDRESS(RCT2_ADDRESS_WEEKLY_PROFIT_HISTORY, money32)[i] = MONEY32_UNDEFINED;
		RCT2_ADDRESS(RCT2_ADDRESS_PARK_VALUE_HISTORY, money32)[i] = MONEY32_UNDEFINED;
	}
}

/**
*
*  rct2: 0x0069DEFB
*/
void finance_init() {

	for (short i = 0; i < 56; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_EXPENDITURE_TABLE, money32)[i] = 0;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_EXPENDITURE, uint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PROFIT, money32) = 0;

	RCT2_GLOBAL(0x01358334, money32) = 0;
	RCT2_GLOBAL(0x01358338, uint16) = 0;

	RCT2_GLOBAL(0x013573DC, money32) = MONEY(10000,00); // Cheat detection

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32) = ENCRYPT_MONEY(MONEY(10000,00));
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32) = MONEY(10000,00);
	RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, money32) = MONEY(20000,00);

	RCT2_GLOBAL(0x013587D0, uint32) = 0;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_INTEREST_RATE, uint8) = 10;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, money32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_COMPANY_VALUE, money32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, money32) = MONEY32_UNDEFINED;
	RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_ADMISSIONS, uint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INCOME_FROM_ADMISSIONS, uint32) = 0;

	RCT2_GLOBAL(0x013587D8, uint16) = 0x3F;

	sub_69E869();
}

/**
*
*  rct2: 0x0069E79A
*/
void finance_update_daily_profit()
{
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PROFIT, money32) = 7 * RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_EXPENDITURE, money32);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_EXPENDITURE, money32) = 0; // Reset daily expenditure

	money32 current_profit = 0;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY))
	{
		// Staff costs
		uint16 sprite_index;
		rct_peep *peep;

		FOR_ALL_STAFF(sprite_index, peep) {
			uint8 staff_type = peep->staff_type;
			current_profit -= wage_table[peep->staff_type];
		}

		// Research costs
		uint8 level = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RESEARCH_LEVEL, uint8);
		current_profit -= research_cost_table[level];

		// Loan costs
		money32 current_loan = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32);
		current_profit -= current_loan / 600;

		// Ride costs
		rct_ride *ride;
		int i;
		FOR_ALL_RIDES(i, ride) {
			if (ride->status != RIDE_STATUS_CLOSED && ride->upkeep_cost != -1) {
				current_profit -= 2 * ride->upkeep_cost;
			}
		}
	}

	// This is not equivalent to / 4 due to rounding of negative numbers
	current_profit = current_profit >> 2;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PROFIT, money32) += current_profit;

	// These are related to weekly profit graph
	RCT2_GLOBAL(0x1358334, money32) += RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PROFIT, money32);
	RCT2_GLOBAL(0x1358338, uint16) += 1;

	window_invalidate_by_class(WC_FINANCES);
}

void sub_69E869()
{
	// This subroutine is loan related and is used for cheat detection
	sint32 value = 0x70093A;
	value -= RCT2_GLOBAL(RCT2_ADDRESS_INITIAL_CASH, money32);
	value = ror32(value, 5);
	value -= RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32);
	value = ror32(value, 7);
	value += RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, money32);
	value = ror32(value, 3);
	RCT2_GLOBAL(0x013587C4, sint32) = value;
}

void finance_set_loan(money32 loan)
{
	game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, loan, GAME_COMMAND_SET_CURRENT_LOAN, 0, 0);
}

/**
 *
 *  rct2: 0x0069DFB3
 */
void game_command_set_current_loan(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp)
{
	money32 money, loanDifference, currentLoan;
	money32 newLoan = *edx;

	currentLoan = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32);
	money = DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, money32));
	loanDifference = currentLoan - newLoan;

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_INTEREST * 4;
	if (newLoan > currentLoan) {
		if (newLoan > RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, money32)) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_BANK_REFUSES_TO_INCREASE_LOAN;
			*ebx = MONEY32_UNDEFINED;
			return;
		}
	} else {
		if (loanDifference > money) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_NOT_ENOUGH_CASH_AVAILABLE;
			*ebx = MONEY32_UNDEFINED;
			return;
		}
	}

	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		money -= loanDifference;
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32) = newLoan;
		RCT2_GLOBAL(RCT2_ADDRESS_INITIAL_CASH, money32) = money;
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, money32) = ENCRYPT_MONEY(money);
		sub_69E869();

		window_invalidate_by_class(WC_FINANCES);
		RCT2_GLOBAL(0x009A9804, uint16) |= 1;
	}

	*ebx = 0;
}
