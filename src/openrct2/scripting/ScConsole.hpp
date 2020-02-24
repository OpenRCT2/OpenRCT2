/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __ENABLE_SCRIPTING__

#    include "../interface/InteractiveConsole.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

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

        void log(DukValue val)
        {
            _console.WriteLine(Stringify(val));
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_method(ctx, &ScConsole::clear, "clear");
            dukglue_register_method(ctx, &ScConsole::log, "log");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
