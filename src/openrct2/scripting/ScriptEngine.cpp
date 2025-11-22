/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScriptEngine.h"

    #include "../Context.h"
    #include "../PlatformEnvironment.h"
    #include "../actions/BannerPlaceAction.h"
    #include "../actions/CustomAction.h"
    #include "../actions/GameAction.h"
    #include "../actions/LargeSceneryPlaceAction.h"
    #include "../actions/RideCreateAction.h"
    #include "../actions/StaffHireNewAction.h"
    #include "../actions/WallPlaceAction.h"
    #include "../config/Config.h"
    #include "../core/Console.hpp"
    #include "../core/EnumMap.hpp"
    #include "../core/File.h"
    #include "../core/FileScanner.h"
    #include "../core/Path.hpp"
    #include "../interface/InteractiveConsole.h"
    #include "../platform/Platform.h"
    #include "../profiling/Profiling.h"
    #include "bindings/entity/ScBalloon.hpp"
    #include "bindings/entity/ScEntity.hpp"
    #include "bindings/entity/ScGuest.hpp"
    #include "bindings/entity/ScLitter.hpp"
    #include "bindings/entity/ScMoneyEffect.hpp"
    #include "bindings/entity/ScParticle.hpp"
    #include "bindings/entity/ScPeep.hpp"
    #include "bindings/entity/ScStaff.hpp"
    #include "bindings/entity/ScVehicle.hpp"
    #include "bindings/game/ScCheats.hpp"
    #include "bindings/game/ScConsole.hpp"
    #include "bindings/game/ScContext.hpp"
    #include "bindings/game/ScDisposable.hpp"
    #include "bindings/game/ScPlugin.hpp"
    #include "bindings/game/ScProfiler.hpp"
    #include "bindings/network/ScNetwork.hpp"
    #include "bindings/network/ScPlayer.hpp"
    #include "bindings/network/ScPlayerGroup.hpp"
    #include "bindings/network/ScSocket.hpp"
    #include "bindings/object/ScInstalledObject.hpp"
    #include "bindings/object/ScObject.hpp"
    #include "bindings/object/ScObjectManager.h"
    #include "bindings/ride/ScRide.hpp"
    #include "bindings/ride/ScRideStation.hpp"
    #include "bindings/world/ScAward.hpp"
    #include "bindings/world/ScClimate.hpp"
    #include "bindings/world/ScDate.hpp"
    #include "bindings/world/ScMap.hpp"
    #include "bindings/world/ScPark.hpp"
    #include "bindings/world/ScParkMessage.hpp"
    #include "bindings/world/ScResearch.hpp"
    #include "bindings/world/ScScenario.hpp"
    #include "bindings/world/ScTile.hpp"
    #include "bindings/world/ScTileElement.hpp"

    #include <cassert>
    #include <iostream>
    #include <memory>
    #include <stdexcept>
    #include <string>

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

struct ExpressionStringifier final
{
private:
    std::stringstream _ss;
    JSContext* _context{};
    int32_t _indent{};

    ExpressionStringifier(JSContext* ctx)
        : _context(ctx)
    {
    }

    void PushIndent(int32_t c = 1)
    {
        _indent += c;
    }

    void PopIndent(int32_t c = 1)
    {
        _indent -= c;
    }

    void LineFeed()
    {
        _ss << "\n" << std::string(_indent, ' ');
    }

    void Stringify(const JSValue val, bool canStartWithNewLine, int32_t nestLevel)
    {
        if (nestLevel >= 8)
        {
            _ss << "[...]";
            return;
        }

        if (JS_IsUndefined(val))
        {
            _ss << "undefined";
        }
        else if (JS_IsNull(val))
        {
            _ss << "null";
        }
        else if (JS_IsUninitialized(val))
        {
            _ss << "uninitialized";
        }
        else if (JS_IsBool(val))
        {
            _ss << (JS_VALUE_GET_BOOL(val) ? "true" : "false");
        }
        else if (JS_IsNumber(val) || JS_IsBigInt(_context, val))
        {
            StringifyNumber(val);
        }
        else if (JS_IsString(val))
        {
            StringifyString(val);
        }
        else if (JS_IsObject(val))
        {
            if (JS_IsFunction(_context, val))
            {
                StringifyFunction(val);
            }
            else if (JS_IsArray(val))
            {
                StringifyArray(val, canStartWithNewLine, nestLevel);
            }
            else if (JS_IsError(_context, val))
            {
                StringifyError(val);
            }
            else
            {
                StringifyObject(val, canStartWithNewLine, nestLevel);
            }
        }
        else if (JS_IsArrayBuffer(val))
        {
            _ss << "[Buffer]";
        }
        else
        {
            _ss << "[Unknown Value]";
        }
    }

    void StringifyArray(const JSValue val, bool canStartWithNewLine, int32_t nestLevel)
    {
        constexpr int64_t maxItemsToShow = 4;

        int64_t arrayLen;
        if (JS_GetLength(_context, val, &arrayLen) == -1)
        {
            _ss << "[error printing array]";
        }
        else if (arrayLen == 0)
        {
            _ss << "[]";
        }
        else if (arrayLen == 1)
        {
            _ss << "[ ";
            JSValue prop = JS_GetPropertyInt64(_context, val, 0);
            Stringify(prop, false, nestLevel + 1);
            JS_FreeValue(_context, prop);
            _ss << " ]";
        }
        else
        {
            if (canStartWithNewLine)
            {
                PushIndent();
                LineFeed();
            }
            _ss << "[ ";
            PushIndent(2);
            for (int64_t i = 0; i < arrayLen; i++)
            {
                if (i != 0)
                {
                    _ss << ",";
                    LineFeed();
                }
                if (i >= maxItemsToShow)
                {
                    auto remainingItemsNotShown = arrayLen - maxItemsToShow;
                    if (remainingItemsNotShown == 1)
                    {
                        _ss << "... 1 more item";
                    }
                    else
                    {
                        _ss << "... " << std::to_string(remainingItemsNotShown) << " more items";
                    }
                    break;
                }

                JSValue prop = JS_GetPropertyInt64(_context, val, i);
                Stringify(prop, false, nestLevel + 1);
                JS_FreeValue(_context, prop);
            }
            _ss << " ]";
            PopIndent(2);
            if (canStartWithNewLine)
            {
                PopIndent();
            }
        }
    }

    void StringifyObject(const JSValue val, bool canStartWithNewLine, int32_t nestLevel)
    {
        JSPropertyEnum* props;
        uint32_t propsLen;
        JS_GetOwnPropertyNames(_context, &props, &propsLen, val, JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK | JS_GPN_PRIVATE_MASK);

        if (propsLen == 0)
        {
            _ss << "{}";
        }
        else if (propsLen == 1)
        {
            _ss << "{ ";

            const char* key = JS_AtomToCString(_context, props[0].atom);
            if (key)
            {
                _ss << key << ": ";
                JS_FreeCString(_context, key);
            }
            else
            {
                // For some reason the key was not a string
                _ss << "?: ";
            }

            JSValue prop = JS_GetProperty(_context, val, props[0].atom);
            Stringify(prop, true, nestLevel + 1);
            JS_FreeValue(_context, prop);

            _ss << " }";
        }
        else
        {
            if (canStartWithNewLine)
            {
                PushIndent();
                LineFeed();
            }

            _ss << "{ ";
            PushIndent(2);

            for (uint32_t i = 0; i < propsLen; i++)
            {
                if (i != 0)
                {
                    _ss << ",";
                    LineFeed();
                }
                const char* key = JS_AtomToCString(_context, props[i].atom);
                if (key)
                {
                    _ss << key << ": ";
                    JS_FreeCString(_context, key);
                }
                else
                {
                    // For some reason the key was not a string
                    _ss << "?: ";
                }

                JSValue prop = JS_GetProperty(_context, val, props[i].atom);
                Stringify(prop, true, nestLevel + 1);
                JS_FreeValue(_context, prop);
            }

            PopIndent(2);
            _ss << " }";

            if (canStartWithNewLine)
            {
                PopIndent();
            }
        }

        JS_FreePropertyEnum(_context, props, propsLen);
    }

