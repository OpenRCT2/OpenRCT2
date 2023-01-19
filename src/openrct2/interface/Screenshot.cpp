/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Screenshot.h"

#include "../Context.h"
#include "../Game.h"
#include "../GameState.h"
#include "../Intro.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../actions/CheatSetAction.h"
#include "../audio/audio.h"
#include "../core/Console.hpp"
#include "../core/File.h"
#include "../core/Imaging.h"
#include "../core/Path.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/X8DrawingEngine.h"
#include "../localisation/Formatter.h"
#include "../localisation/Localisation.h"
#include "../platform/Platform.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Surface.h"
#include "Viewport.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>

using namespace std::literals::string_literals;
using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

extern CoordsXY gClipSelectionA;
extern CoordsXY gClipSelectionB;
extern uint8_t gClipHeight;

uint8_t gScreenshotCountdown = 0;

static bool WriteDpiToFile(std::string_view path, const DrawPixelInfo* dpi, const GamePalette& palette)
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
        image.Palette = std::make_unique<GamePalette>(palette);
        image.Pixels = std::vector<uint8_t>(pixels8, pixels8 + pixelsLen);
        Imaging::WriteToFile(path, image, IMAGE_FORMAT::PNG);
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Unable to write png: %s", e.what());
        return false;
    }
}

/**
 *
 *  rct2: 0x006E3AEC
 */
void ScreenshotCheck()
{
    if (gScreenshotCountdown != 0)
    {
        gScreenshotCountdown--;
        if (gScreenshotCountdown == 0)
        {
            // update_weather_animation();
            std::string screenshotPath = ScreenshotDump();

            if (!screenshotPath.empty())
            {
                OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::WindowOpen, 100, ContextGetWidth() / 2);
            }
            else
            {
                ContextShowError(STR_SCREENSHOT_FAILED, STR_NONE, {});
            }

            // redraw_weather();
        }
    }
}

static std::string ScreenshotGetParkName()
{
    return GetContext()->GetGameState()->GetPark().Name;
}

static std::string ScreenshotGetDirectory()
{
    auto env = GetContext()->GetPlatformEnvironment();
    return env->GetDirectoryPath(DIRBASE::USER, DIRID::SCREENSHOT);
}

static std::pair<PODDate, TimeOfDay> ScreenshotGetDateTime()
{
    auto date = Platform::GetDateLocal();
    auto time = Platform::GetTimeLocal();

    return { date, time };
}

static std::string ScreenshotGetFormattedDateTime()
{
    auto [date, time] = ScreenshotGetDateTime();
    char formatted[64];
    snprintf(
        formatted, sizeof(formatted), "%4d-%02d-%02d %02d-%02d-%02d", date.year, date.month, date.day, time.hour, time.minute,
        time.second);
    return formatted;
}

static std::optional<std::string> ScreenshotGetNextPath()
{
    auto screenshotDirectory = ScreenshotGetDirectory();
    if (!Platform::EnsureDirectoryExists(screenshotDirectory.c_str()))
    {
        LOG_ERROR("Unable to save screenshots in OpenRCT2 screenshot directory.");
        return std::nullopt;
    }

    auto parkName = ScreenshotGetParkName();
    auto dateTime = ScreenshotGetFormattedDateTime();
    auto name = parkName + " " + dateTime;

    // Generate a path with a `tries` number
    auto pathComposer = [&screenshotDirectory, &name](int tries) {
        auto composedFilename = Platform::SanitiseFilename(
            name + ((tries > 0) ? " ("s + std::to_string(tries) + ")" : ""s) + ".png");
        return screenshotDirectory + PATH_SEPARATOR + composedFilename;
    };

    for (int tries = 0; tries < 100; tries++)
    {
        auto path = pathComposer(tries);
        if (!File::Exists(path))
        {
            return path;
        }
    }

    LOG_ERROR("You have too many saved screenshots saved at exactly the same date and time.");
    return std::nullopt;
};

