/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "NetworkBase.h"
#include "TcpSocket.h"

// The Network class will use this by default.
class NetworkStub : public NetworkBase
{
public:
    virtual NETWORK_MODE GetMode() const override
    {
        return NETWORK_MODE::NONE;
    }
    virtual bool Startup() override
    {
        return true;
    }
    virtual bool Shutdown() override
    {
        return true;
    }
    virtual void Close() override
    {
    }
    virtual void Update() override
    {
    }
    virtual void Flush() override
    {
    }
};
