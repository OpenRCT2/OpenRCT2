#pragma once

#include "../common.h"

struct Peep;
struct Litter;
struct Vehicle;
struct Balloon;
struct Duck;
struct JumpingFountain;
struct MoneyEffect;
struct VehicleCrashParticle;
struct CrashSplashParticle;
struct SteamParticle;
struct ExplosionFlare;
struct ExplosionCloud;

struct SpriteBase
{
    uint8_t sprite_identifier;
    uint8_t type;
    uint16_t next_in_quadrant;
    uint16_t next;
    uint16_t previous;
    // Valid values are SPRITE_LINKEDLIST_OFFSET_...
    uint8_t linked_list_index;
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

public:
    template<typename T> bool Is();
    template<typename T> T* As()
    {
        T* result = nullptr;
        if (Is<T>())
        {
            result = reinterpret_cast<T*>(this);
        }
        return result;
    }
    template<> bool Is<Peep>();
    template<> bool Is<Litter>();
    template<> bool Is<Vehicle>();
    template<> bool Is<Balloon>();
    template<> bool Is<Duck>();
    template<> bool Is<JumpingFountain>();
    template<> bool Is<MoneyEffect>();
    template<> bool Is<VehicleCrashParticle>();
    template<> bool Is<CrashSplashParticle>();
    template<> bool Is<SteamParticle>();
    template<> bool Is<ExplosionFlare>();
    template<> bool Is<ExplosionCloud>();
};

struct SpriteGeneric : SpriteBase
{
    uint16_t frame;
};
