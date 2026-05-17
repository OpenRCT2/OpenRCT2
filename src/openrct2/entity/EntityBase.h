#pragma once

#include "../Identifiers.h"
#include "../world/Location.hpp"

namespace OpenRCT2
{
    class DataSerialiser;

    enum class EntityType : uint8_t
    {
        vehicle,
        guest,
        staff,
        litter,
        steamParticle,
        moneyEffect,
        crashedVehicleParticle,
        explosionCloud,
        crashSplash,
        explosionFlare,
        jumpingFountain,
        balloon,
        duck,
        count,
        null = 255
    };

    struct EntitySpriteData
    {
        // Width from centre of sprite to edge
        uint8_t width;
        // Height from centre of sprite to bottom
        uint8_t heightMin;
        // Height from centre of sprite to top
        uint8_t heightMax;
        // Screen Coordinates of sprite
        ScreenRect spriteRect;
    };

    struct EntityBase
    {
        EntityType type;
        EntityId id;
        int32_t x;
        int32_t y;
        int32_t z;
        EntitySpriteData spriteData;
        // Used as direction or rotation depending on the entity.
        uint8_t orientation;
        uint32_t spatialIndex;

        /**
         * Moves a sprite to a new location, invalidates the current position if valid
         * and also the new position.
         *
         *  rct2: 0x0069E9D3
         */
        void moveTo(const CoordsXYZ& newLocation);

        void moveToAndUpdateSpatialIndex(const CoordsXYZ& newLocation);

        /**
         * Sets the entity location without screen invalidation.
         */
        void setLocation(const CoordsXYZ& newLocation);

        /**
         * Gets the entity current location.
         */
        CoordsXYZ getLocation() const;

        void invalidate();
        template<typename T>
        bool is() const;
        template<typename T>
        T* as()
        {
            return is<T>() ? reinterpret_cast<T*>(this) : nullptr;
        }
        template<typename T>
        const T* as() const
        {
            return is<T>() ? reinterpret_cast<const T*>(this) : nullptr;
        }

        template<typename T>
        T* cast()
        {
            return reinterpret_cast<T*>(this);
        }

        template<typename T>
        const T* cast() const
        {
            return reinterpret_cast<const T*>(this);
        }

        void serialise(class DataSerialiser& stream);

        void paint() const;
    };
} // namespace OpenRCT2
