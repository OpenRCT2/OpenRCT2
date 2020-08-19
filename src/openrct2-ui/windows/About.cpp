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
    MakeWidget     ({ 0, TABHEIGHT}, {WW, WH - TABHEIGHT}, WWT_IMGBTN, WindowColour::Secondary               ), /* page background */       \
    MakeRemapWidget({ 3,        17}, {91, TABHEIGHT - 16}, WWT_TAB,    WindowColour::Secondary, SPR_TAB_LARGE), /* about OpenRCT2 button */ \
    MakeRemapWidget({94,        17}, {91, TABHEIGHT - 16}, WWT_TAB,    WindowColour::Secondary, SPR_TAB_LARGE)  /* about RCT2 button */

static rct_widget window_about_openrct2_widgets[] = {
    WIDGETS_MAIN,
    MakeWidget({100, WH - TABHEIGHT - (14 + 3) * 2}, {200, 14}, WWT_BUTTON,      WindowColour::Secondary, STR_CHANGELOG_ELLIPSIS), // changelog button
    MakeWidget({100, WH - TABHEIGHT - (14 + 3) * 1}, {200, 14}, WWT_BUTTON,      WindowColour::Secondary, STR_JOIN_DISCORD      ), // "join discord" button
    MakeWidget({100, WH - TABHEIGHT - (14 + 3) * 0}, {200, 14}, WWT_PLACEHOLDER, WindowColour::Secondary, STR_UPDATE_AVAILABLE  ), // "new version" button
    { WIDGETS_END }
};

static rct_widget window_about_rct2_widgets[] = {
    WIDGETS_MAIN,
    MakeWidget({100, WH - TABHEIGHT}, {200, 14}, WWT_BUTTON, WindowColour::Secondary, STR_MUSIC_ACKNOWLEDGEMENTS_ELLIPSIS), // music credits button
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

static rct_window_event_list window_about_openrct2_events = {
    nullptr,
    window_about_openrct2_mouseup,
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
    window_about_openrct2_invalidate,
    window_about_openrct2_paint,
    nullptr
};

static rct_window_event_list window_about_rct2_events = {
    nullptr,
    window_about_rct2_mouseup,
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
    nullptr,
    window_about_rct2_paint,
    nullptr
};

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

    window = window_create_centred(WW, WH, window_about_page_events[WINDOW_ABOUT_PAGE_OPENRCT2], WC_ABOUT, 0);

    window_about_set_page(window, WINDOW_ABOUT_PAGE_OPENRCT2);

    window_init_scroll_widgets(window);
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
    window_draw_widgets(w, dpi);

    const auto& aboutOpenRCT2 = w->widgets[WIDX_TAB_ABOUT_OPENRCT2];
    const auto& aboutRCT2 = w->widgets[WIDX_TAB_ABOUT_RCT2];

    int32_t y = w->windowPos.y + aboutOpenRCT2.midY() - 3;
    ScreenCoordsXY aboutOpenRCT2Coords(w->windowPos.x + aboutOpenRCT2.left + 45, y);
    ScreenCoordsXY aboutRCT2Coords(w->windowPos.x + aboutRCT2.left + 45, y);

    auto ft = Formatter::Common();
    ft.Add<rct_string_id>(STR_TITLE_SEQUENCE_OPENRCT2);
    gfx_draw_string_centred_wrapped(
        dpi, gCommonFormatArgs, aboutOpenRCT2Coords, 87, STR_WINDOW_COLOUR_2_STRINGID, COLOUR_AQUAMARINE);

    ft = Formatter::Common();
    ft.Add<rct_string_id>(STR_TITLE_SEQUENCE_RCT2);
    gfx_draw_string_centred_wrapped(
        dpi, gCommonFormatArgs, aboutRCT2Coords, 87, STR_WINDOW_COLOUR_2_STRINGID, COLOUR_AQUAMARINE);
}

static void window_about_openrct2_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_about_openrct2_common_paint(w, dpi);

    int32_t width;
    rct_size16 logoSize;

    int32_t lineHeight = font_get_line_height(gCurrentFontSpriteBase);

    ScreenCoordsXY aboutCoords(
        w->windowPos.x + (w->width / 2), w->windowPos.y + w->widgets[WIDX_PAGE_BACKGROUND].top + lineHeight);
    width = w->width - 20;

    aboutCoords.y += gfx_draw_string_centred_wrapped(
                         dpi, nullptr, aboutCoords, width, STR_ABOUT_OPENRCT2_DESCRIPTION, w->colours[2])
        + lineHeight;

    logoSize = gfx_get_sprite_size(SPR_G2_LOGO);
    gfx_draw_sprite(dpi, SPR_G2_LOGO, aboutCoords - ScreenCoordsXY{ logoSize.width / 2, 0 }, 0);
    aboutCoords.y += logoSize.height + lineHeight * 2;

    // About OpenRCT2 text
    aboutCoords.y += gfx_draw_string_centred_wrapped(
                         dpi, nullptr, aboutCoords, width, STR_ABOUT_OPENRCT2_DESCRIPTION_2, w->colours[2])
        + lineHeight + 5;

    // Copyright disclaimer; hidden when using truetype fonts to prevent
    // the text from overlapping the changelog button.
    if (!LocalisationService_UseTrueTypeFont())
    {
        gfx_draw_string_centred_wrapped(dpi, nullptr, aboutCoords, width, STR_ABOUT_OPENRCT2_DESCRIPTION_3, w->colours[2]);
    }

    // Version info
    utf8 buffer[256];
    utf8* ch = buffer;
    openrct2_write_full_version_info(ch, sizeof(buffer) - (ch - buffer));

    aboutCoords.y = w->windowPos.y + WH - 25;
    gfx_draw_string_centred_wrapped(dpi, &ch, aboutCoords, width, STR_STRING, w->colours[2]);
}

static void window_about_openrct2_invalidate(rct_window* w)
{
    if (w->page == WINDOW_ABOUT_PAGE_OPENRCT2 && OpenRCT2::GetContext()->HasNewVersionInfo())
    {
        w->enabled_widgets |= (1ULL << WIDX_NEW_VERSION);
        w->widgets[WIDX_NEW_VERSION].type = WWT_BUTTON;
        window_about_openrct2_widgets[WIDX_NEW_VERSION].type = WWT_BUTTON;
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
    int32_t yPage;

    window_about_openrct2_common_paint(w, dpi);

    yPage = w->windowPos.y + w->widgets[WIDX_PAGE_BACKGROUND].top + 5;

    auto screenCoords = ScreenCoordsXY{ w->windowPos.x + 200, yPage + 5 };

    int32_t lineHeight = font_get_line_height(gCurrentFontSpriteBase);

    // Credits
    gfx_draw_string_centred(dpi, STR_COPYRIGHT_CS, screenCoords, COLOUR_BLACK, nullptr);
    screenCoords.y += lineHeight + 74;
    gfx_draw_string_centred(dpi, STR_DESIGNED_AND_PROGRAMMED_BY_CS, screenCoords, COLOUR_BLACK, nullptr);
    screenCoords.y += lineHeight;
    gfx_draw_string_centred(dpi, STR_GRAPHICS_BY_SF, screenCoords, COLOUR_BLACK, nullptr);
    screenCoords.y += lineHeight;
    gfx_draw_string_centred(dpi, STR_SOUND_AND_MUSIC_BY_AB, screenCoords, COLOUR_BLACK, nullptr);
    screenCoords.y += lineHeight;
    gfx_draw_string_centred(dpi, STR_ADDITIONAL_SOUNDS_RECORDED_BY_DE, screenCoords, COLOUR_BLACK, nullptr);
    screenCoords.y += lineHeight + 3;
    gfx_draw_string_centred(dpi, STR_REPRESENTATION_BY_JL, screenCoords, COLOUR_BLACK, nullptr);
    screenCoords.y += 2 * lineHeight + 5;
    gfx_draw_string_centred(dpi, STR_THANKS_TO, screenCoords, COLOUR_BLACK, nullptr);
    screenCoords.y += lineHeight;
    gfx_draw_string_centred(dpi, STR_THANKS_TO_PEOPLE, screenCoords, COLOUR_BLACK, nullptr);
    screenCoords.y += 2 * lineHeight + 5;
    gfx_draw_string_centred(dpi, STR_LICENSED_TO_INFOGRAMES_INTERACTIVE_INC, screenCoords, COLOUR_BLACK, nullptr);

    // Images
    gfx_draw_sprite(dpi, SPR_CREDITS_CHRIS_SAWYER_SMALL, { w->windowPos.x + 92, yPage + 24 }, 0);

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

    window_init_scroll_widgets(w);
    w->Invalidate();
}
