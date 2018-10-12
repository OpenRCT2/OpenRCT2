#include <cstdint>

#pragma once

// Based on SDL2

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__

static inline uint16_t ORCT_Swapu16(uint16_t x)
{
    return static_cast<uint16_t>((x << 8) | (x >> 8));
}

static inline uint32_t ORCT_Swapu32(uint32_t x)
{
    return static_cast<uint32_t>(((x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24)));
}

static inline uint64_t ORCT_Swapu64(uint64_t x)
{
    uint32_t hi, lo;

    /* Separate into high and low 32-bit values and swap them */
    lo = static_cast<uint32_t>(x & 0xFFFFFFFF);
    x >>= 32;
    hi = static_cast<uint32_t>(x & 0xFFFFFFFF);
    x = ORCT_Swapu32(lo);
    x <<= 32;
    x |= ORCT_Swapu32(hi);
    return (x);
}

static inline int16_t ORCT_Swapi16(int16_t x)
{
    return static_cast<uint16_t>((x << 8) | (x >> 8));
}

static inline int32_t ORCT_Swapi32(int32_t x)
{
    return static_cast<uint32_t>(((x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24)));
}

static inline int64_t ORCT_Swapi64(int64_t x)
{
    uint32_t hi, lo;

    /* Separate into high and low 32-bit values and swap them */
    lo = static_cast<uint32_t>(x & 0xFFFFFFFF);
    x >>= 32;
    hi = static_cast<uint32_t>(x & 0xFFFFFFFF);
    x = ORCT_Swapu32(lo);
    x <<= 32;
    x |= ORCT_Swapu32(hi);
    return (x);
}

#    define ORCT_SwapLEi16(X) ORCT_Swapi16(X)
#    define ORCT_SwapLEi32(X) ORCT_Swapi32(X)
#    define ORCT_SwapLEi64(X) ORCT_Swapi64(X)
#    define ORCT_SwapLEu16(X) ORCT_Swapu16(X)
#    define ORCT_SwapLEu32(X) ORCT_Swapu32(X)
#    define ORCT_SwapLEu64(X) ORCT_Swapu64(X)
#else
#    define ORCT_SwapLEi16(X) (X)
#    define ORCT_SwapLEi32(X) (X)
#    define ORCT_SwapLEi64(X) (X)
#    define ORCT_SwapLEu16(X) (X)
#    define ORCT_SwapLEu32(X) (X)
#    define ORCT_SwapLEu64(X) (X)
#endif
