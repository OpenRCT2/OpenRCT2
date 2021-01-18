#include "network.h"

#include "../Context.h"
#include "../Game.h"
#include "../GameStateSnapshots.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../Version.h"
#include "../actions/LoadOrQuitAction.h"
#include "../actions/NetworkModifyGroupAction.h"
#include "../actions/PeepPickupAction.h"
#include "../core/FileStream.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../interface/Chat.h"
#include "../platform/Platform2.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../util/SawyerCoding.h"
#include "../world/Location.hpp"
#include "NetworkAction.h"
#include "NetworkBase.h"
#include "NetworkClient.h"
#include "NetworkServer.h"

using namespace OpenRCT2;

// This string specifies which version of network stream current build uses.
// It is used for making sure only compatible builds get connected, even within
// single OpenRCT2 version.
#define NETWORK_STREAM_VERSION "9"
#define NETWORK_STREAM_ID OPENRCT2_VERSION "-" NETWORK_STREAM_VERSION

namespace OpenRCT2
{
#ifndef DISABLE_NETWORK
    class Network : public INetwork
    {
        std::unique_ptr<NetworkBase> _network;
        std::shared_ptr<OpenRCT2::IPlatformEnvironment> _env;
        Peep* _pickup_peep = nullptr;
        int32_t _pickup_peep_old_x = LOCATION_NULL;
        int32_t _mode = NETWORK_MODE_NONE;

    public:
        Network(std::shared_ptr<IPlatformEnvironment> env)
            : _network(std::make_unique<NetworkBase>(env))
            , _env(env)
        {
        }

        NetworkBase* GetBase() override
        {
            return _network.get();
        }

        void Close() override
        {
            _network->Close();
        }

        bool BeginClient(const std::string& host, int32_t port) override
        {
            auto client = std::make_unique<NetworkClient>(_env);
            if (!client->BeginClient(host, port))
                return false;

            _mode = NETWORK_MODE_CLIENT;
            _network = std::move(client);

            return true;
        }

        bool BeginServer(int32_t port, const std::string& address) override
        {
            auto server = std::make_unique<NetworkServer>(_env);
            if (!server->BeginServer(port, address))
                return false;

            _mode = NETWORK_MODE_SERVER;
            _network = std::move(server);

            return true;
        }

        void Update() override
        {
            _network->Update();
        }

        void ProcessPending() override
        {
            _network->PostUpdate();
        }

        void Flush() override
        {
            _network->Flush();
        }

        int32_t GetMode() override
        {
            return _mode;
        }

        int32_t GetStatus() override
        {
            return _network->GetStatus();
        }

        bool IsDesynchronised() override
        {
            return _network->IsDesynchronised();
        }

        bool CheckDesynchronisation() override
        {
            return _network->CheckDesynchronizaton();
        }

        NetworkAuth GetAuthStatus() override
        {
            return _network->GetAuthStatus();
        }

        uint32_t GetServerTick() override
        {
            return _network->GetServerTick();
        }

        uint8_t GetCurrentPlayerId() override
        {
            return _network->GetPlayerID();
        }

        int32_t GetNumPlayers() override
        {
            return static_cast<int32_t>(_network->player_list.size());
        }

        const char* GetPlayerName(uint32_t index) override
        {
            Guard::IndexInRange(index, _network->player_list);

            return static_cast<const char*>(_network->player_list[index]->Name.c_str());
        }

        uint32_t GetPlayerFlags(uint32_t index) override
        {
            Guard::IndexInRange(index, _network->player_list);

            return _network->player_list[index]->Flags;
        }

        int32_t GetPlayerPing(uint32_t index) override
        {
            Guard::IndexInRange(index, _network->player_list);

            return _network->player_list[index]->Ping;
        }

        int32_t GetPlayerId(uint32_t index) override
        {
            Guard::IndexInRange(index, _network->player_list);

            return _network->player_list[index]->Id;
        }

        money32 GetPlayerMoneySpent(uint32_t index) override
        {
            Guard::IndexInRange(index, _network->player_list);

            return _network->player_list[index]->MoneySpent;
        }

