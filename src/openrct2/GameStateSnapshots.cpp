/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameStateSnapshots.h"

#include "core/CircularBuffer.h"
#include "peep/Peep.h"
#include "world/EntityList.h"
#include "world/Sprite.h"

static constexpr size_t MaximumGameStateSnapshots = 32;
static constexpr uint32_t InvalidTick = 0xFFFFFFFF;

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

    // Must pass a function that can access the sprite.
    void SerialiseSprites(std::function<rct_sprite*(const size_t)> getEntity, const size_t numSprites, bool saving)
    {
        const bool loading = !saving;

        storedSprites.SetPosition(0);
        DataSerialiser ds(saving, storedSprites);

        std::vector<uint32_t> indexTable;
        indexTable.reserve(numSprites);

        uint32_t numSavedSprites = 0;

        if (saving)
        {
            for (size_t i = 0; i < numSprites; i++)
            {
                auto entity = getEntity(i);
                if (entity == nullptr || entity->misc.sprite_identifier == SpriteIdentifier::Null)
                    continue;
                indexTable.push_back(static_cast<uint32_t>(i));
            }
            numSavedSprites = static_cast<uint32_t>(indexTable.size());
        }

        ds << numSavedSprites;

        if (loading)
        {
            indexTable.resize(numSavedSprites);
        }

        for (uint32_t i = 0; i < numSavedSprites; i++)
        {
            ds << indexTable[i];

            const uint32_t spriteIdx = indexTable[i];
            rct_sprite* entity = getEntity(spriteIdx);
            if (entity == nullptr)
            {
                log_error("Entity index corrupted!");
                return;
            }
            auto& sprite = *entity;

            ds << sprite.misc.sprite_identifier;

            switch (sprite.misc.sprite_identifier)
            {
                case SpriteIdentifier::Vehicle:
                    ds << reinterpret_cast<uint8_t(&)[sizeof(Vehicle)]>(sprite.vehicle);
                    break;
                case SpriteIdentifier::Peep:
                    ds << reinterpret_cast<uint8_t(&)[sizeof(Peep)]>(sprite.peep);
                    break;
                case SpriteIdentifier::Litter:
                    ds << reinterpret_cast<uint8_t(&)[sizeof(Litter)]>(sprite.litter);
                    break;
                case SpriteIdentifier::Misc:
                {
                    ds << sprite.misc.SubType;
                    switch (sprite.misc.SubType)
                    {
                        case MiscEntityType::MoneyEffect:
                            ds << reinterpret_cast<uint8_t(&)[sizeof(MoneyEffect)]>(sprite.money_effect);
                            break;
                        case MiscEntityType::Balloon:
                            ds << reinterpret_cast<uint8_t(&)[sizeof(Balloon)]>(sprite.balloon);
                            break;
                        case MiscEntityType::Duck:
                            ds << reinterpret_cast<uint8_t(&)[sizeof(Duck)]>(sprite.duck);
                            break;
                        case MiscEntityType::JumpingFountainWater:
                            ds << reinterpret_cast<uint8_t(&)[sizeof(JumpingFountain)]>(sprite.jumping_fountain);
                            break;
                        case MiscEntityType::SteamParticle:
                            ds << reinterpret_cast<uint8_t(&)[sizeof(SteamParticle)]>(sprite.steam_particle);
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case SpriteIdentifier::Null:
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
            [](const size_t index) { return reinterpret_cast<rct_sprite*>(GetEntity(index)); }, MAX_ENTITIES, true);

        // log_info("Snapshot size: %u bytes", static_cast<uint32_t>(snapshot.storedSprites.GetLength()));
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

    std::vector<rct_sprite> BuildSpriteList(GameStateSnapshot_t& snapshot) const
    {
        std::vector<rct_sprite> spriteList;
        spriteList.resize(MAX_ENTITIES);

        for (auto& sprite : spriteList)
        {
            // By default they don't exist.
            sprite.misc.sprite_identifier = SpriteIdentifier::Null;
        }

        snapshot.SerialiseSprites([&spriteList](const size_t index) { return &spriteList[index]; }, MAX_ENTITIES, false);

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
            GameStateSpriteChange_t::Diff_t{ static_cast<size_t>(offset), sizeof(struc::field), #struc, #field, valA, valB }); \
    }

    void CompareSpriteDataCommon(
        const SpriteBase& spriteBase, const SpriteBase& spriteCmp, GameStateSpriteChange_t& changeData) const
    {
        COMPARE_FIELD(SpriteBase, sprite_identifier);
        COMPARE_FIELD(SpriteBase, linked_list_index);
        COMPARE_FIELD(SpriteBase, sprite_index);
        COMPARE_FIELD(SpriteBase, flags);
        COMPARE_FIELD(SpriteBase, x);
        COMPARE_FIELD(SpriteBase, y);
        COMPARE_FIELD(SpriteBase, z);
        /* Only relevant for rendering, does not affect game state.
        COMPARE_FIELD(SpriteBase, sprite_width);
        COMPARE_FIELD(SpriteBase, sprite_height_negative);
        COMPARE_FIELD(SpriteBase, sprite_height_positive);
        COMPARE_FIELD(SpriteBase, sprite_left);
        COMPARE_FIELD(SpriteBase, sprite_top);
        COMPARE_FIELD(SpriteBase, sprite_right);
        COMPARE_FIELD(SpriteBase, sprite_bottom);
        */
        COMPARE_FIELD(SpriteBase, sprite_direction);
    }

    void CompareSpriteDataPeep(const Peep& spriteBase, const Peep& spriteCmp, GameStateSpriteChange_t& changeData) const
    {
        COMPARE_FIELD(Peep, NextLoc.x);
        COMPARE_FIELD(Peep, NextLoc.y);
        COMPARE_FIELD(Peep, NextLoc.z);
        COMPARE_FIELD(Peep, NextFlags);
        COMPARE_FIELD(Peep, OutsideOfPark);
        COMPARE_FIELD(Peep, State);
        COMPARE_FIELD(Peep, SubState);
        COMPARE_FIELD(Peep, SpriteType);
        COMPARE_FIELD(Peep, AssignedPeepType);
        COMPARE_FIELD(Peep, GuestNumRides);
        COMPARE_FIELD(Peep, TshirtColour);
        COMPARE_FIELD(Peep, TrousersColour);
        COMPARE_FIELD(Peep, DestinationX);
        COMPARE_FIELD(Peep, DestinationY);
        COMPARE_FIELD(Peep, DestinationTolerance);
        COMPARE_FIELD(Peep, Var37);
        COMPARE_FIELD(Peep, Energy);
        COMPARE_FIELD(Peep, EnergyTarget);
        COMPARE_FIELD(Peep, Happiness);
        COMPARE_FIELD(Peep, HappinessTarget);
        COMPARE_FIELD(Peep, Nausea);
        COMPARE_FIELD(Peep, NauseaTarget);
        COMPARE_FIELD(Peep, Hunger);
        COMPARE_FIELD(Peep, Thirst);
        COMPARE_FIELD(Peep, Toilet);
        COMPARE_FIELD(Peep, Mass);
        COMPARE_FIELD(Peep, TimeToConsume);
        COMPARE_FIELD(Peep, Intensity);
        COMPARE_FIELD(Peep, NauseaTolerance);
        COMPARE_FIELD(Peep, WindowInvalidateFlags);
        COMPARE_FIELD(Peep, PaidOnDrink);
        for (int i = 0; i < 16; i++)
        {
            COMPARE_FIELD(Peep, RideTypesBeenOn[i]);
        }
        COMPARE_FIELD(Peep, ItemFlags);
        COMPARE_FIELD(Peep, Photo2RideRef);
        COMPARE_FIELD(Peep, Photo3RideRef);
        COMPARE_FIELD(Peep, Photo4RideRef);
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
        COMPARE_FIELD(Peep, GuestNextInQueue);
        COMPARE_FIELD(Peep, MazeLastEdge);
        COMPARE_FIELD(Peep, InteractionRideIndex);
        COMPARE_FIELD(Peep, TimeInQueue);
        for (int i = 0; i < 32; i++)
        {
            COMPARE_FIELD(Peep, RidesBeenOn[i]);
        }
        COMPARE_FIELD(Peep, Id);
        COMPARE_FIELD(Peep, CashInPocket);
        COMPARE_FIELD(Peep, CashSpent);
        COMPARE_FIELD(Peep, ParkEntryTime);
        COMPARE_FIELD(Peep, RejoinQueueTimeout);
        COMPARE_FIELD(Peep, PreviousRide);
        COMPARE_FIELD(Peep, PreviousRideTimeOut);
        for (int i = 0; i < PEEP_MAX_THOUGHTS; i++)
        {
            COMPARE_FIELD(Peep, Thoughts[i]);
        }
        COMPARE_FIELD(Peep, PathCheckOptimisation);
        COMPARE_FIELD(Peep, GuestHeadingToRideId);
        COMPARE_FIELD(Peep, StaffOrders);
        COMPARE_FIELD(Peep, Photo1RideRef);
        COMPARE_FIELD(Peep, PeepFlags);
        COMPARE_FIELD(Peep, PathfindGoal);
        for (int i = 0; i < 4; i++)
        {
            COMPARE_FIELD(Peep, PathfindHistory[i]);
        }
        COMPARE_FIELD(Peep, WalkingFrameNum);
        COMPARE_FIELD(Peep, LitterCount);
        COMPARE_FIELD(Peep, GuestTimeOnRide);
        COMPARE_FIELD(Peep, DisgustingCount);
        COMPARE_FIELD(Peep, PaidToEnter);
        COMPARE_FIELD(Peep, PaidOnRides);
        COMPARE_FIELD(Peep, PaidOnFood);
        COMPARE_FIELD(Peep, PaidOnSouvenirs);
        COMPARE_FIELD(Peep, AmountOfFood);
        COMPARE_FIELD(Peep, AmountOfDrinks);
        COMPARE_FIELD(Peep, AmountOfSouvenirs);
        COMPARE_FIELD(Peep, VandalismSeen);
        COMPARE_FIELD(Peep, VoucherType);
        COMPARE_FIELD(Peep, VoucherRideId);
        COMPARE_FIELD(Peep, SurroundingsThoughtTimeout);
        COMPARE_FIELD(Peep, Angriness);
        COMPARE_FIELD(Peep, TimeLost);
        COMPARE_FIELD(Peep, DaysInQueue);
        COMPARE_FIELD(Peep, BalloonColour);
        COMPARE_FIELD(Peep, UmbrellaColour);
        COMPARE_FIELD(Peep, HatColour);
        COMPARE_FIELD(Peep, FavouriteRide);
        COMPARE_FIELD(Peep, FavouriteRideRating);
    }

    void CompareSpriteDataVehicle(
        const Vehicle& spriteBase, const Vehicle& spriteCmp, GameStateSpriteChange_t& changeData) const
    {
        COMPARE_FIELD(Vehicle, vehicle_sprite_type);
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
        COMPARE_FIELD(Vehicle, update_flags);
        COMPARE_FIELD(Vehicle, SwingSprite);
        COMPARE_FIELD(Vehicle, current_station);
        COMPARE_FIELD(Vehicle, SwingPosition);
        COMPARE_FIELD(Vehicle, SwingSpeed);
        COMPARE_FIELD(Vehicle, status);
        COMPARE_FIELD(Vehicle, sub_state);
        for (int i = 0; i < 32; i++)
        {
            COMPARE_FIELD(Vehicle, peep[i]);
        }
        for (int i = 0; i < 32; i++)
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
        COMPARE_FIELD(Vehicle, var_C4);
        COMPARE_FIELD(Vehicle, animation_frame);
        for (int i = 0; i < 2; i++)
        {
            COMPARE_FIELD(Vehicle, pad_C6[i]);
        }
        COMPARE_FIELD(Vehicle, var_C8);
        COMPARE_FIELD(Vehicle, var_CA);
        COMPARE_FIELD(Vehicle, scream_sound_id);
        COMPARE_FIELD(Vehicle, TrackSubposition);
        COMPARE_FIELD(Vehicle, num_laps);
        COMPARE_FIELD(Vehicle, brake_speed);
        COMPARE_FIELD(Vehicle, lost_time_out);
        COMPARE_FIELD(Vehicle, vertical_drop_countdown);
        COMPARE_FIELD(Vehicle, var_D3);
        COMPARE_FIELD(Vehicle, mini_golf_current_animation);
        COMPARE_FIELD(Vehicle, mini_golf_flags);
        COMPARE_FIELD(Vehicle, ride_subtype);
        COMPARE_FIELD(Vehicle, colours_extended);
        COMPARE_FIELD(Vehicle, seat_rotation);
        COMPARE_FIELD(Vehicle, target_seat_rotation);
        COMPARE_FIELD(Vehicle, BoatLocation.x);
        COMPARE_FIELD(Vehicle, BoatLocation.y);
    }

    void CompareSpriteDataLitter(const Litter& spriteBase, const Litter& spriteCmp, GameStateSpriteChange_t& changeData) const
    {
        COMPARE_FIELD(Litter, creationTick);
    }

    void CompareSpriteDataMoneyEffect(
        const MoneyEffect& spriteBase, const MoneyEffect& spriteCmp, GameStateSpriteChange_t& changeData) const
    {
        COMPARE_FIELD(MoneyEffect, MoveDelay);
        COMPARE_FIELD(MoneyEffect, NumMovements);
        COMPARE_FIELD(MoneyEffect, Vertical);
        COMPARE_FIELD(MoneyEffect, Value);
        COMPARE_FIELD(MoneyEffect, OffsetX);
        COMPARE_FIELD(MoneyEffect, Wiggle);
    }

    void CompareSpriteDataSteamParticle(
        const SteamParticle& spriteBase, const SteamParticle& spriteCmp, GameStateSpriteChange_t& changeData) const
    {
        COMPARE_FIELD(SteamParticle, time_to_move);
    }

    void CompareSpriteDataVehicleCrashParticle(
        const VehicleCrashParticle& spriteBase, const VehicleCrashParticle& spriteCmp,
        GameStateSpriteChange_t& changeData) const
    {
        COMPARE_FIELD(VehicleCrashParticle, time_to_live);
        for (int i = 0; i < 2; i++)
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

    void CompareSpriteDataDuck(const Duck& spriteBase, const Duck& spriteCmp, GameStateSpriteChange_t& changeData) const
    {
        COMPARE_FIELD(Duck, target_x);
        COMPARE_FIELD(Duck, target_y);
        COMPARE_FIELD(Duck, state);
    }

    void CompareSpriteDataBalloon(
        const Balloon& spriteBase, const Balloon& spriteCmp, GameStateSpriteChange_t& changeData) const
    {
        COMPARE_FIELD(Balloon, popped);
        COMPARE_FIELD(Balloon, time_to_move);
        COMPARE_FIELD(Balloon, colour);
    }

    void CompareSpriteDataJumpingFountain(
        const JumpingFountain& spriteBase, const JumpingFountain& spriteCmp, GameStateSpriteChange_t& changeData) const
    {
        COMPARE_FIELD(JumpingFountain, NumTicksAlive);
        COMPARE_FIELD(JumpingFountain, FountainFlags);
        COMPARE_FIELD(JumpingFountain, TargetX);
        COMPARE_FIELD(JumpingFountain, TargetY);
        COMPARE_FIELD(JumpingFountain, Iteration);
    }

    void CompareSpriteDataMisc(
        const MiscEntity& spriteBase, const MiscEntity& spriteCmp, GameStateSpriteChange_t& changeData) const
    {
        COMPARE_FIELD(MiscEntity, SubType);
        COMPARE_FIELD(MiscEntity, frame);
    }

    void CompareSpriteData(const rct_sprite& spriteBase, const rct_sprite& spriteCmp, GameStateSpriteChange_t& changeData) const
    {
        CompareSpriteDataCommon(spriteBase.misc, spriteCmp.misc, changeData);
        if (spriteBase.misc.sprite_identifier == spriteCmp.misc.sprite_identifier)
        {
            switch (spriteBase.misc.sprite_identifier)
            {
                case SpriteIdentifier::Peep:
                    CompareSpriteDataPeep(spriteBase.peep, spriteCmp.peep, changeData);
                    break;
                case SpriteIdentifier::Vehicle:
                    CompareSpriteDataVehicle(spriteBase.vehicle, spriteCmp.vehicle, changeData);
                    break;
                case SpriteIdentifier::Litter:
                    CompareSpriteDataLitter(spriteBase.litter, spriteCmp.litter, changeData);
                    break;
                case SpriteIdentifier::Misc:
                    // This is not expected to happen, as misc sprites do not constitute sprite checksum
                    CompareSpriteDataMisc(spriteBase.misc, spriteCmp.misc, changeData);
                    switch (spriteBase.misc.SubType)
                    {
                        case MiscEntityType::SteamParticle:
                            CompareSpriteDataSteamParticle(spriteBase.steam_particle, spriteCmp.steam_particle, changeData);
                            break;
                        case MiscEntityType::MoneyEffect:
                            CompareSpriteDataMoneyEffect(spriteBase.money_effect, spriteCmp.money_effect, changeData);
                            break;
                        case MiscEntityType::CrashedVehicleParticle:
                            CompareSpriteDataVehicleCrashParticle(
                                spriteBase.crashed_vehicle_particle, spriteCmp.crashed_vehicle_particle, changeData);
                            break;
                        case MiscEntityType::ExplosionCloud:
                        case MiscEntityType::CrashSplash:
                        case MiscEntityType::ExplosionFlare:
                            // MiscEntity
                            break;
                        case MiscEntityType::JumpingFountainWater:
                        case MiscEntityType::JumpingFountainSnow:
                            CompareSpriteDataJumpingFountain(
                                spriteBase.jumping_fountain, spriteCmp.jumping_fountain, changeData);
                            break;
                        case MiscEntityType::Balloon:
                            CompareSpriteDataBalloon(spriteBase.balloon, spriteCmp.balloon, changeData);
                            break;
                        case MiscEntityType::Duck:
                            CompareSpriteDataDuck(spriteBase.duck, spriteCmp.duck, changeData);
                            break;
                        default:
                            break;
                    }
                    break;
                case SpriteIdentifier::Null:
                    break;
            }
        }
    }

    virtual GameStateCompareData_t Compare(const GameStateSnapshot_t& base, const GameStateSnapshot_t& cmp) const override final
    {
        GameStateCompareData_t res;
        res.tickLeft = base.tick;
        res.tickRight = cmp.tick;
        res.srand0Left = base.srand0;
        res.srand0Right = cmp.srand0;

        std::vector<rct_sprite> spritesBase = BuildSpriteList(const_cast<GameStateSnapshot_t&>(base));
        std::vector<rct_sprite> spritesCmp = BuildSpriteList(const_cast<GameStateSnapshot_t&>(cmp));

        for (uint32_t i = 0; i < static_cast<uint32_t>(spritesBase.size()); i++)
        {
            GameStateSpriteChange_t changeData;
            changeData.spriteIndex = i;

            const rct_sprite& spriteBase = spritesBase[i];
            const rct_sprite& spriteCmp = spritesCmp[i];

            changeData.spriteIdentifier = spriteBase.misc.sprite_identifier;
            // This will be nonsense information for all types apart from MiscEntities.
            // This is not an issue though as only MiscEntities will use this field in GetSpriteIdentifierName
            // TODO: Don't do this.
            changeData.miscIdentifier = spriteBase.misc.SubType;

            if (spriteBase.misc.sprite_identifier == SpriteIdentifier::Null
                && spriteCmp.misc.sprite_identifier != SpriteIdentifier::Null)
            {
                // Sprite was added.
                changeData.changeType = GameStateSpriteChange_t::ADDED;
                changeData.spriteIdentifier = spriteCmp.misc.sprite_identifier;
            }
            else if (
                spriteBase.misc.sprite_identifier != SpriteIdentifier::Null
                && spriteCmp.misc.sprite_identifier == SpriteIdentifier::Null)
            {
                // Sprite was removed.
                changeData.changeType = GameStateSpriteChange_t::REMOVED;
                changeData.spriteIdentifier = spriteBase.misc.sprite_identifier;
            }
            else if (
                spriteBase.misc.sprite_identifier == SpriteIdentifier::Null
                && spriteCmp.misc.sprite_identifier == SpriteIdentifier::Null)
            {
                // Do nothing.
                changeData.changeType = GameStateSpriteChange_t::EQUAL;
            }
            else
            {
                CompareSpriteData(spriteBase, spriteCmp, changeData);
                if (changeData.diffs.size() == 0)
                {
                    changeData.changeType = GameStateSpriteChange_t::EQUAL;
                }
                else
                {
                    changeData.changeType = GameStateSpriteChange_t::MODIFIED;
                }
            }

            res.spriteChanges.push_back(std::move(changeData));
        }

        return res;
    }

    static const char* GetSpriteIdentifierName(SpriteIdentifier spriteIdentifier, MiscEntityType miscIdentifier)
    {
        switch (spriteIdentifier)
        {
            case SpriteIdentifier::Null:
                return "Null";
            case SpriteIdentifier::Peep:
                return "Peep";
            case SpriteIdentifier::Vehicle:
                return "Vehicle";
            case SpriteIdentifier::Litter:
                return "Litter";
            case SpriteIdentifier::Misc:
                switch (miscIdentifier)
                {
                    case MiscEntityType::SteamParticle:
                        return "Misc: Steam Particle";
                    case MiscEntityType::MoneyEffect:
                        return "Misc: Money effect";
                    case MiscEntityType::CrashedVehicleParticle:
                        return "Misc: Crash Vehicle Particle";
                    case MiscEntityType::ExplosionCloud:
                        return "Misc: Explosion Cloud";
                    case MiscEntityType::CrashSplash:
                        return "Misc: Crash Splash";
                    case MiscEntityType::ExplosionFlare:
                        return "Misc: Explosion Flare";
                    case MiscEntityType::JumpingFountainWater:
                        return "Misc: Jumping fountain water";
                    case MiscEntityType::Balloon:
                        return "Misc: Balloon";
                    case MiscEntityType::Duck:
                        return "Misc: Duck";
                    case MiscEntityType::JumpingFountainSnow:
                        return "Misc: Jumping fountain snow";
                    default:
                        break;
                }
                return "Misc";
        }
        return "Unknown";
    }

    virtual bool LogCompareDataToFile(const std::string& fileName, const GameStateCompareData_t& cmpData) const override
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
            if (change.changeType == GameStateSpriteChange_t::EQUAL)
                continue;

            const char* typeName = GetSpriteIdentifierName(change.spriteIdentifier, change.miscIdentifier);

            if (change.changeType == GameStateSpriteChange_t::ADDED)
            {
                snprintf(tempBuffer, sizeof(tempBuffer), "Sprite added (%s), index: %u\n", typeName, change.spriteIndex);
                outputBuffer += tempBuffer;
            }
            else if (change.changeType == GameStateSpriteChange_t::REMOVED)
            {
                snprintf(tempBuffer, sizeof(tempBuffer), "Sprite removed (%s), index: %u\n", typeName, change.spriteIndex);
                outputBuffer += tempBuffer;
            }
            else if (change.changeType == GameStateSpriteChange_t::MODIFIED)
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

        FILE* fp = fopen(fileName.c_str(), "wt");
        if (!fp)
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
