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
#include "../core/StringTypes.h"
#include "../interface/ColourWithFlags.h"
#include "../world/Location.hpp"
#include "TextColour.h"

#include <cstdint>

using StringId = uint16_t;
struct ScreenCoordsXY;

enum class FontStyle : uint8_t;
enum class TextDarkness : uint8_t;

namespace OpenRCT2::Drawing
{
    struct RenderTarget;

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
        ScreenCoordsXY start{};
        ScreenCoordsXY current{};
        ScreenCoordsXY max{};
        TextDrawFlags textDrawFlags{};
        ColourFlags colourFlags{};
        TextDarkness darkness{};
        TextColours palette{};
        FontStyle fontStyle{};
        const int8_t* yOffset{};
    };

    void drawStringCentredRaw(
        RenderTarget& rt, const ScreenCoordsXY& coords, int32_t numLines, const utf8* text, FontStyle fontStyle);
    void drawNewsTicker(
        RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, Colour colour, StringId format, u8string_view args,
        int32_t ticks);
    void drawStringWithYOffsets(
        RenderTarget& rt, const utf8* text, ColourWithFlags colour, const ScreenCoordsXY& coords, const int8_t* yOffsets,
        bool forceSpriteFont, FontStyle fontStyle);

    int32_t wrapString(u8string_view text, int32_t width, FontStyle fontStyle, u8string* outWrappedText, int32_t* outNumLines);
    int32_t getStringWidthNewlined(std::string_view text, FontStyle fontStyle);
    int32_t getStringHeightRaw(std::string_view text, FontStyle fontStyle);
    int32_t clipString(char* buffer, int32_t width, FontStyle fontStyle);
    u8string shortenPath(const u8string& path, int32_t availableWidth, FontStyle fontStyle);
    TextColours ttfDrawString(
        RenderTarget& rt, u8string_view text, ColourWithFlags colour, const ScreenCoordsXY& coords, bool noFormatting,
        FontStyle fontStyle, TextDarkness darkness);
    int32_t getStringWidth(std::string_view text, FontStyle fontStyle, bool noFormatting = false);

} // namespace OpenRCT2::Drawing
