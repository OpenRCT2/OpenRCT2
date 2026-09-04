/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/JsonFwd.hpp"

#include <cstdint>
#include <functional>
#include <memory>

namespace OpenRCT2::Network
{
    enum class AdvertiseStatus
    {
        disabled,
        unregistered,
        registering,
        registered,
    };

    struct INetworkServerAdvertiser
    {
        virtual ~INetworkServerAdvertiser()
        {
        }

        virtual AdvertiseStatus getStatus() const = 0;
        virtual void update() = 0;

        // Detach any in-flight master-server request and stop completion callbacks from touching
        // this object, so destroying the advertiser never blocks the calling thread. Safe to call
        // more than once; the destructor calls it too.
        virtual void beginShutdown()
        {
        }
    };

    [[nodiscard]] std::unique_ptr<INetworkServerAdvertiser> CreateServerAdvertiser(uint16_t port);
    // wanEnabledProvider decides per-tick whether to register/heartbeat with the master server. When
    // empty, the config's network.advertise is used (co-op default). The competitive session passes
    // its own predicate so competition hosting has an independent "public game list" preference.
    [[nodiscard]] std::unique_ptr<INetworkServerAdvertiser> CreateServerAdvertiser(
        uint16_t port, std::function<json_t()> serverInfoProvider, std::function<uint32_t()> playerCountProvider,
        std::function<json_t()> gameInfoProvider, std::function<bool()> wanEnabledProvider = {});
} // namespace OpenRCT2::Network
