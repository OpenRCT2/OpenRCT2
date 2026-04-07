/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScCompetition.hpp"

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../network/Network.h"
    #include "../../CompetitionScoring.h"
    #include "../../CompetitionState.h"

    #include <algorithm>
    #include <vector>

namespace OpenRCT2::Scripting
{
    static const char* CompetitionStatusToString(CompetitionStatus status)
    {
        switch (status)
        {
            case CompetitionStatus::Idle:
                return "idle";
            case CompetitionStatus::Active:
                return "active";
            case CompetitionStatus::Finished:
                return "finished";
        }
        return "idle";
    }

    static CompetitionMetric MetricFromString(const std::string& str)
    {
        if (str == "parkValue")
            return CompetitionMetric::ParkValue;
        if (str == "guestCount")
            return CompetitionMetric::GuestCount;
        if (str == "parkRating")
            return CompetitionMetric::ParkRating;
        if (str == "rideCount")
            return CompetitionMetric::RideCount;
        if (str == "moneyEarned")
            return CompetitionMetric::MoneyEarned;
        if (str == "custom")
            return CompetitionMetric::Custom;
        return CompetitionMetric::ParkValue;
    }

    // Ticks per in-game day (matches the engine constant).
    static constexpr uint32_t kTicksPerDay = 4096;

    JSValue ScCompetition::status_get(JSContext* ctx, [[maybe_unused]] JSValue thisVal)
    {
        auto& comp = getGameState().Competition;
        return JSFromStdString(ctx, CompetitionStatusToString(comp.Status));
    }

    JSValue ScCompetition::timeRemaining_get(JSContext* ctx, [[maybe_unused]] JSValue thisVal)
    {
        auto& comp = getGameState().Competition;
        if (!comp.IsActive())
            return JS_NewInt32(ctx, 0);
        return JS_NewUint32(ctx, comp.TicksRemaining(getGameState().currentTicks));
    }

