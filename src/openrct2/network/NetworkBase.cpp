/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NetworkBase.h"

#include "../Context.h"
#include "../Game.h"
#include "../GameStateSnapshots.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../actions/LoadOrQuitAction.h"
#include "../actions/NetworkModifyGroupAction.h"
#include "../actions/PeepPickupAction.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../platform/Platform2.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../util/SawyerCoding.h"
#include "../world/Location.hpp"
#include "network.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>

 // This string specifies which version of network stream current build uses.
 // It is used for making sure only compatible builds get connected, even within
 // single OpenRCT2 version.
#define NETWORK_STREAM_VERSION "15"
#define NETWORK_STREAM_ID OPENRCT2_VERSION "-" NETWORK_STREAM_VERSION

static Peep* _pickup_peep = nullptr;
static int32_t _pickup_peep_old_x = LOCATION_NULL;

#ifndef DISABLE_NETWORK

#    include "../Cheats.h"
#    include "../Version.h"
#    include "../actions/GameAction.h"
#    include "../config/Config.h"
#    include "../core/Console.hpp"
#    include "../core/FileStream.h"
#    include "../core/MemoryStream.h"
#    include "../core/Nullable.hpp"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../interface/Chat.h"
#    include "../interface/Window.h"
#    include "../localisation/Date.h"
#    include "../localisation/Localisation.h"
#    include "../object/ObjectManager.h"
#    include "../object/ObjectRepository.h"
#    include "../rct2/S6Exporter.h"
#    include "../scenario/Scenario.h"
#    include "../util/Util.h"
#    include "../world/Park.h"
#    include "NetworkAction.h"
#    include "NetworkConnection.h"
#    include "NetworkGroup.h"
#    include "NetworkKey.h"
#    include "NetworkPacket.h"
#    include "NetworkPlayer.h"
#    include "NetworkServerAdvertiser.h"
#    include "NetworkUser.h"
#    include "Socket.h"

#    include <algorithm>
#    include <array>
#    include <cerrno>
#    include <cmath>
#    include <fstream>
#    include <functional>
#    include <list>
#    include <map>
#    include <memory>
#    include <set>
#    include <string>
#    include <vector>

using namespace OpenRCT2;

NetworkBase::NetworkBase(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env)
    : _env(env)
{
    _chat_log_fs << std::unitbuf;
}

bool NetworkBase::Init()
{
    status = NETWORK_STATUS_READY;

    ServerName = std::string();
    ServerDescription = std::string();
    ServerGreeting = std::string();
    ServerProviderName = std::string();
    ServerProviderEmail = std::string();
    ServerProviderWebsite = std::string();
    return true;
}

void NetworkBase::Close()
{
    if (status == NETWORK_STATUS_NONE)
        return;

    CloseChatLog();

    GameActions::ClearQueue();
    GameActions::ResumeQueue();

    _playerList.clear();
    group_list.clear();

    gfx_invalidate_screen();

    status = NETWORK_STATUS_NONE;
}

void NetworkBase::GetKeysDirectory(utf8* buffer, size_t bufferSize)
{
    platform_get_user_directory(buffer, "keys", bufferSize);
}

void NetworkBase::GetPrivateKeyPath(utf8* buffer, size_t bufferSize, const std::string& playerName)
{
    GetKeysDirectory(buffer, bufferSize);
    Path::Append(buffer, bufferSize, playerName.c_str());
    String::Append(buffer, bufferSize, ".privkey");
}

void NetworkBase::GetPublicKeyPath(utf8* buffer, size_t bufferSize, const std::string& playerName, const utf8* hash)
{
    GetKeysDirectory(buffer, bufferSize);
    Path::Append(buffer, bufferSize, playerName.c_str());
    String::Append(buffer, bufferSize, "-");
    String::Append(buffer, bufferSize, hash);
    String::Append(buffer, bufferSize, ".pubkey");
}

int32_t NetworkBase::GetStatus()
{
    return status;
}

uint8_t NetworkBase::GetPlayerID()
{
    return _playerId;
}

void NetworkBase::Update()
{
    // Update is not necessarily called per game tick, maintain our own delta time
    uint32_t ticks = platform_get_ticks();
    _currentDeltaTime = std::max<uint32_t>(ticks - _lastUpdateTime, 1);
    _lastUpdateTime = ticks;
}

void NetworkBase::PostUpdate()
{
}

void NetworkBase::Flush()
{
}

int32_t NetworkBase::GetMode() const
{
    return NETWORK_MODE_NONE;
}

