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

TEST(Rct1TablesTests, MapSlopedWall_SweepFullRangeAndOutOfRange)
{
    EXPECT_EQ(RCT1::MapSlopedWall(255), -1);

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

    for (uint32_t i = 0; i < static_cast<uint32_t>(RT::count); ++i)
        (void)RCT1::GetRideType(static_cast<RT>(i), VT::steelRollerCoasterTrain);
}

TEST(Rct1TablesTests, RideTypeUsesVehicles_AllCases)
{
    bool seenTrue = false;
    bool seenFalse = false;
    for (uint32_t i = 0; i < static_cast<uint32_t>(RCT1::RideType::count); ++i)
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

    for (uint32_t i = 0; i < static_cast<uint32_t>(VT::count); ++i)
        (void)RCT1::VehicleTypeIsReversed(static_cast<VT>(i));
}

TEST(Rct1TablesTests, GetVehicleSubEntryIndex_AllVehicleTypesAndSubEntries)
{
    using VT = RCT1::VehicleType;

    // The internal map has more than 256 entries, so any uint8_t sub-entry is safe to query.
    for (uint32_t v = 0; v < static_cast<uint32_t>(VT::count); ++v)
    {
        for (uint32_t s = 0; s <= 255; ++s)
            (void)RCT1::GetVehicleSubEntryIndex(static_cast<VT>(v), static_cast<uint8_t>(s));
    }

    (void)RCT1::GetVehicleSubEntryIndex(VT::heartlineTwisterCars, RCT1::HEARTLINE_TWISTER_FORWARDS);
    (void)RCT1::GetVehicleSubEntryIndex(VT::heartlineTwisterCars, 0);
    (void)RCT1::GetVehicleSubEntryIndex(VT::heartlineTwisterCarsReversed, RCT1::HEARTLINE_TWISTER_BACKWARDS);
    (void)RCT1::GetVehicleSubEntryIndex(VT::heartlineTwisterCarsReversed, 0);
}

TEST(Rct1TablesTests, GetColourSchemeCopyDescriptor_AllVehicleTypes)
{
    for (uint32_t i = 0; i < static_cast<uint32_t>(RCT1::VehicleType::count); ++i)
        (void)RCT1::GetColourSchemeCopyDescriptor(static_cast<RCT1::VehicleType>(i));
}

TEST(Rct1TablesTests, GetRideTypeObject_AllRideTypesBothExpansions)
{
    for (uint32_t i = 0; i < static_cast<uint32_t>(RCT1::RideType::count); ++i)
    {
        (void)RCT1::GetRideTypeObject(static_cast<RCT1::RideType>(i), false);
        (void)RCT1::GetRideTypeObject(static_cast<RCT1::RideType>(i), true);
    }
}

TEST(Rct1TablesTests, GetVehicleObject_AllVehicleTypes)
{
    for (uint32_t i = 0; i < static_cast<uint32_t>(RCT1::VehicleType::count); ++i)
        (void)RCT1::GetVehicleObject(static_cast<RCT1::VehicleType>(i));
}

TEST(Rct1TablesTests, GetSmallSceneryObject_SweepFullRangeAndOutOfRange)
{
    EXPECT_FALSE(RCT1::GetSmallSceneryObject(0).empty());
    EXPECT_TRUE(RCT1::GetSmallSceneryObject(255).empty());

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetSmallSceneryObject(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, GetLargeSceneryObject_SweepFullRangeAndOutOfRange)
{
    EXPECT_FALSE(RCT1::GetLargeSceneryObject(0).empty());
    EXPECT_TRUE(RCT1::GetLargeSceneryObject(255).empty());

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetLargeSceneryObject(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, GetBannerObject_SweepFullRangeAndOutOfRange)
{
    EXPECT_EQ(RCT1::GetBannerObject(RCT1::BannerType::plain), "rct2.footpath_banner.bn1");
    EXPECT_TRUE(RCT1::GetBannerObject(static_cast<RCT1::BannerType>(200)).empty());

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetBannerObject(static_cast<RCT1::BannerType>(i));
}

TEST(Rct1TablesTests, GetPathSurfaceObject_SweepFullRangeAndOutOfRange)
{
    EXPECT_FALSE(RCT1::GetPathSurfaceObject(0).empty());
    EXPECT_TRUE(RCT1::GetPathSurfaceObject(255).empty());

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetPathSurfaceObject(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, GetPathAddtionObject_SweepFullRangeAndOutOfRange)
{
    // Index 0 is RCT1_PATH_ADDITION_NONE which maps to an empty string
    // index 1 is the first real entry.
    EXPECT_FALSE(RCT1::GetPathAddtionObject(1).empty());
    EXPECT_TRUE(RCT1::GetPathAddtionObject(255).empty());

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetPathAddtionObject(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, GetFootpathRailingsObject_SweepFullRangeAndOutOfRange)
{
    EXPECT_FALSE(RCT1::GetFootpathRailingsObject(0).empty());
    EXPECT_TRUE(RCT1::GetFootpathRailingsObject(255).empty());

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetFootpathRailingsObject(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, GetSceneryGroupObject_SweepFullRangeAndOutOfRange)
{
    // Index 0 is RCT1_SCENERY_THEME_GENERAL which maps to an empty string
    // index 1 is the first real entry.
    EXPECT_FALSE(RCT1::GetSceneryGroupObject(1).empty());
    EXPECT_TRUE(RCT1::GetSceneryGroupObject(255).empty());

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetSceneryGroupObject(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, GetWaterObject_SweepFullRangeAndOutOfRange)
{
    EXPECT_FALSE(RCT1::GetWaterObject(0).empty());
    EXPECT_TRUE(RCT1::GetWaterObject(255).empty());

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetWaterObject(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, GetSceneryObjects_SweepFullRangeAndOutOfRange)
{
    EXPECT_FALSE(RCT1::GetSceneryObjects(0).empty());
    EXPECT_TRUE(RCT1::GetSceneryObjects(255).empty());

    for (uint32_t i = 0; i <= 255; ++i)
        (void)RCT1::GetSceneryObjects(static_cast<uint8_t>(i));
}

TEST(Rct1TablesTests, RCT1TrackTypeToOpenRCT2_FlatAndNonFlatRides)
{
    (void)RCT1::RCT1TrackTypeToOpenRCT2(static_cast<RCT12::TrackElemType>(0), RIDE_TYPE_FERRIS_WHEEL);
    (void)RCT1::RCT1TrackTypeToOpenRCT2(static_cast<RCT12::TrackElemType>(0), RIDE_TYPE_LOOPING_ROLLER_COASTER);
}
