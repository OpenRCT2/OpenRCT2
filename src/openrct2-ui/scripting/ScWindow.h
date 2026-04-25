/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include <openrct2/interface/WindowBase.h>
    #include <openrct2/scripting/ScriptEngine.h>
    #include <quickjs.h>

namespace OpenRCT2::Scripting
{
    class ScWindow;
    extern ScWindow gScWindow;

    class ScWindow final : public ScBase
    {
    private:
        static JSValue classification_get(JSContext* ctx, JSValue thisVal);

        static JSValue number_get(JSContext* ctx, JSValue thisVal);

        static JSValue x_get(JSContext* ctx, JSValue thisVal);
        static JSValue x_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue y_get(JSContext* ctx, JSValue thisVal);
        static JSValue y_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue width_get(JSContext* ctx, JSValue thisVal);
        static JSValue width_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue height_get(JSContext* ctx, JSValue thisVal);
        static JSValue height_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue minWidth_get(JSContext* ctx, JSValue thisVal);
        static JSValue minWidth_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue maxWidth_get(JSContext* ctx, JSValue thisVal);
        static JSValue maxWidth_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue minHeight_get(JSContext* ctx, JSValue thisVal);
        static JSValue minHeight_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue maxHeight_get(JSContext* ctx, JSValue thisVal);
        static JSValue maxHeight_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue isSticky_get(JSContext* ctx, JSValue thisVal);

        static JSValue widgets_get(JSContext* ctx, JSValue thisVal);

        static JSValue colours_get(JSContext* ctx, JSValue thisVal);
        static JSValue colours_set(JSContext* ctx, JSValue thisVal, JSValue colours);

        static JSValue title_get(JSContext* ctx, JSValue thisVal);
        static JSValue title_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue tabIndex_get(JSContext* ctx, JSValue thisVal);
        static JSValue tabIndex_set(JSContext* ctx, JSValue thisVal, JSValue tab);

        static JSValue close(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue findWidget(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue bringToFront(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

    public:
        JSValue New(JSContext* ctx, WindowBase* w);
        JSValue New(JSContext* ctx, WindowClass wClass, WindowNumber wNum);

        void Register(JSContext* ctx);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);

        static WindowBase* GetWindow(JSValue thisVal);
    };
} // namespace OpenRCT2::Scripting

#endif
