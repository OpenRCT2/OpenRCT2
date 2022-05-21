/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../Duktape.hpp"

#    include <string.h>

namespace OpenRCT2::Scripting
{
    class ScPlayer
    {
    private:
        int32_t _id;

    public:
        ScPlayer(int32_t id);

        int32_t id_get() const;

        std::string name_get() const;

        int32_t group_get() const;
        void group_set(int32_t value);

        int32_t ping_get() const;

        int32_t commandsRan_get() const;

        int32_t moneySpent_get() const;

        std::string ipAddress_get() const;

        std::string publicKeyHash_get() const;

        static void Register(duk_context* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif
