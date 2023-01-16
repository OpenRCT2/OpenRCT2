/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Text.h"

#include "../localisation/Formatter.h"
#include "../localisation/Localisation.h"
#include "Drawing.h"

static void DrawText(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const TextPaint& paint, const_utf8string text,
    bool noFormatting = false);
static void DrawText(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const TextPaint& paint, StringId format, const void* args);

class StaticLayout
{
private:
    utf8string Buffer;
    TextPaint Paint;
    int32_t LineCount = 0;
    int32_t LineHeight;
    int32_t MaxWidth;

public:
    StaticLayout(utf8string source, const TextPaint& paint, int32_t width)
    {
        Buffer = source;
        Paint = paint;

        MaxWidth = GfxWrapString(Buffer, width, paint.FontStyle, &LineCount);
        LineCount += 1;
        LineHeight = FontGetLineHeight(paint.FontStyle);
    }

    void Draw(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords)
    {
        TextPaint tempPaint = Paint;

        auto lineCoords = coords;
        switch (Paint.Alignment)
        {
            case TextAlignment::LEFT:
                break;
            case TextAlignment::CENTRE:
                lineCoords.x += MaxWidth / 2;
                break;
            case TextAlignment::RIGHT:
                lineCoords.x += MaxWidth;
                break;
        }
        utf8* buffer = Buffer;
        for (int32_t line = 0; line < LineCount; ++line)
        {
            DrawText(dpi, lineCoords, tempPaint, buffer);
            tempPaint.Colour = TEXT_COLOUR_254;
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

static void DrawText(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const TextPaint& paint, const_utf8string text, bool noFormatting)
{
    int32_t width = noFormatting ? GfxGetStringWidthNoFormatting(text, paint.FontStyle)
                                 : GfxGetStringWidth(text, paint.FontStyle);

    auto alignedCoords = coords;
    switch (paint.Alignment)
    {
        case TextAlignment::LEFT:
            break;
        case TextAlignment::CENTRE:
            alignedCoords.x -= (width - 1) / 2;
            break;
        case TextAlignment::RIGHT:
            alignedCoords.x -= width;
            break;
    }

    TTFDrawString(dpi, text, paint.Colour, alignedCoords, noFormatting, paint.FontStyle, paint.Darkness);

    if (paint.UnderlineText == TextUnderline::On)
    {
        GfxFillRect(
            dpi, { { alignedCoords + ScreenCoordsXY{ 0, 11 } }, { alignedCoords + ScreenCoordsXY{ width, 11 } } },
            gTextPalette[1]);
        if (gTextPalette[2] != 0)
        {
            GfxFillRect(
                dpi, { { alignedCoords + ScreenCoordsXY{ 1, 12 } }, { alignedCoords + ScreenCoordsXY{ width + 1, 12 } } },
                gTextPalette[2]);
        }
    }
}

static void DrawText(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const TextPaint& paint, StringId format, const void* args)
{
    utf8 buffer[512];
    format_string(buffer, sizeof(buffer), format, args);
    DrawText(dpi, coords, paint, buffer);
}

void DrawTextBasic(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, StringId format)
{
    Formatter ft{};
    TextPaint textPaint{};
    DrawTextBasic(dpi, coords, format, ft, textPaint);
}

void DrawTextBasic(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, StringId format, const Formatter& ft, TextPaint textPaint)
{
    DrawText(dpi, coords, textPaint, format, ft.Data());
}

void DrawTextEllipsised(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format)
{
    Formatter ft{};
    TextPaint textPaint{};
    DrawTextEllipsised(dpi, coords, width, format, ft, textPaint);
}

void DrawTextEllipsised(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format, const Formatter& ft,
    TextPaint textPaint)
{
    utf8 buffer[512];
    format_string(buffer, sizeof(buffer), format, ft.Data());
    GfxClipString(buffer, width, textPaint.FontStyle);

    DrawText(dpi, coords, textPaint, buffer);
}

void GfxDrawString(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const_utf8string buffer, TextPaint textPaint)
{
    DrawText(dpi, coords, textPaint, buffer);
}

void GfxDrawStringNoFormatting(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const_utf8string buffer, TextPaint textPaint)
{
    DrawText(dpi, coords, textPaint, buffer, true);
}

int32_t DrawTextWrapped(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format)
{
    Formatter ft{};
    TextPaint textPaint{};
    return DrawTextWrapped(dpi, coords, width, format, ft, textPaint);
}

int32_t DrawTextWrapped(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format, const Formatter& ft,
    TextPaint textPaint)
{
    const void* args = ft.Data();

    // TODO: Refactor StaticLayout to take a std::string_view instead. It shouldn't have to write to the buffer.
    const std::string buffer = FormatStringID(format, args);
    StaticLayout layout(const_cast<char*>(buffer.c_str()), textPaint, width);

    if (textPaint.Alignment == TextAlignment::CENTRE)
    {
        // The original tried to vertically centre the text, but used line count - 1
        int32_t lineCount = layout.GetLineCount();
        int32_t lineHeight = layout.GetHeight() / lineCount;
        int32_t yOffset = (lineCount - 1) * lineHeight / 2;

        layout.Draw(dpi, coords - ScreenCoordsXY{ layout.GetWidth() / 2, yOffset });
    }
    else
    {
        layout.Draw(dpi, coords);
    }

    return layout.GetHeight();
}
