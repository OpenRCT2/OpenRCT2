/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../ride/Ride.h"
#    include "ScEntity.hpp"

#    include <optional>

namespace OpenRCT2::Scripting
{
    class ScVehicle : public ScEntity
    {
    public:
        ScVehicle(EntityId id);

        static void Register(duk_context* ctx);

    private:
        Vehicle* GetVehicle() const;

        ObjectEntryIndex rideObject_get() const;
        void rideObject_set(ObjectEntryIndex value);

        uint8_t vehicleObject_get() const;
        void vehicleObject_set(uint8_t value);

        uint8_t spriteType_get() const;
        void spriteType_set(uint8_t value);

        int32_t ride_get() const;
        void ride_set(int32_t value);

        uint8_t numSeats_get() const;
        void numSeats_set(uint8_t value);

        DukValue nextCarOnTrain_get() const;
        void nextCarOnTrain_set(DukValue value);

        DukValue previousCarOnRide_get() const;
        void previousCarOnRide_set(DukValue value);

        DukValue nextCarOnRide_get() const;
        void nextCarOnRide_set(DukValue value);

        StationIndex::UnderlyingType currentStation_get() const;
        void currentStation_set(StationIndex::UnderlyingType value);

        uint16_t mass_get() const;
        void mass_set(uint16_t value);

        int32_t acceleration_get() const;
        void acceleration_set(int32_t value);

        int32_t velocity_get() const;
        void velocity_set(int32_t value);

        uint8_t bankRotation_get() const;
        void bankRotation_set(uint8_t value);

        DukValue colours_get() const;
        void colours_set(const DukValue& value);

        DukValue trackLocation_get() const;
        void trackLocation_set(const DukValue& value);

        uint16_t trackProgress_get() const;

        int32_t remainingDistance_get() const;

        uint8_t subposition_get() const;

        uint8_t poweredAcceleration_get() const;
        void poweredAcceleration_set(uint8_t value);

        uint8_t poweredMaxSpeed_get() const;
        void poweredMaxSpeed_set(uint8_t value);

        std::string status_get() const;
        void status_set(const std::string& value);

        std::vector<DukValue> guests_get() const;

        DukValue gForces_get() const;

        void travelBy(int32_t value);
    };

} // namespace OpenRCT2::Scripting

#endif
