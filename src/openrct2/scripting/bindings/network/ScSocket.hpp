/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING
#    ifndef DISABLE_NETWORK

#        include "../../../Context.h"
#        include "../../../config/Config.h"
#        include "../../../network/Socket.h"
#        include "../../Duktape.hpp"
#        include "../../ScriptEngine.h"

#        include <algorithm>
#        include <memory>
#        include <vector>

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
            listeners.erase(std::remove(listeners.begin(), listeners.end(), value), listeners.end());
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
        static bool IsLocalhostAddress(std::string_view s)
        {
            return s == "localhost" || s == "127.0.0.1" || s == "::";
        }

        static bool IsOnWhiteList(std::string_view host)
        {
            constexpr char delimiter = ',';
            size_t start_pos = 0;
            size_t end_pos = 0;
            while ((end_pos = gConfigPlugin.AllowedHosts.find(delimiter, start_pos)) != std::string::npos)
            {
                if (host == gConfigPlugin.AllowedHosts.substr(start_pos, end_pos - start_pos))
                {
                    return true;
                }
                start_pos = end_pos + 1;
            }
            return host == gConfigPlugin.AllowedHosts.substr(start_pos, gConfigPlugin.AllowedHosts.length() - start_pos);
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

    class ScSocket final : public ScSocketBase
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
        bool _wasConnected{};

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
            CloseSocket();
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
            else if (!IsLocalhostAddress(host) && !IsOnWhiteList(host))
            {
                duk_error(ctx, DUK_ERR_ERROR, "For security reasons, only connecting to localhost is allowed.");
            }
            else
            {
                _socket = CreateTcpSocket();
                try
                {
                    _socket->ConnectAsync(host, port);
                    _eventList.AddListener(EVENT_CONNECT_ONCE, callback);
                    _connecting = true;
                }
                catch (const std::exception& e)
                {
                    duk_error(ctx, DUK_ERR_ERROR, e.what());
                }
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
                    _socket->Finish();
                }
                else
                {
                    _socket->Finish();
                    auto ctx = GetContext()->GetScriptEngine().GetContext();
                    duk_error(ctx, DUK_ERR_ERROR, "Only sending strings is currently supported.");
                }
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
                try
                {
                    auto sentBytes = _socket->SendData(data.c_str(), data.size());
                    return sentBytes != data.size();
                }
                catch (const std::exception&)
                {
                    return false;
                }
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
                if (_wasConnected)
                {
                    _wasConnected = false;
                    RaiseOnClose(false);
                }
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

        uint32_t GetEventType(std::string_view name)
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
                    if (status == SocketStatus::Connected)
                    {
                        _connecting = false;
                        _wasConnected = true;
                        _eventList.Raise(EVENT_CONNECT_ONCE, GetPlugin(), {}, false);
                        _eventList.RemoveAllListeners(EVENT_CONNECT_ONCE);
                    }
                    else if (status == SocketStatus::Closed)
                    {
                        _connecting = false;

                        auto& scriptEngine = GetContext()->GetScriptEngine();
                        auto ctx = scriptEngine.GetContext();
                        auto err = _socket->GetError();
                        if (err == nullptr)
                        {
                            err = "";
                        }
                        auto dukErr = ToDuk(ctx, std::string_view(err));
                        _eventList.Raise(EVENT_ERROR, GetPlugin(), { dukErr }, true);
                    }
                }
                else if (status == SocketStatus::Connected)
                {
                    char buffer[2048];
                    size_t bytesRead{};
                    auto result = _socket->ReceiveData(buffer, sizeof(buffer), &bytesRead);
                    switch (result)
                    {
                        case NetworkReadPacket::Success:
                            RaiseOnData(std::string(buffer, bytesRead));
                            break;
                        case NetworkReadPacket::NoData:
                            break;
                        case NetworkReadPacket::MoreData:
                            break;
                        case NetworkReadPacket::Disconnected:
                            CloseSocket();
                            break;
                    }
                }
                else
                {
                    CloseSocket();
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

    class ScListener final : public ScSocketBase
    {
    private:
        static constexpr uint32_t EVENT_NONE = std::numeric_limits<uint32_t>::max();
        static constexpr uint32_t EVENT_CONNECTION = 0;

        EventList _eventList;
        std::unique_ptr<ITcpSocket> _socket;
        std::vector<std::shared_ptr<ScSocket>> _scClientSockets;
        bool _disposed{};

        bool listening_get()
        {
            if (_socket != nullptr)
            {
                return _socket->GetStatus() == SocketStatus::Listening;
            }
            return false;
        }

        ScListener* close()
        {
            CloseSocket();
            return this;
        }

        ScListener* listen(int32_t port, const DukValue& dukHost)
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

                if (_socket->GetStatus() == SocketStatus::Listening)
                {
                    duk_error(ctx, DUK_ERR_ERROR, "Server is already listening.");
                }
                else
                {
                    if (dukHost.type() == DukValue::Type::STRING)
                    {
                        auto host = dukHost.as_string();
                        if (IsLocalhostAddress(host) || IsOnWhiteList(host))
                        {
                            try
                            {
                                _socket->Listen(host, port);
                            }
                            catch (const std::exception& e)
                            {
                                duk_error(ctx, DUK_ERR_ERROR, e.what());
                            }
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

        ScListener* on(const std::string& eventType, const DukValue& callback)
        {
            auto eventId = GetEventType(eventType);
            if (eventId != EVENT_NONE)
            {
                _eventList.AddListener(eventId, callback);
            }
            return this;
        }

        ScListener* off(const std::string& eventType, const DukValue& callback)
        {
            auto eventId = GetEventType(eventType);
            if (eventId != EVENT_NONE)
            {
                _eventList.RemoveListener(eventId, callback);
            }
            return this;
        }

        uint32_t GetEventType(std::string_view name)
        {
            if (name == "connection")
                return EVENT_CONNECTION;
            return EVENT_NONE;
        }

    public:
        ScListener(const std::shared_ptr<Plugin>& plugin)
            : ScSocketBase(plugin)
        {
        }

        void Update() override
        {
            if (_disposed)
                return;

            if (_socket == nullptr)
                return;

            if (_socket->GetStatus() == SocketStatus::Listening)
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

        void CloseSocket()
        {
            if (_socket != nullptr)
            {
                _socket->Close();
                _socket = nullptr;
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
            dukglue_register_property(ctx, &ScListener::listening_get, nullptr, "listening");
            dukglue_register_method(ctx, &ScListener::close, "close");
            dukglue_register_method(ctx, &ScListener::listen, "listen");
            dukglue_register_method(ctx, &ScListener::on, "on");
            dukglue_register_method(ctx, &ScListener::off, "off");
        }
    };
} // namespace OpenRCT2::Scripting

#    endif
#endif
