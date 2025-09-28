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

    #include "../../../interface/InteractiveConsole.h"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScConsole;
    extern ScConsole gScConsole;

    class ScConsole final : public ScBase
    {
    private:
        static JSValue clear(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            if (const auto console = gScConsole.GetOpaque<InteractiveConsole*>(thisVal))
                console->Clear();
            return JS_UNDEFINED;
        }

        static JSValue log(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            if (const auto console = gScConsole.GetOpaque<InteractiveConsole*>(thisVal))
            {
                std::string line;
                for (int i = 0; i < argc; i++)
                {
                    if (i != 0)
                        line.push_back(' ');
                    line += Stringify(ctx, argv[i]);
                }
                console->WriteLine(line);
            }
            return JS_UNDEFINED;
        }

        static JSValue executeLegacy(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            if (const auto console = gScConsole.GetOpaque<InteractiveConsole*>(thisVal))
            {
                JS_UNPACK_STR(str, ctx, argv[0]);
                console->Execute(str);
            }
            return JS_UNDEFINED;
        }

        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CFUNC_DEF("clear", 0, clear),
            JS_CFUNC_DEF("log", 0, log),
            JS_CFUNC_DEF("executeLegacy", 0, executeLegacy),
        };

    public:
        JSValue New(JSContext* ctx, InteractiveConsole& console)
        {
            return MakeWithOpaque(ctx, funcs, &console);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Console");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
