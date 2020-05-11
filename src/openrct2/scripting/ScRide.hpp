/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../common.h"
#    include "../ride/Ride.h"
#    include "Duktape.hpp"
#    include "ScObject.hpp"
#    include "ScriptEngine.h"

namespace OpenRCT2::Scripting
{
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
        int32_t id_get() const
        {
            return _rideId;
        }

        std::shared_ptr<ScRideObject> object_get()
        {
            auto ride = GetRide();
            if (ride != nullptr)
            {
                auto rideObject = GetContext()->GetObjectManager().GetLoadedObject(OBJECT_TYPE_RIDE, ride->subtype);
                if (rideObject != nullptr)
                {
                    return std::make_shared<ScRideObject>(OBJECT_TYPE_RIDE, ride->subtype);
                }
            }
            return nullptr;
        }

        int32_t type_get() const
        {
            auto ride = GetRide();
            return ride != nullptr ? ride->type : 0;
        }

        std::string name_get() const
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

        int32_t excitement_get() const
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

        int32_t intensity_get() const
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

        int32_t nausea_get() const
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

        int32_t totalCustomers_get() const
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

        Ride* GetRide() const
        {
            return get_ride(_rideId);
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScRide::id_get, nullptr, "PeepId");
            dukglue_register_property(ctx, &ScRide::object_get, nullptr, "object");
            dukglue_register_property(ctx, &ScRide::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScRide::name_get, &ScRide::name_set, "Name");
            dukglue_register_property(ctx, &ScRide::excitement_get, &ScRide::excitement_set, "excitement");
            dukglue_register_property(ctx, &ScRide::intensity_get, &ScRide::intensity_set, "intensity");
            dukglue_register_property(ctx, &ScRide::nausea_get, &ScRide::nausea_set, "nausea");
            dukglue_register_property(ctx, &ScRide::totalCustomers_get, &ScRide::totalCustomers_set, "totalCustomers");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
