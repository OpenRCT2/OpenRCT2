/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "ScEntity.hpp"

struct Litter;

namespace OpenRCT2::Scripting
{
    class ScLitter : public ScEntity
    {
    public:
        ScLitter(EntityId Id);

        static void Register(duk_context* ctx);

    private:
        Litter* GetLitter() const;

        std::string litterType_get() const;
        void litterType_set(const std::string& litterType);

        uint32_t creationTick_get() const;
    };

} // namespace OpenRCT2::Scripting

#endif
