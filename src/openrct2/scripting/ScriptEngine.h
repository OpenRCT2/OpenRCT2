/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../actions/CustomAction.h"
    #include "../core/FileWatcher.h"
    #include "../management/Finance.h"
    #include "HookEngine.h"
    #include "Plugin.h"

    #include <future>
    #include <list>
    #include <memory>
    #include <mutex>
    #include <queue>
    #include <quickjs.h>
    #include <string>
    #include <unordered_map>
    #include <unordered_set>
    #include <vector>

class GameAction;
namespace OpenRCT2::GameActions
{
    class Result;
}
class FileWatcher;
class InteractiveConsole;

namespace OpenRCT2
{
    struct IPlatformEnvironment;
}

namespace OpenRCT2::Scripting
{
    static constexpr int32_t OPENRCT2_PLUGIN_API_VERSION = 103;

    // Versions marking breaking changes.
    static constexpr int32_t API_VERSION_33_PEEP_DEPRECATION = 33;
    static constexpr int32_t API_VERSION_63_G2_REORDER = 63;
    static constexpr int32_t API_VERSION_68_CUSTOM_ACTION_ARGS = 68;
    static constexpr int32_t API_VERSION_77_NETWORK_IDS = 77;

    #ifndef DISABLE_NETWORK
    class ScSocketBase;
    #endif

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

            std::shared_ptr<Plugin> _backupPlugin;
            bool _backupIsGameStateMutable;

