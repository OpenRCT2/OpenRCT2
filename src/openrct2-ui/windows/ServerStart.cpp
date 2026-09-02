/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "../interface/Theme.h"

    #include <algorithm>
    #include <openrct2-ui/interface/Dropdown.h>
    #include <openrct2-ui/interface/Widget.h>
    #include <openrct2-ui/interface/Window.h>
    #include <openrct2-ui/windows/Windows.h>
    #include <openrct2/Context.h>
    #include <openrct2/Game.h>
    #include <openrct2/config/Config.h>
    #include <openrct2/competitive/CompetitiveSession.h>
    #include <openrct2/core/String.hpp>
    #include <openrct2/drawing/Text.h>
    #include <openrct2/interface/Chat.h>
    #include <openrct2/localisation/Formatting.h>
    #include <openrct2/network/Network.h>
    #include <openrct2/ui/WindowManager.h>
    #include <openrct2/windows/Intent.h>

namespace OpenRCT2::Ui::Windows
{
    enum WindowServerStartWidgetIdx : WidgetIndex
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
        makeSpinnerWidgets({ 120, 100 }, { 173, 12 }, WidgetType::spinner, WindowColour::secondary,kStringIdEmpty), // max players (3 widgets)
        makeWidget({ 6, 117 }, { 287, 14 }, WidgetType::checkbox, WindowColour::secondary, STR_ADVERTISE,STR_ADVERTISE_SERVER_TIP), // advertise checkbox
        makeWidget({ 6, kWindowSize.height - 6 - 13 }, { 101, 14 }, WidgetType::button, WindowColour::secondary,STR_NEW_GAME), // start server button
        makeWidget({ 112, kWindowSize.height - 6 - 13 }, { 101, 14 }, WidgetType::button, WindowColour::secondary, STR_LOAD_GAME) // None
    );
    // clang-format on

    class ServerStartWindow final : public Window
    {
        u8string _maxPlayersCaption{};

    public:
        void onOpen() override
        {
            setWidgets(_windowServerStartWidgets);
            widgets[WIDX_PORT_INPUT].string = _port;
            widgets[WIDX_NAME_INPUT].string = _name;
            widgets[WIDX_DESCRIPTION_INPUT].string = _description;
            widgets[WIDX_GREETING_INPUT].string = _greeting;
            widgets[WIDX_PASSWORD_INPUT].string = _password;

            initScrollWidgets();
            WindowSetResize(*this, { width, height }, { width, height });

            currentFrame = 0;
            page = 0;
            listInformationType = 0;

            snprintf(_port, 7, "%u", Config::Get().network.defaultPort);
            String::safeUtf8Copy(_name, Config::Get().network.serverName.c_str(), sizeof(_name));
            String::safeUtf8Copy(_description, Config::Get().network.serverDescription.c_str(), sizeof(_description));
            String::safeUtf8Copy(_greeting, Config::Get().network.serverGreeting.c_str(), sizeof(_greeting));
        }
        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_PORT_INPUT:
                    WindowStartTextbox(*this, widgetIndex, _port, 6);
                    break;
                case WIDX_NAME_INPUT:
                    WindowStartTextbox(*this, widgetIndex, _name, 64);
                    break;
                case WIDX_DESCRIPTION_INPUT:
                    WindowStartTextbox(*this, widgetIndex, _description, Network::kMaxServerDescriptionLength);
                    break;
                case WIDX_GREETING_INPUT:
                    WindowStartTextbox(*this, widgetIndex, _greeting, kChatInputSize);
                    break;
                case WIDX_PASSWORD_INPUT:
                    WindowStartTextbox(*this, widgetIndex, _password, 32);
                    break;
                case WIDX_MAXPLAYERS_INCREASE:
                    if (Config::Get().network.maxplayers < 255)
                    {
                        Config::Get().network.maxplayers++;
                    }
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_MAXPLAYERS_DECREASE:
                    if (Config::Get().network.maxplayers > 1)
                    {
                        Config::Get().network.maxplayers--;
                    }
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_ADVERTISE_CHECKBOX:
                    Config::Get().network.advertise = !Config::Get().network.advertise;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_START_SERVER:
                    Network::SetPassword(_password);
                    ScenarioselectOpen(ScenarioSelectCallback);
                    break;
                case WIDX_LOAD_SERVER:
                    Network::SetPassword(_password);
                    auto intent = Intent(WindowClass::loadsave);
                    intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::load);
                    intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::park);
                    intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(LoadSaveCallback));
                    ContextOpenIntent(&intent);
                    break;
            }
        }
        void onPrepareDraw() override
        {
            ColourSchemeUpdateByClass(this, WindowClass::serverList);

            setCheckboxValue(WIDX_ADVERTISE_CHECKBOX, Config::Get().network.advertise);

            _maxPlayersCaption = std::to_string(Config::Get().network.maxplayers);
            widgets[WIDX_MAXPLAYERS].setString(_maxPlayersCaption.c_str());
        }
        void onUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                invalidateWidget(WIDX_NAME_INPUT);
                invalidateWidget(WIDX_DESCRIPTION_INPUT);
                invalidateWidget(WIDX_GREETING_INPUT);
                invalidateWidget(WIDX_PASSWORD_INPUT);
            }
        }
        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
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
                        Config::Get().network.defaultPort = tempPort;
                        Config::Save();
                    }

                    invalidateWidget(WIDX_PORT_INPUT);
                    break;
                case WIDX_NAME_INPUT:
                    if (strcmp(_name, temp.c_str()) == 0)
                        return;

                    String::safeUtf8Copy(_name, temp.c_str(), sizeof(_name));

                    // Don't allow empty server names
                    if (_name[0] != '\0')
                    {
                        Config::Get().network.serverName = _name;
                        Config::Save();
                    }

                    invalidateWidget(WIDX_NAME_INPUT);
                    break;
                case WIDX_DESCRIPTION_INPUT:
                    if (strcmp(_description, temp.c_str()) == 0)
                        return;

                    String::safeUtf8Copy(_description, temp.c_str(), sizeof(_description));
                    Config::Get().network.serverDescription = _description;
                    Config::Save();

                    invalidateWidget(WIDX_DESCRIPTION_INPUT);
                    break;
                case WIDX_GREETING_INPUT:
                    if (strcmp(_greeting, temp.c_str()) == 0)
                        return;

                    String::safeUtf8Copy(_greeting, temp.c_str(), sizeof(_greeting));
                    Config::Get().network.serverGreeting = _greeting;
                    Config::Save();

                    invalidateWidget(WIDX_GREETING_INPUT);
                    break;
                case WIDX_PASSWORD_INPUT:
                    if (strcmp(_password, temp.c_str()) == 0)
                        return;

                    String::safeUtf8Copy(_password, temp.c_str(), sizeof(_password));

                    invalidateWidget(WIDX_PASSWORD_INPUT);
                    break;
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);
            drawText(rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_PORT_INPUT].top }, STR_PORT, { colours[1] });
            drawText(rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_NAME_INPUT].top }, STR_SERVER_NAME, { colours[1] });
            drawText(
                rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_DESCRIPTION_INPUT].top }, STR_SERVER_DESCRIPTION,
                { colours[1] });
            drawText(
                rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_GREETING_INPUT].top }, STR_SERVER_GREETING, { colours[1] });
            drawText(rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_PASSWORD_INPUT].top }, STR_PASSWORD, { colours[1] });
            drawText(rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_MAXPLAYERS].top }, STR_MAX_PLAYERS, { colours[1] });
        }

    private:
        char _port[7];
        char _name[65];
        char _description[Network::kMaxServerDescriptionLength];
        char _greeting[kChatInputSize];
        char _password[33];
        static void ScenarioSelectCallback(const utf8* path)
        {
            GameNotifyMapChange();
            if (GetContext()->LoadParkFromFile(path, false, true))
            {
                Network::BeginServer(Config::Get().network.defaultPort, Config::Get().network.listenAddress);
            }
        }

        static void LoadSaveCallback(ModalResult result, const utf8* path)
        {
            if (result == ModalResult::ok)
            {
                GameNotifyMapChange();
                GetContext()->LoadParkFromFile(path);
                Network::BeginServer(Config::Get().network.defaultPort, Config::Get().network.listenAddress);
            }
        }
    };

    WindowBase* ServerStartOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<ServerStartWindow>(WindowClass::serverStart, kWindowSize, WindowFlag::centreScreen);
    }

    enum CompetitiveServerStartWidgetIdx : WidgetIndex
    {
        CWIDX_BACKGROUND,
        CWIDX_TITLE,
        CWIDX_CLOSE,
        CWIDX_TAB_MATCH,
        CWIDX_TAB_SABOTAGE,
        CWIDX_COMPETITION_NAME,
        CWIDX_PLAYER_NAME,
        CWIDX_PORT,
        CWIDX_MAX_PLAYERS,
        CWIDX_MAX_PLAYERS_UP,
        CWIDX_MAX_PLAYERS_DOWN,
        CWIDX_ADVERTISE,
        CWIDX_LATE_JOIN,
        CWIDX_VICTORY,
        CWIDX_VICTORY_BUTTON,
        CWIDX_METRIC,
        CWIDX_METRIC_BUTTON,
        CWIDX_DEADLINE,
        CWIDX_DEADLINE_UP,
        CWIDX_DEADLINE_DOWN,
        CWIDX_TARGET,
        CWIDX_TARGET_UP,
        CWIDX_TARGET_DOWN,
        CWIDX_MAX_SPEED,
        CWIDX_MAX_SPEED_UP,
        CWIDX_MAX_SPEED_DOWN,
        CWIDX_VANDAL_ENABLED,
        CWIDX_VANDAL_COST,
        CWIDX_VANDAL_COST_UP,
        CWIDX_VANDAL_COST_DOWN,
        CWIDX_VANDAL_COOLDOWN,
        CWIDX_VANDAL_COOLDOWN_UP,
        CWIDX_VANDAL_COOLDOWN_DOWN,
        CWIDX_VANDAL_DURATION,
        CWIDX_VANDAL_DURATION_UP,
        CWIDX_VANDAL_DURATION_DOWN,
        CWIDX_VANDAL_POTENCY,
        CWIDX_VANDAL_POTENCY_UP,
        CWIDX_VANDAL_POTENCY_DOWN,
        CWIDX_VANDAL_USESYEAR,
        CWIDX_VANDAL_USESYEAR_UP,
        CWIDX_VANDAL_USESYEAR_DOWN,
        CWIDX_MISINFO_ENABLED,
        CWIDX_MISINFO_COST,
        CWIDX_MISINFO_COST_UP,
        CWIDX_MISINFO_COST_DOWN,
        CWIDX_MISINFO_COOLDOWN,
        CWIDX_MISINFO_COOLDOWN_UP,
        CWIDX_MISINFO_COOLDOWN_DOWN,
        CWIDX_MISINFO_DURATION,
        CWIDX_MISINFO_DURATION_UP,
        CWIDX_MISINFO_DURATION_DOWN,
        CWIDX_MISINFO_POTENCY,
        CWIDX_MISINFO_POTENCY_UP,
        CWIDX_MISINFO_POTENCY_DOWN,
        CWIDX_MISINFO_USESYEAR,
        CWIDX_MISINFO_USESYEAR_UP,
        CWIDX_MISINFO_USESYEAR_DOWN,
        CWIDX_POISON_ENABLED,
        CWIDX_POISON_COST,
        CWIDX_POISON_COST_UP,
        CWIDX_POISON_COST_DOWN,
        CWIDX_POISON_COOLDOWN,
        CWIDX_POISON_COOLDOWN_UP,
        CWIDX_POISON_COOLDOWN_DOWN,
        CWIDX_POISON_DURATION,
        CWIDX_POISON_DURATION_UP,
        CWIDX_POISON_DURATION_DOWN,
        CWIDX_POISON_POTENCY,
        CWIDX_POISON_POTENCY_UP,
        CWIDX_POISON_POTENCY_DOWN,
        CWIDX_POISON_USESYEAR,
        CWIDX_POISON_USESYEAR_UP,
        CWIDX_POISON_USESYEAR_DOWN,
        CWIDX_TOILET_ENABLED,
        CWIDX_TOILET_COST,
        CWIDX_TOILET_COST_UP,
        CWIDX_TOILET_COST_DOWN,
        CWIDX_TOILET_COOLDOWN,
        CWIDX_TOILET_COOLDOWN_UP,
        CWIDX_TOILET_COOLDOWN_DOWN,
        CWIDX_TOILET_DURATION,
        CWIDX_TOILET_DURATION_UP,
        CWIDX_TOILET_DURATION_DOWN,
        CWIDX_TOILET_USESYEAR,
        CWIDX_TOILET_USESYEAR_UP,
        CWIDX_TOILET_USESYEAR_DOWN,
        CWIDX_AGITATOR_ENABLED,
        CWIDX_AGITATOR_COST,
        CWIDX_AGITATOR_COST_UP,
        CWIDX_AGITATOR_COST_DOWN,
        CWIDX_AGITATOR_COOLDOWN,
        CWIDX_AGITATOR_COOLDOWN_UP,
        CWIDX_AGITATOR_COOLDOWN_DOWN,
        CWIDX_AGITATOR_DURATION,
        CWIDX_AGITATOR_DURATION_UP,
        CWIDX_AGITATOR_DURATION_DOWN,
        CWIDX_AGITATOR_POTENCY,
        CWIDX_AGITATOR_POTENCY_UP,
        CWIDX_AGITATOR_POTENCY_DOWN,
        CWIDX_AGITATOR_USESYEAR,
        CWIDX_AGITATOR_USESYEAR_UP,
        CWIDX_AGITATOR_USESYEAR_DOWN,
        CWIDX_SABOTEUR_ENABLED,
        CWIDX_SABOTEUR_COST,
        CWIDX_SABOTEUR_COST_UP,
        CWIDX_SABOTEUR_COST_DOWN,
        CWIDX_SABOTEUR_COOLDOWN,
        CWIDX_SABOTEUR_COOLDOWN_UP,
        CWIDX_SABOTEUR_COOLDOWN_DOWN,
        CWIDX_SABOTEUR_DURATION,
        CWIDX_SABOTEUR_DURATION_UP,
        CWIDX_SABOTEUR_DURATION_DOWN,
        CWIDX_SABOTEUR_USESYEAR,
        CWIDX_SABOTEUR_USESYEAR_UP,
        CWIDX_SABOTEUR_USESYEAR_DOWN,
        CWIDX_HITMAN_ENABLED,
        CWIDX_HITMAN_COST,
        CWIDX_HITMAN_COST_UP,
        CWIDX_HITMAN_COST_DOWN,
        CWIDX_HITMAN_COOLDOWN,
        CWIDX_HITMAN_COOLDOWN_UP,
        CWIDX_HITMAN_COOLDOWN_DOWN,
        CWIDX_HITMAN_DURATION,
        CWIDX_HITMAN_DURATION_UP,
        CWIDX_HITMAN_DURATION_DOWN,
        CWIDX_HITMAN_USESYEAR,
        CWIDX_HITMAN_USESYEAR_UP,
        CWIDX_HITMAN_USESYEAR_DOWN,
        CWIDX_RESEARCH_ENABLED,
        CWIDX_RESEARCH_COST,
        CWIDX_RESEARCH_COST_UP,
        CWIDX_RESEARCH_COST_DOWN,
        CWIDX_RESEARCH_COOLDOWN,
        CWIDX_RESEARCH_COOLDOWN_UP,
        CWIDX_RESEARCH_COOLDOWN_DOWN,
        CWIDX_RESEARCH_DURATION,
        CWIDX_RESEARCH_DURATION_UP,
        CWIDX_RESEARCH_DURATION_DOWN,
        CWIDX_RESEARCH_USESYEAR,
        CWIDX_RESEARCH_USESYEAR_UP,
        CWIDX_RESEARCH_USESYEAR_DOWN,
        CWIDX_UNION_ENABLED,
        CWIDX_UNION_COST,
        CWIDX_UNION_COST_UP,
        CWIDX_UNION_COST_DOWN,
        CWIDX_UNION_COOLDOWN,
        CWIDX_UNION_COOLDOWN_UP,
        CWIDX_UNION_COOLDOWN_DOWN,
        CWIDX_UNION_DURATION,
        CWIDX_UNION_DURATION_UP,
        CWIDX_UNION_DURATION_DOWN,
        CWIDX_UNION_USESYEAR,
        CWIDX_UNION_USESYEAR_UP,
        CWIDX_UNION_USESYEAR_DOWN,
        CWIDX_KARENS_ENABLED,
        CWIDX_KARENS_COST,
        CWIDX_KARENS_COST_UP,
        CWIDX_KARENS_COST_DOWN,
        CWIDX_KARENS_COOLDOWN,
        CWIDX_KARENS_COOLDOWN_UP,
        CWIDX_KARENS_COOLDOWN_DOWN,
        CWIDX_KARENS_DURATION,
        CWIDX_KARENS_DURATION_UP,
        CWIDX_KARENS_DURATION_DOWN,
        CWIDX_KARENS_POTENCY,
        CWIDX_KARENS_POTENCY_UP,
        CWIDX_KARENS_POTENCY_DOWN,
        CWIDX_KARENS_USESYEAR,
        CWIDX_KARENS_USESYEAR_UP,
        CWIDX_KARENS_USESYEAR_DOWN,
        CWIDX_STONERS_ENABLED,
        CWIDX_STONERS_COST,
        CWIDX_STONERS_COST_UP,
        CWIDX_STONERS_COST_DOWN,
        CWIDX_STONERS_COOLDOWN,
        CWIDX_STONERS_COOLDOWN_UP,
        CWIDX_STONERS_COOLDOWN_DOWN,
        CWIDX_STONERS_DURATION,
        CWIDX_STONERS_DURATION_UP,
        CWIDX_STONERS_DURATION_DOWN,
        CWIDX_STONERS_POTENCY,
        CWIDX_STONERS_POTENCY_UP,
        CWIDX_STONERS_POTENCY_DOWN,
        CWIDX_STONERS_USESYEAR,
        CWIDX_STONERS_USESYEAR_UP,
        CWIDX_STONERS_USESYEAR_DOWN,
        CWIDX_CREATE_LOBBY,
    };

    static constexpr ScreenSize kCompetitiveWindowSize = { 770, 750 };

    // clang-format off
    static constexpr auto _competitiveServerStartWidgets = makeWidgets(
        makeWindowShim(STR_START_SERVER, kCompetitiveWindowSize),
        makeWidget({  3, 17}, {140, 26}, WidgetType::button,       WindowColour::secondary, kStringIdEmpty),
        makeWidget({143, 17}, {140, 26}, WidgetType::button,       WindowColour::secondary, kStringIdEmpty),
        makeWidget({170, 50}, {440, 13}, WidgetType::textBox,      WindowColour::secondary),
        makeWidget({170, 67}, {440, 13}, WidgetType::textBox,      WindowColour::secondary),
        makeWidget({170, 84}, {120, 13}, WidgetType::textBox,      WindowColour::secondary),
        makeHoldableSpinnerWidgets({170,101}, {120, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({310,101}, {140, 14}, WidgetType::checkbox,     WindowColour::secondary, kStringIdEmpty),
        makeWidget({460,101}, {150, 14}, WidgetType::checkbox,     WindowColour::secondary, kStringIdEmpty),
        makeWidget({170,123}, {440, 14}, WidgetType::dropdownMenu, WindowColour::secondary),
        makeWidget({598,124}, { 11, 12}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH),
        makeWidget({170,140}, {440, 14}, WidgetType::dropdownMenu, WindowColour::secondary),
        makeWidget({598,141}, { 11, 12}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH),
        makeHoldableSpinnerWidgets({170,157}, {120, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({170,174}, {160, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({170,191}, {120, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),

        makeWidget({  8, 68}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112, 68}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226, 68}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322, 68}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({443, 68}, {167, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({632, 68}, {130, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,125}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,125}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,125}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,125}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({443,125}, {167, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({632,125}, {130, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,182}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,182}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,182}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,182}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({443,182}, {167, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({632,182}, {130, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,239}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,239}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,239}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,239}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({632,239}, {130, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,296}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,296}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,296}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,296}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({443,296}, {167, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({632,296}, {130, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,353}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,353}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,353}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,353}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({632,353}, {130, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,410}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,410}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,410}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,410}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({632,410}, {130, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,467}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,467}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,467}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,467}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({632,467}, {130, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,524}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,524}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,524}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,524}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({632,524}, {130, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,581}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,581}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,581}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,581}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({443,581}, {167, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({632,581}, {130, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,638}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,638}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,638}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,638}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({443,638}, {167, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({632,638}, {130, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({430,728}, {180, 14}, WidgetType::button, WindowColour::secondary, kStringIdEmpty)
    );
    // clang-format on

    class CompetitiveServerStartWindow final : public Window
    {
    private:
        Competitive::MatchRules _rules{};
        u8string _competitionName;
        u8string _playerName;
        u8string _port;
        std::string _matchTabText = "Match rules";
        std::string _sabotageTabText = "Sabotage rules";
        std::string _advertiseText = "Public game list";
        std::string _lateJoinText = "Allow late join";
        std::string _createText = "Choose scenario and create lobby";
        std::string _vandalText = "Vandal";
        std::string _misinfoText = "Misinformation";
        std::string _poisonText = "Poisoning";
        std::string _toiletText = "Toilet bomber";
        std::string _agitatorText = "Agitator";
        std::string _saboteurText = "Saboteur";
        std::string _hitmanText = "Hitman";
        std::string _researchText = "Research sabotage";
        std::string _unionText = "Union disruption";
        std::string _karensText = "Karens";
        std::string _stonersText = "Stoners";
        std::string _maxPlayersCaption;
        std::string _victoryCaption;
        std::string _metricCaption;
        std::string _deadlineCaption;
        std::string _targetCaption;
        std::string _maxSpeedCaption;
        std::string _pacingHint;
        std::string _vandalCostCaption;
        std::string _vandalCooldownCaption;
        std::string _vandalDurationCaption;
        std::string _vandalPotencyCaption;
        std::string _misinfoCostCaption;
        std::string _misinfoCooldownCaption;
        std::string _misinfoDurationCaption;
        std::string _misinfoPotencyCaption;
        std::string _poisonCostCaption;
        std::string _poisonCooldownCaption;
        std::string _poisonDurationCaption;
        std::string _poisonPotencyCaption;
        std::string _toiletCostCaption;
        std::string _toiletCooldownCaption;
        std::string _toiletDurationCaption;
        std::string _agitatorCostCaption;
        std::string _agitatorCooldownCaption;
        std::string _agitatorDurationCaption;
        std::string _agitatorPotencyCaption;
        std::string _saboteurCostCaption;
        std::string _saboteurCooldownCaption;
        std::string _saboteurDurationCaption;
        std::string _hitmanCostCaption;
        std::string _hitmanCooldownCaption;
        std::string _hitmanDurationCaption;
        std::string _vandalUsesCaption;
        std::string _misinfoUsesCaption;
        std::string _poisonUsesCaption;
        std::string _toiletUsesCaption;
        std::string _agitatorUsesCaption;
        std::string _saboteurUsesCaption;
        std::string _hitmanUsesCaption;
        std::string _researchCostCaption;
        std::string _researchCooldownCaption;
        std::string _researchDurationCaption;
        std::string _researchUsesCaption;
        std::string _unionCostCaption;
        std::string _unionCooldownCaption;
        std::string _unionDurationCaption;
        std::string _unionUsesCaption;
        std::string _karensCostCaption;
        std::string _karensCooldownCaption;
        std::string _karensPotencyCaption;
        std::string _karensUsesCaption;
        std::string _stonersCostCaption;
        std::string _stonersCooldownCaption;
        std::string _stonersPotencyCaption;
        std::string _stonersUsesCaption;
        std::string _groupDurationCaption = "leaves freely";

    public:
        void onOpen() override
        {
            setWidgets(_competitiveServerStartWidgets);
            page = 0;
            _competitionName = Config::Get().network.serverName;
            if (_competitionName.empty())
                _competitionName = "OpenRCT2 Competition";
            _playerName = Config::Get().network.playerName;
            _port = std::to_string(Competitive::kDefaultPort);

            widgets[CWIDX_TAB_MATCH].setString(_matchTabText.c_str());
            widgets[CWIDX_TAB_SABOTAGE].setString(_sabotageTabText.c_str());
            widgets[CWIDX_COMPETITION_NAME].string = const_cast<utf8*>(_competitionName.c_str());
            widgets[CWIDX_PLAYER_NAME].string = const_cast<utf8*>(_playerName.c_str());
            widgets[CWIDX_PORT].string = const_cast<utf8*>(_port.c_str());
            widgets[CWIDX_ADVERTISE].setString(_advertiseText.c_str());
            widgets[CWIDX_LATE_JOIN].setString(_lateJoinText.c_str());
            widgets[CWIDX_VANDAL_ENABLED].setString(_vandalText.c_str());
            widgets[CWIDX_MISINFO_ENABLED].setString(_misinfoText.c_str());
            widgets[CWIDX_POISON_ENABLED].setString(_poisonText.c_str());
            widgets[CWIDX_TOILET_ENABLED].setString(_toiletText.c_str());
            widgets[CWIDX_AGITATOR_ENABLED].setString(_agitatorText.c_str());
            widgets[CWIDX_SABOTEUR_ENABLED].setString(_saboteurText.c_str());
            widgets[CWIDX_HITMAN_ENABLED].setString(_hitmanText.c_str());
            widgets[CWIDX_RESEARCH_ENABLED].setString(_researchText.c_str());
            widgets[CWIDX_UNION_ENABLED].setString(_unionText.c_str());
            widgets[CWIDX_KARENS_ENABLED].setString(_karensText.c_str());
            widgets[CWIDX_STONERS_ENABLED].setString(_stonersText.c_str());
            widgets[CWIDX_CREATE_LOBBY].setString(_createText.c_str());
            initScrollWidgets();
            WindowSetResize(*this, kCompetitiveWindowSize, kCompetitiveWindowSize);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case CWIDX_CLOSE:
                    close();
                    break;
                case CWIDX_TAB_MATCH:
                    SetPage(0);
                    break;
                case CWIDX_TAB_SABOTAGE:
                    SetPage(1);
                    break;
                case CWIDX_COMPETITION_NAME:
                    WindowStartTextbox(*this, widgetIndex, _competitionName, 64);
                    break;
                case CWIDX_PLAYER_NAME:
                    WindowStartTextbox(*this, widgetIndex, _playerName, kMaxPlayerNameLength);
                    break;
                case CWIDX_PORT:
                    WindowStartTextbox(*this, widgetIndex, _port, 6);
                    break;
                case CWIDX_ADVERTISE:
                    Config::Get().network.advertise = !Config::Get().network.advertise;
                    Config::Save();
                    invalidate();
                    break;
                case CWIDX_LATE_JOIN:
                    _rules.allowLateJoin = !_rules.allowLateJoin;
                    invalidate();
                    break;
                case CWIDX_VANDAL_ENABLED:
                    _rules.vandal.enabled = !_rules.vandal.enabled;
                    invalidate();
                    break;
                case CWIDX_MISINFO_ENABLED:
                    _rules.misinformation.enabled = !_rules.misinformation.enabled;
                    invalidate();
                    break;
                case CWIDX_POISON_ENABLED:
                    _rules.poison.enabled = !_rules.poison.enabled;
                    invalidate();
                    break;
                case CWIDX_TOILET_ENABLED:
                    _rules.toiletBomber.enabled = !_rules.toiletBomber.enabled;
                    invalidate();
                    break;
                case CWIDX_AGITATOR_ENABLED:
                    _rules.agitator.enabled = !_rules.agitator.enabled;
                    invalidate();
                    break;
                case CWIDX_SABOTEUR_ENABLED:
                    _rules.saboteur.enabled = !_rules.saboteur.enabled;
                    invalidate();
                    break;
                case CWIDX_HITMAN_ENABLED:
                    _rules.hitman.enabled = !_rules.hitman.enabled;
                    invalidate();
                    break;
                case CWIDX_RESEARCH_ENABLED:
                    _rules.researchSabotage.enabled = !_rules.researchSabotage.enabled;
                    invalidate();
                    break;
                case CWIDX_UNION_ENABLED:
                    _rules.unionDisruption.enabled = !_rules.unionDisruption.enabled;
                    invalidate();
                    break;
                case CWIDX_KARENS_ENABLED:
                    _rules.karens.enabled = !_rules.karens.enabled;
                    invalidate();
                    break;
                case CWIDX_STONERS_ENABLED:
                    _rules.stoners.enabled = !_rules.stoners.enabled;
                    invalidate();
                    break;
                case CWIDX_CREATE_LOBBY:
                    ChooseScenario();
                    break;
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case CWIDX_VICTORY_BUTTON:
                    ShowVictoryDropdown();
                    return;
                case CWIDX_METRIC_BUTTON:
                    ShowMetricDropdown();
                    return;
                case CWIDX_MAX_PLAYERS_UP:
                    _rules.maxPlayers = std::min<uint8_t>(64, _rules.maxPlayers + 1);
                    break;
                case CWIDX_MAX_PLAYERS_DOWN:
                    _rules.maxPlayers = std::max<uint8_t>(2, _rules.maxPlayers - 1);
                    break;
                case CWIDX_DEADLINE_UP:
                    _rules.deadlineYear = std::min<uint16_t>(100, _rules.deadlineYear + 1);
                    break;
                case CWIDX_DEADLINE_DOWN:
                    _rules.deadlineYear = std::max<uint16_t>(1, _rules.deadlineYear - 1);
                    break;
                case CWIDX_TARGET_UP:
                    AdjustTarget(1);
                    break;
                case CWIDX_TARGET_DOWN:
                    AdjustTarget(-1);
                    break;
                case CWIDX_MAX_SPEED_UP:
                    _rules.maxGameSpeed = std::min<uint8_t>(4, _rules.maxGameSpeed + 1);
                    break;
                case CWIDX_MAX_SPEED_DOWN:
                    _rules.maxGameSpeed = std::max<uint8_t>(1, _rules.maxGameSpeed - 1);
                    break;
                case CWIDX_VANDAL_USESYEAR_UP: AdjustUsesPerYear(_rules.vandal, 1); break;
                case CWIDX_VANDAL_USESYEAR_DOWN: AdjustUsesPerYear(_rules.vandal, -1); break;
                case CWIDX_MISINFO_USESYEAR_UP: AdjustUsesPerYear(_rules.misinformation, 1); break;
                case CWIDX_MISINFO_USESYEAR_DOWN: AdjustUsesPerYear(_rules.misinformation, -1); break;
                case CWIDX_POISON_USESYEAR_UP: AdjustUsesPerYear(_rules.poison, 1); break;
                case CWIDX_POISON_USESYEAR_DOWN: AdjustUsesPerYear(_rules.poison, -1); break;
                case CWIDX_TOILET_USESYEAR_UP: AdjustUsesPerYear(_rules.toiletBomber, 1); break;
                case CWIDX_TOILET_USESYEAR_DOWN: AdjustUsesPerYear(_rules.toiletBomber, -1); break;
                case CWIDX_AGITATOR_USESYEAR_UP: AdjustUsesPerYear(_rules.agitator, 1); break;
                case CWIDX_AGITATOR_USESYEAR_DOWN: AdjustUsesPerYear(_rules.agitator, -1); break;
                case CWIDX_SABOTEUR_USESYEAR_UP: AdjustUsesPerYear(_rules.saboteur, 1); break;
                case CWIDX_SABOTEUR_USESYEAR_DOWN: AdjustUsesPerYear(_rules.saboteur, -1); break;
                case CWIDX_HITMAN_USESYEAR_UP: AdjustUsesPerYear(_rules.hitman, 1); break;
                case CWIDX_HITMAN_USESYEAR_DOWN: AdjustUsesPerYear(_rules.hitman, -1); break;
                case CWIDX_RESEARCH_USESYEAR_UP: AdjustUsesPerYear(_rules.researchSabotage, 1); break;
                case CWIDX_RESEARCH_USESYEAR_DOWN: AdjustUsesPerYear(_rules.researchSabotage, -1); break;
                case CWIDX_UNION_USESYEAR_UP: AdjustUsesPerYear(_rules.unionDisruption, 1); break;
                case CWIDX_UNION_USESYEAR_DOWN: AdjustUsesPerYear(_rules.unionDisruption, -1); break;
                case CWIDX_KARENS_USESYEAR_UP: AdjustUsesPerYear(_rules.karens, 1); break;
                case CWIDX_KARENS_USESYEAR_DOWN: AdjustUsesPerYear(_rules.karens, -1); break;
                case CWIDX_STONERS_USESYEAR_UP: AdjustUsesPerYear(_rules.stoners, 1); break;
                case CWIDX_STONERS_USESYEAR_DOWN: AdjustUsesPerYear(_rules.stoners, -1); break;
                default:
                    if (AdjustAbilityRule(widgetIndex))
                        break;
                    return;
            }
            invalidate();
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (dropdownIndex < 0)
                return;
            if (widgetIndex == CWIDX_VICTORY_BUTTON && dropdownIndex <= 1)
            {
                _rules.victoryMode = static_cast<Competitive::VictoryMode>(dropdownIndex);
            }
            else if (widgetIndex == CWIDX_METRIC_BUTTON && dropdownIndex <= 4)
            {
                _rules.metric = static_cast<Competitive::Metric>(dropdownIndex);
                NormaliseTarget();
            }
            invalidate();
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (text.empty())
                return;
            if (widgetIndex == CWIDX_COMPETITION_NAME)
            {
                _competitionName = text;
                widgets[CWIDX_COMPETITION_NAME].string = const_cast<utf8*>(_competitionName.c_str());
            }
            else if (widgetIndex == CWIDX_PLAYER_NAME)
            {
                _playerName = text;
                Config::Get().network.playerName = _playerName;
                widgets[CWIDX_PLAYER_NAME].string = const_cast<utf8*>(_playerName.c_str());
                Config::Save();
            }
            else if (widgetIndex == CWIDX_PORT)
            {
                const auto parsed = String::tryParse<uint16_t>(text);
                if (parsed.has_value() && parsed.value() > 0)
                {
                    _port = text;
                    widgets[CWIDX_PORT].string = const_cast<utf8*>(_port.c_str());
                }
            }
            invalidate();
        }

        void onUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                invalidate();
            }
        }

        void onPrepareDraw() override
        {
            ColourSchemeUpdateByClass(this, WindowClass::serverList);
            const bool matchPage = page == 0;
            setWidgetPressed(CWIDX_TAB_MATCH, matchPage);
            setWidgetPressed(CWIDX_TAB_SABOTAGE, !matchPage);

            for (WidgetIndex i = CWIDX_COMPETITION_NAME; i <= CWIDX_MAX_SPEED_DOWN; i++)
                widgets[i].setVisible(matchPage);
            for (WidgetIndex i = CWIDX_VANDAL_ENABLED; i <= CWIDX_STONERS_USESYEAR_DOWN; i++)
                widgets[i].setVisible(!matchPage);

            const bool targetMode = _rules.victoryMode == Competitive::VictoryMode::target;
            for (WidgetIndex i = CWIDX_TARGET; i <= CWIDX_TARGET_DOWN; i++)
                widgets[i].setVisible(matchPage && targetMode);

            setCheckboxValue(CWIDX_ADVERTISE, Config::Get().network.advertise);
            setCheckboxValue(CWIDX_LATE_JOIN, _rules.allowLateJoin);
            setCheckboxValue(CWIDX_VANDAL_ENABLED, _rules.vandal.enabled);
            setCheckboxValue(CWIDX_MISINFO_ENABLED, _rules.misinformation.enabled);
            setCheckboxValue(CWIDX_POISON_ENABLED, _rules.poison.enabled);
            setCheckboxValue(CWIDX_TOILET_ENABLED, _rules.toiletBomber.enabled);
            setCheckboxValue(CWIDX_AGITATOR_ENABLED, _rules.agitator.enabled);
            setCheckboxValue(CWIDX_SABOTEUR_ENABLED, _rules.saboteur.enabled);
            setCheckboxValue(CWIDX_HITMAN_ENABLED, _rules.hitman.enabled);
            setCheckboxValue(CWIDX_RESEARCH_ENABLED, _rules.researchSabotage.enabled);
            setCheckboxValue(CWIDX_UNION_ENABLED, _rules.unionDisruption.enabled);
            setCheckboxValue(CWIDX_KARENS_ENABLED, _rules.karens.enabled);
            setCheckboxValue(CWIDX_STONERS_ENABLED, _rules.stoners.enabled);

            SetAbilityWidgetsEnabled(CWIDX_VANDAL_COST, CWIDX_VANDAL_USESYEAR_DOWN, _rules.vandal.enabled);
            SetAbilityWidgetsEnabled(CWIDX_MISINFO_COST, CWIDX_MISINFO_USESYEAR_DOWN, _rules.misinformation.enabled);
            SetAbilityWidgetsEnabled(CWIDX_POISON_COST, CWIDX_POISON_USESYEAR_DOWN, _rules.poison.enabled);
            SetAbilityWidgetsEnabled(CWIDX_TOILET_COST, CWIDX_TOILET_USESYEAR_DOWN, _rules.toiletBomber.enabled);
            SetAbilityWidgetsEnabled(CWIDX_AGITATOR_COST, CWIDX_AGITATOR_USESYEAR_DOWN, _rules.agitator.enabled);
            SetAbilityWidgetsEnabled(CWIDX_SABOTEUR_COST, CWIDX_SABOTEUR_USESYEAR_DOWN, _rules.saboteur.enabled);
            SetAbilityWidgetsEnabled(CWIDX_HITMAN_COST, CWIDX_HITMAN_USESYEAR_DOWN, _rules.hitman.enabled);
            SetAbilityWidgetsEnabled(
                CWIDX_RESEARCH_COST, CWIDX_RESEARCH_USESYEAR_DOWN, _rules.researchSabotage.enabled);
            SetAbilityWidgetsEnabled(CWIDX_UNION_COST, CWIDX_UNION_USESYEAR_DOWN, _rules.unionDisruption.enabled);
            SetAbilityWidgetsEnabled(CWIDX_KARENS_COST, CWIDX_KARENS_USESYEAR_DOWN, _rules.karens.enabled);
            SetAbilityWidgetsEnabled(CWIDX_STONERS_COST, CWIDX_STONERS_USESYEAR_DOWN, _rules.stoners.enabled);
            // Misinformation is a fixed 4-week campaign at a fixed price.
            setWidgetDisabled(CWIDX_MISINFO_COST_UP, true);
            setWidgetDisabled(CWIDX_MISINFO_COST_DOWN, true);
            setWidgetDisabled(CWIDX_MISINFO_DURATION_UP, true);
            setWidgetDisabled(CWIDX_MISINFO_DURATION_DOWN, true);
            // Toilet bomber / saboteur / hitman act once then leave: no host-set victim time.
            for (const auto widget :
                 { CWIDX_TOILET_DURATION_UP, CWIDX_TOILET_DURATION_DOWN, CWIDX_SABOTEUR_DURATION_UP,
                   CWIDX_SABOTEUR_DURATION_DOWN, CWIDX_HITMAN_DURATION_UP, CWIDX_HITMAN_DURATION_DOWN,
                   CWIDX_KARENS_DURATION_UP, CWIDX_KARENS_DURATION_DOWN, CWIDX_STONERS_DURATION_UP,
                   CWIDX_STONERS_DURATION_DOWN })
                setWidgetDisabled(widget, true);

            UpdateCaptions();
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);
            auto colour = colours[1];
            if (page == 0)
            {
                DrawLabel(rt, 8, CWIDX_COMPETITION_NAME, "Competition name", colour);
                DrawLabel(rt, 8, CWIDX_PLAYER_NAME, "Your park / player name", colour);
                DrawLabel(rt, 8, CWIDX_PORT, "Competitive port", colour);
                DrawLabel(rt, 8, CWIDX_MAX_PLAYERS, "Maximum competing parks", colour);
                DrawLabel(rt, 8, CWIDX_VICTORY, "Victory condition", colour);
                DrawLabel(rt, 8, CWIDX_METRIC, "Ranking metric", colour);
                DrawLabel(rt, 8, CWIDX_DEADLINE, _rules.victoryMode == Competitive::VictoryMode::target ? "Fallback deadline year" : "Deadline year", colour);
                if (_rules.victoryMode == Competitive::VictoryMode::target)
                    DrawLabel(rt, 8, CWIDX_TARGET, "Target", colour);
                DrawLabel(rt, 8, CWIDX_MAX_SPEED, "Maximum game speed", colour);
                drawText(rt, windowPos + ScreenCoordsXY{ 8, 226 }, "Every competitor receives a fresh copy of the selected scenario, paused on its opening day.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 8, 241 }, "Each park then runs on its own clock. Cash is the scenario's actual park cash.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 8, 256 }, "Attack prices use park cash; no-money scenarios use cooldowns without a cash charge.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 8, 271 }, "Competition results use the rule above instead of the scenario objective.", { colour });
                drawTextWrapped(rt, windowPos + ScreenCoordsXY{ 8, 286 }, 600, _pacingHint, { colour });
            }
            else
            {
                drawText(rt, windowPos + ScreenCoordsXY{ 112, 51 }, "Cost", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 226, 51 }, "Minimum gap", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 322, 51 }, "Victim time", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 443, 51 }, "Effect strength", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 632, 51 }, "Uses per year", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10, 88 }, "Angry guest; security-blocked attempts also consume its quota.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,145 }, "Cancels future arrivals at the reverse strength of a half-price entry campaign.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,202 }, "Exact buyers at the selected food/drink stall receive the configured nausea chance.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{443,241 }, "All occupants", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,259 }, "Walks to a selected toilet, kills its current occupants, and destroys the building.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,316 }, "Guests passed once receive a rude-guest thought and the configured happiness penalty.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{443,355 }, "Forced breakdown", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,373 }, "Rides a selected attraction, forces a supported breakdown after exiting, then leaves.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{443,412 }, "One victim", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,430 }, "Photographs and kills one guest, applying the normal single-accident rating consequence.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,487 }, "Forces the rival's research funding to None for the duration, then restores their last setting.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,544 }, "Doubles the rival's staff wage bill for the duration (kept longer than a month so a full month is hit).", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,601 }, "A coach party of demanding guests: high needs, low mood, complain often, and hound staff to speak to a manager.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,658 }, "A coach party of easily-distracted guests: hungry, unbothered by prices, and forever stopping to stare at rides.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,681 }, "Groups leave on their own accord. Minimum gap = shortest wait between uses. Uses per year = hard cap per in-game year.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,696 }, "Victim time uses victim-local days. Failed delivery is refunded. No-money scenarios ignore action prices.", { colour });
            }
        }

    private:
        static constexpr size_t kMaxPlayerNameLength = 32;

        void SetPage(uint8_t newPage)
        {
            page = newPage;
            invalidate();
        }

        std::string BuildPacingHint() const
        {
            const auto speed = std::max<uint8_t>(1, _rules.maxGameSpeed);
            const double seconds = _rules.deadlineYear * Competitive::kRealSecondsPerInGameYear / speed;
            const int32_t totalMinutes = static_cast<int32_t>(seconds / 60.0);
            const int32_t hours = totalMinutes / 60;
            const int32_t minutes = totalMinutes % 60;
            std::string duration;
            if (hours > 0)
                duration = std::to_string(hours) + "h " + std::to_string(minutes) + "m";
            else
                duration = std::to_string(minutes) + "m";
            return "Reaching Year " + std::to_string(_rules.deadlineYear) + " takes at least ~" + duration
                + " of real time at the " + std::to_string(speed) + "x speed cap (longer with pauses and spectating).";
        }

        void DrawLabel(Drawing::RenderTarget& rt, int32_t x, WidgetIndex widget, const char* text, ColourWithFlags colour)
        {
            drawText(rt, windowPos + ScreenCoordsXY{ x, widgets[widget].top }, text, { colour });
        }

        void SetAbilityWidgetsEnabled(WidgetIndex first, WidgetIndex last, bool enabled)
        {
            for (WidgetIndex i = first; i <= last; i++)
                setWidgetDisabled(i, !enabled);
        }

        static const char* VictoryName(Competitive::VictoryMode mode)
        {
            return mode == Competitive::VictoryMode::deadline ? "Highest metric at each park's local deadline"
                                                               : "First park to target (deadline fallback)";
        }

        static const char* MetricName(Competitive::Metric metric)
        {
            switch (metric)
            {
                case Competitive::Metric::points:
                    return "Cumulative competitive points";
                case Competitive::Metric::rating:
                    return "Park rating";
                case Competitive::Metric::guests:
                    return "Guests in park";
                case Competitive::Metric::cash:
                    return "Cash";
                case Competitive::Metric::parkValue:
                    return "Park value";
            }
            return "Cumulative competitive points";
        }

        void ShowVictoryDropdown()
        {
            std::array<Dropdown::Item, 2> items = {
                Dropdown::MenuLabel(VictoryName(Competitive::VictoryMode::deadline)),
                Dropdown::MenuLabel(VictoryName(Competitive::VictoryMode::target)),
            };
            ShowDropdown(CWIDX_VICTORY_BUTTON, items);
        }

        void ShowMetricDropdown()
        {
            std::array<Dropdown::Item, 5> items = {
                Dropdown::MenuLabel(MetricName(Competitive::Metric::points)),
                Dropdown::MenuLabel(MetricName(Competitive::Metric::rating)),
                Dropdown::MenuLabel(MetricName(Competitive::Metric::guests)),
                Dropdown::MenuLabel(MetricName(Competitive::Metric::cash)),
                Dropdown::MenuLabel(MetricName(Competitive::Metric::parkValue)),
            };
            ShowDropdown(CWIDX_METRIC_BUTTON, items);
        }

        template<size_t TCount> void ShowDropdown(WidgetIndex widgetIndex, const std::array<Dropdown::Item, TCount>& items)
        {
            const auto& widget = widgets[widgetIndex];
            WindowDropdownShowTextCustomWidth(
                windowPos + ScreenCoordsXY{ widget.left, widget.top }, widget.height(), colours[1], 0, {}, items,
                widgets[widgetIndex - 1].width());
        }

        void AdjustTarget(int32_t direction)
        {
            int64_t step = 10000;
            int64_t minimum = 1000;
            int64_t maximum = 1000000000;
            switch (_rules.metric)
            {
                case Competitive::Metric::rating:
                    step = 50;
                    minimum = 100;
                    maximum = 999;
                    break;
                case Competitive::Metric::guests:
                    step = 100;
                    minimum = 100;
                    maximum = 1000000;
                    break;
                case Competitive::Metric::cash:
                case Competitive::Metric::parkValue:
                    step = ToMoney64FromGBP(10000);
                    minimum = ToMoney64FromGBP(1000);
                    maximum = ToMoney64FromGBP(100000000);
                    break;
                case Competitive::Metric::points:
                    break;
            }
            _rules.target = std::clamp(_rules.target + (step * direction), minimum, maximum);
        }

        void NormaliseTarget()
        {
            switch (_rules.metric)
            {
                case Competitive::Metric::rating:
                    _rules.target = 900;
                    break;
                case Competitive::Metric::guests:
                    _rules.target = 2000;
                    break;
                case Competitive::Metric::cash:
                    _rules.target = ToMoney64FromGBP(100000);
                    break;
                case Competitive::Metric::parkValue:
                    _rules.target = ToMoney64FromGBP(500000);
                    break;
                case Competitive::Metric::points:
                    _rules.target = 100000;
                    break;
            }
        }

        static void AdjustUsesPerYear(Competitive::AbilityRule& rule, int32_t direction)
        {
            rule.usesPerYear = static_cast<uint16_t>(std::clamp<int32_t>(rule.usesPerYear + direction, 1, 64));
        }

        bool AdjustAbilityRule(WidgetIndex widgetIndex)
        {
            Competitive::AbilityRule* rule = nullptr;
            WidgetIndex first{};
            if (widgetIndex >= CWIDX_VANDAL_COST && widgetIndex <= CWIDX_VANDAL_POTENCY_DOWN)
            {
                rule = &_rules.vandal;
                first = CWIDX_VANDAL_COST;
            }
            else if (widgetIndex >= CWIDX_MISINFO_COST && widgetIndex <= CWIDX_MISINFO_POTENCY_DOWN)
            {
                rule = &_rules.misinformation;
                first = CWIDX_MISINFO_COST;
            }
            else if (widgetIndex >= CWIDX_POISON_COST && widgetIndex <= CWIDX_POISON_POTENCY_DOWN)
            {
                rule = &_rules.poison;
                first = CWIDX_POISON_COST;
            }
            else if (widgetIndex >= CWIDX_TOILET_COST && widgetIndex <= CWIDX_TOILET_DURATION_DOWN)
            {
                rule = &_rules.toiletBomber;
                first = CWIDX_TOILET_COST;
            }
            else if (widgetIndex >= CWIDX_AGITATOR_COST && widgetIndex <= CWIDX_AGITATOR_POTENCY_DOWN)
            {
                rule = &_rules.agitator;
                first = CWIDX_AGITATOR_COST;
            }
            else if (widgetIndex >= CWIDX_SABOTEUR_COST && widgetIndex <= CWIDX_SABOTEUR_DURATION_DOWN)
            {
                rule = &_rules.saboteur;
                first = CWIDX_SABOTEUR_COST;
            }
            else if (widgetIndex >= CWIDX_HITMAN_COST && widgetIndex <= CWIDX_HITMAN_DURATION_DOWN)
            {
                rule = &_rules.hitman;
                first = CWIDX_HITMAN_COST;
            }
            else if (widgetIndex >= CWIDX_RESEARCH_COST && widgetIndex <= CWIDX_RESEARCH_DURATION_DOWN)
            {
                rule = &_rules.researchSabotage;
                first = CWIDX_RESEARCH_COST;
            }
            else if (widgetIndex >= CWIDX_UNION_COST && widgetIndex <= CWIDX_UNION_DURATION_DOWN)
            {
                rule = &_rules.unionDisruption;
                first = CWIDX_UNION_COST;
            }
            else if (widgetIndex >= CWIDX_KARENS_COST && widgetIndex <= CWIDX_KARENS_POTENCY_DOWN)
            {
                rule = &_rules.karens;
                first = CWIDX_KARENS_COST;
            }
            else if (widgetIndex >= CWIDX_STONERS_COST && widgetIndex <= CWIDX_STONERS_POTENCY_DOWN)
            {
                rule = &_rules.stoners;
                first = CWIDX_STONERS_COST;
            }
            if (rule == nullptr)
                return false;

            const auto offset = widgetIndex - first;
            const auto field = offset / 3;
            const bool increase = (offset % 3) == 1;
            const bool decrease = (offset % 3) == 2;
            if (!increase && !decrease)
                return true;
            // Misinformation is a fixed 4-week campaign at a fixed price: cost (0) and duration (2) are locked.
            if (rule == &_rules.misinformation && (field == 0 || field == 2))
                return true;
            // Toilet bomber / saboteur / hitman act once then leave; Karens / Stoners leave on their own
            // accord: their duration (2) is a hidden backstop, not a knob.
            if (field == 2
                && (rule == &_rules.toiletBomber || rule == &_rules.saboteur || rule == &_rules.hitman
                    || rule == &_rules.karens || rule == &_rules.stoners))
                return true;
            const int32_t direction = increase ? 1 : -1;
            switch (field)
            {
                case 0:
                    rule->cost = std::clamp<money64>(rule->cost + (direction * 50.00_GBP), 0.00_GBP, 100000.00_GBP);
                    break;
                case 1:
                    rule->cooldownDays = static_cast<uint16_t>(std::clamp<int32_t>(
                        rule->cooldownDays + (direction * static_cast<int32_t>(Competitive::kCompetitiveDaysPerMonth)), 0,
                        2048));
                    break;
                case 2:
                    rule->durationDays = static_cast<uint16_t>(std::clamp<int32_t>(rule->durationDays + direction, 1, 512));
                    break;
                case 3:
                {
                    const bool isGroup = rule == &_rules.karens || rule == &_rules.stoners;
                    const int32_t step = rule == &_rules.misinformation ? 20 : (isGroup ? 5 : 1);
                    const int32_t min = isGroup ? 5 : 1;
                    const int32_t max = rule == &_rules.misinformation ? 2000 : (isGroup ? 40 : 100);
                    rule->potency = static_cast<uint16_t>(
                        std::clamp<int32_t>(rule->potency + (direction * step), min, max));
                    break;
                }
            }
            return true;
        }

        // "Minimum gap" caption: the shortest wait between uses, in attacker-local days.
        static std::string CooldownCaption(uint16_t days)
        {
            if (days == 0)
                return "no minimum";
            const auto perYear = Competitive::kCompetitiveDaysPerYear;
            const auto perMonth = Competitive::kCompetitiveDaysPerMonth;
            std::string suffix;
            if (days % perYear == 0)
            {
                const auto years = days / perYear;
                suffix = years == 1 ? " (1 year)" : " (" + std::to_string(years) + " years)";
            }
            else if (days % perMonth == 0)
            {
                const auto months = days / perMonth;
                suffix = months == 1 ? " (1 month)" : " (" + std::to_string(months) + " months)";
            }
            return std::to_string(days) + " days" + suffix;
        }

        // "Uses per year" caption: the hard cap on uses per in-game year.
        static std::string UsesPerYearCaption(uint16_t usesPerYear)
        {
            if (usesPerYear == 0)
                return "unlimited";
            return std::to_string(usesPerYear) + " / year";
        }

        void UpdateCaptions()
        {
            _maxPlayersCaption = std::to_string(_rules.maxPlayers);
            _victoryCaption = VictoryName(_rules.victoryMode);
            _metricCaption = MetricName(_rules.metric);
            _deadlineCaption = "Year " + std::to_string(_rules.deadlineYear);
            if (_rules.metric == Competitive::Metric::cash || _rules.metric == Competitive::Metric::parkValue)
                _targetCaption = FormatStringID(STR_CURRENCY_FORMAT, static_cast<money64>(_rules.target));
            else
                _targetCaption = std::to_string(_rules.target);
            _maxSpeedCaption = std::to_string(_rules.maxGameSpeed) + "x";
            _pacingHint = BuildPacingHint();

            _vandalCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.vandal.cost);
            _vandalCooldownCaption = CooldownCaption(_rules.vandal.cooldownDays);
            _vandalDurationCaption = std::to_string(_rules.vandal.durationDays) + " days max";
            _vandalPotencyCaption = std::to_string(_rules.vandal.potency) + " attempts";
            _misinfoCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.misinformation.cost);
            _misinfoCooldownCaption = CooldownCaption(_rules.misinformation.cooldownDays);
            _misinfoDurationCaption = std::to_string(_rules.misinformation.durationDays / 7) + " weeks (fixed)";
            _misinfoPotencyCaption = std::to_string((_rules.misinformation.potency * 100) / 200) + "% half-price";
            _poisonCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.poison.cost);
            _poisonCooldownCaption = CooldownCaption(_rules.poison.cooldownDays);
            _poisonDurationCaption = std::to_string(_rules.poison.durationDays) + " days";
            _poisonPotencyCaption = std::to_string(_rules.poison.potency) + "% buyers";
            _toiletCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.toiletBomber.cost);
            _toiletCooldownCaption = CooldownCaption(_rules.toiletBomber.cooldownDays);
            _toiletDurationCaption = "acts once";
            _agitatorCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.agitator.cost);
            _agitatorCooldownCaption = CooldownCaption(_rules.agitator.cooldownDays);
            _agitatorDurationCaption = std::to_string(_rules.agitator.durationDays) + " days";
            _agitatorPotencyCaption = "-" + std::to_string(_rules.agitator.potency) + " happiness";
            _saboteurCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.saboteur.cost);
            _saboteurCooldownCaption = CooldownCaption(_rules.saboteur.cooldownDays);
            _saboteurDurationCaption = "acts once";
            _hitmanCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.hitman.cost);
            _hitmanCooldownCaption = CooldownCaption(_rules.hitman.cooldownDays);
            _hitmanDurationCaption = "acts once";
            _researchCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.researchSabotage.cost);
            _researchCooldownCaption = CooldownCaption(_rules.researchSabotage.cooldownDays);
            _researchDurationCaption = std::to_string(_rules.researchSabotage.durationDays) + " days";
            _researchUsesCaption = UsesPerYearCaption(_rules.researchSabotage.usesPerYear);
            _unionCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.unionDisruption.cost);
            _unionCooldownCaption = CooldownCaption(_rules.unionDisruption.cooldownDays);
            _unionDurationCaption = std::to_string(_rules.unionDisruption.durationDays) + " days";
            _unionUsesCaption = UsesPerYearCaption(_rules.unionDisruption.usesPerYear);
            _karensCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.karens.cost);
            _karensCooldownCaption = CooldownCaption(_rules.karens.cooldownDays);
            _karensPotencyCaption = std::to_string(_rules.karens.potency) + " guests";
            _karensUsesCaption = UsesPerYearCaption(_rules.karens.usesPerYear);
            _stonersCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.stoners.cost);
            _stonersCooldownCaption = CooldownCaption(_rules.stoners.cooldownDays);
            _stonersPotencyCaption = std::to_string(_rules.stoners.potency) + " guests";
            _stonersUsesCaption = UsesPerYearCaption(_rules.stoners.usesPerYear);
            _vandalUsesCaption = UsesPerYearCaption(_rules.vandal.usesPerYear);
            _misinfoUsesCaption = UsesPerYearCaption(_rules.misinformation.usesPerYear);
            _poisonUsesCaption = UsesPerYearCaption(_rules.poison.usesPerYear);
            _toiletUsesCaption = UsesPerYearCaption(_rules.toiletBomber.usesPerYear);
            _agitatorUsesCaption = UsesPerYearCaption(_rules.agitator.usesPerYear);
            _saboteurUsesCaption = UsesPerYearCaption(_rules.saboteur.usesPerYear);
            _hitmanUsesCaption = UsesPerYearCaption(_rules.hitman.usesPerYear);

            widgets[CWIDX_MAX_PLAYERS].setString(_maxPlayersCaption.c_str());
            widgets[CWIDX_VICTORY].setString(_victoryCaption.c_str());
            widgets[CWIDX_METRIC].setString(_metricCaption.c_str());
            widgets[CWIDX_DEADLINE].setString(_deadlineCaption.c_str());
            widgets[CWIDX_TARGET].setString(_targetCaption.c_str());
            widgets[CWIDX_MAX_SPEED].setString(_maxSpeedCaption.c_str());
            widgets[CWIDX_VANDAL_COST].setString(_vandalCostCaption.c_str());
            widgets[CWIDX_VANDAL_COOLDOWN].setString(_vandalCooldownCaption.c_str());
            widgets[CWIDX_VANDAL_DURATION].setString(_vandalDurationCaption.c_str());
            widgets[CWIDX_VANDAL_POTENCY].setString(_vandalPotencyCaption.c_str());
            widgets[CWIDX_MISINFO_COST].setString(_misinfoCostCaption.c_str());
            widgets[CWIDX_MISINFO_COOLDOWN].setString(_misinfoCooldownCaption.c_str());
            widgets[CWIDX_MISINFO_DURATION].setString(_misinfoDurationCaption.c_str());
            widgets[CWIDX_MISINFO_POTENCY].setString(_misinfoPotencyCaption.c_str());
            widgets[CWIDX_POISON_COST].setString(_poisonCostCaption.c_str());
            widgets[CWIDX_POISON_COOLDOWN].setString(_poisonCooldownCaption.c_str());
            widgets[CWIDX_POISON_DURATION].setString(_poisonDurationCaption.c_str());
            widgets[CWIDX_POISON_POTENCY].setString(_poisonPotencyCaption.c_str());
            widgets[CWIDX_TOILET_COST].setString(_toiletCostCaption.c_str());
            widgets[CWIDX_TOILET_COOLDOWN].setString(_toiletCooldownCaption.c_str());
            widgets[CWIDX_TOILET_DURATION].setString(_toiletDurationCaption.c_str());
            widgets[CWIDX_AGITATOR_COST].setString(_agitatorCostCaption.c_str());
            widgets[CWIDX_AGITATOR_COOLDOWN].setString(_agitatorCooldownCaption.c_str());
            widgets[CWIDX_AGITATOR_DURATION].setString(_agitatorDurationCaption.c_str());
            widgets[CWIDX_AGITATOR_POTENCY].setString(_agitatorPotencyCaption.c_str());
            widgets[CWIDX_SABOTEUR_COST].setString(_saboteurCostCaption.c_str());
            widgets[CWIDX_SABOTEUR_COOLDOWN].setString(_saboteurCooldownCaption.c_str());
            widgets[CWIDX_SABOTEUR_DURATION].setString(_saboteurDurationCaption.c_str());
            widgets[CWIDX_HITMAN_COST].setString(_hitmanCostCaption.c_str());
            widgets[CWIDX_HITMAN_COOLDOWN].setString(_hitmanCooldownCaption.c_str());
            widgets[CWIDX_HITMAN_DURATION].setString(_hitmanDurationCaption.c_str());
            widgets[CWIDX_VANDAL_USESYEAR].setString(_vandalUsesCaption.c_str());
            widgets[CWIDX_MISINFO_USESYEAR].setString(_misinfoUsesCaption.c_str());
            widgets[CWIDX_POISON_USESYEAR].setString(_poisonUsesCaption.c_str());
            widgets[CWIDX_TOILET_USESYEAR].setString(_toiletUsesCaption.c_str());
            widgets[CWIDX_AGITATOR_USESYEAR].setString(_agitatorUsesCaption.c_str());
            widgets[CWIDX_SABOTEUR_USESYEAR].setString(_saboteurUsesCaption.c_str());
            widgets[CWIDX_HITMAN_USESYEAR].setString(_hitmanUsesCaption.c_str());
            widgets[CWIDX_RESEARCH_COST].setString(_researchCostCaption.c_str());
            widgets[CWIDX_RESEARCH_COOLDOWN].setString(_researchCooldownCaption.c_str());
            widgets[CWIDX_RESEARCH_DURATION].setString(_researchDurationCaption.c_str());
            widgets[CWIDX_RESEARCH_USESYEAR].setString(_researchUsesCaption.c_str());
            widgets[CWIDX_UNION_COST].setString(_unionCostCaption.c_str());
            widgets[CWIDX_UNION_COOLDOWN].setString(_unionCooldownCaption.c_str());
            widgets[CWIDX_UNION_DURATION].setString(_unionDurationCaption.c_str());
            widgets[CWIDX_UNION_USESYEAR].setString(_unionUsesCaption.c_str());
            widgets[CWIDX_KARENS_COST].setString(_karensCostCaption.c_str());
            widgets[CWIDX_KARENS_COOLDOWN].setString(_karensCooldownCaption.c_str());
            widgets[CWIDX_KARENS_DURATION].setString(_groupDurationCaption.c_str());
            widgets[CWIDX_KARENS_POTENCY].setString(_karensPotencyCaption.c_str());
            widgets[CWIDX_KARENS_USESYEAR].setString(_karensUsesCaption.c_str());
            widgets[CWIDX_STONERS_COST].setString(_stonersCostCaption.c_str());
            widgets[CWIDX_STONERS_COOLDOWN].setString(_stonersCooldownCaption.c_str());
            widgets[CWIDX_STONERS_DURATION].setString(_groupDurationCaption.c_str());
            widgets[CWIDX_STONERS_POTENCY].setString(_stonersPotencyCaption.c_str());
            widgets[CWIDX_STONERS_USESYEAR].setString(_stonersUsesCaption.c_str());
        }

        void ChooseScenario()
        {
            if (_competitionName.empty() || _playerName.empty())
            {
                ErrorOpen("Cannot create competition", "Enter both a competition name and your park/player name.");
                return;
            }
            const auto parsedPort = String::tryParse<uint16_t>(_port);
            if (!parsedPort.has_value() || parsedPort.value() == 0)
            {
                ErrorOpen("Cannot create competition", "Enter a valid competitive network port.");
                return;
            }

            Competitive::HostConfiguration configuration;
            configuration.competitionName = _competitionName;
            configuration.playerName = _playerName;
            configuration.listenAddress = Config::Get().network.listenAddress;
            configuration.port = parsedPort.value();
            configuration.rules = _rules;
            Config::Get().network.serverName = _competitionName;
            Config::Save();

            ScenarioselectOpen([configuration = std::move(configuration)](std::string_view path) mutable {
                GameNotifyMapChange();
                if (!GetContext()->LoadParkFromFile(std::string(path), false, true))
                    return;
                if (!GameIsPaused())
                    PauseToggle();
                configuration.scenario = Competitive::GetCurrentScenarioIdentity();
                std::string error;
                if (!Competitive::GetSession().StartHost(configuration, error))
                {
                    ErrorOpen("Cannot create competition", error);
                }
                else
                {
                    ContextOpenWindow(WindowClass::multiplayer);
                }
            });
        }
    };

    WindowBase* ServerStartOpenCompetitive()
    {
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::serverStart);
        return windowMgr->Create<CompetitiveServerStartWindow>(
            WindowClass::serverStart, kCompetitiveWindowSize, WindowFlag::centreScreen);
    }
} // namespace OpenRCT2::Ui::Windows

#endif
