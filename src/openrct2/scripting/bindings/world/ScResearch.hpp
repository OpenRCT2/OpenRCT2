/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScResearch
    {
    private:
        duk_context* _context;

    public:
        ScResearch(duk_context* ctx);

        static void Register(duk_context* ctx);

    private:
        uint8_t funding_get() const;
        void funding_set(uint8_t value);

        std::vector<std::string> priorities_get() const;
        void priorities_set(const std::vector<std::string>& values);

        std::string stage_get() const;
        void stage_set(const std::string& value);

        uint16_t progress_get() const;
        void progress_set(uint16_t value);

        DukValue expectedMonth_get() const;
        DukValue expectedDay_get() const;
        DukValue lastResearchedItem_get() const;
        DukValue expectedItem_get() const;

        std::vector<DukValue> inventedItems_get() const;
        void inventedItems_set(const std::vector<DukValue>& value);

        std::vector<DukValue> uninventedItems_get() const;
        void uninventedItems_set(const std::vector<DukValue>& value);

        bool isObjectResearched(const std::string& typez, ObjectEntryIndex index);

        static std::vector<ResearchItem> ConvertResearchList(const std::vector<DukValue>& value);
    };

} // namespace OpenRCT2::Scripting

#endif
