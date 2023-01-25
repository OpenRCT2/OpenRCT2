/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include <openrct2/platform/Platform.h>
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
        if (isalnum(static_cast<unsigned char>(c)))
            res += c;
    }
    return res;
}

static std::vector<ReplayTestData> GetReplayFiles()
{
    std::vector<ReplayTestData> res;
    std::string basePath = TestData::GetBasePath();
    std::string replayPath = Path::Combine(basePath, u8"replays");
    std::string replayPathPattern = Path::Combine(replayPath, u8"*.parkrep");
    std::vector<std::string> files;

    auto scanner = Path::ScanDirectory(replayPathPattern, true);
    while (scanner->Next())
    {
        ReplayTestData test;
        test.name = sanitizeTestName(scanner->GetFileInfo()->Name);
        test.filePath = scanner->GetPath();
        res.push_back(std::move(test));
    }
    return res;
}

class ReplayTests : public testing::TestWithParam<ReplayTestData>
{
protected:
};

TEST_P(ReplayTests, RunReplay)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;
    Platform::CoreInit();

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
        if (replayManager->IsPlaybackStateMismatching())
            break;
    }
    ASSERT_FALSE(replayManager->IsReplaying());
    ASSERT_FALSE(replayManager->IsPlaybackStateMismatching());
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

INSTANTIATE_TEST_SUITE_P(Replay, ReplayTests, testing::ValuesIn(GetReplayFiles()), PrintReplayParameter());
