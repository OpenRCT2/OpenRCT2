/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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

enum class PeepActionSpriteType : uint8_t;
enum class StaffType : uint8_t;

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

        std::vector<std::string> availableCostumes_get() const;
        std::string costume_get() const;
        void costume_set(const DukValue& value);

        std::shared_ptr<ScPatrolArea> patrolArea_get() const;

        uint8_t orders_get() const;
        void orders_set(uint8_t value);

        const DukEnumMap<PeepActionSpriteType>& animationsByStaffType(StaffType staffType) const;
        std::vector<uint32_t> getAnimationSpriteIds(std::string groupKey, uint8_t rotation) const;
        std::vector<std::string> availableAnimations_get() const;
        std::string animation_get() const;
        void animation_set(std::string groupKey);
        uint8_t animationOffset_get() const;
        void animationOffset_set(uint8_t offset);
        uint8_t animationLength_get() const;

        DukValue lawnsMown_get() const;
        void lawnsMown_set(uint32_t value);

        DukValue gardensWatered_get() const;
        void gardensWatered_set(uint32_t value);

        DukValue litterSwept_get() const;
        void litterSwept_set(uint32_t value);

        DukValue binsEmptied_get() const;
        void binsEmptied_set(uint32_t value);

        DukValue ridesFixed_get() const;
        void ridesFixed_set(uint32_t value);

        DukValue ridesInspected_get() const;
        void ridesInspected_set(uint32_t value);

        DukValue vandalsStopped_get() const;
        void vandalsStopped_set(uint32_t value);
    };

} // namespace OpenRCT2::Scripting

#endif
