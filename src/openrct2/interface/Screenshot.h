/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/FileSystem.hpp"
#include "../world/Climate.h"
#include "../world/Location.hpp"
#include "ZoomLevel.h"

#include <optional>
#include <string>

struct rct_drawpixelinfo;

extern uint8_t gScreenshotCountdown;

struct ScreenshotOptions
{
    WeatherType weather = WeatherType::Sunny;
    bool hide_guests = false;
    bool hide_sprites = false;
    bool clear_grass = false;
    bool mowed_grass = false;
    bool water_plants = false;
    bool fix_vandalism = false;
    bool remove_litter = false;
    bool tidy_up_park = false;
    bool transparent = false;
};

struct CaptureView
{
    int32_t Width{};
    int32_t Height{};
    CoordsXY Position;
};

struct CaptureOptions
{
    fs::path Filename;
    std::optional<CaptureView> View;
    ZoomLevel Zoom;
    uint8_t Rotation{};
    bool Transparent{};
};

void ScreenshotCheck();
std::string ScreenshotDump();
std::string ScreenshotDumpPNG(rct_drawpixelinfo* dpi);
std::string ScreenshotDumpPNG32bpp(int32_t width, int32_t height, const void* pixels);

void ScreenshotGiant();
int32_t CmdlineForScreenshot(const char** argv, int32_t argc, ScreenshotOptions* options);
int32_t CmdlineForGfxbench(const char** argv, int32_t argc);

void CaptureImage(const CaptureOptions& options);
