/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "Plugin.h"

#    include "../Diagnostic.h"
#    include "../OpenRCT2.h"
#    include "../core/File.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

#    include <algorithm>
#    include <fstream>
#    include <memory>

using namespace OpenRCT2::Scripting;

Plugin::Plugin(duk_context* context, std::string_view path)
    : _context(context)
    , _path(path)
{
}

void Plugin::SetCode(std::string_view code)
{
    _code = code;
}

void Plugin::Load()
{
    if (!_path.empty())
    {
        LoadCodeFromFile();
    }

    std::string projectedVariables = "console,context,date,map,network,park,profiler";
    if (!gOpenRCT2Headless)
    {
        projectedVariables += ",ui";
    }

    // Wrap the script in a function and pass the global objects as variables
    // so that if the script modifies them, they are not modified for other scripts.

    // clang-format off
    auto code = _code;
    code =
        "     (function(" + projectedVariables + ") {"
        "         var __metadata__ = null;"
        "         var registerPlugin = function(m) { __metadata__ = m };"
        "         (function(__metadata__) {"
                      + code +
        "         })();"
        "         return __metadata__;"
        "     })(" + projectedVariables + ");";
    // clang-format on

    auto flags = DUK_COMPILE_EVAL | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_NOFILENAME;
    auto result = duk_eval_raw(_context, code.c_str(), code.size(), flags);
    if (result != DUK_ERR_NONE)
    {
        auto val = std::string(duk_safe_to_string(_context, -1));
        duk_pop(_context);
        throw std::runtime_error("Failed to load plug-in script: " + val);
    }

    _metadata = GetMetadata(DukValue::take_from_stack(_context));
    _hasLoaded = true;
}

void Plugin::Start()
{
    if (!_hasLoaded)
    {
        throw std::runtime_error("Plugin has not been loaded.");
    }

    const auto& mainFunc = _metadata.Main;
    if (mainFunc.context() == nullptr)
    {
        throw std::runtime_error("No main function specified.");
    }

    _hasStarted = true;

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

void Plugin::StopBegin()
{
    _isStopping = true;
}

void Plugin::StopEnd()
{
    _isStopping = false;
    _hasStarted = false;
}

void Plugin::ThrowIfStopping() const
{
    if (IsStopping())
    {
        duk_error(_context, DUK_ERR_ERROR, "Plugin is stopping.");
    }
}

void Plugin::Unload()
{
    _metadata.Main = {};
    _hasLoaded = false;
}

void Plugin::LoadCodeFromFile()
{
    _code = File::ReadAllText(_path);
}

static std::string TryGetString(const DukValue& value, const std::string& message)
{
    if (value.type() != DukValue::Type::STRING)
        throw std::runtime_error(message);
    return value.as_string();
}

PluginMetadata Plugin::GetMetadata(const DukValue& dukMetadata)
{
    PluginMetadata metadata;
    if (dukMetadata.type() == DukValue::Type::OBJECT)
    {
        metadata.Name = TryGetString(dukMetadata["name"], "Plugin name not specified.");
        metadata.Version = TryGetString(dukMetadata["version"], "Plugin version not specified.");
        metadata.Type = ParsePluginType(TryGetString(dukMetadata["type"], "Plugin type not specified."));

        CheckForLicence(dukMetadata["licence"], metadata.Name);

        auto dukMinApiVersion = dukMetadata["minApiVersion"];
        if (dukMinApiVersion.type() == DukValue::Type::NUMBER)
        {
            metadata.MinApiVersion = dukMinApiVersion.as_int();
        }

        auto dukTargetApiVersion = dukMetadata["targetApiVersion"];
        if (dukTargetApiVersion.type() == DukValue::Type::NUMBER)
        {
            metadata.TargetApiVersion = dukTargetApiVersion.as_int();
        }

        auto dukAuthors = dukMetadata["authors"];
        dukAuthors.push();
        if (dukAuthors.is_array())
        {
            auto elements = dukAuthors.as_array();
            std::transform(elements.begin(), elements.end(), std::back_inserter(metadata.Authors), [](const DukValue& v) {
                return v.as_string();
            });
        }
        else if (dukAuthors.type() == DukValue::Type::STRING)
        {
            metadata.Authors = { dukAuthors.as_string() };
        }
        metadata.Main = dukMetadata["main"];
    }
    return metadata;
}

PluginType Plugin::ParsePluginType(std::string_view type)
{
    if (type == "local")
        return PluginType::Local;
    if (type == "remote")
        return PluginType::Remote;
    if (type == "intransient")
        return PluginType::Intransient;
    throw std::invalid_argument("Unknown plugin type.");
}

void Plugin::CheckForLicence(const DukValue& dukLicence, std::string_view pluginName)
{
    if (dukLicence.type() != DukValue::Type::STRING || dukLicence.as_string().empty())
        log_error("Plugin %s does not specify a licence", std::string(pluginName).c_str());
}

int32_t Plugin::GetTargetAPIVersion() const
{
    if (_metadata.TargetApiVersion)
        return *_metadata.TargetApiVersion;

    // If not specified, default to 33 since that is the API version from before 'targetAPIVersion' was introduced.
    return 33;
}

bool Plugin::IsTransient() const
{
    return _metadata.Type != PluginType::Intransient;
}

#endif
