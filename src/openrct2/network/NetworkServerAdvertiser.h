/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <memory>
namespace OpenRCT2
{
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

        virtual ADVERTISE_STATUS GetStatus() const abstract;
        virtual void Update() abstract;
    };

    [[nodiscard]] std::unique_ptr<INetworkServerAdvertiser> CreateServerAdvertiser(uint16_t port);
} // namespace OpenRCT2
