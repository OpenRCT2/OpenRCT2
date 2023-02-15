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
#include "../localisation/Formatting.h"
#include "../localisation/Localisation.h"
#include "Drawing.h"

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

    void Draw(DrawPixelInfo* dpi, const ScreenCoordsXY& coords)
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
        const utf8* buffer = Buffer.data();
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

void DrawText(
    DrawPixelInfo* dpi, const ScreenCoordsXY& coords, const TextPaint& paint, const_utf8string text, bool noFormatting)
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

void DrawTextBasic(DrawPixelInfo* dpi, const ScreenCoordsXY& coords, StringId format)
{
    Formatter ft{};
    TextPaint textPaint{};
    DrawTextBasic(dpi, coords, format, ft, textPaint);
}

void DrawTextBasic(DrawPixelInfo* dpi, const ScreenCoordsXY& coords, StringId format, const Formatter& ft, TextPaint textPaint)
{
    utf8 buffer[512];
    OpenRCT2::FormatStringLegacy(buffer, sizeof(buffer), format, ft.Data());
    DrawText(dpi, coords, textPaint, buffer);
}

void DrawTextEllipsised(DrawPixelInfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format)
{
    Formatter ft{};
    TextPaint textPaint{};
    DrawTextEllipsised(dpi, coords, width, format, ft, textPaint);
}

void DrawTextEllipsised(
    DrawPixelInfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format, const Formatter& ft, TextPaint textPaint)
{
    utf8 buffer[512];
    OpenRCT2::FormatStringLegacy(buffer, sizeof(buffer), format, ft.Data());
    GfxClipString(buffer, width, textPaint.FontStyle);

    DrawText(dpi, coords, textPaint, buffer);
}

void GfxDrawString(DrawPixelInfo* dpi, const ScreenCoordsXY& coords, const_utf8string buffer, TextPaint textPaint)
{
    DrawText(dpi, coords, textPaint, buffer);
}

int32_t DrawTextWrapped(DrawPixelInfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format)
{
    Formatter ft{};
    TextPaint textPaint{};
    return DrawTextWrapped(dpi, coords, width, format, ft, textPaint);
}

int32_t DrawTextWrapped(
    DrawPixelInfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format, const Formatter& ft, TextPaint textPaint)
{
    const void* args = ft.Data();

    StaticLayout layout(FormatStringID(format, args), textPaint, width);

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