std::vector<std::unique_ptr<NetworkPlayer>>::iterator NetworkBase::GetPlayerIteratorByID(uint8_t id)
{
    auto it = std::find_if(_playerList.begin(), _playerList.end(), [&id](std::unique_ptr<NetworkPlayer> const& player) {
        return player->Id == id;
    });
    if (it != _playerList.end())
    {
        return it;
    }
    return _playerList.end();
}

NetworkPlayer* NetworkBase::GetPlayerByID(uint8_t id)
{
    auto it = GetPlayerIteratorByID(id);
    if (it != _playerList.end())
    {
        return it->get();
    }
    return nullptr;
}

std::vector<std::unique_ptr<NetworkGroup>>::iterator NetworkBase::GetGroupIteratorByID(uint8_t id)
{
    auto it = std::find_if(
        group_list.begin(), group_list.end(), [&id](std::unique_ptr<NetworkGroup> const& group) { return group->Id == id; });
    if (it != group_list.end())
    {
        return it;
    }
    return group_list.end();
}

NetworkGroup* NetworkBase::GetGroupByID(uint8_t id)
{
    auto it = GetGroupIteratorByID(id);
    if (it != group_list.end())
    {
        return it->get();
    }
    return nullptr;
}

const char* NetworkBase::FormatChat(NetworkPlayer* fromplayer, const char* text)
{
    static std::string formatted;
    formatted.clear();
    formatted += "{OUTLINE}";
    if (fromplayer)
    {
        formatted += "{BABYBLUE}";
        formatted += fromplayer->Name;
        formatted += ": ";
    }
    formatted += "{WHITE}";
    formatted += text;
    return formatted.c_str();
}

void NetworkBase::SetPassword(const char* password)
{
    _password = password == nullptr ? "" : password;
}

std::string NetworkBase::GenerateAdvertiseKey()
{
    // Generate a string of 16 random hex characters (64-integer key as a hex formatted string)
    static char hexChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    char key[17];
    for (int32_t i = 0; i < 16; i++)
    {
        int32_t hexCharIndex = util_rand() % std::size(hexChars);
        key[i] = hexChars[hexCharIndex];
    }
    key[std::size(key) - 1] = 0;

    return key;
}

std::string NetworkBase::GetMasterServerUrl()
{
    if (gConfigNetwork.master_server_url.empty())
    {
        return OPENRCT2_MASTER_SERVER_URL;
    }
    else
    {
        return gConfigNetwork.master_server_url;
    }
}

NetworkGroup* NetworkBase::AddGroup()
{
    NetworkGroup* addedgroup = nullptr;
    int32_t newid = -1;
    // Find first unused group id
    for (int32_t id = 0; id < 255; id++)
    {
        if (std::find_if(
            group_list.begin(), group_list.end(),
            [&id](std::unique_ptr<NetworkGroup> const& group) { return group->Id == id; })
            == group_list.end())
        {
            newid = id;
            break;
        }
    }
    if (newid != -1)
    {
        auto group = std::make_unique<NetworkGroup>();
        group->Id = newid;
        group->SetName("Group #" + std::to_string(newid));
        addedgroup = group.get();
        group_list.push_back(std::move(group));
    }
    return addedgroup;
}

void NetworkBase::RemoveGroup(uint8_t id)
{
    auto group = GetGroupIteratorByID(id);
    if (group != group_list.end())
    {
        group_list.erase(group);
    }

    if (GetMode() == NETWORK_MODE_SERVER)
    {
        _userManager.UnsetUsersOfGroup(id);
        _userManager.Save();
    }
}

uint8_t NetworkBase::GetGroupIDByHash(const std::string& keyhash)
{
    const NetworkUser* networkUser = _userManager.GetUserByHash(keyhash);

    uint8_t groupId = GetDefaultGroup();
    if (networkUser != nullptr && networkUser->GroupId.HasValue())
    {
        const uint8_t assignedGroup = networkUser->GroupId.GetValue();
        if (GetGroupByID(assignedGroup) != nullptr)
        {
            groupId = assignedGroup;
        }
        else
        {
            log_warning(
                "User %s is assigned to non-existent group %u. Assigning to default group (%u)", keyhash.c_str(), assignedGroup,
                groupId);
        }
    }
    return groupId;
}

uint8_t NetworkBase::GetDefaultGroup()
{
    return default_group;
}

void NetworkBase::SetDefaultGroup(uint8_t id)
{
    if (GetGroupByID(id))
    {
        default_group = id;
    }
}

