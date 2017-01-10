#include <string>
#include <gtest/gtest.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/OpenRCT2.h>

extern "C"
{
    #include <openrct2/platform/platform.h>
    #include <openrct2/scenario/scenario.h>
}

class RideRatings : public testing::Test
{
protected:
    static const char * ExpectedRideRatings[];
};

TEST_F(RideRatings, all)
{
    std::string testdataPath = "testdata";
    std::string path = Path::Combine(testdataPath, "sfmm.sc6");

    gOpenRCT2Headless = true;
    core_init();
    openrct2_initialise();
    scenario_load_and_play_from_path(path.c_str());

    // Check ride count to check load was successful
    ASSERT_EQ(gRideCount, 77);

    // Check ride ratings
    int expI = 0;
    for (int rideId = 0; rideId < MAX_RIDES; rideId++)
    {
        rct_ride * ride = get_ride(rideId);
        if (ride->type != RIDE_TYPE_NULL)
        {
            rating_tuple ratings = ride->ratings;
            std::string actual = String::StdFormat("Ride type: %d, Ratings: %d, %d, %d",
                (int)ride->type,
                (int)ratings.excitement,
                (int)ratings.intensity,
                (int)ratings.nausea);

            const char * expected = ExpectedRideRatings[expI];
            ASSERT_STREQ(actual.c_str(), expected);

            expI++;
        }
    }

    openrct2_dispose();
}

const char * RideRatings::ExpectedRideRatings[] =
{
    "Ride type: 52, Ratings: 857, 979, 545",
    "Ride type: 52, Ratings: 846, 928, 523",
    "Ride type: 51, Ratings: 839, 702, 346",
    "Ride type: 42, Ratings: 713, 886, 620",
    "Ride type: 26, Ratings: 242, 245, 251",
    "Ride type: 46, Ratings: 197, 157, 250",
    "Ride type: 28, Ratings: -1, 0, 0",
    "Ride type: 32, Ratings: -1, 0, 0",
    "Ride type: 32, Ratings: -1, 0, 0",
    "Ride type: 32, Ratings: -1, 0, 0",
    "Ride type: 30, Ratings: -1, 0, 0",
    "Ride type: 30, Ratings: -1, 0, 0",
    "Ride type: 28, Ratings: -1, 0, 0",
    "Ride type: 28, Ratings: -1, 0, 0",
    "Ride type: 28, Ratings: -1, 0, 0",
    "Ride type: 11, Ratings: 328, 58, 1",
    "Ride type: 36, Ratings: -1, 0, 0",
    "Ride type: 42, Ratings: 710, 890, 624",
    "Ride type: 6, Ratings: 379, 38, 15",
    "Ride type: 17, Ratings: 715, 628, 403",
    "Ride type: 15, Ratings: 760, 579, 292",
    "Ride type: 5, Ratings: 176, 11, 2",
    "Ride type: 5, Ratings: 175, 11, 2",
    "Ride type: 43, Ratings: 563, 35, 102",
    "Ride type: 23, Ratings: 618, 217, 111",
    "Ride type: 19, Ratings: 729, 937, 446",
    "Ride type: 51, Ratings: 730, 734, 386",
    "Ride type: 2, Ratings: 677, 678, 684",
    "Ride type: 23, Ratings: 587, 248, 131",
    "Ride type: 24, Ratings: 371, 113, 67",
    "Ride type: 73, Ratings: 661, 947, 757",
    "Ride type: 52, Ratings: 751, 968, 559",
    "Ride type: 60, Ratings: 471, 242, 118",
    "Ride type: 33, Ratings: 143, 60, 75",
    "Ride type: 30, Ratings: -1, 0, 0",
    "Ride type: 35, Ratings: -1, 0, 0",
    "Ride type: 28, Ratings: -1, 0, 0",
    "Ride type: 28, Ratings: -1, 0, 0",
    "Ride type: 36, Ratings: -1, 0, 0",
    "Ride type: 32, Ratings: -1, 0, 0",
    "Ride type: 32, Ratings: -1, 0, 0",
    "Ride type: 32, Ratings: -1, 0, 0",
    "Ride type: 32, Ratings: -1, 0, 0",
    "Ride type: 36, Ratings: -1, 0, 0",
    "Ride type: 36, Ratings: -1, 0, 0",
    "Ride type: 46, Ratings: 200, 157, 250",
    "Ride type: 25, Ratings: 281, 80, 35",
    "Ride type: 81, Ratings: 400, 647, 764",
    "Ride type: 36, Ratings: -1, 0, 0",
    "Ride type: 48, Ratings: -1, 0, 0",
    "Ride type: 36, Ratings: -1, 0, 0",
    "Ride type: 36, Ratings: -1, 0, 0",
    "Ride type: 36, Ratings: -1, 0, 0",
    "Ride type: 36, Ratings: -1, 0, 0",
    "Ride type: 45, Ratings: -1, 0, 0",
    "Ride type: 37, Ratings: 123, 50, 55",
    "Ride type: 35, Ratings: -1, 0, 0",
    "Ride type: 35, Ratings: -1, 0, 0",
    "Ride type: 28, Ratings: -1, 0, 0",
    "Ride type: 30, Ratings: -1, 0, 0",
    "Ride type: 28, Ratings: -1, 0, 0",
    "Ride type: 36, Ratings: -1, 0, 0",
    "Ride type: 32, Ratings: -1, 0, 0",
    "Ride type: 35, Ratings: -1, 0, 0",
    "Ride type: 35, Ratings: -1, 0, 0",
    "Ride type: 22, Ratings: 527, 254, 41",
    "Ride type: 28, Ratings: -1, 0, 0",
    "Ride type: 28, Ratings: -1, 0, 0",
    "Ride type: 30, Ratings: -1, 0, 0",
    "Ride type: 28, Ratings: -1, 0, 0",
    "Ride type: 30, Ratings: -1, 0, 0",
    "Ride type: 49, Ratings: 210, 30, 0",
    "Ride type: 71, Ratings: 215, 62, 34",
    "Ride type: 38, Ratings: 290, 350, 300",
    "Ride type: 46, Ratings: 195, 157, 250",
    "Ride type: 47, Ratings: 341, 153, 10",
    "Ride type: 55, Ratings: 728, 850, 803",
};
