/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#if defined(__unix__)
#    include <sys/mman.h>
#    include <unistd.h>
#endif // defined(__unix__)

#ifdef __WINDOWS__
#include <Windows.h>
#endif

#include "Data.h"
#include "PaintIntercept.hpp"
#include "TestTrack.hpp"
#include "Utils.hpp"

#include <openrct2/rct2/RCT2.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>

struct TestCase
{
    uint8_t rideType;
    std::vector<uint8_t> trackTypes;
};

enum CLIColour
{
    DEFAULT,
    RED,
    YELLOW,
    GREEN,
};

bool gTestColor = true;
Verbosity _verbosity = NORMAL;

static bool CStringEquals(const char* lhs, const char* rhs)
{
    if (lhs == nullptr)
        return rhs == nullptr;

    if (rhs == nullptr)
        return false;

    return strcmp(lhs, rhs) == 0;
}

enum COLOUR_METHOD
{
    COLOUR_METHOD_NONE,
    COLOUR_METHOD_ANSI,
    COLOUR_METHOD_WINDOWS,
};

static COLOUR_METHOD GetColourMethod()
{
    if (!gTestColor)
    {
        return COLOUR_METHOD_NONE;
    }

    const char* const term = getenv("TERM");
    const bool term_supports_color = CStringEquals(term, "xterm") || CStringEquals(term, "xterm-color")
        || CStringEquals(term, "xterm-256color") || CStringEquals(term, "screen") || CStringEquals(term, "screen-256color")
        || CStringEquals(term, "tmux") || CStringEquals(term, "tmux-256color") || CStringEquals(term, "rxvt-unicode")
        || CStringEquals(term, "rxvt-unicode-256color") || CStringEquals(term, "linux") || CStringEquals(term, "cygwin");

    if (term_supports_color)
    {
        return COLOUR_METHOD_ANSI;
    }

#ifdef __WINDOWS__
    return COLOUR_METHOD_WINDOWS;
#else
    return COLOUR_METHOD_NONE;
#endif
}

static const char* GetAnsiColorCode(CLIColour color)
{
    switch (color)
    {
        case RED:
            return "1";
        case GREEN:
            return "2";
        case YELLOW:
            return "3";
        default:
            return nullptr;
    };
}

#ifdef __WINDOWS__

static WORD GetCurrentWindowsConsoleAttribute(HANDLE hConsoleOutput)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
    return csbi.wAttributes;
}

static WORD GetWindowsConsoleAttribute(CLIColour color, WORD defaultAttr)
{
    switch (color)
    {
        case RED:
            return FOREGROUND_RED;
        case GREEN:
            return FOREGROUND_GREEN;
        case YELLOW:
            return FOREGROUND_RED | FOREGROUND_GREEN;
        default:
            return defaultAttr;
    };
}

#endif

static void Write_VA(Verbosity verbosity, CLIColour colour, const char* fmt, va_list args)
{
    if (_verbosity < verbosity)
        return;

    COLOUR_METHOD colourMethod = GetColourMethod();

    if (colour == CLIColour::DEFAULT || colourMethod == COLOUR_METHOD_NONE)
    {
        vprintf(fmt, args);
    }
    else if (colourMethod == COLOUR_METHOD_ANSI)
    {
        printf("\033[0;3%sm", GetAnsiColorCode(colour));
        vprintf(fmt, args);
        printf("\033[m");
    }
    else if (colourMethod == COLOUR_METHOD_WINDOWS)
    {
#ifdef __WINDOWS__
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        WORD defaultAttr = GetCurrentWindowsConsoleAttribute(hStdOut);
        SetConsoleTextAttribute(hStdOut, GetWindowsConsoleAttribute(colour, defaultAttr));
        vprintf(fmt, args);
        SetConsoleTextAttribute(hStdOut, defaultAttr);
#endif
    }
}

static void Write(Verbosity verbosity, CLIColour colour, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Write_VA(verbosity, colour, fmt, args);
    va_end(args);
}

static void Write(Verbosity verbosity, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Write_VA(verbosity, DEFAULT, fmt, args);
    va_end(args);
}

static void Write(CLIColour colour, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Write_VA(NORMAL, colour, fmt, args);
    va_end(args);
}

static void Write(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Write_VA(NORMAL, DEFAULT, fmt, args);
    va_end(args);
}

static void PrintRideTypes()
{
    for (uint8_t rideType = 0; rideType < RCT2_RIDE_TYPE_COUNT; rideType++)
    {
        CLIColour colour = CLIColour::DEFAULT;
        bool implemented = Utils::rideIsImplemented(rideType);
        const char* rideName = RideNames[rideType];
        const char* status = "";
        if (implemented)
        {
            status = " [IMPLEMENTED]";
            colour = CLIColour::GREEN;
        }

        Write(colour, "%2d: %-30s%s\n", rideType, rideName, status);
    }
}

#include "GeneralSupportHeightCall.hpp"

static void TestGeneralSupportHeightCall()
{
    SupportCall callA = { 16, 0x20 };
    SupportCall callB = { 32, 0x20 };
    SupportCall callC = { 48, 0x20 };
    SupportCall callD = { 48, 0x1F };

    SupportCall out = { 0, 0 };
    bool success;

    SupportCall groupA[4] = { callA, callA, callA, callA };
    success = GeneralSupportHeightCall::FindMostCommonSupportCall(groupA, &out);
    assert(success);
    assert(out == callA);

    SupportCall groupB[4] = { callB, callA, callA, callA };
    success = GeneralSupportHeightCall::FindMostCommonSupportCall(groupB, &out);
    assert(success);
    assert(out == callA);

    SupportCall groupC[4] = { callB, callA, callB, callA };
    success = GeneralSupportHeightCall::FindMostCommonSupportCall(groupC, &out);
    assert(!success);

    SupportCall groupD[4] = { callB, callC, callB, callA };
    success = GeneralSupportHeightCall::FindMostCommonSupportCall(groupD, &out);
    assert(!success);

    SupportCall groupE[4] = { callD, callC, callB, callA };
    success = GeneralSupportHeightCall::FindMostCommonSupportCall(groupE, &out);
    assert(!success);
}

