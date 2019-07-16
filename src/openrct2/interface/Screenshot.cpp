/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Screenshot.h"

#include "../Context.h"
#include "../Game.h"
#include "../Intro.h"
#include "../OpenRCT2.h"
#include "../actions/SetCheatAction.hpp"
#include "../audio/audio.h"
#include "../core/Console.hpp"
#include "../core/Imaging.h"
#include "../core/Optional.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/X8DrawingEngine.h"
#include "../localisation/Localisation.h"
#include "../platform/platform.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Surface.h"
#include "Viewport.h"

#include <cctype>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <string>

using namespace std::literals::string_literals;
using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

uint8_t gScreenshotCountdown = 0;

static bool WriteDpiToFile(const std::string_view& path, const rct_drawpixelinfo* dpi, const rct_palette& palette)
{
    auto const pixels8 = dpi->bits;
    auto const pixelsLen = (dpi->width + dpi->pitch) * dpi->height;
    try
    {
        Image image;
        image.Width = dpi->width;
        image.Height = dpi->height;
        image.Depth = 8;
        image.Stride = dpi->width + dpi->pitch;
        image.Palette = std::make_unique<rct_palette>(palette);
        image.Pixels = std::vector<uint8_t>(pixels8, pixels8 + pixelsLen);
        Imaging::WriteToFile(path, image, IMAGE_FORMAT::PNG);
        return true;
    }
    catch (const std::exception& e)
    {
        log_error("Unable to write png: %s", e.what());
        return false;
    }
}

/**
 *
 *  rct2: 0x006E3AEC
 */
void screenshot_check()
{
    if (gScreenshotCountdown != 0)
    {
        gScreenshotCountdown--;
        if (gScreenshotCountdown == 0)
        {
            // update_rain_animation();
            std::string screenshotPath = screenshot_dump();

            if (!screenshotPath.empty())
            {
                audio_play_sound(SOUND_WINDOW_OPEN, 100, context_get_width() / 2);
            }
            else
            {
                context_show_error(STR_SCREENSHOT_FAILED, STR_NONE);
            }

            // redraw_rain();
        }
    }
}

static void screenshot_get_rendered_palette(rct_palette* palette)
{
    for (int32_t i = 0; i < 256; i++)
    {
        palette->entries[i] = gPalette[i];
    }
}

static std::string screenshot_get_park_name()
{
    return GetContext()->GetGameState()->GetPark().Name;
}

static std::string screenshot_get_directory()
{
    char screenshotPath[MAX_PATH];
    platform_get_user_directory(screenshotPath, "screenshot", sizeof(screenshotPath));
    return screenshotPath;
}

static std::pair<rct2_date, rct2_time> screenshot_get_date_time()
{
    rct2_date date;
    platform_get_date_local(&date);

    rct2_time time;
    platform_get_time_local(&time);

    return { date, time };
}

static std::string screenshot_get_formatted_date_time()
{
    auto [date, time] = screenshot_get_date_time();
    char formatted[64];
    snprintf(
        formatted, sizeof(formatted), "%4d-%02d-%02d %02d-%02d-%02d", date.year, date.month, date.day, time.hour, time.minute,
        time.second);
    return formatted;
}

static opt::optional<std::string> screenshot_get_next_path()
{
    auto screenshotDirectory = screenshot_get_directory();
    if (!platform_ensure_directory_exists(screenshotDirectory.c_str()))
    {
        log_error("Unable to save screenshots in OpenRCT2 screenshot directory.");
        return {};
    }

    auto parkName = screenshot_get_park_name();
    auto dateTime = screenshot_get_formatted_date_time();
    auto name = parkName + " " + dateTime;

    // Generate a path with a `tries` number
    auto pathComposer = [&screenshotDirectory, &name](int tries) {
        auto composedFilename = platform_sanitise_filename(
            name + ((tries > 0) ? " ("s + std::to_string(tries) + ")" : ""s) + ".png");
        return screenshotDirectory + PATH_SEPARATOR + composedFilename;
    };

    for (int tries = 0; tries < 100; tries++)
    {
        auto path = pathComposer(tries);
        if (!platform_file_exists(path.c_str()))
        {
            return path;
        }
    }

    log_error("You have too many saved screenshots saved at exactly the same date and time.");
    return {};
};

