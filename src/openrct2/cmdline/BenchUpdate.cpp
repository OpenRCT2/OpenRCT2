/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#    include "../platform/platform.h"

#    include <benchmark/benchmark.h>
#    include <cstdint>
#    include <iterator>
#    include <vector>

using namespace OpenRCT2;

static void BM_update(benchmark::State& state, const std::string& filename)
{
    std::unique_ptr<IContext> context(CreateContext());
    if (context->Initialise())
    {
        if (!filename.empty() && !context->LoadParkFromFile(filename))
        {
            state.SkipWithError("Failed to load file!");
        }

        for (auto _ : state)
        {
            context->GetGameState()->UpdateLogic();
        }
        state.SetItemsProcessed(state.iterations());
    }
    else
    {
        state.SkipWithError("Context initialization failed.");
    }
}

static int cmdline_for_bench_sprite_sort(int argc, const char** argv)
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
        if (platform_file_exists(argv[i]))
        {
            // Register benchmark for sv6 if valid
            benchmark::RegisterBenchmark(argv[i], BM_update, argv[i]);
        }
        else
        {
            argv_for_benchmark.push_back((char*)argv[i]);
        }
    }
    // Update argc with all the changes made
    argc = (int)argv_for_benchmark.size();
    ::benchmark::Initialize(&argc, &argv_for_benchmark[0]);
    if (::benchmark::ReportUnrecognizedArguments(argc, &argv_for_benchmark[0]))
        return -1;

    core_init();
    gOpenRCT2Headless = true;

    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
}

static exitcode_t HandleBenchUpdate(CommandLineArgEnumerator* argEnumerator)
{
    const char** argv = (const char**)argEnumerator->GetArguments() + argEnumerator->GetIndex();
    int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex();
    int32_t result = cmdline_for_bench_sprite_sort(argc, argv);
    if (result < 0)
    {
        return EXITCODE_FAIL;
    }
    return EXITCODE_OK;
}

#else
static exitcode_t HandleBenchUpdate(CommandLineArgEnumerator* argEnumerator)
{
    log_error("Sorry, Google benchmark not enabled in this build");
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
