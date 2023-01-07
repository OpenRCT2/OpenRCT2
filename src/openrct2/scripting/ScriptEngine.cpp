/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScriptEngine.h"

#    include "../PlatformEnvironment.h"
#    include "../actions/CustomAction.h"
#    include "../actions/GameAction.h"
#    include "../actions/RideCreateAction.h"
#    include "../actions/StaffHireNewAction.h"
#    include "../config/Config.h"
#    include "../core/EnumMap.hpp"
#    include "../core/File.h"
#    include "../core/FileScanner.h"
#    include "../core/Path.hpp"
#    include "../interface/InteractiveConsole.h"
#    include "../platform/Platform.h"
#    include "Duktape.hpp"
#    include "bindings/entity/ScEntity.hpp"
#    include "bindings/entity/ScGuest.hpp"
#    include "bindings/entity/ScLitter.hpp"
#    include "bindings/entity/ScPeep.hpp"
#    include "bindings/entity/ScStaff.hpp"
#    include "bindings/entity/ScVehicle.hpp"
#    include "bindings/game/ScCheats.hpp"
#    include "bindings/game/ScConsole.hpp"
#    include "bindings/game/ScContext.hpp"
#    include "bindings/game/ScDisposable.hpp"
#    include "bindings/game/ScProfiler.hpp"
#    include "bindings/network/ScNetwork.hpp"
#    include "bindings/network/ScPlayer.hpp"
#    include "bindings/network/ScPlayerGroup.hpp"
#    include "bindings/network/ScSocket.hpp"
#    include "bindings/object/ScObject.hpp"
#    include "bindings/ride/ScRide.hpp"
#    include "bindings/ride/ScRideStation.hpp"
#    include "bindings/world/ScClimate.hpp"
#    include "bindings/world/ScDate.hpp"
#    include "bindings/world/ScMap.hpp"
#    include "bindings/world/ScPark.hpp"
#    include "bindings/world/ScParkMessage.hpp"
#    include "bindings/world/ScScenario.hpp"
#    include "bindings/world/ScTile.hpp"
#    include "bindings/world/ScTileElement.hpp"

#    include <iostream>
#    include <memory>
#    include <stdexcept>
#    include <string>

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

struct ExpressionStringifier final
{
private:
    std::stringstream _ss;
    duk_context* _context{};
    int32_t _indent{};

    ExpressionStringifier(duk_context* ctx)
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

    void Stringify(const DukValue& val, bool canStartWithNewLine, int32_t nestLevel)
    {
        if (nestLevel >= 8)
        {
            _ss << "[...]";
            return;
        }

        switch (val.type())
        {
            case DukValue::Type::UNDEFINED:
                _ss << "undefined";
                break;
            case DukValue::Type::NULLREF:
                _ss << "null";
                break;
            case DukValue::Type::BOOLEAN:
                StringifyBoolean(val);
                break;
            case DukValue::Type::NUMBER:
                StringifyNumber(val);
                break;
            case DukValue::Type::STRING:
                _ss << "'" << val.as_string() << "'";
                break;
            case DukValue::Type::OBJECT:
                if (val.is_function())
                {
                    StringifyFunction(val);
                }
                else if (val.is_array())
                {
                    StringifyArray(val, canStartWithNewLine, nestLevel);
                }
                else
                {
                    StringifyObject(val, canStartWithNewLine, nestLevel);
                }
                break;
            case DukValue::Type::BUFFER:
                _ss << "[Buffer]";
                break;
            case DukValue::Type::POINTER:
                _ss << "[Pointer]";
                break;
            case DukValue::Type::LIGHTFUNC:
                _ss << "[LightFunc]";
                break;
        }
    }

