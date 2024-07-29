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
#include <openrct2/Context.h>
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
    constexpr ScreenCoordsXY kFinanceTopLeftPadding{ 88, 20 };
    constexpr ScreenCoordsXY kFinanceBottomRightPadding{ 15, 18 };
    constexpr uint8_t kNumFinanceGraphYLabels = 5;

    template<typename T, T TkNoValue>
    static void DrawMonths(
        DrawPixelInfo& dpi, const T* series, int32_t count, const ScreenRect& bounds, const int32_t xStep)
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

    template<typename T, T TkNoValue>
    static void DrawHoveredValue(
        DrawPixelInfo& dpi, const T* series, const int32_t count, const ScreenRect& bounds, const int32_t xStep,
        const T minValue, const T maxValue)
    {
        const ScreenCoordsXY cursorPos = ContextGetCursorPositionScaled();
        if (!bounds.Contains(cursorPos))
            return;

        int32_t i = (count - 1) - (cursorPos.x - bounds.GetLeft() + (xStep / 2)) / xStep;
        if (i < 0)
            i = 1;
        if (i > count - 1)
            i = count - 1;

        T value = series[i];
        if (value == TkNoValue)
            return;
        const int32_t screenRange = bounds.GetHeight();
        const int32_t valueRange = maxValue - minValue;
        int32_t test = bounds.GetBottom() - ((value - minValue) * screenRange) / valueRange;
        ScreenCoordsXY coords = { bounds.GetRight() - i * xStep, test };
        ScreenCoordsXY lineCoords = { coords.x + 1, coords.y };

        GfxDrawDashedLine(dpi, { { lineCoords.x, bounds.GetTop() }, lineCoords }, kDashLength, PALETTE_INDEX_10);
        GfxDrawDashedLine(dpi, { { bounds.GetLeft(), lineCoords.y }, lineCoords }, kDashLength, PALETTE_INDEX_10);

        if (cursorPos.y > coords.y)
        {
            GfxDrawDashedLine(dpi, { lineCoords, { lineCoords.x, cursorPos.y } }, kDashLength, PALETTE_INDEX_10);
        }

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

    void DrawFinanceGraph(
        DrawPixelInfo& dpi, const money64 (&series)[128], const ScreenRect& graphBounds, const bool centred,
        const ColourWithFlags lineCol)
    {
        constexpr int32_t count = 64; // todo for whatever reason this is 64.

        ScreenRect internalBounds{ graphBounds.Point1 + kFinanceTopLeftPadding,
                                   graphBounds.Point2 - kFinanceBottomRightPadding };
        const int32_t yLabelStepPx = (internalBounds.GetBottom() - internalBounds.GetTop()) / (kNumFinanceGraphYLabels - 1);
        const int32_t xStepPx = (internalBounds.GetRight() - internalBounds.GetLeft()) / (count - 1);

        // adjust bounds to be exact multiples of the steps.
        internalBounds.Point2 = internalBounds.Point1
            + ScreenCoordsXY{ xStepPx * (count - 1), yLabelStepPx * (kNumFinanceGraphYLabels - 1) };

        money64 graphMaximum = centred ? 12.00_GBP : 24.00_GBP;
        for (int32_t i = 0; i < count; i++)
        {
            auto currentValue = series[i];
            if (currentValue == kMoney64Undefined)
                continue;
            while (std::abs(currentValue) > graphMaximum)
                graphMaximum *= 2;
        }
        const money64 graphMinimum = centred ? -graphMaximum : 0.00_GBP;

        const money64 yLabelStep = (graphMaximum - graphMinimum) / (kNumFinanceGraphYLabels - 1);
        money64 curLabel = graphMaximum;
        int32_t curScreenPos = internalBounds.GetTop() - 5;
        for (uint8_t i = 0; i < kNumFinanceGraphYLabels; i++)
        {
            Formatter ft;
            ft.Add<money64>(curLabel);
            DrawTextBasic(
                dpi, { internalBounds.GetLeft(), curScreenPos }, STR_FINANCES_FINANCIAL_GRAPH_CASH_VALUE, ft,
                { FontStyle::Small, TextAlignment::RIGHT });
            GfxFillRectInset(
                dpi, { { internalBounds.GetLeft(), curScreenPos + 5 }, { internalBounds.GetRight(), curScreenPos + 5 } },
                lineCol, INSET_RECT_FLAG_BORDER_INSET);
            curScreenPos += yLabelStepPx;
            curLabel -= yLabelStep;
        }

        DrawMonths<money64, kMoney64Undefined>(dpi, series, count, internalBounds, xStepPx);
        DrawLine<money64, kMoney64Undefined, true>(dpi, series, count, internalBounds, xStepPx, graphMinimum, graphMaximum);
        DrawLine<money64, kMoney64Undefined, false>(dpi, series, count, internalBounds, xStepPx, graphMinimum, graphMaximum);
        DrawHoveredValue<money64, kMoney64Undefined>(dpi, series, count, internalBounds, xStepPx, graphMinimum, graphMaximum);

        // todo debug code.
        ScreenCoordsXY bottomLeft{ internalBounds.Point1.x, internalBounds.Point2.y };
        ScreenCoordsXY topRight{ internalBounds.Point2.x, internalBounds.Point1.y };
        GfxDrawLine(dpi, { internalBounds.Point1, topRight }, 33);
        GfxDrawLine(dpi, { internalBounds.Point1, bottomLeft }, 33);
        GfxDrawLine(dpi, { bottomLeft, internalBounds.Point2 }, 33);
        GfxDrawLine(dpi, { topRight, internalBounds.Point2 }, 33);
    }
} // namespace OpenRCT2::Graph
