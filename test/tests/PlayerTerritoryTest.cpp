/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/competition/PlayerTerritory.h>

using namespace OpenRCT2::Competition;

class PlayerTerritoryTest : public testing::Test
{
protected:
    std::unique_ptr<PlayerTerritory> _territory;

    void SetUp() override
    {
        _territory = std::make_unique<PlayerTerritory>(1);
    }

    void TearDown() override
    {
        _territory.reset();
    }
};

// ====================
// Territory Bounds Tests
// ====================

TEST(TerritoryBoundsTest, ContainsTileCoords)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 0, 0 };
    bounds.BottomRight = TileCoordsXY{ 10, 10 };

    EXPECT_TRUE(bounds.Contains(TileCoordsXY{ 5, 5 }));
    EXPECT_TRUE(bounds.Contains(TileCoordsXY{ 0, 0 }));   // Top-left corner
    EXPECT_TRUE(bounds.Contains(TileCoordsXY{ 10, 10 })); // Bottom-right corner
}

TEST(TerritoryBoundsTest, NotContainsTileCoords)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 0, 0 };
    bounds.BottomRight = TileCoordsXY{ 10, 10 };

    EXPECT_FALSE(bounds.Contains(TileCoordsXY{ -1, 5 }));
    EXPECT_FALSE(bounds.Contains(TileCoordsXY{ 11, 5 }));
    EXPECT_FALSE(bounds.Contains(TileCoordsXY{ 5, -1 }));
    EXPECT_FALSE(bounds.Contains(TileCoordsXY{ 5, 11 }));
}

TEST(TerritoryBoundsTest, GetDimensions)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 0, 0 };
    bounds.BottomRight = TileCoordsXY{ 9, 9 };

    EXPECT_EQ(bounds.GetWidth(), 10);
    EXPECT_EQ(bounds.GetHeight(), 10);
    EXPECT_EQ(bounds.GetTileCount(), 100);
}

TEST(TerritoryBoundsTest, GetDimensionsAsymmetric)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 0, 0 };
    bounds.BottomRight = TileCoordsXY{ 19, 9 };

    EXPECT_EQ(bounds.GetWidth(), 20);
    EXPECT_EQ(bounds.GetHeight(), 10);
    EXPECT_EQ(bounds.GetTileCount(), 200);
}

// ====================
// Player Territory Tests
// ====================

TEST_F(PlayerTerritoryTest, OwnerIdSetCorrectly)
{
    EXPECT_EQ(_territory->GetOwnerId(), 1);
}

TEST_F(PlayerTerritoryTest, SetOwnerId)
{
    _territory->SetOwnerId(5);
    EXPECT_EQ(_territory->GetOwnerId(), 5);
}

TEST_F(PlayerTerritoryTest, SetBounds)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 0, 0 };
    bounds.BottomRight = TileCoordsXY{ 127, 127 };

    _territory->SetBounds(bounds);

    EXPECT_TRUE(_territory->HasBounds());
    EXPECT_EQ(_territory->GetBounds().TopLeft.x, 0);
    EXPECT_EQ(_territory->GetBounds().TopLeft.y, 0);
    EXPECT_EQ(_territory->GetBounds().BottomRight.x, 127);
    EXPECT_EQ(_territory->GetBounds().BottomRight.y, 127);
}

TEST_F(PlayerTerritoryTest, HasBoundsInitially)
{
    EXPECT_FALSE(_territory->HasBounds());
}

TEST_F(PlayerTerritoryTest, IsInTerritory)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 10, 10 };
    bounds.BottomRight = TileCoordsXY{ 20, 20 };
    _territory->SetBounds(bounds);

    EXPECT_TRUE(_territory->IsInTerritory(TileCoordsXY{ 15, 15 }));
    EXPECT_FALSE(_territory->IsInTerritory(TileCoordsXY{ 5, 5 }));
    EXPECT_FALSE(_territory->IsInTerritory(TileCoordsXY{ 25, 25 }));
}

TEST_F(PlayerTerritoryTest, CanPlayerBuildHere)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 0, 0 };
    bounds.BottomRight = TileCoordsXY{ 50, 50 };
    _territory->SetBounds(bounds);

    // Owner can build
    EXPECT_TRUE(_territory->CanPlayerBuildHere(1, TileCoordsXY{ 25, 25 }));
    
    // Non-owner cannot build
    EXPECT_FALSE(_territory->CanPlayerBuildHere(2, TileCoordsXY{ 25, 25 }));
    
    // Nobody can build outside bounds
    EXPECT_FALSE(_territory->CanPlayerBuildHere(1, TileCoordsXY{ 60, 60 }));
}