        std::string GetPlayerIPAddress(uint32_t id) override
        {
            auto conn = _network->GetPlayerConnection(id);
            if (conn != nullptr && conn->Socket != nullptr)
            {
                return conn->Socket->GetIpAddress();
            }
            return {};
        }

        std::string GetPlayerPublicKeyHash(uint32_t id) override
        {
            auto player = _network->GetPlayerByID(id);
            if (player != nullptr)
            {
                return player->KeyHash;
            }
            return {};
        }

        void AddPlayerMoneySpent(uint32_t index, money32 cost) override
        {
            Guard::IndexInRange(index, _network->player_list);

            _network->player_list[index]->AddMoneySpent(cost);
        }

        int32_t GetPlayerLastAction(uint32_t index, int32_t time) override
        {
            Guard::IndexInRange(index, _network->player_list);

            if (time && platform_get_ticks() > _network->player_list[index]->LastActionTime + time)
            {
                return -999;
            }
            return _network->player_list[index]->LastAction;
        }

        void SetPlayerLastAction(uint32_t index, GameCommand command) override
        {
            Guard::IndexInRange(index, _network->player_list);

            _network->player_list[index]->LastAction = static_cast<int32_t>(NetworkActions::FindCommand(command));
            _network->player_list[index]->LastActionTime = platform_get_ticks();
        }

        CoordsXYZ GetPlayerLastActionCoord(uint32_t index) override
        {
            Guard::IndexInRange(index, _network->player_list);

            return _network->player_list[index]->LastActionCoord;
        }

        void SetPlayerLastActionCoord(uint32_t index, const CoordsXYZ& coord) override
        {
            Guard::IndexInRange(index, _network->player_list);

            if (index < _network->player_list.size())
            {
                _network->player_list[index]->LastActionCoord = coord;
            }
        }

        uint32_t GetPlayerCommandsRan(uint32_t index) override
        {
            Guard::IndexInRange(index, _network->player_list);

            return _network->player_list[index]->CommandsRan;
        }

        int32_t GetPlayerIndex(uint32_t id) override
        {
            auto it = _network->GetPlayerIteratorByID(id);
            if (it == _network->player_list.end())
            {
                return -1;
            }
            return static_cast<int32_t>(_network->GetPlayerIteratorByID(id) - _network->player_list.begin());
        }

        uint8_t GetPlayerGroup(uint32_t index) override
        {
            Guard::IndexInRange(index, _network->player_list);

            return _network->player_list[index]->Group;
        }

        void SetPlayerGroup(uint32_t index, uint32_t groupindex) override
        {
            Guard::IndexInRange(index, _network->player_list);
            Guard::IndexInRange(groupindex, _network->group_list);

            _network->player_list[index]->Group = _network->group_list[groupindex]->Id;
        }

        int32_t GetGroupIndex(uint8_t id) override
        {
            auto it = _network->GetGroupIteratorByID(id);
            if (it == _network->group_list.end())
            {
                return -1;
            }
            return static_cast<int32_t>(_network->GetGroupIteratorByID(id) - _network->group_list.begin());
        }

        uint8_t GetGroupId(uint32_t index) override
        {
            Guard::IndexInRange(index, _network->group_list);

            return _network->group_list[index]->Id;
        }

        int32_t GetNumGroups() override
        {
            return static_cast<int32_t>(_network->group_list.size());
        }

        const char* GetGroupName(uint32_t index) override
        {
            return _network->group_list[index]->GetName().c_str();
        }

