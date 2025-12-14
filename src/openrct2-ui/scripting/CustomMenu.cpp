/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "CustomMenu.h"

    #include "../interface/Viewport.h"

    #include <openrct2-ui/UiContext.h>
    #include <openrct2-ui/input/ShortcutManager.h>
    #include <openrct2/Input.h>
    #include <openrct2/core/EnumMap.hpp>
    #include <openrct2/scripting/ScriptUtil.hpp>
    #include <openrct2/ui/UiContext.h>
    #include <openrct2/ui/WindowManager.h>
    #include <openrct2/world/Map.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

namespace OpenRCT2::Scripting
{
    std::optional<CustomTool> ActiveCustomTool;
    std::vector<CustomToolbarMenuItem> CustomMenuItems;
    std::vector<std::unique_ptr<CustomShortcut>> CustomShortcuts;

    CustomShortcut::CustomShortcut(
        std::shared_ptr<Plugin> owner, std::string_view id, std::string_view text, const std::vector<std::string>& bindings,
        JSCallback callback)
        : Owner(owner)
        , Id(id)
        , Text(text)
        , Bindings(bindings)
        , Callback(callback)
    {
        auto& shortcutManager = GetShortcutManager();

        RegisteredShortcut registeredShortcut(Id, Text, [this]() { Invoke(); });
        for (const auto& binding : bindings)
        {
            registeredShortcut.standard.emplace_back(binding);
        }
        registeredShortcut.current = registeredShortcut.standard;
        shortcutManager.registerShortcut(std::move(registeredShortcut));
        shortcutManager.loadUserBindings();
    }

    CustomShortcut::~CustomShortcut()
    {
        auto& shortcutManager = GetShortcutManager();
        shortcutManager.removeShortcut(Id);
    }

