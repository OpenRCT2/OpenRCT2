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
#include <openrct2/OpenRCT2.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/UiContext.h>

static constexpr const int32_t WW = 400;
static constexpr const int32_t WH = 385;
static constexpr const rct_string_id WINDOW_TITLE = STR_ABOUT;
constexpr int32_t TABHEIGHT = 50;

// clang-format off
enum
{
    WINDOW_ABOUT_PAGE_OPENRCT2,
    WINDOW_ABOUT_PAGE_RCT2,
};

enum WINDOW_ABOUT_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_ABOUT_OPENRCT2,
    WIDX_TAB_ABOUT_RCT2,

    WIDX_PAGE_START,

    // About OpenRCT2
    WIDX_CHANGELOG = WIDX_PAGE_START,
    WIDX_JOIN_DISCORD,
    WIDX_NEW_VERSION,

    // About RCT2
    WIDX_MUSIC_CREDITS = WIDX_PAGE_START,
};

#define WIDGETS_MAIN \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget     ({ 0, TABHEIGHT}, {WW, WH - TABHEIGHT}, WindowWidgetType::ImgBtn, WindowColour::Secondary               ), /* page background */       \
    MakeRemapWidget({ 3,        17}, {91, TABHEIGHT - 16}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE), /* about OpenRCT2 button */ \
    MakeRemapWidget({94,        17}, {91, TABHEIGHT - 16}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE)  /* about RCT2 button */

static rct_widget window_about_openrct2_widgets[] = {
    WIDGETS_MAIN,
    MakeWidget({100, WH - TABHEIGHT - (14 + 3) * 2}, {200, 14}, WindowWidgetType::Button,      WindowColour::Secondary, STR_CHANGELOG_ELLIPSIS), // changelog button
    MakeWidget({100, WH - TABHEIGHT - (14 + 3) * 1}, {200, 14}, WindowWidgetType::Button,      WindowColour::Secondary, STR_JOIN_DISCORD      ), // "join discord" button
    MakeWidget({100, WH - TABHEIGHT - (14 + 3) * 0}, {200, 14}, WindowWidgetType::Placeholder, WindowColour::Secondary, STR_UPDATE_AVAILABLE  ), // "new version" button
    { WIDGETS_END }
};

static rct_widget window_about_rct2_widgets[] = {
    WIDGETS_MAIN,
    MakeWidget({100, WH - TABHEIGHT}, {200, 14}, WindowWidgetType::Button, WindowColour::Secondary, STR_MUSIC_ACKNOWLEDGEMENTS_ELLIPSIS), // music credits button
    { WIDGETS_END },
};

static rct_widget *window_about_page_widgets[] = {
    window_about_openrct2_widgets,
    window_about_rct2_widgets,
};

#define DEFAULT_ENABLED_WIDGETS \
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_TAB_ABOUT_OPENRCT2) | (1ULL << WIDX_TAB_ABOUT_RCT2)

static uint64_t window_about_page_enabled_widgets[] = {
    DEFAULT_ENABLED_WIDGETS | (1ULL << WIDX_CHANGELOG) | (1 << WIDX_JOIN_DISCORD),
    DEFAULT_ENABLED_WIDGETS | (1ULL << WIDX_MUSIC_CREDITS),
};

static void window_about_openrct2_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_about_openrct2_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_about_openrct2_invalidate(rct_window *w);

static void window_about_rct2_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_about_rct2_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_about_openrct2_common_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_about_openrct2_events([](auto& events)
{
    events.mouse_up = &window_about_openrct2_mouseup;
    events.invalidate = &window_about_openrct2_invalidate;
    events.paint = &window_about_openrct2_paint;
});

static rct_window_event_list window_about_rct2_events([](auto& events)
{
    events.mouse_up = &window_about_rct2_mouseup;
    events.paint = &window_about_rct2_paint;
});

static rct_window_event_list *window_about_page_events[] = {
    &window_about_openrct2_events,
    &window_about_rct2_events,
};
// clang-format on

static void window_about_set_page(rct_window* w, int32_t page);

/**
 *
 *  rct2: 0x0066D2AC
 */
