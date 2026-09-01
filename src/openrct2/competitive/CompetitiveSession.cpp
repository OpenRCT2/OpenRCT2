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

#include "../Context.h"
#include "../Date.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../config/Config.h"
#include "../core/Crypt.h"
#include "../core/File.h"
#include "../core/Json.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../entity/EntityList.h"
#include "../entity/Guest.h"
#include "../network/Network.h"
#include "../network/NetworkConnection.h"
#include "../network/NetworkServerAdvertiser.h"
#include "../network/Socket.h"
#include "../platform/Platform.h"
#include "../ride/RideManager.hpp"
#include "../scenario/ScenarioRepository.h"

#include <algorithm>
#include <cstdio>
#include <list>
#include <random>

namespace OpenRCT2::Competitive
{
#ifndef DISABLE_NETWORK
    namespace
    {
        constexpr uint32_t kHeartbeatIntervalMs = 5000;
        constexpr uint32_t kReconnectIntervalMs = 3000;

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

        const AbilityRule& GetAbilityRule(const MatchRules& rules, Ability ability)
        {
            switch (ability)
            {
                case Ability::misinformation:
                    return rules.misinformation;
                case Ability::poison:
                    return rules.poison;
                case Ability::vandal:
                    return rules.vandal;
            }
            return rules.vandal;
        }

        std::string MetricName(Metric metric)
        {
            switch (metric)
            {
                case Metric::rating:
                    return "park rating";
                case Metric::guests:
                    return "guests";
                case Metric::competitiveCash:
                    return "competitive cash";
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
        JoinConfiguration joinConfiguration{};
        bool helloSent = false;
        bool scenarioLoaded = false;
        uint32_t lastHeartbeatAt{};
        uint32_t nextReconnectAt{};
        uint32_t lastReportedDay{};
        bool forcedPause = false;
        bool wasPausedBeforeForcedPause = false;

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

        void SendError(Network::Connection& connection, const std::string& message)
        {
            QueueJson(connection, Network::Command::competitiveError, { { "message", message } });
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
                    state.scores.push_back({ .participantId = participant.id, .competitiveCash = state.rules.economy.startingCash });
                    state.reports.push_back({ .participantId = participant.id });
                }
            }

            peer.lastSeenAt = Platform::GetTicks();
            QueueJson(
                *peer.connection, Network::Command::competitiveWelcome,
                { { "participantId", peer.participantId }, { "state", ToJson(state) } });
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
            ApplyEconomyDelta(*score, report->acceptedEconomy, metrics, state.rules.economy);

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
                SendError(*peer.connection, "The lobby is no longer accepting ready changes.");
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
                SendError(*peer.connection, "The park does not match the lobby's scenario and starting date.");
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
            SendSnapshot();
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
                case Network::Command::competitiveHeartbeat:
                    QueueJson(*peer.connection, Network::Command::competitiveHeartbeat, { { "reply", true } });
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
                const auto metrics = CollectParkMetrics();
                if (metrics.localDay != lastReportedDay)
                {
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
            QueueJson(
                *serverConnection, Network::Command::competitiveHello,
                {
                    { "protocol", kProtocolVersion },
                    { "role", joinConfiguration.role },
                    { "identityKey", Config::Get().network.competitiveIdentity },
                    { "name", joinConfiguration.playerName },
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
                    status = ConnectionStatus::online;
                    if (!scenarioLoaded && !LoadRequiredScenario())
                    {
                        serverConnection->disconnect();
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
                case Network::Command::competitiveError:
                {
                    const auto body = ReadJson(packet);
                    if (body.has_value())
                    {
                        SetError(body->value("message", "The competitive host rejected the request."));
                    }
                    break;
                }
                case Network::Command::competitiveHeartbeat:
                    break;
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
                {
                    status = ConnectionStatus::reconnecting;
                }
                nextReconnectAt = now + kReconnectIntervalMs;
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
                {
                    status = ConnectionStatus::reconnecting;
                }
                nextReconnectAt = now + kReconnectIntervalMs;
                if (auto* participant = FindParticipant(state, localParticipantId))
                {
                    participant->online = false;
                }
                return;
            }

            if (status == ConnectionStatus::online && localRole == Role::player && scenarioLoaded
                && state.phase == Phase::running && gLegacyScene == LegacyScene::playing)
            {
                const auto metrics = CollectParkMetrics();
                if (metrics.localDay != lastReportedDay)
                {
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
        _impl->state.scores.push_back(
            { .participantId = _impl->localParticipantId,
              .competitiveCash = configuration.rules.economy.startingCash });
        _impl->state.reports.push_back({ _impl->localParticipantId, metrics, {}, metrics.localDay });
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
        Config::Save();
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
        _impl->ClearTransport();
        _impl->mode = SessionMode::none;
        _impl->status = ConnectionStatus::disconnected;
        _impl->state = {};
        _impl->localParticipantId = kInvalidParticipantId;
        _impl->localRole = Role::none;
        _impl->lastError.clear();
        _impl->scenarioLoaded = false;
        _impl->EnsurePauseState();
    }

    void Session::Update()
    {
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
        _impl->EnsurePauseState();
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
        {
            report.acceptedEconomy = {
                report.metrics.arrivalsGenerated,
                report.metrics.constructionSpend,
                report.metrics.rideCustomers,
                report.metrics.stallCustomers,
            };
            report.lastScoredDay = report.metrics.localDay;
        }
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
            score = { .participantId = score.participantId, .competitiveCash = rules.economy.startingCash };
        }
        _impl->SendSnapshot();
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

        uint64_t rideCustomers = 0;
        uint64_t stallCustomers = 0;
        for (const auto& ride : RideManager(gameState))
        {
            if (ride.getClassification() == RideClassification::shopOrStall)
                stallCustomers += ride.totalCustomers;
            else if (ride.getClassification() == RideClassification::ride)
                rideCustomers += ride.totalCustomers;
        }

        return {
            static_cast<uint16_t>(date.GetYear() + 1),
            date.GetMonthsElapsed(),
            date.GetMonthsElapsed() * 32 + static_cast<uint32_t>(date.GetDay()) + 1,
            gameState.park.rating,
            gameState.park.numGuestsInPark,
            gameState.park.value,
            static_cast<uint8_t>(guestCount == 0 ? 0 : happinessTotal / guestCount),
            gameState.park.totalAdmissions,
            0,
            rideCustomers,
            stallCustomers,
        };
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
    bool Session::Forfeit(ParticipantId, std::string& error) { error = _impl->error; return false; }
    bool Session::CloseEarly(std::string& error) { error = _impl->error; return false; }
    Session& GetSession() { static Session session; return session; }
    void Update() {}
    ScenarioIdentity GetCurrentScenarioIdentity() { return {}; }
    ScenarioIdentity GetScenarioIdentityForPath(const std::string&) { return {}; }
    ParkMetrics CollectParkMetrics() { return {}; }
#endif
} // namespace OpenRCT2::Competitive
