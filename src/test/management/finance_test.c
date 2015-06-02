/*****************************************************************************
* Copyright (c) 2015 Marco Costa
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

#include "../../addresses.h"
#include "../../openrct2.h"
#include "../../scenario.h"

#include "../../management/finance.h"
#include "finance_test.h"

void test_finance_setup(CuTest* tc) {
	test_load_scenario(tc, "Build your own Six Flags Park.SC6");
}

void test_finance_loan_increase(CuTest* tc) {
	money32 initialCash = finance_get_current_cash();
	money32 initialLoan = finance_get_current_loan();

	money32 newLoan = finance_get_maximum_loan();
	finance_set_loan(newLoan);

	money32 actual = finance_get_current_loan();
	CuAssertIntEquals(tc, newLoan, actual);

	money32 actualCash = finance_get_current_cash();
	CuAssertIntEquals(tc, initialCash + newLoan - initialLoan, actualCash);
}

void test_finance_loan_pay_back(CuTest* tc) {
	money32 initialCash = finance_get_current_cash();
	money32 initialLoan = finance_get_current_loan();

	money32 newLoan = MONEY(0, 00);
	finance_set_loan(newLoan);

	money32 actual = finance_get_current_loan();
	CuAssertIntEquals(tc, newLoan, actual);

	money32 actualCash = finance_get_current_cash();
	CuAssertIntEquals(tc, MONEY(0, 00), actualCash);
}