void NetworkBase::SaveGroups()
{
    if (GetMode() == NETWORK_MODE_SERVER)
    {
        utf8 path[MAX_PATH];

        platform_get_user_directory(path, nullptr, sizeof(path));
        safe_strcat_path(path, "groups.json", sizeof(path));

        json_t jsonGroups = json_t::array();
        for (auto& group : group_list)
        {
            jsonGroups.push_back(group->ToJson());
        }
        json_t jsonGroupsCfg = {
            { "default_group", default_group },
            { "groups", jsonGroups },
        };
        try
        {
            Json::WriteToFile(path, jsonGroupsCfg);
        }
        catch (const std::exception& ex)
        {
            log_error("Unable to save %s: %s", path, ex.what());
        }
    }
}

void NetworkBase::SetupDefaultGroups()
{
    // Admin group
    auto admin = std::make_unique<NetworkGroup>();
    admin->SetName("Admin");
    admin->ActionsAllowed.fill(0xFF);
    admin->Id = 0;
    group_list.push_back(std::move(admin));

    // Spectator group
    auto spectator = std::make_unique<NetworkGroup>();
    spectator->SetName("Spectator");
    spectator->ToggleActionPermission(NetworkPermission::Chat);
    spectator->Id = 1;
    group_list.push_back(std::move(spectator));

    // User group
    auto user = std::make_unique<NetworkGroup>();
    user->SetName("User");
    user->ActionsAllowed.fill(0xFF);
    user->ToggleActionPermission(NetworkPermission::KickPlayer);
    user->ToggleActionPermission(NetworkPermission::ModifyGroups);
    user->ToggleActionPermission(NetworkPermission::SetPlayerGroup);
    user->ToggleActionPermission(NetworkPermission::Cheat);
    user->ToggleActionPermission(NetworkPermission::PasswordlessLogin);
    user->ToggleActionPermission(NetworkPermission::ModifyTile);
    user->ToggleActionPermission(NetworkPermission::EditScenarioOptions);
    user->Id = 2;
    group_list.push_back(std::move(user));

    SetDefaultGroup(1);
}

void NetworkBase::LoadGroups()
{
    group_list.clear();

    utf8 path[MAX_PATH];

    platform_get_user_directory(path, nullptr, sizeof(path));
    safe_strcat_path(path, "groups.json", sizeof(path));

    json_t jsonGroupConfig;
    if (Platform::FileExists(path))
    {
        try
        {
            jsonGroupConfig = Json::ReadFromFile(path);
        }
        catch (const std::exception& e)
        {
            log_error("Failed to read %s as JSON. Setting default groups. %s", path, e.what());
        }
    }

    if (!jsonGroupConfig.is_object())
    {
        SetupDefaultGroups();
    }
    else
    {
        json_t jsonGroups = jsonGroupConfig["groups"];
        if (jsonGroups.is_array())
        {
            for (auto& jsonGroup : jsonGroups)
            {
                group_list.emplace_back(std::make_unique<NetworkGroup>(NetworkGroup::FromJson(jsonGroup)));
            }
        }

        default_group = Json::GetNumber<uint8_t>(jsonGroupConfig["default_group"]);
        if (GetGroupByID(default_group) == nullptr)
        {
            default_group = 0;
        }
    }

    // Host group should always contain all permissions.
    group_list.at(0)->ActionsAllowed.fill(0xFF);
}

std::string NetworkBase::BeginLog(const std::string& directory, const std::string& midName, const std::string& filenameFormat)
{
    utf8 filename[256];
    time_t timer;
    time(&timer);
    auto tmInfo = localtime(&timer);
    if (strftime(filename, sizeof(filename), filenameFormat.c_str(), tmInfo) == 0)
    {
        throw std::runtime_error("strftime failed");
    }

    platform_ensure_directory_exists(Path::Combine(directory, midName).c_str());
    return Path::Combine(directory, midName, filename);
}

void NetworkBase::AppendLog(std::ostream& fs, const std::string& s)
{
    if (fs.fail())
    {
        log_error("bad ostream failed to append log");
        return;
    }
    try
    {
        utf8 buffer[1024];
        time_t timer;
        time(&timer);
        auto tmInfo = localtime(&timer);
        if (strftime(buffer, sizeof(buffer), "[%Y/%m/%d %H:%M:%S] ", tmInfo) != 0)
        {
            String::Append(buffer, sizeof(buffer), s.c_str());
            String::Append(buffer, sizeof(buffer), PLATFORM_NEWLINE);

            fs.write(buffer, strlen(buffer));
        }
    }
    catch (const std::exception& ex)
    {
        log_error("%s", ex.what());
    }
}

