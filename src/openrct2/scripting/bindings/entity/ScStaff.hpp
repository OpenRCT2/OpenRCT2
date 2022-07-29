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

#    include "ScPeep.hpp"

#    include <memory>

namespace OpenRCT2::Scripting
{
    class ScPatrolArea
    {
    private:
        EntityId _staffId;

    public:
        ScPatrolArea(EntityId id);

        static void Register(duk_context* ctx);

    private:
        Staff* GetStaff() const;
        void ModifyArea(const DukValue& coordsOrRange, bool value) const;

        DukValue tiles_get() const;
        void tiles_set(const DukValue& value);

        void clear();
        void add(const DukValue& coordsOrRange);
        void remove(const DukValue& coordsOrRange);
        bool contains(const DukValue& coord) const;
    };

    class ScStaff : public ScPeep
    {
    public:
        ScStaff(EntityId Id);

        static void Register(duk_context* ctx);

    private:
        Staff* GetStaff() const;

        std::string staffType_get() const;
        void staffType_set(const std::string& value);

        uint8_t colour_get() const;
        void colour_set(uint8_t value);

        uint8_t costume_get() const;
        void costume_set(uint8_t value);

        std::shared_ptr<ScPatrolArea> patrolArea_get() const;

        uint8_t orders_get() const;
        void orders_set(uint8_t value);
    };

} // namespace OpenRCT2::Scripting

#endif
