/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>

namespace OpenRCT2::Network
{
    struct ITcpSocket;
}

/**
 * Listens on a random loopback port. Base for the fake servers below.
 */
class LoopbackTcpListener
{
public:
    std::string getUrl() const;

protected:
    LoopbackTcpListener();
    ~LoopbackTcpListener();

    std::unique_ptr<OpenRCT2::Network::ITcpSocket> _listener;

private:
    uint16_t _port{};
};

/**
 * Lets the kernel complete the handshake but never accepts, reads from or replies to any
 * connection. A request sent to it stalls until the client gives up, which makes it suitable
 * for exercising client-side timeouts.
 */
class UnresponsiveTcpServer final : public LoopbackTcpListener
{
};

/**
 * Answers the first connection with HTTP 200 and sends the body one byte per interval, so a
 * transfer that keeps progressing can be observed outlasting a client-side timeout.
 */
class TrickleHttpServer final : public LoopbackTcpListener
{
public:
    TrickleHttpServer(std::string body, std::chrono::milliseconds interval);
    ~TrickleHttpServer();

private:
    void serve();
    std::unique_ptr<OpenRCT2::Network::ITcpSocket> acceptClient();

    std::string _body;
    std::chrono::milliseconds _interval;
    std::atomic<bool> _stop{ false };
    std::thread _thread;
};