    void StringifyArray(const DukValue& val, bool canStartWithNewLine, int32_t nestLevel)
    {
        constexpr auto maxItemsToShow = 4;

        val.push();
        auto arrayLen = duk_get_length(_context, -1);
        if (arrayLen == 0)
        {
            _ss << "[]";
        }
        else if (arrayLen == 1)
        {
            _ss << "[ ";
            for (duk_uarridx_t i = 0; i < arrayLen; i++)
            {
                if (duk_get_prop_index(_context, -1, i))
                {
                    if (i != 0)
                    {
                        _ss << ", ";
                    }
                    Stringify(DukValue::take_from_stack(_context), false, nestLevel + 1);
                }
            }
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
            for (duk_uarridx_t i = 0; i < arrayLen; i++)
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

                if (duk_get_prop_index(_context, -1, i))
                {
                    Stringify(DukValue::take_from_stack(_context), false, nestLevel + 1);
                }
            }
            _ss << " ]";
            PopIndent(2);
            if (canStartWithNewLine)
            {
                PopIndent();
            }
        }
        duk_pop(_context);
    }

    void StringifyObject(const DukValue& val, bool canStartWithNewLine, int32_t nestLevel)
    {
        auto numEnumerables = GetNumEnumerablesOnObject(val);
        if (numEnumerables == 0)
        {
            _ss << "{}";
        }
        else if (numEnumerables == 1)
        {
            _ss << "{ ";

            val.push();
            duk_enum(_context, -1, 0);
            auto index = 0;
            while (duk_next(_context, -1, 1))
            {
                if (index != 0)
                {
                    _ss << ", ";
                }
                auto value = DukValue::take_from_stack(_context, -1);
                auto key = DukValue::take_from_stack(_context, -1);
                if (key.type() == DukValue::Type::STRING)
                {
                    _ss << key.as_string() << ": ";
                }
                else
                {
                    // For some reason the key was not a string
                    _ss << "?: ";
                }
                Stringify(value, true, nestLevel + 1);
                index++;
            }
            duk_pop_2(_context);

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

            val.push();
            duk_enum(_context, -1, 0);
            auto index = 0;
            while (duk_next(_context, -1, 1))
            {
                if (index != 0)
                {
                    _ss << ",";
                    LineFeed();
                }
                auto value = DukValue::take_from_stack(_context, -1);
                auto key = DukValue::take_from_stack(_context, -1);
                if (key.type() == DukValue::Type::STRING)
                {
                    _ss << key.as_string() << ": ";
                }
                else
                {
                    // For some reason the key was not a string
                    _ss << "?: ";
                }
                Stringify(value, true, nestLevel + 1);
                index++;
            }
            duk_pop_2(_context);

            PopIndent(2);
            _ss << " }";

            if (canStartWithNewLine)
            {
                PopIndent();
            }
        }
    }

    void StringifyFunction(const DukValue& val)
    {
        val.push();
        if (duk_is_c_function(_context, -1))
        {
            _ss << "[Native Function]";
        }
        else if (duk_is_ecmascript_function(_context, -1))
        {
            _ss << "[ECMAScript Function]";
        }
        else
        {
            _ss << "[Function]";
        }
        duk_pop(_context);
    }

    void StringifyBoolean(const DukValue& val)
    {
        _ss << (val.as_bool() ? "true" : "false");
    }

    void StringifyNumber(const DukValue& val)
    {
        const auto d = val.as_double();
        const duk_int_t i = val.as_int();
        if (AlmostEqual<double>(d, i))
        {
            _ss << std::to_string(i);
        }
        else
        {
            _ss << std::to_string(d);
        }
    }

    size_t GetNumEnumerablesOnObject(const DukValue& val)
    {
        size_t count = 0;
        val.push();
        duk_enum(_context, -1, 0);
        while (duk_next(_context, -1, 0))
        {
            count++;
            duk_pop(_context);
        }
        duk_pop_2(_context);
        return count;
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
    static std::string StringifyExpression(const DukValue& val)
    {
        ExpressionStringifier instance(val.context());
        instance.Stringify(val, false, 0);
        return instance._ss.str();
    }
};

DukContext::DukContext()
{
    _context = duk_create_heap_default();
    if (_context == nullptr)
    {
        throw std::runtime_error("Unable to initialise duktape context.");
    }
}

