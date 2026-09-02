/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cassert>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/GameState.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/GameActionRunner.h>
#include <openrct2/actions/network/NetworkModifyGroupAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/competitive/CompetitiveSession.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.String.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/RenderTarget.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/interface/ColourWithFlags.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/network/Network.h>
#include <openrct2/ui/WindowManager.h>

#include <algorithm>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WINDOW_MULTIPLAYER_PAGE_INFORMATION,
        WINDOW_MULTIPLAYER_PAGE_PLAYERS,
        WINDOW_MULTIPLAYER_PAGE_GROUPS,
        WINDOW_MULTIPLAYER_PAGE_OPTIONS,
        WINDOW_MULTIPLAYER_PAGE_COMPETITION,
    };

    enum WindowMultiplayerWidgetIdx : WidgetIndex
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CONTENT_PANEL,
        WIDX_TAB1,
        WIDX_TAB2,
        WIDX_TAB3,
        WIDX_TAB4,
        WIDX_TAB5,

        WIDX_HEADER_PLAYER = 9,
        WIDX_HEADER_GROUP,
        WIDX_HEADER_LAST_ACTION,
        WIDX_HEADER_PING,
        WIDX_LIST,

        WIDX_DEFAULT_GROUP = 9,
        WIDX_DEFAULT_GROUP_DROPDOWN,
        WIDX_ADD_GROUP,
        WIDX_REMOVE_GROUP,
        WIDX_RENAME_GROUP,
        WIDX_SELECTED_GROUP,
        WIDX_SELECTED_GROUP_DROPDOWN,
        WIDX_PERMISSIONS_LIST,

        WIDX_LOG_CHAT_CHECKBOX = 9,
        WIDX_LOG_SERVER_ACTIONS_CHECKBOX,
        WIDX_KNOWN_KEYS_ONLY_CHECKBOX,

        WIDX_COMP_PLACE = 9,
        WIDX_COMP_PARK,
        WIDX_COMP_STATUS,
        WIDX_COMP_YEAR,
        WIDX_COMP_SCORE,
        WIDX_COMP_LIST,
        WIDX_COMP_READY,
        WIDX_COMP_START,
        WIDX_COMP_ACTIONS,
        WIDX_COMP_WATCH,
        WIDX_COMP_HOST_CONTROLS,
        WIDX_COMP_LEAVE,
    };

    static constexpr ScreenSize kWindowSize = { 340, 240 };
    // clang-format off

    static constexpr auto kMainMultiplayerWidgets = makeWidgets(
        makeWindowShim(kStringIdNone, kWindowSize),
        makeWidget({  0, 43}, {340, 197}, WidgetType::resize, WindowColour::secondary                          ),
        makeTab   ({  3, 17},                                                          STR_SHOW_SERVER_INFO_TIP),
        makeTab   ({ 34, 17},                                                          STR_PLAYERS_TIP         ),
        makeTab   ({ 65, 17},                                                          STR_GROUPS_TIP          ),
        makeTab   ({ 96, 17},                                                          STR_OPTIONS_TIP         ),
        makeTab   ({127, 17},                                                          kStringIdNone            )
    );

    static constexpr auto window_multiplayer_information_widgets = makeWidgets(
        kMainMultiplayerWidgets
    );

    static constexpr auto window_multiplayer_players_widgets = makeWidgets(
        kMainMultiplayerWidgets,
        makeWidget({  3, 46}, {173,  15}, WidgetType::tableHeader, WindowColour::primary  , STR_PLAYER     ), // Player name
        makeWidget({176, 46}, { 83,  15}, WidgetType::tableHeader, WindowColour::primary  , STR_GROUP      ), // Player name
        makeWidget({259, 46}, {100,  15}, WidgetType::tableHeader, WindowColour::primary  , STR_LAST_ACTION), // Player name
        makeWidget({359, 46}, { 42,  15}, WidgetType::tableHeader, WindowColour::primary  , STR_PING       ), // Player name
        makeWidget({  3, 60}, {334, 177}, WidgetType::scroll,      WindowColour::secondary, SCROLL_VERTICAL) // list
    );

    static constexpr auto window_multiplayer_groups_widgets = makeWidgets(
        kMainMultiplayerWidgets,
        makeWidget({141, 46}, {175,  12}, WidgetType::dropdownMenu, WindowColour::secondary                    ), // default group
        makeWidget({305, 47}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH),
        makeWidget({ 11, 65}, { 92,  12}, WidgetType::button,       WindowColour::secondary, STR_ADD_GROUP     ), // add group button
        makeWidget({113, 65}, { 92,  12}, WidgetType::button,       WindowColour::secondary, STR_REMOVE_GROUP  ), // remove group button
        makeWidget({215, 65}, { 92,  12}, WidgetType::button,       WindowColour::secondary, STR_RENAME_GROUP  ), // rename group button
        makeWidget({ 72, 80}, {175,  12}, WidgetType::dropdownMenu, WindowColour::secondary                    ), // selected group
        makeWidget({236, 81}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH),
        makeWidget({  3, 94}, {314, 207}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL   ) // permissions list
    );

    static constexpr auto window_multiplayer_options_widgets = makeWidgets(
        kMainMultiplayerWidgets,
        makeWidget({3, 50}, {295, 12}, WidgetType::checkbox, WindowColour::secondary, STR_LOG_CHAT,              STR_LOG_CHAT_TIP             ),
        makeWidget({3, 64}, {295, 12}, WidgetType::checkbox, WindowColour::secondary, STR_LOG_SERVER_ACTIONS,    STR_LOG_SERVER_ACTIONS_TIP   ),
        makeWidget({3, 78}, {295, 12}, WidgetType::checkbox, WindowColour::secondary, STR_ALLOW_KNOWN_KEYS_ONLY, STR_ALLOW_KNOWN_KEYS_ONLY_TIP)
    );

    static constexpr auto window_multiplayer_competition_widgets = makeWidgets(
        kMainMultiplayerWidgets,
        makeWidget({  3, 47}, { 40, 15}, WidgetType::tableHeader, WindowColour::primary, kStringIdEmpty),
        makeWidget({ 43, 47}, {220, 15}, WidgetType::tableHeader, WindowColour::primary, kStringIdEmpty),
        makeWidget({263, 47}, {100, 15}, WidgetType::tableHeader, WindowColour::primary, kStringIdEmpty),
        makeWidget({363, 47}, { 60, 15}, WidgetType::tableHeader, WindowColour::primary, kStringIdEmpty),
        makeWidget({423, 47}, {194, 15}, WidgetType::tableHeader, WindowColour::primary, kStringIdEmpty),
        makeWidget({  3, 61}, {614,210}, WidgetType::scroll,      WindowColour::secondary, SCROLL_VERTICAL),
        makeWidget({  6,292}, { 90, 14}, WidgetType::button,      WindowColour::secondary, kStringIdEmpty),
        makeWidget({101,292}, { 90, 14}, WidgetType::button,      WindowColour::secondary, kStringIdEmpty),
        makeWidget({196,292}, {100, 14}, WidgetType::button,      WindowColour::secondary, kStringIdEmpty),
        makeWidget({301,292}, {100, 14}, WidgetType::button,      WindowColour::secondary, kStringIdEmpty),
        makeWidget({406,292}, {105, 14}, WidgetType::button,      WindowColour::secondary, kStringIdEmpty),
        makeWidget({516,292}, { 98, 14}, WidgetType::button,      WindowColour::secondary, kStringIdEmpty)
    );

    static std::span<const Widget> window_multiplayer_page_widgets[] = {
        window_multiplayer_information_widgets,
        window_multiplayer_players_widgets,
        window_multiplayer_groups_widgets,
        window_multiplayer_options_widgets,
        window_multiplayer_competition_widgets,
    };

    static constexpr StringId WindowMultiplayerPageTitles[] = {
        STR_MULTIPLAYER_INFORMATION_TITLE,
        STR_MULTIPLAYER_PLAYERS_TITLE,
        STR_MULTIPLAYER_GROUPS_TITLE,
        STR_MULTIPLAYER_OPTIONS_TITLE,
        kStringIdNone,
    };

    // clang-format on

    static constexpr int32_t window_multiplayer_animation_divisor[] = {
        4,
        4,
        2,
        2,
        1,
    };
    static constexpr int32_t window_multiplayer_animation_frames[] = {
        8,
        8,
        7,
        4,
        1,
    };

    static bool IsServerPlayerInvisible()
    {
        return Network::IsServerPlayerInvisible() && !Config::Get().general.debuggingTools;
    }

    static const Competitive::AbilityRule& GetCompetitiveAbilityRule(
        const Competitive::MatchRules& rules, Competitive::Ability ability)
    {
        switch (ability)
        {
            case Competitive::Ability::vandal:
                return rules.vandal;
            case Competitive::Ability::misinformation:
                return rules.misinformation;
            case Competitive::Ability::poison:
                return rules.poison;
            case Competitive::Ability::toiletBomber:
                return rules.toiletBomber;
            case Competitive::Ability::agitator:
                return rules.agitator;
            case Competitive::Ability::saboteur:
                return rules.saboteur;
            case Competitive::Ability::hitman:
                return rules.hitman;
        }
        return rules.vandal;
    }

    enum CompetitiveActionsWidgetIdx : WidgetIndex
    {
        CAWIDX_BACKGROUND,
        CAWIDX_TITLE,
        CAWIDX_CLOSE,
        CAWIDX_VANDAL,
        CAWIDX_MISINFORMATION,
        CAWIDX_AGITATOR,
        CAWIDX_HITMAN,
        CAWIDX_STALL,
        CAWIDX_STALL_DROPDOWN,
        CAWIDX_POISON,
        CAWIDX_TOILET,
        CAWIDX_TOILET_DROPDOWN,
        CAWIDX_TOILET_BOMBER,
        CAWIDX_RIDE,
        CAWIDX_RIDE_DROPDOWN,
        CAWIDX_SABOTEUR,
        CAWIDX_CANCEL,
    };

    static constexpr ScreenSize kCompetitiveActionsWindowSize = { 620, 455 };
    static constexpr auto kCompetitiveActionsWidgets = makeWidgets(
        makeWindowShim(kStringIdNone, kCompetitiveActionsWindowSize),
        makeWidget({ 12, 47 }, { 190, 16 }, WidgetType::button, WindowColour::secondary, kStringIdEmpty),
        makeWidget({ 12, 87 }, { 190, 16 }, WidgetType::button, WindowColour::secondary, kStringIdEmpty),
        makeWidget({ 12, 127 }, { 190, 16 }, WidgetType::button, WindowColour::secondary, kStringIdEmpty),
        makeWidget({ 12, 167 }, { 190, 16 }, WidgetType::button, WindowColour::secondary, kStringIdEmpty),
        makeWidget({112, 213 }, {493, 14 }, WidgetType::dropdownMenu, WindowColour::secondary, kStringIdEmpty),
        makeWidget({593, 214 }, { 11, 12 }, WidgetType::button, WindowColour::secondary, STR_DROPDOWN_GLYPH),
        makeWidget({ 12, 233 }, {190, 16 }, WidgetType::button, WindowColour::secondary, kStringIdEmpty),
        makeWidget({112, 278 }, {493, 14 }, WidgetType::dropdownMenu, WindowColour::secondary, kStringIdEmpty),
        makeWidget({593, 279 }, { 11, 12 }, WidgetType::button, WindowColour::secondary, STR_DROPDOWN_GLYPH),
        makeWidget({ 12, 298 }, {190, 16 }, WidgetType::button, WindowColour::secondary, kStringIdEmpty),
        makeWidget({112, 343 }, {493, 14 }, WidgetType::dropdownMenu, WindowColour::secondary, kStringIdEmpty),
        makeWidget({593, 344 }, { 11, 12 }, WidgetType::button, WindowColour::secondary, STR_DROPDOWN_GLYPH),
        makeWidget({ 12, 363 }, {190, 16 }, WidgetType::button, WindowColour::secondary, kStringIdEmpty),
        makeWidget({508, 430 }, {100, 14 }, WidgetType::button, WindowColour::secondary, STR_SAVE_PROMPT_CANCEL));

    class CompetitiveActionsWindow final : public Window
    {
    private:
        Competitive::ParticipantId _targetId = Competitive::kInvalidParticipantId;
        std::vector<Competitive::ParkMetrics::TargetRide> _stalls;
        std::vector<Competitive::ParkMetrics::TargetRide> _toilets;
        std::vector<Competitive::ParkMetrics::TargetRide> _rides;
        int32_t _selectedStall = -1;
        int32_t _selectedToilet = -1;
        int32_t _selectedRide = -1;
        std::string _title;
        std::string _vandalLabel;
        std::string _misinformationLabel;
        std::string _agitatorLabel;
        std::string _hitmanLabel;
        std::string _poisonLabel;
        std::string _stallLabel;
        std::string _toiletBomberLabel;
        std::string _toiletLabel;
        std::string _saboteurLabel;
        std::string _rideLabel;
        std::string _vandalProblem;
        std::string _misinformationProblem;
        std::string _agitatorProblem;
        std::string _hitmanProblem;
        std::string _poisonProblem;
        std::string _toiletBomberProblem;
        std::string _saboteurProblem;

        std::string GetProblem(Competitive::Ability ability, int32_t targetRideId) const
        {
            const auto& session = Competitive::GetSession();
            const auto* state = session.GetState();
            const auto* local = session.GetLocalParticipant();
            const auto* target = state == nullptr ? nullptr : Competitive::FindParticipant(*state, _targetId);
            if (state == nullptr || state->phase != Competitive::Phase::running || local == nullptr
                || local->role == Competitive::Role::spectator || local->finished || local->forfeited)
                return "Rival actions are available only while your park is competing.";
            if (target == nullptr || target->id == local->id || !Competitive::CanTarget(*target))
                return "This rival is offline or has already finished.";

            const auto& rule = GetCompetitiveAbilityRule(state->rules, ability);
            if (!rule.enabled)
                return "Disabled in this competition's rules.";
            if (!state->scenario.noMoney && session.GetAvailableParkCash() < rule.cost)
                return "Your park does not have enough cash.";

            const auto* localReport = Competitive::FindReport(*state, local->id);
            const auto cooldown = std::find_if(state->cooldowns.begin(), state->cooldowns.end(), [&](const auto& value) {
                return value.participantId == local->id && value.ability == ability;
            });
            if (localReport != nullptr && cooldown != state->cooldowns.end()
                && localReport->metrics.localDay < cooldown->availableAtDay)
                return "On cooldown until your local day " + std::to_string(cooldown->availableAtDay) + ".";

            const auto duplicate = std::find_if(state->effects.begin(), state->effects.end(), [&](const auto& effect) {
                if (effect.targetId != _targetId || effect.ability != ability)
                    return false;
                if (ability == Competitive::Ability::poison || ability == Competitive::Ability::toiletBomber
                    || ability == Competitive::Ability::saboteur)
                    return effect.targetRideId == targetRideId;
                return ability != Competitive::Ability::vandal;
            });
            if (duplicate != state->effects.end())
                return "Already active until rival local day " + std::to_string(duplicate->endsAtDay) + ".";
            if (ability == Competitive::Ability::poison && targetRideId < 0)
                return _stalls.empty() ? "This rival has no open food or drink stall." : "Choose a target stall first.";
            if (ability == Competitive::Ability::toiletBomber && targetRideId < 0)
                return _toilets.empty() ? "This rival has no open toilet." : "Choose a target toilet first.";
            if (ability == Competitive::Ability::saboteur && targetRideId < 0)
                return _rides.empty() ? "This rival has no open ride that can break down." : "Choose a target ride first.";
            if (ability == Competitive::Ability::hitman)
            {
                const auto* report = Competitive::FindReport(*state, _targetId);
                if (report == nullptr || report->metrics.guests == 0)
                    return "This rival has no guest for the hitman to target.";
            }
            return {};
        }

        void SendAbility(Competitive::Ability ability, int32_t targetRideId)
        {
            std::string error;
            if (!Competitive::GetSession().UseAbility(ability, _targetId, targetRideId, error))
            {
                ErrorOpen("Rival action failed", error);
                return;
            }
            close();
        }

        void ShowTargetDropdown(
            WidgetIndex widgetIndex, const std::vector<Competitive::ParkMetrics::TargetRide>& targets,
            int32_t selectedTarget)
        {
            if (targets.empty())
                return;
            const auto& widget = widgets[widgetIndex];
            WindowDropdownShowTextCustomWidth(
                windowPos + ScreenCoordsXY{ widget.left, widget.top }, widget.height(), colours[1], 0,
                { Dropdown::Flag::autoClose }, static_cast<int32_t>(targets.size()), widget.width());
            for (size_t index = 0; index < targets.size(); index++)
            {
                gDropdown.items[index] = Dropdown::MenuLabel(targets[index].name.c_str());
                gDropdown.items[index].setChecked(static_cast<int32_t>(index) == selectedTarget);
            }
        }

    public:
        void SetTarget(Competitive::ParticipantId targetId)
        {
            _targetId = targetId;
            const auto* state = Competitive::GetSession().GetState();
            const auto* report = state == nullptr ? nullptr : Competitive::FindReport(*state, targetId);
            _stalls = report == nullptr ? std::vector<Competitive::ParkMetrics::TargetRide>{}
                                        : report->metrics.openFoodDrinkStalls;
            _toilets = report == nullptr ? std::vector<Competitive::ParkMetrics::TargetRide>{}
                                         : report->metrics.openToilets;
            _rides = report == nullptr ? std::vector<Competitive::ParkMetrics::TargetRide>{}
                                       : report->metrics.openRides;
            _selectedStall = _stalls.empty() ? -1 : 0;
            _selectedToilet = _toilets.empty() ? -1 : 0;
            _selectedRide = _rides.empty() ? -1 : 0;
            invalidate();
        }

        void onOpen() override
        {
            setWidgets(kCompetitiveActionsWidgets);
            WindowInitScrollWidgets(*this);
        }

        void onPrepareDraw() override
        {
            const auto& session = Competitive::GetSession();
            const auto* state = session.GetState();
            const auto* target = state == nullptr ? nullptr : Competitive::FindParticipant(*state, _targetId);
            _title = target == nullptr ? "Rival actions" : "Rival actions — " + target->name;
            widgets[CAWIDX_TITLE].setString(_title.c_str());

            const auto unavailableRule = Competitive::AbilityRule{};
            const auto& vandal = state == nullptr ? unavailableRule : state->rules.vandal;
            const auto& misinformation = state == nullptr ? unavailableRule : state->rules.misinformation;
            const auto& poison = state == nullptr ? unavailableRule : state->rules.poison;
            const auto& toiletBomber = state == nullptr ? unavailableRule : state->rules.toiletBomber;
            const auto& agitator = state == nullptr ? unavailableRule : state->rules.agitator;
            const auto& saboteur = state == nullptr ? unavailableRule : state->rules.saboteur;
            const auto& hitman = state == nullptr ? unavailableRule : state->rules.hitman;
            const auto PriceLabel = [state](money64 cost) {
                return state != nullptr && state->scenario.noMoney
                    ? std::string(" — no cash charge")
                    : " — " + FormatStringID(STR_CURRENCY_FORMAT, cost);
            };
            _vandalLabel = "Send vandal" + PriceLabel(vandal.cost);
            _misinformationLabel = "Run misinformation" + PriceLabel(misinformation.cost);
            _agitatorLabel = "Send agitator" + PriceLabel(agitator.cost);
            _hitmanLabel = "Send hitman" + PriceLabel(hitman.cost);
            _poisonLabel = "Poison selected stall" + PriceLabel(poison.cost);
            _toiletBomberLabel = "Bomb selected toilet" + PriceLabel(toiletBomber.cost);
            _saboteurLabel = "Sabotage selected ride" + PriceLabel(saboteur.cost);
            _stallLabel = _selectedStall >= 0 && _selectedStall < static_cast<int32_t>(_stalls.size())
                ? _stalls[_selectedStall].name
                : (_stalls.empty() ? "No open food or drink stalls reported" : "Choose a stall");
            _toiletLabel = _selectedToilet >= 0 && _selectedToilet < static_cast<int32_t>(_toilets.size())
                ? _toilets[_selectedToilet].name
                : (_toilets.empty() ? "No open toilets reported" : "Choose a toilet");
            _rideLabel = _selectedRide >= 0 && _selectedRide < static_cast<int32_t>(_rides.size())
                ? _rides[_selectedRide].name
                : (_rides.empty() ? "No open breakdown-capable rides reported" : "Choose a ride");
            widgets[CAWIDX_VANDAL].setString(_vandalLabel.c_str());
            widgets[CAWIDX_MISINFORMATION].setString(_misinformationLabel.c_str());
            widgets[CAWIDX_AGITATOR].setString(_agitatorLabel.c_str());
            widgets[CAWIDX_HITMAN].setString(_hitmanLabel.c_str());
            widgets[CAWIDX_POISON].setString(_poisonLabel.c_str());
            widgets[CAWIDX_STALL].setString(_stallLabel.c_str());
            widgets[CAWIDX_TOILET_BOMBER].setString(_toiletBomberLabel.c_str());
            widgets[CAWIDX_TOILET].setString(_toiletLabel.c_str());
            widgets[CAWIDX_SABOTEUR].setString(_saboteurLabel.c_str());
            widgets[CAWIDX_RIDE].setString(_rideLabel.c_str());

            const auto targetRideId = _selectedStall >= 0 && _selectedStall < static_cast<int32_t>(_stalls.size())
                ? _stalls[_selectedStall].rideId
                : -1;
            _vandalProblem = GetProblem(Competitive::Ability::vandal, -1);
            _misinformationProblem = GetProblem(Competitive::Ability::misinformation, -1);
            _agitatorProblem = GetProblem(Competitive::Ability::agitator, -1);
            _hitmanProblem = GetProblem(Competitive::Ability::hitman, -1);
            _poisonProblem = GetProblem(Competitive::Ability::poison, targetRideId);
            const auto targetToiletId = _selectedToilet >= 0 && _selectedToilet < static_cast<int32_t>(_toilets.size())
                ? _toilets[_selectedToilet].rideId
                : -1;
            const auto targetSabotageRideId = _selectedRide >= 0 && _selectedRide < static_cast<int32_t>(_rides.size())
                ? _rides[_selectedRide].rideId
                : -1;
            _toiletBomberProblem = GetProblem(Competitive::Ability::toiletBomber, targetToiletId);
            _saboteurProblem = GetProblem(Competitive::Ability::saboteur, targetSabotageRideId);
            setWidgetDisabled(CAWIDX_VANDAL, !_vandalProblem.empty());
            setWidgetDisabled(CAWIDX_MISINFORMATION, !_misinformationProblem.empty());
            setWidgetDisabled(CAWIDX_AGITATOR, !_agitatorProblem.empty());
            setWidgetDisabled(CAWIDX_HITMAN, !_hitmanProblem.empty());
            setWidgetDisabled(CAWIDX_STALL, _stalls.empty());
            setWidgetDisabled(CAWIDX_STALL_DROPDOWN, _stalls.empty());
            setWidgetDisabled(CAWIDX_POISON, !_poisonProblem.empty());
            setWidgetDisabled(CAWIDX_TOILET, _toilets.empty());
            setWidgetDisabled(CAWIDX_TOILET_DROPDOWN, _toilets.empty());
            setWidgetDisabled(CAWIDX_TOILET_BOMBER, !_toiletBomberProblem.empty());
            setWidgetDisabled(CAWIDX_RIDE, _rides.empty());
            setWidgetDisabled(CAWIDX_RIDE_DROPDOWN, _rides.empty());
            setWidgetDisabled(CAWIDX_SABOTEUR, !_saboteurProblem.empty());
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case CAWIDX_CLOSE:
                case CAWIDX_CANCEL:
                    close();
                    break;
                case CAWIDX_VANDAL:
                    SendAbility(Competitive::Ability::vandal, -1);
                    break;
                case CAWIDX_MISINFORMATION:
                    SendAbility(Competitive::Ability::misinformation, -1);
                    break;
                case CAWIDX_AGITATOR:
                    SendAbility(Competitive::Ability::agitator, -1);
                    break;
                case CAWIDX_HITMAN:
                    SendAbility(Competitive::Ability::hitman, -1);
                    break;
                case CAWIDX_POISON:
                    if (_selectedStall >= 0 && _selectedStall < static_cast<int32_t>(_stalls.size()))
                        SendAbility(Competitive::Ability::poison, _stalls[_selectedStall].rideId);
                    break;
                case CAWIDX_TOILET_BOMBER:
                    if (_selectedToilet >= 0 && _selectedToilet < static_cast<int32_t>(_toilets.size()))
                        SendAbility(Competitive::Ability::toiletBomber, _toilets[_selectedToilet].rideId);
                    break;
                case CAWIDX_SABOTEUR:
                    if (_selectedRide >= 0 && _selectedRide < static_cast<int32_t>(_rides.size()))
                        SendAbility(Competitive::Ability::saboteur, _rides[_selectedRide].rideId);
                    break;
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            if (widgetIndex == CAWIDX_STALL || widgetIndex == CAWIDX_STALL_DROPDOWN)
                ShowTargetDropdown(CAWIDX_STALL, _stalls, _selectedStall);
            else if (widgetIndex == CAWIDX_TOILET || widgetIndex == CAWIDX_TOILET_DROPDOWN)
                ShowTargetDropdown(CAWIDX_TOILET, _toilets, _selectedToilet);
            else if (widgetIndex == CAWIDX_RIDE || widgetIndex == CAWIDX_RIDE_DROPDOWN)
                ShowTargetDropdown(CAWIDX_RIDE, _rides, _selectedRide);
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if ((widgetIndex == CAWIDX_STALL || widgetIndex == CAWIDX_STALL_DROPDOWN) && selectedIndex >= 0
                && selectedIndex < static_cast<int32_t>(_stalls.size()))
            {
                _selectedStall = selectedIndex;
                invalidate();
            }
            else if ((widgetIndex == CAWIDX_TOILET || widgetIndex == CAWIDX_TOILET_DROPDOWN) && selectedIndex >= 0
                && selectedIndex < static_cast<int32_t>(_toilets.size()))
            {
                _selectedToilet = selectedIndex;
                invalidate();
            }
            else if ((widgetIndex == CAWIDX_RIDE || widgetIndex == CAWIDX_RIDE_DROPDOWN) && selectedIndex >= 0
                && selectedIndex < static_cast<int32_t>(_rides.size()))
            {
                _selectedRide = selectedIndex;
                invalidate();
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            const auto* state = Competitive::GetSession().GetState();
            if (state != nullptr && state->scenario.noMoney)
            {
                drawText(
                    rt, windowPos + ScreenCoordsXY{ 12, 27 },
                    "No-money scenario: rival actions use cooldowns and do not charge cash.", { colours[1] });
            }
            else
            {
                const auto cash = Competitive::GetSession().GetAvailableParkCash();
                drawText(
                    rt, windowPos + ScreenCoordsXY{ 12, 27 },
                    "Available park cash: " + FormatStringID(STR_CURRENCY_FORMAT, cash), { colours[1] });
            }

            const auto vandalDescription = _vandalProblem.empty()
                ? "A named guest uses normal vandal and security behaviour; leaves after "
                    + std::to_string(state->rules.vandal.potency) + " breakages or "
                    + std::to_string(state->rules.vandal.durationDays) + " target-park days."
                : _vandalProblem;
            const auto misinformationDescription = _misinformationProblem.empty()
                ? "For " + std::to_string(state->rules.misinformation.durationDays)
                    + " target-park days, removes arrivals at the full inverse strength of half-price entry ("
                    + std::to_string(state->rules.misinformation.potency) + "/65535 per generation tick)."
                : _misinformationProblem;
            const auto poisonDescription = _poisonProblem.empty()
                ? "For " + std::to_string(state->rules.poison.durationDays) + " target-park days, each exact successful purchase has a "
                    + std::to_string(state->rules.poison.potency) + "% chance to give that buyer maximum nausea."
                : _poisonProblem;
            const auto agitatorDescription = _agitatorProblem.empty()
                ? "Acts like a normal guest. Each guest passed once gets a rude-guest thought and a "
                    + std::to_string(state->rules.agitator.potency) + "-point happiness-target penalty."
                : _agitatorProblem;
            const auto hitmanDescription = _hitmanProblem.empty()
                ? "Acts normally until a guest comes close, photographs them with the handheld camera animation, then kills one guest."
                : _hitmanProblem;
            const auto toiletDescription = _toiletBomberProblem.empty()
                ? "Walks to the selected toilet, kills its current occupants, and destroys it without granting a demolition refund."
                : _toiletBomberProblem;
            const auto saboteurDescription = _saboteurProblem.empty()
                ? "Rides the selected attraction normally, forces a supported breakdown after exiting, then leaves."
                : _saboteurProblem;
            drawTextWrapped(rt, windowPos + ScreenCoordsXY{ 215, 48 }, 393, vandalDescription, { colours[1] });
            drawTextWrapped(rt, windowPos + ScreenCoordsXY{ 215, 88 }, 393, misinformationDescription, { colours[1] });
            drawTextWrapped(rt, windowPos + ScreenCoordsXY{ 215,128 }, 393, agitatorDescription, { colours[1] });
            drawTextWrapped(rt, windowPos + ScreenCoordsXY{ 215,168 }, 393, hitmanDescription, { colours[1] });
            drawText(rt, windowPos + ScreenCoordsXY{ 12, 215 }, "Target stall", { colours[1] });
            drawTextWrapped(rt, windowPos + ScreenCoordsXY{ 215,234 }, 393, poisonDescription, { colours[1] });
            drawText(rt, windowPos + ScreenCoordsXY{ 12, 280 }, "Target toilet", { colours[1] });
            drawTextWrapped(rt, windowPos + ScreenCoordsXY{ 215,299 }, 393, toiletDescription, { colours[1] });
            drawText(rt, windowPos + ScreenCoordsXY{ 12, 345 }, "Target ride", { colours[1] });
            drawTextWrapped(rt, windowPos + ScreenCoordsXY{ 215,364 }, 393, saboteurDescription, { colours[1] });
        }
    };

    enum class CompetitiveHostDecision : uint8_t
    {
        forfeitPark,
        closeEarly,
        leaveCompetition,
    };

    enum CompetitiveHostPromptWidgetIdx : WidgetIndex
    {
        CHPWIDX_BACKGROUND,
        CHPWIDX_TITLE,
        CHPWIDX_CLOSE,
        CHPWIDX_CONFIRM,
        CHPWIDX_CANCEL,
    };

    static constexpr ScreenSize kCompetitiveHostPromptSize = { 380, 130 };
    static constexpr auto kCompetitiveHostPromptWidgets = makeWidgets(
        makeWindowShim(kStringIdNone, kCompetitiveHostPromptSize),
        makeWidget({ 50, 104 }, { 130, 14 }, WidgetType::button, WindowColour::secondary, kStringIdEmpty),
        makeWidget({ 200, 104 }, { 130, 14 }, WidgetType::button, WindowColour::secondary, STR_SAVE_PROMPT_CANCEL));

    class CompetitiveHostPromptWindow final : public Window
    {
    private:
        CompetitiveHostDecision _decision = CompetitiveHostDecision::closeEarly;
        Competitive::ParticipantId _targetId = Competitive::kInvalidParticipantId;
        std::string _title;
        std::string _message;
        std::string _confirmLabel;

    public:
        void SetDecision(CompetitiveHostDecision decision, Competitive::ParticipantId targetId)
        {
            _decision = decision;
            _targetId = targetId;
            invalidate();
        }

        void onOpen() override
        {
            setWidgets(kCompetitiveHostPromptWidgets);
            WindowInitScrollWidgets(*this);
        }

        void onPrepareDraw() override
        {
            const auto* state = Competitive::GetSession().GetState();
            const auto* target = state == nullptr ? nullptr : Competitive::FindParticipant(*state, _targetId);
            if (_decision == CompetitiveHostDecision::forfeitPark)
            {
                _title = "Confirm park forfeit";
                _confirmLabel = "Forfeit park";
                _message = "Forfeit " + (target == nullptr ? std::string("this park") : target->name)
                    + "? It will be excluded from the result and cannot rejoin.";
            }
            else if (_decision == CompetitiveHostDecision::closeEarly)
            {
                _title = "Confirm early result";
                _confirmLabel = "Calculate result now";
                _message = "End the competition now? Every unfinished park's current score will be frozen and the winner calculated.";
            }
            else
            {
                const bool host = Competitive::GetSession().GetMode() == Competitive::SessionMode::host;
                _title = "Confirm leaving competition";
                _confirmLabel = "Leave competition";
                _message = host
                    ? "Leave and close the host session? Connected parks will pause and attempt to reconnect."
                    : "Leave this competition? Your park will no longer be connected to the match.";
            }
            widgets[CHPWIDX_TITLE].setString(_title.c_str());
            widgets[CHPWIDX_CONFIRM].setString(_confirmLabel.c_str());
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            if (widgetIndex == CHPWIDX_CLOSE || widgetIndex == CHPWIDX_CANCEL)
            {
                close();
                return;
            }
            if (widgetIndex != CHPWIDX_CONFIRM)
                return;
            if (_decision == CompetitiveHostDecision::leaveCompetition)
            {
                Competitive::GetSession().Stop();
                GetWindowManager()->CloseByClass(WindowClass::multiplayer);
                close();
                return;
            }
            std::string error;
            auto& session = Competitive::GetSession();
            const bool success = _decision == CompetitiveHostDecision::forfeitPark ? session.Forfeit(_targetId, error)
                                                                                    : session.CloseEarly(error);
            if (!success)
            {
                ErrorOpen("Host action failed", error);
                return;
            }
            close();
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            drawTextWrapped(
                rt, windowPos + ScreenCoordsXY{ kCompetitiveHostPromptSize.width / 2, 42 },
                kCompetitiveHostPromptSize.width - 30, _message, { colours[1], TextAlignment::centre });
        }
    };

    static void CompetitiveActionsOpen(Competitive::ParticipantId targetId)
    {
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::competitiveActions);
        auto* window = windowMgr->Create<CompetitiveActionsWindow>(
            WindowClass::competitiveActions, kCompetitiveActionsWindowSize,
            { WindowFlag::centreScreen });
        window->SetTarget(targetId);
    }

    static void CompetitiveHostPromptOpen(CompetitiveHostDecision decision, Competitive::ParticipantId targetId)
    {
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::competitiveHostPrompt);
        auto* window = windowMgr->Create<CompetitiveHostPromptWindow>(
            WindowClass::competitiveHostPrompt, kCompetitiveHostPromptSize,
            { WindowFlag::centreScreen, WindowFlag::transparent });
        window->SetDecision(decision, targetId);
    }

    class MultiplayerWindow final : public Window
    {
    private:
        std::optional<ScreenSize> _windowInformationSize;
        uint8_t _selectedGroup{ 0 };
        std::vector<Competitive::ParticipantId> _competitionRows;
        Competitive::ParticipantId _selectedCompetitionId = Competitive::kInvalidParticipantId;
        std::string _competitionTitle;
        std::string _competitionTabTooltip = "Competition leaderboard and lobby";
        std::string _placeHeader = "Place";
        std::string _parkHeader = "Park / player";
        std::string _statusHeader = "Status";
        std::string _yearHeader = "Year";
        std::string _scoreHeader;
        std::string _readyText;
        std::string _startText = "Start competition";
        std::string _actionsText = "Attack rival...";
        std::string _actionsTooltip = "Select an online, unfinished rival in the leaderboard first.";
        std::string _watchText = "Watch park";
        std::string _hostControlsText = "Host controls";
        std::string _leaveText = "Leave competition";

    private:
        const Competitive::Participant* getSelectedCompetitionParticipant() const
        {
            const auto* state = Competitive::GetSession().GetState();
            return state == nullptr ? nullptr : Competitive::FindParticipant(*state, _selectedCompetitionId);
        }

        void showGroupDropdown(WidgetIndex widgetIndex)
        {
            auto widget = &widgets[widgetIndex];
            Widget* dropdownWidget = widget - 1;
            auto numItems = Network::GetNumGroups();

            WindowDropdownShowTextCustomWidth(
                windowPos + ScreenCoordsXY{ dropdownWidget->left, dropdownWidget->top }, dropdownWidget->height(), colours[1],
                0, { Dropdown::Flag::autoClose }, numItems, widget->right - dropdownWidget->left);

            for (auto i = 0; i < Network::GetNumGroups(); i++)
            {
                gDropdown.items[i] = Dropdown::MenuLabel(Network::GetGroupName(i));
            }
            if (widget == &widgets[WIDX_DEFAULT_GROUP_DROPDOWN])
            {
                gDropdown.items[Network::GetGroupIndex(Network::GetDefaultGroup())].setChecked(true);
            }
            else if (widget == &widgets[WIDX_SELECTED_GROUP_DROPDOWN])
            {
                gDropdown.items[Network::GetGroupIndex(_selectedGroup)].setChecked(true);
            }
        }

        void informationPaint(RenderTarget& rt)
        {
            RenderTarget clippedRT;
            if (ClipRenderTarget(clippedRT, rt, windowPos, width, height))
            {
                auto screenCoords = ScreenCoordsXY{ 3, widgets[WIDX_CONTENT_PANEL].top + 7 };
                int32_t newWidth = width - 6;

                const auto& name = Network::GetServerName();
                {
                    screenCoords.y += drawTextWrapped(clippedRT, screenCoords, newWidth, name, { colours[1] });
                    screenCoords.y += kListRowHeight / 2;
                }

                const auto& description = Network::GetServerDescription();
                if (!description.empty())
                {
                    screenCoords.y += drawTextWrapped(clippedRT, screenCoords, newWidth, description, { colours[1] });
                    screenCoords.y += kListRowHeight / 2;
                }

                const auto& providerName = Network::GetServerProviderName();
                if (!providerName.empty())
                {
                    auto ft = Formatter();
                    ft.Add<const char*>(providerName.c_str());
                    drawText(clippedRT, screenCoords, STR_PROVIDER_NAME, ft);
                    screenCoords.y += kListRowHeight;
                }

                const auto& providerEmail = Network::GetServerProviderEmail();
                if (!providerEmail.empty())
                {
                    auto ft = Formatter();
                    ft.Add<const char*>(providerEmail.c_str());
                    drawText(clippedRT, screenCoords, STR_PROVIDER_EMAIL, ft);
                    screenCoords.y += kListRowHeight;
                }

                const auto& providerWebsite = Network::GetServerProviderWebsite();
                if (!providerWebsite.empty())
                {
                    auto ft = Formatter();
                    ft.Add<const char*>(providerWebsite.c_str());
                    drawText(clippedRT, screenCoords, STR_PROVIDER_WEBSITE, ft);
                }
            }
        }

        void playersPaint(RenderTarget& rt)
        {
            // Number of players
            StringId stringId = numListItems == 1 ? STR_MULTIPLAYER_PLAYER_COUNT : STR_MULTIPLAYER_PLAYER_COUNT_PLURAL;
            auto screenCoords = windowPos + ScreenCoordsXY{ 4, widgets[WIDX_LIST].bottom + 2 };
            auto ft = Formatter();
            ft.Add<uint16_t>(numListItems);
            drawText(rt, screenCoords, stringId, ft, { colours[2] });
        }

        void playersScrollPaint(int32_t scrollIndex, RenderTarget& rt) const
        {
            ScreenCoordsXY screenCoords;
            screenCoords.y = 0;

            const int32_t firstPlayerInList = (IsServerPlayerInvisible() ? 1 : 0);
            int32_t listPosition = 0;

            for (int32_t player = firstPlayerInList; player < Network::GetNumPlayers(); player++)
            {
                if (screenCoords.y > rt.y + rt.height)
                {
                    break;
                }

                if (screenCoords.y + kScrollableRowHeight + 1 >= rt.y)
                {
                    thread_local std::string _buffer;
                    _buffer.reserve(512);
                    _buffer.clear();

                    // Draw player name
                    auto colour = ColourWithFlags{ Drawing::Colour::black };
                    if (listPosition == selectedListItem)
                    {
                        Rectangle::filter(
                            rt, { 0, screenCoords.y, 800, screenCoords.y + kScrollableRowHeight - 1 },
                            FilterPaletteID::paletteDarken1);
                        _buffer += Network::GetPlayerName(player);
                        colour = colours[2];
                    }
                    else
                    {
                        if (Network::GetPlayerFlags(player) & Network::PlayerFlags::kIsServer)
                        {
                            _buffer += "{BABYBLUE}";
                        }
                        else
                        {
                            _buffer += "{BLACK}";
                        }
                        _buffer += Network::GetPlayerName(player);
                    }
                    screenCoords.x = 0;
                    drawTextEllipsised(rt, screenCoords, 230, _buffer, { colour });

                    // Draw group name
                    _buffer.resize(0);
                    int32_t group = Network::GetGroupIndex(Network::GetPlayerGroup(player));
                    if (group != -1)
                    {
                        _buffer += "{BLACK}";
                        screenCoords.x = 173;
                        _buffer += Network::GetGroupName(group);
                        drawTextEllipsised(rt, screenCoords, 80, _buffer, { colour });
                    }

                    // Draw last action
                    int32_t action = Network::GetPlayerLastAction(player, 2000);
                    auto ft = Formatter();
                    if (action != -999)
                    {
                        ft.Add<StringId>(Network::GetActionNameStringID(action));
                    }
                    else
                    {
                        ft.Add<StringId>(STR_ACTION_NA);
                    }
                    drawTextEllipsised(rt, { 256, screenCoords.y }, 100, STR_BLACK_STRING, ft);

                    // Draw ping
                    _buffer.resize(0);
                    int32_t ping = Network::GetPlayerPing(player);
                    if (ping <= 100)
                    {
                        _buffer += "{GREEN}";
                    }
                    else if (ping <= 250)
                    {
                        _buffer += "{YELLOW}";
                    }
                    else
                    {
                        _buffer += "{RED}";
                    }

                    char pingBuffer[64]{};
                    snprintf(pingBuffer, sizeof(pingBuffer), "%d ms", ping);
                    _buffer += pingBuffer;

                    screenCoords.x = 356;
                    drawText(rt, screenCoords, _buffer, { colour });
                }
                screenCoords.y += kScrollableRowHeight;
                listPosition++;
            }
        }

        void groupsPaint(RenderTarget& rt)
        {
            thread_local std::string _buffer;

            Widget* widget = &widgets[WIDX_DEFAULT_GROUP];
            int32_t group = Network::GetGroupIndex(Network::GetDefaultGroup());
            if (group != -1)
            {
                _buffer.assign("{WINDOW_COLOUR_2}");
                _buffer += Network::GetGroupName(group);

                auto ft = Formatter();
                ft.Add<const char*>(_buffer.c_str());
                drawTextEllipsised(
                    rt, windowPos + ScreenCoordsXY{ widget->midX() - 5, widget->top }, widget->width() - 9, STR_STRING, ft,
                    { TextAlignment::centre });
            }

            auto screenPos = windowPos
                + ScreenCoordsXY{ widgets[WIDX_CONTENT_PANEL].left + 4, widgets[WIDX_CONTENT_PANEL].top + 4 };

            drawText(rt, screenPos, STR_DEFAULT_GROUP, { colours[2] });

            screenPos.y += 20;

            Rectangle::fillInset(
                rt, { screenPos - ScreenCoordsXY{ 0, 6 }, screenPos + ScreenCoordsXY{ 310, -5 } }, colours[1],
                Rectangle::BorderStyle::inset);

            widget = &widgets[WIDX_SELECTED_GROUP];
            group = Network::GetGroupIndex(_selectedGroup);
            if (group != -1)
            {
                _buffer.assign("{WINDOW_COLOUR_2}");
                _buffer += Network::GetGroupName(group);
                drawTextEllipsised(
                    rt, windowPos + ScreenCoordsXY{ widget->midX() - 5, widget->top }, widget->width() - 9, _buffer,
                    { TextAlignment::centre });
            }
        }

        void groupsScrollPaint(int32_t scrollIndex, RenderTarget& rt) const
        {
            auto screenCoords = ScreenCoordsXY{ 0, 0 };

            auto rtCoords = ScreenCoordsXY{ rt.x, rt.y };
            Rectangle::fill(
                rt, { rtCoords, rtCoords + ScreenCoordsXY{ rt.width - 1, rt.height - 1 } },
                getColourMap(colours[1].colour).midLight);

            for (int32_t i = 0; i < Network::GetNumActions(); i++)
            {
                if (i == selectedListItem)
                {
                    Rectangle::filter(
                        rt, { 0, screenCoords.y, 800, screenCoords.y + kScrollableRowHeight - 1 },
                        FilterPaletteID::paletteDarken1);
                }
                if (screenCoords.y > rt.y + rt.height)
                {
                    break;
                }

                if (screenCoords.y + kScrollableRowHeight + 1 >= rt.y)
                {
                    int32_t groupindex = Network::GetGroupIndex(_selectedGroup);
                    if (groupindex != -1)
                    {
                        if (Network::CanPerformAction(groupindex, static_cast<Network::Permission>(i)))
                        {
                            screenCoords.x = 0;
                            drawText(rt, screenCoords, u8"{WINDOW_COLOUR_2}✓");
                        }
                    }

                    // Draw action name
                    auto ft = Formatter();
                    ft.Add<uint16_t>(Network::GetActionNameStringID(i));
                    drawText(rt, { 10, screenCoords.y }, STR_WINDOW_COLOUR_2_STRINGID, ft);
                }
                screenCoords.y += kScrollableRowHeight;
            }
        }

        void drawTabImage(RenderTarget& rt, int32_t page_number, int32_t spriteIndex)
        {
            WidgetIndex widgetIndex = WIDX_TAB1 + page_number;

            if (!isWidgetDisabled(widgetIndex))
            {
                if (page == page_number)
                {
                    int32_t numFrames = window_multiplayer_animation_frames[page];
                    if (numFrames > 1)
                    {
                        int32_t frame = currentFrame / window_multiplayer_animation_divisor[page];
                        spriteIndex += (frame % numFrames);
                    }
                }

                GfxDrawSprite(
                    rt, ImageId(spriteIndex),
                    windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
            }
        }

        void drawTabImages(RenderTarget& rt)
        {
            drawTabImage(rt, WINDOW_MULTIPLAYER_PAGE_INFORMATION, SPR_TAB_KIOSKS_AND_FACILITIES_0);
            drawTabImage(rt, WINDOW_MULTIPLAYER_PAGE_PLAYERS, SPR_TAB_GUESTS_0);
            drawTabImage(rt, WINDOW_MULTIPLAYER_PAGE_GROUPS, SPR_TAB_STAFF_OPTIONS_0);
            drawTabImage(rt, WINDOW_MULTIPLAYER_PAGE_OPTIONS, SPR_TAB_GEARS_0);
            if (Competitive::GetSession().GetMode() != Competitive::SessionMode::none)
                drawTabImage(rt, WINDOW_MULTIPLAYER_PAGE_COMPETITION, SPR_TAB_AWARDS);
        }

        ScreenSize informationGetSize()
        {
            assert(!_windowInformationSize.has_value());

            int32_t lineHeight = FontGetLineHeight(FontStyle::medium);

            // Base dimensions.
            const int32_t baseWidth = 450;
            int32_t baseHeight = 55;

            // Server name is displayed word-wrapped, so figure out how high it will be.
            {
                int32_t numLines;
                wrapString(Network::GetServerName(), baseWidth, FontStyle::medium, nullptr, &numLines);
                baseHeight += (numLines + 1) * lineHeight + (kListRowHeight / 2);
            }

            // Likewise, for the optional server description -- which can be a little longer.
            const auto& descString = Network::GetServerDescription();
            if (!descString.empty())
            {
                int32_t numLines;
                wrapString(descString, baseWidth, FontStyle::medium, nullptr, &numLines);
                baseHeight += (numLines + 1) * lineHeight + (kListRowHeight / 2);
            }

            // Finally, account for provider info, if present.
            {
                const auto& providerName = Network::GetServerProviderName();
                if (!providerName.empty())
                    baseHeight += kListRowHeight;

                const auto& providerEmail = Network::GetServerProviderEmail();
                if (!providerEmail.empty())
                    baseHeight += kListRowHeight;

                const auto& providerWebsite = Network::GetServerProviderWebsite();
                if (!providerWebsite.empty())
                    baseHeight += kListRowHeight;
            }

            // TODO: Are these casts still neccessary?
            _windowInformationSize = { static_cast<int16_t>(baseWidth), static_cast<int16_t>(baseHeight) };
            return _windowInformationSize.value();
        }

    public:
        void onOpen() override
        {
            setPage(
                Competitive::GetSession().GetMode() == Competitive::SessionMode::none
                    ? WINDOW_MULTIPLAYER_PAGE_INFORMATION
                    : WINDOW_MULTIPLAYER_PAGE_COMPETITION);
        }

        void setPage(int32_t page_number)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == page_number && !widgets.empty())
                return;

            _windowInformationSize.reset();

            page = page_number;
            currentFrame = 0;
            numListItems = 0;
            selectedListItem = -1;
            _selectedCompetitionId = Competitive::kInvalidParticipantId;

            setWidgets(window_multiplayer_page_widgets[page]);
            widgets[WIDX_TITLE].setString(WindowMultiplayerPageTitles[page]);
            widgets[WIDX_TAB5].setTooltip(_competitionTabTooltip.c_str());
            if (page == WINDOW_MULTIPLAYER_PAGE_COMPETITION)
            {
                widgets[WIDX_COMP_PLACE].setString(_placeHeader.c_str());
                widgets[WIDX_COMP_PARK].setString(_parkHeader.c_str());
                widgets[WIDX_COMP_STATUS].setString(_statusHeader.c_str());
                widgets[WIDX_COMP_YEAR].setString(_yearHeader.c_str());
                widgets[WIDX_COMP_START].setString(_startText.c_str());
                widgets[WIDX_COMP_ACTIONS].setString(_actionsText.c_str());
                widgets[WIDX_COMP_ACTIONS].setTooltip(_actionsTooltip.c_str());
                widgets[WIDX_COMP_WATCH].setString(_watchText.c_str());
                widgets[WIDX_COMP_HOST_CONTROLS].setString(_hostControlsText.c_str());
                widgets[WIDX_COMP_LEAVE].setString(_leaveText.c_str());
            }
            setWidgetPressed(WIDX_TAB1 + page, true);

            refreshList();
            onResize();
            onPrepareDraw();
            initScrollWidgets();
            invalidate();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_TAB1:
                case WIDX_TAB2:
                case WIDX_TAB3:
                case WIDX_TAB4:
                case WIDX_TAB5:
                    if (page != widgetIndex - WIDX_TAB1)
                    {
                        setPage(widgetIndex - WIDX_TAB1);
                    }
                    break;
            }

            auto& gameState = getGameState();
            switch (page)
            {
                case WINDOW_MULTIPLAYER_PAGE_COMPETITION:
                {
                    auto& session = Competitive::GetSession();
                    std::string error;
                    switch (widgetIndex)
                    {
                        case WIDX_COMP_READY:
                        {
                            const auto* local = session.GetLocalParticipant();
                            if (local != nullptr && !session.SetReady(!local->ready, error))
                                ErrorOpen("Cannot change readiness", error);
                            break;
                        }
                        case WIDX_COMP_START:
                            if (!session.StartMatch(error))
                                ErrorOpen("Cannot start competition", error);
                            break;
                        case WIDX_COMP_ACTIONS:
                        {
                            const auto* target = getSelectedCompetitionParticipant();
                            const auto* local = session.GetLocalParticipant();
                            if (target == nullptr || local == nullptr || target->id == local->id
                                || !Competitive::CanTarget(*target))
                            {
                                ErrorOpen("Rival actions", "Select an online, unfinished rival park in the leaderboard.");
                                break;
                            }
                            CompetitiveActionsOpen(target->id);
                            break;
                        }
                        case WIDX_COMP_WATCH:
                        {
                            const auto* target = getSelectedCompetitionParticipant();
                            if (target == nullptr || !session.WatchParticipant(target->id, error))
                                ErrorOpen("Cannot watch park", error.empty() ? "Select an online rival park." : error);
                            break;
                        }
                        case WIDX_COMP_HOST_CONTROLS:
                        {
                            const auto* target = getSelectedCompetitionParticipant();
                            const bool canForfeit = target != nullptr && target->id != session.GetLocalParticipantId()
                                && target->role != Competitive::Role::spectator && !target->online && !target->finished
                                && !target->forfeited;
                            CompetitiveHostPromptOpen(
                                canForfeit ? CompetitiveHostDecision::forfeitPark : CompetitiveHostDecision::closeEarly,
                                canForfeit ? target->id : Competitive::kInvalidParticipantId);
                            break;
                        }
                        case WIDX_COMP_LEAVE:
                            CompetitiveHostPromptOpen(
                                CompetitiveHostDecision::leaveCompetition, Competitive::kInvalidParticipantId);
                            break;
                    }
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_GROUPS:
                {
                    switch (widgetIndex)
                    {
                        case WIDX_ADD_GROUP:
                        {
                            auto networkModifyGroup = GameActions::NetworkModifyGroupAction(
                                GameActions::ModifyGroupType::addGroup);
                            GameActions::Execute(&networkModifyGroup, gameState);
                            break;
                        }
                        case WIDX_REMOVE_GROUP:
                        {
                            auto networkModifyGroup = GameActions::NetworkModifyGroupAction(
                                GameActions::ModifyGroupType::removeGroup, _selectedGroup);
                            GameActions::Execute(&networkModifyGroup, gameState);
                            break;
                        }
                        case WIDX_RENAME_GROUP:
                        {
                            int32_t groupIndex = Network::GetGroupIndex(_selectedGroup);
                            const utf8* groupName = Network::GetGroupName(groupIndex);
                            WindowTextInputRawOpen(
                                this, widgetIndex, STR_GROUP_NAME, STR_ENTER_NEW_NAME_FOR_THIS_GROUP, {}, groupName, 32);
                            break;
                        }
                    }
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_OPTIONS:
                {
                    switch (widgetIndex)
                    {
                        case WIDX_LOG_CHAT_CHECKBOX:
                            Config::Get().network.logChat = !Config::Get().network.logChat;
                            Config::Save();
                            break;
                        case WIDX_LOG_SERVER_ACTIONS_CHECKBOX:
                            Config::Get().network.logServerActions = !Config::Get().network.logServerActions;
                            Config::Save();
                            break;
                        case WIDX_KNOWN_KEYS_ONLY_CHECKBOX:
                            Config::Get().network.knownKeysOnly = !Config::Get().network.knownKeysOnly;
                            Config::Save();
                            break;
                    }
                    break;
                }
            }
        }

        void onResize() override
        {
            switch (page)
            {
                case WINDOW_MULTIPLAYER_PAGE_INFORMATION:
                {
                    auto size = _windowInformationSize ? _windowInformationSize.value() : informationGetSize();
                    WindowSetResize(*this, size, size);
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
                {
                    WindowSetResize(*this, { 420, 124 }, { 500, 450 });

                    widgets[WIDX_HEADER_PING].right = width - 5;

                    selectedListItem = -1;
                    invalidate();
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_GROUPS:
                {
                    WindowSetResize(*this, { 320, 200 }, { 320, 500 });

                    numListItems = Network::GetNumActions();

                    selectedListItem = -1;
                    invalidate();
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_OPTIONS:
                {
                    WindowSetResize(*this, { 300, 100 }, { 300, 100 });
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_COMPETITION:
                {
                    WindowSetResize(*this, { 620, 320 }, { 900, 650 });
                    selectedListItem = -1;
                    invalidate();
                    break;
                }
            }
        }

        void onUpdate() override
        {
            currentFrame++;
            invalidateWidget(WIDX_TAB1 + page);
            if (page == WINDOW_MULTIPLAYER_PAGE_COMPETITION)
            {
                refreshList();
                invalidate();
            }
        }

        void onPrepareDraw() override
        {
            switch (page)
            {
                case WINDOW_MULTIPLAYER_PAGE_INFORMATION:
                {
                    WindowAlignTabs(this, WIDX_TAB1, WIDX_TAB4);
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
                {
                    widgets[WIDX_LIST].right = width - 4;
                    widgets[WIDX_LIST].bottom = height - 0x0F;
                    WindowAlignTabs(this, WIDX_TAB1, WIDX_TAB4);
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_GROUPS:
                {
                    widgets[WIDX_PERMISSIONS_LIST].right = width - 4;
                    widgets[WIDX_PERMISSIONS_LIST].bottom = height - 0x0F;
                    WindowAlignTabs(this, WIDX_TAB1, WIDX_TAB4);

                    // select other group if one is removed
                    while (Network::GetGroupIndex(_selectedGroup) == -1 && _selectedGroup > 0)
                    {
                        _selectedGroup--;
                    }
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_OPTIONS:
                {
                    WindowAlignTabs(this, WIDX_TAB1, WIDX_TAB4);

                    widgets[WIDX_KNOWN_KEYS_ONLY_CHECKBOX].setHidden(Network::GetMode() == Network::Mode::client);

                    setCheckboxValue(WIDX_LOG_CHAT_CHECKBOX, Config::Get().network.logChat);
                    setCheckboxValue(WIDX_LOG_SERVER_ACTIONS_CHECKBOX, Config::Get().network.logServerActions);
                    setCheckboxValue(WIDX_KNOWN_KEYS_ONLY_CHECKBOX, Config::Get().network.knownKeysOnly);
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_COMPETITION:
                {
                    widgets[WIDX_CONTENT_PANEL].right = width - 1;
                    widgets[WIDX_CONTENT_PANEL].bottom = height - 1;
                    widgets[WIDX_COMP_LIST].right = width - 4;
                    widgets[WIDX_COMP_LIST].bottom = height - 49;
                    widgets[WIDX_COMP_SCORE].right = width - 4;
                    widgets[WIDX_COMP_READY].top = height - 28;
                    widgets[WIDX_COMP_READY].bottom = height - 15;
                    widgets[WIDX_COMP_START].top = height - 28;
                    widgets[WIDX_COMP_START].bottom = height - 15;
                    widgets[WIDX_COMP_ACTIONS].top = height - 28;
                    widgets[WIDX_COMP_ACTIONS].bottom = height - 15;
                    widgets[WIDX_COMP_WATCH].top = height - 28;
                    widgets[WIDX_COMP_WATCH].bottom = height - 15;
                    widgets[WIDX_COMP_HOST_CONTROLS].top = height - 28;
                    widgets[WIDX_COMP_HOST_CONTROLS].bottom = height - 15;
                    widgets[WIDX_COMP_LEAVE].top = height - 28;
                    widgets[WIDX_COMP_LEAVE].bottom = height - 15;
                    WindowAlignTabs(this, WIDX_TAB1, WIDX_TAB5);

                    auto& session = Competitive::GetSession();
                    const auto* state = session.GetState();
                    const auto* local = session.GetLocalParticipant();
                    const bool lobby = state != nullptr && state->phase == Competitive::Phase::lobby;
                    const bool running = state != nullptr && state->phase == Competitive::Phase::running;
                    const bool host = session.GetMode() == Competitive::SessionMode::host;
                    const bool competitor = local != nullptr && local->role != Competitive::Role::spectator;
                    const auto* selected = getSelectedCompetitionParticipant();
                    const bool targetable = selected != nullptr && local != nullptr && selected->id != local->id
                        && Competitive::CanTarget(*selected);
                    const bool canAttack = running && competitor && targetable;
                    const bool watchable = selected != nullptr && local != nullptr && selected->id != local->id
                        && selected->role != Competitive::Role::spectator && selected->online && selected->watchPort != 0;
                    const bool forfeitEligible = selected != nullptr && selected->id != session.GetLocalParticipantId()
                        && selected->role != Competitive::Role::spectator && !selected->online && !selected->finished
                        && !selected->forfeited;
                    widgets[WIDX_COMP_READY].setVisible(lobby && competitor);
                    widgets[WIDX_COMP_START].setVisible(lobby && host);
                    widgets[WIDX_COMP_ACTIONS].setVisible((lobby || running) && competitor);
                    setWidgetDisabled(WIDX_COMP_ACTIONS, !canAttack);
                    widgets[WIDX_COMP_WATCH].setVisible(watchable);
                    widgets[WIDX_COMP_HOST_CONTROLS].setVisible(
                        host && running);
                    widgets[WIDX_COMP_LEAVE].setVisible(!host || !running);
                    _actionsText = canAttack ? "Attack " + selected->name + "..." : "Attack rival...";
                    if (lobby)
                        _actionsTooltip = "Competitive attacks become available when the host starts the match.";
                    else if (canAttack)
                        _actionsTooltip = "Send a competitive attack to " + selected->name + ".";
                    else
                        _actionsTooltip = "Select an online, unfinished rival in the leaderboard first.";
                    _hostControlsText = forfeitEligible ? "Forfeit " + selected->name + "..." : "End early...";
                    widgets[WIDX_COMP_ACTIONS].setString(_actionsText.c_str());
                    widgets[WIDX_COMP_ACTIONS].setTooltip(_actionsTooltip.c_str());
                    widgets[WIDX_COMP_HOST_CONTROLS].setString(_hostControlsText.c_str());
                    setWidgetDisabled(WIDX_COMP_START, !host || !session.GetStartProblems().empty());
                    _readyText = local != nullptr && local->ready ? "Not ready" : "Ready";
                    widgets[WIDX_COMP_READY].setString(_readyText.c_str());
                    if (state != nullptr)
                    {
                        _competitionTitle = "Competition — " + state->name;
                        _scoreHeader = competitionMetricName(state->rules.metric);
                    }
                    else
                    {
                        _competitionTitle = "Competition";
                        _scoreHeader = "Score";
                    }
                    widgets[WIDX_TITLE].setString(_competitionTitle.c_str());
                    widgets[WIDX_COMP_SCORE].setString(_scoreHeader.c_str());
                    break;
                }
            }
            widgets[WIDX_TAB5].setVisible(Competitive::GetSession().GetMode() != Competitive::SessionMode::none);
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            drawTabImages(rt);
            switch (page)
            {
                case WINDOW_MULTIPLAYER_PAGE_INFORMATION:
                {
                    informationPaint(rt);
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
                {
                    playersPaint(rt);
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_GROUPS:
                {
                    groupsPaint(rt);
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_COMPETITION:
                    competitionPaint(rt);
                    break;
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (page)
            {
                case WINDOW_MULTIPLAYER_PAGE_GROUPS:
                {
                    if (selectedIndex == -1)
                    {
                        return;
                    }

                    switch (widgetIndex)
                    {
                        case WIDX_DEFAULT_GROUP_DROPDOWN:
                        {
                            auto networkModifyGroup = GameActions::NetworkModifyGroupAction(
                                GameActions::ModifyGroupType::setDefault, Network::GetGroupID(selectedIndex));
                            GameActions::Execute(&networkModifyGroup, getGameState());
                            break;
                        }
                        case WIDX_SELECTED_GROUP_DROPDOWN:
                        {
                            _selectedGroup = Network::GetGroupID(selectedIndex);
                            break;
                        }
                    }
                    invalidate();
                    break;
                }
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            switch (page)
            {
                case WINDOW_MULTIPLAYER_PAGE_GROUPS:
                {
                    if (widgetIndex != WIDX_RENAME_GROUP)
                        return;

                    if (text.empty())
                        return;

                    auto networkModifyGroup = GameActions::NetworkModifyGroupAction(
                        GameActions::ModifyGroupType::setName, _selectedGroup, std::string(text));
                    GameActions::Execute(&networkModifyGroup, getGameState());
                    break;
                }
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_MULTIPLAYER_PAGE_GROUPS:
                {
                    switch (widgetIndex)
                    {
                        case WIDX_DEFAULT_GROUP_DROPDOWN:
                        case WIDX_SELECTED_GROUP_DROPDOWN:
                            showGroupDropdown(widgetIndex);
                            break;
                    }
                    break;
                }
            }
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            ScreenSize screenSize{};
            switch (page)
            {
                case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
                {
                    if (selectedListItem != -1)
                    {
                        selectedListItem = -1;
                        invalidate();
                    }

                    screenSize = { 0, Network::GetNumPlayers() * kScrollableRowHeight };
                    int32_t i = screenSize.height - widgets[WIDX_LIST].bottom + widgets[WIDX_LIST].top + 21;
                    if (i < 0)
                        i = 0;
                    if (i < scrolls[0].contentOffsetY)
                    {
                        scrolls[0].contentOffsetY = i;
                        invalidate();
                    }
                    break;
                }

                case WINDOW_MULTIPLAYER_PAGE_GROUPS:
                {
                    if (selectedListItem != -1)
                    {
                        selectedListItem = -1;
                        invalidate();
                    }

                    screenSize = { 0, Network::GetNumActions() * kScrollableRowHeight };
                    int32_t i = screenSize.height - widgets[WIDX_LIST].bottom + widgets[WIDX_LIST].top + 21;
                    if (i < 0)
                        i = 0;
                    if (i < scrolls[0].contentOffsetY)
                    {
                        scrolls[0].contentOffsetY = i;
                        invalidate();
                    }
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_COMPETITION:
                    screenSize = { 0, static_cast<int32_t>(_competitionRows.size()) * kScrollableRowHeight };
                    break;
            }
            return screenSize;
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (page)
            {
                case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
                {
                    int32_t index = screenCoords.y / kScrollableRowHeight;
                    if (index >= numListItems)
                        return;

                    selectedListItem = index;
                    invalidate();

                    int32_t player = (IsServerPlayerInvisible() ? index + 1 : index);
                    PlayerOpen(Network::GetPlayerID(player));
                    break;
                }

                case WINDOW_MULTIPLAYER_PAGE_GROUPS:
                {
                    int32_t index = screenCoords.y / kScrollableRowHeight;
                    if (index >= numListItems)
                        return;

                    selectedListItem = index;
                    invalidate();

                    auto networkModifyGroup = GameActions::NetworkModifyGroupAction(
                        GameActions::ModifyGroupType::setPermissions, _selectedGroup, "", index,
                        GameActions::PermissionState::toggle);
                    GameActions::Execute(&networkModifyGroup, getGameState());
                    break;
                }
                case WINDOW_MULTIPLAYER_PAGE_COMPETITION:
                {
                    const int32_t index = screenCoords.y / kScrollableRowHeight;
                    if (index >= 0 && index < static_cast<int32_t>(_competitionRows.size()))
                    {
                        const auto participantId = _competitionRows[index];
                        if (_selectedCompetitionId == participantId)
                        {
                            _selectedCompetitionId = Competitive::kInvalidParticipantId;
                            selectedListItem = -1;
                        }
                        else
                        {
                            _selectedCompetitionId = participantId;
                            selectedListItem = index;
                        }
                        invalidate();
                    }
                    break;
                }
            }
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (page)
            {
                case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
                case WINDOW_MULTIPLAYER_PAGE_GROUPS:
                {
                    int32_t index = screenCoords.y / kScrollableRowHeight;
                    if (index >= numListItems)
                        return;

                    selectedListItem = index;
                    invalidate();
                    break;
                }
            }
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            switch (page)
            {
                case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
                    playersScrollPaint(scrollIndex, rt);
                    break;

                case WINDOW_MULTIPLAYER_PAGE_GROUPS:
                    groupsScrollPaint(scrollIndex, rt);
                    break;

                case WINDOW_MULTIPLAYER_PAGE_COMPETITION:
                    competitionScrollPaint(rt);
                    break;
            }
        }

        void refreshList()
        {
            if (page == WINDOW_MULTIPLAYER_PAGE_PLAYERS)
            {
                numListItems = (IsServerPlayerInvisible() ? Network::GetNumVisiblePlayers() : Network::GetNumPlayers());
            }
            else if (page == WINDOW_MULTIPLAYER_PAGE_COMPETITION)
            {
                _competitionRows.clear();
                const auto* state = Competitive::GetSession().GetState();
                if (state == nullptr)
                {
                    numListItems = 0;
                    return;
                }
                for (const auto& participant : state->participants)
                {
                    if (participant.role != Competitive::Role::spectator)
                        _competitionRows.push_back(participant.id);
                }
                std::sort(_competitionRows.begin(), _competitionRows.end(), [state](auto lhsId, auto rhsId) {
                    const auto* lhsParticipant = Competitive::FindParticipant(*state, lhsId);
                    const auto* rhsParticipant = Competitive::FindParticipant(*state, rhsId);
                    if (lhsParticipant->forfeited != rhsParticipant->forfeited)
                        return !lhsParticipant->forfeited;
                    const auto* lhsScore = Competitive::FindScore(*state, lhsId);
                    const auto* rhsScore = Competitive::FindScore(*state, rhsId);
                    const auto lhsMetric = lhsScore == nullptr ? INT64_MIN : Competitive::GetMetricValue(*lhsScore, state->rules.metric);
                    const auto rhsMetric = rhsScore == nullptr ? INT64_MIN : Competitive::GetMetricValue(*rhsScore, state->rules.metric);
                    if (lhsMetric != rhsMetric)
                        return lhsMetric > rhsMetric;
                    const auto lhsPoints = lhsScore == nullptr ? INT64_MIN : lhsScore->points;
                    const auto rhsPoints = rhsScore == nullptr ? INT64_MIN : rhsScore->points;
                    if (lhsPoints != rhsPoints)
                        return lhsPoints > rhsPoints;
                    return lhsId < rhsId;
                });
                const auto selected = std::find(
                    _competitionRows.begin(), _competitionRows.end(), _selectedCompetitionId);
                if (selected == _competitionRows.end())
                {
                    _selectedCompetitionId = Competitive::kInvalidParticipantId;
                    selectedListItem = -1;
                }
                else
                {
                    selectedListItem = static_cast<int32_t>(std::distance(_competitionRows.begin(), selected));
                }
                numListItems = static_cast<uint16_t>(_competitionRows.size());
            }
        }

        static const char* competitionStatusName(const Competitive::Participant& participant, Competitive::Phase phase)
        {
            switch (Competitive::GetParticipantStatus(participant, phase))
            {
                case Competitive::ParticipantStatus::lobby:
                    return "In lobby";
                case Competitive::ParticipantStatus::ready:
                    return "Ready";
                case Competitive::ParticipantStatus::playing:
                    return "Playing";
                case Competitive::ParticipantStatus::offline:
                    return "Offline — paused";
                case Competitive::ParticipantStatus::finished:
                    return "Finished";
                case Competitive::ParticipantStatus::forfeited:
                    return "Forfeited";
            }
            return "Unknown";
        }

        static const char* competitionPhaseName(Competitive::Phase phase)
        {
            switch (phase)
            {
                case Competitive::Phase::lobby:
                    return "Lobby";
                case Competitive::Phase::running:
                    return "Running";
                case Competitive::Phase::finished:
                    return "Finished";
                default:
                    return "Connecting";
            }
        }

        static const char* competitionMetricName(Competitive::Metric metric)
        {
            switch (metric)
            {
                case Competitive::Metric::points:
                    return "Points";
                case Competitive::Metric::rating:
                    return "Rating";
                case Competitive::Metric::guests:
                    return "Guests";
                case Competitive::Metric::cash:
                    return "Cash";
                case Competitive::Metric::parkValue:
                    return "Park value";
            }
            return "Score";
        }

        static std::string competitionMetricValue(const Competitive::Score& score, Competitive::Metric metric)
        {
            const auto value = Competitive::GetMetricValue(score, metric);
            if (metric == Competitive::Metric::cash || metric == Competitive::Metric::parkValue)
                return FormatStringID(STR_CURRENCY_FORMAT, static_cast<money64>(value));
            return std::to_string(value);
        }

        void competitionPaint(RenderTarget& rt)
        {
            const auto& session = Competitive::GetSession();
            const auto* state = session.GetState();
            if (state == nullptr)
            {
                drawText(rt, windowPos + ScreenCoordsXY{ 7, 70 }, session.GetStatusText(), { colours[1] });
                return;
            }
            std::string summary = std::string(competitionPhaseName(state->phase));
            const auto* local = Competitive::GetSession().GetLocalParticipant();
            if (local != nullptr && local->role != Competitive::Role::spectator)
            {
                if (state->scenario.noMoney)
                    summary += " — No-money scenario";
                else
                    summary += " — Park cash: "
                        + FormatStringID(STR_CURRENCY_FORMAT, Competitive::GetSession().GetAvailableParkCash());
            }
            if (state->phase == Competitive::Phase::finished && state->winnerId.has_value())
            {
                const auto* winner = Competitive::FindParticipant(*state, *state->winnerId);
                if (winner != nullptr)
                    summary += std::string(state->closedEarly ? " (resolved early)" : "") + " — Winner: " + winner->name;
            }
            summary += " — " + state->scenario.name;
            if (state->rules.victoryMode == Competitive::VictoryMode::deadline)
                summary += " — highest " + _scoreHeader + " at local Year " + std::to_string(state->rules.deadlineYear);
            else
            {
                std::string target;
                if (state->rules.metric == Competitive::Metric::cash
                    || state->rules.metric == Competitive::Metric::parkValue)
                    target = FormatStringID(STR_CURRENCY_FORMAT, static_cast<money64>(state->rules.target));
                else
                    target = std::to_string(state->rules.target);
                summary += " — first to " + target + " " + _scoreHeader;
            }
            if (state->phase == Competitive::Phase::lobby
                && Competitive::GetSession().GetMode() == Competitive::SessionMode::host)
            {
                const auto problems = Competitive::GetSession().GetStartProblems();
                if (!problems.empty())
                    summary = "Cannot start: " + problems.front() + " — " + summary;
            }
            drawTextEllipsised(rt, windowPos + ScreenCoordsXY{ 6, height - 43 }, width - 12, summary, { colours[1] });
        }

        void competitionScrollPaint(RenderTarget& rt) const
        {
            const auto* state = Competitive::GetSession().GetState();
            if (state == nullptr)
                return;
            int32_t y = 0;
            for (size_t row = 0; row < _competitionRows.size(); row++, y += kScrollableRowHeight)
            {
                const auto* participant = Competitive::FindParticipant(*state, _competitionRows[row]);
                const auto* score = Competitive::FindScore(*state, _competitionRows[row]);
                if (participant == nullptr || score == nullptr)
                    continue;
                if (static_cast<int32_t>(row) == selectedListItem)
                {
                    Rectangle::filter(rt, { 0, y, 1000, y + kScrollableRowHeight - 1 }, FilterPaletteID::paletteDarken1);
                }
                auto colour = participant->online ? colours[2] : ColourWithFlags{ Drawing::Colour::grey };
                drawText(rt, { 5, y }, std::to_string(row + 1), { colour });
                std::string name = participant->name;
                if (participant->id == Competitive::GetSession().GetLocalParticipantId())
                    name += " (you)";
                drawTextEllipsised(rt, { 45, y }, 214, name, { colour });
                drawTextEllipsised(rt, { 265, y }, 94, competitionStatusName(*participant, state->phase), { colour });
                drawText(rt, { 370, y }, std::to_string(participant->currentYear), { colour });
                drawTextEllipsised(rt, { 425, y }, 185, competitionMetricValue(*score, state->rules.metric), { colour });
            }
        }
    };

    WindowBase* MultiplayerOpen()
    {
        // Check if window is already open
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::multiplayer);
        if (window == nullptr)
        {
            window = windowMgr->Create<MultiplayerWindow>(
                WindowClass::multiplayer, { 320, 144 },
                { WindowFlag::higherContrastOnPress, WindowFlag::resizable, WindowFlag::autoPosition });
        }

        return window;
    }

    void MultiplayerRefreshList()
    {
        auto* windowMgr = GetWindowManager();
        auto w = static_cast<MultiplayerWindow*>(windowMgr->FindByClass(WindowClass::multiplayer));
        if (w != nullptr)
        {
            w->refreshList();
        }
    }
} // namespace OpenRCT2::Ui::Windows
