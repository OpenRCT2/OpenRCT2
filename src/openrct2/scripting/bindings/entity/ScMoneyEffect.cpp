/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScMoneyEffect.hpp"

    #include "../../../GameState.h"
    #include "../../../entity/MoneyEffect.h"

namespace OpenRCT2::Scripting
{
    ScMoneyEffect::ScMoneyEffect(EntityId Id)
        : ScEntity(Id)
    {
    }

    void ScMoneyEffect::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScEntity, ScMoneyEffect>(ctx);
        dukglue_register_property(ctx, &ScMoneyEffect::value_get, &ScMoneyEffect::value_set, "value");
    }

    MoneyEffect* ScMoneyEffect::GetMoneyEffect() const
    {
        return getGameState().entities.GetEntity<MoneyEffect>(_id);
    }

    money64 ScMoneyEffect::value_get() const
    {
        auto moneyEffect = GetMoneyEffect();
        if (moneyEffect != nullptr)
        {
            return moneyEffect->Value;
        }
        return 0;
    }

    void ScMoneyEffect::value_set(money64 value)
    {
        auto moneyEffect = GetMoneyEffect();
        if (moneyEffect != nullptr)
        {
            moneyEffect->SetValue(value);
        }
    }

} // namespace OpenRCT2::Scripting

#endif
