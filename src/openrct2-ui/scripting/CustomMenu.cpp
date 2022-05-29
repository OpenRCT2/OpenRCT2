/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "CustomMenu.h"

#    include <openrct2-ui/input/ShortcutManager.h>
#    include <openrct2/Input.h>
#    include <openrct2/world/Map.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

namespace OpenRCT2::Scripting
{
    std::optional<CustomTool> ActiveCustomTool;
    std::vector<CustomToolbarMenuItem> CustomMenuItems;
    std::vector<std::unique_ptr<CustomShortcut>> CustomShortcuts;

    CustomShortcut::CustomShortcut(
        std::shared_ptr<Plugin> owner, std::string_view id, std::string_view text, const std::vector<std::string>& bindings,
        DukValue callback)
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
        auto* scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine->ExecutePluginCall(Owner, Callback, {}, false);
    }

    static constexpr std::array<std::string_view, EnumValue(CursorID::Count)> CursorNames = {
        "arrow",         "blank",      "up_arrow",      "up_down_arrow", "hand_point", "zzz",         "diagonal_arrows",
        "picker",        "tree_down",  "fountain_down", "statue_down",   "bench_down", "cross_hair",  "bin_down",
        "lamppost_down", "fence_down", "flower_down",   "path_down",     "dig_down",   "water_down",  "house_down",
        "volcano_down",  "walk_down",  "paint_down",    "entrance_down", "hand_open",  "hand_closed",
    };

    static const DukEnumMap<ViewportInteractionItem> ToolFilterMap({
        { "terrain", ViewportInteractionItem::Terrain },
        { "entity", ViewportInteractionItem::Entity },
        { "ride", ViewportInteractionItem::Ride },
        { "water", ViewportInteractionItem::Water },
        { "scenery", ViewportInteractionItem::Scenery },
        { "footpath", ViewportInteractionItem::Footpath },
        { "footpath_item", ViewportInteractionItem::FootpathItem },
        { "park_entrance", ViewportInteractionItem::ParkEntrance },
        { "wall", ViewportInteractionItem::Wall },
        { "large_scenery", ViewportInteractionItem::LargeScenery },
        { "label", ViewportInteractionItem::Label },
        { "banner", ViewportInteractionItem::Banner },
    });

    template<> DukValue ToDuk(duk_context* ctx, const CursorID& cursorId)
    {
        auto value = EnumValue(cursorId);
        if (value < std::size(CursorNames))
        {
            auto str = CursorNames[value];
            duk_push_lstring(ctx, str.data(), str.size());
            DukValue::take_from_stack(ctx);
        }
        return {};
    }

    template<> CursorID FromDuk(const DukValue& s)
    {
        if (s.type() == DukValue::Type::STRING)
        {
            auto it = std::find(std::begin(CursorNames), std::end(CursorNames), s.as_c_string());
            if (it != std::end(CursorNames))
            {
                return static_cast<CursorID>(std::distance(std::begin(CursorNames), it));
            }
        }
        return CursorID::Undefined;
    }

    static void RemoveMenuItemsAndTool(std::shared_ptr<Plugin> owner)
    {
        if (ActiveCustomTool)
        {
            if (ActiveCustomTool->Owner == owner)
            {
                tool_cancel();
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
        InvokeEventHandler(onMove, screenCoords);
    }

    void CustomTool::OnDown(const ScreenCoordsXY& screenCoords)
    {
        MouseDown = true;
        InvokeEventHandler(onDown, screenCoords);
    }

    void CustomTool::OnDrag(const ScreenCoordsXY& screenCoords)
    {
    }

    void CustomTool::Start()
    {
        auto* scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine->ExecutePluginCall(Owner, onStart, {}, false);
    }

    void CustomTool::OnUp(const ScreenCoordsXY& screenCoords)
    {
        MouseDown = false;
        InvokeEventHandler(onUp, screenCoords);
    }

    void CustomTool::OnAbort()
    {
        auto* scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine->ExecutePluginCall(Owner, onFinish, {}, false);
    }

    void CustomTool::InvokeEventHandler(const DukValue& dukHandler, const ScreenCoordsXY& screenCoords)
    {
        if (dukHandler.is_function())
        {
            auto ctx = dukHandler.context();
            auto info = get_map_coordinates_from_pos(screenCoords, Filter);

            DukObject obj(dukHandler.context());
            obj.Set("isDown", MouseDown);
            obj.Set("screenCoords", ToDuk(ctx, screenCoords));
            obj.Set("mapCoords", ToDuk(ctx, info.Loc));

            if (info.SpriteType == ViewportInteractionItem::Entity && info.Entity != nullptr)
            {
                obj.Set("entityId", info.Entity->sprite_index.ToUnderlying());
            }
            else if (info.Element != nullptr)
            {
                int32_t index = 0;
                auto el = map_get_first_element_at(info.Loc);
                if (el != nullptr)
                {
                    do
                    {
                        if (el == info.Element)
                        {
                            obj.Set("tileElementIndex", index);
                            break;
                        }
                        index++;
                    } while (!(el++)->IsLastForTile());
                }
            }

            auto* scriptEngine = GetContext()->GetScriptEngine();
            std::vector<DukValue> args;
            args.emplace_back(obj.Take());
            scriptEngine->ExecutePluginCall(Owner, dukHandler, args, false);
        }
    }

    void InitialiseCustomTool(ScriptEngine& scriptEngine, const DukValue& dukValue)
    {
        try
        {
            if (dukValue.type() == DukValue::Type::OBJECT)
            {
                CustomTool customTool;
                customTool.Owner = scriptEngine.GetExecInfo().GetCurrentPlugin();
                customTool.Id = dukValue["id"].as_string();
                customTool.Cursor = FromDuk<CursorID>(dukValue["cursor"]);
                if (customTool.Cursor == CursorID::Undefined)
                {
                    customTool.Cursor = CursorID::Arrow;
                }

                auto dukFilter = dukValue["filter"];
                if (dukFilter.is_array())
                {
                    customTool.Filter = 0;
                    auto dukItems = dukFilter.as_array();
                    for (const auto& dukItem : dukItems)
                    {
                        if (dukItem.type() == DukValue::Type::STRING)
                        {
                            auto value = ToolFilterMap[dukItem.as_string()];
                            customTool.Filter |= static_cast<uint32_t>(EnumToFlag(value));
                        }
                    }
                }
                else
                {
                    customTool.Filter = ViewportInteractionItemAll;
                }

                customTool.onStart = dukValue["onStart"];
                customTool.onDown = dukValue["onDown"];
                customTool.onMove = dukValue["onMove"];
                customTool.onUp = dukValue["onUp"];
                customTool.onFinish = dukValue["onFinish"];

                auto toolbarWindow = window_find_by_class(WC_TOP_TOOLBAR);
                if (toolbarWindow != nullptr)
                {
                    // Use a widget that does not exist on top toolbar but also make sure it isn't -1 as that
                    // prevents abort from being called.
                    rct_widgetindex widgetIndex = -2;
                    tool_cancel();
                    tool_set(toolbarWindow, widgetIndex, static_cast<Tool>(customTool.Cursor));
                    ActiveCustomTool = std::move(customTool);
                    ActiveCustomTool->Start();
                }
            }
        }
        catch (const DukException&)
        {
            duk_error(scriptEngine.GetContext(), DUK_ERR_ERROR, "Invalid parameters.");
        }
    }
} // namespace OpenRCT2::Scripting

#endif
