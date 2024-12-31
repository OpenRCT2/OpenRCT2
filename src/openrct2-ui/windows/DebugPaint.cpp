/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/paint/Paint.h>
#include <openrct2/paint/tile_element/Paint.TileElement.h>
#include <openrct2/ride/TrackPaint.h>

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

    constexpr int32_t WINDOW_WIDTH = 200;
    constexpr int32_t WINDOW_HEIGHT = 8 + (15 * 6) + 8;

    // clang-format off
    static Widget window_debug_paint_widgets[] = {
        MakeWidget({0,          0}, {WINDOW_WIDTH, WINDOW_HEIGHT}, WindowWidgetType::Frame,    WindowColour::Primary                                        ),
        MakeWidget({8, 8 + 15 * 0}, {         185,            12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_DEBUG_PAINT_SHOW_WIDE_PATHS     ),
        MakeWidget({8, 8 + 15 * 1}, {         185,            12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_DEBUG_PAINT_SHOW_BLOCKED_TILES  ),
        MakeWidget({8, 8 + 15 * 2}, {         185,            12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_DEBUG_PAINT_SHOW_SEGMENT_HEIGHTS),
        MakeWidget({8, 8 + 15 * 3}, {         185,            12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_DEBUG_PAINT_SHOW_BOUND_BOXES    ),
        MakeWidget({8, 8 + 15 * 4}, {         185,            12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_DEBUG_PAINT_SHOW_DIRTY_VISUALS  ),
        MakeWidget({8, 8 + 15 * 5}, {         185,            12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_DEBUG_PAINT_STABLE_SORT  ),
        kWidgetsEnd,
    };
    // clang-format on

    class DebugPaintWindow final : public Window
    {
    private:
        int32_t ResizeLanguage = LANGUAGE_UNDEFINED;

    public:
        void OnOpen() override
        {
            widgets = window_debug_paint_widgets;

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

            WidgetSetCheckboxValue(*this, WIDX_TOGGLE_SHOW_WIDE_PATHS, gPaintWidePathsAsGhost);
            WidgetSetCheckboxValue(*this, WIDX_TOGGLE_SHOW_BLOCKED_TILES, gPaintBlockedTiles);
            WidgetSetCheckboxValue(*this, WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS, gShowSupportSegmentHeights);
            WidgetSetCheckboxValue(*this, WIDX_TOGGLE_SHOW_BOUND_BOXES, gPaintBoundingBoxes);
            WidgetSetCheckboxValue(*this, WIDX_TOGGLE_SHOW_DIRTY_VISUALS, gShowDirtyVisuals);
            WidgetSetCheckboxValue(*this, WIDX_TOGGLE_STABLE_PAINT_SORT, gPaintStableSort);
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);
        }
    };

    WindowBase* DebugPaintOpen()
    {
        auto* window = WindowFocusOrCreate<DebugPaintWindow>(
            WindowClass::DebugPaint, { 16, ContextGetHeight() - 16 - 33 - WINDOW_HEIGHT }, WINDOW_WIDTH, WINDOW_HEIGHT,
            WF_STICK_TO_FRONT | WF_TRANSPARENT);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
