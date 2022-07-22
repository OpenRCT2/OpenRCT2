/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

static char _port[7];
static char _name[65];
static char _description[MAX_SERVER_DESCRIPTION_LENGTH];
static char _greeting[CHAT_INPUT_SIZE];
static char _password[33];

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
// clang-format on
static constexpr const int32_t WW = 300;
static constexpr const int32_t WH = 154;

static rct_widget _windowServerStartWidgets[] = {
    MakeWidget({ 0, 0 }, { WW, WH }, WindowWidgetType::Frame, WindowColour::Primary), // panel / background
    MakeWidget(
        { 1, 1 }, { 298, 14 }, WindowWidgetType::Caption, WindowColour::Primary, STR_START_SERVER,
        STR_WINDOW_TITLE_TIP), // title bar
    MakeWidget(
        { WW - 13, 2 }, { 11, 12 }, WindowWidgetType::CloseBox, WindowColour::Primary, STR_CLOSE_X,
        STR_CLOSE_WINDOW_TIP),                                                                // close x button
    MakeWidget({ 120, 20 }, { 173, 13 }, WindowWidgetType::TextBox, WindowColour::Secondary), // port text box
    MakeWidget({ 120, 36 }, { 173, 13 }, WindowWidgetType::TextBox, WindowColour::Secondary), // name text box
    MakeWidget({ 120, 52 }, { 173, 13 }, WindowWidgetType::TextBox, WindowColour::Secondary), // description text box
    MakeWidget({ 120, 68 }, { 173, 13 }, WindowWidgetType::TextBox, WindowColour::Secondary), // greeting text box
    MakeWidget({ 120, 84 }, { 173, 13 }, WindowWidgetType::TextBox, WindowColour::Secondary), // password text box
    MakeSpinnerWidgets(
        { 120, 100 }, { 173, 12 }, WindowWidgetType::Spinner, WindowColour::Secondary,
        STR_SERVER_MAX_PLAYERS_VALUE), // max players (3 widgets)
    MakeWidget(
        { 6, 117 }, { 287, 14 }, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_ADVERTISE,
        STR_ADVERTISE_SERVER_TIP), // advertise checkbox
    MakeWidget(
        { 6, WH - 6 - 13 }, { 101, 14 }, WindowWidgetType::Button, WindowColour::Secondary,
        STR_NEW_GAME), // start server button
    MakeWidget({ 112, WH - 6 - 13 }, { 101, 14 }, WindowWidgetType::Button, WindowColour::Secondary, STR_LOAD_GAME), // None
    WIDGETS_END,
};