std::string ScreenshotDumpPNG(DrawPixelInfo* dpi)
{
    // Get a free screenshot path
    auto path = ScreenshotGetNextPath();

    if (!path.has_value())
    {
        return "";
    }

    if (WriteDpiToFile(path.value(), dpi, gPalette))
    {
        return path.value();
    }

    return "";
}

std::string ScreenshotDumpPNG32bpp(int32_t width, int32_t height, const void* pixels)
{
    auto path = ScreenshotGetNextPath();

    if (!path.has_value())
    {
        return "";
    }

    const auto pixels8 = static_cast<const uint8_t*>(pixels);
    const auto pixelsLen = width * 4 * height;

    try
    {
        Image image;
        image.Width = width;
        image.Height = height;
        image.Depth = 32;
        image.Stride = width * 4;
        image.Pixels = std::vector<uint8_t>(pixels8, pixels8 + pixelsLen);
        Imaging::WriteToFile(path.value(), image, IMAGE_FORMAT::PNG_32);
        return path.value();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Unable to save screenshot: %s", e.what());
        return "";
    }
}

static int32_t GetHighestBaseClearanceZ(const CoordsXY& location, const bool useViewClipping)
{
    int32_t z = 0;
    auto element = MapGetFirstElementAt(location);
    if (element != nullptr)
    {
        do
        {
            if (useViewClipping && (element->GetBaseZ() > gClipHeight * COORDS_Z_STEP))
            {
                continue;
            }
            z = std::max<int32_t>(z, element->GetBaseZ());
            z = std::max<int32_t>(z, element->GetClearanceZ());
        } while (!(element++)->IsLastForTile());
    }
    return z;
}

static int32_t GetTallestVisibleTileTop(
    int32_t rotation, TileCoordsXY startCoords, TileCoordsXY endCoords, const bool useViewClipping)
{
    int32_t minViewY = std::numeric_limits<int32_t>::max();
    for (int32_t y = startCoords.y; y <= endCoords.y; y++)
    {
        for (int32_t x = startCoords.x; x <= endCoords.x; x++)
        {
            auto location = TileCoordsXY(x, y).ToCoordsXY();
            int32_t z = GetHighestBaseClearanceZ(location, useViewClipping);
            int32_t viewY = Translate3DTo2DWithZ(rotation, CoordsXYZ(location.ToTileCentre(), z)).y;
            minViewY = std::min(minViewY, viewY);
        }
    }
    // Some objects have a lower clearance than the actual sprite.
    return minViewY - 64;
}

static DrawPixelInfo CreateDPI(const rct_viewport& viewport)
{
    DrawPixelInfo dpi;
    dpi.width = viewport.width;
    dpi.height = viewport.height;
    dpi.bits = new (std::nothrow) uint8_t[dpi.width * dpi.height];
    if (dpi.bits == nullptr)
    {
        throw std::runtime_error("Giant screenshot failed, unable to allocate memory for image.");
    }

    if (viewport.flags & VIEWPORT_FLAG_TRANSPARENT_BACKGROUND)
    {
        std::memset(dpi.bits, PALETTE_INDEX_0, static_cast<size_t>(dpi.width) * dpi.height);
    }

    return dpi;
}

static void ReleaseDPI(DrawPixelInfo& dpi)
{
    if (dpi.bits != nullptr)
        delete[] dpi.bits;
    dpi.bits = nullptr;
    dpi.width = 0;
    dpi.height = 0;
}

