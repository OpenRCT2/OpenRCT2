/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../interface/Window.h"
    #include "../windows/Windows.h"
    #include "CustomMenu.h"
    #include "CustomWindow.h"
    #include "ScImageManager.hpp"
    #include "ScTileSelection.hpp"
    #include "ScViewport.hpp"
    #include "ScWindow.h"

    #include <algorithm>
    #include <memory>
    #include <openrct2/Context.h>
    #include <openrct2/Input.h>
    #include <openrct2/scenario/ScenarioCategory.h>
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

    static const EnumMap<Scenario::Category> ScenarioCategoryMap{ {
        { "beginner", Scenario::Category::beginner },
        { "challenging", Scenario::Category::challenging },
        { "expert", Scenario::Category::expert },
        { "real", Scenario::Category::real },
        { "other", Scenario::Category::other },
        { "dlc", Scenario::Category::dlc },
        { "build_your_own", Scenario::Category::buildYourOwn },
        { "competitions", Scenario::Category::competitions },
    } };

    static const EnumMap<ScenarioSource> ScenarioSourceMap{
        { "rct1", ScenarioSource::RCT1 }, { "rct1_aa", ScenarioSource::RCT1_AA }, { "rct1_ll", ScenarioSource::RCT1_LL },
        { "rct2", ScenarioSource::RCT2 }, { "rct2_ww", ScenarioSource::RCT2_WW }, { "rct2_tt", ScenarioSource::RCT2_TT },
        { "real", ScenarioSource::Real }, { "extras", ScenarioSource::Extras },   { "other", ScenarioSource::Other },
    };

    inline JSValue ScenarioCategoryToJS(JSContext* ctx, Scenario::Category value)
    {
        const auto& entry = ScenarioCategoryMap.find(value);
        if (entry != ScenarioCategoryMap.end())
            return JSFromStdString(ctx, entry->first);
        return JSFromStdString(ctx, ScenarioCategoryMap[Scenario::Category::other]);
    }

    inline JSValue ScenarioSourceToJS(JSContext* ctx, ScenarioSource value)
    {
        const auto& entry = ScenarioSourceMap.find(value);
        if (entry != ScenarioSourceMap.end())
            return JSFromStdString(ctx, entry->first);
        return JSFromStdString(ctx, ScenarioSourceMap[ScenarioSource::Other]);
    }

    class ScTool final : public ScBase
    {
    private:
        static JSValue id_get(JSContext* ctx, JSValue thisVal)
        {
            return JSFromStdString(ctx, ActiveCustomTool ? ActiveCustomTool->Id : "");
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
            return JS_NewInt32(ctx, static_cast<int32_t>(gWindowList.size()));
        }

        static JSValue mainViewport_get(JSContext* ctx, JSValue thisVal)
        {
            return gScViewport.New(ctx, WindowClass::mainWindow);
        }

        static JSValue tileSelection_get(JSContext* ctx, JSValue thisVal)
        {
            return gScTileSelection.New(ctx);
        }

        static JSValue tool_get(JSContext* ctx, JSValue thisVal)
        {
            if (gInputFlags.has(InputFlag::toolActive))
            {
                return gScTool.New(ctx);
            }
            return JS_NULL;
        }

        static JSValue imageManager_get(JSContext* ctx, JSValue thisVal)
        {
            return gScImageManager.New(ctx);
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

        static JSValue closeWindows(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(classification, ctx, argv[0]);
            JSValue id = argv[1];

            auto* windowMgr = Ui::GetWindowManager();
            auto cls = GetClassification(classification);
            if (cls != WindowClass::null)
            {
                if (JS_IsNumber(id))
                {
                    windowMgr->CloseByNumber(cls, static_cast<WindowNumber>(JSToInt(ctx, id)));
                }
                else
                {
                    windowMgr->CloseByClass(cls);
                }
            }
            return JS_UNDEFINED;
        }

        static JSValue closeAllWindows(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();
            return JS_UNDEFINED;
        }

        static JSValue getWindow(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            if (JS_IsNumber(argv[0]))
            {
                int32_t index = -1;
                JS_ToInt32(ctx, &index, argv[0]);
                int32_t i = 0;
                for (const auto& w : gWindowList)
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
                std::string classification = JSToStdString(ctx, argv[0]);
                auto w = Ui::Windows::FindCustomWindowByClassification(classification);
                if (w != nullptr)
                {
                    return gScWindow.New(ctx, w);
                }
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
            std::string title = JSToStdString(ctx, argv[0]);
            std::string message = JSToStdString(ctx, argv[1]);
            Ui::Windows::ErrorOpen(title, message);
            return JS_UNDEFINED;
        }

        static JSValue showTextInput(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JSValue desc = argv[0];
            if (!JS_IsObject(desc))
            {
                JS_ThrowInternalError(ctx, "Invalid parameters.");
                return JS_EXCEPTION;
            }

            ScriptEngine* scriptEngine = gScUi.GetOpaque<ScriptEngine*>(thisVal);
            if (!scriptEngine)
            {
                JS_ThrowInternalError(ctx, "No script engine");
                return JS_EXCEPTION;
            }
            constexpr int32_t kMaxLengthAllowed = 4096;
            auto plugin = scriptEngine->GetExecInfo().GetCurrentPlugin();
            auto title = JSToStdString(ctx, desc, "title");
            auto description = JSToStdString(ctx, desc, "description");
            auto initialValue = AsOrDefault(ctx, desc, "initialValue", "");
            auto maxLength = AsOrDefault(ctx, desc, "maxLength", kMaxLengthAllowed);
            auto callback = JSToCallback(ctx, desc, "callback");
            Ui::Windows::WindowTextInputOpen(
                title, description, initialValue, std::clamp(maxLength, 0, kMaxLengthAllowed),
                [scriptEngine, ctx, plugin, callback](std::string_view value) {
                    scriptEngine->ExecutePluginCall(plugin, callback.callback, { JSFromStdString(ctx, value) }, false);
                },
                {});
            return JS_UNDEFINED;
        }

        static JSValue showFileBrowse(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JSValue desc = argv[0];
            if (!JS_IsObject(desc))
            {
                JS_ThrowInternalError(ctx, "Invalid parameters.");
                return JS_EXCEPTION;
            }

            ScriptEngine* scriptEngine = gScUi.GetOpaque<ScriptEngine*>(thisVal);
            if (!scriptEngine)
            {
                JS_ThrowInternalError(ctx, "No script engine");
                return JS_EXCEPTION;
            }
            auto plugin = scriptEngine->GetExecInfo().GetCurrentPlugin();
            auto type = JSToStdString(ctx, desc, "type");
            auto fileType = JSToStdString(ctx, desc, "fileType");
            auto defaultPath = AsOrDefault(ctx, desc, "defaultPath", "");
            auto callback = JSToCallback(ctx, desc, "callback");

            auto loadSaveAction = LoadSaveAction::load;
            if (type == "load")
                loadSaveAction = LoadSaveAction::load;
            else
            {
                JS_ThrowPlainError(ctx, "Invalid type parameter");
                return JS_EXCEPTION;
            }

            LoadSaveType loadSaveType;
            if (fileType == "game")
                loadSaveType = LoadSaveType::park;
            else if (fileType == "heightmap")
                loadSaveType = LoadSaveType::heightmap;
            else
            {
                JS_ThrowPlainError(ctx, "Invalid fileType parameter");
                return JS_EXCEPTION;
            }

            Ui::Windows::LoadsaveOpen(
                loadSaveAction, loadSaveType, defaultPath,
                [scriptEngine, ctx, plugin, callback](ModalResult result, std::string_view path) {
                    if (result == ModalResult::ok)
                    {
                        scriptEngine->ExecutePluginCall(plugin, callback.callback, { JSFromStdString(ctx, path) }, false);
                    }
                },
                true, nullptr);
            return JS_UNDEFINED;
        }

        static JSValue showScenarioSelect(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JSValue desc = argv[0];
            if (!JS_IsObject(desc))
            {
                JS_ThrowInternalError(ctx, "Invalid parameters.");
                return JS_EXCEPTION;
            }

            ScriptEngine* scriptEngine = gScUi.GetOpaque<ScriptEngine*>(thisVal);
            if (!scriptEngine)
            {
                JS_ThrowInternalError(ctx, "No script engine");
                return JS_EXCEPTION;
            }
            auto plugin = scriptEngine->GetExecInfo().GetCurrentPlugin();
            auto callback = JSToCallback(ctx, desc, "callback");

            Ui::Windows::ScenarioselectOpen([scriptEngine, ctx, plugin, callback](std::string_view path) {
                scriptEngine->ExecutePluginCall(plugin, callback.callback, { GetScenarioFile(ctx, path) }, false);
            });
            return JS_UNDEFINED;
        }

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
            std::string text = JSToStdString(ctx, argv[0]);
            assert(owner->GetContext() == ctx);
            CustomMenuItems.emplace_back(owner, CustomToolbarMenuItemKind::Standard, text, JSCallback(ctx, argv[1]));
            std::ranges::sort(CustomMenuItems, [](auto&& a, auto&& b) { return a.Text < b.Text; });

            return JS_UNDEFINED;
        }

        static JSValue registerToolboxMenuItem(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
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
            if (owner->GetMetadata().Type == PluginType::Intransient)
            {
                CustomMenuItems.emplace_back(
                    owner, CustomToolbarMenuItemKind::Toolbox, JSToStdString(ctx, argv[0]), JSCallback(ctx, argv[1]));
                std::ranges::sort(CustomMenuItems, [](auto&& a, auto&& b) { return a.Text < b.Text; });
            }
            else
            {
                JS_ThrowPlainError(ctx, "Plugin must be intransient.");
                return JS_EXCEPTION;
            }
            return JS_UNDEFINED;
        }

        static JSValue registerShortcut(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JSValue desc = argv[0];
            if (!JS_IsObject(desc))
            {
                JS_ThrowInternalError(ctx, "Invalid parameters.");
                return JS_EXCEPTION;
            }

            ScriptEngine* scriptEngine = gScUi.GetOpaque<ScriptEngine*>(thisVal);
            if (!scriptEngine)
            {
                JS_ThrowInternalError(ctx, "No script engine");
                return JS_EXCEPTION;
            }

            auto owner = scriptEngine->GetExecInfo().GetCurrentPlugin();
            auto id = JSToStdString(ctx, desc, "id");
            auto text = JSToStdString(ctx, desc, "text");

            std::vector<std::string> bindings;
            JSIterateArray(
                ctx, desc, "bindings", [&bindings](JSContext* ctx2, JSValue x) { bindings.push_back(JSToStdString(ctx2, x)); });

            auto callback = JSToCallback(ctx, desc, "callback");
            CustomShortcuts.emplace_back(std::make_unique<CustomShortcut>(owner, id, text, bindings, callback));
            return JS_UNDEFINED;
        }

        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("height", ScUi::height_get, nullptr),
            JS_CGETSET_DEF("width", ScUi::width_get, nullptr),
            JS_CGETSET_DEF("windows", ScUi::windows_get, nullptr),
            JS_CGETSET_DEF("mainViewport", ScUi::mainViewport_get, nullptr),
            JS_CGETSET_DEF("tileSelection", ScUi::tileSelection_get, nullptr),
            JS_CGETSET_DEF("tool", ScUi::tool_get, nullptr),
            JS_CGETSET_DEF("imageManager", ScUi::imageManager_get, nullptr),

            JS_CFUNC_DEF("openWindow", 1, ScUi::openWindow),
            JS_CFUNC_DEF("closeWindows", 2, ScUi::closeWindows),
            JS_CFUNC_DEF("closeAllWindows", 0, ScUi::closeAllWindows),
            JS_CFUNC_DEF("getWindow", 1, ScUi::getWindow),
            JS_CFUNC_DEF("showError", 2, ScUi::showError),
            JS_CFUNC_DEF("showTextInput", 1, ScUi::showTextInput),
            JS_CFUNC_DEF("showFileBrowse", 1, ScUi::showFileBrowse),
            JS_CFUNC_DEF("showScenarioSelect", 1, ScUi::showScenarioSelect),
            JS_CFUNC_DEF("activateTool", 1, ScUi::activateTool),
            JS_CFUNC_DEF("registerMenuItem", 2, ScUi::registerMenuItem),
            JS_CFUNC_DEF("registerToolboxMenuItem", 2, ScUi::registerToolboxMenuItem),
            JS_CFUNC_DEF("registerShortcut", 1, ScUi::registerShortcut),
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
        static WindowClass GetClassification(const std::string& key)
        {
            return WindowClass::null;
        }

        static JSValue GetScenarioFile(JSContext* ctx, std::string_view path)
        {
            JSValue obj = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, obj, "path", JSFromStdString(ctx, path));

            auto* scenarioRepo = GetScenarioRepository();
            auto entry = scenarioRepo->GetByPath(std::string(path).c_str());
            if (entry != nullptr)
            {
                JS_SetPropertyStr(ctx, obj, "id", JS_NewInt32(ctx, entry->ScenarioId));
                JS_SetPropertyStr(ctx, obj, "category", ScenarioCategoryToJS(ctx, entry->Category));
                JS_SetPropertyStr(ctx, obj, "sourceGame", ScenarioSourceToJS(ctx, entry->SourceGame));
                JS_SetPropertyStr(ctx, obj, "internalName", JSFromStdString(ctx, entry->InternalName));
                JS_SetPropertyStr(ctx, obj, "name", JSFromStdString(ctx, entry->Name));
                JS_SetPropertyStr(ctx, obj, "details", JSFromStdString(ctx, entry->Details));

                auto* highscore = entry->Highscore;
                if (highscore == nullptr)
                {
                    JS_SetPropertyStr(ctx, obj, "highscore", JS_NULL);
                }
                else
                {
                    JSValue jsHighscore = JS_NewObject(ctx);
                    JS_SetPropertyStr(ctx, jsHighscore, "name", JSFromStdString(ctx, highscore->name));
                    JS_SetPropertyStr(ctx, jsHighscore, "companyValue", JS_NewInt64(ctx, highscore->company_value));
                    JS_SetPropertyStr(ctx, obj, "highscore", jsHighscore);
                }
            }
            return obj;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
