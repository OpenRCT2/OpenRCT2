/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/UiStringIds.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Text.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Currency.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_CUSTOM_CURRENCY_WINDOW_TITLE;
    static constexpr ScreenSize kWindowSize = { 400, 100 };

    enum WindowCustomCurrencyWidgetIdx
    {
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

    // clang-format off
    static constexpr auto window_custom_currency_widgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeSpinnerWidgets({100, 30}, {101, 11}, WidgetType::spinner,      WindowColour::secondary, STR_CURRENCY_FORMAT), // NB: 3 widgets
        makeWidget        ({120, 50}, { 81, 11}, WidgetType::button,       WindowColour::secondary, kStringIdEmpty     ),
        makeWidget        ({220, 50}, {131, 11}, WidgetType::dropdownMenu, WindowColour::secondary                     ),
        makeWidget        ({339, 51}, { 11,  9}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH )
    );
    // clang-format on

    class CustomCurrencyWindow final : public Window
    {
    public:
        void OnOpen() override
        {
            SetWidgets(window_custom_currency_widgets);
            hold_down_widgets = (1uLL << WIDX_RATE_UP) | (1uLL << WIDX_RATE_DOWN);
            WindowInitScrollWidgets(*this);
            colours[0] = COLOUR_LIGHT_BROWN;
            colours[1] = COLOUR_LIGHT_BROWN;
            colours[2] = COLOUR_LIGHT_BROWN;
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            auto* widget = &widgets[widgetIndex - 1];
            auto* windowMgr = Ui::GetWindowManager();

            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_RATE_UP:
                    CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate += 1;
                    Config::Get().general.CustomCurrencyRate = CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate;
                    Config::Save();
                    windowMgr->InvalidateAll();
                    break;
                case WIDX_RATE_DOWN:
                    if (CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate > 1)
                    {
                        CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate -= 1;
                        Config::Get().general.CustomCurrencyRate = CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate;
                        Config::Save();
                        windowMgr->InvalidateAll();
                    }
                    break;
                case WIDX_AFFIX_DROPDOWN_BUTTON:
                    gDropdown.items[0] = Dropdown::MenuLabel(STR_PREFIX);
                    gDropdown.items[1] = Dropdown::MenuLabel(STR_SUFFIX);

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0,
                        Dropdown::Flag::StayOpen, 2, widget->width() - 3);

                    if (CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_unicode == CurrencyAffix::Prefix)
                    {
                        gDropdown.items[0].setChecked(true);
                    }
                    else
                    {
                        gDropdown.items[1].setChecked(true);
                    }

                    break;
                case WIDX_SYMBOL_TEXT:
                    WindowTextInputRawOpen(
                        this, WIDX_SYMBOL_TEXT, STR_CUSTOM_CURRENCY_SYMBOL_INPUT_TITLE, STR_CUSTOM_CURRENCY_SYMBOL_INPUT_DESC,
                        {}, CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode, kCurrencySymbolMaxSize);
                    break;
            }
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_RATE:
                    WindowTextInputOpen(
                        this, WIDX_RATE, STR_RATE_INPUT_TITLE, STR_RATE_INPUT_DESC, {}, STR_FORMAT_INTEGER,
                        static_cast<uint32_t>(CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate),
                        kCurrencyRateMaxNumDigits);
                    break;
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
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

                Config::Get().general.CustomCurrencyAffix = CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_unicode;
                Config::Save();

                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->InvalidateAll();
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (text.empty())
                return;

            auto* windowMgr = Ui::GetWindowManager();

            switch (widgetIndex)
            {
                case WIDX_SYMBOL_TEXT:
                    String::safeUtf8Copy(
                        CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode, std::string(text).c_str(),
                        kCurrencySymbolMaxSize);

                    Config::Get().general.CustomCurrencySymbol = CurrencyDescriptors[EnumValue(CurrencyType::Custom)]
                                                                     .symbol_unicode;

                    Config::Save();

                    windowMgr->InvalidateAll();
                    break;

                case WIDX_RATE:
                    const auto res = String::Parse<int32_t>(text);
                    if (res.has_value())
                    {
                        int32_t rate = res.value();
                        CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate = rate;
                        Config::Get().general.CustomCurrencyRate = CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate;
                        Config::Save();
                        windowMgr->InvalidateAll();
                    }
                    break;
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            auto ft = Formatter::Common();
            ft.Add<money64>(10.00_GBP);

            DrawWidgets(rt);

            auto screenCoords = windowPos + ScreenCoordsXY{ 10, 18 + widgets[WIDX_TITLE].height() };

            DrawTextBasic(rt, screenCoords, STR_RATE, {}, { colours[1] });

            int32_t baseExchange = CurrencyDescriptors[EnumValue(CurrencyType::Pounds)].rate;
            ft = Formatter();
            ft.Add<int32_t>(baseExchange);
            DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ 200, 0 }, STR_CUSTOM_CURRENCY_EQUIVALENCY, ft, { colours[1] });

            screenCoords.y += 20;

            DrawTextBasic(rt, screenCoords, STR_CURRENCY_SYMBOL_TEXT, {}, { colours[1] });

            screenCoords = windowPos + ScreenCoordsXY{ widgets[WIDX_SYMBOL_TEXT].left + 1, widgets[WIDX_SYMBOL_TEXT].top };

            DrawText(rt, screenCoords, { colours[1] }, CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode);

            auto drawPos = windowPos
                + ScreenCoordsXY{ widgets[WIDX_AFFIX_DROPDOWN].left + 1, widgets[WIDX_AFFIX_DROPDOWN].top };
            StringId stringId = (CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_unicode == CurrencyAffix::Prefix)
                ? STR_PREFIX
                : STR_SUFFIX;
            DrawTextBasic(rt, drawPos, stringId, {}, { colours[1] });
        }
    };

    WindowBase* CustomCurrencyOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<CustomCurrencyWindow>(WindowClass::CustomCurrencyConfig, kWindowSize, WF_CENTRE_SCREEN);
    }
} // namespace OpenRCT2::Ui::Windows
