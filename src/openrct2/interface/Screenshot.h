/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FileSystem.hpp"
#include "../world/Climate.h"
#include "../world/Location.hpp"
#include "ZoomLevel.h"

#include <optional>
#include <string>

struct DrawPixelInfo;

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
std::string ScreenshotDumpPNG(DrawPixelInfo& dpi);

void ScreenshotGiant();
int32_t CommandLineForScreenshot(const char** argv, int32_t argc, ScreenshotOptions* options);

void CaptureImage(const CaptureOptions& options);
