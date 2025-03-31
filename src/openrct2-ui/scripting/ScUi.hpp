/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../windows/Windows.h"
    #include "CustomMenu.h"
    #include "CustomWindow.h"
    #include "ScImageManager.hpp"
    #include "ScTileSelection.hpp"
    #include "ScViewport.hpp"
    #include "ScWindow.hpp"

    #include <memory>
    #include <openrct2/Context.h>
    #include <openrct2/Input.h>
    #include <openrct2/interface/Window_internal.h>
    #include <openrct2/scenario/Scenario.h>
    #include <openrct2/scenario/ScenarioRepository.h>
    #include <openrct2/scripting/ScriptEngine.h>
    #include <string>

namespace OpenRCT2::Scripting
{
    class Plugin;
    class ScUi;
    extern ScUi gScUi;
    class ScTool;
    extern ScTool gScTool;

    // TODO (mber) consider EnumMaps
    static constexpr std::array ScenarioCategoryMap = std::to_array({
        std::make_tuple("beginner", SCENARIO_CATEGORY_BEGINNER),
        std::make_tuple("challenging", SCENARIO_CATEGORY_CHALLENGING),
        std::make_tuple("expert", SCENARIO_CATEGORY_EXPERT),
        std::make_tuple("real", SCENARIO_CATEGORY_REAL),
        std::make_tuple("other", SCENARIO_CATEGORY_OTHER),
        std::make_tuple("dlc", SCENARIO_CATEGORY_DLC),
        std::make_tuple("build_your_own", SCENARIO_CATEGORY_BUILD_YOUR_OWN),
        std::make_tuple("competitions", SCENARIO_CATEGORY_COMPETITIONS),
    });

    static constexpr std::array ScenarioSourceMap = std::to_array({
        std::make_tuple("rct1", ScenarioSource::RCT1),
        std::make_tuple("rct1_aa", ScenarioSource::RCT1_AA),
        std::make_tuple("rct1_ll", ScenarioSource::RCT1_LL),
        std::make_tuple("rct2", ScenarioSource::RCT2),
        std::make_tuple("rct2_ww", ScenarioSource::RCT2_WW),
        std::make_tuple("rct2_tt", ScenarioSource::RCT2_TT),
        std::make_tuple("real", ScenarioSource::Real),
        std::make_tuple("extras", ScenarioSource::Extras),
        std::make_tuple("other", ScenarioSource::Other),
    });

    /*
    template<>
    inline DukValue ToDuk(duk_context* ctx, const SCENARIO_CATEGORY& value)
    {
        const auto& entry = ScenarioCategoryMap.find(value);
        if (entry != ScenarioCategoryMap.end())
            return ToDuk(ctx, entry->first);
        return ToDuk(ctx, ScenarioCategoryMap[SCENARIO_CATEGORY_OTHER]);
    }

    template<>
    inline DukValue ToDuk(duk_context* ctx, const ScenarioSource& value)
    {
        const auto& entry = ScenarioSourceMap.find(value);
        if (entry != ScenarioSourceMap.end())
            return ToDuk(ctx, entry->first);
        return ToDuk(ctx, ScenarioSourceMap[ScenarioSource::Other]);
    }
    */
    class ScTool final : public ScBase
    {
    private:
        static JSValue id_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewString(ctx, ActiveCustomTool ? ActiveCustomTool->Id.c_str() : "");
        }

        static JSValue cursor_get(JSContext* ctx, JSValue thisVal)
        {
            return CursorIDToJSValue(ctx, static_cast<CursorID>(gCurrentToolId));
        }

