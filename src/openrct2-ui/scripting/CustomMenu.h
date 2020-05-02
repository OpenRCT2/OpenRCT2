/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include <memory>
#    include <openrct2/Context.h>
#    include <openrct2/interface/Cursors.h>
#    include <openrct2/scripting/Duktape.hpp>
#    include <openrct2/scripting/ScriptEngine.h>
#    include <string>
#    include <vector>

namespace OpenRCT2::Scripting
{
    class CustomToolbarMenuItem
    {
    public:
        std::shared_ptr<Plugin> Owner;
        std::string Text;
        DukValue Callback;

        CustomToolbarMenuItem(std::shared_ptr<Plugin> owner, const std::string& text, DukValue callback)
            : Owner(owner)
            , Text(text)
            , Callback(callback)
        {
        }

        void Invoke() const
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.ExecutePluginCall(Owner, Callback, {}, false);
        }
    };

    struct CustomTool
    {
        std::shared_ptr<Plugin> Owner;
        std::string Id;
        CURSOR_ID Cursor{};
        bool MouseDown{};

        // Event handlers
        DukValue onStart;
        DukValue onDown;
        DukValue onMove;
        DukValue onUp;
        DukValue onFinish;

        void Start();
        void OnUpdate(const ScreenCoordsXY& screenCoords);
        void OnDown(const ScreenCoordsXY& screenCoords);
        void OnDrag(const ScreenCoordsXY& screenCoords);
        void OnUp(const ScreenCoordsXY& screenCoords);
        void OnAbort();

    private:
        void InvokeEventHandler(const DukValue& dukHandler, const ScreenCoordsXY& screenCoords);
    };

    extern std::optional<CustomTool> ActiveCustomTool;
    extern std::vector<CustomToolbarMenuItem> CustomMenuItems;

    void InitialiseCustomMenuItems(ScriptEngine& scriptEngine);
    void InitialiseCustomTool(ScriptEngine& scriptEngine, const DukValue& dukValue);

    template<> DukValue ToDuk(duk_context* ctx, const CURSOR_ID& value);
    template<> CURSOR_ID FromDuk(const DukValue& s);

} // namespace OpenRCT2::Scripting

#endif
