#pragma once

#include "../actions/GameAction.h"
#include "NetworkConnection.h"
#include "NetworkGroup.h"
#include "NetworkPlayer.h"
#include "NetworkServerAdvertiser.h"
#include "NetworkTypes.h"
#include "NetworkUser.h"

#include <fstream>

#ifndef DISABLE_NETWORK

namespace OpenRCT2
{
    class NetworkBase
    {
    public:
        // General chunk size is 63 KiB, this can not be any larger because the packet size is encoded
        // with uint16_t and needs some spare room for other data in the packet.
        static constexpr uint32_t PacketChunkSize = 1024 * 63;

    public:
        NetworkBase(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
        virtual ~NetworkBase() = default;

    public: // Common
        bool Init();

        virtual void Close();
        virtual void Update();
        virtual void PostUpdate();
        virtual void Flush();
        virtual int32_t GetMode() const;

        std::vector<std::unique_ptr<NetworkPlayer>>::iterator GetPlayerIteratorByID(uint8_t id);
        std::vector<std::unique_ptr<NetworkGroup>>::iterator GetGroupIteratorByID(uint8_t id);
        NetworkPlayer* GetPlayerByID(uint8_t id);
        NetworkGroup* GetGroupByID(uint8_t id);
        void SetPassword(const char* password);
        uint8_t GetDefaultGroup();
        std::string BeginLog(const std::string& directory, const std::string& midName, const std::string& filenameFormat);
        void AppendLog(std::ostream& fs, const std::string& s);
        void BeginChatLog();
        void AppendChatLog(const std::string& s);
        void CloseChatLog();
        virtual NetworkStats_t GetStats() const;
        json_t GetServerInfoAsJson() const;
        bool ProcessConnection(NetworkConnection& connection);
        NetworkPlayer* AddPlayer(const std::string& name, const std::string& keyhash);
        void ProcessPacket(NetworkConnection& connection, NetworkPacket& packet);

    public: // Key managment
        void GetKeysDirectory(utf8* buffer, size_t bufferSize);
        void GetPublicKeyPath(utf8* buffer, size_t bufferSize, const std::string& playerName, const utf8* hash);
        void GetPrivateKeyPath(utf8* buffer, size_t bufferSize, const std::string& playerName);

    public: // Server
        NetworkGroup* AddGroup();
        void LoadGroups();
        void SetDefaultGroup(uint8_t id);
        void SaveGroups();
        void RemoveGroup(uint8_t id);
        uint8_t GetGroupIDByHash(const std::string& keyhash);
        void AddClient(std::unique_ptr<ITcpSocket>&& socket);
        std::string GetMasterServerUrl();
        std::string GenerateAdvertiseKey();
        void SetupDefaultGroups();
        void RemovePlayer(std::unique_ptr<NetworkConnection>& connection);
        std::string MakePlayerNameUnique(const std::string& name);

    public: // Client
        virtual NetworkAuth GetAuthStatus()
        {
            return NetworkAuth::None;
        }

        int32_t GetStatus();

        uint8_t GetPlayerID();
        static const char* FormatChat(NetworkPlayer* fromplayer, const char* text);

    protected: // Scripting
        void ProcessPlayerJoinedPluginHooks(uint8_t playerId);
        void ProcessPlayerLeftPluginHooks(uint8_t playerId);

    public:
        void SetPickupPeep(uint8_t playerid, Peep* peep);
        Peep* GetPickupPeep(uint8_t playerid);
        void SetPickupPeepOldX(uint8_t playerid, int32_t x);
        int32_t GetPickupPeepOldX(uint8_t playerid);

        void ChatShowConnectedMessage();

        // Display server greeting if one exists
        void ChatShowServerGreeting();

        std::vector<uint8_t> _challenge;
        std::map<uint32_t, GameAction::Callback_t> _gameActionCallbacks;
        NetworkKey _key;
        NetworkUserManager _userManager;

    public: // Public common
        std::string ServerName;
        std::string ServerDescription;
        std::string ServerGreeting;
        std::string ServerProviderName;
        std::string ServerProviderEmail;
        std::string ServerProviderWebsite;
        std::vector<std::unique_ptr<NetworkPlayer>> _playerList;
        std::vector<std::unique_ptr<NetworkGroup>> group_list;

    protected: // Common Data
        using CommandHandler = std::function<void(NetworkConnection& connection, NetworkPacket& packet)>;

        std::unordered_map<NetworkCommand, CommandHandler> command_handlers;

        template<typename T> void SetupCommandHandler(NetworkCommand cmd, void (T::*fn)(NetworkConnection&, NetworkPacket&))
        {
            command_handlers[cmd] = std::bind(fn, static_cast<T*>(this), std::placeholders::_1, std::placeholders::_2);
        }

        std::shared_ptr<OpenRCT2::IPlatformEnvironment> _env;

        int32_t status = NETWORK_STATUS_NONE;

        std::string _password;
        std::string _chatLogPath;
        std::string _chatLogFilenameFormat = "%Y%m%d-%H%M%S.txt";
        std::ofstream _chat_log_fs;
        std::vector<uint8_t> chunk_buffer;
        uint32_t _lastUpdateTime = 0;
        uint32_t _currentDeltaTime = 0;
        uint8_t default_group = 0;
        uint32_t _actionId = 0;
        NetworkPlayerId_t _playerId = 0;
        bool _closeLock = false;
        bool _requireClose = false;
        bool wsa_initialized = false;
        bool _playerListInvalidated = false;
    };

} // namespace OpenRCT2

#endif // DISABLE_NETWORK
