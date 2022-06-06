/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <iterator>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/util/Util.h>

static constexpr const int32_t WW = 250;
static constexpr const int32_t WH = 90;

enum WindowTextInputWidgetIdx
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_CANCEL,
    WIDX_OKAY
};

static rct_widget window_text_input_widgets[] = {
    WINDOW_SHIM(STR_NONE, WW, WH),
    MakeWidget({ 170, 68 }, { 71, 14 }, WindowWidgetType::Button, WindowColour::Secondary, STR_CANCEL),
    MakeWidget({ 10, 68 }, { 71, 14 }, WindowWidgetType::Button, WindowColour::Secondary, STR_OK),
    WIDGETS_END,
};

class TextInputWindow final : public Window
{
private:
    widget_identifier _parentWidget{};

    std::string _title;
    rct_string_id _titleStringId = STR_NONE;

    std::string _description;
    rct_string_id _descriptionStringId = STR_NONE;
    Formatter _descriptionArgs;

    std::function<void(std::string_view)> _callback;
    std::function<void()> _cancelCallback;

    int32_t _cursorBlink{};
    size_t _maxInputLength{};
    std::vector<utf8> _buffer;

public:
    void OnOpen() override
    {
        widgets = window_text_input_widgets;
        WindowInitScrollWidgets(this);
        SetParentWindow(nullptr, 0);
    }

    void SetParentWindow(rct_window* parentWindow, rct_widgetindex widgetIndex)
    {
        // Save calling window details so that the information can be passed back to the correct window & widget
        if (parentWindow == nullptr)
        {
            _parentWidget.window.classification = WC_NULL;
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

    void SetTitle(rct_string_id title, rct_string_id description, const Formatter& decriptionArgs)
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
        _buffer.resize(maxLength);
        safe_strcpy(_buffer.data(), std::string(text).c_str(), maxLength);
        _maxInputLength = maxLength;
        gTextInput = context_start_text_input(_buffer.data(), maxLength);
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
        context_stop_text_input();
    }

    void OnPeriodicUpdate() override
    {
        if (HasParentWindow())
        {
            // If the calling window is closed then close the text input window
            auto parentWindow = GetParentWindow();
            if (parentWindow == nullptr)
            {
                window_close(this);
                return;
            }
        }

        // Used to blink the cursor.
        _cursorBlink++;
        if (_cursorBlink > 30)
            _cursorBlink = 0;

        Invalidate();
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CANCEL:
            case WIDX_CLOSE:
                context_stop_text_input();
                ExecuteCallback(false);
                window_close(this);
                break;
            case WIDX_OKAY:
                context_stop_text_input();
                ExecuteCallback(true);
                window_close(this);
        }
    }

    void OnPrepareDraw() override
    {
        // Change window size if required.
        int32_t newHeight = CalculateWindowHeight(_buffer.data());
        if (newHeight != height)
        {
            window_set_resize(this, WW, newHeight, WW, newHeight);
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

    void OnDraw(rct_drawpixelinfo& dpi) override
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
                &dpi, { windowPos.x + WW / 2, screenCoords.y }, WW, STR_STRING, ft, { colours[1], TextAlignment::CENTRE });
        }
        else
        {
            DrawTextWrapped(
                &dpi, { windowPos.x + WW / 2, screenCoords.y }, WW, _descriptionStringId, _descriptionArgs,
                { colours[1], TextAlignment::CENTRE });
        }

        screenCoords.y += 25;

        char wrapped_string[TEXT_INPUT_SIZE];
        safe_strcpy(wrapped_string, _buffer.data(), _buffer.size());

        // String length needs to add 12 either side of box
        // +13 for cursor when max length.
        gfx_wrap_string(wrapped_string, WW - (24 + 13), FontSpriteBase::MEDIUM, &no_lines);

        gfx_fill_rect_inset(
            &dpi, { { windowPos.x + 10, screenCoords.y }, { windowPos.x + WW - 10, screenCoords.y + 10 * (no_lines + 1) + 3 } },
            colours[1], INSET_RECT_F_60);

        screenCoords.y += 1;

        char* wrap_pointer = wrapped_string;
        size_t char_count = 0;
        uint8_t cur_drawn = 0;

