/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
enum WindowNetworkStatusWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PASSWORD
};

static Widget window_network_status_widgets[] = {
    MakeWidget({  0, 0}, {441, 91}, WindowWidgetType::Frame,    WindowColour::Primary                                   ), // panel / background
    MakeWidget({  1, 1}, {438, 14}, WindowWidgetType::Caption,  WindowColour::Primary, STR_NONE,    STR_WINDOW_TITLE_TIP), // title bar
    MakeWidget({427, 2}, { 11, 12}, WindowWidgetType::CloseBox, WindowColour::Primary, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP), // close x button
    WIDGETS_END,
};

// clang-format on
class NetworkStatusWindow final : public Window
{
public:
    void OnOpen() override
    {
        widgets = window_network_status_widgets;
        WindowInitScrollWidgets(*this);

        no_list_items = 0;
        selected_list_item = -1;
        frame_no = 0;
        min_width = 320;
        min_height = 90;
        max_width = min_width;
        max_height = min_height;

        page = 0;
        list_information_type = 0;
    }

    void OnClose() override
    {
        if (_onClose != nullptr)
        {
            _onClose();
        }
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
        }
    }

    void OnUpdate() override
    {
        InvalidateWidget(WIDX_BACKGROUND);
    }

    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
    {
        _password.clear();
        switch (widgetIndex)
        {
            case WIDX_PASSWORD:
                _password = text;
                break;
        }
        if (text.empty())
        {
            network_shutdown_client();
        }
        else
        {
            network_send_password(_password);
        }
    }

    void OnPrepareDraw() override
    {
        ResizeFrame();
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        WindowDrawWidgets(*this, &dpi);
        thread_local std::string _buffer;
        _buffer.assign("{BLACK}");
        _buffer += _windowNetworkStatusText;
        GfxClipString(_buffer.data(), widgets[WIDX_BACKGROUND].right - 50, FontStyle::Medium);
        ScreenCoordsXY screenCoords(windowPos.x + (width / 2), windowPos.y + (height / 2));
        screenCoords.x -= GfxGetStringWidth(_buffer, FontStyle::Medium) / 2;
        GfxDrawString(&dpi, screenCoords, _buffer.c_str());
    }

    void SetCloseCallBack(close_callback onClose)
    {
        _onClose = onClose;
    }

    void SetWindowNetworkStatusText(const std::string& text)
    {
        _windowNetworkStatusText = text;
    }

    void SetPassword(char* password)
    {
        _password = password;
    }

private:
    close_callback _onClose = nullptr;
    std::string _windowNetworkStatusText;
    std::string _password;
};

rct_window* WindowNetworkStatusOpen(const std::string& text, close_callback onClose)
{
    auto window = WindowFocusOrCreate<NetworkStatusWindow>(
        WindowClass::NetworkStatus, 420, 90, WF_10 | WF_TRANSPARENT | WF_CENTRE_SCREEN);
    window->SetCloseCallBack(onClose);
    window->SetWindowNetworkStatusText(text);
    return window;
}

// force close
void WindowNetworkStatusClose()
{
    auto window = WindowFindByClass(WindowClass::NetworkStatus);
    if (window == nullptr)
    {
        return;
    }
    auto networkWindow = static_cast<NetworkStatusWindow*>(window);
    networkWindow->SetCloseCallBack(nullptr);
    networkWindow->Close();
}

rct_window* WindowNetworkStatusOpenPassword()
{
    auto window = WindowFocusOrCreate<NetworkStatusWindow>(
        WindowClass::NetworkStatus, 420, 90, WF_10 | WF_TRANSPARENT | WF_CENTRE_SCREEN);
    char password[33]{};
    WindowTextInputRawOpen(window, WIDX_PASSWORD, STR_PASSWORD_REQUIRED, STR_PASSWORD_REQUIRED_DESC, {}, password, 32);
    window->SetPassword(password);
    return window;
}
