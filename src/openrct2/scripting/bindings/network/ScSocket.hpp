/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING
    #ifndef DISABLE_NETWORK

        #include "../../../Context.h"
        #include "../../../config/Config.h"
        #include "../../../network/Socket.h"
        #include "../../ScriptEngine.h"

        #include <memory>
        #include <vector>

namespace OpenRCT2::Scripting
{
    class EventList
    {
    private:
        std::vector<std::vector<JSCallback>> _listeners;

        std::vector<JSCallback>& GetListenerList(uint32_t id)
        {
            if (_listeners.size() <= id)
            {
                _listeners.resize(static_cast<size_t>(id) + 1);
            }
            return _listeners[id];
        }

    public:
        void Raise(
            uint32_t id, const std::shared_ptr<Plugin>& plugin, const std::vector<JSValue>& args, bool isGameStateMutable)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();

            // Use simple for i loop in case listeners is modified during the loop
            auto listeners = GetListenerList(id);
            for (size_t i = 0; i < listeners.size(); i++)
            {
                scriptEngine.ExecutePluginCall(plugin, listeners[i].callback, args, isGameStateMutable);

                // Safety, listeners might get reallocated
                listeners = GetListenerList(id);
            }
        }

        void AddListener(uint32_t id, const JSCallback& listener)
        {
            auto& listeners = GetListenerList(id);
            listeners.push_back(listener);
        }

        void RemoveListener(uint32_t id, const JSCallback& value)
        {
            auto& listeners = GetListenerList(id);
            std::erase_if(listeners, [&value](const JSCallback& x) {
                // Note: this might be hacky because I am not sure if/when quickjs will keep these function ptrs stable.
                return JS_VALUE_GET_PTR(value.callback) == JS_VALUE_GET_PTR(x.callback);
            });
        }

        void RemoveAllListeners(uint32_t id)
        {
            auto& listeners = GetListenerList(id);
            listeners.clear();
        }