    void StringifyFunction(const JSValue val)
    {
        if (JS_IsConstructor(_context, val))
        {
            _ss << "[Constructor]";
        }
        else
        {
            _ss << "[Function]";
        }
    }

    void StringifyString(const JSValue val)
    {
        size_t len;
        const char* str = JS_ToCStringLen(_context, &len, val);
        if (str)
        {
            _ss << "'" << std::string_view(str, len) << "'";
            JS_FreeCString(_context, str);
        }
        else
        {
            _ss << "[error printing string]";
        }
    }

    void StringifyNumber(const JSValue val)
    {
        if (val.tag == JS_TAG_INT)
        {
            _ss << std::to_string(JS_VALUE_GET_INT(val));
        }
        else if (val.tag == JS_TAG_FLOAT64)
        {
            const double d = JS_VALUE_GET_FLOAT64(val);
            const int64_t i = static_cast<int64_t>(d);
            if (AlmostEqual<double>(d, i))
            {
                _ss << std::to_string(i);
            }
            else
            {
                _ss << std::to_string(d);
            }
        }
        else if (val.tag == JS_TAG_SHORT_BIG_INT)
        {
            _ss << std::to_string(JS_VALUE_GET_SHORT_BIG_INT(val));
        }
        else if (val.tag == JS_TAG_BIG_INT)
        {
            const char* str = JS_ToCString(_context, val);
            if (str)
            {
                _ss << str;
                JS_FreeCString(_context, str);
            }
            else
            {
                _ss << "[BitInt error]";
            }
        }
    }

    void StringifyError(const JSValue val)
    {
        const char* str = JS_ToCString(_context, val);
        if (str)
        {
            _ss << str;
            JS_FreeCString(_context, str);
        }
        else
        {
            _ss << "[error]";
        }
        JSValue stackVal = JS_GetPropertyStr(_context, val, "stack");
        if (!JS_IsUndefined(stackVal))
        {
            const char* stackStr = JS_ToCString(_context, stackVal);
            if (stackStr)
            {
                LineFeed();
                std::string_view view(stackStr);
                if (view.ends_with('\n'))
                    view = view.substr(0, view.length() - 1);
                _ss << view;
                JS_FreeCString(_context, stackStr);
            }
            else
            {
                LineFeed();
                _ss << "[no stack trace]";
            }
        }
        JS_FreeValue(_context, stackVal);
    }

    // Taken from http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
    template<class T>
    static typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type AlmostEqual(T x, T y, int32_t ulp = 20)
    {
        // the machine epsilon has to be scaled to the magnitude of the values used
        // and multiplied by the desired precision in ULPs (units in the last place)
        return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
            // unless the result is subnormal
            || std::abs(x - y)
            < (std::numeric_limits<T>::min)(); // TODO: Remove parentheses around min once the macro is removed
    }

public:
    static std::string StringifyExpression(JSContext* ctx, const JSValue val)
    {
        ExpressionStringifier instance(ctx);
        instance.Stringify(val, false, 0);
        return instance._ss.str();
    }
};

ScriptEngine::ScriptEngine(InteractiveConsole& console, IPlatformEnvironment& env)
    : _console(console)
    , _env(env)
    , _hookEngine(*this)
{
}

void ScriptEngine::Initialise()
{
    if (_initialised)
        throw std::runtime_error("Script engine already initialised.");
    if (!_runtime)
    {
        _runtime = JS_NewRuntime();
        if (!_runtime)
            throw std::runtime_error("QuickJS: cannot allocate JS runtime\n");

    #ifndef NDEBUG
        // Dump JS engine memory leaks
        JS_SetDumpFlags(_runtime, JS_DUMP_LEAKS);
    #endif
    }

    // Disable maximum stack size limit for the JS engine.
    JS_SetMaxStackSize(_runtime, 0);

    _replContext = JS_NewContext(_runtime);
    if (!_replContext)
        throw std::runtime_error("QuickJS: cannot allocate REPL JS context\n");
    RegisterClasses(_replContext);
    InitialiseContext(_replContext);

    _initialised = true;
    _transientPluginsEnabled = false;
    _transientPluginsStarted = false;

    LoadSharedStorage();
    ClearParkStorage();
}

JSRuntime* ScriptEngine::_runtime = nullptr;
ScAward Scripting::gScAward;
ScCheats Scripting::gScCheats;
ScClimate Scripting::gScClimate;
ScWeatherState Scripting::gScWeatherState;
ScConfiguration Scripting::gScConfiguration;
ScConsole Scripting::gScConsole;
ScContext Scripting::gScContext;
ScDate Scripting::gScDate;
ScDisposable Scripting::gScDisposable;
ScMap Scripting::gScMap;
ScNetwork Scripting::gScNetwork;
ScObjectManager Scripting::gScObjectManager;
ScInstalledObject Scripting::gScInstalledObject;
ScLargeSceneryObjectTile Scripting::gScLargeSceneryObjectTile;
ScObject Scripting::gScObject;
ScPark Scripting::gScPark;
ScParkMessage Scripting::gScParkMessage;
ScPlayer Scripting::gScPlayer;
ScPlayerGroup Scripting::gScPlayerGroup;
ScProfiler Scripting::gScProfiler;
ScResearch Scripting::gScResearch;
ScRide Scripting::gScRide;
ScRideStation Scripting::gScRideStation;
ScRideObjectVehicle Scripting::gScRideObjectVehicle;
ScTile Scripting::gScTile;
ScTileElement Scripting::gScTileElement;
ScTrackIterator Scripting::gScTrackIterator;
ScTrackSegment Scripting::gScTrackSegment;
ScEntity Scripting::gScEntity;
ScThought Scripting::gScThought;
    #ifndef DISABLE_NETWORK
ScSocket Scripting::gScSocket;
ScListener Scripting::gScListener;
    #endif
ScScenario Scripting::gScScenario;
ScScenarioObjective Scripting::gScScenarioObjective;
ScPatrolArea Scripting::gScPatrolArea;
ScPlugin Scripting::gScPlugin;

void ScriptEngine::RegisterClasses(JSContext* ctx)
{
    gScAward.Register(ctx);
    gScCheats.Register(ctx);
    gScClimate.Register(ctx);
    gScWeatherState.Register(ctx);
    gScConfiguration.Register(ctx);
    gScConsole.Register(ctx);
    gScContext.Register(ctx);
    gScDate.Register(ctx);
    gScDisposable.Register(ctx);
    gScMap.Register(ctx);
    gScNetwork.Register(ctx);
    gScObjectManager.Register(ctx);
    gScInstalledObject.Register(ctx);
    gScObject.Register(ctx);
    gScLargeSceneryObjectTile.Register(ctx);
    gScPark.Register(ctx);
    gScParkMessage.Register(ctx);
    gScPlayer.Register(ctx);
    gScPlayerGroup.Register(ctx);
    gScProfiler.Register(ctx);
    gScResearch.Register(ctx);
    gScRide.Register(ctx);
    gScRideStation.Register(ctx);
    gScRideObjectVehicle.Register(ctx);
    gScTile.Register(ctx);
    gScTileElement.Register(ctx);
    gScTrackIterator.Register(ctx);
    gScTrackSegment.Register(ctx);
    gScEntity.Register(ctx);
    gScThought.Register(ctx);
    #ifndef DISABLE_NETWORK
    gScSocket.Register(ctx);
    gScListener.Register(ctx);
    #endif
    gScScenario.Register(ctx);
    gScScenarioObjective.Register(ctx);
    gScPatrolArea.Register(ctx);
    gScPlugin.Register(ctx);
}

JSContext* ScriptEngine::CreateContext() const
{
    JSContext* newCtx = JS_NewContext(_runtime);
    if (!newCtx)
    {
        throw std::runtime_error("QuickJS: cannot allocate JS context\n");
    }
    InitialiseContext(newCtx);

    for (const auto& callback : _extensions)
    {
        callback(newCtx);
    }
    return newCtx;
}

