/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Intro.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Localisation.h>

// clang-format off
enum WINDOW_TITLE_OPTIONS_WIDGET_IDX {
    WIDX_NEW_VERSION,
    WIDX_OPTIONS,
};

static constexpr ScreenSize OptionsButtonDims = { 80, 15 };
static constexpr ScreenSize UpdateButtonDims = { 125, 15 };

static rct_widget window_title_options_widgets[] = {
    MakeWidget({                     0, 0},  UpdateButtonDims, WWT_EMPTY,  WindowColour::Secondary, STR_UPDATE_AVAILABLE),
    MakeWidget({UpdateButtonDims.width, 0}, OptionsButtonDims, WWT_BUTTON, WindowColour::Tertiary,  STR_OPTIONS, STR_OPTIONS_TIP),
    { WIDGETS_END },
};

static void window_title_options_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_title_options_invalidate(rct_window *w);
static void window_title_options_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_options_events = {
    nullptr,
    window_title_options_mouseup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_title_options_invalidate,
    window_title_options_paint,
    nullptr
};
// clang-format on

/**
 * Creates the window containing the options button on the title screen.
 */
rct_window* window_title_options_open()
{
    const uint16_t windowWidth = UpdateButtonDims.width + OptionsButtonDims.width;

    rct_window* window = window_create(
        ScreenCoordsXY(context_get_width() - windowWidth, 0), windowWidth, OptionsButtonDims.height,
        &window_title_options_events, WC_TITLE_OPTIONS, WF_STICK_TO_BACK | WF_TRANSPARENT);
    window->widgets = window_title_options_widgets;
    window->enabled_widgets = (1ULL << WIDX_OPTIONS);
    window->colours[1] = TRANSLUCENT(COLOUR_LIGHT_ORANGE);
    window_init_scroll_widgets(window);

    return window;
}

static void window_title_options_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (gIntroState != IntroState::None)
        return;

    switch (widgetIndex)
    {
        case WIDX_OPTIONS:
            context_open_window(WC_OPTIONS);
            break;
        case WIDX_NEW_VERSION:
            context_open_window_view(WV_NEW_VERSION_INFO);
            break;
    }
}

static void window_title_options_invalidate(rct_window* w)
{
    rct_widget& updateButton = w->widgets[WIDX_NEW_VERSION];
    rct_widget& optionsButton = w->widgets[WIDX_OPTIONS];

    if (OpenRCT2::GetContext()->HasNewVersionInfo())
    {
        w->width = UpdateButtonDims.width + OptionsButtonDims.width;
        w->enabled_widgets |= (1ULL << WIDX_NEW_VERSION);
        updateButton.type = WWT_BUTTON;
        optionsButton.left = UpdateButtonDims.width;
    }
    else
    {
        w->width = OptionsButtonDims.width;
        updateButton.type = WWT_EMPTY;
        optionsButton.left = 0;
    }

    optionsButton.right = optionsButton.left + OptionsButtonDims.width;
    w->windowPos.x = context_get_width() - w->width;
}

static void window_title_options_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    gfx_filter_rect(dpi, { w->windowPos.x, w->windowPos.y, w->windowPos.x + w->width - 1, w->windowPos.y - 1 }, PALETTE_51);
    window_draw_widgets(w, dpi);
}
