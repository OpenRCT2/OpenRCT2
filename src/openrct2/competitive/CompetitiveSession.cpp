/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CompetitiveSession.h"

#include "CompetitiveProtocol.h"

#include "../actions/GameActionRunner.h"
#include "../actions/ride/RideDemolishAction.h"
#include "../audio/Audio.h"
#include "../Cheats.h"
#include "../Context.h"
#include "../Date.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../config/Config.h"
#include "../core/Crypt.h"
#include "../core/File.h"
#include "../core/Json.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../entity/EntityList.h"
#include "../entity/Guest.h"
#include "../entity/Particle.h"
#include "../entity/Staff.h"
#include "../interface/Chat.h"
#include "../management/Finance.h"
#include "../management/NewsItem.h"
#include "../management/Research.h"
#include "../network/Network.h"
#include "../network/NetworkConnection.h"
#include "../network/NetworkServerAdvertiser.h"
#include "../network/Socket.h"
#include "../platform/Platform.h"
#include "../ride/RideManager.hpp"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioRepository.h"
#include "../util/Util.h"
#include "../world/Location.hpp"
#include "../world/Park.h"

#include <algorithm>
#include <cstdio>
#include <list>
#include <random>
#include <unordered_map>
#include <unordered_set>

namespace OpenRCT2::Competitive
{
#ifndef DISABLE_NETWORK
    namespace
    {
        constexpr uint32_t kHeartbeatIntervalMs = 5000;
        constexpr uint32_t kReconnectIntervalMs = 3000;
        constexpr int32_t kAutomaticSaveFlag = static_cast<int32_t>(1u << 31);

        struct WatchReturnState
        {
            bool active = false;
            bool hasReturnPark = false;
            std::string returnPath;
        };

        WatchReturnState gWatchReturnState;

        std::string MakeMatchId()
        {
            std::random_device random;
            char value[48]{};
            snprintf(
                value, sizeof(value), "%08x-%08x-%08x-%08x", random(), random(), random(),
                Platform::GetTicks());
            return value;
        }

        ParticipantId MakeParticipantId()
        {
            std::random_device random;
            ParticipantId result = (static_cast<ParticipantId>(random()) << 32) | random();
            return result == kInvalidParticipantId ? 1 : result;
        }

        uint32_t CountCompetingParks(const MatchState& state)
        {
            return static_cast<uint32_t>(std::count_if(
                state.participants.begin(), state.participants.end(),
                [](const Participant& participant) { return participant.role != Role::spectator && !participant.forfeited; }));
        }

        uint32_t GetCurrentLocalDay()
        {
            const auto& date = getGameState().date;
            return date.GetMonthsElapsed() * 32 + static_cast<uint32_t>(date.GetDay()) + 1;
        }

        const AbilityRule& GetAbilityRule(const MatchRules& rules, Ability ability)
        {
            switch (ability)
            {
                case Ability::misinformation:
                    return rules.misinformation;
                case Ability::poison:
                    return rules.poison;
                case Ability::toiletBomber:
                    return rules.toiletBomber;
                case Ability::agitator:
                    return rules.agitator;
                case Ability::saboteur:
                    return rules.saboteur;
                case Ability::hitman:
                    return rules.hitman;
                case Ability::researchSabotage:
                    return rules.researchSabotage;
                case Ability::karens:
                    return rules.karens;
                case Ability::stoners:
                    return rules.stoners;
                case Ability::unionDisruption:
                    return rules.unionDisruption;
                case Ability::vandal:
                    return rules.vandal;
            }
            return rules.vandal;
        }

        // Whether an existing effect clashes with a new use of `ability` against the same target.
        // Per-ride actions clash only on the same ride; research sabotage and union disruption toggle
        // park state and can't stack; every other action is additive, so allied attackers may pile
        // several onto one rival at once.
        bool EffectClashes(const ActiveEffect& existing, Ability ability, int32_t targetRideId)
        {
            if (existing.ability != ability)
                return false;
            if (ability == Ability::poison || ability == Ability::toiletBomber || ability == Ability::saboteur)
                return existing.targetRideId == targetRideId;
            return ability == Ability::researchSabotage || ability == Ability::unionDisruption;
        }

        uint16_t AbilityUsesThisYear(const MatchState& state, ParticipantId participantId, Ability ability, uint16_t year)
        {
            for (const auto& usage : state.usages)
            {
                if (usage.participantId == participantId && usage.ability == ability)
                    return usage.year == year ? usage.used : uint16_t{ 0 };
            }
            return 0;
        }

        void RecordAbilityUse(MatchState& state, ParticipantId participantId, Ability ability, uint16_t year)
        {
            for (auto& usage : state.usages)
            {
                if (usage.participantId == participantId && usage.ability == ability)
                {
                    if (usage.year != year)
                    {
                        usage.year = year;
                        usage.used = 0;
                    }
                    usage.used++;
                    return;
                }
            }
            state.usages.push_back({ participantId, ability, year, 1 });
        }

        const char* AbilityName(Ability ability)
        {
            switch (ability)
            {
                case Ability::vandal:
                    return "Vandal";
                case Ability::misinformation:
                    return "Misinformation";
                case Ability::poison:
                    return "Poisoning";
                case Ability::toiletBomber:
                    return "Toilet bomber";
                case Ability::agitator:
                    return "Agitator";
                case Ability::saboteur:
                    return "Saboteur";
                case Ability::hitman:
                    return "Hitman";
                case Ability::researchSabotage:
                    return "Research sabotage";
                case Ability::karens:
                    return "Karens";
                case Ability::stoners:
                    return "Stoners";
                case Ability::unionDisruption:
                    return "Union disruption";
            }
            return "Rival action";
        }

        std::string MetricName(Metric metric)
        {
            switch (metric)
            {
                case Metric::rating:
                    return "park rating";
                case Metric::guests:
                    return "guests";
                case Metric::cash:
                    return "cash";
                case Metric::parkValue:
                    return "park value";
                case Metric::points:
                    return "points";
            }
            return "points";
        }

        std::string PhaseName(Phase phase)
        {
            switch (phase)
            {
                case Phase::lobby:
                    return "lobby";
                case Phase::running:
                    return "running";
                case Phase::finished:
                    return "finished";
                case Phase::none:
                    return "none";
            }
            return "none";
        }

        void QueueJson(Network::Connection& connection, Network::Command command, const json_t& body)
        {
            Network::Packet packet(command);
            packet.writeString(body.dump());
            connection.queuePacket(packet);
        }

        std::optional<json_t> ReadJson(Network::Packet& packet)
        {
            const auto raw = packet.readString();
            if (raw.empty())
            {
                return std::nullopt;
            }
            try
            {
                auto value = Json::FromString(raw);
                return value.is_object() ? std::optional<json_t>{ std::move(value) } : std::nullopt;
            }
            catch (const std::exception&)
            {
                return std::nullopt;
            }
        }

        std::string HashFile(const std::string& path)
        {
            try
            {
                const auto data = File::ReadAllBytes(path);
                if (data.empty())
                {
                    return {};
                }
                return String::StringFromHex(Crypt::SHA256(data.data(), data.size()));
            }
            catch (const std::exception&)
            {
                return {};
            }
        }
    } // namespace

    struct Session::Impl
    {
        struct Peer
        {
            std::unique_ptr<Network::Connection> connection;
            ParticipantId participantId = kInvalidParticipantId;
            uint32_t lastSeenAt{};
        };

        struct LocalVandal
        {
            uint32_t effectId{};
            EntityId guestId = EntityId::GetNull();
            uint16_t remainingQuota{};
            uint32_t nextAttemptTick{};
        };

        struct PendingAbilityPayment
        {
            Ability ability = Ability::vandal;
            ParticipantId targetId = kInvalidParticipantId;
            money64 cost{};
        };

        struct LocalOperative
        {
            uint32_t effectId{};
            Ability ability = Ability::agitator;
            EntityId guestId = EntityId::GetNull();
            int32_t targetRideId = -1;
            EntityId victimId = EntityId::GetNull();
            uint8_t stage{};
            uint32_t triggerAtTick{};
            std::vector<EntityId> affectedGuests;
        };

        // A "coach party" of disruptive guests (Karens / Stoners) spawned in the victim's park.
        struct LocalGroup
        {
            uint32_t effectId{};
            Ability ability = Ability::karens;
            std::vector<EntityId> guestIds;
            uint32_t nextScanTick{}; // throttles the per-member Karen / Stoner behaviour scan
        };

        // One staff member cornered by a Karen: both stand frozen, face to face, until releaseTick.
        struct StaffDetention
        {
            uint32_t releaseTick{};
            EntityId karenId = EntityId::GetNull();
        };
        static constexpr uint32_t kStaffDetentionTicks = 400;        // ~10s that a staff member is held
        static constexpr uint32_t kKarenComplaintCooldownTicks = 2400; // ~60s before a Karen demands again
        static constexpr uint32_t kStaffComplaintCooldownTicks = 600;  // ~15s of peace for a released staff member
        static constexpr uint32_t kStonerPuffMinTicks = 4800;         // ~2 min minimum between a Stoner's smoke puffs
        static constexpr uint32_t kStonerPuffJitterTicks = 7200;      // + up to ~3 min of random jitter (so ~2-5 min)
        static constexpr uint32_t kStonerStareGuardTicks = 2400;      // ~60s a Stoner must wander before staring again
        static constexpr int32_t kStonerPuffHappinessHit = 5;        // minor mood hit for a bystander in the smoke

        SessionMode mode = SessionMode::none;
        ConnectionStatus status = ConnectionStatus::disconnected;
        MatchState state{};
        ParticipantId localParticipantId = kInvalidParticipantId;
        Role localRole = Role::none;
        std::string lastError;
        std::unique_ptr<Network::ITcpSocket> listener;
        std::unique_ptr<Network::INetworkServerAdvertiser> advertiser;
        std::list<Peer> peers;
        std::unique_ptr<Network::Connection> serverConnection;
        HostConfiguration hostConfiguration{};
        JoinConfiguration joinConfiguration{};
        bool helloSent = false;
        bool scenarioLoaded = false;
        bool loadingScenario = false; // re-entrancy guard around LoadRequiredScenario's park load
        uint32_t lastHeartbeatAt{};
        uint32_t nextReconnectAt{};
        uint32_t lastReportedDay{};
        bool forcedPause = false;
        bool wasPausedBeforeForcedPause = false;
        uint32_t pendingMisinformationCancellations{};
        uint32_t nextAbilityRequestId = 1;
        std::unordered_map<uint32_t, PendingAbilityPayment> pendingAbilityPayments;
        std::unordered_map<uint32_t, ActiveEffect> localEffects;
        std::unordered_set<uint32_t> alertedEffects;
        std::vector<LocalVandal> localVandals;
        std::vector<LocalOperative> localOperatives;
        std::vector<LocalGroup> localGroups;
        // Flat guest-id -> spawning-ability index over every live rival actor, so the core-simulation
        // hooks (pathfinding, guest AI) resolve "is this a bad actor / which group" in O(1). Rebuilt
        // whenever actors are added or removed.
        std::unordered_map<uint32_t, Ability> actorKinds;
        std::unordered_map<uint32_t, StaffDetention> detainedStaff; // staff entity id -> who holds them and until when
        std::unordered_map<uint32_t, uint32_t> karenComplaintCooldown; // Karen guest id -> earliest tick it may corner staff again
        std::unordered_map<uint32_t, uint32_t> staffComplaintCooldown; // staff id -> earliest tick it may be cornered again
        std::unordered_map<uint32_t, uint32_t> stonerNextPuffTick;     // Stoner guest id -> next tick it puffs smoke
        std::unordered_map<uint32_t, uint32_t> stonerLastStareTick;    // Stoner guest id -> tick it last started staring
        bool openWindowAfterRestore = false;
        bool hostLossHandled = false;
        bool startedWatchServer = false;

        void SetError(std::string message)
        {
            lastError = std::move(message);
            status = ConnectionStatus::error;
        }

        void ClearTransport()
        {
            advertiser.reset();
            peers.clear();
            listener.reset();
            serverConnection.reset();
            helloSent = false;
        }

        void EnsurePauseState()
        {
            const bool isParticipant = localRole == Role::host || localRole == Role::player;
            const bool mustPause = isParticipant && gLegacyScene == LegacyScene::playing
                && (state.phase == Phase::lobby || status == ConnectionStatus::reconnecting
                    || status == ConnectionStatus::disconnected || status == ConnectionStatus::error);
            if (mustPause)
            {
                if (!forcedPause)
                {
                    wasPausedBeforeForcedPause = GameIsPaused();
                    forcedPause = true;
                }
                if (!GameIsPaused())
                {
                    PauseToggle();
                }
            }
            else if (forcedPause)
            {
                forcedPause = false;
                if (!wasPausedBeforeForcedPause && GameIsPaused())
                {
                    PauseToggle();
                }
            }
        }

        Peer* FindPeer(ParticipantId participantId)
        {
            const auto iterator = std::find_if(peers.begin(), peers.end(), [&](const auto& peer) {
                return peer.participantId == participantId && peer.connection != nullptr && peer.connection->isValid();
            });
            return iterator == peers.end() ? nullptr : &*iterator;
        }

        void AddLocalNotice(const std::string& message, News::ItemType type = News::ItemType::campaign, uint32_t subject = 0)
        {
            if (gLegacyScene == LegacyScene::playing || localRole == Role::spectator)
                News::AddItemToQueue(type, message.c_str(), subject);
        }

        // Shows a competition-wide chat line in this park's chat window.
        void ShowChatLine(const std::string& from, const std::string& text)
        {
            if (text.empty())
                return;
            ChatAddHistory("{BABYBLUE}[Competition] " + from + ":{WHITE} " + text);
        }

        // Host: echo a chat line locally and forward it to every connected park.
        void HostBroadcastChat(const std::string& from, const std::string& text)
        {
            ShowChatLine(from, text);
            const json_t body = { { "from", from }, { "text", text } };
            for (auto& peer : peers)
            {
                if (peer.participantId != kInvalidParticipantId && peer.connection != nullptr
                    && peer.connection->isValid())
                    QueueJson(*peer.connection, Network::Command::competitiveChat, body);
            }
        }

        void SendChatLine(std::string text)
        {
            if (text.size() > 240)
                text.resize(240);
            if (text.empty() || state.phase == Phase::none)
                return;
            const auto* me = FindParticipant(state, localParticipantId);
            const std::string from = me != nullptr && !me->name.empty() ? me->name : std::string("competitor");
            if (mode == SessionMode::host)
            {
                HostBroadcastChat(from, text);
            }
            else if (serverConnection != nullptr && serverConnection->isValid())
            {
                QueueJson(*serverConnection, Network::Command::competitiveChat, { { "text", std::move(text) } });
            }
        }

        // Posts a prominent, map-anchored notice for a guest killed by a rival action, mirroring ride/drowning deaths.
        void AddLocalDeathNotice(const std::string& message, const CoordsXY& location)
        {
            if (gLegacyScene == LegacyScene::playing || localRole == Role::spectator)
            {
                const uint32_t subject = static_cast<uint16_t>(location.x) | (static_cast<uint32_t>(location.y) << 16);
                News::AddItemToQueue(News::ItemType::blank, message.c_str(), subject);
            }
        }

        // Fires the victim-facing alert for an effect the first time its harmful action actually lands.
        void AlertVictimOnce(uint32_t effectId, const std::string& message, uint32_t guestSubject = 0)
        {
            if (!alertedEffects.insert(effectId).second)
                return;
            AddLocalNotice(
                message, guestSubject != 0 ? News::ItemType::peepOnRide : News::ItemType::campaign, guestSubject);
        }

        void HandleHostConnectionLoss(uint32_t now)
        {
            const bool wasOnline = status == ConnectionStatus::online;
            status = ConnectionStatus::reconnecting;
            nextReconnectAt = now + kReconnectIntervalMs;
            if (auto* participant = FindParticipant(state, localParticipantId))
                participant->online = false;

            if (!wasOnline || hostLossHandled || localRole != Role::player || !scenarioLoaded
                || state.phase != Phase::running || gLegacyScene != LegacyScene::playing)
                return;

            hostLossHandled = true;
            GameAutosave();
            AddLocalNotice(
                "Connection to the competition host was lost. Your park was paused and recovery-autosaved; "
                "OpenRCT2 will keep trying to reconnect.");
            ContextOpenWindow(WindowClass::multiplayer);
        }

        void NotifyParticipant(ParticipantId participantId, const std::string& message)
        {
            if (participantId == localParticipantId)
            {
                AddLocalNotice(message);
                return;
            }
            if (auto* peer = FindPeer(participantId))
            {
                QueueJson(
                    *peer->connection, Network::Command::competitiveEffectAck,
                    { { "notification", message } });
            }
        }

