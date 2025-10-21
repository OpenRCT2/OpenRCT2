/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "VehicleGeometry.h"

#include <cstddef>

namespace OpenRCT2::RideVehicle::Geometry
{

    /** rct2: 0x009A2930 */
    constexpr auto kSubpositionTranslationDistances = std::to_array({
        // For a base length of 8716 (0x220C) on the horizontal and 6554 (0x199A) on the vertical,
        // use the Pythagoras theorem and round up.
        0,     // no movement
        8716,  // X translation
        8716,  // Y translation
        12327, // XY translation
        6554,  // Z translation
        10905, // XZ translation
        10905, // YZ translation
        13961, // XYZ translation
        // For the reverser car, multiply the horizontal distance by 2.5 and the vertical distance by 4.072.
        0,     // no movement
        21790, // X translation
        21790, // Y translation
        30817, // Z translation
        16385, // XY translation
        27262, // XZ translation
        27262, // YZ translation
        34902, // XYZ translation
    });
    static_assert(std::size(kSubpositionTranslationDistances) == 16);

    /** rct2: 0x009A36C4 */
    constexpr auto kFreeroamVehicleMovementData = std::to_array<Unk9A36C4Struct>({
        { -1, 0, 8716 }, { -1, 0, 8716 },   { -1, 0, 8716 },  { -1, 1, 12327 },  { -1, 1, 12327 },  { -1, 1, 12327 },
        { 0, 1, 8716 },  { -1, 1, 12327 },  { 0, 1, 8716 },   { 0, 1, 8716 },    { 0, 1, 8716 },    { 1, 1, 12327 },
        { 1, 1, 12327 }, { 1, 1, 12327 },   { 1, 0, 8716 },   { 1, 1, 12327 },   { 1, 0, 8716 },    { 1, 0, 8716 },
        { 1, 0, 8716 },  { 1, -1, 12327 },  { 1, -1, 12327 }, { 1, -1, 12327 },  { 0, -1, 8716 },   { 1, -1, 12327 },
        { 0, -1, 8716 }, { 0, -1, 8716 },   { 0, -1, 8716 },  { -1, -1, 12327 }, { -1, -1, 12327 }, { -1, -1, 12327 },
        { -1, 0, 8716 }, { -1, -1, 12327 },
    });
    static_assert(std::size(kFreeroamVehicleMovementData) == 32);

    /** rct2: 0x009A2970 */
    constexpr auto kAccelerationFromPitch = std::to_array<int32_t>({
        0,       // Flat
        -124548, // 1 Slope Up 12.5
        -243318, // 2 Slope Up 25
        -416016, // 3 Slope Up 42.5
        -546342, // 4 Slope Up 60
        124548,  // 5 Slope Down 12.5
        243318,  // 6 Slope Down 25
        416016,  // 7 Slope Down 42.5
        546342,  // 8 Slope Down 60
        -617604, // 9 Slope Up 75
        -642000, // 10 Slope Up 90
        -620172, // 11 Slope Up 105
        -555972, // 12 Slope Up 120
        -453894, // 13 Slope Up 135
        -321000, // 14 Slope Up 150
        -166278, // 15 Slope Up 165
        0,       // 16 Fully Inverted
        617604,  // 17 Slope Down 75
        642000,  // 18 Slope Down 90
        620172,  // 19 Slope Down 105
        555972,  // 20 Slope Down 120
        453894,  // 21 Slope Down 135
        321000,  // 22 Slope Down 150
        166278,  // 23 Slope Down 165
        -321000, // 24 Corkscrew Right Up 0
        -555972, // 25 Corkscrew Right Up 1
        -642000, // 26 Corkscrew Right Up 2
        -555972, // 27 Corkscrew Right Up 3
        -321000, // 28 Corkscrew Right Up 4
        321000,  // 29 Corkscrew Right Down 4
        555972,  // 30 Corkscrew Right Down 3
        642000,  // 31 Corkscrew Right Down 2
        555972,  // 32 Corkscrew Right Down 1
        321000,  // 33 Corkscrew Right Down 0
        -321000, // 34 Corkscrew Left Up 0
        -555972, // 35 Corkscrew Left Up 1
        -642000, // 36 Corkscrew Left Up 2
        -555972, // 37 Corkscrew Left Up 3
        -321000, // 38 Corkscrew Left Up 4
        321000,  // 39 Corkscrew Left Down 4
        555972,  // 40 Corkscrew Left Down 2
        642000,  // 41 Corkscrew Left Down 1
        555972,  // 42 Corkscrew Left Down 1
        321000,  // 43 Corkscrew Left Down 0
        -33384,  // 44 Half Helix Up Large
        -55854,  // 45 Half Helix Up Small
        33384,   // 46 Half Helix Down Large
        55854,   // 47 Half Helix Down Small
        -66768,  // 48 Quarter Helix Up
        66768,   // 49 Quarter Helix Down
        -90522,  // 50 Slope Up 8
        -179760, // 51 Slope Down 16
        -484068, // 52 Slope Up 50
        90522,   // 53 Slope Down 8
        179760,  // 54 Slope Down 16
        484068,  // 55 Slope Down 50
        243318,  // 56 Inverting Loop Down 25
        416016,  // 57 Inverting Loop Down 42.5
        546342,  // 58 Inverting Loop Down 60
        -110424, // 59 Slope Up Spiral Lift Hill
        110424,  // 60 Slope Down Spiral Lift Hill
    });
    static_assert(std::size(kAccelerationFromPitch) == EnumValue(VehiclePitch::pitchCount));

