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

TEST(CompetitiveTests, FrozenScoresIgnoreLaterReports)
{
    Score score{ .participantId = 1, .cash = 20000.00_GBP };
    ParkMetrics frozen{
        .localYear = 20, .rating = 900, .guests = 2000, .parkValue = 500000.00_GBP, .cash = 75000.00_GBP
    };
    FreezeScore(score, frozen, 20);

    ParkMetrics later{ .localYear = 21, .rating = 999, .guests = 9000, .parkValue = 900000.00_GBP };
    UpdateLiveScore(score, later);
    FreezeScore(score, later, 21);

    EXPECT_EQ(score.rating, 900);
    EXPECT_EQ(score.guests, 2000u);
    EXPECT_EQ(score.frozenAtYear, 20);
}

TEST(CompetitiveTests, ScoreTracksActualScenarioCash)
{
    Score score{ .participantId = 1 };
    ParkMetrics report{ .cash = 12345.60_GBP };

    UpdateLiveScore(score, report);

    EXPECT_EQ(score.cash, 12345.60_GBP);
    EXPECT_EQ(GetMetricValue(score, Metric::cash), 12345.60_GBP);
}

TEST(CompetitiveTests, WeightedScoreRanksAndBreaksTiesByStableId)
{
    std::vector<Participant> participants = {
        { .id = 10, .online = true },
        { .id = 20, .online = true },
        { .id = 30, .online = true, .forfeited = true },
    };
    std::vector<Score> scores = {
        { .participantId = 10, .guests = 400 },
        { .participantId = 20, .guests = 500 },
        { .participantId = 30, .guests = 9999 }, // forfeited: excluded from ranking and normalisation
    };

    MatchRules rules;
    rules.metricWeights = { 0, 0, 100, 0, 0 }; // 100% guest count

    // 20 leads on guests; the forfeited park's larger count does not count.
    EXPECT_EQ(ChooseWinner(scores, participants, rules), 20u);
    EXPECT_EQ(ComputeCompositeScore(20, scores, participants, rules), 1000);
    EXPECT_EQ(ComputeCompositeScore(10, scores, participants, rules), 800); // 400/500 * 1000

    // Exact tie on the only weighted metric -> lower participant id wins.
    scores[0].guests = 500;
    EXPECT_EQ(ChooseWinner(scores, participants, rules), 10u);
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
    state.rules.toiletBomber.cost = 4321.00_GBP;
    state.rules.agitator.potency = 17;
    state.rules.saboteur.cooldownDays = 300;
    state.rules.hitman.durationDays = 80;
    state.rules.vandal.usesPerYear = 5;
    state.rules.researchSabotage.durationDays = 30;
    state.rules.unionDisruption.cost = 275.00_GBP;
    state.rules.karens.potency = 25;
    state.rules.stoners.usesPerYear = 3;
    state.rules.maxRidesPerType = 3;
    state.rules.maxStallsPerType = 5;
    state.rules.metricWeights = { 20, 40, 40, 0, 0 };
    state.rules.realTimeLimitSeconds = 900;
    state.rules.anonymousAttacks = true;
    state.rules.customDesignsOnly = true;
    state.liveSecondsElapsed = 120;
    state.participants.push_back(
        { 10, "host-key", "Host Park", Role::host, true, false, false, false, 0, 1, state.scenario });
    state.participants[0].watchHost = "192.0.2.10";
    state.participants[0].watchPort = 12010;
    state.scores.push_back({ .participantId = 10, .happiness = 180, .cash = 19000.00_GBP });
    ParkMetrics reportMetrics{ .localDay = 3, .rating = 700, .guests = 300 };
    reportMetrics.openFoodDrinkStalls.push_back({ 7, "Chief Beef" });
    reportMetrics.openToilets.push_back({ 8, "Restroom 1" });
    reportMetrics.openRides.push_back({ 9, "Wooden Roller Coaster 1" });
    state.reports.push_back({ 10, reportMetrics, 3 });
    state.cooldowns.push_back({ 10, Ability::poison, 2 });
    state.usages.push_back({ 10, Ability::vandal, 2, 3 });
    state.effects.push_back({ 1, Ability::misinformation, 10, 20, -1, 42, true, 1800.00_GBP, 3, 17, 200 });

    const auto parsed = MatchStateFromJson(ToJson(state));
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->matchId, state.matchId);
    EXPECT_EQ(parsed->scenario.contentHash, state.scenario.contentHash);
    EXPECT_EQ(parsed->participants.at(0).identityKey, "host-key");
    EXPECT_EQ(parsed->participants.at(0).watchHost, "192.0.2.10");
    EXPECT_EQ(parsed->participants.at(0).watchPort, 12010);
    EXPECT_EQ(parsed->scores.at(0).happiness, 180);
    EXPECT_EQ(parsed->scores.at(0).cash, 19000.00_GBP);
    EXPECT_EQ(parsed->rules.metricWeights, state.rules.metricWeights);
    EXPECT_EQ(parsed->rules.realTimeLimitSeconds, 900u);
    EXPECT_TRUE(parsed->rules.anonymousAttacks);
    EXPECT_TRUE(parsed->rules.customDesignsOnly);
    EXPECT_EQ(parsed->liveSecondsElapsed, 120u);
    EXPECT_EQ(parsed->reports.at(0).lastScoredDay, 3u);
    EXPECT_EQ(parsed->reports.at(0).metrics.openFoodDrinkStalls.at(0).rideId, 7);
    EXPECT_EQ(parsed->reports.at(0).metrics.openToilets.at(0).rideId, 8);
    EXPECT_EQ(parsed->reports.at(0).metrics.openRides.at(0).rideId, 9);
    EXPECT_EQ(parsed->rules.toiletBomber.cost, 4321.00_GBP);
    EXPECT_EQ(parsed->rules.agitator.potency, 17);
    EXPECT_EQ(parsed->rules.saboteur.cooldownDays, 300);
    EXPECT_EQ(parsed->rules.hitman.durationDays, 80);
    EXPECT_EQ(parsed->rules.vandal.usesPerYear, 5);
    EXPECT_EQ(parsed->rules.researchSabotage.durationDays, 30);
    EXPECT_EQ(parsed->rules.unionDisruption.cost, 275.00_GBP);
    EXPECT_EQ(parsed->rules.karens.potency, 25);
    EXPECT_EQ(parsed->rules.stoners.usesPerYear, 3);
    EXPECT_EQ(parsed->rules.maxRidesPerType, 3);
    EXPECT_EQ(parsed->rules.maxStallsPerType, 5);
    EXPECT_EQ(parsed->cooldowns.at(0).ability, Ability::poison);
    EXPECT_EQ(parsed->cooldowns.at(0).availableAtDay, 2u);
    EXPECT_EQ(parsed->usages.at(0).ability, Ability::vandal);
    EXPECT_EQ(parsed->usages.at(0).year, 2);
    EXPECT_EQ(parsed->usages.at(0).used, 3);
    EXPECT_EQ(parsed->effects.at(0).endsAtDay, 17u);
}

