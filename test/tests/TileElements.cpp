/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Map.h>
#include "TestData.h"

using namespace OpenRCT2;

class TileElementWantsFootpathConnection : public testing::Test
{
protected:
    static void SetUpTestCase()
    {
        std::string parkPath = TestData::GetParkPath("tile-element-tests.sv6");
        gOpenRCT2Headless    = true;
        gOpenRCT2NoGraphics  = true;
        _context             = CreateContext();
        bool initialised     = _context->Initialise();
        ASSERT_TRUE(initialised);

        load_from_sv6(parkPath.c_str());
        game_load_init();
        SUCCEED();
    }
private:
    static std::shared_ptr<IContext> _context;
};

std::shared_ptr<IContext> TileElementWantsFootpathConnection::_context;

TEST_F(TileElementWantsFootpathConnection, FlatPath)
{
    // Flat paths want to connect to other paths in any direction
    const rct_tile_element * const pathElement = map_get_footpath_element(19, 18, 14);
    ASSERT_NE(pathElement, nullptr);
    EXPECT_TRUE(tile_element_wants_path_connection_towards({ 19, 18, 14, 0 }, nullptr));
    EXPECT_TRUE(tile_element_wants_path_connection_towards({ 19, 18, 14, 1 }, nullptr));
    EXPECT_TRUE(tile_element_wants_path_connection_towards({ 19, 18, 14, 2 }, nullptr));
    EXPECT_TRUE(tile_element_wants_path_connection_towards({ 19, 18, 14, 3 }, nullptr));
    SUCCEED();
}

TEST_F(TileElementWantsFootpathConnection, SlopedPath)
{
    // Sloped paths only want to connect in two directions, of which is one at a higher offset
    const rct_tile_element * const slopedPathElement = map_get_footpath_element(18, 18, 14);
    ASSERT_NE(slopedPathElement, nullptr);
    ASSERT_TRUE(footpath_element_is_sloped(slopedPathElement));
    // Bottom and top of sloped path want a path connection
    EXPECT_TRUE(tile_element_wants_path_connection_towards({ 18, 18, 14, 2 }, nullptr));
    EXPECT_TRUE(tile_element_wants_path_connection_towards({ 18, 18, 16, 0 }, nullptr));
    // Other directions at both heights do not want a connection
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 18, 14, 0 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 18, 14, 1 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 18, 14, 3 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 18, 16, 1 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 18, 16, 2 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 18, 16, 3 }, nullptr));
    SUCCEED();
}

TEST_F(TileElementWantsFootpathConnection, Stall)
{
    // Stalls usually have one path direction flag, but can have multiple (info kiosk for example)
    const rct_tile_element * const stallElement = map_get_track_element_at(19 << 5, 15 << 5, 14);
    ASSERT_NE(stallElement, nullptr);
    EXPECT_TRUE(tile_element_wants_path_connection_towards({ 19, 15, 14, 0 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 15, 14, 1 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 15, 14, 2 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 15, 14, 3 }, nullptr));
    SUCCEED();
}

TEST_F(TileElementWantsFootpathConnection, RideEntrance)
{
    // Ride entrances and exits want a connection in one direction
    const rct_tile_element * const entranceElement = map_get_ride_entrance_element_at(18 << 5, 8 << 5, 14, false);
    ASSERT_NE(entranceElement, nullptr);
    EXPECT_TRUE(tile_element_wants_path_connection_towards({ 18, 8, 14, 0 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 8, 14, 1 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 8, 14, 2 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 8, 14, 3 }, nullptr));
    SUCCEED();
}

TEST_F(TileElementWantsFootpathConnection, RideExit)
{
    // The exit has been rotated; it wants a path connection in direction 1, but not 0 like the entrance
    const rct_tile_element * const exitElement = map_get_ride_exit_element_at(18 << 5, 10 << 5, 14, false);
    ASSERT_NE(exitElement, nullptr);
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 10, 14, 0 }, nullptr));
    EXPECT_TRUE(tile_element_wants_path_connection_towards({ 18, 10, 14, 1 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 10, 14, 2 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 10, 14, 3 }, nullptr));
    SUCCEED();
}

TEST_F(TileElementWantsFootpathConnection, DifferentHeight)
{
    // Test at different heights, all of these should fail
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 18, 4, 0 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 18, 4, 1 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 18, 4, 2 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 18, 4, 3 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 18, 4, 2 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 18, 6, 0 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 15, 4, 0 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 8, 4, 0 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 10, 4, 1 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 18, 24, 0 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 18, 24, 1 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 18, 24, 2 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 18, 24, 3 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 18, 24, 2 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 18, 26, 0 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 19, 15, 24, 0 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 8, 24, 0 }, nullptr));
    EXPECT_FALSE(tile_element_wants_path_connection_towards({ 18, 10, 24, 1 }, nullptr));
    SUCCEED();
}
