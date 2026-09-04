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
#include <optional>
#include <string>
#include <string_view>
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
        // notifyPeers: when leaving a live session, tell the host/clients so a deliberate
        // departure is not mistaken for a crash. Internal resets (StartHost/Join/restore) pass false.
        // forfeit:     permanent - the park is dropped from the result and can never rejoin. When
        //              false the host keeps the seat so the park can rejoin later (suspend). Only an
        //              explicit, confirmed "Forfeit" action passes true.
        void Stop(bool notifyPeers = true, bool forfeit = false);
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
        [[nodiscard]] bool WatchParticipant(ParticipantId targetId, std::string& error);
        [[nodiscard]] bool Forfeit(ParticipantId participantId, std::string& error);
        [[nodiscard]] bool CloseEarly(std::string& error);
        // Write a recovery save into saves/competitive-suspend/ and disconnect WITHOUT forfeiting -
        // the seat is held so the player can reload that save and rejoin while the match runs.
        // On success, outPath receives the save path. Only valid for a competing (non-spectator) park.
        [[nodiscard]] bool SuspendAndSave(std::string& outPath, std::string& error);
        // Relay a chat line to every other park in the competition via the coordinator.
        void SendChat(std::string_view text);

        void UpdateGuestGenerationInterference();
        [[nodiscard]] bool ConsumeGuestArrivalCancellation();
        void OnGuestPurchase(OpenRCT2::Guest& guest, RideId rideId, bool isFoodOrDrink);
        void OnVandalAttempt(OpenRCT2::Guest& guest, bool succeeded);
        void OnGuestExitRide(OpenRCT2::Guest& guest, RideId rideId);
        [[nodiscard]] bool IsProtectedAgent(EntityId guestId) const;
        [[nodiscard]] bool IsProtectedRide(RideId rideId) const;
        [[nodiscard]] money64 GetAvailableParkCash() const;
        [[nodiscard]] int32_t GetStaffWageMultiplier() const;
        // 0 = not in a disruptive group, 1 = Karen, 2 = Stoner.
        [[nodiscard]] uint8_t GetGroupGuestKind(EntityId guestId) const;
        [[nodiscard]] bool IsStaffDetained(EntityId staffId) const;
        // True while this Karen guest is cornering a staff member (both frozen face to face).
        [[nodiscard]] bool IsKarenConfrontingStaff(EntityId guestId) const;
        // Stoner "stop and stare" gate: false while the guest is on its wander-between-stares cooldown.
        [[nodiscard]] bool StonerMayStare(EntityId guestId) const;
        void NoteStonerStareEnded(EntityId guestId);
        [[nodiscard]] std::string ExportParkStorage() const;
        [[nodiscard]] bool RestoreParkStorage(std::string_view storage, std::string& error);

    private:
        struct Impl;
        std::unique_ptr<Impl> _impl;
    };

    [[nodiscard]] Session& GetSession();
    void Update();

    // Fast, allocation-free gate for the per-entity hooks in the core simulation (pathfinding,
    // guest AI, finance). It is only true while this park has at least one live rival actor
    // (vandal, operative, or disruptive guest group), so outside a competition - and in every
    // non-competitive game - the hooks pay nothing but a bool read.
    inline bool gLocalActorsActive = false;

    [[nodiscard]] ScenarioIdentity GetCurrentScenarioIdentity();
    [[nodiscard]] ScenarioIdentity GetScenarioIdentityForPath(const std::string& path);
    [[nodiscard]] ParkMetrics CollectParkMetrics();

    void UpdateGuestGenerationInterference();
    [[nodiscard]] bool ConsumeGuestArrivalCancellation();
    void OnGuestPurchase(OpenRCT2::Guest& guest, RideId rideId, bool isFoodOrDrink);
    void OnVandalAttempt(OpenRCT2::Guest& guest, bool succeeded);
    void OnGuestExitRide(OpenRCT2::Guest& guest, RideId rideId);
    [[nodiscard]] bool IsProtectedAgent(EntityId guestId);
    [[nodiscard]] bool IsProtectedRide(RideId rideId);
    // Multiplier applied to this park's staff wage payments (2 while a union-disruption effect is active, else 1).
    [[nodiscard]] int32_t GetStaffWageMultiplier();
    // 0 = not in a disruptive group, 1 = Karen, 2 = Stoner.
    [[nodiscard]] uint8_t GetGroupGuestKind(EntityId guestId);
    [[nodiscard]] bool IsStaffDetained(EntityId staffId);
    [[nodiscard]] bool IsKarenConfrontingStaff(EntityId guestId);
    [[nodiscard]] bool StonerMayStare(EntityId guestId);
    void NoteStonerStareEnded(EntityId guestId);
    [[nodiscard]] std::string ExportParkStorage();
    [[nodiscard]] bool RestoreParkStorage(std::string_view storage, std::string& error);
    [[nodiscard]] bool IsWatchingPark();
    [[nodiscard]] bool ReturnFromWatchedPark(std::string& error);

    // A recovery park written by Session::SuspendAndSave, tracked in the competitive-suspend index
    // so the competitive server list can offer to reload it instead of a fresh join.
    struct SuspendedSave
    {
        std::string matchId;
        std::string name; // competition display name
        std::string host;
        uint16_t port = 0;
        Role role = Role::player;
        std::string savePath;
    };

    // Every tracked suspended save whose park file still exists, newest first.
    [[nodiscard]] std::vector<SuspendedSave> ListSuspendedSaves();
    // The tracked suspended save for exactly this competition, matched on matchId (a per-competition
    // token). matchId must be non-empty - a host running a different competition, even on the same
    // address, has a different matchId and must not match.
    [[nodiscard]] std::optional<SuspendedSave> FindSuspendedSave(std::string_view matchId);
} // namespace OpenRCT2::Competitive