std::string screenshot_dump_png(rct_drawpixelinfo* dpi)
{
    // Get a free screenshot path
    auto path = screenshot_get_next_path();

    if (path == opt::nullopt)
    {
        return "";
    }

    rct_palette renderedPalette;
    screenshot_get_rendered_palette(&renderedPalette);

    if (WriteDpiToFile(path->c_str(), dpi, renderedPalette))
    {
        return *path;
    }
    else
    {
        return "";
    }
}

std::string screenshot_dump_png_32bpp(int32_t width, int32_t height, const void* pixels)
{
    auto path = screenshot_get_next_path();

    if (path == opt::nullopt)
    {
        return "";
    }

    const auto pixels8 = (const uint8_t*)pixels;
    const auto pixelsLen = width * 4 * height;

    try
    {
        Image image;
        image.Width = width;
        image.Height = height;
        image.Depth = 32;
        image.Stride = width * 4;
        image.Pixels = std::vector<uint8_t>(pixels8, pixels8 + pixelsLen);
        Imaging::WriteToFile(path->c_str(), image, IMAGE_FORMAT::PNG_32);
        return *path;
    }
    catch (const std::exception& e)
    {
        log_error("Unable to save screenshot: %s", e.what());
        return "";
    }
}

void screenshot_giant()
{
    int32_t originalRotation = get_current_rotation();
    int32_t originalZoom = 0;

    rct_window* mainWindow = window_get_main();
    rct_viewport* vp = window_get_viewport(mainWindow);
    if (mainWindow != nullptr && vp != nullptr)
        originalZoom = vp->zoom;

    int32_t rotation = originalRotation;
    int32_t zoom = originalZoom;
    int32_t mapSize = gMapSize;
    int32_t resolutionWidth = (mapSize * 32 * 2) >> zoom;
    int32_t resolutionHeight = (mapSize * 32 * 1) >> zoom;

    resolutionWidth += 8;
    resolutionHeight += 128;

    rct_viewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = resolutionWidth;
    viewport.height = resolutionHeight;
    viewport.view_width = viewport.width;
    viewport.view_height = viewport.height;
    viewport.var_11 = 0;
    viewport.flags = vp->flags;

    int32_t centreX = (mapSize / 2) * 32 + 16;
    int32_t centreY = (mapSize / 2) * 32 + 16;

    int32_t z = tile_element_height(centreX, centreY);

    CoordsXYZ centreCoords3d = { centreX, centreY, z };
    CoordsXY centreCoords2d = translate_3d_to_2d_with_z(rotation, centreCoords3d);

    viewport.view_x = centreCoords2d.x - ((viewport.view_width << zoom) / 2);
    viewport.view_y = centreCoords2d.y - ((viewport.view_height << zoom) / 2);
    viewport.zoom = zoom;
    gCurrentRotation = rotation;

    // Ensure sprites appear regardless of rotation
    reset_all_sprite_quadrant_placements();

    rct_drawpixelinfo dpi;
    dpi.x = 0;
    dpi.y = 0;
    dpi.width = resolutionWidth;
    dpi.height = resolutionHeight;
    dpi.pitch = 0;
    dpi.zoom_level = 0;
    dpi.bits = (uint8_t*)malloc(dpi.width * dpi.height);

    if (gConfigGeneral.transparent_screenshot)
    {
        std::memset(dpi.bits, PALETTE_INDEX_0, dpi.width * dpi.height);
        viewport.flags |= VIEWPORT_FLAG_TRANSPARENT_BACKGROUND;
    }

    auto drawingEngine = std::make_unique<X8DrawingEngine>(GetContext()->GetUiContext());
    dpi.DrawingEngine = drawingEngine.get();

    viewport_render(&dpi, &viewport, 0, 0, viewport.width, viewport.height);

    auto path = screenshot_get_next_path();
    if (path == opt::nullopt)
    {
        log_error("Giant screenshot failed, unable to find a suitable destination path.");
        context_show_error(STR_SCREENSHOT_FAILED, STR_NONE);
        return;
    }

    rct_palette renderedPalette;
    screenshot_get_rendered_palette(&renderedPalette);

    WriteDpiToFile(path->c_str(), &dpi, renderedPalette);

    free(dpi.bits);

    // Show user that screenshot saved successfully
    set_format_arg(0, rct_string_id, STR_STRING);
    set_format_arg(2, char*, path_get_filename(path->c_str()));
    context_show_error(STR_SCREENSHOT_SAVED_AS, STR_NONE);
}