        void ReportLocalMetricsNow()
        {
            if (!scenarioLoaded || gLegacyScene != LegacyScene::playing || localParticipantId == kInvalidParticipantId)
                return;
            const auto metrics = CollectParkMetrics();
            if (mode == SessionMode::host)
            {
                IngestMetrics(localParticipantId, metrics);
            }
            else if (mode == SessionMode::client && serverConnection != nullptr && serverConnection->isValid())
            {
                QueueJson(
                    *serverConnection, Network::Command::competitiveMetrics,
                    { { "metrics", ToJson(metrics) } });
            }
        }

        void ResolveAbilityPayment(
            ParticipantId sourceId, uint32_t requestId, bool accepted, const std::string& message)
        {
            if (requestId == 0)
                return;
            const auto notification = accepted || state.scenario.noMoney
                ? message
                : message + " Its park-cash cost was refunded.";
            if (sourceId == localParticipantId)
            {
                const auto pending = pendingAbilityPayments.find(requestId);
                if (pending == pendingAbilityPayments.end())
                    return;
                if (!accepted && pending->second.cost > 0 && !state.scenario.noMoney)
                    FinancePayment(-pending->second.cost, ExpenditureType::marketing);
                pendingAbilityPayments.erase(pending);
                ReportLocalMetricsNow();
                AddLocalNotice(notification);
                return;
            }
            if (auto* peer = FindPeer(sourceId))
            {
                QueueJson(
                    *peer->connection, Network::Command::competitiveEffectAck,
                    {
                        { "paymentRequestId", requestId },
                        { "paymentAccepted", accepted },
                        { "notification", notification },
                    });
            }
        }

        void HandleAbilityPaymentResult(const json_t& body)
        {
            const auto requestId = body.value("paymentRequestId", 0u);
            if (requestId == 0)
                return;
            const auto pending = pendingAbilityPayments.find(requestId);
            if (pending == pendingAbilityPayments.end())
                return;
            if (!body.value("paymentAccepted", false) && pending->second.cost > 0 && !state.scenario.noMoney)
                FinancePayment(-pending->second.cost, ExpenditureType::marketing);
            pendingAbilityPayments.erase(pending);
            ReportLocalMetricsNow();
        }

        bool EnsureLocalWatchServer(std::string& warning)
        {
            if (localRole == Role::spectator || !scenarioLoaded || gLegacyScene != LegacyScene::playing)
                return true;

            uint16_t port{};
            if (Network::GetMode() == Network::Mode::server)
            {
                port = Network::GetListeningPort();
            }
            else if (Network::GetMode() == Network::Mode::none)
            {
                constexpr uint16_t kFirstWatchPort = 12000;
                constexpr uint16_t kWatchPortCount = 1000;
                const auto firstOffset = static_cast<uint16_t>(localParticipantId % kWatchPortCount);
                for (uint16_t attempt = 0; attempt < 64; attempt++)
                {
                    const auto candidate = static_cast<uint16_t>(
                        kFirstWatchPort + ((firstOffset + attempt) % kWatchPortCount));
                    if (Network::BeginServer(candidate, Config::Get().network.listenAddress, false, true))
                    {
                        port = candidate;
                        startedWatchServer = true;
                        break;
                    }
                }
            }

            auto* participant = FindParticipant(state, localParticipantId);
            if (participant == nullptr || port == 0)
            {
                warning = Network::GetMode() == Network::Mode::client
                    ? "This park is itself a normal multiplayer client, so its server must publish the watch endpoint."
                    : "No local port was available for native park spectating.";
                return false;
            }

            participant->watchPort = port;
            if (mode == SessionMode::host)
            {
                participant->watchHost = Config::Get().network.advertiseAddress;
                SendSnapshot();
            }
            else if (serverConnection != nullptr && serverConnection->isValid())
            {
                QueueJson(
                    *serverConnection, Network::Command::competitiveWatchEndpoint,
                    { { "port", participant->watchPort } });
            }
            return true;
        }

        void SendLocalEffectReply(uint32_t effectId, bool accepted, const std::string& message, bool complete = false)
        {
            if (mode == SessionMode::host)
            {
                ResolveEffectReply(localParticipantId, effectId, accepted, message, complete);
            }
            else if (mode == SessionMode::client && serverConnection != nullptr && serverConnection->isValid())
            {
                QueueJson(
                    *serverConnection, Network::Command::competitiveEffectAck,
                    { { "effectId", effectId }, { "accepted", accepted }, { "message", message }, { "complete", complete } });
            }
        }

        bool IsOpenFoodDrinkStall(int32_t rideId) const
        {
            if (rideId < 0 || rideId > std::numeric_limits<RideId::UnderlyingType>::max())
                return false;
            const auto* ride = GetRide(RideId::FromUnderlying(static_cast<RideId::UnderlyingType>(rideId)));
            if (ride == nullptr || ride->status != RideStatus::open
                || ride->getClassification() != RideClassification::shopOrStall)
                return false;
            const auto* rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return false;
            for (const auto item : rideEntry->shop_item)
            {
                if (item != ShopItem::none && GetShopItemDescriptor(item).IsFoodOrDrink())
                    return true;
            }
            return false;
        }

        bool IsValidOperativeTarget(Ability ability, int32_t rideId) const
        {
            if (rideId < 0 || rideId > std::numeric_limits<RideId::UnderlyingType>::max())
                return false;
            const auto* ride = GetRide(RideId::FromUnderlying(static_cast<RideId::UnderlyingType>(rideId)));
            if (ride == nullptr || ride->status != RideStatus::open)
                return false;
            if (ability == Ability::toiletBomber)
                return ride->getRideTypeDescriptor().specialType == RtdSpecialType::toilet;
            if (ability == Ability::saboteur)
                return ride->getClassification() == RideClassification::ride
                    && !ride->getRideTypeDescriptor().availableBreakdowns.isEmpty();
            return true;
        }

        Guest* SpawnOperative(const ActiveEffect& effect, const std::string& sourceName, std::string& error)
        {
            auto* guest = Park::GenerateGuest();
            if (guest == nullptr)
            {
                error = "The victim park has no valid guest spawn for the operative.";
                return nullptr;
            }
            guest->setName(std::string(AbilityName(effect.ability)) + " sent by " + sourceName);
            if (effect.targetRideId >= 0)
            {
                guest->guestHeadingToRideId = RideId::FromUnderlying(
                    static_cast<RideId::UnderlyingType>(effect.targetRideId));
                guest->guestIsLostCountdown = 200;
                guest->resetPathfindGoal();
                guest->cashInPocket = std::max(guest->cashInPocket, 1000.00_GBP);
                guest->intensity = IntensityRange(0, 15);
                guest->nauseaTolerance = PeepNauseaTolerance::high;
            }
            if (effect.ability == Ability::toiletBomber)
            {
                // Enough toilet need to walk in and use it (>= 70), but below the thresholds for the
                // "need toilet" thought (160) and the tell-tale desperate animation (220).
                guest->toilet = 120;
            }
            localOperatives.push_back({ effect.id, effect.ability, guest->id, effect.targetRideId });
            return guest;
        }

        bool ApplyLocalEffect(const ActiveEffect& effect, std::string& error)
        {
            if (effect.targetId != localParticipantId || localRole == Role::spectator || gLegacyScene != LegacyScene::playing)
            {
                error = "This process is not the effect's active park.";
                return false;
            }
            if (localEffects.contains(effect.id))
                return true;

            const auto* source = FindParticipant(state, effect.sourceId);
            const auto sourceName = source == nullptr ? std::string("A rival park") : source->name;
            switch (effect.ability)
            {
                case Ability::vandal:
                {
                    auto* guest = Park::GenerateGuest();
                    if (guest == nullptr)
                    {
                        error = "The victim park has no valid guest spawn for the vandal.";
                        return false;
                    }
                    guest->setName("Vandal sent by " + sourceName);
                    guest->happiness = 0;
                    guest->happinessTarget = 0;
                    guest->peepFlags.set(PeepFlag::angry);
                    guest->angriness = 255;
                    localVandals.push_back({ effect.id, guest->id, effect.potency, 0 });
                    // No arrival notice: the victim is only alerted on the first successful break.
                    break;
                }
                case Ability::misinformation:
                    AddLocalNotice(
                        sourceName + " started a misinformation campaign against your park for "
                        + std::to_string(effect.endsAtDay - effect.startsAtDay)
                        + " local days. It cancels future ordinary arrivals at the configured campaign strength.");
                    break;
                case Ability::poison:
                {
                    if (!IsOpenFoodDrinkStall(effect.targetRideId))
                    {
                        error = "The selected food or drink stall is no longer open and valid.";
                        return false;
                    }
                    // No arrival notice: the victim is only alerted when a buyer first falls ill.
                    break;
                }
                case Ability::toiletBomber:
                case Ability::saboteur:
                {
                    if (!IsValidOperativeTarget(effect.ability, effect.targetRideId))
                    {
                        error = effect.ability == Ability::toiletBomber
                            ? "The selected toilet is no longer open and valid."
                            : "The selected ride is no longer open or cannot break down.";
                        return false;
                    }
                    auto* guest = SpawnOperative(effect, sourceName, error);
                    if (guest == nullptr)
                        return false;
                    // No arrival notice: the victim is only alerted when the operative acts.
                    break;
                }
                case Ability::agitator:
                case Ability::hitman:
                {
                    auto* guest = SpawnOperative(effect, sourceName, error);
                    if (guest == nullptr)
                        return false;
                    // No arrival notice: the victim is only alerted once the operative causes harm.
                    break;
                }
                case Ability::researchSabotage:
                    AddLocalNotice(
                        sourceName + " sabotaged your research programme. Funding is forced to None for "
                        + std::to_string(effect.endsAtDay - effect.startsAtDay)
                        + " local days, then restored to your last setting.");
                    alertedEffects.insert(effect.id);
                    break;
                case Ability::unionDisruption:
                    AddLocalNotice(
                        sourceName + " incited a payroll dispute in your park. Staff wages are doubled for "
                        + std::to_string(effect.endsAtDay - effect.startsAtDay) + " local days.");
                    alertedEffects.insert(effect.id);
                    break;
                case Ability::karens:
                case Ability::stoners:
                {
                    const int32_t count = std::clamp<int32_t>(effect.potency == 0 ? 20 : effect.potency, 1, 40);
                    LocalGroup group{ effect.id, effect.ability, {}, 0 };
                    for (int32_t i = 0; i < count; i++)
                    {
                        auto* guest = Park::GenerateGuest();
                        if (guest == nullptr)
                            break;
                        // Name each party member in the singular ("Karen sent by ...", "Stoner sent by ...").
                        const char* memberName = effect.ability == Ability::karens ? "Karen" : "Stoner";
                        guest->setName(std::string(memberName) + " sent by " + sourceName);
                        if (effect.ability == Ability::karens)
                        {
                            // thirst is inverted (low = thirsty); toilet is not (high = needs to go).
                            guest->thirst = 10;
                            guest->toilet = 200;
                            guest->happiness = 60;
                            guest->happinessTarget = 60;
                            // Their own umbrella: guests never buy an item they already carry.
                            guest->giveItem(ShopItem::umbrella);
                        }
                        else
                        {
                            // The munchies: spawn hungry (hunger is inverted - low means hungry) so they
                            // head for food, and lethargic so they shuffle slowly and stand around.
                            guest->hunger = 10;
                            guest->energy = kPeepMinEnergy;
                            guest->energyTarget = kPeepMinEnergy;
                        }
                        group.guestIds.push_back(guest->id);
                    }
                    if (group.guestIds.empty())
                    {
                        error = "The victim park has no valid guest spawn for the group.";
                        return false;
                    }
                    localGroups.push_back(std::move(group));
                    AddLocalNotice(
                        sourceName + " sent a coach party of "
                        + std::to_string(localGroups.back().guestIds.size())
                        + (effect.ability == Ability::karens ? " demanding guests to your park."
                                                             : " easily-distracted guests to your park."));
                    alertedEffects.insert(effect.id);
                    break;
                }
            }
            localEffects.emplace(effect.id, effect);
            if (effect.ability == Ability::researchSabotage)
            {
                auto& gameState = getGameState();
                // Stash the victim's current funding level in the stored effect copy (potency is
                // otherwise unused for this ability) so it survives a mid-effect reconnect.
                localEffects.at(effect.id).potency = gameState.researchFundingLevel;
                gameState.researchFundingLevel = RESEARCH_FUNDING_NONE;
            }
            RebuildActorKinds();
            return true;
        }

        void EndLocalVandal(uint32_t effectId)
        {
            const auto iterator = std::find_if(localVandals.begin(), localVandals.end(), [&](const auto& vandal) {
                return vandal.effectId == effectId;
            });
            if (iterator == localVandals.end())
                return;
            if (auto* guest = getGameState().entities.getEntity<Guest>(iterator->guestId))
            {
                guest->peepFlags.unset(PeepFlag::angry);
                guest->peepFlags.set(PeepFlag::leavingPark);
                guest->angriness = 0;
            }
            localVandals.erase(iterator);
        }

        void EndLocalOperative(uint32_t effectId)
        {
            const auto iterator = std::find_if(localOperatives.begin(), localOperatives.end(), [&](const auto& operative) {
                return operative.effectId == effectId;
            });
            if (iterator == localOperatives.end())
                return;
            if (auto* guest = getGameState().entities.getEntity<Guest>(iterator->guestId))
            {
                guest->guestHeadingToRideId = RideId::GetNull();
                guest->peepFlags.set(PeepFlag::leavingPark);
                guest->peepFlags.unset(PeepFlag::parkEntranceChosen);
                guest->resetPathfindGoal();
            }
            localOperatives.erase(iterator);
        }

        // Hold a peep still with a static sprite (no walking-in-place shuffle) facing `faceToward`.
        static void FreezeFacing(Peep& peep, const CoordsXY& faceToward)
        {
            peep.orientation = DirectionFromTo({ peep.x, peep.y }, faceToward) << 3;
            peep.animationImageIdOffset = 0;
            peep.peepFlags.set(PeepFlag::positionFrozen);
            peep.peepFlags.set(PeepFlag::animationFrozen);
        }

        static void Unfreeze(Peep& peep)
        {
            peep.peepFlags.unset(PeepFlag::positionFrozen);
            peep.peepFlags.unset(PeepFlag::animationFrozen);
        }

        void EndDetention(uint32_t staffId, const StaffDetention& detention)
        {
            if (auto* staff = getGameState().entities.getEntity<Staff>(
                    EntityId::FromUnderlying(static_cast<EntityId::UnderlyingType>(staffId))))
                Unfreeze(*staff);
            if (!detention.karenId.IsNull())
                if (auto* karen = getGameState().entities.getEntity<Guest>(detention.karenId))
                    Unfreeze(*karen);
        }

        void ReleaseAllDetainedStaff()
        {
            for (const auto& [staffId, detention] : detainedStaff)
                EndDetention(staffId, detention);
            detainedStaff.clear();
            karenComplaintCooldown.clear();
            staffComplaintCooldown.clear();
        }

        // True while this Karen is cornering a staff member (both are frozen face to face).
        bool IsKarenConfrontingStaff(EntityId guestId) const
        {
            for (const auto& [staffId, detention] : detainedStaff)
                if (detention.karenId == guestId)
                    return true;
            return false;
        }

        void EndLocalGroup(uint32_t effectId)
        {
            const auto iterator = std::find_if(localGroups.begin(), localGroups.end(), [&](const auto& group) {
                return group.effectId == effectId;
            });
            if (iterator == localGroups.end())
                return;
            // Any group members still in the park head for the exit; the rest have already left.
            for (const auto guestId : iterator->guestIds)
            {
                if (auto* guest = getGameState().entities.getEntity<Guest>(guestId))
                    guest->peepFlags.set(PeepFlag::leavingPark);
            }
            localGroups.erase(iterator);
            const bool anyKarens = std::any_of(localGroups.begin(), localGroups.end(), [](const auto& g) {
                return g.ability == Ability::karens;
            });
            if (!anyKarens)
                ReleaseAllDetainedStaff();
            const bool anyStoners = std::any_of(localGroups.begin(), localGroups.end(), [](const auto& g) {
                return g.ability == Ability::stoners;
            });
            if (!anyStoners)
            {
                stonerNextPuffTick.clear();
                stonerLastStareTick.clear();
            }
        }

        // A Stoner may enter the "stop and stare" state only once per guard window, so they wander
        // between stares instead of re-locking onto the same spot. Called from the core guest AI.
        bool StonerMayStare(EntityId guestId) const
        {
            const auto it = stonerLastStareTick.find(guestId.ToUnderlying());
            return it == stonerLastStareTick.end() || getGameState().currentTicks - it->second >= kStonerStareGuardTicks;
        }

        void NoteStonerStareStarted(EntityId guestId)
        {
            stonerLastStareTick[guestId.ToUnderlying()] = getGameState().currentTicks;
        }

