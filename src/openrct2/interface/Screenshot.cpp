/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <chrono>
#include <cstdlib>
#include <memory>

#include "../audio/audio.h"
#include "../Context.h"
#include "../core/Console.hpp"
#include "../core/Imaging.h"
#include "../OpenRCT2.h"
#include "Screenshot.h"

#include "../drawing/Drawing.h"
#include "../Game.h"
#include "../Intro.h"
#include "../localisation/Localisation.h"
#include "../platform/platform.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Surface.h"
#include "Viewport.h"

using namespace OpenRCT2;

uint8_t gScreenshotCountdown = 0;

static bool WriteDpiToFile(const std::string_view& path, const rct_drawpixelinfo * dpi, const rct_palette& palette)
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
    int32_t screenshotIndex;

    if (gScreenshotCountdown != 0) {
        gScreenshotCountdown--;
        if (gScreenshotCountdown == 0) {
            // update_rain_animation();
            screenshotIndex = screenshot_dump();

            if (screenshotIndex != -1) {
                audio_play_sound(SOUND_WINDOW_OPEN, 100, context_get_width() / 2);
            } else {
                context_show_error(STR_SCREENSHOT_FAILED, STR_NONE);
            }

            // redraw_rain();
        }
    }
}

static void screenshot_get_rendered_palette(rct_palette* palette) {
    for (int32_t i = 0; i < 256; i++) {
        palette->entries[i] = gPalette[i];
    }
}

static int32_t screenshot_get_next_path(char *path, size_t size)
{
    char screenshotPath[MAX_PATH];

    platform_get_user_directory(screenshotPath, "screenshot", sizeof(screenshotPath));
    if (!platform_ensure_directory_exists(screenshotPath)) {
        log_error("Unable to save screenshots in OpenRCT2 screenshot directory.\n");
        return -1;
    }

    char park_name[128];
    format_string(park_name, 128, gParkName, &gParkNameArgs);

    // Retrieve current time
    rct2_date currentDate;
    platform_get_date_local(&currentDate);
    rct2_time currentTime;
    platform_get_time_local(&currentTime);

#ifdef _WIN32
    // On NTFS filesystems, a colon (:) in a path
    // indicates you want to write a file stream
    // (hidden metadata). This will pass the
    // file_exists and fopen checks, since it is
    // technically valid. We don't want that, so
    // replace colons with hyphens in the park name.
    char * foundColon = park_name;
    while ((foundColon = strchr(foundColon, ':')) != nullptr)
    {
        *foundColon = '-';
    }
#endif

    // Glue together path and filename
    safe_strcpy(path, screenshotPath, size);
    path_end_with_separator(path, size);
    auto fileNameCh = strchr(path, '\0');
    if (fileNameCh == nullptr)
    {
        log_error("Unable to generate a screenshot filename.");
        return -1;
    }
    const size_t leftBytes = size - strlen(path);
    snprintf(fileNameCh, leftBytes, "%s %d-%02d-%02d %02d-%02d-%02d.png", park_name, currentDate.year, currentDate.month, currentDate.day, currentTime.hour, currentTime.minute, currentTime.second);

    if (!platform_file_exists(path)) {
        return 0; // path ok
    }

    // multiple screenshots with same timestamp
    // might be possible when switching timezones
    // in the unlikely case that this does happen,
    // append (%d) to the filename and increment
    // this int32_t until it doesn't overwrite any
    // other file in the directory.
    int32_t i;
    for (i = 1; i < 1000; i++) {
        // Glue together path and filename
        snprintf(fileNameCh, leftBytes, "%s %d-%02d-%02d %02d-%02d-%02d (%d).png", park_name, currentDate.year, currentDate.month, currentDate.day, currentTime.hour, currentTime.minute, currentTime.second, i);

        if (!platform_file_exists(path)) {
            return i;
        }
    }

    log_error("You have too many saved screenshots saved at exactly the same date and time.\n");
    return -1;
}

