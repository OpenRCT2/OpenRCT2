/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "NetworkServerAdvertiser.h"

    #include "../Diagnostic.h"
    #include "../GameState.h"
    #include "../config/Config.h"
    #include "../core/Console.hpp"
    #include "../core/Guard.hpp"
    #include "../core/Http.h"
    #include "../core/Json.hpp"
    #include "../core/String.hpp"
    #include "../platform/Platform.h"
    #include "Network.h"
    #include "Socket.h"

    #include <chrono>
    #include <cstring>
    #include <iterator>
    #include <memory>
    #include <mutex>
    #include <random>
    #include <string>
    #include <vector>

namespace OpenRCT2::Network
{
    enum class MasterServerStatus
    {
        ok = 200,
        invalidToken = 401,
        serverNotFound = 404,
        internalError = 500
    };

    #ifndef DISABLE_HTTP
    using namespace std::chrono_literals;
    constexpr int32_t kMasterServerRegisterTime = std::chrono::milliseconds(30s).count();
    constexpr int32_t kMasterServerHeartbeatTime = std::chrono::milliseconds(1min).count();
    #endif

    class NetworkServerAdvertiser final : public INetworkServerAdvertiser
    {
    private:
        // Shared between the advertiser and its in-flight HTTP completion lambdas. A lambda holds a
        // copy, locks the mutex, and bails when 'self' is null - so a completion running on an Http
        // worker thread can never touch a destroyed advertiser, and beginShutdown() cannot return
        // while a completion is mid-flight.
        struct AdvertiserGuard
        {
            std::mutex mutex;
            NetworkServerAdvertiser* self = nullptr;
        };
        std::shared_ptr<AdvertiserGuard> _guard = std::make_shared<AdvertiserGuard>();

        uint16_t _port;
        std::function<json_t()> _serverInfoProvider;
        std::function<uint32_t()> _playerCountProvider;
        std::function<json_t()> _gameInfoProvider;
        std::function<bool()> _wanEnabledProvider;

        std::unique_ptr<IUdpSocket> _lanListener;
        uint32_t _lastListenTime{};

    #ifndef DISABLE_HTTP
        // One independent registration/heartbeat cycle per master server. The official server is
        // always present here so advertising keeps working exactly as before; a configured
        // masterServerUrl is an ADDITIONAL entry, not a replacement - see docs/competitive/README.md
        // for why (the official server silently drops the custom fields a competitive listing needs).
        struct WanEndpoint
        {
            std::string url;
            AdvertiseStatus status = AdvertiseStatus::unregistered;
            uint32_t lastAdvertiseTime = 0;
            uint32_t lastHeartbeatTime = 0;
            std::string token;
            std::string key = generateAdvertiseKey();
            bool forceIPv4 = false;
            std::shared_future<void> currentRequest;
        };
        std::vector<WanEndpoint> _wanEndpoints;
    #endif

    public:
        explicit NetworkServerAdvertiser(
            uint16_t port, std::function<json_t()> serverInfoProvider = {},
            std::function<uint32_t()> playerCountProvider = {}, std::function<json_t()> gameInfoProvider = {},
            std::function<bool()> wanEnabledProvider = {})
        {
            _guard->self = this;
            _port = port;
            _serverInfoProvider = std::move(serverInfoProvider);
            _playerCountProvider = std::move(playerCountProvider);
            _gameInfoProvider = std::move(gameInfoProvider);
            _wanEnabledProvider = std::move(wanEnabledProvider);
            _lanListener = CreateUdpSocket();
    #ifndef DISABLE_HTTP
            _wanEndpoints.push_back(WanEndpoint{ .url = kMasterServerURL });
            const auto& customUrl = Config::Get().network.masterServerUrl;
            if (!customUrl.empty() && customUrl != kMasterServerURL)
            {
                _wanEndpoints.push_back(WanEndpoint{ .url = customUrl });
            }
    #endif
        }