        // Each live Stoner puffs a small cloud of smoke every ~2-5 minutes; any ordinary guest sharing
        // the tile at that moment gets a minor mood hit and a "smells of weed" thought.
        void UpdateLocalStoners(const LocalGroup& group, uint32_t currentTicks)
        {
            for (const auto guestId : group.guestIds)
            {
                auto* stoner = getGameState().entities.getEntity<Guest>(guestId);
                if (stoner == nullptr || stoner->x == kLocationNull || stoner->outsideOfPark)
                    continue;

                const auto key = stoner->id.ToUnderlying();
                auto& nextPuff = stonerNextPuffTick[key];
                if (nextPuff == 0)
                {
                    // First puff lands 30s-2.5min after arrival so the effect is visible early on.
                    nextPuff = currentTicks + 1200 + (ScenarioRand() % kStonerPuffJitterTicks);
                    continue;
                }
                if (currentTicks < nextPuff)
                    continue;
                nextPuff = currentTicks + kStonerPuffMinTicks + (ScenarioRand() % kStonerPuffJitterTicks);

                // A small, short-lived wisp: start the steam animation partway through.
                SteamParticle::create({ stoner->x, stoner->y, stoner->z + 4 }, 256 * 6);

                bool hitSomeone = false;
                for (auto* bystander : EntityTileList<Guest>({ stoner->x, stoner->y }))
                {
                    if (bystander->id == stoner->id || bystander->outsideOfPark || bystander->x == kLocationNull
                        || IsCompetitiveAgent(bystander->id))
                        continue;
                    bystander->insertNewThought(PeepThoughtType::weedSmell);
                    bystander->happinessTarget = static_cast<uint8_t>(
                        std::max(0, bystander->happinessTarget - kStonerPuffHappinessHit));
                    bystander->windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;
                    hitSomeone = true;
                }
                if (hitSomeone)
                    AlertVictimOnce(group.effectId, "A group of guests is fouling the air in your park with drug smoke.");
            }
        }

        void UpdateLocalGroups()
        {
            if (localGroups.empty())
                return;
            const auto currentTicks = getGameState().currentTicks;

            // Drop expired cooldown entries (also clears them for guests/staff that have since left).
            std::erase_if(karenComplaintCooldown, [&](const auto& kv) { return currentTicks >= kv.second; });
            std::erase_if(staffComplaintCooldown, [&](const auto& kv) { return currentTicks >= kv.second; });

            // Release staff whose "speak to a manager" hold has expired; keep the rest - and the Karen
            // cornering them - frozen in place, face to face.
            for (auto it = detainedStaff.begin(); it != detainedStaff.end();)
            {
                auto* staff = getGameState().entities.getEntity<Staff>(EntityId::FromUnderlying(
                    static_cast<EntityId::UnderlyingType>(it->first)));
                auto* karen = getGameState().entities.getEntity<Guest>(it->second.karenId);
                if (currentTicks >= it->second.releaseTick || staff == nullptr || karen == nullptr)
                {
                    EndDetention(it->first, it->second);
                    // Give both sides a breather so they walk apart instead of re-locking instantly.
                    staffComplaintCooldown[it->first] = currentTicks + kStaffComplaintCooldownTicks;
                    if (!it->second.karenId.IsNull())
                        karenComplaintCooldown[it->second.karenId.ToUnderlying()]
                            = currentTicks + kKarenComplaintCooldownTicks;
                    it = detainedStaff.erase(it);
                }
                else
                {
                    // Re-assert the hold each tick in case other logic cleared a flag.
                    FreezeFacing(*staff, { karen->x, karen->y });
                    FreezeFacing(*karen, { staff->x, staff->y });
                    ++it;
                }
            }

            std::vector<uint32_t> emptied;
            for (auto& group : localGroups)
            {
                // Drop members who have left the park (their entity is gone).
                std::erase_if(group.guestIds, [&](EntityId id) {
                    return getGameState().entities.getEntity<Guest>(id) == nullptr;
                });
                if (group.guestIds.empty())
                {
                    emptied.push_back(group.effectId);
                    continue;
                }

                if (group.ability == Ability::stoners)
                {
                    if (currentTicks < group.nextScanTick)
                        continue;
                    group.nextScanTick = currentTicks + 8;
                    UpdateLocalStoners(group, currentTicks);
                    continue;
                }

                if (group.ability == Ability::karens)
                {
                    if (currentTicks < group.nextScanTick)
                        continue;
                    group.nextScanTick = currentTicks + 8;
                    for (const auto guestId : group.guestIds)
                    {
                        auto* karen = getGameState().entities.getEntity<Guest>(guestId);
                        if (karen == nullptr || karen->x == kLocationNull || karen->outsideOfPark)
                            continue;
                        // Keep them grumpy so they complain (including in the rain) and leave on their own.
                        karen->happinessTarget = std::min<uint8_t>(karen->happinessTarget, 70);
                        // A Karen already cornering someone stays put until that hold expires; one that
                        // just finished a demand waits out its cooldown before starting another.
                        if (IsKarenConfrontingStaff(karen->id) || karenComplaintCooldown.count(karen->id.ToUnderlying()))
                            continue;
                        for (auto* staff : EntityTileList<Staff>({ karen->x, karen->y }))
                        {
                            const auto key = staff->id.ToUnderlying();
                            if (detainedStaff.count(key) != 0 || staffComplaintCooldown.count(key) != 0)
                                continue;
                            detainedStaff[key] = { currentTicks + kStaffDetentionTicks, karen->id };
                            // Both stop dead where they are (no walking-in-place shuffle) and turn to
                            // face each other for the duration of the hold.
                            FreezeFacing(*staff, { karen->x, karen->y });
                            FreezeFacing(*karen, { staff->x, staff->y });
                            karen->insertNewThought(PeepThoughtType::speakToManager);
                            karen->happinessTarget = static_cast<uint8_t>(std::max(0, karen->happinessTarget - 10));
                            AlertVictimOnce(
                                group.effectId,
                                "A group of guests is hounding your staff, demanding to speak to a manager.");
                            break; // this Karen has its target
                        }
                    }
                }
            }
            for (const auto effectId : emptied)
                CompleteLocalEffect(effectId, "the visiting group has left your park");
        }

        void CompleteLocalEffect(uint32_t effectId, const std::string& reason)
        {
            const auto iterator = localEffects.find(effectId);
            if (iterator == localEffects.end())
                return;
            const auto ability = iterator->second.ability;
            if (ability == Ability::vandal)
                EndLocalVandal(effectId);
            else if (ability == Ability::toiletBomber || ability == Ability::agitator
                || ability == Ability::saboteur || ability == Ability::hitman)
                EndLocalOperative(effectId);
            else if (ability == Ability::researchSabotage)
            {
                // Restore whatever funding level the victim last chose (captured in potency).
                auto& gameState = getGameState();
                gameState.researchFundingLevel = std::min<uint8_t>(
                    iterator->second.potency, RESEARCH_FUNDING_MAXIMUM);
            }
            else if (ability == Ability::karens || ability == Ability::stoners)
                EndLocalGroup(effectId);
            // Only report the wind-down for effects the victim has already been alerted to (or the
            // passive misinformation campaign); silent infiltrators that never acted stay silent.
            if (ability == Ability::misinformation || alertedEffects.contains(effectId))
                AddLocalNotice(std::string(AbilityName(ability)) + " effect ended: " + reason);
            alertedEffects.erase(effectId);
            localEffects.erase(iterator);
            SendLocalEffectReply(effectId, true, reason, true);
        }

        // Rebuild the flat guest-id -> ability index from the authoritative actor lists.
        void RebuildActorKinds()
        {
            actorKinds.clear();
            for (const auto& vandal : localVandals)
                actorKinds[vandal.guestId.ToUnderlying()] = Ability::vandal;
            for (const auto& operative : localOperatives)
                actorKinds[operative.guestId.ToUnderlying()] = operative.ability;
            for (const auto& group : localGroups)
                for (const auto guestId : group.guestIds)
                    actorKinds[guestId.ToUnderlying()] = group.ability;
            gLocalActorsActive = !actorKinds.empty();
        }

        bool IsCompetitiveAgent(EntityId guestId) const
        {
            return actorKinds.find(guestId.ToUnderlying()) != actorKinds.end();
        }

        // Returns which disruptive-group ability a guest belongs to, if any.
        std::optional<Ability> GroupGuestKind(EntityId guestId) const
        {
            const auto it = actorKinds.find(guestId.ToUnderlying());
            if (it != actorKinds.end() && (it->second == Ability::karens || it->second == Ability::stoners))
                return it->second;
            return std::nullopt;
        }

        bool IsStaffDetained(EntityId staffId) const
        {
            const auto it = detainedStaff.find(staffId.ToUnderlying());
            return it != detainedStaff.end() && getGameState().currentTicks < it->second.releaseTick;
        }

        void ApplySingleAccidentConsequence()
        {
            auto& park = getGameState().park;
            if (park.ratingCasualtyPenalty < 500)
                park.ratingCasualtyPenalty += 200;
        }

        // True when a guest is currently occupying (approaching / using / leaving) the given facility or shop.
        // Toilets and other facilities never enter PeepState::buying - guests sit in enteringRide with a
        // shop ride-substate - so the plain buying check misses them entirely.
        static bool IsOccupyingRide(const Guest& guest, RideId rideId)
        {
            if (guest.currentRide != rideId || guest.outsideOfPark)
                return false;
            if (guest.state == PeepState::buying)
                return true;
            return guest.state == PeepState::enteringRide
                && (guest.rideSubState == PeepRideSubState::approachShop
                    || guest.rideSubState == PeepRideSubState::interactShop
                    || guest.rideSubState == PeepRideSubState::leaveShop);
        }

        void TriggerToiletBomber(LocalOperative& operative, Guest& bomber, std::vector<uint32_t>& completed)
        {
            const auto rideId = RideId::FromUnderlying(static_cast<RideId::UnderlyingType>(operative.targetRideId));
            auto* ride = GetRide(rideId);
            if (ride == nullptr)
            {
                completed.push_back(operative.effectId);
                return;
            }
            const auto rideName = ride->getName();

            std::vector<EntityId> victims;
            for (auto* guest : EntityList<Guest>())
            {
                if (guest->id != bomber.id && IsOccupyingRide(*guest, rideId))
                {
                    victims.push_back(guest->id);
                }
            }
            for (const auto victimId : victims)
            {
                if (auto* victim = getGameState().entities.getEntity<Guest>(victimId))
                {
                    const auto victimName = victim->getName();
                    const CoordsXY victimLocation{ victim->x, victim->y };
                    ExplosionCloud::create({ victim->x, victim->y, static_cast<int16_t>(victim->z + 16) });
                    ExplosionFlare::create({ victim->x, victim->y, static_cast<int16_t>(victim->z + 16) });
                    victim->remove();
                    AddLocalDeathNotice(victimName + " was killed by an explosion at " + rideName + "!", victimLocation);
                }
            }

            // The bomber is a suicide bomber: it detonates inside the toilet and dies with any occupants.
            // Its own death always counts, so the accident consequence applies even for an empty toilet.
            const auto bomberName = bomber.getName();
            const CoordsXY bomberLocation{ bomber.x, bomber.y };
            const auto explosionLocation = bomber.getLocation();
            ExplosionCloud::create(explosionLocation + CoordsXYZ{ 0, 0, 16 });
            ExplosionFlare::create(explosionLocation + CoordsXYZ{ 0, 0, 16 });
            bomber.remove();
            AddLocalDeathNotice(bomberName + " blew themselves up at " + rideName + "!", bomberLocation);
            ApplySingleAccidentConsequence();

            GameActions::RideDemolishAction action(rideId, GameActions::RideModifyType::demolish);
            // ExecuteNested performs the demolition but skips the automatic demolition refund and its
            // floating money effect, so the victim isn't paid for a ride a rival blew up.
            const auto result = GameActions::ExecuteNested(&action, getGameState());
            AddLocalNotice(
                result.error == GameActions::Status::ok
                    ? "The toilet bomber destroyed " + rideName + " and caught "
                        + std::to_string(victims.size()) + (victims.size() == 1 ? " guest inside." : " guests inside.")
                    : "The toilet bomber reached the target, but OpenRCT2 could not demolish it.");
            alertedEffects.insert(operative.effectId);
            completed.push_back(operative.effectId);
        }

        void UpdateLocalOperatives()
        {
            if (localOperatives.empty())
                return;
            const auto currentTicks = getGameState().currentTicks;
            std::vector<uint32_t> completed;
            for (auto& operative : localOperatives)
            {
                auto* agent = getGameState().entities.getEntity<Guest>(operative.guestId);
                if (agent == nullptr)
                {
                    completed.push_back(operative.effectId);
                    continue;
                }

                if (operative.targetRideId >= 0 && agent->state == PeepState::walking
                    && agent->guestHeadingToRideId.IsNull())
                {
                    agent->guestHeadingToRideId = RideId::FromUnderlying(
                        static_cast<RideId::UnderlyingType>(operative.targetRideId));
                    agent->guestIsLostCountdown = 200;
                    agent->resetPathfindGoal();
                }

                if (operative.ability == Ability::toiletBomber)
                {
                    const auto targetRide = RideId::FromUnderlying(
                        static_cast<RideId::UnderlyingType>(operative.targetRideId));
                    if (IsOccupyingRide(*agent, targetRide))
                        TriggerToiletBomber(operative, *agent, completed);
                    continue;
                }
                if (operative.ability == Ability::saboteur)
                    continue;
                if ((currentTicks & 0x0F) != 0 || agent->x == kLocationNull)
                    continue;

                if (operative.ability == Ability::agitator)
                {
                    const auto effect = localEffects.find(operative.effectId);
                    if (effect == localEffects.end())
                        continue;
                    for (auto* guest : EntityTileList<Guest>({ agent->x, agent->y }))
                    {
                        if (guest->id == agent->id || guest->outsideOfPark || guest->x == kLocationNull
                            || IsCompetitiveAgent(guest->id)
                            || std::find(operative.affectedGuests.begin(), operative.affectedGuests.end(), guest->id)
                                != operative.affectedGuests.end())
                            continue;
                        if (std::abs(guest->x - agent->x) > 48 || std::abs(guest->y - agent->y) > 48
                            || std::abs(guest->z - agent->z) > 16)
                            continue;
                        guest->insertNewThought(PeepThoughtType::rudeGuest);
                        guest->happinessTarget = static_cast<uint8_t>(
                            std::max<int32_t>(0, guest->happinessTarget - effect->second.potency));
                        guest->windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;
                        operative.affectedGuests.push_back(guest->id);
                    }
                    if (!operative.affectedGuests.empty())
                        AlertVictimOnce(
                            operative.effectId,
                            "Some of your guests are turning rude and unhappy - a troublemaker is loose in the park.");
                    continue;
                }

                if (operative.ability == Ability::hitman)
                {
                    if (operative.stage == 0)
                    {
                        Guest* victim = nullptr;
                        int32_t bestDistance = std::numeric_limits<int32_t>::max();
                        for (auto* guest : EntityTileList<Guest>({ agent->x, agent->y }))
                        {
                            if (guest->id == agent->id || guest->outsideOfPark || guest->x == kLocationNull
                                || IsCompetitiveAgent(guest->id)
                                || guest->peepFlags.has(PeepFlag::leavingPark)
                                || (guest->state != PeepState::walking && guest->state != PeepState::sitting))
                                continue;
                            const auto distance = std::abs(guest->x - agent->x) + std::abs(guest->y - agent->y);
                            if (distance > 96 || std::abs(guest->z - agent->z) > 16)
                                continue;
                            if (distance < bestDistance
                                || (distance == bestDistance
                                    && (victim == nullptr
                                        || guest->id.ToUnderlying() < victim->id.ToUnderlying())))
                            {
                                victim = guest;
                                bestDistance = distance;
                            }
                        }
                        if (victim != nullptr && agent->isActionInterruptableSafely())
                        {
                            operative.victimId = victim->id;
                            operative.stage = 1;
                            operative.triggerAtTick = currentTicks + 24;
                            agent->action = PeepActionType::takePhoto;
                            agent->animationFrameNum = 0;
                            agent->animationImageIdOffset = 0;
                            agent->updateCurrentAnimationType();
                        }
                    }
                    else if (currentTicks >= operative.triggerAtTick)
                    {
                        auto* victim = getGameState().entities.getEntity<Guest>(operative.victimId);
                        if (victim == nullptr)
                        {
                            operative.victimId = EntityId::GetNull();
                            operative.stage = 0;
                            continue;
                        }
                        if (victim->state != PeepState::walking && victim->state != PeepState::sitting)
                        {
                            // Victim stepped onto a ride/queue right after the photo; keep watching them
                            // for a short while before giving up and re-acquiring a target.
                            if (currentTicks < operative.triggerAtTick + 400)
                                continue;
                            operative.victimId = EntityId::GetNull();
                            operative.stage = 0;
                            continue;
                        }
                        const auto victimName = victim->getName();
                        const CoordsXY victimLocation{ victim->x, victim->y };
                        Audio::Play3D(Audio::SoundId::crash, victim->getLocation());
                        ExplosionCloud::create({ victim->x, victim->y, static_cast<int16_t>(victim->z + 16) });
                        ExplosionFlare::create({ victim->x, victim->y, static_cast<int16_t>(victim->z + 16) });
                        victim->remove();
                        ApplySingleAccidentConsequence();
                        AddLocalDeathNotice(
                            victimName + " was found dead in your park after a rival sent a hitman.", victimLocation);
                        alertedEffects.insert(operative.effectId);
                        completed.push_back(operative.effectId);
                    }
                }
            }
            for (const auto effectId : completed)
            {
                if (localEffects.contains(effectId))
                    CompleteLocalEffect(effectId, "the operative completed the assignment and was sent home");
            }
        }

