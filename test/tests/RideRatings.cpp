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
        int expI = 0;
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
    std::string testdataPath = "testdata";
    std::string path = Path::Combine(testdataPath, "sfmm.sc6");

    gOpenRCT2Headless = true;
    core_init();
    openrct2_initialise();
    scenario_load_and_play_from_path(path.c_str());

    // Check ride count to check load was successful
    ASSERT_EQ(gRideCount, 77);

    CalculateRatingsForAllRides();

    // Check ride ratings
    int expI = 0;
    for (int rideId = 0; rideId < MAX_RIDES; rideId++)
    {
        rct_ride * ride = get_ride(rideId);
        if (ride->type != RIDE_TYPE_NULL)
        {
            std::string actual = FormatRatings(ride);
            std::string expected = ExpectedRideRatings[expI];
            ASSERT_STREQ(actual.c_str(), expected.c_str());

            expI++;
        }
    }

    openrct2_dispose();
}

const char * RideRatings::ExpectedRideRatings[] =
{
    "RIDE_TYPE_WOODEN_ROLLER_COASTER: (838, 979, 545)",
    "RIDE_TYPE_WOODEN_ROLLER_COASTER: (846, 928, 523)",
    "RIDE_TYPE_TWISTER_ROLLER_COASTER: (822, 702, 346)",
    "RIDE_TYPE_REVERSE_FREEFALL_COASTER: (727, 886, 620)",
    "RIDE_TYPE_PIRATE_SHIP: (242, 245, 251)",
    "RIDE_TYPE_TWIST: (197, 157, 250)",
    "RIDE_TYPE_FOOD_STALL: (-1, 0, 0)",
    "RIDE_TYPE_SHOP: (-1, 0, 0)",
    "RIDE_TYPE_SHOP: (-1, 0, 0)",
    "RIDE_TYPE_SHOP: (-1, 0, 0)",
    "RIDE_TYPE_DRINK_STALL: (-1, 0, 0)",
    "RIDE_TYPE_DRINK_STALL: (-1, 0, 0)",
    "RIDE_TYPE_FOOD_STALL: (-1, 0, 0)",
    "RIDE_TYPE_FOOD_STALL: (-1, 0, 0)",
    "RIDE_TYPE_FOOD_STALL: (-1, 0, 0)",
    "RIDE_TYPE_CAR_RIDE: (328, 58, 1)",
    "RIDE_TYPE_TOILETS: (-1, 0, 0)",
    "RIDE_TYPE_REVERSE_FREEFALL_COASTER: (724, 890, 624)",
    "RIDE_TYPE_MONORAIL: (385, 38, 15)",
    "RIDE_TYPE_MINE_TRAIN_COASTER: (706, 628, 403)",
    "RIDE_TYPE_LOOPING_ROLLER_COASTER: (742, 579, 292)",
    "RIDE_TYPE_MINIATURE_RAILWAY: (175, 11, 2)",
    "RIDE_TYPE_MINIATURE_RAILWAY: (174, 11, 2)",
    "RIDE_TYPE_LIFT: (563, 35, 102)",
    "RIDE_TYPE_LOG_FLUME: (599, 217, 111)",
    "RIDE_TYPE_CORKSCREW_ROLLER_COASTER: (717, 937, 448)",
    "RIDE_TYPE_TWISTER_ROLLER_COASTER: (726, 734, 392)",
    "RIDE_TYPE_SUSPENDED_SWINGING_COASTER: (677, 678, 684)",
    "RIDE_TYPE_LOG_FLUME: (580, 248, 131)",
    "RIDE_TYPE_RIVER_RAPIDS: (393, 113, 67)",
    "RIDE_TYPE_COMPACT_INVERTED_COASTER: (687, 947, 763)",
    "RIDE_TYPE_WOODEN_ROLLER_COASTER: (751, 968, 559)",
    "RIDE_TYPE_SPLASH_BOATS: (471, 242, 118)",
    "RIDE_TYPE_MERRY_GO_ROUND: (143, 60, 75)",
    "RIDE_TYPE_DRINK_STALL: (-1, 0, 0)",
    "RIDE_TYPE_INFORMATION_KIOSK: (-1, 0, 0)",
    "RIDE_TYPE_FOOD_STALL: (-1, 0, 0)",
    "RIDE_TYPE_FOOD_STALL: (-1, 0, 0)",
    "RIDE_TYPE_TOILETS: (-1, 0, 0)",
    "RIDE_TYPE_SHOP: (-1, 0, 0)",
    "RIDE_TYPE_SHOP: (-1, 0, 0)",
    "RIDE_TYPE_SHOP: (-1, 0, 0)",
    "RIDE_TYPE_SHOP: (-1, 0, 0)",
    "RIDE_TYPE_TOILETS: (-1, 0, 0)",
    "RIDE_TYPE_TOILETS: (-1, 0, 0)",
    "RIDE_TYPE_TWIST: (200, 157, 250)",
    "RIDE_TYPE_DODGEMS: (281, 80, 35)",
    "RIDE_TYPE_ENTERPRISE: (400, 647, 764)",
    "RIDE_TYPE_TOILETS: (-1, 0, 0)",
    "RIDE_TYPE_FIRST_AID: (-1, 0, 0)",
    "RIDE_TYPE_TOILETS: (-1, 0, 0)",
    "RIDE_TYPE_TOILETS: (-1, 0, 0)",
    "RIDE_TYPE_TOILETS: (-1, 0, 0)",
    "RIDE_TYPE_TOILETS: (-1, 0, 0)",
    "RIDE_TYPE_CASH_MACHINE: (-1, 0, 0)",
    "RIDE_TYPE_FERRIS_WHEEL: (123, 50, 55)",
    "RIDE_TYPE_INFORMATION_KIOSK: (-1, 0, 0)",
    "RIDE_TYPE_INFORMATION_KIOSK: (-1, 0, 0)",
    "RIDE_TYPE_FOOD_STALL: (-1, 0, 0)",
    "RIDE_TYPE_DRINK_STALL: (-1, 0, 0)",
    "RIDE_TYPE_FOOD_STALL: (-1, 0, 0)",
    "RIDE_TYPE_TOILETS: (-1, 0, 0)",
    "RIDE_TYPE_SHOP: (-1, 0, 0)",
    "RIDE_TYPE_INFORMATION_KIOSK: (-1, 0, 0)",
    "RIDE_TYPE_INFORMATION_KIOSK: (-1, 0, 0)",
    "RIDE_TYPE_GO_KARTS: (527, 254, 41)",
    "RIDE_TYPE_FOOD_STALL: (-1, 0, 0)",
    "RIDE_TYPE_FOOD_STALL: (-1, 0, 0)",
    "RIDE_TYPE_DRINK_STALL: (-1, 0, 0)",
    "RIDE_TYPE_FOOD_STALL: (-1, 0, 0)",
    "RIDE_TYPE_DRINK_STALL: (-1, 0, 0)",
    "RIDE_TYPE_CIRCUS_SHOW: (210, 30, 0)",
    "RIDE_TYPE_CROOKED_HOUSE: (215, 62, 34)",
    "RIDE_TYPE_MOTION_SIMULATOR: (290, 350, 300)",
    "RIDE_TYPE_TWIST: (195, 157, 250)",
    "RIDE_TYPE_HAUNTED_HOUSE: (341, 153, 10)",
    "RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER: (733, 850, 809)",
};
