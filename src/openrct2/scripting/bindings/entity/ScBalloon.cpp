/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScBalloon.hpp"

    #include "../../../entity/Balloon.h"

namespace OpenRCT2::Scripting
{
    ScBalloon::ScBalloon(EntityId Id)
        : ScEntity(Id)
    {
    }

    void ScBalloon::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScEntity, ScBalloon>(ctx);
        dukglue_register_property(ctx, &ScBalloon::colour_get, &ScBalloon::colour_set, "colour");
    }

    Balloon* ScBalloon::GetBalloon() const
    {
        return getGameState().entities.GetEntity<Balloon>(_id);
    }

    uint8_t ScBalloon::colour_get() const
    {
        auto balloon = GetBalloon();
        if (balloon != nullptr)
        {
            return EnumValue(balloon->colour);
        }
        return 0;
    }

    void ScBalloon::colour_set(uint8_t value)
    {
        auto balloon = GetBalloon();
        if (balloon != nullptr)
        {
            balloon->colour = static_cast<Drawing::Colour>(value);
            balloon->Invalidate();
        }
    }

} // namespace OpenRCT2::Scripting

#endif