        // Vandals attack in bursts: after each attempt they calm down and wander for a randomised
        // spell so they roam the park instead of smashing everything by the entrance.
        void UpdateLocalVandals()
        {
            if (localVandals.empty())
                return;
            const auto currentTicks = getGameState().currentTicks;
            for (auto& vandal : localVandals)
            {
                if (vandal.nextAttemptTick == 0 || currentTicks < vandal.nextAttemptTick)
                    continue;
                auto* guest = getGameState().entities.getEntity<Guest>(vandal.guestId);
                if (guest == nullptr)
                    continue;
                vandal.nextAttemptTick = 0;
                guest->peepFlags.set(PeepFlag::angry);
                guest->angriness = 255;
                guest->happiness = 0;
                guest->happinessTarget = 0;
            }
        }

        void UpdateLocalEffects()
        {
            if (localEffects.empty() || gLegacyScene != LegacyScene::playing)
            {
                if (!actorKinds.empty())
                    RebuildActorKinds();
                return;
            }
            UpdateLocalOperatives();
            UpdateLocalVandals();
            UpdateLocalGroups();
            // Refresh the flat actor index (and the core-simulation hook gate) after this tick's changes.
            RebuildActorKinds();
            for (auto& [effectId, effect] : localEffects)
            {
                if (effect.ability != Ability::researchSabotage)
                    continue;
                auto& gameState = getGameState();
                // If the victim raised funding during the effect, remember their new choice and force it back down.
                if (gameState.researchFundingLevel != RESEARCH_FUNDING_NONE)
                {
                    effect.potency = gameState.researchFundingLevel;
                    gameState.researchFundingLevel = RESEARCH_FUNDING_NONE;
                }
            }
            const auto localDay = GetCurrentLocalDay();
            std::vector<uint32_t> expired;
            for (const auto& [effectId, effect] : localEffects)
            {
                if (localDay >= effect.endsAtDay)
                    expired.push_back(effectId);
            }
            for (const auto effectId : expired)
                CompleteLocalEffect(effectId, "its configured victim-local duration elapsed");
        }

        void SendSnapshot()
        {
            state.revision++;
            const json_t body = ToJson(state);
            for (auto& peer : peers)
            {
                if (peer.participantId != kInvalidParticipantId && peer.connection->isValid())
                {
                    QueueJson(*peer.connection, Network::Command::competitiveSnapshot, body);
                }
            }
        }

        json_t GetServerInfo() const
        {
            const auto metric = MetricName(state.rules.metric);
            const auto victory = state.rules.victoryMode == VictoryMode::target
                ? "First to " + std::to_string(state.rules.target) + " " + metric
                : "Top " + metric + " at end of Year " + std::to_string(state.rules.deadlineYear);
            return {
                { "name", state.name },
                { "requiresPassword", false },
                { "version", Network::GetVersion() },
                { "players", CountCompetingParks(state) },
                { "maxPlayers", state.rules.maxPlayers },
                { "description", victory + " — " + state.scenario.name },
                { "greeting", "Competitive parks use independent simulations." },
                { "dedicated", false },
                { "gameMode", "competitive" },
                { "competitiveProtocol", kProtocolVersion },
                { "phase", PhaseName(state.phase) },
                { "scenarioName", state.scenario.name },
                { "scenarioFileName", state.scenario.fileName },
                { "scenarioHash", state.scenario.contentHash },
                { "victory", victory },
                { "allowLateJoin", state.rules.allowLateJoin },
            };
        }

        void SendGameInfo(Network::Connection& connection)
        {
            Network::Packet packet(Network::Command::gameInfo);
            packet.writeString(GetServerInfo().dump());
            packet << false << false;
            connection.queuePacket(packet);
        }

        void SendError(Network::Connection& connection, const std::string& message, bool fatal = true)
        {
            QueueJson(
                connection, Network::Command::competitiveError,
                { { "message", message }, { "fatal", fatal } });
        }

        void AcceptPeers()
        {
            if (listener == nullptr || listener->GetStatus() != Network::SocketStatus::listening)
            {
                return;
            }
            while (auto socket = listener->Accept())
            {
                auto connection = std::make_unique<Network::Connection>();
                connection->socket = std::move(socket);
                connection->authStatus = Network::Auth::ok;
                peers.push_back({ std::move(connection), kInvalidParticipantId, Platform::GetTicks() });
            }
        }

        void HandleHello(Peer& peer, const json_t& body)
        {
            if (peer.participantId != kInvalidParticipantId)
            {
                SendError(*peer.connection, "This connection has already joined the competition.");
                return;
            }
            if (body.value("protocol", 0) != kProtocolVersion)
            {
                SendError(*peer.connection, "Competitive protocol version does not match this server.");
                return;
            }

            const auto requestedRole = body.value("role", Role::none);
            const auto identityKey = body.value("identityKey", std::string{});
            auto playerName = String::trim(body.value("name", std::string{}));
            if ((requestedRole != Role::player && requestedRole != Role::spectator) || identityKey.empty()
                || playerName.empty())
            {
                SendError(*peer.connection, "Invalid competitive join request.");
                return;
            }
            if (playerName.size() > 32)
            {
                playerName.resize(32);
            }

            auto existing = std::find_if(state.participants.begin(), state.participants.end(), [&](const auto& value) {
                return value.identityKey == identityKey && value.role == requestedRole;
            });
            if (existing != state.participants.end())
            {
                if (existing->forfeited)
                {
                    SendError(*peer.connection, "This park was forfeited and cannot rejoin.");
                    return;
                }
                existing->online = true;
                existing->name = playerName;
                existing->watchHost.clear();
                existing->watchPort = 0;
                peer.participantId = existing->id;
            }
            else
            {
                if (requestedRole == Role::player)
                {
                    if (state.phase != Phase::lobby && !(state.phase == Phase::running && state.rules.allowLateJoin))
                    {
                        SendError(*peer.connection, "This competition is not accepting new parks.");
                        return;
                    }
                    if (CountCompetingParks(state) >= state.rules.maxPlayers)
                    {
                        SendError(*peer.connection, "This competition is full.");
                        return;
                    }
                }

                Participant participant{
                    MakeParticipantId(),
                    identityKey,
                    playerName,
                    requestedRole,
                    true,
                    false,
                    false,
                    false,
                    static_cast<uint32_t>(state.participants.size()),
                    1,
                    state.scenario,
                };
                peer.participantId = participant.id;
                state.participants.push_back(participant);
                if (requestedRole == Role::player)
                {
                    state.scores.push_back({ .participantId = participant.id });
                    state.reports.push_back({ .participantId = participant.id });
                }
            }

            peer.lastSeenAt = Platform::GetTicks();
            QueueJson(
                *peer.connection, Network::Command::competitiveWelcome,
                { { "participantId", peer.participantId }, { "state", ToJson(state) } });
            if (body.contains("pendingPayments") && body["pendingPayments"].is_array())
            {
                for (const auto& value : body["pendingPayments"])
                {
                    if (!value.is_number_unsigned())
                        continue;
                    const auto requestId = value.get<uint32_t>();
                    const auto effect = std::find_if(state.effects.begin(), state.effects.end(), [&](const auto& item) {
                        return item.sourceId == peer.participantId && item.sourceRequestId == requestId;
                    });
                    if (effect != state.effects.end() && !effect->delivered)
                        continue;
                    const bool accepted = effect != state.effects.end();
                    QueueJson(
                        *peer.connection, Network::Command::competitiveEffectAck,
                        {
                            { "paymentRequestId", requestId },
                            { "paymentAccepted", accepted },
                            { "notification",
                              accepted ? std::string("The previously submitted rival action was delivered.")
                                       : std::string("The previously submitted rival action was not accepted. Its park-cash cost was refunded.") },
                        });
                }
            }
            SendSnapshot();
        }

        void IngestMetrics(ParticipantId participantId, const ParkMetrics& metrics)
        {
            if (state.phase != Phase::running)
            {
                return;
            }
            auto* participant = FindParticipant(state, participantId);
            auto* score = FindScore(state, participantId);
            auto* report = FindReport(state, participantId);
            if (participant == nullptr || score == nullptr || report == nullptr || participant->forfeited
                || participant->finished || metrics.localDay < report->metrics.localDay)
            {
                return;
            }

            report->metrics = metrics;
            participant->currentYear = metrics.localYear;
            if (metrics.localDay > report->lastScoredDay)
            {
                UpdateLiveScore(*score, metrics);
                report->lastScoredDay = metrics.localDay;
            }
            else if (!score->frozenAtYear.has_value())
            {
                score->rating = metrics.rating;
                score->guests = metrics.guests;
                score->parkValue = metrics.parkValue;
                score->cash = metrics.cash;
            }

            std::vector<uint32_t> expiredEffects;
            for (const auto& effect : state.effects)
            {
                if (effect.targetId == participantId && effect.delivered && metrics.localDay >= effect.endsAtDay)
                    expiredEffects.push_back(effect.id);
            }
            for (const auto effectId : expiredEffects)
                ResolveEffectReply(participantId, effectId, true, "The configured duration elapsed.", true);

            if (DeadlineReached(metrics.monthsElapsed, state.rules.deadlineYear))
            {
                FreezeScore(*score, metrics, metrics.localYear);
                participant->finished = true;
            }

            if (TargetReached(*score, state.rules))
            {
                state.winnerId = participantId;
                state.phase = Phase::finished;
                participant->finished = true;
            }
            else if (CompetitionComplete(state.participants))
            {
                state.winnerId = ChooseWinner(state.scores, state.participants, state.rules.metric);
                state.phase = Phase::finished;
            }
            SendSnapshot();
        }

        void HandleReady(Peer& peer, const json_t& body)
        {
            if (state.phase != Phase::lobby)
            {
                SendError(*peer.connection, "The lobby is no longer accepting ready changes.", false);
                return;
            }
            auto* participant = FindParticipant(state, peer.participantId);
            if (participant == nullptr || participant->role == Role::spectator)
            {
                return;
            }
            const bool ready = body.value("ready", false);
            auto scenario = ScenarioIdentityFromJson(body["scenario"]);
            auto metrics = ParkMetricsFromJson(body["metrics"]);
            if (!scenario.has_value() || !metrics.has_value() || !IsSameScenario(*scenario, state.scenario)
                || metrics->localDay != state.startLocalDay || metrics->localYear >= state.rules.deadlineYear)
            {
                participant->ready = false;
                SendError(*peer.connection, "The park does not match the lobby's scenario and starting date.", false);
                SendSnapshot();
                return;
            }
            participant->scenario = std::move(*scenario);
            participant->currentYear = metrics->localYear;
            participant->ready = ready;
            if (auto* report = FindReport(state, participant->id))
            {
                report->metrics = *metrics;
            }
            if (auto* score = FindScore(state, participant->id))
            {
                score->rating = metrics->rating;
                score->guests = metrics->guests;
                score->parkValue = metrics->parkValue;
                score->cash = metrics->cash;
            }
            SendSnapshot();
        }

        void ResolveEffectReply(
            ParticipantId targetId, uint32_t effectId, bool accepted, const std::string& message, bool complete)
        {
            const auto iterator = std::find_if(state.effects.begin(), state.effects.end(), [&](const auto& effect) {
                return effect.id == effectId && effect.targetId == targetId;
            });
            if (iterator == state.effects.end())
                return;

            if (complete)
            {
                if (!iterator->delivered)
                    return;
                const auto sourceId = iterator->sourceId;
                const auto ability = iterator->ability;
                state.effects.erase(iterator);
                NotifyParticipant(sourceId, std::string(AbilityName(ability)) + " effect has ended in the rival park.");
                SendSnapshot();
                return;
            }

            const auto sourceId = iterator->sourceId;
            const auto ability = iterator->ability;
            const auto requestId = iterator->sourceRequestId;
            if (!accepted)
            {
                state.effects.erase(iterator);
                ResolveAbilityPayment(
                    sourceId, requestId, false,
                    std::string(AbilityName(ability)) + " could not be delivered. " + message);
                SendSnapshot();
                return;
            }

            iterator->delivered = true;
            const auto* sourceReport = FindReport(state, sourceId);
            const auto currentDay = sourceReport == nullptr ? uint32_t{ 1 } : sourceReport->metrics.localDay;
            const auto availableAtDay = currentDay + GetAbilityRule(state.rules, ability).cooldownDays;
            auto cooldown = std::find_if(state.cooldowns.begin(), state.cooldowns.end(), [&](const auto& value) {
                return value.participantId == sourceId && value.ability == ability;
            });
            if (cooldown == state.cooldowns.end())
                state.cooldowns.push_back({ sourceId, ability, availableAtDay });
            else
                cooldown->availableAtDay = availableAtDay;
            RecordAbilityUse(
                state, sourceId, ability, sourceReport == nullptr ? uint16_t{ 1 } : sourceReport->metrics.localYear);
            ResolveAbilityPayment(
                sourceId, requestId, true, std::string(AbilityName(ability)) + " was delivered successfully.");
            SendSnapshot();
        }

        void RefundPendingEffectsForTarget(ParticipantId targetId)
        {
            std::vector<uint32_t> pending;
            for (const auto& effect : state.effects)
            {
                if (effect.targetId == targetId && !effect.delivered)
                    pending.push_back(effect.id);
            }
            for (const auto effectId : pending)
                ResolveEffectReply(targetId, effectId, false, "The rival went offline before acknowledging it.", false);
        }

        bool BeginAbility(
            ParticipantId sourceId, Ability ability, ParticipantId targetId, int32_t targetRideId,
            uint32_t sourceRequestId, money64 chargedCost, std::string& error)
        {
            if (state.phase != Phase::running)
            {
                error = "Rival actions are available only while the competition is running.";
                return false;
            }
            auto* source = FindParticipant(state, sourceId);
            auto* target = FindParticipant(state, targetId);
            auto* sourceScore = FindScore(state, sourceId);
            const auto* sourceReport = FindReport(state, sourceId);
            const auto* targetReport = FindReport(state, targetId);
            if (source == nullptr || sourceScore == nullptr || sourceReport == nullptr || source->role == Role::spectator
                || source->finished || source->forfeited)
            {
                error = "The attacking park is not eligible to use rival actions.";
                return false;
            }
            if (target == nullptr || targetId == sourceId || !CanTarget(*target) || targetReport == nullptr)
            {
                error = "Choose an online, unfinished rival park.";
                return false;
            }
            const auto& rule = GetAbilityRule(state.rules, ability);
            if (!rule.enabled)
            {
                error = std::string(AbilityName(ability)) + " is disabled by the host's match rules.";
                return false;
            }
            const auto cooldown = std::find_if(state.cooldowns.begin(), state.cooldowns.end(), [&](const auto& value) {
                return value.participantId == sourceId && value.ability == ability;
            });
            if (cooldown != state.cooldowns.end() && sourceReport->metrics.localDay < cooldown->availableAtDay)
            {
                error = std::string(AbilityName(ability)) + " is on cooldown until your local day "
                    + std::to_string(cooldown->availableAtDay) + ".";
                return false;
            }
            if (rule.usesPerYear != 0
                && AbilityUsesThisYear(state, sourceId, ability, sourceReport->metrics.localYear) >= rule.usesPerYear)
            {
                error = std::string(AbilityName(ability)) + " has no uses left for your local year "
                    + std::to_string(sourceReport->metrics.localYear) + " (limit " + std::to_string(rule.usesPerYear)
                    + " per year).";
                return false;
            }
            const auto expectedCost = state.scenario.noMoney ? 0.00_GBP : rule.cost;
            if (sourceRequestId == 0 || chargedCost != expectedCost)
            {
                error = "The action's park-cash payment did not match the host's rules.";
                return false;
            }
            const std::vector<ParkMetrics::TargetRide>* validTargets = nullptr;
            if (ability == Ability::poison)
                validTargets = &targetReport->metrics.openFoodDrinkStalls;
            else if (ability == Ability::toiletBomber)
                validTargets = &targetReport->metrics.openToilets;
            else if (ability == Ability::saboteur)
                validTargets = &targetReport->metrics.openRides;
            if (validTargets != nullptr)
            {
                const auto stall = std::find_if(
                    validTargets->begin(), validTargets->end(),
                    [&](const auto& value) { return value.rideId == targetRideId; });
                if (stall == validTargets->end())
                {
                    error = ability == Ability::poison
                        ? "Choose an open food or drink stall reported by that rival park."
                        : ability == Ability::toiletBomber
                        ? "Choose an open toilet reported by that rival park."
                        : "Choose an open, breakdown-capable ride reported by that rival park.";
                    return false;
                }
            }
            const auto duplicate = std::find_if(state.effects.begin(), state.effects.end(), [&](const auto& effect) {
                return effect.targetId == targetId && EffectClashes(effect, ability, targetRideId);
            });
            if (duplicate != state.effects.end())
            {
                error = std::string(AbilityName(ability)) + " is already active against that target.";
                return false;
            }

            ActiveEffect effect{
                state.nextEffectId++,
                ability,
                sourceId,
                targetId,
                validTargets != nullptr ? targetRideId : -1,
                sourceRequestId,
                false,
                chargedCost,
                targetReport->metrics.localDay,
                targetReport->metrics.localDay + rule.durationDays,
                rule.potency,
            };
            state.effects.push_back(effect);
            SendSnapshot();

            if (targetId == localParticipantId)
            {
                std::string deliveryError;
                const bool delivered = ApplyLocalEffect(effect, deliveryError);
                ResolveEffectReply(targetId, effect.id, delivered, deliveryError, false);
                return delivered;
            }
            auto* targetPeer = FindPeer(targetId);
            if (targetPeer == nullptr)
            {
                ResolveEffectReply(targetId, effect.id, false, "The rival is no longer connected.", false);
                error = "The rival went offline before the action could be delivered.";
                return false;
            }
            QueueJson(
                *targetPeer->connection, Network::Command::competitiveAbility,
                { { "effect", ToJson(effect) } });
            return true;
        }

