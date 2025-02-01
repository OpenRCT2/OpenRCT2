/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScStaff.hpp"

    #include "../../../entity/PatrolArea.h"
    #include "../../../entity/Staff.h"
    #include "../../../object/ObjectManager.h"
    #include "../../../object/PeepAnimationsObject.h"
    #include "../../../peep/PeepAnimations.h"

namespace OpenRCT2::Scripting
{
    ScStaff::ScStaff(EntityId Id)
        : ScPeep(Id)
    {
    }

    void ScStaff::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScPeep, ScStaff>(ctx);
        dukglue_register_property(ctx, &ScStaff::staffType_get, &ScStaff::staffType_set, "staffType");
        dukglue_register_property(ctx, &ScStaff::colour_get, &ScStaff::colour_set, "colour");
        dukglue_register_property(ctx, &ScStaff::availableCostumes_get, nullptr, "availableCostumes");
        dukglue_register_property(ctx, &ScStaff::costume_get, &ScStaff::costume_set, "costume");
        dukglue_register_property(ctx, &ScStaff::patrolArea_get, nullptr, "patrolArea");
        dukglue_register_property(ctx, &ScStaff::orders_get, &ScStaff::orders_set, "orders");
        dukglue_register_property(ctx, &ScStaff::availableAnimations_get, nullptr, "availableAnimations");
        dukglue_register_property(ctx, &ScStaff::animation_get, &ScStaff::animation_set, "animation");
        dukglue_register_property(ctx, &ScStaff::animationOffset_get, &ScStaff::animationOffset_set, "animationOffset");
        dukglue_register_property(ctx, &ScStaff::animationLength_get, nullptr, "animationLength");
        dukglue_register_method(ctx, &ScStaff::getAnimationSpriteIds, "getAnimationSpriteIds");
        dukglue_register_method(ctx, &ScStaff::getCostumeStrings, "getCostumeStrings");
    }

    Staff* ScStaff::GetStaff() const
    {
        return ::GetEntity<Staff>(_id);
    }

    std::string ScStaff::staffType_get() const
    {
        auto peep = GetStaff();
        if (peep != nullptr)
        {
            switch (peep->AssignedStaffType)
            {
                case StaffType::Handyman:
                    return "handyman";
                case StaffType::Mechanic:
                    return "mechanic";
                case StaffType::Security:
                    return "security";
                case StaffType::Entertainer:
                    return "entertainer";
                case StaffType::Count:
                    break;
            }
        }
        return "";
    }

    void ScStaff::staffType_set(const std::string& value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetStaff();
        if (peep != nullptr)
        {
            if (value == "handyman" && peep->AssignedStaffType != StaffType::Handyman)
            {
                peep->AssignedStaffType = StaffType::Handyman;
                peep->AnimationObjectIndex = findPeepAnimationsIndexForType(AnimationPeepType::Handyman);
                peep->AnimationGroup = PeepAnimationGroup::Normal;
            }
            else if (value == "mechanic" && peep->AssignedStaffType != StaffType::Mechanic)
            {
                peep->AssignedStaffType = StaffType::Mechanic;
                peep->AnimationObjectIndex = findPeepAnimationsIndexForType(AnimationPeepType::Mechanic);
                peep->AnimationGroup = PeepAnimationGroup::Normal;
            }
            else if (value == "security" && peep->AssignedStaffType != StaffType::Security)
            {
                peep->AssignedStaffType = StaffType::Security;
                peep->AnimationObjectIndex = findPeepAnimationsIndexForType(AnimationPeepType::Security);
                peep->AnimationGroup = PeepAnimationGroup::Normal;
            }
            else if (value == "entertainer" && peep->AssignedStaffType != StaffType::Entertainer)
            {
                peep->AssignedStaffType = StaffType::Entertainer;
                peep->AnimationObjectIndex = findPeepAnimationsIndexForType(AnimationPeepType::Entertainer);
                peep->AnimationGroup = PeepAnimationGroup::Normal;
            }

            // Reset state to walking to prevent invalid actions from carrying over
            peep->Action = PeepActionType::Walking;
            peep->AnimationType = peep->NextAnimationType = PeepAnimationType::Walking;
        }
    }

    uint8_t ScStaff::colour_get() const
    {
        auto peep = GetStaff();
        return peep != nullptr ? peep->TshirtColour : 0;
    }

    void ScStaff::colour_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetStaff();
        if (peep != nullptr)
        {
            peep->TshirtColour = value;
            peep->TrousersColour = value;
        }
    }

    static const std::vector<AnimationGroupResult> costumesByStaffType(StaffType staffType)
    {
        // TODO: shouldn't get hit repeatedly, but cache these if (and only if) it's too slow
        auto animPeepType = getAnimationPeepType(staffType);
        return getAnimationGroupsByPeepType(animPeepType);
    }

    std::vector<std::string> ScStaff::availableCostumes_get() const
    {
        std::vector<std::string> availableCostumes{};
        auto peep = GetStaff();
        if (peep != nullptr)
        {
            for (auto& costume : costumesByStaffType(peep->AssignedStaffType))
            {
                availableCostumes.push_back(std::string(costume.scriptName));
            }
        }
        return availableCostumes;
    }

    std::vector<std::string> ScStaff::getCostumeStrings() const
    {
        auto peep = GetStaff();
        auto animPeepType = getAnimationPeepType(peep->AssignedStaffType);

        std::vector<std::string> availableCostumes{};
        for (auto& costume : getAvailableCostumeStrings(animPeepType))
        {
            availableCostumes.push_back(costume.friendlyName);
        }
        return availableCostumes;
    }

    std::string ScStaff::costume_get() const
    {
        auto peep = GetStaff();
        if (peep == nullptr)
        {
            return "";
        }

        auto& costumes = costumesByStaffType(peep->AssignedStaffType);

        auto costume = std::find_if(costumes.begin(), costumes.end(), [peep](auto& candidate) {
            return candidate.objectId == peep->AnimationObjectIndex;
        });

        if (costume != costumes.end())
        {
            return std::string(costume->scriptName);
        }
        else
            return "";
    }

    void ScStaff::costume_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();

        auto peep = GetStaff();
        if (peep == nullptr)
        {
            return;
        }

        auto& costumes = costumesByStaffType(peep->AssignedStaffType);
        auto costume = costumes.end();

        // Split by type passed so as to not break old plugins
        if (value.type() == DukValue::Type::STRING)
        {
            costume = std::find_if(costumes.begin(), costumes.end(), [value](auto& candidate) {
                return candidate.scriptName == value.as_string();
            });
        }
        else if (value.type() == DukValue::Type::NUMBER)
        {
            auto target = RCT12PeepAnimationGroup(value.as_uint() + EnumValue(RCT12PeepAnimationGroup::EntertainerPanda));
            costume = std::find_if(
                costumes.begin(), costumes.end(), [target](auto& candidate) { return candidate.legacyPosition == target; });
        }

        if (costume == costumes.end())
            throw DukException() << "Invalid costume for this staff member";

        peep->AnimationObjectIndex = costume->objectId;
        peep->AnimationGroup = costume->group;
    }

    std::shared_ptr<ScPatrolArea> ScStaff::patrolArea_get() const
    {
        return std::make_shared<ScPatrolArea>(_id);
    }

    uint8_t ScStaff::orders_get() const
    {
        auto peep = GetStaff();
        return peep != nullptr ? peep->StaffOrders : 0;
    }

    void ScStaff::orders_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetStaff();
        if (peep != nullptr)
        {
            peep->StaffOrders = value;
        }
    }

    const DukEnumMap<PeepAnimationType>& ScStaff::animationsByStaffType(StaffType staffType) const
    {
        AnimationPeepType animPeepType{};
        switch (staffType)
        {
            case StaffType::Handyman:
                animPeepType = AnimationPeepType::Handyman;
                break;
            case StaffType::Mechanic:
                animPeepType = AnimationPeepType::Mechanic;
                break;
            case StaffType::Security:
                animPeepType = AnimationPeepType::Security;
                break;
            case StaffType::Entertainer:
            default:
                animPeepType = AnimationPeepType::Entertainer;
        }
        return getAnimationsByPeepType(animPeepType);
    }

    std::vector<std::string> ScStaff::availableAnimations_get() const
    {
        std::vector<std::string> availableAnimations{};

        auto* peep = GetStaff();
        if (peep != nullptr)
        {
            for (auto& animation : animationsByStaffType(peep->AssignedStaffType))
            {
                availableAnimations.push_back(std::string(animation.first));
            }
        }

        return availableAnimations;
    }

    std::vector<uint32_t> ScStaff::getAnimationSpriteIds(std::string groupKey, uint8_t rotation) const
    {
        std::vector<uint32_t> spriteIds{};

        auto* peep = GetStaff();
        if (peep == nullptr)
        {
            return spriteIds;
        }

        auto& animationGroups = animationsByStaffType(peep->AssignedStaffType);
        auto animationType = animationGroups.TryGet(groupKey);
        if (animationType == std::nullopt)
        {
            return spriteIds;
        }

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

        const auto& animationGroup = animObj->GetPeepAnimation(peep->AnimationGroup, *animationType);
        for (auto frameOffset : animationGroup.frame_offsets)
        {
            auto imageId = animationGroup.base_image;
            if (animationType != PeepAnimationType::Hanging)
                imageId += rotation + frameOffset * 4;
            else
                imageId += frameOffset;

            spriteIds.push_back(imageId);
        }

        return spriteIds;
    }

    std::string ScStaff::animation_get() const
    {
        auto* peep = GetStaff();
        if (peep == nullptr)
        {
            return nullptr;
        }

        auto& animationGroups = animationsByStaffType(peep->AssignedStaffType);
        std::string_view action = animationGroups[peep->AnimationType];
        return std::string(action);
    }

    void ScStaff::animation_set(std::string groupKey)
    {
        ThrowIfGameStateNotMutable();

        auto* peep = GetStaff();
        auto& animationGroups = animationsByStaffType(peep->AssignedStaffType);
        auto newType = animationGroups.TryGet(groupKey);
        if (newType == std::nullopt)
        {
            throw DukException() << "Invalid animation for this staff member (" << groupKey << ")";
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
        peep->AnimationImageIdOffset = animationGroup.frame_offsets[offset];
        peep->Invalidate();
        peep->UpdateSpriteBoundingBox();
        peep->Invalidate();
    }

    uint8_t ScStaff::animationOffset_get() const
    {
        auto* peep = GetStaff();
        if (peep == nullptr)
        {
            return 0;
        }

        if (peep->IsActionWalking())
            return peep->WalkingAnimationFrameNum;
        else
            return peep->AnimationFrameNum;
    }

    void ScStaff::animationOffset_set(uint8_t offset)
    {
        ThrowIfGameStateNotMutable();

        auto* peep = GetStaff();

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

        const auto& animationGroup = animObj->GetPeepAnimation(peep->AnimationGroup, peep->AnimationType);
        auto length = animationGroup.frame_offsets.size();
        offset %= length;

        if (peep->IsActionWalking())
            peep->WalkingAnimationFrameNum = offset;
        else
            peep->AnimationFrameNum = offset;

        peep->AnimationImageIdOffset = animationGroup.frame_offsets[offset];
        peep->Invalidate();
        peep->UpdateSpriteBoundingBox();
        peep->Invalidate();
    }

    uint8_t ScStaff::animationLength_get() const
    {
        auto* peep = GetStaff();
        if (peep == nullptr)
        {
            return 0;
        }

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

        const auto& animationGroup = animObj->GetPeepAnimation(peep->AnimationGroup, peep->AnimationType);
        return static_cast<uint8_t>(animationGroup.frame_offsets.size());
    }

    ScHandyman::ScHandyman(EntityId Id)
        : ScStaff(Id)
    {
    }

    void ScHandyman::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScStaff, ScHandyman>(ctx);
        dukglue_register_property(ctx, &ScHandyman::lawnsMown_get, nullptr, "lawnsMown");
        dukglue_register_property(ctx, &ScHandyman::gardensWatered_get, nullptr, "gardensWatered");
        dukglue_register_property(ctx, &ScHandyman::litterSwept_get, nullptr, "litterSwept");
        dukglue_register_property(ctx, &ScHandyman::binsEmptied_get, nullptr, "binsEmptied");
    }

    Staff* ScHandyman::GetHandyman() const
    {
        return ::GetEntity<Staff>(_id);
    }

    DukValue ScHandyman::lawnsMown_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto peep = GetHandyman();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Handyman)
        {
            duk_push_uint(ctx, peep->StaffLawnsMown);
        }
        else
        {
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    DukValue ScHandyman::gardensWatered_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto peep = GetHandyman();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Handyman)
        {
            duk_push_uint(ctx, peep->StaffGardensWatered);
        }
        else
        {
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    DukValue ScHandyman::litterSwept_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto peep = GetHandyman();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Handyman)
        {
            duk_push_uint(ctx, peep->StaffLitterSwept);
        }
        else
        {
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    DukValue ScHandyman::binsEmptied_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto peep = GetHandyman();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Handyman)
        {
            duk_push_uint(ctx, peep->StaffBinsEmptied);
        }
        else
        {
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    ScMechanic::ScMechanic(EntityId Id)
        : ScStaff(Id)
    {
    }

    void ScMechanic::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScStaff, ScMechanic>(ctx);
        dukglue_register_property(ctx, &ScMechanic::ridesFixed_get, nullptr, "ridesFixed");
        dukglue_register_property(ctx, &ScMechanic::ridesInspected_get, nullptr, "ridesInspected");
    }

    Staff* ScMechanic::GetMechanic() const
    {
        return ::GetEntity<Staff>(_id);
    }

    DukValue ScMechanic::ridesFixed_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto peep = GetMechanic();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Mechanic)
        {
            duk_push_uint(ctx, peep->StaffRidesFixed);
        }
        else
        {
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    DukValue ScMechanic::ridesInspected_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto peep = GetMechanic();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Mechanic)
        {
            duk_push_uint(ctx, peep->StaffRidesInspected);
        }
        else
        {
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    ScSecurity::ScSecurity(EntityId Id)
        : ScStaff(Id)
    {
    }

    void ScSecurity::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScStaff, ScSecurity>(ctx);
        dukglue_register_property(ctx, &ScSecurity::vandalsStopped_get, nullptr, "vandalsStopped");
    }

    Staff* ScSecurity::GetSecurity() const
    {
        return ::GetEntity<Staff>(_id);
    }

    DukValue ScSecurity::vandalsStopped_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto peep = GetSecurity();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Security)
        {
            duk_push_uint(ctx, peep->StaffVandalsStopped);
        }
        else
        {
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    ScPatrolArea::ScPatrolArea(EntityId id)
        : _staffId(id)
    {
    }

    void ScPatrolArea::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScPatrolArea::tiles_get, &ScPatrolArea::tiles_set, "tiles");
        dukglue_register_method(ctx, &ScPatrolArea::clear, "clear");
        dukglue_register_method(ctx, &ScPatrolArea::add, "add");
        dukglue_register_method(ctx, &ScPatrolArea::remove, "remove");
        dukglue_register_method(ctx, &ScPatrolArea::contains, "contains");
    }

    Staff* ScPatrolArea::GetStaff() const
    {
        return GetEntity<Staff>(_staffId);
    }

    void ScPatrolArea::ModifyArea(const DukValue& coordsOrRange, bool value) const
    {
        auto staff = GetStaff();
        if (staff != nullptr)
        {
            if (coordsOrRange.is_array())
            {
                auto dukCoords = coordsOrRange.as_array();
                for (const auto& dukCoord : dukCoords)
                {
                    auto coord = FromDuk<CoordsXY>(dukCoord);
                    staff->SetPatrolArea(coord, value);
                    MapInvalidateTileFull(coord);
                }
            }
            else
            {
                auto mapRange = FromDuk<MapRange>(coordsOrRange);
                for (int32_t y = mapRange.GetTop(); y <= mapRange.GetBottom(); y += kCoordsXYStep)
                {
                    for (int32_t x = mapRange.GetLeft(); x <= mapRange.GetRight(); x += kCoordsXYStep)
                    {
                        CoordsXY coord(x, y);
                        staff->SetPatrolArea(coord, value);
                        MapInvalidateTileFull(coord);
                    }
                }
            }
            UpdateConsolidatedPatrolAreas();
        }
    }

    DukValue ScPatrolArea::tiles_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        duk_push_array(ctx);

        auto staff = GetStaff();
        if (staff != nullptr && staff->PatrolInfo != nullptr)
        {
            auto tiles = staff->PatrolInfo->ToVector();

            duk_uarridx_t index = 0;
            for (const auto& tile : tiles)
            {
                auto dukCoord = ToDuk(ctx, tile.ToCoordsXY());
                dukCoord.push();
                duk_put_prop_index(ctx, -2, index);
                index++;
            }
        }

        return DukValue::take_from_stack(ctx, -1);
    }

    void ScPatrolArea::tiles_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();

        auto staff = GetStaff();
        if (staff != nullptr)
        {
            staff->ClearPatrolArea();
            if (value.is_array())
            {
                ModifyArea(value, true);
            }
        }
    }

    void ScPatrolArea::clear()
    {
        ThrowIfGameStateNotMutable();

        auto staff = GetStaff();
        if (staff != nullptr)
        {
            staff->ClearPatrolArea();
            UpdateConsolidatedPatrolAreas();
        }
    }

    void ScPatrolArea::add(const DukValue& coordsOrRange)
    {
        ThrowIfGameStateNotMutable();
        ModifyArea(coordsOrRange, true);
    }

    void ScPatrolArea::remove(const DukValue& coordsOrRange)
    {
        ThrowIfGameStateNotMutable();
        ModifyArea(coordsOrRange, false);
    }

    bool ScPatrolArea::contains(const DukValue& coord) const
    {
        auto staff = GetStaff();
        if (staff != nullptr)
        {
            auto pos = FromDuk<CoordsXY>(coord);
            return staff->IsLocationInPatrol(pos);
        }
        return false;
    }

} // namespace OpenRCT2::Scripting

#endif
