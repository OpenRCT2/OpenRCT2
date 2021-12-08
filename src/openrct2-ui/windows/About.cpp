/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/UiContext.h>

static constexpr const int32_t WW = 400;
static constexpr const int32_t WH = 352;
static constexpr const rct_string_id WINDOW_TITLE = STR_ABOUT;
static constexpr const int32_t TABHEIGHT = 50;

// clang-format off
enum
{
    WINDOW_ABOUT_PAGE_OPENRCT2,
    WINDOW_ABOUT_PAGE_RCT2,
};

enum WindowAboutWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_ABOUT_OPENRCT2,
    WIDX_TAB_ABOUT_RCT2,

    WIDX_PAGE_START,

    // About OpenRCT2
    WIDX_INTRO = WIDX_PAGE_START,
    WIDX_OPENRCT2_LOGO,
    WIDX_VERSION,
    WIDX_COPY_BUILD_INFO,
    WIDX_NEW_VERSION,
    WIDX_CHANGELOG,
    WIDX_JOIN_DISCORD,
    WIDX_CONTRIBUTORS,
    WIDX_COPYRIGHT,
};

#define WIDGETS_MAIN \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget     ({ 0, TABHEIGHT}, {WW, WH - TABHEIGHT}, WindowWidgetType::ImgBtn, WindowColour::Secondary               ), /* page background */       \
    MakeRemapWidget({ 3,        17}, {91, TABHEIGHT - 16}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE), /* about OpenRCT2 button */ \
    MakeRemapWidget({94,        17}, {91, TABHEIGHT - 16}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE)  /* about RCT2 button */

static rct_widget window_about_openrct2_widgets[] = {
    WIDGETS_MAIN,
    MakeWidget({10, 60},        {WW - 20, 20}, WindowWidgetType::LabelCentred, WindowColour::Secondary, STR_ABOUT_OPENRCT2_DESCRIPTION), // Introduction
    MakeWidget({30, 90},        {128, 128},    WindowWidgetType::Placeholder,  WindowColour::Secondary, STR_NONE), // OpenRCT2 Logo
    MakeWidget({168, 100},      {173, 24},     WindowWidgetType::Placeholder,  WindowColour::Secondary, STR_NONE), // Build version
    MakeWidget({344, 100 },     {24, 24},      WindowWidgetType::ImgBtn,       WindowColour::Secondary, SPR_G2_COPY, STR_COPY_BUILD_HASH   ), // "Copy build info" button
    MakeWidget({168, 115 + 24}, {200, 14},     WindowWidgetType::Placeholder,  WindowColour::Secondary, STR_UPDATE_AVAILABLE  ), // "new version" button
    MakeWidget({168, 115 + 48}, {200, 14},     WindowWidgetType::Button,       WindowColour::Secondary, STR_CHANGELOG_ELLIPSIS), // changelog button
    MakeWidget({168, 115 + 72}, {200, 14},     WindowWidgetType::Button,       WindowColour::Secondary, STR_JOIN_DISCORD      ), // "join discord" button
    MakeWidget({10, 250},       {WW - 20, 50}, WindowWidgetType::LabelCentred, WindowColour::Secondary, STR_ABOUT_OPENRCT2_DESCRIPTION_2), // Contributors
    MakeWidget({10, 300},       {WW - 20, 50}, WindowWidgetType::LabelCentred, WindowColour::Secondary, STR_ABOUT_OPENRCT2_DESCRIPTION_3), // Copyright
    WIDGETS_END,
};

static rct_widget window_about_rct2_widgets[] = {
    WIDGETS_MAIN,
    WIDGETS_END,
};

static rct_widget *window_about_page_widgets[] = {
    window_about_openrct2_widgets,
    window_about_rct2_widgets,
};

#define DEFAULT_ENABLED_WIDGETS \
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_TAB_ABOUT_OPENRCT2) | (1ULL << WIDX_TAB_ABOUT_RCT2)

static uint64_t window_about_page_enabled_widgets[] = {
    DEFAULT_ENABLED_WIDGETS | (1ULL << WIDX_COPY_BUILD_INFO) | (1ULL << WIDX_CHANGELOG) | (1ULL << WIDX_JOIN_DISCORD),
    DEFAULT_ENABLED_WIDGETS,
};

static void WindowAboutOpenRCT2Mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowAboutOpenRCT2Paint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowAboutOpenRCT2Invalidate(rct_window *w);