        void RemoveAllListeners()
        {
            _listeners.clear();
        }
    };

    inline bool IsLocalhostAddress(std::string_view s)
    {
        return s == "localhost" || s == "127.0.0.1" || s == "::";
    }

    inline bool IsOnWhiteList(std::string_view host)
    {
        constexpr char delimiter = ',';
        size_t start_pos = 0;
        size_t end_pos = 0;
        while ((end_pos = Config::Get().plugin.allowedHosts.find(delimiter, start_pos)) != std::string::npos)
        {
            if (host == Config::Get().plugin.allowedHosts.substr(start_pos, end_pos - start_pos))
            {
                return true;
            }
            start_pos = end_pos + 1;
        }
        return host
            == Config::Get().plugin.allowedHosts.substr(start_pos, Config::Get().plugin.allowedHosts.length() - start_pos);
    }

    struct SocketDataBase
    {
        EventList _eventList;
        std::unique_ptr<Network::ITcpSocket> _socket;
        std::shared_ptr<Plugin> _plugin;
        bool _disposed{};

        virtual ~SocketDataBase() = default;
        virtual void Update() = 0;
        virtual void Dispose() = 0;
    };

    struct SocketData final : SocketDataBase
    {
        static constexpr uint32_t EVENT_NONE = std::numeric_limits<uint32_t>::max();
        static constexpr uint32_t EVENT_CLOSE = 0;
        static constexpr uint32_t EVENT_DATA = 1;
        static constexpr uint32_t EVENT_CONNECT_ONCE = 2;
        static constexpr uint32_t EVENT_ERROR = 3;

        bool _connecting{};
        bool _wasConnected{};

        ~SocketData() override
        {
            SocketData::Dispose();
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
            _eventList.RemoveAllListeners();
        }

        void RaiseOnClose(bool hadError)
        {
            JSContext* ctx = GetContext()->GetScriptEngine().GetContext();
            _eventList.Raise(EVENT_CLOSE, _plugin, { JS_NewBool(ctx, hadError) }, false);
        }

        void RaiseOnData(std::string_view data)
        {
            JSContext* ctx = GetContext()->GetScriptEngine().GetContext();
            _eventList.Raise(EVENT_DATA, _plugin, { JSFromStdString(ctx, data) }, false);
        }

        static uint32_t GetEventType(std::string_view name)
        {
            if (name == "close")
                return EVENT_CLOSE;
            if (name == "data")
                return EVENT_DATA;
            if (name == "error")
                return EVENT_ERROR;
            return EVENT_NONE;
        }

        void Update() override
        {
            if (_disposed)
                return;

            if (_socket != nullptr)
            {
                auto status = _socket->GetStatus();
                if (_connecting)
                {
                    if (status == Network::SocketStatus::connected)
                    {
                        _connecting = false;
                        _wasConnected = true;
                        _eventList.Raise(EVENT_CONNECT_ONCE, _plugin, {}, false);
                        _eventList.RemoveAllListeners(EVENT_CONNECT_ONCE);
                    }
                    else if (status == Network::SocketStatus::closed)
                    {
                        _connecting = false;

                        JSContext* ctx = GetContext()->GetScriptEngine().GetContext();
                        auto err = _socket->GetError();
                        if (err == nullptr)
                        {
                            err = "";
                        }
                        auto jsErr = JSFromStdString(ctx, std::string_view(err));
                        _eventList.Raise(EVENT_ERROR, _plugin, { jsErr }, true);
                    }
                }
                else if (status == Network::SocketStatus::connected)
                {
                    char buffer[16384];
                    size_t bytesRead{};
                    auto result = _socket->ReceiveData(buffer, sizeof(buffer), &bytesRead);
                    switch (result)
                    {
                        case Network::ReadPacket::success:
                            RaiseOnData(std::string_view(buffer, bytesRead));
                            break;
                        case Network::ReadPacket::noData:
                            break;
                        case Network::ReadPacket::moreData:
                            break;
                        case Network::ReadPacket::disconnected:
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
    };

    class ScSocket;
    extern ScSocket gScSocket;
    class ScSocket final : public ScBase
    {
    private:
        static SocketData* GetData(JSValue thisVal)
        {
            return gScSocket.GetOpaque<SocketData*>(thisVal);
        }

        static JSValue destroy(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid socket");

            data->CloseSocket();
            return JS_DupValue(ctx, thisVal);
        }

        static JSValue setNoDelay(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_BOOL(noDelay, ctx, argv[0]);

            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid socket");

            if (data->_socket != nullptr)
            {
                data->_socket->SetNoDelay(noDelay);
            }
            return JS_DupValue(ctx, thisVal);
        }

        static JSValue connect(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_UINT32(port, ctx, argv[0]);
            if (port & 0xFFFF0000)
                return JS_ThrowRangeError(ctx, "Invalid port.");
            JS_UNPACK_STR(host, ctx, argv[1]);

            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid socket");

            if (data->_socket != nullptr)
            {
                JS_ThrowPlainError(ctx, "Socket has already been created.");
            }
            else if (data->_disposed)
            {
                JS_ThrowPlainError(ctx, "Socket is disposed.");
            }
            else if (data->_connecting)
            {
                JS_ThrowPlainError(ctx, "Socket is already connecting.");
            }
            else if (!IsLocalhostAddress(host) && !IsOnWhiteList(host))
            {
                JS_ThrowPlainError(ctx, "For security reasons, only connecting to localhost is allowed.");
            }
            else
            {
                data->_socket = Network::CreateTcpSocket();
                try
                {
                    data->_socket->ConnectAsync(host, port);
                    if (JS_IsFunction(ctx, argv[2]))
                    {
                        data->_eventList.AddListener(SocketData::EVENT_CONNECT_ONCE, JSCallback(ctx, argv[2]));
                    }
                    data->_connecting = true;
                }
                catch (const std::exception& e)
                {
                    JS_ThrowInternalError(ctx, "%s", e.what());
                }
            }
            return JS_DupValue(ctx, thisVal);
        }

        static JSValue end(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid socket");

            if (data->_disposed)
            {
                JS_ThrowPlainError(ctx, "Socket is disposed.");
            }
            else if (data->_socket != nullptr)
            {
                if (JS_IsString(argv[0]))
                {
                    if (JSValue res = write(ctx, thisVal, argc, argv); JS_IsException(res))
                    {
                        return res;
                    }
                    data->_socket->Finish();
                }
                else
                {
                    data->_socket->Finish();
                    JS_ThrowPlainError(ctx, "Only sending strings is currently supported.");
                }
            }
            return JS_DupValue(ctx, thisVal);
        }

        static JSValue write(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(str, ctx, argv[0]);

            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid socket");

            if (data->_disposed)
            {
                JS_ThrowPlainError(ctx, "Socket is disposed.");
            }
            else if (data->_socket != nullptr)
            {
                try
                {
                    auto sentBytes = data->_socket->SendData(str.c_str(), str.size());
                    return JS_NewBool(ctx, sentBytes != str.size());
                }
                catch (const std::exception&)
                {
                    return JS_NewBool(ctx, false);
                }
            }
            return JS_NewBool(ctx, false);
        }

        static JSValue on(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(eventType, ctx, argv[0]);
            JS_UNPACK_CALLBACK(callback, ctx, argv[1]);

            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid socket");

            auto eventId = SocketData::GetEventType(eventType);
            if (eventId != SocketData::EVENT_NONE)
            {
                data->_eventList.AddListener(eventId, callback);
            }
            return JS_DupValue(ctx, thisVal);
        }

        static JSValue off(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(eventType, ctx, argv[0]);
            JS_UNPACK_CALLBACK(callback, ctx, argv[1]);

            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid socket");

            auto eventId = SocketData::GetEventType(eventType);
            if (eventId != SocketData::EVENT_NONE)
            {
                data->_eventList.RemoveListener(eventId, callback);
            }
            return JS_DupValue(ctx, thisVal);
        }

    public:
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CFUNC_DEF("destroy", 1, ScSocket::destroy), JS_CFUNC_DEF("setNoDelay", 1, ScSocket::setNoDelay),
            JS_CFUNC_DEF("connect", 3, ScSocket::connect), JS_CFUNC_DEF("end", 1, ScSocket::end),
            JS_CFUNC_DEF("write", 1, ScSocket::write),     JS_CFUNC_DEF("on", 2, ScSocket::on),
            JS_CFUNC_DEF("off", 2, ScSocket::off),
        };

        JSValue New(JSContext* ctx, const std::shared_ptr<Plugin>& plugin)
        {
            // unique ptr is used to avoid static analyzer false positives.
            auto data = std::make_unique<SocketData>();
            data->_plugin = plugin;
            GetContext()->GetScriptEngine().AddSocket(data.get());
            return MakeWithOpaque(ctx, funcs, data.release());
        }

        JSValue New(JSContext* ctx, const std::shared_ptr<Plugin>& plugin, std::unique_ptr<Network::ITcpSocket>&& socket)
        {
            auto data = std::make_unique<SocketData>();
            data->_plugin = plugin;
            data->_socket = std::move(socket);
            GetContext()->GetScriptEngine().AddSocket(data.get());
            return MakeWithOpaque(ctx, funcs, data.release());
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Socket", Finalize);
        }

        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            SocketData* data = gScSocket.GetOpaque<SocketData*>(thisVal);
            if (data)
            {
                // Note the game context pointer can be null if the game is shutting down,
                // but all sockets should have been cleaned up by then.
                IContext* gameContext = GetContext();
                assert(gameContext != nullptr);
                gameContext->GetScriptEngine().RemoveSocket(data);
                data->Dispose();
                delete data;
            }
        }
    };

    struct ListenerData final : SocketDataBase
    {
        static constexpr uint32_t EVENT_NONE = std::numeric_limits<uint32_t>::max();
        static constexpr uint32_t EVENT_CONNECTION = 0;

        ~ListenerData() override
        {
            ListenerData::Dispose();
        }

        void Update() override
        {
            if (_disposed)
                return;

            if (_socket == nullptr)
                return;

            if (_socket->GetStatus() == Network::SocketStatus::listening)
            {
                auto client = _socket->Accept();
                if (client != nullptr)
                {
                    // Default to using Nagle's algorithm like node.js does
                    client->SetNoDelay(false);

                    JSContext* ctx = GetContext()->GetScriptEngine().GetContext();
                    auto clientSocket = gScSocket.New(ctx, _plugin, std::move(client));
                    _eventList.Raise(EVENT_CONNECTION, _plugin, { clientSocket }, false);
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
            _eventList.RemoveAllListeners();
        }

        void Dispose() override
        {
            CloseSocket();
            _disposed = true;
        }
    };

    class ScListener;
    extern ScListener gScListener;
    class ScListener final : public ScBase
    {
        static ListenerData* GetData(JSValue thisVal)
        {
            return gScListener.GetOpaque<ListenerData*>(thisVal);
        }

        static JSValue listening_get(JSContext* ctx, JSValue thisVal)
        {
            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid listener");

            if (data->_socket != nullptr)
            {
                return JS_NewBool(ctx, data->_socket->GetStatus() == Network::SocketStatus::listening);
            }
            return JS_NewBool(ctx, false);
        }

        static JSValue close(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid listener");

            data->CloseSocket();
            return JS_DupValue(ctx, thisVal);
        }

        static JSValue listen(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_UINT32(port, ctx, argv[0]);
            if (port & 0xFFFF0000)
                return JS_ThrowRangeError(ctx, "Invalid port.");

            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid listener");

            if (data->_disposed)
            {
                JS_ThrowPlainError(ctx, "Socket is disposed.");
            }
            else
            {
                if (data->_socket == nullptr)
                {
                    data->_socket = Network::CreateTcpSocket();
                }

                if (data->_socket->GetStatus() == Network::SocketStatus::listening)
                {
                    JS_ThrowPlainError(ctx, "Server is already listening.");
                }
                else
                {
                    if (JS_IsString(argv[1]))
                    {
                        std::string host = JSToStdString(ctx, argv[1]);
                        if (IsLocalhostAddress(host) || IsOnWhiteList(host))
                        {
                            try
                            {
                                data->_socket->Listen(host, port);
                            }
                            catch (const std::exception& e)
                            {
                                JS_ThrowPlainError(ctx, "%s", e.what());
                            }
                        }
                        else
                        {
                            JS_ThrowPlainError(ctx, "For security reasons, only binding to localhost is allowed.");
                        }
                    }
                    else
                    {
                        data->_socket->Listen("127.0.0.1", port);
                    }
                }
            }
            return JS_DupValue(ctx, thisVal);
        }

        static JSValue on(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(eventType, ctx, argv[0]);
            JS_UNPACK_CALLBACK(callback, ctx, argv[1]);

            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid listener");

            auto eventId = GetEventType(eventType);
            if (eventId != ListenerData::EVENT_NONE)
            {
                data->_eventList.AddListener(eventId, callback);
            }
            return JS_DupValue(ctx, thisVal);
        }

        static JSValue off(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(eventType, ctx, argv[0]);
            JS_UNPACK_CALLBACK(callback, ctx, argv[1]);

            auto data = GetData(thisVal);
            if (!data)
                return JS_ThrowInternalError(ctx, "Invalid listener");

            auto eventId = GetEventType(eventType);
            if (eventId != ListenerData::EVENT_NONE)
            {
                data->_eventList.RemoveListener(eventId, callback);
            }
            return JS_DupValue(ctx, thisVal);
        }

        static uint32_t GetEventType(std::string_view name)
        {
            if (name == "connection")
                return ListenerData::EVENT_CONNECTION;
            return ListenerData::EVENT_NONE;
        }

    public:
        JSValue New(JSContext* ctx, const std::shared_ptr<Plugin>& plugin)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("listening", ScListener::listening_get, nullptr),
                JS_CFUNC_DEF("close", 0, ScListener::close),
                JS_CFUNC_DEF("listen", 2, ScListener::listen),
                JS_CFUNC_DEF("on", 2, ScListener::on),
                JS_CFUNC_DEF("off", 2, ScListener::off),
            };

            // unique ptr is used to avoid static analyzer false positives.
            auto data = std::make_unique<ListenerData>();
            data->_plugin = plugin;
            GetContext()->GetScriptEngine().AddSocket(data.get());
            return MakeWithOpaque(ctx, funcs, data.release());
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Listener", Finalize);
        }

        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            ListenerData* data = gScListener.GetOpaque<ListenerData*>(thisVal);
            if (data)
            {
                // Note the game context pointer can be null if the game is shutting down,
                // but all sockets should have been cleaned up by then.
                IContext* gameContext = GetContext();
                assert(gameContext != nullptr);
                gameContext->GetScriptEngine().RemoveSocket(data);
                data->Dispose();
                delete data;
            }
        }
    };
} // namespace OpenRCT2::Scripting

    #endif
#endif