void ScriptEngine::InitialiseContext(JSContext* ctx) const
{
    JSValue glb = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, glb, "cheats", gScCheats.New(ctx));
    JS_SetPropertyStr(ctx, glb, "climate", gScClimate.New(ctx));
    JS_SetPropertyStr(ctx, glb, "console", gScConsole.New(ctx, _console));
    JS_SetPropertyStr(ctx, glb, "context", gScContext.New(ctx));
    JS_SetPropertyStr(ctx, glb, "date", gScDate.New(ctx));
    JS_SetPropertyStr(ctx, glb, "map", gScMap.New(ctx));
    JS_SetPropertyStr(ctx, glb, "network", gScNetwork.New(ctx));
    JS_SetPropertyStr(ctx, glb, "park", gScPark.New(ctx));
    JS_SetPropertyStr(ctx, glb, "pluginManager", gScPlugin.New(ctx));
    JS_SetPropertyStr(ctx, glb, "profiler", gScProfiler.New(ctx));
    JS_SetPropertyStr(ctx, glb, "scenario", gScScenario.New(ctx));
    JS_SetPropertyStr(ctx, glb, "objectManager", gScObjectManager.New(ctx));
    JS_FreeValue(ctx, glb);

    RegisterConstants(ctx);
}

ScriptEngine::~ScriptEngine()
{
    if (_replContext)
    {
        JS_FreeValue(_replContext, _sharedStorage);
        JS_FreeValue(_replContext, _parkStorage);
        JS_FreeContext(_replContext);
        _replContext = nullptr;
    }
    if (_runtime)
    {
        JS_FreeRuntime(_runtime);
        _runtime = nullptr;
    }
}

class ConstantBuilder
{
private:
    JSContext* _ctx;
    JSValue _obj;

public:
    ConstantBuilder(JSContext* ctx)
        : _ctx(ctx)
    {
        _obj = JS_GetGlobalObject(_ctx);
    }

    ConstantBuilder& Namespace(std::string_view ns)
    {
        // Create a new object for namespace
        // Keep a reference to the namespace object
        JS_FreeValue(_ctx, _obj);
        _obj = JS_NewObject(_ctx);

        // Place the namespace object into the global context
        JSValue global = JS_GetGlobalObject(_ctx);
        JS_SetPropertyStr(_ctx, global, std::string(ns).c_str(), JS_DupValue(_ctx, _obj));
        JS_FreeValue(_ctx, global);

        return *this;
    }

    ConstantBuilder& Constant(std::string_view name, int32_t value)
    {
        JS_SetPropertyStr(_ctx, _obj, std::string(name).c_str(), JS_NewInt32(_ctx, value));
        return *this;
    }

    ~ConstantBuilder()
    {
        JS_FreeValue(_ctx, _obj);
    }
};

void ScriptEngine::RegisterConstants(JSContext* ctx)
{
    ConstantBuilder builder(ctx);
    builder.Namespace("TrackSlope")
        .Constant("None", EnumValue(TrackPitch::none))
        .Constant("Up25", EnumValue(TrackPitch::up25))
        .Constant("Up60", EnumValue(TrackPitch::up60))
        .Constant("Down25", EnumValue(TrackPitch::down25))
        .Constant("Down60", EnumValue(TrackPitch::down60))
        .Constant("Up90", EnumValue(TrackPitch::up90))
        .Constant("Down90", EnumValue(TrackPitch::down90));
    builder.Namespace("TrackBanking")
        .Constant("None", EnumValue(TrackRoll::none))
        .Constant("BankLeft", EnumValue(TrackRoll::left))
        .Constant("BankRight", EnumValue(TrackRoll::right))
        .Constant("UpsideDown", EnumValue(TrackRoll::upsideDown));
}

void ScriptEngine::RefreshPlugins()
{
    // Get a list of removed and added plugin files
    auto pluginFiles = GetPluginFiles();
    std::vector<std::string> plugins;
    std::vector<std::string> removedPlugins;
    std::vector<std::string> addedPlugins;
    for (const auto& plugin : _plugins)
    {
        if (plugin->HasPath())
        {
            plugins.emplace_back(plugin->GetPath());
        }
    }

    // The lists need to be sorted for std::set_difference to work properly
    std::sort(pluginFiles.begin(), pluginFiles.end());
    std::sort(plugins.begin(), plugins.end());

    std::set_difference(
        plugins.begin(), plugins.end(), pluginFiles.begin(), pluginFiles.end(), std::back_inserter(removedPlugins));
    std::set_difference(
        pluginFiles.begin(), pluginFiles.end(), plugins.begin(), plugins.end(), std::back_inserter(addedPlugins));

    // Unregister plugin files that were removed
    for (const auto& plugin : removedPlugins)
    {
        UnregisterPlugin(plugin);
    }

    // Register plugin files that were added
    for (const auto& plugin : addedPlugins)
    {
        RegisterPlugin(plugin);
    }

    // Turn on hot reload if not already enabled
    if (!_hotReloadingInitialised && Config::Get().plugin.enableHotReloading && Network::GetMode() == Network::Mode::none)
    {
        SetupHotReloading();
    }
}

std::vector<std::string> ScriptEngine::GetPluginFiles() const
{
    // Scan for .js files in plugin directory
    std::vector<std::string> pluginFiles;
    auto base = _env.GetDirectoryPath(DirBase::user, DirId::plugins);
    if (Path::DirectoryExists(base))
    {
        auto pattern = Path::Combine(base, u8"*.js");
        auto scanner = Path::ScanDirectory(pattern, true);
        while (scanner->Next())
        {
            auto path = std::string(scanner->GetPath());
            if (ShouldLoadScript(path))
            {
                pluginFiles.push_back(path);
            }
        }
    }
    return pluginFiles;
}

bool ScriptEngine::ShouldLoadScript(std::string_view path)
{
    // A lot of JavaScript is often found in a node_modules directory tree and is most likely unwanted, so ignore it
    return path.find("/node_modules/") == std::string_view::npos && path.find("\\node_modules\\") == std::string_view::npos;
}

void ScriptEngine::UnregisterPlugin(std::string_view path)
{
    try
    {
        auto pluginIt = std::find_if(_plugins.begin(), _plugins.end(), [path](const std::shared_ptr<Plugin>& plugin) {
            return plugin->GetPath() == path;
        });
        auto& plugin = *pluginIt;

        StopPlugin(plugin);
        UnloadPlugin(plugin);
        LogPluginInfo(plugin, "Unregistered");

        _plugins.erase(pluginIt);
    }
    catch (const std::exception& e)
    {
        _console.WriteLineError(e.what());
    }
}

void ScriptEngine::RegisterPlugin(std::string_view path)
{
    try
    {
        auto plugin = std::make_shared<Plugin>(path);

        // We must load the plugin to get the metadata for it
        ScriptExecutionInfo::PluginScope scope(_execInfo, plugin, false);
        plugin->Load();

        // Unload the plugin now, metadata is kept
        plugin->Unload();

        LogPluginInfo(plugin, "Registered");
        _plugins.push_back(std::move(plugin));
    }
    catch (const std::exception& e)
    {
        _console.WriteLineError(e.what());
    }
}

void ScriptEngine::StartIntransientPlugins()
{
    LoadSharedStorage();

    for (auto& plugin : _plugins)
    {
        if (!plugin->HasStarted() && !plugin->IsTransient())
        {
            LoadPlugin(plugin);
            StartPlugin(plugin);
        }
    }

    _intransientPluginsStarted = true;
}

void ScriptEngine::StopUnloadRegisterAllPlugins()
{
    std::vector<std::string> pluginPaths;
    for (auto& plugin : _plugins)
    {
        pluginPaths.emplace_back(plugin->GetPath());
        StopPlugin(plugin);
    }
    for (auto& plugin : _plugins)
    {
        UnloadPlugin(plugin);
    }
    for (auto& pluginPath : pluginPaths)
    {
        UnregisterPlugin(pluginPath);
    }
}

