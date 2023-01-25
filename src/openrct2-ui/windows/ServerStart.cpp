/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#    include "../interface/Theme.h"

#    include <openrct2-ui/interface/Widget.h>
#    include <openrct2-ui/windows/Window.h>
#    include <openrct2/Context.h>
#    include <openrct2/ParkImporter.h>
#    include <openrct2/config/Config.h>
#    include <openrct2/interface/Chat.h>
#    include <openrct2/localisation/Localisation.h>
#    include <openrct2/network/network.h>
#    include <openrct2/util/Util.h>
#    include <openrct2/windows/Intent.h>

using namespace OpenRCT2;

// clang-format off
enum {
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

static constexpr const int32_t WW = 300;
static constexpr const int32_t WH = 154;

static Widget _windowServerStartWidgets[] = {
    MakeWidget({ 0, 0 }, { WW, WH }, WindowWidgetType::Frame, WindowColour::Primary), // panel / background
    MakeWidget({ 1, 1 }, { 298, 14 }, WindowWidgetType::Caption, WindowColour::Primary, STR_START_SERVER,STR_WINDOW_TITLE_TIP), // title bar
    MakeWidget({ WW - 13, 2 }, { 11, 12 }, WindowWidgetType::CloseBox, WindowColour::Primary, STR_CLOSE_X,STR_CLOSE_WINDOW_TIP), // close x button
    MakeWidget({ 120, 20 }, { 173, 13 }, WindowWidgetType::TextBox, WindowColour::Secondary), // port text box
    MakeWidget({ 120, 36 }, { 173, 13 }, WindowWidgetType::TextBox, WindowColour::Secondary), // name text box
    MakeWidget({ 120, 52 }, { 173, 13 }, WindowWidgetType::TextBox, WindowColour::Secondary), // description text box
    MakeWidget({ 120, 68 }, { 173, 13 }, WindowWidgetType::TextBox, WindowColour::Secondary), // greeting text box
    MakeWidget({ 120, 84 }, { 173, 13 }, WindowWidgetType::TextBox, WindowColour::Secondary), // password text box
    MakeSpinnerWidgets({ 120, 100 }, { 173, 12 }, WindowWidgetType::Spinner, WindowColour::Secondary,STR_SERVER_MAX_PLAYERS_VALUE), // max players (3 widgets)
    MakeWidget({ 6, 117 }, { 287, 14 }, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_ADVERTISE,STR_ADVERTISE_SERVER_TIP), // advertise checkbox
    MakeWidget({ 6, WH - 6 - 13 }, { 101, 14 }, WindowWidgetType::Button, WindowColour::Secondary,STR_NEW_GAME), // start server button
    MakeWidget({ 112, WH - 6 - 13 }, { 101, 14 }, WindowWidgetType::Button, WindowColour::Secondary, STR_LOAD_GAME), // None
    WIDGETS_END,
};
// clang-format on
class ServerStartWindow final : public Window
{
public:
    void OnOpen() override
    {
        widgets = _windowServerStartWidgets;
        widgets[WIDX_PORT_INPUT].string = _port;
        widgets[WIDX_NAME_INPUT].string = _name;
        widgets[WIDX_DESCRIPTION_INPUT].string = _description;
        widgets[WIDX_GREETING_INPUT].string = _greeting;
        widgets[WIDX_PASSWORD_INPUT].string = _password;
        InitScrollWidgets();
        no_list_items = 0;
        selected_list_item = -1;
        frame_no = 0;
        min_width = width;
        min_height = height;
        max_width = min_width;
        max_height = min_height;

        page = 0;
        list_information_type = 0;

        snprintf(_port, 7, "%u", gConfigNetwork.DefaultPort);
        SafeStrCpy(_name, gConfigNetwork.ServerName.c_str(), sizeof(_name));
        SafeStrCpy(_description, gConfigNetwork.ServerDescription.c_str(), sizeof(_description));
        SafeStrCpy(_greeting, gConfigNetwork.ServerGreeting.c_str(), sizeof(_greeting));
    }
    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_PORT_INPUT:
                WindowStartTextbox(*this, widgetIndex, STR_STRING, _port, 6);
                break;
            case WIDX_NAME_INPUT:
                WindowStartTextbox(*this, widgetIndex, STR_STRING, _name, 64);
                break;
            case WIDX_DESCRIPTION_INPUT:
                WindowStartTextbox(*this, widgetIndex, STR_STRING, _description, MAX_SERVER_DESCRIPTION_LENGTH);
                break;
            case WIDX_GREETING_INPUT:
                WindowStartTextbox(*this, widgetIndex, STR_STRING, _greeting, CHAT_INPUT_SIZE);
                break;
            case WIDX_PASSWORD_INPUT:
                WindowStartTextbox(*this, widgetIndex, STR_STRING, _password, 32);
                break;
            case WIDX_MAXPLAYERS_INCREASE:
                if (gConfigNetwork.Maxplayers < 255)
                {
                    gConfigNetwork.Maxplayers++;
                }
                ConfigSaveDefault();
                Invalidate();
                break;
            case WIDX_MAXPLAYERS_DECREASE:
                if (gConfigNetwork.Maxplayers > 1)
                {
                    gConfigNetwork.Maxplayers--;
                }
                ConfigSaveDefault();
                Invalidate();
                break;
            case WIDX_ADVERTISE_CHECKBOX:
                gConfigNetwork.Advertise = !gConfigNetwork.Advertise;
                ConfigSaveDefault();
                Invalidate();
                break;
            case WIDX_START_SERVER:
                NetworkSetPassword(_password);
                WindowScenarioselectOpen(ScenarioSelectCallback, false);
                break;
            case WIDX_LOAD_SERVER:
                NetworkSetPassword(_password);
                auto intent = Intent(WindowClass::Loadsave);
                intent.PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME);
                intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(LoadSaveCallback));
                ContextOpenIntent(&intent);
                break;
        }
    }
    void OnPrepareDraw() override
    {
        ColourSchemeUpdateByClass(this, WindowClass::ServerList);

        WidgetSetCheckboxValue(*this, WIDX_ADVERTISE_CHECKBOX, gConfigNetwork.Advertise);
        auto ft = Formatter::Common();
        ft.Increment(18);
        ft.Add<uint16_t>(gConfigNetwork.Maxplayers);
    }
    void OnUpdate() override
    {
        if (gCurrentTextBox.window.classification == classification && gCurrentTextBox.window.number == number)
        {
            WindowUpdateTextboxCaret();
            WidgetInvalidate(*this, WIDX_NAME_INPUT);
            WidgetInvalidate(*this, WIDX_DESCRIPTION_INPUT);
            WidgetInvalidate(*this, WIDX_GREETING_INPUT);
            WidgetInvalidate(*this, WIDX_PASSWORD_INPUT);
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

                SafeStrCpy(_port, temp.c_str(), sizeof(_port));

                // Don't allow negative/zero for port number
                tempPort = atoi(_port);
                if (tempPort > 0)
                {
                    gConfigNetwork.DefaultPort = tempPort;
                    ConfigSaveDefault();
                }

                WidgetInvalidate(*this, WIDX_PORT_INPUT);
                break;
            case WIDX_NAME_INPUT:
                if (strcmp(_name, temp.c_str()) == 0)
                    return;

                SafeStrCpy(_name, temp.c_str(), sizeof(_name));

                // Don't allow empty server names
                if (_name[0] != '\0')
                {
                    gConfigNetwork.ServerName = _name;
                    ConfigSaveDefault();
                }

                WidgetInvalidate(*this, WIDX_NAME_INPUT);
                break;
            case WIDX_DESCRIPTION_INPUT:
                if (strcmp(_description, temp.c_str()) == 0)
                    return;

                SafeStrCpy(_description, temp.c_str(), sizeof(_description));
                gConfigNetwork.ServerDescription = _description;
                ConfigSaveDefault();

                WidgetInvalidate(*this, WIDX_DESCRIPTION_INPUT);
                break;
            case WIDX_GREETING_INPUT:
                if (strcmp(_greeting, temp.c_str()) == 0)
                    return;

                SafeStrCpy(_greeting, temp.c_str(), sizeof(_greeting));
                gConfigNetwork.ServerGreeting = _greeting;
                ConfigSaveDefault();

                WidgetInvalidate(*this, WIDX_GREETING_INPUT);
                break;
            case WIDX_PASSWORD_INPUT:
                if (strcmp(_password, temp.c_str()) == 0)
                    return;

                SafeStrCpy(_password, temp.c_str(), sizeof(_password));

                WidgetInvalidate(*this, WIDX_PASSWORD_INPUT);
                break;
        }
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        DrawWidgets(dpi);
        DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_PORT_INPUT].top }, STR_PORT, {}, { colours[1] });
        DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_NAME_INPUT].top }, STR_SERVER_NAME, {}, { colours[1] });
        DrawTextBasic(
            &dpi, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_DESCRIPTION_INPUT].top }, STR_SERVER_DESCRIPTION, {},
            { colours[1] });
        DrawTextBasic(
            &dpi, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_GREETING_INPUT].top }, STR_SERVER_GREETING, {}, { colours[1] });
        DrawTextBasic(
            &dpi, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_PASSWORD_INPUT].top }, STR_PASSWORD, {}, { colours[1] });
        DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_MAXPLAYERS].top }, STR_MAX_PLAYERS, {}, { colours[1] });
    }

private:
    char _port[7];
    char _name[65];
    char _description[MAX_SERVER_DESCRIPTION_LENGTH];
    char _greeting[CHAT_INPUT_SIZE];
    char _password[33];
    static void ScenarioSelectCallback(const utf8* path)
    {
        GameNotifyMapChange();
        if (GetContext()->LoadParkFromFile(path, false, true))
        {
            NetworkBeginServer(gConfigNetwork.DefaultPort, gConfigNetwork.ListenAddress);
        }
    }

    static void LoadSaveCallback(int32_t result, const utf8* path)
    {
        if (result == MODAL_RESULT_OK)
        {
            GameNotifyMapChange();
            GetContext()->LoadParkFromFile(path);
            NetworkBeginServer(gConfigNetwork.DefaultPort, gConfigNetwork.ListenAddress);
        }
    }
};

WindowBase* WindowServerStartOpen()
{
    return WindowFocusOrCreate<ServerStartWindow>(WindowClass::ServerStart, WW, WH, WF_CENTRE_SCREEN);
}

#endif
