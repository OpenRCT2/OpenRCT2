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
#include <memory>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Map.h>

using namespace OpenRCT2;

class TileElementWantsFootpathConnection : public testing::Test
{
protected:
    static void SetUpTestCase()
    {
        std::string parkPath = TestData::GetParkPath("tile-element-tests.sv6");
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;
        _context = CreateContext();
        bool initialised = _context->Initialise();
        ASSERT_TRUE(initialised);

        GetContext()->LoadParkFromFile(parkPath);
        game_load_init();

        // Changed in some tests. Store to restore its value
        _gScreenFlags = gScreenFlags;
        SUCCEED();
    }

    static void TearDownTestCase()
    {
        if (_context)
            _context.reset();

        gScreenFlags = _gScreenFlags;
    }

private:
    static std::shared_ptr<IContext> _context;
    static uint8_t _gScreenFlags;
};

std::shared_ptr<IContext> TileElementWantsFootpathConnection::_context;
uint8_t TileElementWantsFootpathConnection::_gScreenFlags;

TEST_F(TileElementWantsFootpathConnection, FlatPath)
{
    // Flat paths want to connect to other paths in any direction
    const TileElement* const pathElement = MapGetFootpathElement(TileCoordsXYZ{ 19, 18, 14 }.ToCoordsXYZ());
    ASSERT_NE(pathElement, nullptr);
    EXPECT_TRUE(TileElementWantsPathConnectionTowards({ 19, 18, 14, 0 }, nullptr));
    EXPECT_TRUE(TileElementWantsPathConnectionTowards({ 19, 18, 14, 1 }, nullptr));
    EXPECT_TRUE(TileElementWantsPathConnectionTowards({ 19, 18, 14, 2 }, nullptr));
    EXPECT_TRUE(TileElementWantsPathConnectionTowards({ 19, 18, 14, 3 }, nullptr));
    SUCCEED();
}

TEST_F(TileElementWantsFootpathConnection, SlopedPath)
{
    // Sloped paths only want to connect in two directions, of which is one at a higher offset
    const TileElement* const slopedPathElement = MapGetFootpathElement(TileCoordsXYZ{ 18, 18, 14 }.ToCoordsXYZ());
    ASSERT_NE(slopedPathElement, nullptr);
    ASSERT_TRUE(slopedPathElement->AsPath()->IsSloped());
    // Bottom and top of sloped path want a path connection
    EXPECT_TRUE(TileElementWantsPathConnectionTowards({ 18, 18, 14, 2 }, nullptr));
    EXPECT_TRUE(TileElementWantsPathConnectionTowards({ 18, 18, 16, 0 }, nullptr));
    // Other directions at both heights do not want a connection
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 18, 14, 0 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 18, 14, 1 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 18, 14, 3 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 18, 16, 1 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 18, 16, 2 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 18, 16, 3 }, nullptr));
    SUCCEED();
}

TEST_F(TileElementWantsFootpathConnection, Stall)
{
    // Stalls usually have one path direction flag, but can have multiple (info kiosk for example)
    auto tileCoords = TileCoordsXYZ{ 19, 15, 14 };
    const TrackElement* const stallElement = MapGetTrackElementAt(tileCoords.ToCoordsXYZ());
    ASSERT_NE(stallElement, nullptr);
    EXPECT_TRUE(TileElementWantsPathConnectionTowards({ 19, 15, 14, 0 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 15, 14, 1 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 15, 14, 2 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 15, 14, 3 }, nullptr));
    SUCCEED();
}

TEST_F(TileElementWantsFootpathConnection, RideEntrance)
{
    // Ride entrances and exits want a connection in one direction
    const EntranceElement* const entranceElement = MapGetRideEntranceElementAt(TileCoordsXYZ{ 18, 8, 14 }.ToCoordsXYZ(), false);
    ASSERT_NE(entranceElement, nullptr);
    EXPECT_TRUE(TileElementWantsPathConnectionTowards({ 18, 8, 14, 0 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 8, 14, 1 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 8, 14, 2 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 8, 14, 3 }, nullptr));
    SUCCEED();
}

TEST_F(TileElementWantsFootpathConnection, RideExit)
{
    // The exit has been rotated; it wants a path connection in direction 1, but not 0 like the entrance
    const EntranceElement* const exitElement = MapGetRideExitElementAt(TileCoordsXYZ{ 18, 10, 14 }.ToCoordsXYZ(), false);
    ASSERT_NE(exitElement, nullptr);
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 10, 14, 0 }, nullptr));
    EXPECT_TRUE(TileElementWantsPathConnectionTowards({ 18, 10, 14, 1 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 10, 14, 2 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 10, 14, 3 }, nullptr));
    SUCCEED();
}

TEST_F(TileElementWantsFootpathConnection, DifferentHeight)
{
    // Test at different heights, all of these should fail
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 18, 4, 0 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 18, 4, 1 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 18, 4, 2 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 18, 4, 3 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 18, 4, 2 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 18, 6, 0 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 15, 4, 0 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 8, 4, 0 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 10, 4, 1 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 18, 24, 0 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 18, 24, 1 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 18, 24, 2 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 18, 24, 3 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 18, 24, 2 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 18, 26, 0 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 19, 15, 24, 0 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 8, 24, 0 }, nullptr));
    EXPECT_FALSE(TileElementWantsPathConnectionTowards({ 18, 10, 24, 1 }, nullptr));
    SUCCEED();
}

TEST_F(TileElementWantsFootpathConnection, MapEdge)
{
    // Paths at the map edge should have edge flags turned on when placed in scenario editor
    // This tile is a single, unconnected footpath on the map edge - on load, GetEdges() returns 0
    TileElement* pathElement = MapGetFootpathElement(TileCoordsXYZ{ 1, 4, 14 }.ToCoordsXYZ());

    // Enable flag to simulate enabling the scenario editor
    gScreenFlags |= SCREEN_FLAGS_SCENARIO_EDITOR;

    // Calculate the connected edges and set the appropriate edge flags
    FootpathConnectEdges({ 16, 64 }, pathElement, 0);
    auto edges = pathElement->AsPath()->GetEdges();

    // The tiles alongside in the Y direction are both on the map edge so should be marked as an edge
    EXPECT_TRUE(edges & (1 << 1));
    EXPECT_TRUE(edges & (1 << 3));

    // The tile in the -X direction is off the map so should be marked as an edge
    EXPECT_TRUE(edges & (1 << 0));

    // The tile in the -X direction is a normal tile and should not be marked as an edge
    EXPECT_FALSE(edges & (1 << 2));
}
