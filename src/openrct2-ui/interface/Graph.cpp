/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"

#include <openrct2-ui/interface/Graph.h>
#include <openrct2/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.Date.h>

using namespace OpenRCT2;

namespace OpenRCT2::Graph
{
    static void DrawMonths(DrawPixelInfo& dpi, const uint8_t* history, int32_t count, const ScreenCoordsXY& origCoords)
    {
        auto& date = GetDate();
        int32_t currentMonth = date.GetMonth();
        int32_t currentDay = date.GetMonthTicks();
        int32_t yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
        auto screenCoords = origCoords;
        for (int32_t i = count - 1; i >= 0; i--)
        {
            if (history[i] != 255 && yearOver32 % 4 == 0)
            {
                // Draw month text
                auto ft = Formatter();
                ft.Add<uint32_t>(DateGameShortMonthNames[DateGetMonth((yearOver32 / 4) + MONTH_COUNT)]);
                DrawTextBasic(
                    dpi, screenCoords - ScreenCoordsXY{ 0, 10 }, STR_GRAPH_LABEL, ft,
                    { FontStyle::Small, TextAlignment::CENTRE });

                // Draw month mark
                GfxFillRect(dpi, { screenCoords, screenCoords + ScreenCoordsXY{ 0, 3 } }, PALETTE_INDEX_10);
            }

            yearOver32 = (yearOver32 + 1) % 32;
            screenCoords.x += 6;
        }
    }

    static void DrawLineA(DrawPixelInfo& dpi, const uint8_t* history, int32_t count, const ScreenCoordsXY& origCoords)
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
                    GfxDrawLine(dpi, { leftTop1, rightBottom1 }, PALETTE_INDEX_10);
                    GfxDrawLine(dpi, { leftTop2, rightBottom2 }, PALETTE_INDEX_10);
                }
                if (i == 0)
                    GfxFillRect(dpi, { coords, coords + ScreenCoordsXY{ 2, 2 } }, PALETTE_INDEX_10);

                lastCoords = coords;
            }
            coords.x += 6;
        }
    }

    static void DrawLineB(DrawPixelInfo& dpi, const uint8_t* history, int32_t count, const ScreenCoordsXY& origCoords)
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
                    GfxDrawLine(dpi, { leftTop, rightBottom }, PALETTE_INDEX_21);
                }
                if (i == 0)
                    GfxFillRect(dpi, { coords - ScreenCoordsXY{ 1, 1 }, coords + ScreenCoordsXY{ 1, 1 } }, PALETTE_INDEX_21);

                lastCoords = coords;
            }
            coords.x += 6;
        }
    }

    void Draw(DrawPixelInfo& dpi, uint8_t* history, int32_t count, const ScreenCoordsXY& screenPos)
    {
        DrawMonths(dpi, history, count, screenPos);
        DrawLineA(dpi, history, count, screenPos);
        DrawLineB(dpi, history, count, screenPos);
    }
} // namespace OpenRCT2::Graph

namespace OpenRCT2::Graph
{
    constexpr int32_t kDashLength = 2;

    template<typename T, T TkNoValue>
    static void DrawMonths(DrawPixelInfo& dpi, const T* series, int32_t count, const ScreenRect& bounds, const int32_t xStep)
    {
        auto& date = GetDate();
        int32_t currentMonth = date.GetMonth();
        int32_t currentDay = date.GetMonthTicks();
        int32_t yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
        auto screenCoords = bounds.Point1;
        for (int32_t i = count - 1; i >= 0; i--)
        {
            if (series[i] != TkNoValue && yearOver32 % 4 == 0)
            {
                // Draw month text
                auto ft = Formatter();
                ft.Add<StringId>(DateGameShortMonthNames[DateGetMonth((yearOver32 / 4) + MONTH_COUNT)]);
                DrawTextBasic(
                    dpi, screenCoords - ScreenCoordsXY{ 0, 13 }, STR_GRAPH_LABEL, ft,
                    { FontStyle::Small, TextAlignment::CENTRE });

                // Draw month mark
                GfxFillRect(dpi, { screenCoords - ScreenCoordsXY{ 0, 3 }, screenCoords }, PALETTE_INDEX_10);
            }

            yearOver32 = (yearOver32 + 1) % 32;
            screenCoords.x += xStep;
        }
    }

    template<typename T>
    static void DrawHoveredValue(
        DrawPixelInfo& dpi, const T value, const int32_t hoverIdx, const ScreenRect& bounds, const int32_t xStep,
        const T minValue, const T maxValue)
    {
        const int32_t screenRange = bounds.GetHeight();
        const int32_t valueRange = maxValue - minValue;

        const int32_t yPosition = bounds.GetBottom() - ((value - minValue) * screenRange) / valueRange;
        ScreenCoordsXY coords = { bounds.GetRight() - hoverIdx * xStep, yPosition };
        // Line needs to be shifted over 1 pixel to match with the month ticks.
        ScreenCoordsXY lineCoords = { coords.x + 1, coords.y };

        GfxDrawDashedLine(
            dpi, { { lineCoords.x, bounds.GetTop() }, { lineCoords.x, bounds.GetBottom() } }, kDashLength, PALETTE_INDEX_10);
        GfxDrawDashedLine(dpi, { { bounds.GetLeft(), lineCoords.y }, lineCoords }, kDashLength, PALETTE_INDEX_10);

        auto ft = Formatter();
        ft.Add<money64>(value);
        DrawTextBasic(
            dpi, coords - ScreenCoordsXY{ 0, 16 }, STR_FINANCES_SUMMARY_EXPENDITURE_VALUE, ft, { TextAlignment::CENTRE });

        GfxFillRect(dpi, { { coords - ScreenCoordsXY{ 2, 2 } }, coords + ScreenCoordsXY{ 2, 2 } }, PALETTE_INDEX_10);
        GfxFillRect(dpi, { { coords - ScreenCoordsXY{ 1, 1 } }, { coords + ScreenCoordsXY{ 1, 1 } } }, PALETTE_INDEX_21);
    }

