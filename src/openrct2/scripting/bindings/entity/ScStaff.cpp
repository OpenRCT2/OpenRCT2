/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScStaff.hpp"

    #include "../../../entity/PatrolArea.h"
    #include "../../../entity/Staff.h"
    #include "../../../object/ObjectManager.h"
    #include "../../../object/PeepAnimationsObject.h"
    #include "../../../peep/PeepAnimations.h"
    #include "../../../rct12/RCT12.h"
    #include "../../../world/Map.h"

namespace OpenRCT2::Scripting
{
    JSValue ScStaff::New(JSContext* ctx, EntityId entityId)
    {
        JSValue obj = ScPeep::New(ctx, entityId);
        AddFuncs(ctx, obj);
        return obj;
    }

    void ScStaff::AddFuncs(JSContext* ctx, JSValue obj)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("staffType", &ScStaff::staffType_get, &ScStaff::staffType_set),
            JS_CGETSET_DEF("colour", &ScStaff::colour_get, &ScStaff::colour_set),
            JS_CGETSET_DEF("availableCostumes", &ScStaff::availableCostumes_get, nullptr),
            JS_CGETSET_DEF("costume", &ScStaff::costume_get, &ScStaff::costume_set),
            JS_CGETSET_DEF("patrolArea", &ScStaff::patrolArea_get, nullptr),
            JS_CGETSET_DEF("orders", &ScStaff::orders_get, &ScStaff::orders_set),
            JS_CGETSET_DEF("availableAnimations", &ScStaff::availableAnimations_get, nullptr),
            JS_CGETSET_DEF("animation", &ScStaff::animation_get, &ScStaff::animation_set),
            JS_CGETSET_DEF("animationOffset", &ScStaff::animationOffset_get, &ScStaff::animationOffset_set),
            JS_CGETSET_DEF("animationLength", &ScStaff::animationLength_get, nullptr),
            JS_CFUNC_DEF("getAnimationSpriteIds", 2, &ScStaff::getAnimationSpriteIds),
            JS_CFUNC_DEF("getCostumeStrings", 0, &ScStaff::getCostumeStrings)
        };
        JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
    }

    Staff* ScStaff::GetStaff(JSValue thisVal)
    {
        auto id = GetEntityId(thisVal);
        return getGameState().entities.GetEntity<Staff>(id);
    }

    JSValue ScStaff::staffType_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetStaff(thisVal);
        if (peep != nullptr)
        {
            switch (peep->AssignedStaffType)
            {
                case StaffType::handyman:
                    return JSFromStdString(ctx, "handyman");
                case StaffType::mechanic:
                    return JSFromStdString(ctx, "mechanic");
                case StaffType::security:
                    return JSFromStdString(ctx, "security");
                case StaffType::entertainer:
                    return JSFromStdString(ctx, "entertainer");
                case StaffType::count:
                    break;
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScStaff::staffType_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_STR(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetStaff(thisVal);
        if (peep != nullptr)
        {
            if (value == "handyman" && peep->AssignedStaffType != StaffType::handyman)
            {
                peep->AssignedStaffType = StaffType::handyman;
                peep->AnimationObjectIndex = findPeepAnimationsIndexForType(AnimationPeepType::handyman);
                peep->AnimationGroup = PeepAnimationGroup::normal;
            }
            else if (value == "mechanic" && !peep->IsMechanic())
            {
                peep->AssignedStaffType = StaffType::mechanic;
                peep->AnimationObjectIndex = findPeepAnimationsIndexForType(AnimationPeepType::mechanic);
                peep->AnimationGroup = PeepAnimationGroup::normal;
            }
            else if (value == "security" && peep->AssignedStaffType != StaffType::security)
            {
                peep->AssignedStaffType = StaffType::security;
                peep->AnimationObjectIndex = findPeepAnimationsIndexForType(AnimationPeepType::security);
                peep->AnimationGroup = PeepAnimationGroup::normal;
            }
            else if (value == "entertainer" && !peep->isEntertainer())
            {
                peep->AssignedStaffType = StaffType::entertainer;
                peep->AnimationObjectIndex = findPeepAnimationsIndexForType(AnimationPeepType::entertainer);
                peep->AnimationGroup = PeepAnimationGroup::normal;
            }

            // Reset state to walking to prevent invalid actions from carrying over
            peep->Action = PeepActionType::walking;
            peep->AnimationType = peep->NextAnimationType = PeepAnimationType::walking;
            peep->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScStaff::colour_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetStaff(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->TshirtColour : 0);
    }

    JSValue ScStaff::colour_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetStaff(thisVal);
        if (peep != nullptr)
        {
            peep->TshirtColour = value;
            peep->TrousersColour = value;
            peep->Invalidate();
        }
        return JS_UNDEFINED;
    }

    static const std::vector<AnimationGroupResult> costumesByStaffType(StaffType staffType)
    {
        // TODO: shouldn't get hit repeatedly, but cache these if (and only if) it's too slow
        auto animPeepType = getAnimationPeepType(staffType);
        return getAnimationGroupsByPeepType(animPeepType);
    }

    JSValue ScStaff::availableCostumes_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue availableCostumes = JS_NewArray(ctx);
        auto peep = GetStaff(thisVal);
        if (peep != nullptr)
        {
            auto idx = 0;
            for (auto& costume : costumesByStaffType(peep->AssignedStaffType))
            {
                JS_SetPropertyInt64(ctx, availableCostumes, idx++, JSFromStdString(ctx, costume.scriptName));
            }
        }
        return availableCostumes;
    }

    JSValue ScStaff::getCostumeStrings(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        auto peep = GetStaff(thisVal);
        auto animPeepType = getAnimationPeepType(peep->AssignedStaffType);

        JSValue availableCostumes = JS_NewArray(ctx);
        auto idx = 0;

        for (auto& costume : getAvailableCostumeStrings(animPeepType))
        {
            JS_SetPropertyInt64(ctx, availableCostumes, idx++, JSFromStdString(ctx, costume.friendlyName));
        }
        return availableCostumes;
    }

    JSValue ScStaff::costume_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetStaff(thisVal);
        if (peep == nullptr)
        {
            return JS_UNDEFINED;
        }

        auto& costumes = costumesByStaffType(peep->AssignedStaffType);

        auto costume = std::find_if(costumes.begin(), costumes.end(), [peep](auto& candidate) {
            return candidate.objectId == peep->AnimationObjectIndex;
        });

        if (costume != costumes.end())
        {
            return JSFromStdString(ctx, costume->scriptName);
        }
        else
            return JS_UNDEFINED;
    }

    JSValue ScStaff::costume_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto peep = GetStaff(thisVal);
        if (peep == nullptr)
        {
            return JS_UNDEFINED;
        }

        auto& costumes = costumesByStaffType(peep->AssignedStaffType);
        auto costume = costumes.end();

        // Split by type passed so as to not break old plugins
        if (JS_IsString(value))
        {
            JS_UNPACK_STR(valueString, ctx, value);
            costume = std::find_if(costumes.begin(), costumes.end(), [valueString](auto& candidate) {
                return candidate.scriptName == valueString;
            });
        }
        else if (JS_IsNumber(value))
        {
            JS_UNPACK_UINT32(number, ctx, value);
            auto target = RCT12PeepAnimationGroup(number + EnumValue(RCT12PeepAnimationGroup::entertainerPanda));
            costume = std::find_if(
                costumes.begin(), costumes.end(), [target](auto& candidate) { return candidate.legacyPosition == target; });
        }

        if (costume == costumes.end())
        {
            JS_ThrowPlainError(ctx, "Invalid costume for this staff member");
            return JS_EXCEPTION;
        }

        peep->AnimationObjectIndex = costume->objectId;
        peep->AnimationGroup = costume->group;
        peep->Invalidate();
        return JS_UNDEFINED;
    }

    JSValue ScStaff::patrolArea_get(JSContext* ctx, JSValue thisVal)
    {
        auto staffId = GetEntityId(thisVal);
        return gScPatrolArea.New(ctx, staffId);
    }

    JSValue ScStaff::orders_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetStaff(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->StaffOrders : 0);
    }

    JSValue ScStaff::orders_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetStaff(thisVal);
        if (peep != nullptr)
        {
            peep->StaffOrders = value;
        }
        return JS_UNDEFINED;
    }

    EnumMap<PeepAnimationType> ScStaff::animationsByStaffType(StaffType staffType)
    {
        AnimationPeepType animPeepType{};
        switch (staffType)
        {
            case StaffType::handyman:
                animPeepType = AnimationPeepType::handyman;
                break;
            case StaffType::mechanic:
                animPeepType = AnimationPeepType::mechanic;
                break;
            case StaffType::security:
                animPeepType = AnimationPeepType::security;
                break;
            case StaffType::entertainer:
            default:
                animPeepType = AnimationPeepType::entertainer;
        }
        return getAnimationsByPeepType(animPeepType);
    }

    JSValue ScStaff::availableAnimations_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue availableAnimations = JS_NewArray(ctx);

        auto* peep = GetStaff(thisVal);
        if (peep != nullptr)
        {
            auto idx = 0;
            for (auto& animation : animationsByStaffType(peep->AssignedStaffType))
            {
                JS_SetPropertyInt64(ctx, availableAnimations, idx++, JSFromStdString(ctx, animation.first));
            }
        }

        return availableAnimations;
    }

    JSValue ScStaff::getAnimationSpriteIds(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(groupKey, ctx, argv[0]);
        JS_UNPACK_UINT32(rotation, ctx, argv[1]);
        JSValue spriteIds = JS_NewArray(ctx);

        auto* peep = GetStaff(thisVal);
        if (peep == nullptr)
        {
            return spriteIds;
        }

        auto animationGroups = animationsByStaffType(peep->AssignedStaffType);
        auto animationType = animationGroups.TryGet(groupKey);
        if (animationType == std::nullopt)
        {
            return spriteIds;
        }

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

        const auto& animationGroup = animObj->GetPeepAnimation(peep->AnimationGroup, *animationType);
        auto idx = 0;
        for (auto frameOffset : animationGroup.frameOffsets)
        {
            auto imageId = animationGroup.baseImage;
            if (animationType != PeepAnimationType::hanging)
                imageId += rotation + frameOffset * 4;
            else
                imageId += frameOffset;

            JS_SetPropertyInt64(ctx, spriteIds, idx++, JS_NewUint32(ctx, imageId));
        }

        return spriteIds;
    }

    JSValue ScStaff::animation_get(JSContext* ctx, JSValue thisVal)
    {
        auto* peep = GetStaff(thisVal);
        if (peep == nullptr)
        {
            return JS_NULL;
        }

        auto animationGroups = animationsByStaffType(peep->AssignedStaffType);
        return JSFromStdString(ctx, animationGroups[peep->AnimationType]);
    }

    JSValue ScStaff::animation_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_STR(groupKey, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto* peep = GetStaff(thisVal);
        auto animationGroups = animationsByStaffType(peep->AssignedStaffType);
        auto newType = animationGroups.TryGet(groupKey);
        if (newType == std::nullopt)
        {
            JS_ThrowPlainError(ctx, "Invalid animation for this staff member (%s)", groupKey.data());
            return JS_EXCEPTION;
        }

        peep->AnimationType = peep->NextAnimationType = *newType;

        auto offset = 0;
        if (peep->IsActionWalking())
            peep->WalkingAnimationFrameNum = offset;
        else
            peep->AnimationFrameNum = offset;

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

        const auto& animationGroup = animObj->GetPeepAnimation(peep->AnimationGroup, peep->AnimationType);
        peep->AnimationImageIdOffset = animationGroup.frameOffsets[offset];
        peep->Invalidate();
        peep->UpdateSpriteBoundingBox();
        peep->Invalidate();
        return JS_UNDEFINED;
    }

    JSValue ScStaff::animationOffset_get(JSContext* ctx, JSValue thisVal)
    {
        auto* peep = GetStaff(thisVal);
        if (peep == nullptr)
        {
            return JS_NewUint32(ctx, 0);
        }

        auto frame = peep->IsActionWalking() ? peep->WalkingAnimationFrameNum : peep->AnimationFrameNum;
        return JS_NewUint32(ctx, frame);
    }

    JSValue ScStaff::animationOffset_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(offset, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto* peep = GetStaff(thisVal);

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

        const auto& animationGroup = animObj->GetPeepAnimation(peep->AnimationGroup, peep->AnimationType);
        auto length = animationGroup.frameOffsets.size();
        offset %= length;

        if (peep->IsActionWalking())
            peep->WalkingAnimationFrameNum = offset;
        else
            peep->AnimationFrameNum = offset;

        peep->AnimationImageIdOffset = animationGroup.frameOffsets[offset];
        peep->Invalidate();
        peep->UpdateSpriteBoundingBox();
        peep->Invalidate();
        return JS_UNDEFINED;
    }

    JSValue ScStaff::animationLength_get(JSContext* ctx, JSValue thisVal)
    {
        auto* peep = GetStaff(thisVal);
        if (peep == nullptr)
        {
            return JS_NewUint32(ctx, 0);
        }

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

        const auto& animationGroup = animObj->GetPeepAnimation(peep->AnimationGroup, peep->AnimationType);
        auto length = static_cast<uint8_t>(animationGroup.frameOffsets.size());
        return JS_NewUint32(ctx, length);
    }

    JSValue ScHandyman::New(JSContext* ctx, EntityId entityId)
    {
        JSValue obj = ScStaff::New(ctx, entityId);
        AddFuncs(ctx, obj);
        return obj;
    }

    void ScHandyman::AddFuncs(JSContext* ctx, JSValue obj)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("lawnsMown", &ScHandyman::lawnsMown_get, nullptr),
            JS_CGETSET_DEF("gardensWatered", &ScHandyman::gardensWatered_get, nullptr),
            JS_CGETSET_DEF("litterSwept", &ScHandyman::litterSwept_get, nullptr),
            JS_CGETSET_DEF("binsEmptied", &ScHandyman::binsEmptied_get, nullptr),
        };
        JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
    }

    JSValue ScHandyman::lawnsMown_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetStaff(thisVal);
        if (peep != nullptr && peep->AssignedStaffType == StaffType::handyman)
        {
            return JS_NewUint32(ctx, peep->StaffLawnsMown);
        }
        else
        {
            return JS_NULL;
        }
    }

    JSValue ScHandyman::gardensWatered_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetStaff(thisVal);
        if (peep != nullptr && peep->AssignedStaffType == StaffType::handyman)
        {
            return JS_NewUint32(ctx, peep->StaffGardensWatered);
        }
        else
        {
            return JS_NULL;
        }
    }

    JSValue ScHandyman::litterSwept_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetStaff(thisVal);
        if (peep != nullptr && peep->AssignedStaffType == StaffType::handyman)
        {
            return JS_NewUint32(ctx, peep->StaffLitterSwept);
        }
        else
        {
            return JS_NULL;
        }
    }

    JSValue ScHandyman::binsEmptied_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetStaff(thisVal);
        if (peep != nullptr && peep->AssignedStaffType == StaffType::handyman)
        {
            return JS_NewUint32(ctx, peep->StaffBinsEmptied);
        }
        else
        {
            return JS_NULL;
        }
    }

    JSValue ScMechanic::New(JSContext* ctx, EntityId entityId)
    {
        JSValue obj = ScStaff::New(ctx, entityId);
        AddFuncs(ctx, obj);
        return obj;
    }

    void ScMechanic::AddFuncs(JSContext* ctx, JSValue obj)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("ridesFixed", &ScMechanic::ridesFixed_get, nullptr),
            JS_CGETSET_DEF("ridesInspected", &ScMechanic::ridesInspected_get, nullptr),
        };
        JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
    }

    JSValue ScMechanic::ridesFixed_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetStaff(thisVal);
        if (peep != nullptr && peep->IsMechanic())
        {
            return JS_NewUint32(ctx, peep->StaffRidesFixed);
        }
        else
        {
            return JS_NULL;
        }
    }

    JSValue ScMechanic::ridesInspected_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetStaff(thisVal);
        if (peep != nullptr && peep->IsMechanic())
        {
            return JS_NewUint32(ctx, peep->StaffRidesInspected);
        }
        else
        {
            return JS_NULL;
        }
    }

    JSValue ScSecurity::New(JSContext* ctx, EntityId entityId)
    {
        JSValue obj = ScStaff::New(ctx, entityId);
        AddFuncs(ctx, obj);
        return obj;
    }

    void ScSecurity::AddFuncs(JSContext* ctx, JSValue obj)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("vandalsStopped", &ScSecurity::vandalsStopped_get, nullptr),
        };
        JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
    }

    JSValue ScSecurity::vandalsStopped_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetStaff(thisVal);
        if (peep != nullptr && peep->AssignedStaffType == StaffType::security)
        {
            return JS_NewUint32(ctx, peep->StaffVandalsStopped);
        }
        else
        {
            return JS_NULL;
        }
    }

    using OpaquePatrolAreaData = struct
    {
        EntityId staffId;
    };

    JSValue ScPatrolArea::New(JSContext* ctx, EntityId staffId)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("tiles", &ScPatrolArea::tiles_get, &ScPatrolArea::tiles_set),
            JS_CFUNC_DEF("clear", 0, &ScPatrolArea::clear),
            JS_CFUNC_DEF("add", 1, &ScPatrolArea::add),
            JS_CFUNC_DEF("remove", 1, &ScPatrolArea::remove),
            JS_CFUNC_DEF("contains", 1, &ScPatrolArea::contains),
        };
        return MakeWithOpaque(ctx, funcs, new OpaquePatrolAreaData{ staffId });
    }

    void ScPatrolArea::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "PatrolArea", Finalize);
    }

    void ScPatrolArea::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        OpaquePatrolAreaData* data = gScPatrolArea.GetOpaque<OpaquePatrolAreaData*>(thisVal);
        if (data)
            delete data;
    }

    Staff* ScPatrolArea::GetStaff(JSValue thisVal)
    {
        OpaquePatrolAreaData* data = gScPatrolArea.GetOpaque<OpaquePatrolAreaData*>(thisVal);
        return getGameState().entities.GetEntity<Staff>(data->staffId);
    }

    void ScPatrolArea::ModifyArea(JSContext* ctx, JSValue thisVal, JSValue coordsOrRange, bool reset)
    {
        auto staff = GetStaff(thisVal);
        if (staff != nullptr)
        {
            if (JS_IsArray(coordsOrRange))
            {
                JSIterateArray(ctx, coordsOrRange, [staff, reset](JSContext* ctx2, JSValue v) {
                    auto coord = JSToCoordsXY(ctx2, v);
                    staff->SetPatrolArea(coord, reset);
                    MapInvalidateTileFull(coord);
                });
            }
            else
            {
                MapRange mapRange = { JSToCoordXY(ctx, coordsOrRange, "leftTop"),
                                      JSToCoordXY(ctx, coordsOrRange, "rightBottom") };
                for (int32_t y = mapRange.GetY1(); y <= mapRange.GetY2(); y += kCoordsXYStep)
                {
                    for (int32_t x = mapRange.GetX1(); x <= mapRange.GetX2(); x += kCoordsXYStep)
                    {
                        CoordsXY coord(x, y);
                        staff->SetPatrolArea(coord, reset);
                        MapInvalidateTileFull(coord);
                    }
                }
            }
            UpdateConsolidatedPatrolAreas();
        }
    }

    JSValue ScPatrolArea::tiles_get(JSContext* ctx, JSValue thisVal)
    {
        auto array = JS_NewArray(ctx);

        auto staff = GetStaff(thisVal);
        if (staff != nullptr && staff->PatrolInfo != nullptr)
        {
            auto tiles = staff->PatrolInfo->ToVector();

            auto index = 0;
            for (const auto& tile : tiles)
            {
                auto coords = ToJSValue(ctx, tile);
                JS_SetPropertyInt64(ctx, array, index, coords);
                index++;
            }
        }

        return array;
    }

    JSValue ScPatrolArea::tiles_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto staff = GetStaff(thisVal);
        if (staff != nullptr)
        {
            staff->ClearPatrolArea();
            if (JS_IsArray(value))
            {
                ModifyArea(ctx, thisVal, value, true);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScPatrolArea::clear(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto staff = GetStaff(thisVal);
        if (staff != nullptr)
        {
            staff->ClearPatrolArea();
            UpdateConsolidatedPatrolAreas();
        }
        return JS_UNDEFINED;
    }

    JSValue ScPatrolArea::add(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_OBJECT(coordsOrRange, ctx, argv[0])
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        ModifyArea(ctx, thisVal, coordsOrRange, true);
        return JS_UNDEFINED;
    }

    JSValue ScPatrolArea::remove(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_OBJECT(coordsOrRange, ctx, argv[0])
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        ModifyArea(ctx, thisVal, coordsOrRange, false);
        return JS_UNDEFINED;
    }

    JSValue ScPatrolArea::contains(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_OBJECT(coord, ctx, argv[0])
        auto staff = GetStaff(thisVal);
        if (staff != nullptr)
        {
            auto pos = JSToCoordsXY(ctx, coord);
            return JS_NewBool(ctx, staff->IsLocationInPatrol(pos));
        }
        return JS_NewBool(ctx, false);
    }

} // namespace OpenRCT2::Scripting

#endif
