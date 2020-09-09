/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#    include "NetworkServerAdvertiser.h"

#    include "../config/Config.h"
#    include "../core/Console.hpp"
#    include "../core/Http.h"
#    include "../core/Json.hpp"
#    include "../core/String.hpp"
#    include "../localisation/Date.h"
#    include "../management/Finance.h"
#    include "../peep/Peep.h"
#    include "../platform/Platform2.h"
#    include "../platform/platform.h"
#    include "../util/Util.h"
#    include "../world/Map.h"
#    include "../world/Park.h"
#    include "Socket.h"
#    include "network.h"

#    include <cstring>
#    include <iterator>
#    include <memory>
#    include <random>
#    include <string>

enum MASTER_SERVER_STATUS
{
    MASTER_SERVER_STATUS_OK = 200,
    MASTER_SERVER_STATUS_INVALID_TOKEN = 401,
    MASTER_SERVER_STATUS_SERVER_NOT_FOUND = 404,
    MASTER_SERVER_STATUS_INTERNAL_ERROR = 500
};

#    ifndef DISABLE_HTTP
constexpr int32_t MASTER_SERVER_REGISTER_TIME = 120 * 1000; // 2 minutes
constexpr int32_t MASTER_SERVER_HEARTBEAT_TIME = 60 * 1000; // 1 minute
#    endif

class NetworkServerAdvertiser final : public INetworkServerAdvertiser
{
private:
    uint16_t _port;

    std::unique_ptr<IUdpSocket> _lanListener;
    uint32_t _lastListenTime{};

    ADVERTISE_STATUS _status = ADVERTISE_STATUS::UNREGISTERED;

#    ifndef DISABLE_HTTP
    uint32_t _lastAdvertiseTime = 0;
    uint32_t _lastHeartbeatTime = 0;

    // Our unique token for this server
    std::string _token;

    // Key received from the master server
    std::string _key;

    // See https://github.com/OpenRCT2/OpenRCT2/issues/6277 and 4953
    bool _forceIPv4 = false;
#    endif

public:
    explicit NetworkServerAdvertiser(uint16_t port)
    {
        _port = port;
        _lanListener = CreateUdpSocket();
#    ifndef DISABLE_HTTP
        _key = GenerateAdvertiseKey();
#    endif
    }

    ADVERTISE_STATUS GetStatus() const override
    {
        return _status;
    }

    void Update() override
    {
        UpdateLAN();
#    ifndef DISABLE_HTTP
        if (gConfigNetwork.advertise)
        {
            UpdateWAN();
        }
#    endif
    }

private:
    void UpdateLAN()
    {
        auto ticks = Platform::GetTicks();
        if (ticks > _lastListenTime + 500)
        {
            if (_lanListener->GetStatus() != SOCKET_STATUS_LISTENING)
            {
                _lanListener->Listen(NETWORK_LAN_BROADCAST_PORT);
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
                    log_verbose("Received %zu bytes from %s on LAN broadcast port", recievedBytes, sender.c_str());
                    if (String::Equals(buffer, NETWORK_LAN_BROADCAST_MSG))
                    {
                        auto body = GetBroadcastJson();
                        auto bodyDump = json_dumps(body, JSON_COMPACT);
                        size_t sendLen = strlen(bodyDump) + 1;
                        log_verbose("Sending %zu bytes back to %s", sendLen, sender.c_str());
                        _lanListener->SendData(*endpoint, bodyDump, sendLen);
                        free(bodyDump);
                        json_decref(body);
                    }
                }
            }
            _lastListenTime = ticks;
        }
    }

    json_t* GetBroadcastJson()
    {
        auto root = network_get_server_info_as_json();
        json_object_set(root, "port", json_integer(_port));
        return root;
    }

