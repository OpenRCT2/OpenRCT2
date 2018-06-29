/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/config/Config.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>

#include <openrct2/Game.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/actions/BannerSetNameAction.hpp>

#define WW 113
#define WH 96

// clang-format off
enum WINDOW_BANNER_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_VIEWPORT,
    WIDX_BANNER_TEXT,
    WIDX_BANNER_NO_ENTRY,
    WIDX_BANNER_DEMOLISH,
    WIDX_MAIN_COLOUR,
    WIDX_TEXT_COLOUR_DROPDOWN,
    WIDX_TEXT_COLOUR_DROPDOWN_BUTTON
};

static constexpr const rct_string_id BannerColouredTextFormats[] = {
    STR_TEXT_COLOR_BLACK,
    STR_TEXT_COLOR_GREY,
    STR_TEXT_COLOR_WHITE,
    STR_TEXT_COLOR_RED,
    STR_TEXT_COLOR_GREEN,
    STR_TEXT_COLOR_YELLOW,
    STR_TEXT_COLOR_TOPAZ,
    STR_TEXT_COLOR_CELADON,
    STR_TEXT_COLOR_BABYBLUE,
    STR_TEXT_COLOR_PALELAVENDER,
    STR_TEXT_COLOR_PALEGOLD,
    STR_TEXT_COLOR_LIGHTPINK,
    STR_TEXT_COLOR_PEARLAQUA,
    STR_TEXT_COLOR_PALESILVER,
};

static rct_widget window_banner_widgets[] = {
    { WWT_FRAME,            0,  0,          WW - 1, 0,      WH - 1,     0xFFFFFFFF,                 STR_NONE},                          // panel / background
    { WWT_CAPTION,          0,  1,          WW - 2, 1,      14,         STR_BANNER_WINDOW_TITLE,    STR_WINDOW_TITLE_TIP},              // title bar
    { WWT_CLOSEBOX,         0,  WW - 13,    WW - 3, 2,      13,         STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP},              // close x button
    { WWT_VIEWPORT,         1,  3,          WW - 26,17,     WH - 20,    0x0FFFFFFFE,                STR_NONE},                          // tab content panel
    { WWT_FLATBTN,          1,  WW - 25,    WW - 2, 19,     42,         SPR_RENAME,                 STR_CHANGE_BANNER_TEXT_TIP},        // change banner button
    { WWT_FLATBTN,          1,  WW - 25,    WW - 2, 43,     66,         SPR_NO_ENTRY,               STR_SET_AS_NO_ENTRY_BANNER_TIP},    // no entry button
    { WWT_FLATBTN,          1,  WW - 25,    WW - 2, 67,     90,         SPR_DEMOLISH,               STR_DEMOLISH_BANNER_TIP},           // demolish button
    { WWT_COLOURBTN,        1,  5,          16,     WH - 16,WH - 5,     0xFFFFFFFF,                 STR_SELECT_MAIN_SIGN_COLOUR_TIP},   // high money
    { WWT_DROPDOWN,         1,  43,         81,     WH - 16,WH - 5,     0xFFFFFFFF,                 STR_NONE},                          // high money
    { WWT_BUTTON,           1,  70,         80,     WH - 15,WH - 6,     STR_DROPDOWN_GLYPH,         STR_SELECT_TEXT_COLOUR_TIP},        // high money
    { WIDGETS_END },
};

static void window_banner_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_banner_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_banner_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_banner_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_banner_viewport_rotate(rct_window *w);
static void window_banner_invalidate(rct_window *w);
static void window_banner_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_banner_events = {
    nullptr,
    window_banner_mouseup,
    nullptr,
    window_banner_mousedown,
    window_banner_dropdown,
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
    window_banner_textinput,
    window_banner_viewport_rotate,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_banner_invalidate,
    window_banner_paint,
    nullptr
};
// clang-format on

