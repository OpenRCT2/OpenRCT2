/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    #include "../entity/Guest.h"
    #include "../localisation/Localisation.Date.h"
    #include "../management/Finance.h"
    #include "../platform/Platform.h"
    #include "../world/Map.h"
    #include "../world/Park.h"
    #include "Socket.h"
    #include "network.h"

    #include <chrono>
    #include <cstring>
    #include <iterator>
    #include <memory>
    #include <random>
    #include <string>

using namespace OpenRCT2;

enum class MasterServerStatus
{
    Ok = 200,
    InvalidToken = 401,
    ServerNotFound = 404,
    InternalError = 500
};

    #ifndef DISABLE_HTTP
using namespace std::chrono_literals;
constexpr int32_t kMasterServerRegisterTime = std::chrono::milliseconds(2min).count();
constexpr int32_t kMasterServerHeartbeatTime = std::chrono::milliseconds(1min).count();
    #endif

class NetworkServerAdvertiser final : public INetworkServerAdvertiser
{
private:
    uint16_t _port;

    std::unique_ptr<IUdpSocket> _lanListener;
    uint32_t _lastListenTime{};

    ADVERTISE_STATUS _status = ADVERTISE_STATUS::UNREGISTERED;

    #ifndef DISABLE_HTTP
    uint32_t _lastAdvertiseTime = 0;
    uint32_t _lastHeartbeatTime = 0;

    // Our unique token for this server
    std::string _token;

    // Key received from the master server
    std::string _key;

    // See https://github.com/OpenRCT2/OpenRCT2/issues/6277 and 4953
    bool _forceIPv4 = false;
    #endif

public:
    explicit NetworkServerAdvertiser(uint16_t port)
    {
        _port = port;
        _lanListener = CreateUdpSocket();
    #ifndef DISABLE_HTTP
        _key = GenerateAdvertiseKey();
    #endif
    }

    ADVERTISE_STATUS GetStatus() const override
    {
        return _status;
    }

