/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

/**
 * Text Input Window
 *
 * This is a new window created to replace the windows dialog box
 * that is used for inputing new text for ride names and peep names.
 */

#include <algorithm>
#include <iterator>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/util/Util.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_OPTIONS;
static constexpr const int32_t WW = 250;
static constexpr const int32_t WH = 90;

// clang-format off
enum WINDOW_TEXT_INPUT_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_CANCEL,
    WIDX_OKAY
};

// 0x9DE4E0
static rct_widget window_text_input_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({170, 68}, {71, 14}, WWT_BUTTON, WindowColour::Secondary, STR_CANCEL),
    MakeWidget({ 10, 68}, {71, 14}, WWT_BUTTON, WindowColour::Secondary, STR_OK    ),
    { WIDGETS_END }
};

static void window_text_input_close(rct_window *w);
static void window_text_input_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_text_input_periodic_update(rct_window *w);
static void window_text_input_invalidate(rct_window *w);
static void window_text_input_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void draw_ime_composition(rct_drawpixelinfo * dpi, int cursorX, int cursorY);

//0x9A3F7C
static rct_window_event_list window_text_input_events = {
    window_text_input_close,
    window_text_input_mouseup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_text_input_periodic_update,
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
    window_text_input_invalidate,
    window_text_input_paint,
    nullptr
};
// clang-format on

static rct_string_id input_text_description;
static utf8 text_input[TEXT_INPUT_SIZE] = { 0 };
static rct_windowclass calling_class = 0;
static rct_windownumber calling_number = 0;
static int32_t calling_widget = 0;
static int32_t _maxInputLength;

static std::string _title;
static std::string _description;
static std::function<void(const std::string_view&)> _callback;
static std::function<void()> _cancelCallback;

void window_text_input_open(
    rct_window* call_w, rct_widgetindex call_widget, rct_string_id title, rct_string_id description,
    rct_string_id existing_text, uintptr_t existing_args, int32_t maxLength)
{
    // Get the raw string
    utf8 buffer[std::size(text_input)]{};
    if (existing_text != STR_NONE)
        format_string(buffer, maxLength, existing_text, &existing_args);

    utf8_remove_format_codes(buffer, false);
    window_text_input_raw_open(call_w, call_widget, title, description, buffer, maxLength);
}

void window_text_input_raw_open(
    rct_window* call_w, rct_widgetindex call_widget, rct_string_id title, rct_string_id description,
    const_utf8string existing_text, int32_t maxLength)
{
    _maxInputLength = maxLength;

    window_close_by_class(WC_TEXTINPUT);

    // Set the input text
    if (existing_text != nullptr)
        String::Set(text_input, sizeof(text_input), existing_text);
    else
        String::Set(text_input, sizeof(text_input), "");

    // This is the text displayed above the input box
    input_text_description = description;

    // Work out the existing size of the window
    char wrapped_string[TEXT_INPUT_SIZE];
    safe_strcpy(wrapped_string, text_input, TEXT_INPUT_SIZE);

    int32_t no_lines = 0, font_height = 0;

    // String length needs to add 12 either side of box +13 for cursor when max length.
    gfx_wrap_string(wrapped_string, WW - (24 + 13), &no_lines, &font_height);

    int32_t height = no_lines * 10 + WH;

    // Window will be in the centre of the screen
    rct_window* w = window_create_centred(WW, height, &window_text_input_events, WC_TEXTINPUT, WF_STICK_TO_FRONT);

    w->widgets = window_text_input_widgets;
    w->enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_CANCEL) | (1ULL << WIDX_OKAY);

    window_text_input_widgets[WIDX_TITLE].text = title;

    // Save calling window details so that the information can be passed back to the correct window & widget
    if (call_w == nullptr)
    {
        calling_class = WC_NULL;
        calling_number = 0;
        calling_widget = 0;
    }
    else
    {
        calling_class = call_w->classification;
        calling_number = call_w->number;
        calling_widget = call_widget;
    }

    gTextInput = context_start_text_input(text_input, maxLength);

    window_init_scroll_widgets(w);

    if (call_w == nullptr)
    {
        w->colours[0] = COLOUR_GREY;
        w->colours[1] = COLOUR_GREY;
        w->colours[2] = COLOUR_GREY;
    }
    else
    {
        w->colours[0] = call_w->colours[1];
        w->colours[1] = call_w->colours[1];
        w->colours[2] = call_w->colours[1];
    }
}

