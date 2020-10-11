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
#    include "../platform/Platform2.h"
#    include "../util/Util.h"
#    include "../world/Climate.h"
#    include "../world/Map.h"
#    include "../world/Park.h"
#    include "../world/Surface.h"

#    include <benchmark/benchmark.h>
#    include <cstdint>
#    include <iterator>
#    include <vector>

static void fixup_pointers(paint_session* s, size_t paint_session_entries, size_t paint_struct_entries, size_t quadrant_entries)
{
    for (size_t i = 0; i < paint_session_entries; i++)
    {
        for (size_t j = 0; j < paint_struct_entries; j++)
        {
            if (s[i].PaintStructs[j].basic.next_quadrant_ps == (paint_struct*)paint_struct_entries)
            {
                s[i].PaintStructs[j].basic.next_quadrant_ps = nullptr;
            }
            else
            {
                auto nextQuadrantPs = reinterpret_cast<uintptr_t>(s[i].PaintStructs[j].basic.next_quadrant_ps);
                s[i].PaintStructs[j].basic.next_quadrant_ps = &s[i].PaintStructs[nextQuadrantPs].basic;
            }
        }
        for (size_t j = 0; j < quadrant_entries; j++)
        {
            if (s[i].Quadrants[j] == (paint_struct*)quadrant_entries)
            {
                s[i].Quadrants[j] = nullptr;
            }
            else
            {
                s[i].Quadrants[j] = &s[i].PaintStructs[(size_t)s[i].Quadrants[j]].basic;
            }
        }
    }
}

static std::vector<paint_session> extract_paint_session(const std::string parkFileName)
{
    core_init();
    gOpenRCT2Headless = true;
    auto context = OpenRCT2::CreateContext();
    std::vector<paint_session> sessions;
    log_info("Starting...");
    if (context->Initialise())
    {
        drawing_engine_init();
        if (!context->LoadParkFromFile(parkFileName))
        {
            log_error("Failed to load park!");
            return {};
        }

        gIntroState = IntroState::None;
        gScreenFlags = SCREEN_FLAGS_PLAYING;

        int32_t mapSize = gMapSize;
        int32_t resolutionWidth = (mapSize * 32 * 2);
        int32_t resolutionHeight = (mapSize * 32 * 1);

        resolutionWidth += 8;
        resolutionHeight += 128;

        rct_viewport viewport;
        viewport.pos = { 0, 0 };
        viewport.width = resolutionWidth;
        viewport.height = resolutionHeight;
        viewport.view_width = viewport.width;
        viewport.view_height = viewport.height;
        viewport.var_11 = 0;
        viewport.flags = 0;

        int32_t customX = (gMapSize / 2) * 32 + 16;
        int32_t customY = (gMapSize / 2) * 32 + 16;

        int32_t x = 0, y = 0;
        int32_t z = tile_element_height({ customX, customY });
        x = customY - customX;
        y = ((customX + customY) / 2) - z;

        viewport.viewPos = { x - ((viewport.view_width) / 2), y - ((viewport.view_height) / 2) };
        viewport.zoom = 0;
        gCurrentRotation = 0;

        // Ensure sprites appear regardless of rotation
        reset_all_sprite_quadrant_placements();

        rct_drawpixelinfo dpi;
        dpi.x = 0;
        dpi.y = 0;
        dpi.width = resolutionWidth;
        dpi.height = resolutionHeight;
        dpi.pitch = 0;
        dpi.bits = static_cast<uint8_t*>(malloc(dpi.width * dpi.height));

        log_info("Obtaining sprite data...");
        viewport_render(&dpi, &viewport, 0, 0, viewport.width, viewport.height, &sessions);

        free(dpi.bits);
        drawing_engine_dispose();
    }
    log_info("Got %u paint sessions.", std::size(sessions));
    return sessions;
}

// This function is based on benchgfx_render_screenshots
static void BM_paint_session_arrange(benchmark::State& state, const std::vector<paint_session> inputSessions)
{
    std::vector<paint_session> sessions = inputSessions;
    // Fixing up the pointers continuously is wasteful. Fix it up once for `sessions` and store a copy.
    // Keep in mind we need bit-exact copy, as the lists use pointers.
    // Once sorted, just restore the copy with the original fixed-up version.
    paint_session* local_s = new paint_session[std::size(sessions)];
    fixup_pointers(&sessions[0], std::size(sessions), std::size(local_s->PaintStructs), std::size(local_s->Quadrants));
    std::copy_n(sessions.cbegin(), std::size(sessions), local_s);
    for (auto _ : state)
    {
        state.PauseTiming();
        std::copy_n(local_s, std::size(sessions), sessions.begin());
        state.ResumeTiming();
        paint_session_arrange(&sessions[0]);
        benchmark::DoNotOptimize(sessions);
    }
    state.SetItemsProcessed(state.iterations() * std::size(sessions));
    delete[] local_s;
}

static int cmdline_for_bench_sprite_sort(int argc, const char** argv)
{
    {
        // Register some basic "baseline" benchmark
        std::vector<paint_session> sessions(1);
        for (auto& ps : sessions[0].PaintStructs)
        {
            ps.basic.next_quadrant_ps = (paint_struct*)(std::size(sessions[0].PaintStructs));
        }
        for (auto& quad : sessions[0].Quadrants)
        {
            quad = (paint_struct*)(std::size(sessions[0].Quadrants));
        }
        benchmark::RegisterBenchmark("baseline", BM_paint_session_arrange, sessions);
    }

    // Google benchmark does stuff to argv. It doesn't modify the pointees,
    // but it wants to reorder the pointers, so present a copy of them.
    std::vector<char*> argv_for_benchmark;

    // argv[0] is expected to contain the binary name. It's only for logging purposes, don't bother.
    argv_for_benchmark.push_back(nullptr);

    // Extract file names from argument list. If there is no such file, consider it benchmark option.
    for (int i = 0; i < argc; i++)
    {
        if (Platform::FileExists(argv[i]))
        {
            // Register benchmark for sv6 if valid
            std::vector<paint_session> sessions = extract_paint_session(argv[i]);
            if (!sessions.empty())
                benchmark::RegisterBenchmark(argv[i], BM_paint_session_arrange, sessions);
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
    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
}

static exitcode_t HandleBenchSpriteSort(CommandLineArgEnumerator* argEnumerator)
{
    const char** argv = const_cast<const char**>(argEnumerator->GetArguments()) + argEnumerator->GetIndex();
    int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex();
    int32_t result = cmdline_for_bench_sprite_sort(argc, argv);
    if (result < 0)
    {
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

const CommandLineCommand CommandLine::BenchSpriteSortCommands[]{
#ifdef USE_BENCHMARK
    DefineCommand(
        "",
        "[<file>]... [--benchmark_list_tests={true|false}] [--benchmark_filter=<regex>] [--benchmark_min_time=<min_time>] "
        "[--benchmark_repetitions=<num_repetitions>] [--benchmark_report_aggregates_only={true|false}] "
        "[--benchmark_format=<console|json|csv>] [--benchmark_out=<filename>] [--benchmark_out_format=<json|console|csv>] "
        "[--benchmark_color={auto|true|false}] [--benchmark_counters_tabular={true|false}] [--v=<verbosity>]",
        nullptr, HandleBenchSpriteSort),
    CommandTableEnd
#else
    DefineCommand("", "*** SORRY NOT ENABLED IN THIS BUILD ***", nullptr, HandleBenchSpriteSort), CommandTableEnd
#endif // USE_BENCHMARK
};