int main(int argc, char* argv[])
{
    TestGeneralSupportHeightCall();

    std::vector<TestCase> testCases;

    uint8_t specificRideType = 0xFF;
    for (int i = 0; i < argc; ++i)
    {
        char* arg = argv[i];
        if (strcmp(arg, "--gtest_color=no") == 0)
        {
            gTestColor = false;
        }
        else if (strcmp(arg, "--quiet") == 0)
        {
            _verbosity = Verbosity::QUIET;
        }
        else if (strcmp(arg, "--ride-type") == 0)
        {
            if (i + 1 < argc)
            {
                i++;
                specificRideType = atoi(argv[i]);
            }
            else
            {
                PrintRideTypes();
                return 2;
            }
        }
    }

    for (uint8_t rideType = 0; rideType < RCT2_RIDE_TYPE_COUNT; rideType++)
    {
        if (specificRideType != RIDE_TYPE_NULL && rideType != specificRideType)
        {
            continue;
        }

        if (!Utils::rideIsImplemented(rideType))
        {
            continue;
        }

        TestCase testCase = {};
        testCase.rideType = rideType;

        if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            testCase.trackTypes.push_back(GetRideTypeDescriptor(rideType).StartTrackPiece);
        }
        else
        {
            for (int trackType = 0; trackType < 256; trackType++)
            {
                if (Utils::rideSupportsTrackType(rideType, trackType))
                {
                    testCase.trackTypes.push_back(trackType);
                }
            }
        }

        testCases.push_back(testCase);
    }

    int testCaseCount = (int)testCases.size();
    size_t testCount = 0;
    for (auto&& tc : testCases)
    {
        testCount += tc.trackTypes.size();
    }

    Write(CLIColour::GREEN, "[==========] ");
    Write("Running %d tests from %d test cases.\n", testCount, testCaseCount);

    Write(CLIColour::GREEN, "[----------] ");
    Write("Global test environment set-up.\n");
    //PaintIntercept::InitHooks();

    int successCount = 0;
    std::vector<utf8string> failures;
    for (auto&& tc : testCases)
    {
        const_utf8string rideTypeName = RideNames[tc.rideType];
        Write(CLIColour::GREEN, "[----------] ");
        Write("%d tests from %s\n", (int)tc.trackTypes.size(), rideTypeName);

        for (auto&& trackType : tc.trackTypes)
        {
            const_utf8string trackTypeName;
            if (GetRideTypeDescriptor(tc.rideType).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                trackTypeName = FlatTrackNames[trackType];
            }
            else
            {
                trackTypeName = TrackNames[trackType];
            }

            Write(CLIColour::GREEN, "[ RUN      ] ");
            Write("%s.%s\n", rideTypeName, trackTypeName);

            std::string out;
            int retVal = TestTrack::TestPaintTrackElement(tc.rideType, trackType, &out);
            Write("%s", out.c_str());
            switch (retVal)
            {
                case TEST_SUCCESS:
                    Write(CLIColour::GREEN, "[       OK ] ");
                    Write("%s.%s (0 ms)\n", rideTypeName, trackTypeName);
                    successCount++;
                    break;

                case TEST_SKIPPED:
                    Write("Skipped\n");
                    // Outputting this as OK because CLion only allows FAILED or OK
                    Write(CLIColour::YELLOW, "[       OK ] ");
                    Write("%s.%s (0 ms)\n", rideTypeName, trackTypeName);
                    successCount++;
                    break;

                case TEST_FAILED:
                    utf8string testCaseName = new utf8[64];
                    snprintf(testCaseName, 64, "%s.%s", rideTypeName, trackTypeName);

                    Write(CLIColour::RED, "[  FAILED  ] ");
                    Write("%s (0 ms)\n", testCaseName);
                    failures.push_back(testCaseName);
                    break;
            }
        }

        Write(CLIColour::GREEN, "[----------] ");
        Write("%d tests from %s (0 ms total)\n", (int)tc.trackTypes.size(), rideTypeName);
    }
    Write("\n");

    Write(CLIColour::GREEN, "[----------] ");
    Write("Global test environment tear-down\n");

    Write(CLIColour::GREEN, "[==========] ");
    Write("%d tests from %d test cases ran. (0 ms total).\n", testCount, testCaseCount);

    Write(Verbosity::QUIET, CLIColour::GREEN, "[  PASSED  ] ");
    Write(Verbosity::QUIET, "%d tests.\n", successCount);

    if (!failures.empty())
    {
        Write(Verbosity::QUIET, CLIColour::RED, "[  FAILED  ] ");
        Write(Verbosity::QUIET, "%d tests, listed below:\n", (int)failures.size());

        for (auto&& failure : failures)
        {
            Write(Verbosity::QUIET, CLIColour::RED, "[  FAILED  ] ");
            Write(Verbosity::QUIET, "%s\n", failure);
            delete[] failure;
        }

        Write(Verbosity::QUIET, "\n");

        Write(Verbosity::QUIET, "%d FAILED TESTS\n", (int)failures.size());

        return 1;
    }

    return 0;
}
