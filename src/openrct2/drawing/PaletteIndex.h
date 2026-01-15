/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2::Drawing
{
    enum class PaletteIndex : uint8_t
    {
        pi0 = 0,   // Transparent
        pi10 = 10, // Black (0-dark), Dark grey (0)
        pi11 = 11, // Black (middark)
        pi12 = 12, // Black (midlight), Dark grey (1-darkest)
        pi14 = 14, // Black (lighter), Dark grey (dark)
        pi13 = 13, //
        pi16 = 16, //
        pi17 = 17, // Black (11), Dark grey (light), White (dark)
        pi18 = 18,
        pi20 = 20,   // Dark grey (10), White (light)
        pi21 = 21,   // Dark grey (11), White (lighter-11)
        pi40 = 40,   //
        pi42 = 42,   // Light Brown (lighter), Saturated brown (11)
        pi43 = 43,   //
        pi44 = 44,   // Construction marker
        pi46 = 46,   // Tertiary remap 0  / Yellow (darkest)
        pi47 = 47,   // Tertiary remap 1  / Yellow
        pi48 = 48,   // Tertiary remap 2  / Yellow
        pi49 = 49,   // Tertiary remap 3  / Yellow
        pi50 = 50,   // Tertiary remap 4  / Yellow
        pi51 = 51,   // Tertiary remap 5  / Yellow
        pi52 = 52,   // Tertiary remap 6  / Yellow
        pi53 = 53,   // Tertiary remap 7  / Yellow
        pi54 = 54,   // Tertiary remap 8  / Yellow
        pi55 = 55,   // Tertiary remap 9  / Yellow
        pi56 = 56,   // Tertiary remap 10 / Yellow
        pi57 = 57,   // Tertiary remap 11 / Yellow (lightest)
        pi61 = 61,   // Bordeaux Red (darker)
        pi62 = 62,   //
        pi68 = 68,   //
        pi69 = 69,   //
        pi73 = 73,   //
        pi99 = 99,   //
        pi102 = 102, // Bright green (lighter)
        pi108 = 108, //
        pi111 = 111, //
        pi114 = 114,
        pi126 = 126,
        pi129 = 129, // Light Purple (11)
        pi135 = 135, //
        pi136 = 136, // Dark Blue (10-11), Light Blue (midlight), Icy Blue (darker)
        pi138 = 138, // Light Blue (lighter), Icy Blue (middark)
        pi139 = 139,
        pi141 = 141, //
        pi144 = 144, // Dark Green (1-darkest,
        pi150 = 150,
        pi152 = 152,
        pi161 = 161, // Bright Purple (light)
        pi162 = 162, //
        pi164 = 164, //
        pi171 = 171, // Saturated Red (lightest) Bright Red (middark)
        pi172 = 172, // Saturated Red (10-11), Bright Red (midlight)
        pi173 = 173, // Used to draw intense lines in the Ride Graphs window
        pi174 = 174,
        pi183 = 183, // Used to draw rides in the Map window
        pi186 = 186, //
        pi187 = 187, //
        pi194 = 194, //
        pi195 = 195, //
        pi198 = 198,
        pi199 = 199, //
        pi202 = 202, // Secondary remap 0  / Pink (darkest)
        pi203 = 203, // Secondary remap 1  / Pink
        pi204 = 204, // Secondary remap 2  / Pink
        pi205 = 205, // Secondary remap 3  / Pink
        pi206 = 206, // Secondary remap 4  / Pink
        pi207 = 207, // Secondary remap 5  / Pink
        pi208 = 208, // Secondary remap 6  / Pink
        pi209 = 209, // Secondary remap 7  / Pink
        pi210 = 210, // Secondary remap 8  / Pink
        pi211 = 211, // Secondary remap 9  / Pink
        pi212 = 212, // Secondary remap 10 / Pink
        pi213 = 213, // Secondary remap 11 / Pink (lightest)
        pi222 = 222, //
        pi223 = 223, //
        pi229 = 229,
        pi230 = 230, // Water (waves)
        pi231 = 231, // Water (waves)
        pi232 = 232, // Water (waves)
        pi233 = 233, // Water (waves)
        pi234 = 234, // Water (waves)
        pi235 = 235, // Water (sparkles)
        pi236 = 236, // Water (sparkles)
        pi237 = 237, // Water (sparkles)
        pi238 = 238, // Water (sparkles)
        pi239 = 239, // Water (sparkles)
        pi240 = 240, // Track rails
        pi241 = 241, // Track rails
        pi242 = 242, // Track rails
        pi243 = 243, // Primary remap 0
        pi244 = 244, // Primary remap 1
        pi245 = 245, // Primary remap 2
        pi246 = 246, // Primary remap 3
        pi247 = 247, // Primary remap 4
        pi248 = 248, // Primary remap 5
        pi249 = 249, // Primary remap 6
        pi250 = 250, // Primary remap 7
        pi251 = 251, // Primary remap 8
        pi252 = 252, // Primary remap 9
        pi253 = 253, // Primary remap 10
        pi254 = 254, // Primary remap 11
        pi255 = 255, // White
    };

}
