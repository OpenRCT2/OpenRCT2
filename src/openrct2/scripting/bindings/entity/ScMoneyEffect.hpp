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
    struct MoneyEffect;
}

namespace OpenRCT2::Scripting
{
    class ScMoneyEffect;
    extern ScMoneyEffect gScMoneyEffect;

    class ScMoneyEffect final : public ScEntity
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId);
        void Register(JSContext* ctx);

    private:
        static MoneyEffect* GetMoneyEffect(JSValue thisVal);

        static JSValue value_get(JSContext* ctx, JSValue thisVal);
        static JSValue value_set(JSContext* ctx, JSValue thisVal, JSValue value);
    };
} // namespace OpenRCT2::Scripting
#endif
