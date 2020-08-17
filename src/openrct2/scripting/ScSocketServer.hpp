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

namespace OpenRCT2::Scripting
{
    class ScSocketBase
    {
    private:
        std::shared_ptr<Plugin> _plugin;

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
        std::unique_ptr<ITcpSocket> _socket;
        bool _disposed{};

        DukValue _onClose;
        DukValue _onData;

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
            if (eventType == "close")
            {
                _onClose = callback;
            }
            else if (eventType == "data")
            {
                _onData = callback;
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
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto ctx = scriptEngine.GetContext();
            scriptEngine.ExecutePluginCall(GetPlugin(), _onClose, { ToDuk(ctx, hadError) }, false);
        }

        void RaiseOnData(const std::string& data)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto ctx = scriptEngine.GetContext();
            scriptEngine.ExecutePluginCall(GetPlugin(), _onData, { ToDuk(ctx, data) }, false);
        }

    public:
        void Update() override
        {
            if (_disposed)
                return;

            if (_socket != nullptr)
            {
                if (_socket->GetStatus() == SOCKET_STATUS_CONNECTED)
                {
                    char buffer[128];
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
            dukglue_register_method(ctx, &ScSocket::end, "end");
            dukglue_register_method(ctx, &ScSocket::write, "write");
            dukglue_register_method(ctx, &ScSocket::on, "on");
        }
    };

    class ScSocketServer : public ScSocketBase
    {
    private:
        std::unique_ptr<ITcpSocket> _socket;
        DukValue _onConnection;
        std::vector<std::shared_ptr<ScSocket>> _scClientSockets;
        bool _disposed{};

        ScSocketServer* close()
        {
            Dispose();
            return this;
        }

        ScSocketServer* listen(int32_t port, const DukValue& callback)
        {
            if (_disposed)
            {
                auto ctx = GetContext()->GetScriptEngine().GetContext();
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
                    auto ctx = GetContext()->GetScriptEngine().GetContext();
                    duk_error(ctx, DUK_ERR_ERROR, "Server is already listening.");
                }
                else
                {
                    _socket->Listen(port);
                }
            }
            return this;
        }

        ScSocketServer* on(const std::string& eventType, const DukValue& callback)
        {
            if (eventType == "connection")
            {
                _onConnection = callback;
            }
            return this;
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
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    auto clientSocket = std::make_shared<ScSocket>(GetPlugin(), std::move(client));
                    scriptEngine.AddSocket(clientSocket);

                    auto ctx = scriptEngine.GetContext();
                    auto dukClientSocket = GetObjectAsDukValue(ctx, clientSocket);
                    scriptEngine.ExecutePluginCall(GetPlugin(), _onConnection, { dukClientSocket }, false);
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
        }
    };
} // namespace OpenRCT2::Scripting

#endif
