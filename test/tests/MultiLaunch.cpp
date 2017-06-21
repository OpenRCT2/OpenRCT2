#include <string>
#include <gtest/gtest.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/Context.h>
#include <openrct2/core/File.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/OpenRCT2.h>
#include "TestData.h"

extern "C"
{
    #include <openrct2/platform/platform.h>
    #include <openrct2/game.h>
}

using namespace OpenRCT2;

TEST(MultiLaunchTest, all)
{
    std::string path = TestData::GetParkPath("bpb.sv6");

    gOpenRCT2Headless = true;

    core_init();
    for (int i = 0; i < 10; i++)
    {
        auto context = CreateContext();
        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        game_load_sv6_path(path.c_str());

        // Check ride count to check load was successful
        ASSERT_EQ(gRideCount, 134);

        delete context;
    }
    SUCCEED();
}
