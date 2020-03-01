/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __ENABLE_SCRIPTING__

#    include "../common.h"
#    include "../object/RideObject.h"
#    include "../ride/Ride.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScRideObject
    {
    private:
        rct_ride_entry* _entry{};

    public:
        ScRideObject(rct_ride_entry* entry)
            : _entry(entry)
        {
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
        ride_id_t _rideId = RIDE_ID_NULL;

    public:
        ScRide(ride_id_t rideId)
            : _rideId(rideId)
        {
        }

    private:
        int32_t id_get()
        {
            return _rideId;
        }

        std::shared_ptr<ScRideObject> object_get()
        {
            auto ride = GetRide();
            if (ride != nullptr)
            {
                auto rideEntry = ride->GetRideEntry();
                if (rideEntry != nullptr)
                {
                    return std::make_shared<ScRideObject>(rideEntry);
                }
            }
            return nullptr;
        }

        int32_t type_get()
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->type : 0;
        }

        std::string name_get()
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->GetName() : std::string();
        }
        void name_set(std::string value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->custom_name = value;
            }
        }

        int32_t excitement_get()
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->excitement : 0;
        }
        void excitement_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->excitement = value;
            }
        }

        int32_t intensity_get()
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->intensity : 0;
        }
        void intensity_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->intensity = value;
            }
        }

        int32_t nausea_get()
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->nausea : 0;
        }
        void nausea_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->nausea = value;
            }
        }

        int32_t totalCustomers_get()
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->total_customers : 0;
        }
        void totalCustomers_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto ride = GetRide();
            if (ride != nullptr)
            {
                ride->total_customers = value;
            }
        }

        Ride* GetRide()
        {
            return get_ride(_rideId);
        }

    public:
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

#endif
