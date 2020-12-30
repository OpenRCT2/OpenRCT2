/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestData.h"

#include <gtest/gtest.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Crypt.h>
#include <openrct2/core/File.h>
#include <openrct2/core/MemoryStream.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/network/network.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/platform/platform.h>
#include <openrct2/rct2/S6Exporter.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Sprite.h>
#include <stdio.h>
#include <string>

using namespace OpenRCT2;

struct GameState_t
{
    rct_sprite sprites[MAX_SPRITES];
};

static bool LoadFileToBuffer(MemoryStream& stream, const std::string& filePath)
{
    FILE* fp = fopen(filePath.c_str(), "rb");
    EXPECT_NE(fp, nullptr);
    if (fp == nullptr)
        return false;

    uint8_t buf[1024];
    size_t bytesRead = fread(buf, 1, sizeof(buf), fp);
    while (bytesRead > 0)
    {
        stream.Write(buf, bytesRead);
        bytesRead = fread(buf, 1, sizeof(buf), fp);
    }
    fclose(fp);

    return true;
}

static void GameInit(bool retainSpatialIndices)
{
    if (!retainSpatialIndices)
        reset_sprite_spatial_index();

    reset_all_sprite_quadrant_placements();
    scenery_set_default_placement_configuration();
    load_palette();
    map_reorganise_elements();
    sprite_position_tween_reset();
    AutoCreateMapAnimations();
    fix_invalid_vehicle_sprite_sizes();

    gGameSpeed = 1;
}

static bool ImportSave(MemoryStream& stream, std::unique_ptr<IContext>& context, bool retainSpatialIndices)
{
    stream.SetPosition(0);

    auto& objManager = context->GetObjectManager();

    auto importer = ParkImporter::CreateS6(context->GetObjectRepository());
    auto loadResult = importer->LoadFromStream(&stream, false);
    objManager.LoadObjects(loadResult.RequiredObjects.data(), loadResult.RequiredObjects.size());
    importer->Import();

    GameInit(retainSpatialIndices);

    return true;
}

static bool ExportSave(MemoryStream& stream, std::unique_ptr<IContext>& context)
{
    auto& objManager = context->GetObjectManager();

    auto exporter = std::make_unique<S6Exporter>();
    exporter->ExportObjectsList = objManager.GetPackableObjects();
    exporter->Export();
    exporter->SaveGame(&stream);

    return true;
}

static std::unique_ptr<GameState_t> GetGameState(std::unique_ptr<IContext>& context)
{
    std::unique_ptr<GameState_t> res = std::make_unique<GameState_t>();
    for (size_t spriteIdx = 0; spriteIdx < MAX_SPRITES; spriteIdx++)
    {
        rct_sprite* sprite = reinterpret_cast<rct_sprite*>(GetEntity(spriteIdx));
        if (sprite == nullptr)
            res->sprites[spriteIdx].generic.sprite_identifier = SpriteIdentifier::Null;
        else
            res->sprites[spriteIdx] = *sprite;
    }
    return res;
}

static void AdvanceGameTicks(uint32_t ticks, std::unique_ptr<IContext>& context)
{
    auto* gameState = context->GetGameState();
    for (uint32_t i = 0; i < ticks; i++)
    {
        gameState->UpdateLogic();
    }
}

#define COMPARE_FIELD(field) EXPECT_EQ(left.field, right.field)

static void CompareSpriteDataCommon(const SpriteBase& left, const SpriteBase& right)
{
    COMPARE_FIELD(sprite_identifier);
    COMPARE_FIELD(type);
    COMPARE_FIELD(next_in_quadrant);
    COMPARE_FIELD(next);
    COMPARE_FIELD(previous);
    COMPARE_FIELD(linked_list_index);
    COMPARE_FIELD(sprite_index);
    COMPARE_FIELD(flags);
    COMPARE_FIELD(x);
    COMPARE_FIELD(y);
    COMPARE_FIELD(z);
    // INVESTIGATE: These fields never match but are also not important to the game state.
    /*
    COMPARE_FIELD(sprite_width);
    COMPARE_FIELD(sprite_height_negative);
    COMPARE_FIELD(sprite_height_positive);
    COMPARE_FIELD(sprite_left);
    COMPARE_FIELD(sprite_top);
    COMPARE_FIELD(sprite_right);
    COMPARE_FIELD(sprite_bottom);
    */
    COMPARE_FIELD(sprite_direction);
}

