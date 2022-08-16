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
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/util/Util.h>

// clang-format off
static char _password[33];

enum WindowNetworkStatusWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PASSWORD
};

static rct_widget window_network_status_widgets[] = {
    MakeWidget({  0, 0}, {441, 91}, WindowWidgetType::Frame,    WindowColour::Primary                                   ), // panel / background
    MakeWidget({  1, 1}, {438, 14}, WindowWidgetType::Caption,  WindowColour::Primary, STR_NONE,    STR_WINDOW_TITLE_TIP), // title bar
    MakeWidget({427, 2}, { 11, 12}, WindowWidgetType::CloseBox, WindowColour::Primary, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP), // close x button
    WIDGETS_END,
};

static char window_network_status_text[1024];

static void WindowNetworkStatusOnclose(rct_window *w);
static void WindowNetworkStatusMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowNetworkStatusUpdate(rct_window *w);
static void WindowNetworkStatusTextinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void WindowNetworkStatusInvalidate(rct_window *w);
static void WindowNetworkStatusPaint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_network_status_events([](auto& events)
{
    events.close = &WindowNetworkStatusOnclose;
    events.mouse_up = &WindowNetworkStatusMouseup;
    events.update = &WindowNetworkStatusUpdate;
    events.text_input = &WindowNetworkStatusTextinput;
    events.invalidate = &WindowNetworkStatusInvalidate;
    events.paint = &WindowNetworkStatusPaint;
});
// clang-format on

static close_callback _onClose = nullptr;

rct_window* WindowNetworkStatusOpen(const char* text, close_callback onClose)
{
    _onClose = onClose;
    safe_strcpy(window_network_status_text, text, sizeof(window_network_status_text));

    // Check if window is already open
    rct_window* window = window_bring_to_front_by_class_with_flags(WindowClass::NetworkStatus, 0);
    if (window != nullptr)
        return window;

    window = WindowCreateCentred(420, 90, &window_network_status_events, WindowClass::NetworkStatus, WF_10 | WF_TRANSPARENT);

    window->widgets = window_network_status_widgets;
    WindowInitScrollWidgets(*window);
    window->no_list_items = 0;
    window->selected_list_item = -1;
    window->frame_no = 0;
    window->min_width = 320;
    window->min_height = 90;
    window->max_width = window->min_width;
    window->max_height = window->min_height;

    window->page = 0;
    window->list_information_type = 0;

    return window;
}

void WindowNetworkStatusClose()
{
    _onClose = nullptr;
    window_close_by_class(WindowClass::NetworkStatus);
}

rct_window* WindowNetworkStatusOpenPassword()
{
    rct_window* window;
    window = window_bring_to_front_by_class(WindowClass::NetworkStatus);
    if (window == nullptr)
        return nullptr;

    WindowTextInputRawOpen(window, WIDX_PASSWORD, STR_PASSWORD_REQUIRED, STR_PASSWORD_REQUIRED_DESC, {}, _password, 32);

    return window;
}

static void WindowNetworkStatusOnclose(rct_window* w)
{
    if (_onClose != nullptr)
    {
        _onClose();
    }
}

static void WindowNetworkStatusMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(*w);
            break;
    }
}

static void WindowNetworkStatusUpdate(rct_window* w)
{
    widget_invalidate(*w, WIDX_BACKGROUND);
}

static void WindowNetworkStatusTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    _password[0] = '\0';
    switch (widgetIndex)
    {
        case WIDX_PASSWORD:
            if (text != nullptr)
                safe_strcpy(_password, text, sizeof(_password));
            break;
    }
    if (text == nullptr)
    {
        network_shutdown_client();
    }
    else
    {
        network_send_password(_password);
    }
}

static void WindowNetworkStatusInvalidate(rct_window* w)
{
    window_network_status_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_network_status_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    window_network_status_widgets[WIDX_TITLE].right = w->width - 2;
    window_network_status_widgets[WIDX_CLOSE].left = w->width - 2 - 0x0B;
    window_network_status_widgets[WIDX_CLOSE].right = w->width - 2 - 0x0B + 0x0A;
}

static void WindowNetworkStatusPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);

    thread_local std::string _buffer;
    _buffer.assign("{BLACK}");
    _buffer += window_network_status_text;
    gfx_clip_string(_buffer.data(), w->widgets[WIDX_BACKGROUND].right - 50, FontSpriteBase::MEDIUM);
    ScreenCoordsXY screenCoords(w->windowPos.x + (w->width / 2), w->windowPos.y + (w->height / 2));
    screenCoords.x -= gfx_get_string_width(_buffer, FontSpriteBase::MEDIUM) / 2;
    gfx_draw_string(dpi, screenCoords, _buffer.c_str());
}
