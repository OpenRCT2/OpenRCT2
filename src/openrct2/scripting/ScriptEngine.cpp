/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScriptEngine.h"

#    include "../PlatformEnvironment.h"
#    include "../actions/GameAction.h"
#    include "../actions/RideCreateAction.hpp"
#    include "../config/Config.h"
#    include "../core/File.h"
#    include "../core/FileScanner.h"
#    include "../core/Path.hpp"
#    include "../interface/InteractiveConsole.h"
#    include "../platform/Platform2.h"
#    include "Duktape.hpp"
#    include "ScConsole.hpp"
#    include "ScContext.hpp"
#    include "ScDate.hpp"
#    include "ScDisposable.hpp"
#    include "ScEntity.hpp"
#    include "ScMap.hpp"
#    include "ScNetwork.hpp"
#    include "ScObject.hpp"
#    include "ScPark.hpp"
#    include "ScRide.hpp"
#    include "ScTile.hpp"

#    include <iostream>
#    include <stdexcept>

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

static constexpr int32_t OPENRCT2_PLUGIN_API_VERSION = 1;

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

    void Stringify(const DukValue& val, bool canStartWithNewLine)
    {
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
                    StringifyArray(val, canStartWithNewLine);
                }
                else
                {
                    StringifyObject(val, canStartWithNewLine);
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

    void StringifyArray(const DukValue& val, bool canStartWithNewLine)
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
                    Stringify(DukValue::take_from_stack(_context), false);
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
                else
                {
                    if (duk_get_prop_index(_context, -1, i))
                    {
                        Stringify(DukValue::take_from_stack(_context), false);
                    }
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

    void StringifyObject(const DukValue& val, bool canStartWithNewLine)
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
                Stringify(value, true);
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
                Stringify(value, true);
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
        instance.Stringify(val, false);
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
    auto ctx = (duk_context*)_context;
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
    ScPlayer::Register(ctx);
    ScPlayerGroup::Register(ctx);
    ScRide::Register(ctx);
    ScRideObject::Register(ctx);
    ScTile::Register(ctx);
    ScTileElement::Register(ctx);
    ScEntity::Register(ctx);
    ScPeep::Register(ctx);

    dukglue_register_global(ctx, std::make_shared<ScConsole>(_console), "console");
    dukglue_register_global(ctx, std::make_shared<ScContext>(_execInfo, _hookEngine), "context");
    dukglue_register_global(ctx, std::make_shared<ScDate>(), "date");
    dukglue_register_global(ctx, std::make_shared<ScMap>(ctx), "map");
    dukglue_register_global(ctx, std::make_shared<ScNetwork>(ctx), "network");
    dukglue_register_global(ctx, std::make_shared<ScPark>(), "park");

    _initialised = true;
    _pluginsLoaded = false;
    _pluginsStarted = false;

    InitSharedStorage();
}

void ScriptEngine::LoadPlugins()
{
    if (!_initialised)
    {
        Initialise();
    }
    if (_pluginsLoaded)
    {
        UnloadPlugins();
    }

    auto base = _env.GetDirectoryPath(DIRBASE::USER, DIRID::PLUGIN);
    if (Path::DirectoryExists(base))
    {
        auto pattern = Path::Combine(base, "*.js");
        auto scanner = std::unique_ptr<IFileScanner>(Path::ScanDirectory(pattern, true));
        while (scanner->Next())
        {
            auto path = std::string(scanner->GetPath());
            if (ShouldLoadScript(path))
            {
                LoadPlugin(path);
            }
        }

        if (gConfigPlugin.enable_hot_reloading && network_get_mode() == NETWORK_MODE_NONE)
        {
            SetupHotReloading();
        }
    }
    _pluginsLoaded = true;
    _pluginsStarted = false;
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
        ScriptExecutionInfo::PluginScope scope(_execInfo, plugin, false);
        plugin->Load();

        auto metadata = plugin->GetMetadata();
        if (metadata.MinApiVersion <= OPENRCT2_PLUGIN_API_VERSION)
        {
            LogPluginInfo(plugin, "Loaded");
            _plugins.push_back(std::move(plugin));
        }
        else
        {
            LogPluginInfo(plugin, "Requires newer API version: v" + std::to_string(metadata.MinApiVersion));
        }
    }
    catch (const std::exception& e)
    {
        _console.WriteLineError(e.what());
    }
}

void ScriptEngine::StopPlugin(std::shared_ptr<Plugin> plugin)
{
    if (plugin->HasStarted())
    {
        RemoveCustomGameActions(plugin);
        _hookEngine.UnsubscribeAll(plugin);
        for (auto callback : _pluginStoppedSubscriptions)
        {
            callback(plugin);
        }

        ScriptExecutionInfo::PluginScope scope(_execInfo, plugin, false);
        try
        {
            plugin->Stop();
        }
        catch (const std::exception& e)
        {
            _console.WriteLineError(e.what());
        }
    }
}

bool ScriptEngine::ShouldLoadScript(const std::string& path)
{
    // A lot of JavaScript is often found in a node_modules directory tree and is most likely unwanted, so ignore it
    return path.find("/node_modules/") == std::string::npos && path.find("\\node_modules\\") == std::string::npos;
}

void ScriptEngine::SetupHotReloading()
{
    try
    {
        auto base = _env.GetDirectoryPath(DIRBASE::USER, DIRID::PLUGIN);
        _pluginFileWatcher = std::make_unique<FileWatcher>(base);
        _pluginFileWatcher->OnFileChanged = [this](const std::string& path) {
            std::lock_guard<std::mutex> guard(_changedPluginFilesMutex);
            _changedPluginFiles.emplace(path);
        };
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "Unable to enable hot reloading of plugins: %s\n", e.what());
    }
}

void ScriptEngine::AutoReloadPlugins()
{
    if (_changedPluginFiles.size() > 0)
    {
        std::lock_guard<std::mutex> guard(_changedPluginFilesMutex);
        for (auto& path : _changedPluginFiles)
        {
            auto findResult = std::find_if(_plugins.begin(), _plugins.end(), [&path](const std::shared_ptr<Plugin>& plugin) {
                return Path::Equals(path, plugin->GetPath());
            });
            if (findResult != _plugins.end())
            {
                auto& plugin = *findResult;
                try
                {
                    StopPlugin(plugin);

                    ScriptExecutionInfo::PluginScope scope(_execInfo, plugin, false);
                    plugin->Load();
                    LogPluginInfo(plugin, "Reloaded");
                    plugin->Start();
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

void ScriptEngine::UnloadPlugins()
{
    StopPlugins();
    for (auto& plugin : _plugins)
    {
        LogPluginInfo(plugin, "Unloaded");
    }
    _plugins.clear();
    _pluginsLoaded = false;
    _pluginsStarted = false;
}

void ScriptEngine::StartPlugins()
{
    LoadSharedStorage();

    for (auto& plugin : _plugins)
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
    _pluginsStarted = true;
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

void ScriptEngine::StopPlugins()
{
    for (auto& plugin : _plugins)
    {
        if (plugin->HasStarted())
        {
            StopPlugin(plugin);
            LogPluginInfo(plugin, "Stopped");
        }
    }
    _pluginsStarted = false;
}

void ScriptEngine::Update()
{
    if (!_initialised)
    {
        Initialise();
    }

    if (_pluginsLoaded)
    {
        if (!_pluginsStarted)
        {
            StartPlugins();
        }
        else
        {
            auto tick = Platform::GetTicks();
            if (tick - _lastHotReloadCheckTick > 1000)
            {
                AutoReloadPlugins();
                _lastHotReloadCheckTick = tick;
            }
        }
    }

    ProcessREPL();
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
    DukStackFrame frame(_context);
    if (func.is_function())
    {
        ScriptExecutionInfo::PluginScope scope(_execInfo, plugin, isGameStateMutable);
        func.push();
        for (const auto& arg : args)
        {
            arg.push();
        }
        auto result = duk_pcall(_context, static_cast<duk_idx_t>(args.size()));
        if (result == DUK_EXEC_SUCCESS)
        {
            return DukValue::take_from_stack(_context);
        }
        else
        {
            auto message = duk_safe_to_string(_context, -1);
            LogPluginInfo(plugin, message);
            duk_pop(_context);
        }
    }
    return DukValue();
}

void ScriptEngine::LogPluginInfo(const std::shared_ptr<Plugin>& plugin, const std::string_view& message)
{
    const auto& pluginName = plugin->GetMetadata().Name;
    _console.WriteLine("[" + pluginName + "] " + std::string(message));
}

void ScriptEngine::AddNetworkPlugin(const std::string_view& code)
{
    auto plugin = std::make_shared<Plugin>(_context, std::string());
    plugin->SetCode(code);
    LoadPlugin(plugin);
}

std::unique_ptr<GameActionResult> ScriptEngine::QueryOrExecuteCustomGameAction(
    const std::string_view& id, const std::string_view& args, bool isExecute)
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
            auto action = std::make_unique<GameActionResult>();
            action->Error = GA_ERROR::INVALID_PARAMETERS;
            action->ErrorTitle = "Invalid JSON";
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
    else
    {
        auto action = std::make_unique<GameActionResult>();
        action->Error = GA_ERROR::UNKNOWN;
        action->ErrorTitle = "Unknown custom action";
        return action;
    }
}

std::unique_ptr<GameActionResult> ScriptEngine::DukToGameActionResult(const DukValue& d)
{
    auto result = std::make_unique<GameActionResult>();
    result->Error = static_cast<GA_ERROR>(AsOrDefault<int32_t>(d["error"]));
    result->ErrorTitle = AsOrDefault<std::string>(d["errorTitle"]);
    result->ErrorMessage = AsOrDefault<std::string>(d["errorMessage"]);
    result->Cost = AsOrDefault<int32_t>(d["cost"]);
    return result;
}

DukValue ScriptEngine::PositionToDuk(const CoordsXYZ& position)
{
    DukStackFrame frame(_context);
    duk_context* ctx = _context;
    auto obj = duk_push_object(ctx);
    duk_push_int(ctx, position.x);
    duk_put_prop_string(ctx, obj, "x");
    duk_push_int(ctx, position.y);
    duk_put_prop_string(ctx, obj, "y");
    duk_push_int(ctx, position.z);
    duk_put_prop_string(ctx, obj, "z");
    return DukValue::take_from_stack(ctx);
}

DukValue ScriptEngine::GameActionResultToDuk(const GameAction& action, const std::unique_ptr<GameActionResult>& result)
{
    DukStackFrame frame(_context);
    DukObject obj(_context);

    auto player = action.GetPlayer();
    if (player != -1)
    {
        obj.Set("player", action.GetPlayer());
    }
    if (result->Cost != MONEY32_UNDEFINED)
    {
        obj.Set("cost", result->Cost);
    }
    if (!result->Position.isNull())
    {
        obj.Set("position", PositionToDuk(result->Position));
    }

    if (action.GetType() == GAME_COMMAND_CREATE_RIDE)
    {
        auto& rideCreateResult = static_cast<RideCreateGameActionResult&>(*result.get());
        if (rideCreateResult.rideIndex != RIDE_ID_NULL)
        {
            obj.Set("ride", rideCreateResult.rideIndex);
        }
    }

    return obj.Take();
}

bool ScriptEngine::RegisterCustomAction(
    const std::shared_ptr<Plugin>& plugin, const std::string_view& action, const DukValue& query, const DukValue& execute)
{
    std::string actionz = std::string(action);
    if (_customActions.find(actionz) != _customActions.end())
    {
        return false;
    }

    CustomAction customAction;
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

void ScriptEngine::RunGameActionHooks(const GameAction& action, std::unique_ptr<GameActionResult>& result, bool isExecute)
{
    DukStackFrame frame(_context);

    auto hookType = isExecute ? HOOK_TYPE::ACTION_EXECUTE : HOOK_TYPE::ACTION_QUERY;
    if (_hookEngine.HasSubscriptions(hookType))
    {
        DukObject obj(_context);
        obj.Set("type", action.GetType());

        auto flags = action.GetActionFlags();
        obj.Set("isClientOnly", (flags & GA_FLAGS::CLIENT_ONLY) != 0);

        if (action.GetType() == GAME_COMMAND_CREATE_RIDE)
        {
            auto& rideCreateAction = static_cast<const RideCreateAction&>(action);
            obj.Set("rideType", rideCreateAction.GetRideType());
            obj.Set("rideObject", rideCreateAction.GetRideObject());
        }

        obj.Set("result", GameActionResultToDuk(action, result));
        auto dukEventArgs = obj.Take();

        _hookEngine.Call(hookType, dukEventArgs, false);

        if (!isExecute)
        {
            auto error = AsOrDefault<int32_t>(dukEventArgs["error"]);
            if (error != 0)
            {
                result->Error = static_cast<GA_ERROR>(error);
                result->ErrorTitle = AsOrDefault<std::string>(dukEventArgs["errorTitle"]);
                result->ErrorMessage = AsOrDefault<std::string>(dukEventArgs["errorMessage"]);
            }
        }
    }
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
            auto result = DuktapeTryParseJson(_context, std::string_view((const char*)data.data(), data.size()));
            if (result)
            {
                _sharedStorage = std::move(*result);
            }
        }
    }
    catch (const std::exception&)
    {
        fprintf(stderr, "Unable to read '%s'\n", path.c_str());
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
        fprintf(stderr, "Unable to write to '%s'\n", path.c_str());
    }
}

std::string OpenRCT2::Scripting::Stringify(const DukValue& val)
{
    return ExpressionStringifier::StringifyExpression(val);
}

bool OpenRCT2::Scripting::IsGameStateMutable()
{
    // Allow single player to alter game state anywhere
    if (network_get_mode() == NETWORK_MODE_NONE)
    {
        return true;
    }
    else
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto& execInfo = scriptEngine.GetExecInfo();
        return execInfo.IsGameStateMutable();
    }
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

#endif
