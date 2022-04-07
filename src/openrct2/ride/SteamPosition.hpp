/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "../world/Location.hpp"

namespace OpenRCT2::Math::Trigonometry
{
    // 0x00503B6A
    // ROUND(COS((32/64+(L1/64))*(2*PI()))*256,0), ROUND(SIN(((L1/64))*(2*PI())) * 256,0)
    // Where L1 represents an incrementing column 0 - 63
    // Note: Must be at least 32bit to ensure all users do not overflow
    static constexpr CoordsXY yawToDirectionVector[64] = {
        { -256, 0 },    { -255, 25 },   { -251, 50 },   { -245, 74 },   { -237, 98 },   { -226, 121 },  { -213, 142 },
        { -198, 162 },  { -181, 181 },  { -162, 198 },  { -142, 213 },  { -121, 226 },  { -98, 237 },   { -74, 245 },
        { -50, 251 },   { -25, 255 },   { 0, 256 },     { 25, 255 },    { 50, 251 },    { 74, 245 },    { 98, 237 },
        { 121, 226 },   { 142, 213 },   { 162, 198 },   { 181, 181 },   { 198, 162 },   { 213, 142 },   { 226, 121 },
        { 237, 98 },    { 245, 74 },    { 251, 50 },    { 255, 25 },    { 256, 0 },     { 255, -25 },   { 251, -50 },
        { 245, -74 },   { 237, -98 },   { 226, -121 },  { 213, -142 },  { 198, -162 },  { 181, -181 },  { 162, -198 },
        { 142, -213 },  { 121, -226 },  { 98, -237 },   { 74, -245 },   { 50, -251 },   { 25, -255 },   { 0, -256 },
        { -25, -255 },  { -50, -251 },  { -74, -245 },  { -98, -237 },  { -121, -226 }, { -142, -213 }, { -162, -198 },
        { -181, -181 }, { -198, -162 }, { -213, -142 }, { -226, -121 }, { -237, -98 },  { -245, -74 },  { -251, -50 },
        { -255, -25 },
    };

    // 0x00503B50
    // -SIN((Y1/360)*2*PI())*256
    // Where Y1 represents the angle of pitch in degrees
    constexpr int16_t pitchSin[] = {
        0,                               // flat
        -49,  -97,  -165, -217,          // slopes up
        49,   97,   165,  217,           // slopes down
        -246, -256,                      // slopes vertical up
        -247, -221, -181, -128, -66,  0, // slopes looping up
        246,  256,                       // slopes vertical down
        247,  221,  181,  128,  66,      // slopes looping down
        -128, -221, -256, -221, -128,    // corkscrew up left
        128,  221,  256,  221,  128,     // corkscrew down left
        -128, -221, -256, -221, -128,    // corkscrew up right
        128,  221,  256,  221,  128,     // corkscrew down right
        -13,  -22,  13,   22,            // half helixes
        -26,  26,                        // quarter helixes
        -36,  -71,  -193,                // diagonal slopes up
        36,   71,   193,                 // diagonal slopes down
        97,   165,  217,                 // inverting transition slopes down
        -44                              // spiral lift hill up
    };

    // COS((Y1/360)*2*PI())*256
    // Where Y1 represents the angle of pitch in degrees
    constexpr int16_t pitchCos[] = {
        256,                                // Flat
        251,  236,  195,  134,              // slopes up
        251,  236,  195,  134,              // slopes down
        70,   0,                            // slopes vertical up
        -66,  -128, -181, -221, -247, -256, // slopes looping up
        70,   0,                            // slopes vertical down
        -66,  -128, -181, -221, -247,       // slopes looping down
        221,  128,  0,    -128, -221,       // corkscrew up left
        -221, -128, 0,    128,  221,        // corkscrew down left
        221,  128,  0,    -128, -221,       // corkscrew up right
        -221, -128, 0,    128,  221,        // corkscrew down right
        255,  255,  255,  255,              // half helixes
        254,  254,                          // quarter helixes
        253,  245,  168,                    // diagonal slopes up
        253,  245,  168,                    // diagonal slopes down
        236,  195,  134,                    // slopes up
        252                                 // spiral lift hill up
    };

    constexpr auto computeXYMagnitude(int32_t height, uint8_t pitch)
    {
        return (pitchSin[static_cast<uint8_t>(pitch)] * height) / 256;
    }

    constexpr CoordsXY computeXYVector(int32_t magnitude, uint8_t yaw)
    {
        return (yawToDirectionVector[yaw] * magnitude) / 256;
    }
    constexpr CoordsXY computeXYVector(int32_t height, uint8_t pitch, uint8_t yaw)
    {
        return computeXYVector(computeXYMagnitude(height, pitch), yaw);
    }

} // namespace OpenRCT2::Math::Trigonometry