    void Update() override
    {
        UpdateLAN();
    #ifndef DISABLE_HTTP
        if (Config::Get().network.Advertise)
        {
            UpdateWAN();
        }
    #endif
    }

private:
    void UpdateLAN()
    {
        auto ticks = Platform::GetTicks();
        if (ticks > _lastListenTime + 500)
        {
            if (_lanListener->GetStatus() != SocketStatus::Listening)
            {
                _lanListener->Listen(kNetworkLanBroadcastPort);
            }
            else
            {
                char buffer[256]{};
                size_t recievedBytes{};
                std::unique_ptr<INetworkEndpoint> endpoint;
                auto p = _lanListener->ReceiveData(buffer, sizeof(buffer) - 1, &recievedBytes, &endpoint);
                if (p == NetworkReadPacket::Success)
                {
                    std::string sender = endpoint->GetHostname();
                    LOG_VERBOSE("Received %zu bytes from %s on LAN broadcast port", recievedBytes, sender.c_str());
                    if (String::equals(buffer, kNetworkLanBroadcastMsg))
                    {
                        auto body = GetBroadcastJson();
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

    json_t GetBroadcastJson()
    {
        json_t root = NetworkGetServerInfoAsJson();
        root["port"] = _port;
        return root;
    }

    #ifndef DISABLE_HTTP
    void UpdateWAN()
    {
        switch (_status)
        {
            case ADVERTISE_STATUS::UNREGISTERED:
                if (_lastAdvertiseTime == 0 || Platform::GetTicks() > _lastAdvertiseTime + kMasterServerRegisterTime)
                {
                    if (_lastAdvertiseTime == 0)
                    {
                        Console::WriteLine("Registering server on master server");
                    }
                    SendRegistration(_forceIPv4);
                }
                break;
            case ADVERTISE_STATUS::REGISTERED:
                if (Platform::GetTicks() > _lastHeartbeatTime + kMasterServerHeartbeatTime)
                {
                    SendHeartbeat();
                }
                break;
            // exhaust enum values to satisfy clang
            case ADVERTISE_STATUS::DISABLED:
                break;
        }
    }

    void SendRegistration(bool forceIPv4)
    {
        _lastAdvertiseTime = Platform::GetTicks();

        // Send the registration request
        Http::Request request;
        request.url = GetMasterServerUrl();
        request.method = Http::Method::POST;
        request.forceIPv4 = forceIPv4;

        json_t body = {
            { "key", _key },
            { "port", _port },
        };

        if (!Config::Get().network.AdvertiseAddress.empty())
        {
            body["address"] = Config::Get().network.AdvertiseAddress;
        }

        request.body = body.dump();
        request.header["Content-Type"] = "application/json";

        Http::DoAsync(request, [&](Http::Response response) -> void {
            if (response.status != Http::Status::Ok)
            {
                Console::Error::WriteLine("Unable to connect to master server");
                return;
            }

            json_t root = Json::FromString(response.body);
            root = Json::AsObject(root);
            this->OnRegistrationResponse(root);
        });
    }

    void SendHeartbeat()
    {
        Http::Request request;
        request.url = GetMasterServerUrl();
        request.method = Http::Method::PUT;

        json_t body = GetHeartbeatJson();
        request.body = body.dump();
        request.header["Content-Type"] = "application/json";

        _lastHeartbeatTime = Platform::GetTicks();
        Http::DoAsync(request, [&](Http::Response response) -> void {
            if (response.status != Http::Status::Ok)
            {
                Console::Error::WriteLine("Unable to connect to master server");
                return;
            }

            json_t root = Json::FromString(response.body);
            root = Json::AsObject(root);
            this->OnHeartbeatResponse(root);
        });
    }

    /**
     * @param jsonRoot must be of JSON type object or null
     * @note jsonRoot is deliberately left non-const: json_t behaviour changes when const
     */
    void OnRegistrationResponse(json_t& jsonRoot)
    {
        Guard::Assert(jsonRoot.is_object(), "OnRegistrationResponse expects parameter jsonRoot to be object");

        auto status = Json::GetEnum<MasterServerStatus>(jsonRoot["status"], MasterServerStatus::InternalError);

        if (status == MasterServerStatus::Ok)
        {
            Console::WriteLine("Server successfully registered on master server");
            json_t jsonToken = jsonRoot["token"];
            if (jsonToken.is_string())
            {
                _token = Json::GetString(jsonToken);
                _status = ADVERTISE_STATUS::REGISTERED;
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
                "Unable to advertise (%d): %s\n  * Check that you have port forwarded %u\n  * Try setting "
                "advertise_address in config.ini",
                status, message.c_str(), _port);

            // Hack for https://github.com/OpenRCT2/OpenRCT2/issues/6277
            // Master server may not reply correctly if using IPv6, retry forcing IPv4,
            // don't wait the full timeout.
            if (!_forceIPv4 && status == MasterServerStatus::InternalError)
            {
                _forceIPv4 = true;
                _lastAdvertiseTime = 0;
                LOG_INFO("Forcing HTTP(S) over IPv4");
            }
        }
    }

    /**
     * @param jsonRoot must be of JSON type object or null
     * @note jsonRoot is deliberately left non-const: json_t behaviour changes when const
     */
    void OnHeartbeatResponse(json_t& jsonRoot)
    {
        Guard::Assert(jsonRoot.is_object(), "OnHeartbeatResponse expects parameter jsonRoot to be object");

        auto status = Json::GetEnum<MasterServerStatus>(jsonRoot["status"], MasterServerStatus::InternalError);
        if (status == MasterServerStatus::Ok)
        {
            // Master server has successfully updated our server status
        }
        else if (status == MasterServerStatus::InvalidToken)
        {
            _status = ADVERTISE_STATUS::UNREGISTERED;
            _lastAdvertiseTime = 0;
            Console::Error::WriteLine("Master server heartbeat failed: Invalid Token");
        }
    }

    json_t GetHeartbeatJson()
    {
        uint32_t numPlayers = NetworkGetNumVisiblePlayers();

        json_t root = {
            { "token", _token },
            { "players", numPlayers },
        };

        const auto& gameState = GetGameState();
        const auto& date = GetDate();
        json_t mapSize = { { "x", gameState.MapSize.x - 2 }, { "y", gameState.MapSize.y - 2 } };
        json_t gameInfo = {
            { "mapSize", mapSize },
            { "day", date.GetMonthTicks() },
            { "month", date.GetMonthsElapsed() },
            { "guests", gameState.NumGuestsInPark },
            { "parkValue", gameState.Park.Value },
        };

        if (!(gameState.Park.Flags & PARK_FLAGS_NO_MONEY))
        {
            gameInfo["cash"] = gameState.Cash;
        }

        root["gameInfo"] = gameInfo;

        return root;
    }

    static std::string GenerateAdvertiseKey()
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

    static std::string GetMasterServerUrl()
    {
        std::string result = kMasterServerURL;
        if (!Config::Get().network.MasterServerUrl.empty())
        {
            result = Config::Get().network.MasterServerUrl;
        }
        return result;
    }
    #endif
};

std::unique_ptr<INetworkServerAdvertiser> CreateServerAdvertiser(uint16_t port)
{
    return std::make_unique<NetworkServerAdvertiser>(port);
}

#endif // DISABLE_NETWORK
