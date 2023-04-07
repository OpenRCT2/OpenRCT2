/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameStateSnapshots.h"

#include "core/CircularBuffer.h"
#include "entity/Balloon.h"
#include "entity/Duck.h"
#include "entity/EntityList.h"
#include "entity/EntityRegistry.h"
#include "entity/Fountain.h"
#include "entity/Guest.h"
#include "entity/Litter.h"
#include "entity/MoneyEffect.h"
#include "entity/Particle.h"
#include "entity/Staff.h"
#include "ride/Vehicle.h"

static constexpr size_t MaximumGameStateSnapshots = 32;
static constexpr uint32_t InvalidTick = 0xFFFFFFFF;

#pragma pack(push, 1)
union EntitySnapshot
{
    uint8_t Pad00[0x200];
    EntityBase base;
    EntitySnapshot()
        : Pad00()
    {
    }
};
assert_struct_size(EntitySnapshot, 0x200);
#pragma pack(pop)

struct GameStateSnapshot_t
{
    GameStateSnapshot_t& operator=(GameStateSnapshot_t&& mv) noexcept
    {
        tick = mv.tick;
        storedSprites = std::move(mv.storedSprites);
        return *this;
    }

    uint32_t tick = InvalidTick;
    uint32_t srand0 = 0;

    OpenRCT2::MemoryStream storedSprites;
    OpenRCT2::MemoryStream parkParameters;

    template<typename T> bool EntitySizeCheck(DataSerialiser& ds)
    {
        uint32_t size = sizeof(T);
        ds << size;
        if (ds.IsLoading())
        {
            return size == sizeof(T);
        }
        return true;
    }
    template<typename... T> bool EntitiesSizeCheck(DataSerialiser& ds)
    {
        return (EntitySizeCheck<T>(ds) && ...);
    }

    // Must pass a function that can access the sprite.
    void SerialiseSprites(std::function<EntitySnapshot*(const EntityId)> getEntity, const size_t numSprites, bool saving)
    {
        const bool loading = !saving;

        storedSprites.SetPosition(0);
        DataSerialiser ds(saving, storedSprites);

        std::vector<uint32_t> indexTable;
        indexTable.reserve(numSprites);

        uint32_t numSavedSprites = 0;

        if (saving)
        {
            for (EntityId::UnderlyingType i = 0; i < numSprites; i++)
            {
                auto entity = getEntity(EntityId::FromUnderlying(i));
                if (entity == nullptr || entity->base.Type == EntityType::Null)
                    continue;
                indexTable.push_back(static_cast<uint32_t>(i));
            }
            numSavedSprites = static_cast<uint32_t>(indexTable.size());
        }

        // Encodes and checks the size of each of the entity so that we
        // can fail gracefully when fields added/removed
        if (!EntitiesSizeCheck<Vehicle, Guest, Staff, Litter, MoneyEffect, Balloon, Duck, JumpingFountain, SteamParticle>(ds))
        {
            LOG_ERROR("Entity index corrupted!");
            return;
        }
        ds << numSavedSprites;

        if (loading)
        {
            indexTable.resize(numSavedSprites);
        }

        for (uint32_t i = 0; i < numSavedSprites; i++)
        {
            ds << indexTable[i];

            const EntityId spriteIdx = EntityId::FromUnderlying(indexTable[i]);
            EntitySnapshot* entity = getEntity(spriteIdx);
            if (entity == nullptr)
            {
                LOG_ERROR("Entity index corrupted!");
                return;
            }
            auto& sprite = *entity;

            ds << sprite.base.Type;

            switch (sprite.base.Type)
            {
                case EntityType::Vehicle:
                    reinterpret_cast<Vehicle&>(sprite).Serialise(ds);
                    break;
                case EntityType::Guest:
                    reinterpret_cast<Guest&>(sprite).Serialise(ds);
                    break;
                case EntityType::Staff:
                    reinterpret_cast<Staff&>(sprite).Serialise(ds);
                    break;
                case EntityType::Litter:
                    reinterpret_cast<Litter&>(sprite).Serialise(ds);
                    break;
                case EntityType::MoneyEffect:
                    reinterpret_cast<MoneyEffect&>(sprite).Serialise(ds);
                    break;
                case EntityType::Balloon:
                    reinterpret_cast<Balloon&>(sprite).Serialise(ds);
                    break;
                case EntityType::Duck:
                    reinterpret_cast<Duck&>(sprite).Serialise(ds);
                    break;
                case EntityType::JumpingFountain:
                    reinterpret_cast<JumpingFountain&>(sprite).Serialise(ds);
                    break;
                case EntityType::SteamParticle:
                    reinterpret_cast<SteamParticle&>(sprite).Serialise(ds);
                    break;
                case EntityType::Null:
                    break;
                default:
                    break;
            }
        }
    }
};

