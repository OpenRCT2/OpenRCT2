/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#define NETWORK_DEFAULT_PORT 11753
#define MAX_SERVER_DESCRIPTION_LENGTH 256

#include "../common.h"
#include "../localisation/StringIds.h"
#include "NetworkTypes.h"

#include <memory>
#include <string>

struct GameAction;
struct rct_peep;
struct LocationXYZ16;

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

void network_set_env(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
void network_close();
void network_shutdown_client();
int32_t network_begin_client(const char* host, int32_t port);
int32_t network_begin_server(int32_t port, const char* address);

int32_t network_get_mode();
int32_t network_get_status();
void network_check_desynchronization();
void network_send_tick();
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
void network_add_player_money_spent(uint32_t index, money32 cost);
int32_t network_get_player_last_action(uint32_t index, int32_t time);
void network_set_player_last_action(uint32_t index, int32_t command);
LocationXYZ16 network_get_player_last_action_coord(uint32_t index);
void network_set_player_last_action_coord(uint32_t index, LocationXYZ16 coord);
uint32_t network_get_player_commands_ran(uint32_t index);
int32_t network_get_player_index(uint32_t id);
uint8_t network_get_player_group(uint32_t index);
void network_set_player_group(uint32_t index, uint32_t groupindex);
int32_t network_get_group_index(uint8_t id);
int32_t network_get_current_player_group_index();
uint8_t network_get_group_id(uint32_t index);
int32_t network_get_num_groups();
const char* network_get_group_name(uint32_t index);
void game_command_set_player_group(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_modify_groups(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_kick_player(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
uint8_t network_get_default_group();
int32_t network_get_num_actions();
rct_string_id network_get_action_name_string_id(uint32_t index);
int32_t network_can_perform_action(uint32_t groupindex, uint32_t index);
int32_t network_can_perform_command(uint32_t groupindex, int32_t index);
void network_set_pickup_peep(uint8_t playerid, rct_peep* peep);
rct_peep* network_get_pickup_peep(uint8_t playerid);
void network_set_pickup_peep_old_x(uint8_t playerid, int32_t x);
int32_t network_get_pickup_peep_old_x(uint8_t playerid);

void network_send_map();
void network_send_chat(const char* text);
void network_send_gamecmd(
    uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint8_t callback);
void network_send_game_action(const GameAction* action);
void network_enqueue_game_action(const GameAction* action);
void network_send_password(const char* password);

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