        void beginShutdown() override
        {
            _lanListener->Close();
            {
                std::scoped_lock lock(_guard->mutex);
                _guard->self = nullptr;
            }
            // Hand any in-flight requests to the process-lifetime reaper so this never blocks.
    #ifndef DISABLE_HTTP
            for (auto& endpoint : _wanEndpoints)
                Http::Detach(std::move(endpoint.currentRequest));
    #endif
        }

        ~NetworkServerAdvertiser() final
        {
            beginShutdown(); // idempotent: currentRequest futures are moved-from (invalid) by now
        }

        AdvertiseStatus getStatus() const override
        {
    #ifndef DISABLE_HTTP
            // Report the best status across every endpoint: registered on any one of them is enough
            // for callers that just want to know "are we visible somewhere".
            AdvertiseStatus best = AdvertiseStatus::disabled;
            for (const auto& endpoint : _wanEndpoints)
            {
                if (endpoint.status == AdvertiseStatus::registered)
                    return AdvertiseStatus::registered;
                if (endpoint.status == AdvertiseStatus::registering && best != AdvertiseStatus::registered)
                    best = AdvertiseStatus::registering;
                else if (best == AdvertiseStatus::disabled)
                    best = endpoint.status;
            }
            return best;
    #else
            return AdvertiseStatus::disabled;
    #endif
        }

        void update() override
        {
            updateLAN();
    #ifndef DISABLE_HTTP
            const bool wanEnabled = _wanEnabledProvider ? _wanEnabledProvider() : Config::Get().network.advertise;
            if (wanEnabled)
            {
                for (auto& endpoint : _wanEndpoints)
                    updateWAN(endpoint);
            }
    #endif
        }

    private:
        void updateLAN()
        {
            auto ticks = Platform::GetTicks();
            if (ticks > _lastListenTime + 500)
            {
                if (_lanListener->GetStatus() != SocketStatus::listening)
                {
                    _lanListener->Listen(kLanBroadcastPort);
                }
                else
                {
                    char buffer[256]{};
                    size_t receivedBytes{};
                    std::unique_ptr<INetworkEndpoint> endpoint;
                    auto p = _lanListener->ReceiveData(buffer, sizeof(buffer) - 1, &receivedBytes, &endpoint);
                    if (p == ReadPacket::success)
                    {
                        std::string sender = endpoint->GetHostname();
                        LOG_VERBOSE("Received %zu bytes from %s on LAN broadcast port", receivedBytes, sender.c_str());
                        if (String::equals(buffer, kLanBroadcastMsg))
                        {
                            auto body = getBroadcastJson();
                            auto bodyDump = body.dump();
                            size_t sendLen = bodyDump.size() + 1;
                            LOG_VERBOSE("Sending %zu bytes back to %s", sendLen, sender.c_str());
                            _lanListener->SendData(*endpoint, bodyDump.c_str(), sendLen);
                        }
                    }
                }
                _lastListenTime = ticks;
            }
        }

        json_t getBroadcastJson()
        {
            json_t root = _serverInfoProvider ? _serverInfoProvider() : GetServerInfoAsJson();
            root["port"] = _port;
            return root;
        }

    #ifndef DISABLE_HTTP
        void updateWAN(WanEndpoint& endpoint)
        {
            switch (endpoint.status)
            {
                case AdvertiseStatus::unregistered:
                    if (endpoint.lastAdvertiseTime == 0
                        || Platform::GetTicks() > endpoint.lastAdvertiseTime + kMasterServerRegisterTime)
                    {
                        Console::WriteLine("Registering server on master server (%s)...", endpoint.url.c_str());
                        sendRegistration(endpoint);
                    }
                    break;
                case AdvertiseStatus::registering:
                    // Waiting for registration response.
                    break;
                case AdvertiseStatus::registered:
                    if (Platform::GetTicks() > endpoint.lastHeartbeatTime + kMasterServerHeartbeatTime)
                    {
                        sendHeartbeat(endpoint);
                    }
                    break;
                // exhaust enum values to satisfy clang
                case AdvertiseStatus::disabled:
                    break;
            }
        }

