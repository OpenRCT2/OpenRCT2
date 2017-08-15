#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <openrct2/OpenRCT2.h>
#include <openrct2/Context.h>
#include <openrct2-ui/windows/Window.h>

extern "C"
{
    #include <openrct2/interface/widget.h>
    #include <openrct2/localisation/localisation.h>
    #include <openrct2/sprites.h>
}

#define WW 400
#define WH 350
#define TABHEIGHT 50

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

    // About RCT2
    WIDX_MUSIC_CREDITS = WIDX_PAGE_START,
};

#define WIDGETS_MAIN \
      WWT_FRAME,            0,  0,              WW - 1,         0,          WH - 1,     0xFFFFFFFF,                             STR_NONE },             /* panel / background */ \
    { WWT_CAPTION,          0,  1,              WW - 2,         1,          14,         STR_ABOUT,                              STR_WINDOW_TITLE_TIP }, /* title bar */ \
    { WWT_CLOSEBOX,         0,  WW - 13,        WW - 3,         2,          13,         STR_CLOSE_X,                            STR_CLOSE_WINDOW_TIP }, /* close x button */ \
    { WWT_IMGBTN,           1,  0,              WW - 1,         TABHEIGHT,  WH - 1,     0xFFFFFFFF,                             STR_NONE },             /* page background */ \
    { WWT_TAB,              1,  3,              93,             17,         TABHEIGHT,  IMAGE_TYPE_REMAP | SPR_TAB_LARGE,       STR_NONE },             /* about OpenRCT2 button */ \
    { WWT_TAB,              1,  94,             184,            17,         TABHEIGHT,  IMAGE_TYPE_REMAP | SPR_TAB_LARGE,       STR_NONE                /* about RCT2 button */

static rct_widget window_about_openrct2_widgets[] = {
    { WIDGETS_MAIN },
    { WWT_DROPDOWN_BUTTON,  1,  100,            299,            WH - 50,    WH - 39,    STR_CHANGELOG_ELLIPSIS,                 STR_NONE },             // changelog button
    { WIDGETS_END }
};

static rct_widget window_about_rct2_widgets[] = {
    { WIDGETS_MAIN },
    { WWT_DROPDOWN_BUTTON,  1,  100,            299,            WH - 50,    WH - 39,    STR_MUSIC_ACKNOWLEDGEMENTS_ELLIPSIS,    STR_NONE },             // music credits button
    { WIDGETS_END },
};

static rct_widget *window_about_page_widgets[] = {
    window_about_openrct2_widgets,
    window_about_rct2_widgets,
};

#define DEFAULT_ENABLED_WIDGETS \
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_TAB_ABOUT_OPENRCT2) | (1ULL << WIDX_TAB_ABOUT_RCT2)

static uint64 window_about_page_enabled_widgets[] = {
    DEFAULT_ENABLED_WIDGETS | (1ULL << WIDX_CHANGELOG),
    DEFAULT_ENABLED_WIDGETS | (1ULL << WIDX_MUSIC_CREDITS),
};

static void window_about_openrct2_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_about_openrct2_paint(rct_window *w, rct_drawpixelinfo *dpi);

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
    nullptr,
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

static void window_about_set_page(rct_window *w, sint32 page);

/**
 *
 *  rct2: 0x0066D2AC
 */
rct_window * window_about_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_ABOUT);
    if (window != nullptr)
        return window;

    window = window_create_centred(
        WW,
        WH,
        window_about_page_events[WINDOW_ABOUT_PAGE_OPENRCT2],
        WC_ABOUT,
        0
    );

    window_about_set_page(window, WINDOW_ABOUT_PAGE_OPENRCT2);

    window_init_scroll_widgets(window);
    window->colours[0] = COLOUR_GREY;
    window->colours[1] = COLOUR_LIGHT_BLUE;
    window->colours[2] = COLOUR_LIGHT_BLUE;

    return window;
}

#pragma region OpenRCT2

static void window_about_openrct2_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_ABOUT_OPENRCT2:
    case WIDX_TAB_ABOUT_RCT2:
        window_about_set_page(w, widgetIndex - WIDX_TAB_ABOUT_OPENRCT2);
        break;
    case WIDX_CHANGELOG:
        context_open_window(WC_CHANGELOG);
        break;
    }
}

