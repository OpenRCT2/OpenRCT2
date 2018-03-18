#pragma once

#include <dukglue/dukglue.h>
#include <functional>

namespace OpenRCT2::Scripting
{
    class ScDisposable
    {
    private:
        std::function<void()> _onDispose;

    public:
        ScDisposable(std::function<void()> onDispose) :
            _onDispose(onDispose)
        {
        }

        void dispose()
        {
            if (_onDispose)
            {
                _onDispose();
            }
        }

        static void Register(duk_context * ctx)
        {
            dukglue_register_method(ctx, &ScDisposable::dispose, "dispose");
        }
    };
}