static rct_viewport GetGiantViewport(int32_t rotation, ZoomLevel zoom)
{
    // Get the tile coordinates of each corner
    const TileCoordsXY cornerCoords[2][4] = {
        {
            // Map corners
            { 1, 1 },
            { gMapSize.x - 2, gMapSize.y - 2 },
            { 1, gMapSize.y - 2 },
            { gMapSize.x - 2, 1 },
        },
        {
            // Horizontal view clipping corners
            TileCoordsXY{ CoordsXY{ std::max(gClipSelectionA.x, 32), std::max(gClipSelectionA.y, 32) } },
            TileCoordsXY{ CoordsXY{ std::min(gClipSelectionB.x, (gMapSize.x - 2) * 32),
                                    std::min(gClipSelectionB.y, (gMapSize.y - 2) * 32) } },
            TileCoordsXY{ CoordsXY{ std::max(gClipSelectionA.x, 32), std::min(gClipSelectionB.y, (gMapSize.y - 2) * 32) } },
            TileCoordsXY{ CoordsXY{ std::min(gClipSelectionB.x, (gMapSize.x - 2) * 32), std::max(gClipSelectionA.y, 32) } },
        },
    };

    auto* const mainWindow = WindowGetMain();
    const auto* const mainViewport = WindowGetViewport(mainWindow);
    const bool useViewClipping = (mainViewport != nullptr && mainViewport->flags & VIEWPORT_FLAG_CLIP_VIEW);

    // Calculate the viewport bounds
    auto corners = cornerCoords[useViewClipping ? 1 : 0];
    auto screenCoords1 = Translate3DTo2DWithZ(rotation, { corners[0].ToCoordsXY().ToTileCentre(), 0 });
    auto screenCoords2 = Translate3DTo2DWithZ(rotation, { corners[1].ToCoordsXY().ToTileCentre(), 0 });
    auto screenCoords3 = Translate3DTo2DWithZ(rotation, { corners[2].ToCoordsXY().ToTileCentre(), 0 });
    auto screenCoords4 = Translate3DTo2DWithZ(rotation, { corners[3].ToCoordsXY().ToTileCentre(), 0 });

    auto left = std::min({ screenCoords1.x, screenCoords2.x, screenCoords3.x, screenCoords4.x }) - 32;
    auto top = GetTallestVisibleTileTop(rotation, corners[0], corners[1], useViewClipping);
    auto bottom = std::max({ screenCoords1.y, screenCoords2.y, screenCoords3.y, screenCoords4.y });
    auto right = std::max({ screenCoords1.x, screenCoords2.x, screenCoords3.x, screenCoords4.x }) + 32;

    rct_viewport viewport{};
    viewport.viewPos = { left, top };
    viewport.view_width = right - left;
    viewport.view_height = bottom - top;
    viewport.width = zoom.ApplyInversedTo(viewport.view_width);
    viewport.height = zoom.ApplyInversedTo(viewport.view_height);
    viewport.zoom = zoom;
    return viewport;
}

static void RenderViewport(IDrawingEngine* drawingEngine, const rct_viewport& viewport, DrawPixelInfo& dpi)
{
    // Ensure sprites appear regardless of rotation
    ResetAllSpriteQuadrantPlacements();

    std::unique_ptr<X8DrawingEngine> tempDrawingEngine;
    if (drawingEngine == nullptr)
    {
        tempDrawingEngine = std::make_unique<X8DrawingEngine>(GetContext()->GetUiContext());
        drawingEngine = tempDrawingEngine.get();
    }
    dpi.DrawingEngine = drawingEngine;
    ViewportRender(&dpi, &viewport, { { 0, 0 }, { viewport.width, viewport.height } });
}

void ScreenshotGiant()
{
    DrawPixelInfo dpi{};
    try
    {
        auto path = ScreenshotGetNextPath();
        if (!path.has_value())
        {
            throw std::runtime_error("Giant screenshot failed, unable to find a suitable destination path.");
        }

        const auto rotation = GetCurrentRotation();
        auto zoom = ZoomLevel{ 0 };
        auto* mainWindow = WindowGetMain();
        const auto* vp = WindowGetViewport(mainWindow);
        if (mainWindow != nullptr && vp != nullptr)
        {
            zoom = vp->zoom;
        }

        auto viewport = GetGiantViewport(rotation, zoom);
        if (vp != nullptr)
        {
            viewport.flags = vp->flags;
        }
        if (gConfigGeneral.TransparentScreenshot)
        {
            viewport.flags |= VIEWPORT_FLAG_TRANSPARENT_BACKGROUND;
        }

        dpi = CreateDPI(viewport);

        RenderViewport(nullptr, viewport, dpi);
        WriteDpiToFile(path.value(), &dpi, gPalette);

        // Show user that screenshot saved successfully
        const auto filename = Path::GetFileName(path.value());
        Formatter ft;
        ft.Add<StringId>(STR_STRING);
        ft.Add<const utf8*>(filename.c_str());
        ContextShowError(STR_SCREENSHOT_SAVED_AS, STR_NONE, ft);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("%s", e.what());
        ContextShowError(STR_SCREENSHOT_FAILED, STR_NONE, {});
    }

    ReleaseDPI(dpi);
}

