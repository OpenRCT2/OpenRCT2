/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
#include <openrct2/ReplayManager.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/core/File.h>
#include <openrct2/core/FileScanner.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/platform/platform.h>
#include <openrct2/ride/Ride.h>
#include <string>

using namespace OpenRCT2;

class ReplayTests : public testing::Test
{
protected:
    std::vector<std::string> GetReplayFiles()
    {
        std::string basePath = TestData::GetBasePath();
        std::string replayPath = Path::Combine(basePath, "replays");
        std::string replayPathPattern = Path::Combine(replayPath, "*.sv6r");
        std::vector<std::string> files;

        std::unique_ptr<IFileScanner> scanner = std::unique_ptr<IFileScanner>(Path::ScanDirectory(replayPathPattern, true));
        while (scanner->Next())
        {
            files.push_back(scanner->GetPath());
        }

        return files;
    }
};

TEST_F(ReplayTests, all)
{
    std::vector<std::string> replayFiles = GetReplayFiles();
    if (replayFiles.empty())
    {
        SUCCEED();
    }

    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    core_init();
    for (auto&& replayFile : replayFiles)
    {
        auto context = CreateContext();
        bool initialised = context->Initialise();
        ASSERT_TRUE(initialised);

        auto gs = context->GetGameState();
        ASSERT_NE(gs, nullptr);

        IReplayManager* replayManager = context->GetReplayManager();
        ASSERT_NE(replayManager, nullptr);

        bool startedReplay = replayManager->StartPlayback(replayFile);
        ASSERT_TRUE(startedReplay);

        while (replayManager->IsReplaying())
        {
            gs->UpdateLogic();
            ASSERT_TRUE(replayManager->IsPlaybackStateMismatching());
        }
    }
    SUCCEED();
}
