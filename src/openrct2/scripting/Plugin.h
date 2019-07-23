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
#include <vector>

namespace OpenRCT2::Scripting
{
    struct PluginMetadata
    {
        std::string Name;
        std::string Version;
        std::vector<std::string> Authors;
        DukValue Main;
    };

    class Plugin
    {
    private:
        duk_context* _context;
        std::string _path;
        PluginMetadata _metadata;

    public:
        std::string GetPath() const
        {
            return _path;
        };

        Plugin()
        {
        }
        Plugin(duk_context* context, const std::string& path);
        Plugin(const Plugin&) = delete;
        Plugin(Plugin&&) = delete;

        void Load();
        void Start();
        void Update();

    private:
        static PluginMetadata GetMetadata(const DukValue& dukMetadata);
    };
} // namespace OpenRCT2::Scripting
