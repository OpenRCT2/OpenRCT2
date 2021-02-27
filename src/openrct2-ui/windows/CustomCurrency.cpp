/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/util/Util.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_CUSTOM_CURRENCY_WINDOW_TITLE;
static constexpr const int32_t WH = 100;
static constexpr const int32_t WW = 400;

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
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeSpinnerWidgets({100, 30}, {101, 11}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_CHEAT_CURRENCY_FORMAT), // NB: 3 widgets
    MakeWidget        ({120, 50}, { 81, 11}, WindowWidgetType::Button,   WindowColour::Secondary, STR_EMPTY                ),
    MakeWidget        ({220, 50}, {131, 11}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                           ),
    MakeWidget        ({339, 51}, { 11,  9}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH       ),
    { WIDGETS_END },
};


static void custom_currency_window_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void custom_currency_window_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void custom_currency_window_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void custom_currency_window_text_input(struct rct_window *w, rct_widgetindex widgetIndex, char *text);
static void custom_currency_window_paint(rct_window *w, rct_drawpixelinfo *dpi);


static rct_window_event_list _windowCustomCurrencyEvents([](auto& events)
{
    events.mouse_up = &custom_currency_window_mouseup;
    events.mouse_down = &custom_currency_window_mousedown;
    events.dropdown = &custom_currency_window_dropdown;
    events.text_input = &custom_currency_window_text_input;
    events.paint = &custom_currency_window_paint;
});
// clang-format on

rct_window* custom_currency_window_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_CUSTOM_CURRENCY_CONFIG);
    if (window != nullptr)
        return window;

    window = WindowCreateCentred(400, 100, &_windowCustomCurrencyEvents, WC_CUSTOM_CURRENCY_CONFIG, 0);
    window->widgets = window_custom_currency_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_RATE) | (1 << WIDX_RATE_UP) | (1 << WIDX_RATE_DOWN)
        | (1 << WIDX_SYMBOL_TEXT) | (1 << WIDX_AFFIX_DROPDOWN) | (1 << WIDX_AFFIX_DROPDOWN_BUTTON);

    window->hold_down_widgets = (1 << WIDX_RATE_UP) | (1 << WIDX_RATE_DOWN);
    WindowInitScrollWidgets(window);
    window->colours[0] = COLOUR_LIGHT_BROWN;
    window->colours[1] = COLOUR_LIGHT_BROWN;
    window->colours[2] = COLOUR_LIGHT_BROWN;

    return window;
}

static void custom_currency_window_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    widget = &w->widgets[widgetIndex - 1];

    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;

        case WIDX_RATE_UP:
            CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate += 1;
            gConfigGeneral.custom_currency_rate = CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate;
            config_save_default();
            window_invalidate_all();
            break;

        case WIDX_RATE_DOWN:
            if (CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate > 1)
            {
                CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate -= 1;
                gConfigGeneral.custom_currency_rate = CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate;
                config_save_default();
                window_invalidate_all();
            }
            break;

        case WIDX_AFFIX_DROPDOWN_BUTTON:
            gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[0] = STR_PREFIX;

            gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[1] = STR_SUFFIX;

            WindowDropdownShowTextCustomWidth(
                { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
                Dropdown::Flag::StayOpen, 2, widget->width() - 3);

            if (CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_unicode == CurrencyAffix::Prefix)
            {
                Dropdown::SetChecked(0, true);
            }
            else
            {
                Dropdown::SetChecked(1, true);
            }

            break;

        case WIDX_SYMBOL_TEXT:
            window_text_input_raw_open(
                w, WIDX_SYMBOL_TEXT, STR_CUSTOM_CURRENCY_SYMBOL_INPUT_TITLE, STR_CUSTOM_CURRENCY_SYMBOL_INPUT_DESC,
                CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode, CURRENCY_SYMBOL_MAX_SIZE);
            break;
    }
}

static void custom_currency_window_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_RATE:
            window_text_input_open(
                w, WIDX_RATE, STR_RATE_INPUT_TITLE, STR_RATE_INPUT_DESC, STR_FORMAT_INTEGER,
                static_cast<uint32_t>(CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate), CURRENCY_RATE_MAX_NUM_DIGITS);
            break;
    }
}

static void custom_currency_window_dropdown([[maybe_unused]] rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    if (widgetIndex == WIDX_AFFIX_DROPDOWN_BUTTON)
    {
        if (dropdownIndex == 0)
        {
            CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_ascii = CurrencyAffix::Prefix;
            CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_unicode = CurrencyAffix::Prefix;
        }
        else if (dropdownIndex == 1)
        {
            CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_ascii = CurrencyAffix::Suffix;
            CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_unicode = CurrencyAffix::Suffix;
        }

        gConfigGeneral.custom_currency_affix = CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_unicode;
        config_save_default();

        window_invalidate_all();
    }
}

static void custom_currency_window_text_input([[maybe_unused]] struct rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (text == nullptr)
        return;
    int32_t rate;
    char* end;
    switch (widgetIndex)
    {
        case WIDX_SYMBOL_TEXT:
            safe_strcpy(CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode, text, CURRENCY_SYMBOL_MAX_SIZE);

            safe_strcpy(
                gConfigGeneral.custom_currency_symbol, CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode,
                CURRENCY_SYMBOL_MAX_SIZE);

            config_save_default();
            window_invalidate_all();
            break;

        case WIDX_RATE:
            rate = strtol(text, &end, 10);
            if (*end == '\0')
            {
                CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate = rate;
                gConfigGeneral.custom_currency_rate = CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate;
                config_save_default();
                window_invalidate_all();
            }
            break;
    }
}

static void custom_currency_window_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    auto ft = Formatter::Common();
    ft.Add<int32_t>(100);

    WindowDrawWidgets(w, dpi);

    auto screenCoords = w->windowPos + ScreenCoordsXY{ 10, 30 };

    DrawTextBasic(dpi, screenCoords, STR_RATE, {}, { w->colours[1] });

    int32_t baseExchange = CurrencyDescriptors[EnumValue(CurrencyType::Pounds)].rate;
    ft = Formatter();
    ft.Add<int32_t>(baseExchange);
    DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 200, 0 }, STR_CUSTOM_CURRENCY_EQUIVALENCY, ft, { w->colours[1] });

    screenCoords.y += 20;

    DrawTextBasic(dpi, screenCoords, STR_CURRENCY_SYMBOL_TEXT, {}, { w->colours[1] });

    screenCoords = w->windowPos
        + ScreenCoordsXY{ window_custom_currency_widgets[WIDX_SYMBOL_TEXT].left + 1,
                          window_custom_currency_widgets[WIDX_SYMBOL_TEXT].top };

    gfx_draw_string(dpi, screenCoords, CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode, { w->colours[1] });

    auto drawPos = w->windowPos
        + ScreenCoordsXY{ window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].left + 1,
                          window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].top };
    rct_string_id stringId = (CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_unicode == CurrencyAffix::Prefix)
        ? STR_PREFIX
        : STR_SUFFIX;
    DrawTextBasic(dpi, drawPos, stringId, w, { w->colours[1] });
}
