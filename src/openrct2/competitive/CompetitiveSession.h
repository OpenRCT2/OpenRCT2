/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "CompetitiveState.h"
#include "../Identifiers.h"

#include <memory>
#include <string>
#include <vector>

namespace OpenRCT2
{
    struct Guest;
}

namespace OpenRCT2::Competitive
{
    enum class SessionMode : uint8_t
    {
        none,
        host,
        client,
    };

    enum class ConnectionStatus : uint8_t
    {
        disconnected,
        connecting,
        online,
        reconnecting,
        error,
    };

    struct HostConfiguration
    {
        std::string competitionName;
        std::string playerName;
        std::string listenAddress;
        uint16_t port = kDefaultPort;
        MatchRules rules{};
        ScenarioIdentity scenario{};
    };

    struct JoinConfiguration
    {
        std::string host;
        uint16_t port = kDefaultPort;
        std::string playerName;
        Role role = Role::player;
    };

    class Session final
    {
    public:
        Session();
        ~Session();

        Session(const Session&) = delete;
        Session& operator=(const Session&) = delete;

        [[nodiscard]] bool StartHost(const HostConfiguration& configuration, std::string& error);
        [[nodiscard]] bool Join(const JoinConfiguration& configuration, std::string& error);
        void Stop();
        void Update();

        [[nodiscard]] SessionMode GetMode() const;
        [[nodiscard]] ConnectionStatus GetConnectionStatus() const;
        [[nodiscard]] const MatchState* GetState() const;
        [[nodiscard]] ParticipantId GetLocalParticipantId() const;
        [[nodiscard]] const Participant* GetLocalParticipant() const;
        [[nodiscard]] bool IsOnline() const;
        [[nodiscard]] const std::string& GetLastError() const;
        [[nodiscard]] std::string GetStatusText() const;

        [[nodiscard]] std::vector<std::string> GetStartProblems() const;
        [[nodiscard]] bool SetReady(bool ready, std::string& error);
        [[nodiscard]] bool StartMatch(std::string& error);
        [[nodiscard]] bool UpdateRules(const MatchRules& rules, std::string& error);
        [[nodiscard]] bool UseAbility(Ability ability, ParticipantId targetId, int32_t targetRideId, std::string& error);
        [[nodiscard]] bool Forfeit(ParticipantId participantId, std::string& error);
        [[nodiscard]] bool CloseEarly(std::string& error);

        void UpdateGuestGenerationInterference();
        [[nodiscard]] bool ConsumeGuestArrivalCancellation();
        void OnGuestPurchase(OpenRCT2::Guest& guest, RideId rideId, bool isFoodOrDrink);
        void OnVandalDamage(OpenRCT2::Guest& guest);
        [[nodiscard]] money64 GetAvailableCompetitiveCash() const;
        [[nodiscard]] money64 GetConstructionSpend() const;
        [[nodiscard]] bool CanSpendConstruction(money64 cost) const;
        void RecordConstructionSpend(money64 cost);

    private:
        struct Impl;
        std::unique_ptr<Impl> _impl;
    };

    [[nodiscard]] Session& GetSession();
    void Update();

    [[nodiscard]] ScenarioIdentity GetCurrentScenarioIdentity();
    [[nodiscard]] ScenarioIdentity GetScenarioIdentityForPath(const std::string& path);
    [[nodiscard]] ParkMetrics CollectParkMetrics();

    void UpdateGuestGenerationInterference();
    [[nodiscard]] bool ConsumeGuestArrivalCancellation();
    void OnGuestPurchase(OpenRCT2::Guest& guest, RideId rideId, bool isFoodOrDrink);
    void OnVandalDamage(OpenRCT2::Guest& guest);
    [[nodiscard]] bool CanSpendConstruction(money64 cost);
    void RecordConstructionSpend(money64 cost);
} // namespace OpenRCT2::Competitive
