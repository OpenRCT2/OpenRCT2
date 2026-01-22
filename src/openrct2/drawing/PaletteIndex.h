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
        transparent = 0,

        pi10 = 10, // Black (0-dark), Dark grey (0)
        pi11 = 11, // Black (middark)
        pi12 = 12, // Black (midlight), Dark grey (1-darkest)
        pi14 = 14, // Black (lighter), Dark grey (dark)
        pi13 = 13, //
        pi16 = 16, //
        pi17 = 17, // Black (11), Dark grey (light), White (dark)
        pi18 = 18,
        pi20 = 20, // Dark grey (10), White (light)
        pi21 = 21, // Dark grey (11), White (lighter-11)
        pi40 = 40, //
        pi42 = 42, // Light Brown (lighter), Saturated brown (11)
        pi43 = 43, //
        pi44 = 44, // Construction marker

        yellow0 = 46, // Darkest
        yellow1 = 47,
        yellow2 = 48,
        yellow3 = 49,
        yellow4 = 50,
        yellow5 = 51,
        yellow6 = 52,
        yellow7 = 53,
        yellow8 = 54,
        yellow9 = 55,
        yellow10 = 56,
        yellow11 = 57, // Lightest

        tertiaryRemap0 = 46,
        tertiaryRemap1 = 47,
        tertiaryRemap2 = 48,
        tertiaryRemap3 = 49,
        tertiaryRemap4 = 50,
        tertiaryRemap5 = 51,
        tertiaryRemap6 = 52,
        tertiaryRemap7 = 53,
        tertiaryRemap8 = 54,
        tertiaryRemap9 = 55,
        tertiaryRemap10 = 56,
        tertiaryRemap11 = 57,

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

        hotPink0 = 202, // Darkest
        hotPink1 = 203,
        hotPink2 = 204,
        hotPink3 = 205,
        hotPink4 = 206,
        hotPink5 = 207,
        hotPink6 = 208,
        hotPink7 = 209,
        hotPink8 = 210,
        hotPink9 = 211,
        hotPink10 = 212,
        hotPink11 = 213, // Lightest

        secondaryRemap0 = 202,
        secondaryRemap1 = 203,
        secondaryRemap2 = 204,
        secondaryRemap3 = 205,
        secondaryRemap4 = 206,
        secondaryRemap5 = 207,
        secondaryRemap6 = 208,
        secondaryRemap7 = 209,
        secondaryRemap8 = 210,
        secondaryRemap9 = 211,
        secondaryRemap10 = 212,
        secondaryRemap11 = 213,

        pi222 = 222, //
        pi223 = 223, //
        pi229 = 229,

        waterWaves0 = 230,
        waterWaves1 = 231,
        waterWaves2 = 232,
        waterWaves3 = 233,
        waterWaves4 = 234,
        waterSparkles0 = 235,
        waterSparkles1 = 236,
        waterSparkles2 = 237,
        waterSparkles3 = 238,
        waterSparkles4 = 239,

        trackRails0 = 240,
        trackRails1 = 241,
        trackRails2 = 242,

        primaryRemap0 = 243,
        primaryRemap1 = 244,
        primaryRemap2 = 245,
        primaryRemap3 = 246,
        primaryRemap4 = 247,
        primaryRemap5 = 248,
        primaryRemap6 = 249,
        primaryRemap7 = 250,
        primaryRemap8 = 251,
        primaryRemap9 = 252,
        primaryRemap10 = 253,
        primaryRemap11 = 254,

        pi255 = 255, // White
    };
}
