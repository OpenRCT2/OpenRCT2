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

    /**
     * The cos and sin of sprite direction
     * ROUND(COS((32/64+(L1/64))*(2*PI()))*256,0), ROUND(SIN(((L1/64))*(2*PI())) * 256,0)
     * Where L1 represents an incrementing column 0 - 63
     * Note: Must be at least 32bit to ensure all users do not overflow
     */
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

    /**
     * The cos and sin of vehicle pitch
     * X represents Z  offset and Y represents XY magnitude
     * COS((Y1/360)*2*PI())*256,-SIN((Y1/360)*2*PI())*256
     * Where Y1 represents the angle of pitch in degrees
     * Using physical pitch calculated by gravity
     */
    constexpr CoordsXY pitchOffsetsFromPhysics[60] = {
        { 256, 0 },     // flat
        { 251, -49 },   // slopes up
        { 236, -97 },   // slopes up
        { 195, -165 },  // slopes up
        { 134, -217 },  // slopes up
        { 251, 49 },    // slopes down
        { 236, 97 },    // slopes down
        { 195, 165 },   // slopes down
        { 135, 217 },   // slopes down
        { 70, -246 },   // slopes vertical up
        { 0, -256 },    // slopes vertical up
        { -66, -247 },  // slopes looping up
        { -128, -221 }, // slopes looping up
        { -181, -181 }, // slopes looping up
        { -221, -128 }, // slopes looping up
        { -247, -66 },  // slopes looping up
        { -256, 0 },    // inverted
        { 70, 246 },    // slopes vertical down
        { 0, 256 },     // slopes vertical down
        { -66, 247 },   // slopes looping down
        { -128, 221 },  // slopes looping down
        { -181, 181 },  // slopes looping down
        { -221, 128 },  // slopes looping down
        { -247, 66 },   // slopes looping down
        { 221, -128 },  // corkscrew up left
        { 128, -221 },  // corkscrew up left
        { 0, -256 },    // corkscrew up left
        { -128, -221 }, // corkscrew up left
        { -221, -128 }, // corkscrew up left
        { -221, 128 },  // corkscrew down left
        { -128, 221 },  // corkscrew down left
        { 0, 256 },     // corkscrew down left
        { 128, 221 },   // corkscrew down left
        { 221, 128 },   // corkscrew down left
        { 221, -128 },  // corkscrew up right
        { 128, -221 },  // corkscrew up right
        { 0, -256 },    // corkscrew up right
        { -128, -221 }, // corkscrew up right
        { -221, -128 }, // corkscrew up right
        { -221, 128 },  // corkscrew down right
        { -128, 221 },  // corkscrew down right
        { 0, 256 },     // corkscrew down right
        { 128, 221 },   // corkscrew down right
        { 221, 128 },   // corkscrew down right
        { 255, -13 },   // half helixes
        { 255, -22 },   // half helixes
        { 255, 13 },    // half helixes
        { 255, 22 },    // half helixes
        { 254, -26 },   // quarter helixes
        { 254, 26 },    // quarter helixes
        { 253, -36 },   // diagonal slopes up
        { 245, -71 },   // diagonal slopes up
        { 168, -193 },  // diagonal slopes up
        { 254, 36 },    // diagonal slopes down
        { 245, 71 },    // diagonal slopes down
        { 168, 193 },   // diagonal slopes down
        { 236, 97 },    // inverting transition slopes down
        { 195, 165 },   // inverting transition slopes down
        { 134, 217 },   // inverting transition slopes down
        { 252, -44 }    // spiral lift hill up
    };

    /**
     * The cos and sin of vehicle pitch from
     * X represents Z  offset and Y represents XY magnitude
     * COS((Y1/360)*2*PI())*256,-SIN((Y1/360)*2*PI())*256
     * Where Y1 represents the angle of pitch in degrees
     * Calculated using the pitch of the sprites represented by the angle
     */
    constexpr CoordsXY pitchOffsetsFromGeometry[60] = {
        { 256, 0 },     // flat
        { 251, -49 },   // slopes up
        { 236, -97 },   // slopes up
        { 195, -165 },  // slopes up
        { 134, -217 },  // slopes up
        { 251, 49 },    // slopes down
        { 236, 97 },    // slopes down
        { 195, 165 },   // slopes down
        { 135, 217 },   // slopes down
        { 70, -246 },   // slopes vertical up
        { 0, -256 },    // slopes vertical up
        { -66, -247 },  // slopes looping up
        { -128, -221 }, // slopes looping up
        { -181, -181 }, // slopes looping up
        { -221, -128 }, // slopes looping up
        { -247, -66 },  // slopes looping up
        { -256, 0 },    // inverted
        { 70, 246 },    // slopes vertical down
        { 0, 256 },     // slopes vertical down
        { -66, 247 },   // slopes looping down
        { -128, 221 },  // slopes looping down
        { -181, 181 },  // slopes looping down
        { -221, 128 },  // slopes looping down
        { -247, 66 },   // slopes looping down
        { 221, -128 },  // corkscrew up left
        { 128, -221 },  // corkscrew up left
        { 0, -256 },    // corkscrew up left
        { -128, -221 }, // corkscrew up left
        { -221, -128 }, // corkscrew up left
        { -221, 128 },  // corkscrew down left
        { -128, 221 },  // corkscrew down left
        { 0, 256 },     // corkscrew down left
        { 128, 221 },   // corkscrew down left
        { 221, 128 },   // corkscrew down left
        { 221, -128 },  // corkscrew up right
        { 128, -221 },  // corkscrew up right
        { 0, -256 },    // corkscrew up right
        { -128, -221 }, // corkscrew up right
        { -221, -128 }, // corkscrew up right
        { -221, 128 },  // corkscrew down right
        { -128, 221 },  // corkscrew down right
        { 0, 256 },     // corkscrew down right
        { 128, 221 },   // corkscrew down right
        { 221, 128 },   // corkscrew down right
        { 256, 0 },     // half helixes
        { 256, 0 },     // half helixes
        { 256, 0 },     // half helixes
        { 256, 0 },     // half helixes
        { 256, 0 },     // quarter helixes
        { 256, 0 },     // quarter helixes
        { 252, -42 },   // diagonal slopes up
        { 241, -83 },   // diagonal slopes up
        { 168, -193 },  // diagonal slopes up
        { 252, 42 },    // diagonal slopes down
        { 241, 83 },    // diagonal slopes down
        { 168, 193 },   // diagonal slopes down
        { 236, 97 },    // inverting transition slopes down
        { 195, 165 },   // inverting transition slopes down
        { 134, 217 },   // inverting transition slopes down
        { 252, -44 }    // spiral lift hill up // TODO: calculate this
    };

    constexpr auto computeXYMagnitude(int32_t length, uint8_t pitch)
    {
        return (pitchOffsetsFromPhysics[static_cast<uint8_t>(pitch)].y * length) / 256;
    }

    constexpr CoordsXY computeXYVector(int32_t magnitude, uint8_t yaw)
    {
        return (static_cast<CoordsXY>(yawToDirectionVector[yaw]) * magnitude) / 256;
    }
    constexpr CoordsXY computeXYVector(int32_t length, uint8_t pitch, uint8_t yaw)
    {
        return computeXYVector(computeXYMagnitude(length, pitch), yaw);
    }
    constexpr CoordsXYZ computeXYZVector(int32_t height, int32_t length, uint8_t pitch, uint8_t yaw)
    {
        auto offsets = pitchOffsetsFromGeometry[pitch];
        int32_t projectedRun = (offsets.x * length + offsets.y * height) / 256;
        int32_t projectedHeight = (offsets.x * height - offsets.y * length) / 256;
        return { computeXYVector(projectedRun, yaw), projectedHeight };
    }

} // namespace OpenRCT2::Math::Trigonometry
