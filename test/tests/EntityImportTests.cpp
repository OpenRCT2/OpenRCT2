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
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/core/MemoryStream.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Guest.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/world/MapAnimation.h>

using namespace OpenRCT2;

class EntityImportTests : public testing::Test
{
protected:
    std::unique_ptr<IContext> context;

    void SetUp() override
    {
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;
        context = CreateContext();
        ASSERT_NE(context, nullptr);
        ASSERT_TRUE(context->Initialise());
    }

    void TearDown() override
    {
        context.reset();
    }
};

static bool LoadFileToBuffer(MemoryStream& stream, const std::string& filePath)
{
    FILE* fp = fopen(filePath.c_str(), "rb");
    EXPECT_NE(fp, nullptr) << "Failed to open file: " << filePath;
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

// This here tests that CreateEntityAt returns nullptr when trying to create an entity at an index that's already occupied.
// This behavior is what caused crashes before, corrupted saves had duplicate EntityIndex values that caused CreateEntityAt to
// return nullptr, which was then dereferenced.
TEST_F(EntityImportTests, CreateEntityAtDuplicateIndexReturnsNull)
{
    auto& gameState = getGameState();
    gameState.entities.ResetAllEntities();

    // Create an entity at index 100
    auto* entity1 = gameState.entities.CreateEntityAt<Guest>(EntityId::FromUnderlying(100));
    ASSERT_NE(entity1, nullptr);
    EXPECT_EQ(entity1->Id.ToUnderlying(), 100u);

    // Try to create another entity at the same index, which should return nullptr
    auto* entity2 = gameState.entities.CreateEntityAt<Guest>(EntityId::FromUnderlying(100));
    EXPECT_EQ(entity2, nullptr);
}

// This test verifies that corrupted S6 files with duplicate EntityIndex values can be loaded without crashing.
TEST_F(EntityImportTests, S6ImportCorruptedDuplicateEntityIndicesDoesNotCrash)
{
    MemoryStream buffer;
    std::string testParkPath = TestData::GetParkPath("corrupted_duplicate_entity_indices.sv6");
    ASSERT_TRUE(LoadFileToBuffer(buffer, testParkPath));

    buffer.SetPosition(0);
    auto& objManager = context->GetObjectManager();
    auto importer = ParkImporter::CreateS6(context->GetObjectRepository());
    auto loadResult = importer->LoadFromStream(&buffer, false);
    objManager.LoadObjects(loadResult.RequiredObjects);

    MapAnimations::ClearAll();
    auto& gameState = getGameState();

    // This should not crash because the code should sanitize EntityIndex values before CreateEntityAt is called
    EXPECT_NO_THROW(importer->Import(gameState));
}
