/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "ScPeep.hpp"

    #include <memory>

enum class PeepAnimationType : uint8_t;
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
        std::vector<std::string> getCostumeStrings() const;
        std::string costume_get() const;
        void costume_set(const DukValue& value);

        std::shared_ptr<ScPatrolArea> patrolArea_get() const;

        uint8_t orders_get() const;
        void orders_set(uint8_t value);

        const DukEnumMap<PeepAnimationType>& animationsByStaffType(StaffType staffType) const;
        std::vector<uint32_t> getAnimationSpriteIds(std::string groupKey, uint8_t rotation) const;
        std::vector<std::string> availableAnimations_get() const;
        std::string animation_get() const;
        void animation_set(std::string groupKey);
        uint8_t animationOffset_get() const;
        void animationOffset_set(uint8_t offset);
        uint8_t animationLength_get() const;
    };

    class ScHandyman : public ScStaff
    {
    public:
        ScHandyman(EntityId Id);

        static void Register(duk_context* ctx);

    private:
        Staff* GetHandyman() const;

        DukValue lawnsMown_get() const;

        DukValue gardensWatered_get() const;

        DukValue litterSwept_get() const;

        DukValue binsEmptied_get() const;
    };

    class ScMechanic : public ScStaff
    {
    public:
        ScMechanic(EntityId Id);

        static void Register(duk_context* ctx);

    private:
        Staff* GetMechanic() const;

        DukValue ridesFixed_get() const;

        DukValue ridesInspected_get() const;
    };

    class ScSecurity : public ScStaff
    {
    public:
        ScSecurity(EntityId Id);

        static void Register(duk_context* ctx);

    private:
        Staff* GetSecurity() const;

        DukValue vandalsStopped_get() const;
    };

} // namespace OpenRCT2::Scripting

#endif
