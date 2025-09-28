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

        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("classification", ScWindow::classification_get, nullptr),
            JS_CGETSET_DEF("number", ScWindow::number_get, nullptr),
            JS_CGETSET_DEF("x", ScWindow::x_get, ScWindow::x_set),
            JS_CGETSET_DEF("y", ScWindow::y_get, ScWindow::y_set),
            JS_CGETSET_DEF("width", ScWindow::width_get, ScWindow::width_set),
            JS_CGETSET_DEF("height", ScWindow::height_get, ScWindow::height_set),
            JS_CGETSET_DEF("minWidth", ScWindow::minWidth_get, ScWindow::minWidth_set),
            JS_CGETSET_DEF("maxWidth", ScWindow::maxWidth_get, ScWindow::maxWidth_set),
            JS_CGETSET_DEF("minHeight", ScWindow::minHeight_get, ScWindow::minHeight_set),
            JS_CGETSET_DEF("maxHeight", ScWindow::maxHeight_get, ScWindow::maxHeight_set),
            JS_CGETSET_DEF("isSticky", ScWindow::isSticky_get, nullptr),
            JS_CGETSET_DEF("widgets", ScWindow::widgets_get, nullptr),
            JS_CGETSET_DEF("colours", ScWindow::colours_get, ScWindow::colours_set),
            JS_CGETSET_DEF("title", ScWindow::title_get, ScWindow::title_set),
            JS_CGETSET_DEF("tabIndex", ScWindow::tabIndex_get, ScWindow::tabIndex_set),

            JS_CFUNC_DEF("close", 0, ScWindow::close),
            JS_CFUNC_DEF("findWidget", 1, ScWindow::findWidget),
            JS_CFUNC_DEF("bringToFront", 0, ScWindow::bringToFront)
        };

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);

        static WindowBase* GetWindow(JSValue thisVal);
    };
} // namespace OpenRCT2::Scripting

#endif
