/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NetworkBase.h"

#include "../Context.h"
#include "../competition/CompetitionManager.h"
#include "../competition/PlayerTerritory.h"
#include "NetworkPacket.h"

#ifndef DISABLE_NETWORK

namespace OpenRCT2::Network
{
    // Server: Send competition start
    void NetworkBase::ServerSendCompetitionStart(const Competition::CompetitionConfig& config)
    {
        Packet packet;
        packet << static_cast<uint32_t>(Command::competitionStart);
        packet << static_cast<uint8_t>(config.Type);
        packet << config.DurationMinutes;
        packet << config.MaxPlayers;
        packet << static_cast<uint8_t>(config.EnableChat ? 1 : 0);
        packet << static_cast<uint8_t>(config.ShowLiveScores ? 1 : 0);
        packet << config.MapName;

        SendPacketToClients(packet);
    }

    // Server: Send competition update
    void NetworkBase::ServerSendCompetitionUpdate()
    {
        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        if (!competitionMgr || !competitionMgr->IsActive())
            return;

        Packet packet;
        packet << static_cast<uint32_t>(Command::competitionUpdate);
        packet << competitionMgr->GetTimeRemaining();
        packet << static_cast<uint8_t>(competitionMgr->GetStatus());

        SendPacketToClients(packet);
    }

    // Server: Send competition end
    void NetworkBase::ServerSendCompetitionEnd(uint8_t winnerId)
    {
        Packet packet;
        packet << static_cast<uint32_t>(Command::competitionEnd);
        packet << winnerId;

        SendPacketToClients(packet);
    }

    // Server: Send score update for a player
    void NetworkBase::ServerSendScoreUpdate(uint8_t playerId)
    {
        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();
        auto* playerData = competitionMgr->GetPlayerData(playerId);

        if (!playerData)
            return;

        Packet packet;
        packet << static_cast<uint32_t>(Command::scoreUpdate);
        packet << playerId;
        packet << playerData->Stats.Score;
        packet << playerData->Rank;
        packet << static_cast<uint8_t>(playerData->IsEliminated ? 1 : 0);

        SendPacketToClients(packet);
    }

    // Server: Send full leaderboard
    void NetworkBase::ServerSendLeaderboard()
    {
        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        if (!competitionMgr || !competitionMgr->IsActive())
            return;

        auto leaderboard = competitionMgr->GetLeaderboard();

        Packet packet;
        packet << static_cast<uint32_t>(Command::leaderboardUpdate);
        packet << static_cast<uint32_t>(leaderboard.size());

        for (const auto& entry : leaderboard)
        {
            packet << entry.PlayerId;
            packet << entry.PlayerName;
            packet << entry.Stats.Score;
            packet << entry.Rank;
            packet << entry.Stats.ParkValue;
            packet << entry.Stats.GuestCount;
            packet << entry.Stats.RidesBuilt;
        }

        SendPacketToClients(packet);
    }

    // Server: Send territory assignment
    void NetworkBase::ServerSendTerritoryAssignment(
        uint8_t playerId, const Competition::TerritoryBounds& bounds)
    {
        Packet packet;
        packet << static_cast<uint32_t>(Command::competitionStart); // Reuse start for territory info
        packet << playerId;
        packet << bounds.TopLeft.x;
        packet << bounds.TopLeft.y;
        packet << bounds.BottomRight.x;
        packet << bounds.BottomRight.y;

        SendPacketToClients(packet);
    }

    // Server: Handle competition ready
    void NetworkBase::ServerHandleCompetitionReady(Connection& connection, Packet& packet)
    {
        // Player indicated they're ready to start competition
        // Can be used for lobby synchronization
    }

    // Client: Send competition ready
    void NetworkBase::Client_Send_CompetitionReady()
    {
        Packet packet;
        packet << static_cast<uint32_t>(Command::competitionStart);
        packet << static_cast<uint8_t>(1); // Ready flag

        _serverConnection->QueuePacket(std::move(packet));
    }

    // Client: Handle competition start
    void NetworkBase::Client_Handle_COMPETITION_START(Connection& connection, Packet& packet)
    {
        Competition::CompetitionConfig config;
        
        uint8_t typeValue;
        packet >> typeValue;
        config.Type = static_cast<Competition::CompetitionType>(typeValue);
        packet >> config.DurationMinutes;
        packet >> config.MaxPlayers;
        
        uint8_t enableChat, showScores;
        packet >> enableChat >> showScores;
        config.EnableChat = (enableChat != 0);
        config.ShowLiveScores = (showScores != 0);
        packet >> config.MapName;

        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();
        
        competitionMgr->Initialize(config);
        // Don't start yet - wait for territory assignment and server signal
    }

    // Client: Handle competition update
    void NetworkBase::Client_Handle_COMPETITION_UPDATE(Connection& connection, Packet& packet)
    {
        uint32_t timeRemaining;
        uint8_t statusValue;
        
        packet >> timeRemaining >> statusValue;

        // Update local competition state
        // This is passive - just reflects server state
    }

