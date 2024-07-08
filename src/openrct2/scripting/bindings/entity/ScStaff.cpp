/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScStaff.hpp"

#    include "../../../entity/PatrolArea.h"
#    include "../../../entity/Staff.h"
#    include "../../../peep/PeepAnimationData.h"

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<PeepActionSpriteType> availableHandymanAnimations({
        { "walking", PeepActionSpriteType::None },
        { "watchRide", PeepActionSpriteType::WatchRide },
        { "hanging", PeepActionSpriteType::Ui },
        { "staffMower", PeepActionSpriteType::StaffMower },
        { "staffSweep", PeepActionSpriteType::StaffSweep },
        { "drowning", PeepActionSpriteType::Drowning },
        { "staffWatering", PeepActionSpriteType::StaffWatering },
        { "staffEmptyBin", PeepActionSpriteType::StaffEmptyBin },
    });

    static const DukEnumMap<PeepActionSpriteType> availableMechanicAnimations({
        { "walking", PeepActionSpriteType::None },
        { "watchRide", PeepActionSpriteType::WatchRide },
        { "hanging", PeepActionSpriteType::Ui },
        { "drowning", PeepActionSpriteType::Drowning },
        { "staffAnswerCall", PeepActionSpriteType::StaffAnswerCall },
        { "staffAnswerCall2", PeepActionSpriteType::StaffAnswerCall2 },
        { "staffCheckBoard", PeepActionSpriteType::StaffCheckboard },
        { "staffFix", PeepActionSpriteType::StaffFix },
        { "staffFix2", PeepActionSpriteType::StaffFix2 },
        { "staffFixGround", PeepActionSpriteType::StaffFixGround },
        { "staffFix3", PeepActionSpriteType::StaffFix3 },
    });

    static const DukEnumMap<PeepActionSpriteType> availableSecurityAnimations({
        { "walking", PeepActionSpriteType::None },
        { "watchRide", PeepActionSpriteType::WatchRide },
        { "hanging", PeepActionSpriteType::Ui },
        { "drowning", PeepActionSpriteType::Drowning },
    });

    static const DukEnumMap<PeepActionSpriteType> availableEntertainerAnimations({
        { "walking", PeepActionSpriteType::None },
        { "watchRide", PeepActionSpriteType::WatchRide },
        { "wave", PeepActionSpriteType::EatFood }, // NB: this not a typo
        { "hanging", PeepActionSpriteType::Ui },
        { "drowning", PeepActionSpriteType::Drowning },
        { "joy", PeepActionSpriteType::Joy },
        { "wave2", PeepActionSpriteType::Wave2 },
    });

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
                peep->SpriteType = PeepSpriteType::Handyman;
            }
            else if (value == "mechanic" && peep->AssignedStaffType != StaffType::Mechanic)
            {
                peep->AssignedStaffType = StaffType::Mechanic;
                peep->SpriteType = PeepSpriteType::Mechanic;
            }
            else if (value == "security" && peep->AssignedStaffType != StaffType::Security)
            {
                peep->AssignedStaffType = StaffType::Security;
                peep->SpriteType = PeepSpriteType::Security;
            }
            else if (value == "entertainer" && peep->AssignedStaffType != StaffType::Entertainer)
            {
                peep->AssignedStaffType = StaffType::Entertainer;
                peep->SpriteType = PeepSpriteType::EntertainerPanda;
            }

            // Reset state to walking to prevent invalid actions from carrying over
            peep->Action = PeepActionType::Walking;
            peep->ActionSpriteType = peep->NextActionSpriteType = PeepActionSpriteType::None;
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

    static const DukEnumMap<PeepSpriteType> availableHandymanCostumes({
        { "handyman", PeepSpriteType::Handyman },
    });

    static const DukEnumMap<PeepSpriteType> availableMechanicCostumes({
        { "mechanic", PeepSpriteType::Mechanic },
    });

    static const DukEnumMap<PeepSpriteType> availableSecurityCostumes({
        { "security1", PeepSpriteType::Security },
        { "security2", PeepSpriteType::SecurityAlt },
    });

    static const DukEnumMap<PeepSpriteType> availableEntertainerCostumes({
        { "none", PeepSpriteType::Normal },
        { "panda", PeepSpriteType::EntertainerPanda },
        { "tiger", PeepSpriteType::EntertainerTiger },
        { "elephant", PeepSpriteType::EntertainerElephant },
        { "roman", PeepSpriteType::EntertainerRoman },
        { "gorilla", PeepSpriteType::EntertainerGorilla },
        { "snowman", PeepSpriteType::EntertainerSnowman },
        { "knight", PeepSpriteType::EntertainerKnight },
        { "astronaut", PeepSpriteType::EntertainerAstronaut },
        { "bandit", PeepSpriteType::EntertainerBandit },
        { "sheriff", PeepSpriteType::EntertainerSheriff },
        { "pirate", PeepSpriteType::EntertainerPirate },
    });

    static const DukEnumMap<PeepSpriteType>& costumesByStaffType(StaffType staffType)
    {
        switch (staffType)
        {
            case StaffType::Handyman:
                return availableHandymanCostumes;
            case StaffType::Mechanic:
                return availableMechanicCostumes;
            case StaffType::Security:
                return availableSecurityCostumes;
            case StaffType::Entertainer:
            default:
                return availableEntertainerCostumes;
        }
    }

    std::vector<std::string> ScStaff::availableCostumes_get() const
    {
        std::vector<std::string> availableCostumes{};
        auto peep = GetStaff();
        if (peep != nullptr)
        {
            for (auto& costume : costumesByStaffType(peep->AssignedStaffType))
            {
                availableCostumes.push_back(std::string(costume.first));
            }
        }
        return availableCostumes;
    }

    std::string ScStaff::costume_get() const
    {
        auto peep = GetStaff();
        if (peep == nullptr)
        {
            return nullptr;
        }

        auto& availableCostumes = costumesByStaffType(peep->AssignedStaffType);

        auto costume = availableCostumes.find(peep->SpriteType);
        if (costume != availableCostumes.end())
        {
            return std::string(costume->first);
        }
        else
            return nullptr;
    }

    void ScStaff::costume_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();

        auto peep = GetStaff();
        if (peep == nullptr)
        {
            return;
        }

        auto& availableCostumes = costumesByStaffType(peep->AssignedStaffType);

        // Split by type passed so as to not break old plugins
        if (value.type() == DukValue::Type::STRING)
        {
            std::string newCostume = value.as_string();
            auto newSpriteType = availableCostumes.TryGet(newCostume);
            if (newSpriteType != std::nullopt)
            {
                peep->SpriteType = *newSpriteType;
                return;
            }
        }
        else if (value.type() == DukValue::Type::NUMBER)
        {
            auto newSpriteType = PeepSpriteType(value.as_uint() + EnumValue(PeepSpriteType::EntertainerPanda));
            if (availableCostumes.find(newSpriteType) != availableCostumes.end())
            {
                peep->SpriteType = newSpriteType;
                return;
            }
        }

        throw DukException() << "Invalid costume for this staff member";
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

    const DukEnumMap<PeepActionSpriteType>& ScStaff::animationsByStaffType(StaffType staffType) const
    {
        switch (staffType)
        {
            case StaffType::Handyman:
                return availableHandymanAnimations;
            case StaffType::Mechanic:
                return availableMechanicAnimations;
            case StaffType::Security:
                return availableSecurityAnimations;
            case StaffType::Entertainer:
            default:
                return availableEntertainerAnimations;
        }
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

        auto& animationGroup = GetPeepAnimation(peep->SpriteType, *animationType);
        for (auto frameOffset : animationGroup.frame_offsets)
        {
            auto imageId = animationGroup.base_image;
            if (animationType != PeepActionSpriteType::Ui)
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
        std::string_view action = animationGroups[peep->ActionSpriteType];
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

        peep->ActionSpriteType = peep->NextActionSpriteType = *newType;

        auto offset = 0;
        if (peep->IsActionWalking())
            peep->WalkingFrameNum = offset;
        else
            peep->ActionFrame = offset;

        auto& animationGroup = GetPeepAnimation(peep->SpriteType, peep->ActionSpriteType);
        peep->ActionSpriteImageOffset = animationGroup.frame_offsets[offset];
        peep->UpdateSpriteBoundingBox();
    }

    uint8_t ScStaff::animationOffset_get() const
    {
        auto* peep = GetStaff();
        if (peep == nullptr)
        {
            return 0;
        }

        if (peep->IsActionWalking())
            return peep->WalkingFrameNum;
        else
            return peep->ActionFrame;
    }

    void ScStaff::animationOffset_set(uint8_t offset)
    {
        ThrowIfGameStateNotMutable();

        auto* peep = GetStaff();

        auto& animationGroup = GetPeepAnimation(peep->SpriteType, peep->ActionSpriteType);
        auto length = animationGroup.frame_offsets.size();
        offset %= length;

        if (peep->IsActionWalking())
            peep->WalkingFrameNum = offset;
        else
            peep->ActionFrame = offset;

        peep->ActionSpriteImageOffset = animationGroup.frame_offsets[offset];
        peep->UpdateSpriteBoundingBox();
    }

    uint8_t ScStaff::animationLength_get() const
    {
        auto* peep = GetStaff();
        if (peep == nullptr)
        {
            return 0;
        }

        auto& animationGroup = GetPeepAnimation(peep->SpriteType, peep->ActionSpriteType);
        return static_cast<uint8_t>(animationGroup.frame_offsets.size());
    }

    ScHandyman::ScHandyman(EntityId Id)
        : ScStaff(Id)
    {
    }

    void ScHandyman::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScStaff, ScHandyman>(ctx);
        dukglue_register_property(ctx, &ScHandyman::lawnsMown_get, &ScHandyman::lawnsMown_set, "lawnsMown");
        dukglue_register_property(ctx, &ScHandyman::gardensWatered_get, &ScHandyman::gardensWatered_set, "gardensWatered");
        dukglue_register_property(ctx, &ScHandyman::litterSwept_get, &ScHandyman::litterSwept_set, "litterSwept");
        dukglue_register_property(ctx, &ScHandyman::binsEmptied_get, &ScHandyman::binsEmptied_set, "binsEmptied");
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
            duk_push_undefined(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    void ScHandyman::lawnsMown_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetHandyman();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Handyman)
        {
            peep->StaffLawnsMown = value;
        }
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
            duk_push_undefined(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    void ScHandyman::gardensWatered_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetHandyman();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Handyman)
        {
            peep->StaffGardensWatered = value;
        }
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
            duk_push_undefined(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    void ScHandyman::litterSwept_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetHandyman();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Handyman)
        {
            peep->StaffLitterSwept = value;
        }
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
            duk_push_undefined(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    void ScHandyman::binsEmptied_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetHandyman();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Handyman)
        {
            peep->StaffBinsEmptied = value;
        }
    }

    ScMechanic::ScMechanic(EntityId Id)
        : ScStaff(Id)
    {
    }

    void ScMechanic::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScStaff, ScMechanic>(ctx);
        dukglue_register_property(ctx, &ScMechanic::ridesFixed_get, &ScMechanic::ridesFixed_set, "ridesFixed");
        dukglue_register_property(ctx, &ScMechanic::ridesInspected_get, &ScMechanic::ridesInspected_set, "ridesInspected");
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
            duk_push_undefined(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    void ScMechanic::ridesFixed_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetMechanic();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Mechanic)
        {
            peep->StaffRidesFixed = value;
        }
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
            duk_push_undefined(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    void ScMechanic::ridesInspected_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetMechanic();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Mechanic)
        {
            peep->StaffRidesInspected = value;
        }
    }

    ScSecurity::ScSecurity(EntityId Id)
        : ScStaff(Id)
    {
    }

    void ScSecurity::Register(duk_context* ctx)
    {
        dukglue_set_base_class<ScStaff, ScSecurity>(ctx);
        dukglue_register_property(ctx, &ScSecurity::vandalsStopped_get, &ScSecurity::vandalsStopped_set, "vandalsStopped");
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
            duk_push_undefined(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    void ScSecurity::vandalsStopped_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetSecurity();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Security)
        {
            peep->StaffVandalsStopped = value;
        }
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
                for (int32_t y = mapRange.GetTop(); y <= mapRange.GetBottom(); y += COORDS_XY_STEP)
                {
                    for (int32_t x = mapRange.GetLeft(); x <= mapRange.GetRight(); x += COORDS_XY_STEP)
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
