/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScStaff.hpp"

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
        dukglue_register_property(ctx, &ScStaff::costume_get, &ScStaff::costume_set, "costume");
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

    uint8_t ScStaff::costume_get() const
    {
        auto peep = GetStaff();
        if (peep != nullptr && peep->AssignedStaffType == StaffType::Entertainer)
        {
            return peep->GetCostume();
        }
        return 0;
    }

    void ScStaff::costume_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetStaff();
        if (peep != nullptr)
        {
            peep->SetCostume(value);
        }
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

} // namespace OpenRCT2::Scripting

#endif