        GameActions::Result::Ptr SetPlayerGroup(
            NetworkPlayerId_t actionPlayerId, NetworkPlayerId_t playerId, uint8_t groupId, bool isExecuting) override
        {
            NetworkPlayer* player = _network->GetPlayerByID(playerId);

            NetworkGroup* fromgroup = _network->GetGroupByID(actionPlayerId);
            if (player == nullptr)
            {
                return std::make_unique<GameActions::Result>(GameActions::Status::InvalidParameters, STR_CANT_DO_THIS);
            }

            if (!_network->GetGroupByID(groupId))
            {
                return std::make_unique<GameActions::Result>(GameActions::Status::InvalidParameters, STR_CANT_DO_THIS);
            }

            if (player->Flags & NETWORK_PLAYER_FLAG_ISSERVER)
            {
                return std::make_unique<GameActions::Result>(
                    GameActions::Status::InvalidParameters, STR_CANT_CHANGE_GROUP_THAT_THE_HOST_BELONGS_TO);
            }

            if (groupId == 0 && fromgroup && fromgroup->Id != 0)
            {
                return std::make_unique<GameActions::Result>(
                    GameActions::Status::InvalidParameters, STR_CANT_SET_TO_THIS_GROUP);
            }

            if (isExecuting)
            {
                player->Group = groupId;

                if (GetMode() == NETWORK_MODE_SERVER)
                {
                    // Add or update saved user
                    NetworkUserManager* userManager = &_network->_userManager;
                    NetworkUser* networkUser = userManager->GetOrAddUser(player->KeyHash);
                    networkUser->GroupId = groupId;
                    networkUser->Name = player->Name;
                    userManager->Save();
                }

                window_invalidate_by_number(WC_PLAYER, playerId);

                // Log set player group event
                NetworkPlayer* game_command_player = _network->GetPlayerByID(actionPlayerId);
                NetworkGroup* new_player_group = _network->GetGroupByID(groupId);
                char log_msg[256];
                const char* args[3] = {
                    player->Name.c_str(),
                    new_player_group->GetName().c_str(),
                    game_command_player->Name.c_str(),
                };
                format_string(log_msg, 256, STR_LOG_SET_PLAYER_GROUP, args);
                AppendServerLog(log_msg);
            }
            return std::make_unique<GameActions::Result>();
        }

