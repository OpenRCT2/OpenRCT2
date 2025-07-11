/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "../interface/Theme.h"

    #include <openrct2-ui/interface/Widget.h>
    #include <openrct2-ui/windows/Windows.h>
    #include <openrct2/Context.h>
    #include <openrct2/ParkImporter.h>
    #include <openrct2/config/Config.h>
    #include <openrct2/core/String.hpp>
    #include <openrct2/interface/Chat.h>
    #include <openrct2/network/Network.h>
    #include <openrct2/ui/WindowManager.h>
    #include <openrct2/windows/Intent.h>

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_PORT_INPUT,
        WIDX_NAME_INPUT,
        WIDX_DESCRIPTION_INPUT,
        WIDX_GREETING_INPUT,
        WIDX_PASSWORD_INPUT,
        WIDX_MAXPLAYERS,
        WIDX_MAXPLAYERS_INCREASE,
        WIDX_MAXPLAYERS_DECREASE,
        WIDX_ADVERTISE_CHECKBOX,
        WIDX_START_SERVER,
        WIDX_LOAD_SERVER
    };

    static constexpr ScreenSize kWindowSize = { 300, 154 };

    // clang-format off
    static constexpr auto _windowServerStartWidgets = makeWidgets(
        makeWindowShim(STR_START_SERVER, kWindowSize),
        makeWidget({ 120, 20 }, { 173, 13 }, WidgetType::textBox, WindowColour::secondary), // port text box
        makeWidget({ 120, 36 }, { 173, 13 }, WidgetType::textBox, WindowColour::secondary), // name text box
        makeWidget({ 120, 52 }, { 173, 13 }, WidgetType::textBox, WindowColour::secondary), // description text box
        makeWidget({ 120, 68 }, { 173, 13 }, WidgetType::textBox, WindowColour::secondary), // greeting text box
        makeWidget({ 120, 84 }, { 173, 13 }, WidgetType::textBox, WindowColour::secondary), // password text box
        makeSpinnerWidgets({ 120, 100 }, { 173, 12 }, WidgetType::spinner, WindowColour::secondary,STR_SERVER_MAX_PLAYERS_VALUE), // max players (3 widgets)
        makeWidget({ 6, 117 }, { 287, 14 }, WidgetType::checkbox, WindowColour::secondary, STR_ADVERTISE,STR_ADVERTISE_SERVER_TIP), // advertise checkbox
        makeWidget({ 6, kWindowSize.height - 6 - 13 }, { 101, 14 }, WidgetType::button, WindowColour::secondary,STR_NEW_GAME), // start server button
        makeWidget({ 112, kWindowSize.height - 6 - 13 }, { 101, 14 }, WidgetType::button, WindowColour::secondary, STR_LOAD_GAME) // None
    );
    // clang-format on

    class ServerStartWindow final : public Window
    {
    public:
        void OnOpen() override
        {
            SetWidgets(_windowServerStartWidgets);
            widgets[WIDX_PORT_INPUT].string = _port;
            widgets[WIDX_NAME_INPUT].string = _name;
            widgets[WIDX_DESCRIPTION_INPUT].string = _description;
            widgets[WIDX_GREETING_INPUT].string = _greeting;
            widgets[WIDX_PASSWORD_INPUT].string = _password;

            InitScrollWidgets();
            WindowSetResize(*this, { width, height }, { width, height });

            frame_no = 0;
            page = 0;
            list_information_type = 0;

            snprintf(_port, 7, "%u", Config::Get().network.DefaultPort);
            String::safeUtf8Copy(_name, Config::Get().network.ServerName.c_str(), sizeof(_name));
            String::safeUtf8Copy(_description, Config::Get().network.ServerDescription.c_str(), sizeof(_description));
            String::safeUtf8Copy(_greeting, Config::Get().network.ServerGreeting.c_str(), sizeof(_greeting));
        }
        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_PORT_INPUT:
                    WindowStartTextbox(*this, widgetIndex, _port, 6);
                    break;
                case WIDX_NAME_INPUT:
                    WindowStartTextbox(*this, widgetIndex, _name, 64);
                    break;
                case WIDX_DESCRIPTION_INPUT:
                    WindowStartTextbox(*this, widgetIndex, _description, kMaxServerDescriptionLength);
                    break;
                case WIDX_GREETING_INPUT:
                    WindowStartTextbox(*this, widgetIndex, _greeting, kChatInputSize);
                    break;
                case WIDX_PASSWORD_INPUT:
                    WindowStartTextbox(*this, widgetIndex, _password, 32);
                    break;
                case WIDX_MAXPLAYERS_INCREASE:
                    if (Config::Get().network.Maxplayers < 255)
                    {
                        Config::Get().network.Maxplayers++;
                    }
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_MAXPLAYERS_DECREASE:
                    if (Config::Get().network.Maxplayers > 1)
                    {
                        Config::Get().network.Maxplayers--;
                    }
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_ADVERTISE_CHECKBOX:
                    Config::Get().network.Advertise = !Config::Get().network.Advertise;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_START_SERVER:
                    NetworkSetPassword(_password);
                    ScenarioselectOpen(ScenarioSelectCallback);
                    break;
                case WIDX_LOAD_SERVER:
                    NetworkSetPassword(_password);
                    auto intent = Intent(WindowClass::Loadsave);
                    intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::load);
                    intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::park);
                    intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(LoadSaveCallback));
                    ContextOpenIntent(&intent);
                    break;
            }
        }
        void OnPrepareDraw() override
        {
            ColourSchemeUpdateByClass(this, WindowClass::ServerList);

            widgetSetCheckboxValue(*this, WIDX_ADVERTISE_CHECKBOX, Config::Get().network.Advertise);
            auto ft = Formatter::Common();
            ft.Increment(18);
            ft.Add<uint16_t>(Config::Get().network.Maxplayers);
        }
        void OnUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                InvalidateWidget(WIDX_NAME_INPUT);
                InvalidateWidget(WIDX_DESCRIPTION_INPUT);
                InvalidateWidget(WIDX_GREETING_INPUT);
                InvalidateWidget(WIDX_PASSWORD_INPUT);
            }
        }
        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            std::string temp = static_cast<std::string>(text);
            int tempPort = 0;

            switch (widgetIndex)
            {
                case WIDX_PORT_INPUT:
                    if (strcmp(_port, temp.c_str()) == 0)
                        return;

                    String::safeUtf8Copy(_port, temp.c_str(), sizeof(_port));

                    // Don't allow negative/zero for port number
                    tempPort = atoi(_port);
                    if (tempPort > 0)
                    {
                        Config::Get().network.DefaultPort = tempPort;
                        Config::Save();
                    }

                    InvalidateWidget(WIDX_PORT_INPUT);
                    break;
                case WIDX_NAME_INPUT:
                    if (strcmp(_name, temp.c_str()) == 0)
                        return;

                    String::safeUtf8Copy(_name, temp.c_str(), sizeof(_name));

                    // Don't allow empty server names
                    if (_name[0] != '\0')
                    {
                        Config::Get().network.ServerName = _name;
                        Config::Save();
                    }

                    InvalidateWidget(WIDX_NAME_INPUT);
                    break;
                case WIDX_DESCRIPTION_INPUT:
                    if (strcmp(_description, temp.c_str()) == 0)
                        return;

                    String::safeUtf8Copy(_description, temp.c_str(), sizeof(_description));
                    Config::Get().network.ServerDescription = _description;
                    Config::Save();

                    InvalidateWidget(WIDX_DESCRIPTION_INPUT);
                    break;
                case WIDX_GREETING_INPUT:
                    if (strcmp(_greeting, temp.c_str()) == 0)
                        return;

                    String::safeUtf8Copy(_greeting, temp.c_str(), sizeof(_greeting));
                    Config::Get().network.ServerGreeting = _greeting;
                    Config::Save();

                    InvalidateWidget(WIDX_GREETING_INPUT);
                    break;
                case WIDX_PASSWORD_INPUT:
                    if (strcmp(_password, temp.c_str()) == 0)
                        return;

                    String::safeUtf8Copy(_password, temp.c_str(), sizeof(_password));

                    InvalidateWidget(WIDX_PASSWORD_INPUT);
                    break;
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            DrawWidgets(rt);
            DrawTextBasic(rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_PORT_INPUT].top }, STR_PORT, {}, { colours[1] });
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_NAME_INPUT].top }, STR_SERVER_NAME, {}, { colours[1] });
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_DESCRIPTION_INPUT].top }, STR_SERVER_DESCRIPTION, {},
                { colours[1] });
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_GREETING_INPUT].top }, STR_SERVER_GREETING, {}, { colours[1] });
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_PASSWORD_INPUT].top }, STR_PASSWORD, {}, { colours[1] });
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_MAXPLAYERS].top }, STR_MAX_PLAYERS, {}, { colours[1] });
        }

    private:
        char _port[7];
        char _name[65];
        char _description[kMaxServerDescriptionLength];
        char _greeting[kChatInputSize];
        char _password[33];
        static void ScenarioSelectCallback(const utf8* path)
        {
            GameNotifyMapChange();
            if (GetContext()->LoadParkFromFile(path, false, true))
            {
                NetworkBeginServer(Config::Get().network.DefaultPort, Config::Get().network.ListenAddress);
            }
        }

        static void LoadSaveCallback(ModalResult result, const utf8* path)
        {
            if (result == ModalResult::ok)
            {
                GameNotifyMapChange();
                GetContext()->LoadParkFromFile(path);
                NetworkBeginServer(Config::Get().network.DefaultPort, Config::Get().network.ListenAddress);
            }
        }
    };

    WindowBase* ServerStartOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<ServerStartWindow>(WindowClass::ServerStart, kWindowSize, WF_CENTRE_SCREEN);
    }
} // namespace OpenRCT2::Ui::Windows

#endif
