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

#    include "Duktape.hpp"

#    include <memory>
#    include <string>
#    include <string_view>
#    include <vector>

namespace OpenRCT2::Scripting
{
    enum class PluginType
    {
        /**
         * Scripts that can run on servers or clients with no impact on the game state and will not
         * be uploaded to clients.
         */
        Local,

        /**
         * Scripts that can run on servers and will be uploaded to clients with ability to
         * modify game state in certain contexts.
         */
        Remote,

        /**
         * Scripts that run when the game starts and only unload explicitly rather than when the
         * park changes.
         */
        Intransient,
    };

    struct PluginMetadata
    {
        std::string Name;
        std::string Version;
        std::vector<std::string> Authors;
        PluginType Type{};
        int32_t MinApiVersion{};
        std::optional<int32_t> TargetApiVersion{};
        DukValue Main;
    };

    class Plugin
    {
    private:
        duk_context* _context{};
        std::string _path;
        PluginMetadata _metadata{};
        std::string _code;
        bool _hasLoaded{};
        bool _hasStarted{};
        bool _isStopping{};

    public:
        std::string_view GetPath() const
        {
            return _path;
        };

        bool HasPath() const
        {
            return !_path.empty();
        }

        const PluginMetadata& GetMetadata() const
        {
            return _metadata;
        }

        const std::string& GetCode() const
        {
            return _code;
        }

        bool HasStarted() const
        {
            return _hasStarted;
        }

        bool IsStopping() const
        {
            return _isStopping;
        }

        bool IsLoaded() const
        {
            return _hasLoaded;
        }

        int32_t GetTargetAPIVersion() const;

        Plugin() = default;
        Plugin(duk_context* context, std::string_view path);
        Plugin(const Plugin&) = delete;
        Plugin(Plugin&&) = delete;

        void SetCode(std::string_view code);
        void Load();
        void Start();
        void StopBegin();
        void StopEnd();

        void ThrowIfStopping() const;
        void Unload();

        bool IsTransient() const;

    private:
        void LoadCodeFromFile();

        static PluginMetadata GetMetadata(const DukValue& dukMetadata);
        static PluginType ParsePluginType(std::string_view type);
        static void CheckForLicence(const DukValue& dukLicence, std::string_view pluginName);
    };
} // namespace OpenRCT2::Scripting

#endif