TEST_F(PlayerTerritoryTest, StatisticsInitiallyZero)
{
    EXPECT_EQ(_territory->GetTerritoryValue(), 0.00_GBP);
    EXPECT_EQ(_territory->GetGuestCount(), 0);
    EXPECT_EQ(_territory->GetRideCount(), 0);
    EXPECT_EQ(_territory->GetStaffCount(), 0);
}

// ====================
// Territory Manager Tests
// ====================

class TerritoryManagerTest : public testing::Test
{
protected:
    std::unique_ptr<TerritoryManager> _manager;

    void SetUp() override
    {
        _manager = std::make_unique<TerritoryManager>();
    }

    void TearDown() override
    {
        _manager.reset();
    }
};

TEST_F(TerritoryManagerTest, CreateTerritory)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 0, 0 };
    bounds.BottomRight = TileCoordsXY{ 50, 50 };

    _manager->CreateTerritory(1, bounds);

    auto* territory = _manager->GetTerritory(1);
    ASSERT_NE(territory, nullptr);
    EXPECT_EQ(territory->GetOwnerId(), 1);
}

TEST_F(TerritoryManagerTest, RemoveTerritory)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 0, 0 };
    bounds.BottomRight = TileCoordsXY{ 50, 50 };

    _manager->CreateTerritory(1, bounds);
    _manager->RemoveTerritory(1);

    EXPECT_EQ(_manager->GetTerritory(1), nullptr);
}

TEST_F(TerritoryManagerTest, GetTerritoryOwner)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 0, 0 };
    bounds.BottomRight = TileCoordsXY{ 50, 50 };

    _manager->CreateTerritory(1, bounds);

    EXPECT_EQ(_manager->GetTerritoryOwner(TileCoordsXY{ 25, 25 }), 1);
    EXPECT_EQ(_manager->GetTerritoryOwner(TileCoordsXY{ 75, 75 }), 0); // No owner
}

TEST_F(TerritoryManagerTest, CanPlayerBuildAt)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 0, 0 };
    bounds.BottomRight = TileCoordsXY{ 50, 50 };

    _manager->CreateTerritory(1, bounds);

    EXPECT_TRUE(_manager->CanPlayerBuildAt(1, TileCoordsXY{ 25, 25 }));
    EXPECT_FALSE(_manager->CanPlayerBuildAt(2, TileCoordsXY{ 25, 25 }));
    EXPECT_TRUE(_manager->CanPlayerBuildAt(1, TileCoordsXY{ 75, 75 })); // Neutral zone
}

TEST_F(TerritoryManagerTest, DivideMapEqually2Players)
{
    _manager->DivideMapEqually(2, 256, 256);

    EXPECT_EQ(_manager->GetTerritoryCount(), 2);
    
    auto* territory1 = _manager->GetTerritory(1);
    auto* territory2 = _manager->GetTerritory(2);
    
    ASSERT_NE(territory1, nullptr);
    ASSERT_NE(territory2, nullptr);
    
    // Should be split horizontally (2x1 grid)
    EXPECT_EQ(territory1->GetBounds().GetWidth(), 128);
    EXPECT_EQ(territory2->GetBounds().GetWidth(), 128);
}

TEST_F(TerritoryManagerTest, DivideMapEqually4Players)
{
    _manager->DivideMapEqually(4, 256, 256);

    EXPECT_EQ(_manager->GetTerritoryCount(), 4);
    
    // Should be split into 2x2 grid
    for (uint8_t i = 1; i <= 4; i++)
    {
        auto* territory = _manager->GetTerritory(i);
        ASSERT_NE(territory, nullptr);
        EXPECT_EQ(territory->GetBounds().GetWidth(), 128);
        EXPECT_EQ(territory->GetBounds().GetHeight(), 128);
    }
}

TEST_F(TerritoryManagerTest, DivideMapEqually6Players)
{
    _manager->DivideMapEqually(6, 300, 300);

    EXPECT_EQ(_manager->GetTerritoryCount(), 6);
    
    // Should be split into 3x2 grid
    for (uint8_t i = 1; i <= 6; i++)
    {
        auto* territory = _manager->GetTerritory(i);
        ASSERT_NE(territory, nullptr);
    }
}

