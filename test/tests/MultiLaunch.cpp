#include <string>
#include <gtest/gtest.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/Context.h>
#include <openrct2/core/File.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include "TestData.h"

#include <openrct2/platform/platform.h>
#include <openrct2/game.h>

using namespace OpenRCT2;

TEST(MultiLaunchTest, all)
{
    std::string path = TestData::GetParkPath("bpb.sv6");

    gOpenRCT2Headless = true;

    core_init();
    for (int i = 0; i < 3; i++)
    {
        auto context = CreateContext();
        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        ParkLoadResult * plr = load_from_sv6(path.c_str());

        ASSERT_EQ(ParkLoadResult_GetError(plr), PARK_LOAD_ERROR_OK);
        ParkLoadResult_Delete(plr);

        game_load_init();

        // Check ride count to check load was successful
        ASSERT_EQ(gRideCount, 134);

        for (int j = 0; j < 10; j++)
        {
            game_logic_update();
        }

        // Check ride count again
        ASSERT_EQ(gRideCount, 134);

        delete context;
    }
    SUCCEED();
}