rct_window* window_about_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_ABOUT);
    if (window != nullptr)
        return window;

    window = WindowCreateCentred(WW, WH, window_about_page_events[WINDOW_ABOUT_PAGE_OPENRCT2], WC_ABOUT, 0);

    window_about_set_page(window, WINDOW_ABOUT_PAGE_OPENRCT2);

    WindowInitScrollWidgets(window);
    window->colours[0] = COLOUR_GREY;
    window->colours[1] = COLOUR_LIGHT_BLUE;
    window->colours[2] = COLOUR_LIGHT_BLUE;

    return window;
}

#pragma region OpenRCT2

static void window_about_openrct2_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_ABOUT_OPENRCT2:
        case WIDX_TAB_ABOUT_RCT2:
            window_about_set_page(w, widgetIndex - WIDX_TAB_ABOUT_OPENRCT2);
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
    }
}

static void window_about_openrct2_common_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    const auto& aboutOpenRCT2 = w->widgets[WIDX_TAB_ABOUT_OPENRCT2];
    const auto& aboutRCT2 = w->widgets[WIDX_TAB_ABOUT_RCT2];

    int32_t y = w->windowPos.y + aboutOpenRCT2.midY() - 3;
    ScreenCoordsXY aboutOpenRCT2Coords(w->windowPos.x + aboutOpenRCT2.left + 45, y);
    ScreenCoordsXY aboutRCT2Coords(w->windowPos.x + aboutRCT2.left + 45, y);

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

static void window_about_openrct2_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_about_openrct2_common_paint(w, dpi);

    int32_t lineHeight = font_get_line_height(FontSpriteBase::MEDIUM);

    ScreenCoordsXY aboutCoords(
        w->windowPos.x + (w->width / 2), w->windowPos.y + w->widgets[WIDX_PAGE_BACKGROUND].top + lineHeight);
    int32_t width = w->width - 20;

    aboutCoords.y += DrawTextWrapped(
                         dpi, aboutCoords, width, STR_ABOUT_OPENRCT2_DESCRIPTION, {}, { w->colours[2], TextAlignment::CENTRE })
        + lineHeight;

    rct_size16 logoSize = gfx_get_sprite_size(SPR_G2_LOGO);
    gfx_draw_sprite(dpi, ImageId(SPR_G2_LOGO), aboutCoords - ScreenCoordsXY{ logoSize.width / 2, 0 });
    aboutCoords.y += logoSize.height + lineHeight * 2;

    // About OpenRCT2 text
    aboutCoords.y += DrawTextWrapped(
                         dpi, aboutCoords, width, STR_ABOUT_OPENRCT2_DESCRIPTION_2, {},
                         { w->colours[2], TextAlignment::CENTRE })
        + lineHeight + 5;

    // Copyright disclaimer; hidden when using truetype fonts to prevent
    // the text from overlapping the changelog button.
    if (!LocalisationService_UseTrueTypeFont())
    {
        DrawTextWrapped(
            dpi, aboutCoords, width, STR_ABOUT_OPENRCT2_DESCRIPTION_3, {}, { w->colours[2], TextAlignment::CENTRE });
    }

    // Version info
    utf8 buffer[256];
    utf8* ch = buffer;
    openrct2_write_full_version_info(ch, sizeof(buffer) - (ch - buffer));
    auto ft = Formatter();
    ft.Add<const char*>(buffer);

    aboutCoords.y = w->windowPos.y + WH - 25;
    DrawTextWrapped(dpi, aboutCoords, width, STR_STRING, ft, { w->colours[2], TextAlignment::CENTRE });
}

static void window_about_openrct2_invalidate(rct_window* w)
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
static void window_about_rct2_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_ABOUT_OPENRCT2:
        case WIDX_TAB_ABOUT_RCT2:
            window_about_set_page(w, widgetIndex - WIDX_TAB_ABOUT_OPENRCT2);
            break;
        case WIDX_MUSIC_CREDITS:
            context_open_window(WC_MUSIC_CREDITS);
            break;
    }
}

/**
 *
 *  rct2: 0x0066D321
 */
static void window_about_rct2_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_about_openrct2_common_paint(w, dpi);

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

static void window_about_set_page(rct_window* w, int32_t page)
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
