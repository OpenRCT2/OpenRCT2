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
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/config/Config.h>
#include <openrct2/competitive/CompetitiveModel.h>
#include <openrct2/competitive/CompetitiveProtocol.h>
#include <openrct2/competitive/CompetitiveSession.h>
#include <openrct2/core/Json.hpp>
#include <openrct2/park/ParkFile.h>

#include <chrono>
#include <filesystem>
#include <thread>

using namespace OpenRCT2::Competitive;

TEST(CompetitiveTests, ScenarioHashTakesPriorityOverDisplayMetadata)
{
    ScenarioIdentity lhs{ "forest.sc6", "Forest", "abc", 128, 128, false };
    ScenarioIdentity rhs{ "renamed.sc6", "Renamed", "abc", 256, 256, true };
    EXPECT_TRUE(IsSameScenario(lhs, rhs));

    rhs.contentHash = "different";
    EXPECT_FALSE(IsSameScenario(lhs, rhs));
}

TEST(CompetitiveTests, DeadlineUsesEightScenarioMonthsPerYear)
{
    EXPECT_FALSE(DeadlineReached(159, 20));
    EXPECT_TRUE(DeadlineReached(160, 20));
}

TEST(CompetitiveTests, OfflineAndFinishedParksCannotBeTargeted)
{
    Participant participant{ .id = 1, .online = true };
    EXPECT_TRUE(CanTarget(participant));
    participant.online = false;
    EXPECT_FALSE(CanTarget(participant));
    participant.online = true;
    participant.finished = true;
    EXPECT_FALSE(CanTarget(participant));
}

TEST(CompetitiveTests, DailyPointsPreservePluginFormula)
{
    ParkMetrics metrics{ .guests = 500, .meanHappiness = 204 };
    EXPECT_EQ(CalculateDailyPoints(metrics), 400);
}

TEST(CompetitiveTests, FrozenScoresIgnoreLaterReports)
{
    Score score{ .participantId = 1, .competitiveCash = 20000.00_GBP };
    ParkMetrics frozen{ .localYear = 20, .rating = 900, .guests = 2000, .parkValue = 500000.00_GBP };
    FreezeScore(score, frozen, 20);

    ParkMetrics later{ .localYear = 21, .rating = 999, .guests = 9000, .parkValue = 900000.00_GBP };
    UpdateLiveScore(score, later);
    FreezeScore(score, later, 21);

    EXPECT_EQ(score.rating, 900);
    EXPECT_EQ(score.guests, 2000u);
    EXPECT_EQ(score.frozenAtYear, 20);
}

TEST(CompetitiveTests, EconomyReportsAreIdempotent)
{
    MatchRules rules;
    Score score{ .participantId = 1, .competitiveCash = rules.economy.startingCash };
    EconomyTotals accepted;
    ParkMetrics report{
        .arrivalsGenerated = 10,
        .constructionSpend = 100.00_GBP,
        .rideCustomers = 4,
        .stallCustomers = 2,
    };

    ApplyEconomyDelta(score, accepted, report, rules.economy);
    const auto afterFirstReport = score.competitiveCash;
    ApplyEconomyDelta(score, accepted, report, rules.economy);

    EXPECT_EQ(score.competitiveCash, afterFirstReport);
    EXPECT_EQ(score.lifetimeSpend, 100.00_GBP);
    EXPECT_EQ(score.lifetimeIncome, 40.40_GBP);
}

TEST(CompetitiveTests, WinnerUsesMetricThenPointsThenStableId)
{
    std::vector<Participant> participants = {
        { .id = 10, .online = true },
        { .id = 20, .online = true },
        { .id = 30, .online = true, .forfeited = true },
    };
    std::vector<Score> scores = {
        { .participantId = 10, .points = 100, .guests = 500 },
        { .participantId = 20, .points = 200, .guests = 500 },
        { .participantId = 30, .points = 9999, .guests = 9999 },
    };

    EXPECT_EQ(ChooseWinner(scores, participants, Metric::guests), 20);
    scores[0].points = 200;
    EXPECT_EQ(ChooseWinner(scores, participants, Metric::guests), 10);
}

