/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "HookEngine.h"
#include "Plugin.h"
#include <future>
#include <memory>
#include <queue>
#include <string>

struct duk_hthread;
typedef struct duk_hthread duk_context;

class InteractiveConsole;

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

namespace OpenRCT2::Scripting
{
    class ScriptExecutionInfo
    {
    private:
        std::shared_ptr<Plugin> _plugin;

    public:
        class PluginScope
        {
        private:
            ScriptExecutionInfo& _execInfo;
            std::shared_ptr<Plugin> _plugin;

        public:
            PluginScope(ScriptExecutionInfo& execInfo, std::shared_ptr<Plugin> plugin)
                : _execInfo(execInfo),
                  _plugin(plugin)
            {
                _execInfo._plugin = plugin;
            }
            PluginScope(const PluginScope&) = delete;
            ~PluginScope()
            {
                _execInfo._plugin = nullptr;
            }
        };

        std::shared_ptr<Plugin> GetCurrentPlugin() { return _plugin; }
    };

    class ScriptEngine
    {
    private:
        InteractiveConsole& _console;
        IPlatformEnvironment& _env;
        bool _initialised{};
        duk_context * _context{};
        std::queue<std::tuple<std::promise<void>, std::string>> _evalQueue;
        std::vector<std::shared_ptr<Plugin>> _plugins;
        uint32 _lastHotReloadCheckTick{};
        HookEngine _hookEngine;
        ScriptExecutionInfo _execInfo;

    public:
        ScriptEngine(InteractiveConsole& console, IPlatformEnvironment& env);
        ScriptEngine(ScriptEngine&) = delete;
        ~ScriptEngine();

        HookEngine& GetHookEngine() { return _hookEngine; }

        void Update();
        std::future<void> Eval(const std::string &s);

    private:
        void Initialise();
        void LoadPlugins();
        void StartPlugins();
        void AutoReloadPlugins();
    };
}
