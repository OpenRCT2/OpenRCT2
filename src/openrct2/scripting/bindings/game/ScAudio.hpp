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

    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScAudio;
    extern ScAudio gScAudio;

    class ScAudio final : public ScBase
    {
    private:
        static JSValue playSound(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue playMusic(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue loadSound(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue freeSound(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue stop(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue setRate(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue setLocation(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

    public:
        JSValue New(JSContext* ctx);

        void Register(JSContext* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif
