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

#include "../src/openrct2.h"
#include "management/finance_test.h"
#include "ride/ride_ratings_test.h"

int cmdline_for_test(const char **argv, int argc)
{
	// NOTE Currently all tests are running in a OpenRCT2 setup frame. Tests for the initialisation and disposing should be
	// separate and independent.

	int numFailedTests;

	gOpenRCT2Headless = true;
	if (openrct2_initialise()) {
		numFailedTests = run_all_tests();

		openrct2_dispose();
	}
	return numFailedTests > 0 ? -1 : 1;
}

CuSuite* new_suite(void)
{
	CuSuite* suite = CuSuiteNew();
	
	// Test Finance
	SUITE_ADD_TEST(suite, test_finance_setup);
	SUITE_ADD_TEST(suite, test_finance_loan_increase);
	SUITE_ADD_TEST(suite, test_finance_loan_pay_back);

	// Test Ride
	SUITE_ADD_TEST(suite, test_ride_ratings_setup);
	SUITE_ADD_TEST(suite, test_ride_ratings);

	// Future Tests:
	// Test X
	// SUITE_ADD_TEST(suite, test_X_setup);
	// SUITE_ADD_TEST(suite, test_X_Y);
	// SUITE_ADD_TEST(suite, test_X_Z);

	return suite;
}

int run_all_tests(void)
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, new_suite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("Test results:\n%s\n", output->buffer);

	return suite->failCount;
}
