/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Graph.h>
#include <openrct2/Context.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Localisation.h>

namespace Graph
{
    static void DrawMonths(rct_drawpixelinfo* dpi, const uint8_t* history, int32_t count, int32_t baseX, int32_t baseY)
    {
        int32_t i, yearOver32, currentMonth, currentDay;

        currentMonth = date_get_month(gDateMonthsElapsed);
        currentDay = gDateMonthTicks;
        yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
        auto screenCoords = ScreenCoordsXY{ baseX, baseY };
        for (i = count - 1; i >= 0; i--)
        {
            if (history[i] != 255 && yearOver32 % 4 == 0)
            {
                // Draw month text
                auto ft = Formatter();
                ft.Add<uint32_t>(DateGameShortMonthNames[date_get_month((yearOver32 / 4) + MONTH_COUNT)]);
                DrawTextBasic(dpi, screenCoords - ScreenCoordsXY{ 0, 10 }, STR_GRAPH_LABEL, ft, { TextAlignment::CENTRE });

                // Draw month mark
                gfx_fill_rect(dpi, { screenCoords, screenCoords + ScreenCoordsXY{ 0, 3 } }, PALETTE_INDEX_10);
            }

            yearOver32 = (yearOver32 + 1) % 32;
            screenCoords.x += 6;
        }
    }

    static void DrawLineA(rct_drawpixelinfo* dpi, const uint8_t* history, int32_t count, int32_t baseX, int32_t baseY)
    {
        int32_t i, x, y, lastX, lastY;
        lastX = -1;
        lastY = -1;
        x = baseX;
        for (i = count - 1; i >= 0; i--)
        {
            if (history[i] != 255)
            {
                y = baseY + ((255 - history[i]) * 100) / 256;

                if (lastX != -1)
                {
                    auto leftTop1 = ScreenCoordsXY{ lastX + 1, lastY + 1 };
                    auto rightBottom1 = ScreenCoordsXY{ x + 1, y + 1 };
                    auto leftTop2 = ScreenCoordsXY{ lastX, lastY + 1 };
                    auto rightBottom2 = ScreenCoordsXY{ x, y + 1 };
                    gfx_draw_line(dpi, { leftTop1, rightBottom1 }, PALETTE_INDEX_10);
                    gfx_draw_line(dpi, { leftTop2, rightBottom2 }, PALETTE_INDEX_10);
                }
                if (i == 0)
                    gfx_fill_rect(dpi, { { x, y }, { x + 2, y + 2 } }, PALETTE_INDEX_10);

                lastX = x;
                lastY = y;
            }
            x += 6;
        }
    }

    static void DrawLineB(rct_drawpixelinfo* dpi, const uint8_t* history, int32_t count, int32_t baseX, int32_t baseY)
    {
        int32_t i, x, y, lastX, lastY;

        lastX = -1;
        lastY = -1;
        x = baseX;
        for (i = count - 1; i >= 0; i--)
        {
            if (history[i] != 255)
            {
                y = baseY + ((255 - history[i]) * 100) / 256;

                if (lastX != -1)
                {
                    auto leftTop = ScreenCoordsXY{ lastX, lastY };
                    auto rightBottom = ScreenCoordsXY{ x, y };
                    gfx_draw_line(dpi, { leftTop, rightBottom }, PALETTE_INDEX_21);
                }
                if (i == 0)
                    gfx_fill_rect(dpi, { { x - 1, y - 1 }, { x + 1, y + 1 } }, PALETTE_INDEX_21);

                lastX = x;
                lastY = y;
            }
            x += 6;
        }
    }

    void Draw(rct_drawpixelinfo* dpi, uint8_t* history, int32_t count, int32_t baseX, int32_t baseY)
    {
        DrawMonths(dpi, history, count, baseX, baseY);
        DrawLineA(dpi, history, count, baseX, baseY);
        DrawLineB(dpi, history, count, baseX, baseY);
    }
} // namespace Graph

struct FinancialTooltipInfo
{
    const ScreenCoordsXY coords;
    const money32 money{};
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
    const int32_t index, const money32* history, const int32_t chartX, const int32_t chartY, const int32_t modifier,
    const int32_t offset)
{
    ScreenCoordsXY coords;
    coords.x = chartX + ChartDataWidth * (ChartMaxIndex - index);
    coords.y = chartY + ChartMaxHeight - ((((history[index] >> modifier) + offset) * 170) / 256);
    return coords;
}

static const FinancialTooltipInfo finance_tooltip_info_from_money(
    const money32* history, const int32_t historyCount, const int32_t modifier, const int32_t offset,
    const ScreenRect& chartFrame, const ScreenCoordsXY& cursorPosition)
{
    if (!chartFrame.Contains(cursorPosition))
    {
        return { {}, MONEY32_UNDEFINED };
    }

    const auto historyIndex = IndexForCursorAndHistory(historyCount, cursorPosition.x, chartFrame.GetLeft());
    const auto coords = ScreenCoordsForHistoryIndex(
        historyIndex, history, chartFrame.GetLeft(), chartFrame.GetTop(), modifier, offset);

    return { { coords.x, coords.y }, history[historyIndex] };
}

