/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include <SDL_clipboard.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/Version.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/fireworks/FireworksSequence.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/UiContext.h>

using namespace OpenRCT2;

static constexpr const int32_t WW = 400;
static constexpr const int32_t WH = 352;
static constexpr const StringId WINDOW_TITLE = STR_FIREWORKS_EDITOR;
static constexpr const int32_t TABHEIGHT = 50;

// clang-format off

enum WindowAboutWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_SPINNER_X,
    WIDX_SPINNER_X_INCREASE,
    WIDX_SPINNER_X_DECREASE,
    WIDX_SPINNER_Y,
    WIDX_SPINNER_Y_INCREASE,
    WIDX_SPINNER_Y_DECREASE,
    WIDX_SPINNER_HEIGHT,
    WIDX_SPINNER_HEIGHT_INCREASE,
    WIDX_SPINNER_HEIGHT_DECREASE,
    WIDX_SPINNER_DELAY,
    WIDX_SPINNER_DELAY_INCREASE,
    WIDX_SPINNER_DELAY_DECREASE,
};

#define WIDGETS_MAIN \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeSpinnerWidgets({20, 23}, {51, 12}, WindowWidgetType::Spinner, WindowColour::Secondary), /* Spinner X*/ \
    MakeSpinnerWidgets({90, 23}, {51, 12}, WindowWidgetType::Spinner, WindowColour::Secondary), /* Spinner X*/ \
    MakeSpinnerWidgets({45, 40}, {51, 12}, WindowWidgetType::Spinner, WindowColour::Secondary), /* Spinner Height*/ \
    MakeSpinnerWidgets({45, 57}, {51, 12}, WindowWidgetType::Spinner, WindowColour::Secondary) /* Spinner Delay*/

// clang-format on
static rct_widget _windowFireworksEditorWidgets[] = {
    WIDGETS_MAIN,
    WIDGETS_END,
};

class FireworksEditorWindow final : public Window
{
public:
    FireworksEditorWindow()
        : _x(0)
        , _y(0)
        , _height(0)
        , _delay(0)
    {
    }

    void OnOpen() override
    {
        widgets = _windowFireworksEditorWidgets;
        WindowInitScrollWidgets(*this);
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_SPINNER_DELAY_DECREASE:
                decreaseSpinnerDelay();
                break;
            case WIDX_SPINNER_DELAY_INCREASE:
                increaseSpinnerDelay();
                break;
            case WIDX_SPINNER_HEIGHT_DECREASE:
                decreaseSpinnerHeight();
                break;
            case WIDX_SPINNER_HEIGHT_INCREASE:
                increaseSpinnerHeight();
                break;
            case WIDX_SPINNER_X_DECREASE:
                decreaseSpinnerX();
                break;
            case WIDX_SPINNER_X_INCREASE:
                increaseSpinnerX();
                break;
            case WIDX_SPINNER_Y_DECREASE:
                decreaseSpinnerY();
                break;
            case WIDX_SPINNER_Y_INCREASE:
                increaseSpinnerY();
                break;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);

        // draw the labels for the spin buttons
        gfx_draw_string(&dpi, windowPos + ScreenCoordsXY{ 4, 23 }, "X:", { colours[1] });
        gfx_draw_string(&dpi, windowPos + ScreenCoordsXY{ 74, 23 }, "Y:", { colours[1] });
        gfx_draw_string(&dpi, windowPos + ScreenCoordsXY{ 4, 40 }, "Height:", { colours[1] });
        gfx_draw_string(&dpi, windowPos + ScreenCoordsXY{ 4, 57 }, "Delay:", { colours[1] });

        // draw the current values in the spin box
        auto ft = Formatter();
        ft.Add<uint32_t>(_x);
        DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 21, 23 }, STR_FORMAT_INTEGER, ft, { colours[1], TextAlignment::LEFT });

        ft = Formatter();
        ft.Add<uint32_t>(_y);
        DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 91, 23 }, STR_FORMAT_INTEGER, ft, { colours[1], TextAlignment::LEFT });

        ft = Formatter();
        ft.Add<uint32_t>(_height);
        DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 46, 40 }, STR_FORMAT_INTEGER, ft, { colours[1], TextAlignment::LEFT });

        ft = Formatter();
        ft.Add<uint32_t>(_delay);
        DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 46, 57 }, STR_FORMAT_INTEGER, ft, { colours[1], TextAlignment::LEFT });
    }

    virtual void OnClose() override
    {
    }
    virtual void OnResize() override
    {
    }
    virtual void OnUpdate() override
    {
    }
    virtual void OnPeriodicUpdate() override
    {
    }
    virtual void OnPrepareDraw() override
    {
    }

    virtual OpenRCT2String OnTooltip(rct_widgetindex widgetIndex, StringId fallback) override
    {
        return { fallback, {} };
    }

    virtual void OnMouseDown(rct_widgetindex widgetIndex) override
    {
    }

    virtual void OnDropdown(rct_widgetindex widgetIndex, int32_t selectedIndex) override
    {
    }

    virtual void OnTextInput(rct_widgetindex widgetIndex, std::string_view text) override
    {
    }

    virtual ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        return {};
    }

    virtual void OnScrollMouseDrag(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
    }

    virtual void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
    }

    virtual void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
    }

    virtual void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
    }

    virtual void OnToolUpdate(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
    }

    virtual void OnToolDown(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
    }

    virtual void OnToolDrag(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
    }

    virtual void OnToolUp(rct_widgetindex, const ScreenCoordsXY&) override
    {
    }

    virtual void OnToolAbort(rct_widgetindex widgetIndex) override
    {
    }

    virtual void OnViewportRotate() override
    {
    }

    virtual void OnMoved(const ScreenCoordsXY&) override
    {
    }

private:
    void increaseSpinnerX()
    {
        if (_x + 1 < static_cast<uint32_t>(gMapSize.x))
            _x++;
    }

    void decreaseSpinnerX()
    {
        if (_x != 0)
            _x--;
    }
    void increaseSpinnerY()
    {
        if (_x + 1 < static_cast<uint32_t>(gMapSize.y))
            _y++;
    }

    void decreaseSpinnerY()
    {
        if (_y != 0)
            _y--;
    }

    void increaseSpinnerHeight()
    {
        if (_height + 1 < MAX_ELEMENT_HEIGHT)
            _height++;
    }

    void decreaseSpinnerHeight()
    {
        if (_height != 0)
            _height--;
    }

    void increaseSpinnerDelay()
    {
        if (_delay + 1 < Fireworks::MaximumDelay)
            _delay++;
    }

    void decreaseSpinnerDelay()
    {
        if (_delay != 0)
            _delay--;
    }

    uint32_t _x;
    uint32_t _y;
    uint32_t _height;
    uint32_t _delay;
};

/**
 *
 *  rct2: 0x0066D2AC
 */
rct_window* WindowFireworksEditorOpen()
{
    return WindowFocusOrCreate<FireworksEditorWindow>(WC_FIREWORKS, WW, WH, WF_CENTRE_SCREEN);
}
