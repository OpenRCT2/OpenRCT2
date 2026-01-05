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

struct Balloon;

namespace OpenRCT2::Scripting
{

    class ScBalloon : public ScEntity
    {
    public:
        ScBalloon(EntityId Id);

        static void Register(duk_context* ctx);

    private:
        Balloon* GetBalloon() const;

        uint8_t colour_get() const;
        void colour_set(uint8_t);
    };
} // namespace OpenRCT2::Scripting
#endif