static void window_about_openrct2_common_paint(rct_window * w, rct_drawpixelinfo * dpi)
{
    sint32 x1, x2, y;

    x1 = w->x + (&w->widgets[WIDX_TAB_ABOUT_OPENRCT2])->left + 45;
    x2 = w->x + (&w->widgets[WIDX_TAB_ABOUT_RCT2])->left + 45;
    y = w->y + (((&w->widgets[WIDX_TAB_ABOUT_OPENRCT2])->top + (&w->widgets[WIDX_TAB_ABOUT_OPENRCT2])->bottom) / 2) - 3;

    set_format_arg(0, rct_string_id, STR_TITLE_SEQUENCE_OPENRCT2);
    gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x1, y, 87, STR_WINDOW_COLOUR_2_STRINGID, COLOUR_AQUAMARINE);

    set_format_arg(0, rct_string_id, STR_TITLE_SEQUENCE_RCT2);
    gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x2, y, 87, STR_WINDOW_COLOUR_2_STRINGID, COLOUR_AQUAMARINE);
}

static void window_about_openrct2_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_about_openrct2_common_paint(w, dpi);

    sint32 x, y, width;
    rct_size16 logoSize;

    x = w->x + (w->width / 2);
    y = w->y + w->widgets[WIDX_PAGE_BACKGROUND].top + 10;
    width = w->width - 20;
    
    utf8 buffer[256];
    utf8 *ch = buffer;
    openrct2_write_full_version_info(ch, sizeof(buffer) - (ch - buffer));

    y += gfx_draw_string_centred_wrapped(dpi, nullptr, x, y, width, STR_ABOUT_OPENRCT2_DESCRIPTION, w->colours[2]) + 10;

    logoSize = gfx_get_sprite_size(SPR_G2_LOGO);
    gfx_draw_sprite(dpi, SPR_G2_LOGO, x - (logoSize.width / 2), y, 0);
    y += logoSize.height + 20;

    y += gfx_draw_string_centred_wrapped(dpi, nullptr, x, y, width, STR_ABOUT_OPENRCT2_DESCRIPTION_2, w->colours[2]) + 15;
    gfx_draw_string_centred_wrapped(dpi, nullptr, x, y, width, STR_ABOUT_OPENRCT2_DESCRIPTION_3, w->colours[2]);

    gfx_draw_string_centred_wrapped(dpi, &ch, x, w->y + WH - 25, width, STR_STRING, w->colours[2]);

}

#pragma endregion OpenRCT2

#pragma region RCT2

/**
 *
 *  rct2: 0x0066D4D5
 */
static void window_about_rct2_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
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
static void window_about_rct2_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    sint32 x, y, yPage;

    window_draw_widgets(w, dpi);
    window_about_openrct2_common_paint(w, dpi);

    yPage = w->y + w->widgets[WIDX_PAGE_BACKGROUND].top + 5;

    x = w->x + 200;
    y = yPage + 5;

    // Credits
    gfx_draw_string_centred(dpi, STR_COPYRIGHT_CS, x, y, COLOUR_BLACK, nullptr);
    y += 84;
    gfx_draw_string_centred(dpi, STR_DESIGNED_AND_PROGRAMMED_BY_CS, x, y, COLOUR_BLACK, nullptr);
    y += 10;
    gfx_draw_string_centred(dpi, STR_GRAPHICS_BY_SF, x, y, COLOUR_BLACK, nullptr);
    y += 10;
    gfx_draw_string_centred(dpi, STR_SOUND_AND_MUSIC_BY_AB, x, y, COLOUR_BLACK, nullptr);
    y += 10;
    gfx_draw_string_centred(dpi, STR_ADDITIONAL_SOUNDS_RECORDED_BY_DE, x, y, COLOUR_BLACK, nullptr);
    y += 13;
    gfx_draw_string_centred(dpi, STR_REPRESENTATION_BY_JL, x, y, COLOUR_BLACK, nullptr);
    y += 25;
    gfx_draw_string_centred(dpi, STR_THANKS_TO, x, y, COLOUR_BLACK, nullptr);
    y += 10;
    gfx_draw_string_centred(dpi, STR_THANKS_TO_PEOPLE, x, y, COLOUR_BLACK, nullptr);
    y += 25;
    gfx_draw_string_centred(dpi, STR_LICENSED_TO_INFOGRAMES_INTERACTIVE_INC, x, y, COLOUR_BLACK, nullptr);

    // Images
    gfx_draw_sprite(dpi, SPR_CREDITS_CHRIS_SAWYER_SMALL, w->x + 92, yPage + 24, 0);

    // Licence
}

#pragma endregion RCT2

static void window_about_set_page(rct_window *w, sint32 page)
{
    w->page = page;
    w->frame_no = 0;
    w->pressed_widgets = 0;
    w->widgets = window_about_page_widgets[page];
    w->enabled_widgets = window_about_page_enabled_widgets[page];
    w->event_handlers = window_about_page_events[page];

    w->pressed_widgets |= (page == WINDOW_ABOUT_PAGE_RCT2) ? (1ULL << WIDX_TAB_ABOUT_RCT2) : (1ULL << WIDX_TAB_ABOUT_OPENRCT2);

    window_init_scroll_widgets(w);
    window_invalidate(w);
}
