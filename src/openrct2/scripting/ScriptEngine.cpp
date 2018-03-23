/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScriptEngine.h"
#include "../core/FileScanner.h"
#include "../core/Path.hpp"
#include "../interface/InteractiveConsole.h"
#include "../platform/Platform2.h"
#include "../PlatformEnvironment.h"
#include <dukglue/dukglue.h>
#include <duktape.h>
#include <iostream>
#include <stdexcept>

#include "ScConsole.hpp"
#include "ScContext.hpp"
#include "ScDisposable.hpp"
#include "ScMap.hpp"
#include "ScPark.hpp"
#include "ScTile.hpp"
#include "ScThing.hpp"

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

static std::string Stringify(duk_context * ctx, duk_idx_t idx);

ScriptEngine::ScriptEngine(InteractiveConsole& console, IPlatformEnvironment& env) :
    _console(console),
    _env(env),
    _hookEngine(_execInfo)
{
}

ScriptEngine::~ScriptEngine()
{
    duk_destroy_heap(_context);
}

void ScriptEngine::Initialise()
{
    _context = duk_create_heap_default();
    if (_context == nullptr)
    {
        throw std::runtime_error("Unable to initialise duktape context.");
    }

    auto ctx = _context;
    ScConsole::Register(ctx);
    ScContext::Register(ctx);
    ScDisposable::Register(ctx);
    ScMap::Register(ctx);
    ScPark::Register(ctx);
    ScTile::Register(ctx);
    ScTileElement::Register(ctx);
    ScThing::Register(ctx);

    dukglue_register_global(ctx, std::make_shared<ScConsole>(_console), "console");
    dukglue_register_global(ctx, std::make_shared<ScContext>(_execInfo, _hookEngine), "context");
    dukglue_register_global(ctx, std::make_shared<ScMap>(ctx), "map");
    dukglue_register_global(ctx, std::make_shared<ScPark>(), "park");

    LoadPlugins();
    StartPlugins();
}

void ScriptEngine::LoadPlugins()
{
    auto base = _env.GetDirectoryPath(DIRBASE::USER, DIRID::PLUGIN);
    auto pattern = Path::Combine(base, "*.js");
    auto scanner = std::unique_ptr<IFileScanner>(Path::ScanDirectory(pattern, true));
    while (scanner->Next())
    {
        auto path = std::string(scanner->GetPath());
        try
        {
            auto plugin = std::make_shared<Plugin>(_context, path);
            _execInfo.SetCurrentPlugin(plugin);
            plugin->Load();
            plugin->EnableHotReload();
            _plugins.push_back(std::move(plugin));
        }
        catch (const std::exception &e)
        {
            _console.WriteLineError(e.what());
        }
    }
    _execInfo.SetCurrentPlugin(nullptr);
}

void ScriptEngine::AutoReloadPlugins()
{
    for (auto& plugin : _plugins)
    {
        if (plugin->ShouldHotReload())
        {
            try
            {
                _hookEngine.UnsubscribeAll(plugin);
                _execInfo.SetCurrentPlugin(plugin);
                plugin->Load();
                plugin->Start();
            }
            catch (const std::exception &e)
            {
                _console.WriteLineError(e.what());
            }
        }
    }
    _execInfo.SetCurrentPlugin(nullptr);
}

void ScriptEngine::StartPlugins()
{
    for (auto& plugin : _plugins)
    {
        _execInfo.SetCurrentPlugin(plugin);
        try
        {
            plugin->Start();
        }
        catch (const std::exception &e)
        {
            _console.WriteLineError(e.what());
        }
    }
    _execInfo.SetCurrentPlugin(nullptr);
}

void ScriptEngine::Update()
{
    if (!_initialised)
    {
        Initialise();
        _initialised = true;
    }
    while (_evalQueue.size() > 0)
    {
        auto item = std::move(_evalQueue.front());
        _evalQueue.pop();
        auto promise = std::move(std::get<0>(item));
        auto command = std::move(std::get<1>(item));
        if (duk_peval_string(_context, command.c_str()) != 0)
        {
            std::string result = std::string(duk_safe_to_string(_context, -1));
            _console.WriteLineError(result);
        }
        else
        {
            std::string result = Stringify(_context, -1);
            _console.WriteLine(result);
        }
        duk_pop(_context);
        // Signal the promise so caller can continue
        promise.set_value();
    }

    auto tick = Platform::GetTicks();
    if (tick - _lastHotReloadCheckTick > 1000)
    {
        AutoReloadPlugins();
        _lastHotReloadCheckTick = tick;
    }
}

std::future<void> ScriptEngine::Eval(const std::string &s)
{
    std::promise<void> barrier;
    auto future = barrier.get_future();
    _evalQueue.emplace(std::move(barrier), s);
    return future;
}

static std::string Stringify(duk_context * ctx, duk_idx_t idx)
{
    auto type = duk_get_type(ctx, idx);
    if (type == DUK_TYPE_OBJECT && !duk_is_function(ctx, idx))
    {
        return duk_json_encode(ctx, idx);
    }
    else
    {
        return duk_safe_to_string(ctx, idx);
    }
}
