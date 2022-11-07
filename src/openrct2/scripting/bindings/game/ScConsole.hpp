/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../interface/InteractiveConsole.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScConsole
    {
    private:
        InteractiveConsole& _console;

    public:
        ScConsole(InteractiveConsole& console)
            : _console(console)
        {
        }

        void clear()
        {
            _console.Clear();
        }

        duk_ret_t log(duk_context* ctx)
        {
            std::string line;
            auto nargs = duk_get_top(ctx);
            for (duk_idx_t i = 0; i < nargs; i++)
            {
                auto arg = DukValue::copy_from_stack(ctx, i);
                auto argsz = Stringify(arg);
                if (i != 0)
                {
                    line.push_back(' ');
                }
                line += argsz;
            }
            _console.WriteLine(line);
            return 0;
        }

        void executeLegacy(const std::string& command)
        {
            _console.Execute(command);
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_method(ctx, &ScConsole::clear, "clear");
            dukglue_register_method_varargs(ctx, &ScConsole::log, "log");
            dukglue_register_method(ctx, &ScConsole::executeLegacy, "executeLegacy");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