static void benchgfx_render_screenshots(const char* inputPath, std::unique_ptr<IContext>& context, uint32_t iterationCount)
{
    if (!context->LoadParkFromFile(inputPath))
    {
        return;
    }

    gIntroState = INTRO_STATE_NONE;
    gScreenFlags = SCREEN_FLAGS_PLAYING;

    int32_t mapSize = gMapSize;
    int32_t resolutionWidth = (mapSize * 32 * 2);
    int32_t resolutionHeight = (mapSize * 32 * 1);

    resolutionWidth += 8;
    resolutionHeight += 128;

    rct_viewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = resolutionWidth;
    viewport.height = resolutionHeight;
    viewport.view_width = viewport.width;
    viewport.view_height = viewport.height;
    viewport.var_11 = 0;
    viewport.flags = 0;

    int32_t customX = (gMapSize / 2) * 32 + 16;
    int32_t customY = (gMapSize / 2) * 32 + 16;

    int32_t x = 0, y = 0;
    int32_t z = tile_element_height(customX, customY);
    x = customY - customX;
    y = ((customX + customY) / 2) - z;

    viewport.view_x = x - ((viewport.view_width) / 2);
    viewport.view_y = y - ((viewport.view_height) / 2);
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
    dpi.bits = (uint8_t*)malloc(dpi.width * dpi.height);

    auto startTime = std::chrono::high_resolution_clock::now();
    for (uint32_t i = 0; i < iterationCount; i++)
    {
        // Render at various zoom levels
        dpi.zoom_level = i & 3;
        viewport_render(&dpi, &viewport, 0, 0, viewport.width, viewport.height);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = endTime - startTime;
    char engine_name[128];
    rct_string_id engine_id = DrawingEngineStringIds[drawing_engine_get_type()];
    format_string(engine_name, sizeof(engine_name), engine_id, nullptr);
    Console::WriteLine(
        "Rendering %d times with drawing engine %s took %.2f seconds.", iterationCount, engine_name, duration.count());

    free(dpi.bits);
}

int32_t cmdline_for_gfxbench(const char** argv, int32_t argc)
{
    if (argc != 1 && argc != 2)
    {
        printf("Usage: openrct2 benchgfx <file> [<iteration_count>]\n");
        return -1;
    }

    core_init();
    int32_t iterationCount = 40;
    if (argc == 2)
    {
        iterationCount = atoi(argv[1]);
    }

    const char* inputPath = argv[0];

    gOpenRCT2Headless = true;

    std::unique_ptr<IContext> context(CreateContext());
    if (context->Initialise())
    {
        drawing_engine_init();

        benchgfx_render_screenshots(inputPath, context, iterationCount);

        drawing_engine_dispose();
    }

    return 1;
}

int32_t cmdline_for_screenshot(const char** argv, int32_t argc, ScreenshotOptions* options)
{
    // Don't include options in the count (they have been handled by CommandLine::ParseOptions already)
    for (int32_t i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            // Setting argc to i works, because options can only be at the end of the command
            argc = i;
            break;
        }
    }

    bool giantScreenshot = (argc == 5) && _stricmp(argv[2], "giant") == 0;
    if (argc != 4 && argc != 8 && !giantScreenshot)
    {
        std::printf("Usage: openrct2 screenshot <file> <output_image> <width> <height> [<x> <y> <zoom> <rotation>]\n");
        std::printf("Usage: openrct2 screenshot <file> <output_image> giant <zoom> <rotation>\n");
        return -1;
    }

    core_init();
    bool customLocation = false;
    bool centreMapX = false;
    bool centreMapY = false;
    int32_t resolutionWidth, resolutionHeight, customX = 0, customY = 0, customZoom, customRotation = 0;

    const char* inputPath = argv[0];
    const char* outputPath = argv[1];
    if (giantScreenshot)
    {
        resolutionWidth = 0;
        resolutionHeight = 0;
        customLocation = true;
        centreMapX = true;
        centreMapY = true;
        customZoom = std::atoi(argv[3]);
        customRotation = std::atoi(argv[4]) & 3;
    }
    else
    {
        resolutionWidth = std::atoi(argv[2]);
        resolutionHeight = std::atoi(argv[3]);
        if (argc == 8)
        {
            customLocation = true;
            if (argv[4][0] == 'c')
                centreMapX = true;
            else
                customX = std::atoi(argv[4]);

            if (argv[5][0] == 'c')
                centreMapY = true;
            else
                customY = std::atoi(argv[5]);

            customZoom = std::atoi(argv[6]);
            customRotation = std::atoi(argv[7]) & 3;
        }
        else
        {
            customZoom = 0;
        }
    }

    gOpenRCT2Headless = true;
    auto context = CreateContext();
    if (!context->Initialise())
    {
        std::puts("Failed to initialize context.");
        return -1;
    }

    drawing_engine_init();

    try
    {
        context->LoadParkFromFile(inputPath);
    }
    catch (const std::exception& e)
    {
        std::printf("%s\n", e.what());
        drawing_engine_dispose();
        return -1;
    }

    gIntroState = INTRO_STATE_NONE;
    gScreenFlags = SCREEN_FLAGS_PLAYING;

    int32_t mapSize = gMapSize;
    if (resolutionWidth == 0 || resolutionHeight == 0)
    {
        resolutionWidth = (mapSize * 32 * 2) >> customZoom;
        resolutionHeight = (mapSize * 32 * 1) >> customZoom;

        resolutionWidth += 8;
        resolutionHeight += 128;
    }

    rct_viewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = resolutionWidth;
    viewport.height = resolutionHeight;
    viewport.view_width = viewport.width;
    viewport.view_height = viewport.height;
    viewport.var_11 = 0;
    viewport.flags = 0;

    if (customLocation)
    {
        if (centreMapX)
            customX = (mapSize / 2) * 32 + 16;
        if (centreMapY)
            customY = (mapSize / 2) * 32 + 16;

        int32_t z = tile_element_height(customX, customY);
        CoordsXYZ coords3d = { customX, customY, z };

        CoordsXY coords2d = translate_3d_to_2d_with_z(customRotation, coords3d);

        viewport.view_x = coords2d.x - ((viewport.view_width << customZoom) / 2);
        viewport.view_y = coords2d.y - ((viewport.view_height << customZoom) / 2);
        viewport.zoom = customZoom;
        gCurrentRotation = customRotation;
    }
    else
    {
        viewport.view_x = gSavedViewX - (viewport.view_width / 2);
        viewport.view_y = gSavedViewY - (viewport.view_height / 2);
        viewport.zoom = gSavedViewZoom;
        gCurrentRotation = gSavedViewRotation;
    }

    if (options->weather != 0)
    {
        if (options->weather < 1 || options->weather > 6)
        {
            std::printf("Weather can only be set to an integer value from 1 till 6.");
            drawing_engine_dispose();
            return -1;
        }

        uint8_t customWeather = options->weather - 1;
        climate_force_weather(customWeather);
    }

    // Ensure sprites appear regardless of rotation
    reset_all_sprite_quadrant_placements();

    rct_drawpixelinfo dpi;
    dpi.x = 0;
    dpi.y = 0;
    dpi.width = resolutionWidth;
    dpi.height = resolutionHeight;
    dpi.pitch = 0;
    dpi.zoom_level = 0;
    dpi.bits = (uint8_t*)malloc(dpi.width * dpi.height);
    dpi.DrawingEngine = context->GetDrawingEngine();

    std::memset(dpi.bits, PALETTE_INDEX_0, dpi.width * dpi.height);

    if (options->hide_guests)
    {
        viewport.flags |= VIEWPORT_FLAG_INVISIBLE_PEEPS;
    }

    if (options->hide_sprites)
    {
        viewport.flags |= VIEWPORT_FLAG_INVISIBLE_SPRITES;
    }

    if (options->mowed_grass)
    {
        CheatsSet(CheatType::SetGrassLength, GRASS_LENGTH_MOWED);
    }

    if (options->clear_grass || options->tidy_up_park)
    {
        CheatsSet(CheatType::SetGrassLength, GRASS_LENGTH_CLEAR_0);
    }

    if (options->water_plants || options->tidy_up_park)
    {
        CheatsSet(CheatType::WaterPlants);
    }

    if (options->fix_vandalism || options->tidy_up_park)
    {
        CheatsSet(CheatType::FixVandalism);
    }

    if (options->remove_litter || options->tidy_up_park)
    {
        CheatsSet(CheatType::RemoveLitter);
    }

    if (options->transparent || gConfigGeneral.transparent_screenshot)
    {
        viewport.flags |= VIEWPORT_FLAG_TRANSPARENT_BACKGROUND;
    }

    viewport_render(&dpi, &viewport, 0, 0, viewport.width, viewport.height);

    rct_palette renderedPalette;
    screenshot_get_rendered_palette(&renderedPalette);

    WriteDpiToFile(outputPath, &dpi, renderedPalette);

    free(dpi.bits);
    drawing_engine_dispose();

    return 1;
}
