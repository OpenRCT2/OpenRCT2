/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScPeep.hpp"

namespace OpenRCT2::Scripting
{
    ScPeep gScPeep;

    JSValue ScPeep::New(JSContext* ctx, EntityId entityId)
    {
        return gScEntity.NewDerivedInstance(ctx, entityId, gScPeep.GetProto());
    }

    void ScPeep::Register(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("peepType", &ScPeep::peepType_get, nullptr),
            JS_CGETSET_DEF("name", &ScPeep::name_get, &ScPeep::name_set),
            JS_CGETSET_DEF("destination", &ScPeep::destination_get, &ScPeep::destination_set),
            JS_CGETSET_DEF("direction", &ScPeep::direction_get, &ScPeep::direction_set),
            JS_CGETSET_DEF("energy", &ScPeep::energy_get, &ScPeep::energy_set),
            JS_CGETSET_DEF("energyTarget", &ScPeep::energyTarget_get, &ScPeep::energyTarget_set),
            JS_CFUNC_DEF("getFlag", 1, &ScPeep::getFlag),
            JS_CFUNC_DEF("setFlag", 2, &ScPeep::setFlag),
        };
        gScPeep.RegisterDerived(ctx, gScEntity, funcs);
    }

} // namespace OpenRCT2::Scripting

#endif