    JSValue ScCompetition::leaderboard_get(JSContext* ctx, [[maybe_unused]] JSValue thisVal)
    {
        auto& comp = getGameState().Competition;

        // Collect and sort by rank.
        std::vector<const PlayerScore*> sorted;
        sorted.reserve(comp.Scores.size());
        for (const auto& [_, score] : comp.Scores)
        {
            sorted.push_back(&score);
        }
        std::sort(sorted.begin(), sorted.end(), [](const PlayerScore* a, const PlayerScore* b) { return a->Rank < b->Rank; });

        JSValue arr = JS_NewArray(ctx);
        for (size_t i = 0; i < sorted.size(); i++)
        {
            const auto* entry = sorted[i];
            JSValue obj = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, obj, "playerId", JS_NewInt32(ctx, entry->PlayerId));
            JS_SetPropertyStr(ctx, obj, "playerName", JSFromStdString(ctx, entry->PlayerName));
            JS_SetPropertyStr(ctx, obj, "score", JS_NewInt64(ctx, entry->Score));
            JS_SetPropertyStr(ctx, obj, "rank", JS_NewInt32(ctx, entry->Rank));
            JS_SetPropertyInt64(ctx, arr, static_cast<int64_t>(i), obj);
        }
        return arr;
    }

    JSValue ScCompetition::start(JSContext* ctx, [[maybe_unused]] JSValue thisVal, int argc, JSValue* argv)
    {
        if (argc < 1 || !JS_IsObject(argv[0]))
        {
            JS_ThrowTypeError(ctx, "Expected options object");
            return JS_EXCEPTION;
        }

        auto& gameState = getGameState();
        auto& comp = gameState.Competition;

        if (comp.IsActive())
        {
            JS_ThrowPlainError(ctx, "A competition is already active");
            return JS_EXCEPTION;
        }

        JSValue options = argv[0];

        // Read required fields.
        auto name = JSToStdString(ctx, options, "name");
        auto metricStr = AsOrDefault(ctx, options, "metric", "parkValue");
        auto durationDays = JSToOptionalInt(ctx, options, "durationDays");
        auto updateInterval = JSToOptionalUint(ctx, options, "updateIntervalTicks");

        if (name.empty())
        {
            JS_ThrowPlainError(ctx, "Competition name is required");
            return JS_EXCEPTION;
        }

        if (!durationDays.has_value() || durationDays.value() <= 0)
        {
            JS_ThrowPlainError(ctx, "durationDays must be a positive integer");
            return JS_EXCEPTION;
        }

        comp.Name = std::move(name);
        comp.Metric = MetricFromString(metricStr);
        comp.DurationTicks = static_cast<uint32_t>(durationDays.value()) * kTicksPerDay;
        comp.StartTick = gameState.currentTicks;
        comp.UpdateIntervalTicks = updateInterval.value_or(100);
        comp.Status = CompetitionStatus::Active;
        comp.Scores.clear();

        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        hookEngine.Call(HookType::competitionStart, true);

        return JS_UNDEFINED;
    }

    JSValue ScCompetition::stop(
        JSContext* ctx, [[maybe_unused]] JSValue thisVal, [[maybe_unused]] int argc, [[maybe_unused]] JSValue* argv)
    {
        auto& comp = getGameState().Competition;

        if (!comp.IsActive())
        {
            JS_ThrowPlainError(ctx, "No active competition to stop");
            return JS_EXCEPTION;
        }

        comp.Status = CompetitionStatus::Finished;
        comp.Scores.clear();
        comp.DurationTicks = 0;
        comp.StartTick = 0;

        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        hookEngine.Call(HookType::competitionEnd, true);
        Network::SendCompetitionUpdate();

        return JS_UNDEFINED;
    }

    JSValue ScCompetition::setPlayerScore(JSContext* ctx, [[maybe_unused]] JSValue thisVal, int argc, JSValue* argv)
    {
        if (argc < 2)
        {
            JS_ThrowTypeError(ctx, "Expected playerId and score arguments");
            return JS_EXCEPTION;
        }

        auto& comp = getGameState().Competition;

        if (comp.Metric != CompetitionMetric::Custom)
        {
            JS_ThrowPlainError(ctx, "setPlayerScore is only valid when metric is 'custom'");
            return JS_EXCEPTION;
        }

        JS_UNPACK_INT32(playerId, ctx, argv[0]);
        JS_UNPACK_INT64(score, ctx, argv[1]);

        auto id = static_cast<uint8_t>(playerId);
        auto& entry = comp.Scores[id];
        entry.PlayerId = id;
        entry.Score = score;

        return JS_UNDEFINED;
    }

    JSValue ScCompetition::broadcastLeaderboard(
        [[maybe_unused]] JSContext* ctx, [[maybe_unused]] JSValue thisVal, [[maybe_unused]] int argc,
        [[maybe_unused]] JSValue* argv)
    {
        auto& comp = getGameState().Competition;
        if (!comp.IsActive() && comp.Status != CompetitionStatus::Finished)
        {
            return JS_UNDEFINED;
        }

        // Broadcast state to connected clients, then fire the local hook.
        Network::SendCompetitionUpdate();

        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        hookEngine.Call(HookType::competitionTick, true);

        return JS_UNDEFINED;
    }

    JSValue ScCompetition::New(JSContext* ctx)
    {
        return MakeWithOpaque(ctx, nullptr);
    }

    void ScCompetition::Register(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("status", ScCompetition::status_get, nullptr),
            JS_CGETSET_DEF("timeRemaining", ScCompetition::timeRemaining_get, nullptr),
            JS_CGETSET_DEF("leaderboard", ScCompetition::leaderboard_get, nullptr),

            JS_CFUNC_DEF("start", 1, ScCompetition::start),
            JS_CFUNC_DEF("stop", 0, ScCompetition::stop),
            JS_CFUNC_DEF("setPlayerScore", 2, ScCompetition::setPlayerScore),
            JS_CFUNC_DEF("broadcastLeaderboard", 0, ScCompetition::broadcastLeaderboard),
        };
        RegisterBase(ctx, "Competition", nullptr, funcs);
    }

} // namespace OpenRCT2::Scripting

#endif
