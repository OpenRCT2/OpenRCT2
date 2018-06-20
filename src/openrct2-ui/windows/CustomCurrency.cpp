/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/config/Config.h>
#include <openrct2-ui/windows/Window.h>

#include <openrct2/localisation/Localisation.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/util/Util.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/drawing/Drawing.h>

// clang-format off
enum WINDOW_CUSTOM_CURRENCY_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_RATE,
    WIDX_RATE_UP,
    WIDX_RATE_DOWN,
    WIDX_SYMBOL_TEXT,
    WIDX_AFFIX_DROPDOWN,
    WIDX_AFFIX_DROPDOWN_BUTTON,
};

static rct_widget window_custom_currency_widgets[] = {
    { WWT_FRAME,            0,  0,      399,    0,      99, 0xFFFFFFFF,                         STR_NONE },
    { WWT_CAPTION,          0,  1,      398,    1,      14, STR_CUSTOM_CURRENCY_WINDOW_TITLE,   STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  387,    397,    2,      13, STR_CLOSE_X,                        STR_CLOSE_WINDOW_TIP },
      SPINNER_WIDGETS      (1,  100,    200,    30,     40, STR_CHEAT_CURRENCY_FORMAT,          STR_NONE), // NB: 3 widgets
    { WWT_BUTTON,           1,  120,    200,    50,     60, 0,                                  STR_NONE },
    { WWT_DROPDOWN,         1,  220,    350,    50,     60, STR_NONE,                           STR_NONE },
    { WWT_BUTTON,           1,  339,    349,    51,     59, STR_DROPDOWN_GLYPH,                 STR_NONE },
    { WIDGETS_END },
};


static void custom_currency_window_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void custom_currency_window_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void custom_currency_window_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void custom_currency_window_text_input(struct rct_window *w, rct_widgetindex widgetIndex, char *text);
static void custom_currency_window_paint(rct_window *w, rct_drawpixelinfo *dpi);


static rct_window_event_list _windowCustomCurrencyEvents = {
    nullptr,
    custom_currency_window_mouseup,
    nullptr,
    custom_currency_window_mousedown,
    custom_currency_window_dropdown,
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
    custom_currency_window_text_input,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    custom_currency_window_paint,
    nullptr
};
// clang-format on


rct_window * custom_currency_window_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_CUSTOM_CURRENCY_CONFIG);
    if(window != nullptr)
        return window;

    window = window_create_centred(
        400,
        100,
        &_windowCustomCurrencyEvents,
        WC_CUSTOM_CURRENCY_CONFIG,
        0
    );
    window->widgets = window_custom_currency_widgets;
    window->enabled_widgets =
        (1 << WIDX_CLOSE) |
        (1 << WIDX_RATE) |
        (1 << WIDX_RATE_UP) |
        (1 << WIDX_RATE_DOWN) |
        (1 << WIDX_SYMBOL_TEXT) |
        (1 << WIDX_AFFIX_DROPDOWN) |
        (1 << WIDX_AFFIX_DROPDOWN_BUTTON);

    window->hold_down_widgets = (1 << WIDX_RATE_UP) | (1 << WIDX_RATE_DOWN);
    window_init_scroll_widgets(window);
    window->colours[0] = COLOUR_LIGHT_BROWN;
    window->colours[1] = COLOUR_LIGHT_BROWN;
    window->colours[2] = COLOUR_LIGHT_BROWN;

    return window;
}



static void custom_currency_window_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget)
{
    widget = &w->widgets[widgetIndex - 1];

    switch(widgetIndex) {

    case WIDX_CLOSE:
        window_close(w);
        break;

    case WIDX_RATE_UP:
        CurrencyDescriptors[CURRENCY_CUSTOM].rate += 1;
        gConfigGeneral.custom_currency_rate = CurrencyDescriptors[CURRENCY_CUSTOM].rate;
        config_save_default();
        window_invalidate_all();
        break;

    case WIDX_RATE_DOWN:
        if(CurrencyDescriptors[CURRENCY_CUSTOM].rate > 1) {
            CurrencyDescriptors[CURRENCY_CUSTOM].rate -= 1;
            gConfigGeneral.custom_currency_rate = CurrencyDescriptors[CURRENCY_CUSTOM].rate;
            config_save_default();
            window_invalidate_all();
        }
        break;

    case WIDX_AFFIX_DROPDOWN_BUTTON:
        gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[0] = STR_PREFIX;

        gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[1] = STR_SUFFIX;

        window_dropdown_show_text_custom_width(
            w->x + widget->left,
            w->y + widget->top,
            widget->bottom - widget->top + 1,
            w->colours[1],
            0,
            DROPDOWN_FLAG_STAY_OPEN,
            2,
            widget->right - widget->left - 3
        );

        if(CurrencyDescriptors[CURRENCY_CUSTOM].affix_unicode == CURRENCY_PREFIX) {
            dropdown_set_checked(0, true);
        } else {
            dropdown_set_checked(1, true);
        }

        break;

    case WIDX_SYMBOL_TEXT:
        window_text_input_raw_open(
            w,
            WIDX_SYMBOL_TEXT,
            STR_CUSTOM_CURRENCY_SYMBOL_INPUT_TITLE,
            STR_CUSTOM_CURRENCY_SYMBOL_INPUT_DESC,
            CurrencyDescriptors[CURRENCY_CUSTOM].symbol_unicode,
            CURRENCY_SYMBOL_MAX_SIZE
        );
        break;

    }
}

