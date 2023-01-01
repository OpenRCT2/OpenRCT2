/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Graph.h>
#include <openrct2/Context.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>

namespace Graph
{
    static void DrawMonths(rct_drawpixelinfo* dpi, const uint8_t* history, int32_t count, const ScreenCoordsXY& origCoords)
    {
        int32_t currentMonth = date_get_month(gDateMonthsElapsed);
        int32_t currentDay = gDateMonthTicks;
        int32_t yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
        auto screenCoords = origCoords;
        for (int32_t i = count - 1; i >= 0; i--)
        {
            if (history[i] != 255 && yearOver32 % 4 == 0)
            {
                // Draw month text
                auto ft = Formatter();
                ft.Add<uint32_t>(DateGameShortMonthNames[date_get_month((yearOver32 / 4) + MONTH_COUNT)]);
                DrawTextBasic(
                    dpi, screenCoords - ScreenCoordsXY{ 0, 10 }, STR_GRAPH_LABEL, ft,
                    { FontStyle::Small, TextAlignment::CENTRE });

                // Draw month mark
                gfx_fill_rect(dpi, { screenCoords, screenCoords + ScreenCoordsXY{ 0, 3 } }, PALETTE_INDEX_10);
            }

            yearOver32 = (yearOver32 + 1) % 32;
            screenCoords.x += 6;
        }
    }

    static void DrawLineA(rct_drawpixelinfo* dpi, const uint8_t* history, int32_t count, const ScreenCoordsXY& origCoords)
    {
        auto lastCoords = ScreenCoordsXY{ -1, -1 };
        auto coords = origCoords;
        for (int32_t i = count - 1; i >= 0; i--)
        {
            if (history[i] != 255)
            {
                coords.y = origCoords.y + ((255 - history[i]) * 100) / 256;

                if (lastCoords.x != -1)
                {
                    auto leftTop1 = lastCoords + ScreenCoordsXY{ 1, 1 };
                    auto rightBottom1 = coords + ScreenCoordsXY{ 1, 1 };
                    auto leftTop2 = lastCoords + ScreenCoordsXY{ 0, 1 };
                    auto rightBottom2 = coords + ScreenCoordsXY{ 0, 1 };
                    gfx_draw_line(dpi, { leftTop1, rightBottom1 }, PALETTE_INDEX_10);
                    gfx_draw_line(dpi, { leftTop2, rightBottom2 }, PALETTE_INDEX_10);
                }
                if (i == 0)
                    gfx_fill_rect(dpi, { coords, coords + ScreenCoordsXY{ 2, 2 } }, PALETTE_INDEX_10);

                lastCoords = coords;
            }
            coords.x += 6;
        }
    }

    static void DrawLineB(rct_drawpixelinfo* dpi, const uint8_t* history, int32_t count, const ScreenCoordsXY& origCoords)
    {
        auto lastCoords = ScreenCoordsXY{ -1, -1 };
        auto coords = origCoords;
        for (int32_t i = count - 1; i >= 0; i--)
        {
            if (history[i] != 255)
            {
                coords.y = origCoords.y + ((255 - history[i]) * 100) / 256;

                if (lastCoords.x != -1)
                {
                    auto leftTop = lastCoords;
                    auto rightBottom = coords;
                    gfx_draw_line(dpi, { leftTop, rightBottom }, PALETTE_INDEX_21);
                }
                if (i == 0)
                    gfx_fill_rect(dpi, { coords - ScreenCoordsXY{ 1, 1 }, coords + ScreenCoordsXY{ 1, 1 } }, PALETTE_INDEX_21);

                lastCoords = coords;
            }
            coords.x += 6;
        }
    }

    void Draw(rct_drawpixelinfo* dpi, uint8_t* history, int32_t count, const ScreenCoordsXY& screenPos)
    {
        DrawMonths(dpi, history, count, screenPos);
        DrawLineA(dpi, history, count, screenPos);
        DrawLineB(dpi, history, count, screenPos);
    }
} // namespace Graph

struct FinancialTooltipInfo
{
    const ScreenCoordsXY coords;
    const money64 money{};
};

