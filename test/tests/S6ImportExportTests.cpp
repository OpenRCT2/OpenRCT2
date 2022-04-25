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
#include <openrct2/GameStateSnapshots.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Crypt.h>
#include <openrct2/core/File.h>
#include <openrct2/core/MemoryStream.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/EntityTweener.h>
#include <openrct2/network/network.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/park/ParkFile.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/world/MapAnimation.h>
#include <openrct2/world/Scenery.h>
#include <stdio.h>
#include <string>

using namespace OpenRCT2;

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
        ResetEntitySpatialIndices();

    reset_all_sprite_quadrant_placements();
    scenery_set_default_placement_configuration();
    load_palette();
    EntityTweener::Get().Reset();
    AutoCreateMapAnimations();
    fix_invalid_vehicle_sprite_sizes();

    gGameSpeed = 1;
}

static bool ImportS6(MemoryStream& stream, std::unique_ptr<IContext>& context, bool retainSpatialIndices)
{
    stream.SetPosition(0);

    auto& objManager = context->GetObjectManager();

    auto importer = ParkImporter::CreateS6(context->GetObjectRepository());
    auto loadResult = importer->LoadFromStream(&stream, false);
    objManager.LoadObjects(loadResult.RequiredObjects);
    importer->Import();

    GameInit(retainSpatialIndices);

    return true;
}

static bool ImportPark(MemoryStream& stream, std::unique_ptr<IContext>& context, bool retainSpatialIndices)
{
    stream.SetPosition(0);

    auto& objManager = context->GetObjectManager();

    auto importer = ParkImporter::CreateParkFile(context->GetObjectRepository());
    auto loadResult = importer->LoadFromStream(&stream, false);
    objManager.LoadObjects(loadResult.RequiredObjects);
    importer->Import();

    GameInit(retainSpatialIndices);

    return true;
}

static bool ExportSave(MemoryStream& stream, std::unique_ptr<IContext>& context)
{
    auto& objManager = context->GetObjectManager();

    auto exporter = std::make_unique<ParkFileExporter>();
    exporter->ExportObjectsList = objManager.GetPackableObjects();
    exporter->Export(stream);

    return true;
}

static void RecordGameStateSnapshot(std::unique_ptr<IContext>& context, MemoryStream& snapshotStream)
{
    auto* snapshots = context->GetGameStateSnapshots();

    auto& snapshot = snapshots->CreateSnapshot();
    snapshots->Capture(snapshot);
    snapshots->LinkSnapshot(snapshot, gCurrentTicks, scenario_rand_state().s0);
    DataSerialiser snapShotDs(true, snapshotStream);
    snapshots->SerialiseSnapshot(snapshot, snapShotDs);
}

static void AdvanceGameTicks(uint32_t ticks, std::unique_ptr<IContext>& context)
{
    auto* gameState = context->GetGameState();
    for (uint32_t i = 0; i < ticks; i++)
    {
        gameState->UpdateLogic();
    }
}

static void CompareStates(MemoryStream& importBuffer, MemoryStream& exportBuffer, MemoryStream& snapshotStream)
{
    if (importBuffer.GetLength() != exportBuffer.GetLength())
    {
        log_warning(
            "Inconsistent export size! Import Size: %llu bytes, Export Size: %llu bytes",
            static_cast<unsigned long long>(importBuffer.GetLength()),
            static_cast<unsigned long long>(exportBuffer.GetLength()));
    }

    std::unique_ptr<IContext> context = CreateContext();
    EXPECT_NE(context, nullptr);
    bool initialised = context->Initialise();
    ASSERT_TRUE(initialised);

    DataSerialiser ds(false, snapshotStream);
    IGameStateSnapshots* snapshots = GetContext()->GetGameStateSnapshots();

    GameStateSnapshot_t& importSnapshot = snapshots->CreateSnapshot();
    snapshots->SerialiseSnapshot(importSnapshot, ds);

    GameStateSnapshot_t& exportSnapshot = snapshots->CreateSnapshot();
    snapshots->SerialiseSnapshot(exportSnapshot, ds);

    try
    {
        GameStateCompareData_t cmpData = snapshots->Compare(importSnapshot, exportSnapshot);

        // Find out if there are any differences between the two states
        auto res = std::find_if(
            cmpData.spriteChanges.begin(), cmpData.spriteChanges.end(),
            [](const GameStateSpriteChange_t& diff) { return diff.changeType != GameStateSpriteChange_t::EQUAL; });

        if (res != cmpData.spriteChanges.end())
        {
            log_warning("Snapshot data differences. %s", snapshots->GetCompareDataText(cmpData).c_str());
            FAIL();
        }
    }
    catch (const std::runtime_error& err)
    {
        log_warning("Snapshot data failed to be read. Snapshot not compared. %s", err.what());
        FAIL();
    }
}

