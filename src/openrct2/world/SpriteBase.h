#pragma once

#include "../common.h"

struct CoordsXYZ;

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

    void MoveTo(const CoordsXYZ& newLocation);
    template<typename T> bool Is() const;
    template<typename T> T* As()
    {
        T* result = nullptr;
        if (Is<T>())
        {
            result = reinterpret_cast<T*>(this);
        }
        return result;
    }
};

struct SpriteGeneric : SpriteBase
{
    uint16_t frame;
};