// TODO: Move this at some point into a more appropriate place.
template<typename FN> static inline double MeasureFunctionTime(const FN& fn)
{
    const auto startTime = std::chrono::high_resolution_clock::now();
    fn();
    const auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(endTime - startTime).count();
}

static void BenchgfxRenderScreenshots(const char* inputPath, std::unique_ptr<IContext>& context, uint32_t iterationCount)
{
    if (!context->LoadParkFromFile(inputPath))
    {
        return;
    }

    gIntroState = IntroState::None;
    gScreenFlags = SCREEN_FLAGS_PLAYING;

    // Create Viewport and DPI for every rotation and zoom.
    // We iterate from the default zoom level to the max zoomed out zoom level, then run GetGiantViewport once for each
    // rotation.
    constexpr int32_t NUM_ROTATIONS = 4;
    constexpr auto NUM_ZOOM_LEVELS = static_cast<int8_t>(ZoomLevel::max());
    std::array<DrawPixelInfo, NUM_ROTATIONS * NUM_ZOOM_LEVELS> dpis;
    std::array<rct_viewport, NUM_ROTATIONS * NUM_ZOOM_LEVELS> viewports;

    for (ZoomLevel zoom{ 0 }; zoom < ZoomLevel::max(); zoom++)
    {
        int32_t zoomIndex{ static_cast<int8_t>(zoom) };
        for (int32_t rotation = 0; rotation < NUM_ROTATIONS; rotation++)
        {
            auto& viewport = viewports[zoomIndex * NUM_ZOOM_LEVELS + rotation];
            auto& dpi = dpis[zoomIndex * NUM_ZOOM_LEVELS + rotation];
            viewport = GetGiantViewport(rotation, zoom);
            dpi = CreateDPI(viewport);
        }
    }

    const uint32_t totalRenderCount = iterationCount * NUM_ROTATIONS * NUM_ZOOM_LEVELS;

    try
    {
        double totalTime = 0.0;

        std::array<double, NUM_ZOOM_LEVELS> zoomAverages;

        // Render at every zoom.
        for (int32_t zoom = 0; zoom < NUM_ZOOM_LEVELS; zoom++)
        {
            double zoomLevelTime = 0.0;

            // Render at every rotation.
            for (int32_t rotation = 0; rotation < NUM_ROTATIONS; rotation++)
            {
                // N iterations.
                for (uint32_t i = 0; i < iterationCount; i++)
                {
                    auto& dpi = dpis[zoom * NUM_ZOOM_LEVELS + rotation];
                    auto& viewport = viewports[zoom * NUM_ZOOM_LEVELS + rotation];
                    double elapsed = MeasureFunctionTime([&viewport, &dpi]() { RenderViewport(nullptr, viewport, dpi); });
                    totalTime += elapsed;
                    zoomLevelTime += elapsed;
                }
            }

            zoomAverages[zoom] = zoomLevelTime / static_cast<double>(NUM_ROTATIONS * iterationCount);
        }

        const double average = totalTime / static_cast<double>(totalRenderCount);
        const auto engineStringId = DrawingEngineStringIds[EnumValue(DrawingEngine::Software)];
        const auto engineName = FormatStringID(engineStringId, nullptr);
        std::printf("Engine: %s\n", engineName.c_str());
        std::printf("Render Count: %u\n", totalRenderCount);
        for (ZoomLevel zoom{ 0 }; zoom < ZoomLevel::max(); zoom++)
        {
            int32_t zoomIndex{ static_cast<int8_t>(zoom) };
            const auto zoomAverage = zoomAverages[zoomIndex];
            std::printf("Zoom[%d] average: %.06fs, %.f FPS\n", zoomIndex, zoomAverage, 1.0 / zoomAverage);
        }
        std::printf("Total average: %.06fs, %.f FPS\n", average, 1.0 / average);
        std::printf("Time: %.05fs\n", totalTime);
    }
    catch (const std::exception& e)
    {
        Console::Error::WriteLine("%s", e.what());
    }

    for (auto& dpi : dpis)
        ReleaseDPI(dpi);
}

