/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestData.h"

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/core/FileSystem.hpp>
#include <openrct2/core/Guard.hpp>
#include <openrct2/rct12/ScenarioPatcher.h>

/* Test that all JSONs are with the expected formatting, otherwise the fetcher will abort
    NOTE: This will *not* test that it actually applies the patch, due to the scenarios
          not being available on the CI environment.
*/
TEST(FetchAndApplyScenarioPatch, expected_json_format)
{
    // Needs to be headless and without graphics not to prompt for RCT2 path
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;
    auto context = OpenRCT2::CreateContext();
    bool initialised = context->Initialise();
    ASSERT_TRUE(initialised);

    auto env = context->GetPlatformEnvironment();
    auto scenarioPatches = env->GetDirectoryPath(OpenRCT2::DIRBASE::OPENRCT2, OpenRCT2::DIRID::SCENARIO_PATCHES);

    std::error_code ec;
    RCT12::SetDryRun(true);
    Guard::SetAssertBehaviour(ASSERT_BEHAVIOUR::ABORT);
    static const u8string dummySHA;
    for (const fs::directory_entry& entry : fs::directory_iterator(scenarioPatches, ec))
    {
        auto path = entry.path().u8string();
        if (String::EndsWith(path, ".parkpatch"))
        {
            RCT12::ApplyScenarioPatch(path, dummySHA, true);
        }
    }
    SUCCEED();
}
