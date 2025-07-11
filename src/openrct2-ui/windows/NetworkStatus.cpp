/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/network/Network.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    enum WindowNetworkStatusWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_PASSWORD
    };

    static constexpr ScreenSize kWindowSize = { 400, 90 };
    // clang-format off
    static constexpr auto window_network_status_widgets = makeWidgets(
        makeWindowShim(kStringIdEmpty, kWindowSize)
    );
    // clang-format on

    class NetworkStatusWindow final : public Window
    {
    public:
        void OnOpen() override
        {
            WindowSetResize(*this, kWindowSize, kWindowSize);
            SetWidgets(window_network_status_widgets);

            frame_no = 0;
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
                NetworkShutdownClient();
            }
            else
            {
                NetworkSendPassword(_password);
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            WindowDrawWidgets(*this, rt);
            thread_local std::string _buffer;

            _buffer.assign("{WHITE}");
            _buffer += _windowNetworkStatusText;
            GfxClipString(_buffer.data(), widgets[WIDX_BACKGROUND].right - 50, FontStyle::Medium);

            ScreenCoordsXY screenCoords(windowPos.x + (width / 2), windowPos.y + (height / 2));
            screenCoords.x -= GfxGetStringWidth(_buffer, FontStyle::Medium) / 2;
            DrawText(rt, screenCoords, { COLOUR_BLACK }, _buffer.c_str());
        }

        void SetCloseCallBack(CloseCallback onClose)
        {
            _onClose = onClose;
        }

        void SetWindowNetworkStatusText(const std::string& text)
        {
            _windowNetworkStatusText = text;
            Invalidate();
        }

        void SetPassword(char* password)
        {
            _password = password;
        }

    private:
        CloseCallback _onClose = nullptr;
        std::string _windowNetworkStatusText;
        std::string _password;
    };

    WindowBase* NetworkStatusOpen(const std::string& text, CloseCallback onClose)
    {
        ContextForceCloseWindowByClass(WindowClass::ProgressWindow);

        auto* windowMgr = GetWindowManager();

        NetworkStatusWindow* window;
        if ((window = static_cast<NetworkStatusWindow*>(windowMgr->FindByClass(WindowClass::NetworkStatus))) != nullptr)
        {
            windowMgr->BringToFront(*window);
        }
        else
        {
            window = windowMgr->Create<NetworkStatusWindow>(
                WindowClass::NetworkStatus, kWindowSize, WF_10 | WF_TRANSPARENT | WF_CENTRE_SCREEN | WF_STICK_TO_FRONT);
        }

        window->SetCloseCallBack(onClose);
        window->SetWindowNetworkStatusText(text);
        return window;
    }

    // force close
    void WindowNetworkStatusClose()
    {
        auto* windowMgr = GetWindowManager();
        auto window = windowMgr->FindByClass(WindowClass::NetworkStatus);
        if (window == nullptr)
        {
            return;
        }
        auto networkWindow = static_cast<NetworkStatusWindow*>(window);
        networkWindow->SetCloseCallBack(nullptr);
        networkWindow->Close();
    }

    WindowBase* NetworkStatusOpenPassword()
    {
        ContextForceCloseWindowByClass(WindowClass::ProgressWindow);

        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->FocusOrCreate<NetworkStatusWindow>(
            WindowClass::NetworkStatus, kWindowSize, WF_10 | WF_TRANSPARENT | WF_CENTRE_SCREEN);

        char password[33]{};
        WindowTextInputRawOpen(window, WIDX_PASSWORD, STR_PASSWORD_REQUIRED, STR_PASSWORD_REQUIRED_DESC, {}, password, 32);
        window->SetPassword(password);
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
