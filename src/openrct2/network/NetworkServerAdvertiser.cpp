/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#include <string>
#include "../core/Console.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "network.h"
#include "NetworkServerAdvertiser.h"

#include "../config/Config.h"
#include "../localisation/Date.h"
#include "../management/Finance.h"
#include "../peep/Peep.h"
#include "../platform/platform.h"
#include "../util/Util.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "Http.h"

#ifndef DISABLE_HTTP

using namespace OpenRCT2::Network;

enum MASTER_SERVER_STATUS
{
    MASTER_SERVER_STATUS_OK                 = 200,
    MASTER_SERVER_STATUS_INVALID_TOKEN      = 401,
    MASTER_SERVER_STATUS_SERVER_NOT_FOUND   = 404,
    MASTER_SERVER_STATUS_INTERNAL_ERROR     = 500
};

constexpr int32_t MASTER_SERVER_REGISTER_TIME = 120 * 1000; // 2 minutes
constexpr int32_t MASTER_SERVER_HEARTBEAT_TIME = 60 * 1000; // 1 minute

class NetworkServerAdvertiser final : public INetworkServerAdvertiser
{
private:
    uint16_t _port;

    ADVERTISE_STATUS    _status = ADVERTISE_STATUS::UNREGISTERED;
    uint32_t              _lastAdvertiseTime = 0;
    uint32_t              _lastHeartbeatTime = 0;

    // Our unique token for this server
    std::string         _token;

    // Key received from the master server
    std::string         _key;

    // See https://github.com/OpenRCT2/OpenRCT2/issues/6277 and 4953
    bool                _forceIPv4 = false;

public:
    explicit NetworkServerAdvertiser(uint16_t port)
    {
        _port = port;
        _key = GenerateAdvertiseKey();
    }

    ADVERTISE_STATUS GetStatus() const override
    {
        return _status;
    }

    void Update() override
    {
        switch (_status) {
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

private:
    void SendRegistration(bool forceIPv4)
    {
        _lastAdvertiseTime = platform_get_ticks();

        // Send the registration request
        Http::Request request;
        request.url = GetMasterServerUrl();
        request.method = Http::Method::POST;
        request.forceIPv4 = forceIPv4;

        json_t *body = json_object();
        json_object_set_new(body, "key", json_string(_key.c_str()));
        json_object_set_new(body, "port", json_integer(_port));
        request.body                   = json_dumps(body, JSON_COMPACT);
        request.header["Content-Type"] = "application/json";

        Http::DoAsync(request, [&](Http::Response response) -> void {
            if (response.status != Http::Status::OK)
            {
                Console::WriteLine("Unable to connect to master server");
                return;
            }

            json_t * root = Json::FromString(response.body);
            this->OnRegistrationResponse(root);
        });

        json_decref(body);
    }

    void SendHeartbeat()
    {
        Http::Request request;
        request.url = GetMasterServerUrl();
        request.method = Http::Method::PUT;

        json_t * body                  = GetHeartbeatJson();
        request.body                   = json_dumps(body, JSON_COMPACT);
        request.header["Content-Type"] = "application/json";

        _lastHeartbeatTime = platform_get_ticks();
        Http::DoAsync(request, [&](Http::Response response) -> void {
            if (response.status != Http::Status::OK)
            {
                Console::WriteLine("Unable to connect to master server");
                return;
            }

            json_t * root = Json::FromString(response.body);
            this->OnHeartbeatResponse(root);
        });

        json_decref(body);
    }

    void OnRegistrationResponse(json_t * jsonRoot)
    {
        json_t *jsonStatus = json_object_get(jsonRoot, "status");
        if (json_is_integer(jsonStatus))
        {
            int32_t status = (int32_t)json_integer_value(jsonStatus);
            if (status == MASTER_SERVER_STATUS_OK)
            {
                json_t * jsonToken = json_object_get(jsonRoot, "token");
                if (json_is_string(jsonToken))
                {
                    _token = std::string(json_string_value(jsonToken));
                    _status = ADVERTISE_STATUS::REGISTERED;
                }
            }
            else
            {
                const char * message = "Invalid response from server";
                json_t * jsonMessage = json_object_get(jsonRoot, "message");
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

    void OnHeartbeatResponse(json_t * jsonRoot)
    {
        json_t *jsonStatus = json_object_get(jsonRoot, "status");
        if (json_is_integer(jsonStatus))
        {
            int32_t status = (int32_t)json_integer_value(jsonStatus);
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

    json_t * GetHeartbeatJson()
    {
        uint32_t numPlayers = network_get_num_players();

        json_t * root = json_object();
        json_object_set_new(root, "token", json_string(_token.c_str()));
        json_object_set_new(root, "players", json_integer(numPlayers));

        json_t * gameInfo = json_object();
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
        static constexpr const char hexChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
        char key[17];
        for (int32_t i = 0; i < 16; i++)
        {
            int32_t hexCharIndex = util_rand() % Util::CountOf(hexChars);
            key[i] = hexChars[hexCharIndex];
        }
        key[Util::CountOf(key) - 1] = 0;
        return key;
    }

    static const char * GetMasterServerUrl()
    {
        const char * result = OPENRCT2_MASTER_SERVER_URL;
        if (!String::IsNullOrEmpty(gConfigNetwork.master_server_url))
        {
            result = gConfigNetwork.master_server_url;
        }
        return result;
    }
};

INetworkServerAdvertiser * CreateServerAdvertiser(uint16_t port)
{
    return new NetworkServerAdvertiser(port);
}

#else // DISABLE_HTTP

class DummyNetworkServerAdvertiser final : public INetworkServerAdvertiser
{
public:
    virtual ADVERTISE_STATUS    GetStatus() const override { return ADVERTISE_STATUS::DISABLED; };
    virtual void                Update() override {};
};

INetworkServerAdvertiser * CreateServerAdvertiser(uint16_t port)
{
    return new DummyNetworkServerAdvertiser();
}

#endif // DISABLE_HTTP

#endif // DISABLE_NETWORK
