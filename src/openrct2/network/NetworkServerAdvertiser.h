/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>

enum class ADVERTISE_STATUS
{
    DISABLED,
    UNREGISTERED,
    REGISTERED,
};

struct INetworkServerAdvertiser
{
    virtual ~INetworkServerAdvertiser()
    {
    }

    virtual ADVERTISE_STATUS GetStatus() const = 0;
    virtual void Update() = 0;
};

[[nodiscard]] std::unique_ptr<INetworkServerAdvertiser> CreateServerAdvertiser(uint16_t port);