        int32_t cursorX = 0;
        int32_t cursorY = 0;
        for (int32_t line = 0; line <= no_lines; line++)
        {
            screenCoords.x = windowPos.x + 12;
            gfx_draw_string_no_formatting(&dpi, screenCoords, wrap_pointer, { colours[1], FontSpriteBase::MEDIUM });

            size_t string_length = get_string_size(wrap_pointer) - 1;

            if (!cur_drawn && (gTextInput->SelectionStart <= char_count + string_length))
            {
                // Make a copy of the string for measuring the width.
                char temp_string[TEXT_INPUT_SIZE] = { 0 };
                std::memcpy(temp_string, wrap_pointer, gTextInput->SelectionStart - char_count);
                cursorX = windowPos.x + 13 + gfx_get_string_width_no_formatting(temp_string, FontSpriteBase::MEDIUM);
                cursorY = screenCoords.y;

                int32_t textWidth = 6;
                if (gTextInput->SelectionStart < strlen(_buffer.data()))
                {
                    // Make a 1 utf8-character wide string for measuring the width
                    // of the currently selected character.
                    utf8 tmp[5] = { 0 }; // This is easier than setting temp_string[0..5]
                    uint32_t codepoint = utf8_get_next(_buffer.data() + gTextInput->SelectionStart, nullptr);
                    utf8_write_codepoint(tmp, codepoint);
                    textWidth = std::max(gfx_get_string_width_no_formatting(tmp, FontSpriteBase::MEDIUM) - 2, 4);
                }

                if (_cursorBlink > 15)
                {
                    uint8_t colour = ColourMapA[colours[1]].mid_light;
                    // TODO: palette index addition
                    gfx_fill_rect(
                        &dpi, { { cursorX, screenCoords.y + 9 }, { cursorX + textWidth, screenCoords.y + 9 } }, colour + 5);
                }

                cur_drawn++;
            }

            wrap_pointer += string_length + 1;

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
        if (!str_is_null_or_empty(gTextInput->ImeBuffer))
        {
            DrawIMEComposition(dpi, cursorX, cursorY);
        }
    }

    void OnReturnPressed()
    {
        context_stop_text_input();
        ExecuteCallback(true);
        window_close(this);
    }

    static int32_t CalculateWindowHeight(std::string_view text)
    {
        std::string wrappedString(text);
        wrappedString.resize(TEXT_INPUT_SIZE);

        // String length needs to add 12 either side of box +13 for cursor when max length.
        int32_t numLines{};
        gfx_wrap_string(wrappedString.data(), WW - (24 + 13), FontSpriteBase::MEDIUM, &numLines);
        return numLines * 10 + WH;
    }

private:
    static void DrawIMEComposition(rct_drawpixelinfo& dpi, int32_t cursorX, int32_t cursorY)
    {
        int compositionWidth = gfx_get_string_width(gTextInput->ImeBuffer, FontSpriteBase::MEDIUM);
        ScreenCoordsXY screenCoords(cursorX - (compositionWidth / 2), cursorY + 13);
        int width = compositionWidth;
        int height = 10;

        gfx_fill_rect(
            &dpi, { screenCoords - ScreenCoordsXY{ 1, 1 }, screenCoords + ScreenCoordsXY{ width + 1, height + 1 } },
            PALETTE_INDEX_12);
        gfx_fill_rect(&dpi, { screenCoords, screenCoords + ScreenCoordsXY{ width, height } }, PALETTE_INDEX_0);
        gfx_draw_string(&dpi, screenCoords, static_cast<const char*>(gTextInput->ImeBuffer), { COLOUR_DARK_GREEN });
    }

    void ExecuteCallback(bool hasValue)
    {
        if (HasParentWindow())
        {
            auto w = GetParentWindow();
            if (w != nullptr)
            {
                auto value = hasValue ? _buffer.data() : nullptr;
                window_event_textinput_call(w, _parentWidget.widget_index, value);
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
        return _parentWidget.window.classification != WC_NULL;
    }

    rct_window* GetParentWindow() const
    {
        return HasParentWindow() ? window_find_by_number(_parentWidget.window.classification, _parentWidget.window.number)
                                 : nullptr;
    }
};

void WindowTextInputRawOpen(
    rct_window* call_w, rct_widgetindex call_widget, rct_string_id title, rct_string_id description,
    const Formatter& descriptionArgs, const_utf8string existing_text, int32_t maxLength)
{
    window_close_by_class(WC_TEXTINPUT);

    auto height = TextInputWindow::CalculateWindowHeight(existing_text);
    auto w = WindowCreate<TextInputWindow>(WC_TEXTINPUT, WW, height, WF_CENTRE_SCREEN | WF_STICK_TO_FRONT);
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
    auto w = WindowCreate<TextInputWindow>(WC_TEXTINPUT, WW, height, WF_CENTRE_SCREEN | WF_STICK_TO_FRONT);
    if (w != nullptr)
    {
        w->SetTitle(title, description);
        w->SetText(initialValue, maxLength);
        w->SetCallback(callback, cancelCallback);
    }
}

void WindowTextInputOpen(
    rct_window* call_w, rct_widgetindex call_widget, rct_string_id title, rct_string_id description,
    const Formatter& descriptionArgs, rct_string_id existing_text, uintptr_t existing_args, int32_t maxLength)
{
    auto existingText = format_string(existing_text, &existing_args);
    WindowTextInputRawOpen(call_w, call_widget, title, description, descriptionArgs, existingText.c_str(), maxLength);
}

void WindowTextInputKey(rct_window* w, char keychar)
{
    const auto wndNumber = w->number;
    const auto wndClass = w->classification;

    // If the return button is pressed stop text input
    if (keychar == '\r')
    {
        if (w->classification == WC_TEXTINPUT)
        {
            auto textInputWindow = static_cast<TextInputWindow*>(w);
            textInputWindow->OnReturnPressed();
        }
    }

    // The window can be potentially closed within a callback, we need to check if its still alive.
    w = window_find_by_number(wndClass, wndNumber);
    if (w != nullptr)
        w->Invalidate();
}
