#pragma once

#include "../common.h"

struct CoordsXYZ;

enum class EntityType : uint8_t
{
    Vehicle,
    Guest,
    Staff,
    Litter,
    SteamParticle,
    MoneyEffect,
    CrashedVehicleParticle,
    ExplosionCloud,
    CrashSplash,
    ExplosionFlare,
    JumpingFountain,
    Balloon,
    Duck,
    Count,
    Null = 255
};

struct SpriteBase
{
    EntityType Type;
    // Height from centre of sprite to bottom
    uint8_t sprite_height_negative;
    uint16_t sprite_index;
    uint16_t flags;
    int16_t x;
    int16_t y;
    int16_t z;
    // Width from centre of sprite to edge
    uint8_t sprite_width;
    // Height from centre of sprite to top
    uint8_t sprite_height_positive;
    // Screen Coordinates of sprite
    int16_t sprite_left;
    int16_t sprite_top;
    int16_t sprite_right;
    int16_t sprite_bottom;

    uint8_t sprite_direction;

    /**
     * Moves a sprite to a new location, invalidates the current position if valid
     * and also the new position.
     *
     *  rct2: 0x0069E9D3
     */
    void MoveTo(const CoordsXYZ& newLocation);

    /**
     * Sets the entity location without screen invalidation.
     */
    void SetLocation(const CoordsXYZ& newLocation);

    /**
     * Gets the entity current location.
     */
    CoordsXYZ GetLocation() const;

    void Invalidate();
    template<typename T> bool Is() const;
    template<typename T> T* As()
    {
        return Is<T>() ? reinterpret_cast<T*>(this) : nullptr;
    }
    template<typename T> const T* As() const
    {
        return Is<T>() ? reinterpret_cast<const T*>(this) : nullptr;
    }
};

struct MiscEntity : SpriteBase
{
    uint16_t frame;
};