TEST(CompetitiveTests, RideTypeLimitsDefaultToUnlimitedAndRejectOutOfRange)
{
    // Absent fields (e.g. from an older host) default to unlimited.
    auto json = ToJson(MatchRules{});
    json.erase("maxRidesPerType");
    json.erase("maxStallsPerType");
    const auto parsed = MatchRulesFromJson(json);
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->maxRidesPerType, 0);
    EXPECT_EQ(parsed->maxStallsPerType, 0);

    auto tooLarge = ToJson(MatchRules{});
    tooLarge["maxRidesPerType"] = 5000;
    EXPECT_FALSE(MatchRulesFromJson(tooLarge).has_value());
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

TEST(CompetitiveTests, DeliberateSpectatorLeaveIsClean)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;
    auto context = OpenRCT2::CreateContext();
    ASSERT_TRUE(context->Initialise());

    const auto sourcePath = TestData::GetParkPath("small_park_with_ferris_wheel.sv6");
    ASSERT_TRUE(OpenRCT2::GetContext()->LoadParkFromFile(sourcePath));
    GameLoadInit();

    auto& host = GetSession();
    host.Stop();
    const auto advertiseBeforeTest = OpenRCT2::Config::Get().network.advertise;
    OpenRCT2::Config::Get().network.advertise = false;

    HostConfiguration configuration;
    configuration.competitionName = "Leave test";
    configuration.playerName = "Host Park";
    configuration.listenAddress = "127.0.0.1";
    configuration.port = 21757;
    configuration.scenario = GetScenarioIdentityForPath(sourcePath);
    std::string error;
    ASSERT_TRUE(host.StartHost(configuration, error)) << error;

    Session spectator;
    JoinConfiguration joinConfiguration;
    joinConfiguration.host = "127.0.0.1";
    joinConfiguration.port = configuration.port;
    joinConfiguration.playerName = "Watcher";
    joinConfiguration.role = Role::spectator;
    ASSERT_TRUE(spectator.Join(joinConfiguration, error)) << error;
    for (int32_t attempt = 0; attempt < 500 && !spectator.IsOnline(); attempt++)
    {
        host.Update();
        spectator.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    ASSERT_TRUE(spectator.IsOnline());
    EXPECT_EQ(host.GetState()->participants.size(), 2u);

    // A deliberate leave sends the coordinator a competitiveLeave; the host drops the
    // spectator seat without disturbing the competing seats.
    spectator.Stop(true);
    for (int32_t attempt = 0; attempt < 200 && host.GetState()->participants.size() > 1; attempt++)
    {
        host.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT_EQ(host.GetState()->participants.size(), 1u);
    EXPECT_EQ(host.GetState()->phase, Phase::lobby);

    host.Stop();
    OpenRCT2::Config::Get().network.advertise = advertiseBeforeTest;
}

TEST(CompetitiveTests, HostLeaveTellsClientsAndTearsDownCleanly)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;
    auto context = OpenRCT2::CreateContext();
    ASSERT_TRUE(context->Initialise());

    const auto sourcePath = TestData::GetParkPath("small_park_with_ferris_wheel.sv6");
    ASSERT_TRUE(OpenRCT2::GetContext()->LoadParkFromFile(sourcePath));
    GameLoadInit();

    auto& host = GetSession();
    host.Stop();
    const auto advertiseBeforeTest = OpenRCT2::Config::Get().network.advertise;
    OpenRCT2::Config::Get().network.advertise = false;

    HostConfiguration configuration;
    configuration.competitionName = "Host leave test";
    configuration.playerName = "Host Park";
    configuration.listenAddress = "127.0.0.1";
    configuration.port = 21759;
    configuration.scenario = GetScenarioIdentityForPath(sourcePath);
    std::string error;
    ASSERT_TRUE(host.StartHost(configuration, error)) << error;

    Session spectator;
    JoinConfiguration joinConfiguration;
    joinConfiguration.host = "127.0.0.1";
    joinConfiguration.port = configuration.port;
    joinConfiguration.playerName = "Watcher";
    joinConfiguration.role = Role::spectator;
    ASSERT_TRUE(spectator.Join(joinConfiguration, error)) << error;
    for (int32_t attempt = 0; attempt < 500 && !spectator.IsOnline(); attempt++)
    {
        host.Update();
        spectator.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    ASSERT_TRUE(spectator.IsOnline());

    // The host leaves with a connected client - this must broadcast competitiveLeave and tear
    // the host down without crashing.
    host.Stop(true);
    EXPECT_EQ(host.GetState(), nullptr);

    // The client picks up the host-closed message on its next updates and drops its own session.
    for (int32_t attempt = 0; attempt < 300 && spectator.GetState() != nullptr; attempt++)
    {
        spectator.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT_EQ(spectator.GetState(), nullptr);

    spectator.Stop();
    OpenRCT2::Config::Get().network.advertise = advertiseBeforeTest;
}
