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

#ifdef USE_GTEST

#include <algorithm>
#include <string>
#include <vector>

extern "C" {
#include "main.h"
#include "../src/rct2.h"
#include "../src/ride/ride.h"
#include "../src/ride/ride_data.h"
#include "../src/ride/track.h"
#include "../src/ride/track_data.h"
}

typedef struct {
	uint8 rideType;
	std::vector<uint8> trackTypes;
} TestCase;

extern const utf8string RideNames[91];
extern const utf8string TrackNames[256];
extern const utf8string FlatTrackNames[256];

int main(int argc, char *argv[]) {
	std::vector<TestCase> testCases;

	bool gTestColor = true;
	for (int i = 0; i < argc; ++i) {
		char *arg = argv[i];
		if (strcmp(arg, "--gtest_color=no") == 0) {
			gTestColor = false;
		}
	}

	const utf8 *ansiRed = gTestColor ? "\x1b[31m" : "";
	const utf8 *ansiGreen = gTestColor ? "\x1b[32m" : "";
	const utf8 *ansiReset = gTestColor ? "\x1b[0m" : "";

	for (uint8 rideType = 0; rideType < 91; rideType++) {
		if (!rideIsImplemented(rideType)) {
			continue;
		}

		TestCase testCase = {
				.rideType = rideType
		};

		if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_FLAT_RIDE)) {
			testCase.trackTypes.push_back(RideConstructionDefaultTrackType[rideType]);
		} else {
			for (int trackType = 0; trackType < 256; trackType++) {
				if (rideSupportsTrackType(rideType, trackType)) {
					testCase.trackTypes.push_back(trackType);
				}
			}
		}

		testCases.push_back(testCase);
	}

	int testCaseCount = (int) testCases.size();
	int testCount = 0;
	for (auto &&tc : testCases) {
		testCount += tc.trackTypes.size();
	}

	printf("%s[==========]%s Running %d tests from %d test cases.\n", ansiGreen, ansiReset, testCount, testCaseCount);
	printf("%s[----------]%s Global test environment set-up.\n", ansiGreen, ansiReset);
	initHooks();

	int successCount = 0;
	std::vector<utf8string> failures;
	for (auto &&tc : testCases) {
		const utf8string rideTypeName = RideNames[tc.rideType];
		printf("%s[----------]%s %lu tests from %s\n", ansiGreen, ansiReset, tc.trackTypes.size(), rideTypeName);

		for (auto &&trackType : tc.trackTypes) {
			utf8string trackTypeName;
			if (ride_type_has_flag(tc.rideType, RIDE_TYPE_FLAG_FLAT_RIDE)) {
				trackTypeName = FlatTrackNames[trackType];
			} else {
				trackTypeName = TrackNames[trackType];
			}

			printf("%s[ RUN      ]%s %s.%s\n", ansiGreen, ansiReset, rideTypeName, trackTypeName);
			bool success = testTrackPainting(tc.rideType, trackType);
			if (!success) {
				printf("%s[  FAILED  ]%s %s.%s (0 ms)\n", ansiRed, ansiReset, rideTypeName, trackTypeName);
				utf8string testCaseName = new utf8[64];
				sprintf(testCaseName, "%s.%s", rideTypeName, trackTypeName);
				failures.push_back(testCaseName);
			} else {
				printf("%s[       OK ]%s %s.%s (0 ms)\n", ansiGreen, ansiReset, rideTypeName, trackTypeName);
				successCount++;
			}
		}

		printf("%s[----------]%s %lu tests from %s (0 ms total)\n", ansiGreen, ansiReset, tc.trackTypes.size(),
			   rideTypeName);
	}
	printf("\n");

	printf("%s[----------]%s Global test environment tear-down\n", ansiGreen, ansiReset);
	printf("%s[==========]%s %d tests from %d test cases ran. (0 ms total).\n", ansiGreen, ansiReset, testCount,
		   testCaseCount);
	printf("%s[  PASSED  ]%s %d tests.\n", ansiGreen, ansiReset, successCount);

	if (failures.size() > 0) {
		printf("%s[  FAILED  ]%s %lu tests, listed below:\n", ansiRed, ansiReset, failures.size());

		for (auto &&failure : failures) {
			printf("%s[  FAILED  ]%s %s\n", ansiRed, ansiReset, failure);
			delete(failure);
		}

		printf("\n");

		printf("%lu FAILED TESTS\n", failures.size());

		return 1;
	}

	return 0;
}

#endif // USE_GTEST
