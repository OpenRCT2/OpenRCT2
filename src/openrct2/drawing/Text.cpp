/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Text.h"

#include "../core/UTF8.h"
#include "../drawing/Rectangle.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../localisation/Language.h"
#include "Drawing.String.h"
#include "Drawing.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

class StaticLayout
{
private:
    u8string Buffer;
    TextPaint Paint;
    int32_t LineCount = 0;
    int32_t LineHeight;
    int32_t MaxWidth;

public:
    StaticLayout(u8string_view source, const TextPaint& paint, int32_t width)
        : Paint(paint)
    {
        MaxWidth = wrapString(source, width, paint.fontStyle, &Buffer, &LineCount);
        LineCount += 1;
        LineHeight = FontGetLineHeight(paint.fontStyle);
    }

    void Draw(RenderTarget& rt, const ScreenCoordsXY& coords)
    {
        TextPaint tempPaint = Paint;

        auto lineCoords = coords;
        switch (Paint.alignment)
        {
            case TextAlignment::left:
                break;
            case TextAlignment::centre:
                lineCoords.x += MaxWidth / 2;
                break;
            case TextAlignment::right:
                lineCoords.x += MaxWidth;
                break;
        }
        const utf8* buffer = Buffer.data();
        for (int32_t line = 0; line < LineCount; ++line)
        {
            drawText(rt, lineCoords, buffer, tempPaint);
            tempPaint.colour = OpenRCT2::Drawing::kColourNull;
            buffer = GetStringEnd(buffer) + 1;
            lineCoords.y += LineHeight;
        }
    }

    int32_t GetHeight() const
    {
        return LineHeight * LineCount;
    }

    int32_t GetWidth() const
    {
        return MaxWidth;
    }

    int32_t GetLineCount() const
    {
        return LineCount;
    }
};

static void drawTextUnderline(
    RenderTarget& rt, const ScreenCoordsXY& alignedCoords, int32_t width, const TextColours& textPalette)
{
    Rectangle::fill(
        rt, { { alignedCoords + ScreenCoordsXY{ 0, 11 } }, { alignedCoords + ScreenCoordsXY{ width, 11 } } }, textPalette.fill);
    if (textPalette.sunnyOutline != PaletteIndex::transparent)
    {
        Rectangle::fill(
            rt, { { alignedCoords + ScreenCoordsXY{ 1, 12 } }, { alignedCoords + ScreenCoordsXY{ width + 1, 12 } } },
            textPalette.sunnyOutline);
    }
}

void drawText(RenderTarget& rt, const ScreenCoordsXY& coords, StringId format, TextPaint textPaint)
{
    drawText(rt, coords, LanguageGetString(format), textPaint);
}

void drawText(RenderTarget& rt, const ScreenCoordsXY& coords, StringId format, const Formatter& ft, TextPaint textPaint)
{
    utf8 buffer[512];
    FormatStringLegacy(buffer, sizeof(buffer), format, ft.Data());
    drawText(rt, coords, buffer, textPaint);
}

void drawText(RenderTarget& rt, const ScreenCoordsXY& coords, u8string_view string, TextPaint textPaint)
{
    auto noFormatting = textPaint.flags.has(TextPaintFlag::noFormatting);
    int32_t width = getStringWidth(string, textPaint.fontStyle, noFormatting);

    auto alignedCoords = coords;
    switch (textPaint.alignment)
    {
        case TextAlignment::left:
            break;
        case TextAlignment::centre:
            alignedCoords.x -= (width - 1) / 2;
            break;
        case TextAlignment::right:
            alignedCoords.x -= width;
            break;
    }

    auto textPalette = ttfDrawString(
        rt, string, textPaint.colour, alignedCoords, noFormatting, textPaint.fontStyle, textPaint.darkness);

    if (textPaint.flags.has(TextPaintFlag::underline))
    {
        drawTextUnderline(rt, alignedCoords, width, textPalette);
    }
}

void drawTextEllipsised(RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format, TextPaint textPaint)
{
    drawTextEllipsised(rt, coords, width, LanguageGetString(format), textPaint);
}

void drawTextEllipsised(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format, const Formatter& ft, TextPaint textPaint)
{
    utf8 buffer[512];
    FormatStringLegacy(buffer, sizeof(buffer), format, ft.Data());
    clipString(buffer, width, textPaint.fontStyle);
    drawText(rt, coords, buffer, textPaint);
}

void drawTextEllipsised(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, u8string_view string, TextPaint textPaint)
{
    utf8 buffer[512]{};
    string.copy(buffer, std::min(string.length(), sizeof(buffer) - 1));
    clipString(buffer, width, textPaint.fontStyle);
    drawText(rt, coords, string, textPaint);
}

int32_t drawTextWrapped(RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format, TextPaint textPaint)
{
    return drawTextWrapped(rt, coords, width, LanguageGetString(format), textPaint);
}

int32_t drawTextWrapped(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format, const Formatter& ft, TextPaint textPaint)
{
    auto formatted = FormatStringIDLegacy(format, ft.Data());
    return drawTextWrapped(rt, coords, width, formatted, textPaint);
}

int32_t drawTextWrapped(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, u8string_view string, TextPaint textPaint)
{
    StaticLayout layout(string, textPaint, width);

    if (textPaint.alignment == TextAlignment::centre)
    {
        // The original tried to vertically centre the text, but used line count - 1
        int32_t lineCount = layout.GetLineCount();
        int32_t lineHeight = layout.GetHeight() / lineCount;
        int32_t yOffset = (lineCount - 1) * lineHeight / 2;

        layout.Draw(rt, coords - ScreenCoordsXY{ layout.GetWidth() / 2, yOffset });
    }
    else
    {
        layout.Draw(rt, coords);
    }

    return layout.GetHeight();
}
