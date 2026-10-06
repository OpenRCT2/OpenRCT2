/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/drawing/Colour.h>
#include <openrct2/rct1/RCT1.h>
#include <openrct2/rct1/Tables.h>
#include <openrct2/rct12/RCT12.h>
#include <openrct2/ride/Ride.h>

using namespace OpenRCT2;

namespace
{
    constexpr uint8_t kNumRct1RideTypes = 85;
    constexpr uint8_t kNumRct1VehicleTypes = 89;
    constexpr uint8_t kNumRct1BannerTypes = 9;
    constexpr uint8_t kNumRct1SceneryThemes = 18;
    constexpr uint8_t kNumRct1SmallScenery = 244;
    constexpr uint8_t kNumRct1LargeScenery = 39;
    constexpr uint8_t kNumRct1PathSurfaces = 24;
    constexpr uint8_t kNumRct1PathAdditions = 15;
    constexpr uint8_t kNumRct1FootpathRailings = 4;
    constexpr uint8_t kNumRct1SceneryGroups = 18;
    constexpr uint8_t kNumRct1WaterTypes = 2;
} // namespace

TEST(Rct1TablesTests, GetColour_KnownAndOutOfRange)
{
    EXPECT_EQ(RCT1::GetColour(0), Drawing::Colour::black);
    EXPECT_EQ(RCT1::GetColour(31), Drawing::Colour::icyBlue);
    EXPECT_EQ(RCT1::GetColour(32), Drawing::Colour::black);

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetColour(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, GetPeepAnimationGroup_KnownAndOutOfRange)
{
    EXPECT_EQ(RCT1::GetPeepAnimationGroup(static_cast<RCT1::PeepAnimationGroup>(0)), RCT12PeepAnimationGroup::normal);
    EXPECT_EQ(RCT1::GetPeepAnimationGroup(static_cast<RCT1::PeepAnimationGroup>(0xFF)), RCT12PeepAnimationGroup::normal);

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetPeepAnimationGroup(static_cast<RCT1::PeepAnimationGroup>(i));
}

TEST(Rct1TablesTests, GetTerrainSurfaceObject_SweepFullRange)
{
    EXPECT_EQ(RCT1::GetTerrainSurfaceObject(0), "rct2.terrain_surface.grass");
    EXPECT_EQ(RCT1::GetTerrainSurfaceObject(255), RCT1::GetTerrainSurfaceObject(0));

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetTerrainSurfaceObject(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, GetTerrainEdgeObject_SweepFullRange)
{
    EXPECT_EQ(RCT1::GetTerrainEdgeObject(0), "rct2.terrain_edge.rock");
    EXPECT_EQ(RCT1::GetTerrainEdgeObject(255), RCT1::GetTerrainEdgeObject(0));

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetTerrainEdgeObject(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, MapSlopedWall_SweepFullRange)
{
    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::MapSlopedWall(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, GetWallObject_SweepFullRange)
{
    EXPECT_FALSE(RCT1::GetWallObject(0).empty());
    EXPECT_FALSE(RCT1::GetWallObject(255).empty());

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetWallObject(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, GetRideType_SpecialCasesAndAllRideTypes)
{
    using RT = RCT1::RideType;
    using VT = RCT1::VehicleType;

    EXPECT_EQ(
        RCT1::GetRideType(RT::steelTwisterRollerCoaster, VT::nonLoopingSteelTwisterRollerCoasterTrain),
        RIDE_TYPE_HYPER_TWISTER);
    EXPECT_EQ(RCT1::GetRideType(RT::steelCorkscrewRollerCoaster, VT::hypercoasterTrain), RIDE_TYPE_HYPERCOASTER);

    for (uint8_t i = 0; i < kNumRct1RideTypes; ++i)
        (void)RCT1::GetRideType(static_cast<RT>(i), VT::steelRollerCoasterTrain);
}

TEST(Rct1TablesTests, RideTypeUsesVehicles_AllCases)
{
    bool seenTrue = false;
    bool seenFalse = false;
    for (uint8_t i = 0; i < kNumRct1RideTypes; ++i)
    {
        const bool uses = RCT1::RideTypeUsesVehicles(static_cast<RCT1::RideType>(i));
        seenTrue = seenTrue || uses;
        seenFalse = seenFalse || !uses;
    }
    EXPECT_TRUE(seenTrue);
    EXPECT_TRUE(seenFalse);
}

TEST(Rct1TablesTests, PathIsQueue_AllPathTypes)
{
    EXPECT_TRUE(RCT1::PathIsQueue(0));
    EXPECT_TRUE(RCT1::PathIsQueue(1));
    EXPECT_TRUE(RCT1::PathIsQueue(2));
    EXPECT_TRUE(RCT1::PathIsQueue(3));
    for (uint32_t i = 4; i <= 255; ++i)
        EXPECT_FALSE(RCT1::PathIsQueue(static_cast<uint8_t>(i)));
}

TEST(Rct1TablesTests, NormalisePathAddition_BrokenIdsMapToWhole)
{
    for (uint32_t i = 0; i <= 255; ++i)
    {
        const uint8_t in = static_cast<uint8_t>(i);
        const uint8_t out = RCT1::NormalisePathAddition(in);
        EXPECT_TRUE(out == in || out < in);
    }
}

TEST(Rct1TablesTests, VehicleTypeIsReversed_KnownReversedSet)
{
    using VT = RCT1::VehicleType;

    EXPECT_TRUE(RCT1::VehicleTypeIsReversed(VT::steelRollerCoasterTrainBackwards));
    EXPECT_TRUE(RCT1::VehicleTypeIsReversed(VT::woodenRollerCoasterTrainBackwards));
    EXPECT_TRUE(RCT1::VehicleTypeIsReversed(VT::heartlineTwisterCarsReversed));

    EXPECT_FALSE(RCT1::VehicleTypeIsReversed(VT::steelRollerCoasterTrain));
    EXPECT_FALSE(RCT1::VehicleTypeIsReversed(VT::woodenRollerCoasterTrain));

    for (uint8_t i = 0; i < kNumRct1VehicleTypes; ++i)
        (void)RCT1::VehicleTypeIsReversed(static_cast<VT>(i));
}

TEST(Rct1TablesTests, GetVehicleSubEntryIndex_AllVehicleTypes)
{
    using VT = RCT1::VehicleType;

    for (uint8_t v = 0; v < kNumRct1VehicleTypes; ++v)
    {
        for (uint8_t s = 0; s < 4; ++s)
            (void)RCT1::GetVehicleSubEntryIndex(static_cast<VT>(v), s);
    }

    (void)RCT1::GetVehicleSubEntryIndex(VT::heartlineTwisterCars, RCT1::HEARTLINE_TWISTER_FORWARDS);
    (void)RCT1::GetVehicleSubEntryIndex(VT::heartlineTwisterCars, 0);
    (void)RCT1::GetVehicleSubEntryIndex(VT::heartlineTwisterCarsReversed, RCT1::HEARTLINE_TWISTER_BACKWARDS);
    (void)RCT1::GetVehicleSubEntryIndex(VT::heartlineTwisterCarsReversed, 0);
}

TEST(Rct1TablesTests, GetColourSchemeCopyDescriptor_AllVehicleTypes)
{
    for (uint8_t i = 0; i < kNumRct1VehicleTypes; ++i)
        (void)RCT1::GetColourSchemeCopyDescriptor(static_cast<RCT1::VehicleType>(i));
}

TEST(Rct1TablesTests, GetRideTypeObject_AllRideTypesBothExpansions)
{
    for (uint8_t i = 0; i < kNumRct1RideTypes; ++i)
    {
        (void)RCT1::GetRideTypeObject(static_cast<RCT1::RideType>(i), false);
        (void)RCT1::GetRideTypeObject(static_cast<RCT1::RideType>(i), true);
    }
}

TEST(Rct1TablesTests, GetVehicleObject_AllVehicleTypes)
{
    for (uint8_t i = 0; i < kNumRct1VehicleTypes; ++i)
        (void)RCT1::GetVehicleObject(static_cast<RCT1::VehicleType>(i));
}

TEST(Rct1TablesTests, GetSmallSceneryObject_AllIndices)
{
    for (uint8_t i = 0; i < kNumRct1SmallScenery; ++i)
        (void)RCT1::GetSmallSceneryObject(i);
}

TEST(Rct1TablesTests, GetLargeSceneryObject_AllIndices)
{
    for (uint8_t i = 0; i < kNumRct1LargeScenery; ++i)
        (void)RCT1::GetLargeSceneryObject(i);
}

TEST(Rct1TablesTests, GetBannerObject_AllBannerTypes)
{
    for (uint8_t i = 0; i < kNumRct1BannerTypes; ++i)
        (void)RCT1::GetBannerObject(static_cast<RCT1::BannerType>(i));
}

TEST(Rct1TablesTests, GetPathSurfaceObject_AllPathTypes)
{
    for (uint8_t i = 0; i < kNumRct1PathSurfaces; ++i)
        (void)RCT1::GetPathSurfaceObject(i);
}

TEST(Rct1TablesTests, GetPathAddtionObject_AllPathAdditions)
{
    for (uint8_t i = 0; i < kNumRct1PathAdditions; ++i)
        (void)RCT1::GetPathAddtionObject(i);
}

TEST(Rct1TablesTests, GetFootpathRailingsObject_AllRailings)
{
    for (uint8_t i = 0; i < kNumRct1FootpathRailings; ++i)
        (void)RCT1::GetFootpathRailingsObject(i);
}

TEST(Rct1TablesTests, GetSceneryGroupObject_AllSceneryGroups)
{
    for (uint8_t i = 0; i < kNumRct1SceneryGroups; ++i)
        (void)RCT1::GetSceneryGroupObject(i);
}

TEST(Rct1TablesTests, GetWaterObject_AllWaterTypes)
{
    for (uint8_t i = 0; i < kNumRct1WaterTypes; ++i)
        (void)RCT1::GetWaterObject(i);
}

TEST(Rct1TablesTests, GetSceneryObjects_AllSceneryThemes)
{
    for (uint8_t i = 0; i < kNumRct1SceneryThemes; ++i)
    {
        const auto objects = RCT1::GetSceneryObjects(i);
        EXPECT_FALSE(objects.empty());
    }
}

TEST(Rct1TablesTests, RCT1TrackTypeToOpenRCT2_FlatAndNonFlatRides)
{
    (void)RCT1::RCT1TrackTypeToOpenRCT2(static_cast<RCT12::TrackElemType>(0), RIDE_TYPE_FERRIS_WHEEL);
    (void)RCT1::RCT1TrackTypeToOpenRCT2(static_cast<RCT12::TrackElemType>(0), RIDE_TYPE_LOOPING_ROLLER_COASTER);
}
