/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
        std::string line = String::StdFormat(
            "%s: (%d, %d, %d)", ride.GetRideTypeDescriptor().EnumName, static_cast<int>(ratings.Excitement),
            static_cast<int>(ratings.Intensity), static_cast<int>(ratings.Nausea));
        return line;
    }
};

TEST_F(RideRatings, all)
{
    std::string path = TestData::GetParkPath("bpb.sv6");

    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    Platform::CoreInit();
    auto context = CreateContext();
    bool initialised = context->Initialise();
    ASSERT_TRUE(initialised);

    GetContext()->LoadParkFromFile(path);

    // Check ride count to check load was successful
    ASSERT_EQ(RideGetCount(), 134);

    CalculateRatingsForAllRides();

    // Load expected ratings
    auto expectedDataPath = Path::Combine(TestData::GetBasePath(), u8"ratings", u8"bpb.sv6.txt");
    auto expectedRatings = File::ReadAllLines(expectedDataPath);

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