        static JSValue cancel(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            ToolCancel();
            return JS_UNDEFINED;
        }

        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("id", ScTool::id_get, nullptr),
            JS_CGETSET_DEF("cursor", ScTool::cursor_get, nullptr),
            JS_CFUNC_DEF("cancel", 0, ScTool::cancel),
        };

    public:
        JSValue New(JSContext* ctx)
        {
            return MakeWithOpaque(ctx, funcs, nullptr);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Tool");
        }
    };

    class ScUi final : public ScBase
    {
    private:
        static JSValue width_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewInt32(ctx, ContextGetWidth());
        }
        static JSValue height_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewInt32(ctx, ContextGetHeight());
        }
        static JSValue windows_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewInt32(ctx, static_cast<int32_t>(g_window_list.size()));
        }

        static JSValue mainViewport_get(JSContext* ctx, JSValue thisVal)
        {
            // TODO (mber)
            return JS_NewString(ctx, "not yet implemented");

            // return std::make_shared<ScViewport>(WindowClass::MainWindow);
        }

        static JSValue tileSelection_get(JSContext* ctx, JSValue thisVal)
        {
            // TODO (mber)
            return JS_NewString(ctx, "not yet implemented");

            // return std::make_shared<ScTileSelection>(_scriptEngine.GetContext());
        }

        static JSValue tool_get(JSContext* ctx, JSValue thisVal)
        {
            if (InputTestFlag(INPUT_FLAG_TOOL_ACTIVE))
            {
                return gScTool.New(ctx);
            }
            return JS_NULL;
        }

        static JSValue imageManager_get(JSContext* ctx, JSValue thisVal)
        {
            // TODO (mber)
            return JS_NewString(ctx, "not yet implemented");

            // return std::make_shared<ScImageManager>(_scriptEngine.GetContext());
        }

        static JSValue openWindow(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            using namespace OpenRCT2::Ui::Windows;

            ScriptEngine* scriptEngine = gScUi.GetOpaque<ScriptEngine*>(thisVal);
            if (!scriptEngine)
            {
                JS_ThrowInternalError(ctx, "No script engine");
                return JS_EXCEPTION;
            }

            auto& execInfo = scriptEngine->GetExecInfo();
            auto owner = execInfo.GetCurrentPlugin();

            if (owner->IsStopping())
            {
                JS_ThrowInternalError(ctx, "Plugin is stopping.");
                return JS_EXCEPTION;
            }

            auto w = WindowCustomOpen(ctx, owner, argv[0]);
            if (w != nullptr)
            {
                return gScWindow.New(ctx, w);
            }
            return JS_NULL;
        }
        /*
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
        */
        static JSValue getWindow(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            if (JS_IsNumber(argv[0]))
            {
                int32_t index = -1;
                JS_ToInt32(ctx, &index, argv[0]);
                int32_t i = 0;
                for (const auto& w : g_window_list)
                {
                    if (i == index)
                    {
                        return gScWindow.New(ctx, w.get());
                    }
                    i++;
                }
            }
            else if (JS_IsString(argv[0]))
            {
                std::string classification = GetStdString(ctx, argv[0]);
                // TODO (mber)
                // auto w = FindCustomWindowByClassification(classification);
                // if (w != nullptr)
                // {
                //     return std::make_shared<ScWindow>(w);
                // }
            }
            return JS_NULL;
        }

        static JSValue showError(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            if (argc < 2 || !JS_IsString(argv[0]) || !JS_IsString(argv[1]))
            {
                JS_ThrowTypeError(ctx, "Invalid arguments");
                return JS_EXCEPTION;
            }
            const char* title = JS_ToCString(ctx, argv[0]);
            const char* message = JS_ToCString(ctx, argv[1]);
            Ui::Windows::ErrorOpen(title, message);
            JS_FreeCString(ctx, title);
            JS_FreeCString(ctx, message);
            return JS_UNDEFINED;
        }
        /*
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
        */
        static JSValue activateTool(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            ScriptEngine* scriptEngine = gScUi.GetOpaque<ScriptEngine*>(thisVal);
            if (!scriptEngine)
            {
                JS_ThrowInternalError(ctx, "No script engine");
                return JS_EXCEPTION;
            }
            return InitialiseCustomTool(*scriptEngine, ctx, argv[0]);
        }

        static JSValue registerMenuItem(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            ScriptEngine* scriptEngine = gScUi.GetOpaque<ScriptEngine*>(thisVal);
            if (!scriptEngine)
            {
                JS_ThrowInternalError(ctx, "No script engine");
                return JS_EXCEPTION;
            }
            if (argc < 2 || !JS_IsString(argv[0]) || !JS_IsFunction(ctx, argv[1]))
            {
                JS_ThrowTypeError(ctx, "Invalid arguments");
                return JS_EXCEPTION;
            }
            auto& execInfo = scriptEngine->GetExecInfo();
            auto owner = execInfo.GetCurrentPlugin();
            const char* text = JS_ToCString(ctx, argv[0]);
            assert(owner->GetContext() == ctx);
            CustomMenuItems.emplace_back(owner, CustomToolbarMenuItemKind::Standard, text, JSCallback(ctx, argv[1]));
            JS_FreeCString(ctx, text);

            return JS_UNDEFINED;
        }
        /*
        void registerToolboxMenuItem(const std::string& text, DukValue callback)
        {
            auto& execInfo = _scriptEngine.GetExecInfo();
            auto owner = execInfo.GetCurrentPlugin();
            if (owner->GetMetadata().Type == PluginType::Intransient)
            {
                CustomMenuItems.emplace_back(owner, CustomToolbarMenuItemKind::Toolbox, text, callback);
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
        */

        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("height", ScUi::height_get, nullptr), JS_CGETSET_DEF("width", ScUi::width_get, nullptr),
            JS_CGETSET_DEF("windows", ScUi::windows_get, nullptr),
            JS_CGETSET_DEF("mainViewport", ScUi::mainViewport_get, nullptr),
            JS_CGETSET_DEF("tileSelection", ScUi::tileSelection_get, nullptr), JS_CGETSET_DEF("tool", ScUi::tool_get, nullptr),
            JS_CGETSET_DEF("imageManager", ScUi::imageManager_get, nullptr),

            JS_CFUNC_DEF("openWindow", 1, ScUi::openWindow),
            // dukglue_register_method(ctx, &ScUi::closeWindows, "closeWindows"),
            // dukglue_register_method(ctx, &ScUi::closeAllWindows, "closeAllWindows"),
            JS_CFUNC_DEF("getWindow", 1, ScUi::getWindow), JS_CFUNC_DEF("showError", 2, ScUi::showError),
            // dukglue_register_method(ctx, &ScUi::showTextInput, "showTextInput"),
            // dukglue_register_method(ctx, &ScUi::showFileBrowse, "showFileBrowse"),
            // dukglue_register_method(ctx, &ScUi::showScenarioSelect, "showScenarioSelect"),
            JS_CFUNC_DEF("activateTool", 1, ScUi::activateTool), JS_CFUNC_DEF("registerMenuItem", 2, ScUi::registerMenuItem),
            // dukglue_register_method(ctx, &ScUi::registerToolboxMenuItem, "registerToolboxMenuItem"),
            // dukglue_register_method(ctx, &ScUi::registerShortcut, "registerShortcut"),
        };

    public:
        JSValue New(JSContext* ctx, ScriptEngine* scriptEngine)
        {
            return MakeWithOpaque(ctx, funcs, scriptEngine);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Ui");
        }

    private:
        WindowClass GetClassification(const std::string& key) const
        {
            return WindowClass::Null;
        }

        /*
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
            obj.Set("category", ToDuk(ctx, static_cast<SCENARIO_CATEGORY>(entry->Category)));
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
    */
    };
} // namespace OpenRCT2::Scripting

#endif
