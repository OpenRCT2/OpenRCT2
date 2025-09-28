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

    #include <functional>

namespace OpenRCT2::Scripting
{
    class ScDisposable;
    extern ScDisposable gScDisposable;

    class ScDisposable final : public ScBase
    {
        using OpaqueType = std::function<void()>;

    public:
        static JSValue dispose(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            OpaqueType* data = gScDisposable.GetOpaque<OpaqueType*>(thisVal);
            if (data && *data)
            {
                (*data)();
            }
            return JS_UNDEFINED;
        }

        JSValue New(JSContext* ctx, const std::function<void()>& f)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CFUNC_DEF("dispose", 0, ScDisposable::dispose),
            };

            return MakeWithOpaque(ctx, funcs, f ? new OpaqueType(f) : nullptr);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Disposable", Finalize);
        }

        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            OpaqueType* data = gScDisposable.GetOpaque<OpaqueType*>(thisVal);
            if (data)
                delete data;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
