/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CommandLine.hpp"

#ifdef USE_BENCHMARK

#    include "../Context.h"
#    include "../Game.h"
#    include "../Intro.h"
#    include "../OpenRCT2.h"
#    include "../audio/audio.h"
#    include "../core/Console.hpp"
#    include "../core/Imaging.h"
#    include "../drawing/Drawing.h"
#    include "../interface/Viewport.h"
#    include "../localisation/Localisation.h"
#    include "../paint/Paint.h"
#    include "../platform/platform.h"
#    include "../util/Util.h"
#    include "../world/Climate.h"
#    include "../world/Map.h"
#    include "../world/Park.h"
#    include "../world/Surface.h"

#    include <benchmark/benchmark.h>
#    include <cstdint>
#    include <iterator>
#    include <vector>

#include "../network/network.h"
#include "../world/Sprite.h"
#include "../GameState.h"


#include <cstdlib>
#include <chrono>
#include <memory>

using namespace OpenRCT2;

static void BM_gamelogic(benchmark::State& state, IContext* context)
{
    int microseconds = state.range(0);
    std::chrono::duration<double, std::micro> sleep_duration{
        static_cast<double>(microseconds) };
    auto start = std::chrono::high_resolution_clock::now();

    for (auto _ : state)
        context->GetGameState()->UpdateLogic();

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    state.SetIterationTime(elapsed_seconds.count());
}

static exitcode_t HandleBenchLogic(CommandLineArgEnumerator* argEnumerator)
{
    const char** argv = (const char**)argEnumerator->GetArguments() + argEnumerator->GetIndex();
    int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex();

    if (argc < 1)
    {
        Console::Error::WriteLine("Missing arguments <sv6-file>");
        return EXITCODE_FAIL;
    }

    core_init();

    const char* inputPath = argv[0];

    gOpenRCT2Headless = true;

#    ifndef DISABLE_NETWORK
    gNetworkStart = NETWORK_MODE_SERVER;
#    endif

    std::unique_ptr<IContext> context(CreateContext());
    if (context->Initialise())
    {
        if (!context->LoadParkFromFile(inputPath))
        {
            return EXITCODE_FAIL;
        }

        //initialize the google benchmark parameters
        std::vector<char*> argv_for_benchmark;
        argv_for_benchmark.push_back(nullptr);

        for (int i = 1; i < argc; i++)
            argv_for_benchmark.push_back((char*)argv[i]);
        argc = (int)argv_for_benchmark.size();
        ::benchmark::Initialize(&argc, &argv_for_benchmark[0]);

        if (::benchmark::ReportUnrecognizedArguments(argc, &argv_for_benchmark[0]))
            return -1;

        benchmark::RegisterBenchmark(inputPath, BM_gamelogic, context.get())->UseManualTime();
        benchmark::RunSpecifiedBenchmarks();
    }
    else
    {
        Console::Error::WriteLine("Context initialization failed.");
        return EXITCODE_FAIL;
    }

    return EXITCODE_OK;
}

#else
static exitcode_t HandleBenchSpriteSort(CommandLineArgEnumerator* argEnumerator)
{
    log_error("Sorry, Google benchmark not enabled in this build");
    return EXITCODE_FAIL;
}
#endif // USE_BENCHMARK

const CommandLineCommand CommandLine::BenchLogicCommands[]{
#ifdef USE_BENCHMARK
    DefineCommand(
        "",
        "[<file>]... [--benchmark_list_tests={true|false}] [--benchmark_filter=<regex>] [--benchmark_min_time=<min_time>] "
        "[--benchmark_repetitions=<num_repetitions>] [--benchmark_report_aggregates_only={true|false}] "
        "[--benchmark_format=<console|json|csv>] [--benchmark_out=<filename>] [--benchmark_out_format=<json|console|csv>] "
        "[--benchmark_color={auto|true|false}] [--benchmark_counters_tabular={true|false}] [--v=<verbosity>]",
        nullptr, HandleBenchLogic),
    CommandTableEnd
#else
    DefineCommand("", "*** SORRY NOT ENABLED IN THIS BUILD ***", nullptr, HandleBenchSpriteSort), CommandTableEnd
#endif // USE_BENCHMARK
};