static constexpr auto ChartMaxDataCount = 64;
static constexpr auto ChartMaxIndex = ChartMaxDataCount - 1;
static constexpr auto ChartDataWidth = 6;
static constexpr auto ChartMaxWidth = ChartMaxIndex * ChartDataWidth;
static constexpr auto ChartMaxHeight = 164;
static constexpr auto CursorXOffset = 3;
static constexpr auto DefaultDashedLength = 2;

static int32_t IndexForCursorAndHistory(const int32_t historyCount, const int32_t cursorX, const int32_t chartX)
{
    const auto offsettedCursorX = cursorX + CursorXOffset;
    return (historyCount - 1) - (offsettedCursorX - chartX) / ChartDataWidth;
}

static const ScreenCoordsXY ScreenCoordsForHistoryIndex(
    const int32_t index, const money64* history, const int32_t chartX, const int32_t chartY, const int32_t modifier,
    const int32_t offset)
{
    auto coords = ScreenCoordsXY{ chartX + ChartDataWidth * (ChartMaxIndex - index),
                                  chartY + ChartMaxHeight
                                      - (((static_cast<int32_t>(history[index] >> modifier) + offset) * 170) / 256) };
    return coords;
}

static const FinancialTooltipInfo FinanceTooltipInfoFromMoney(
    const money64* history, const int32_t historyCount, const int32_t modifier, const int32_t offset,
    const ScreenRect& chartFrame, const ScreenCoordsXY& cursorPosition)
{
    if (!chartFrame.Contains(cursorPosition))
    {
        return { {}, MONEY64_UNDEFINED };
    }

    const auto historyIndex = IndexForCursorAndHistory(historyCount, cursorPosition.x, chartFrame.GetLeft());
    const auto coords = ScreenCoordsForHistoryIndex(
        historyIndex, history, chartFrame.GetLeft(), chartFrame.GetTop(), modifier, offset);

    return { coords, history[historyIndex] };
}

namespace Graph
{
    static void DrawMonths(rct_drawpixelinfo* dpi, const money64* history, int32_t count, const ScreenCoordsXY& origCoords)
    {
        int32_t i, yearOver32, currentMonth, currentDay;

        currentMonth = date_get_month(gDateMonthsElapsed);
        currentDay = gDateMonthTicks;
        yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
        auto screenCoords = origCoords;
        for (i = count - 1; i >= 0; i--)
        {
            if (history[i] != MONEY64_UNDEFINED && yearOver32 % 4 == 0)
            {
                // Draw month text
                auto ft = Formatter();
                ft.Add<StringId>(DateGameShortMonthNames[date_get_month((yearOver32 / 4) + MONTH_COUNT)]);
                DrawTextBasic(
                    dpi, screenCoords - ScreenCoordsXY{ 0, 10 }, STR_GRAPH_LABEL, ft,
                    { FontStyle::Small, TextAlignment::CENTRE });

                // Draw month mark
                gfx_fill_rect(dpi, { screenCoords, screenCoords + ScreenCoordsXY{ 0, 3 } }, PALETTE_INDEX_10);
            }

            yearOver32 = (yearOver32 + 1) % 32;
            screenCoords.x += 6;
        }
    }

    static void DrawLineA(
        rct_drawpixelinfo* dpi, const money64* history, int32_t count, const ScreenCoordsXY& origCoords, int32_t modifier,
        int32_t offset)
    {
        auto lastCoords = ScreenCoordsXY{ -1, -1 };
        auto coords = origCoords;
        for (int32_t i = count - 1; i >= 0; i--)
        {
            if (history[i] != MONEY64_UNDEFINED)
            {
                coords.y = origCoords.y + 170 - 6 - ((((history[i] >> modifier) + offset) * 170) / 256);

                if (lastCoords.x != -1)
                {
                    auto leftTop1 = lastCoords + ScreenCoordsXY{ 1, 1 };
                    auto rightBottom1 = coords + ScreenCoordsXY{ 1, 1 };
                    auto leftTop2 = lastCoords + ScreenCoordsXY{ 0, 1 };
                    auto rightBottom2 = coords + ScreenCoordsXY{ 0, 1 };
                    gfx_draw_line(dpi, { leftTop1, rightBottom1 }, PALETTE_INDEX_10);
                    gfx_draw_line(dpi, { leftTop2, rightBottom2 }, PALETTE_INDEX_10);
                }
                if (i == 0)
                    gfx_fill_rect(dpi, { coords, coords + ScreenCoordsXY{ 2, 2 } }, PALETTE_INDEX_10);

                lastCoords = coords;
            }
            coords.x += 6;
        }
    }

