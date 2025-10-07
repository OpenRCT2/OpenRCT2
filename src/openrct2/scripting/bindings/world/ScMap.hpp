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

    #include "../../ScriptEngine.h"
    #include "../ride/ScRide.hpp"
    #include "../ride/ScTrackIterator.h"
    #include "../world/ScTile.hpp"

namespace OpenRCT2::Scripting
{
    class ScMap;
    extern ScMap gScMap;

    class ScMap final : public ScBase
    {
    private:
        static JSValue size_get(JSContext* ctx, JSValue thisVal);

        static JSValue numRides_get(JSContext* ctx, JSValue thisVal);

        static JSValue numEntities_get(JSContext* ctx, JSValue thisVal);

        static JSValue rides_get(JSContext* ctx, JSValue thisVal);

        static JSValue getRide(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue getTile(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue getEntity(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue getAllEntities(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue getAllEntitiesOnTile(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue createEntity(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue getTrackIterator(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue GetEntityAsDukValue(JSContext* ctx, const EntityBase* sprite);

    public:
        JSValue New(JSContext* ctx);

        void Register(JSContext* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif
