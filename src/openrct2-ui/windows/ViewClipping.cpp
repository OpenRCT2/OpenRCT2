/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cmath>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/paint/Paint.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Location.hpp>

namespace OpenRCT2::Ui::Windows
{
    enum WindowViewClippingWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CLIP_CHECKBOX_ENABLE,
        WIDX_GROUPBOX_VERTICAL,
        WIDX_CLIP_HEIGHT_VALUE,
        WIDX_CLIP_HEIGHT_INCREASE,
        WIDX_CLIP_HEIGHT_DECREASE,
        WIDX_CLIP_HEIGHT_SLIDER,
        WIDX_CLIP_SEE_THROUGH_CHECKBOX_ENABLE,
        WIDX_GROUPBOX_HORIZONTAL,
        WIDX_CLIP_SELECTOR,
        WIDX_CLIP_CLEAR,
    };

    enum class DisplayType
    {
        DisplayRaw,
        DisplayUnits
    };

#pragma region Widgets

    static constexpr StringId kWindowTitle = STR_VIEW_CLIPPING_TITLE;
    static constexpr ScreenSize kWindowSize = { 180, 172 };

    // clang-format off
    static constexpr auto _viewClippingWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget        ({     11,  19}, {    159,  11},                WidgetType::checkbox, WindowColour::primary, STR_VIEW_CLIPPING_HEIGHT_ENABLE,                  STR_VIEW_CLIPPING_HEIGHT_ENABLE_TIP               ), // clip enable/disable check box
        makeWidget        ({      5,  36}, {kWindowSize.width - 10,  65}, WidgetType::groupbox, WindowColour::primary, STR_VIEW_CLIPPING_VERTICAL_CLIPPING                                                                 ),
        makeSpinnerWidgets({     90,  51}, {     79,  12},                WidgetType::spinner,  WindowColour::primary, kStringIdNone,                                    STR_VIEW_CLIPPING_HEIGHT_VALUE_TOGGLE             ), // clip height (3 widgets)
        makeWidget        ({     11,  66}, {    158,  13},                WidgetType::scroll,   WindowColour::primary, SCROLL_HORIZONTAL,                                STR_VIEW_CLIPPING_HEIGHT_SCROLL_TIP               ), // clip height scrollbar
        makeWidget        ({     11,  83}, {    159,  11},                WidgetType::checkbox, WindowColour::primary, STR_VIEW_CLIPPING_VERTICAL_CLIPPING_SEE_THROUGH, STR_VIEW_CLIPPING_VERTICAL_CLIPPING_SEE_THROUGH_TIP), // clip height enable/disable see-through check box
        makeWidget        ({      5, 107}, {kWindowSize.width - 10,  60}, WidgetType::groupbox, WindowColour::primary, STR_VIEW_CLIPPING_HORIZONTAL_CLIPPING                                                               ),
        makeWidget        ({     11, 122}, {    158,  17},                WidgetType::button,   WindowColour::primary, STR_VIEW_CLIPPING_SELECT_AREA                                                                       ), // selector
        makeWidget        ({     11, 143}, {    158,  18},                WidgetType::button,   WindowColour::primary, STR_VIEW_CLIPPING_CLEAR_SELECTION                                                                   )  // clear
    );
    // clang-format on

