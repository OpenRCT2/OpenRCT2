/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../windows/Windows.h"
    #include "CustomMenu.h"
    #include "ScImageManager.hpp"
    #include "ScTileSelection.hpp"
    #include "ScViewport.hpp"
    #include "ScWindow.hpp"

    #include <algorithm>
    #include <memory>
    #include <openrct2/Context.h>
    #include <openrct2/Input.h>
    #include <openrct2/scenario/Scenario.h>
    #include <openrct2/scenario/ScenarioRepository.h>
    #include <openrct2/scripting/Duktape.hpp>
    #include <openrct2/scripting/ScriptEngine.h>
    #include <string>

namespace OpenRCT2::Scripting
{
    class Plugin;
}

namespace OpenRCT2::Ui::Windows
{
    WindowBase* WindowCustomOpen(std::shared_ptr<OpenRCT2::Scripting::Plugin> owner, DukValue dukDesc);
}

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<ScenarioCategory> ScenarioCategoryMap({
        { "beginner", ScenarioCategory::beginner },
        { "challenging", ScenarioCategory::challenging },
        { "expert", ScenarioCategory::expert },
        { "real", ScenarioCategory::real },
        { "other", ScenarioCategory::other },
        { "dlc", ScenarioCategory::dlc },
        { "build_your_own", ScenarioCategory::buildYourOwn },
        { "competitions", ScenarioCategory::competitions },
    });

    static const DukEnumMap<ScenarioSource> ScenarioSourceMap({
        { "rct1", ScenarioSource::RCT1 },
        { "rct1_aa", ScenarioSource::RCT1_AA },
        { "rct1_ll", ScenarioSource::RCT1_LL },
        { "rct2", ScenarioSource::RCT2 },
        { "rct2_ww", ScenarioSource::RCT2_WW },
        { "rct2_tt", ScenarioSource::RCT2_TT },
        { "real", ScenarioSource::Real },
        { "extras", ScenarioSource::Extras },
        { "other", ScenarioSource::Other },
    });

    template<>
    inline DukValue ToDuk(duk_context* ctx, const ScenarioCategory& value)
    {
        const auto& entry = ScenarioCategoryMap.find(value);
        if (entry != ScenarioCategoryMap.end())
            return ToDuk(ctx, entry->first);
        return ToDuk(ctx, ScenarioCategoryMap[ScenarioCategory::other]);
    }

    template<>
    inline DukValue ToDuk(duk_context* ctx, const ScenarioSource& value)
    {
        const auto& entry = ScenarioSourceMap.find(value);
        if (entry != ScenarioSourceMap.end())
            return ToDuk(ctx, entry->first);
        return ToDuk(ctx, ScenarioSourceMap[ScenarioSource::Other]);
    }

    class ScTool
    {
    private:
        duk_context* _ctx{};

    public:
        ScTool(duk_context* ctx)
            : _ctx(ctx)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScTool::id_get, nullptr, "id");
            dukglue_register_property(ctx, &ScTool::cursor_get, nullptr, "cursor");
            dukglue_register_method(ctx, &ScTool::cancel, "cancel");
        }

    private:
        std::string id_get() const
        {
            return ActiveCustomTool ? ActiveCustomTool->Id : "";
        }

        DukValue cursor_get() const
        {
            return ToDuk(_ctx, static_cast<CursorID>(gCurrentToolId));
        }

        void cancel()
        {
            ToolCancel();
        }
    };

    class ScUi
    {
    private:
        ScriptEngine& _scriptEngine;

    public:
        ScUi(ScriptEngine& scriptEngine)
            : _scriptEngine(scriptEngine)
        {
        }

    private:
        int32_t width_get() const
        {
            return ContextGetWidth();
        }
        int32_t height_get() const
        {
            return ContextGetHeight();
        }
        int32_t windows_get() const
        {
            return static_cast<int32_t>(g_window_list.size());
        }

        std::shared_ptr<ScViewport> mainViewport_get() const
        {
            return std::make_shared<ScViewport>(WindowClass::MainWindow);
        }

        std::shared_ptr<ScTileSelection> tileSelection_get() const
        {
            return std::make_shared<ScTileSelection>(_scriptEngine.GetContext());
        }

        std::shared_ptr<ScTool> tool_get() const
        {
            if (gInputFlags.has(InputFlag::toolActive))
            {
                return std::make_shared<ScTool>(_scriptEngine.GetContext());
            }
            return {};
        }

        std::shared_ptr<ScImageManager> imageManager_get() const
        {
            return std::make_shared<ScImageManager>(_scriptEngine.GetContext());
        }

        std::shared_ptr<ScWindow> openWindow(DukValue desc)
        {
            using namespace OpenRCT2::Ui::Windows;

            auto& execInfo = _scriptEngine.GetExecInfo();
            auto owner = execInfo.GetCurrentPlugin();

            owner->ThrowIfStopping();

            std::shared_ptr<ScWindow> scWindow = nullptr;
            auto w = WindowCustomOpen(owner, desc);
            if (w != nullptr)
            {
                scWindow = std::make_shared<ScWindow>(w);
            }
            return scWindow;
        }

        void closeWindows(std::string classification, DukValue id)
        {
            auto* windowMgr = Ui::GetWindowManager();
            auto cls = GetClassification(classification);
            if (cls != WindowClass::Null)
            {
                if (id.type() == DukValue::Type::NUMBER)
                {
                    windowMgr->CloseByNumber(cls, id.as_uint());
                }
                else
                {
                    windowMgr->CloseByClass(cls);
                }
            }
        }

        void closeAllWindows()
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();
        }

        std::shared_ptr<ScWindow> getWindow(DukValue a) const
        {
            if (a.type() == DukValue::Type::NUMBER)
            {
                auto index = a.as_uint();
                size_t i = 0;
                for (const auto& w : g_window_list)
                {
                    if (i == index)
                    {
                        return std::make_shared<ScWindow>(w.get());
                    }
                    i++;
                }
            }
            else if (a.type() == DukValue::Type::STRING)
            {
                const auto& classification = a.as_string();
                auto w = FindCustomWindowByClassification(classification);
                if (w != nullptr)
                {
                    return std::make_shared<ScWindow>(w);
                }
            }
            return {};
        }

        void showError(const std::string& title, const std::string& message)
        {
            ErrorOpen(title, message);
        }

        void showTextInput(const DukValue& desc)
        {
            try
            {
                constexpr int32_t kMaxLengthAllowed = 4096;
                auto plugin = _scriptEngine.GetExecInfo().GetCurrentPlugin();
                auto title = desc["title"].as_string();
                auto description = desc["description"].as_string();
                auto initialValue = AsOrDefault(desc["initialValue"], "");
                auto maxLength = AsOrDefault(desc["maxLength"], kMaxLengthAllowed);
                auto callback = desc["callback"];
                WindowTextInputOpen(
                    title, description, initialValue, std::clamp(maxLength, 0, kMaxLengthAllowed),
                    [this, plugin, callback](std::string_view value) {
                        auto dukValue = ToDuk(_scriptEngine.GetContext(), value);
                        _scriptEngine.ExecutePluginCall(plugin, callback, { dukValue }, false);
                    },
                    {});
            }
            catch (const DukException&)
            {
                duk_error(_scriptEngine.GetContext(), DUK_ERR_ERROR, "Invalid parameters.");
            }
        }

        void showFileBrowse(const DukValue& desc)
        {
            try
            {
                auto plugin = _scriptEngine.GetExecInfo().GetCurrentPlugin();
                auto type = desc["type"].as_string();
                auto fileType = desc["fileType"].as_string();
                auto defaultPath = AsOrDefault(desc["defaultPath"], "");
                auto callback = desc["callback"];

                auto loadSaveAction = LoadSaveAction::load;
                if (type == "load")
                    loadSaveAction = LoadSaveAction::load;
                else
                    throw DukException();

                LoadSaveType loadSaveType;
                if (fileType == "game")
                    loadSaveType = LoadSaveType::park;
                else if (fileType == "heightmap")
                    loadSaveType = LoadSaveType::heightmap;
                else
                    throw DukException();

                LoadsaveOpen(
                    loadSaveAction, loadSaveType, defaultPath,
                    [this, plugin, callback](ModalResult result, std::string_view path) {
                        if (result == ModalResult::ok)
                        {
                            auto dukValue = ToDuk(_scriptEngine.GetContext(), path);
                            _scriptEngine.ExecutePluginCall(plugin, callback, { dukValue }, false);
                        }
                    },
                    nullptr);
            }
            catch (const DukException&)
            {
                duk_error(_scriptEngine.GetContext(), DUK_ERR_ERROR, "Invalid parameters.");
            }
        }

        void showScenarioSelect(const DukValue& desc)
        {
            auto plugin = _scriptEngine.GetExecInfo().GetCurrentPlugin();
            auto callback = desc["callback"];

            ScenarioselectOpen([this, plugin, callback](std::string_view path) {
                auto dukValue = GetScenarioFile(path);
                _scriptEngine.ExecutePluginCall(plugin, callback, { dukValue }, false);
            });
        }

        void activateTool(const DukValue& desc)
        {
            InitialiseCustomTool(_scriptEngine, desc);
        }

        void registerMenuItem(std::string text, DukValue callback)
        {
            auto& execInfo = _scriptEngine.GetExecInfo();
            auto owner = execInfo.GetCurrentPlugin();
            CustomMenuItems.emplace_back(owner, CustomToolbarMenuItemKind::Standard, text, callback);
            std::ranges::sort(CustomMenuItems, [](auto&& a, auto&& b) { return a.Text < b.Text; });
        }

        void registerToolboxMenuItem(const std::string& text, DukValue callback)
        {
            auto& execInfo = _scriptEngine.GetExecInfo();
            auto owner = execInfo.GetCurrentPlugin();
            if (owner->GetMetadata().Type == PluginType::Intransient)
            {
                CustomMenuItems.emplace_back(owner, CustomToolbarMenuItemKind::Toolbox, text, callback);
                std::ranges::sort(CustomMenuItems, [](auto&& a, auto&& b) { return a.Text < b.Text; });
            }
            else
            {
                duk_error(_scriptEngine.GetContext(), DUK_ERR_ERROR, "Plugin must be intransient.");
            }
        }

        void registerShortcut(DukValue desc)
        {
            try
            {
                auto& execInfo = _scriptEngine.GetExecInfo();
                auto owner = execInfo.GetCurrentPlugin();
                auto id = desc["id"].as_string();
                auto text = desc["text"].as_string();

                std::vector<std::string> bindings;
                auto dukBindings = desc["bindings"];
                if (dukBindings.is_array())
                {
                    for (auto binding : dukBindings.as_array())
                    {
                        bindings.push_back(binding.as_string());
                    }
                }

                auto callback = desc["callback"];
                CustomShortcuts.emplace_back(std::make_unique<CustomShortcut>(owner, id, text, bindings, callback));
            }
            catch (const DukException&)
            {
                duk_error(_scriptEngine.GetContext(), DUK_ERR_ERROR, "Invalid parameters.");
            }
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScUi::height_get, nullptr, "height");
            dukglue_register_property(ctx, &ScUi::width_get, nullptr, "width");
            dukglue_register_property(ctx, &ScUi::windows_get, nullptr, "windows");
            dukglue_register_property(ctx, &ScUi::mainViewport_get, nullptr, "mainViewport");
            dukglue_register_property(ctx, &ScUi::tileSelection_get, nullptr, "tileSelection");
            dukglue_register_property(ctx, &ScUi::tool_get, nullptr, "tool");
            dukglue_register_property(ctx, &ScUi::imageManager_get, nullptr, "imageManager");
            dukglue_register_method(ctx, &ScUi::openWindow, "openWindow");
            dukglue_register_method(ctx, &ScUi::closeWindows, "closeWindows");
            dukglue_register_method(ctx, &ScUi::closeAllWindows, "closeAllWindows");
            dukglue_register_method(ctx, &ScUi::getWindow, "getWindow");
            dukglue_register_method(ctx, &ScUi::showError, "showError");
            dukglue_register_method(ctx, &ScUi::showTextInput, "showTextInput");
            dukglue_register_method(ctx, &ScUi::showFileBrowse, "showFileBrowse");
            dukglue_register_method(ctx, &ScUi::showScenarioSelect, "showScenarioSelect");
            dukglue_register_method(ctx, &ScUi::activateTool, "activateTool");
            dukglue_register_method(ctx, &ScUi::registerMenuItem, "registerMenuItem");
            dukglue_register_method(ctx, &ScUi::registerToolboxMenuItem, "registerToolboxMenuItem");
            dukglue_register_method(ctx, &ScUi::registerShortcut, "registerShortcut");
        }

    private:
        WindowClass GetClassification(const std::string& key) const
        {
            return WindowClass::Null;
        }

        DukValue GetScenarioFile(std::string_view path)
        {
            auto ctx = _scriptEngine.GetContext();
            DukObject obj(ctx);
            obj.Set("path", path);

            auto* scenarioRepo = GetScenarioRepository();
            auto entry = scenarioRepo->GetByPath(std::string(path).c_str());
            if (entry != nullptr)
            {
                obj.Set("id", entry->ScenarioId);
                obj.Set("category", ToDuk(ctx, entry->Category));
                obj.Set("sourceGame", ToDuk(ctx, entry->SourceGame));
                obj.Set("internalName", entry->InternalName);
                obj.Set("name", entry->Name);
                obj.Set("details", entry->Details);

                auto* highscore = entry->Highscore;
                if (highscore == nullptr)
                {
                    obj.Set("highscore", nullptr);
                }
                else
                {
                    DukObject dukHighscore(ctx);
                    dukHighscore.Set("name", highscore->name);
                    dukHighscore.Set("companyValue", highscore->company_value);
                    obj.Set("highscore", dukHighscore.Take());
                }
            }
            return obj.Take();
        }
    };
} // namespace OpenRCT2::Scripting

#endif
