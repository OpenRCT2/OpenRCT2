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

#ifndef _FINANCE_H_
#define _FINANCE_H_

#include "rct2.h"

typedef int rct_expenditure_type;

enum {
	RCT_EXPENDITURE_TYPE_WAGES = 10,
	RCT_EXPENDITURE_TYPE_RESEARCH = 12,
	RCT_EXPENDITURE_TYPE_INTEREST = 13
};


void finance_payment(int amount, rct_expenditure_type type);
void finance_pay_wages();
void finance_pay_research();
void finance_pay_interest();
void finance_init();

#endif