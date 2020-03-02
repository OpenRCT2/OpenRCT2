/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __ENABLE_SCRIPTING__

#    include "../common.h"
#    include "../core/FileWatcher.h"
#    include "HookEngine.h"
#    include "Plugin.h"

#    include <future>
#    include <memory>
#    include <mutex>
#    include <queue>
#    include <string>
#    include <unordered_map>
#    include <unordered_set>
#    include <vector>

struct duk_hthread;
typedef struct duk_hthread duk_context;

class GameActionResult;
class FileWatcher;
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
        bool _isGameStateMutable{};

    public:
        class PluginScope
        {
        private:
            ScriptExecutionInfo& _execInfo;
            std::shared_ptr<Plugin> _plugin;

        public:
            PluginScope(ScriptExecutionInfo& execInfo, std::shared_ptr<Plugin> plugin, bool isGameStateMutable)
                : _execInfo(execInfo)
                , _plugin(plugin)
            {
                _execInfo._plugin = plugin;
                _execInfo._isGameStateMutable = isGameStateMutable;
            }
            PluginScope(const PluginScope&) = delete;
            ~PluginScope()
            {
                _execInfo._plugin = nullptr;
                _execInfo._isGameStateMutable = false;
            }
        };

        std::shared_ptr<Plugin> GetCurrentPlugin()
        {
            return _plugin;
        }

        bool IsGameStateMutable()
        {
            return _isGameStateMutable;
        }
    };

    class DukContext
    {
    private:
        duk_context* _context{};

    public:
        DukContext();
        DukContext(DukContext&) = delete;
        DukContext(DukContext&& src) noexcept
            : _context(std::move(src._context))
        {
        }
        ~DukContext();

        operator duk_context*()
        {
            return _context;
        }
    };

    class ScriptEngine
    {
    private:
        InteractiveConsole& _console;
        IPlatformEnvironment& _env;
        DukContext _context;
        bool _initialised{};
        bool _pluginsLoaded{};
        bool _pluginsStarted{};
        std::queue<std::tuple<std::promise<void>, std::string>> _evalQueue;
        std::vector<std::shared_ptr<Plugin>> _plugins;
        uint32_t _lastHotReloadCheckTick{};
        HookEngine _hookEngine;
        ScriptExecutionInfo _execInfo;

        std::unique_ptr<FileWatcher> _pluginFileWatcher;
        std::unordered_set<std::string> _changedPluginFiles;
        std::mutex _changedPluginFilesMutex;
        std::vector<std::function<void(std::shared_ptr<Plugin>)>> _pluginStoppedSubscriptions;

        struct CustomAction
        {
            std::shared_ptr<Plugin> Plugin;
            std::string Name;
            DukValue Query;
            DukValue Execute;
        };

        std::unordered_map<std::string, CustomAction> _customActions;

    public:
        ScriptEngine(InteractiveConsole& console, IPlatformEnvironment& env);
        ScriptEngine(ScriptEngine&) = delete;

        duk_context* GetContext()
        {
            return _context;
        }
        HookEngine& GetHookEngine()
        {
            return _hookEngine;
        }
        ScriptExecutionInfo& GetExecInfo()
        {
            return _execInfo;
        }
        std::vector<std::shared_ptr<Plugin>>& GetPlugins()
        {
            return _plugins;
        }

        void LoadPlugins();
        void UnloadPlugins();
        void Update();
        std::future<void> Eval(const std::string& s);
        DukValue ExecutePluginCall(
            const std::shared_ptr<Plugin>& plugin, const DukValue& func, const std::vector<DukValue>& args,
            bool isGameStateMutable);

        void LogPluginInfo(const std::shared_ptr<Plugin>& plugin, const std::string_view& message);

        void SubscribeToPluginStoppedEvent(std::function<void(std::shared_ptr<Plugin>)> callback)
        {
            _pluginStoppedSubscriptions.push_back(callback);
        }

        void AddNetworkPlugin(const std::string_view& code);

        std::unique_ptr<GameActionResult> QueryOrExecuteCustomGameAction(
            const std::string_view& id, const std::string_view& args, bool isExecute);
        bool RegisterCustomAction(
            const std::shared_ptr<Plugin>& plugin, const std::string_view& action, const DukValue& query,
            const DukValue& execute);

    private:
        void Initialise();
        void StartPlugins();
        void StopPlugins();
        void LoadPlugin(const std::string& path);
        void LoadPlugin(std::shared_ptr<Plugin>& plugin);
        void StopPlugin(std::shared_ptr<Plugin> plugin);
        bool ShouldLoadScript(const std::string& path);
        bool ShouldStartPlugin(const std::shared_ptr<Plugin>& plugin);
        void SetupHotReloading();
        void AutoReloadPlugins();
        void ProcessREPL();
        void RemoveCustomGameActions(const std::shared_ptr<Plugin>& plugin);
        std::unique_ptr<GameActionResult> DukToGameActionResult(const DukValue& d);
    };

    bool IsGameStateMutable();
    void ThrowIfGameStateNotMutable();
    std::string Stringify(const DukValue& value);

} // namespace OpenRCT2::Scripting

#endif
