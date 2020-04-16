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

struct ReplayTestData
{
    std::string name;
    std::string filePath;
};

// NOTE: gtests expects the name to have no special characters.
static std::string sanitizeTestName(const std::string& name)
{
    std::string nameOnly = Path::GetFileNameWithoutExtension(name);
    std::string res;
    for (char c : nameOnly)
    {
        if (isalnum(c))
            res += c;
    }
    return res;
}

static std::vector<ReplayTestData> GetReplayFiles()
{
    std::vector<ReplayTestData> res;
    std::string basePath = TestData::GetBasePath();
    std::string replayPath = Path::Combine(basePath, "replays");
    std::string replayPathPattern = Path::Combine(replayPath, "*.sv6r");
    std::vector<std::string> files;

    std::unique_ptr<IFileScanner> scanner = std::unique_ptr<IFileScanner>(Path::ScanDirectory(replayPathPattern, true));
    while (scanner->Next())
    {
        ReplayTestData test;
        test.name = sanitizeTestName(scanner->GetFileInfo()->Name);
        test.filePath = scanner->GetPath();
        res.push_back(test);
    }
    return res;
}

class ReplayTests : public testing::TestWithParam<ReplayTestData>
{
protected:
};

TEST_P(ReplayTests, RunReplay)
{
#ifdef PLATFORM_32BIT
    log_warning("Replay Tests have not been performed. OpenRCT2/OpenRCT2#11279.");
    return;
#else
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;
    core_init();

    auto testData = GetParam();
    auto replayFile = testData.filePath;

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
        ASSERT_TRUE(replayManager->IsPlaybackStateMismatching() == false);
    }
#endif
}

static void PrintTo(const ReplayTestData& testData, std::ostream* os)
{
    *os << testData.filePath;
}

struct PrintReplayParameter
{
    template<class ParamType> std::string operator()(const testing::TestParamInfo<ParamType>& info) const
    {
        auto data = static_cast<ReplayTestData>(info.param);
        return data.name;
    }
};

INSTANTIATE_TEST_CASE_P(Replay, ReplayTests, testing::ValuesIn(GetReplayFiles()), PrintReplayParameter());