static void WindowAboutRCT2Mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowAboutRCT2Paint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowAboutOpenRCT2CommonPaint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_about_openrct2_events([](auto& events)
{
    events.mouse_up = &WindowAboutOpenRCT2Mouseup;
    events.invalidate = &WindowAboutOpenRCT2Invalidate;
    events.paint = &WindowAboutOpenRCT2Paint;
});

static rct_window_event_list window_about_rct2_events([](auto& events)
{
    events.mouse_up = &WindowAboutRCT2Mouseup;
    events.paint = &WindowAboutRCT2Paint;
});

static rct_window_event_list *window_about_page_events[] = {
    &window_about_openrct2_events,
    &window_about_rct2_events,
};
// clang-format on

static void WindowAboutSetPage(rct_window* w, int32_t page);

/**
 *
 *  rct2: 0x0066D2AC
 */
rct_window* WindowAboutOpen()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_ABOUT);
    if (window != nullptr)
        return window;

    window = WindowCreateCentred(WW, WH, window_about_page_events[WINDOW_ABOUT_PAGE_OPENRCT2], WC_ABOUT, 0);

    WindowAboutSetPage(window, WINDOW_ABOUT_PAGE_OPENRCT2);

    WindowInitScrollWidgets(window);

    return window;
}

#pragma region OpenRCT2

static void WindowAboutOpenRCT2Mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_ABOUT_OPENRCT2:
        case WIDX_TAB_ABOUT_RCT2:
            WindowAboutSetPage(w, widgetIndex - WIDX_TAB_ABOUT_OPENRCT2);
            break;
        case WIDX_JOIN_DISCORD:
            OpenRCT2::GetContext()->GetUiContext()->OpenURL("https://discord.gg/ZXZd8D8");
            break;
        case WIDX_CHANGELOG:
            context_open_window(WC_CHANGELOG);
            break;
        case WIDX_NEW_VERSION:
            context_open_window_view(WV_NEW_VERSION_INFO);
            break;
        case WIDX_COPY_BUILD_INFO:
            SDL_SetClipboardText(gVersionInfoFull);
            break;
    }
}

static void WindowAboutOpenRCT2CommonPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    const auto& aboutOpenRCT2 = w->widgets[WIDX_TAB_ABOUT_OPENRCT2];
    const auto& aboutRCT2 = w->widgets[WIDX_TAB_ABOUT_RCT2];

    int32_t y = w->windowPos.y + aboutOpenRCT2.midY() - 3;
    ScreenCoordsXY aboutOpenRCT2Coords(w->windowPos.x + aboutOpenRCT2.left + 45, y);
    ScreenCoordsXY aboutRCT2Coords(w->windowPos.x + aboutRCT2.left + 45, y);

    // Draw tab names
    {
        auto ft = Formatter();
        ft.Add<rct_string_id>(STR_TITLE_SEQUENCE_OPENRCT2);
        DrawTextWrapped(
            dpi, aboutOpenRCT2Coords, 87, STR_WINDOW_COLOUR_2_STRINGID, ft, { COLOUR_AQUAMARINE, TextAlignment::CENTRE });
    }
    {
        auto ft = Formatter();
        ft.Add<rct_string_id>(STR_TITLE_SEQUENCE_RCT2);
        DrawTextWrapped(
            dpi, aboutRCT2Coords, 87, STR_WINDOW_COLOUR_2_STRINGID, ft, { COLOUR_AQUAMARINE, TextAlignment::CENTRE });
    }
}

static void WindowAboutOpenRCT2Paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowAboutOpenRCT2CommonPaint(w, dpi);

    // Draw logo on placeholder widget
    ScreenCoordsXY logoCoords = w->windowPos
        + ScreenCoordsXY(w->widgets[WIDX_OPENRCT2_LOGO].left, w->widgets[WIDX_OPENRCT2_LOGO].top);
    gfx_draw_sprite(dpi, ImageId(SPR_G2_LOGO), logoCoords);

    // Version info
    utf8 buffer[256];
    utf8* ch = buffer;
    openrct2_write_full_version_info(ch, sizeof(buffer) - (ch - buffer));
    auto ft = Formatter();
    ft.Add<const char*>(buffer);

    auto const& versionPlaceholder = w->widgets[WIDX_VERSION];
    auto width = versionPlaceholder.right - versionPlaceholder.left;
    auto centreX = versionPlaceholder.left + width / 2;
    auto centreY = (versionPlaceholder.top + versionPlaceholder.bottom - font_get_line_height(FontSpriteBase::MEDIUM)) / 2;
    auto centrePos = w->windowPos + ScreenCoordsXY(centreX, centreY);
    DrawTextWrapped(dpi, centrePos, width, STR_STRING, ft, { w->colours[1], TextAlignment::CENTRE });
}

