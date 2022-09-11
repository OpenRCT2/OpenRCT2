/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <cmath>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Input.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/paint/Paint.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Location.hpp>

// clang-format off
enum WindowViewClippingWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_CLIP_CHECKBOX_ENABLE,
    WIDX_GROUPBOX_VERTICAL,
    WIDX_CLIP_HEIGHT_VALUE,
    WIDX_CLIP_HEIGHT_INCREASE,
    WIDX_CLIP_HEIGHT_DECREASE,
    WIDX_CLIP_HEIGHT_SLIDER,
    WIDX_GROUPBOX_HORIZONTAL,
    WIDX_CLIP_SELECTOR,
    WIDX_CLIP_CLEAR,
};

enum class DisplayType {
    DisplayRaw,
    DisplayUnits
};

#pragma region Widgets

static constexpr const rct_string_id WINDOW_TITLE = STR_VIEW_CLIPPING_TITLE;
static constexpr const int32_t WW = 180;
static constexpr const int32_t WH = 155;

static rct_widget window_view_clipping_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget        ({     11,  19}, {    159,  11}, WindowWidgetType::Checkbox, WindowColour::Primary, STR_VIEW_CLIPPING_HEIGHT_ENABLE,       STR_VIEW_CLIPPING_HEIGHT_ENABLE_TIP  ), // clip enable/disable check box
    MakeWidget        ({      5,  36}, {WW - 10,  48}, WindowWidgetType::Groupbox, WindowColour::Primary, STR_VIEW_CLIPPING_VERTICAL_CLIPPING                                         ),
    MakeSpinnerWidgets({     90,  51}, {     79,  12}, WindowWidgetType::Spinner,  WindowColour::Primary, STR_NONE,                              STR_VIEW_CLIPPING_HEIGHT_VALUE_TOGGLE), // clip height (3 widgets)
    MakeWidget        ({     11,  66}, {    158,  13}, WindowWidgetType::Scroll,   WindowColour::Primary, SCROLL_HORIZONTAL,                     STR_VIEW_CLIPPING_HEIGHT_SCROLL_TIP  ), // clip height scrollbar
    MakeWidget        ({      5,  90}, {WW - 10,  60}, WindowWidgetType::Groupbox, WindowColour::Primary, STR_VIEW_CLIPPING_HORIZONTAL_CLIPPING                                       ),
    MakeWidget        ({     11, 105}, {    158,  17}, WindowWidgetType::Button,   WindowColour::Primary, STR_VIEW_CLIPPING_SELECT_AREA                                               ), // selector
    MakeWidget        ({     11, 126}, {    158,  18}, WindowWidgetType::Button,   WindowColour::Primary, STR_VIEW_CLIPPING_CLEAR_SELECTION                                           ), // clear

    WIDGETS_END,
};

#pragma endregion

