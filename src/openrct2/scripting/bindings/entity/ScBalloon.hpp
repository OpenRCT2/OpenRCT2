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

    #include "ScEntity.hpp"

struct Balloon;

namespace OpenRCT2::Scripting
{
    class ScBalloon final : public ScEntity
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId);

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj);
        static Balloon* GetBalloon(JSValue thisVal);

        static JSValue colour_get(JSContext* ctx, JSValue thisVal);
        static JSValue colour_set(JSContext* ctx, JSValue thisVal, JSValue value);
    };
} // namespace OpenRCT2::Scripting
#endif