DukContext::~DukContext()
{
    duk_destroy_heap(_context);
}

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

    auto ctx = static_cast<duk_context*>(_context);
    ScCheats::Register(ctx);
    ScClimate::Register(ctx);
    ScClimateState::Register(ctx);
    ScConfiguration::Register(ctx);
    ScConsole::Register(ctx);
    ScContext::Register(ctx);
    ScDate::Register(ctx);
    ScDisposable::Register(ctx);
    ScMap::Register(ctx);
    ScNetwork::Register(ctx);
    ScObject::Register(ctx);
    ScSmallSceneryObject::Register(ctx);
    ScPark::Register(ctx);
    ScParkMessage::Register(ctx);
    ScPlayer::Register(ctx);
    ScPlayerGroup::Register(ctx);
    ScProfiler::Register(ctx);
    ScRide::Register(ctx);
    ScRideStation::Register(ctx);
    ScRideObject::Register(ctx);
    ScRideObjectVehicle::Register(ctx);
    ScTile::Register(ctx);
    ScTileElement::Register(ctx);
    ScTrackIterator::Register(ctx);
    ScTrackSegment::Register(ctx);
    ScEntity::Register(ctx);
    ScLitter::Register(ctx);
    ScVehicle::Register(ctx);
    ScPeep::Register(ctx);
    ScGuest::Register(ctx);
    ScThought::Register(ctx);
#    ifndef DISABLE_NETWORK
    ScSocket::Register(ctx);
    ScListener::Register(ctx);
#    endif
    ScScenario::Register(ctx);
    ScScenarioObjective::Register(ctx);
    ScPatrolArea::Register(ctx);
    ScStaff::Register(ctx);

    dukglue_register_global(ctx, std::make_shared<ScCheats>(), "cheats");
    dukglue_register_global(ctx, std::make_shared<ScClimate>(), "climate");
    dukglue_register_global(ctx, std::make_shared<ScConsole>(_console), "console");
    dukglue_register_global(ctx, std::make_shared<ScContext>(_execInfo, _hookEngine), "context");
    dukglue_register_global(ctx, std::make_shared<ScDate>(), "date");
    dukglue_register_global(ctx, std::make_shared<ScMap>(ctx), "map");
    dukglue_register_global(ctx, std::make_shared<ScNetwork>(ctx), "network");
    dukglue_register_global(ctx, std::make_shared<ScPark>(), "park");
    dukglue_register_global(ctx, std::make_shared<ScProfiler>(ctx), "profiler");
    dukglue_register_global(ctx, std::make_shared<ScScenario>(), "scenario");

    RegisterConstants();

    _initialised = true;
    _transientPluginsEnabled = false;
    _transientPluginsStarted = false;

    LoadSharedStorage();
    ClearParkStorage();
}

class ConstantBuilder
{
private:
    duk_context* _ctx;
    DukValue _obj;

public:
    ConstantBuilder(duk_context* ctx)
        : _ctx(ctx)
    {
        duk_push_global_object(_ctx);
        _obj = DukValue::take_from_stack(_ctx);
    }

    ConstantBuilder& Namespace(std::string_view ns)
    {
        auto flags = DUK_DEFPROP_ENUMERABLE | DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_HAVE_ENUMERABLE
            | DUK_DEFPROP_HAVE_CONFIGURABLE | DUK_DEFPROP_HAVE_VALUE;

        // Create a new object for namespace
        duk_push_global_object(_ctx);
        duk_push_lstring(_ctx, ns.data(), ns.size());
        duk_push_object(_ctx);

        // Keep a reference to the namespace object
        duk_dup_top(_ctx);
        _obj = DukValue::take_from_stack(_ctx);

        // Place the namespace object into the global context
        duk_def_prop(_ctx, -3, flags);
        duk_pop(_ctx);

        return *this;
    }

    ConstantBuilder& Constant(std::string_view name, int32_t value)
    {
        auto flags = DUK_DEFPROP_ENUMERABLE | DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_HAVE_ENUMERABLE
            | DUK_DEFPROP_HAVE_CONFIGURABLE | DUK_DEFPROP_HAVE_VALUE;
        _obj.push();
        duk_push_lstring(_ctx, name.data(), name.size());
        duk_push_int(_ctx, value);
        duk_def_prop(_ctx, -3, flags);
        duk_pop(_ctx);
        return *this;
    }
};

