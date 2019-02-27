#include "NetworkClient.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../ParkImporter.h"
#include "../actions/GameAction.h"
#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/FileStream.hpp"
#include "../core/Json.hpp"
#include "../core/MemoryStream.h"
#include "../core/String.hpp"
#include "../interface/Chat.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../network/network.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../windows/Intent.h"
#include "NetworkPackets.h"
#include "TcpSocket.h"

bool NetworkClient::Startup()
{
    log_verbose("%s\n", __FUNCTION__);

    _connection = std::make_unique<NetworkConnection>();

    _state = NETWORK_STATE::READY;
    _lastConnectStatus = SOCKET_STATUS::CLOSED;

    return true;
}

bool NetworkClient::Shutdown()
{
    log_verbose("%s\n", __FUNCTION__);

    if (_state == NETWORK_STATE::NONE)
    {
        return false;
    }

    Close();

    _state = NETWORK_STATE::NONE;

    return true;
}

void NetworkClient::Close()
{
    log_verbose("%s\n", __FUNCTION__);

    if (_state != NETWORK_STATE::CONNECTING && _state != NETWORK_STATE::CONNECTED)
    {
        return;
    }

    _connection->Close();
    _lastConnectStatus = SOCKET_STATUS::CLOSED;

    _state = NETWORK_STATE::READY;
}

void NetworkClient::Update()
{
    if (_state == NETWORK_STATE::CONNECTING)
    {
        if (!UpdateConnecting())
        {
            // TODO: Handle error.
        }
        return;
    }
    else if (_state == NETWORK_STATE::CONNECTED)
    {
        if (!UpdateConnection())
        {
            // TODO: Handle error.
        }
        return;
    }
}

void NetworkClient::Flush()
{
    if (_state == NETWORK_STATE::CONNECTED)
    {
        //_connection->SendQueuedPackets();
    }
}

bool NetworkClient::UpdateConnecting()
{
    switch (_connection->GetStatus())
    {
        case SOCKET_STATUS::RESOLVING:
            if (_lastConnectStatus != SOCKET_STATUS::RESOLVING)
            {
                _lastConnectStatus = SOCKET_STATUS::RESOLVING;
                HandleSocketConnecting();
            }
            break;
        case SOCKET_STATUS::CONNECTING:
            if (_lastConnectStatus != SOCKET_STATUS::CONNECTING)
            {
                _lastConnectStatus = SOCKET_STATUS::CONNECTING;
                HandleSocketConnecting();
            }
            break;
        case SOCKET_STATUS::CONNECTED:
        {
            _state = NETWORK_STATE::CONNECTED;
            _lastConnectStatus = SOCKET_STATUS::CONNECTED;
            HandleSocketConnected();
        }
        break;
        default:
        {
            _state = NETWORK_STATE::READY;
            _lastConnectStatus = SOCKET_STATUS::CLOSED;
            HandleSocketError();
        }
        break;
    }

    return true;
}

bool NetworkClient::Connect(const char* host, int32_t port)
{
    log_verbose("%s\n", __FUNCTION__);

    if (_state != NETWORK_STATE::READY)
    {
        return false;
    }

    log_verbose("Connecting to %s:%d\n", host, port);

    _connection->ConnectAsync(host, port);

    _state = NETWORK_STATE::CONNECTING;

    return true;
}

void NetworkClient::HandleSocketResolving()
{
    log_verbose("%s\n", __FUNCTION__);

    char str_resolving[256];
    format_string(str_resolving, 256, STR_MULTIPLAYER_RESOLVING, nullptr);

    auto intent = Intent(WC_NETWORK_STATUS);
    intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_resolving });
    intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { network_close(); });
    context_open_intent(&intent);
}

void NetworkClient::HandleSocketConnecting()
{
    log_verbose("%s\n", __FUNCTION__);

    char str_connecting[256];
    format_string(str_connecting, 256, STR_MULTIPLAYER_CONNECTING, nullptr);

    auto intent = Intent(WC_NETWORK_STATUS);
    intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_connecting });
    intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { network_close(); });
    context_open_intent(&intent);

    // server_connect_time = platform_get_ticks();
}

void NetworkClient::HandleSocketConnected()
{
    log_verbose("%s\n", __FUNCTION__);

    char str_authenticating[256];
    format_string(str_authenticating, 256, STR_MULTIPLAYER_AUTHENTICATING, nullptr);

    auto intent = Intent(WC_NETWORK_STATUS);
    intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_authenticating });
    intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { network_close(); });
    context_open_intent(&intent);

    RequestToken();
}

void NetworkClient::HandleSocketError()
{
    log_verbose("%s\n", __FUNCTION__);

    const char* error = _connection->GetError();
    if (error != nullptr)
    {
        Console::Error::WriteLine(error);
    }

    context_force_close_window_by_class(WC_NETWORK_STATUS);
    context_show_error(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_NONE);
}

void NetworkClient::RequestToken()
{
    log_verbose("%s\n", __FUNCTION__);

    NetworkPacketRequestToken req;
    req.gameVersion = GetGameVersion();

    //_connection->EnqueuePacket(req);
}

bool NetworkClient::UpdateConnection()
{
    /*
    if (!ProcessConnection(*server_connection))
    {
        // Do not show disconnect message window when password window closed/canceled
        if (server_connection->AuthStatus == NETWORK_AUTH_REQUIREPASSWORD)
        {
            context_force_close_window_by_class(WC_NETWORK_STATUS);
        }
        else
        {
            char str_disconnected[256];

            if (_sock->GetLastDisconnectReason())
            {
                const char* disconnect_reason = server_connection->GetLastDisconnectReason();
                format_string(str_disconnected, 256, STR_MULTIPLAYER_DISCONNECTED_WITH_REASON, &disconnect_reason);
            }
            else
            {
                format_string(str_disconnected, 256, STR_MULTIPLAYER_DISCONNECTED_NO_REASON, nullptr);
            }

            auto intent = Intent(WC_NETWORK_STATUS);
            intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_disconnected });
            context_open_intent(&intent);
        }
        window_close_by_class(WC_MULTIPLAYER);
    }
    */
    return true;
}
