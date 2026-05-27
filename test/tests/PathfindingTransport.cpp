/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/core/String.hpp>
#include <openrct2/entity/Guest.h>
#include <openrct2/peep/GuestPathfinding.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideManager.hpp>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Park.h>
#include <string>

using namespace OpenRCT2;

class TransportPathfindingTest : public testing::Test
{
protected:
    static std::unique_ptr<IContext> StartPark(const std::string& parkPath)
    {
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;

        auto context = CreateContext();
        if (!context->Initialise())
            return {};

        context->LoadParkFromFile(parkPath);
        GameLoadInit();
        gGameSpeed = 1;

        return context;
    }

    static Ride* FindRideByName(const char* name)
    {
        auto& gameState = getGameState();
        for (auto& ride : RideManager(gameState))
        {
            auto thisName = ride.getName();
            if (String::startsWith(thisName, u8string{ name }, true))
                return &ride;
        }
        return nullptr;
    }

    static Ride* FindTransportRide()
    {
        auto& gameState = getGameState();
        for (auto& ride : RideManager(gameState))
        {
            if (ride.getRideTypeDescriptor().flags.has(RtdFlag::isTransportRide) && ride.status == RideStatus::open
                && ride.numStations >= 2)
                return &ride;
        }
        return nullptr;
    }
};

TEST_F(TransportPathfindingTest, GuestsUseTransportToReachDistantRide)
{
    /* Two guests start near a tram station with a merry-go-round at the far end.
     * A walking path runs the full distance, but the tram provides a shortcut.
     * Both guests should board the tram (which needs 2 passengers to dispatch),
     * ride to the second station, exit, and walk to the merry-go-round. */
    std::string parkPath = TestData::GetParkPath("pathfinding-transport-tests.park");
    auto context = StartPark(parkPath);
    ASSERT_NE(context.get(), nullptr);

    auto& gameState = getGameState();

    auto* goalRide = FindRideByName("TransportDirect");
    ASSERT_NE(goalRide, nullptr);
    ASSERT_EQ(goalRide->status, RideStatus::open);

    auto* transport = FindTransportRide();
    ASSERT_NE(transport, nullptr);

    ScenarioRandSeed(0x12345678, 0x87654321);

    TileCoordsXYZ startTile{ 48, 24, 14 };
    auto startCoords = startTile.ToCoordsXYZ().ToTileCentre();

    auto* guest1 = Guest::Generate(startCoords);
    guest1->OutsideOfPark = false;
    guest1->CashInPocket = 0;
    guest1->GuestHeadingToRideId = goalRide->id;

    auto* guest2 = Guest::Generate(startCoords);
    guest2->OutsideOfPark = false;
    guest2->CashInPocket = 0;
    guest2->GuestHeadingToRideId = goalRide->id;

    gameState.cheats.ignoreRideIntensity = true;

    bool guest1ReachedGoal = false;
    bool guest2ReachedGoal = false;
    bool guest1UsedTransport = false;
    bool guest2UsedTransport = false;

    for (int tick = 0; tick < 20000; tick++)
    {
        gameStateUpdateLogic();

        if (guest1->State == PeepState::onRide && guest1->CurrentRide == transport->id)
            guest1UsedTransport = true;
        if (guest2->State == PeepState::onRide && guest2->CurrentRide == transport->id)
            guest2UsedTransport = true;

        if (guest1->State == PeepState::queuing && guest1->CurrentRide == goalRide->id)
            guest1ReachedGoal = true;
        if (guest2->State == PeepState::queuing && guest2->CurrentRide == goalRide->id)
            guest2ReachedGoal = true;

        if (guest1ReachedGoal && guest2ReachedGoal)
            break;
    }

    EXPECT_TRUE(guest1UsedTransport) << "Guest 1 did not board the transport ride";
    EXPECT_TRUE(guest2UsedTransport) << "Guest 2 did not board the transport ride";
    EXPECT_TRUE(guest1ReachedGoal) << "Guest 1 did not reach the merry-go-round";
    EXPECT_TRUE(guest2ReachedGoal) << "Guest 2 did not reach the merry-go-round";
}