#    ifndef DISABLE_HTTP
    void UpdateWAN()
    {
        switch (_status)
        {
            case ADVERTISE_STATUS::UNREGISTERED:
                if (_lastAdvertiseTime == 0 || platform_get_ticks() > _lastAdvertiseTime + MASTER_SERVER_REGISTER_TIME)
                {
                    SendRegistration(_forceIPv4);
                }
                break;
            case ADVERTISE_STATUS::REGISTERED:
                if (platform_get_ticks() > _lastHeartbeatTime + MASTER_SERVER_HEARTBEAT_TIME)
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
        _lastAdvertiseTime = platform_get_ticks();

        // Send the registration request
        Http::Request request;
        request.url = GetMasterServerUrl();
        request.method = Http::Method::POST;
        request.forceIPv4 = forceIPv4;

        json_t* body = json_object();
        json_object_set_new(body, "key", json_string(_key.c_str()));
        json_object_set_new(body, "port", json_integer(_port));

        if (!gConfigNetwork.advertise_address.empty())
        {
            json_object_set_new(body, "address", json_string(gConfigNetwork.advertise_address.c_str()));
        }

        char* bodyDump = json_dumps(body, JSON_COMPACT);
        request.body = bodyDump;
        request.header["Content-Type"] = "application/json";
        free(bodyDump);
        json_decref(body);

        Http::DoAsync(request, [&](Http::Response response) -> void {
            if (response.status != Http::Status::OK)
            {
                Console::WriteLine("Unable to connect to master server");
                return;
            }

            json_t* root = Json::FromString(response.body);
            this->OnRegistrationResponse(root);
            json_decref(root);
        });
    }

    void SendHeartbeat()
    {
        Http::Request request;
        request.url = GetMasterServerUrl();
        request.method = Http::Method::PUT;

        json_t* body = GetHeartbeatJson();
        char* bodyDump = json_dumps(body, JSON_COMPACT);
        request.body = bodyDump;
        request.header["Content-Type"] = "application/json";
        free(bodyDump);
        json_decref(body);

        _lastHeartbeatTime = platform_get_ticks();
        Http::DoAsync(request, [&](Http::Response response) -> void {
            if (response.status != Http::Status::OK)
            {
                Console::WriteLine("Unable to connect to master server");
                return;
            }

            json_t* root = Json::FromString(response.body);
            this->OnHeartbeatResponse(root);
            json_decref(root);
        });
    }

    void OnRegistrationResponse(json_t* jsonRoot)
    {
        json_t* jsonStatus = json_object_get(jsonRoot, "status");
        if (json_is_integer(jsonStatus))
        {
            int32_t status = static_cast<int32_t>(json_integer_value(jsonStatus));
            if (status == MASTER_SERVER_STATUS_OK)
            {
                json_t* jsonToken = json_object_get(jsonRoot, "token");
                if (json_is_string(jsonToken))
                {
                    _token = std::string(json_string_value(jsonToken));
                    _status = ADVERTISE_STATUS::REGISTERED;
                }
            }
            else
            {
                const char* message = "Invalid response from server";
                json_t* jsonMessage = json_object_get(jsonRoot, "message");
                if (json_is_string(jsonMessage))
                {
                    message = json_string_value(jsonMessage);
                }
                Console::Error::WriteLine("Unable to advertise (%d): %s", status, message);
                // Hack for https://github.com/OpenRCT2/OpenRCT2/issues/6277
                // Master server may not reply correctly if using IPv6, retry forcing IPv4,
                // don't wait the full timeout.
                if (!_forceIPv4 && status == 500)
                {
                    _forceIPv4 = true;
                    _lastAdvertiseTime = 0;
                    log_info("Retry with ipv4 only");
                }
            }
        }
    }

    void OnHeartbeatResponse(json_t* jsonRoot)
    {
        json_t* jsonStatus = json_object_get(jsonRoot, "status");
        if (json_is_integer(jsonStatus))
        {
            int32_t status = static_cast<int32_t>(json_integer_value(jsonStatus));
            if (status == MASTER_SERVER_STATUS_OK)
            {
                // Master server has successfully updated our server status
            }
            else if (status == MASTER_SERVER_STATUS_INVALID_TOKEN)
            {
                _status = ADVERTISE_STATUS::UNREGISTERED;
                Console::WriteLine("Master server heartbeat failed: Invalid Token");
            }
        }
    }

    json_t* GetHeartbeatJson()
    {
        uint32_t numPlayers = network_get_num_players();

        json_t* root = json_object();
        json_object_set_new(root, "token", json_string(_token.c_str()));
        json_object_set_new(root, "players", json_integer(numPlayers));

        json_t* gameInfo = json_object();
        json_object_set_new(gameInfo, "mapSize", json_integer(gMapSize - 2));
        json_object_set_new(gameInfo, "day", json_integer(gDateMonthTicks));
        json_object_set_new(gameInfo, "month", json_integer(gDateMonthsElapsed));
        json_object_set_new(gameInfo, "guests", json_integer(gNumGuestsInPark));
        json_object_set_new(gameInfo, "parkValue", json_integer(gParkValue));
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            json_object_set_new(gameInfo, "cash", json_integer(gCash));
        }
        json_object_set_new(root, "gameInfo", gameInfo);

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
        std::string result = OPENRCT2_MASTER_SERVER_URL;
        if (!gConfigNetwork.master_server_url.empty())
        {
            result = gConfigNetwork.master_server_url;
        }
        return result;
    }
#    endif
};

std::unique_ptr<INetworkServerAdvertiser> CreateServerAdvertiser(uint16_t port)
{
    return std::make_unique<NetworkServerAdvertiser>(port);
}

#endif // DISABLE_NETWORK