/**
*
*  rct2: 0x006BA305
*/
rct_window * window_banner_open(rct_windownumber number)
{
    rct_window* w;
    rct_widget *viewportWidget;


    // Check if window is already open
    w = window_bring_to_front_by_number(WC_BANNER, number);
    if (w != nullptr)
        return w;

    w = window_create_auto_pos(WW, WH, &window_banner_events, WC_BANNER, WF_NO_SCROLLING);
    w->widgets = window_banner_widgets;
    w->enabled_widgets =
        (1 << WIDX_CLOSE) |
        (1 << WIDX_BANNER_TEXT) |
        (1 << WIDX_BANNER_NO_ENTRY) |
        (1 << WIDX_BANNER_DEMOLISH) |
        (1 << WIDX_MAIN_COLOUR) |
        (1 << WIDX_TEXT_COLOUR_DROPDOWN) |
        (1 << WIDX_TEXT_COLOUR_DROPDOWN_BUTTON);

    w->number = number;
    window_init_scroll_widgets(w);

    int32_t view_x = gBanners[w->number].x << 5;
    int32_t view_y = gBanners[w->number].y << 5;

    rct_tile_element* tile_element = map_get_first_element_at(view_x / 32, view_y / 32);
    while(1) {
        if (
            (tile_element->GetType() == TILE_ELEMENT_TYPE_BANNER) &&
            (tile_element->properties.banner.index == w->number)
        ) {
            break;
        }

        tile_element++;
    }

    int32_t view_z = tile_element->base_height<<3;
    w->frame_no = view_z;

    view_x += 16;
    view_y += 16;

    // Create viewport
    viewportWidget = &window_banner_widgets[WIDX_VIEWPORT];
    viewport_create(
        w,
        w->x + viewportWidget->left + 1,
        w->y + viewportWidget->top + 1,
        (viewportWidget->right - viewportWidget->left) - 2,
        (viewportWidget->bottom - viewportWidget->top) - 2,
        0,
        view_x,
        view_y,
        view_z,
        0,
        -1
    );

    w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
    window_invalidate(w);

    return w;
}

/**
 *
 *  rct2: 0x6ba4d6
 */
static void window_banner_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    rct_banner* banner = &gBanners[w->number];
    int32_t x = banner->x << 5;
    int32_t y = banner->y << 5;

    rct_tile_element* tile_element = map_get_first_element_at(x / 32, y / 32);

    while (1){
        if ((tile_element->GetType() == TILE_ELEMENT_TYPE_BANNER) &&
            (tile_element->properties.banner.index == w->number)) break;
        tile_element++;
    }

    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_BANNER_DEMOLISH:
        game_do_command(x, 1, y, tile_element->base_height | (tile_element->properties.banner.position << 8), GAME_COMMAND_REMOVE_BANNER, 0, 0);
        break;
    case WIDX_BANNER_TEXT:
        window_text_input_open(w, WIDX_BANNER_TEXT, STR_BANNER_TEXT, STR_ENTER_BANNER_TEXT, gBanners[w->number].string_idx, 0, 32);
        break;
    case WIDX_BANNER_NO_ENTRY:
        textinput_cancel();
        game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, banner->colour, GAME_COMMAND_SET_BANNER_STYLE, banner->text_colour, banner->flags ^ BANNER_FLAG_NO_ENTRY);
        break;
    }
}

/**
 *
 *  rct2: 0x6ba4ff
 */
static void window_banner_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    rct_banner* banner = &gBanners[w->number];

    switch (widgetIndex) {
    case WIDX_MAIN_COLOUR:
        window_dropdown_show_colour(w, widget, TRANSLUCENT(w->colours[1]), banner->colour);
        break;
    case WIDX_TEXT_COLOUR_DROPDOWN_BUTTON:

        for( int32_t i = 0; i < 13; ++i){
            gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[i] = BannerColouredTextFormats[i + 1];
        }

        // Switch to the dropdown box widget.
        widget--;

        window_dropdown_show_text_custom_width(
            widget->left + w->x,
            widget->top + w->y,
            widget->bottom - widget->top + 1,
            w->colours[1],
            0,
            DROPDOWN_FLAG_STAY_OPEN,
            13,
            widget->right - widget->left - 3);

        dropdown_set_checked(banner->text_colour - 1, true);
        break;
    }
}