void ScriptEngine::LoadTransientPlugins()
{
    if (!_initialised)
    {
        Initialise();
        RefreshPlugins();
    }
    _transientPluginsEnabled = true;
}

void ScriptEngine::LoadPlugin(const std::string& path)
{
    auto plugin = std::make_shared<Plugin>(path);
    LoadPlugin(plugin);
}

void ScriptEngine::LoadPlugin(std::shared_ptr<Plugin>& plugin)
{
    try
    {
        if (!plugin->IsLoaded())
        {
            const auto& metadata = plugin->GetMetadata();
            if (metadata.MinApiVersion <= kPluginApiVersion)
            {
                ScriptExecutionInfo::PluginScope scope(_execInfo, plugin, false);
                plugin->Load();
                LogPluginInfo(plugin, "Loaded");
            }
            else
            {
                LogPluginInfo(plugin, "Requires newer API version: v" + std::to_string(metadata.MinApiVersion));
            }
        }
    }
    catch (const std::exception& e)
    {
        _console.WriteLineError(e.what());
    }
}

void ScriptEngine::UnloadPlugin(std::shared_ptr<Plugin>& plugin)
{
    if (plugin->IsLoaded())
    {
        plugin->Unload();
        LogPluginInfo(plugin, "Unloaded");
        // A GC run is required to clean up all JS objects before the game global context is cleaned up on shut down.
        JS_RunGC(_runtime);
    }
}

void ScriptEngine::StartPlugin(std::shared_ptr<Plugin> plugin)
{
    if (!plugin->HasStarted() && ShouldStartPlugin(plugin))
    {
        ScriptExecutionInfo::PluginScope scope(_execInfo, plugin, false);
        try
        {
            LogPluginInfo(plugin, "Started");
            plugin->Start();
        }
        catch (const std::exception& e)
        {
            _console.WriteLineError(e.what());
        }
    }
}

void ScriptEngine::StopPlugin(std::shared_ptr<Plugin> plugin)
{
    if (plugin->HasStarted())
    {
        plugin->StopBegin();

        for (const auto& callback : _pluginStoppedSubscriptions)
        {
            callback(plugin);
        }
        RemoveCustomGameActions(plugin);
        RemoveIntervals(plugin);
        RemoveSockets(plugin);
        _hookEngine.UnsubscribeAll(plugin);

        plugin->StopEnd();
        LogPluginInfo(plugin, "Stopped");
    }
}

void ScriptEngine::ReloadPlugin(std::shared_ptr<Plugin> plugin)
{
    StopPlugin(plugin);
    {
        ScriptExecutionInfo::PluginScope scope(_execInfo, plugin, false);
        plugin->Load();
        LogPluginInfo(plugin, "Reloaded");
    }
    StartPlugin(plugin);
}

void ScriptEngine::SetupHotReloading()
{
    try
    {
        auto base = _env.GetDirectoryPath(DirBase::user, DirId::plugins);
        if (Path::DirectoryExists(base))
        {
            _pluginFileWatcher = std::make_unique<FileWatcher>(base);
            _pluginFileWatcher->OnFileChanged = [this](u8string_view path) {
                std::lock_guard guard(_changedPluginFilesMutex);
                _changedPluginFiles.emplace(path);
            };
            _hotReloadingInitialised = true;
        }
    }
    catch (const std::exception& e)
    {
        Console::Error::WriteLine("Unable to enable hot reloading of plugins: %s", e.what());
    }
}

void ScriptEngine::DoAutoReloadPluginCheck()
{
    if (_hotReloadingInitialised)
    {
        auto tick = Platform::GetTicks();
        if (tick - _lastHotReloadCheckTick > 1000)
        {
            AutoReloadPlugins();
            _lastHotReloadCheckTick = tick;
        }
    }
}

void ScriptEngine::AutoReloadPlugins()
{
    if (!_changedPluginFiles.empty())
    {
        std::lock_guard guard(_changedPluginFilesMutex);
        for (const auto& path : _changedPluginFiles)
        {
            auto findResult = std::find_if(_plugins.begin(), _plugins.end(), [&path](const std::shared_ptr<Plugin>& plugin) {
                return Path::Equals(path, plugin->GetPath());
            });
            if (findResult != _plugins.end())
            {
                auto& plugin = *findResult;
                try
                {
                    ReloadPlugin(plugin);
                }
                catch (const std::exception& e)
                {
                    _console.WriteLineError(e.what());
                }
            }
        }
        _changedPluginFiles.clear();
    }
}

void ScriptEngine::UnloadTransientPlugins()
{
    // Stop them all first
    for (auto& plugin : _plugins)
    {
        if (plugin->IsTransient())
        {
            StopPlugin(plugin);
        }
    }

    // Now unload them
    for (auto& plugin : _plugins)
    {
        if (plugin->IsTransient())
        {
            UnloadPlugin(plugin);
        }
    }

    _transientPluginsEnabled = false;
    _transientPluginsStarted = false;
}

void ScriptEngine::StartTransientPlugins()
{
    LoadSharedStorage();

    // Load transient plugins
    for (auto& plugin : _plugins)
    {
        if (plugin->IsTransient() && !plugin->IsLoaded() && ShouldStartPlugin(plugin))
        {
            LoadPlugin(plugin);
        }
    }

    // Start transient plugins
    for (auto& plugin : _plugins)
    {
        if (plugin->IsTransient() && plugin->IsLoaded() && !plugin->HasStarted())
        {
            StartPlugin(plugin);
        }
    }

    _transientPluginsStarted = true;
}

bool ScriptEngine::ShouldStartPlugin(const std::shared_ptr<Plugin>& plugin)
{
    auto networkMode = Network::GetMode();
    if (networkMode == Network::Mode::client)
    {
        // Only client plugins and plugins downloaded from server should be started
        const auto& metadata = plugin->GetMetadata();
        if (metadata.Type == PluginType::Remote && plugin->HasPath())
        {
            LogPluginInfo(plugin, "Remote plugin not started");
            return false;
        }
    }
    return true;
}

void ScriptEngine::Tick()
{
    if (!_initialised)
    {
        return;
    }

    PROFILED_FUNCTION();

    CheckAndStartPlugins();
    UpdateIntervals();
    UpdateSockets();
    ProcessREPL();
    DoAutoReloadPluginCheck();
}

void ScriptEngine::CheckAndStartPlugins()
{
    auto startIntransient = !_intransientPluginsStarted;
    auto startTransient = !_transientPluginsStarted && _transientPluginsEnabled;

    if (startIntransient || startTransient)
    {
        RefreshPlugins();
    }
    if (startIntransient)
    {
        StartIntransientPlugins();
    }
    if (startTransient)
    {
        StartTransientPlugins();
    }
}

void ScriptEngine::ProcessREPL()
{
    while (_evalQueue.size() > 0)
    {
        auto item = std::move(_evalQueue.front());
        _evalQueue.pop();
        auto promise = std::move(std::get<0>(item));
        auto command = std::move(std::get<1>(item));

        JSValue res = JS_Eval(_replContext, command.c_str(), command.length(), "<repl>", JS_EVAL_TYPE_GLOBAL);
        if (JS_IsException(res))
        {
            JSValue exceptionVal = JS_GetException(_replContext);
            _console.WriteLineError(Stringify(_replContext, exceptionVal));
            JS_FreeValue(_replContext, exceptionVal);
        }
        else if (!JS_IsUndefined(res))
        {
            _console.WriteLine(Stringify(_replContext, res));
        }
        JS_FreeValue(_replContext, res);

        // Signal the promise so caller can continue
        promise.set_value();
    }
}

std::future<void> ScriptEngine::Eval(const std::string& s)
{
    std::promise<void> barrier;
    auto future = barrier.get_future();
    _evalQueue.emplace(std::move(barrier), s);
    return future;
}

void ScriptEngine::ExecutePluginCall(
    const std::shared_ptr<Plugin>& plugin, const JSValue func, const std::vector<JSValue>& args, bool isGameStateMutable,
    bool keepArgsAlive)
{
    ExecutePluginCall(plugin, func, JS_UNDEFINED, args, isGameStateMutable, keepArgsAlive);
}

