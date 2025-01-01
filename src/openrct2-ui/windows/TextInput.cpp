/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <SDL.h>
#include <SDL_keycode.h>
#include <iterator>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/core/String.hpp>
#include <openrct2/core/UTF8.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr int32_t WW = 250;
    static constexpr int32_t WH = 90;

    enum WindowTextInputWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CANCEL,
        WIDX_OKAY
    };

    static Widget _textInputWidgets[] = {
        WINDOW_SHIM(STR_NONE, WW, WH),
        MakeWidget({ 170, 68 }, { 71, 14 }, WindowWidgetType::Button, WindowColour::Secondary, STR_CANCEL),
        MakeWidget({ 10, 68 }, { 71, 14 }, WindowWidgetType::Button, WindowColour::Secondary, STR_OK),
        kWidgetsEnd,
    };

    class TextInputWindow final : public Window
    {
    private:
        WidgetIdentifier _parentWidget{};

        std::string _title;
        StringId _titleStringId = STR_NONE;

        std::string _description;
        StringId _descriptionStringId = STR_NONE;
        Formatter _descriptionArgs;

        std::function<void(std::string_view)> _callback;
        std::function<void()> _cancelCallback;

        int32_t _cursorBlink{};
        size_t _maxInputLength{};
        u8string _buffer;

    public:
        void OnOpen() override
        {
            widgets = _textInputWidgets;
            WindowInitScrollWidgets(*this);
            SetParentWindow(nullptr, 0);
        }

        void SetParentWindow(WindowBase* parentWindow, WidgetIndex widgetIndex)
        {
            // Save calling window details so that the information can be passed back to the correct window & widget
            if (parentWindow == nullptr)
            {
                _parentWidget.window.classification = WindowClass::Null;
                _parentWidget.window.number = 0;
                _parentWidget.widget_index = 0;

                colours[0] = COLOUR_GREY;
                colours[1] = COLOUR_GREY;
                colours[2] = COLOUR_GREY;
            }
            else
            {
                _parentWidget.window.classification = parentWindow->classification;
                _parentWidget.window.number = parentWindow->number;
                _parentWidget.widget_index = widgetIndex;

                colours[0] = parentWindow->colours[1];
                colours[1] = parentWindow->colours[1];
                colours[2] = parentWindow->colours[1];
            }
        }

        void SetTitle(StringId title, StringId description, const Formatter& decriptionArgs)
        {
            _titleStringId = title;
            _descriptionStringId = description;
            _descriptionArgs = decriptionArgs;
        }

        void SetTitle(std::string_view title, std::string_view description)
        {
            _titleStringId = STR_NONE;
            _title = title;
            _descriptionStringId = STR_NONE;
            _description = description;
        }

        void SetText(std::string_view text, size_t maxLength)
        {
            text = String::utf8TruncateCodePoints(text, maxLength);
            _buffer = u8string{ text };
            _maxInputLength = maxLength;
            SetTexboxSession(ContextStartTextInput(_buffer, maxLength));
        }

        void SetCallback(std::function<void(std::string_view)> callback, std::function<void()> cancelCallback)
        {
            _callback = callback;
            _cancelCallback = cancelCallback;
        }

        void OnClose() override
        {
            // Make sure that we take it out of the text input
            // mode otherwise problems may occur.
            ContextStopTextInput();
        }

        void OnUpdate() override
        {
            if (HasParentWindow())
            {
                // If the calling window is closed then close the text input window
                auto parentWindow = GetParentWindow();
                if (parentWindow == nullptr)
                {
                    WindowClose(*this);
                    return;
                }
            }

            // Used to blink the cursor.
            _cursorBlink++;
            if (_cursorBlink > 30)
                _cursorBlink = 0;

            Invalidate();
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CANCEL:
                case WIDX_CLOSE:
                    ContextStopTextInput();
                    ExecuteCallback(false);
                    WindowClose(*this);
                    break;
                case WIDX_OKAY:
                    ContextStopTextInput();
                    ExecuteCallback(true);
                    WindowClose(*this);
            }
        }

        void OnPrepareDraw() override
        {
            // Change window size if required.
            int32_t newHeight = CalculateWindowHeight(_buffer.data());
            if (newHeight != height)
            {
                WindowSetResize(*this, WW, newHeight, WW, newHeight);
            }

            widgets[WIDX_OKAY].top = newHeight - 22;
            widgets[WIDX_OKAY].bottom = newHeight - 9;
            widgets[WIDX_CANCEL].top = newHeight - 22;
            widgets[WIDX_CANCEL].bottom = newHeight - 9;
            widgets[WIDX_BACKGROUND].bottom = newHeight - 1;

            // Set window title argument
            if (_titleStringId == STR_NONE)
            {
                auto ft = Formatter::Common();
                ft.Add<const char*>(_title.c_str());
                widgets[WIDX_TITLE].text = STR_STRING;
            }
            else
            {
                widgets[WIDX_TITLE].text = _titleStringId;
            }
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);

            ScreenCoordsXY screenCoords;
            screenCoords.y = windowPos.y + 25;

            int32_t no_lines = 0;

            if (_descriptionStringId == STR_NONE)
            {
                auto ft = Formatter();
                ft.Add<const char*>(_description.c_str());
                DrawTextWrapped(
                    dpi, { windowPos.x + WW / 2, screenCoords.y }, WW, STR_STRING, ft, { colours[1], TextAlignment::CENTRE });
            }
            else
            {
                DrawTextWrapped(
                    dpi, { windowPos.x + WW / 2, screenCoords.y }, WW, _descriptionStringId, _descriptionArgs,
                    { colours[1], TextAlignment::CENTRE });
            }

            screenCoords.y += 25;

            // String length needs to add 12 either side of box
            // +13 for cursor when max length.
            u8string wrappedString;
            GfxWrapString(
                u8string_view{ _buffer.data(), _buffer.size() }, WW - (24 + 13), FontStyle::Medium, &wrappedString, &no_lines);

            GfxFillRectInset(
                dpi,
                { { windowPos.x + 10, screenCoords.y }, { windowPos.x + WW - 10, screenCoords.y + 10 * (no_lines + 1) + 3 } },
                colours[1], INSET_RECT_F_60);

            screenCoords.y += 1;

            const utf8* wrapPointer = wrappedString.data();
            size_t char_count = 0;
            uint8_t cur_drawn = 0;

            auto* textInput = GetTextboxSession();
            int32_t cursorX = 0;
            int32_t cursorY = 0;
            for (int32_t line = 0; line <= no_lines; line++)
            {
                screenCoords.x = windowPos.x + 12;
                DrawText(dpi, screenCoords, { colours[1], FontStyle::Medium, TextAlignment::LEFT }, wrapPointer, true);

                size_t string_length = GetStringSize(wrapPointer) - 1;
                if (!cur_drawn && (textInput->SelectionStart <= char_count + string_length))
                {
                    // Make a view of the string for measuring the width.
                    cursorX = windowPos.x + 13
                        + GfxGetStringWidthNoFormatting(
                                  u8string_view{ wrapPointer, textInput->SelectionStart - char_count }, FontStyle::Medium);
                    cursorY = screenCoords.y;

                    int32_t textWidth = 6;
                    if (textInput->SelectionStart < strlen(_buffer.data()))
                    {
                        // Make a 1 utf8-character wide string for measuring the width
                        // of the currently selected character.
                        utf8 tmp[5] = {}; // This is easier than setting temp_string[0..5]
                        uint32_t codepoint = UTF8GetNext(_buffer.data() + textInput->SelectionStart, nullptr);
                        UTF8WriteCodepoint(tmp, codepoint);
                        textWidth = std::max(GfxGetStringWidthNoFormatting(tmp, FontStyle::Medium) - 2, 4);
                    }

                    if (_cursorBlink > 15)
                    {
                        uint8_t colour = ColourMapA[colours[1].colour].mid_light;
                        // TODO: palette index addition
                        GfxFillRect(
                            dpi, { { cursorX, screenCoords.y + 9 }, { cursorX + textWidth, screenCoords.y + 9 } }, colour + 5);
                    }

                    cur_drawn++;
                }

                wrapPointer += string_length + 1;

                if (_buffer[char_count + string_length] == ' ')
                    char_count++;
                char_count += string_length;

                screenCoords.y += 10;
            }

            if (!cur_drawn)
            {
                cursorX = dpi.lastStringPos.x;
                cursorY = screenCoords.y - 10;
            }

            // IME composition
            if (!String::isNullOrEmpty(textInput->ImeBuffer))
            {
                IMEComposition(cursorX, cursorY);
            }
        }

        void OnReturnPressed()
        {
            ContextStopTextInput();
            ExecuteCallback(true);
            WindowClose(*this);
        }

        static int32_t CalculateWindowHeight(std::string_view text)
        {
            // String length needs to add 12 either side of box +13 for cursor when max length.
            int32_t numLines{};
            GfxWrapString(text, WW - (24 + 13), FontStyle::Medium, nullptr, &numLines);
            return numLines * 10 + WH;
        }

        void OnResize() override
        {
            ResizeFrame();
        }

    private:
        static void IMEComposition(int32_t cursorX, int32_t cursorY)
        {
            SDL_Rect rect = { cursorX, cursorY, 100, 100 };
            SDL_SetTextInputRect(&rect);
        }

        void ExecuteCallback(bool hasValue)
        {
            if (HasParentWindow())
            {
                if (hasValue)
                {
                    auto w = GetParentWindow();
                    if (w != nullptr)
                    {
                        w->OnTextInput(_parentWidget.widget_index, _buffer);
                    }
                }
            }
            else
            {
                if (hasValue)
                {
                    if (_callback)
                    {
                        _callback(_buffer.data());
                    }
                }
                else
                {
                    if (_cancelCallback)
                    {
                        _cancelCallback();
                    }
                }
            }
        }

        bool HasParentWindow() const
        {
            return _parentWidget.window.classification != WindowClass::Null;
        }

        WindowBase* GetParentWindow() const
        {
            auto* windowMgr = GetContext()->GetUiContext()->GetWindowManager();
            return HasParentWindow() ? windowMgr->FindByNumber(_parentWidget.window.classification, _parentWidget.window.number)
                                     : nullptr;
        }
    };

    void WindowTextInputRawOpen(
        WindowBase* call_w, WidgetIndex call_widget, StringId title, StringId description, const Formatter& descriptionArgs,
        const_utf8string existing_text, int32_t maxLength)
    {
        WindowCloseByClass(WindowClass::Textinput);

        auto height = TextInputWindow::CalculateWindowHeight(existing_text);
        auto w = WindowCreate<TextInputWindow>(WindowClass::Textinput, WW, height, WF_CENTRE_SCREEN | WF_STICK_TO_FRONT);
        if (w != nullptr)
        {
            w->SetParentWindow(call_w, call_widget);
            w->SetTitle(title, description, descriptionArgs);
            w->SetText(existing_text, maxLength);
        }
    }

    void WindowTextInputOpen(
        std::string_view title, std::string_view description, std::string_view initialValue, size_t maxLength,
        std::function<void(std::string_view)> callback, std::function<void()> cancelCallback)
    {
        auto height = TextInputWindow::CalculateWindowHeight(initialValue);
        auto w = WindowCreate<TextInputWindow>(WindowClass::Textinput, WW, height, WF_CENTRE_SCREEN | WF_STICK_TO_FRONT);
        if (w != nullptr)
        {
            w->SetTitle(title, description);
            w->SetText(initialValue, maxLength);
            w->SetCallback(callback, cancelCallback);
        }
    }

    void WindowTextInputOpen(
        WindowBase* call_w, WidgetIndex call_widget, StringId title, StringId description, const Formatter& descriptionArgs,
        StringId existing_text, uintptr_t existing_args, int32_t maxLength)
    {
        auto existingText = FormatStringIDLegacy(existing_text, &existing_args);
        WindowTextInputRawOpen(call_w, call_widget, title, description, descriptionArgs, existingText.c_str(), maxLength);
    }

    void WindowTextInputKey(WindowBase* w, uint32_t keycode)
    {
        const auto wndNumber = w->number;
        const auto wndClass = w->classification;

        // If the return button is pressed stop text input
        if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER)
        {
            if (w->classification == WindowClass::Textinput)
            {
                auto textInputWindow = static_cast<TextInputWindow*>(w);
                textInputWindow->OnReturnPressed();
            }
        }

        // The window can be potentially closed within a callback, we need to check if its still alive.
        auto* windowMgr = GetContext()->GetUiContext()->GetWindowManager();
        w = windowMgr->FindByNumber(wndClass, wndNumber);
        if (w != nullptr)
            w->Invalidate();
    }
} // namespace OpenRCT2::Ui::Windows