// clang-format on
class ViewClippingWindow final : public Window
{
private:
    CoordsXY _selectionStart;
    CoordsXY _previousClipSelectionA = { 0, 0 };
    CoordsXY _previousClipSelectionB = gMapSize.ToCoordsXY();
    bool _toolActive{ false };
    bool _dragging{ false };
    bool _firstStart = true;
    static inline DisplayType _clipHeightDisplayType;

public:
    void OnCloseButton()
    {
        OnClose();
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        // mouseup appears to be used for buttons, checkboxes
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                window_close(this);
                break;
            case WIDX_CLIP_CHECKBOX_ENABLE:
            {
                // Toggle height clipping.
                rct_window* mainWindow = window_get_main();
                if (mainWindow != nullptr)
                {
                    mainWindow->viewport->flags ^= VIEWPORT_FLAG_CLIP_VIEW;
                    mainWindow->Invalidate();
                }
                this->Invalidate();
                break;
            }
            case WIDX_CLIP_HEIGHT_VALUE:
                // Toggle display of the cut height value in RAW vs UNITS
                if (_clipHeightDisplayType == DisplayType::DisplayRaw)
                {
                    _clipHeightDisplayType = DisplayType::DisplayUnits;
                }
                else
                {
                    _clipHeightDisplayType = DisplayType::DisplayRaw;
                }
                this->Invalidate();
                break;
            case WIDX_CLIP_SELECTOR:
                // Activate the selection tool
                tool_set(this, WIDX_BACKGROUND, Tool::Crosshair);
                _toolActive = true;
                _dragging = false;

                // Reset clip selection to show all tiles
                _previousClipSelectionA = gClipSelectionA;
                _previousClipSelectionB = gClipSelectionB;
                gClipSelectionA = { 0, 0 };
                gClipSelectionB = { gMapSize.x * 32, gMapSize.y * 32 };
                gfx_invalidate_screen();
                break;
            case WIDX_CLIP_CLEAR:
                if (IsActive())
                {
                    _toolActive = false;
                    tool_cancel();
                }
                gClipSelectionA = { 0, 0 };
                gClipSelectionB = { gMapSize.x * 32, gMapSize.y * 32 };
                gfx_invalidate_screen();
                break;
        }
    }

    void OnMouseDown(rct_widgetindex widgetIndex) override
    {
        rct_window* mainWindow;

        switch (widgetIndex)
        {
            case WIDX_CLIP_HEIGHT_INCREASE:
                if (gClipHeight < 255)
                    SetClipHeight(gClipHeight + 1);
                mainWindow = window_get_main();
                if (mainWindow != nullptr)
                    mainWindow->Invalidate();
                break;
            case WIDX_CLIP_HEIGHT_DECREASE:
                if (gClipHeight > 0)
                    SetClipHeight(gClipHeight - 1);
                mainWindow = window_get_main();
                if (mainWindow != nullptr)
                    mainWindow->Invalidate();
                break;
        }
    }

    void OnUpdate() override
    {
        const auto& widget = widgets[WIDX_CLIP_HEIGHT_SLIDER];
        const rct_scroll* const scroll = &this->scrolls[0];
        const int16_t scroll_width = widget.width() - 1;
        const uint8_t clip_height = static_cast<uint8_t>(
            (static_cast<float>(scroll->h_left) / (scroll->h_right - scroll_width)) * 255);
        if (clip_height != gClipHeight)
        {
            gClipHeight = clip_height;

            // Update the main window accordingly.
            rct_window* mainWindow = window_get_main();
            if (mainWindow != nullptr)
            {
                mainWindow->Invalidate();
            }
        }

        // Restore previous selection if the tool has been interrupted
        if (_toolActive && !IsActive())
        {
            _toolActive = false;
            gClipSelectionA = _previousClipSelectionA;
            gClipSelectionB = _previousClipSelectionB;
        }

        widget_invalidate(this, WIDX_CLIP_HEIGHT_SLIDER);
    }

    void OnToolUpdate(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (_dragging)
        {
            return;
        }

        int32_t direction;
        auto mapCoords = screen_pos_to_map_pos(screenCoords, &direction);
        if (mapCoords.has_value())
        {
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
            map_invalidate_tile_full(gMapSelectPositionA);
            gMapSelectPositionA = gMapSelectPositionB = mapCoords.value();
            map_invalidate_tile_full(mapCoords.value());
            gMapSelectType = MAP_SELECT_TYPE_FULL;
        }
    }

    void OnToolDown(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        int32_t direction;
        auto mapCoords = screen_pos_to_map_pos(screenCoords, &direction);
        if (mapCoords.has_value())
        {
            _dragging = true;
            _selectionStart = mapCoords.value();
        }
    }

    void OnToolDrag(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (!_dragging)
        {
            return;
        }

        int32_t direction;
        auto mapCoords = screen_pos_to_map_pos(screenCoords, &direction);
        if (mapCoords)
        {
            map_invalidate_selection_rect();
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
            gMapSelectPositionA.x = std::min(_selectionStart.x, mapCoords->x);
            gMapSelectPositionB.x = std::max(_selectionStart.x, mapCoords->x);
            gMapSelectPositionA.y = std::min(_selectionStart.y, mapCoords->y);
            gMapSelectPositionB.y = std::max(_selectionStart.y, mapCoords->y);
            gMapSelectType = MAP_SELECT_TYPE_FULL;
            map_invalidate_selection_rect();
        }
    }

    void OnToolUp(rct_widgetindex, const ScreenCoordsXY&) override
    {
        gClipSelectionA = gMapSelectPositionA;
        gClipSelectionB = gMapSelectPositionB;
        _toolActive = false;
        tool_cancel();
        gfx_invalidate_screen();
    }

    void OnPrepareDraw() override
    {
        WidgetScrollUpdateThumbs(this, WIDX_CLIP_HEIGHT_SLIDER);

        rct_window* mainWindow = window_get_main();
        if (mainWindow != nullptr)
        {
            WidgetSetCheckboxValue(this, WIDX_CLIP_CHECKBOX_ENABLE, mainWindow->viewport->flags & VIEWPORT_FLAG_CLIP_VIEW);
        }

        if (IsActive())
        {
            this->pressed_widgets |= 1ULL << WIDX_CLIP_SELECTOR;
        }
        else
        {
            this->pressed_widgets &= ~(1ULL << WIDX_CLIP_SELECTOR);
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        WindowDrawWidgets(this, &dpi);

        // Clip height value
        auto screenCoords = this->windowPos + ScreenCoordsXY{ 8, this->widgets[WIDX_CLIP_HEIGHT_VALUE].top };
        DrawTextBasic(&dpi, screenCoords, STR_VIEW_CLIPPING_HEIGHT_VALUE, {}, { this->colours[0] });

        screenCoords = this->windowPos
            + ScreenCoordsXY{ this->widgets[WIDX_CLIP_HEIGHT_VALUE].left + 1, this->widgets[WIDX_CLIP_HEIGHT_VALUE].top };

        switch (_clipHeightDisplayType)
        {
            case DisplayType::DisplayRaw:
            default:
            {
                auto ft = Formatter();
                ft.Add<int32_t>(static_cast<int32_t>(gClipHeight));

                // Printing the raw value.
                DrawTextBasic(&dpi, screenCoords, STR_FORMAT_INTEGER, ft, { this->colours[0] });
                break;
            }
            case DisplayType::DisplayUnits:
            {
                // Print the value in the configured height label type:
                if (gConfigGeneral.show_height_as_units)
                {
                    // Height label is Units.
                    auto ft = Formatter();
                    ft.Add<fixed16_1dp>(static_cast<fixed16_1dp>(FIXED_1DP(gClipHeight, 0) / 2 - FIXED_1DP(7, 0)));
                    DrawTextBasic(
                        &dpi, screenCoords, STR_UNIT1DP_NO_SUFFIX, ft,
                        { this->colours[0] }); // Printing the value in Height Units.
                }
                else
                {
                    // Height label is Real Values.
                    // Print the value in the configured measurement units.
                    switch (gConfigGeneral.measurement_format)
                    {
                        case MeasurementFormat::Metric:
                        case MeasurementFormat::SI:
                        {
                            auto ft = Formatter();
                            ft.Add<fixed32_2dp>(
                                static_cast<fixed32_2dp>(FIXED_2DP(gClipHeight, 0) / 2 * 1.5f - FIXED_2DP(10, 50)));
                            DrawTextBasic(&dpi, screenCoords, STR_UNIT2DP_SUFFIX_METRES, ft, { this->colours[0] });
                            break;
                        }
                        case MeasurementFormat::Imperial:
                        {
                            auto ft = Formatter();
                            ft.Add<fixed16_1dp>(
                                static_cast<fixed16_1dp>(FIXED_1DP(gClipHeight, 0) / 2.0f * 5 - FIXED_1DP(35, 0)));
                            DrawTextBasic(&dpi, screenCoords, STR_UNIT1DP_SUFFIX_FEET, ft, { this->colours[0] });
                            break;
                        }
                    }
                }
            }
        }
    }

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        return { 1000, 0 };
    }

    void OnOpen() override
    {
        if (_firstStart)
        {
            _firstStart = false;
            gClipSelectionA = { 0, 0 };
            gClipSelectionB = { gMapSize.x * 32, gMapSize.y * 32 };
        }

        this->widgets = window_view_clipping_widgets;
        this->hold_down_widgets = (1ULL << WIDX_CLIP_HEIGHT_INCREASE) | (1UL << WIDX_CLIP_HEIGHT_DECREASE);
        WindowInitScrollWidgets(this);

        _clipHeightDisplayType = DisplayType::DisplayUnits;

        // Initialise the clip height slider from the current clip height value.
        this->SetClipHeight(gClipHeight);

        window_push_others_below(this);

        // Get the main viewport to set the view clipping flag.
        rct_window* mainWindow = window_get_main();

        // Turn on view clipping when the window is opened.
        if (mainWindow != nullptr)
        {
            mainWindow->viewport->flags |= VIEWPORT_FLAG_CLIP_VIEW;
            mainWindow->Invalidate();
        }
    }

