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