JSValue ScriptEngine::ExecutePluginCall(
    const std::shared_ptr<Plugin>& plugin, const JSValue func, const JSValue thisValue, const std::vector<JSValue>& args,
    bool isGameStateMutable, bool keepArgsAlive, bool keepRetValueAlive)
{
    JSValue ret = JS_UNDEFINED;

    // Note: the plugin pointer is null when called from the repl, so we assume the repl JSContext in that case.
    JSContext* ctx = plugin ? plugin->GetContext() : _replContext;
    if (JS_IsFunction(ctx, func) && (!plugin || plugin->HasStarted()))
    {
        ScriptExecutionInfo::PluginScope scope(_execInfo, plugin, isGameStateMutable);
        ret = JS_Call(ctx, func, thisValue, static_cast<int>(args.size()), const_cast<JSValue*>(args.data()));
        if (JS_IsException(ret))
        {
            JSValue exceptionVal = JS_GetException(ctx);
            _console.WriteLineError(Stringify(ctx, exceptionVal));
            JS_FreeValue(ctx, exceptionVal);
        }
        if (!keepRetValueAlive)
        {
            JS_FreeValue(ctx, ret);
            ret = JS_UNDEFINED;
        }
    }
    [[likely]] if (!keepArgsAlive)
    {
        for (const JSValue& arg : args)
        {
            JS_FreeValue(ctx, arg);
        }
    }
    return ret;
}

void ScriptEngine::LogPluginInfo(std::string_view message)
{
    auto plugin = _execInfo.GetCurrentPlugin();
    LogPluginInfo(plugin, message);
}

void ScriptEngine::LogPluginInfo(const std::shared_ptr<Plugin>& plugin, std::string_view message)
{
    if (plugin == nullptr)
    {
        _console.WriteLine(std::string(message));
    }
    else
    {
        const auto& pluginName = plugin->GetMetadata().Name;
        _console.WriteLine("[" + pluginName + "] " + std::string(message));
    }
}

void ScriptEngine::AddNetworkPlugin(std::string_view code)
{
    auto plugin = std::make_shared<Plugin>(std::string());
    plugin->SetCode(code);
    _plugins.push_back(plugin);
}

void ScriptEngine::RemoveNetworkPlugins()
{
    auto it = _plugins.begin();
    while (it != _plugins.end())
    {
        auto plugin = (*it);
        if (!plugin->HasPath())
        {
            StopPlugin(plugin);
            UnloadPlugin(plugin);
            LogPluginInfo(plugin, "Unregistered network plugin");

            it = _plugins.erase(it);
        }
        else
        {
            it++;
        }
    }
}

GameActions::Result ScriptEngine::QueryOrExecuteCustomGameAction(const GameActions::CustomAction& customAction, bool isExecute)
{
    JSContext* ctx = _replContext;
    std::string actionz = customAction.GetId();
    auto kvp = _customActions.find(actionz);
    if (kvp != _customActions.end())
    {
        const auto& customActionInfo = kvp->second;

        // Deserialise the JSON args
        const std::string& argsz = customAction.GetJson();

        auto jsArgs = JS_ParseJSON(ctx, argsz.c_str(), argsz.size(), customActionInfo.Name.c_str());
        if (JS_IsException(jsArgs))
        {
            auto res = GameActions::Result();
            res.error = GameActions::Status::invalidParameters;
            res.errorTitle = "Invalid JSON";
            return res;
        }

        std::vector<JSValue> pluginCallArgs;
        if (customActionInfo.Owner->GetTargetAPIVersion() <= kApiVersionCustomActionArgs)
        {
            pluginCallArgs = { jsArgs };
        }
        else
        {
            JSValue obj = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, obj, "action", JSFromStdString(ctx, actionz));
            JS_SetPropertyStr(ctx, obj, "args", jsArgs);
            JS_SetPropertyStr(ctx, obj, "player", JS_NewInt32(ctx, customAction.GetPlayer()));
            JS_SetPropertyStr(ctx, obj, "type", JS_NewInt32(ctx, EnumValue(customAction.GetType())));

            auto flags = customAction.GetActionFlags();
            JS_SetPropertyStr(ctx, obj, "isClientOnly", JS_NewBool(ctx, (flags & GameActions::Flags::ClientOnly) != 0));
            pluginCallArgs = { obj };
        }

        // Ready to call plugin handler
        JSValue jsResult;
        if (!isExecute)
        {
            jsResult = ExecutePluginCall(
                customActionInfo.Owner, customActionInfo.Query.callback, JS_UNDEFINED, pluginCallArgs, false, false, true);
        }
        else
        {
            jsResult = ExecutePluginCall(
                customActionInfo.Owner, customActionInfo.Execute.callback, JS_UNDEFINED, pluginCallArgs, true, false, true);
        }

        GameActions::Result res = JSToGameActionResult(ctx, jsResult);
        JS_FreeValue(ctx, jsResult);
        return res;
    }

    auto res = GameActions::Result();
    res.error = GameActions::Status::unknown;
    res.errorTitle = "Unknown custom action";
    res.errorMessage = customAction.GetPluginName() + ": " + actionz;
    return res;
}

GameActions::Result ScriptEngine::JSToGameActionResult(JSContext* ctx, JSValue d)
{
    auto result = GameActions::Result();
    if (JS_IsObject(d))
    {
        result.error = static_cast<GameActions::Status>(AsOrDefault(ctx, d, "error", int32_t()));
        result.errorTitle = AsOrDefault(ctx, d, "errorTitle", "");
        result.errorMessage = AsOrDefault(ctx, d, "errorMessage", "");
        result.cost = AsOrDefault(ctx, d, "cost", int64_t());
        auto expenditureType = AsOrDefault(ctx, d, "expenditureType", "");
        if (!expenditureType.empty())
        {
            auto expenditure = StringToExpenditureType(expenditureType);
            if (expenditure != ExpenditureType::count)
            {
                result.expenditure = expenditure;
            }
        }
    }
    else
    {
        result.error = GameActions::Status::unknown;
        result.errorTitle = "Unknown";
        result.errorMessage = "Unknown";
    }
    return result;
}

constexpr static const char* ExpenditureTypes[] = {
    "ride_construction",
    "ride_runningcosts",
    "land_purchase",
    "landscaping",
    "park_entrance_tickets",
    "park_ride_tickets",
    "shop_sales",
    "shop_stock",
    "food_drink_sales",
    "food_drink_stock",
    "wages",
    "marketing",
    "research",
    "interest",
};

std::string_view ScriptEngine::ExpenditureTypeToString(ExpenditureType expenditureType)
{
    auto index = static_cast<size_t>(expenditureType);
    if (index < std::size(ExpenditureTypes))
    {
        return ExpenditureTypes[index];
    }
    return {};
}

ExpenditureType ScriptEngine::StringToExpenditureType(std::string_view expenditureType)
{
    auto it = std::find(std::begin(ExpenditureTypes), std::end(ExpenditureTypes), expenditureType);
    if (it != std::end(ExpenditureTypes))
    {
        return static_cast<ExpenditureType>(std::distance(std::begin(ExpenditureTypes), it));
    }
    return ExpenditureType::count;
}