static void CompareSpriteDataPeep(const Peep& left, const Peep& right)
{
    COMPARE_FIELD(NextLoc.x);
    COMPARE_FIELD(NextLoc.y);
    COMPARE_FIELD(NextLoc.z);
    COMPARE_FIELD(NextFlags);
    COMPARE_FIELD(OutsideOfPark);
    COMPARE_FIELD(State);
    COMPARE_FIELD(SubState);
    COMPARE_FIELD(SpriteType);
    COMPARE_FIELD(AssignedPeepType);
    COMPARE_FIELD(GuestNumRides);
    COMPARE_FIELD(TshirtColour);
    COMPARE_FIELD(TrousersColour);
    COMPARE_FIELD(DestinationX);
    COMPARE_FIELD(DestinationY);
    COMPARE_FIELD(DestinationTolerance);
    COMPARE_FIELD(Var37);
    COMPARE_FIELD(Energy);
    COMPARE_FIELD(EnergyTarget);
    COMPARE_FIELD(Happiness);
    COMPARE_FIELD(HappinessTarget);
    COMPARE_FIELD(Nausea);
    COMPARE_FIELD(NauseaTarget);
    COMPARE_FIELD(Hunger);
    COMPARE_FIELD(Thirst);
    COMPARE_FIELD(Toilet);
    COMPARE_FIELD(Mass);
    COMPARE_FIELD(TimeToConsume);
    COMPARE_FIELD(Intensity);
    COMPARE_FIELD(NauseaTolerance);
    COMPARE_FIELD(WindowInvalidateFlags);
    COMPARE_FIELD(PaidOnDrink);
    for (int i = 0; i < PEEP_MAX_THOUGHTS; i++)
    {
        COMPARE_FIELD(RideTypesBeenOn[i]);
    }
    COMPARE_FIELD(ItemFlags);
    COMPARE_FIELD(Photo2RideRef);
    COMPARE_FIELD(Photo3RideRef);
    COMPARE_FIELD(Photo4RideRef);
    COMPARE_FIELD(CurrentRide);
    COMPARE_FIELD(CurrentRideStation);
    COMPARE_FIELD(CurrentTrain);
    COMPARE_FIELD(TimeToSitdown);
    COMPARE_FIELD(SpecialSprite);
    COMPARE_FIELD(ActionSpriteType);
    COMPARE_FIELD(NextActionSpriteType);
    COMPARE_FIELD(ActionSpriteImageOffset);
    COMPARE_FIELD(Action);
    COMPARE_FIELD(ActionFrame);
    COMPARE_FIELD(StepProgress);
    COMPARE_FIELD(GuestNextInQueue);
    COMPARE_FIELD(MazeLastEdge);
    COMPARE_FIELD(InteractionRideIndex);
    COMPARE_FIELD(TimeInQueue);
    for (int i = 0; i < 32; i++)
    {
        COMPARE_FIELD(RidesBeenOn[i]);
    }
    COMPARE_FIELD(Id);
    COMPARE_FIELD(CashInPocket);
    COMPARE_FIELD(CashSpent);
    COMPARE_FIELD(ParkEntryTime);
    COMPARE_FIELD(RejoinQueueTimeout);
    COMPARE_FIELD(PreviousRide);
    COMPARE_FIELD(PreviousRideTimeOut);
    for (int i = 0; i < PEEP_MAX_THOUGHTS; i++)
    {
        COMPARE_FIELD(Thoughts[i].type);
        COMPARE_FIELD(Thoughts[i].item);
        COMPARE_FIELD(Thoughts[i].freshness);
        COMPARE_FIELD(Thoughts[i].fresh_timeout);
    }
    COMPARE_FIELD(PathCheckOptimisation);
    COMPARE_FIELD(GuestHeadingToRideId);
    COMPARE_FIELD(StaffOrders);
    COMPARE_FIELD(Photo1RideRef);
    COMPARE_FIELD(PeepFlags);
    COMPARE_FIELD(PathfindGoal.x);
    COMPARE_FIELD(PathfindGoal.y);
    COMPARE_FIELD(PathfindGoal.z);
    COMPARE_FIELD(PathfindGoal.direction);
    for (int i = 0; i < 4; i++)
    {
        COMPARE_FIELD(PathfindHistory[i].x);
        COMPARE_FIELD(PathfindHistory[i].y);
        COMPARE_FIELD(PathfindHistory[i].z);
        COMPARE_FIELD(PathfindHistory[i].direction);
    }
    COMPARE_FIELD(WalkingFrameNum);
    COMPARE_FIELD(LitterCount);
    COMPARE_FIELD(GuestTimeOnRide);
    COMPARE_FIELD(DisgustingCount);
    COMPARE_FIELD(PaidToEnter);
    COMPARE_FIELD(PaidOnRides);
    COMPARE_FIELD(PaidOnFood);
    COMPARE_FIELD(PaidOnSouvenirs);
    COMPARE_FIELD(AmountOfFood);
    COMPARE_FIELD(AmountOfDrinks);
    COMPARE_FIELD(AmountOfSouvenirs);
    COMPARE_FIELD(VandalismSeen);
    COMPARE_FIELD(VoucherType);
    COMPARE_FIELD(VoucherRideId);
    COMPARE_FIELD(SurroundingsThoughtTimeout);
    COMPARE_FIELD(Angriness);
    COMPARE_FIELD(TimeLost);
    COMPARE_FIELD(DaysInQueue);
    COMPARE_FIELD(BalloonColour);
    COMPARE_FIELD(UmbrellaColour);
    COMPARE_FIELD(HatColour);
    COMPARE_FIELD(FavouriteRide);
    COMPARE_FIELD(FavouriteRideRating);
}