        GameActions::Result::Ptr ModifyGroups(
            NetworkPlayerId_t actionPlayerId, ModifyGroupType type, uint8_t groupId, const std::string& name,
            uint32_t permissionIndex, PermissionState permissionState, bool isExecuting) override
        {
            switch (type)
            {
                case ModifyGroupType::AddGroup:
                {
                    if (isExecuting)
                    {
                        NetworkGroup* newgroup = _network->AddGroup();
                        if (newgroup == nullptr)
                        {
                            return std::make_unique<GameActions::Result>(GameActions::Status::Unknown, STR_CANT_DO_THIS);
                        }
                    }
                }
                break;
                case ModifyGroupType::RemoveGroup:
                {
                    if (groupId == 0)
                    {
                        return std::make_unique<GameActions::Result>(
                            GameActions::Status::Disallowed, STR_THIS_GROUP_CANNOT_BE_MODIFIED);
                    }
                    for (const auto& it : _network->player_list)
                    {
                        if ((it.get())->Group == groupId)
                        {
                            return std::make_unique<GameActions::Result>(
                                GameActions::Status::Disallowed, STR_CANT_REMOVE_GROUP_THAT_PLAYERS_BELONG_TO);
                        }
                    }
                    if (isExecuting)
                    {
                        _network->RemoveGroup(groupId);
                    }
                }
                break;
                case ModifyGroupType::SetPermissions:
                {
                    if (groupId == 0)
                    { // cant change admin group permissions
                        return std::make_unique<GameActions::Result>(
                            GameActions::Status::Disallowed, STR_THIS_GROUP_CANNOT_BE_MODIFIED);
                    }
                    NetworkGroup* mygroup = nullptr;
                    NetworkPlayer* player = _network->GetPlayerByID(actionPlayerId);
                    auto networkPermission = static_cast<NetworkPermission>(permissionIndex);
                    if (player != nullptr && permissionState == PermissionState::Toggle)
                    {
                        mygroup = _network->GetGroupByID(player->Group);
                        if (mygroup == nullptr || !mygroup->CanPerformAction(networkPermission))
                        {
                            return std::make_unique<GameActions::Result>(
                                GameActions::Status::Disallowed, STR_CANT_MODIFY_PERMISSION_THAT_YOU_DO_NOT_HAVE_YOURSELF);
                        }
                    }
                    if (isExecuting)
                    {
                        NetworkGroup* group = _network->GetGroupByID(groupId);
                        if (group != nullptr)
                        {
                            if (permissionState != PermissionState::Toggle)
                            {
                                if (mygroup != nullptr)
                                {
                                    if (permissionState == PermissionState::SetAll)
                                    {
                                        group->ActionsAllowed = mygroup->ActionsAllowed;
                                    }
                                    else
                                    {
                                        group->ActionsAllowed.fill(0x00);
                                    }
                                }
                            }
                            else
                            {
                                group->ToggleActionPermission(networkPermission);
                            }
                        }
                    }
                }
                break;
                case ModifyGroupType::SetName:
                {
                    NetworkGroup* group = _network->GetGroupByID(groupId);
                    const char* oldName = group->GetName().c_str();

                    if (strcmp(oldName, name.c_str()) == 0)
                    {
                        return std::make_unique<GameActions::Result>();
                    }

                    if (name.empty())
                    {
                        return std::make_unique<GameActions::Result>(
                            GameActions::Status::InvalidParameters, STR_CANT_RENAME_GROUP, STR_INVALID_GROUP_NAME);
                    }

                    if (isExecuting)
                    {
                        if (group != nullptr)
                        {
                            group->SetName(name);
                        }
                    }
                }
                break;
                case ModifyGroupType::SetDefault:
                {
                    if (groupId == 0)
                    {
                        return std::make_unique<GameActions::Result>(
                            GameActions::Status::Disallowed, STR_CANT_SET_TO_THIS_GROUP);
                    }
                    if (isExecuting)
                    {
                        _network->SetDefaultGroup(groupId);
                    }
                }
                break;
                default:
                    log_error("Invalid Modify Group Type: %u", static_cast<uint8_t>(type));
                    return std::make_unique<GameActions::Result>(GameActions::Status::InvalidParameters, STR_NONE);
            }

            _network->SaveGroups();

            return std::make_unique<GameActions::Result>();
        }

        GameActions::Result::Ptr KickPlayer(NetworkPlayerId_t playerId, bool isExecuting) override
        {
            NetworkPlayer* player = _network->GetPlayerByID(playerId);
            if (player == nullptr)
            {
                // Player might be already removed by the PLAYERLIST command, need to refactor non-game commands executing too
                // early.
                return std::make_unique<GameActions::Result>(GameActions::Status::Unknown, STR_NONE);
            }

            if (player && player->Flags & NETWORK_PLAYER_FLAG_ISSERVER)
            {
                return std::make_unique<GameActions::Result>(GameActions::Status::Disallowed, STR_CANT_KICK_THE_HOST);
            }

            if (isExecuting)
            {
                if (auto* server = As<NetworkServer>())
                {
                    server->KickPlayer(playerId);

                    NetworkUserManager& networkUserManager = server->_userManager;
                    networkUserManager.Load();
                    networkUserManager.RemoveUser(player->KeyHash);
                    networkUserManager.Save();
                }
            }
            return std::make_unique<GameActions::Result>();
        }

        uint8_t GetDefaultGroup() override
        {
            return _network->GetDefaultGroup();
        }

        int32_t GetNumActions() override
        {
            return static_cast<int32_t>(NetworkActions::Actions.size());
        }

        rct_string_id GetActionNameStringId(uint32_t index) override
        {
            if (index < NetworkActions::Actions.size())
            {
                return NetworkActions::Actions[index].Name;
            }
            else
            {
                return STR_NONE;
            }
        }

        int32_t CanPerformAction(uint32_t groupindex, NetworkPermission index) override
        {
            Guard::IndexInRange(groupindex, _network->group_list);

            return _network->group_list[groupindex]->CanPerformAction(index);
        }

