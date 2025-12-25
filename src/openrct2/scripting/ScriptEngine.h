/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

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

namespace OpenRCT2::GameActions
{
    class GameAction;
    class Result;
} // namespace OpenRCT2::GameActions

class FileWatcher;
class InteractiveConsole;

namespace OpenRCT2
{
    struct IPlatformEnvironment;
}

namespace OpenRCT2::Scripting
{
    static constexpr int32_t kPluginApiVersion = 110;

    // Versions marking breaking changes.
    static constexpr int32_t kApiVersionPeepDeprecation = 33;
    static constexpr int32_t kApiVersionG2Reorder = 63;
    static constexpr int32_t kApiVersionCustomActionArgs = 68;
    static constexpr int32_t kApiVersionNetworkIDs = 77;

    #ifndef DISABLE_NETWORK
    struct SocketDataBase;
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
        JSCallback Callback;
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
        JSValue _sharedStorage = JS_UNDEFINED;
        JSValue _parkStorage = JS_UNDEFINED;

        uint32_t _lastIntervalTimestamp{};
        std::map<IntervalHandle, ScriptInterval> _intervals;
        IntervalHandle _nextIntervalHandle = 1;

        std::unique_ptr<FileWatcher> _pluginFileWatcher;
        std::unordered_set<std::string> _changedPluginFiles;
        std::mutex _changedPluginFilesMutex;
        std::vector<std::function<void(std::shared_ptr<Plugin>)>> _pluginStoppedSubscriptions;

        struct ExtensionCallbacks
        {
            std::function<void(JSContext*)> newContext;
            std::function<void()> unregister;
        };

        std::vector<ExtensionCallbacks> _extensions;

        struct CustomActionInfo
        {
            std::shared_ptr<Plugin> Owner;
            std::string Name;
            JSCallback Query;
            JSCallback Execute;
        };

        std::unordered_map<std::string, CustomActionInfo> _customActions;
    #ifndef DISABLE_NETWORK
        std::vector<SocketDataBase*> _sockets;
    #endif