static void CompareSpriteDataVehicle(const Vehicle& left, const Vehicle& right)
{
    COMPARE_FIELD(vehicle_sprite_type);
    COMPARE_FIELD(bank_rotation);
    COMPARE_FIELD(remaining_distance);
    COMPARE_FIELD(velocity);
    COMPARE_FIELD(acceleration);
    COMPARE_FIELD(ride);
    COMPARE_FIELD(vehicle_type);
    COMPARE_FIELD(colours.body_colour);
    COMPARE_FIELD(colours.trim_colour);
    COMPARE_FIELD(track_progress);
    COMPARE_FIELD(track_direction);
    COMPARE_FIELD(TrackLocation.x);
    COMPARE_FIELD(TrackLocation.y);
    COMPARE_FIELD(TrackLocation.z);
    COMPARE_FIELD(next_vehicle_on_train);
    COMPARE_FIELD(prev_vehicle_on_ride);
    COMPARE_FIELD(next_vehicle_on_ride);
    COMPARE_FIELD(var_44);
    COMPARE_FIELD(mass);
    COMPARE_FIELD(update_flags);
    COMPARE_FIELD(SwingSprite);
    COMPARE_FIELD(current_station);
    COMPARE_FIELD(SwingPosition);
    COMPARE_FIELD(SwingSpeed);
    COMPARE_FIELD(status);
    COMPARE_FIELD(sub_state);
    for (int i = 0; i < 32; i++)
    {
        COMPARE_FIELD(peep[i]);
    }
    for (int i = 0; i < 32; i++)
    {
        COMPARE_FIELD(peep_tshirt_colours[i]);
    }
    COMPARE_FIELD(num_seats);
    COMPARE_FIELD(num_peeps);
    COMPARE_FIELD(next_free_seat);
    COMPARE_FIELD(restraints_position);
    COMPARE_FIELD(spin_speed);
    COMPARE_FIELD(sound2_flags);
    COMPARE_FIELD(spin_sprite);
    COMPARE_FIELD(sound1_id);
    COMPARE_FIELD(sound1_volume);
    COMPARE_FIELD(sound2_id);
    COMPARE_FIELD(sound2_volume);
    COMPARE_FIELD(sound_vector_factor);
    COMPARE_FIELD(cable_lift_target);
    COMPARE_FIELD(speed);
    COMPARE_FIELD(powered_acceleration);
    COMPARE_FIELD(var_C4);
    COMPARE_FIELD(animation_frame);
    for (int i = 0; i < 2; i++)
    {
        COMPARE_FIELD(pad_C6[i]);
    }
    COMPARE_FIELD(var_C8);
    COMPARE_FIELD(var_CA);
    COMPARE_FIELD(scream_sound_id);
    COMPARE_FIELD(TrackSubposition);
    COMPARE_FIELD(num_laps);
    COMPARE_FIELD(brake_speed);
    COMPARE_FIELD(lost_time_out);
    COMPARE_FIELD(vertical_drop_countdown);
    COMPARE_FIELD(var_D3);
    COMPARE_FIELD(mini_golf_current_animation);
    COMPARE_FIELD(mini_golf_flags);
    COMPARE_FIELD(ride_subtype);
    COMPARE_FIELD(colours_extended);
    COMPARE_FIELD(seat_rotation);
    COMPARE_FIELD(target_seat_rotation);
}

