/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Vehicle.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../actions/GameActionRunner.h"
#include "../actions/ride/RideSetStatusAction.h"
#include "../audio/Audio.h"
#include "../config/Config.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Guest.h"
#include "../entity/Particle.h"
#include "../entity/Peep.h"
#include "../localisation/Formatter.h"
#include "../management/NewsItem.h"
#include "../scenario/Scenario.h"
#include "../scripting/HookEngine.h"
#include "../scripting/ScriptEngine.h"
#include "../windows/Intent.h"
#include "../world/Map.h"
#include "../world/tile_element/TileElement.h"
#include "Ride.h"
#include "VehicleGeometry.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::RideVehicle;

#ifdef ENABLE_SCRIPTING
/**
 * Fires the "vehicle.crash" api hook
 * @param vehicleId Entity id of the vehicle that just crashed
 * @param crashId What the vehicle crashed into. Should be either "another_vehicle", "land", or "water"
 */
static void InvokeVehicleCrashHook(const EntityId vehicleId, const std::string_view crashId)
{
    auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
    if (hookEngine.HasSubscriptions(Scripting::HookType::vehicleCrash))
    {
        JSContext* ctx = GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "id", JS_NewInt64(ctx, vehicleId.ToUnderlying()));
        JS_SetPropertyStr(ctx, obj, "crashIntoType", Scripting::JSFromStdString(ctx, crashId));

        // Call the subscriptions
        hookEngine.Call(Scripting::HookType::vehicleCrash, obj, true);
    }
}
#endif

void Vehicle::SimulateCrash() const
{
    auto curRide = GetRide();
    if (curRide != nullptr)
    {
        curRide->flags.set(RideFlag::crashed);
    }
}

/**
 * Setup function for a vehicle colliding with
 * another vehicle.
 *
 *  rct2: 0x006DA059
 */
void Vehicle::UpdateCollisionSetup()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RideStatus::simulating)
    {
        SimulateCrash();
        return;
    }

    SetState(Status::crashed, sub_state);

    if (!curRide->flags.has(RideFlag::crashed))
    {
        auto frontVehicle = GetHead();
        auto trainIndex = ride_get_train_index_from_vehicle(*curRide, frontVehicle->Id);
        if (!trainIndex.has_value())
        {
            return;
        }

        curRide->crash(trainIndex.value());

        if (curRide->status != RideStatus::closed)
        {
            // We require this to execute right away during the simulation, always ignore network and queue.
            auto gameAction = GameActions::RideSetStatusAction(curRide->id, RideStatus::closed);
            GameActions::ExecuteNested(&gameAction, getGameState());
        }
    }

    curRide->flags.set(RideFlag::crashed);
    curRide->windowInvalidateFlags.set(RideInvalidateFlag::main, RideInvalidateFlag::list);
    KillAllPassengersInTrain();

    Vehicle* lastVehicle = this;
    for (Vehicle* train = getGameState().entities.GetEntity<Vehicle>(Id); train != nullptr;
         train = getGameState().entities.GetEntity<Vehicle>(train->next_vehicle_on_train))
    {
        lastVehicle = train;

        train->sub_state = 2;

#ifdef ENABLE_SCRIPTING
        InvokeVehicleCrashHook(train->Id, "another_vehicle");
#endif
        const auto trainLoc = train->GetLocation();

        Play3D(SoundId::crash, trainLoc);

        ExplosionCloud::Create(trainLoc);

        for (int32_t i = 0; i < 10; i++)
        {
            VehicleCrashParticle::Create(train->colours, trainLoc);
        }

        train->flags.set(VehicleFlag::crashed);
        train->animationState = ScenarioRand() & 0xFFFF;

        train->animation_frame = ScenarioRand() & 0x7;
        train->SpriteData.Width = 13;
        train->SpriteData.HeightMin = 45;
        train->SpriteData.HeightMax = 5;

        train->MoveTo(trainLoc);

        train->SwingSpeed = 0;
    }

    // Remove the current train from the ride linked list of trains
    auto prevTrain = getGameState().entities.GetEntity<Vehicle>(prev_vehicle_on_ride);
    auto nextTrain = getGameState().entities.GetEntity<Vehicle>(lastVehicle->next_vehicle_on_ride);
    if (prevTrain == nullptr || nextTrain == nullptr)
    {
        LOG_ERROR("Corrupted vehicle list for ride!");
    }
    else
    {
        prevTrain->next_vehicle_on_ride = lastVehicle->next_vehicle_on_ride;
        nextTrain->prev_vehicle_on_ride = prev_vehicle_on_ride;
    }

    velocity = 0;
}