    void CustomShortcut::Invoke() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine.ExecutePluginCall(Owner, Callback.callback, {}, false);
    }

    static constexpr std::array<std::string_view, EnumValue(CursorID::Count)> CursorNames = {
        "arrow",         "blank",      "up_arrow",      "up_down_arrow", "hand_point", "zzz",         "diagonal_arrows",
        "picker",        "tree_down",  "fountain_down", "statue_down",   "bench_down", "cross_hair",  "bin_down",
        "lamppost_down", "fence_down", "flower_down",   "path_down",     "dig_down",   "water_down",  "house_down",
        "volcano_down",  "walk_down",  "paint_down",    "entrance_down", "hand_open",  "hand_closed",
    };

    JSValue CursorIDToJSValue(JSContext* ctx, CursorID id)
    {
        auto idVal = EnumValue(id);
        if (idVal < CursorNames.size())
        {
            return JSFromStdString(ctx, CursorNames[idVal]);
        }
        return JS_UNDEFINED;
    }

    static CursorID CursorJSValToID(JSContext* ctx, JSValue value)
    {
        if (JS_IsString(value))
        {
            std::string valueStr = JSToStdString(ctx, value);
            for (uint8_t i = 0; i < EnumValue(CursorID::Count); i++)
            {
                if (CursorNames[i] == valueStr)
                {
                    return static_cast<CursorID>(i);
                }
            }
        }
        return CursorID::Arrow;
    }

    static const EnumMap<ViewportInteractionItem> ToolFilterMap{
        { "terrain", ViewportInteractionItem::terrain },
        { "entity", ViewportInteractionItem::entity },
        { "ride", ViewportInteractionItem::ride },
        { "water", ViewportInteractionItem::water },
        { "scenery", ViewportInteractionItem::scenery },
        { "footpath", ViewportInteractionItem::footpath },
        { "footpath_item", ViewportInteractionItem::pathAddition },
        { "park_entrance", ViewportInteractionItem::parkEntrance },
        { "wall", ViewportInteractionItem::wall },
        { "large_scenery", ViewportInteractionItem::largeScenery },
        { "label", ViewportInteractionItem::label },
        { "banner", ViewportInteractionItem::banner },
    };

    static ViewportInteractionItem FilterJSValToEnum(JSContext* ctx, JSValue value)
    {
        if (JS_IsString(value))
        {
            if (auto val = ToolFilterMap.TryGet(JSToStdString(ctx, value)); val.has_value())
            {
                return val.value();
            }
        }
        return ViewportInteractionItem::none;
    }

    static void RemoveMenuItemsAndTool(std::shared_ptr<Plugin> owner)
    {
        if (ActiveCustomTool)
        {
            if (ActiveCustomTool->Owner == owner)
            {
                ToolCancel();
            }
        }

        auto& menuItems = CustomMenuItems;
        for (auto it = menuItems.begin(); it != menuItems.end();)
        {
            if (it->Owner == owner)
            {
                it = menuItems.erase(it);
            }
            else
            {
                it++;
            }
        }

        auto& shortcuts = CustomShortcuts;
        for (auto it = shortcuts.begin(); it != shortcuts.end();)
        {
            if ((*it)->Owner == owner)
            {
                it = shortcuts.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    void InitialiseCustomMenuItems(ScriptEngine& scriptEngine)
    {
        scriptEngine.SubscribeToPluginStoppedEvent(
            [](std::shared_ptr<Plugin> plugin) -> void { RemoveMenuItemsAndTool(plugin); });
    }

    void CustomTool::OnUpdate(const ScreenCoordsXY& screenCoords)
    {
        InvokeEventHandler(onMove.callback, screenCoords);
    }

    void CustomTool::OnDown(const ScreenCoordsXY& screenCoords)
    {
        MouseDown = true;
        InvokeEventHandler(onDown.callback, screenCoords);
    }

    void CustomTool::OnDrag(const ScreenCoordsXY& screenCoords)
    {
    }

    void CustomTool::Start()
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine.ExecutePluginCall(Owner, onStart.callback, {}, false);
    }

    void CustomTool::OnUp(const ScreenCoordsXY& screenCoords)
    {
        MouseDown = false;
        InvokeEventHandler(onUp.callback, screenCoords);
    }

    void CustomTool::OnAbort()
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine.ExecutePluginCall(Owner, onFinish.callback, {}, false);
    }

    void CustomTool::InvokeEventHandler(JSValue handler, const ScreenCoordsXY& screenCoords)
    {
        JSContext* ctx = Owner->GetContext();
        if (JS_IsFunction(ctx, handler))
        {
            auto info = GetMapCoordinatesFromPos(screenCoords, Filter);

            JSValue obj = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, obj, "isDown", JS_NewBool(ctx, MouseDown));
            JS_SetPropertyStr(ctx, obj, "screenCoords", ToJSValue(ctx, screenCoords));
            JS_SetPropertyStr(ctx, obj, "mapCoords", ToJSValue(ctx, info.Loc));

            if (info.interactionType == ViewportInteractionItem::entity && info.Entity != nullptr)
            {
                JS_SetPropertyStr(ctx, obj, "entityId", JS_NewInt32(ctx, info.Entity->Id.ToUnderlying()));
            }
            else if (info.Element != nullptr)
            {
                auto el = MapGetFirstElementAt(info.Loc);
                if (el != nullptr)
                {
                    int32_t index = 0;
                    do
                    {
                        if (el == info.Element)
                        {
                            JS_SetPropertyStr(ctx, obj, "tileElementIndex", JS_NewInt32(ctx, index));
                            break;
                        }
                        index++;
                    } while (!(el++)->IsLastForTile());
                }
            }

            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.ExecutePluginCall(Owner, handler, { obj }, false);
        }
    }

    [[nodiscard]] JSValue InitialiseCustomTool(ScriptEngine& scriptEngine, JSContext* ctx, JSValue value)
    {
        std::shared_ptr<Plugin> currentPlugin = scriptEngine.GetExecInfo().GetCurrentPlugin();

        if (JS_IsObject(value))
        {
            JSValue idVal = JS_GetPropertyStr(ctx, value, "id");
            if (!JS_IsString(idVal))
            {
                JS_FreeValue(ctx, idVal);
                JS_ThrowTypeError(ctx, "Invalid id. Expected string");
                return JS_EXCEPTION;
            }

            CustomTool customTool;
            customTool.Owner = currentPlugin;

            customTool.Id = JSToStdString(ctx, idVal);
            JS_FreeValue(ctx, idVal);

            JSValue cursorVal = JS_GetPropertyStr(ctx, value, "cursor");
            customTool.Cursor = CursorJSValToID(ctx, cursorVal);
            JS_FreeValue(ctx, cursorVal);

            JSValue filter = JS_GetPropertyStr(ctx, value, "filter");
            if (JS_IsArray(filter))
            {
                customTool.Filter = 0;
                int64_t len = -1;
                JS_GetLength(ctx, filter, &len);
                for (int64_t i = 0; i < len; i++)
                {
                    JSValue curFilter = JS_GetPropertyInt64(ctx, filter, i);
                    auto elem = FilterJSValToEnum(ctx, curFilter);
                    customTool.Filter |= static_cast<uint32_t>(EnumToFlag(elem));
                    JS_FreeValue(ctx, curFilter);
                }
            }
            else
            {
                customTool.Filter = kViewportInteractionItemAll;
            }
            JS_FreeValue(ctx, filter);

            customTool.onStart = JSToCallback(ctx, value, "onStart");
            customTool.onDown = JSToCallback(ctx, value, "onDown");
            customTool.onMove = JSToCallback(ctx, value, "onMove");
            customTool.onUp = JSToCallback(ctx, value, "onUp");
            customTool.onFinish = JSToCallback(ctx, value, "onFinish");

            auto* windowMgr = GetWindowManager();
            auto toolbarWindow = windowMgr->FindByClass(WindowClass::topToolbar);
            if (toolbarWindow != nullptr)
            {
                // Use a widget that does not exist on top toolbar but also make sure it isn't
                // kWidgetIndexNull, as that prevents abort from being called.
                // TODO: refactor this to not leech on the top toolbar.
                WidgetIndex widgetIndex = 0xFFFE;
                ToolCancel();
                ToolSet(*toolbarWindow, widgetIndex, static_cast<Tool>(customTool.Cursor));
                ActiveCustomTool = std::move(customTool);
                ActiveCustomTool->Start();
            }
        }
        return JS_UNDEFINED;
    }
} // namespace OpenRCT2::Scripting

#endif
