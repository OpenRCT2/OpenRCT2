/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScPeep.hpp"

    #include <memory>

enum class PeepAnimationType : uint8_t;
enum class StaffType : uint8_t;

namespace OpenRCT2::Scripting
{
    class ScPatrolArea;
    extern ScPatrolArea gScPatrolArea;

    class ScPatrolArea final : public ScBase
    {
    public:
        JSValue New(JSContext* ctx, EntityId staffId);
        void Register(JSContext* ctx);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
        static Staff* GetStaff(JSValue thisVal);
        static void ModifyArea(JSContext* ctx, JSValue thisVal, JSValue coordsOrRange, bool reset);

        static JSValue tiles_get(JSContext* ctx, JSValue thisVal);
        static JSValue tiles_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue clear(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue add(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue remove(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue contains(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
    };

    class ScStaff : public ScPeep
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId);

    protected:
        static Staff* GetStaff(JSValue thisVal);

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj);

        static JSValue staffType_get(JSContext* ctx, JSValue thisVal);
        static JSValue staffType_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue colour_get(JSContext* ctx, JSValue thisVal);
        static JSValue colour_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue availableCostumes_get(JSContext* ctx, JSValue thisVal);
        static JSValue getCostumeStrings(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue costume_get(JSContext* ctx, JSValue thisVal);
        static JSValue costume_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue patrolArea_get(JSContext* ctx, JSValue thisVal);

        static JSValue orders_get(JSContext* ctx, JSValue thisVal);
        static JSValue orders_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static EnumMap<PeepAnimationType> animationsByStaffType(StaffType staffType);
        static JSValue getAnimationSpriteIds(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue availableAnimations_get(JSContext* ctx, JSValue thisVal);
        static JSValue animation_get(JSContext* ctx, JSValue thisVal);
        static JSValue animation_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);
        static JSValue animationOffset_get(JSContext* ctx, JSValue thisVal);
        static JSValue animationOffset_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);
        static JSValue animationLength_get(JSContext* ctx, JSValue thisVal);
    };

    class ScHandyman final : public ScStaff
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId);

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj);

        static JSValue lawnsMown_get(JSContext* ctx, JSValue thisVal);

        static JSValue gardensWatered_get(JSContext* ctx, JSValue thisVal);

        static JSValue litterSwept_get(JSContext* ctx, JSValue thisVal);

        static JSValue binsEmptied_get(JSContext* ctx, JSValue thisVal);
    };

    class ScMechanic final : public ScStaff
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId);

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj);

        static JSValue ridesFixed_get(JSContext* ctx, JSValue thisVal);

        static JSValue ridesInspected_get(JSContext* ctx, JSValue thisVal);
    };

    class ScSecurity final : public ScStaff
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId);

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj);

        static JSValue vandalsStopped_get(JSContext* ctx, JSValue thisVal);
    };

} // namespace OpenRCT2::Scripting

#endif
