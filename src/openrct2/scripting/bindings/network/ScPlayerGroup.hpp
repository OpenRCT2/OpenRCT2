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

#    include "../../Duktape.hpp"

#    include <string>
#    include <vector>

namespace OpenRCT2::Scripting
{
    class ScPlayerGroup
    {
        int32_t _id;

    public:
        ScPlayerGroup(int32_t id);

        int32_t id_get();

        std::string name_get() const;
        void name_set(std::string value);

        std::vector<std::string> permissions_get() const;
        void permissions_set(std::vector<std::string> value);

        static void Register(duk_context* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif
