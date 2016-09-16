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

#include <algorithm>
#include <string>
#include <vector>

#if defined(__unix__)
#include <unistd.h>
#include <sys/mman.h>
#endif // defined(__unix__)

extern "C" {
#include "data.h"
#include "intercept.h"
#include "../../src/rct2.h"
#include "../../src/ride/ride.h"
#include "../../src/ride/ride_data.h"
#include "../../src/ride/track.h"
#include "../../src/ride/track_data.h"
}

typedef struct {
	uint8 rideType;
	std::vector<uint8> trackTypes;
} TestCase;

enum CLIColour {
	RED,
	GREEN,
};

bool gTestColor = true;

static bool CStringEquals(const char *lhs, const char *rhs) {
	if (lhs == NULL) return rhs == NULL;

	if (rhs == NULL) return false;

	return strcmp(lhs, rhs) == 0;
}

static bool ShouldUseColor() {
	if (gTestColor == false) {
		return false;
	}

	const char* const term = getenv("TERM");
	const bool term_supports_color =
		CStringEquals(term, "xterm") ||
		CStringEquals(term, "xterm-color") ||
		CStringEquals(term, "xterm-256color") ||
		CStringEquals(term, "screen") ||
		CStringEquals(term, "screen-256color") ||
		CStringEquals(term, "tmux") ||
		CStringEquals(term, "tmux-256color") ||
		CStringEquals(term, "rxvt-unicode") ||
		CStringEquals(term, "rxvt-unicode-256color") ||
		CStringEquals(term, "linux") ||
		CStringEquals(term, "cygwin");

	return term_supports_color;
}

static const char* GetAnsiColorCode(CLIColour color) {
	switch (color) {
		case RED:     return "1";
		case GREEN:   return "2";
		default:            return NULL;
	};
}

static void ColouredPrintF(CLIColour colour, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	if(!ShouldUseColor()) {
		vprintf(fmt, args);
		va_end(args);
		return;
	}

	printf("\033[0;3%sm", GetAnsiColorCode(colour));
	vprintf(fmt, args);
	printf("\033[m");
	va_end(args);
}

#if defined(__WINDOWS__)
int main(int argc, char *argv[]);

#define OPENRCT2_DLL_MODULE_NAME "openrct2.dll"

static HMODULE _dllModule = NULL;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	_dllModule = (HMODULE)hModule;
	return TRUE;
}

__declspec(dllexport) int StartOpenRCT(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	
	if (_dllModule == NULL) {
		_dllModule = GetModuleHandleA(OPENRCT2_DLL_MODULE_NAME);
	}

	int gExitCode = main(0, NULL);
	
	exit(gExitCode);
	return gExitCode;
}

#endif

char *segments = (char *)(GOOD_PLACE_FOR_DATA_SEGMENT);

static uint32 sawyercoding_calculate_checksum(const uint8* buffer, size_t length)
{
	size_t i;
	uint32 checksum = 0;
	for (i = 0; i < length; i++)
		checksum += buffer[i];

	return checksum;
}

/**
 * Loads RCT2's data model and remaps the addresses.
 * @returns true if the data integrity check succeeded, otherwise false.
 */
static bool openrct2_setup_rct2_segment()
{
	// OpenRCT2 on Linux and macOS is wired to have the original Windows PE sections loaded
	// necessary. Windows does not need to do this as OpenRCT2 runs as a DLL loaded from the Windows PE.
	int len = 0x01429000 - 0x8a4000; // 0xB85000, 12079104 bytes or around 11.5MB
	int err = 0;

#if defined(__unix__)
	int pageSize = getpagesize();
	int numPages = (len + pageSize - 1) / pageSize;
	unsigned char *dummy = (unsigned char *)malloc(numPages);

	err = mincore((void *)segments, len, dummy);
	bool pagesMissing = false;
	if (err != 0)
	{
		err = errno;
#ifdef __LINUX__
		// On Linux ENOMEM means all requested range is unmapped
		if (err != ENOMEM)
		{
			pagesMissing = true;
			perror("mincore");
		}
#else
		pagesMissing = true;
		perror("mincore");
#endif // __LINUX__
	} else {
		for (int i = 0; i < numPages; i++)
		{
			if (dummy[i] != 1)
			{
				pagesMissing = true;
				void *start = (void *)(segments + i * pageSize);
				void *end = (void *)(segments + (i + 1) * pageSize - 1);
				log_warning("required page %p - %p is not in memory!", start, end);
			}
		}
	}
	free(dummy);
	if (pagesMissing)
	{
		log_error("At least one of required pages was not found in memory. This can cause segfaults later on.");
	}

	// section: text
	err = mprotect((void *)0x401000, 0x8a4000 - 0x401000, PROT_READ | PROT_EXEC | PROT_WRITE);
	if (err != 0)
	{
		perror("mprotect");
	}

	// section: rw data
	err = mprotect((void *)segments, 0x01429000 - 0x8a4000, PROT_READ | PROT_WRITE);
	if (err != 0)
	{
		perror("mprotect");
	}
#endif // defined(__unix__)

	// Check that the expected data is at various addresses.
	// Start at 0x9a6000, which is start of .data, to skip the region containing addresses to DLL
	// calls, which can be changed by windows/wine loader.
	const uint32 c1 = sawyercoding_calculate_checksum((const uint8*)(segments + (uintptr_t)(0x009A6000 - 0x8a4000)), 0x009E0000 - 0x009A6000);
	const uint32 c2 = sawyercoding_calculate_checksum((const uint8*)(segments + (uintptr_t)(0x01428000 - 0x8a4000)), 0x014282BC - 0x01428000);
	const uint32 exp_c1 = 10114815;
	const uint32 exp_c2 = 23564;
	if (c1 != exp_c1 || c2 != exp_c2) {
		log_warning("c1 = %u, expected %u, match %d", c1, exp_c1, c1 == exp_c1);
		log_warning("c2 = %u, expected %u, match %d", c2, exp_c2, c2 == exp_c2);
		return false;
	}

	return true;
}