void ScriptEngine::RegisterConstants()
{
    ConstantBuilder builder(_context);
    builder.Namespace("TrackSlope")
        .Constant("None", TRACK_SLOPE_NONE)
        .Constant("Up25", TRACK_SLOPE_UP_25)
        .Constant("Up60", TRACK_SLOPE_UP_60)
        .Constant("Down25", TRACK_SLOPE_DOWN_25)
        .Constant("Down60", TRACK_SLOPE_DOWN_60)
        .Constant("Up90", TRACK_SLOPE_UP_90)
        .Constant("Down90", TRACK_SLOPE_DOWN_90);
    builder.Namespace("TrackBanking")
        .Constant("None", TRACK_BANK_NONE)
        .Constant("BankLeft", TRACK_BANK_LEFT)
        .Constant("BankRight", TRACK_BANK_RIGHT)
        .Constant("UpsideDown", TRACK_BANK_UPSIDE_DOWN);
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
    if (!_hotReloadingInitialised && gConfigPlugin.EnableHotReloading && network_get_mode() == NETWORK_MODE_NONE)
    {
        SetupHotReloading();
    }
}

std::vector<std::string> ScriptEngine::GetPluginFiles() const
{
    // Scan for .js files in plugin directory
    std::vector<std::string> pluginFiles;
    auto base = _env.GetDirectoryPath(DIRBASE::USER, DIRID::PLUGIN);
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
        auto plugin = std::make_shared<Plugin>(_context, path);

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
    auto plugin = std::make_shared<Plugin>(_context, path);
    LoadPlugin(plugin);
}