JSValue ScriptEngine::GameActionResultToJS(
    JSContext* ctx, const GameActions::GameAction& action, const GameActions::Result& result)
{
    JSValue obj = JS_NewObject(ctx);

    JS_SetPropertyStr(
        ctx, obj, "error", JS_NewInt32(ctx, static_cast<std::underlying_type_t<GameActions::Status>>(result.error)));
    if (result.error != GameActions::Status::ok)
    {
        JS_SetPropertyStr(ctx, obj, "errorTitle", JSFromStdString(ctx, result.getErrorTitle()));
        JS_SetPropertyStr(ctx, obj, "errorMessage", JSFromStdString(ctx, result.getErrorMessage()));
    }

    if (result.cost != kMoney64Undefined)
    {
        JS_SetPropertyStr(ctx, obj, "cost", JS_NewInt64(ctx, result.cost));
    }
    if (!result.position.IsNull())
    {
        JS_SetPropertyStr(ctx, obj, "position", ToJSValue(ctx, result.position));
    }
    if (result.expenditure != ExpenditureType::count)
    {
        JS_SetPropertyStr(ctx, obj, "expenditureType", JSFromStdString(ctx, ExpenditureTypeToString(result.expenditure)));
    }

    // RideCreateAction only
    if (action.GetType() == GameCommand::CreateRide)
    {
        if (result.error == GameActions::Status::ok)
        {
            const auto rideIndex = result.getData<RideId>();
            JS_SetPropertyStr(ctx, obj, "ride", JS_NewInt32(ctx, rideIndex.ToUnderlying()));
        }
    }
    // StaffHireNewAction only
    else if (action.GetType() == GameCommand::HireNewStaffMember)
    {
        if (result.error == GameActions::Status::ok)
        {
            const auto actionResult = result.getData<GameActions::StaffHireNewActionResult>();
            if (!actionResult.StaffEntityId.IsNull())
            {
                JS_SetPropertyStr(ctx, obj, "peep", JS_NewInt32(ctx, actionResult.StaffEntityId.ToUnderlying()));
            }
        }
    }
    // BannerPlaceAction, LargeSceneryPlaceAction, WallPlaceAction
    auto bannerId = BannerIndex::GetNull();
    switch (action.GetType())
    {
        case GameCommand::PlaceBanner:
            bannerId = result.getData<GameActions::BannerPlaceActionResult>().bannerId;
            break;
        case GameCommand::PlaceLargeScenery:
            bannerId = result.getData<GameActions::LargeSceneryPlaceActionResult>().bannerId;
            break;
        case GameCommand::PlaceWall:
            bannerId = result.getData<GameActions::WallPlaceActionResult>().BannerId;
            break;
        default:
            break;
    }
    if (!bannerId.IsNull())
    {
        JS_SetPropertyStr(ctx, obj, "bannerIndex", JS_NewInt32(ctx, bannerId.ToUnderlying()));
    }

    return obj;
}

bool ScriptEngine::RegisterCustomAction(
    const std::shared_ptr<Plugin>& plugin, std::string_view action, const JSCallback& query, const JSCallback& execute)
{
    std::string actionz = std::string(action);
    if (_customActions.find(actionz) != _customActions.end())
    {
        return false;
    }

    CustomActionInfo customAction;
    customAction.Owner = plugin;
    customAction.Name = std::move(actionz);
    customAction.Query = query;
    customAction.Execute = execute;
    _customActions[customAction.Name] = std::move(customAction);
    return true;
}

void ScriptEngine::RemoveCustomGameActions(const std::shared_ptr<Plugin>& plugin)
{
    for (auto it = _customActions.begin(); it != _customActions.end();)
    {
        if (it->second.Owner == plugin)
        {
            it = _customActions.erase(it);
        }
        else
        {
            it++;
        }
    }
}

class JSToGameActionParameterVisitor : public GameActions::GameActionParameterVisitor
{
private:
    JSValue _jsValue;
    JSContext* _ctx;
    bool _error = false;

public:
    JSToGameActionParameterVisitor(JSContext* ctx, JSValue jsValue)
        : _jsValue(jsValue)
        , _ctx(ctx)
    {
    }

    void Visit(std::string_view name, bool& param) override
    {
        auto val = JSToOptionalBool(_ctx, _jsValue, std::string(name).c_str());
        if (val.has_value())
            param = val.value();
        else
            _error = true;
    }

    void Visit(std::string_view name, int32_t& param) override
    {
        auto val = JSToOptionalInt(_ctx, _jsValue, std::string(name).c_str());
        if (val.has_value())
            param = val.value();
        else
            _error = true;
    }

    void Visit(std::string_view name, std::string& param) override
    {
        auto val = JSToOptionalStdString(_ctx, _jsValue, std::string(name).c_str());
        if (val.has_value())
            param = val.value();
        else
            _error = true;
    }

    bool GetErrorFlag() const
    {
        return _error;
    }
};

class JSFromGameActionParameterVisitor : public GameActions::GameActionParameterVisitor
{
private:
    JSValue _jsObject;
    JSContext* _ctx;

public:
    JSFromGameActionParameterVisitor(JSContext* ctx, JSValue jsObject)
        : _jsObject(jsObject)
        , _ctx(ctx)
    {
    }

    void Visit(std::string_view name, bool& param) override
    {
        std::string szName(name);
        JS_SetPropertyStr(_ctx, _jsObject, szName.c_str(), JS_NewBool(_ctx, param));
    }

    void Visit(std::string_view name, int32_t& param) override
    {
        std::string szName(name);
        JS_SetPropertyStr(_ctx, _jsObject, szName.c_str(), JS_NewInt32(_ctx, param));
    }

    void Visit(std::string_view name, std::string& param) override
    {
        std::string szName(name);
        JS_SetPropertyStr(_ctx, _jsObject, szName.c_str(), JSFromStdString(_ctx, param));
    }
};

// clang-format off
const static EnumMap<GameCommand> ActionNameToType = {
    { "balloonpress", GameCommand::BalloonPress },
    { "bannerplace", GameCommand::PlaceBanner },
    { "bannerremove", GameCommand::RemoveBanner },
    { "bannersetcolour", GameCommand::SetBannerColour },
    { "bannersetname", GameCommand::SetBannerName },
    { "bannersetstyle", GameCommand::SetBannerStyle },
    { "clearscenery", GameCommand::ClearScenery },
    { "footpathplace", GameCommand::PlacePath },
    { "footpathlayoutplace", GameCommand::PlacePathLayout },
    { "footpathremove", GameCommand::RemovePath },
    { "footpathadditionplace", GameCommand::PlaceFootpathAddition },
    { "footpathadditionremove", GameCommand::RemoveFootpathAddition },
    { "gamesetspeed", GameCommand::SetGameSpeed },
    { "guestsetflags", GameCommand::GuestSetFlags },
    { "guestsetname", GameCommand::SetGuestName },
    { "landbuyrights", GameCommand::BuyLandRights },
    { "landlower", GameCommand::LowerLand },
    { "landraise", GameCommand::RaiseLand },
    { "landsetheight", GameCommand::SetLandHeight },
    { "landsetrights", GameCommand::SetLandOwnership },
    { "landsmooth", GameCommand::EditLandSmooth },
    { "largesceneryplace", GameCommand::PlaceLargeScenery },
    { "largesceneryremove", GameCommand::RemoveLargeScenery },
    { "largescenerysetcolour", GameCommand::SetLargeSceneryColour },
    { "loadorquit", GameCommand::LoadOrQuit },
    { "mapchangesize", GameCommand::ChangeMapSize },
    { "mazeplacetrack", GameCommand::PlaceMazeDesign },
    { "mazesettrack", GameCommand::SetMazeTrack },
    { "networkmodifygroup", GameCommand::ModifyGroups },
    { "parkentranceplace", GameCommand::PlaceParkEntrance },
    { "parkentranceremove", GameCommand::RemoveParkEntrance },
    { "parkmarketing", GameCommand::StartMarketingCampaign },
    { "parksetdate", GameCommand::SetDate },
    { "parksetentrancefee", GameCommand::SetParkEntranceFee },
    { "parksetloan", GameCommand::SetCurrentLoan },
    { "parksetname", GameCommand::SetParkName },
    { "parksetparameter", GameCommand::SetParkOpen },
    { "parksetresearchfunding", GameCommand::SetResearchFunding },
    { "pausetoggle", GameCommand::TogglePause },
    { "peeppickup", GameCommand::PickupGuest },
    { "peepspawnplace", GameCommand::PlacePeepSpawn },
    { "playerkick", GameCommand::KickPlayer },
    { "playersetgroup", GameCommand::SetPlayerGroup },
    { "ridecreate", GameCommand::CreateRide },
    { "ridedemolish", GameCommand::DemolishRide },
    { "rideentranceexitplace", GameCommand::PlaceRideEntranceOrExit },
    { "rideentranceexitremove", GameCommand::RemoveRideEntranceOrExit },
    { "ridefreezerating", GameCommand::FreezeRideRating },
    { "ridesetappearance", GameCommand::SetRideAppearance },
    { "ridesetcolourscheme", GameCommand::SetColourScheme },
    { "ridesetname", GameCommand::SetRideName },
    { "ridesetprice", GameCommand::SetRidePrice },
    { "ridesetsetting", GameCommand::SetRideSetting },
    { "ridesetstatus", GameCommand::SetRideStatus },
    { "ridesetvehicle", GameCommand::SetRideVehicles },
    { "scenariosetsetting", GameCommand::EditScenarioOptions },
    { "cheatset", GameCommand::Cheat },
    { "signsetname", GameCommand::SetSignName },
    { "signsetstyle", GameCommand::SetSignStyle },
    { "smallsceneryplace", GameCommand::PlaceScenery },
    { "smallsceneryremove", GameCommand::RemoveScenery },
    { "smallscenerysetcolour", GameCommand::SetSceneryColour},
    { "stafffire", GameCommand::FireStaffMember },
    { "staffhire", GameCommand::HireNewStaffMember },
    { "staffsetcolour", GameCommand::SetStaffColour },
    { "staffsetcostume", GameCommand::SetStaffCostume },
    { "staffsetname", GameCommand::SetStaffName },
    { "staffsetorders", GameCommand::SetStaffOrders },
    { "staffsetpatrolarea", GameCommand::SetStaffPatrol },
    { "surfacesetstyle", GameCommand::ChangeSurfaceStyle },
    { "tilemodify", GameCommand::ModifyTile },
    { "trackdesign", GameCommand::PlaceTrackDesign },
    { "trackplace", GameCommand::PlaceTrack },
    { "trackremove", GameCommand::RemoveTrack },
    { "tracksetbrakespeed", GameCommand::SetBrakesSpeed },
    { "wallplace", GameCommand::PlaceWall },
    { "wallremove", GameCommand::RemoveWall },
    { "wallsetcolour", GameCommand::SetWallColour },
    { "waterlower", GameCommand::LowerWater },
    { "waterraise", GameCommand::RaiseWater },
    { "watersetheight", GameCommand::SetWaterHeight }
};
// clang-format on