int32_t CmdlineForGfxbench(const char** argv, int32_t argc)
{
    if (argc != 1 && argc != 2)
    {
        printf("Usage: openrct2 benchgfx <file> [<iteration_count>]\n");
        return -1;
    }

    Platform::CoreInit();
    int32_t iterationCount = 5;
    if (argc == 2)
    {
        iterationCount = atoi(argv[1]);
    }

    const char* inputPath = argv[0];

    gOpenRCT2Headless = true;

    std::unique_ptr<IContext> context(CreateContext());
    if (context->Initialise())
    {
        DrawingEngineInit();

        BenchgfxRenderScreenshots(inputPath, context, iterationCount);

        DrawingEngineDispose();
    }

    return 1;
}

static void ApplyOptions(const ScreenshotOptions* options, rct_viewport& viewport)
{
    if (options->weather != WeatherType::Sunny && options->weather != WeatherType::Count)
    {
        ClimateForceWeather(WeatherType{ static_cast<uint8_t>(EnumValue(options->weather) - 1) });
    }

    if (options->hide_guests)
    {
        viewport.flags |= VIEWPORT_FLAG_HIDE_GUESTS | VIEWPORT_FLAG_HIDE_STAFF;
    }

    if (options->hide_sprites)
    {
        viewport.flags |= VIEWPORT_FLAG_HIDE_ENTITIES;
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

    if (options->transparent || gConfigGeneral.TransparentScreenshot)
    {
        viewport.flags |= VIEWPORT_FLAG_TRANSPARENT_BACKGROUND;
    }
}

int32_t CmdlineForScreenshot(const char** argv, int32_t argc, ScreenshotOptions* options)
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

    int32_t exitCode = 1;
    DrawPixelInfo dpi;
    try
    {
        Platform::CoreInit();
        bool customLocation = false;
        bool centreMapX = false;
        bool centreMapY = false;

        const char* inputPath = argv[0];
        const char* outputPath = argv[1];

        gOpenRCT2Headless = true;
        auto context = CreateContext();
        if (!context->Initialise())
        {
            throw std::runtime_error("Failed to initialize context.");
        }

        DrawingEngineInit();

        if (!context->LoadParkFromFile(inputPath))
        {
            throw std::runtime_error("Failed to load park.");
        }

        gIntroState = IntroState::None;
        gScreenFlags = SCREEN_FLAGS_PLAYING;

        rct_viewport viewport{};
        if (giantScreenshot)
        {
            auto customZoom = static_cast<int8_t>(std::atoi(argv[3]));
            auto zoom = ZoomLevel{ customZoom };
            auto rotation = std::atoi(argv[4]) & 3;
            viewport = GetGiantViewport(rotation, zoom);
            gCurrentRotation = rotation;
        }
        else
        {
            int32_t resolutionWidth = std::atoi(argv[2]);
            int32_t resolutionHeight = std::atoi(argv[3]);
            int32_t customX = 0;
            int32_t customY = 0;
            int32_t customZoom = 0;
            int32_t customRotation = 0;
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

            const auto& mapSize = gMapSize;
            if (resolutionWidth == 0 || resolutionHeight == 0)
            {
                resolutionWidth = (mapSize.x * COORDS_XY_STEP * 2) >> customZoom;
                resolutionHeight = (mapSize.y * COORDS_XY_STEP * 1) >> customZoom;

                resolutionWidth += 8;
                resolutionHeight += 128;
            }

            viewport.width = resolutionWidth;
            viewport.height = resolutionHeight;
            viewport.view_width = viewport.width;
            viewport.view_height = viewport.height;
            if (customLocation)
            {
                if (centreMapX)
                    customX = (mapSize.x / 2) * 32 + 16;
                if (centreMapY)
                    customY = (mapSize.y / 2) * 32 + 16;

                int32_t z = TileElementHeight({ customX, customY });
                CoordsXYZ coords3d = { customX, customY, z };

                auto coords2d = Translate3DTo2DWithZ(customRotation, coords3d);

                viewport.viewPos = { coords2d.x - ((viewport.view_width << customZoom) / 2),
                                     coords2d.y - ((viewport.view_height << customZoom) / 2) };
                viewport.zoom = ZoomLevel{ static_cast<int8_t>(customZoom) };
                gCurrentRotation = customRotation;
            }
            else
            {
                viewport.viewPos = { gSavedView - ScreenCoordsXY{ (viewport.view_width / 2), (viewport.view_height / 2) } };
                viewport.zoom = gSavedViewZoom;
                gCurrentRotation = gSavedViewRotation;
            }
        }

        ApplyOptions(options, viewport);

        dpi = CreateDPI(viewport);

        RenderViewport(nullptr, viewport, dpi);
        WriteDpiToFile(outputPath, &dpi, gPalette);
    }
    catch (const std::exception& e)
    {
        std::printf("%s\n", e.what());
        exitCode = -1;
    }
    ReleaseDPI(dpi);

    DrawingEngineDispose();

    return exitCode;
}

