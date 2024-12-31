/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/Context.h>
#include <openrct2/core/Money.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/world/Location.hpp>

namespace OpenRCT2::Graph
{
    constexpr int32_t kYTickMarkPadding = 8;

    template<typename T>
    struct GraphProperties
    {
        ScreenRect internalBounds;
        const T* series;
        T min;
        T max;
        int32_t hoverIdx;
        int32_t numPoints;
        int32_t numYLabels;
        int32_t xStepPx;
        int32_t yLabelStepPx;
        ColourWithFlags lineCol;

        void RecalculateLayout(const ScreenRect newBounds, const int32_t newNumYLabels, const int32_t newNumPoints)
        {
            yLabelStepPx = (newBounds.GetBottom() - newBounds.GetTop()) / (newNumYLabels - 1);
            xStepPx = (newBounds.GetRight() - newBounds.GetLeft()) / (newNumPoints - 1);
            // adjust bounds to be exact multiples of the steps.
            internalBounds = { newBounds.Point1,
                               newBounds.Point1
                                   + ScreenCoordsXY{ xStepPx * (newNumPoints - 1), yLabelStepPx * (newNumYLabels - 1) } };
            numPoints = newNumPoints;
            numYLabels = newNumYLabels;
        }

        bool UpdateHoverIndex()
        {
            const ScreenCoordsXY cursorPos = ContextGetCursorPositionScaled();

            int32_t i = -1;
            if (internalBounds.Contains(cursorPos))
            {
                i = (numPoints - 1) - (cursorPos.x - internalBounds.GetLeft() + (xStepPx / 2)) / xStepPx;
                if (i < 0)
                    i = 1;
                if (i > numPoints - 1)
                    i = numPoints - 1;
            }

            if (hoverIdx != i)
            {
                hoverIdx = i;
                return true;
            }
            return false;
        }
    };

    void DrawFinanceGraph(DrawPixelInfo& dpi, const GraphProperties<money64>& p);
    void DrawRatingGraph(DrawPixelInfo& dpi, const GraphProperties<uint16_t>& p);
    void DrawGuestGraph(DrawPixelInfo& dpi, const GraphProperties<uint32_t>& p);
} // namespace OpenRCT2::Graph
