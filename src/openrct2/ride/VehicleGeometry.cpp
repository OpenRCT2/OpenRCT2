/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

    // clang-format off
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
    // clang-format on
    static_assert(std::size(kSubpositionTranslationDistances) == 16);

    /** rct2: 0x009A36C4 */
    const auto kFreeroamVehicleMovementData = std::to_array<Unk9A36C4Struct>({
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

    /** rct2: 0x009A37E4
     *  rct2: 0x009A38D4
     */
    constexpr auto kPitchToDirectionVectorInt32 = std::to_array<CoordsXY>({
        { 2147483647, 0 },            // flat
        { 2106585154, 417115092 },    // up12
        { 1985590284, 817995863 },    // up25
        { 1636362342, 1390684831 },   // up42
        { 1127484953, 1827693544 },   // up60
        { 2106585154, -417115092 },   // down12
        { 1985590284, -817995863 },   // down25
        { 1636362342, -1390684831 },  // down42
        { 1127484953, -1827693544 },  // down60
        { 58579923, 2066040965 },     // up75
        { 0, 2147483647 },            // up90
        { -555809667, 2074309916 },   // up115
        { -1073741824, 1859775393 },  // up120
        { -1518500249, 1518500249 },  // up135
        { -1859775391, 1073741824 },  // up150
        { -2074309916, 555809666 },   // up165
        { -2147483647, 0 },           // inverted
        { 58579923, -2066040965 },    // down75
        { 0, -2147483647 },           // down90
        { -555809667, -2074309916 },  // down115
        { -1073741824, -1859775393 }, // down120
        { -1518500249, -1518500249 }, // down135
        { -1859775391, -1073741824 }, // down150
        { -2074309916, -555809666 },  // down165
        { 1859775393, 1073741824 },   // corkscrewUpRight0
        { 1073741824, 1859775393 },   // corkscrewUpRight1
        { 0, 2147483647 },            // corkscrewUpRight2
        { -1073741824, 1859775393 },  // corkscrewUpRight3
        { -1859775393, 1073741824 },  // corkscrewUpRight4
        { 1859775393, -1073741824 },  // corkscrewDownLeft0
        { 1073741824, -1859775393 },  // corkscrewDownLeft1
        { 0, -2147483647 },           // corkscrewDownLeft2
        { -1073741824, -1859775393 }, // corkscrewDownLeft3
        { -1859775393, -1073741824 }, // corkscrewDownLeft4
        { 1859775393, 1073741824 },   // corkscrewUpLeft0
        { 1073741824, 1859775393 },   // corkscrewUpLeft1
        { 0, 2147483647 },            // corkscrewUpLeft2
        { -1073741824, 1859775393 },  // corkscrewUpLeft3
        { -1859775393, 1073741824 },  // corkscrewUpLeft4
        { 1859775393, -1073741824 },  // corkscrewDownRight0
        { 1073741824, -1859775393 },  // corkscrewDownRight1
        { 0, -2147483647 },           // corkscrewDownRight2
        { -1073741824, -1859775393 }, // corkscrewDownRight3
        { -1859775393, -1073741824 }, // corkscrewDownRight4
        { 2144540595, 112390610 },    // upHalfHelixLarge
        { 2139311823, 187165532 },    // upHalfHelixSmall
        { 2144540595, -112390610 },   // downHalfHelixLarge
        { 2139311823, -187165532 },   // downHalfHelixSmall
        { 2135719507, 224473165 },    // upQuarterHelix
        { 2135719507, -224473165 },   // downQuarterHelix
        { 2125953864, 303325208 },    // up8
        { 2061796213, 600568389 },    // up16
        { 1411702590, 1618265062 },   // up50
        { 2125953864, -303325208 },   // down8
        { 2061796213, -600568389 },   // down16
        { 1411702590, -1618265062 },  // down50
        { 1985590284, -817995863 },   // uninvertingDown25
        { 1636362342, -1390684831 },  // uninvertingDown42
        { 1127484953, -1827693544 },  // uninvertingDown60
        { 2115506168, 369214930 },    // curvedLifthillUp
        { 2115506168, -369214930 },   // curvedLiftHillDown
    });
    static_assert(std::size(kPitchToDirectionVectorInt32) == EnumValue(VehiclePitch::pitchCount));

    /** rct2: 0x009A39C4 */
    const auto kRollHorizontalComponent = std::to_array<int32_t>({
        2147483647,  2096579710,  1946281152,  2096579710, 1946281152, 1380375879, 555809667,
        -372906620,  -1231746017, -1859775391, 1380375879, 555809667,  -372906620, -1231746017,
        -1859775391, 0,           2096579710,  1946281152, 2096579710, 1946281152,
    });
    static_assert(std::size(kRollHorizontalComponent) == EnumValue(VehicleRoll::rollCount));

    /** rct2: 0x009A3684 */
    const auto kSpriteDirectionToSoundDirection = std::to_array<int32_t>({
        -0x4000, // 0
        -0x3000, // 1
        -0x2000, // 2
        -0x1000, // 3
        0,       // 4
        0x1000,  // 5
        0x2000,  // 6
        0x3000,  // 7
        0x4000,  // 8
        0x4800,  // 9
        0x4800,  // 10
        0x4800,  // 11
        0x4800,  // 12
        0x4800,  // 13
        0x4800,  // 14
        0x4800,  // 15
        0x4000,  // 16
        0x3000,  // 17
        0x2000,  // 18
        0x1000,  // 19
        0,       // 20
        -0x1000, // 21
        -0x2000, // 22
        -0x3000, // 23
        -0x4000, // 24
        -0x4800, // 25
        -0x4800, // 26
        -0x4800, // 27
        -0x4800, // 28
        -0x4800, // 29
        -0x4800, // 30
        -0x4800, // 31
    });

    /** rct2: 0x009A3AC4, 0x009A3AC6 */
    const auto kCrashDirectionComponents = std::to_array<CoordsXY>({
        { -256, 0 },
        { -236, 98 },
        { -181, 181 },
        { -98, 236 },
        { 0, 256 },
        { 98, 236 },
        { 181, 181 },
        { 236, 98 },
        { 256, 0 },
        { 236, -98 },
        { 181, -181 },
        { 98, -236 },
        { 0, -256 },
        { -98, -236 },
        { -181, -181 },
        { -236, -98 },
    });
    static_assert(std::size(kCrashDirectionComponents) == 16);

    Unk9A36C4Struct getFreeroamVehicleMovementData(uint8_t orientation)
    {
        return kFreeroamVehicleMovementData[orientation];
    }

    int32_t getRollHorizontalComponent(VehicleRoll roll)
    {
        return kRollHorizontalComponent[EnumValue(roll)];
    }

    int32_t getSoundDirectionFromOrientation(uint8_t orientation)
    {
        return kSpriteDirectionToSoundDirection[orientation];
    }

    CoordsXY getCrashDirectionComponents(uint8_t orientation)
    {
        return kCrashDirectionComponents[orientation / 2];
    }

    int32_t getTranslationDistance(CoordsXYZ distance, bool useReverserDistance)
    {
        uint8_t index = ((distance.x != 0) << 0) | ((distance.y != 0) << 1) | ((distance.z != 0) << 2)
            | ((useReverserDistance) << 3);
        return kSubpositionTranslationDistances[index];
    }

    int32_t getAccelerationFromPitch(VehiclePitch pitch)
    {
        if (pitch >= VehiclePitch::pitchCount)
        {
            return 0;
        }
        return kAccelerationFromPitch[EnumValue(pitch)];
    }

    CoordsXY getPitchVector32(VehiclePitch pitch)
    {
        if (pitch >= VehiclePitch::pitchCount)
        {
            pitch = VehiclePitch::flat;
        }
        return kPitchToDirectionVectorInt32[EnumValue(pitch)];
    }

} // namespace OpenRCT2::RideVehicle::Geometry