        int32_t CanPerformCommand(uint32_t groupindex, int32_t index) override
        {
            Guard::IndexInRange(groupindex, _network->group_list);

            return _network->group_list[groupindex]->CanPerformCommand(static_cast<GameCommand>(index)); // TODO
        }

        void SetPickupPeep(uint8_t playerid, Peep* peep) override
        {
            if (GetMode() == NETWORK_MODE_NONE)
            {
                _pickup_peep = peep;
            }
            else
            {
                NetworkPlayer* player = _network->GetPlayerByID(playerid);
                if (player)
                {
                    player->PickupPeep = peep;
                }
            }
        }

        Peep* GetPickupPeep(uint8_t playerid) override
        {
            if (GetMode() == NETWORK_MODE_NONE)
            {
                return _pickup_peep;
            }
            else
            {
                NetworkPlayer* player = _network->GetPlayerByID(playerid);
                if (player)
                {
                    return player->PickupPeep;
                }
                return nullptr;
            }
        }

        void SetPickupPeepOldX(uint8_t playerid, int32_t x) override
        {
            if (GetMode() == NETWORK_MODE_NONE)
            {
                _pickup_peep_old_x = x;
            }
            else
            {
                NetworkPlayer* player = _network->GetPlayerByID(playerid);
                if (player)
                {
                    player->PickupPeepOldX = x;
                }
            }
        }

        int32_t GetPickupPeepOldX(uint8_t playerid) override
        {
            if (GetMode() == NETWORK_MODE_NONE)
            {
                return _pickup_peep_old_x;
            }
            else
            {
                NetworkPlayer* player = _network->GetPlayerByID(playerid);
                if (player)
                {
                    return player->PickupPeepOldX;
                }
                return -1;
            }
        }

        int32_t GetCurrentPlayerGroupIndex() override
        {
            NetworkPlayer* player = _network->GetPlayerByID(_network->GetPlayerID());
            if (player)
            {
                return GetGroupIndex(player->Group);
            }
            return -1;
        }

        static bool ProcessChatMessagePluginHooks(uint8_t playerId, std::string& text)
        {
#    ifdef ENABLE_SCRIPTING
            auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
            if (hookEngine.HasSubscriptions(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_CHAT))
            {
                auto ctx = GetContext()->GetScriptEngine().GetContext();

                // Create event args object
                auto objIdx = duk_push_object(ctx);
                duk_push_number(ctx, playerId);
                duk_put_prop_string(ctx, objIdx, "player");
                duk_push_string(ctx, text.c_str());
                duk_put_prop_string(ctx, objIdx, "message");
                auto e = DukValue::take_from_stack(ctx);

                // Call the subscriptions
                hookEngine.Call(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_CHAT, e, false);

                // Update text from object if subscriptions changed it
                if (e["message"].type() != DukValue::Type::STRING)
                {
                    // Subscription set text to non-string, do not relay message
                    return false;
                }
                text = e["message"].as_string();
                if (text.empty())
                {
                    // Subscription set text to empty string, do not relay message
                    return false;
                }
            }
#    endif
            return true;
        }

        void SendChat(const char* text, const std::vector<uint8_t>& playerIds) override
        {
            if (auto* client = As<NetworkClient>())
            {
                client->SendChat(text);
            }
            else if (auto* server = As<NetworkServer>())
            {
                std::string message = text;
                if (ProcessChatMessagePluginHooks(_network->GetPlayerID(), message))
                {
                    auto player = _network->GetPlayerByID(_network->GetPlayerID());
                    if (player != nullptr)
                    {
                        auto formatted = _network->FormatChat(player, message.c_str());
                        if (playerIds.empty()
                            || std::find(playerIds.begin(), playerIds.end(), _network->GetPlayerID()) != playerIds.end())
                        {
                            // Server is one of the recipients
                            chat_history_add(formatted);
                        }
                        server->SendChat(formatted, playerIds);
                    }
                }
            }
        }

        void SendGameAction(const GameAction* action) override
        {
            if (auto* client = As<NetworkClient>())
            {
                client->SendGameAction(action);
            }
            else if (auto* server = As<NetworkServer>())
            {
                server->SendGameAction(action);
            }
        }