static std::string GetActionName(GameCommand commandId)
{
    auto it = ActionNameToType.find(commandId);
    if (it != ActionNameToType.end())
    {
        return std::string{ it->first };
    }
    return {};
}

static std::unique_ptr<GameActions::GameAction> CreateGameActionFromActionId(const std::string& name)
{
    auto result = ActionNameToType.find(name);
    if (result != ActionNameToType.end())
    {
        return GameActions::Create(result->second);
    }
    return nullptr;
}

void ScriptEngine::RunGameActionHooks(const GameActions::GameAction& action, GameActions::Result& result, bool isExecute)
{
    auto hookType = isExecute ? HookType::actionExecute : HookType::actionQuery;
    if (_hookEngine.HasSubscriptions(hookType))
    {
        JSContext* ctx = _replContext;
        JSValue obj = JS_NewObject(ctx);

        auto actionId = action.GetType();
        if (action.GetType() == GameCommand::Custom)
        {
            auto customAction = static_cast<const GameActions::CustomAction&>(action);
            JS_SetPropertyStr(ctx, obj, "action", JSFromStdString(ctx, customAction.GetId()));

            JSValue jsArgs = JS_ParseJSON(
                ctx, customAction.GetJson().c_str(), customAction.GetJson().length(), customAction.GetName());
            if (!JS_IsException(jsArgs))
            {
                JS_SetPropertyStr(ctx, obj, "args", jsArgs);
            }
            else
            {
                JS_SetPropertyStr(ctx, obj, "args", JS_NewObject(ctx));
            }
            JS_FreeValue(ctx, jsArgs);
        }
        else
        {
            auto actionName = GetActionName(actionId);
            if (!actionName.empty())
            {
                JS_SetPropertyStr(ctx, obj, "action", JSFromStdString(ctx, actionName));
            }

            JSValue args = JS_NewObject(ctx);
            JSFromGameActionParameterVisitor visitor(ctx, args);
            const_cast<GameActions::GameAction&>(action).AcceptParameters(visitor);
            const_cast<GameActions::GameAction&>(action).AcceptFlags(visitor);
            JS_SetPropertyStr(ctx, obj, "args", args);
        }

        JS_SetPropertyStr(ctx, obj, "player", JS_NewInt32(ctx, action.GetPlayer()));
        JS_SetPropertyStr(ctx, obj, "type", JS_NewInt32(ctx, EnumValue(actionId)));

        auto flags = action.GetActionFlags();
        JS_SetPropertyStr(ctx, obj, "isClientOnly", JS_NewBool(ctx, (flags & GameActions::Flags::ClientOnly) != 0));

        JS_SetPropertyStr(ctx, obj, "result", GameActionResultToJS(ctx, action, result));

        _hookEngine.Call(hookType, obj, false, true);

        if (!isExecute)
        {
            JSValue jsResult = JS_GetPropertyStr(ctx, obj, "result");
            if (JS_IsObject(jsResult))
            {
                int32_t error = AsOrDefault(ctx, jsResult, "error", int32_t());
                if (error != 0)
                {
                    result.error = static_cast<GameActions::Status>(error);
                    result.errorTitle = AsOrDefault(ctx, jsResult, "errorTitle", "");
                    result.errorMessage = AsOrDefault(ctx, jsResult, "errorMessage", "");
                }
            }
            JS_FreeValue(ctx, jsResult);
        }
        JS_FreeValue(ctx, obj);
    }
}

std::pair<std::unique_ptr<GameActions::GameAction>, bool> ScriptEngine::CreateGameAction(
    JSContext* ctx, const std::string& actionid, JSValue args, const std::string& pluginName)
{
    auto action = CreateGameActionFromActionId(actionid);
    if (action != nullptr)
    {
        JSToGameActionParameterVisitor visitor(ctx, args);
        action->AcceptParameters(visitor);

        JSValue flags = JS_GetPropertyStr(ctx, args, "flags");
        if (JS_IsNumber(flags))
        {
            action->AcceptFlags(visitor);
        }
        JS_FreeValue(ctx, flags);
        return { std::move(action), visitor.GetErrorFlag() };
    }

    // Serialise args to json so that it can be sent
    std::string json;
    if (JS_IsObject(args))
    {
        JSValue jsonVal = JS_JSONStringify(ctx, args, JS_UNDEFINED, JS_UNDEFINED);
        if (JS_IsString(jsonVal))
        {
            json = JSToStdString(ctx, jsonVal);
        }
        JS_FreeValue(ctx, jsonVal);
    }
    if (json.empty())
    {
        JSValue emptyObj = JS_NewObject(ctx);
        JSValue jsonVal = JS_JSONStringify(ctx, args, JS_UNDEFINED, JS_UNDEFINED);
        json = JSToStdString(ctx, jsonVal);
        JS_FreeValue(ctx, emptyObj);
        JS_FreeValue(ctx, jsonVal);
    }
    auto customAction = std::make_unique<GameActions::CustomAction>(actionid, json, pluginName);

    if (customAction->GetPlayer() == -1 && Network::GetMode() != Network::Mode::none)
    {
        customAction->SetPlayer(Network::GetCurrentPlayerId());
    }
    return { std::move(customAction), false };
}

void ScriptEngine::InitSharedStorage()
{
    if (_replContext)
    {
        JS_FreeValue(_replContext, _sharedStorage);
        _sharedStorage = JS_NewObject(_replContext);
    }
}

