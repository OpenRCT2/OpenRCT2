/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "NetworkKey.h"
#include "NetworkPacket.h"
#include "NetworkTypes.h"
#include "TcpSocket.h"

#include <list>
#include <memory>
#include <string_view>
#include <vector>

class NetworkConnection final
{
public:
    NetworkConnection();
    NetworkConnection(std::unique_ptr<ITcpSocket>&& sock);
    ~NetworkConnection();

    void ConnectAsync(const std::string& host, uint16_t port);
    void Close();
    void Flush();

    SOCKET_STATUS GetStatus() const;
    const char* GetError() const;

private:
    std::unique_ptr<ITcpSocket> _sock;
};
