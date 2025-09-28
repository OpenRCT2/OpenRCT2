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

    #include <memory>
    #include <openrct2/Context.h>
    #include <openrct2/interface/Cursors.h>
    #include <openrct2/scripting/ScriptEngine.h>
    #include <string>
    #include <vector>

enum class CursorID : uint8_t;

namespace OpenRCT2::Scripting
{
    enum class CustomToolbarMenuItemKind
    {
        Standard,
        Toolbox,
    };

    class CustomToolbarMenuItem
    {
    public:
        std::shared_ptr<Plugin> Owner;
        CustomToolbarMenuItemKind Kind;
        std::string Text;
        JSCallback Callback;

        CustomToolbarMenuItem(
            std::shared_ptr<Plugin> owner, CustomToolbarMenuItemKind kind, const std::string& text, JSCallback callback)
            : Owner(owner)
            , Kind(kind)
            , Text(text)
            , Callback(callback)
        {
        }

        void Invoke() const
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.ExecutePluginCall(Owner, Callback.callback, {}, false);
        }
    };

    class CustomShortcut
    {
    public:
        std::shared_ptr<Plugin> Owner;
        std::string Id;
        std::string Text;
        std::vector<std::string> Bindings;
        JSCallback Callback;

        CustomShortcut(
            std::shared_ptr<Plugin> owner, std::string_view id, std::string_view text, const std::vector<std::string>& bindings,
            JSCallback callback);
        CustomShortcut(CustomShortcut&&) = default;
        CustomShortcut(const CustomShortcut&) = delete;
        ~CustomShortcut();

        CustomShortcut& operator=(const CustomShortcut&) = delete;
        CustomShortcut& operator=(CustomShortcut&& other) = default;

        void Invoke() const;
    };

    struct CustomTool
    {
        std::shared_ptr<Plugin> Owner;
        std::string Id;
        CursorID Cursor = CursorID::Undefined;
        uint32_t Filter{};
        bool MouseDown{};

        // Event handlers
        JSCallback onStart;
        JSCallback onDown;
        JSCallback onMove;
        JSCallback onUp;
        JSCallback onFinish;

        void Start();
        void OnUpdate(const ScreenCoordsXY& screenCoords);
        void OnDown(const ScreenCoordsXY& screenCoords);
        void OnDrag(const ScreenCoordsXY& screenCoords);
        void OnUp(const ScreenCoordsXY& screenCoords);
        void OnAbort();

    private:
        void InvokeEventHandler(JSValue handler, const ScreenCoordsXY& screenCoords);
    };

    extern std::optional<CustomTool> ActiveCustomTool;
    extern std::vector<CustomToolbarMenuItem> CustomMenuItems;
    extern std::vector<std::unique_ptr<CustomShortcut>> CustomShortcuts;

    void InitialiseCustomMenuItems(ScriptEngine& scriptEngine);
    JSValue InitialiseCustomTool(ScriptEngine& scriptEngine, JSContext* ctx, JSValue value);

    JSValue CursorIDToJSValue(JSContext* ctx, CursorID id);
} // namespace OpenRCT2::Scripting

#endif
