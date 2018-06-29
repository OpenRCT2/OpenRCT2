/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ENABLE_DISCORD__

#include <discord_rpc.h>
#include "../Context.h"
#include "../core/Console.hpp"
#include "../core/String.hpp"
#include "../localisation/Localisation.h"
#include "../OpenRCT2.h"
#include "../world/Park.h"
#include "DiscordService.h"
#include "network.h"

constexpr const char * APPLICATION_ID = "378612438200877056";
constexpr const char * STEAM_APP_ID = nullptr;
constexpr const uint32_t REFRESH_INTERVAL = 5 * GAME_UPDATE_FPS; // 5 seconds

static void OnReady([[maybe_unused]] const DiscordUser* request)
{
    log_verbose("DiscordService::OnReady()");
}

static void OnDisconnected(int errorCode, const char * message)
{
    Console::Error::WriteLine("DiscordService::OnDisconnected(%d, %s)", errorCode, message);
}

static void OnErrored(int errorCode, const char * message)
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
    utf8 parkName[128] = {};
    format_string(parkName, sizeof(parkName), gParkName, &gParkNameArgs);
    return std::string(parkName);
}

void DiscordService::Update()
{
    Discord_RunCallbacks();

    if (_ticksSinceLastRefresh >= REFRESH_INTERVAL)
    {
        _ticksSinceLastRefresh = 0;
        RefreshPresence();
    }
    else
    {
        _ticksSinceLastRefresh++;
    }
}

void DiscordService::RefreshPresence()
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
            state = String::ToStd(network_get_server_name());

            // NOTE: the party size is displayed next to state
            discordPresence.partyId = network_get_server_name();
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