        void SetPassword(const char* password) override
        {
            _network->SetPassword(password);
        }

        void AppendChatLog(const utf8* text) override
        {
            _network->AppendChatLog(text);
        }

        void AppendServerLog(const utf8* text) override
        {
            _network->AppendServerLog(text);
        }

        const utf8* GetServerName() override
        {
            return _network->ServerName.c_str();
        }
        const utf8* GetServerDescription() override
        {
            return _network->ServerDescription.c_str();
        }
        const utf8* GetServerGreeting() override
        {
            return _network->ServerGreeting.c_str();
        }
        const utf8* GetServerProviderName() override
        {
            return _network->ServerProviderName.c_str();
        }
        const utf8* GetServerProviderEmail() override
        {
            return _network->ServerProviderEmail.c_str();
        }
        const utf8* GetServerProvideWebsite() override
        {
            return _network->ServerProviderWebsite.c_str();
        }

        std::string GetVersion() override
        {
            return NETWORK_STREAM_ID;
        }

        NetworkStats_t GetStats() override
        {
            return _network->GetStats();
        }

        NetworkServerState_t GetServerState() override
        {
            return _network->GetServerState();
        }

        bool GamestateSnapshotsEnabled() override
        {
            return GetServerState().gamestateSnapshotsEnabled;
        }

        json_t GetServerInfoAsJson() override
        {
            return _network->GetServerInfoAsJson();
        }
    };

#else
    class NetworkDummy : public INetwork
    {
    public:
        Network(std::shared_ptr<IPlatformEnvironment>)
        {
        }
        void SetEnv(const std::shared_ptr<IPlatformEnvironment>&) override
        {
        }
        void Close() override
        {
        }
        void Reconnect() override
        {
        }
        void Disconnect() override
        {
        }

        int32_t BeginClient(const std::string&, int32_t) override
        {
        }

        int32_t BeginServer(int32_t, const std::string&) override
        {
        }

        void Update() override
        {
        }

        void ProcessPending() override
        {
        }

        void Flush() override
        {
        }

        int32_t GetMode() override
        {
            return NETWORK_MODE_NONE;
        }

        int32_t GetStatus() override
        {
            return NETWORK_STATUS_NONE;
        }

        bool IsDesynchronised() override
        {
            return false;
        }

        bool CheckDesynchronisation() override
        {
            return false;
        }

        void RequestGamestateSnapshot() override
        {
        }

        void SendTick() override
        {
        }

        NetworkAuth GetAuthStatus() override
        {
            return NetworkAuth::None;
        }

        uint32_t GetServerTick() override
        {
            return 0;
        }

        uint8_t GetCurrentPlayerId() override
        {
            return 0xFF;
        }

        int32_t GetNumPlayers() override
        {
            return 0;
        }

        const char* GetPlayerName(uint32_t) override
        {
            return "Invalid";
        }

        uint32_t GetPlayerFlags(uint32_t) override
        {
            return 0;
        }

        int32_t GetPlayerPing(uint32_t index) override
        {
            return 0;
        }

        int32_t GetPlayerId(uint32_t index) override
        {
            return -1;
        }

        money32 GetPlayerMoneySpent(uint32_t index) override
        {
            return {};
        }

        std::string GetPlayerIPAddress(uint32_t id) override
        {
            return {};
        }

        std::string GetPlayerPublicKeyHash(uint32_t id) override
        {
            return {};
        }

        void AddPlayerMoneySpent(uint32_t, money32) override
        {
        }

        int32_t GetPlayerLastAction(uint32_t, int32_t) override
        {
            return 0;
        }

        void SetPlayerLastAction(uint32_t, GameCommand) override
        {
        }

        CoordsXYZ GetPlayerLastActionCoord(uint32_t) override
        {
            return {};
        }

        void SetPlayerLastActionCoord(uint32_t, const CoordsXYZ&) override
        {
        }