static bool IsPathChildOf(fs::path x, const fs::path& parent)
{
    auto xp = x.parent_path();
    while (xp != x)
    {
        if (xp == parent)
        {
            return true;
        }
        x = xp;
        xp = x.parent_path();
    }
    return false;
}

static std::string ResolveFilenameForCapture(const fs::path& filename)
{
    if (filename.empty())
    {
        // Automatic filename
        auto path = ScreenshotGetNextPath();
        if (!path)
        {
            throw std::runtime_error("Unable to generate a filename for capture.");
        }
        return *path;
    }

    auto screenshotDirectory = fs::u8path(ScreenshotGetDirectory());
    auto screenshotPath = fs::absolute(screenshotDirectory / filename);

    // Check the filename isn't attempting to leave the screenshot directory for security
    if (!IsPathChildOf(screenshotPath, screenshotDirectory))
    {
        throw std::runtime_error("Filename is not a child of the screenshot directory.");
    }

    auto directory = screenshotPath.parent_path();
    if (!fs::is_directory(directory))
    {
        if (!fs::create_directory(directory, screenshotDirectory))
        {
            throw std::runtime_error("Unable to create directory.");
        }
    }

    return screenshotPath.u8string();
}

void CaptureImage(const CaptureOptions& options)
{
    rct_viewport viewport{};
    if (options.View.has_value())
    {
        viewport.width = options.View->Width;
        viewport.height = options.View->Height;
        viewport.view_width = viewport.width;
        viewport.view_height = viewport.height;

        auto z = TileElementHeight(options.View->Position);
        CoordsXYZ coords3d(options.View->Position, z);
        auto coords2d = Translate3DTo2DWithZ(options.Rotation, coords3d);
        viewport.viewPos = { coords2d.x - ((options.Zoom.ApplyTo(viewport.view_width)) / 2),
                             coords2d.y - ((options.Zoom.ApplyTo(viewport.view_height)) / 2) };
        viewport.zoom = options.Zoom;
    }
    else
    {
        viewport = GetGiantViewport(options.Rotation, options.Zoom);
    }

    auto backupRotation = gCurrentRotation;
    gCurrentRotation = options.Rotation;

    if (options.Transparent)
    {
        viewport.flags |= VIEWPORT_FLAG_TRANSPARENT_BACKGROUND;
    }

    auto outputPath = ResolveFilenameForCapture(options.Filename);
    auto dpi = CreateDPI(viewport);
    RenderViewport(nullptr, viewport, dpi);
    WriteDpiToFile(outputPath, &dpi, gPalette);
    ReleaseDPI(dpi);

    gCurrentRotation = backupRotation;
}