    template<typename T, T TkNoValue, bool TbackgroundLine>
    static void DrawLine(
        DrawPixelInfo& dpi, const T* series, const int32_t count, const ScreenRect& bounds, const int32_t xStep,
        const T minValue, const T maxValue)
    {
        const int32_t screenRange = bounds.GetHeight();
        const int32_t valueRange = maxValue - minValue;

        ScreenCoordsXY lastCoords;
        bool lastCoordsValid = false;
        ScreenCoordsXY coords = bounds.Point1;
        for (int32_t i = count - 1; i >= 0; i--)
        {
            auto value = series[i];
            if (value != TkNoValue)
            {
                coords.y = bounds.GetBottom() - ((value - minValue) * screenRange) / valueRange;

                if constexpr (TbackgroundLine)
                {
                    if (lastCoordsValid)
                    {
                        auto leftTop1 = lastCoords + ScreenCoordsXY{ 1, 1 };
                        auto rightBottom1 = coords + ScreenCoordsXY{ 1, 1 };
                        auto leftTop2 = lastCoords + ScreenCoordsXY{ 0, 1 };
                        auto rightBottom2 = coords + ScreenCoordsXY{ 0, 1 };
                        GfxDrawLine(dpi, { leftTop1, rightBottom1 }, PALETTE_INDEX_10);
                        GfxDrawLine(dpi, { leftTop2, rightBottom2 }, PALETTE_INDEX_10);
                    }
                    if (i == 0)
                    {
                        GfxFillRect(dpi, { coords, coords + ScreenCoordsXY{ 2, 2 } }, PALETTE_INDEX_10);
                    }
                }
                else
                {
                    if (lastCoordsValid)
                    {
                        auto leftTop = lastCoords;
                        auto rightBottom = coords;
                        GfxDrawLine(dpi, { leftTop, rightBottom }, PALETTE_INDEX_21);
                    }
                    if (i == 0)
                    {
                        GfxFillRect(
                            dpi, { coords - ScreenCoordsXY{ 1, 1 }, coords + ScreenCoordsXY{ 1, 1 } }, PALETTE_INDEX_21);
                    }
                }

                lastCoords = coords;
                lastCoordsValid = true;
            }
            coords.x += xStep;
        }
    }

    void DrawFinanceGraph(DrawPixelInfo& dpi, const GraphProperties<money64>& p)
    {
        money64 curLabel = p.max;
        int32_t curScreenPos = p.internalBounds.GetTop() - 5;
        const money64 yLabelStep = (p.max - p.min) / (p.numYLabels - 1);
        for (int32_t i = 0; i < p.numYLabels; i++)
        {
            Formatter ft;
            ft.Add<money64>(curLabel);
            DrawTextBasic(
                dpi, { p.internalBounds.GetLeft(), curScreenPos }, STR_FINANCES_FINANCIAL_GRAPH_CASH_VALUE, ft,
                { FontStyle::Small, TextAlignment::RIGHT });
            GfxFillRectInset(
                dpi, { { p.internalBounds.GetLeft(), curScreenPos + 5 }, { p.internalBounds.GetRight(), curScreenPos + 5 } },
                p.lineCol, INSET_RECT_FLAG_BORDER_INSET);
            curScreenPos += p.yLabelStepPx;
            curLabel -= yLabelStep;
        }

        DrawMonths<money64, kMoney64Undefined>(dpi, p.series, p.numPoints, p.internalBounds, p.xStepPx);
        DrawLine<money64, kMoney64Undefined, true>(dpi, p.series, p.numPoints, p.internalBounds, p.xStepPx, p.min, p.max);
        DrawLine<money64, kMoney64Undefined, false>(dpi, p.series, p.numPoints, p.internalBounds, p.xStepPx, p.min, p.max);
        if (p.hoverIdx >= 0 && p.hoverIdx < p.numPoints)
        {
            money64 value = p.series[p.hoverIdx];
            if (value != kMoney64Undefined)
                DrawHoveredValue<money64>(dpi, value, p.hoverIdx, p.internalBounds, p.xStepPx, p.min, p.max);
        }

        // todo debug code.
        ScreenCoordsXY bottomLeft{ p.internalBounds.Point1.x, p.internalBounds.Point2.y };
        ScreenCoordsXY topRight{ p.internalBounds.Point2.x, p.internalBounds.Point1.y };
        GfxDrawLine(dpi, { p.internalBounds.Point1, topRight }, 33);
        GfxDrawLine(dpi, { p.internalBounds.Point1, bottomLeft }, 33);
        GfxDrawLine(dpi, { bottomLeft, p.internalBounds.Point2 }, 33);
        GfxDrawLine(dpi, { topRight, p.internalBounds.Point2 }, 33);
    }
} // namespace OpenRCT2::Graph
