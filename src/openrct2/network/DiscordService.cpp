/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ENABLE_DISCORD__
#    include "DiscordService.h"

#    include "../Context.h"
#    include "../GameState.h"
#    include "../OpenRCT2.h"
#    include "../core/Console.hpp"
#    include "../core/String.hpp"
#    include "../localisation/Formatting.h"
#    include "../localisation/Localisation.h"
#    include "../world/Park.h"
#    include "network.h"

#    include <chrono>
#    include <discord_rpc.h>

namespace
{
    using namespace std::chrono_literals;

    constexpr const char* APPLICATION_ID = "378612438200877056";
    constexpr const char* STEAM_APP_ID = nullptr;
    constexpr const auto REFRESH_INTERVAL = 5.0s;
} // namespace

static void OnReady([[maybe_unused]] const DiscordUser* request)
{
    log_verbose("DiscordService::OnReady()");
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
    Discord_Initialize(APPLICATION_ID, &handlers, 1, STEAM_APP_ID);
}

DiscordService::~DiscordService()
{
    Discord_Shutdown();
}

static std::string GetParkName()
{
    auto gameState = OpenRCT2::GetContext()->GetGameState();
    if (gameState != nullptr)
    {
        return gameState->GetPark().Name;
    }
    return {};
}

void DiscordService::Tick()
{
    Discord_RunCallbacks();

    if (_updateTimer.GetElapsedTime() < REFRESH_INTERVAL)
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
    switch (gScreenFlags)
    {
        default:
            details = GetParkName();
            if (network_get_mode() == NETWORK_MODE_NONE)
            {
                state = "Playing Solo";
            }
            else
            {
                OpenRCT2::FmtString fmtServerName(network_get_server_name());
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
                        utf8_write_codepoint(buffer, codepoint);
                        serverName += buffer;
                    }
                }
                state = serverName;

                // NOTE: the party size is displayed next to state
                discordPresence.partyId = network_get_server_name().c_str();
                discordPresence.partySize = network_get_num_players();
                discordPresence.partyMax = 256;

                // TODO generate secrets for the server
                discordPresence.matchSecret = nullptr;
                discordPresence.spectateSecret = nullptr;
                discordPresence.instance = 1;
            }
            break;
        case SCREEN_FLAGS_TITLE_DEMO:
            details = "In Menus";
            break;
        case SCREEN_FLAGS_SCENARIO_EDITOR:
            details = "In Scenario Editor";
            break;
        case SCREEN_FLAGS_TRACK_DESIGNER:
            details = "In Track Designer";
            break;
        case SCREEN_FLAGS_TRACK_MANAGER:
            details = "In Track Designs Manager";
            break;
    }

    discordPresence.state = state.c_str();
    discordPresence.details = details.c_str();

    Discord_UpdatePresence(&discordPresence);
}

#endif