void ScriptEngine::LoadPlugin(std::shared_ptr<Plugin>& plugin)
{
    try
    {
        if (!plugin->IsLoaded())
        {
            const auto& metadata = plugin->GetMetadata();
            if (metadata.MinApiVersion <= OPENRCT2_PLUGIN_API_VERSION)
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
        auto base = _env.GetDirectoryPath(DIRBASE::USER, DIRID::PLUGIN);
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
    auto networkMode = network_get_mode();
    if (networkMode == NETWORK_MODE_CLIENT)
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
        if (duk_peval_string(_context, command.c_str()) != 0)
        {
            std::string result = std::string(duk_safe_to_string(_context, -1));
            _console.WriteLineError(result);
        }
        else if (duk_get_type(_context, -1) != DUK_TYPE_UNDEFINED)
        {
            auto result = Stringify(DukValue::copy_from_stack(_context, -1));
            _console.WriteLine(result);
        }
        duk_pop(_context);
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

DukValue ScriptEngine::ExecutePluginCall(
    const std::shared_ptr<Plugin>& plugin, const DukValue& func, const std::vector<DukValue>& args, bool isGameStateMutable)
{
    duk_push_undefined(_context);
    auto dukUndefined = DukValue::take_from_stack(_context);
    return ExecutePluginCall(plugin, func, dukUndefined, args, isGameStateMutable);
}

// Must pass plugin by-value, a JS function could destroy the original reference
DukValue ScriptEngine::ExecutePluginCall(
    std::shared_ptr<Plugin> plugin, const DukValue& func, const DukValue& thisValue, const std::vector<DukValue>& args,
    bool isGameStateMutable)
{
    DukStackFrame frame(_context);
    if (func.is_function() && plugin->HasStarted())
    {
        ScriptExecutionInfo::PluginScope scope(_execInfo, plugin, isGameStateMutable);
        func.push();
        thisValue.push();
        for (const auto& arg : args)
        {
            arg.push();
        }
        auto result = duk_pcall_method(_context, static_cast<duk_idx_t>(args.size()));
        if (result == DUK_EXEC_SUCCESS)
        {
            return DukValue::take_from_stack(_context);
        }

        auto message = duk_safe_to_string(_context, -1);
        LogPluginInfo(plugin, message);
        duk_pop(_context);
    }
    return DukValue();
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
    auto plugin = std::make_shared<Plugin>(_context, std::string());
    plugin->SetCode(code);
    _plugins.push_back(plugin);
}

void ScriptEngine::RemoveNetworkPlugins()
{
    auto it = _plugins.begin();
    while (it != _plugins.end())
    {
        if (!(*it)->HasPath())
        {
            it = _plugins.erase(it);
        }
        else
        {
            it++;
        }
    }
}

GameActions::Result ScriptEngine::QueryOrExecuteCustomGameAction(std::string_view id, std::string_view args, bool isExecute)
{
    std::string actionz = std::string(id);
    auto kvp = _customActions.find(actionz);
    if (kvp != _customActions.end())
    {
        const auto& customAction = kvp->second;

        // Deserialise the JSON args
        std::string argsz(args);

        auto dukArgs = DuktapeTryParseJson(_context, argsz);
        if (!dukArgs)
        {
            auto action = GameActions::Result();
            action.Error = GameActions::Status::InvalidParameters;
            action.ErrorTitle = "Invalid JSON";
            return action;
        }

        // Ready to call plugin handler
        DukValue dukResult;
        if (!isExecute)
        {
            dukResult = ExecutePluginCall(customAction.Owner, customAction.Query, { *dukArgs }, false);
        }
        else
        {
            dukResult = ExecutePluginCall(customAction.Owner, customAction.Execute, { *dukArgs }, true);
        }
        return DukToGameActionResult(dukResult);
    }

    auto action = GameActions::Result();
    action.Error = GameActions::Status::Unknown;
    action.ErrorTitle = "Unknown custom action";
    return action;
}

GameActions::Result ScriptEngine::DukToGameActionResult(const DukValue& d)
{
    auto result = GameActions::Result();
    result.Error = static_cast<GameActions::Status>(AsOrDefault<int32_t>(d["error"]));
    result.ErrorTitle = AsOrDefault<std::string>(d["errorTitle"]);
    result.ErrorMessage = AsOrDefault<std::string>(d["errorMessage"]);
    result.Cost = AsOrDefault<int32_t>(d["cost"]);

    auto expenditureType = AsOrDefault<std::string>(d["expenditureType"]);
    if (!expenditureType.empty())
    {
        auto expenditure = StringToExpenditureType(expenditureType);
        if (expenditure != ExpenditureType::Count)
        {
            result.Expenditure = expenditure;
        }
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
    return ExpenditureType::Count;
}

DukValue ScriptEngine::GameActionResultToDuk(const GameAction& action, const GameActions::Result& result)
{
    DukStackFrame frame(_context);
    DukObject obj(_context);

    auto player = action.GetPlayer();
    if (player != -1)
    {
        obj.Set("player", action.GetPlayer());
    }
    if (result.Cost != MONEY32_UNDEFINED)
    {
        obj.Set("cost", result.Cost);
    }
    if (!result.Position.IsNull())
    {
        obj.Set("position", ToDuk(_context, result.Position));
    }

    if (result.Expenditure != ExpenditureType::Count)
    {
        obj.Set("expenditureType", ExpenditureTypeToString(result.Expenditure));
    }

    if (action.GetType() == GameCommand::CreateRide)
    {
        if (result.Error == GameActions::Status::Ok)
        {
            const auto rideIndex = result.GetData<RideId>();
            obj.Set("ride", rideIndex.ToUnderlying());
        }
    }
    else if (action.GetType() == GameCommand::HireNewStaffMember)
    {
        if (result.Error == GameActions::Status::Ok)
        {
            const auto actionResult = result.GetData<StaffHireNewActionResult>();
            if (!actionResult.StaffEntityId.IsNull())
            {
                obj.Set("peep", actionResult.StaffEntityId.ToUnderlying());
            }
        }
    }

    return obj.Take();
}

bool ScriptEngine::RegisterCustomAction(
    const std::shared_ptr<Plugin>& plugin, std::string_view action, const DukValue& query, const DukValue& execute)
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

class DukToGameActionParameterVisitor : public GameActionParameterVisitor
{
private:
    DukValue _dukValue;

public:
    DukToGameActionParameterVisitor(DukValue&& dukValue)
        : _dukValue(std::move(dukValue))
    {
    }

    void Visit(std::string_view name, bool& param) override
    {
        param = _dukValue[name].as_bool();
    }

    void Visit(std::string_view name, int32_t& param) override
    {
        param = _dukValue[name].as_int();
    }

    void Visit(std::string_view name, std::string& param) override
    {
        param = _dukValue[name].as_string();
    }
};

class DukFromGameActionParameterVisitor : public GameActionParameterVisitor
{
private:
    DukObject& _dukObject;

public:
    DukFromGameActionParameterVisitor(DukObject& dukObject)
        : _dukObject(dukObject)
    {
    }

    void Visit(std::string_view name, bool& param) override
    {
        std::string szName(name);
        _dukObject.Set(szName.c_str(), param);
    }

    void Visit(std::string_view name, int32_t& param) override
    {
        std::string szName(name);
        _dukObject.Set(szName.c_str(), param);
    }

    void Visit(std::string_view name, std::string& param) override
    {
        std::string szName(name);
        _dukObject.Set(szName.c_str(), param);
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
    { "climateset", GameCommand::SetClimate },
    { "footpathplace", GameCommand::PlacePath },
    { "footpathlayoutplace", GameCommand::PlacePathLayout },
    { "footpathremove", GameCommand::RemovePath },
    { "footpathadditionplace", GameCommand::PlaceFootpathAddition },
    { "footpathadditionremove", GameCommand::RemoveFootpathAddition },
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
    { "setcheat", GameCommand::Cheat },
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

static std::unique_ptr<GameAction> CreateGameActionFromActionId(const std::string& name)
{
    auto result = ActionNameToType.find(name);
    if (result != ActionNameToType.end())
    {
        return GameActions::Create(result->second);
    }
    return nullptr;
}

void ScriptEngine::RunGameActionHooks(const GameAction& action, GameActions::Result& result, bool isExecute)
{
    DukStackFrame frame(_context);

    auto hookType = isExecute ? HOOK_TYPE::ACTION_EXECUTE : HOOK_TYPE::ACTION_QUERY;
    if (_hookEngine.HasSubscriptions(hookType))
    {
        DukObject obj(_context);

        auto actionId = action.GetType();
        if (action.GetType() == GameCommand::Custom)
        {
            auto customAction = static_cast<const CustomAction&>(action);
            obj.Set("action", customAction.GetId());

            auto dukArgs = DuktapeTryParseJson(_context, customAction.GetJson());
            if (dukArgs)
            {
                obj.Set("args", *dukArgs);
            }
            else
            {
                DukObject args(_context);
                obj.Set("args", args.Take());
            }
        }
        else
        {
            auto actionName = GetActionName(actionId);
            if (!actionName.empty())
            {
                obj.Set("action", actionName);
            }

            DukObject args(_context);
            DukFromGameActionParameterVisitor visitor(args);
            const_cast<GameAction&>(action).AcceptParameters(visitor);
            const_cast<GameAction&>(action).AcceptFlags(visitor);
            obj.Set("args", args.Take());
        }

        obj.Set("player", action.GetPlayer());
        obj.Set("type", EnumValue(actionId));

        auto flags = action.GetActionFlags();
        obj.Set("isClientOnly", (flags & GameActions::Flags::ClientOnly) != 0);

        obj.Set("result", GameActionResultToDuk(action, result));
        auto dukEventArgs = obj.Take();

        _hookEngine.Call(hookType, dukEventArgs, false);

        if (!isExecute)
        {
            auto dukResult = dukEventArgs["result"];
            if (dukResult.type() == DukValue::Type::OBJECT)
            {
                auto error = AsOrDefault<int32_t>(dukResult["error"]);
                if (error != 0)
                {
                    result.Error = static_cast<GameActions::Status>(error);
                    result.ErrorTitle = AsOrDefault<std::string>(dukResult["errorTitle"]);
                    result.ErrorMessage = AsOrDefault<std::string>(dukResult["errorMessage"]);
                }
            }
        }
    }
}

std::unique_ptr<GameAction> ScriptEngine::CreateGameAction(const std::string& actionid, const DukValue& args)
{
    auto action = CreateGameActionFromActionId(actionid);
    if (action != nullptr)
    {
        DukValue argsCopy = args;
        DukToGameActionParameterVisitor visitor(std::move(argsCopy));
        action->AcceptParameters(visitor);
        if (args["flags"].type() == DukValue::Type::NUMBER)
        {
            action->AcceptFlags(visitor);
        }
        return action;
    }

    // Serialise args to json so that it can be sent
    auto ctx = args.context();
    if (args.type() == DukValue::Type::OBJECT)
    {
        args.push();
    }
    else
    {
        duk_push_object(ctx);
    }
    auto jsonz = duk_json_encode(ctx, -1);
    auto json = std::string(jsonz);
    duk_pop(ctx);
    return std::make_unique<CustomAction>(actionid, json);
}

void ScriptEngine::InitSharedStorage()
{
    duk_push_object(_context);
    _sharedStorage = std::move(DukValue::take_from_stack(_context));
}

void ScriptEngine::LoadSharedStorage()
{
    InitSharedStorage();

    auto path = _env.GetFilePath(PATHID::PLUGIN_STORE);
    try
    {
        if (File::Exists(path))
        {
            auto data = File::ReadAllBytes(path);
            auto result = DuktapeTryParseJson(
                _context, std::string_view(reinterpret_cast<const char*>(data.data()), data.size()));
            if (result)
            {
                _sharedStorage = std::move(*result);
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
    auto path = _env.GetFilePath(PATHID::PLUGIN_STORE);
    try
    {
        _sharedStorage.push();
        auto json = std::string(duk_json_encode(_context, -1));
        duk_pop(_context);

        File::WriteAllBytes(path, json.c_str(), json.size());
    }
    catch (const std::exception&)
    {
        Console::Error::WriteLine("Unable to write to '%s'", path.c_str());
    }
}

void ScriptEngine::ClearParkStorage()
{
    duk_push_object(_context);
    _parkStorage = std::move(DukValue::take_from_stack(_context));
}

std::string ScriptEngine::GetParkStorageAsJSON()
{
    _parkStorage.push();
    auto json = std::string(duk_json_encode(_context, -1));
    duk_pop(_context);
    return json;
}

void ScriptEngine::SetParkStorageFromJSON(std::string_view value)
{
    auto result = DuktapeTryParseJson(_context, value);
    if (result)
    {
        _parkStorage = std::move(*result);
    }
}

IntervalHandle ScriptEngine::AllocateHandle()
{
    for (size_t i = 0; i < _intervals.size(); i++)
    {
        if (!_intervals[i].IsValid())
        {
            return static_cast<IntervalHandle>(i + 1);
        }
    }
    _intervals.emplace_back();
    return static_cast<IntervalHandle>(_intervals.size());
}

IntervalHandle ScriptEngine::AddInterval(const std::shared_ptr<Plugin>& plugin, int32_t delay, bool repeat, DukValue&& callback)
{
    auto handle = AllocateHandle();
    if (handle != 0)
    {
        auto& interval = _intervals[static_cast<size_t>(handle) - 1];
        interval.Owner = plugin;
        interval.Handle = handle;
        interval.Delay = delay;
        interval.LastTimestamp = _lastIntervalTimestamp;
        interval.Callback = std::move(callback);
        interval.Repeat = repeat;
    }
    return handle;
}

void ScriptEngine::RemoveInterval(const std::shared_ptr<Plugin>& plugin, IntervalHandle handle)
{
    if (handle > 0 && static_cast<size_t>(handle) <= _intervals.size())
    {
        auto& interval = _intervals[static_cast<size_t>(handle) - 1];

        // Only allow owner or REPL (nullptr) to remove intervals
        if (plugin == nullptr || interval.Owner == plugin)
        {
            interval = {};
        }
    }
}

void ScriptEngine::UpdateIntervals()
{
    uint32_t timestamp = Platform::GetTicks();
    if (timestamp < _lastIntervalTimestamp)
    {
        // timestamp has wrapped, subtract all intervals by the remaining amount before wrap
        auto delta = static_cast<int64_t>(std::numeric_limits<uint32_t>::max() - _lastIntervalTimestamp);
        for (auto& interval : _intervals)
        {
            if (interval.IsValid())
            {
                interval.LastTimestamp = -delta;
            }
        }
    }
    _lastIntervalTimestamp = timestamp;

    for (auto& interval : _intervals)
    {
        if (interval.IsValid())
        {
            if (timestamp >= interval.LastTimestamp + interval.Delay)
            {
                ExecutePluginCall(interval.Owner, interval.Callback, {}, false);

                interval.LastTimestamp = timestamp;
                if (!interval.Repeat)
                {
                    RemoveInterval(nullptr, interval.Handle);
                }
            }
        }
    }
}

void ScriptEngine::RemoveIntervals(const std::shared_ptr<Plugin>& plugin)
{
    for (auto& interval : _intervals)
    {
        if (interval.Owner == plugin)
        {
            interval = {};
        }
    }
}

#    ifndef DISABLE_NETWORK
void ScriptEngine::AddSocket(const std::shared_ptr<ScSocketBase>& socket)
{
    _sockets.push_back(socket);
}
#    endif

void ScriptEngine::UpdateSockets()
{
#    ifndef DISABLE_NETWORK
    // Use simple for i loop as Update calls can modify the list
    auto it = _sockets.begin();
    while (it != _sockets.end())
    {
        auto& socket = *it;
        socket->Update();
        if (socket->IsDisposed())
        {
            it = _sockets.erase(it);
        }
        else
        {
            it++;
        }
    }
#    endif
}

void ScriptEngine::RemoveSockets(const std::shared_ptr<Plugin>& plugin)
{
#    ifndef DISABLE_NETWORK
    auto it = _sockets.begin();
    while (it != _sockets.end())
    {
        auto socket = it->get();
        if (socket->GetPlugin() == plugin)
        {
            socket->Dispose();
            it = _sockets.erase(it);
        }
        else
        {
            it++;
        }
    }
#    endif
}

std::string OpenRCT2::Scripting::Stringify(const DukValue& val)
{
    return ExpressionStringifier::StringifyExpression(val);
}

std::string OpenRCT2::Scripting::ProcessString(const DukValue& value)
{
    if (value.type() == DukValue::Type::STRING)
        return value.as_string();
    return {};
}

bool OpenRCT2::Scripting::IsGameStateMutable()
{
    // Allow single player to alter game state anywhere
    if (network_get_mode() == NETWORK_MODE_NONE)
    {
        return true;
    }

    auto& scriptEngine = GetContext()->GetScriptEngine();
    auto& execInfo = scriptEngine.GetExecInfo();
    return execInfo.IsGameStateMutable();
}

void OpenRCT2::Scripting::ThrowIfGameStateNotMutable()
{
    // Allow single player to alter game state anywhere
    if (network_get_mode() != NETWORK_MODE_NONE)
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto& execInfo = scriptEngine.GetExecInfo();
        if (!execInfo.IsGameStateMutable())
        {
            auto ctx = scriptEngine.GetContext();
            duk_error(ctx, DUK_ERR_ERROR, "Game state is not mutable in this context.");
        }
    }
}

int32_t OpenRCT2::Scripting::GetTargetAPIVersion()
{
    auto& scriptEngine = GetContext()->GetScriptEngine();
    auto& execInfo = scriptEngine.GetExecInfo();

    // Commands from the in-game console do not have a plug-in set
    auto plugin = execInfo.GetCurrentPlugin();
    if (plugin == nullptr)
    {
        // For in-game console, default to the current API version
        return OPENRCT2_PLUGIN_API_VERSION;
    }

    return plugin->GetTargetAPIVersion();
}

duk_bool_t duk_exec_timeout_check(void*)
{
    return false;
}

#endif
