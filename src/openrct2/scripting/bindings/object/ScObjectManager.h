/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"
#    include "ScInstalledObject.hpp"
#    include "ScObject.hpp"

#    include <memory>

namespace OpenRCT2::Scripting
{
    class ScObjectManager
    {
    public:
        static void Register(duk_context* ctx);

        std::vector<std::shared_ptr<ScInstalledObject>> installedObjects_get() const;

        DukValue load(const DukValue& p1, const DukValue& p2);
        void unload(const DukValue& p1, const DukValue& p2);

        DukValue getObject(const std::string& typez, int32_t index) const;
        std::vector<DukValue> getAllObjects(const std::string& typez) const;

    private:
        static void MarkAsResearched(const Object* object);
        static void RefreshResearchedItems();
        static DukValue CreateScObject(duk_context* ctx, ObjectType type, int32_t index);
    };
} // namespace OpenRCT2::Scripting

#endif