TEST(CompetitiveTests, MatchProtocolRoundTripsAllAuthoritativeState)
{
    MatchState state;
    state.matchId = "match-one";
    state.name = "Forest Frontiers Competition";
    state.phase = Phase::running;
    state.hostId = 10;
    state.startLocalDay = 1;
    state.scenario = { "forest.sc6", "Forest Frontiers", "012345", 128, 128, false };
    state.participants.push_back(
        { 10, "host-key", "Host Park", Role::host, true, false, false, false, 0, 1, state.scenario });
    state.participants[0].watchHost = "192.0.2.10";
    state.participants[0].watchPort = 12010;
    state.scores.push_back({ .participantId = 10, .points = 123, .competitiveCash = 19000.00_GBP });
    ParkMetrics reportMetrics{ .localDay = 3, .rating = 700, .guests = 300 };
    reportMetrics.openFoodDrinkStalls.push_back({ 7, "Chief Beef" });
    state.reports.push_back({ 10, reportMetrics, {}, 3 });
    state.cooldowns.push_back({ 10, Ability::poison, 2 });
    state.effects.push_back({ 1, Ability::misinformation, 10, 20, -1, true, 1800.00_GBP, 3, 17, 200 });

    const auto parsed = MatchStateFromJson(ToJson(state));
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->matchId, state.matchId);
    EXPECT_EQ(parsed->scenario.contentHash, state.scenario.contentHash);
    EXPECT_EQ(parsed->participants.at(0).identityKey, "host-key");
    EXPECT_EQ(parsed->participants.at(0).watchHost, "192.0.2.10");
    EXPECT_EQ(parsed->participants.at(0).watchPort, 12010);
    EXPECT_EQ(parsed->scores.at(0).points, 123);
    EXPECT_EQ(parsed->reports.at(0).lastScoredDay, 3u);
    EXPECT_EQ(parsed->reports.at(0).metrics.openFoodDrinkStalls.at(0).rideId, 7);
    EXPECT_EQ(parsed->cooldowns.at(0).ability, Ability::poison);
    EXPECT_EQ(parsed->cooldowns.at(0).availableAtDay, 2u);
    EXPECT_EQ(parsed->effects.at(0).endsAtDay, 17u);
}

TEST(CompetitiveTests, HostSessionRestoresFromNativeParkSave)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;
    auto context = OpenRCT2::CreateContext();
    ASSERT_TRUE(context->Initialise());

    const auto sourcePath = TestData::GetParkPath("small_park_with_ferris_wheel.sv6");
    ASSERT_TRUE(OpenRCT2::GetContext()->LoadParkFromFile(sourcePath));
    GameLoadInit();

    auto& session = GetSession();
    session.Stop();
    const auto advertiseBeforeTest = OpenRCT2::Config::Get().network.advertise;
    OpenRCT2::Config::Get().network.advertise = false;
    HostConfiguration configuration;
    configuration.competitionName = "Recovery test";
    configuration.playerName = "Host Park";
    configuration.listenAddress = "127.0.0.1";
    configuration.port = 21756;
    configuration.scenario = GetScenarioIdentityForPath(sourcePath);
    std::string error;
    ASSERT_TRUE(session.StartHost(configuration, error)) << error;
    const auto matchId = session.GetState()->matchId;

    Session spectator;
    JoinConfiguration joinConfiguration;
    joinConfiguration.host = "127.0.0.1";
    joinConfiguration.port = configuration.port;
    joinConfiguration.playerName = "Spectator";
    joinConfiguration.role = Role::spectator;
    ASSERT_TRUE(spectator.Join(joinConfiguration, error)) << error;
    for (int32_t attempt = 0; attempt < 500 && !spectator.IsOnline(); attempt++)
    {
        session.Update();
        spectator.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    ASSERT_TRUE(spectator.IsOnline());
    ASSERT_NE(spectator.GetState(), nullptr);
    EXPECT_EQ(spectator.GetState()->matchId, matchId);
    spectator.Stop();
    session.Update();

    const auto savePath = std::filesystem::temp_directory_path() / "openrct2-competitive-recovery-test.park";
    OpenRCT2::ParkFileExporter exporter;
    exporter.Export(OpenRCT2::getGameState(), savePath.u8string(), OpenRCT2::kParkFileSaveCompressionLevel);
    session.Stop();

    ASSERT_TRUE(OpenRCT2::GetContext()->LoadParkFromFile(savePath.u8string()));
    ASSERT_EQ(session.GetMode(), SessionMode::host);
    ASSERT_NE(session.GetState(), nullptr);
    EXPECT_EQ(session.GetState()->matchId, matchId);
    EXPECT_EQ(session.GetConnectionStatus(), ConnectionStatus::online);

    session.Stop();
    OpenRCT2::Config::Get().network.advertise = advertiseBeforeTest;
    std::filesystem::remove(savePath);
}