    /** rct2: 0x009A37E4 */
    constexpr auto kPitchHorizontalComponent = std::to_array<int32_t>({
        2147483647,  2106585154,  1985590284,  1636362342,  1127484953,  2106585154,  1985590284,  1636362342,  1127484953,
        58579923,    0,           -555809667,  -1073741824, -1518500249, -1859775391, -2074309916, -2147483647, 58579923,
        0,           -555809667,  -1073741824, -1518500249, -1859775391, -2074309916, 1859775393,  1073741824,  0,
        -1073741824, -1859775393, 1859775393,  1073741824,  0,           -1073741824, -1859775393, 1859775393,  1073741824,
        0,           -1073741824, -1859775393, 1859775393,  1073741824,  0,           -1073741824, -1859775393, 2144540595,
        2139311823,  2144540595,  2139311823,  2135719507,  2135719507,  2125953864,  2061796213,  1411702590,  2125953864,
        2061796213,  1411702590,  1985590284,  1636362342,  1127484953,  2115506168,  2115506168,
    });
    static_assert(std::size(kPitchHorizontalComponent) == EnumValue(VehiclePitch::pitchCount));

    /** rct2: 0x009A38D4 */
    constexpr auto kPitchVerticalComponent = std::to_array<int32_t>({
        0,           417115092,   817995863,   1390684831,  1827693544,  -417115092,  -817995863,  -1390684831, -1827693544,
        2066040965,  2147483647,  2074309916,  1859775393,  1518500249,  1073741824,  555809666,   0,           -2066040965,
        -2147483647, -2074309916, -1859775393, -1518500249, -1073741824, -555809666,  1073741824,  1859775393,  2147483647,
        1859775393,  1073741824,  -1073741824, -1859775393, -2147483647, -1859775393, -1073741824, 1073741824,  1859775393,
        2147483647,  1859775393,  1073741824,  -1073741824, -1859775393, -2147483647, -1859775393, -1073741824, 112390610,
        187165532,   -112390610,  -187165532,  224473165,   -224473165,  303325208,   600568389,   1618265062,  -303325208,
        -600568389,  -1618265062, -817995863,  -1390684831, -1827693544, 369214930,   -369214930,
    });
    static_assert(std::size(kPitchVerticalComponent) == EnumValue(VehiclePitch::pitchCount));

} // namespace OpenRCT2::RideVehicle::Geometry
