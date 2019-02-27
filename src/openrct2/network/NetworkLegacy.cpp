/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Cheats.h"
#include "../ParkImporter.h"
#include "../Version.h"
#include "../actions/GameAction.h"
#include "../actions/PauseToggleAction.hpp"
#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/FileStream.hpp"
#include "../core/Json.hpp"
#include "../core/MemoryStream.h"
#include "../core/Nullable.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../interface/Chat.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../rct2/S6Exporter.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../world/Park.h"

int32_t network_get_mode()
{
    return NETWORK_MODE_NONE;
}
int32_t network_get_status()
{
    return NETWORK_STATUS_NONE;
}
int32_t network_get_authstatus()
{
    return NETWORK_AUTH_NONE;
}

uint32_t network_get_server_tick()
{
    return 0;
}

void network_flush()
{
}

void network_send_tick()
{
}

void network_check_desynchronization()
{
}

void network_enqueue_game_action(const GameAction* action)
{
}

void network_send_gamecmd(
    uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint8_t callback)
{
}

void network_send_game_action(const GameAction* action)
{
}

void network_send_map()
{
}

void network_update()
{
}

void network_process_pending()
{
}

int32_t network_begin_client(const std::string& host, int32_t port)
{
    return 1;
}

int32_t network_begin_server(int32_t port, const std::string& address)
{
    return 1;
}

int32_t network_get_num_players()
{
    return 1;
}

const char* network_get_player_name(uint32_t index)
{
    return "local (OpenRCT2 compiled without MP)";
}

uint32_t network_get_player_flags(uint32_t index)
{
    return 0;
}

int32_t network_get_player_ping(uint32_t index)
{
    return 0;
}

int32_t network_get_player_id(uint32_t index)
{
    return 0;
}

money32 network_get_player_money_spent(uint32_t index)
{
    return MONEY(0, 0);
}

void network_add_player_money_spent(uint32_t index, money32 cost)
{
}

int32_t network_get_player_last_action(uint32_t index, int32_t time)
{
    return -999;
}

void network_set_player_last_action(uint32_t index, int32_t command)
{
}

LocationXYZ16 network_get_player_last_action_coord(uint32_t index)
{
    return { 0, 0, 0 };
}

void network_set_player_last_action_coord(uint32_t index, LocationXYZ16 coord)
{
}

uint32_t network_get_player_commands_ran(uint32_t index)
{
    return 0;
}

int32_t network_get_player_index(uint32_t id)
{
    return -1;
}

uint8_t network_get_player_group(uint32_t index)
{
    return 0;
}

void network_set_player_group(uint32_t index, uint32_t groupindex)
{
}

int32_t network_get_group_index(uint8_t id)
{
    return -1;
}

uint8_t network_get_group_id(uint32_t index)
{
    return 0;
}

int32_t network_get_num_groups()
{
    return 0;
}

const char* network_get_group_name(uint32_t index)
{
    return "";
};

void game_command_set_player_group(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp)
{
}

void game_command_modify_groups(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp)
{
}

void game_command_kick_player(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp)
{
}

uint8_t network_get_default_group()
{
    return 0;
}

int32_t network_get_num_actions()
{
    return 0;
}

rct_string_id network_get_action_name_string_id(uint32_t index)
{
    return 0;
}

int32_t network_can_perform_action(uint32_t groupindex, uint32_t index)
{
    return 0;
}

int32_t network_can_perform_command(uint32_t groupindex, int32_t index)
{
    return 0;
}

void network_set_pickup_peep(uint8_t playerid, rct_peep* peep)
{
}

rct_peep* network_get_pickup_peep(uint8_t playerid)
{
    return nullptr;
}

void network_set_pickup_peep_old_x(uint8_t playerid, int32_t x)
{
}

int32_t network_get_pickup_peep_old_x(uint8_t playerid)
{
    return 0;
}

void network_send_chat(const char* text)
{
}

void network_send_password(const std::string& password)
{
}

void network_close()
{
}

void network_set_env(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>&)
{
}

void network_shutdown_client()
{
}

void network_set_password(const char* password)
{
}

uint8_t network_get_current_player_id()
{
    return 0;
}

int32_t network_get_current_player_group_index()
{
    return 0;
}

void network_append_chat_log(const utf8* text)
{
}
void network_append_server_log(const utf8* text)
{
}
const utf8* network_get_server_name()
{
    return nullptr;
}
const utf8* network_get_server_description()
{
    return nullptr;
}
const utf8* network_get_server_greeting()
{
    return nullptr;
}
const utf8* network_get_server_provider_name()
{
    return nullptr;
}
const utf8* network_get_server_provider_email()
{
    return nullptr;
}
const utf8* network_get_server_provider_website()
{
    return nullptr;
}
std::string network_get_version()
{
    return "Multiplayer disabled";
}
NetworkStats_t network_get_stats()
{
    return NetworkStats_t{};
}