void NetworkBase::BeginChatLog()
{
    auto directory = _env->GetDirectoryPath(DIRBASE::USER, DIRID::LOG_CHAT);
    _chatLogPath = BeginLog(directory, "", _chatLogFilenameFormat);

#    if defined(_WIN32) && !defined(__MINGW32__)
    auto pathW = String::ToWideChar(_chatLogPath.c_str());
    _chat_log_fs.open(pathW.c_str(), std::ios::out | std::ios::app);
#    else
    _chat_log_fs.open(_chatLogPath, std::ios::out | std::ios::app);
#    endif
}

void NetworkBase::AppendChatLog(const std::string& s)
{
    if (gConfigNetwork.log_chat && _chat_log_fs.is_open())
    {
        AppendLog(_chat_log_fs, s);
    }
}

void NetworkBase::CloseChatLog()
{
    _chat_log_fs.close();
}

void NetworkBase::ChatShowConnectedMessage()
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    std::string s = windowManager->GetKeyboardShortcutString(41 /* SHORTCUT_OPEN_CHAT_WINDOW */);
    const char* sptr = s.c_str();

    utf8 buffer[256];
    format_string(buffer, sizeof(buffer), STR_MULTIPLAYER_CONNECTED_CHAT_HINT, &sptr);

    NetworkPlayer server;
    server.Name = "Server";
    const char* formatted = NetworkBase::FormatChat(&server, buffer);
    chat_history_add(formatted);
}

void NetworkBase::ChatShowServerGreeting()
{
    if (ServerGreeting.empty())
        return;

    thread_local std::string greeting_formatted;
    greeting_formatted.assign("{OUTLINE}{GREEN}");
    greeting_formatted += ServerGreeting;
    chat_history_add(greeting_formatted.c_str());
}

NetworkStats_t NetworkBase::GetStats() const
{
    return {};
}

json_t NetworkBase::GetServerInfoAsJson() const
{
    json_t jsonObj = {
        { "name", gConfigNetwork.server_name },
        { "requiresPassword", _password.size() > 0 },
        { "version", OpenRCT2::GetContext()->GetNetwork()->GetVersion() },
        { "players", _playerList.size() },
        { "maxPlayers", gConfigNetwork.maxplayers },
        { "description", gConfigNetwork.server_description },
        { "greeting", gConfigNetwork.server_greeting },
        { "dedicated", gOpenRCT2Headless },
    };
    return jsonObj;
}

bool NetworkBase::ProcessConnection(NetworkConnection& connection)
{
    NetworkReadPacket packetStatus;
    do
    {
        packetStatus = connection.ReadPacket();
        switch (packetStatus)
        {
        case NetworkReadPacket::Disconnected:
            // closed connection or network error
            if (!connection.GetLastDisconnectReason())
            {
                connection.SetLastDisconnectReason(STR_MULTIPLAYER_CONNECTION_CLOSED);
            }
            return false;
        case NetworkReadPacket::Success:
            // done reading in packet
            ProcessPacket(connection, connection.InboundPacket);
            if (connection.Socket == nullptr)
            {
                return false;
            }
            break;
        case NetworkReadPacket::MoreData:
            // more data required to be read
            break;
        case NetworkReadPacket::NoData:
            // could not read anything from socket
            break;
        }
    } while (packetStatus == NetworkReadPacket::Success);

    connection.SendQueuedPackets();

    if (!connection.ReceivedPacketRecently())
    {
        if (!connection.GetLastDisconnectReason())
        {
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_NO_DATA);
        }
        return false;
    }

    return true;
}

void NetworkBase::ProcessPacket(NetworkConnection& connection, NetworkPacket& packet)
{
    auto it = command_handlers.find(packet.GetCommand());
    if (it != command_handlers.end())
    {
        auto commandHandler = it->second;
        if (connection.AuthStatus == NetworkAuth::Ok || !packet.CommandRequiresAuth())
        {
            commandHandler(connection, packet);
        }
    }
    packet.Clear();
}

