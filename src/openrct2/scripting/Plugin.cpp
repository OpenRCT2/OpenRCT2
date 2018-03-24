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

#include "Plugin.h"
#include <dukglue/dukglue.h>
#include <duktape.h>
#include <algorithm>
#include <fstream>
#include <memory>

#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>

using namespace OpenRCT2::Scripting;

Plugin::Plugin(duk_context * context, const std::string &path)
    : _context(context),
      _path(path)
{
}

Plugin::~Plugin()
{
    DisableHotReload();
}

void Plugin::Load()
{
    std::string projectedVariables = "console,context,map,park,ui";
    std::string code;
    {
        std::ifstream fs(_path);
        if (fs.is_open())
        {
            fs.seekg(0, std::ios::end);   
            code.reserve(fs.tellg());
            fs.seekg(0, std::ios::beg);
            code.assign(
                std::istreambuf_iterator<char>(fs),
                std::istreambuf_iterator<char>());
        }
    }
    // Wrap the script in a function and pass the global objects as variables
    // so that if the script modifies them, they are not modified for other scripts.
    code = "(function(" + projectedVariables + "){" + code + "})(" + projectedVariables + ");";
    auto flags = DUK_COMPILE_EVAL | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_NOFILENAME;
    auto result = duk_eval_raw(_context, code.c_str(), code.size(), flags);
    if (result != DUK_ERR_NONE)
    {
        auto val = std::string(duk_safe_to_string(_context, -1));
        duk_pop(_context);
        throw std::runtime_error("Failed to load plug-in script: " + val);
    }

    _metadata = GetMetadata(DukValue::take_from_stack(_context));
}

void Plugin::Start()
{
    const auto& mainFunc = _metadata.Main;
    if (mainFunc.context() == nullptr)
    {
        throw std::runtime_error("No main function specified.");
    }

    mainFunc.push();
    auto result = duk_pcall(_context, 0);
    if (result != DUK_ERR_NONE)
    {
        auto val = std::string(duk_safe_to_string(_context, -1));
        duk_pop(_context);
        throw std::runtime_error("[" + _metadata.Name + "] " + val);
    }
    duk_pop(_context);
}

void Plugin::Update()
{
}

void Plugin::EnableHotReload()
{
    auto fd = inotify_init();
    if (fd >= 0)
    {
        // Mark file as non-blocking
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);

        auto wd = inotify_add_watch(fd, _path.c_str(), IN_CLOSE_WRITE);
        if (wd >= 0)
        {
            _hotReloadData.FileDesc = fd;
            _hotReloadData.WatchDesc = wd;
            _hotReloadEnabled = true;
        }
        else
        {
            close(fd);
        }
    }
}

bool Plugin::ShouldHotReload()
{
    if (_hotReloadEnabled)
    {
        std::vector<char> eventData;
        eventData.resize(1024);

        auto length = read(_hotReloadData.FileDesc, eventData.data(), eventData.size());
        int offset = 0;
        while (offset < length)
        {
            auto e = (inotify_event*)&eventData[offset];
            if ((e->mask & IN_CLOSE_WRITE) && !(e->mask & IN_ISDIR))
            {
                return true;
            }
            offset += sizeof(inotify_event) + e->len;
        }
    }
    return false;
}

void Plugin::DisableHotReload()
{
    if (_hotReloadEnabled)
    {
        inotify_rm_watch(_hotReloadData.FileDesc, _hotReloadData.WatchDesc);
        close(_hotReloadData.FileDesc);
        _hotReloadData = HotReloadData();
        _hotReloadEnabled = false;
    }
}

PluginMetadata Plugin::GetMetadata(const DukValue& dukMetadata)
{
    PluginMetadata metadata;
    if (dukMetadata.type() == DukValue::Type::OBJECT)
    {
        metadata.Name = dukMetadata["name"].as_string();
        metadata.Version = dukMetadata["version"].as_string();

        auto dukAuthors = dukMetadata["authors"];
        dukAuthors.push();
        if (dukAuthors.is_array())
        {
            auto elements = dukAuthors.as_array();
            std::transform(
                elements.begin(),
                elements.end(),
                std::back_inserter(metadata.Authors),
                [](const DukValue& v) { return v.as_string(); });
        }
        else
        {
            metadata.Authors = { dukAuthors.as_string() };
        }
        metadata.Main = dukMetadata["main"];
    }
    return metadata;
}