static void custom_currency_window_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch(widgetIndex) {
    case WIDX_RATE:
        window_text_input_open(
            w,
            WIDX_RATE,
            STR_RATE_INPUT_TITLE,
            STR_RATE_INPUT_DESC,
            STR_FORMAT_INTEGER,
            (uint32_t)CurrencyDescriptors[CURRENCY_CUSTOM].rate,
            CURRENCY_RATE_MAX_NUM_DIGITS
        );
        break;
    }
}

static void custom_currency_window_dropdown([[maybe_unused]] rct_window * w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if(dropdownIndex == -1)
        return;

    if(widgetIndex == WIDX_AFFIX_DROPDOWN_BUTTON) {

        if(dropdownIndex == 0) {
            CurrencyDescriptors[CURRENCY_CUSTOM].affix_ascii = CURRENCY_PREFIX;
            CurrencyDescriptors[CURRENCY_CUSTOM].affix_unicode = CURRENCY_PREFIX;
        } else if(dropdownIndex == 1) {
            CurrencyDescriptors[CURRENCY_CUSTOM].affix_ascii = CURRENCY_SUFFIX;
            CurrencyDescriptors[CURRENCY_CUSTOM].affix_unicode = CURRENCY_SUFFIX;
        }


        gConfigGeneral.custom_currency_affix = CurrencyDescriptors[CURRENCY_CUSTOM].affix_unicode;
        config_save_default();

        window_invalidate_all();

    }
}

static void custom_currency_window_text_input([[maybe_unused]] struct rct_window * w, rct_widgetindex widgetIndex, char * text)
{
    if (text == nullptr)
        return;
    int32_t rate;
    char* end;
    switch(widgetIndex){
    case WIDX_SYMBOL_TEXT:
        safe_strcpy(
            CurrencyDescriptors[CURRENCY_CUSTOM].symbol_unicode,
            text,
            CURRENCY_SYMBOL_MAX_SIZE
        );

        safe_strcpy(
            gConfigGeneral.custom_currency_symbol,
            CurrencyDescriptors[CURRENCY_CUSTOM].symbol_unicode,
            CURRENCY_SYMBOL_MAX_SIZE
        );

        config_save_default();
        window_invalidate_all();
        break;

    case WIDX_RATE:
        rate = strtol(text, &end, 10);
        if (*end == '\0') {
            CurrencyDescriptors[CURRENCY_CUSTOM].rate = rate;
            gConfigGeneral.custom_currency_rate = CurrencyDescriptors[CURRENCY_CUSTOM].rate;
            config_save_default();
            window_invalidate_all();
        }
        break;
    }
}


static void custom_currency_window_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    int32_t x, y;

    set_format_arg(0, int32_t, 100);

    window_draw_widgets(w, dpi);

    x = w->x + 10;
    y = w->y + 30;

    gfx_draw_string_left(dpi, STR_RATE, nullptr, w->colours[1], x, y);

    int32_t baseExchange = CurrencyDescriptors[CURRENCY_POUNDS].rate;
    set_format_arg(0, int32_t, baseExchange);
    gfx_draw_string_left(dpi, STR_CUSTOM_CURRENCY_EQUIVALENCY, gCommonFormatArgs, w->colours[1], x + 200, y);

    y += 20;

    gfx_draw_string_left(dpi, STR_CURRENCY_SYMBOL_TEXT, nullptr, w->colours[1], x, y);

    gfx_draw_string(
        dpi,
        CurrencyDescriptors[CURRENCY_CUSTOM].symbol_unicode,
        w->colours[1],
        w->x + window_custom_currency_widgets[WIDX_SYMBOL_TEXT].left + 1,
        w->y + window_custom_currency_widgets[WIDX_SYMBOL_TEXT].top);

    if(CurrencyDescriptors[CURRENCY_CUSTOM].affix_unicode == CURRENCY_PREFIX) {
        gfx_draw_string_left(
            dpi,
            STR_PREFIX,
            w,
            w->colours[1],
            w->x + window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].left + 1,
            w->y + window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].top
        );
    } else {
        gfx_draw_string_left(
            dpi,
            STR_SUFFIX,
            w,
            w->colours[1],
            w->x + window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].left + 1,
            w->y + window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].top
        );
    }
}
