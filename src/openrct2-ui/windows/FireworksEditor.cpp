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
    WIDX_PAGE_BACKGROUND,
};

#define WIDGETS_MAIN \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget     ({ 0, TABHEIGHT}, {WW, WH - TABHEIGHT}, WindowWidgetType::ImgBtn, WindowColour::Secondary               ) /* page background */       \


// clang-format on
static rct_widget _windowFireworksEditorWidgets[] = {
    WIDGETS_MAIN,
    WIDGETS_END,
};

class FireworksEditorWindow final : public Window
{
public:
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
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
    }

private:
};

/**
 *
 *  rct2: 0x0066D2AC
 */
rct_window* WindowFireworksEditorOpen()
{
    return WindowFocusOrCreate<FireworksEditorWindow>(WC_ABOUT, WW, WH, WF_CENTRE_SCREEN);
}
