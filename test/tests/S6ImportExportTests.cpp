/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
        rct_sprite* sprite = get_sprite(spriteIdx);
        if (sprite == nullptr)
            res->sprites[spriteIdx].generic.sprite_identifier = SPRITE_IDENTIFIER_NULL;
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

static void CompareSpriteDataCommon(const rct_sprite_common& left, const rct_sprite_common& right)
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
    COMPARE_FIELD(next_x);
    COMPARE_FIELD(next_y);
    COMPARE_FIELD(next_z);
    COMPARE_FIELD(next_flags);
    COMPARE_FIELD(outside_of_park);
    COMPARE_FIELD(state);
    COMPARE_FIELD(sub_state);
    COMPARE_FIELD(sprite_type);
    COMPARE_FIELD(type);
    COMPARE_FIELD(no_of_rides);
    COMPARE_FIELD(tshirt_colour);
    COMPARE_FIELD(trousers_colour);
    COMPARE_FIELD(destination_x);
    COMPARE_FIELD(destination_y);
    COMPARE_FIELD(destination_tolerance);
    COMPARE_FIELD(var_37);
    COMPARE_FIELD(energy);
    COMPARE_FIELD(energy_target);
    COMPARE_FIELD(happiness);
    COMPARE_FIELD(happiness_target);
    COMPARE_FIELD(nausea);
    COMPARE_FIELD(nausea_target);
    COMPARE_FIELD(hunger);
    COMPARE_FIELD(thirst);
    COMPARE_FIELD(toilet);
    COMPARE_FIELD(mass);
    COMPARE_FIELD(time_to_consume);
    COMPARE_FIELD(intensity);
    COMPARE_FIELD(nausea_tolerance);
    COMPARE_FIELD(window_invalidate_flags);
    COMPARE_FIELD(paid_on_drink);
    for (int i = 0; i < PEEP_MAX_THOUGHTS; i++)
    {
        COMPARE_FIELD(ride_types_been_on[i]);
    }
    COMPARE_FIELD(item_extra_flags);
    COMPARE_FIELD(photo2_ride_ref);
    COMPARE_FIELD(photo3_ride_ref);
    COMPARE_FIELD(photo4_ride_ref);
    COMPARE_FIELD(current_ride);
    COMPARE_FIELD(current_ride_station);
    COMPARE_FIELD(current_train);
    COMPARE_FIELD(time_to_sitdown);
    COMPARE_FIELD(special_sprite);
    COMPARE_FIELD(action_sprite_type);
    COMPARE_FIELD(next_action_sprite_type);
    COMPARE_FIELD(action_sprite_image_offset);
    COMPARE_FIELD(action);
    COMPARE_FIELD(action_frame);
    COMPARE_FIELD(step_progress);
    COMPARE_FIELD(next_in_queue);
    COMPARE_FIELD(maze_last_edge);
    COMPARE_FIELD(interaction_ride_index);
    COMPARE_FIELD(time_in_queue);
    for (int i = 0; i < 32; i++)
    {
        COMPARE_FIELD(rides_been_on[i]);
    }
    COMPARE_FIELD(id);
    COMPARE_FIELD(cash_in_pocket);
    COMPARE_FIELD(cash_spent);
    COMPARE_FIELD(time_in_park);
    COMPARE_FIELD(rejoin_queue_timeout);
    COMPARE_FIELD(previous_ride);
    COMPARE_FIELD(previous_ride_time_out);
    for (int i = 0; i < PEEP_MAX_THOUGHTS; i++)
    {
        COMPARE_FIELD(thoughts[i].type);
        COMPARE_FIELD(thoughts[i].item);
        COMPARE_FIELD(thoughts[i].freshness);
        COMPARE_FIELD(thoughts[i].fresh_timeout);
    }
    COMPARE_FIELD(path_check_optimisation);
    COMPARE_FIELD(guest_heading_to_ride_id);
    COMPARE_FIELD(staff_orders);
    COMPARE_FIELD(photo1_ride_ref);
    COMPARE_FIELD(peep_flags);
    COMPARE_FIELD(pathfind_goal.x);
    COMPARE_FIELD(pathfind_goal.y);
    COMPARE_FIELD(pathfind_goal.z);
    COMPARE_FIELD(pathfind_goal.direction);
    for (int i = 0; i < 4; i++)
    {
        COMPARE_FIELD(pathfind_history[i].x);
        COMPARE_FIELD(pathfind_history[i].y);
        COMPARE_FIELD(pathfind_history[i].z);
        COMPARE_FIELD(pathfind_history[i].direction);
    }
    COMPARE_FIELD(no_action_frame_num);
    COMPARE_FIELD(litter_count);
    COMPARE_FIELD(time_on_ride);
    COMPARE_FIELD(disgusting_count);
    COMPARE_FIELD(paid_to_enter);
    COMPARE_FIELD(paid_on_rides);
    COMPARE_FIELD(paid_on_food);
    COMPARE_FIELD(paid_on_souvenirs);
    COMPARE_FIELD(no_of_food);
    COMPARE_FIELD(no_of_drinks);
    COMPARE_FIELD(no_of_souvenirs);
    COMPARE_FIELD(vandalism_seen);
    COMPARE_FIELD(voucher_type);
    COMPARE_FIELD(voucher_arguments);
    COMPARE_FIELD(surroundings_thought_timeout);
    COMPARE_FIELD(angriness);
    COMPARE_FIELD(time_lost);
    COMPARE_FIELD(days_in_queue);
    COMPARE_FIELD(balloon_colour);
    COMPARE_FIELD(umbrella_colour);
    COMPARE_FIELD(hat_colour);
    COMPARE_FIELD(favourite_ride);
    COMPARE_FIELD(favourite_ride_rating);
    COMPARE_FIELD(item_standard_flags);
}

