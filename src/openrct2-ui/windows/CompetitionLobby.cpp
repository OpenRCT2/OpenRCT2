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
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/network/Network.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Competition;
using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    enum WindowCompetitionLobbyWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CONTENT_PANEL,
        WIDX_PLAYER_LIST,
        WIDX_READY_BUTTON,
        WIDX_START_BUTTON,
        WIDX_LEAVE_BUTTON,
        WIDX_INFO_PANEL,
    };

    static constexpr ScreenSize kWindowSize = { 400, 350 };
    static constexpr int32_t kPlayerListItemHeight = 16;

    // clang-format off
    static constexpr auto _lobbyWidgets = makeWidgets(
        makeWindowShim(STR_COMPETITION_LOBBY, kWindowSize),
        makeWidget({   0,  43}, {400, 307}, WidgetType::resize,  WindowColour::secondary                      ),
        makeWidget({   5,  90}, {390, 180}, WidgetType::scroll,  WindowColour::secondary, SCROLL_VERTICAL     ),
        makeWidget({  10, 280}, {180,  24}, WidgetType::button,  WindowColour::secondary, STR_TOGGLE_READY    ),
        makeWidget({ 200, 280}, {190,  24}, WidgetType::button,  WindowColour::secondary, STR_START_COMPETITION_HOST),
        makeWidget({ 200, 310}, {190,  24}, WidgetType::button,  WindowColour::secondary, STR_LEAVE_LOBBY     ),
        makeWidget({   5,  55}, {390,  30}, WidgetType::groupbox, WindowColour::secondary                      )
    );
    // clang-format on

    class CompetitionLobbyWindow final : public Window
    {
    private:
        uint8_t _localPlayerId = 0;
        bool _isHost = false;

    public:
        void onOpen() override
        {
            setWidgets(_lobbyWidgets);
            initScrollWidgets();
            WindowInitScrollWidgets(*this);

            colours[0] = Colour::grey;
            colours[1] = Colour::grey;
            colours[2] = Colour::grey;

            minWidth = kWindowSize.width;
            minHeight = kWindowSize.height;
            maxWidth = kWindowSize.width;
            maxHeight = kWindowSize.height;

            _localPlayerId = Network::GetCurrentPlayerId();
            _isHost = (_localPlayerId == 0);
        }

        void onUpdate() override
        {
            invalidate();
            
            // Auto-refresh lobby state
            static uint32_t lastUpdate = 0;
            uint32_t currentTick = gCurrentTicks;
            if (currentTick - lastUpdate > 20) // Update every ~0.5 seconds
            {
                lastUpdate = currentTick;
                WidgetScrollUpdateThumbs(*this, WIDX_PLAYER_LIST);
            }
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                case WIDX_LEAVE_BUTTON:
                    LeaveLobby();
                    close();
                    break;
                case WIDX_READY_BUTTON:
                    ToggleReady();
                    break;
                case WIDX_START_BUTTON:
                    StartCompetition();
                    break;
            }
        }

        void onPrepareDraw() override
        {
            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            if (!competitionMgr || !competitionMgr->IsInLobby())
            {
                close();
                return;
            }

            // Update start button state
            bool canStart = competitionMgr->CanStartCompetition();
            widgets[WIDX_START_BUTTON].disabled = !_isHost || !canStart;
        }

        void onDraw(DrawPixelInfo& dpi) override
        {
            WindowDrawWidgets(*this, dpi);

            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            if (!competitionMgr || !competitionMgr->IsInLobby())
                return;

            const auto& config = competitionMgr->GetConfig();
            
            // Draw competition info
            auto screenCoords = windowPos + ScreenCoordsXY{ 10, 60 };
            
            // Competition type
            auto ft = Formatter();
            ft.Add<StringId>(GetCompetitionTypeString(config.Type));
            DrawTextBasic(dpi, screenCoords, STR_COMPETITION_TYPE_LABEL, ft);
            
            screenCoords.y += 14;
            
            // Duration
            ft = Formatter();
            ft.Add<int16_t>(config.DurationMinutes);
            DrawTextBasic(dpi, screenCoords, STR_DURATION_LABEL, ft);
            DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 150, 0 }, STR_COMMA16, ft);
            
            // Player count
            screenCoords = windowPos + ScreenCoordsXY{ 250, 60 };
            uint8_t playerCount = competitionMgr->GetLobbyPlayerCount();
            ft = Formatter();
            ft.Add<int16_t>(playerCount);
            ft.Add<int16_t>(config.MaxPlayers);
            DrawTextBasic(dpi, screenCoords, STR_PLAYERS_COUNT_FORMAT, ft);

            // Status message
            screenCoords = windowPos + ScreenCoordsXY{ 10, 275 };
            if (competitionMgr->AreAllPlayersReady() && playerCount >= 2)
            {
                DrawTextBasic(dpi, screenCoords, STR_ALL_PLAYERS_READY);
            }
            else if (playerCount < 2)
            {
                DrawTextBasic(dpi, screenCoords, STR_WAITING_FOR_PLAYERS);
            }
            else
            {
                DrawTextBasic(dpi, screenCoords, STR_WAITING_FOR_PLAYERS);
            }
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            if (!competitionMgr || !competitionMgr->IsInLobby())
                return { 0, 0 };

            const auto& players = competitionMgr->GetAllPlayerData();
            int32_t height = static_cast<int32_t>(players.size()) * kPlayerListItemHeight;
            
            return { 0, height };
        }

        void onScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            if (!competitionMgr || !competitionMgr->IsInLobby())
                return;

            const auto& players = competitionMgr->GetAllPlayerData();
            
            auto screenCoords = ScreenCoordsXY{ 0, 0 };

            for (const auto& player : players)
            {
                // Draw player name
                auto ft = Formatter();
                ft.Add<const char*>(player.PlayerName.c_str());
                DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 5, 0 }, STR_STRING, ft);

                // Draw ready status
                StringId statusStr = player.IsReady ? STR_READY : STR_NOT_READY;
                Colour statusColour = player.IsReady ? Colour::green : Colour::red;
                
                auto statusCoords = screenCoords + ScreenCoordsXY{ 250, 0 };
                DrawTextBasic(dpi, statusCoords, statusStr, {}, { statusColour });

                screenCoords.y += kPlayerListItemHeight;
            }
        }

    private:
        void ToggleReady()
        {
            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            if (!competitionMgr || !competitionMgr->IsInLobby())
                return;

            bool currentReady = competitionMgr->IsPlayerReady(_localPlayerId);
            
            #ifndef DISABLE_NETWORK
            if (Network::GetMode() == Network::Mode::client)
            {
                auto* network = Network::GetNetwork();
                if (network)
                {
                    static_cast<Network::NetworkBase*>(network)->Client_Send_LobbyPlayerReady(!currentReady);
                }
            }
            else
            #endif
            {
                competitionMgr->SetPlayerReady(_localPlayerId, !currentReady);
            }
        }

        void StartCompetition()
        {
            if (!_isHost)
                return;

            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            if (!competitionMgr || !competitionMgr->CanStartCompetition())
                return;

            #ifndef DISABLE_NETWORK
            if (Network::GetMode() == Network::Mode::server)
            {
                auto* network = Network::GetNetwork();
                if (network)
                {
                    competitionMgr->Start();
                    auto& config = competitionMgr->GetConfig();
                    static_cast<Network::NetworkBase*>(network)->ServerSendCompetitionStart(config);
                }
            }
            else
            #endif
            {
                competitionMgr->Start();
            }

            close();
        }

        void LeaveLobby()
        {
            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            if (!competitionMgr || !competitionMgr->IsInLobby())
                return;

            #ifndef DISABLE_NETWORK
            if (Network::GetMode() == Network::Mode::client)
            {
                auto* network = Network::GetNetwork();
                if (network)
                {
                    static_cast<Network::NetworkBase*>(network)->Client_Send_LobbyLeave();
                }
            }
            #endif

            competitionMgr->RemovePlayerFromLobby(_localPlayerId);
        }

        StringId GetCompetitionTypeString(CompetitionType type)
        {
            switch (type)
            {
                case CompetitionType::ParkValue:
                    return STR_COMPETITION_TYPE_PARK_VALUE;
                case CompetitionType::GuestCount:
                    return STR_COMPETITION_TYPE_GUEST_COUNT;
                case CompetitionType::RideIncome:
                    return STR_COMPETITION_TYPE_RIDE_INCOME;
                case CompetitionType::FastestGoal:
                    return STR_COMPETITION_TYPE_SPEED;
                case CompetitionType::Survival:
                    return STR_COMPETITION_TYPE_SURVIVAL;
                case CompetitionType::RideMaster:
                    return STR_COMPETITION_TYPE_RIDE_MASTER;
                default:
                    return STR_NONE;
            }
        }
    };

    WindowBase* CompetitionLobbyOpen()
    {
        auto* window = WindowBringToFrontByClass(WindowClass::competitionLobby);
        if (window == nullptr)
        {
            window = WindowCreate<CompetitionLobbyWindow>(
                WindowClass::competitionLobby, kWindowSize, WindowFlags::windowAutoCentre);
        }
        return window;
    }

} // namespace OpenRCT2::Ui::Windows