        void InitialiseContext(JSContext* ctx) const;

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
                if (metadata.Type == PluginType::Remote)
                {
                    res.push_back(plugin);
                }
            }
            return res;
        }

        void ClearParkStorage();
        std::string GetParkStorageAsJSON();
        void SetParkStorageFromJSON(const std::string& value, const std::string& filename);

        void Initialise();
        JSContext* CreateContext() const;
        void LoadTransientPlugins();
        void UnloadTransientPlugins();
        void StopUnloadRegisterAllPlugins();
        void Tick();
        std::future<void> Eval(const std::string& s);
        void ExecutePluginCall(
            const std::shared_ptr<Plugin>& plugin, JSValue func, const std::vector<JSValue>& args, bool isGameStateMutable,
            bool keepArgsAlive = false);
        JSValue ExecutePluginCall(
            const std::shared_ptr<Plugin>& plugin, JSValue func, JSValue thisValue, const std::vector<JSValue>& args,
            bool isGameStateMutable, bool keepArgsAlive = false, bool keepRetValueAlive = false);

        void LogPluginInfo(std::string_view message);
        void LogPluginInfo(const std::shared_ptr<Plugin>& plugin, std::string_view message);

        void SubscribeToPluginStoppedEvent(std::function<void(std::shared_ptr<Plugin>)> callback)
        {
            _pluginStoppedSubscriptions.push_back(callback);
        }

        void RegisterExtension(std::function<void(JSContext*)> newContext, std::function<void()> unregister)
        {
            _extensions.emplace_back(newContext, unregister);
            newContext(_replContext);
        }

        void AddNetworkPlugin(std::string_view code);
        void RemoveNetworkPlugins();

        [[nodiscard]] GameActions::Result QueryOrExecuteCustomGameAction(
            const GameActions::CustomAction& action, bool isExecute);
        bool RegisterCustomAction(
            const std::shared_ptr<Plugin>& plugin, std::string_view action, const JSCallback& query, const JSCallback& execute);
        void RunGameActionHooks(const GameActions::GameAction& action, GameActions::Result& result, bool isExecute);
        [[nodiscard]] std::pair<std::unique_ptr<GameActions::GameAction>, bool> CreateGameAction(
            JSContext* ctx, const std::string& actionid, JSValue args, const std::string& pluginName);
        [[nodiscard]] JSValue GameActionResultToJS(
            JSContext* ctx, const GameActions::GameAction& action, const GameActions::Result& result);

        void SaveSharedStorage();

        IntervalHandle AddInterval(
            const std::shared_ptr<Plugin>& plugin, int32_t delay, bool repeat, const JSCallback& callback);
        void RemoveInterval(const std::shared_ptr<Plugin>& plugin, IntervalHandle handle);

        static std::string_view ExpenditureTypeToString(ExpenditureType expenditureType);
        static ExpenditureType StringToExpenditureType(std::string_view expenditureType);

    #ifndef DISABLE_NETWORK
        void AddSocket(SocketDataBase* data);
        void RemoveSocket(SocketDataBase* data);
    #endif

    private:
        static void RegisterClasses(JSContext* ctx);
        static void UnregisterClasses();
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
        [[nodiscard]] static GameActions::Result JSToGameActionResult(JSContext* ctx, JSValue d);

        void InitSharedStorage();
        void LoadSharedStorage();

        IntervalHandle AllocateHandle();
        void UpdateIntervals();
        void RemoveIntervals(const std::shared_ptr<Plugin>& plugin);

        void UpdateSockets();
        void RemoveSockets(const std::shared_ptr<Plugin>& plugin);
    };

    bool IsGameStateMutable();
    int32_t GetTargetAPIVersion();

    std::string Stringify(JSContext* context, JSValue value);

    class ScBase
    {
    private:
        JSClassID classId = JS_INVALID_CLASS_ID;

    public:
        void Unregister()
        {
            classId = JS_INVALID_CLASS_ID;
        }

    protected:
        [[nodiscard]] JSValue MakeWithOpaque(JSContext* ctx, std::span<const JSCFunctionListEntry> classFuncs, void* opaque)
        {
            JSValue obj = JS_NewObjectClass(ctx, classId);
            if (JS_IsException(obj))
                throw std::runtime_error("Failed to create new object for class.");
            JS_SetOpaque(obj, opaque);

            // Note: Usually one would set a class prototype rather than setting the functions as properties on every creation.
            //       However, that causes the attached functions to not be "own properties" which make them a little less
            //       visible to the user, and also does not match the previous behaviour with the DukTape engine.
            JS_SetPropertyFunctionList(ctx, obj, classFuncs.data(), static_cast<int>(classFuncs.size()));
            return obj;
        }

        template<typename T>
        [[nodiscard]] T GetOpaque(JSValue obj) const
        {
            return static_cast<T>(JS_GetOpaque(obj, classId));
        }

        void RegisterBase(JSContext* ctx, const JSClassDef& classDef)
        {
            Guard::Assert(classId == JS_INVALID_CLASS_ID);
            // Note: Technically JS_NewClassID is meant to be called once during the lifetime of the program
            //       whereas JS_NewClass is meant to be called for each runtime.
            //       If we ever have any more runtimes, this flow would be wrong.
            //       More runtimes would also require refactoring the way values are passed to the JS code when
            //       calling callbacks.
            JSRuntime* rt = JS_GetRuntime(ctx);
            JS_NewClassID(rt, &classId);
            JS_NewClass(rt, classId, &classDef);
            JS_SetClassProto(ctx, classId, JS_NewObject(ctx));
        }

        void RegisterBaseStr(JSContext* ctx, const char* className)
        {
            RegisterBase(ctx, { className, nullptr, nullptr, nullptr, nullptr });
        }

        void RegisterBaseStr(JSContext* ctx, const char* className, JSClassFinalizer* finalizer)
        {
            RegisterBase(ctx, { className, finalizer, nullptr, nullptr, nullptr });
        }
    };

} // namespace OpenRCT2::Scripting

#endif
