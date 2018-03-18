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
#include "../PlatformEnvironment.h"
#include <dukglue/dukglue.h>
#include <duktape.h>
#include <iostream>
#include <stdexcept>

#include "ScConsole.hpp"
#include "ScPark.hpp"

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

static std::string Stringify(duk_context * ctx, duk_idx_t idx);

ScriptEngine::ScriptEngine(InteractiveConsole& console, IPlatformEnvironment& env) :
    _console(console),
    _env(env)
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
    ScPark::Register(ctx);

    dukglue_register_global(ctx, std::make_shared<ScConsole>(_console), "console");
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
            Plugin p(_context, path);
            p.Load();
            _plugins.push_back(std::move(p));
        }
        catch (const std::exception &e)
        {
            _console.WriteLineError(e.what());
        }
    }
}

void ScriptEngine::StartPlugins()
{
    for (auto& plugin : _plugins)
    {
        plugin.Start();
    }
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
