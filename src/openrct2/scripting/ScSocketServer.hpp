/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../Context.h"
#    include "../network/Socket.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

#    include <algorithm>
#    include <vector>

namespace OpenRCT2::Scripting
{
    class EventList
    {
    private:
        std::vector<std::vector<DukValue>> _listeners;

        std::vector<DukValue>& GetListenerList(uint32_t id)
        {
            if (_listeners.size() <= id)
            {
                _listeners.resize(static_cast<size_t>(id) + 1);
            }
            return _listeners[id];
        }

    public:
        void Raise(
            uint32_t id, const std::shared_ptr<Plugin>& plugin, const std::vector<DukValue>& args, bool isGameStateMutable)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();

            // Use simple for i loop in case listeners is modified during the loop
            auto listeners = GetListenerList(id);
            for (size_t i = 0; i < listeners.size(); i++)
            {
                scriptEngine.ExecutePluginCall(plugin, listeners[i], args, isGameStateMutable);

                // Safety, listeners might get reallocated
                listeners = GetListenerList(id);
            }
        }

        void AddListener(uint32_t id, const DukValue& listener)
        {
            auto& listeners = GetListenerList(id);
            listeners.push_back(listener);
        }

        void RemoveListener(uint32_t id, const DukValue& value)
        {
            auto& listeners = GetListenerList(id);
            auto it = std::find(listeners.begin(), listeners.end(), value);
            if (it != listeners.end())
            {
                listeners.erase(it);
            }
        }