static void WindowAboutOpenRCT2Invalidate(rct_window* w)
{
    if (w->page == WINDOW_ABOUT_PAGE_OPENRCT2 && OpenRCT2::GetContext()->HasNewVersionInfo())
    {
        w->enabled_widgets |= (1ULL << WIDX_NEW_VERSION);
        w->widgets[WIDX_NEW_VERSION].type = WindowWidgetType::Button;
        window_about_openrct2_widgets[WIDX_NEW_VERSION].type = WindowWidgetType::Button;
    }
}

#pragma endregion OpenRCT2

#pragma region RCT2

/**
 *
 *  rct2: 0x0066D4D5
 */
static void WindowAboutRCT2Mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_ABOUT_OPENRCT2:
        case WIDX_TAB_ABOUT_RCT2:
            WindowAboutSetPage(w, widgetIndex - WIDX_TAB_ABOUT_OPENRCT2);
            break;
    }
}

/**
 *
 *  rct2: 0x0066D321
 */
static void WindowAboutRCT2Paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowAboutOpenRCT2CommonPaint(w, dpi);

    int32_t yPage = w->windowPos.y + w->widgets[WIDX_PAGE_BACKGROUND].top + 5;

    auto screenCoords = ScreenCoordsXY{ w->windowPos.x + 200, yPage + 5 };

    int32_t lineHeight = font_get_line_height(FontSpriteBase::MEDIUM);

    // Credits
    DrawTextBasic(dpi, screenCoords, STR_COPYRIGHT_CS, {}, { TextAlignment::CENTRE });
    screenCoords.y += lineHeight + 74;
    DrawTextBasic(dpi, screenCoords, STR_DESIGNED_AND_PROGRAMMED_BY_CS, {}, { TextAlignment::CENTRE });
    screenCoords.y += lineHeight;
    DrawTextBasic(dpi, screenCoords, STR_GRAPHICS_BY_SF, {}, { TextAlignment::CENTRE });
    screenCoords.y += lineHeight;
    DrawTextBasic(dpi, screenCoords, STR_SOUND_AND_MUSIC_BY_AB, {}, { TextAlignment::CENTRE });
    screenCoords.y += lineHeight;
    DrawTextBasic(dpi, screenCoords, STR_ADDITIONAL_SOUNDS_RECORDED_BY_DE, {}, { TextAlignment::CENTRE });
    screenCoords.y += lineHeight + 3;
    DrawTextBasic(dpi, screenCoords, STR_REPRESENTATION_BY_JL, {}, { TextAlignment::CENTRE });
    screenCoords.y += 2 * lineHeight + 5;
    DrawTextBasic(dpi, screenCoords, STR_THANKS_TO, {}, { TextAlignment::CENTRE });
    screenCoords.y += lineHeight;
    DrawTextBasic(dpi, screenCoords, STR_THANKS_TO_PEOPLE, {}, { TextAlignment::CENTRE });
    screenCoords.y += 2 * lineHeight + 5;
    DrawTextBasic(dpi, screenCoords, STR_LICENSED_TO_INFOGRAMES_INTERACTIVE_INC, {}, { TextAlignment::CENTRE });

    // Images
    gfx_draw_sprite(dpi, ImageId(SPR_CREDITS_CHRIS_SAWYER_SMALL), { w->windowPos.x + 92, yPage + 24 });

    // Licence
}

#pragma endregion RCT2

static void WindowAboutSetPage(rct_window* w, int32_t page)
{
    w->page = page;
    w->frame_no = 0;
    w->pressed_widgets = 0;
    w->widgets = window_about_page_widgets[page];
    w->enabled_widgets = window_about_page_enabled_widgets[page];
    w->event_handlers = window_about_page_events[page];

    w->pressed_widgets |= (page == WINDOW_ABOUT_PAGE_RCT2) ? (1ULL << WIDX_TAB_ABOUT_RCT2) : (1ULL << WIDX_TAB_ABOUT_OPENRCT2);

    WindowInitScrollWidgets(w);
    w->Invalidate();
}
