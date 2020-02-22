/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../object/RideObject.h"
#include "../ride/Ride.h"
#include "Duktape.hpp"

namespace OpenRCT2::Scripting
{
    class ScRideObject
    {
    private:
        rct_ride_entry* _entry;

    public:
        ScRideObject(rct_ride_entry* entry)
        {
            _entry = entry;
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScRideObject::id_get, nullptr, "identifier");
            dukglue_register_property(ctx, &ScRideObject::name_get, nullptr, "name");
            dukglue_register_property(ctx, &ScRideObject::description_get, nullptr, "description");
            dukglue_register_property(ctx, &ScRideObject::capacity_get, nullptr, "capacity");
        }

    private:
        std::string id_get()
        {
            auto obj = static_cast<Object*>(_entry->obj);
            if (obj != nullptr)
            {
                return obj->GetIdentifier();
            }
            return {};
        }

        std::string name_get()
        {
            auto obj = static_cast<Object*>(_entry->obj);
            if (obj != nullptr)
            {
                return obj->GetName();
            }
            return {};
        }

        std::string description_get()
        {
            auto obj = static_cast<RideObject*>(_entry->obj);
            if (obj != nullptr)
            {
                return obj->GetDescription();
            }
            return {};
        }

        std::string capacity_get()
        {
            auto obj = static_cast<RideObject*>(_entry->obj);
            if (obj != nullptr)
            {
                return obj->GetCapacity();
            }
            return {};
        }
    };

    class ScRide
    {
    private:
        Ride* _ride;

    public:
        ScRide(Ride* ride)
            : _ride(ride)
        {
        }

        int32_t id_get()
        {
            return _ride->id;
        }

        std::shared_ptr<ScRideObject> object_get()
        {
            auto rideEntry = _ride->GetRideEntry();
            if (rideEntry != nullptr)
            {
                return std::make_shared<ScRideObject>(rideEntry);
            }
            return nullptr;
        }

        int32_t type_get()
        {
            return _ride->type;
        }

        std::string name_get()
        {
            return _ride->GetName();
        }
        void name_set(std::string value)
        {
            _ride->custom_name = value;
        }

        int32_t excitement_get()
        {
            return _ride->excitement;
        }
        void excitement_set(int32_t value)
        {
            _ride->excitement = value;
        }

        int32_t intensity_get()
        {
            return _ride->intensity;
        }
        void intensity_set(int32_t value)
        {
            _ride->intensity = value;
        }

        int32_t nausea_get()
        {
            return _ride->nausea;
        }
        void nausea_set(int32_t value)
        {
            _ride->nausea = value;
        }

        int32_t totalCustomers_get()
        {
            return _ride->total_customers;
        }
        void totalCustomers_set(int32_t value)
        {
            _ride->total_customers = value;
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScRide::id_get, nullptr, "id");
            dukglue_register_property(ctx, &ScRide::object_get, nullptr, "object");
            dukglue_register_property(ctx, &ScRide::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScRide::name_get, &ScRide::name_set, "name");
            dukglue_register_property(ctx, &ScRide::excitement_get, &ScRide::excitement_set, "excitement");
            dukglue_register_property(ctx, &ScRide::intensity_get, &ScRide::intensity_set, "intensity");
            dukglue_register_property(ctx, &ScRide::nausea_get, &ScRide::nausea_set, "nausea");
            dukglue_register_property(ctx, &ScRide::totalCustomers_get, &ScRide::totalCustomers_set, "totalCustomers");
        }
    };
} // namespace OpenRCT2::Scripting