TEST_F(TerritoryManagerTest, DivideMapEqually9Players)
{
    _manager->DivideMapEqually(9, 300, 300);

    EXPECT_EQ(_manager->GetTerritoryCount(), 9);
    
    // Should be split into 3x3 grid
    for (uint8_t i = 1; i <= 9; i++)
    {
        auto* territory = _manager->GetTerritory(i);
        ASSERT_NE(territory, nullptr);
        EXPECT_EQ(territory->GetBounds().GetWidth(), 100);
        EXPECT_EQ(territory->GetBounds().GetHeight(), 100);
    }
}

TEST_F(TerritoryManagerTest, AssignTerritoryToPlayer)
{
    _manager->DivideMapEqually(4, 256, 256);
    
    // Reassign territory 0 to player 5
    _manager->AssignTerritoryToPlayer(5, 0);
    
    auto* territory = _manager->GetTerritory(5);
    ASSERT_NE(territory, nullptr);
}

TEST_F(TerritoryManagerTest, Clear)
{
    TerritoryBounds bounds;
    bounds.TopLeft = TileCoordsXY{ 0, 0 };
    bounds.BottomRight = TileCoordsXY{ 50, 50 };

    _manager->CreateTerritory(1, bounds);
    _manager->CreateTerritory(2, bounds);
    
    _manager->Clear();
    
    EXPECT_EQ(_manager->GetTerritoryCount(), 0);
}

// ====================
// Edge Cases
// ====================

TEST_F(TerritoryManagerTest, DivideMapZeroPlayers)
{
    _manager->DivideMapEqually(0, 256, 256);
    EXPECT_EQ(_manager->GetTerritoryCount(), 0);
}

TEST_F(TerritoryManagerTest, GetNonExistentTerritory)
{
    auto* territory = _manager->GetTerritory(99);
    EXPECT_EQ(territory, nullptr);
}

TEST_F(TerritoryManagerTest, RemoveNonExistentTerritory)
{
    EXPECT_NO_THROW(_manager->RemoveTerritory(99));
}

TEST_F(TerritoryManagerTest, DivideMapLargePlayers)
{
    _manager->DivideMapEqually(12, 512, 512);
    EXPECT_EQ(_manager->GetTerritoryCount(), 12);
    
    // Should use 4xN grid
    for (uint8_t i = 1; i <= 12; i++)
    {
        auto* territory = _manager->GetTerritory(i);
        ASSERT_NE(territory, nullptr);
    }
}

// ====================
// Integration Tests
// ====================

TEST_F(TerritoryManagerTest, MultipleTerritoryOwnership)
{
    TerritoryBounds bounds1;
    bounds1.TopLeft = TileCoordsXY{ 0, 0 };
    bounds1.BottomRight = TileCoordsXY{ 50, 50 };

    TerritoryBounds bounds2;
    bounds2.TopLeft = TileCoordsXY{ 51, 0 };
    bounds2.BottomRight = TileCoordsXY{ 100, 50 };

    _manager->CreateTerritory(1, bounds1);
    _manager->CreateTerritory(2, bounds2);

    EXPECT_EQ(_manager->GetTerritoryOwner(TileCoordsXY{ 25, 25 }), 1);
    EXPECT_EQ(_manager->GetTerritoryOwner(TileCoordsXY{ 75, 25 }), 2);
    EXPECT_EQ(_manager->GetTerritoryOwner(TileCoordsXY{ 120, 25 }), 0);
}

TEST_F(TerritoryManagerTest, TerritoryBoundariesNoOverlap)
{
    _manager->DivideMapEqually(4, 256, 256);

    // Check that no two territories overlap
    std::vector<TileCoordsXY> testPoints = {
        {64, 64}, {192, 64}, {64, 192}, {192, 192}
    };

    for (const auto& point : testPoints)
    {
        int ownerCount = 0;
        for (uint8_t i = 1; i <= 4; i++)
        {
            auto* territory = _manager->GetTerritory(i);
            if (territory && territory->IsInTerritory(point))
            {
                ownerCount++;
            }
        }
        EXPECT_EQ(ownerCount, 1) << "Point (" << point.x << "," << point.y << ") has " << ownerCount << " owners";
    }
}
