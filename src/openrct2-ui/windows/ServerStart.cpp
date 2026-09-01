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
        CWIDX_STARTING_CASH,
        CWIDX_STARTING_CASH_UP,
        CWIDX_STARTING_CASH_DOWN,
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
        CWIDX_CREATE_LOBBY,
    };

    static constexpr ScreenSize kCompetitiveWindowSize = { 620, 375 };

    // clang-format off
    static constexpr auto _competitiveServerStartWidgets = makeWidgets(
        makeWindowShim(STR_START_SERVER, kCompetitiveWindowSize),
        makeWidget({  3, 17}, {140, 26}, WidgetType::tab,          WindowColour::secondary, kStringIdEmpty),
        makeWidget({143, 17}, {140, 26}, WidgetType::tab,          WindowColour::secondary, kStringIdEmpty),
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
        makeHoldableSpinnerWidgets({170,208}, {160, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),

        makeWidget({  8, 68}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112, 68}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226, 68}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322, 68}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({443, 68}, {167, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,125}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,125}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,125}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,125}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({443,125}, {167, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({  8,182}, {100, 14}, WidgetType::checkbox, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({112,182}, {108, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({226,182}, { 90, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({322,182}, {115, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeHoldableSpinnerWidgets({443,182}, {167, 13}, WidgetType::spinner, WindowColour::secondary, kStringIdEmpty),
        makeWidget({430,350}, {180, 14}, WidgetType::button, WindowColour::secondary, kStringIdEmpty)
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
        std::string _maxPlayersCaption;
        std::string _victoryCaption;
        std::string _metricCaption;
        std::string _deadlineCaption;
        std::string _targetCaption;
        std::string _maxSpeedCaption;
        std::string _startingCashCaption;
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
                case CWIDX_STARTING_CASH_UP:
                    _rules.economy.startingCash = std::min<money64>(1000000.00_GBP, _rules.economy.startingCash + 1000.00_GBP);
                    break;
                case CWIDX_STARTING_CASH_DOWN:
                    _rules.economy.startingCash = std::max<money64>(0.00_GBP, _rules.economy.startingCash - 1000.00_GBP);
                    break;
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

            for (WidgetIndex i = CWIDX_COMPETITION_NAME; i <= CWIDX_STARTING_CASH_DOWN; i++)
                widgets[i].setVisible(matchPage);
            for (WidgetIndex i = CWIDX_VANDAL_ENABLED; i <= CWIDX_POISON_POTENCY_DOWN; i++)
                widgets[i].setVisible(!matchPage);

            const bool targetMode = _rules.victoryMode == Competitive::VictoryMode::target;
            for (WidgetIndex i = CWIDX_TARGET; i <= CWIDX_TARGET_DOWN; i++)
                widgets[i].setVisible(matchPage && targetMode);

            setCheckboxValue(CWIDX_ADVERTISE, Config::Get().network.advertise);
            setCheckboxValue(CWIDX_LATE_JOIN, _rules.allowLateJoin);
            setCheckboxValue(CWIDX_VANDAL_ENABLED, _rules.vandal.enabled);
            setCheckboxValue(CWIDX_MISINFO_ENABLED, _rules.misinformation.enabled);
            setCheckboxValue(CWIDX_POISON_ENABLED, _rules.poison.enabled);

            SetAbilityWidgetsEnabled(CWIDX_VANDAL_COST, CWIDX_VANDAL_POTENCY_DOWN, _rules.vandal.enabled);
            SetAbilityWidgetsEnabled(CWIDX_MISINFO_COST, CWIDX_MISINFO_POTENCY_DOWN, _rules.misinformation.enabled);
            SetAbilityWidgetsEnabled(CWIDX_POISON_COST, CWIDX_POISON_POTENCY_DOWN, _rules.poison.enabled);

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
                DrawLabel(rt, 8, CWIDX_STARTING_CASH, "Starting competitive cash", colour);

                drawText(rt, windowPos + ScreenCoordsXY{ 8, 241 }, "Every competitor receives a fresh copy of the selected scenario, paused on its opening day.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 8, 256 }, "Each park then runs on its own clock. Normal guest generation and scenario finances remain local.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 8, 271 }, "Competition results use the rule above instead of the scenario objective.", { colour });
            }
            else
            {
                drawText(rt, windowPos + ScreenCoordsXY{ 112, 51 }, "Cost", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 226, 51 }, "Cooldown", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 322, 51 }, "Victim time", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 443, 51 }, "Effect strength", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10, 88 }, "Angry guest; normal security applies. Ends after quota or maximum lifetime.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,145 }, "Cancels future arrivals at the reverse strength of a half-price entry campaign.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,202 }, "Exact buyers at the selected food/drink stall receive the configured nausea chance.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,231 }, "Cooldown follows the attacker's local year; duration follows the victim's local calendar.", { colour });
                drawText(rt, windowPos + ScreenCoordsXY{ 10,246 }, "Only online, unfinished rivals can be targeted. Failed delivery costs nothing and starts no cooldown.", { colour });
            }
        }

    private:
        static constexpr size_t kMaxPlayerNameLength = 32;

        void SetPage(uint8_t newPage)
        {
            page = newPage;
            invalidate();
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
                case Competitive::Metric::competitiveCash:
                    return "Competitive cash";
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
                Dropdown::MenuLabel(MetricName(Competitive::Metric::competitiveCash)),
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
                case Competitive::Metric::competitiveCash:
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
                case Competitive::Metric::competitiveCash:
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
            if (rule == nullptr)
                return false;

            const auto offset = widgetIndex - first;
            const auto field = offset / 3;
            const bool increase = (offset % 3) == 1;
            const bool decrease = (offset % 3) == 2;
            if (!increase && !decrease)
                return true;
            const int32_t direction = increase ? 1 : -1;
            switch (field)
            {
                case 0:
                    rule->cost = std::clamp<money64>(rule->cost + (direction * 100.00_GBP), 0.00_GBP, 100000.00_GBP);
                    break;
                case 1:
                    rule->cooldownYears = static_cast<uint8_t>(std::clamp<int32_t>(rule->cooldownYears + direction, 0, 20));
                    break;
                case 2:
                    rule->durationDays = static_cast<uint16_t>(std::clamp<int32_t>(rule->durationDays + direction, 1, 512));
                    break;
                case 3:
                {
                    const int32_t step = rule == &_rules.misinformation ? 20 : 1;
                    const int32_t max = rule == &_rules.misinformation ? 2000 : 100;
                    rule->potency = static_cast<uint16_t>(std::clamp<int32_t>(rule->potency + (direction * step), 1, max));
                    break;
                }
            }
            return true;
        }

        static std::string YearsCaption(uint8_t years)
        {
            return std::to_string(years) + (years == 1 ? " year" : " years");
        }

        void UpdateCaptions()
        {
            _maxPlayersCaption = std::to_string(_rules.maxPlayers);
            _victoryCaption = VictoryName(_rules.victoryMode);
            _metricCaption = MetricName(_rules.metric);
            _deadlineCaption = "Year " + std::to_string(_rules.deadlineYear);
            if (_rules.metric == Competitive::Metric::competitiveCash || _rules.metric == Competitive::Metric::parkValue)
                _targetCaption = FormatStringID(STR_CURRENCY_FORMAT, static_cast<money64>(_rules.target));
            else
                _targetCaption = std::to_string(_rules.target);
            _maxSpeedCaption = std::to_string(_rules.maxGameSpeed) + "x";
            _startingCashCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.economy.startingCash);

            _vandalCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.vandal.cost);
            _vandalCooldownCaption = YearsCaption(_rules.vandal.cooldownYears);
            _vandalDurationCaption = std::to_string(_rules.vandal.durationDays) + " days max";
            _vandalPotencyCaption = std::to_string(_rules.vandal.potency) + " items";
            _misinfoCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.misinformation.cost);
            _misinfoCooldownCaption = YearsCaption(_rules.misinformation.cooldownYears);
            _misinfoDurationCaption = std::to_string(_rules.misinformation.durationDays) + " days";
            _misinfoPotencyCaption = std::to_string((_rules.misinformation.potency * 100) / 200) + "% half-price";
            _poisonCostCaption = FormatStringID(STR_CURRENCY_FORMAT, _rules.poison.cost);
            _poisonCooldownCaption = YearsCaption(_rules.poison.cooldownYears);
            _poisonDurationCaption = std::to_string(_rules.poison.durationDays) + " days";
            _poisonPotencyCaption = std::to_string(_rules.poison.potency) + "% buyers";

            widgets[CWIDX_MAX_PLAYERS].setString(_maxPlayersCaption.c_str());
            widgets[CWIDX_VICTORY].setString(_victoryCaption.c_str());
            widgets[CWIDX_METRIC].setString(_metricCaption.c_str());
            widgets[CWIDX_DEADLINE].setString(_deadlineCaption.c_str());
            widgets[CWIDX_TARGET].setString(_targetCaption.c_str());
            widgets[CWIDX_MAX_SPEED].setString(_maxSpeedCaption.c_str());
            widgets[CWIDX_STARTING_CASH].setString(_startingCashCaption.c_str());
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