#pragma endregion

    class ViewClippingWindow final : public Window
    {
    private:
        CoordsXY _selectionStart;
        CoordsXY _previousClipSelectionA;
        CoordsXY _previousClipSelectionB;
        bool _toolActive{ false };
        bool _dragging{ false };
        static inline DisplayType _clipHeightDisplayType;

    public:
        void OnCloseButton()
        {
            OnClose();
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            // mouseup appears to be used for buttons, checkboxes
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_CLIP_CHECKBOX_ENABLE:
                {
                    // Toggle height clipping.
                    WindowBase* mainWindow = WindowGetMain();
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
                    ToolSet(*this, WIDX_BACKGROUND, Tool::crosshair);
                    _toolActive = true;
                    _dragging = false;

                    // Reset clip selection to show all tiles
                    _previousClipSelectionA = gClipSelectionA;
                    _previousClipSelectionB = gClipSelectionB;
                    gClipSelectionA = { 0, 0 };
                    gClipSelectionB = { kMaximumMapSizeBig - 1, kMaximumMapSizeBig - 1 };
                    GfxInvalidateScreen();
                    break;
                case WIDX_CLIP_CLEAR:
                    if (IsActive())
                    {
                        ToolCancel();
                        _toolActive = false;
                    }
                    gClipSelectionA = { 0, 0 };
                    gClipSelectionB = { kMaximumMapSizeBig - 1, kMaximumMapSizeBig - 1 };
                    GfxInvalidateScreen();
                    break;
                case WIDX_CLIP_SEE_THROUGH_CHECKBOX_ENABLE:
                {
                    // Toggle height clipping see-through.
                    if (auto mainWindow = WindowGetMain(); mainWindow != nullptr)
                    {
                        mainWindow->viewport->flags ^= VIEWPORT_FLAG_CLIP_VIEW_SEE_THROUGH;
                        mainWindow->Invalidate();
                    }
                    Invalidate();
                    break;
                }
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            WindowBase* mainWindow;

            switch (widgetIndex)
            {
                case WIDX_CLIP_HEIGHT_INCREASE:
                    if (gClipHeight < 255)
                        SetClipHeight(gClipHeight + 1);
                    mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                        mainWindow->Invalidate();
                    break;
                case WIDX_CLIP_HEIGHT_DECREASE:
                    if (gClipHeight > 0)
                        SetClipHeight(gClipHeight - 1);
                    mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                        mainWindow->Invalidate();
                    break;
            }
        }

        void OnUpdate() override
        {
            const auto& widget = widgets[WIDX_CLIP_HEIGHT_SLIDER];
            const ScrollArea* const scroll = &this->scrolls[0];
            const int16_t scroll_width = widget.width() - 1;
            const uint8_t clip_height = static_cast<uint8_t>(
                (static_cast<float>(scroll->contentOffsetX) / (scroll->contentWidth - scroll_width)) * 255);
            if (clip_height != gClipHeight)
            {
                gClipHeight = clip_height;

                // Update the main window accordingly.
                WindowBase* mainWindow = WindowGetMain();
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

            InvalidateWidget(WIDX_CLIP_HEIGHT_SLIDER);
        }

        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (_dragging)
            {
                return;
            }

            int32_t direction;
            auto mapCoords = ScreenPosToMapPos(screenCoords, &direction);
            if (mapCoords.has_value())
            {
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                MapInvalidateTileFull(gMapSelectPositionA);
                gMapSelectPositionA = gMapSelectPositionB = mapCoords.value();
                MapInvalidateTileFull(mapCoords.value());
                gMapSelectType = MAP_SELECT_TYPE_FULL;
            }
        }

        void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            int32_t direction;
            auto mapCoords = ScreenPosToMapPos(screenCoords, &direction);
            if (mapCoords.has_value())
            {
                _dragging = true;
                _selectionStart = mapCoords.value();
            }
        }

        void OnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (!_dragging)
            {
                return;
            }

            int32_t direction;
            auto mapCoords = ScreenPosToMapPos(screenCoords, &direction);
            if (mapCoords)
            {
                MapInvalidateSelectionRect();
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                gMapSelectPositionA.x = std::min(_selectionStart.x, mapCoords->x);
                gMapSelectPositionB.x = std::max(_selectionStart.x, mapCoords->x);
                gMapSelectPositionA.y = std::min(_selectionStart.y, mapCoords->y);
                gMapSelectPositionB.y = std::max(_selectionStart.y, mapCoords->y);
                gMapSelectType = MAP_SELECT_TYPE_FULL;
                MapInvalidateSelectionRect();
            }
        }

        void OnToolUp(WidgetIndex, const ScreenCoordsXY&) override
        {
            gClipSelectionA = gMapSelectPositionA;
            gClipSelectionB = gMapSelectPositionB;
            _toolActive = false;
            ToolCancel();
            GfxInvalidateScreen();
        }

        void OnPrepareDraw() override
        {
            widgetScrollUpdateThumbs(*this, WIDX_CLIP_HEIGHT_SLIDER);

            WindowBase* mainWindow = WindowGetMain();
            if (mainWindow != nullptr)
            {
                widgetSetCheckboxValue(*this, WIDX_CLIP_CHECKBOX_ENABLE, mainWindow->viewport->flags & VIEWPORT_FLAG_CLIP_VIEW);
                widgetSetCheckboxValue(
                    *this, WIDX_CLIP_SEE_THROUGH_CHECKBOX_ENABLE,
                    mainWindow->viewport->flags & VIEWPORT_FLAG_CLIP_VIEW_SEE_THROUGH);
            }

            if (IsActive())
            {
                this->pressed_widgets |= 1uLL << WIDX_CLIP_SELECTOR;
            }
            else
            {
                this->pressed_widgets &= ~(1uLL << WIDX_CLIP_SELECTOR);
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            WindowDrawWidgets(*this, rt);

            // Clip height value
            auto screenCoords = this->windowPos + ScreenCoordsXY{ 8, this->widgets[WIDX_CLIP_HEIGHT_VALUE].top };
            DrawTextBasic(rt, screenCoords, STR_VIEW_CLIPPING_HEIGHT_VALUE, {}, { this->colours[0] });

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
                    DrawTextBasic(rt, screenCoords, STR_FORMAT_INTEGER, ft, { this->colours[0] });
                    break;
                }
                case DisplayType::DisplayUnits:
                {
                    // Print the value in the configured height label type:
                    auto ft = Formatter();
                    if (Config::Get().general.ShowHeightAsUnits)
                    {
                        ft.Add<fixed32_2dp>(FIXED_2DP(gClipHeight, 0) / 2 - FIXED_2DP(7, 0));
                        DrawTextBasic(rt, screenCoords, STR_UNIT2DP_NO_SUFFIX, ft, { this->colours[0] });
                    }
                    else
                    {
                        switch (Config::Get().general.MeasurementFormat)
                        {
                            case MeasurementFormat::Imperial:
                            {
                                ft.Add<fixed32_2dp>(FIXED_2DP(gClipHeight, 0) / 2.0f * 5.0f - FIXED_2DP(35, 0));
                                DrawTextBasic(rt, screenCoords, STR_UNIT2DP_SUFFIX_FEET, ft, { this->colours[0] });
                                break;
                            }
                            case MeasurementFormat::Metric:
                            case MeasurementFormat::SI:
                            {
                                ft.Add<fixed32_2dp>(FIXED_2DP(gClipHeight, 0) / 2.0f * 1.5f - FIXED_2DP(10, 50));
                                DrawTextBasic(rt, screenCoords, STR_UNIT2DP_SUFFIX_METRES, ft, { this->colours[0] });
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
            SetWidgets(_viewClippingWidgets);

            this->hold_down_widgets = (1uLL << WIDX_CLIP_HEIGHT_INCREASE) | (1uL << WIDX_CLIP_HEIGHT_DECREASE);
            WindowInitScrollWidgets(*this);

            _clipHeightDisplayType = DisplayType::DisplayUnits;

            // Initialise the clip height slider from the current clip height value.
            this->SetClipHeight(gClipHeight);

            WindowPushOthersBelow(*this);

            // Get the main viewport to set the view clipping flag.
            WindowBase* mainWindow = WindowGetMain();

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
            WindowBase* mainWindow = WindowGetMain();
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
            this->scrolls[0].contentOffsetX = static_cast<int16_t>(
                std::ceil(clip_height_ratio * (this->scrolls[0].contentWidth - (widget.width() - 1))));
        }

        bool IsActive()
        {
            return isToolActive(WindowClass::ViewClipping);
        }
    };

    WindowBase* ViewClippingOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::ViewClipping);
        if (window == nullptr)
        {
            window = windowMgr->Create<ViewClippingWindow>(WindowClass::ViewClipping, ScreenCoordsXY(32, 32), kWindowSize);
        }
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