void ScriptEngine::LoadSharedStorage()
{
    InitSharedStorage();

    auto path = _env.GetFilePath(PathId::pluginStore);
    try
    {
        if (File::Exists(path))
        {
            auto data = File::ReadAllBytes(path);
            // quickjs wants a null terminator not counted in the buf_len
            data.push_back(0);
            JSValue result = JS_ParseJSON(
                _replContext, reinterpret_cast<const char*>(data.data()), data.size() - 1, path.c_str());
            if (JS_IsObject(result))
            {
                JS_FreeValue(_replContext, _sharedStorage);
                _sharedStorage = result;
            }
            else
            {
                Console::Error::WriteLine("Unable to load plugin shared storage");
            }
        }
    }
    catch (const std::exception&)
    {
        Console::Error::WriteLine("Unable to read '%s'", path.c_str());
    }
}

void ScriptEngine::SaveSharedStorage()
{
    auto path = _env.GetFilePath(PathId::pluginStore);
    try
    {
        JSValue jsonVal = JS_JSONStringify(_replContext, _sharedStorage, JS_UNDEFINED, JS_UNDEFINED);
        if (JS_IsString(jsonVal))
        {
            // inefficient to copy the whole string out first, but we have to do this to avoid breaking the exception flow
            std::string json = JSToStdString(_replContext, jsonVal);
            JS_FreeValue(_replContext, jsonVal);
            File::WriteAllBytes(path, json.c_str(), json.size());
            return;
        }
        JS_FreeValue(_replContext, jsonVal);
        Console::Error::WriteLine("Unable to stringify shared storage JSON");
    }
    catch (const std::exception&)
    {
        Console::Error::WriteLine("Unable to write to '%s'", path.c_str());
    }
}

void ScriptEngine::ClearParkStorage()
{
    if (_replContext)
    {
        JS_FreeValue(_replContext, _parkStorage);
        _parkStorage = JS_NewObject(_replContext);
    }
}

std::string ScriptEngine::GetParkStorageAsJSON()
{
    std::string retStr{};
    JSValue jsonVal = JS_JSONStringify(_replContext, _parkStorage, JS_UNDEFINED, JS_UNDEFINED);
    if (JS_IsString(jsonVal))
    {
        retStr = JSToStdString(_replContext, jsonVal);
    }
    else
    {
        Console::Error::WriteLine("Could not stringify park storage");
    }
    JS_FreeValue(_replContext, jsonVal);
    return retStr;
}

void ScriptEngine::SetParkStorageFromJSON(const std::string& value, const std::string& filename)
{
    if (value.empty())
    {
        ClearParkStorage();
        return;
    }
    JSValue result = JS_ParseJSON(_replContext, value.c_str(), value.size(), filename.c_str());
    if (JS_IsObject(result))
    {
        JS_FreeValue(_replContext, _parkStorage);
        _parkStorage = result;
    }
    else
    {
        ClearParkStorage();
        Console::Error::WriteLine("Could not load park storage");
    }
}

IntervalHandle ScriptEngine::AllocateHandle()
{
    const auto nextHandle = _nextIntervalHandle;

    // In case of overflow start from 1 again
    _nextIntervalHandle = std::max(_nextIntervalHandle + 1u, 1u);

    return nextHandle;
}

IntervalHandle ScriptEngine::AddInterval(
    const std::shared_ptr<Plugin>& plugin, int32_t delay, bool repeat, const JSCallback& callback)
{
    auto handle = AllocateHandle();
    assert(handle != 0);

    auto& interval = _intervals[handle];
    interval.Owner = plugin;
    interval.Delay = delay;
    interval.LastTimestamp = _lastIntervalTimestamp;
    interval.Callback = callback;
    interval.Repeat = repeat;

    return handle;
}

void ScriptEngine::RemoveInterval(const std::shared_ptr<Plugin>& plugin, IntervalHandle handle)
{
    if (handle == 0)
        return;

    auto it = _intervals.find(handle);
    if (it == _intervals.end())
        return;

    auto& interval = it->second;

    // Only allow owner or REPL (nullptr) to remove intervals
    if (plugin == nullptr || interval.Owner == plugin)
    {
        interval.Deleted = true;
    }
}

void ScriptEngine::UpdateIntervals()
{
    uint32_t timestamp = Platform::GetTicks();
    if (timestamp < _lastIntervalTimestamp)
    {
        // timestamp has wrapped, subtract all intervals by the remaining amount before wrap
        auto delta = static_cast<int64_t>(std::numeric_limits<uint32_t>::max() - _lastIntervalTimestamp);
        for (auto& entry : _intervals)
        {
            auto& interval = entry.second;
            interval.LastTimestamp = -delta;
        }
    }
    _lastIntervalTimestamp = timestamp;

    // Erase all intervals marked as deleted.
    for (auto it = _intervals.begin(); it != _intervals.end();)
    {
        auto& interval = it->second;

        if (interval.Deleted)
        {
            it = _intervals.erase(it);
        }
        else
        {
            it++;
        }
    }

    // Execute all intervals that are due.
    for (auto it = _intervals.begin(); it != _intervals.end(); it++)
    {
        auto& interval = it->second;

        if (timestamp < interval.LastTimestamp + interval.Delay)
        {
            continue;
        }

        if (interval.Deleted)
        {
            // There is a chance that in one of the callbacks it deletes another interval.
            continue;
        }

        ExecutePluginCall(interval.Owner, interval.Callback.callback, {}, false);

        interval.LastTimestamp = timestamp;
        if (!interval.Repeat)
        {
            interval.Deleted = true;
        }
    }
}

void ScriptEngine::RemoveIntervals(const std::shared_ptr<Plugin>& plugin)
{
    for (auto it = _intervals.begin(); it != _intervals.end();)
    {
        auto& interval = it->second;

        if (interval.Owner == plugin)
        {
            it = _intervals.erase(it);
        }
        else
        {
            it++;
        }
    }
}

    #ifndef DISABLE_NETWORK
void ScriptEngine::AddSocket(SocketDataBase* data)
{
    _sockets.push_back(data);
}

void ScriptEngine::RemoveSocket(SocketDataBase* data)
{
    // Just remove one
    auto it = std::find(_sockets.begin(), _sockets.end(), data);
    if (it != _sockets.end())
        *it = nullptr;
}
    #endif

void ScriptEngine::UpdateSockets()
{
    #ifndef DISABLE_NETWORK
    // AddSocket and RemoveSocket can be called as a result of the Update
    // Therefore we add to the end and remove by setting to null and cleaning up
    // after the update. We also must use [] here and we remember the original
    // sockets vector size so that we process new ones in the next tick.
    const size_t sz = _sockets.size();
    for (size_t i = 0; i < sz; i++)
    {
        if (_sockets[i] != nullptr)
            _sockets[i]->Update();
    }
    std::erase(_sockets, nullptr);
    #endif
}

void ScriptEngine::RemoveSockets(const std::shared_ptr<Plugin>& plugin)
{
    #ifndef DISABLE_NETWORK
    // The ownership model has the javascript engine owning all the socket data.
    // Therefore we rely on the javascript engine to finalise all sockets.
    // We just remove the listeners since they can hold references to the javascript socket objects which will
    // prevent finalisation.
    for (SocketDataBase* socket : _sockets)
    {
        if (socket != nullptr)
            socket->_eventList.RemoveAllListeners();
    }
    #endif
}

std::string Scripting::Stringify(JSContext* ctx, const JSValue val)
{
    return ExpressionStringifier::StringifyExpression(ctx, val);
}

bool Scripting::IsGameStateMutable()
{
    // Allow single player to alter game state anywhere
    if (Network::GetMode() == Network::Mode::none)
    {
        return true;
    }

    auto& scriptEngine = GetContext()->GetScriptEngine();
    auto& execInfo = scriptEngine.GetExecInfo();
    return execInfo.IsGameStateMutable();
}

int32_t Scripting::GetTargetAPIVersion()
{
    auto& scriptEngine = GetContext()->GetScriptEngine();
    auto& execInfo = scriptEngine.GetExecInfo();

    // Commands from the in-game console do not have a plug-in set
    auto plugin = execInfo.GetCurrentPlugin();
    if (plugin == nullptr)
    {
        // For in-game console, default to the current API version
        return kPluginApiVersion;
    }

    return plugin->GetTargetAPIVersion();
}

#endif