/**
 *
 *  rct2: 0x006D9EFE
 */
void Vehicle::UpdateCrashSetup()
{
    auto curRide = GetRide();
    if (curRide != nullptr && curRide->status == RideStatus::simulating)
    {
        SimulateCrash();
        return;
    }
    SetState(Status::crashing, sub_state);

    if (NumPeepsUntilTrainTail() != 0)
    {
        Play3D(SoundId::hauntedHouseScream2, GetLocation());
    }

    int32_t edx = velocity >> 10;

    Vehicle* lastVehicle = this;
    auto spriteId = Id;
    for (Vehicle* trainVehicle; !spriteId.IsNull(); spriteId = trainVehicle->next_vehicle_on_train)
    {
        trainVehicle = getGameState().entities.GetEntity<Vehicle>(spriteId);
        if (trainVehicle == nullptr)
        {
            break;
        }
        lastVehicle = trainVehicle;

        trainVehicle->sub_state = 0;
        auto crashDirection = Geometry::getCrashDirectionComponents(trainVehicle->Orientation);
        int32_t trainX = crashDirection.x;
        int32_t trainY = crashDirection.y;

        auto carLaunchDirection = Geometry::getPitchVector32(trainVehicle->pitch);

        auto trainZ = carLaunchDirection.y >> 23;
        int32_t ecx = carLaunchDirection.x >> 15;
        trainX *= ecx;
        trainY *= ecx;
        trainX >>= 16;
        trainY >>= 16;
        trainX *= edx;
        trainY *= edx;
        trainZ *= edx;
        trainX >>= 8;
        trainY >>= 8;
        trainZ >>= 8;

        trainVehicle->crash_x = trainX;
        trainVehicle->crash_y = trainY;
        trainVehicle->crash_z = trainZ;
        trainVehicle->crash_x += (ScenarioRand() & 0xF) - 8;
        trainVehicle->crash_y += (ScenarioRand() & 0xF) - 8;
        trainVehicle->crash_z += (ScenarioRand() & 0xF) - 8;

        trainVehicle->TrackLocation = { 0, 0, 0 };
    }

    // Remove the current train from the ride linked list of trains
    auto prevTrain = getGameState().entities.GetEntity<Vehicle>(prev_vehicle_on_ride);
    auto nextTrain = getGameState().entities.GetEntity<Vehicle>(lastVehicle->next_vehicle_on_ride);
    if (prevTrain == nullptr || nextTrain == nullptr)
    {
        LOG_ERROR("Corrupted vehicle list for ride!");
    }
    else
    {
        prevTrain->next_vehicle_on_ride = lastVehicle->next_vehicle_on_ride;
        nextTrain->prev_vehicle_on_ride = prev_vehicle_on_ride;
    }
    velocity = 0;
}

/**
 *
 *  rct2: 0x0068B8BD
 * @returns the map element that the vehicle will collide with or NULL if no collisions.
 */
