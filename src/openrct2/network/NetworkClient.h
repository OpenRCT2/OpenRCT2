#pragma once

#ifndef DISABLE_NETWORK

#    include "NetworkBase.h"

namespace OpenRCT2
{
    class NetworkClient final : public NetworkBase
    {
    public:
        static constexpr int32_t NetworkMode = NETWORK_MODE_CLIENT;

    public:
        NetworkClient(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);

        bool BeginClient(const std::string& host, uint16_t port);
        void Reconnect();
        void Close() override;
        void Update() override;
        void PostUpdate() override;
        void Flush() override;
        int32_t GetMode() const override;
        NetworkStats_t GetStats() const override;
        NetworkAuth GetAuthStatus() override;

    public:
        // Packet dispatchers.
        void SendPassword(const std::string& password);
        void SendGameStateRequest(uint32_t tick);
        void SendGameStateRequest()
        {
            SendGameStateRequest(_serverState.desyncTick);
        }
        void SendToken();
        void SendAuthRequest(
            const std::string& name, const std::string& password, const std::string& pubkey,
            const std::vector<uint8_t>& signature);
        void SendPing();
        void SendGameInfoRequest();
        void SendMapRequest(const std::vector<std::string>& objects);
        void SendHeartbeat(NetworkConnection& connection) const;
        void SendChat(const char* text);
        void SendGameAction(const GameAction* action);

    private:
        void ProcessPlayerInfo();
        void ProcessPlayerList();

    private:
        void OnPacketAuth(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketMap(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketChat(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketGameAction(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketTick(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketPlayerInfo(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketPlayerList(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketPing(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketPingList(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketDisconnectMsg(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketGameInfo(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketShowError(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketGroupList(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketEvent(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketToken(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketObjectsList(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketScripts(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketGameState(NetworkConnection& connection, NetworkPacket& packet);
    };

} // namespace OpenRCT2

#endif // DISABLE_NETWORK