    static void DrawLineB(
        rct_drawpixelinfo* dpi, const money64* history, int32_t count, const ScreenCoordsXY& origCoords, int32_t modifier,
        int32_t offset)
    {
        auto lastCoords = ScreenCoordsXY{ -1, -1 };
        auto coords = origCoords;
        for (int32_t i = count - 1; i >= 0; i--)
        {
            if (history[i] != MONEY64_UNDEFINED)
            {
                coords.y = origCoords.y + 170 - 6 - ((((history[i] >> modifier) + offset) * 170) / 256);

                if (lastCoords.x != -1)
                {
                    auto leftTop = lastCoords;
                    auto rightBottom = coords;
                    gfx_draw_line(dpi, { leftTop, rightBottom }, PALETTE_INDEX_21);
                }
                if (i == 0)
                    gfx_fill_rect(dpi, { coords - ScreenCoordsXY{ 1, 1 }, coords + ScreenCoordsXY{ 1, 1 } }, PALETTE_INDEX_21);

                lastCoords = coords;
            }
            coords.x += 6;
        }
    }

    static void DrawHoveredValue(
        rct_drawpixelinfo* dpi, const money64* history, const int32_t historyCount, const ScreenCoordsXY& screenCoords,
        const int32_t modifier, const int32_t offset)
    {
        const auto cursorPosition = ContextGetCursorPositionScaled();
        const ScreenRect chartFrame{ screenCoords, screenCoords + ScreenCoordsXY{ ChartMaxWidth, ChartMaxHeight } };

        if (!chartFrame.Contains(cursorPosition))
        {
            return;
        }

        const auto info = FinanceTooltipInfoFromMoney(history, ChartMaxDataCount, modifier, offset, chartFrame, cursorPosition);

        if (info.money == MONEY64_UNDEFINED)
        {
            return;
        }
        gfx_draw_dashed_line(dpi, { { info.coords.x, chartFrame.GetTop() }, info.coords }, DefaultDashedLength, 0);
        gfx_draw_dashed_line(dpi, { { chartFrame.GetLeft() - 10, info.coords.y }, info.coords }, DefaultDashedLength, 0);

        if (cursorPosition.y > info.coords.y)
        {
            gfx_draw_dashed_line(dpi, { info.coords, { info.coords.x, cursorPosition.y } }, DefaultDashedLength, 0);
        }

        auto ft = Formatter();
        ft.Add<money64>(info.money);
        DrawTextBasic(
            dpi, info.coords - ScreenCoordsXY{ 0, 16 }, STR_FINANCES_SUMMARY_EXPENDITURE_VALUE, ft, { TextAlignment::CENTRE });

        gfx_fill_rect(
            dpi, { { info.coords - ScreenCoordsXY{ 2, 2 } }, info.coords + ScreenCoordsXY{ 2, 2 } }, PALETTE_INDEX_10);
        gfx_fill_rect(
            dpi, { { info.coords - ScreenCoordsXY{ 1, 1 } }, { info.coords + ScreenCoordsXY{ 1, 1 } } }, PALETTE_INDEX_21);
    }

    void Draw(
        rct_drawpixelinfo* dpi, const money64* history, const int32_t count, const ScreenCoordsXY& screenCoords,
        const int32_t modifier, const int32_t offset)
    {
        DrawMonths(dpi, history, count, screenCoords);
        DrawLineA(dpi, history, count, screenCoords, modifier, offset);
        DrawLineB(dpi, history, count, screenCoords, modifier, offset);
        DrawHoveredValue(dpi, history, count, screenCoords, modifier, offset);
    }
} // namespace Graph
