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
#    include "../Game.h"
#    include "../Intro.h"
#    include "../OpenRCT2.h"
#    include "../audio/audio.h"
#    include "../core/Console.hpp"
#    include "../core/File.h"
#    include "../core/Imaging.h"
#    include "../drawing/Drawing.h"
#    include "../interface/Viewport.h"
#    include "../localisation/Localisation.h"
#    include "../paint/Paint.h"
#    include "../platform/Platform.h"
#    include "../util/Util.h"
#    include "../world/Climate.h"
#    include "../world/Map.h"
#    include "../world/Park.h"
#    include "../world/Surface.h"

#    include <benchmark/benchmark.h>
#    include <cstdint>
#    include <iterator>
#    include <vector>

static void fixup_pointers(std::vector<RecordedPaintSession>& s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        auto& entries = s[i].Entries;
        auto& quadrants = s[i].Session.Quadrants;
        for (size_t j = 0; j < entries.size(); j++)
        {
            if (entries[j].AsBasic()->next_quadrant_ps == reinterpret_cast<PaintStruct*>(-1))
            {
                entries[j].AsBasic()->next_quadrant_ps = nullptr;
            }
            else
            {
                auto nextQuadrantPs = reinterpret_cast<size_t>(entries[j].AsBasic()->next_quadrant_ps) / sizeof(PaintEntry);
                entries[j].AsBasic()->next_quadrant_ps = s[i].Entries[nextQuadrantPs].AsBasic();
            }
        }
        for (size_t j = 0; j < std::size(quadrants); j++)
        {
            if (quadrants[j] == reinterpret_cast<PaintStruct*>(-1))
            {
                quadrants[j] = nullptr;
            }
            else
            {
                auto ps = reinterpret_cast<size_t>(quadrants[j]) / sizeof(PaintEntry);
                quadrants[j] = entries[ps].AsBasic();
            }
        }
    }
}

static std::vector<RecordedPaintSession> extract_paint_session(std::string_view parkFileName)
{
    Platform::CoreInit();
    gOpenRCT2Headless = true;
    auto context = OpenRCT2::CreateContext();
    std::vector<RecordedPaintSession> sessions;
    log_info("Starting...");
    if (context->Initialise())
    {
        DrawingEngineInit();
        if (!context->LoadParkFromFile(std::string(parkFileName)))
        {
            log_error("Failed to load park!");
            return {};
        }

        gIntroState = IntroState::None;
        gScreenFlags = SCREEN_FLAGS_PLAYING;

        int32_t resolutionWidth = (gMapSize.x * COORDS_XY_STEP * 2);
        int32_t resolutionHeight = (gMapSize.y * COORDS_XY_STEP * 1);

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

        auto customXY = TileCoordsXY(gMapSize.x / 2, gMapSize.y / 2).ToCoordsXY().ToTileCentre();
        auto customXYZ = CoordsXYZ(customXY, TileElementHeight(customXY));
        auto screenXY = Translate3DTo2DWithZ(0, customXYZ);

        viewport.viewPos = { screenXY.x - (viewport.view_width / 2), screenXY.y - (viewport.view_height / 2) };
        viewport.zoom = ZoomLevel{ 0 };
        gCurrentRotation = 0;

        // Ensure sprites appear regardless of rotation
        ResetAllSpriteQuadrantPlacements();

        rct_drawpixelinfo dpi;
        dpi.x = 0;
        dpi.y = 0;
        dpi.width = resolutionWidth;
        dpi.height = resolutionHeight;
        dpi.pitch = 0;
        dpi.bits = static_cast<uint8_t*>(malloc(dpi.width * dpi.height));

        log_info("Obtaining sprite data...");
        ViewportRender(&dpi, &viewport, { { 0, 0 }, { viewport.width, viewport.height } }, &sessions);

        free(dpi.bits);
        DrawingEngineDispose();
    }
    log_info("Got %u paint sessions.", std::size(sessions));
    return sessions;
}

// This function is based on BenchgfxRenderScreenshots
static void BM_paint_session_arrange(benchmark::State& state, const std::vector<RecordedPaintSession> inputSessions)
{
    auto sessions = inputSessions;
    // Fixing up the pointers continuously is wasteful. Fix it up once for `sessions` and store a copy.
    // Keep in mind we need bit-exact copy, as the lists use pointers.
    // Once sorted, just restore the copy with the original fixed-up version.
    RecordedPaintSession* local_s = new RecordedPaintSession[std::size(sessions)];
    fixup_pointers(sessions);
    std::copy_n(sessions.cbegin(), std::size(sessions), local_s);
    for (auto _ : state)
    {
        state.PauseTiming();
        std::copy_n(local_s, std::size(sessions), sessions.begin());
        state.ResumeTiming();
        PaintSessionArrange(sessions[0].Session);
        benchmark::DoNotOptimize(sessions);
    }
    state.SetItemsProcessed(state.iterations() * std::size(sessions));
    delete[] local_s;
}

static int cmdline_for_bench_sprite_sort(int argc, const char** argv)
{
    {
        // Register some basic "baseline" benchmark
        std::vector<RecordedPaintSession> sessions(1);
        for (auto& ps : sessions[0].Entries)
        {
            ps.AsBasic()->next_quadrant_ps = reinterpret_cast<PaintStruct*>(-1);
        }
        for (auto& quad : sessions[0].Session.Quadrants)
        {
            quad = reinterpret_cast<PaintStruct*>(-1);
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
        if (File::Exists(argv[i]))
        {
            // Register benchmark for sv6 if valid
            std::vector<RecordedPaintSession> sessions = extract_paint_session(argv[i]);
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