int32_t screenshot_dump_png(rct_drawpixelinfo *dpi)
{
    // Get a free screenshot path
    int32_t index;
    char path[MAX_PATH] = "";
    if ((index = screenshot_get_next_path(path, MAX_PATH)) == -1) {
        return -1;
    }

    rct_palette renderedPalette;
    screenshot_get_rendered_palette(&renderedPalette);

    if (WriteDpiToFile(path, dpi, renderedPalette))
    {
        return index;
    }
    else
    {
        return -1;
    }
}

int32_t screenshot_dump_png_32bpp(int32_t width, int32_t height, const void *pixels)
{
    // Get a free screenshot path
    int32_t index;
    char path[MAX_PATH] = "";
    if ((index = screenshot_get_next_path(path, MAX_PATH)) == -1) {
        return -1;
    }

    const auto pixels8 = (const uint8_t *)pixels;
    const auto pixelsLen = width * 4 * height;

    try
    {
        Image image;
        image.Width = width;
        image.Height = height;
        image.Depth = 32;
        image.Stride = width * 4;
        image.Pixels = std::vector<uint8_t>(pixels8, pixels8 + pixelsLen);
        Imaging::WriteToFile(path, image, IMAGE_FORMAT::PNG_32);
        return index;
    }
    catch (const std::exception &e)
    {
        log_error("Unable to save screenshot: %s", e.what());
        return -1;
    }
}

void screenshot_giant()
{
    int32_t originalRotation = get_current_rotation();
    int32_t originalZoom = 0;

    rct_window *mainWindow = window_get_main();
    rct_viewport * vp = window_get_viewport(mainWindow);
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
    viewport.flags = 0;

    int32_t centreX = (mapSize / 2) * 32 + 16;
    int32_t centreY = (mapSize / 2) * 32 + 16;

    int32_t x = 0, y = 0;
    int32_t z = tile_element_height(centreX, centreY) & 0xFFFF;
    switch (rotation) {
    case 0:
        x = centreY - centreX;
        y = ((centreX + centreY) / 2) - z;
        break;
    case 1:
        x = -centreY - centreX;
        y = ((-centreX + centreY) / 2) - z;
        break;
    case 2:
        x = -centreY + centreX;
        y = ((-centreX - centreY) / 2) - z;
        break;
    case 3:
        x = centreY + centreX;
        y = ((centreX - centreY) / 2) - z;
        break;
    }

    viewport.view_x = x - ((viewport.view_width << zoom) / 2);
    viewport.view_y = y - ((viewport.view_height << zoom) / 2);
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
    dpi.bits = (uint8_t *)malloc(dpi.width * dpi.height);

    viewport_render(&dpi, &viewport, 0, 0, viewport.width, viewport.height);

    // Get a free screenshot path
    char path[MAX_PATH];
    if (screenshot_get_next_path(path, MAX_PATH) == -1) {
        log_error("Giant screenshot failed, unable to find a suitable destination path.");
        context_show_error(STR_SCREENSHOT_FAILED, STR_NONE);
        return;
    }

    rct_palette renderedPalette;
    screenshot_get_rendered_palette(&renderedPalette);

    WriteDpiToFile(path, &dpi, renderedPalette);

    free(dpi.bits);

    // Show user that screenshot saved successfully
    set_format_arg(0, rct_string_id, STR_STRING);
    set_format_arg(2, char *, path_get_filename(path));
    context_show_error(STR_SCREENSHOT_SAVED_AS, STR_NONE);
}

static void benchgfx_render_screenshots(const char *inputPath, std::unique_ptr<IContext>& context, uint32_t iterationCount)
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
    int32_t z = tile_element_height(customX, customY) & 0xFFFF;
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
    dpi.bits = (uint8_t *)malloc(dpi.width * dpi.height);

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
    Console::WriteLine("Rendering %d times with drawing engine %s took %.2f seconds.",
        iterationCount, engine_name,
        duration.count());

    free(dpi.bits);
}

