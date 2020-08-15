/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#define NETWORK_DEFAULT_PORT 11753
#define NETWORK_LAN_BROADCAST_PORT 11754
#define NETWORK_LAN_BROADCAST_MSG "openrct2.server.query"
#define MAX_SERVER_DESCRIPTION_LENGTH 256

#include "../common.h"
#include "../localisation/StringIds.h"
#include "NetworkTypes.h"

#include <memory>
#include <string>
#include <vector>

struct json_t;
struct GameAction;
struct Peep;
struct CoordsXYZ;
class GameActionResult;
enum class ModifyGroupType : uint8_t;
enum class PermissionState : uint8_t;

namespace OpenRCT2
{
    struct IPlatformEnvironment;
}

void network_set_env(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
void network_close();
void network_reconnect();
void network_shutdown_client();
int32_t network_begin_client(const std::string& host, int32_t port);
int32_t network_begin_server(int32_t port, const std::string& address);

int32_t network_get_mode();
int32_t network_get_status();
bool network_is_desynchronised();
bool network_check_desynchronisation();
void network_request_gamestate_snapshot();
void network_send_tick();
bool network_gamestate_snapshots_enabled();
void network_update();
void network_process_pending();
void network_flush();

int32_t network_get_authstatus();
uint32_t network_get_server_tick();
uint8_t network_get_current_player_id();
int32_t network_get_num_players();
const char* network_get_player_name(uint32_t index);
uint32_t network_get_player_flags(uint32_t index);
int32_t network_get_player_ping(uint32_t index);
int32_t network_get_player_id(uint32_t index);
money32 network_get_player_money_spent(uint32_t index);
std::string network_get_player_ip_address(uint32_t id);
std::string network_get_player_public_key_hash(uint32_t id);
void network_add_player_money_spent(uint32_t index, money32 cost);
int32_t network_get_player_last_action(uint32_t index, int32_t time);
void network_set_player_last_action(uint32_t index, int32_t command);
CoordsXYZ network_get_player_last_action_coord(uint32_t index);
void network_set_player_last_action_coord(uint32_t index, const CoordsXYZ& coord);
uint32_t network_get_player_commands_ran(uint32_t index);
int32_t network_get_player_index(uint32_t id);
uint8_t network_get_player_group(uint32_t index);
void network_set_player_group(uint32_t index, uint32_t groupindex);
int32_t network_get_group_index(uint8_t id);
int32_t network_get_current_player_group_index();
uint8_t network_get_group_id(uint32_t index);
int32_t network_get_num_groups();
const char* network_get_group_name(uint32_t index);
std::unique_ptr<GameActionResult> network_set_player_group(
    NetworkPlayerId_t actionPlayerId, NetworkPlayerId_t playerId, uint8_t groupId, bool isExecuting);
std::unique_ptr<GameActionResult> network_modify_groups(
    NetworkPlayerId_t actionPlayerId, ModifyGroupType type, uint8_t groupId, const std::string& name, uint32_t permissionIndex,
    PermissionState permissionState, bool isExecuting);
std::unique_ptr<GameActionResult> network_kick_player(NetworkPlayerId_t playerId, bool isExecuting);
uint8_t network_get_default_group();
int32_t network_get_num_actions();
rct_string_id network_get_action_name_string_id(uint32_t index);
int32_t network_can_perform_action(uint32_t groupindex, uint32_t index);
int32_t network_can_perform_command(uint32_t groupindex, int32_t index);
void network_set_pickup_peep(uint8_t playerid, Peep* peep);
Peep* network_get_pickup_peep(uint8_t playerid);
void network_set_pickup_peep_old_x(uint8_t playerid, int32_t x);
int32_t network_get_pickup_peep_old_x(uint8_t playerid);

void network_send_map();
void network_send_chat(const char* text, const std::vector<uint8_t>& playerIds = {});
void network_send_game_action(const GameAction* action);
void network_enqueue_game_action(const GameAction* action);
void network_send_password(const std::string& password);

void network_set_password(const char* password);

void network_print_error();
void network_append_chat_log(const utf8* text);
void network_append_server_log(const utf8* text);
const utf8* network_get_server_name();
const utf8* network_get_server_description();
const utf8* network_get_server_greeting();
const utf8* network_get_server_provider_name();
const utf8* network_get_server_provider_email();
const utf8* network_get_server_provider_website();

std::string network_get_version();

NetworkStats_t network_get_stats();
NetworkServerState_t network_get_server_state();
json_t* network_get_server_info_as_json();
