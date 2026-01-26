/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
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
        void onOpen() override
        {
            WindowSetResize(*this, kWindowSize, kWindowSize);
            setWidgets(window_network_status_widgets);

            currentFrame = 0;
            page = 0;
            listInformationType = 0;
        }

        void onClose() override
        {
            if (_onClose != nullptr)
            {
                _onClose();
            }
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
            }
        }

        void onUpdate() override
        {
            invalidateWidget(WIDX_BACKGROUND);
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
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
                Network::ShutdownClient();
            }
            else
            {
                Network::SendPassword(_password);
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            WindowDrawWidgets(*this, rt);
            thread_local std::string _buffer;

            _buffer.assign("{WHITE}");
            _buffer += _windowNetworkStatusText;
            GfxClipString(_buffer.data(), widgets[WIDX_BACKGROUND].right - 50, FontStyle::medium);

            ScreenCoordsXY screenCoords(windowPos.x + (width / 2), windowPos.y + (height / 2));
            screenCoords.x -= GfxGetStringWidth(_buffer, FontStyle::medium) / 2;
            DrawText(rt, screenCoords, { Drawing::Colour::black }, _buffer.c_str());
        }

        void setCloseCallBack(CloseCallback callback)
        {
            _onClose = callback;
        }

        void setWindowNetworkStatusText(const std::string& text)
        {
            _windowNetworkStatusText = text;
            invalidate();
        }

        void setPassword(char* password)
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
        ContextForceCloseWindowByClass(WindowClass::progressWindow);

        auto* windowMgr = GetWindowManager();

        NetworkStatusWindow* window;
        if ((window = static_cast<NetworkStatusWindow*>(windowMgr->FindByClass(WindowClass::networkStatus))) != nullptr)
        {
            windowMgr->BringToFront(*window);
        }
        else
        {
            window = windowMgr->Create<NetworkStatusWindow>(
                WindowClass::networkStatus, kWindowSize,
                { WindowFlag::higherContrastOnPress, WindowFlag::transparent, WindowFlag::centreScreen,
                  WindowFlag::stickToFront });
        }

        window->setCloseCallBack(onClose);
        window->setWindowNetworkStatusText(text);
        return window;
    }

    // force close
    void WindowNetworkStatusClose()
    {
        auto* windowMgr = GetWindowManager();
        auto window = windowMgr->FindByClass(WindowClass::networkStatus);
        if (window == nullptr)
        {
            return;
        }
        auto networkWindow = static_cast<NetworkStatusWindow*>(window);
        networkWindow->setCloseCallBack(nullptr);
        networkWindow->close();
    }

    WindowBase* NetworkStatusOpenPassword()
    {
        ContextForceCloseWindowByClass(WindowClass::progressWindow);

        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->FocusOrCreate<NetworkStatusWindow>(
            WindowClass::networkStatus, kWindowSize,
            { WindowFlag::higherContrastOnPress, WindowFlag::transparent, WindowFlag::centreScreen });

        char password[33]{};
        WindowTextInputRawOpen(window, WIDX_PASSWORD, STR_PASSWORD_REQUIRED, STR_PASSWORD_REQUIRED_DESC, {}, password, 32);
        window->setPassword(password);
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