class ServerStartWindow final : public Window
{
public:
    void OnOpen() override
    {
        _windowServerStartWidgets[WIDX_PORT_INPUT].string = _port;
        _windowServerStartWidgets[WIDX_NAME_INPUT].string = _name;
        _windowServerStartWidgets[WIDX_DESCRIPTION_INPUT].string = _description;
        _windowServerStartWidgets[WIDX_GREETING_INPUT].string = _greeting;
        _windowServerStartWidgets[WIDX_PASSWORD_INPUT].string = _password;
        widgets = _windowServerStartWidgets;
        WindowInitScrollWidgets(this);
        no_list_items = 0;
        selected_list_item = -1;
        frame_no = 0;
        min_width = width;
        min_height = height;
        max_width = min_width;
        max_height = min_height;

        page = 0;
        list_information_type = 0;

        snprintf(_port, 7, "%u", gConfigNetwork.default_port);
        safe_strcpy(_name, gConfigNetwork.server_name.c_str(), sizeof(_name));
        safe_strcpy(_description, gConfigNetwork.server_description.c_str(), sizeof(_description));
        safe_strcpy(_greeting, gConfigNetwork.server_greeting.c_str(), sizeof(_greeting));
    }
    void OnClose() override
    {
    }
    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                window_close(this);
                break;
            case WIDX_PORT_INPUT:
                window_start_textbox(this, widgetIndex, STR_STRING, _port, 6);
                break;
            case WIDX_NAME_INPUT:
                window_start_textbox(this, widgetIndex, STR_STRING, _name, 64);
                break;
            case WIDX_DESCRIPTION_INPUT:
                window_start_textbox(this, widgetIndex, STR_STRING, _description, MAX_SERVER_DESCRIPTION_LENGTH);
                break;
            case WIDX_GREETING_INPUT:
                window_start_textbox(this, widgetIndex, STR_STRING, _greeting, CHAT_INPUT_SIZE);
                break;
            case WIDX_PASSWORD_INPUT:
                window_start_textbox(this, widgetIndex, STR_STRING, _password, 32);
                break;
            case WIDX_MAXPLAYERS_INCREASE:
                if (gConfigNetwork.maxplayers < 255)
                {
                    gConfigNetwork.maxplayers++;
                }
                config_save_default();
                OnPrepareDraw();
                break;
            case WIDX_MAXPLAYERS_DECREASE:
                if (gConfigNetwork.maxplayers > 1)
                {
                    gConfigNetwork.maxplayers--;
                }
                config_save_default();
                OnPrepareDraw();
                break;
            case WIDX_ADVERTISE_CHECKBOX:
                gConfigNetwork.advertise = !gConfigNetwork.advertise;
                config_save_default();
                OnPrepareDraw();
                break;
            case WIDX_START_SERVER:
                network_set_password(_password);
                WindowScenarioselectOpen(ScenarioSelectCallback, false);
                break;
            case WIDX_LOAD_SERVER:
                network_set_password(_password);
                auto intent = Intent(WC_LOADSAVE);
                intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME);
                intent.putExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(LoadSaveCallback));
                context_open_intent(&intent);
                break;
        }
    }
    void OnUpdate() override
    {
        if (gCurrentTextBox.window.classification == classification && gCurrentTextBox.window.number == number)
        {
            window_update_textbox_caret();
            widget_invalidate(this, WIDX_NAME_INPUT);
            widget_invalidate(this, WIDX_DESCRIPTION_INPUT);
            widget_invalidate(this, WIDX_GREETING_INPUT);
            widget_invalidate(this, WIDX_PASSWORD_INPUT);
        }
    }
    void OnTextInput(rct_widgetindex widgetIndex, std::string_view text) override
    {
        if (text.empty())
            return;

        switch (widgetIndex)
        {
            case WIDX_PORT_INPUT:
                if (strcmp(_port, text.data()) == 0)
                    return;

                std::fill_n(_port, sizeof(_port), 0x00);
                if (text[0] != '\0')
                {
                    safe_strcpy(_port, text.data(), sizeof(_port));
                }

                gConfigNetwork.default_port = atoi(_port);
                config_save_default();

                widget_invalidate(this, WIDX_NAME_INPUT);
                break;
            case WIDX_NAME_INPUT:
                if (strcmp(_name, text.data()) == 0)
                    return;

                std::fill_n(_name, sizeof(_name), 0x00);
                if (text[0] != '\0')
                {
                    safe_strcpy(_name, text.data(), sizeof(_name));
                }

                if (_name[0] != '\0')
                {
                    gConfigNetwork.server_name = _name;
                    config_save_default();
                }

                widget_invalidate(this, WIDX_NAME_INPUT);
                break;
            case WIDX_DESCRIPTION_INPUT:
                if (strcmp(_description, text.data()) == 0)
                    return;

                std::fill_n(_description, sizeof(_description), 0x00);
                if (text[0] != '\0')
                {
                    safe_strcpy(_description, text.data(), sizeof(_description));
                }

                if (_description[0] != '\0')
                {
                    gConfigNetwork.server_description = _description;
                    config_save_default();
                }

                widget_invalidate(this, WIDX_DESCRIPTION_INPUT);
                break;
            case WIDX_GREETING_INPUT:
                if (strcmp(_greeting, text.data()) == 0)
                    return;

                std::fill_n(_greeting, sizeof(_greeting), 0x00);
                if (text[0] != '\0')
                {
                    safe_strcpy(_greeting, text.data(), sizeof(_greeting));
                }

                if (_greeting[0] != '\0')
                {
                    gConfigNetwork.server_greeting = _greeting;
                    config_save_default();
                }

                widget_invalidate(this, WIDX_GREETING_INPUT);
                break;
            case WIDX_PASSWORD_INPUT:
                if (strcmp(_password, text.data()) == 0)
                    return;

                std::fill_n(_password, sizeof(_password), 0x00);
                if (text[0] != '\0')
                {
                    safe_strcpy(_password, text.data(), sizeof(_password));
                }

                widget_invalidate(this, WIDX_PASSWORD_INPUT);
                break;
        }
    }
    void OnPrepareDraw() override
    {
        ColourSchemeUpdateByClass(this, WC_SERVER_LIST);

        WidgetSetCheckboxValue(this, WIDX_ADVERTISE_CHECKBOX, gConfigNetwork.advertise);
        auto ft = Formatter::Common();
        ft.Increment(18);
        ft.Add<uint16_t>(gConfigNetwork.maxplayers);
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        WindowDrawWidgets(this, &dpi);

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
    static void ScenarioSelectCallback(const utf8* path)
    {
        game_notify_map_change();
        if (GetContext()->LoadParkFromFile(path, false, true))
        {
            network_begin_server(gConfigNetwork.default_port, gConfigNetwork.listen_address);
        }
    }

    static void LoadSaveCallback(int32_t result, const utf8* path)
    {
        if (result == MODAL_RESULT_OK)
        {
            game_notify_map_change();
            context_load_park_from_file(path);
            network_begin_server(gConfigNetwork.default_port, gConfigNetwork.listen_address);
        }
    }
};

rct_window* WindowServerStartOpen()
{
    return WindowFocusOrCreate<ServerStartWindow>(WC_SERVER_START, WW, WH, WF_CENTRE_SCREEN);
}

#endif
