/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ENABLE_DISCORD__
    #include "DiscordService.h"

    #include "../Context.h"
    #include "../Diagnostic.h"
    #include "../GameState.h"
    #include "../OpenRCT2.h"
    #include "../core/Console.hpp"
    #include "../core/String.hpp"
    #include "../core/UTF8.h"
    #include "../localisation/Formatting.h"
    #include "../world/Park.h"
    #include "Network.h"

    #include <chrono>
    #include <discord_rpc.h>

namespace OpenRCT2::Network
{
    using namespace std::chrono_literals;

    constexpr const char* kApplicationID = "378612438200877056";
    constexpr const char* kSteamAppID = nullptr;
    constexpr auto kRefreshInterval = 5.0s;

    static void OnReady([[maybe_unused]] const DiscordUser* request)
    {
        LOG_VERBOSE("DiscordService::OnReady()");
    }

    static void OnDisconnected(int errorCode, const char* message)
    {
        Console::Error::WriteLine("DiscordService::OnDisconnected(%d, %s)", errorCode, message);
    }

    static void OnErrored(int errorCode, const char* message)
    {
        Console::Error::WriteLine("DiscordService::OnErrored(%d, %s)", errorCode, message);
    }

    DiscordService::DiscordService()
    {
        DiscordEventHandlers handlers = {};
        handlers.ready = OnReady;
        handlers.disconnected = OnDisconnected;
        handlers.errored = OnErrored;
        Discord_Initialize(kApplicationID, &handlers, 1, kSteamAppID);
    }

    DiscordService::~DiscordService()
    {
        Discord_Shutdown();
    }

    static std::string GetParkName()
    {
        auto& gameState = getGameState();
        return gameState.park.name;
    }

    void DiscordService::Tick()
    {
        Discord_RunCallbacks();

        if (_updateTimer.GetElapsedTime() < kRefreshInterval)
            return;

        RefreshPresence();
        _updateTimer.Restart();
    }

    void DiscordService::RefreshPresence() const
    {
        DiscordRichPresence discordPresence = {};
        discordPresence.largeImageKey = "logo";

        std::string state;
        std::string details;
        std::string partyId;

        switch (gLegacyScene)
        {
            default:
                details = GetParkName();
                if (GetMode() == Mode::none)
                {
                    state = "Playing Solo";
                }
                else
                {
                    FmtString fmtServerName(GetServerName());
                    std::string serverName;
                    for (const auto& token : fmtServerName)
                    {
                        if (token.IsLiteral())
                        {
                            serverName += token.text;
                        }
                        else if (token.IsCodepoint())
                        {
                            auto codepoint = token.GetCodepoint();
                            char buffer[8]{};
                            UTF8WriteCodepoint(buffer, codepoint);
                            serverName += buffer;
                        }
                    }
                    state = serverName;

                    partyId = GetServerName();
                    // NOTE: the party size is displayed next to state
                    discordPresence.partyId = partyId.c_str();
                    discordPresence.partySize = GetNumPlayers();
                    discordPresence.partyMax = 256;

                    // TODO generate secrets for the server
                    discordPresence.matchSecret = nullptr;
                    discordPresence.spectateSecret = nullptr;
                    discordPresence.instance = 1;
                }
                break;
            case LegacyScene::titleSequence:
                details = "In Menus";
                break;
            case LegacyScene::scenarioEditor:
                details = "In Scenario Editor";
                break;
            case LegacyScene::trackDesigner:
                details = "In Track Designer";
                break;
            case LegacyScene::trackDesignsManager:
                details = "In Track Designs Manager";
                break;
        }

        discordPresence.state = state.c_str();
        discordPresence.details = details.c_str();

        Discord_UpdatePresence(&discordPresence);
    }
} // namespace OpenRCT2::Network

#endif