        void sendRegistration(WanEndpoint& endpoint)
        {
            endpoint.lastAdvertiseTime = Platform::GetTicks();
            endpoint.status = AdvertiseStatus::registering;

            // Send the registration request
            Http::Request request;
            request.url = endpoint.url;
            request.method = Http::Method::post;
            request.forceIPv4 = endpoint.forceIPv4;

            json_t body = {
                { "key", endpoint.key },
                { "port", _port },
            };

            if (!Config::Get().network.advertiseAddress.empty())
            {
                body["address"] = Config::Get().network.advertiseAddress;
            }

            request.body = body.dump();
            request.header["Content-Type"] = "application/json";

            endpoint.currentRequest = Http::DoAsync(
                                           request,
                                           [guard = _guard, url = endpoint.url](Http::Response response) -> void {
                                               std::scoped_lock lock(guard->mutex);
                                               auto* self = guard->self;
                                               if (self == nullptr)
                                                   return;
                                               auto* ep = self->findEndpoint(url);
                                               if (ep == nullptr)
                                                   return;

                                               if (response.status != Http::Status::ok)
                                               {
                                                   Console::Error::WriteLine(
                                                       "Unable to connect to master server (%s), retrying in %d seconds",
                                                       url.c_str(), kMasterServerRegisterTime / 1000);

                                                   ep->status = AdvertiseStatus::unregistered;
                                                   return;
                                               }

                                               json_t root = Json::FromString(response.body);
                                               root = Json::AsObject(root);
                                               self->onRegistrationResponse(*ep, root);
                                           })
                                           .share();
        }

        void sendHeartbeat(WanEndpoint& endpoint)
        {
            Http::Request request;
            request.url = endpoint.url;
            request.method = Http::Method::put;

            json_t body = getHeartbeatJson(endpoint);
            request.body = body.dump();
            request.header["Content-Type"] = "application/json";

            endpoint.lastHeartbeatTime = Platform::GetTicks();

            endpoint.currentRequest = Http::DoAsync(
                                           request,
                                           [guard = _guard, url = endpoint.url](Http::Response response) -> void {
                                               std::scoped_lock lock(guard->mutex);
                                               auto* self = guard->self;
                                               if (self == nullptr)
                                                   return;
                                               auto* ep = self->findEndpoint(url);
                                               if (ep == nullptr)
                                                   return;

                                               if (response.status != Http::Status::ok)
                                               {
                                                   Console::Error::WriteLine(
                                                       "Unable to connect to master server (%s), retrying in %d seconds",
                                                       url.c_str(), kMasterServerRegisterTime / 1000);

                                                   ep->status = AdvertiseStatus::unregistered;
                                                   // Don't immediately retry advertising, wait for kMasterServerRegisterTime.
                                                   ep->lastAdvertiseTime = Platform::GetTicks();
                                                   return;
                                               }

                                               json_t root = Json::FromString(response.body);
                                               root = Json::AsObject(root);
                                               self->onHeartbeatResponse(*ep, root);
                                           })
                                           .share();
        }

        WanEndpoint* findEndpoint(const std::string& url)
        {
            for (auto& endpoint : _wanEndpoints)
            {
                if (endpoint.url == url)
                    return &endpoint;
            }
            return nullptr;
        }

        /**
         * @param jsonRoot must be of JSON type object or null
         * @note jsonRoot is deliberately left non-const: json_t behaviour changes when const
         */
        void onRegistrationResponse(WanEndpoint& endpoint, json_t& jsonRoot)
        {
            Guard::Assert(jsonRoot.is_object(), "onRegistrationResponse expects parameter jsonRoot to be object");

            auto status = Json::GetEnum<MasterServerStatus>(jsonRoot["status"], MasterServerStatus::internalError);

            if (status == MasterServerStatus::ok)
            {
                Console::WriteLine("Server successfully registered on master server (%s)", endpoint.url.c_str());
                json_t jsonToken = jsonRoot["token"];
                if (jsonToken.is_string())
                {
                    endpoint.token = Json::GetString(jsonToken);
                    endpoint.status = AdvertiseStatus::registered;
                }
            }
            else
            {
                std::string message = Json::GetString(jsonRoot["message"]);
                if (message.empty())
                {
                    message = "Invalid response from server";
                }
                Console::Error::WriteLine(
                    "Unable to advertise on %s (%d): %s\n  * Check that you have port forwarded %u\n  * Try setting "
                    "advertise_address in config.ini",
                    endpoint.url.c_str(), status, message.c_str(), _port);

                // Hack for https://github.com/OpenRCT2/OpenRCT2/issues/6277
                // Master server may not reply correctly if using IPv6, retry forcing IPv4,
                // don't wait the full timeout.
                if (!endpoint.forceIPv4 && status == MasterServerStatus::internalError)
                {
                    endpoint.forceIPv4 = true;
                    endpoint.lastAdvertiseTime = 0;
                    LOG_INFO("Forcing HTTP(S) over IPv4 for %s", endpoint.url.c_str());
                }
            }
        }

