#include <string>
#include <gtest/gtest.h>
#include <openrct2/core/File.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/OpenRCT2.h>
#include "TestData.h"

extern "C"
{
    #include <openrct2/platform/platform.h>
    #include <openrct2/game.h>
}

class RideRatings : public testing::Test
{
protected:
    void CalculateRatingsForAllRides()
    {
        for (int rideId = 0; rideId < MAX_RIDES; rideId++)
        {
            rct_ride * ride = get_ride(rideId);
            if (ride->type != RIDE_TYPE_NULL)
            {
                ride_ratings_update_ride(rideId);
            }
        }
    }

    void DumpRatings()
    {
        for (int rideId = 0; rideId < MAX_RIDES; rideId++)
        {
            rct_ride * ride = get_ride(rideId);
            if (ride->type != RIDE_TYPE_NULL)
            {
                std::string line = FormatRatings(ride);
                printf("%s\n", line.c_str());
            }
        }
    }

    std::string FormatRatings(rct_ride * ride)
    {
        rating_tuple ratings = ride->ratings;
        std::string line = String::StdFormat("%s: (%d, %d, %d)",
            ride_type_get_enum_name(ride->type),
            (int)ratings.excitement,
            (int)ratings.intensity,
            (int)ratings.nausea);
        return line;
    }
};

TEST_F(RideRatings, all)
{
    std::string path = TestData::GetParkPath("bpb.sv6");

    gOpenRCT2Headless = true;
    core_init();
    openrct2_initialise();
    game_load_sv6_path(path.c_str());

    // Check ride count to check load was successful
    ASSERT_EQ(gRideCount, 134);

    CalculateRatingsForAllRides();

    // Load expected ratings
    auto expectedDataPath = Path::Combine(TestData::GetBasePath(), "ratings", "bpb.sv6.txt");
    auto expectedRatings = File::ReadAllLines(expectedDataPath);

    // Check ride ratings
    int expI = 0;
    for (int rideId = 0; rideId < MAX_RIDES; rideId++)
    {
        rct_ride * ride = get_ride(rideId);
        if (ride->type != RIDE_TYPE_NULL)
        {
            std::string actual = FormatRatings(ride);
            std::string expected = expectedRatings[expI];
            ASSERT_STREQ(actual.c_str(), expected.c_str());

            expI++;
        }
    }

    openrct2_dispose();
}