namespace Graph
{
    static void DrawMonths(rct_drawpixelinfo* dpi, const money32* history, int32_t count, int32_t baseX, int32_t baseY)
    {
        int32_t i, yearOver32, currentMonth, currentDay;

        currentMonth = date_get_month(gDateMonthsElapsed);
        currentDay = gDateMonthTicks;
        yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
        auto screenCoords = ScreenCoordsXY{ baseX, baseY };
        for (i = count - 1; i >= 0; i--)
        {
            if (history[i] != MONEY32_UNDEFINED && yearOver32 % 4 == 0)
            {
                // Draw month text
                int32_t monthFormat = DateGameShortMonthNames[date_get_month((yearOver32 / 4) + MONTH_COUNT)];
                DrawTextBasic(
                    dpi, screenCoords - ScreenCoordsXY{ 0, 10 }, STR_GRAPH_LABEL, &monthFormat, { TextAlignment::CENTRE });

                // Draw month mark
                gfx_fill_rect(dpi, { screenCoords, screenCoords + ScreenCoordsXY{ 0, 3 } }, PALETTE_INDEX_10);
            }

            yearOver32 = (yearOver32 + 1) % 32;
            screenCoords.x += 6;
        }
    }

    static void DrawLineA(
        rct_drawpixelinfo* dpi, const money32* history, int32_t count, int32_t baseX, int32_t baseY, int32_t modifier,
        int32_t offset)
    {
        int32_t i, x, y, lastX, lastY;
        lastX = -1;
        lastY = -1;
        x = baseX;
        for (i = count - 1; i >= 0; i--)
        {
            if (history[i] != MONEY32_UNDEFINED)
            {
                y = baseY + 170 - 6 - ((((history[i] >> modifier) + offset) * 170) / 256);

                if (lastX != -1)
                {
                    auto leftTop1 = ScreenCoordsXY{ lastX + 1, lastY + 1 };
                    auto rightBottom1 = ScreenCoordsXY{ x + 1, y + 1 };
                    auto leftTop2 = ScreenCoordsXY{ lastX, lastY + 1 };
                    auto rightBottom2 = ScreenCoordsXY{ x, y + 1 };
                    gfx_draw_line(dpi, { leftTop1, rightBottom1 }, PALETTE_INDEX_10);
                    gfx_draw_line(dpi, { leftTop2, rightBottom2 }, PALETTE_INDEX_10);
                }
                if (i == 0)
                    gfx_fill_rect(dpi, { { x, y }, { x + 2, y + 2 } }, PALETTE_INDEX_10);

                lastX = x;
                lastY = y;
            }
            x += 6;
        }
    }

    static void DrawLineB(
        rct_drawpixelinfo* dpi, const money32* history, int32_t count, int32_t baseX, int32_t baseY, int32_t modifier,
        int32_t offset)
    {
        int32_t i, x, y, lastX, lastY;

        lastX = -1;
        lastY = -1;
        x = baseX;
        for (i = count - 1; i >= 0; i--)
        {
            if (history[i] != MONEY32_UNDEFINED)
            {
                y = baseY + 170 - 6 - ((((history[i] >> modifier) + offset) * 170) / 256);

                if (lastX != -1)
                {
                    auto leftTop = ScreenCoordsXY{ lastX, lastY };
                    auto rightBottom = ScreenCoordsXY{ x, y };
                    gfx_draw_line(dpi, { leftTop, rightBottom }, PALETTE_INDEX_21);
                }
                if (i == 0)
                    gfx_fill_rect(dpi, { { x - 1, y - 1 }, { x + 1, y + 1 } }, PALETTE_INDEX_21);

                lastX = x;
                lastY = y;
            }
            x += 6;
        }
    }

    static void DrawHoveredValue(
        rct_drawpixelinfo* dpi, const money32* history, const int32_t historyCount, const int32_t baseX, const int32_t baseY,
        const int32_t modifier, const int32_t offset)
    {
        const auto cursorPosition = context_get_cursor_position_scaled();
        const ScreenRect chartFrame{ { baseX, baseY }, { baseX + ChartMaxWidth, baseY + ChartMaxHeight } };

        if (!chartFrame.Contains(cursorPosition))
        {
            return;
        }

        const auto info = finance_tooltip_info_from_money(
            history, ChartMaxDataCount, modifier, offset, chartFrame, cursorPosition);

        if (info.money == MONEY32_UNDEFINED)
        {
            return;
        }
        gfx_draw_dashed_line(dpi, { { info.coords.x, chartFrame.GetTop() }, info.coords }, DefaultDashedLength, 0);
        gfx_draw_dashed_line(dpi, { { chartFrame.GetLeft() - 10, info.coords.y }, info.coords }, DefaultDashedLength, 0);

        if (cursorPosition.y > info.coords.y)
        {
            gfx_draw_dashed_line(dpi, { info.coords, { info.coords.x, cursorPosition.y } }, DefaultDashedLength, 0);
        }

        DrawTextBasic(
            dpi, info.coords - ScreenCoordsXY{ 0, 16 }, STR_FINANCES_SUMMARY_EXPENDITURE_VALUE, &info.money,
            { TextAlignment::CENTRE });

        gfx_fill_rect(
            dpi, { { info.coords - ScreenCoordsXY{ 2, 2 } }, info.coords + ScreenCoordsXY{ 2, 2 } }, PALETTE_INDEX_10);
        gfx_fill_rect(
            dpi, { { info.coords - ScreenCoordsXY{ 1, 1 } }, { info.coords + ScreenCoordsXY{ 1, 1 } } }, PALETTE_INDEX_21);
    }

    void Draw(
        rct_drawpixelinfo* dpi, const money32* history, const int32_t count, const int32_t baseX, const int32_t baseY,
        const int32_t modifier, const int32_t offset)
    {
        DrawMonths(dpi, history, count, baseX, baseY);
        DrawLineA(dpi, history, count, baseX, baseY, modifier, offset);
        DrawLineB(dpi, history, count, baseX, baseY, modifier, offset);
        DrawHoveredValue(dpi, history, count, baseX, baseY, modifier, offset);
    }
} // namespace Graph