TEST(S6ImportExportBasic, all)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    Platform::CoreInit();

    MemoryStream importBuffer;
    MemoryStream exportBuffer;
    MemoryStream snapshotStream;

    // Load initial park data.
    {
        std::unique_ptr<IContext> context = CreateContext();
        EXPECT_NE(context, nullptr);

        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        std::string testParkPath = TestData::GetParkPath("BigMapTest.sv6");
        ASSERT_TRUE(LoadFileToBuffer(importBuffer, testParkPath));
        ASSERT_TRUE(ImportS6(importBuffer, context, false));
        RecordGameStateSnapshot(context, snapshotStream);

        ASSERT_TRUE(ExportSave(exportBuffer, context));
    }

    // Import the exported version.
    {
        std::unique_ptr<IContext> context = CreateContext();
        EXPECT_NE(context, nullptr);

        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        ASSERT_TRUE(ImportPark(exportBuffer, context, true));

        RecordGameStateSnapshot(context, snapshotStream);
    }

    snapshotStream.SetPosition(0);
    CompareStates(importBuffer, exportBuffer, snapshotStream);

    SUCCEED();
}

TEST(S6ImportExportAdvanceTicks, all)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    Platform::CoreInit();

    MemoryStream importBuffer;
    MemoryStream exportBuffer;
    MemoryStream snapshotStream;

    // Load initial park data.
    {
        std::unique_ptr<IContext> context = CreateContext();
        EXPECT_NE(context, nullptr);

        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        std::string testParkPath = TestData::GetParkPath("BigMapTest.sv6");
        ASSERT_TRUE(LoadFileToBuffer(importBuffer, testParkPath));
        ASSERT_TRUE(ImportS6(importBuffer, context, false));
        AdvanceGameTicks(1000, context);
        ASSERT_TRUE(ExportSave(exportBuffer, context));

        RecordGameStateSnapshot(context, snapshotStream);
    }

    // Import the exported version.
    {
        std::unique_ptr<IContext> context = CreateContext();
        EXPECT_NE(context, nullptr);

        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        ASSERT_TRUE(ImportPark(exportBuffer, context, true));

        RecordGameStateSnapshot(context, snapshotStream);
    }

    snapshotStream.SetPosition(0);
    CompareStates(importBuffer, exportBuffer, snapshotStream);

    SUCCEED();
}

TEST(SeaDecrypt, DecryptSea)
{
    auto path = TestData::GetParkPath("volcania.sea");
    auto decrypted = DecryptSea(path);
    auto sha1 = Crypt::SHA1(decrypted.data(), decrypted.size());
    std::array<uint8_t, 20> expected = {
        0x1B, 0x85, 0xFC, 0xC0, 0xE8, 0x9B, 0xBE, 0x72, 0xD9, 0x1F, 0x6E, 0xC8, 0xB1, 0xFF, 0xEC, 0x70, 0x2A, 0x72, 0x05, 0xBB,
    };
    ASSERT_EQ(sha1, expected);
}