static TileElement* vehicle_check_collision(const CoordsXYZ& vehiclePosition)
{
    TileElement* tileElement = MapGetFirstElementAt(vehiclePosition);
    if (tileElement == nullptr)
    {
        return nullptr;
    }

    uint8_t quadrant;
    if ((vehiclePosition.x & 0x1F) >= 16)
    {
        quadrant = 1;
        if ((vehiclePosition.y & 0x1F) < 16)
            quadrant = 2;
    }
    else
    {
        quadrant = 4;
        if ((vehiclePosition.y & 0x1F) >= 16)
            quadrant = 8;
    }

    do
    {
        if (vehiclePosition.z < tileElement->GetBaseZ())
            continue;

        if (vehiclePosition.z >= tileElement->GetClearanceZ())
            continue;

        if (tileElement->GetOccupiedQuadrants() & quadrant)
            return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

static void ride_train_crash(Ride& ride, uint16_t numFatalities)
{
    Formatter ft;
    ft.Add<uint16_t>(numFatalities);

    uint8_t crashType = numFatalities == 0 ? RIDE_CRASH_TYPE_NO_FATALITIES : RIDE_CRASH_TYPE_FATALITIES;

    if (crashType >= ride.lastCrashType)
        ride.lastCrashType = crashType;

    if (numFatalities != 0)
    {
        if (Config::Get().notifications.rideCasualties)
        {
            ride.formatNameTo(ft);
            News::AddItemToQueue(
                News::ItemType::ride, numFatalities == 1 ? STR_X_PERSON_DIED_ON_X : STR_X_PEOPLE_DIED_ON_X,
                ride.id.ToUnderlying(), ft);
        }

        // TODO: get park id from ride/vehicle
        auto& park = getGameState().park;
        if (park.ratingCasualtyPenalty < 500)
        {
            park.ratingCasualtyPenalty += 200;
        }
    }
}
/**
 *
 *  rct2: 0x006DE6C6
 */
void Vehicle::KillAllPassengersInTrain()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    ride_train_crash(*curRide, NumPeepsUntilTrainTail());

    for (Vehicle* trainCar = getGameState().entities.GetEntity<Vehicle>(Id); trainCar != nullptr;
         trainCar = getGameState().entities.GetEntity<Vehicle>(trainCar->next_vehicle_on_train))
    {
        trainCar->KillPassengers(*curRide);
    }
}

void Vehicle::KillPassengers(const Ride& curRide)
{
    if (num_peeps != next_free_seat)
        return;

    if (num_peeps == 0)
        return;

    for (auto i = 0; i < num_peeps; i++)
    {
        auto* curPeep = getGameState().entities.GetEntity<Guest>(peep[i]);
        if (curPeep == nullptr)
            continue;

        if (!curPeep->OutsideOfPark)
        {
            DecrementGuestsInPark();
            auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
            ContextBroadcastIntent(&intent);
        }
        PeepEntityRemove(curPeep);
    }

    num_peeps = 0;
    next_free_seat = 0;
}

void Vehicle::CrashOnLand()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RideStatus::simulating)
    {
        SimulateCrash();
        return;
    }
    SetState(Status::crashed, sub_state);

#ifdef ENABLE_SCRIPTING
    InvokeVehicleCrashHook(Id, "land");
#endif

    if (!curRide->flags.has(RideFlag::crashed))
    {
        auto frontVehicle = GetHead();
        auto trainIndex = ride_get_train_index_from_vehicle(*curRide, frontVehicle->Id);
        if (!trainIndex.has_value())
        {
            return;
        }

        curRide->crash(trainIndex.value());

        if (curRide->status != RideStatus::closed)
        {
            // We require this to execute right away during the simulation, always ignore network and queue.
            auto gameAction = GameActions::RideSetStatusAction(curRide->id, RideStatus::closed);
            GameActions::ExecuteNested(&gameAction, getGameState());
        }
    }
    curRide->flags.set(RideFlag::crashed);
    curRide->windowInvalidateFlags.set(RideInvalidateFlag::main, RideInvalidateFlag::list);

    if (IsHead())
    {
        KillAllPassengersInTrain();
    }

    sub_state = 2;

    const auto curLoc = GetLocation();
    Play3D(SoundId::crash, curLoc);

    ExplosionCloud::Create(curLoc);
    ExplosionFlare::Create(curLoc);

    uint8_t numParticles = std::min(SpriteData.Width, static_cast<uint8_t>(7));

    while (numParticles-- != 0)
        VehicleCrashParticle::Create(colours, curLoc);

    flags.set(VehicleFlag::crashed);
    animation_frame = 0;
    animationState = 0;
    SpriteData.Width = 13;
    SpriteData.HeightMin = 45;
    SpriteData.HeightMax = 5;

    MoveTo(curLoc);

    crash_z = 0;
}

void Vehicle::CrashOnWater()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RideStatus::simulating)
    {
        SimulateCrash();
        return;
    }
    SetState(Status::crashed, sub_state);

#ifdef ENABLE_SCRIPTING
    InvokeVehicleCrashHook(Id, "water");