        public:
            PluginScope(ScriptExecutionInfo& execInfo, std::shared_ptr<Plugin> plugin, bool isGameStateMutable)
                : _execInfo(execInfo)
                , _plugin(plugin)
            {
                _backupPlugin = _execInfo._plugin;
                _backupIsGameStateMutable = _execInfo._isGameStateMutable;

                _execInfo._plugin = std::move(plugin);
                _execInfo._isGameStateMutable = isGameStateMutable;
            }
            PluginScope(const PluginScope&) = delete;
            ~PluginScope()
            {
                _execInfo._plugin = _backupPlugin;
                _execInfo._isGameStateMutable = _backupIsGameStateMutable;
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

    using IntervalHandle = uint32_t;
    struct ScriptInterval
    {
        std::shared_ptr<Plugin> Owner;
        uint32_t Delay{};
        int64_t LastTimestamp{};
        JSValue Callback;
        bool Repeat{};
        bool Deleted{};
    };

    class ScriptEngine
    {
    private:
        InteractiveConsole& _console;
        IPlatformEnvironment& _env;
        static JSRuntime* _runtime;
        JSContext* _replContext = nullptr;
        bool _initialised{};
        bool _hotReloadingInitialised{};
        bool _transientPluginsEnabled{};
        bool _transientPluginsStarted{};
        bool _intransientPluginsStarted{};
        std::queue<std::tuple<std::promise<void>, std::string>> _evalQueue;
        std::vector<std::shared_ptr<Plugin>> _plugins;
        uint32_t _lastHotReloadCheckTick{};
        HookEngine _hookEngine;
        ScriptExecutionInfo _execInfo;
        JSValue _sharedStorage{};
        JSValue _parkStorage{};

        uint32_t _lastIntervalTimestamp{};
        std::map<IntervalHandle, ScriptInterval> _intervals;
        IntervalHandle _nextIntervalHandle = 1;

        std::unique_ptr<FileWatcher> _pluginFileWatcher;
        std::unordered_set<std::string> _changedPluginFiles;
        std::mutex _changedPluginFilesMutex;
        std::vector<std::function<void(std::shared_ptr<Plugin>)>> _pluginStoppedSubscriptions;

        struct CustomActionInfo
        {
            std::shared_ptr<Plugin> Owner;
            std::string Name;
            JSValue Query;
            JSValue Execute;
        };

        std::unordered_map<std::string, CustomActionInfo> _customActions;
    #ifndef DISABLE_NETWORK
        std::list<std::shared_ptr<ScSocketBase>> _sockets;
    #endif

    public:
        ScriptEngine(InteractiveConsole& console, IPlatformEnvironment& env);
        ScriptEngine(ScriptEngine&) = delete;
        ~ScriptEngine();

        JSContext* GetContext()
        {
            return _replContext;
        }
        HookEngine& GetHookEngine()
        {
            return _hookEngine;
        }
        ScriptExecutionInfo& GetExecInfo()
        {
            return _execInfo;
        }
        JSValue GetSharedStorage()
        {
            return _sharedStorage;
        }
        JSValue GetParkStorage()
        {
            return _parkStorage;
        }
        std::vector<std::shared_ptr<Plugin>>& GetPlugins()
        {
            return _plugins;
        }

        std::vector<std::shared_ptr<Plugin>> GetRemotePlugins()
        {
            std::vector<std::shared_ptr<Plugin>> res;
            for (const auto& plugin : _plugins)
            {
                const auto& metadata = plugin->GetMetadata();
                if (metadata.Type == OpenRCT2::Scripting::PluginType::Remote)
                {
                    res.push_back(plugin);
                }
            }
            return res;
        }

        void ClearParkStorage();
        std::string GetParkStorageAsJSON();
        void SetParkStorageFromJSON(std::string_view value);

        void Initialise();
        void InitialiseContext(JSContext* ctx) const;
        void LoadTransientPlugins();
        void UnloadTransientPlugins();
        void StopUnloadRegisterAllPlugins();
        void Tick();
        std::future<void> Eval(const std::string& s);
        JSValue ExecutePluginCall(
            const std::shared_ptr<Plugin>& plugin, JSValue func, const std::vector<JSValue>& args, bool isGameStateMutable);
        JSValue ExecutePluginCall(
            std::shared_ptr<Plugin> plugin, JSValue func, JSValue thisValue, const std::vector<JSValue>& args,
            bool isGameStateMutable);

        void LogPluginInfo(std::string_view message);
        void LogPluginInfo(const std::shared_ptr<Plugin>& plugin, std::string_view message);

        void SubscribeToPluginStoppedEvent(std::function<void(std::shared_ptr<Plugin>)> callback)
        {
            _pluginStoppedSubscriptions.push_back(callback);
        }

        void AddNetworkPlugin(std::string_view code);
        void RemoveNetworkPlugins();

        [[nodiscard]] GameActions::Result QueryOrExecuteCustomGameAction(const CustomAction& action, bool isExecute);
        bool RegisterCustomAction(
            const std::shared_ptr<Plugin>& plugin, std::string_view action, JSValue query, JSValue execute);
        void RunGameActionHooks(const GameAction& action, GameActions::Result& result, bool isExecute);
        [[nodiscard]] std::unique_ptr<GameAction> CreateGameAction(
            const std::string& actionid, JSValue args, const std::string& pluginName);
        [[nodiscard]] JSValue GameActionResultToDuk(const GameAction& action, const GameActions::Result& result);

        void SaveSharedStorage();

        IntervalHandle AddInterval(const std::shared_ptr<Plugin>& plugin, int32_t delay, bool repeat, JSValue callback);
        void RemoveInterval(const std::shared_ptr<Plugin>& plugin, IntervalHandle handle);

        static std::string_view ExpenditureTypeToString(ExpenditureType expenditureType);
        static ExpenditureType StringToExpenditureType(std::string_view expenditureType);

    #ifndef DISABLE_NETWORK
        void AddSocket(const std::shared_ptr<ScSocketBase>& socket);
    #endif

    private:
        static void RegisterConstants(JSContext* ctx);
        void RefreshPlugins();
        std::vector<std::string> GetPluginFiles() const;
        void UnregisterPlugin(std::string_view path);
        void RegisterPlugin(std::string_view path);
        void CheckAndStartPlugins();
        void StartIntransientPlugins();
        void StartTransientPlugins();
        void LoadPlugin(const std::string& path);
        void LoadPlugin(std::shared_ptr<Plugin>& plugin);
        void UnloadPlugin(std::shared_ptr<Plugin>& plugin);
        void StartPlugin(std::shared_ptr<Plugin> plugin);
        void StopPlugin(std::shared_ptr<Plugin> plugin);
        void ReloadPlugin(std::shared_ptr<Plugin> plugin);
        static bool ShouldLoadScript(std::string_view path);
        bool ShouldStartPlugin(const std::shared_ptr<Plugin>& plugin);
        void SetupHotReloading();
        void DoAutoReloadPluginCheck();
        void AutoReloadPlugins();
        void ProcessREPL();
        void RemoveCustomGameActions(const std::shared_ptr<Plugin>& plugin);
        [[nodiscard]] GameActions::Result DukToGameActionResult(JSValue d);

        void InitSharedStorage();
        void LoadSharedStorage();

        IntervalHandle AllocateHandle();
        void UpdateIntervals();
        void RemoveIntervals(const std::shared_ptr<Plugin>& plugin);

        void UpdateSockets();
        void RemoveSockets(const std::shared_ptr<Plugin>& plugin);
    };

    bool IsGameStateMutable();
    void ThrowIfGameStateNotMutable();
    int32_t GetTargetAPIVersion();

    std::string Stringify(JSContext* context, JSValue value);
    std::string ProcessString(JSValue value);

    class ScBase
    {
    private:
        JSClassID classId = JS_INVALID_CLASS_ID;
        virtual const JSClassDef* GetClassDef() = 0;
        virtual std::span<const JSCFunctionListEntry> GetClassFuncs() = 0;

    protected:
        [[nodiscard]] JSValue MakeWithOpaque(JSContext* ctx, void* opaque)
        {
            JSValue obj = JS_NewObjectClass(ctx, classId);
            if (JS_IsException(obj))
                throw std::runtime_error("Failed to create new object for class.");
            JS_SetOpaque(obj, opaque);

            // Note: Usually one would set a class prototype rather than setting the functions as properties on every creation.
            //       However, that causes the attached functions to not be "own properties" which make them a little less
            //       visible to the user, and also does not match the previous behaviour with the DukTape engine.
            const auto funcs = GetClassFuncs();
            JS_SetPropertyFunctionList(ctx, obj, funcs.data(), funcs.size());
            return obj;
        }

        template<typename T>
        [[nodiscard]] T GetOpaque(JSValue obj) const
        {
            return static_cast<T>(JS_GetOpaque(obj, classId));
        }

    public:
        virtual ~ScBase() = default;
        void Register(JSContext* ctx)
        {
            if (classId == JS_INVALID_CLASS_ID)
            {
                // Note: Technically JS_NewClassID is meant to be called once during the lifetime of the program
                //       whereas JS_NewClass is meant to be called for each runtime.
                //       If we ever have any more runtimes, this flow would be wrong.
                JSRuntime* rt = JS_GetRuntime(ctx);
                JS_NewClassID(rt, &classId);
                JS_NewClass(rt, classId, GetClassDef());
            }
        }
    };

} // namespace OpenRCT2::Scripting

#endif