static void CompareSpriteDataLitter(const Litter& left, const Litter& right)
{
    COMPARE_FIELD(creationTick);
}

static void CompareSpriteDataSteamParticle(const SteamParticle& left, const SteamParticle& right)
{
    COMPARE_FIELD(time_to_move);
}

static void CompareSpriteDataMoneyEffect(const MoneyEffect& left, const MoneyEffect& right)
{
    COMPARE_FIELD(MoveDelay);
    COMPARE_FIELD(NumMovements);
    COMPARE_FIELD(Vertical);
    COMPARE_FIELD(Value);
    COMPARE_FIELD(OffsetX);
    COMPARE_FIELD(Wiggle);
}

static void CompareSpriteDataCrashedVehicleParticle(const VehicleCrashParticle& left, const VehicleCrashParticle& right)
{
    COMPARE_FIELD(time_to_live);
    for (size_t i = 0; i < std::size(left.colour); i++)
    {
        COMPARE_FIELD(colour[i]);
    }
    COMPARE_FIELD(crashed_sprite_base);
    COMPARE_FIELD(velocity_x);
    COMPARE_FIELD(velocity_y);
    COMPARE_FIELD(velocity_z);
    COMPARE_FIELD(acceleration_x);
    COMPARE_FIELD(acceleration_y);
    COMPARE_FIELD(acceleration_z);
}

static void CompareSpriteDataJumpingFountain(const JumpingFountain& left, const JumpingFountain& right)
{
    COMPARE_FIELD(NumTicksAlive);
    COMPARE_FIELD(FountainFlags);
    COMPARE_FIELD(TargetX);
    COMPARE_FIELD(TargetY);
    COMPARE_FIELD(Iteration);
}

static void CompareSpriteDataBalloon(const Balloon& left, const Balloon& right)
{
    COMPARE_FIELD(popped);
    COMPARE_FIELD(time_to_move);
    COMPARE_FIELD(colour);
}

static void CompareSpriteDataDuck(const Duck& left, const Duck& right)
{
    COMPARE_FIELD(target_x);
    COMPARE_FIELD(target_y);
    COMPARE_FIELD(state);
}

static void CompareSpriteData(const rct_sprite& left, const rct_sprite& right)
{
    CompareSpriteDataCommon(left.generic, right.generic);
    if (left.generic.sprite_identifier == right.generic.sprite_identifier)
    {
        switch (left.generic.sprite_identifier)
        {
            case SpriteIdentifier::Peep:
                CompareSpriteDataPeep(left.peep, right.peep);
                break;
            case SpriteIdentifier::Vehicle:
                CompareSpriteDataVehicle(left.vehicle, right.vehicle);
                break;
            case SpriteIdentifier::Litter:
                CompareSpriteDataLitter(left.litter, right.litter);
                break;
            case SpriteIdentifier::Misc:
                switch (left.generic.type)
                {
                    case SPRITE_MISC_STEAM_PARTICLE:
                        CompareSpriteDataSteamParticle(left.steam_particle, right.steam_particle);
                        break;
                    case SPRITE_MISC_MONEY_EFFECT:
                        CompareSpriteDataMoneyEffect(left.money_effect, right.money_effect);
                        break;
                    case SPRITE_MISC_CRASHED_VEHICLE_PARTICLE:
                        CompareSpriteDataCrashedVehicleParticle(left.crashed_vehicle_particle, right.crashed_vehicle_particle);
                        break;
                    case SPRITE_MISC_JUMPING_FOUNTAIN_SNOW:
                    case SPRITE_MISC_JUMPING_FOUNTAIN_WATER:
                        CompareSpriteDataJumpingFountain(left.jumping_fountain, right.jumping_fountain);
                        break;
                    case SPRITE_MISC_BALLOON:
                        CompareSpriteDataBalloon(left.balloon, right.balloon);
                        break;
                    case SPRITE_MISC_DUCK:
                        CompareSpriteDataDuck(left.duck, right.duck);
                        break;
                }
                break;
            case SpriteIdentifier::Null:
                break;
        }
    }
}