        /**
         * @param jsonRoot must be of JSON type object or null
         * @note jsonRoot is deliberately left non-const: json_t behaviour changes when const
         */
        void onHeartbeatResponse(WanEndpoint& endpoint, json_t& jsonRoot)
        {
            Guard::Assert(jsonRoot.is_object(), "onHeartbeatResponse expects parameter jsonRoot to be object");

            auto status = Json::GetEnum<MasterServerStatus>(jsonRoot["status"], MasterServerStatus::internalError);
            if (status == MasterServerStatus::ok)
            {
                // Master server has successfully updated our server status
            }
            else if (status == MasterServerStatus::invalidToken)
            {
                endpoint.status = AdvertiseStatus::unregistered;
                endpoint.lastAdvertiseTime = 0;
                Console::Error::WriteLine("Master server heartbeat failed on %s: Invalid Token", endpoint.url.c_str());
            }
        }

        json_t getHeartbeatJson(WanEndpoint& endpoint)
        {
            uint32_t numPlayers = _playerCountProvider ? _playerCountProvider() : GetNumVisiblePlayers();

            json_t root = {
                { "token", endpoint.token },
                { "players", numPlayers },
            };

            if (_gameInfoProvider)
            {
                root["gameInfo"] = _gameInfoProvider();
            }
            else
            {
                const auto& gameState = getGameState();
                const auto& date = GetDate();
                json_t mapSize = { { "x", gameState.mapSize.x - 2 }, { "y", gameState.mapSize.y - 2 } };
                json_t gameInfo = {
                    { "mapSize", mapSize },
                    { "day", date.GetMonthTicks() },
                    { "month", date.GetMonthsElapsed() },
                    { "guests", gameState.park.numGuestsInPark },
                    { "parkValue", gameState.park.value },
                };

                if (!gameState.park.flags.has(ParkFlag::noMoney))
                {
                    gameInfo["cash"] = gameState.park.cash;
                }

                root["gameInfo"] = gameInfo;
            }

            return root;
        }

        static std::string generateAdvertiseKey()
        {
            // Generate a string of 16 random hex characters (64-integer key as a hex formatted string)
            static constexpr char hexChars[] = {
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
            };

            std::random_device rd;
            std::uniform_int_distribution<int32_t> dist(0, static_cast<int32_t>(std::size(hexChars) - 1));

            char key[17];
            for (int32_t i = 0; i < 16; i++)
            {
                int32_t hexCharIndex = dist(rd);
                key[i] = hexChars[hexCharIndex];
            }
            key[std::size(key) - 1] = 0;
            return key;
        }

    #endif
    };

    std::unique_ptr<INetworkServerAdvertiser> CreateServerAdvertiser(uint16_t port)
    {
        return std::make_unique<NetworkServerAdvertiser>(port);
    }

    std::unique_ptr<INetworkServerAdvertiser> CreateServerAdvertiser(
        uint16_t port, std::function<json_t()> serverInfoProvider, std::function<uint32_t()> playerCountProvider,
        std::function<json_t()> gameInfoProvider, std::function<bool()> wanEnabledProvider)
    {
        return std::make_unique<NetworkServerAdvertiser>(
            port, std::move(serverInfoProvider), std::move(playerCountProvider), std::move(gameInfoProvider),
            std::move(wanEnabledProvider));
    }
} // namespace OpenRCT2::Network

#endif // DISABLE_NETWORK
