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

#    include <openrct2/Input.h>
#    include <openrct2/world/Map.h>
#    include <openrct2/world/Sprite.h>

namespace OpenRCT2::Scripting
{
    std::optional<CustomTool> ActiveCustomTool;
    std::vector<CustomToolbarMenuItem> CustomMenuItems;

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
        auto& scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine.ExecutePluginCall(Owner, onStart, {}, false);
    }

    void CustomTool::OnUp(const ScreenCoordsXY& screenCoords)
    {
        MouseDown = false;
        InvokeEventHandler(onUp, screenCoords);
    }

    void CustomTool::OnAbort()
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine.ExecutePluginCall(Owner, onFinish, {}, false);
    }

    static DukValue CoordsXYToDuk(duk_context* ctx, const CoordsXY& coords)
    {
        DukObject dukCoords(ctx);
        dukCoords.Set("x", coords.x);
        dukCoords.Set("y", coords.y);
        return dukCoords.Take();
    }

    static DukValue CoordsXYToDuk(duk_context* ctx, const ScreenCoordsXY& coords)
    {
        DukObject dukCoords(ctx);
        dukCoords.Set("x", coords.x);
        dukCoords.Set("y", coords.y);
        return dukCoords.Take();
    }

    void CustomTool::InvokeEventHandler(const DukValue& dukHandler, const ScreenCoordsXY& screenCoords)
    {
        if (dukHandler.is_function())
        {
            auto ctx = dukHandler.context();

            auto flags = 0;
            CoordsXY mapCoords{};
            int32_t interactionType = 0;
            TileElement* tileElement{};
            get_map_coordinates_from_pos(screenCoords, flags, mapCoords, &interactionType, &tileElement, nullptr);

            DukObject obj(dukHandler.context());
            obj.Set("isDown", MouseDown);
            obj.Set("screenCoords", CoordsXYToDuk(ctx, screenCoords));
            obj.Set("mapCoords", CoordsXYToDuk(ctx, mapCoords));

            if (interactionType == VIEWPORT_INTERACTION_ITEM_SPRITE && tileElement != nullptr)
            {
                // get_map_coordinates_from_pos returns the sprite using tileElement... ugh
                auto sprite = reinterpret_cast<rct_sprite*>(tileElement);
                obj.Set("entityId", sprite->generic.sprite_index);
            }
            else if (tileElement != nullptr)
            {
                int32_t index = 0;
                auto el = map_get_first_element_at(mapCoords);
                if (el != nullptr)
                {
                    do
                    {
                        if (el == tileElement)
                        {
                            obj.Set("tileElementIndex", index);
                            break;
                        }
                        index++;
                    } while (!(el++)->IsLastForTile());
                }
            }
            auto eventArgs = obj.Take();

            auto& scriptEngine = GetContext()->GetScriptEngine();
            std::vector<DukValue> args;
            args.push_back(eventArgs);
            scriptEngine.ExecutePluginCall(Owner, dukHandler, args, false);
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
                customTool.Cursor = StringToCursor(dukValue["cursor"].as_string());
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
                    tool_set(toolbarWindow, widgetIndex, static_cast<TOOL_IDX>(customTool.Cursor));
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

    static constexpr const char* CursorNames[] = {
        "arrow",         "blank",      "up_arrow",      "up_down_arrow", "hand_point", "zzz",         "diagonal_arrows",
        "picker",        "tree_down",  "fountain_down", "statue_down",   "bench_down", "cross_hair",  "bin_down",
        "lamppost_down", "fence_down", "flower_down",   "path_down",     "dig_down",   "water_down",  "house_down",
        "volcano_down",  "walk_down",  "paint_down",    "entrance_down", "hand_open",  "hand_closed",
    };

    std::string CursorToString(int32_t cursor)
    {
        if (cursor >= 0 && static_cast<size_t>(cursor) < std::size(CursorNames))
        {
            return CursorNames[cursor];
        }
        return {};
    }

    CURSOR_ID StringToCursor(const std::string_view& cursor)
    {
        auto it = std::find(std::begin(CursorNames), std::end(CursorNames), cursor);
        if (it != std::end(CursorNames))
        {
            return static_cast<CURSOR_ID>(std::distance(std::begin(CursorNames), it));
        }
        return CURSOR_UNDEFINED;
    }

} // namespace OpenRCT2::Scripting

#endif
