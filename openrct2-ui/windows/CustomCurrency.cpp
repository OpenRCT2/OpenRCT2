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
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/util/Util.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_CUSTOM_CURRENCY_WINDOW_TITLE;
static constexpr const int32_t WH = 100;
static constexpr const int32_t WW = 400;

// clang-format off
enum WindowCustomCurrencyWidgetIdx {
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
    MakeSpinnerWidgets({100, 30}, {101, 11}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_CURRENCY_FORMAT), // NB: 3 widgets
    MakeWidget        ({120, 50}, { 81, 11}, WindowWidgetType::Button,   WindowColour::Secondary, STR_EMPTY          ),
    MakeWidget        ({220, 50}, {131, 11}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                 ),
    MakeWidget        ({339, 51}, { 11,  9}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH ),
    WIDGETS_END,
};

// clang-format on

class CustomCurrencyWindow final : public Window
{
public:
    void OnOpen() override
    {
        widgets = window_custom_currency_widgets;
        hold_down_widgets = (1ULL << WIDX_RATE_UP) | (1ULL << WIDX_RATE_DOWN);
        WindowInitScrollWidgets(this);
        colours[0] = COLOUR_LIGHT_BROWN;
        colours[1] = COLOUR_LIGHT_BROWN;
        colours[2] = COLOUR_LIGHT_BROWN;
    }

    void OnMouseDown(rct_widgetindex widgetIndex) override
    {
        auto* widget = &widgets[widgetIndex - 1];

        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
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
                gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[0].Args = STR_PREFIX;

                gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[1].Args = STR_SUFFIX;

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0,
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
                WindowTextInputRawOpen(
                    this, WIDX_SYMBOL_TEXT, STR_CUSTOM_CURRENCY_SYMBOL_INPUT_TITLE, STR_CUSTOM_CURRENCY_SYMBOL_INPUT_DESC, {},
                    CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode, CURRENCY_SYMBOL_MAX_SIZE);
                break;
        }
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_RATE:
                WindowTextInputOpen(
                    this, WIDX_RATE, STR_RATE_INPUT_TITLE, STR_RATE_INPUT_DESC, {}, STR_FORMAT_INTEGER,
                    static_cast<uint32_t>(CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate),
                    CURRENCY_RATE_MAX_NUM_DIGITS);
                break;
        }
    }

    void OnDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex) override
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

    void OnTextInput(rct_widgetindex widgetIndex, std::string_view text) override
    {
        if (text.empty())
            return;

        int32_t rate;

        switch (widgetIndex)
        {
            case WIDX_SYMBOL_TEXT:
                safe_strcpy(
                    CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode, std::string(text).c_str(),
                    CURRENCY_SYMBOL_MAX_SIZE);

                safe_strcpy(
                    gConfigGeneral.custom_currency_symbol, CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode,
                    CURRENCY_SYMBOL_MAX_SIZE);

                config_save_default();
                window_invalidate_all();
                break;

            case WIDX_RATE:
                const auto res = String::Parse<int32_t>(text);
                if (res.has_value())
                {
                    rate = res.value();
                    CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate = rate;
                    gConfigGeneral.custom_currency_rate = CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate;
                    config_save_default();
                    window_invalidate_all();
                }
                break;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        auto ft = Formatter::Common();
        ft.Add<money64>(10.00_GBP);

        DrawWidgets(dpi);

        auto screenCoords = windowPos + ScreenCoordsXY{ 10, 30 };

        DrawTextBasic(&dpi, screenCoords, STR_RATE, {}, { colours[1] });

        int32_t baseExchange = CurrencyDescriptors[EnumValue(CurrencyType::Pounds)].rate;
        ft = Formatter();
        ft.Add<int32_t>(baseExchange);
        DrawTextBasic(&dpi, screenCoords + ScreenCoordsXY{ 200, 0 }, STR_CUSTOM_CURRENCY_EQUIVALENCY, ft, { colours[1] });

        screenCoords.y += 20;

        DrawTextBasic(&dpi, screenCoords, STR_CURRENCY_SYMBOL_TEXT, {}, { colours[1] });

        screenCoords = windowPos
            + ScreenCoordsXY{ window_custom_currency_widgets[WIDX_SYMBOL_TEXT].left + 1,
                              window_custom_currency_widgets[WIDX_SYMBOL_TEXT].top };

        gfx_draw_string(
            &dpi, screenCoords, CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode, { colours[1] });

        auto drawPos = windowPos
            + ScreenCoordsXY{ window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].left + 1,
                              window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].top };
        rct_string_id stringId = (CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_unicode == CurrencyAffix::Prefix)
            ? STR_PREFIX
            : STR_SUFFIX;
        DrawTextBasic(&dpi, drawPos, stringId, {}, { colours[1] });
    }
};

rct_window* CustomCurrencyWindowOpen()
{
    return WindowFocusOrCreate<CustomCurrencyWindow>(WC_CUSTOM_CURRENCY_CONFIG, WW, WH, WF_CENTRE_SCREEN);
}
