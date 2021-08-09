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

#    include "../../Duktape.hpp"

#    include <functional>

namespace OpenRCT2::Scripting
{
    class ScDisposable
    {
    private:
        std::function<void()> _onDispose;

    public:
        ScDisposable(const std::function<void()>& onDispose)
            : _onDispose(onDispose)
        {
        }

        void dispose() const
        {
            if (_onDispose)
            {
                _onDispose();
            }
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_method(ctx, &ScDisposable::dispose, "dispose");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
