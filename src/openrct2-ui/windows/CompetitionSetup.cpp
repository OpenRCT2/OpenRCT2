/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/UiStringIds.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/competition/CompetitionManager.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/network/Network.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Map.h>

using namespace OpenRCT2::Competition;
using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    enum WindowCompetitionSetupWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CONTENT_PANEL,
        WIDX_TYPE_LABEL,
        WIDX_TYPE_DROPDOWN,
        WIDX_TYPE_DROPDOWN_BTN,
        WIDX_DURATION_LABEL,
        WIDX_DURATION_INCREASE,
        WIDX_DURATION_DECREASE,
        WIDX_DURATION_VALUE,
        WIDX_START_BUTTON,
        WIDX_VIEW_LEADERBOARD,
        WIDX_STOP_BUTTON,
    };

    static constexpr ScreenSize kWindowSize = { 320, 200 };

    // clang-format off
    static constexpr auto _competitionSetupWidgets = makeWidgets(
        makeWindowShim(STR_COMPETITION_SETUP, kWindowSize),
        makeWidget({   0,  43}, {320, 157}, WidgetType::resize,       WindowColour::secondary                        ),
        makeWidget({  10,  55}, {100,  14}, WidgetType::label,        WindowColour::secondary, STR_OBJECTIVE_LABEL   ),
        makeWidget({ 120,  55}, {175,  14}, WidgetType::dropdownMenu, WindowColour::secondary                        ),
        makeWidget({ 283,  56}, { 11,  12}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH    ),
        makeWidget({  10,  75}, {100,  14}, WidgetType::label,        WindowColour::secondary, STR_COMPETITION_DURATION),
        makeWidget({ 270,  75}, { 12,  14}, WidgetType::button,       WindowColour::secondary, STR_NUMERIC_UP        ),
        makeWidget({ 283,  75}, { 12,  14}, WidgetType::button,       WindowColour::secondary, STR_NUMERIC_DOWN      ),
        makeWidget({ 120,  75}, {145,  14}, WidgetType::label,        WindowColour::secondary                        ),
        makeWidget({  10, 160}, {140,  24}, WidgetType::button,       WindowColour::secondary, STR_START_COMPETITION ),
        makeWidget({ 170, 160}, {140,  24}, WidgetType::button,       WindowColour::secondary, STR_VIEW_LEADERBOARD  ),
        makeWidget({  10, 130}, {140,  24}, WidgetType::button,       WindowColour::secondary, STR_CANCEL            )
    );
    // clang-format on

    static constexpr StringId CompetitionTypeStrings[] = {
        STR_COMPETITION_TYPE_PARK_VALUE,
        STR_COMPETITION_TYPE_GUEST_COUNT,
        STR_COMPETITION_TYPE_RIDE_INCOME,
        STR_COMPETITION_TYPE_SPEED,
        STR_COMPETITION_TYPE_SURVIVAL,
        STR_COMPETITION_TYPE_RIDE_MASTER,
    };

    class CompetitionSetupWindow final : public Window
    {
    private:
        CompetitionType _selectedType = CompetitionType::ParkValue;
        uint32_t _durationMinutes = 30;

    public:
        void onOpen() override
        {
            setWidgets(_competitionSetupWidgets);
            initScrollWidgets();
            WindowInitScrollWidgets(*this);

            colours[0] = Colour::grey;
            colours[1] = Colour::grey;
            colours[2] = Colour::grey;

            _selectedType = CompetitionType::ParkValue;
            _durationMinutes = 30;

            UpdateWidgetState();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_START_BUTTON:
                    StartCompetition();
                    break;
                case WIDX_STOP_BUTTON:
                    StopCompetition();
                    break;
                case WIDX_VIEW_LEADERBOARD:
                    ContextOpenWindow(WindowClass::competitionLeaderboard);
                    break;
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_TYPE_DROPDOWN_BTN:
                    ShowTypeDropdown();
                    break;
                case WIDX_DURATION_INCREASE:
                    if (_durationMinutes < 180)
                    {
                        _durationMinutes += 5;
                        invalidate();
                    }
                    break;
                case WIDX_DURATION_DECREASE:
                    if (_durationMinutes > 5)
                    {
                        _durationMinutes -= 5;
                        invalidate();
                    }
                    break;
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (widgetIndex == WIDX_TYPE_DROPDOWN_BTN && dropdownIndex >= 0 && dropdownIndex < 6)
            {
                _selectedType = static_cast<CompetitionType>(dropdownIndex + 1);
                invalidate();
            }
        }

        void onUpdate() override
        {
            UpdateWidgetState();
            invalidate();
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);

            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            // Draw selected competition type in dropdown
            auto typeWidget = &widgets[WIDX_TYPE_DROPDOWN];
            auto screenCoords = windowPos + ScreenCoordsXY{ typeWidget->left + 2, typeWidget->top + 2 };
            DrawTextBasic(rt, screenCoords, GetCompetitionTypeString(_selectedType));

            // Draw duration value
            auto durationWidget = &widgets[WIDX_DURATION_VALUE];
            screenCoords = windowPos + ScreenCoordsXY{ durationWidget->left + 2, durationWidget->top + 2 };
            auto ft = Formatter();
            ft.Add<uint32_t>(_durationMinutes);
            DrawTextBasic(rt, screenCoords, STR_COMMA16, ft);

            // Draw competition type description
            screenCoords = windowPos + ScreenCoordsXY{ 10, 100 };
            DrawCompetitionTypeDescription(rt, screenCoords);

            // Draw current status
            screenCoords = windowPos + ScreenCoordsXY{ 10, 115 };
            if (competitionMgr)
            {
                if (competitionMgr->IsActive())
                {
                    DrawTextBasic(rt, screenCoords, STR_COMPETITION_STARTED, {}, { Colour::brightGreen });

                    // Show time remaining
                    screenCoords.y += 14;
                    uint32_t timeRemaining = competitionMgr->GetTimeRemaining();
                    uint32_t minutes = timeRemaining / 60;
                    uint32_t seconds = timeRemaining % 60;

                    ft = Formatter();
                    ft.Add<uint32_t>(minutes);
                    ft.Add<uint32_t>(seconds);
                    DrawTextBasic(rt, screenCoords, STR_TIME_REMAINING_FORMAT, ft);
                }
                else if (competitionMgr->IsInLobby())
                {
                    DrawTextBasic(rt, screenCoords, STR_COMPETITION_STARTING, {}, { Colour::yellow });
                }
                else if (competitionMgr->IsFinished())
                {
                    DrawTextBasic(rt, screenCoords, STR_COMPETITION_ENDED, {}, { Colour::brightRed });
                }
                else
                {
                    DrawTextBasic(rt, screenCoords, STR_NO_ACTIVE_COMPETITION, {}, { Colour::grey });
                }
            }
            else
            {
                DrawTextBasic(rt, screenCoords, STR_NO_ACTIVE_COMPETITION, {}, { Colour::grey });
            }
        }

    private:
        void ShowTypeDropdown()
        {
            auto widget = &widgets[WIDX_TYPE_DROPDOWN];

            for (int32_t i = 0; i < 6; i++)
            {
                gDropdown.items[i] = Dropdown::PlainMenuLabel(CompetitionTypeStrings[i]);
            }

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1,
                colours[1], 0, Dropdown::Flag::StayOpen, 6, widget->width() - 3);

            gDropdown.defaultIndex = static_cast<int32_t>(_selectedType) - 1;
        }

        StringId GetCompetitionTypeString(CompetitionType type) const
        {
            int32_t index = static_cast<int32_t>(type) - 1;
            if (index >= 0 && index < 6)
            {
                return CompetitionTypeStrings[index];
            }
            return STR_COMPETITIONS;
        }

        void DrawCompetitionTypeDescription(RenderTarget& rt, const ScreenCoordsXY& coords) const
        {
            // Draw a brief description of the selected competition type
            StringId description = STR_COMPETITIONS;
            switch (_selectedType)
            {
                case CompetitionType::ParkValue:
                    description = STR_OBJECTIVE_PARK_VALUE_BY;
                    break;
                case CompetitionType::GuestCount:
                    description = STR_OBJECTIVE_GUESTS_BY;
                    break;
                case CompetitionType::RideIncome:
                    description = STR_OBJECTIVE_MONTHLY_RIDE_INCOME;
                    break;
                case CompetitionType::FastestGoal:
                    description = STR_OBJECTIVE_HAVE_FUN;
                    break;
                case CompetitionType::Survival:
                    description = STR_OBJECTIVE_10_ROLLERCOASTERS;
                    break;
                case CompetitionType::RideMaster:
                    description = STR_OBJECTIVE_BUILD_THE_BEST;
                    break;
                default:
                    break;
            }
            DrawTextBasic(rt, coords, description);
        }

        void UpdateWidgetState()
        {
            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            bool isActive = competitionMgr && (competitionMgr->IsActive() || competitionMgr->IsInLobby());

            // Disable setup controls if competition is running
            setWidgetDisabled(WIDX_TYPE_DROPDOWN, isActive);
            setWidgetDisabled(WIDX_TYPE_DROPDOWN_BTN, isActive);
            setWidgetDisabled(WIDX_DURATION_INCREASE, isActive);
            setWidgetDisabled(WIDX_DURATION_DECREASE, isActive);
            setWidgetDisabled(WIDX_START_BUTTON, isActive);

            // Enable stop button only if competition is running
            setWidgetDisabled(WIDX_STOP_BUTTON, !isActive);

            // Show stop button when competition is active, hide when not active
            if (isActive)
            {
                widgets[WIDX_STOP_BUTTON].flags.unset(WidgetFlag::isHidden);
            }
            else
            {
                widgets[WIDX_STOP_BUTTON].flags.set(WidgetFlag::isHidden);
            }
        }

        void StartCompetition()
        {
            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            if (!competitionMgr)
                return;

            // Don't start if already running
            if (competitionMgr->IsActive())
                return;

            // Configure competition
            CompetitionConfig config;
            config.Type = _selectedType;
            config.DurationMinutes = _durationMinutes;
            config.MaxPlayers = 8;
            config.EnableChat = true;
            config.ShowLiveScores = true;

            // Initialize competition
            competitionMgr->Initialize(config);

            // Add players from network
#ifndef DISABLE_NETWORK
            // Get current player info
            auto currentPlayerId = Network::GetCurrentPlayerId();
            auto playerIndex = Network::GetPlayerIndex(currentPlayerId);
            if (playerIndex >= 0)
            {
                auto playerName = Network::GetPlayerName(playerIndex);
                competitionMgr->AddPlayer(currentPlayerId, playerName);
            }

            // Add other network players if server
            if (Network::GetMode() == Network::Mode::server)
            {
                for (int32_t i = 0; i < Network::GetNumPlayers(); i++)
                {
                    auto id = Network::GetPlayerID(i);
                    if (id != currentPlayerId)
                    {
                        competitionMgr->AddPlayer(id, Network::GetPlayerName(i));
                    }
                }
            }
#else
            // Single player mode - add host player
            competitionMgr->AddPlayer(1, "Player");
#endif

            // Initialize territories based on map size
            uint32_t mapWidth = getGameState().mapSize.x;
            uint32_t mapHeight = getGameState().mapSize.y;
            competitionMgr->InitializeTerritories(mapWidth, mapHeight);

            // Start competition
            competitionMgr->Start();

            UpdateWidgetState();
            invalidate();

            // Open leaderboard to show progress
            ContextOpenWindow(WindowClass::competitionLeaderboard);
        }

        void StopCompetition()
        {
            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            if (competitionMgr && (competitionMgr->IsActive() || competitionMgr->IsInLobby()))
            {
                competitionMgr->End();
                competitionMgr->Reset();
            }

            UpdateWidgetState();
            invalidate();
        }
    };

    WindowBase* CompetitionSetupOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::competitionSetup);
        if (window == nullptr)
        {
            window = windowMgr->Create<CompetitionSetupWindow>(
                WindowClass::competitionSetup, kWindowSize, { WindowFlag::autoPosition });
        }
        return window;
    }

} // namespace OpenRCT2::Ui::Windows
