/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"
#include "../interface/ColourWithFlags.h"
#include "TextColour.h"

#include <cstdint>

enum class FontStyle : uint8_t;
enum class TextDarkness : uint8_t;

enum class TextDrawFlag : uint8_t
{
    noFormatting,
    yOffsetEffect,
    ttf,
    noDraw,
};
using TextDrawFlags = FlagHolder<uint8_t, TextDrawFlag>;

struct TextDrawInfo
{
    int32_t startX{};
    int32_t startY{};
    int32_t x{};
    int32_t y{};
    int32_t maxX{};
    int32_t maxY{};
    TextDrawFlags textDrawFlags{};
    OpenRCT2::ColourFlags colourFlags{};
    TextDarkness darkness{};
    OpenRCT2::Drawing::TextColours palette{};
    FontStyle fontStyle{};
    const int8_t* yOffset{};
};
