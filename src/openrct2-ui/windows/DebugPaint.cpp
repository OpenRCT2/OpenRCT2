/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/paint/Paint.h>
#include <openrct2/paint/tile_element/Paint.TileElement.h>
#include <openrct2/ride/TrackPaint.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    enum WindowDebugPaintWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TOGGLE_SHOW_WIDE_PATHS,
        WIDX_TOGGLE_SHOW_BLOCKED_TILES,
        WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS,
        WIDX_TOGGLE_SHOW_BOUND_BOXES,
        WIDX_TOGGLE_SHOW_DIRTY_VISUALS,
        WIDX_TOGGLE_STABLE_PAINT_SORT,
    };

    static constexpr ScreenSize kWindowSize = { 200, 8 + (15 * 6) + 8 };

    // clang-format off
    static constexpr Widget window_debug_paint_widgets[] = {
        makeWidget({0,          0}, kWindowSize,                   WidgetType::frame,    WindowColour::primary                                        ),
        makeWidget({8, 8 + 15 * 0}, {         185,            12}, WidgetType::checkbox, WindowColour::secondary, STR_DEBUG_PAINT_SHOW_WIDE_PATHS     ),
        makeWidget({8, 8 + 15 * 1}, {         185,            12}, WidgetType::checkbox, WindowColour::secondary, STR_DEBUG_PAINT_SHOW_BLOCKED_TILES  ),
        makeWidget({8, 8 + 15 * 2}, {         185,            12}, WidgetType::checkbox, WindowColour::secondary, STR_DEBUG_PAINT_SHOW_SEGMENT_HEIGHTS),
        makeWidget({8, 8 + 15 * 3}, {         185,            12}, WidgetType::checkbox, WindowColour::secondary, STR_DEBUG_PAINT_SHOW_BOUND_BOXES    ),
        makeWidget({8, 8 + 15 * 4}, {         185,            12}, WidgetType::checkbox, WindowColour::secondary, STR_DEBUG_PAINT_SHOW_DIRTY_VISUALS  ),
        makeWidget({8, 8 + 15 * 5}, {         185,            12}, WidgetType::checkbox, WindowColour::secondary, STR_DEBUG_PAINT_STABLE_SORT  ),
    };
    // clang-format on

    class DebugPaintWindow final : public Window
    {
    private:
        int32_t ResizeLanguage = LANGUAGE_UNDEFINED;

    public:
        void OnOpen() override
        {
            SetWidgets(window_debug_paint_widgets);

            InitScrollWidgets();
            WindowPushOthersBelow(*this);

            colours[0] = ColourWithFlags{ COLOUR_BLACK }.withFlag(ColourFlag::translucent, true);
            colours[1] = COLOUR_GREY;

            ResizeLanguage = LANGUAGE_UNDEFINED;
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_TOGGLE_SHOW_WIDE_PATHS:
                    gPaintWidePathsAsGhost = !gPaintWidePathsAsGhost;
                    GfxInvalidateScreen();
                    break;

                case WIDX_TOGGLE_SHOW_BLOCKED_TILES:
                    gPaintBlockedTiles = !gPaintBlockedTiles;
                    GfxInvalidateScreen();
                    break;

                case WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS:
                    gShowSupportSegmentHeights = !gShowSupportSegmentHeights;
                    GfxInvalidateScreen();
                    break;

                case WIDX_TOGGLE_SHOW_BOUND_BOXES:
                    gPaintBoundingBoxes = !gPaintBoundingBoxes;
                    GfxInvalidateScreen();
                    break;

                case WIDX_TOGGLE_SHOW_DIRTY_VISUALS:
                    gShowDirtyVisuals = !gShowDirtyVisuals;
                    GfxInvalidateScreen();
                    break;

                case WIDX_TOGGLE_STABLE_PAINT_SORT:
                    gPaintStableSort = !gPaintStableSort;
                    GfxInvalidateScreen();
                    break;
            }
        }

        void OnPrepareDraw() override
        {
            const auto& ls = OpenRCT2::GetContext()->GetLocalisationService();
            const auto currentLanguage = ls.GetCurrentLanguage();
            if (ResizeLanguage != currentLanguage)
            {
                ResizeLanguage = currentLanguage;
                Invalidate();

                // Find the width of the longest string
                int16_t newWidth = 0;
                for (size_t widgetIndex = WIDX_TOGGLE_SHOW_WIDE_PATHS; widgetIndex <= WIDX_TOGGLE_SHOW_DIRTY_VISUALS;
                     widgetIndex++)
                {
                    const auto& stringIdx = widgets[widgetIndex].text;
                    auto string = ls.GetString(stringIdx);
                    Guard::ArgumentNotNull(string);
                    const auto strWidth = GfxGetStringWidth(string, FontStyle::Medium);
                    newWidth = std::max<int16_t>(strWidth, newWidth);
                }

                // Add padding for both sides (8) and the offset for the text after the checkbox (15)
                newWidth += 8 * 2 + 15;

                width = newWidth;
                max_width = newWidth;
                min_width = newWidth;
                widgets[WIDX_BACKGROUND].right = newWidth - 1;
                widgets[WIDX_TOGGLE_SHOW_WIDE_PATHS].right = newWidth - 8;
                widgets[WIDX_TOGGLE_SHOW_BLOCKED_TILES].right = newWidth - 8;
                widgets[WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS].right = newWidth - 8;
                widgets[WIDX_TOGGLE_SHOW_BOUND_BOXES].right = newWidth - 8;
                widgets[WIDX_TOGGLE_SHOW_DIRTY_VISUALS].right = newWidth - 8;

                Invalidate();
            }

            widgetSetCheckboxValue(*this, WIDX_TOGGLE_SHOW_WIDE_PATHS, gPaintWidePathsAsGhost);
            widgetSetCheckboxValue(*this, WIDX_TOGGLE_SHOW_BLOCKED_TILES, gPaintBlockedTiles);
            widgetSetCheckboxValue(*this, WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS, gShowSupportSegmentHeights);
            widgetSetCheckboxValue(*this, WIDX_TOGGLE_SHOW_BOUND_BOXES, gPaintBoundingBoxes);
            widgetSetCheckboxValue(*this, WIDX_TOGGLE_SHOW_DIRTY_VISUALS, gShowDirtyVisuals);
            widgetSetCheckboxValue(*this, WIDX_TOGGLE_STABLE_PAINT_SORT, gPaintStableSort);
        }

        void OnDraw(RenderTarget& rt) override
        {
            DrawWidgets(rt);
        }
    };

    WindowBase* DebugPaintOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->FocusOrCreate<DebugPaintWindow>(
            WindowClass::DebugPaint, { 16, ContextGetHeight() - 16 - 33 - kWindowSize.height }, kWindowSize,
            WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_TITLE_BAR);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
