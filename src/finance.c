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

#include "addresses.h"
#include "finance.h"
#include "sprite.h"
#include "park.h"
#include "peep.h"
#include "window.h"


const int wage_table[4] = { 500, 800, 600, 550 };
const int research_cost_table[4] = { 0, 1000, 2000, 4000 }; // monthly cost

/**
 * 
 * rct2: 0x069C674
 * @param amount (eax)
 * @param type passed via global var 0x0141F56C, our type is that var/4.
 **/
void finance_payment(int amount, rct_expenditure_type type)
{
	int test = RCT2_GLOBAL(0x13CA740, uint32);
	sint32 cur_money = DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32));
	sint32 new_money = cur_money - amount;

	//overflow check
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32) = ENCRYPT_MONEY(new_money);
	RCT2_ADDRESS(RCT2_ADDRESS_EXPENDITURE_TABLE, sint32)[type] -= amount;
	if (RCT2_ADDRESS(0x00988E60, uint32)[type] & 1)
		RCT2_GLOBAL(0x0135832C, sint32) -= amount;
	

	RCT2_GLOBAL(0x009A9804, uint32) |= 1; // money diry flag
	window_invalidate_by_id(WC_FINANCES, 0);
}

/**
 * Pays the wages of all active staff members in the park.
 * rct2: 0x006C18A9
 **/
void finance_pay_wages()
{
	rct_peep* peep;
	uint16 sprite_idx;

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & 0x800)
		return;

	for (sprite_idx = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16); sprite_idx != SPRITE_INDEX_NULL; sprite_idx = peep->next) {
		peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_idx].peep);
		if (peep->type == PEEP_TYPE_STAFF) 
			finance_payment(wage_table[peep->staff_type], RCT_EXPENDITURE_TYPE_WAGES);
	}
}

/**
* Pays the current research level's cost.
* rct2: 0x00684DA5
**/
void finance_pay_research()
{
	uint8 level;

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & 0x800)
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
	sint32 current_loan = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, sint32);
	sint16 current_interest = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_INTEREST_RATE, sint16);
	sint64 tempcost = (current_loan * 5 * current_interest) >> 14; // (5*interest)/2^14 is pretty close to

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & 0x800)
		return;

	finance_payment((sint32)tempcost, RCT_EXPENDITURE_TYPE_INTEREST);
}

/**
*
*  rct2: 0x0069DEFB
*/
void finance_init() {

	for (short i = 0; i < 56; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_EXPENDITURE_TABLE, uint32)[i] = 0;
	}

	RCT2_GLOBAL(0x0135832C, uint32) = 0;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PROFIT, uint32) = 0;

	RCT2_GLOBAL(0x01358334, uint32) = 0;
	RCT2_GLOBAL(0x01358338, uint16) = 0;

	RCT2_GLOBAL(0x013573DC, uint32) = 100000; // Cheat detection

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32) = ENCRYPT_MONEY(100000);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, sint32) = 100000;
	RCT2_GLOBAL(RCT2_ADDRESS_MAXIMUM_LOAN, uint32) = 200000;

	RCT2_GLOBAL(0x013587D0, uint32) = 0;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_INTEREST_RATE, uint8) = 10;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, sint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_COMPANY_VALUE, sint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, sint32) = 0x80000000;
	RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_ADMISSIONS, uint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INCOME_FROM_ADMISSIONS, uint32) = 0;

	RCT2_GLOBAL(0x013587D8, uint16) = 0x3F;

	RCT2_CALLPROC_EBPSAFE(0x0069E869);
}