/**
 *
 *  rct2: 0x6ba517
 */
static void window_banner_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    rct_banner* banner = &gBanners[w->number];

    switch(widgetIndex){
    case WIDX_MAIN_COLOUR:
        if (dropdownIndex == -1)
            break;

        game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, dropdownIndex, GAME_COMMAND_SET_BANNER_STYLE, banner->text_colour, banner->flags);
        break;
    case WIDX_TEXT_COLOUR_DROPDOWN_BUTTON:
        if (dropdownIndex == -1)
            break;

        game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, banner->colour, GAME_COMMAND_SET_BANNER_STYLE, dropdownIndex + 1, banner->flags);
        break;
    }
}

/**
 *
 *  rct2: 0x6ba50c
 */
static void window_banner_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (widgetIndex == WIDX_BANNER_TEXT && text != nullptr)
    {
        auto bannerSetNameAction = BannerSetNameAction(w->number, text);
        GameActions::Execute(&bannerSetNameAction);
    }
}

/**
 *
 *  rct2: 0x006BA44D
 */
static void window_banner_invalidate(rct_window *w)
{
    rct_banner* banner = &gBanners[w->number];
    rct_widget* colour_btn = &window_banner_widgets[WIDX_MAIN_COLOUR];
    colour_btn->type = WWT_EMPTY;

    // Scenery item not sure why we use this instead of banner?
    rct_scenery_entry* sceneryEntry = get_banner_entry(banner->type);
    if (sceneryEntry->banner.flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
    {
        colour_btn->type = WWT_COLOURBTN;
    }

    w->pressed_widgets &= ~(1ULL<<WIDX_BANNER_NO_ENTRY);
    w->disabled_widgets &= ~(
        (1ULL<<WIDX_BANNER_TEXT)|
        (1ULL<<WIDX_TEXT_COLOUR_DROPDOWN)|
        (1ULL<<WIDX_TEXT_COLOUR_DROPDOWN_BUTTON));

    if (banner->flags & BANNER_FLAG_NO_ENTRY){
        w->pressed_widgets |= (1ULL<<WIDX_BANNER_NO_ENTRY);
        w->disabled_widgets |=
            (1ULL<<WIDX_BANNER_TEXT)|
            (1ULL<<WIDX_TEXT_COLOUR_DROPDOWN)|
            (1ULL<<WIDX_TEXT_COLOUR_DROPDOWN_BUTTON);
    }

    colour_btn->image = SPRITE_ID_PALETTE_COLOUR_1(banner->colour) | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;

    rct_widget* drop_down_widget = &window_banner_widgets[WIDX_TEXT_COLOUR_DROPDOWN];
    drop_down_widget->text = BannerColouredTextFormats[banner->text_colour];
}

/* rct2: 0x006BA4C5 */
static void window_banner_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    // Draw viewport
    if (w->viewport != nullptr) {
        window_draw_viewport(dpi, w);
    }
}

/**
 *
 *  rct2: 0x6BA7B5
 */
static void window_banner_viewport_rotate(rct_window *w)
{
    rct_viewport* view = w->viewport;
    w->viewport = nullptr;

    view->width = 0;

    rct_banner* banner = &gBanners[w->number];

    int32_t view_x = (banner->x << 5) + 16;
    int32_t view_y = (banner->y << 5) + 16;
    int32_t view_z = w->frame_no;

    // Create viewport
    rct_widget* viewportWidget = &window_banner_widgets[WIDX_VIEWPORT];
    viewport_create(
        w,
        w->x + viewportWidget->left + 1,
        w->y + viewportWidget->top + 1,
        (viewportWidget->right - viewportWidget->left) - 1,
        (viewportWidget->bottom - viewportWidget->top) - 1,
        0,
        view_x,
        view_y,
        view_z,
        0,
        -1
    );

    w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
    window_invalidate(w);
}