struct GameStateSnapshots final : public IGameStateSnapshots
{
    virtual void Reset() override final
    {
        _snapshots.clear();
    }

    virtual GameStateSnapshot_t& CreateSnapshot() override final
    {
        auto snapshot = std::make_unique<GameStateSnapshot_t>();
        _snapshots.push_back(std::move(snapshot));

        return *_snapshots.back();
    }

    virtual void LinkSnapshot(GameStateSnapshot_t& snapshot, uint32_t tick, uint32_t srand0) override final
    {
        snapshot.tick = tick;
        snapshot.srand0 = srand0;
    }

    virtual void Capture(GameStateSnapshot_t& snapshot) override final
    {
        snapshot.SerialiseSprites(
            [](const EntityId index) { return reinterpret_cast<EntitySnapshot*>(GetEntity(index)); }, MAX_ENTITIES, true);

        // LOG_INFO("Snapshot size: %u bytes", static_cast<uint32_t>(snapshot.storedSprites.GetLength()));
    }

    virtual const GameStateSnapshot_t* GetLinkedSnapshot(uint32_t tick) const override final
    {
        for (size_t i = 0; i < _snapshots.size(); i++)
        {
            if (_snapshots[i]->tick == tick)
                return _snapshots[i].get();
        }
        return nullptr;
    }

    virtual void SerialiseSnapshot(GameStateSnapshot_t& snapshot, DataSerialiser& ds) const override final
    {
        ds << snapshot.tick;
        ds << snapshot.srand0;
        ds << snapshot.storedSprites;
        ds << snapshot.parkParameters;
    }