void window_text_input_open(
    const std::string_view& title, const std::string_view& description, const std::string_view& initialValue, size_t maxLength,
    std::function<void(const std::string_view&)> callback, std::function<void()> cancelCallback)
{
    _title = title;
    _description = description;
    _callback = callback;
    _cancelCallback = cancelCallback;

    std::string szInitialValue(initialValue);
    auto szDescription = _description.c_str();
    std::memcpy(TextInputDescriptionArgs, &szDescription, sizeof(const char*));
    maxLength = std::min(sizeof(text_input) - 1, maxLength);
    window_text_input_raw_open(nullptr, 0, STR_STRING, STR_STRING, szInitialValue.c_str(), static_cast<int32_t>(maxLength));
}

static void window_text_input_execute_callback(bool hasValue)
{
    if (calling_class == WC_NULL)
    {
        if (hasValue)
        {
            if (_callback)
            {
                _callback(text_input);
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
    else
    {
        auto calling_w = window_find_by_number(calling_class, calling_number);
        if (calling_w != nullptr)
        {
            auto value = hasValue ? text_input : nullptr;
            window_event_textinput_call(calling_w, calling_widget, value);
        }
    }
}

/**
 *
 */
static void window_text_input_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CANCEL:
        case WIDX_CLOSE:
            context_stop_text_input();
            window_text_input_execute_callback(false);
            window_close(w);
            break;
        case WIDX_OKAY:
            context_stop_text_input();
            window_text_input_execute_callback(true);
            window_close(w);
    }
}

/**
 *
 */
static void window_text_input_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);

    ScreenCoordsXY screenCoords;
    screenCoords.y = w->windowPos.y + 25;

    int32_t no_lines = 0;
    int32_t font_height = 0;

    gfx_draw_string_centred(
        dpi, input_text_description, { w->windowPos.x + WW / 2, screenCoords.y }, w->colours[1], &TextInputDescriptionArgs);

    screenCoords.y += 25;

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    gCurrentFontFlags = 0;

    char wrapped_string[TEXT_INPUT_SIZE];
    safe_strcpy(wrapped_string, text_input, TEXT_INPUT_SIZE);

    // String length needs to add 12 either side of box
    // +13 for cursor when max length.
    gfx_wrap_string(wrapped_string, WW - (24 + 13), &no_lines, &font_height);

    gfx_fill_rect_inset(
        dpi, w->windowPos.x + 10, screenCoords.y, w->windowPos.x + WW - 10, screenCoords.y + 10 * (no_lines + 1) + 3,
        w->colours[1], INSET_RECT_F_60);

    screenCoords.y += 1;

    char* wrap_pointer = wrapped_string;
    size_t char_count = 0;
    uint8_t cur_drawn = 0;

    int32_t cursorX = 0;
    int32_t cursorY = 0;
    for (int32_t line = 0; line <= no_lines; line++)
    {
        screenCoords.x = w->windowPos.x + 12;
        gfx_draw_string(dpi, wrap_pointer, w->colours[1], screenCoords);

        size_t string_length = get_string_size(wrap_pointer) - 1;

        if (!cur_drawn && (gTextInput->SelectionStart <= char_count + string_length))
        {
            // Make a copy of the string for measuring the width.
            char temp_string[TEXT_INPUT_SIZE] = { 0 };
            std::memcpy(temp_string, wrap_pointer, gTextInput->SelectionStart - char_count);
            cursorX = w->windowPos.x + 13 + gfx_get_string_width(temp_string);
            cursorY = screenCoords.y;

            int32_t width = 6;
            if (gTextInput->SelectionStart < strlen(text_input))
            {
                // Make a 1 utf8-character wide string for measuring the width
                // of the currently selected character.
                utf8 tmp[5] = { 0 }; // This is easier than setting temp_string[0..5]
                uint32_t codepoint = utf8_get_next(text_input + gTextInput->SelectionStart, nullptr);
                utf8_write_codepoint(tmp, codepoint);
                width = std::max(gfx_get_string_width(tmp) - 2, 4);
            }

            if (w->frame_no > 15)
            {
                uint8_t colour = ColourMapA[w->colours[1]].mid_light;
                // TODO: palette index addition
                gfx_fill_rect(dpi, { { cursorX, screenCoords.y + 9 }, { cursorX + width, screenCoords.y + 9 } }, colour + 5);
            }

            cur_drawn++;
        }

        wrap_pointer += string_length + 1;

        if (text_input[char_count + string_length] == ' ')
            char_count++;
        char_count += string_length;

        screenCoords.y += 10;
    }

    if (!cur_drawn)
    {
        cursorX = gLastDrawStringX;
        cursorY = screenCoords.y - 10;
    }

    // IME composition
    if (!str_is_null_or_empty(gTextInput->ImeBuffer))
    {
        draw_ime_composition(dpi, cursorX, cursorY);
    }
}

