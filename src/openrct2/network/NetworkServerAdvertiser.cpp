#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef DISABLE_NETWORK

#include <string>
#include "../core/Console.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "network.h"
#include "NetworkServerAdvertiser.h"

#include "../config/Config.h"
#include "../localisation/date.h"
#include "../management/finance.h"
#include "../peep/peep.h"
#include "../platform/platform.h"
#include "../util/util.h"
#include "../world/map.h"
#include "../world/park.h"
#include "http.h"

enum MASTER_SERVER_STATUS
{
    MASTER_SERVER_STATUS_OK                 = 200,
    MASTER_SERVER_STATUS_INVALID_TOKEN      = 401,
    MASTER_SERVER_STATUS_SERVER_NOT_FOUND   = 404,
    MASTER_SERVER_STATUS_INTERNAL_ERROR     = 500
};

constexpr sint32 MASTER_SERVER_REGISTER_TIME = 120 * 1000; // 2 minutes
constexpr sint32 MASTER_SERVER_HEARTBEAT_TIME = 60 * 1000; // 1 minute

class NetworkServerAdvertiser : public INetworkServerAdvertiser
{
private:
    uint16 _port;

    ADVERTISE_STATUS    _status = ADVERTISE_STATUS_UNREGISTERED;
    uint32              _lastAdvertiseTime = 0;
    uint32              _lastHeartbeatTime = 0;

    // Our unique token for this server
    std::string         _token;

    // Key received from the master server
    std::string         _key;

public:
    explicit NetworkServerAdvertiser(uint16 port)
    {
        _port = port;
        _key = GenerateAdvertiseKey();
    }

    ADVERTISE_STATUS GetStatus() override
    {
        return _status;
    }

    void Update() override
    {
        switch (_status) {
        case ADVERTISE_STATUS_UNREGISTERED:
            if (_lastAdvertiseTime == 0 || platform_get_ticks() > _lastAdvertiseTime + MASTER_SERVER_REGISTER_TIME)
            {
                SendRegistration();
            }
            break;
        case ADVERTISE_STATUS_REGISTERED:
            if (platform_get_ticks() > _lastHeartbeatTime + MASTER_SERVER_HEARTBEAT_TIME)
            {
                SendHeartbeat();
            }
            break;
        // exhaust enum values to satisfy clang
        case ADVERTISE_STATUS_DISABLED:
            break;
        }
    }

private:
    void SendRegistration()
    {
        _lastAdvertiseTime = platform_get_ticks();

        // Send the registration request
        http_request_t request = {};
        request.tag = this;
        request.url = GetMasterServerUrl();
        request.method = HTTP_METHOD_POST;

        json_t *body = json_object();
        json_object_set_new(body, "key", json_string(_key.c_str()));
        json_object_set_new(body, "port", json_integer(_port));
        request.root = body;
        request.type = HTTP_DATA_JSON;

        http_request_async(&request, [](http_response_t * response) -> void
        {
            if (response == nullptr)
            {
                Console::WriteLine("Unable to connect to master server");
            }
            else
            {
                auto advertiser = static_cast<NetworkServerAdvertiser*>(response->tag);
                advertiser->OnRegistrationResponse(response->root);
                http_request_dispose(response);
            }
        });

        json_decref(body);
    }

    void SendHeartbeat()
    {
        http_request_t request = {};
        request.tag = this;
        request.url = GetMasterServerUrl();
        request.method = HTTP_METHOD_PUT;

        json_t * jsonBody = GetHeartbeatJson();
        request.root = jsonBody;
        request.type = HTTP_DATA_JSON;

        _lastHeartbeatTime = platform_get_ticks();
        http_request_async(&request, [](http_response_t *response) -> void
        {
            if (response == nullptr)
            {
                log_warning("Unable to connect to master server");
            }
            else
            {
                auto advertiser = static_cast<NetworkServerAdvertiser*>(response->tag);
                advertiser->OnHeartbeatResponse(response->root);
                http_request_dispose(response);
            }
        });

        json_decref(jsonBody);
    }

    void OnRegistrationResponse(json_t * jsonRoot)
    {
        json_t *jsonStatus = json_object_get(jsonRoot, "status");
        if (json_is_integer(jsonStatus))
        {
            sint32 status = (sint32)json_integer_value(jsonStatus);
            if (status == MASTER_SERVER_STATUS_OK)
            {
                json_t * jsonToken = json_object_get(jsonRoot, "token");
                if (json_is_string(jsonToken))
                {
                    _token = std::string(json_string_value(jsonToken));
                    _status = ADVERTISE_STATUS_REGISTERED;
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
                Console::Error::WriteLine("Unable to advertise: %s", message);
            }
        }
    }

    void OnHeartbeatResponse(json_t * jsonRoot)
    {
        json_t *jsonStatus = json_object_get(jsonRoot, "status");
        if (json_is_integer(jsonStatus))
        {
            sint32 status = (sint32)json_integer_value(jsonStatus);
            if (status == MASTER_SERVER_STATUS_OK)
            {
                // Master server has successfully updated our server status
            }
            else if (status == MASTER_SERVER_STATUS_INVALID_TOKEN)
            {
                _status = ADVERTISE_STATUS_UNREGISTERED;
                Console::WriteLine("Master server heartbeat failed: Invalid Token");
            }
        }
    }

    json_t * GetHeartbeatJson()
    {
        uint32 numPlayers = network_get_num_players();

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
            money32 cash = DECRYPT_MONEY(gCashEncrypted);
            json_object_set_new(gameInfo, "cash", json_integer(cash));
        }
        json_object_set_new(root, "gameInfo", gameInfo);

        return root;
    }

    static std::string GenerateAdvertiseKey()
    {
        // Generate a string of 16 random hex characters (64-integer key as a hex formatted string)
        static const char hexChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
        char key[17];
        for (sint32 i = 0; i < 16; i++)
        {
            sint32 hexCharIndex = util_rand() % Util::CountOf(hexChars);
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

INetworkServerAdvertiser * CreateServerAdvertiser(uint16 port)
{
    return new NetworkServerAdvertiser(port);
}

#endif // DISABLE_NETWORK
