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
        MaxWidth = GfxWrapString(source, width, paint.FontStyle, &Buffer, &LineCount);
        LineCount += 1;
        LineHeight = FontGetLineHeight(paint.FontStyle);
    }

    void Draw(RenderTarget& rt, const ScreenCoordsXY& coords)
    {
        TextPaint tempPaint = Paint;

        auto lineCoords = coords;
        switch (Paint.Alignment)
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
            DrawText(rt, lineCoords, tempPaint, buffer);
            tempPaint.Colour = OpenRCT2::Drawing::kColourNull;
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

void DrawText(RenderTarget& rt, const ScreenCoordsXY& coords, const TextPaint& paint, const_utf8string text, bool noFormatting)
{
    int32_t width = noFormatting ? GfxGetStringWidthNoFormatting(text, paint.FontStyle)
                                 : GfxGetStringWidth(text, paint.FontStyle);

    auto alignedCoords = coords;
    switch (paint.Alignment)
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

    TTFDrawString(rt, text, paint.Colour, alignedCoords, noFormatting, paint.FontStyle, paint.Darkness);

    if (paint.UnderlineText == TextUnderline::on)
    {
        Rectangle::fill(
            rt, { { alignedCoords + ScreenCoordsXY{ 0, 11 } }, { alignedCoords + ScreenCoordsXY{ width, 11 } } },
            gTextPalette.fill);
        if (gTextPalette.sunnyOutline != PaletteIndex::transparent)
        {
            Rectangle::fill(
                rt, { { alignedCoords + ScreenCoordsXY{ 1, 12 } }, { alignedCoords + ScreenCoordsXY{ width + 1, 12 } } },
                gTextPalette.sunnyOutline);
        }
    }
}

void DrawTextBasic(RenderTarget& rt, const ScreenCoordsXY& coords, StringId format)
{
    Formatter ft{};
    TextPaint textPaint{};
    DrawTextBasic(rt, coords, format, ft, textPaint);
}

void DrawTextBasic(RenderTarget& rt, const ScreenCoordsXY& coords, StringId format, const Formatter& ft, TextPaint textPaint)
{
    utf8 buffer[512];
    FormatStringLegacy(buffer, sizeof(buffer), format, ft.Data());
    DrawText(rt, coords, textPaint, buffer);
}

void DrawTextEllipsised(RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format)
{
    Formatter ft{};
    TextPaint textPaint{};
    DrawTextEllipsised(rt, coords, width, format, ft, textPaint);
}

void DrawTextEllipsised(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format, const Formatter& ft, TextPaint textPaint)
{
    utf8 buffer[512];
    FormatStringLegacy(buffer, sizeof(buffer), format, ft.Data());
    GfxClipString(buffer, width, textPaint.FontStyle);

    DrawText(rt, coords, textPaint, buffer);
}

int32_t DrawTextWrapped(RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format)
{
    Formatter ft{};
    TextPaint textPaint{};
    return DrawTextWrapped(rt, coords, width, format, ft, textPaint);
}

int32_t DrawTextWrapped(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format, const Formatter& ft, TextPaint textPaint)
{
    const void* args = ft.Data();

    StaticLayout layout(FormatStringIDLegacy(format, args), textPaint, width);

    if (textPaint.Alignment == TextAlignment::centre)
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
