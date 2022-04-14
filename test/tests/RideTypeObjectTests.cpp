
#include "TestData.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <gtest/gtest.h>
#include <iterator>
#include <openrct2/ride/RideData.h>
#include <openrct2/core/Json.hpp>

class RideTypeObjectTests : public testing::Test
{
public:

private:

};

TEST_F(RideTypeObjectTests, CircusJsonExport)
{
    auto jsonResult = RideTypeDescriptorToJson(RIDE_TYPE_CIRCUS);
    const auto& rtd = GetRideTypeDescriptor(RIDE_TYPE_CIRCUS);

    //check the id, authors, version, sourceGame, object_type fields
    ASSERT_TRUE(jsonResult["id"] == "openrct2.ride_type.circus");
    ASSERT_TRUE(jsonResult["version"] == "1.0");
    ASSERT_TRUE(jsonResult["sourceGame"] == "rct2");
    ASSERT_TRUE(jsonResult["object_type"] == "ride_type");

    //test the fields
    auto properties = jsonResult["properties"];
    ASSERT_TRUE(properties["RideType"] == "circus");

    ASSERT_TRUE(properties["AlternateType"] == "");
    ASSERT_TRUE(properties["Category"] == "gentle");

    auto enabledTrackPieces = properties["EnabledTrackPieces"];
    ASSERT_TRUE(enabledTrackPieces.empty());
    ASSERT_TRUE(properties["StartTrackPiece"] == "flat_track_3x3");

    //check the flags, the order in the table is not hardcoded!
    //expect to have
    // has_single_piece_station, cannot_have_gaps, has_load_options, has_no_track, vehicle_is_integral,
    // track_no_walls, flat_ride, in_ride, has_vehicle_colours, music_on_default, has_entrance_exit, has_single_session, list_vehicle_separately
    const auto& flags = properties["Flags"];
    uint64_t flags_res = rtd.Flags;
    for (const auto& flag : flags)
    {
        if (flag == "has_single_piece_station")
            flags_res ^= RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION;
        else if (flag == "cannot_have_gaps")
            flags_res ^= RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS;
        else if (flag == "has_load_options")
            flags_res ^= RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS;
        else if (flag == "has_no_track")
            flags_res ^= RIDE_TYPE_FLAG_HAS_NO_TRACK;
        else if (flag == "vehicle_is_integral")
            flags_res ^= RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL;
        else if (flag == "track_no_walls")
            flags_res ^= RIDE_TYPE_FLAG_TRACK_NO_WALLS;
        else if (flag == "flat_ride")
            flags_res ^= RIDE_TYPE_FLAG_FLAT_RIDE;
        else if (flag == "in_ride")
            flags_res ^= RIDE_TYPE_FLAG_IN_RIDE;
        else if (flag == "has_vehicle_colours")
            flags_res ^= RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS;
        else if (flag == "music_on_default")
            flags_res ^= RIDE_TYPE_FLAG_MUSIC_ON_DEFAULT;
        else if (flag == "has_entrance_exit")
            flags_res ^= RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT;
        else if (flag == "single_session")
            flags_res ^= RIDE_TYPE_FLAG_SINGLE_SESSION;
        else if (flag == "list_vehicles_separately")
            flags_res ^= RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY;
    }
    //flags res should be zero, check flag by flag
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_HAS_NO_TRACK) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_TRACK_NO_WALLS) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_FLAT_RIDE) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_IN_RIDE) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_MUSIC_ON_DEFAULT) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_SINGLE_SESSION) == 0);
    ASSERT_TRUE((flags_res & RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY) == 0);

    //check that ride mode has only 1 element and it is circus
    ASSERT_TRUE(properties["RideModes"].size() == 1);
    ASSERT_TRUE(properties["RideModes"][0] == "circus");

    //check for OperatingSettings
    const auto& operatingSettings = properties["OperatingSettings"];
    ASSERT_TRUE(operatingSettings["MinValue"] == 0);
    ASSERT_TRUE(operatingSettings["MaxValue"] == 0);
    ASSERT_TRUE(operatingSettings["MaxBrakesSpeed"] == 0);
    ASSERT_TRUE(operatingSettings["PoweredLiftAcceleration"] == 0);
    ASSERT_TRUE(operatingSettings["BoosterAcceleration"] == 0);
    ASSERT_TRUE(operatingSettings["BoosterSpeedFactor"] == 0);

    //check for Naming
    const auto& naming = properties["Naming"];
    ASSERT_TRUE(naming["Name"] == "circus");
    ASSERT_TRUE(naming["Description"] == "circus");

    //check for NameConvention
    const auto& nameConvention = properties["NameConvention"];
    ASSERT_TRUE(nameConvention["Vehicle"] == "building");
    ASSERT_TRUE(nameConvention["Structure"] == "structure");
    ASSERT_TRUE(nameConvention["Station"] == "station");

    //check for available breakdowns
    //there should only be safety_cut_out
    const auto& availableBreakdowns = properties["AvailableBreakdowns"];
    ASSERT_TRUE(availableBreakdowns.size() == 1);
    ASSERT_TRUE(availableBreakdowns[0] == "safety_cut_out");

    //check for Heights
    const auto& heights = properties["Heights"];
    ASSERT_TRUE(heights["MaxHeight"] == 12);
    ASSERT_TRUE(heights["ClearanceHeight"] == 128);
    ASSERT_TRUE(heights["VehicleZOffset"] == 3);
    ASSERT_TRUE(heights["PlatformHeight"] == 2);

    //check for maxmass
    ASSERT_TRUE(properties["MaxMass"] == 255);

    //check for LiftData
    const auto& liftData = properties["LiftData"];
    ASSERT_TRUE(liftData["SoundId"] == "");
    ASSERT_TRUE(liftData["MinimumSpeed"] == 5);
    ASSERT_TRUE(liftData["MaximumSpeed"] == 5);

    //check for RatingsCalculationFunction
    ASSERT_TRUE(properties["RatingsCalculationFunction"] == "circus");

    //check for RatingsMultipliers
    const auto& ratingsMultipliers = properties["RatingsMultipliers"];
    ASSERT_TRUE(ratingsMultipliers["Excitement"] == 20);
    ASSERT_TRUE(ratingsMultipliers["Intensity"] == 10);
    ASSERT_TRUE(ratingsMultipliers["Nausea"] == 0);

    //check for UpkeepCosts
    const auto& upkeepCosts = properties["UpkeepCosts"];
    ASSERT_TRUE(upkeepCosts["BaseCost"] == 50);
    ASSERT_TRUE(upkeepCosts["TrackLengthMultiplier"] == 1);
    ASSERT_TRUE(upkeepCosts["CostPerTrackPiece"] == 0);
    ASSERT_TRUE(upkeepCosts["CostPerTrain"] == 0);
    ASSERT_TRUE(upkeepCosts["CostPerCar"] == 0);
    ASSERT_TRUE(upkeepCosts["CostPerStation"] == 0);

    //check for buildCosts
    const auto& buildCosts = properties["BuildCosts"];
    ASSERT_TRUE(buildCosts["TrackPrice"] == 62.50_GBP);
    ASSERT_TRUE(buildCosts["SupportPrice"] == 1.00_GBP);
    ASSERT_TRUE(buildCosts["PriceEstimateMultiplier"] == 1);

    //check for defaultPrices
    //expect 2 integers 15 and 0
    const auto& defaultPrices = properties["DefaultPrices"];
    ASSERT_TRUE(defaultPrices.size() == 2);
    ASSERT_TRUE(defaultPrices[0] == 15);
    ASSERT_TRUE(defaultPrices[1] == 0);

    // check for default music
    ASSERT_TRUE(properties["DefaultMusic"] == "");

    //check for photoItem
    ASSERT_TRUE(properties["PhotoItem"] == "photo");

    //bonus value
    ASSERT_TRUE(properties["BonusValue"] == 39);

    //colour presets
    //expect 1 entry in table, the entry is a struct with Main=bright_red, Additional=light_blue and Supports=yellow
    const auto& colourPresets = properties["ColourPresets"];
    ASSERT_TRUE(colourPresets.size() == 1);
    ASSERT_TRUE(colourPresets[0]["Main"] == "bright_red");
    ASSERT_TRUE(colourPresets[0]["Additional"] == "light_blue");
    ASSERT_TRUE(colourPresets[0]["Supports"] == "yellow");

    //colour preview is 2 integers
    const auto& colourPreview = properties["ColourPreview"];
    ASSERT_TRUE(colourPreview.size() == 2);
    ASSERT_TRUE(colourPreview["Track"] == 0);
    ASSERT_TRUE(colourPreview["Supports"] == 0);

    //colourKey
    ASSERT_TRUE(properties["ColourKey"] == "ride");

    //save the json dump to a file
    std::ofstream file("circus_dump.json");
    file << jsonResult.dump(4) << std::endl;
    file.close();
    SUCCEED();
}
