/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "CustomMenu.h"

    #include "../interface/Viewport.h"

    #include <openrct2-ui/UiContext.h>
    #include <openrct2-ui/input/ShortcutManager.h>
    #include <openrct2/Input.h>
    #include <openrct2/scripting/ScriptUtil.hpp>
    #include <openrct2/ui/UiContext.h>
    #include <openrct2/ui/WindowManager.h>

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
            registeredShortcut.Default.emplace_back(binding);
        }
        registeredShortcut.Current = registeredShortcut.Default;
        shortcutManager.RegisterShortcut(std::move(registeredShortcut));
        shortcutManager.LoadUserBindings();
    }

    CustomShortcut::~CustomShortcut()
    {
        auto& shortcutManager = GetShortcutManager();
        shortcutManager.RemoveShortcut(Id);
    }

    void CustomShortcut::Invoke() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine.ExecutePluginCall(Owner, Callback.callback, {}, false);
    }

    static constexpr std::array<std::string, EnumValue(CursorID::Count)> CursorNames = {
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
            return JS_NewString(ctx, CursorNames[idVal].c_str());
        }
        return JS_UNDEFINED;
    }

    static CursorID CursorJSValToID(JSContext* ctx, JSValue value)
    {
        if (JS_IsString(value))
        {
            const char* valueStr = JS_ToCString(ctx, value);
            std::string_view valueStrView(valueStr);
            for (uint8_t i = 0; i < EnumValue(CursorID::Count); i++)
            {
                if (CursorNames[i] == valueStrView)
                {
                    JS_FreeCString(ctx, valueStr);
                    return static_cast<CursorID>(i);
                }
            }
            JS_FreeCString(ctx, valueStr);
        }
        return CursorID::Arrow;
    }

    // TODO (mber) consider making this a normal EnumMap
    static constexpr std::array ToolFilterMap = std::to_array({
        std::make_tuple(std::string_view("terrain"), ViewportInteractionItem::Terrain),
        std::make_tuple(std::string_view("entity"), ViewportInteractionItem::Entity),
        std::make_tuple(std::string_view("ride"), ViewportInteractionItem::Ride),
        std::make_tuple(std::string_view("water"), ViewportInteractionItem::Water),
        std::make_tuple(std::string_view("scenery"), ViewportInteractionItem::Scenery),
        std::make_tuple(std::string_view("footpath"), ViewportInteractionItem::Footpath),
        std::make_tuple(std::string_view("footpath_item"), ViewportInteractionItem::PathAddition),
        std::make_tuple(std::string_view("park_entrance"), ViewportInteractionItem::ParkEntrance),
        std::make_tuple(std::string_view("wall"), ViewportInteractionItem::Wall),
        std::make_tuple(std::string_view("large_scenery"), ViewportInteractionItem::LargeScenery),
        std::make_tuple(std::string_view("label"), ViewportInteractionItem::Label),
        std::make_tuple(std::string_view("banner"), ViewportInteractionItem::Banner),
    });

    static ViewportInteractionItem FilterJSValToEnum(JSContext* ctx, JSValue value)
    {
        if (JS_IsString(value))
        {
            const char* valueStr = JS_ToCString(ctx, value);
            std::string_view valueStrView(valueStr);
            for (uint8_t i = 0; i < ToolFilterMap.size(); i++)
            {
                if (std::get<0>(ToolFilterMap[i]) == valueStrView)
                {
                    JS_FreeCString(ctx, valueStr);
                    return std::get<1>(ToolFilterMap[i]);
                }
            }
            JS_FreeCString(ctx, valueStr);
        }
        return ViewportInteractionItem::None;
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

            if (info.interactionType == ViewportInteractionItem::Entity && info.Entity != nullptr)
            {
                JS_SetPropertyStr(ctx, obj, "entityId", JS_NewInt32(ctx, info.Entity->Id.ToUnderlying()));
            }
            else if (info.Element != nullptr)
            {
                int32_t index = 0;
                auto el = MapGetFirstElementAt(info.Loc);
                if (el != nullptr)
                {
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

            const char* id = JS_ToCString(ctx, idVal);
            customTool.Id = std::string(id);
            JS_FreeCString(ctx, id);
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

            customTool.onStart = GetCallbackProperty(ctx, value, "onStart");
            customTool.onDown = GetCallbackProperty(ctx, value, "onDown");
            customTool.onMove = GetCallbackProperty(ctx, value, "onMove");
            customTool.onUp = GetCallbackProperty(ctx, value, "onUp");
            customTool.onFinish = GetCallbackProperty(ctx, value, "onFinish");

            auto* windowMgr = GetWindowManager();
            auto toolbarWindow = windowMgr->FindByClass(WindowClass::TopToolbar);
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
