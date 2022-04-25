/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScLitter.hpp"

#    include "../../../entity/Litter.h"

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<Litter::Type> LitterTypeMap({
        { "vomit", Litter::Type::Vomit },
        { "vomit_alt", Litter::Type::VomitAlt },
        { "empty_can", Litter::Type::EmptyCan },
        { "rubbish", Litter::Type::Rubbish },
        { "burger_box", Litter::Type::BurgerBox },
        { "empty_cup", Litter::Type::EmptyCup },
        { "empty_box", Litter::Type::EmptyBox },
        { "empty_bottle", Litter::Type::EmptyBottle },
        { "empty_bowl_red", Litter::Type::EmptyBowlRed },
        { "empty_drink_carton", Litter::Type::EmptyDrinkCarton },
        { "empty_juice_cup", Litter::Type::EmptyJuiceCup },
        { "empty_bowl_blue", Litter::Type::EmptyBowlBlue },
    });

    ScLitter::ScLitter(EntityId Id)
        : ScEntity(Id)
    {
    }

    void ScLitter::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScEntity, ScLitter>(ctx);
        dukglue_register_property(ctx, &ScLitter::litterType_get, &ScLitter::litterType_set, "litterType");
        dukglue_register_property(ctx, &ScLitter::creationTick_get, nullptr, "creationTick");
    }

    Litter* ScLitter::GetLitter() const
    {
        return ::GetEntity<Litter>(_id);
    }

    std::string ScLitter::litterType_get() const
    {
        auto* litter = GetLitter();
        if (litter != nullptr)
        {
            auto it = LitterTypeMap.find(litter->SubType);
            if (it != LitterTypeMap.end())
            {
                return std::string{ it->first };
            }
        }
        return "";
    }

    void ScLitter::litterType_set(const std::string& litterType)
    {
        ThrowIfGameStateNotMutable();

        auto it = LitterTypeMap.find(litterType);
        if (it == LitterTypeMap.end())
            return;
        auto* litter = GetLitter();
        litter->SubType = it->second;
    }

    uint32_t ScLitter::creationTick_get() const
    {
        auto* litter = GetLitter();
        if (litter == nullptr)
            return 0;
        return litter->creationTick;
    }

} // namespace OpenRCT2::Scripting

#endif
