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
#include <string>

using namespace OpenRCT2;

// Copied from Network.cpp
static bool LoadMap(IStream* stream)
{
    bool result = false;
    try
    {
        auto context = GetContext();
        auto& objManager = context->GetObjectManager();
        auto importer = ParkImporter::CreateS6(context->GetObjectRepository());
        auto loadResult = importer->LoadFromStream(stream, false);
        objManager.LoadObjects(loadResult.RequiredObjects.data(), loadResult.RequiredObjects.size());
        importer->Import();

        sprite_position_tween_reset();

        // Read checksum
        [[maybe_unused]] uint32_t checksum = stream->ReadValue<uint32_t>();

        // Read other data not in normal save files
        stream->Read(gSpriteSpatialIndex, 0x10001 * sizeof(uint16_t));
        gGamePaused = stream->ReadValue<uint32_t>();
        _guestGenerationProbability = stream->ReadValue<uint32_t>();
        _suggestedGuestMaximum = stream->ReadValue<uint32_t>();
        gCheatsSandboxMode = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableClearanceChecks = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableSupportLimits = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableTrainLengthLimit = stream->ReadValue<uint8_t>() != 0;
        gCheatsEnableChainLiftOnAllTrack = stream->ReadValue<uint8_t>() != 0;
        gCheatsShowAllOperatingModes = stream->ReadValue<uint8_t>() != 0;
        gCheatsShowVehiclesFromOtherTrackTypes = stream->ReadValue<uint8_t>() != 0;
        gCheatsFastLiftHill = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableBrakesFailure = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableAllBreakdowns = stream->ReadValue<uint8_t>() != 0;
        gCheatsBuildInPauseMode = stream->ReadValue<uint8_t>() != 0;
        gCheatsIgnoreRideIntensity = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableVandalism = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableLittering = stream->ReadValue<uint8_t>() != 0;
        gCheatsNeverendingMarketing = stream->ReadValue<uint8_t>() != 0;
        gCheatsFreezeWeather = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisablePlantAging = stream->ReadValue<uint8_t>() != 0;
        gCheatsAllowArbitraryRideTypeChanges = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableRideValueAging = stream->ReadValue<uint8_t>() != 0;
        gConfigGeneral.show_real_names_of_guests = stream->ReadValue<uint8_t>() != 0;
        gCheatsIgnoreResearchStatus = stream->ReadValue<uint8_t>() != 0;

        result = true;
    }
    catch (const std::exception&)
    {
    }
    return result;
}

// Copied from Network.cpp
static bool SaveMap(IStream* stream, const std::vector<const ObjectRepositoryItem*>& objects)
{
    bool result = false;
    map_reorganise_elements();
    viewport_set_saved_view();
    try
    {
        auto s6exporter = std::make_unique<S6Exporter>();
        s6exporter->ExportObjectsList = objects;
        s6exporter->Export();
        s6exporter->SaveGame(stream);

        // Write other data not in normal save files
        stream->Write(gSpriteSpatialIndex, 0x10001 * sizeof(uint16_t));
        stream->WriteValue<uint32_t>(gGamePaused);
        stream->WriteValue<uint32_t>(_guestGenerationProbability);
        stream->WriteValue<uint32_t>(_suggestedGuestMaximum);
        stream->WriteValue<uint8_t>(gCheatsSandboxMode);
        stream->WriteValue<uint8_t>(gCheatsDisableClearanceChecks);
        stream->WriteValue<uint8_t>(gCheatsDisableSupportLimits);
        stream->WriteValue<uint8_t>(gCheatsDisableTrainLengthLimit);
        stream->WriteValue<uint8_t>(gCheatsEnableChainLiftOnAllTrack);
        stream->WriteValue<uint8_t>(gCheatsShowAllOperatingModes);
        stream->WriteValue<uint8_t>(gCheatsShowVehiclesFromOtherTrackTypes);
        stream->WriteValue<uint8_t>(gCheatsFastLiftHill);
        stream->WriteValue<uint8_t>(gCheatsDisableBrakesFailure);
        stream->WriteValue<uint8_t>(gCheatsDisableAllBreakdowns);
        stream->WriteValue<uint8_t>(gCheatsBuildInPauseMode);
        stream->WriteValue<uint8_t>(gCheatsIgnoreRideIntensity);
        stream->WriteValue<uint8_t>(gCheatsDisableVandalism);
        stream->WriteValue<uint8_t>(gCheatsDisableLittering);
        stream->WriteValue<uint8_t>(gCheatsNeverendingMarketing);
        stream->WriteValue<uint8_t>(gCheatsFreezeWeather);
        stream->WriteValue<uint8_t>(gCheatsDisablePlantAging);
        stream->WriteValue<uint8_t>(gCheatsAllowArbitraryRideTypeChanges);
        stream->WriteValue<uint8_t>(gCheatsDisableRideValueAging);
        stream->WriteValue<uint8_t>(gConfigGeneral.show_real_names_of_guests);
        stream->WriteValue<uint8_t>(gCheatsIgnoreResearchStatus);

        result = true;
    }
    catch (const std::exception&)
    {
    }
    return result;
}

// Special version of game_load_init which does not call reset_sprite_spatial_index
// This is conditionally done in game_load_init for when we are not a client.
static void network_game_load_init()
{
    gScreenFlags = SCREEN_FLAGS_PLAYING;

    reset_all_sprite_quadrant_placements();
    scenery_set_default_placement_configuration();

    gWindowUpdateTicks = 0;

    load_palette();

    gGameSpeed = 1;
}

TEST(NetworkLoadSave, all)
{
    std::string path = TestData::GetParkPath("bpb.sv6");

    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    core_init();

    auto mainContext = CreateContext();
    bool initialised = mainContext->Initialise();
    ASSERT_TRUE(initialised);

    auto& objManager = mainContext->GetObjectManager();

    // Load initial park data.
    {
        auto importer = ParkImporter::CreateS6(mainContext->GetObjectRepository());
        auto loadResult = importer->Load(path.c_str());
        objManager.LoadObjects(loadResult.RequiredObjects.data(), loadResult.RequiredObjects.size());
        importer->Import();

        game_load_init();
    }

    // Advance the park for 100 ticks to have changes in the park.
    {
        for (int i = 0; i < 100; i++)
        {
            mainContext->GetGameState()->UpdateLogic();
        }
    }

    MemoryStream savedPark;
    rct_sprite_checksum checksumSave;
    rct_sprite_checksum checksumLoad;

    // Save park.
    {
        std::vector<const ObjectRepositoryItem*> objects = objManager.GetPackableObjects();

        bool saveResult = SaveMap(&savedPark, objects);
        ASSERT_TRUE(saveResult);

        checksumSave = sprite_checksum();
    }

    // Import the exported version.
    {
        savedPark.SetPosition(0);

        bool loadResult = LoadMap(&savedPark);
        ASSERT_TRUE(loadResult);

        network_game_load_init();

        checksumLoad = sprite_checksum();
    }

    ASSERT_EQ(checksumSave.ToString(), checksumLoad.ToString());

    SUCCEED();
}