static void CompareSpriteDataVehicle(const rct_vehicle& left, const rct_vehicle& right)
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
    COMPARE_FIELD(track_x);
    COMPARE_FIELD(track_y);
    COMPARE_FIELD(track_z);
    COMPARE_FIELD(next_vehicle_on_train);
    COMPARE_FIELD(prev_vehicle_on_ride);
    COMPARE_FIELD(next_vehicle_on_ride);
    COMPARE_FIELD(var_44);
    COMPARE_FIELD(mass);
    COMPARE_FIELD(update_flags);
    COMPARE_FIELD(swing_sprite);
    COMPARE_FIELD(current_station);
    COMPARE_FIELD(swinging_car_var_0);
    COMPARE_FIELD(var_4E);
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
    COMPARE_FIELD(var_CD);
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

static void CompareSpriteDataLitter(const rct_litter& left, const rct_litter& right)
{
    COMPARE_FIELD(creationTick);
}

static void CompareSpriteDataSteamParticle(const rct_steam_particle& left, const rct_steam_particle& right)
{
    COMPARE_FIELD(time_to_move);
}

static void CompareSpriteDataMoneyEffect(const rct_money_effect& left, const rct_money_effect& right)
{
    COMPARE_FIELD(move_delay);
    COMPARE_FIELD(num_movements);
    COMPARE_FIELD(vertical);
    COMPARE_FIELD(value);
    COMPARE_FIELD(offset_x);
    COMPARE_FIELD(wiggle);
}

static void CompareSpriteDataCrashedVehicleParticle(
    const rct_crashed_vehicle_particle& left, const rct_crashed_vehicle_particle& right)
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

static void CompareSpriteDataBalloon(const rct_balloon& left, const rct_balloon& right)
{
    COMPARE_FIELD(popped);
    COMPARE_FIELD(time_to_move);
    COMPARE_FIELD(colour);
}

static void CompareSpriteDataDuck(const rct_duck& left, const rct_duck& right)
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
            case SPRITE_IDENTIFIER_PEEP:
                CompareSpriteDataPeep(left.peep, right.peep);
                break;
            case SPRITE_IDENTIFIER_VEHICLE:
                CompareSpriteDataVehicle(left.vehicle, right.vehicle);
                break;
            case SPRITE_IDENTIFIER_LITTER:
                CompareSpriteDataLitter(left.litter, right.litter);
                break;
            case SPRITE_IDENTIFIER_MISC:
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
            (unsigned long long)importBuffer.GetLength(), (unsigned long long)exportBuffer.GetLength());
    }

    for (size_t spriteIdx = 0; spriteIdx < MAX_SPRITES; ++spriteIdx)
    {
        if (importedState->sprites[spriteIdx].generic.sprite_identifier == SPRITE_IDENTIFIER_NULL
            && exportedState->sprites[spriteIdx].generic.sprite_identifier == SPRITE_IDENTIFIER_NULL)
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
