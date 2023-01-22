/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CommandLine.hpp"

#ifdef USE_BENCHMARK

#    include "../Context.h"
#    include "../GameState.h"
#    include "../OpenRCT2.h"
#    include "../core/File.h"
#    include "../platform/Platform.h"

#    include <benchmark/benchmark.h>
#    include <cstdint>
#    include <iterator>
#    include <numeric>
#    include <vector>

namespace OpenRCT2
{

    static void BM_update(benchmark::State& state, const std::string& filename)
    {
        std::unique_ptr<IContext> context(CreateContext());
        if (context->Initialise())
        {
            if (!filename.empty() && !context->LoadParkFromFile(filename))
            {
                state.SkipWithError("Failed to load file!");
            }

            std::vector<LogicTimings> timings(1);
            timings.reserve(100);
            int currentTimingIdx = 0;
            for (auto _ : state)
            {
                if (timings[currentTimingIdx].CurrentIdx == (LOGIC_UPDATE_MEASUREMENTS_COUNT - 1))
                {
                    timings.resize(timings.size() + 1);
                    currentTimingIdx++;
                }
                LogicTimings* timingToUse = &timings[currentTimingIdx];
                context->GetGameState()->UpdateLogic(timingToUse);
            }
            state.SetItemsProcessed(state.iterations());
            auto accumulator = [timings](LogicTimePart part) -> double {
                std::chrono::duration<double> timesum;
                for (const auto& timing : timings)
                {
                    timesum = std::accumulate(
                        timing.TimingInfo.at(part).begin(), timing.TimingInfo.at(part).end(), std::chrono::duration<double>());
                }
                return std::chrono::duration_cast<std::chrono::milliseconds>(timesum).count();
            };
            state.counters["NetworkUpdateAcc_ms"] = accumulator(LogicTimePart::NetworkUpdate);
            state.counters["DateAcc_ms"] = accumulator(LogicTimePart::Date);
            state.counters["ScenarioAcc_ms"] = accumulator(LogicTimePart::Scenario);
            state.counters["ClimateAcc_ms"] = accumulator(LogicTimePart::Climate);
            state.counters["MapTilesAcc_ms"] = accumulator(LogicTimePart::MapTiles);
            state.counters["MapStashProvisionalElementsAcc_ms"] = accumulator(LogicTimePart::MapStashProvisionalElements);
            state.counters["MapPathWideFlagsAcc_ms"] = accumulator(LogicTimePart::MapPathWideFlags);
            state.counters["PeepAcc_ms"] = accumulator(LogicTimePart::Peep);
            state.counters["MapRestoreProvisionalElementsAcc_ms"] = accumulator(LogicTimePart::MapRestoreProvisionalElements);
            state.counters["VehicleAcc_ms"] = accumulator(LogicTimePart::Vehicle);
            state.counters["MiscAcc_ms"] = accumulator(LogicTimePart::Misc);
            state.counters["RideAcc_ms"] = accumulator(LogicTimePart::Ride);
            state.counters["ParkAcc_ms"] = accumulator(LogicTimePart::Park);
            state.counters["ResearchAcc_ms"] = accumulator(LogicTimePart::Research);
            state.counters["RideRatingsAcc_ms"] = accumulator(LogicTimePart::RideRatings);
            state.counters["RideMeasurmentsAcc_ms"] = accumulator(LogicTimePart::RideMeasurments);
            state.counters["NewsAcc_ms"] = accumulator(LogicTimePart::News);
            state.counters["MapAnimationAcc_ms"] = accumulator(LogicTimePart::MapAnimation);
            state.counters["SoundsAcc_ms"] = accumulator(LogicTimePart::Sounds);
            state.counters["GameActionsAcc_ms"] = accumulator(LogicTimePart::GameActions);
            state.counters["NetworkFlushAcc_ms"] = accumulator(LogicTimePart::NetworkFlush);
            state.counters["ScriptsAcc_ms"] = accumulator(LogicTimePart::Scripts);
        }
        else
        {
            state.SkipWithError("Context initialization failed.");
        }
    }

    static int CmdlineForBenchSpriteSort(int argc, const char* const* argv)
    {
        // Add a baseline test on an empty park
        benchmark::RegisterBenchmark("baseline", BM_update, std::string{});

        // Google benchmark does stuff to argv. It doesn't modify the pointees,
        // but it wants to reorder the pointers, so present a copy of them.
        std::vector<char*> argv_for_benchmark;

        // argv[0] is expected to contain the binary name. It's only for logging purposes, don't bother.
        argv_for_benchmark.push_back(nullptr);

        // Extract file names from argument list. If there is no such file, consider it benchmark option.
        for (int i = 0; i < argc; i++)
        {
            if (File::Exists(argv[i]))
            {
                // Register benchmark for sv6 if valid
                benchmark::RegisterBenchmark(argv[i], BM_update, argv[i]);
            }
            else
            {
                argv_for_benchmark.push_back(const_cast<char*>(argv[i]));
            }
        }
        // Update argc with all the changes made
        argc = static_cast<int>(argv_for_benchmark.size());
        ::benchmark::Initialize(&argc, &argv_for_benchmark[0]);
        if (::benchmark::ReportUnrecognizedArguments(argc, &argv_for_benchmark[0]))
            return -1;

        Platform::CoreInit();
        gOpenRCT2Headless = true;

        ::benchmark::RunSpecifiedBenchmarks();
        return 0;
    }

    static exitcode_t HandleBenchUpdate(CommandLineArgEnumerator* argEnumerator)
    {
        const char* const* argv = static_cast<const char* const*>(argEnumerator->GetArguments()) + argEnumerator->GetIndex();
        int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex();
        int32_t result = CmdlineForBenchSpriteSort(argc, argv);
        if (result < 0)
        {
            return EXITCODE_FAIL;
        }
        return EXITCODE_OK;
    }

#else
static exitcode_t HandleBenchUpdate(CommandLineArgEnumerator* argEnumerator)
{
    LOG_ERROR("Sorry, Google benchmark not enabled in this build");
    return EXITCODE_FAIL;
}
#endif // USE_BENCHMARK

    const CommandLineCommand CommandLine::BenchUpdateCommands[]{
#ifdef USE_BENCHMARK
        DefineCommand(
            "",
            "<file>... [--benchmark_list_tests={true|false}] [--benchmark_filter=<regex>] [--benchmark_min_time=<min_time>] "
            "[--benchmark_repetitions=<num_repetitions>] [--benchmark_report_aggregates_only={true|false}] "
            "[--benchmark_format=<console|json|csv>] [--benchmark_out=<filename>] [--benchmark_out_format=<json|console|csv>] "
            "[--benchmark_color={auto|true|false}] [--benchmark_counters_tabular={true|false}] [--v=<verbosity>]",
            nullptr, HandleBenchUpdate),
        CommandTableEnd
#else
    DefineCommand("", "*** SORRY NOT ENABLED IN THIS BUILD ***", nullptr, HandleBenchUpdate), CommandTableEnd
#endif // USE_BENCHMARK
    };
}
