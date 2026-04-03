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

    #include "ScEntity.hpp"

namespace OpenRCT2
{
    struct Litter;
}

namespace OpenRCT2::Scripting
{
    class ScLitter;
    extern ScLitter gScLitter;

    class ScLitter final : public ScEntity
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId);
        void Register(JSContext* ctx);

    private:
        static Litter* GetLitter(JSValue thisVal);

        static JSValue litterType_get(JSContext* ctx, JSValue thisVal);
        static JSValue litterType_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue creationTick_get(JSContext* ctx, JSValue thisVal);
    };
} // namespace OpenRCT2::Scripting

#endif
