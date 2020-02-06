/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Plugin.h"

#include "..\OpenRCT2.h"
#include "Duktape.hpp"

#include <algorithm>
#include <fstream>
#include <memory>

using namespace OpenRCT2::Scripting;

Plugin::Plugin(duk_context* context, const std::string& path)
    : _context(context)
    , _path(path)
{
}

void Plugin::Load()
{
    std::string projectedVariables = "console,context,map,network,park";
    if (!gOpenRCT2Headless)
    {
        projectedVariables += ",ui";
    }
    std::string code;
    {
        std::ifstream fs(_path);
        if (fs.is_open())
        {
            fs.seekg(0, std::ios::end);
            code.reserve(fs.tellg());
            fs.seekg(0, std::ios::beg);
            code.assign(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>());
        }
    }
    // Wrap the script in a function and pass the global objects as variables
    // so that if the script modifies them, they are not modified for other scripts.
    code = "(function(" + projectedVariables + "){" + code + "})(" + projectedVariables + ");";
    auto flags = DUK_COMPILE_EVAL | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_NOFILENAME;
    auto result = duk_eval_raw(_context, code.c_str(), code.size(), flags);
    if (result != DUK_ERR_NONE)
    {
        auto val = std::string(duk_safe_to_string(_context, -1));
        duk_pop(_context);
        throw std::runtime_error("Failed to load plug-in script: " + val);
    }

    _metadata = GetMetadata(DukValue::take_from_stack(_context));
}

void Plugin::Start()
{
    const auto& mainFunc = _metadata.Main;
    if (mainFunc.context() == nullptr)
    {
        throw std::runtime_error("No main function specified.");
    }

    mainFunc.push();
    auto result = duk_pcall(_context, 0);
    if (result != DUK_ERR_NONE)
    {
        auto val = std::string(duk_safe_to_string(_context, -1));
        duk_pop(_context);
        throw std::runtime_error("[" + _metadata.Name + "] " + val);
    }
    duk_pop(_context);

    _hasStarted = true;
}

void Plugin::Stop()
{
    _hasStarted = false;
}

void Plugin::Update()
{
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

        auto dukMinApiVersion = dukMetadata["minApiVersion"];
        if (dukMinApiVersion.type() == DukValue::Type::NUMBER)
        {
            metadata.MinApiVersion = dukMinApiVersion.as_int();
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

PluginType Plugin::ParsePluginType(const std::string_view& type)
{
    if (type == "server")
        return PluginType::Server;
    if (type == "client")
        return PluginType::Server;
    if (type == "server_client")
        return PluginType::Server;
    throw std::invalid_argument("Unknown plugin type.");
}
