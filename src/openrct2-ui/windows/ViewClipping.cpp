/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/localisation/StringIds.h>
#include <openrct2/paint/Paint.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Location.hpp>
#include <openrct2/world/Map.h>
#include <openrct2/world/MapSelection.h>

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
        void onCloseButton()
        {
            onClose();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            // mouseup appears to be used for buttons, checkboxes
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_CLIP_CHECKBOX_ENABLE:
                {
                    // Toggle height clipping.
                    WindowBase* mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                    {
                        mainWindow->viewport->flags ^= VIEWPORT_FLAG_CLIP_VIEW;
                        mainWindow->invalidate();
                    }
                    this->invalidate();
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
                    this->invalidate();
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
                        mainWindow->invalidate();
                    }
                    invalidate();
                    break;
                }
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            WindowBase* mainWindow;

            switch (widgetIndex)
            {
                case WIDX_CLIP_HEIGHT_INCREASE:
                    if (gClipHeight < 255)
                        SetClipHeight(gClipHeight + 1);
                    mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                        mainWindow->invalidate();
                    break;
                case WIDX_CLIP_HEIGHT_DECREASE:
                    if (gClipHeight > 0)
                        SetClipHeight(gClipHeight - 1);
                    mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                        mainWindow->invalidate();
                    break;
            }
        }

        void onUpdate() override
        {
            const auto& widget = widgets[WIDX_CLIP_HEIGHT_SLIDER];
            const ScrollArea* const scroll = &this->scrolls[0];
            const int16_t scroll_width = widget.width() - 2;
            const uint8_t clip_height = static_cast<uint8_t>(
                (static_cast<float>(scroll->contentOffsetX) / (scroll->contentWidth - scroll_width)) * 255);
            if (clip_height != gClipHeight)
            {
                gClipHeight = clip_height;

                // Update the main window accordingly.
                WindowBase* mainWindow = WindowGetMain();
                if (mainWindow != nullptr)
                {
                    mainWindow->invalidate();
                }
            }

            // Restore previous selection if the tool has been interrupted
            if (_toolActive && !IsActive())
            {
                _toolActive = false;
                gClipSelectionA = _previousClipSelectionA;
                gClipSelectionB = _previousClipSelectionB;
            }

            invalidateWidget(WIDX_CLIP_HEIGHT_SLIDER);
        }

        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (_dragging)
            {
                return;
            }

            int32_t direction;
            auto mapCoords = ScreenPosToMapPos(screenCoords, &direction);
            if (mapCoords.has_value())
            {
                gMapSelectFlags.set(MapSelectFlag::enable);
                MapInvalidateTileFull(gMapSelectPositionA);
                gMapSelectPositionA = gMapSelectPositionB = mapCoords.value();
                MapInvalidateTileFull(mapCoords.value());
                gMapSelectType = MapSelectType::full;
            }
        }

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            int32_t direction;
            auto mapCoords = ScreenPosToMapPos(screenCoords, &direction);
            if (mapCoords.has_value())
            {
                _dragging = true;
                _selectionStart = mapCoords.value();
            }
        }

        void onToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (!_dragging)
            {
                return;
            }

            int32_t direction;
            auto mapCoords = ScreenPosToMapPos(screenCoords, &direction);
            if (mapCoords)
            {
                gMapSelectFlags.set(MapSelectFlag::enable);
                gMapSelectPositionA.x = std::min(_selectionStart.x, mapCoords->x);
                gMapSelectPositionB.x = std::max(_selectionStart.x, mapCoords->x);
                gMapSelectPositionA.y = std::min(_selectionStart.y, mapCoords->y);
                gMapSelectPositionB.y = std::max(_selectionStart.y, mapCoords->y);
                gMapSelectType = MapSelectType::full;
            }
        }

        void onToolUp(WidgetIndex, const ScreenCoordsXY&) override
        {
            gClipSelectionA = gMapSelectPositionA;
            gClipSelectionB = gMapSelectPositionB;
            _toolActive = false;
            ToolCancel();
            GfxInvalidateScreen();
        }

        void onPrepareDraw() override
        {
            widgetScrollUpdateThumbs(*this, WIDX_CLIP_HEIGHT_SLIDER);

            WindowBase* mainWindow = WindowGetMain();
            if (mainWindow != nullptr)
            {
                setCheckboxValue(WIDX_CLIP_CHECKBOX_ENABLE, mainWindow->viewport->flags & VIEWPORT_FLAG_CLIP_VIEW);
                setCheckboxValue(
                    WIDX_CLIP_SEE_THROUGH_CHECKBOX_ENABLE, mainWindow->viewport->flags & VIEWPORT_FLAG_CLIP_VIEW_SEE_THROUGH);
            }

            if (IsActive())
            {
                this->pressedWidgets |= 1uLL << WIDX_CLIP_SELECTOR;
            }
            else
            {
                this->pressedWidgets &= ~(1uLL << WIDX_CLIP_SELECTOR);
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
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
                    if (Config::Get().general.showHeightAsUnits)
                    {
                        // Height label is Units.
                        auto ft = Formatter();
                        ft.Add<fixed16_1dp>((MakeFixed1dp<fixed16_1dp>(gClipHeight, 0) / 2 - MakeFixed1dp<fixed16_1dp>(7, 0)));
                        DrawTextBasic(
                            rt, screenCoords, STR_UNIT1DP_NO_SUFFIX, ft,
                            { this->colours[0] }); // Printing the value in Height Units.
                    }
                    else
                    {
                        // Height label is Real Values.
                        // Print the value in the configured measurement units.
                        switch (Config::Get().general.measurementFormat)
                        {
                            case MeasurementFormat::Metric:
                            case MeasurementFormat::SI:
                            {
                                auto ft = Formatter();
                                ft.Add<fixed32_2dp>(
                                    MakeFixed2dp<fixed32_2dp>(gClipHeight, 0) / 2 * 1.5f - MakeFixed2dp<fixed32_2dp>(10, 50));
                                DrawTextBasic(rt, screenCoords, STR_UNIT2DP_SUFFIX_METRES, ft, { this->colours[0] });
                                break;
                            }
                            case MeasurementFormat::Imperial:
                            {
                                auto ft = Formatter();
                                ft.Add<fixed16_1dp>(
                                    MakeFixed1dp<fixed16_1dp>(gClipHeight, 0) / 2.0f * 5 - MakeFixed1dp<fixed16_1dp>(35, 0));
                                DrawTextBasic(rt, screenCoords, STR_UNIT1DP_SUFFIX_FEET, ft, { this->colours[0] });
                                break;
                            }
                        }
                    }
                }
            }
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            return { 1000, 0 };
        }

        void onOpen() override
        {
            setWidgets(_viewClippingWidgets);

            this->holdDownWidgets = (1uLL << WIDX_CLIP_HEIGHT_INCREASE) | (1uL << WIDX_CLIP_HEIGHT_DECREASE);
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
                mainWindow->invalidate();
            }
        }

    private:
        void onClose() override
        {
            // Turn off view clipping when the window is closed.
            WindowBase* mainWindow = WindowGetMain();
            if (mainWindow != nullptr)
            {
                mainWindow->viewport->flags &= ~VIEWPORT_FLAG_CLIP_VIEW;
                mainWindow->invalidate();
            }
        }

        void SetClipHeight(const uint8_t clipHeight)
        {
            gClipHeight = clipHeight;
            const auto& widget = widgets[WIDX_CLIP_HEIGHT_SLIDER];
            const float clip_height_ratio = static_cast<float>(gClipHeight) / 255;
            this->scrolls[0].contentOffsetX = static_cast<int16_t>(
                std::ceil(clip_height_ratio * (this->scrolls[0].contentWidth - (widget.width() - 2))));
        }

        bool IsActive()
        {
            return isToolActive(WindowClass::viewClipping);
        }
    };

    WindowBase* ViewClippingOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::viewClipping);
        if (window == nullptr)
        {
            window = windowMgr->Create<ViewClippingWindow>(WindowClass::viewClipping, ScreenCoordsXY(32, 32), kWindowSize);
        }
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
