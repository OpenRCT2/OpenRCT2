/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "TestData.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/fireworks/FireworksSequence.h>
#include <openrct2/object/FireworkObject.h>
#include <openrct2/object/ObjectFactory.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>

using namespace OpenRCT2::Fireworks;
class FireworkHandlerMock : public IFireworkHandler
{
public:
    MOCK_METHOD(void, OnEvent, (const Event& e), (override));
    MOCK_METHOD(void, OnUpdate, (), (override));
    MOCK_METHOD(void, OnMusicLaunch, (const std::string& musicId), (override));
    MOCK_METHOD(void, OnReset, (), (override));
};

class FireworkTest : public ::testing::Test
{
protected:
    virtual void SetUp() override
    {
        // set the handler for all sequences
        _sequence0.SetHandler(_fireworkHandler);
        _sequence1.SetHandler(_fireworkHandler);
        _sequence2.SetHandler(_fireworkHandler);
        _sequence3.SetHandler(_fireworkHandler);
    }
    FireworksSequence _sequence0;
    FireworksSequence _sequence1;
    FireworksSequence _sequence2;
    FireworksSequence _sequence3;

    FireworkHandlerMock _fireworkHandler;

    static std::string GetFireworksJsonPath()
    {
        return Path::Combine(TestData::GetBasePath(), u8"fireworks/firework_test.json");
    }
};

TEST_F(FireworkTest, TestSequence)
{
    // add some spawners to the sequence0
    auto spawner0 = _sequence0.AddSpawner({ 0, 0 });
    auto spawner1 = _sequence0.AddSpawner({ 1, 0 });
    auto spawner3 = _sequence0.AddSpawner({ 0, 1 });
    _sequence0.AddSpawner({ 1, 1 });

    // remove a spawner
    _sequence0.RemoveSpawner({ 1, 1 });

    // add a new one
    auto spawner2 = _sequence0.AddSpawner({ 10, 10 });

    // add some events
    std::string objectId = "firework_test";
    auto e0 = _sequence0.AddEvent(objectId, spawner0, 10, 0);
    auto e1 = _sequence0.AddEvent(objectId, spawner0, 8, 7);
    auto e2 = _sequence0.AddEvent(objectId, spawner1, 8, 4);
    auto e3 = _sequence0.AddEvent(objectId, spawner2, 8, 10);
    auto e4 = _sequence0.AddEvent(objectId, spawner3, 8, 15);

    // remove an event
    _sequence0.RemoveEvent(15, spawner3);

    // set the background music string id
    std::string musicId = "test_music";
    _sequence0.BackgroundMusic = musicId;

    // expect the call to onMusicLaunch
    EXPECT_CALL(_fireworkHandler, OnMusicLaunch(musicId)).Times(1);

    // start the sequence0
    _sequence0.Start();

    // check the events in this sequence
    {
        ::testing::InSequence seq;
        EXPECT_CALL(_fireworkHandler, OnEvent(e0)).Times(1);
        EXPECT_CALL(_fireworkHandler, OnEvent(e2)).Times(1);
        EXPECT_CALL(_fireworkHandler, OnEvent(e1)).Times(1);
        EXPECT_CALL(_fireworkHandler, OnEvent(e3)).Times(1);
        EXPECT_CALL(_fireworkHandler, OnEvent(e4)).Times(0);

        // test the turtle speed
        for (int i = 0; i < 20; i++)
        {
            _sequence0.Update(1);
        }
    }
}

TEST_F(FireworkTest, TestFireworkObject)
{
    // create context
    auto context = OpenRCT2::CreateContext();

    // create object repository
    auto objRepository = CreateObjectRepository(context->GetPlatformEnvironment());

    // load the object from json file
    std::string filepath = GetFireworksJsonPath();
    auto fireworkObjectPtr = ObjectFactory::CreateObjectFromJsonFile(*objRepository, filepath, true);
    auto fireworkObject = reinterpret_cast<FireworkObject*>(fireworkObjectPtr.get());

    ASSERT_NE(fireworkObject, nullptr);

    auto firework = fireworkObject->GetFirework();
    EXPECT_EQ(firework.Height, 4);
    EXPECT_EQ(firework.Width, 4);
    EXPECT_EQ(firework.SoundId, "firework_sound");
    EXPECT_EQ(firework.UseRemap1, false);
    EXPECT_EQ(firework.UseRemap2, false);
    EXPECT_EQ(firework.UseRemap3, false);

    EXPECT_EQ(fireworkObject->GetNumImages(), static_cast<uint32_t>(4));
}