NetworkPlayer* NetworkBase::AddPlayer(const std::string& name, const std::string& keyhash)
{
    NetworkPlayer* addedplayer = nullptr;
    int32_t newid = -1;
    if (GetMode() == NETWORK_MODE_SERVER)
    {
        // Find first unused player id
        for (int32_t id = 0; id < 255; id++)
        {
            if (std::find_if(
                    _playerList.begin(), _playerList.end(),
                    [&id](std::unique_ptr<NetworkPlayer> const& player) { return player->Id == id; })
                == _playerList.end())
            {
                newid = id;
                break;
            }
        }
    }
    else
    {
        newid = 0;
    }
    if (newid != -1)
    {
        std::unique_ptr<NetworkPlayer> player;
        if (GetMode() == NETWORK_MODE_SERVER)
        {
            // Load keys host may have added manually
            _userManager.Load();

            // Check if the key is registered
            const NetworkUser* networkUser = _userManager.GetUserByHash(keyhash);

            player = std::make_unique<NetworkPlayer>();
            player->Id = newid;
            player->KeyHash = keyhash;
            if (networkUser == nullptr)
            {
                player->Group = GetDefaultGroup();
                if (!name.empty())
                {
                    player->SetName(MakePlayerNameUnique(String::Trim(name)));
                }
            }
            else
            {
                player->Group = networkUser->GroupId.GetValueOrDefault(GetDefaultGroup());
                player->SetName(networkUser->Name);
            }

            // Send new player list.
            _playerListInvalidated = true;
        }
        else
        {
            player = std::make_unique<NetworkPlayer>();
            player->Id = newid;
            player->Group = GetDefaultGroup();
            player->SetName(String::Trim(std::string(name)));
        }

        addedplayer = player.get();
        _playerList.push_back(std::move(player));
    }
    return addedplayer;
}

std::string NetworkBase::MakePlayerNameUnique(const std::string& name)
{
    // Note: Player names are case-insensitive

    std::string new_name = name.substr(0, 31);
    int32_t counter = 1;
    bool unique;
    do
    {
        unique = true;

        // Check if there is already a player with this name in the server
        for (const auto& player : _playerList)
        {
            if (String::Equals(player->Name.c_str(), new_name.c_str(), true))
            {
                unique = false;
                break;
            }
        }

        if (unique)
        {
            // Check if there is already a registered player with this name
            if (_userManager.GetUserByName(new_name) != nullptr)
            {
                unique = false;
            }
        }

        if (!unique)
        {
            // Increment name counter
            counter++;
            new_name = name.substr(0, 31) + " #" + std::to_string(counter);
        }
    } while (!unique);
    return new_name;
}

void NetworkBase::ProcessPlayerJoinedPluginHooks(uint8_t playerId)
{
#    ifdef ENABLE_SCRIPTING
    using namespace OpenRCT2::Scripting;

    auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
    if (hookEngine.HasSubscriptions(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_JOIN))
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        DukObject eObj(ctx);
        eObj.Set("player", playerId);
        auto e = eObj.Take();

        // Call the subscriptions
        hookEngine.Call(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_JOIN, e, false);
    }
#    endif
}

void NetworkBase::ProcessPlayerLeftPluginHooks(uint8_t playerId)
{
#    ifdef ENABLE_SCRIPTING
    using namespace OpenRCT2::Scripting;

    auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
    if (hookEngine.HasSubscriptions(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_LEAVE))
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        DukObject eObj(ctx);
        eObj.Set("player", playerId);
        auto e = eObj.Take();

        // Call the subscriptions
        hookEngine.Call(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_LEAVE, e, false);
    }
#    endif
}

void NetworkBase::SetPickupPeep(uint8_t playerid, Peep* peep)
{
    if (GetMode() == NETWORK_MODE_NONE)
    {
        _pickup_peep = peep;
    }
    else
    {
        NetworkPlayer* player = GetPlayerByID(playerid);
        if (player)
        {
            player->PickupPeep = peep;
        }
    }
}

Peep* NetworkBase::GetPickupPeep(uint8_t playerid)
{
    if (GetMode() == NETWORK_MODE_NONE)
    {
        return _pickup_peep;
    }
    else
    {
        NetworkPlayer* player = GetPlayerByID(playerid);
        if (player)
        {
            return player->PickupPeep;
        }
        return nullptr;
    }
}

void NetworkBase::SetPickupPeepOldX(uint8_t playerid, int32_t x)
{
    if (GetMode() == NETWORK_MODE_NONE)
    {
        _pickup_peep_old_x = x;
    }
    else
    {
        NetworkPlayer* player = GetPlayerByID(playerid);
        if (player)
        {
            player->PickupPeepOldX = x;
        }
    }
}

int32_t NetworkBase::GetPickupPeepOldX(uint8_t playerid)
{
    if (GetMode() == NETWORK_MODE_NONE)
    {
        return _pickup_peep_old_x;
    }
    else
    {
        NetworkPlayer* player = GetPlayerByID(playerid);
        if (player)
        {
            return player->PickupPeepOldX;
        }
        return -1;
    }
}

#endif /* DISABLE_NETWORK */
