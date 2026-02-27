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
        auto* ctx = GetContext();
        auto* competitionMgr = ctx->GetCompetitionManager();

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
        auto* ctx = GetContext();
        auto* competitionMgr = ctx->GetCompetitionManager();
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
        auto* ctx = GetContext();
        auto* competitionMgr = ctx->GetCompetitionManager();

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

        auto* ctx = GetContext();
        auto* competitionMgr = ctx->GetCompetitionManager();
        
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

        auto* ctx = GetContext();
        auto* competitionMgr = ctx->GetCompetitionManager();
        
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

        auto* ctx = GetContext();
        auto* competitionMgr = ctx->GetCompetitionManager();

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
        bounds.TopLeft = TileCoordsXY{ topLeftX, topLeftY };
        bounds.BottomRight = TileCoordsXY{ bottomRightX, bottomRightY };

        auto* ctx = GetContext();
        auto* competitionMgr = ctx->GetCompetitionManager();
        auto& territoryMgr = competitionMgr->GetTerritoryManager();

        territoryMgr.CreateTerritory(playerId, bounds);
    }

} // namespace OpenRCT2::Network

#endif // DISABLE_NETWORK