        void RemoveAllListeners(uint32_t id)
        {
            auto& listeners = GetListenerList(id);
            listeners.clear();
        }
    };

    class ScSocketBase
    {
    private:
        std::shared_ptr<Plugin> _plugin;

    protected:
        static bool IsLocalhostAddress(const std::string_view& s)
        {
            return s == "localhost" || s == "127.0.0.1" || s == "::";
        }

    public:
        ScSocketBase(const std::shared_ptr<Plugin>& plugin)
            : _plugin(plugin)
        {
        }

        virtual ~ScSocketBase()
        {
            Dispose();
        }

        const std::shared_ptr<Plugin>& GetPlugin() const
        {
            return _plugin;
        }

        virtual void Update() = 0;

        virtual void Dispose()
        {
        }

        virtual bool IsDisposed() const = 0;
    };

    class ScSocket : public ScSocketBase
    {
    private:
        static constexpr uint32_t EVENT_NONE = std::numeric_limits<uint32_t>::max();
        static constexpr uint32_t EVENT_CLOSE = 0;
        static constexpr uint32_t EVENT_DATA = 1;
        static constexpr uint32_t EVENT_CONNECT_ONCE = 2;
        static constexpr uint32_t EVENT_ERROR = 3;

        EventList _eventList;
        std::unique_ptr<ITcpSocket> _socket;
        bool _disposed{};
        bool _connecting{};

    public:
        ScSocket(const std::shared_ptr<Plugin>& plugin)
            : ScSocketBase(plugin)
        {
        }

        ScSocket(const std::shared_ptr<Plugin>& plugin, std::unique_ptr<ITcpSocket>&& socket)
            : ScSocketBase(plugin)
            , _socket(std::move(socket))
        {
        }

    private:
        ScSocket* destroy(const DukValue& error)
        {
            if (_socket != nullptr)
            {
                _socket->Close();
                _socket = nullptr;
            }
            return this;
        }

        ScSocket* setNoDelay(bool noDelay)
        {
            if (_socket != nullptr)
            {
                _socket->SetNoDelay(noDelay);
            }
            return this;
        }

        ScSocket* connect(uint16_t port, const std::string& host, const DukValue& callback)
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            if (_socket != nullptr)
            {
                duk_error(ctx, DUK_ERR_ERROR, "Socket has already been created.");
            }
            else if (_disposed)
            {
                duk_error(ctx, DUK_ERR_ERROR, "Socket is disposed.");
            }
            else if (_connecting)
            {
                duk_error(ctx, DUK_ERR_ERROR, "Socket is already connecting.");
            }
            else if (!IsLocalhostAddress(host))
            {
                duk_error(ctx, DUK_ERR_ERROR, "For security reasons, only connecting to localhost is allowed.");
            }
            else
            {
                _socket = CreateTcpSocket();
                _socket->Connect(host, port);
                _eventList.AddListener(EVENT_CONNECT_ONCE, callback);
                _connecting = true;
            }
            return this;
        }

        ScSocket* end(const DukValue& data)
        {
            if (_disposed)
            {
                auto ctx = GetContext()->GetScriptEngine().GetContext();
                duk_error(ctx, DUK_ERR_ERROR, "Socket is disposed.");
            }
            else if (_socket != nullptr)
            {
                if (data.type() == DukValue::Type::STRING)
                {
                    write(data.as_string());
                }
                _socket->Finish();
            }
            return this;
        }

        bool write(const std::string& data)
        {
            if (_disposed)
            {
                auto ctx = GetContext()->GetScriptEngine().GetContext();
                duk_error(ctx, DUK_ERR_ERROR, "Socket is disposed.");
            }
            else if (_socket != nullptr)
            {
                _socket->SendData(data.c_str(), data.size());
                return true;
            }
            return false;
        }

        ScSocket* on(const std::string& eventType, const DukValue& callback)
        {
            auto eventId = GetEventType(eventType);
            if (eventId != EVENT_NONE)
            {
                _eventList.AddListener(eventId, callback);
            }
            return this;
        }

        ScSocket* off(const std::string& eventType, const DukValue& callback)
        {
            auto eventId = GetEventType(eventType);
            if (eventId != EVENT_NONE)
            {
                _eventList.RemoveListener(eventId, callback);
            }
            return this;
        }

        void CloseSocket()
        {
            if (_socket != nullptr)
            {
                _socket->Close();
                _socket = nullptr;
                RaiseOnClose(false);
            }
        }

        void RaiseOnClose(bool hadError)
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            _eventList.Raise(EVENT_CLOSE, GetPlugin(), { ToDuk(ctx, hadError) }, false);
        }

        void RaiseOnData(const std::string& data)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto ctx = scriptEngine.GetContext();
            _eventList.Raise(EVENT_DATA, GetPlugin(), { ToDuk(ctx, data) }, false);
        }

        uint32_t GetEventType(const std::string_view& name)
        {
            if (name == "close")
                return EVENT_CLOSE;
            if (name == "data")
                return EVENT_DATA;
            if (name == "error")
                return EVENT_ERROR;
            return EVENT_NONE;
        }

    public:
        void Update() override
        {
            if (_disposed)
                return;

            if (_socket != nullptr)
            {
                auto status = _socket->GetStatus();
                if (_connecting)
                {
                    if (status == SOCKET_STATUS_CONNECTED)
                    {
                        _connecting = false;
                        _eventList.Raise(EVENT_CONNECT_ONCE, GetPlugin(), {}, false);
                        _eventList.RemoveAllListeners(EVENT_CONNECT_ONCE);
                    }
                    else if (status == SOCKET_STATUS_CLOSED)
                    {
                        _connecting = false;
                        _eventList.Raise(EVENT_ERROR, GetPlugin(), {}, false);
                    }
                }
                else if (status == SOCKET_STATUS_CONNECTED)
                {
                    char buffer[2048];
                    size_t bytesRead{};
                    auto result = _socket->ReceiveData(buffer, sizeof(buffer), &bytesRead);
                    switch (result)
                    {
                        case NETWORK_READPACKET_SUCCESS:
                            RaiseOnData(std::string(buffer, bytesRead));
                            break;
                        case NETWORK_READPACKET_NO_DATA:
                            break;
                        case NETWORK_READPACKET_MORE_DATA:
                            break;
                        case NETWORK_READPACKET_DISCONNECTED:
                            CloseSocket();
                            _disposed = true;
                            break;
                    }
                }
                else
                {
                    CloseSocket();
                    _disposed = true;
                }
            }
        }

        void Dispose() override
        {
            CloseSocket();
            _disposed = true;
        }

        bool IsDisposed() const override
        {
            return _disposed;
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_method(ctx, &ScSocket::destroy, "destroy");
            dukglue_register_method(ctx, &ScSocket::setNoDelay, "setNoDelay");
            dukglue_register_method(ctx, &ScSocket::connect, "connect");
            dukglue_register_method(ctx, &ScSocket::end, "end");
            dukglue_register_method(ctx, &ScSocket::write, "write");
            dukglue_register_method(ctx, &ScSocket::on, "on");
            dukglue_register_method(ctx, &ScSocket::off, "off");
        }
    };

    class ScSocketServer : public ScSocketBase
    {
    private:
        static constexpr uint32_t EVENT_NONE = std::numeric_limits<uint32_t>::max();
        static constexpr uint32_t EVENT_CONNECTION = 0;

        EventList _eventList;
        std::unique_ptr<ITcpSocket> _socket;
        std::vector<std::shared_ptr<ScSocket>> _scClientSockets;
        bool _disposed{};

        ScSocketServer* close()
        {
            Dispose();
            return this;
        }

        ScSocketServer* listen(int32_t port, const DukValue& dukHost)
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            if (_disposed)
            {
                duk_error(ctx, DUK_ERR_ERROR, "Socket is disposed.");
            }
            else
            {
                if (_socket == nullptr)
                {
                    _socket = CreateTcpSocket();
                }

                if (_socket->GetStatus() == SOCKET_STATUS_LISTENING)
                {
                    duk_error(ctx, DUK_ERR_ERROR, "Server is already listening.");
                }
                else
                {
                    if (dukHost.type() == DukValue::Type::STRING)
                    {
                        auto host = dukHost.as_string();
                        if (IsLocalhostAddress(host))
                        {
                            _socket->Listen(host, port);
                        }
                        else
                        {
                            duk_error(ctx, DUK_ERR_ERROR, "For security reasons, only binding to localhost is allowed.");
                        }
                    }
                    else
                    {
                        _socket->Listen("127.0.0.1", port);
                    }
                }
            }
            return this;
        }

        ScSocketServer* on(const std::string& eventType, const DukValue& callback)
        {
            auto eventId = GetEventType(eventType);
            if (eventId != EVENT_NONE)
            {
                _eventList.AddListener(eventId, callback);
            }
            return this;
        }

        ScSocketServer* off(const std::string& eventType, const DukValue& callback)
        {
            auto eventId = GetEventType(eventType);
            if (eventId != EVENT_NONE)
            {
                _eventList.RemoveListener(eventId, callback);
            }
            return this;
        }

        uint32_t GetEventType(const std::string_view& name)
        {
            if (name == "connection")
                return EVENT_CONNECTION;
            return EVENT_NONE;
        }

    public:
        ScSocketServer(const std::shared_ptr<Plugin>& plugin)
            : ScSocketBase(plugin)
        {
        }

        void Update() override
        {
            if (_disposed)
                return;

            if (_socket == nullptr)
                return;

            if (_socket->GetStatus() == SOCKET_STATUS_LISTENING)
            {
                auto client = _socket->Accept();
                if (client != nullptr)
                {
                    // Default to using Nagle's algorithm like node.js does
                    client->SetNoDelay(false);

                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    auto clientSocket = std::make_shared<ScSocket>(GetPlugin(), std::move(client));
                    scriptEngine.AddSocket(clientSocket);

                    auto ctx = scriptEngine.GetContext();
                    auto dukClientSocket = GetObjectAsDukValue(ctx, clientSocket);
                    _eventList.Raise(EVENT_CONNECTION, GetPlugin(), { dukClientSocket }, false);
                }
            }
        }

        void Dispose() override
        {
            if (_socket != nullptr)
            {
                _socket->Close();
                _socket = nullptr;
            }
            _disposed = true;
        }

        bool IsDisposed() const override
        {
            return _disposed;
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_method(ctx, &ScSocketServer::close, "close");
            dukglue_register_method(ctx, &ScSocketServer::listen, "listen");
            dukglue_register_method(ctx, &ScSocketServer::on, "on");
            dukglue_register_method(ctx, &ScSocketServer::off, "off");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