#endif

    if (!curRide->flags.has(RideFlag::crashed))
    {
        auto frontVehicle = GetHead();
        auto trainIndex = ride_get_train_index_from_vehicle(*curRide, frontVehicle->Id);
        if (!trainIndex.has_value())
        {
            return;
        }

        curRide->crash(trainIndex.value());

        if (curRide->status != RideStatus::closed)
        {
            // We require this to execute right away during the simulation, always ignore network and queue.
            auto gameAction = GameActions::RideSetStatusAction(curRide->id, RideStatus::closed);
            GameActions::ExecuteNested(&gameAction, getGameState());
        }
    }
    curRide->flags.set(RideFlag::crashed);
    curRide->windowInvalidateFlags.set(RideInvalidateFlag::main, RideInvalidateFlag::list);

    if (IsHead())
    {
        KillAllPassengersInTrain();
    }

    sub_state = 2;

    const auto curLoc = GetLocation();
    Play3D(SoundId::water1, curLoc);

    CrashSplashParticle::Create(curLoc);
    CrashSplashParticle::Create(curLoc + CoordsXYZ{ -8, -9, 0 });
    CrashSplashParticle::Create(curLoc + CoordsXYZ{ 11, -9, 0 });
    CrashSplashParticle::Create(curLoc + CoordsXYZ{ 11, 8, 0 });
    CrashSplashParticle::Create(curLoc + CoordsXYZ{ -4, 8, 0 });

    for (int32_t i = 0; i < 10; ++i)
        VehicleCrashParticle::Create(colours, curLoc + CoordsXYZ{ -4, 8, 0 });

    flags.set(VehicleFlag::crashed);
    animation_frame = 0;
    animationState = 0;
    SpriteData.Width = 13;
    SpriteData.HeightMin = 45;
    SpriteData.HeightMax = 5;

    MoveTo(curLoc);

    crash_z = -1;
}

/**
 *
 *  rct2: 0x006D98CA
 */
void Vehicle::UpdateCrash()
{
    for (Vehicle* curVehicle = getGameState().entities.GetEntity<Vehicle>(Id); curVehicle != nullptr;
         curVehicle = getGameState().entities.GetEntity<Vehicle>(curVehicle->next_vehicle_on_train))
    {
        CoordsXYZ curPos = curVehicle->GetLocation();

        if (curVehicle->sub_state > 1)
        {
            if (curVehicle->crash_z <= 96)
            {
                curVehicle->crash_z++;
                if ((ScenarioRand() & 0xFFFF) <= 0x1555)
                {
                    int32_t xOffset = (ScenarioRand() & 2) - 1;
                    int32_t yOffset = (ScenarioRand() & 2) - 1;

                    ExplosionCloud::Create(curPos + CoordsXYZ{ xOffset, yOffset, 0 });
                }
            }
            if (curVehicle->animationState <= 0xe388)
            {
                curVehicle->animationState += 0x1c71;
            }
            else
            {
                curVehicle->animationState = 0;
                curVehicle->animation_frame++;
                if (curVehicle->animation_frame >= 8)
                    curVehicle->animation_frame = 0;
                curVehicle->Invalidate();
            }
            continue;
        }

        TileElement* collideElement = vehicle_check_collision(curPos);
        if (collideElement == nullptr)
        {
            curVehicle->sub_state = 1;
        }
        else if (curVehicle->sub_state == 1)
        {
            curVehicle->CrashOnLand();
            continue;
        }

        int16_t height = TileElementHeight(curPos);
        int16_t waterHeight = TileElementWaterHeight(curPos);
        int16_t zDiff;
        if (waterHeight != 0)
        {
            zDiff = curPos.z - waterHeight;
            if (zDiff <= 0 && zDiff >= -20)
            {
                curVehicle->CrashOnWater();
                continue;
            }
        }

        zDiff = curPos.z - height;
        if ((zDiff <= 0 && zDiff >= -20) || curPos.z < 16)
        {
            curVehicle->CrashOnLand();
            continue;
        }

        curVehicle->Invalidate();

        curPos.x += static_cast<int8_t>(curVehicle->crash_x >> 8);
        curPos.y += static_cast<int8_t>(curVehicle->crash_y >> 8);
        curPos.z += static_cast<int8_t>(curVehicle->crash_z >> 8);
        curVehicle->TrackLocation = { (curVehicle->crash_x << 8), (curVehicle->crash_y << 8), (curVehicle->crash_z << 8) };

        if (!MapIsLocationValid(curPos))
        {
            curVehicle->CrashOnLand();
            continue;
        }

        curVehicle->MoveTo(curPos);

        if (curVehicle->sub_state == 1)
        {
            curVehicle->crash_z -= 20;
        }
    }
}
