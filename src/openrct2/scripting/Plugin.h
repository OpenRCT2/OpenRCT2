/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Duktape.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace OpenRCT2::Scripting
{
    enum class PluginType
    {
        /**
         * Scripts that can run on any client with no impact on the game state.
         */
        Client,

        /**
         * Scripts that can run on servers with no impact on the game state and will not
         * be uploaded to clients.
         */
        Server,

        /**
         * Scripts that can run on servers and will be uploaded to clients with ability to
         * modify game state in certain contexts.
         */
        ServerClient,
    };

    struct PluginMetadata
    {
        std::string Name;
        std::string Version;
        std::vector<std::string> Authors;
        PluginType Type;
        int32_t MinApiVersion{};
        DukValue Main;
    };

    class Plugin
    {
    private:
        duk_context* _context{};
        std::string _path;
        PluginMetadata _metadata;
        bool _hasStarted{};

    public:
        std::string GetPath() const
        {
            return _path;
        };

        const PluginMetadata& GetMetadata() const
        {
            return _metadata;
        }

        bool HasStarted() const
        {
            return _hasStarted;
        }

        Plugin()
        {
        }
        Plugin(duk_context* context, const std::string& path);
        Plugin(const Plugin&) = delete;
        Plugin(Plugin&&) = delete;

        void Load();
        void Start();
        void Stop();
        void Update();

    private:
        static PluginMetadata GetMetadata(const DukValue& dukMetadata);
        static PluginType ParsePluginType(const std::string_view& type);
    };
} // namespace OpenRCT2::Scripting