    // Client: Handle competition end
    void NetworkBase::Client_Handle_COMPETITION_END(Connection& connection, Packet& packet)
    {
        uint8_t winnerId;
        packet >> winnerId;

        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();
        
        competitionMgr->End();
        
        // Show victory/defeat screen based on winnerId
    }

    // Client: Handle score update
    void NetworkBase::Client_Handle_SCORE_UPDATE(Connection& connection, Packet& packet)
    {
        uint8_t playerId, isEliminated;
        uint32_t score, rank;
        
        packet >> playerId >> score >> rank >> isEliminated;

        auto* player = GetPlayerByID(playerId);
        if (player)
        {
            player->CompetitionScore = score;
            player->CompetitionRank = rank;
        }
    }

    // Client: Handle leaderboard update
    void NetworkBase::Client_Handle_LEADERBOARD_UPDATE(Connection& connection, Packet& packet)
    {
        uint32_t numPlayers;
        packet >> numPlayers;

        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        for (uint32_t i = 0; i < numPlayers; i++)
        {
            uint8_t playerId;
            std::string playerName;
            uint32_t score, rank, guestCount, ridesBuilt;
            money64 parkValue;

            packet >> playerId >> playerName >> score >> rank;
            packet >> parkValue >> guestCount >> ridesBuilt;

            auto* playerData = competitionMgr->GetPlayerData(playerId);
            if (playerData)
            {
                playerData->Stats.Score = score;
                playerData->Rank = rank;
                playerData->Stats.ParkValue = parkValue;
                playerData->Stats.GuestCount = guestCount;
                playerData->Stats.RidesBuilt = ridesBuilt;
            }
        }
    }

    // Client: Handle territory assignment
    void NetworkBase::Client_Handle_TERRITORY_ASSIGNMENT(Connection& connection, Packet& packet)
    {
        uint8_t playerId;
        int32_t topLeftX, topLeftY, bottomRightX, bottomRightY;

        packet >> playerId;
        packet >> topLeftX >> topLeftY >> bottomRightX >> bottomRightY;

        Competition::TerritoryBounds bounds;
        // Territory assignment logic would go here
    }

    // ===========================
    // LOBBY NETWORK FUNCTIONS
    // ===========================

    // Server: Send lobby state sync to all clients
    void NetworkBase::ServerSendLobbyStateSync()
    {
        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        if (!competitionMgr || !competitionMgr->IsInLobby())
            return;

        const auto& players = competitionMgr->GetAllPlayerData();
        const auto& config = competitionMgr->GetConfig();

        Packet packet;
        packet << static_cast<uint32_t>(Command::lobbyStateSync);
        packet << static_cast<uint8_t>(config.Type);
        packet << config.DurationMinutes;
        packet << config.MaxPlayers;
        packet << static_cast<uint32_t>(players.size());

        for (const auto& player : players)
        {
            packet << player.PlayerId;
            packet << player.PlayerName;
            packet << static_cast<uint8_t>(player.IsReady ? 1 : 0);
        }

        SendPacketToClients(packet);
    }

    // Server: Send lobby player joined
    void NetworkBase::ServerSendLobbyPlayerJoined(uint8_t playerId, const std::string& name)
    {
        Packet packet;
        packet << static_cast<uint32_t>(Command::lobbyJoin);
        packet << playerId;
        packet << name;

        SendPacketToClients(packet);
    }

    // Server: Send lobby player left
    void NetworkBase::ServerSendLobbyPlayerLeft(uint8_t playerId)
    {
        Packet packet;
        packet << static_cast<uint32_t>(Command::lobbyLeave);
        packet << playerId;

        SendPacketToClients(packet);
    }

    // Server: Send lobby player ready state change
    void NetworkBase::ServerSendLobbyPlayerReady(uint8_t playerId, bool ready)
    {
        Packet packet;
        packet << static_cast<uint32_t>(Command::lobbyPlayerReady);
        packet << playerId;
        packet << static_cast<uint8_t>(ready ? 1 : 0);

        SendPacketToClients(packet);
    }

    // Server: Handle lobby join request
    void NetworkBase::ServerHandleLobbyJoin(Connection& connection, Packet& packet)
    {
        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        if (!competitionMgr || !competitionMgr->IsInLobby())
            return;

        auto* player = GetPlayerByID(connection.player->Id);
        if (!player)
            return;

        if (competitionMgr->AddPlayerToLobby(player->Id, player->Name))
        {
            ServerSendLobbyPlayerJoined(player->Id, player->Name);
            ServerSendLobbyStateSync();
        }
    }

    // Server: Handle lobby leave request
    void NetworkBase::ServerHandleLobbyLeave(Connection& connection, Packet& packet)
    {
        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        if (!competitionMgr || !competitionMgr->IsInLobby())
            return;

        auto* player = GetPlayerByID(connection.player->Id);
        if (!player)
            return;

        competitionMgr->RemovePlayerFromLobby(player->Id);
        ServerSendLobbyPlayerLeft(player->Id);
        ServerSendLobbyStateSync();
    }

