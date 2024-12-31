/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestData.h"

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/core/File.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideManager.hpp>
#include <string>

using namespace OpenRCT2;

class RideRatings : public testing::Test
{
protected:
    void CalculateRatingsForAllRides()
    {
        for (const auto& ride : GetRideManager())
        {
            RideRatingsUpdateRide(ride);
        }
    }

    void DumpRatings()
    {
        for (const auto& ride : GetRideManager())
        {
            std::string line = FormatRatings(ride);
            printf("%s\n", line.c_str());
        }
    }

    std::string FormatRatings(const Ride& ride)
    {
        RatingTuple ratings = ride.ratings;
        auto name = std::string(ride.GetRideTypeDescriptor().Name);
        std::string line = String::stdFormat(
            "%s: (%d, %d, %d)", name.c_str(), static_cast<int>(ratings.excitement), static_cast<int>(ratings.intensity),
            static_cast<int>(ratings.nausea));
        return line;
    }

    void TestRatings(const u8string& parkFile, uint16_t expectedRideCount)
    {
        const auto parkFilePath = TestData::GetParkPath(parkFile);
        const auto ratingsDataPath = Path::Combine(TestData::GetBasePath(), u8"ratings", parkFile + u8".txt");

        // Load expected ratings
        auto expectedRatings = File::ReadAllLines(ratingsDataPath);
        ASSERT_FALSE(expectedRatings.empty());

        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;

        auto context = CreateContext();
        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        GetContext()->LoadParkFromFile(parkFilePath);

        // Check ride count to check load was successful
        ASSERT_EQ(RideGetCount(), expectedRideCount);

        CalculateRatingsForAllRides();

        // Check ride ratings
        int expI = 0;
        for (const auto& ride : GetRideManager())
        {
            auto actual = FormatRatings(ride);
            auto expected = expectedRatings[expI];
            ASSERT_STREQ(actual.c_str(), expected.c_str());

            expI++;
        }
    }
};

TEST_F(RideRatings, bpb)
{
    TestRatings("bpb.sv6", 134);
}

TEST_F(RideRatings, BigMap)
{
    TestRatings("BigMapTest.sv6", 100);
}

TEST_F(RideRatings, EverythingPark)
{
    TestRatings("EverythingPark.park", 529);
}