        void HandleEffectReply(Peer& peer, const json_t& body)
        {
            const auto effectId = body.value("effectId", 0u);
            if (effectId == 0)
                return;
            ResolveEffectReply(
                peer.participantId, effectId, body.value("accepted", false), body.value("message", std::string{}),
                body.value("complete", false));
        }

        void HandleHostPacket(Peer& peer, Network::Packet& packet)
        {
            peer.lastSeenAt = Platform::GetTicks();
            switch (packet.getCommand())
            {
                case Network::Command::gameInfo:
                    SendGameInfo(*peer.connection);
                    break;
                case Network::Command::competitiveHello:
                    if (auto body = ReadJson(packet))
                        HandleHello(peer, *body);
                    break;
                case Network::Command::competitiveReady:
                    if (peer.participantId != kInvalidParticipantId)
                        if (auto body = ReadJson(packet))
                            HandleReady(peer, *body);
                    break;
                case Network::Command::competitiveMetrics:
                    if (peer.participantId != kInvalidParticipantId)
                    {
                        if (auto body = ReadJson(packet))
                        {
                            if (auto metrics = ParkMetricsFromJson((*body)["metrics"]))
                                IngestMetrics(peer.participantId, *metrics);
                        }
                    }
                    break;
                case Network::Command::competitiveAbility:
                    if (peer.participantId != kInvalidParticipantId)
                    {
                        if (auto body = ReadJson(packet))
                        {
                            std::string error;
                            const auto ability = body->value("ability", Ability::vandal);
                            const auto targetId = body->value("targetId", kInvalidParticipantId);
                            const auto targetRideId = body->value("targetRideId", -1);
                            const auto sourceRequestId = body->value("sourceRequestId", 0u);
                            const auto chargedCost = body->value("chargedCost", 0.00_GBP);
                            if (ability > kLastAbility
                                || !BeginAbility(
                                    peer.participantId, ability, targetId, targetRideId, sourceRequestId, chargedCost,
                                    error))
                            {
                                ResolveAbilityPayment(
                                    peer.participantId, sourceRequestId, false,
                                    "Rival action failed: "
                                        + (error.empty() ? std::string("Invalid request.") : error));
                            }
                        }
                    }
                    break;
                case Network::Command::competitiveEffectAck:
                    if (peer.participantId != kInvalidParticipantId)
                    {
                        if (auto body = ReadJson(packet))
                            HandleEffectReply(peer, *body);
                    }
                    break;
                case Network::Command::competitiveHeartbeat:
                    QueueJson(*peer.connection, Network::Command::competitiveHeartbeat, { { "reply", true } });
                    break;
                case Network::Command::competitiveWatchEndpoint:
                    if (peer.participantId != kInvalidParticipantId)
                    {
                        if (auto body = ReadJson(packet))
                        {
                            auto* participant = FindParticipant(state, peer.participantId);
                            const auto port = body->value("port", uint16_t{});
                            if (participant != nullptr && participant->role != Role::spectator && port != 0)
                            {
                                participant->watchHost = peer.connection->socket->GetIpAddress();
                                participant->watchPort = port;
                                SendSnapshot();
                            }
                        }
                    }
                    break;
                case Network::Command::competitiveChat:
                    if (peer.participantId != kInvalidParticipantId)
                    {
                        if (auto body = ReadJson(packet))
                        {
                            auto text = body->value("text", std::string{});
                            if (text.size() > 240)
                                text.resize(240);
                            const auto* participant = FindParticipant(state, peer.participantId);
                            if (participant != nullptr && !text.empty())
                                HostBroadcastChat(
                                    participant->name.empty() ? std::string("competitor") : participant->name, text);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        void UpdateHost()
        {
            AcceptPeers();
            for (auto iterator = peers.begin(); iterator != peers.end();)
            {
                auto& peer = *iterator;
                peer.connection->update();
                while (peer.connection->isValid())
                {
                    const auto read = peer.connection->readPacket();
                    if (read != Network::ReadPacket::success)
                        break;
                    HandleHostPacket(peer, peer.connection->inboundPacket);
                    peer.connection->inboundPacket.clear();
                }
                peer.connection->sendQueuedData();
                if (!peer.connection->isValid())
                {
                    if (auto* participant = FindParticipant(state, peer.participantId))
                    {
                        RefundPendingEffectsForTarget(participant->id);
                        participant->online = false;
                        participant->ready = false;
                        SendSnapshot();
                    }
                    iterator = peers.erase(iterator);
                }
                else
                {
                    ++iterator;
                }
            }

            if (state.phase == Phase::running && gLegacyScene == LegacyScene::playing)
            {
                const auto localDay = GetCurrentLocalDay();
                if (localDay != lastReportedDay)
                {
                    const auto metrics = CollectParkMetrics();
                    lastReportedDay = metrics.localDay;
                    IngestMetrics(localParticipantId, metrics);
                }
            }
        }

        bool LoadRequiredScenario()
        {
            if (localRole == Role::spectator)
            {
                scenarioLoaded = true;
                return true;
            }
            if (loadingScenario)
                return false;
            loadingScenario = true;
            struct LoadingScope
            {
                bool& flag;
                ~LoadingScope()
                {
                    flag = false;
                }
            } loadingScope{ loadingScenario };
            const auto* repository = GetScenarioRepository();
            const auto* scenario = repository == nullptr ? nullptr : repository->GetByFilename(state.scenario.fileName);
            if (scenario == nullptr)
            {
                SetError("Required scenario is not installed: " + state.scenario.name);
                return false;
            }
            const auto localIdentity = GetScenarioIdentityForPath(scenario->Path);
            if (!IsSameScenario(localIdentity, state.scenario))
            {
                SetError("The installed scenario does not match the host's scenario content.");
                return false;
            }

            GameNotifyMapChange();
            if (!GetContext()->LoadParkFromFile(scenario->Path, false, true))
            {
                SetError("OpenRCT2 could not load the competition's required scenario.");
                return false;
            }
            if (!GameIsPaused())
            {
                PauseToggle();
            }
            forcedPause = true;
            wasPausedBeforeForcedPause = false;
            scenarioLoaded = true;
            lastReportedDay = CollectParkMetrics().localDay;
            return true;
        }

        void SendHello()
        {
            json_t pendingPayments = json_t::array();
            for (const auto& [requestId, payment] : pendingAbilityPayments)
                pendingPayments.push_back(requestId);
            QueueJson(
                *serverConnection, Network::Command::competitiveHello,
                {
                    { "protocol", kProtocolVersion },
                    { "role", joinConfiguration.role },
                    { "identityKey", Config::Get().network.competitiveIdentity },
                    { "name", joinConfiguration.playerName },
                    { "pendingPayments", std::move(pendingPayments) },
                });
            helloSent = true;
        }

        void HandleClientPacket(Network::Packet& packet)
        {
            switch (packet.getCommand())
            {
                case Network::Command::competitiveWelcome:
                {
                    const auto body = ReadJson(packet);
                    if (!body.has_value())
                    {
                        SetError("The host sent an invalid competition welcome.");
                        break;
                    }
                    auto parsed = MatchStateFromJson((*body)["state"]);
                    const auto participantId = body->value("participantId", kInvalidParticipantId);
                    if (!parsed.has_value() || FindParticipant(*parsed, participantId) == nullptr)
                    {
                        SetError("The host sent invalid competition state.");
                        break;
                    }
                    state = std::move(*parsed);
                    localParticipantId = participantId;
                    localRole = joinConfiguration.role;
                    const bool reconnectedAfterHostLoss = hostLossHandled;
                    status = ConnectionStatus::online;
                    if (!scenarioLoaded && !LoadRequiredScenario())
                    {
                        serverConnection->disconnect();
                    }
                    else
                    {
                        std::string watchWarning;
                        if (!EnsureLocalWatchServer(watchWarning))
                            AddLocalNotice("Park spectating is unavailable for this park: " + watchWarning);
                        if (reconnectedAfterHostLoss)
                        {
                            hostLossHandled = false;
                            AddLocalNotice("Reconnected to the competition host. Your park can continue.");
                        }
                        ContextOpenWindow(WindowClass::multiplayer);
                    }
                    break;
                }
                case Network::Command::competitiveSnapshot:
                {
                    const auto body = ReadJson(packet);
                    if (!body.has_value())
                        break;
                    auto parsed = MatchStateFromJson(*body);
                    if (parsed.has_value()
                        && (parsed->epoch > state.epoch
                            || (parsed->epoch == state.epoch && parsed->revision >= state.revision)))
                    {
                        state = std::move(*parsed);
                    }
                    break;
                }
                case Network::Command::competitiveAbility:
                {
                    const auto body = ReadJson(packet);
                    const auto effect = body.has_value() ? ActiveEffectFromJson((*body)["effect"]) : std::nullopt;
                    if (!effect.has_value())
                        break;
                    std::string error;
                    const bool accepted = ApplyLocalEffect(*effect, error);
                    SendLocalEffectReply(effect->id, accepted, error);
                    break;
                }
                case Network::Command::competitiveEffectAck:
                {
                    const auto body = ReadJson(packet);
                    if (body.has_value())
                    {
                        HandleAbilityPaymentResult(*body);
                        const auto notification = body->value("notification", std::string{});
                        if (!notification.empty())
                            AddLocalNotice(notification);
                    }
                    break;
                }
                case Network::Command::competitiveError:
                {
                    const auto body = ReadJson(packet);
                    if (body.has_value())
                    {
                        const auto message = body->value("message", "The competitive host rejected the request.");
                        if (body->value("fatal", true))
                            SetError(message);
                        else
                            AddLocalNotice(message);
                    }
                    break;
                }
                case Network::Command::competitiveHeartbeat:
                    break;
                case Network::Command::competitiveChat:
                {
                    const auto body = ReadJson(packet);
                    if (body.has_value())
                        ShowChatLine(body->value("from", std::string("competitor")), body->value("text", std::string{}));
                    break;
                }
                default:
                    break;
            }
        }

        void BeginClientConnection(bool reconnect)
        {
            serverConnection = std::make_unique<Network::Connection>();
            serverConnection->socket = Network::CreateTcpSocket();
            serverConnection->socket->ConnectAsync(joinConfiguration.host, joinConfiguration.port);
            serverConnection->authStatus = Network::Auth::ok;
            helloSent = false;
            status = reconnect ? ConnectionStatus::reconnecting : ConnectionStatus::connecting;
        }

        void UpdateClient()
        {
            const auto now = Platform::GetTicks();
            if (serverConnection == nullptr)
            {
                if (now >= nextReconnectAt)
                {
                    BeginClientConnection(true);
                }
                return;
            }

            const auto socketStatus = serverConnection->socket->GetStatus();
            if (socketStatus == Network::SocketStatus::waiting || socketStatus == Network::SocketStatus::resolving
                || socketStatus == Network::SocketStatus::connecting)
            {
                return;
            }
            if (socketStatus == Network::SocketStatus::closed)
            {
                serverConnection.reset();
                if (status != ConnectionStatus::error)
                    HandleHostConnectionLoss(now);
                return;
            }
            if (socketStatus == Network::SocketStatus::connected && !helloSent)
            {
                SendHello();
            }
            serverConnection->update();
            while (serverConnection->isValid())
            {
                const auto read = serverConnection->readPacket();
                if (read != Network::ReadPacket::success)
                    break;
                HandleClientPacket(serverConnection->inboundPacket);
                serverConnection->inboundPacket.clear();
            }

            if (serverConnection->isValid() && now - lastHeartbeatAt >= kHeartbeatIntervalMs)
            {
                lastHeartbeatAt = now;
                QueueJson(*serverConnection, Network::Command::competitiveHeartbeat, { { "reply", false } });
            }
            serverConnection->sendQueuedData();

            if (!serverConnection->isValid())
            {
                serverConnection.reset();
                if (status != ConnectionStatus::error)
                    HandleHostConnectionLoss(now);
                return;
            }

            if (status == ConnectionStatus::online && localRole == Role::player && scenarioLoaded
                && state.phase == Phase::running && gLegacyScene == LegacyScene::playing)
            {
                const auto localDay = GetCurrentLocalDay();
                if (localDay != lastReportedDay)
                {
                    const auto metrics = CollectParkMetrics();
                    lastReportedDay = metrics.localDay;
                    QueueJson(
                        *serverConnection, Network::Command::competitiveMetrics, { { "metrics", ToJson(metrics) } });
                }
            }
        }
    };

    Session::Session()
        : _impl(std::make_unique<Impl>())
    {
    }

    Session::~Session() = default;

    bool Session::StartHost(const HostConfiguration& configuration, std::string& error)
    {
        Stop();
        if (configuration.competitionName.empty() || configuration.playerName.empty())
        {
            error = "Competition and player names are required.";
            return false;
        }
        if (configuration.port == 0 || configuration.scenario.fileName.empty()
            || configuration.scenario.contentHash.empty())
        {
            error = "A valid scenario and competitive network port are required.";
            return false;
        }
        if (gLegacyScene != LegacyScene::playing)
        {
            error = "Load the competition scenario before creating its lobby.";
            return false;
        }

        _impl->listener = Network::CreateTcpSocket();
        try
        {
            _impl->listener->Listen(configuration.listenAddress, configuration.port);
        }
        catch (const std::exception& exception)
        {
            error = exception.what();
            _impl->ClearTransport();
            return false;
        }

        _impl->mode = SessionMode::host;
        _impl->status = ConnectionStatus::online;
        _impl->localRole = Role::host;
        _impl->hostConfiguration = configuration;
        _impl->localParticipantId = MakeParticipantId();
        _impl->state = {};
        _impl->state.matchId = MakeMatchId();
        _impl->state.name = configuration.competitionName;
        _impl->state.phase = Phase::lobby;
        _impl->state.hostId = _impl->localParticipantId;
        _impl->state.rules = configuration.rules;
        _impl->state.scenario = configuration.scenario;
        const auto metrics = CollectParkMetrics();
        _impl->state.startLocalDay = metrics.localDay;
        _impl->state.participants.push_back({
            _impl->localParticipantId,
            Config::Get().network.competitiveIdentity,
            configuration.playerName,
            Role::host,
            true,
            false,
            false,
            false,
            0,
            metrics.localYear,
            configuration.scenario,
        });
        _impl->state.scores.push_back({
            .participantId = _impl->localParticipantId,
            .rating = metrics.rating,
            .guests = metrics.guests,
            .parkValue = metrics.parkValue,
            .cash = metrics.cash,
        });
        _impl->state.reports.push_back({ _impl->localParticipantId, metrics, metrics.localDay });
        _impl->lastReportedDay = metrics.localDay;
        _impl->scenarioLoaded = true;
        _impl->advertiser = Network::CreateServerAdvertiser(
            configuration.port, [impl = _impl.get()] { return impl->GetServerInfo(); },
            [impl = _impl.get()] { return CountCompetingParks(impl->state); },
            [impl = _impl.get()] {
                return json_t{
                    { "gameMode", "competitive" },
                    { "phase", PhaseName(impl->state.phase) },
                    { "scenario", impl->state.scenario.name },
                };
            });
        std::string watchWarning;
        if (!_impl->EnsureLocalWatchServer(watchWarning))
            _impl->AddLocalNotice("Park spectating is unavailable for the host park: " + watchWarning);
        Config::Save();
        // The caller loads (and hand-pauses) the scenario before StartHost. Record that as a
        // session-forced pause so StartMatch later unpauses the host exactly like it unpauses the
        // clients - otherwise EnsurePauseState treats the hand-pause as "the host wanted it paused".
        if (!GameIsPaused())
            PauseToggle();
        _impl->forcedPause = true;
        _impl->wasPausedBeforeForcedPause = false;
        _impl->EnsurePauseState();
        return true;
    }

    bool Session::Join(const JoinConfiguration& configuration, std::string& error)
    {
        Stop();
        if (configuration.host.empty() || configuration.port == 0 || configuration.playerName.empty()
            || (configuration.role != Role::player && configuration.role != Role::spectator))
        {
            error = "Host, player name, and a valid competitive role are required.";
            return false;
        }
        _impl->mode = SessionMode::client;
        _impl->localRole = configuration.role;
        _impl->joinConfiguration = configuration;
        _impl->BeginClientConnection(false);
        Config::Save();
        return true;
    }

    void Session::Stop()
    {
        if (_impl == nullptr)
            return;
        std::vector<uint32_t> localEffectIds;
        for (const auto& [effectId, effect] : _impl->localEffects)
            localEffectIds.push_back(effectId);
        for (const auto effectId : localEffectIds)
        {
            _impl->EndLocalVandal(effectId);
            _impl->EndLocalOperative(effectId);
        }
        _impl->ClearTransport();
        _impl->mode = SessionMode::none;
        _impl->status = ConnectionStatus::disconnected;
        _impl->state = {};
        _impl->localParticipantId = kInvalidParticipantId;
        _impl->localRole = Role::none;
        _impl->hostConfiguration = {};
        _impl->joinConfiguration = {};
        _impl->lastError.clear();
        _impl->scenarioLoaded = false;
        _impl->pendingMisinformationCancellations = 0;
        _impl->nextAbilityRequestId = 1;
        _impl->pendingAbilityPayments.clear();
        _impl->localEffects.clear();
        _impl->alertedEffects.clear();
        _impl->localVandals.clear();
        _impl->localOperatives.clear();
        _impl->localGroups.clear();
        _impl->actorKinds.clear();
        _impl->ReleaseAllDetainedStaff(); // also clears the complaint cooldowns
        _impl->stonerNextPuffTick.clear();
        _impl->stonerLastStareTick.clear();
        gLocalActorsActive = false;
        _impl->openWindowAfterRestore = false;
        _impl->hostLossHandled = false;
        if (_impl->startedWatchServer && Network::GetMode() == Network::Mode::server)
            Network::Close();
        _impl->startedWatchServer = false;
        _impl->EnsurePauseState();
    }

    void Session::Update()
    {
        const auto* local = GetLocalParticipant();
        if (gLegacyScene == LegacyScene::playing && local != nullptr && local->role != Role::spectator
            && !local->finished && !local->forfeited
            && (_impl->state.phase == Phase::lobby || _impl->state.phase == Phase::running))
        {
            CheatsReset();
        }
        if (_impl->mode == SessionMode::host)
        {
            if (_impl->advertiser != nullptr)
                _impl->advertiser->update();
            _impl->UpdateHost();
        }
        else if (_impl->mode == SessionMode::client)
        {
            _impl->UpdateClient();
        }
        _impl->UpdateLocalEffects();
        _impl->EnsurePauseState();
        if (_impl->openWindowAfterRestore && gLegacyScene == LegacyScene::playing)
        {
            _impl->openWindowAfterRestore = false;
            ContextOpenWindow(WindowClass::multiplayer);
        }
    }

    SessionMode Session::GetMode() const
    {
        return _impl->mode;
    }

    ConnectionStatus Session::GetConnectionStatus() const
    {
        return _impl->status;
    }

    const MatchState* Session::GetState() const
    {
        return _impl->state.phase == Phase::none ? nullptr : &_impl->state;
    }

    ParticipantId Session::GetLocalParticipantId() const
    {
        return _impl->localParticipantId;
    }

    const Participant* Session::GetLocalParticipant() const
    {
        return FindParticipant(_impl->state, _impl->localParticipantId);
    }

    bool Session::IsOnline() const
    {
        return _impl->status == ConnectionStatus::online;
    }

    const std::string& Session::GetLastError() const
    {
        return _impl->lastError;
    }

    std::string Session::GetStatusText() const
    {
        switch (_impl->status)
        {
            case ConnectionStatus::connecting:
                return "Connecting to competition";
            case ConnectionStatus::online:
                return _impl->state.phase == Phase::lobby ? "Online — waiting in lobby" : "Online";
            case ConnectionStatus::reconnecting:
                return "Connection lost — reconnecting; this park is paused";
            case ConnectionStatus::error:
                return _impl->lastError;
            case ConnectionStatus::disconnected:
                return "Not in a competition";
        }
        return "Not in a competition";
    }

    std::vector<std::string> Session::GetStartProblems() const
    {
        std::vector<std::string> problems;
        if (_impl->mode != SessionMode::host || _impl->state.phase != Phase::lobby)
        {
            problems.push_back("Only the host can start a competition lobby.");
            return problems;
        }
        std::vector<const Participant*> competitors;
        for (const auto& participant : _impl->state.participants)
        {
            if (participant.role != Role::spectator && !participant.forfeited)
                competitors.push_back(&participant);
        }
        if (competitors.size() < 2)
            problems.push_back("At least two parks must join.");
        if (std::any_of(competitors.begin(), competitors.end(), [](const auto* value) { return !value->online; }))
            problems.push_back("Every competing park must be online.");
        if (std::any_of(competitors.begin(), competitors.end(), [](const auto* value) { return !value->ready; }))
            problems.push_back("Every competing park must ready up.");
        for (const auto* participant : competitors)
        {
            const auto* report = FindReport(_impl->state, participant->id);
            if (!IsSameScenario(participant->scenario, _impl->state.scenario) || report == nullptr
                || report->metrics.localDay != _impl->state.startLocalDay)
            {
                problems.push_back("Every park must use the required scenario at the same starting date.");
                break;
            }
            if (report->metrics.localYear >= _impl->state.rules.deadlineYear)
            {
                problems.push_back("The deadline must be later than every park's starting year.");
                break;
            }
        }
        if (_impl->state.rules.victoryMode == VictoryMode::target)
        {
            for (const auto& score : _impl->state.scores)
            {
                if (GetMetricValue(score, _impl->state.rules.metric) >= _impl->state.rules.target)
                {
                    problems.push_back("The victory target must exceed every starting value.");
                    break;
                }
            }
        }
        if (_impl->state.scenario.noMoney && _impl->state.rules.metric == Metric::cash)
            problems.push_back("Cash cannot be the victory metric in a no-money scenario.");
        return problems;
    }

    bool Session::SetReady(bool ready, std::string& error)
    {
        if (_impl->state.phase != Phase::lobby || _impl->localRole == Role::spectator
            || gLegacyScene != LegacyScene::playing)
        {
            error = "Only a loaded competing park can change its lobby readiness.";
            return false;
        }
        const auto scenario = GetCurrentScenarioIdentity();
        const auto metrics = CollectParkMetrics();
        if (!IsSameScenario(scenario, _impl->state.scenario) || metrics.localDay != _impl->state.startLocalDay)
        {
            error = "This park does not match the lobby's required scenario and starting date.";
            return false;
        }

        if (_impl->mode == SessionMode::host)
        {
            auto* participant = FindParticipant(_impl->state, _impl->localParticipantId);
            auto* report = FindReport(_impl->state, _impl->localParticipantId);
            if (participant == nullptr || report == nullptr)
            {
                error = "The host park has no competition seat.";
                return false;
            }
            participant->ready = ready;
            participant->scenario = scenario;
            participant->currentYear = metrics.localYear;
            report->metrics = metrics;
            if (auto* score = FindScore(_impl->state, participant->id))
            {
                score->rating = metrics.rating;
                score->guests = metrics.guests;
                score->parkValue = metrics.parkValue;
                score->cash = metrics.cash;
            }
            _impl->SendSnapshot();
            return true;
        }
        if (_impl->mode == SessionMode::client && _impl->serverConnection != nullptr && _impl->serverConnection->isValid())
        {
            QueueJson(
                *_impl->serverConnection, Network::Command::competitiveReady,
                { { "ready", ready }, { "scenario", ToJson(scenario) }, { "metrics", ToJson(metrics) } });
            return true;
        }
        error = "The competition connection is offline.";
        return false;
    }

    bool Session::StartMatch(std::string& error)
    {
        const auto problems = GetStartProblems();
        if (!problems.empty())
        {
            error = problems.front();
            return false;
        }
        _impl->state.phase = Phase::running;
        for (auto& participant : _impl->state.participants)
        {
            participant.ready = false;
        }
        for (auto& report : _impl->state.reports)
            report.lastScoredDay = report.metrics.localDay;
        _impl->SendSnapshot();
        _impl->EnsurePauseState();
        return true;
    }

    bool Session::UpdateRules(const MatchRules& rules, std::string& error)
    {
        if (_impl->mode != SessionMode::host || _impl->state.phase != Phase::lobby)
        {
            error = "Rules can be changed only by the host before the match starts.";
            return false;
        }
        _impl->state.rules = rules;
        for (auto& participant : _impl->state.participants)
            participant.ready = false;
        for (auto& score : _impl->state.scores)
        {
            const auto participantId = score.participantId;
            const auto* report = FindReport(_impl->state, participantId);
            score = {
                .participantId = participantId,
                .rating = report == nullptr ? uint16_t{} : report->metrics.rating,
                .guests = report == nullptr ? uint32_t{} : report->metrics.guests,
                .parkValue = report == nullptr ? money64{} : report->metrics.parkValue,
                .cash = report == nullptr ? money64{} : report->metrics.cash,
            };
        }
        _impl->SendSnapshot();
        return true;
    }

    bool Session::UseAbility(Ability ability, ParticipantId targetId, int32_t targetRideId, std::string& error)
    {
        if (ability > kLastAbility || _impl->status != ConnectionStatus::online)
        {
            error = "The competition connection is not online.";
            return false;
        }
        if (_impl->mode != SessionMode::host
            && (_impl->mode != SessionMode::client || _impl->serverConnection == nullptr
                || !_impl->serverConnection->isValid()))
        {
            error = "The competition connection is not online.";
            return false;
        }
        const auto* local = GetLocalParticipant();
        const auto* target = FindParticipant(_impl->state, targetId);
        if (_impl->state.phase != Phase::running || local == nullptr || local->role == Role::spectator
            || local->finished || target == nullptr || targetId == local->id || !CanTarget(*target))
        {
            error = "Choose an online, unfinished rival while the competition is running.";
            return false;
        }
        const auto& rule = GetAbilityRule(_impl->state.rules, ability);
        if (!rule.enabled)
        {
            error = std::string(AbilityName(ability)) + " is disabled by the host's match rules.";
            return false;
        }
        const auto* localReport = FindReport(_impl->state, local->id);
        const auto cooldown = std::find_if(
            _impl->state.cooldowns.begin(), _impl->state.cooldowns.end(), [&](const auto& value) {
                return value.participantId == local->id && value.ability == ability;
            });
        if (localReport != nullptr && cooldown != _impl->state.cooldowns.end()
            && localReport->metrics.localDay < cooldown->availableAtDay)
        {
            error = std::string(AbilityName(ability)) + " is on cooldown until your local day "
                + std::to_string(cooldown->availableAtDay) + ".";
            return false;
        }
        if (localReport != nullptr && rule.usesPerYear != 0
            && AbilityUsesThisYear(_impl->state, local->id, ability, localReport->metrics.localYear) >= rule.usesPerYear)
        {
            error = std::string(AbilityName(ability)) + " has no uses left for your local year "
                + std::to_string(localReport->metrics.localYear) + " (limit " + std::to_string(rule.usesPerYear)
                + " per year).";
            return false;
        }
        const auto duplicate = std::find_if(
            _impl->state.effects.begin(), _impl->state.effects.end(), [&](const auto& effect) {
                return effect.targetId == targetId && EffectClashes(effect, ability, targetRideId);
            });
        if (duplicate != _impl->state.effects.end())
        {
            error = std::string(AbilityName(ability)) + " is already active against that target.";
            return false;
        }
        const auto cost = _impl->state.scenario.noMoney ? 0.00_GBP : rule.cost;
        if (cost > 0 && getGameState().park.cash < cost)
        {
            error = "Your park does not have enough cash for this action.";
            return false;
        }
        auto requestId = _impl->nextAbilityRequestId++;
        if (requestId == 0)
            requestId = _impl->nextAbilityRequestId++;
        _impl->pendingAbilityPayments.emplace(
            requestId, Impl::PendingAbilityPayment{ ability, targetId, cost });
        if (cost > 0)
            FinancePayment(cost, ExpenditureType::marketing);
        _impl->ReportLocalMetricsNow();

        if (_impl->mode == SessionMode::host)
        {
            const auto accepted = _impl->BeginAbility(
                _impl->localParticipantId, ability, targetId, targetRideId, requestId, cost, error);
            if (!accepted)
                _impl->ResolveAbilityPayment(_impl->localParticipantId, requestId, false, error);
            return accepted;
        }
        QueueJson(
            *_impl->serverConnection, Network::Command::competitiveAbility,
            {
                { "ability", ability },
                { "targetId", targetId },
                { "targetRideId", targetRideId },
                { "sourceRequestId", requestId },
                { "chargedCost", cost },
            });
        return true;
    }

    bool Session::WatchParticipant(ParticipantId targetId, std::string& error)
    {
        if (gWatchReturnState.active)
        {
            error = "Return from the park currently being watched before choosing another.";
            return false;
        }
        const auto* local = GetLocalParticipant();
        const auto* target = FindParticipant(_impl->state, targetId);
        if (_impl->status != ConnectionStatus::online || local == nullptr || target == nullptr || target->id == local->id
            || target->role == Role::spectator || !target->online || target->watchPort == 0)
        {
            error = "Choose an online rival park with an available watch connection.";
            return false;
        }

        auto host = target->watchHost;
        const auto watchPort = target->watchPort;
        const auto targetName = target->name;
        if (host.empty() && target->id == _impl->state.hostId && _impl->mode == SessionMode::client)
            host = _impl->joinConfiguration.host;
        if (host.empty())
        {
            error = "The rival park has not published a reachable watch address.";
            return false;
        }

        WatchReturnState returnState;
        returnState.active = true;
        if (local->role != Role::spectator)
        {
            if (gLegacyScene != LegacyScene::playing)
            {
                error = "Your competitive park must be loaded before watching a rival.";
                return false;
            }
            auto& environment = GetContext()->GetPlatformEnvironment();
            const auto returnDirectory = Path::Combine(
                environment.GetDirectoryPath(DirBase::user, DirId::saves), u8"competitive-return");
            if (!Path::CreateDirectory(returnDirectory))
            {
                error = "OpenRCT2 could not create the competitive return-save folder.";
                return false;
            }
            returnState.returnPath = Path::Combine(
                returnDirectory, "return-" + _impl->state.matchId + "-" + std::to_string(local->id) + ".park");
            if (!ScenarioSave(getGameState(), returnState.returnPath, kAutomaticSaveFlag))
            {
                error = "OpenRCT2 could not save your park before spectating.";
                return false;
            }
            returnState.hasReturnPark = true;
        }

        gWatchReturnState = returnState;
        if (local->role != Role::spectator)
            Stop();
        if (Network::GetMode() != Network::Mode::none)
            Network::Close();

        if (!Network::BeginClient(host, watchPort, true))
        {
            const auto failedState = gWatchReturnState;
            gWatchReturnState = {};
            if (failedState.hasReturnPark)
                GetContext()->LoadParkFromFile(failedState.returnPath);
            error = "OpenRCT2 could not start the read-only connection to " + targetName + ".";
            return false;
        }
        return true;
    }

    void Session::UpdateGuestGenerationInterference()
    {
        if (_impl->state.phase != Phase::running || _impl->status != ConnectionStatus::online)
            return;
        const auto localDay = GetCurrentLocalDay();
        uint32_t potency{};
        for (const auto& [effectId, effect] : _impl->localEffects)
        {
            // Allied attackers can run several campaigns at once; their strengths add up.
            if (effect.ability == Ability::misinformation && localDay >= effect.startsAtDay && localDay < effect.endsAtDay)
                potency += effect.potency;
        }
        potency = std::min<uint32_t>(potency, std::numeric_limits<uint16_t>::max());
        if (potency != 0 && ScenarioRandMax(std::numeric_limits<uint16_t>::max()) < potency)
        {
            _impl->pendingMisinformationCancellations = std::min<uint32_t>(
                1024, _impl->pendingMisinformationCancellations + 1);
        }
    }

    bool Session::ConsumeGuestArrivalCancellation()
    {
        if (_impl->pendingMisinformationCancellations == 0)
            return false;
        _impl->pendingMisinformationCancellations--;
        return true;
    }

    void Session::OnGuestPurchase(OpenRCT2::Guest& guest, RideId rideId, bool isFoodOrDrink)
    {
        if (!isFoodOrDrink || _impl->state.phase != Phase::running || _impl->status != ConnectionStatus::online)
            return;
        if (_impl->localEffects.empty())
            return;
        const auto localDay = GetCurrentLocalDay();
        uint16_t chance{};
        for (const auto& [effectId, effect] : _impl->localEffects)
        {
            if (effect.ability == Ability::poison && effect.targetRideId == rideId.ToUnderlying()
                && localDay >= effect.startsAtDay && localDay < effect.endsAtDay)
            {
                chance = std::max(chance, effect.potency);
            }
        }
        uint32_t poisonedEffectId = 0;
        for (const auto& [effectId, effect] : _impl->localEffects)
        {
            if (effect.ability == Ability::poison && effect.targetRideId == rideId.ToUnderlying())
            {
                poisonedEffectId = effectId;
                break;
            }
        }
        if (chance == 0 || ScenarioRandMax(100) >= chance)
            return;
        guest.nausea = 255;
        guest.nauseaTarget = 255;
        guest.windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;
        if (poisonedEffectId != 0)
        {
            const auto* ride = GetRide(rideId);
            _impl->AlertVictimOnce(
                poisonedEffectId,
                std::string("Guests are being taken ill after buying from ")
                    + (ride == nullptr ? std::string("one of your stalls") : ride->getName())
                    + " - it looks like the food or drink was tampered with.");
        }
    }

    void Session::OnVandalAttempt(OpenRCT2::Guest& guest, bool succeeded)
    {
        const auto iterator = std::find_if(_impl->localVandals.begin(), _impl->localVandals.end(), [&](const auto& vandal) {
            return vandal.guestId == guest.id;
        });
        if (iterator == _impl->localVandals.end())
            return;
        if (iterator->remainingQuota > 0)
            iterator->remainingQuota--;

        if (succeeded)
        {
            _impl->AlertVictimOnce(
                iterator->effectId, "A vandal is loose in your park, damaging benches, lamps and signs.",
                guest.id.ToUnderlying());
        }

        if (iterator->remainingQuota == 0)
        {
            const auto effectId = iterator->effectId;
            _impl->CompleteLocalEffect(effectId, "the vandal used its final attempt and was sent home");
            return;
        }

        // Pace the attacks: calm down and wander before the next attempt.
        iterator->nextAttemptTick = getGameState().currentTicks + 1200 + ScenarioRandMax(2400);
        guest.peepFlags.unset(PeepFlag::angry);
        guest.angriness = 0;
        guest.happinessTarget = 40;
    }

    void Session::OnGuestExitRide(OpenRCT2::Guest& guest, RideId rideId)
    {
        const auto iterator = std::find_if(_impl->localOperatives.begin(), _impl->localOperatives.end(), [&](const auto& operative) {
            return operative.ability == Ability::saboteur && operative.guestId == guest.id
                && operative.targetRideId == rideId.ToUnderlying();
        });
        if (iterator == _impl->localOperatives.end())
            return;
        auto* ride = GetRide(rideId);
        if (ride == nullptr)
            return;
        auto breakdown = Breakdown::none;
        for (const auto candidate : kAllBreakdownTypes)
        {
            if (ride->getRideTypeDescriptor().availableBreakdowns.has(candidate))
            {
                breakdown = candidate;
                break;
            }
        }
        if (breakdown == Breakdown::none)
            return;
        RidePrepareBreakdown(*ride, breakdown);
        const auto effectId = iterator->effectId;
        _impl->AlertVictimOnce(
            effectId, "A ride in your park was sabotaged: " + ride->getName() + " has broken down.");
        _impl->CompleteLocalEffect(effectId, "the saboteur completed the ride and forced a breakdown");
    }

    bool Session::IsProtectedAgent(EntityId guestId) const
    {
        return _impl->IsCompetitiveAgent(guestId);
    }

    bool Session::IsProtectedRide(RideId rideId) const
    {
        const auto underlying = static_cast<int32_t>(rideId.ToUnderlying());
        for (const auto& [effectId, effect] : _impl->localEffects)
        {
            if (effect.targetRideId != underlying)
                continue;
            if (effect.ability == Ability::poison || effect.ability == Ability::toiletBomber
                || effect.ability == Ability::saboteur)
                return true;
        }
        return false;
    }

    money64 Session::GetAvailableParkCash() const
    {
        if (_impl->state.scenario.noMoney || gLegacyScene != LegacyScene::playing)
            return 0;
        return getGameState().park.cash;
    }

    int32_t Session::GetStaffWageMultiplier() const
    {
        for (const auto& [effectId, effect] : _impl->localEffects)
        {
            if (effect.ability == Ability::unionDisruption)
                return 2;
        }
        return 1;
    }

    uint8_t Session::GetGroupGuestKind(EntityId guestId) const
    {
        const auto kind = _impl->GroupGuestKind(guestId);
        if (!kind.has_value())
            return 0;
        return *kind == Ability::stoners ? 2 : 1;
    }

    bool Session::IsStaffDetained(EntityId staffId) const
    {
        return _impl->IsStaffDetained(staffId);
    }

    bool Session::IsKarenConfrontingStaff(EntityId guestId) const
    {
        return _impl->IsKarenConfrontingStaff(guestId);
    }

    bool Session::StonerMayStare(EntityId guestId) const
    {
        return _impl->StonerMayStare(guestId);
    }

    void Session::NoteStonerStareStarted(EntityId guestId)
    {
        _impl->NoteStonerStareStarted(guestId);
    }

    std::string Session::ExportParkStorage() const
    {
        if (_impl->mode == SessionMode::none || _impl->state.phase == Phase::none
            || _impl->localParticipantId == kInvalidParticipantId)
            return {};

        json_t localEffects = json_t::array();
        for (const auto& [effectId, effect] : _impl->localEffects)
            localEffects.push_back(ToJson(effect));

        json_t localVandals = json_t::array();
        for (const auto& vandal : _impl->localVandals)
        {
            localVandals.push_back({
                { "effectId", vandal.effectId },
                { "guestId", vandal.guestId.ToUnderlying() },
                { "remainingQuota", vandal.remainingQuota },
            });
        }

        json_t pendingPayments = json_t::array();
        for (const auto& [requestId, payment] : _impl->pendingAbilityPayments)
        {
            pendingPayments.push_back({
                { "requestId", requestId },
                { "ability", payment.ability },
                { "targetId", payment.targetId },
                { "cost", payment.cost },
            });
        }

        json_t localGroups = json_t::array();
        for (const auto& group : _impl->localGroups)
        {
            json_t guestIds = json_t::array();
            for (const auto guestId : group.guestIds)
                guestIds.push_back(guestId.ToUnderlying());
            localGroups.push_back({
                { "effectId", group.effectId },
                { "ability", group.ability },
                { "guestIds", std::move(guestIds) },
            });
        }

        json_t detainedStaff = json_t::array();
        for (const auto& [staffId, detention] : _impl->detainedStaff)
            detainedStaff.push_back({ { "staffId", staffId },
                                      { "releaseTick", detention.releaseTick },
                                      { "karenId", detention.karenId.ToUnderlying() } });

        json_t localOperatives = json_t::array();
        for (const auto& operative : _impl->localOperatives)
        {
            json_t affectedGuests = json_t::array();
            for (const auto guestId : operative.affectedGuests)
                affectedGuests.push_back(guestId.ToUnderlying());
            localOperatives.push_back({
                { "effectId", operative.effectId },
                { "ability", operative.ability },
                { "guestId", operative.guestId.ToUnderlying() },
                { "targetRideId", operative.targetRideId },
                { "victimId", operative.victimId.ToUnderlying() },
                { "stage", operative.stage },
                { "triggerAtTick", operative.triggerAtTick },
                { "affectedGuests", std::move(affectedGuests) },
            });
        }

        return json_t{
            { "schema", 4 },
            { "protocol", kProtocolVersion },
            { "mode", _impl->mode },
            { "localRole", _impl->localRole },
            { "localParticipantId", _impl->localParticipantId },
            { "state", ToJson(_impl->state) },
            { "host",
              {
                  { "listenAddress", _impl->hostConfiguration.listenAddress },
                  { "port", _impl->hostConfiguration.port },
              } },
            { "join",
              {
                  { "host", _impl->joinConfiguration.host },
                  { "port", _impl->joinConfiguration.port },
                  { "playerName", _impl->joinConfiguration.playerName },
                  { "role", _impl->joinConfiguration.role },
              } },
            { "pendingMisinformationCancellations", _impl->pendingMisinformationCancellations },
            { "nextAbilityRequestId", _impl->nextAbilityRequestId },
            { "pendingAbilityPayments", std::move(pendingPayments) },
            { "localEffects", std::move(localEffects) },
            { "localVandals", std::move(localVandals) },
            { "localOperatives", std::move(localOperatives) },
            { "localGroups", std::move(localGroups) },
            { "detainedStaff", std::move(detainedStaff) },
        }.dump();
    }

    bool Session::RestoreParkStorage(std::string_view storage, std::string& error)
    {
        if (storage.empty())
            return true;
        const auto body = json_t::parse(storage.begin(), storage.end(), nullptr, false);
        if (body.is_discarded() || !body.is_object() || body.value("schema", 0) != 4
            || body.value("protocol", 0) != kProtocolVersion)
        {
            error = "This park contains unsupported or malformed competitive session data.";
            return false;
        }

        auto restoredState = MatchStateFromJson(body["state"]);
        const auto restoredMode = body.value("mode", SessionMode::none);
        const auto restoredRole = body.value("localRole", Role::none);
        const auto restoredParticipantId = body.value("localParticipantId", kInvalidParticipantId);
        const auto* restoredParticipant = restoredState.has_value()
            ? FindParticipant(*restoredState, restoredParticipantId)
            : nullptr;
        if (!restoredState.has_value() || restoredParticipant == nullptr
            || (restoredMode != SessionMode::host && restoredMode != SessionMode::client)
            || (restoredRole != Role::host && restoredRole != Role::player && restoredRole != Role::spectator)
            || restoredParticipant->role != restoredRole)
        {
            error = "The saved competitive membership is inconsistent.";
            return false;
        }
        if ((restoredMode == SessionMode::host) != (restoredRole == Role::host)
            || restoredState->hostId == kInvalidParticipantId)
        {
            error = "The saved competitive host role is inconsistent.";
            return false;
        }
        const auto currentScenario = GetCurrentScenarioIdentity();
        const auto currentMetrics = CollectParkMetrics();
        const auto* savedLocalReport = FindReport(*restoredState, restoredParticipantId);
        if (restoredRole != Role::spectator
            && (currentScenario.mapWidth != restoredState->scenario.mapWidth
                || currentScenario.mapHeight != restoredState->scenario.mapHeight
                || savedLocalReport == nullptr || currentMetrics.localDay < savedLocalReport->metrics.localDay))
        {
            error = "The loaded park does not match this competitive membership or is older than its saved report.";
            return false;
        }
        if (!currentScenario.contentHash.empty() && !IsSameScenario(currentScenario, restoredState->scenario))
        {
            error = "The loaded park's original scenario does not match the competition.";
            return false;
        }

        const auto& host = body["host"];
        const auto& join = body["join"];
        HostConfiguration restoredHostConfiguration{
            restoredState->name,
            restoredParticipant->name,
            host.is_object() ? host.value("listenAddress", std::string{}) : std::string{},
            host.is_object() ? host.value("port", kDefaultPort) : kDefaultPort,
            restoredState->rules,
            restoredState->scenario,
        };
        JoinConfiguration restoredJoinConfiguration{
            join.is_object() ? join.value("host", std::string{}) : std::string{},
            join.is_object() ? join.value("port", kDefaultPort) : kDefaultPort,
            join.is_object() ? join.value("playerName", restoredParticipant->name) : restoredParticipant->name,
            join.is_object() ? join.value("role", restoredRole) : restoredRole,
        };
        const auto restoredIdentityKey = restoredParticipant->identityKey;
        if ((restoredMode == SessionMode::host && restoredHostConfiguration.port == 0)
            || (restoredMode == SessionMode::client
                && (restoredJoinConfiguration.host.empty() || restoredJoinConfiguration.port == 0)))
        {
            error = "The saved competitive connection details are incomplete.";
            return false;
        }

        Stop();
        _impl->state = std::move(*restoredState);
        _impl->mode = restoredMode;
        _impl->localRole = restoredRole;
        _impl->localParticipantId = restoredParticipantId;
        _impl->hostConfiguration = restoredHostConfiguration;
        _impl->joinConfiguration = restoredJoinConfiguration;
        _impl->scenarioLoaded = true;
        _impl->pendingMisinformationCancellations = std::min<uint32_t>(
            1024, body.value("pendingMisinformationCancellations", 0u));
        _impl->nextAbilityRequestId = std::max<uint32_t>(1, body.value("nextAbilityRequestId", 1u));
        if (body.contains("pendingAbilityPayments") && body["pendingAbilityPayments"].is_array())
        {
            for (const auto& value : body["pendingAbilityPayments"])
            {
                if (!value.is_object())
                    continue;
                const auto requestId = value.value("requestId", 0u);
                const auto ability = value.value("ability", Ability::vandal);
                const auto targetId = value.value("targetId", kInvalidParticipantId);
                const auto cost = value.value("cost", 0.00_GBP);
                if (requestId != 0 && ability <= kLastAbility && targetId != kInvalidParticipantId && cost >= 0)
                    _impl->pendingAbilityPayments.emplace(
                        requestId, Impl::PendingAbilityPayment{ ability, targetId, cost });
            }
        }
        _impl->lastReportedDay = GetCurrentLocalDay();
        _impl->openWindowAfterRestore = true;

        Config::Get().network.competitiveIdentity = restoredIdentityKey;
        Config::Save();

        if (body.contains("localEffects") && body["localEffects"].is_array())
        {
            for (const auto& value : body["localEffects"])
            {
                auto effect = ActiveEffectFromJson(value);
                if (!effect.has_value() || !effect->delivered || effect->targetId != restoredParticipantId)
                    continue;
                const auto authoritative = std::find_if(
                    _impl->state.effects.begin(), _impl->state.effects.end(),
                    [&](const auto& current) { return current.id == effect->id && current.delivered; });
                if (authoritative != _impl->state.effects.end())
                    _impl->localEffects.emplace(effect->id, *effect);
            }
        }
        if (body.contains("localVandals") && body["localVandals"].is_array())
        {
            for (const auto& value : body["localVandals"])
            {
                if (!value.is_object())
                    continue;
                const auto effectId = value.value("effectId", 0u);
                const auto guestIdValue = value.value("guestId", EntityId::GetNull().ToUnderlying());
                const auto quota = value.value("remainingQuota", uint16_t{});
                const auto effect = _impl->localEffects.find(effectId);
                const auto guestId = EntityId::FromUnderlying(guestIdValue);
                if (effect != _impl->localEffects.end() && effect->second.ability == Ability::vandal && quota > 0
                    && getGameState().entities.getEntity<Guest>(guestId) != nullptr)
                    _impl->localVandals.push_back({ effectId, guestId, quota });
            }
        }
        if (body.contains("localOperatives") && body["localOperatives"].is_array())
        {
            for (const auto& value : body["localOperatives"])
            {
                if (!value.is_object())
                    continue;
                Impl::LocalOperative operative;
                operative.effectId = value.value("effectId", 0u);
                operative.ability = value.value("ability", Ability::agitator);
                operative.guestId = EntityId::FromUnderlying(
                    value.value("guestId", EntityId::GetNull().ToUnderlying()));
                operative.targetRideId = value.value("targetRideId", -1);
                operative.victimId = EntityId::FromUnderlying(
                    value.value("victimId", EntityId::GetNull().ToUnderlying()));
                operative.stage = value.value("stage", uint8_t{});
                operative.triggerAtTick = value.value("triggerAtTick", 0u);
                if (value.contains("affectedGuests") && value["affectedGuests"].is_array())
                {
                    for (const auto& guestId : value["affectedGuests"])
                    {
                        if (guestId.is_number_unsigned())
                            operative.affectedGuests.push_back(EntityId::FromUnderlying(guestId.get<uint16_t>()));
                    }
                }
                const auto effect = _impl->localEffects.find(operative.effectId);
                if (effect != _impl->localEffects.end() && effect->second.ability == operative.ability
                    && operative.ability >= Ability::toiletBomber && operative.ability <= Ability::hitman
                    && getGameState().entities.getEntity<Guest>(operative.guestId) != nullptr)
                    _impl->localOperatives.push_back(std::move(operative));
            }
        }
        if (body.contains("localGroups") && body["localGroups"].is_array())
        {
            for (const auto& value : body["localGroups"])
            {
                if (!value.is_object())
                    continue;
                Impl::LocalGroup group;
                group.effectId = value.value("effectId", 0u);
                group.ability = value.value("ability", Ability::karens);
                if (value.contains("guestIds") && value["guestIds"].is_array())
                {
                    for (const auto& guestId : value["guestIds"])
                    {
                        if (!guestId.is_number_unsigned())
                            continue;
                        const auto id = EntityId::FromUnderlying(guestId.get<uint16_t>());
                        if (getGameState().entities.getEntity<Guest>(id) != nullptr)
                            group.guestIds.push_back(id);
                    }
                }
                const auto effect = _impl->localEffects.find(group.effectId);
                if (effect != _impl->localEffects.end() && effect->second.ability == group.ability
                    && (group.ability == Ability::karens || group.ability == Ability::stoners)
                    && !group.guestIds.empty())
                    _impl->localGroups.push_back(std::move(group));
            }
        }
        if (body.contains("detainedStaff") && body["detainedStaff"].is_array())
        {
            for (const auto& value : body["detainedStaff"])
            {
                if (value.is_object() && value.contains("staffId") && value.contains("releaseTick"))
                    _impl->detainedStaff[value.value("staffId", 0u)] = {
                        value.value("releaseTick", 0u),
                        EntityId::FromUnderlying(
                            static_cast<EntityId::UnderlyingType>(value.value("karenId", 0xFFFFu))),
                    };
            }
        }
        _impl->RebuildActorKinds();

        if (_impl->mode == SessionMode::host)
        {
            for (auto& participant : _impl->state.participants)
            {
                participant.online = participant.id == _impl->localParticipantId;
                if (!participant.online)
                    participant.ready = false;
            }
            for (auto iterator = _impl->state.effects.begin(); iterator != _impl->state.effects.end();)
            {
                if (iterator->delivered)
                {
                    ++iterator;
                    continue;
                }
                iterator = _impl->state.effects.erase(iterator);
            }
            if (!_impl->state.scenario.noMoney)
            {
                for (const auto& [requestId, payment] : _impl->pendingAbilityPayments)
                {
                    if (payment.cost > 0)
                        FinancePayment(-payment.cost, ExpenditureType::marketing);
                }
            }
            _impl->pendingAbilityPayments.clear();

            _impl->listener = Network::CreateTcpSocket();
            try
            {
                _impl->listener->Listen(restoredHostConfiguration.listenAddress, restoredHostConfiguration.port);
            }
            catch (const std::exception& exception)
            {
                _impl->SetError(exception.what());
                _impl->EnsurePauseState();
                error = "The park was restored, but its competitive host port could not be reopened: "
                    + std::string(exception.what());
                return false;
            }
            _impl->status = ConnectionStatus::online;
            _impl->advertiser = Network::CreateServerAdvertiser(
                restoredHostConfiguration.port, [impl = _impl.get()] { return impl->GetServerInfo(); },
                [impl = _impl.get()] { return CountCompetingParks(impl->state); },
                [impl = _impl.get()] {
                    return json_t{
                        { "gameMode", "competitive" },
                        { "phase", PhaseName(impl->state.phase) },
                        { "scenario", impl->state.scenario.name },
                    };
                });
            std::string watchWarning;
            if (!_impl->EnsureLocalWatchServer(watchWarning))
                _impl->AddLocalNotice("Park spectating is unavailable for the host park: " + watchWarning);
            if (_impl->state.phase == Phase::running)
                _impl->IngestMetrics(_impl->localParticipantId, CollectParkMetrics());
        }
        else
        {
            if (auto* participant = FindParticipant(_impl->state, _impl->localParticipantId))
                participant->online = false;
            _impl->BeginClientConnection(true);
        }
        _impl->EnsurePauseState();
        return true;
    }

    bool Session::Forfeit(ParticipantId participantId, std::string& error)
    {
        if (_impl->mode != SessionMode::host || _impl->state.phase == Phase::finished)
        {
            error = "Only the host can forfeit an abandoned park before the result is final.";
            return false;
        }
        auto* participant = FindParticipant(_impl->state, participantId);
        if (participant == nullptr || participant->id == _impl->localParticipantId || participant->role == Role::spectator
            || participant->online || participant->finished || participant->forfeited)
        {
            error = "Only an offline, unfinished rival park can be forfeited.";
            return false;
        }
        participant->forfeited = true;
        participant->ready = false;
        if (CompetitionComplete(_impl->state.participants))
        {
            _impl->state.winnerId = ChooseWinner(
                _impl->state.scores, _impl->state.participants, _impl->state.rules.metric);
            _impl->state.phase = Phase::finished;
        }
        _impl->SendSnapshot();
        return true;
    }

    bool Session::CloseEarly(std::string& error)
    {
        if (_impl->mode != SessionMode::host || _impl->state.phase != Phase::running)
        {
            error = "Only the host can close a running competition early.";
            return false;
        }
        for (auto& participant : _impl->state.participants)
        {
            if (participant.role == Role::spectator || participant.forfeited || participant.finished)
                continue;
            auto* report = FindReport(_impl->state, participant.id);
            auto* score = FindScore(_impl->state, participant.id);
            if (report != nullptr && score != nullptr)
                FreezeScore(*score, report->metrics, report->metrics.localYear);
            participant.finished = true;
        }
        _impl->state.closedEarly = true;
        _impl->state.winnerId = ChooseWinner(_impl->state.scores, _impl->state.participants, _impl->state.rules.metric);
        _impl->state.phase = Phase::finished;
        _impl->SendSnapshot();
        return true;
    }

    void Session::SendChat(std::string_view text)
    {
        _impl->SendChatLine(std::string(text));
    }

    Session& GetSession()
    {
        static Session session;
        return session;
    }

    void Update()
    {
        GetSession().Update();
    }

    ScenarioIdentity GetScenarioIdentityForPath(const std::string& path)
    {
        ScenarioIdentity result;
        const auto* repository = GetScenarioRepository();
        const auto* entry = repository == nullptr ? nullptr : repository->GetByPath(path.c_str());
        result.fileName = Path::GetFileName(path);
        result.name = entry == nullptr ? result.fileName : entry->Name;
        result.contentHash = HashFile(path);
        if (gLegacyScene == LegacyScene::playing)
        {
            const auto& gameState = getGameState();
            result.mapWidth = gameState.mapSize.x;
            result.mapHeight = gameState.mapSize.y;
            result.noMoney = gameState.park.flags.has(ParkFlag::noMoney);
        }
        return result;
    }

    ScenarioIdentity GetCurrentScenarioIdentity()
    {
        const auto& gameState = getGameState();
        const auto* repository = GetScenarioRepository();
        const auto* entry = repository == nullptr ? nullptr : repository->GetByFilename(gameState.scenarioFileName);
        ScenarioIdentity result;
        result.fileName = Path::GetFileName(gameState.scenarioFileName);
        result.name = gameState.scenarioOptions.name;
        result.mapWidth = gameState.mapSize.x;
        result.mapHeight = gameState.mapSize.y;
        result.noMoney = gameState.park.flags.has(ParkFlag::noMoney);
        if (entry != nullptr)
        {
            result.contentHash = HashFile(entry->Path);
            if (result.name.empty())
                result.name = entry->Name;
        }
        return result;
    }

    ParkMetrics CollectParkMetrics()
    {
        const auto& gameState = getGameState();
        const auto& date = gameState.date;
        uint64_t happinessTotal = 0;
        uint64_t guestCount = 0;
        for (const auto* guest : EntityList<Guest>())
        {
            happinessTotal += guest->happiness;
            guestCount++;
        }

        std::vector<ParkMetrics::TargetRide> openFoodDrinkStalls;
        std::vector<ParkMetrics::TargetRide> openToilets;
        std::vector<ParkMetrics::TargetRide> openRides;
        for (const auto& ride : RideManager(gameState))
        {
            // Toilets are classified as kioskOrFacility, not shopOrStall, so test them independently.
            if (ride.status == RideStatus::open
                && ride.getRideTypeDescriptor().specialType == RtdSpecialType::toilet)
            {
                openToilets.push_back({ static_cast<int32_t>(ride.id.ToUnderlying()), ride.getName() });
            }

            if (ride.getClassification() == RideClassification::shopOrStall)
            {
                if (ride.status == RideStatus::open)
                {
                    const auto* rideEntry = ride.getRideEntry();
                    const bool sellsFoodOrDrink = rideEntry != nullptr
                        && std::any_of(
                            std::begin(rideEntry->shop_item), std::end(rideEntry->shop_item), [](const auto item) {
                                return item != ShopItem::none && GetShopItemDescriptor(item).IsFoodOrDrink();
                            });
                    if (sellsFoodOrDrink)
                    {
                        openFoodDrinkStalls.push_back(
                            { static_cast<int32_t>(ride.id.ToUnderlying()), ride.getName() });
                    }
                }
            }
            else if (ride.getClassification() == RideClassification::ride && ride.status == RideStatus::open
                && !ride.getRideTypeDescriptor().availableBreakdowns.isEmpty())
            {
                openRides.push_back({ ride.id.ToUnderlying(), ride.getName() });
            }
        }

        ParkMetrics result{
            static_cast<uint16_t>(date.GetYear() + 1),
            date.GetMonthsElapsed(),
            date.GetMonthsElapsed() * 32 + static_cast<uint32_t>(date.GetDay()) + 1,
            gameState.park.rating,
            gameState.park.numGuestsInPark,
            gameState.park.value,
            gameState.park.flags.has(ParkFlag::noMoney) ? 0.00_GBP : gameState.park.cash,
            static_cast<uint8_t>(guestCount == 0 ? 0 : happinessTotal / guestCount),
        };
        result.openFoodDrinkStalls = std::move(openFoodDrinkStalls);
        result.openToilets = std::move(openToilets);
        result.openRides = std::move(openRides);
        return result;
    }

    void UpdateGuestGenerationInterference()
    {
        GetSession().UpdateGuestGenerationInterference();
    }

    bool ConsumeGuestArrivalCancellation()
    {
        return GetSession().ConsumeGuestArrivalCancellation();
    }

    void OnGuestPurchase(OpenRCT2::Guest& guest, RideId rideId, bool isFoodOrDrink)
    {
        GetSession().OnGuestPurchase(guest, rideId, isFoodOrDrink);
    }

    void OnVandalAttempt(OpenRCT2::Guest& guest, bool succeeded)
    {
        GetSession().OnVandalAttempt(guest, succeeded);
    }

    void OnGuestExitRide(OpenRCT2::Guest& guest, RideId rideId)
    {
        GetSession().OnGuestExitRide(guest, rideId);
    }

    bool IsProtectedAgent(EntityId guestId)
    {
        return GetSession().IsProtectedAgent(guestId);
    }

    bool IsProtectedRide(RideId rideId)
    {
        return GetSession().IsProtectedRide(rideId);
    }

    int32_t GetStaffWageMultiplier()
    {
        return GetSession().GetStaffWageMultiplier();
    }

    uint8_t GetGroupGuestKind(EntityId guestId)
    {
        return GetSession().GetGroupGuestKind(guestId);
    }

    bool IsStaffDetained(EntityId staffId)
    {
        return GetSession().IsStaffDetained(staffId);
    }

    bool IsKarenConfrontingStaff(EntityId guestId)
    {
        return GetSession().IsKarenConfrontingStaff(guestId);
    }

    bool StonerMayStare(EntityId guestId)
    {
        return GetSession().StonerMayStare(guestId);
    }

    void NoteStonerStareStarted(EntityId guestId)
    {
        GetSession().NoteStonerStareStarted(guestId);
    }

    std::string ExportParkStorage()
    {
        return GetSession().ExportParkStorage();
    }

    bool RestoreParkStorage(std::string_view storage, std::string& error)
    {
        return GetSession().RestoreParkStorage(storage, error);
    }

    bool IsWatchingPark()
    {
        return gWatchReturnState.active;
    }

    bool ReturnFromWatchedPark(std::string& error)
    {
        if (!gWatchReturnState.active)
        {
            error = "No rival park is currently being watched.";
            return false;
        }

        const auto returnState = gWatchReturnState;
        Network::Close();
        if (returnState.hasReturnPark)
        {
            gWatchReturnState = {};
            if (!GetContext()->LoadParkFromFile(returnState.returnPath))
            {
                gWatchReturnState = returnState;
                error = "OpenRCT2 could not reload your competitive return save.";
                return false;
            }
            File::Delete(returnState.returnPath);
        }
        else
        {
            gWatchReturnState = {};
            if (!GameIsPaused())
                PauseToggle();
            ContextOpenWindow(WindowClass::multiplayer);
        }
        return true;
    }
#else
    struct Session::Impl
    {
        std::string error = "Competitive multiplayer is unavailable in this build.";
    };

    Session::Session()
        : _impl(std::make_unique<Impl>())
    {
    }
    Session::~Session() = default;
    bool Session::StartHost(const HostConfiguration&, std::string& error)
    {
        error = _impl->error;
        return false;
    }
    bool Session::Join(const JoinConfiguration&, std::string& error)
    {
        error = _impl->error;
        return false;
    }
    void Session::Stop() {}
    void Session::Update() {}
    SessionMode Session::GetMode() const { return SessionMode::none; }
    ConnectionStatus Session::GetConnectionStatus() const { return ConnectionStatus::error; }
    const MatchState* Session::GetState() const { return nullptr; }
    ParticipantId Session::GetLocalParticipantId() const { return kInvalidParticipantId; }
    const Participant* Session::GetLocalParticipant() const { return nullptr; }
    bool Session::IsOnline() const { return false; }
    const std::string& Session::GetLastError() const { return _impl->error; }
    std::string Session::GetStatusText() const { return _impl->error; }
    std::vector<std::string> Session::GetStartProblems() const { return { _impl->error }; }
    bool Session::SetReady(bool, std::string& error) { error = _impl->error; return false; }
    bool Session::StartMatch(std::string& error) { error = _impl->error; return false; }
    bool Session::UpdateRules(const MatchRules&, std::string& error) { error = _impl->error; return false; }
    bool Session::UseAbility(Ability, ParticipantId, int32_t, std::string& error) { error = _impl->error; return false; }
    void Session::UpdateGuestGenerationInterference() {}
    bool Session::ConsumeGuestArrivalCancellation() { return false; }
    void Session::OnGuestPurchase(OpenRCT2::Guest&, RideId, bool) {}
    void Session::OnVandalAttempt(OpenRCT2::Guest&, bool) {}
    void Session::OnGuestExitRide(OpenRCT2::Guest&, RideId) {}
    bool Session::IsProtectedAgent(EntityId) const { return false; }
    bool Session::IsProtectedRide(RideId) const { return false; }
    money64 Session::GetAvailableParkCash() const { return 0; }
    int32_t Session::GetStaffWageMultiplier() const { return 1; }
    uint8_t Session::GetGroupGuestKind(EntityId) const { return 0; }
    bool Session::IsStaffDetained(EntityId) const { return false; }
    bool Session::IsKarenConfrontingStaff(EntityId) const { return false; }
    bool Session::StonerMayStare(EntityId) const { return true; }
    void Session::NoteStonerStareStarted(EntityId) {}
    std::string Session::ExportParkStorage() const { return {}; }
    bool Session::RestoreParkStorage(std::string_view, std::string& error) { error = _impl->error; return false; }
    bool Session::WatchParticipant(ParticipantId, std::string& error) { error = _impl->error; return false; }
    bool Session::Forfeit(ParticipantId, std::string& error) { error = _impl->error; return false; }
    bool Session::CloseEarly(std::string& error) { error = _impl->error; return false; }
    void Session::SendChat(std::string_view) {}
    Session& GetSession() { static Session session; return session; }
    void Update() {}
    ScenarioIdentity GetCurrentScenarioIdentity() { return {}; }
    ScenarioIdentity GetScenarioIdentityForPath(const std::string&) { return {}; }
    ParkMetrics CollectParkMetrics() { return {}; }
    void UpdateGuestGenerationInterference() {}
    bool ConsumeGuestArrivalCancellation() { return false; }
    void OnGuestPurchase(OpenRCT2::Guest&, RideId, bool) {}
    void OnVandalAttempt(OpenRCT2::Guest&, bool) {}
    void OnGuestExitRide(OpenRCT2::Guest&, RideId) {}
    bool IsProtectedAgent(EntityId) { return false; }
    bool IsProtectedRide(RideId) { return false; }
    int32_t GetStaffWageMultiplier() { return 1; }
    uint8_t GetGroupGuestKind(EntityId) { return 0; }
    bool IsStaffDetained(EntityId) { return false; }
    bool IsKarenConfrontingStaff(EntityId) { return false; }
    bool StonerMayStare(EntityId) { return true; }
    void NoteStonerStareStarted(EntityId) {}
    std::string ExportParkStorage() { return {}; }
    bool RestoreParkStorage(std::string_view, std::string& error) { error = "Networking is disabled in this build."; return false; }
    bool IsWatchingPark() { return false; }
    bool ReturnFromWatchedPark(std::string& error) { error = "Networking is disabled in this build."; return false; }
#endif
} // namespace OpenRCT2::Competitive
