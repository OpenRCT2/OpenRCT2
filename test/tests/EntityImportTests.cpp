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
#include <openrct2/core/FileStream.h>
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
    std::string testParkPath = TestData::GetParkPath("corrupted_duplicate_entity_indices.sv6");
    auto fs = FileStream(testParkPath, FileMode::open);

    auto& objManager = context->GetObjectManager();
    auto importer = ParkImporter::CreateS6(context->GetObjectRepository());
    auto loadResult = importer->LoadFromStream(&fs, false);
    objManager.LoadObjects(loadResult.RequiredObjects);

    MapAnimations::ClearAll();
    auto& gameState = getGameState();

    // This should not crash because the code should sanitize EntityIndex values before CreateEntityAt is called
    EXPECT_NO_THROW(importer->Import(gameState));
}