int main(int argc, char *argv[]) {
	std::vector<TestCase> testCases;

	for (int i = 0; i < argc; ++i) {
		char *arg = argv[i];
		if (strcmp(arg, "--gtest_color=no") == 0) {
			gTestColor = false;
		}
	}

	for (uint8 rideType = 0; rideType < 91; rideType++) {
		if (!rideIsImplemented(rideType)) {
			continue;
		}

		TestCase testCase = {0};
		testCase.rideType = rideType;

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

	ColouredPrintF(CLIColour::GREEN, "[==========] ");
	printf("Running %d tests from %d test cases.\n", testCount, testCaseCount);

	ColouredPrintF(CLIColour::GREEN, "[----------] ");
	printf("Global test environment set-up.\n");
	openrct2_setup_rct2_segment();
	initHooks();

	int successCount = 0;
	std::vector<utf8string> failures;
	for (auto &&tc : testCases) {
		const utf8string rideTypeName = RideNames[tc.rideType];
		ColouredPrintF(CLIColour::GREEN, "[----------] ");
		printf("%d tests from %s\n", (int)tc.trackTypes.size(), rideTypeName);

		for (auto &&trackType : tc.trackTypes) {
			utf8string trackTypeName;
			if (ride_type_has_flag(tc.rideType, RIDE_TYPE_FLAG_FLAT_RIDE)) {
				trackTypeName = FlatTrackNames[trackType];
			} else {
				trackTypeName = TrackNames[trackType];
			}

			ColouredPrintF(CLIColour::GREEN, "[ RUN      ] ");
			printf("%s.%s\n", rideTypeName, trackTypeName);
			bool success = testTrackPainting(tc.rideType, trackType);
			if (!success) {
				utf8string testCaseName = new utf8[64];
				sprintf(testCaseName, "%s.%s", rideTypeName, trackTypeName);

				ColouredPrintF(CLIColour::RED, "[  FAILED  ] ");
				printf("%s (0 ms)\n", testCaseName);
				failures.push_back(testCaseName);
			} else {
				ColouredPrintF(CLIColour::GREEN, "[       OK ] ");
				printf("%s.%s (0 ms)\n", rideTypeName, trackTypeName);
				successCount++;
			}
		}

		ColouredPrintF(CLIColour::GREEN, "[----------] ");
		printf("%d tests from %s (0 ms total)\n",  (int)tc.trackTypes.size(), rideTypeName);
	}
	printf("\n");

	ColouredPrintF(CLIColour::GREEN, "[----------] ");
	printf("Global test environment tear-down\n");

	ColouredPrintF(CLIColour::GREEN, "[==========] ");
	printf("%d tests from %d test cases ran. (0 ms total).\n", testCount, testCaseCount);

	ColouredPrintF(CLIColour::GREEN, "[  PASSED  ] ");
	printf("%d tests.\n", successCount);

	if (failures.size() > 0) {
		ColouredPrintF(CLIColour::RED, "[  FAILED  ] ");
		printf("%d tests, listed below:\n", (int)failures.size());

		for (auto &&failure : failures) {
			ColouredPrintF(CLIColour::RED, "[  FAILED  ] ");
			printf("%s\n", failure);
			delete [] failure;
		}

		printf("\n");

		printf("%d FAILED TESTS\n", (int)failures.size());

		return 1;
	}

	return 0;
}