        uint32_t GetPlayerCommandsRan(uint32_t) override
        {
            return 0;
        }

        int32_t GetPlayerIndex(uint32_t) override
        {
            return 0;
        }

        uint8_t GetPlayerGroup(uint32_t) override
        {
            return 0;
        }

        void SetPlayerGroup(uint32_t, uint32_t) override
        {
        }

        int32_t GetGroupIndex(uint8_t) override
        {
            return 0;
        }

        uint8_t GetGroupId(uint32_t) override
        {
            return 0;
        }

        int32_t GetNumGroups() override
        {
            return 0;
        }

        const char* GetGroupName(uint32_t index) override
        {
            return "Invalid";
        }

        GameActions::Result::Ptr SetPlayerGroup(
            NetworkPlayerId_t actionPlayerId, NetworkPlayerId_t playerId, uint8_t groupId, bool isExecuting) override
        {
            return std::make_unique<GameActions::Result>();
        }

        GameActions::Result::Ptr ModifyGroups(
            NetworkPlayerId_t actionPlayerId, ModifyGroupType type, uint8_t groupId, const std::string& name,
            uint32_t permissionIndex, PermissionState permissionState, bool isExecuting) override
        {
            return std::make_unique<GameActions::Result>();
        }

        GameActions::Result::Ptr KickPlayer(NetworkPlayerId_t playerId, bool isExecuting) override
        {
            return std::make_unique<GameActions::Result>();
        }

        uint8_t GetDefaultGroup() override
        {
            return 0;
        }

        int32_t GetNumActions() override
        {
            return 0;
        }

        rct_string_id GetActionNameStringId(uint32_t index) override
        {
            return STR_NONE;
        }

        int32_t CanPerformAction(uint32_t groupindex, NetworkPermission index) override
        {
            return 0;
        }

        int32_t CanPerformCommand(uint32_t groupindex, int32_t index) override
        {
            return 0;
        }

        void SetPickupPeep(uint8_t playerid, Peep* peep) override
        {
        }

        Peep* GetPickupPeep(uint8_t playerid) override
        {
            return nullptr;
        }

        void SetPickupPeepOldX(uint8_t playerid, int32_t x) override
        {
        }

        int32_t GetPickupPeepOldX(uint8_t playerid) override
        {
            return 0;
        }

        int32_t GetCurrentPlayerGroupIndex() override
        {
            return -1;
        }

        void SendMap() override
        {
        }

        void SendChat(const char* text, const std::vector<uint8_t>& playerIds) override
        {
        }

        void SendGameAction(const GameAction* action) override
        {
        }

        void SendPassword(const std::string& password) override
        {
        }

        void SetPassword(const char* password) override
        {
        }

        void AppendChatLog(const utf8* text) override
        {
        }

        void AppendServerLog(const utf8* text) override
        {
        }

        const utf8* GetServerName() override
        {
            return "Invalid";
        }
        const utf8* GetServerDescription() override
        {
            return "Invalid";
        }
        const utf8* GetServerGreeting() override
        {
            return "Invalid";
        }
        const utf8* GetServerProviderName() override
        {
            return "Invalid";
        }
        const utf8* GetServerProviderEmail() override
        {
            return "Invalid";
        }
        const utf8* GetServerProvideWebsite() override
        {
            return "Invalid";
        }

        std::string GetVersion() override
        {
            return "";
        }

        NetworkStats_t GetStats() override
        {
            return {};
        }

        NetworkServerState_t GetServerState() override
        {
            return {};
        }

        bool GamestateSnapshotsEnabled() override
        {
            return false;
        }

        json_t GetServerInfoAsJson() override
        {
            return {};
        }
    };

    using Network = NetworkDummy;
#endif

    std::unique_ptr<INetwork> CreateNetwork(const std::shared_ptr<IPlatformEnvironment>& env)
    {
        return std::make_unique<Network>(env);
    }

    bool IsMultiplayerGame()
    {
        return GetContext()->GetNetwork()->GetMode() != NETWORK_MODE_NONE;
    }

} // namespace OpenRCT2