int32_t cmdline_for_gfxbench(const char **argv, int32_t argc)
{
    if (argc != 1 && argc != 2) {
        printf("Usage: openrct2 benchgfx <file> [<iteration_count>]\n");
        return -1;
    }

    core_init();
    int32_t iterationCount = 40;
    if (argc == 2)
    {
        iterationCount = atoi(argv[1]);
    }

    const char *inputPath = argv[0];

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

int32_t cmdline_for_screenshot(const char * * argv, int32_t argc, ScreenshotOptions * options)
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
    if (argc != 4 && argc != 8 && !giantScreenshot) {
        std::printf("Usage: openrct2 screenshot <file> <ouput_image> <width> <height> [<x> <y> <zoom> <rotation>]\n");
        std::printf("Usage: openrct2 screenshot <file> <ouput_image> giant <zoom> <rotation>\n");
        return -1;
    }

    core_init();
    bool customLocation = false;
    bool centreMapX = false;
    bool centreMapY = false;
    int32_t resolutionWidth, resolutionHeight, customX = 0, customY = 0, customZoom, customRotation = 0;

    const char *inputPath = argv[0];
    const char *outputPath = argv[1];
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
    if (context->Initialise())
    {
        drawing_engine_init();

        try
        {
            context->LoadParkFromFile(inputPath);
        }
        catch (const std::exception &e)
        {
            std::printf("%s\n", e.what());
            drawing_engine_dispose();
            return -1;
        }

        gIntroState = INTRO_STATE_NONE;
        gScreenFlags = SCREEN_FLAGS_PLAYING;

        int32_t mapSize = gMapSize;
        if (resolutionWidth == 0 || resolutionHeight == 0) {
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

        if (customLocation) {
            if (centreMapX)
                customX = (mapSize / 2) * 32 + 16;
            if (centreMapY)
                customY = (mapSize / 2) * 32 + 16;

            int32_t x = 0, y = 0;
            int32_t z = tile_element_height(customX, customY) & 0xFFFF;
            switch (customRotation) {
            case 0:
                x = customY - customX;
                y = ((customX + customY) / 2) - z;
                break;
            case 1:
                x = -customY - customX;
                y = ((-customX + customY) / 2) - z;
                break;
            case 2:
                x = -customY + customX;
                y = ((-customX - customY) / 2) - z;
                break;
            case 3:
                x = customY + customX;
                y = ((customX - customY) / 2) - z;
                break;
            }

            viewport.view_x = x - ((viewport.view_width << customZoom) / 2);
            viewport.view_y = y - ((viewport.view_height << customZoom) / 2);
            viewport.zoom = customZoom;
            gCurrentRotation = customRotation;
        } else {
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
        dpi.bits = (uint8_t *)malloc(dpi.width * dpi.height);

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
            game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGRASSLENGTH, GRASS_LENGTH_MOWED, GAME_COMMAND_CHEAT, 0, 0);
        }

        if (options->clear_grass || options->tidy_up_park)
        {
            game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGRASSLENGTH, GRASS_LENGTH_CLEAR_0, GAME_COMMAND_CHEAT, 0, 0);
        }

        if (options->water_plants || options->tidy_up_park)
        {
            game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_WATERPLANTS, 0, GAME_COMMAND_CHEAT, 0, 0);
        }

        if (options->fix_vandalism || options->tidy_up_park)
        {
            game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_FIXVANDALISM, 0, GAME_COMMAND_CHEAT, 0, 0);
        }

        if (options->remove_litter || options->tidy_up_park)
        {
            game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_REMOVELITTER, 0, GAME_COMMAND_CHEAT, 0, 0);
        }

        viewport_render(&dpi, &viewport, 0, 0, viewport.width, viewport.height);

        rct_palette renderedPalette;
        screenshot_get_rendered_palette(&renderedPalette);

        WriteDpiToFile(outputPath, &dpi, renderedPalette);

        free(dpi.bits);
        drawing_engine_dispose();
    }
    return 1;
}