static void CompareStates(
    MemoryStream& importBuffer, MemoryStream& exportBuffer, std::unique_ptr<GameState_t>& importedState,
    std::unique_ptr<GameState_t>& exportedState)
{
    if (importBuffer.GetLength() != exportBuffer.GetLength())
    {
        log_warning(
            "Inconsistent export size! Import Size: %llu bytes, Export Size: %llu bytes",
            static_cast<unsigned long long>(importBuffer.GetLength()),
            static_cast<unsigned long long>(exportBuffer.GetLength()));
    }

    for (size_t spriteIdx = 0; spriteIdx < MAX_SPRITES; ++spriteIdx)
    {
        if (importedState->sprites[spriteIdx].generic.sprite_identifier == SpriteIdentifier::Null
            && exportedState->sprites[spriteIdx].generic.sprite_identifier == SpriteIdentifier::Null)
        {
            continue;
        }
        CompareSpriteData(importedState->sprites[spriteIdx], exportedState->sprites[spriteIdx]);
    }
}

TEST(S6ImportExportBasic, all)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    core_init();

    MemoryStream importBuffer;
    MemoryStream exportBuffer;

    std::unique_ptr<GameState_t> importedState;
    std::unique_ptr<GameState_t> exportedState;

    // Load initial park data.
    {
        std::unique_ptr<IContext> context = CreateContext();
        EXPECT_NE(context, nullptr);

        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        std::string testParkPath = TestData::GetParkPath("BigMapTest.sv6");
        ASSERT_TRUE(LoadFileToBuffer(importBuffer, testParkPath));
        ASSERT_TRUE(ImportSave(importBuffer, context, false));
        importedState = GetGameState(context);
        ASSERT_NE(importedState, nullptr);

        ASSERT_TRUE(ExportSave(exportBuffer, context));
    }

    // Import the exported version.
    {
        std::unique_ptr<IContext> context = CreateContext();
        EXPECT_NE(context, nullptr);

        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        ASSERT_TRUE(ImportSave(exportBuffer, context, true));

        exportedState = GetGameState(context);
        ASSERT_NE(exportedState, nullptr);
    }

    CompareStates(importBuffer, exportBuffer, importedState, exportedState);

    SUCCEED();
}

TEST(S6ImportExportAdvanceTicks, all)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    core_init();

    MemoryStream importBuffer;
    MemoryStream exportBuffer;

    std::unique_ptr<GameState_t> importedState;
    std::unique_ptr<GameState_t> exportedState;

    // Load initial park data.
    {
        std::unique_ptr<IContext> context = CreateContext();
        EXPECT_NE(context, nullptr);

        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        std::string testParkPath = TestData::GetParkPath("BigMapTest.sv6");
        ASSERT_TRUE(LoadFileToBuffer(importBuffer, testParkPath));
        ASSERT_TRUE(ImportSave(importBuffer, context, false));
        AdvanceGameTicks(1000, context);
        ASSERT_TRUE(ExportSave(exportBuffer, context));

        importedState = GetGameState(context);
        ASSERT_NE(importedState, nullptr);
    }

    // Import the exported version.
    {
        std::unique_ptr<IContext> context = CreateContext();
        EXPECT_NE(context, nullptr);

        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        ASSERT_TRUE(ImportSave(exportBuffer, context, true));

        exportedState = GetGameState(context);
        ASSERT_NE(exportedState, nullptr);
    }

    CompareStates(importBuffer, exportBuffer, importedState, exportedState);

    SUCCEED();
}

TEST(SeaDecrypt, DecryptSea)
{
    auto path = TestData::GetParkPath("volcania.sea");
    auto decrypted = DecryptSea(path);
    auto sha1 = Crypt::SHA1(decrypted.data(), decrypted.size());
    std::array<uint8_t, 20> expected = { 0x1B, 0x85, 0xFC, 0xC0, 0xE8, 0x9B, 0xBE, 0x72, 0xD9, 0x1F,
                                         0x6E, 0xC8, 0xB1, 0xFF, 0xEC, 0x70, 0x2A, 0x72, 0x05, 0xBB };
    ASSERT_EQ(sha1, expected);
}