    std::vector<EntitySnapshot> BuildSpriteList(GameStateSnapshot_t& snapshot) const
    {
        std::vector<EntitySnapshot> spriteList;
        spriteList.resize(MAX_ENTITIES);

        for (auto& sprite : spriteList)
        {
            // By default they don't exist.
            sprite.base.Type = EntityType::Null;
        }

        snapshot.SerialiseSprites(
            [&spriteList](const EntityId index) { return &spriteList[index.ToUnderlying()]; }, MAX_ENTITIES, false);

        return spriteList;
    }

#define COMPARE_FIELD(struc, field)                                                                                            \
    if (std::memcmp(&spriteBase.field, &spriteCmp.field, sizeof(struc::field)) != 0)                                           \
    {                                                                                                                          \
        uint64_t valA = 0;                                                                                                     \
        uint64_t valB = 0;                                                                                                     \
        std::memcpy(&valA, &spriteBase.field, sizeof(struc::field));                                                           \
        std::memcpy(&valB, &spriteCmp.field, sizeof(struc::field));                                                            \
        uintptr_t offset = reinterpret_cast<uintptr_t>(&spriteBase.field) - reinterpret_cast<uintptr_t>(&spriteBase);          \
        changeData.diffs.push_back(                                                                                            \
            GameStateSpriteChange::Diff{ static_cast<size_t>(offset), sizeof(struc::field), #struc, #field, valA, valB });     \
    }

    void CompareSpriteDataCommon(
        const EntityBase& spriteBase, const EntityBase& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(EntityBase, Type);
        COMPARE_FIELD(EntityBase, Id);
        COMPARE_FIELD(EntityBase, x);
        COMPARE_FIELD(EntityBase, y);
        COMPARE_FIELD(EntityBase, z);
        /* Only relevant for rendering, does not affect game state.
        COMPARE_FIELD(SpriteBase, sprite_width);
        COMPARE_FIELD(SpriteBase, sprite_height_negative);
        COMPARE_FIELD(SpriteBase, sprite_height_positive);
        COMPARE_FIELD(SpriteBase, sprite_left);
        COMPARE_FIELD(SpriteBase, sprite_top);
        COMPARE_FIELD(SpriteBase, sprite_right);
        COMPARE_FIELD(SpriteBase, sprite_bottom);
        */
        COMPARE_FIELD(EntityBase, SpriteData.Direction);
    }

    void CompareSpriteDataPeep(const Peep& spriteBase, const Peep& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(Peep, NextLoc.x);
        COMPARE_FIELD(Peep, NextLoc.y);
        COMPARE_FIELD(Peep, NextLoc.z);
        COMPARE_FIELD(Peep, NextFlags);
        COMPARE_FIELD(Peep, State);
        COMPARE_FIELD(Peep, SubState);
        COMPARE_FIELD(Peep, SpriteType);
        COMPARE_FIELD(Peep, TshirtColour);
        COMPARE_FIELD(Peep, TrousersColour);
        COMPARE_FIELD(Peep, DestinationX);
        COMPARE_FIELD(Peep, DestinationY);
        COMPARE_FIELD(Peep, DestinationTolerance);
        COMPARE_FIELD(Peep, Var37);
        COMPARE_FIELD(Peep, Energy);
        COMPARE_FIELD(Peep, EnergyTarget);
        COMPARE_FIELD(Peep, Mass);
        COMPARE_FIELD(Peep, WindowInvalidateFlags);
        COMPARE_FIELD(Peep, CurrentRide);
        COMPARE_FIELD(Peep, CurrentRideStation);
        COMPARE_FIELD(Peep, CurrentTrain);
        COMPARE_FIELD(Peep, TimeToSitdown);
        COMPARE_FIELD(Peep, SpecialSprite);
        COMPARE_FIELD(Peep, ActionSpriteType);
        COMPARE_FIELD(Peep, NextActionSpriteType);
        COMPARE_FIELD(Peep, ActionSpriteImageOffset);
        COMPARE_FIELD(Peep, Action);
        COMPARE_FIELD(Peep, ActionFrame);
        COMPARE_FIELD(Peep, StepProgress);
        COMPARE_FIELD(Peep, MazeLastEdge);
        COMPARE_FIELD(Peep, InteractionRideIndex);
        COMPARE_FIELD(Peep, PeepId);
        COMPARE_FIELD(Peep, PathCheckOptimisation);
        COMPARE_FIELD(Peep, PathfindGoal.x);
        COMPARE_FIELD(Peep, PathfindGoal.y);
        COMPARE_FIELD(Peep, PathfindGoal.z);
        COMPARE_FIELD(Peep, PathfindGoal.direction);
        for (std::size_t i = 0; i < spriteCmp.PathfindHistory.size(); i++)
        {
            COMPARE_FIELD(Peep, PathfindHistory[i].x);
            COMPARE_FIELD(Peep, PathfindHistory[i].y);
            COMPARE_FIELD(Peep, PathfindHistory[i].z);
            COMPARE_FIELD(Peep, PathfindHistory[i].direction);
        }
        COMPARE_FIELD(Peep, WalkingFrameNum);
        COMPARE_FIELD(Peep, PeepFlags);
    }

    void CompareSpriteDataStaff(const Staff& spriteBase, const Staff& spriteCmp, GameStateSpriteChange& changeData) const
    {
        CompareSpriteDataPeep(spriteBase, spriteCmp, changeData);

        COMPARE_FIELD(Staff, AssignedStaffType);
        COMPARE_FIELD(Staff, MechanicTimeSinceCall);
        COMPARE_FIELD(Staff, HireDate);
        COMPARE_FIELD(Staff, StaffOrders);
        COMPARE_FIELD(Staff, StaffMowingTimeout);
        COMPARE_FIELD(Staff, StaffRidesFixed);
        COMPARE_FIELD(Staff, StaffRidesInspected);
        COMPARE_FIELD(Staff, StaffLitterSwept);
        COMPARE_FIELD(Staff, StaffBinsEmptied);
    }

    void CompareSpriteDataGuest(const Guest& spriteBase, const Guest& spriteCmp, GameStateSpriteChange& changeData) const
    {
        CompareSpriteDataPeep(spriteBase, spriteCmp, changeData);

        COMPARE_FIELD(Guest, GuestNumRides);
        COMPARE_FIELD(Guest, GuestNextInQueue);
        COMPARE_FIELD(Guest, ParkEntryTime);
        COMPARE_FIELD(Guest, GuestHeadingToRideId);
        COMPARE_FIELD(Guest, GuestIsLostCountdown);
        COMPARE_FIELD(Guest, GuestTimeOnRide);
        COMPARE_FIELD(Guest, PaidToEnter);
        COMPARE_FIELD(Guest, PaidOnRides);
        COMPARE_FIELD(Guest, PaidOnFood);
        COMPARE_FIELD(Guest, PaidOnDrink);
        COMPARE_FIELD(Guest, PaidOnSouvenirs);
        COMPARE_FIELD(Guest, OutsideOfPark);
        COMPARE_FIELD(Guest, Happiness);
        COMPARE_FIELD(Guest, HappinessTarget);
        COMPARE_FIELD(Guest, Nausea);
        COMPARE_FIELD(Guest, NauseaTarget);
        COMPARE_FIELD(Guest, Hunger);
        COMPARE_FIELD(Guest, Thirst);
        COMPARE_FIELD(Guest, Toilet);
        COMPARE_FIELD(Guest, TimeToConsume);
        COMPARE_FIELD(Guest, Intensity);
        COMPARE_FIELD(Guest, NauseaTolerance);
        COMPARE_FIELD(Guest, TimeInQueue);
        COMPARE_FIELD(Guest, CashInPocket);
        COMPARE_FIELD(Guest, CashSpent);
        COMPARE_FIELD(Guest, Photo1RideRef);
        COMPARE_FIELD(Guest, Photo2RideRef);
        COMPARE_FIELD(Guest, Photo3RideRef);
        COMPARE_FIELD(Guest, Photo4RideRef);
        COMPARE_FIELD(Guest, RejoinQueueTimeout);
        COMPARE_FIELD(Guest, PreviousRide);
        COMPARE_FIELD(Guest, PreviousRideTimeOut);
        for (std::size_t i = 0; i < PEEP_MAX_THOUGHTS; i++)
        {
            COMPARE_FIELD(Guest, Thoughts[i].type);
            COMPARE_FIELD(Guest, Thoughts[i].item);
            COMPARE_FIELD(Guest, Thoughts[i].freshness);
            COMPARE_FIELD(Guest, Thoughts[i].fresh_timeout);
        }
        COMPARE_FIELD(Guest, LitterCount);
        COMPARE_FIELD(Guest, DisgustingCount);
        COMPARE_FIELD(Guest, AmountOfFood);
        COMPARE_FIELD(Guest, AmountOfDrinks);
        COMPARE_FIELD(Guest, AmountOfSouvenirs);
        COMPARE_FIELD(Guest, VandalismSeen);
        COMPARE_FIELD(Guest, VoucherType);
        COMPARE_FIELD(Guest, VoucherRideId);
        COMPARE_FIELD(Guest, SurroundingsThoughtTimeout);
        COMPARE_FIELD(Guest, Angriness);
        COMPARE_FIELD(Guest, TimeLost);
        COMPARE_FIELD(Guest, DaysInQueue);
        COMPARE_FIELD(Guest, BalloonColour);
        COMPARE_FIELD(Guest, UmbrellaColour);
        COMPARE_FIELD(Guest, HatColour);
        COMPARE_FIELD(Guest, FavouriteRide);
        COMPARE_FIELD(Guest, FavouriteRideRating);
        COMPARE_FIELD(Guest, ItemFlags);
    }

    void CompareSpriteDataVehicle(const Vehicle& spriteBase, const Vehicle& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(Vehicle, SubType);
        COMPARE_FIELD(Vehicle, Pitch);
        COMPARE_FIELD(Vehicle, bank_rotation);
        COMPARE_FIELD(Vehicle, remaining_distance);
        COMPARE_FIELD(Vehicle, velocity);
        COMPARE_FIELD(Vehicle, acceleration);
        COMPARE_FIELD(Vehicle, ride);
        COMPARE_FIELD(Vehicle, vehicle_type);
        COMPARE_FIELD(Vehicle, colours);
        COMPARE_FIELD(Vehicle, track_progress);
        COMPARE_FIELD(Vehicle, TrackTypeAndDirection);
        COMPARE_FIELD(Vehicle, TrackLocation.x);
        COMPARE_FIELD(Vehicle, TrackLocation.y);
        COMPARE_FIELD(Vehicle, TrackLocation.z);
        COMPARE_FIELD(Vehicle, next_vehicle_on_train);
        COMPARE_FIELD(Vehicle, prev_vehicle_on_ride);
        COMPARE_FIELD(Vehicle, next_vehicle_on_ride);
        COMPARE_FIELD(Vehicle, var_44);
        COMPARE_FIELD(Vehicle, mass);
        COMPARE_FIELD(Vehicle, Flags);
        COMPARE_FIELD(Vehicle, SwingSprite);
        COMPARE_FIELD(Vehicle, current_station);
        COMPARE_FIELD(Vehicle, SwingPosition);
        COMPARE_FIELD(Vehicle, SwingSpeed);
        COMPARE_FIELD(Vehicle, status);
        COMPARE_FIELD(Vehicle, sub_state);
        for (std::size_t i = 0; i < sizeof(Vehicle::peep) / sizeof(*Vehicle::peep); i++)
        {
            COMPARE_FIELD(Vehicle, peep[i]);
        }
        for (std::size_t i = 0; i < sizeof(Vehicle::peep_tshirt_colours) / sizeof(*Vehicle::peep_tshirt_colours); i++)
        {
            COMPARE_FIELD(Vehicle, peep_tshirt_colours[i]);
        }
        COMPARE_FIELD(Vehicle, num_seats);
        COMPARE_FIELD(Vehicle, num_peeps);
        COMPARE_FIELD(Vehicle, next_free_seat);
        COMPARE_FIELD(Vehicle, restraints_position);
        COMPARE_FIELD(Vehicle, spin_speed);
        COMPARE_FIELD(Vehicle, sound2_flags);
        COMPARE_FIELD(Vehicle, spin_sprite);
        COMPARE_FIELD(Vehicle, sound1_id);
        COMPARE_FIELD(Vehicle, sound1_volume);
        COMPARE_FIELD(Vehicle, sound2_id);
        COMPARE_FIELD(Vehicle, sound2_volume);
        COMPARE_FIELD(Vehicle, sound_vector_factor);
        COMPARE_FIELD(Vehicle, cable_lift_target);
        COMPARE_FIELD(Vehicle, speed);
        COMPARE_FIELD(Vehicle, powered_acceleration);
        COMPARE_FIELD(Vehicle, CollisionDetectionTimer);
        COMPARE_FIELD(Vehicle, animation_frame);
        for (std::size_t i = 0; i < sizeof(Vehicle::PadC6) / sizeof(*Vehicle::PadC6); i++)
        {
            COMPARE_FIELD(Vehicle, PadC6[i]);
        }
        COMPARE_FIELD(Vehicle, animationState);
        COMPARE_FIELD(Vehicle, scream_sound_id);
        COMPARE_FIELD(Vehicle, TrackSubposition);
        COMPARE_FIELD(Vehicle, NumLaps);
        COMPARE_FIELD(Vehicle, brake_speed);
        COMPARE_FIELD(Vehicle, lost_time_out);
        COMPARE_FIELD(Vehicle, vertical_drop_countdown);
        COMPARE_FIELD(Vehicle, var_D3);
        COMPARE_FIELD(Vehicle, mini_golf_current_animation);
        COMPARE_FIELD(Vehicle, mini_golf_flags);
        COMPARE_FIELD(Vehicle, ride_subtype);
        COMPARE_FIELD(Vehicle, seat_rotation);
        COMPARE_FIELD(Vehicle, target_seat_rotation);
        COMPARE_FIELD(Vehicle, BoatLocation.x);
        COMPARE_FIELD(Vehicle, BoatLocation.y);
    }

    void CompareSpriteDataLitter(const Litter& spriteBase, const Litter& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(Litter, creationTick);
    }

    void CompareSpriteDataMoneyEffect(
        const MoneyEffect& spriteBase, const MoneyEffect& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(MoneyEffect, frame);
        COMPARE_FIELD(MoneyEffect, MoveDelay);
        COMPARE_FIELD(MoneyEffect, NumMovements);
        COMPARE_FIELD(MoneyEffect, GuestPurchase);
        COMPARE_FIELD(MoneyEffect, Value);
        COMPARE_FIELD(MoneyEffect, OffsetX);
        COMPARE_FIELD(MoneyEffect, Wiggle);
    }

    void CompareSpriteDataSteamParticle(
        const SteamParticle& spriteBase, const SteamParticle& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(SteamParticle, frame);
        COMPARE_FIELD(SteamParticle, time_to_move);
    }

    void CompareSpriteDataVehicleCrashParticle(
        const VehicleCrashParticle& spriteBase, const VehicleCrashParticle& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(VehicleCrashParticle, frame);
        COMPARE_FIELD(VehicleCrashParticle, time_to_live);
        for (std::size_t i = 0; i < sizeof(VehicleCrashParticle::colour) / sizeof(*VehicleCrashParticle::colour); i++)
        {
            COMPARE_FIELD(VehicleCrashParticle, colour[i]);
        }
        COMPARE_FIELD(VehicleCrashParticle, crashed_sprite_base);
        COMPARE_FIELD(VehicleCrashParticle, velocity_x);
        COMPARE_FIELD(VehicleCrashParticle, velocity_y);
        COMPARE_FIELD(VehicleCrashParticle, velocity_z);
        COMPARE_FIELD(VehicleCrashParticle, acceleration_x);
        COMPARE_FIELD(VehicleCrashParticle, acceleration_y);
        COMPARE_FIELD(VehicleCrashParticle, acceleration_z);
    }

    void CompareSpriteDataDuck(const Duck& spriteBase, const Duck& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(Duck, frame);
        COMPARE_FIELD(Duck, target_x);
        COMPARE_FIELD(Duck, target_y);
        COMPARE_FIELD(Duck, state);
    }

    void CompareSpriteDataBalloon(const Balloon& spriteBase, const Balloon& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(Balloon, frame);
        COMPARE_FIELD(Balloon, popped);
        COMPARE_FIELD(Balloon, time_to_move);
        COMPARE_FIELD(Balloon, colour);
    }

    void CompareSpriteDataJumpingFountain(
        const JumpingFountain& spriteBase, const JumpingFountain& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(JumpingFountain, frame);
        COMPARE_FIELD(JumpingFountain, FountainType);
        COMPARE_FIELD(JumpingFountain, NumTicksAlive);
        COMPARE_FIELD(JumpingFountain, FountainFlags);
        COMPARE_FIELD(JumpingFountain, TargetX);
        COMPARE_FIELD(JumpingFountain, TargetY);
        COMPARE_FIELD(JumpingFountain, Iteration);
    }

    void CompareSpriteDataExplosionCloud(
        const ExplosionCloud& spriteBase, const ExplosionCloud& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(ExplosionCloud, frame);
    }

    void CompareSpriteDataCrashSplash(
        const CrashSplashParticle& spriteBase, const CrashSplashParticle& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(CrashSplashParticle, frame);
    }

    void CompareSpriteDataExplosionFlare(
        const ExplosionFlare& spriteBase, const ExplosionFlare& spriteCmp, GameStateSpriteChange& changeData) const
    {
        COMPARE_FIELD(ExplosionFlare, frame);
    }

    void CompareSpriteData(
        const EntitySnapshot& spriteBase, const EntitySnapshot& spriteCmp, GameStateSpriteChange& changeData) const
    {
        CompareSpriteDataCommon(spriteBase.base, spriteCmp.base, changeData);
        if (spriteBase.base.Type == spriteCmp.base.Type)
        {
            switch (spriteBase.base.Type)
            {
                case EntityType::Guest:
                    CompareSpriteDataGuest(
                        static_cast<const Guest&>(spriteBase.base), static_cast<const Guest&>(spriteCmp.base), changeData);
                    break;
                case EntityType::Staff:
                    CompareSpriteDataStaff(
                        static_cast<const Staff&>(spriteBase.base), static_cast<const Staff&>(spriteCmp.base), changeData);
                    break;
                case EntityType::Vehicle:
                    CompareSpriteDataVehicle(
                        static_cast<const Vehicle&>(spriteBase.base), static_cast<const Vehicle&>(spriteCmp.base), changeData);
                    break;
                case EntityType::Litter:
                    CompareSpriteDataLitter(
                        static_cast<const Litter&>(spriteBase.base), static_cast<const Litter&>(spriteCmp.base), changeData);
                    break;
                case EntityType::SteamParticle:
                    CompareSpriteDataSteamParticle(
                        static_cast<const SteamParticle&>(spriteBase.base), static_cast<const SteamParticle&>(spriteCmp.base),
                        changeData);
                    break;
                case EntityType::MoneyEffect:
                    CompareSpriteDataMoneyEffect(
                        static_cast<const MoneyEffect&>(spriteBase.base), static_cast<const MoneyEffect&>(spriteCmp.base),
                        changeData);
                    break;
                case EntityType::CrashedVehicleParticle:
                    CompareSpriteDataVehicleCrashParticle(
                        static_cast<const VehicleCrashParticle&>(spriteBase.base),
                        static_cast<const VehicleCrashParticle&>(spriteCmp.base), changeData);
                    break;
                case EntityType::ExplosionCloud:
                    CompareSpriteDataExplosionCloud(
                        static_cast<const ExplosionCloud&>(spriteBase.base), static_cast<const ExplosionCloud&>(spriteCmp.base),
                        changeData);
                    break;
                case EntityType::CrashSplash:
                    CompareSpriteDataCrashSplash(
                        static_cast<const CrashSplashParticle&>(spriteBase.base),
                        static_cast<const CrashSplashParticle&>(spriteCmp.base), changeData);
                    break;
                case EntityType::ExplosionFlare:
                    CompareSpriteDataExplosionFlare(
                        static_cast<const ExplosionFlare&>(spriteBase.base), static_cast<const ExplosionFlare&>(spriteCmp.base),
                        changeData);
                    break;
                case EntityType::JumpingFountain:
                    CompareSpriteDataJumpingFountain(
                        static_cast<const JumpingFountain&>(spriteBase.base),
                        static_cast<const JumpingFountain&>(spriteCmp.base), changeData);
                    break;
                case EntityType::Balloon:
                    CompareSpriteDataBalloon(
                        static_cast<const Balloon&>(spriteBase.base), static_cast<const Balloon&>(spriteCmp.base), changeData);
                    break;
                case EntityType::Duck:
                    CompareSpriteDataDuck(
                        static_cast<const Duck&>(spriteBase.base), static_cast<const Duck&>(spriteCmp.base), changeData);
                    break;
                case EntityType::Null:
                    break;
                default:
                    break;
            }
        }
    }

    virtual GameStateCompareData Compare(const GameStateSnapshot_t& base, const GameStateSnapshot_t& cmp) const override final
    {
        GameStateCompareData res;
        res.tickLeft = base.tick;
        res.tickRight = cmp.tick;
        res.srand0Left = base.srand0;
        res.srand0Right = cmp.srand0;

        std::vector<EntitySnapshot> spritesBase = BuildSpriteList(const_cast<GameStateSnapshot_t&>(base));
        std::vector<EntitySnapshot> spritesCmp = BuildSpriteList(const_cast<GameStateSnapshot_t&>(cmp));

        for (uint32_t i = 0; i < static_cast<uint32_t>(spritesBase.size()); i++)
        {
            GameStateSpriteChange changeData;
            changeData.spriteIndex = i;

            const EntitySnapshot& spriteBase = spritesBase[i];
            const EntitySnapshot& spriteCmp = spritesCmp[i];

            changeData.entityType = spriteBase.base.Type;

            if (spriteBase.base.Type == EntityType::Null && spriteCmp.base.Type != EntityType::Null)
            {
                // Sprite was added.
                changeData.changeType = GameStateSpriteChange::ADDED;
                changeData.entityType = spriteCmp.base.Type;
            }
            else if (spriteBase.base.Type != EntityType::Null && spriteCmp.base.Type == EntityType::Null)
            {
                // Sprite was removed.
                changeData.changeType = GameStateSpriteChange::REMOVED;
                changeData.entityType = spriteBase.base.Type;
            }
            else if (spriteBase.base.Type == EntityType::Null && spriteCmp.base.Type == EntityType::Null)
            {
                // Do nothing.
                changeData.changeType = GameStateSpriteChange::EQUAL;
            }
            else
            {
                CompareSpriteData(spriteBase, spriteCmp, changeData);
                if (changeData.diffs.size() == 0)
                {
                    changeData.changeType = GameStateSpriteChange::EQUAL;
                }
                else
                {
                    changeData.changeType = GameStateSpriteChange::MODIFIED;
                }
            }

            res.spriteChanges.push_back(std::move(changeData));
        }

        return res;
    }

    static const char* GetEntityTypeName(EntityType type)
    {
        switch (type)
        {
            case EntityType::Null:
                return "Null";
            case EntityType::Guest:
                return "Guest";
            case EntityType::Staff:
                return "Staff";
            case EntityType::Vehicle:
                return "Vehicle";
            case EntityType::Litter:
                return "Litter";
            case EntityType::SteamParticle:
                return "Misc: Steam Particle";
            case EntityType::MoneyEffect:
                return "Misc: Money effect";
            case EntityType::CrashedVehicleParticle:
                return "Misc: Crash Vehicle Particle";
            case EntityType::ExplosionCloud:
                return "Misc: Explosion Cloud";
            case EntityType::CrashSplash:
                return "Misc: Crash Splash";
            case EntityType::ExplosionFlare:
                return "Misc: Explosion Flare";
            case EntityType::JumpingFountain:
                return "Misc: Jumping fountain";
            case EntityType::Balloon:
                return "Misc: Balloon";
            case EntityType::Duck:
                return "Misc: Duck";
            default:
                break;
        }
        return "Unknown";
    }

    virtual std::string GetCompareDataText(const GameStateCompareData& cmpData) const override
    {
        std::string outputBuffer;
        char tempBuffer[1024] = {};

        if (cmpData.tickLeft != cmpData.tickRight)
        {
            outputBuffer += "WARNING: Comparing two snapshots with different ticks, this will very likely result in false "
                            "positives\n";
        }

        snprintf(tempBuffer, sizeof(tempBuffer), "tick left = %08X, tick right = %08X\n", cmpData.tickLeft, cmpData.tickRight);
        outputBuffer += tempBuffer;

        snprintf(
            tempBuffer, sizeof(tempBuffer), "srand0 left = %08X, srand0 right = %08X\n", cmpData.srand0Left,
            cmpData.srand0Right);
        outputBuffer += tempBuffer;

        for (auto& change : cmpData.spriteChanges)
        {
            if (change.changeType == GameStateSpriteChange::EQUAL)
                continue;

            const char* typeName = GetEntityTypeName(change.entityType);

            if (change.changeType == GameStateSpriteChange::ADDED)
            {
                snprintf(tempBuffer, sizeof(tempBuffer), "Sprite added (%s), index: %u\n", typeName, change.spriteIndex);
                outputBuffer += tempBuffer;
            }
            else if (change.changeType == GameStateSpriteChange::REMOVED)
            {
                snprintf(tempBuffer, sizeof(tempBuffer), "Sprite removed (%s), index: %u\n", typeName, change.spriteIndex);
                outputBuffer += tempBuffer;
            }
            else if (change.changeType == GameStateSpriteChange::MODIFIED)
            {
                snprintf(
                    tempBuffer, sizeof(tempBuffer), "Sprite modifications (%s), index: %u\n", typeName, change.spriteIndex);
                outputBuffer += tempBuffer;
                for (auto& diff : change.diffs)
                {
                    snprintf(
                        tempBuffer, sizeof(tempBuffer),
                        "  %s::%s, len = %u, offset = %u, left = 0x%.16llX, right = 0x%.16llX\n", diff.structname,
                        diff.fieldname, static_cast<uint32_t>(diff.length), static_cast<uint32_t>(diff.offset),
                        static_cast<unsigned long long>(diff.valueA), static_cast<unsigned long long>(diff.valueB));
                    outputBuffer += tempBuffer;
                }
            }
        }
        return outputBuffer;
    }

    virtual bool LogCompareDataToFile(const std::string& fileName, const GameStateCompareData& cmpData) const override
    {
        auto outputBuffer = GetCompareDataText(cmpData);

        FILE* fp = fopen(fileName.c_str(), "wt");
        if (fp == nullptr)
            return false;

        fputs(outputBuffer.c_str(), fp);
        fclose(fp);

        return true;
    }

private:
    CircularBuffer<std::unique_ptr<GameStateSnapshot_t>, MaximumGameStateSnapshots> _snapshots;
};

std::unique_ptr<IGameStateSnapshots> CreateGameStateSnapshots()
{
    return std::make_unique<GameStateSnapshots>();
}