void window_text_input_key(rct_window* w, char keychar)
{
    // If the return button is pressed stop text input
    if (keychar == '\r')
    {
        context_stop_text_input();
        window_text_input_execute_callback(true);
        window_close(w);
    }
    w->Invalidate();
}

void window_text_input_periodic_update(rct_window* w)
{
    if (calling_class != WC_NULL)
    {
        auto calling_w = window_find_by_number(calling_class, calling_number);
        // If the calling window is closed then close the text
        // input window.
        if (!calling_w)
        {
            window_close(w);
            return;
        }
    }

    // Used to blink the cursor.
    w->frame_no++;
    if (w->frame_no > 30)
        w->frame_no = 0;

    w->Invalidate();
}

static void window_text_input_close(rct_window* w)
{
    // Make sure that we take it out of the text input
    // mode otherwise problems may occur.
    context_stop_text_input();

    _title = {};
    _description = {};
    _callback = {};
    _cancelCallback = {};
}

static void window_text_input_invalidate(rct_window* w)
{
    // Work out the existing size of the window
    char wrapped_string[TEXT_INPUT_SIZE];
    safe_strcpy(wrapped_string, text_input, TEXT_INPUT_SIZE);

    int32_t no_lines = 0, font_height = 0;

    // String length needs to add 12 either side of box
    // +13 for cursor when max length.
    gfx_wrap_string(wrapped_string, WW - (24 + 13), &no_lines, &font_height);

    int32_t height = no_lines * 10 + WH;

    // Change window size if required.
    if (height != w->height)
    {
        w->Invalidate();
        window_set_resize(w, WW, height, WW, height);
    }

    window_text_input_widgets[WIDX_OKAY].top = height - 22;
    window_text_input_widgets[WIDX_OKAY].bottom = height - 9;

    window_text_input_widgets[WIDX_CANCEL].top = height - 22;
    window_text_input_widgets[WIDX_CANCEL].bottom = height - 9;

    window_text_input_widgets[WIDX_BACKGROUND].bottom = height - 1;

    // Set window title argument
    auto ft = Formatter::Common();
    ft.Add<const char*>(_title.c_str());
}

static void draw_ime_composition(rct_drawpixelinfo* dpi, int cursorX, int cursorY)
{
    int compositionWidth = gfx_get_string_width(gTextInput->ImeBuffer);
    ScreenCoordsXY screenCoords(cursorX - (compositionWidth / 2), cursorY + 13);
    int width = compositionWidth;
    int height = 10;

    gfx_fill_rect(
        dpi, { screenCoords - ScreenCoordsXY{ 1, 1 }, screenCoords + ScreenCoordsXY{ width + 1, height + 1 } },
        PALETTE_INDEX_12);
    gfx_fill_rect(dpi, { screenCoords, screenCoords + ScreenCoordsXY{ width, height } }, PALETTE_INDEX_0);
    gfx_draw_string(dpi, static_cast<const char*>(gTextInput->ImeBuffer), COLOUR_DARK_GREEN, screenCoords);
}
