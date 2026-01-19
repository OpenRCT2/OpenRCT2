/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestData.h"

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/core/String.hpp>
#include <openrct2/ride/Ride.h>
#include <string>

using namespace OpenRCT2;

constexpr int32_t updatesToTest = 10;

TEST(MultiLaunchTest, all)
{
    std::string path = TestData::GetParkPath("bpb.sv6");

    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    for (int i = 0; i < 3; i++)
    {
        auto context = CreateContext();
        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        GetContext()->LoadParkFromFile(path);
        GameLoadInit();

        // Check ride count to check load was successful
        ASSERT_EQ(RideGetCount(), 134);

        auto& date = getGameState().date;
        // NOTE: This value is saved in the SV6 file, after the import this will be the current state.
        // In case the save file gets replaced this needs to be adjusted.
        ASSERT_EQ(date.GetMonthTicks(), 0x1e98);

        for (int j = 0; j < updatesToTest; j++)
        {
            gameStateUpdateLogic();
        }

        ASSERT_EQ(date.GetMonthTicks(), 7862 + updatesToTest);

        // Check ride count again
        ASSERT_EQ(RideGetCount(), 134);
    }
    SUCCEED();
}
