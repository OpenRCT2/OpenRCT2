#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <dukglue/dukglue.h>

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
        struct HotReloadData
        {
            int FileDesc{};
            int WatchDesc{};
        };

        duk_context * _context;
        std::string _path;
        PluginMetadata _metadata;
        HotReloadData _hotReloadData;
        bool _hotReloadEnabled{};

    public:
        Plugin() { }
        Plugin(duk_context * context, const std::string &path);
        Plugin(const Plugin&) = delete;
        Plugin(Plugin&&) = delete;
        ~Plugin();

        void Load();
        void Start();
        void Update();

        void EnableHotReload();
        bool ShouldHotReload();
        void DisableHotReload();

    private:
        static PluginMetadata GetMetadata(const DukValue& dukMetadata);
    };
}
