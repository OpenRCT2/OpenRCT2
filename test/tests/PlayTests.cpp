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
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/actions/ParkSetEntranceFeeAction.h>
#include <openrct2/actions/ParkSetParameterAction.h>
#include <openrct2/actions/RideSetPriceAction.h>
#include <openrct2/actions/RideSetStatusAction.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/EntityTweener.h>
#include <openrct2/entity/Peep.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/world/MapAnimation.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <string>

using namespace OpenRCT2;

class PlayTests : public testing::Test
{
};

static std::unique_ptr<IContext> localStartGame(const std::string& parkPath)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;
    Platform::CoreInit();

    auto context = CreateContext();
    if (!context->Initialise())
        return {};

    auto importer = ParkImporter::CreateS6(context->GetObjectRepository());
    auto loadResult = importer->LoadSavedGame(parkPath.c_str(), false);
    context->GetObjectManager().LoadObjects(loadResult.RequiredObjects);
    importer->Import();

    ResetEntitySpatialIndices();

    reset_all_sprite_quadrant_placements();
    ScenerySetDefaultPlacementConfiguration();
    load_palette();
    EntityTweener::Get().Reset();
    MapAnimationAutoCreate();
    fix_invalid_vehicle_sprite_sizes();

    gGameSpeed = 1;

    return context;
}

template<class Fn> static bool updateUntil(GameState& gs, int maxSteps, Fn&& fn)
{
    while (maxSteps-- && !fn())
    {
        gs.UpdateLogic();
    }
    return maxSteps > 0;
}

template<class GA, class... Args> static void execute(Args&&... args)
{
    GA ga(std::forward<Args>(args)...);
    GameActions::Execute(&ga);
}

TEST_F(PlayTests, SecondGuestInQueueShouldNotRideIfNoFunds)
{
    /* This test verifies that a guest, when second in queue, won't be forced to enter
     * the ride if it has not enough money to pay for it.
     * To simulate this scenario, two guests (a rich and a poor) are encouraged to enter
     * the ride queue, and then the price is raised such that the second guest in line
     * (the poor one) cannot pay. The poor guest should not enter the ride.
     */
    std::string initStateFile = TestData::GetParkPath("small_park_with_ferris_wheel.sv6");

    auto context = localStartGame(initStateFile);
    ASSERT_NE(context.get(), nullptr);

    auto gs = context->GetGameState();
    ASSERT_NE(gs, nullptr);

    // Open park for free but charging for rides
    execute<ParkSetParameterAction>(ParkParameter::Open);
    execute<ParkSetEntranceFeeAction>(0);
    gParkFlags |= PARK_FLAGS_UNLOCK_ALL_PRICES;

    // Find ferris wheel
    auto rideManager = GetRideManager();
    auto it = std::find_if(
        rideManager.begin(), rideManager.end(), [](auto& ride) { return ride.type == RIDE_TYPE_FERRIS_WHEEL; });
    ASSERT_NE(it, rideManager.end());
    Ride& ferrisWheel = *it;

    // Open it for free
    execute<RideSetStatusAction>(ferrisWheel.id, RideStatus::Open);
    execute<RideSetPriceAction>(ferrisWheel.id, 0, true);

    // Ignore intensity to stimulate peeps to queue into ferris wheel
    gCheatsIgnoreRideIntensity = true;

    // Insert a rich guest
    auto richGuest = gs->GetPark().GenerateGuest();
    richGuest->CashInPocket = 3000;

    // Wait for rich guest to get in queue
    bool matched = updateUntil(*gs, 1000, [&]() { return richGuest->State == PeepState::Queuing; });
    ASSERT_TRUE(matched);

    // Insert poor guest
    auto poorGuest = gs->GetPark().GenerateGuest();
    poorGuest->CashInPocket = 5;

    // Wait for poor guest to get in queue
    matched = updateUntil(*gs, 1000, [&]() { return poorGuest->State == PeepState::Queuing; });
    ASSERT_TRUE(matched);

    // Raise the price of the ride to a value poor guest can't pay
    execute<RideSetPriceAction>(ferrisWheel.id, 10, true);

    // Verify that the poor guest goes back to walking without riding
    // since it doesn't have enough money to pay for it
    bool enteredTheRide = false;
    matched = updateUntil(*gs, 10000, [&]() {
        enteredTheRide |= poorGuest->State == PeepState::OnRide;
        return poorGuest->State == PeepState::Walking || enteredTheRide;
    });

    ASSERT_TRUE(matched);
    ASSERT_FALSE(enteredTheRide);
}

TEST_F(PlayTests, CarRideWithOneCarOnlyAcceptsTwoGuests)
{
    // This test verifies that a car ride with one car will accept at most two guests
    std::string initStateFile = TestData::GetParkPath("small_park_car_ride_one_car.sv6");

    auto context = localStartGame(initStateFile);
    ASSERT_NE(context.get(), nullptr);

    auto gs = context->GetGameState();
    ASSERT_NE(gs, nullptr);

    // Open park for free but charging for rides
    execute<ParkSetParameterAction>(ParkParameter::Open);
    execute<ParkSetEntranceFeeAction>(0);
    gParkFlags |= PARK_FLAGS_UNLOCK_ALL_PRICES;

    // Find car ride
    auto rideManager = GetRideManager();
    auto it = std::find_if(rideManager.begin(), rideManager.end(), [](auto& ride) { return ride.type == RIDE_TYPE_CAR_RIDE; });
    ASSERT_NE(it, rideManager.end());
    Ride& carRide = *it;

    // Open it for free
    execute<RideSetStatusAction>(carRide.id, RideStatus::Open);
    execute<RideSetPriceAction>(carRide.id, 0, true);

    // Ignore intensity to stimulate peeps to queue into the ride
    gCheatsIgnoreRideIntensity = true;

    // Create some guests
    std::vector<Peep*> guests;
    for (int i = 0; i < 25; i++)
    {
        guests.push_back(gs->GetPark().GenerateGuest());
    }

    // Wait until one of them is riding
    auto guestIsOnRide = [](auto* g) { return g->State == PeepState::OnRide; };
    bool matched = updateUntil(*gs, 10000, [&]() { return std::any_of(guests.begin(), guests.end(), guestIsOnRide); });
    ASSERT_TRUE(matched);

    // For the next few ticks at most two guests can be on the ride
    for (int i = 0; i < 100; i++)
    {
        int numRiding = std::count_if(guests.begin(), guests.end(), guestIsOnRide);
        ASSERT_LE(numRiding, 2);
        gs->UpdateLogic();
    }
}