private:
    void OnClose() override
    {
        // Turn off view clipping when the window is closed.
        rct_window* mainWindow = window_get_main();
        if (mainWindow != nullptr)
        {
            mainWindow->viewport->flags &= ~VIEWPORT_FLAG_CLIP_VIEW;
            mainWindow->Invalidate();
        }
    }

    void SetClipHeight(const uint8_t clipHeight)
    {
        gClipHeight = clipHeight;
        const auto& widget = widgets[WIDX_CLIP_HEIGHT_SLIDER];
        const float clip_height_ratio = static_cast<float>(gClipHeight) / 255;
        this->scrolls[0].h_left = static_cast<int16_t>(
            std::ceil(clip_height_ratio * (this->scrolls[0].h_right - (widget.width() - 1))));
    }

    bool IsActive()
    {
        if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
            return false;
        if (gCurrentToolWidget.window_classification != WC_VIEW_CLIPPING)
            return false;
        return _toolActive;
    }
};

rct_window* WindowViewClippingOpen()
{
    auto* window = window_bring_to_front_by_class(WC_VIEW_CLIPPING);
    if (window == nullptr)
    {
        window = WindowCreate<ViewClippingWindow>(WC_VIEW_CLIPPING, ScreenCoordsXY(32, 32), WW, WH);
    }
    return window;
}