    // Server: Handle player ready state change
    void NetworkBase::ServerHandleLobbyPlayerReady(Connection& connection, Packet& packet)
    {
        uint8_t ready;
        packet >> ready;

        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        if (!competitionMgr || !competitionMgr->IsInLobby())
            return;

        auto* player = GetPlayerByID(connection.player->Id);
        if (!player)
            return;

        competitionMgr->SetPlayerReady(player->Id, ready != 0);
        ServerSendLobbyPlayerReady(player->Id, ready != 0);
        ServerSendLobbyStateSync();
    }

    // Server: Handle lobby start request (host only)
    void NetworkBase::ServerHandleLobbyStartRequest(Connection& connection, Packet& packet)
    {
        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        if (!competitionMgr || !competitionMgr->IsInLobby())
            return;

        auto* player = GetPlayerByID(connection.player->Id);
        if (!player || player->Id != 0) // Only host (player 0) can start
            return;

        if (competitionMgr->CanStartCompetition())
        {
            competitionMgr->Start();
            ServerSendCompetitionStart(competitionMgr->GetConfig());
        }
    }

    // Client: Send lobby join request
    void NetworkBase::Client_Send_LobbyJoin()
    {
        Packet packet;
        packet << static_cast<uint32_t>(Command::lobbyJoin);
        _serverConnection->QueuePacket(std::move(packet));
    }

    // Client: Send lobby leave request
    void NetworkBase::Client_Send_LobbyLeave()
    {
        Packet packet;
        packet << static_cast<uint32_t>(Command::lobbyLeave);
        _serverConnection->QueuePacket(std::move(packet));
    }

    // Client: Send player ready state
    void NetworkBase::Client_Send_LobbyPlayerReady(bool ready)
    {
        Packet packet;
        packet << static_cast<uint32_t>(Command::lobbyPlayerReady);
        packet << static_cast<uint8_t>(ready ? 1 : 0);
        _serverConnection->QueuePacket(std::move(packet));
    }

    // Client: Send lobby start request
    void NetworkBase::Client_Send_LobbyStartRequest()
    {
        Packet packet;
        packet << static_cast<uint32_t>(Command::lobbyStartRequest);
        _serverConnection->QueuePacket(std::move(packet));
    }

    // Client: Handle lobby state sync
    void NetworkBase::Client_Handle_LOBBY_STATE_SYNC(Connection& connection, Packet& packet)
    {
        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        if (!competitionMgr)
            return;

        uint8_t typeValue;
        uint32_t durationMinutes, maxPlayers, numPlayers;

        packet >> typeValue >> durationMinutes >> maxPlayers >> numPlayers;

        Competition::CompetitionConfig config;
        config.Type = static_cast<Competition::CompetitionType>(typeValue);
        config.DurationMinutes = durationMinutes;
        config.MaxPlayers = maxPlayers;

        if (!competitionMgr->IsInLobby())
        {
            competitionMgr->Initialize(config);
        }

        // Clear existing player data and rebuild from packet
        competitionMgr->Reset();
        competitionMgr->Initialize(config);

        for (uint32_t i = 0; i < numPlayers; i++)
        {
            uint8_t playerId, ready;
            std::string playerName;

            packet >> playerId >> playerName >> ready;

            competitionMgr->AddPlayerToLobby(playerId, playerName);
            competitionMgr->SetPlayerReady(playerId, ready != 0);
        }
    }

    // Client: Handle player joined lobby
    void NetworkBase::Client_Handle_LOBBY_PLAYER_JOINED(Connection& connection, Packet& packet)
    {
        uint8_t playerId;
        std::string playerName;

        packet >> playerId >> playerName;

        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        if (competitionMgr && competitionMgr->IsInLobby())
        {
            competitionMgr->AddPlayerToLobby(playerId, playerName);
        }
    }

    // Client: Handle player left lobby
    void NetworkBase::Client_Handle_LOBBY_PLAYER_LEFT(Connection& connection, Packet& packet)
    {
        uint8_t playerId;
        packet >> playerId;

        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        if (competitionMgr && competitionMgr->IsInLobby())
        {
            competitionMgr->RemovePlayerFromLobby(playerId);
        }
    }

    // Client: Handle player ready state change
    void NetworkBase::Client_Handle_LOBBY_PLAYER_READY(Connection& connection, Packet& packet)
    {
        uint8_t playerId, ready;
        packet >> playerId >> ready;

        auto& ctx = GetContext();
        auto* competitionMgr = ctx.GetCompetitionManager();

        if (competitionMgr && competitionMgr->IsInLobby())
        {
            competitionMgr->SetPlayerReady(playerId, ready != 0);
        }
    }

} // namespace OpenRCT2::Network

#endif // DISABLE_NETWORK
