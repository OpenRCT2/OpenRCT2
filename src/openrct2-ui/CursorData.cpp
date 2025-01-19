/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CursorData.h"

#include <cassert>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/interface/Cursors.h>

namespace OpenRCT2::Ui
{
    constexpr char kRawTransparent = ' ';
    constexpr char kRawWhite = '.';
    constexpr char kRawBlack = 'X';

    constexpr static CursorData cursorFromBitMap(int x, int y, std::string_view bitmap)
    {
        assert(bitmap.length() == kRawCursorSize);

        CursorData newCursor{};
        newCursor.HotSpot.X = x;
        newCursor.HotSpot.Y = y;

        uint8_t curBit{};
        uint16_t curPixel{};
        uint8_t dataByte{}, maskByte{};
        for (char rawPixel : bitmap)
        {
            uint8_t dataBit{}, maskBit{};
            switch (rawPixel)
            {
                case kRawBlack:
                    dataBit = 1;
                    maskBit = 1;
                    break;
                case kRawWhite:
                    dataBit = 0;
                    maskBit = 1;
                    break;
                case kRawTransparent:
                    dataBit = 0;
                    maskBit = 0;
                    break;
                default:
                    assert("Invalid character in cursor bitmap!");
            }

            dataByte |= (dataBit << (7 - curBit));
            maskByte |= (maskBit << (7 - curBit));

            // Save dataBit once 8 chars have been processed.
            curBit = (curBit + 1) % 8;
            if (curBit == 0)
            {
                newCursor.Data[curPixel] = dataByte;
                newCursor.Mask[curPixel] = maskByte;

                dataByte = 0;
                maskByte = 0;
                curPixel++;
            }
        }

        return newCursor;
    }

    static constexpr CursorData kBlankCursorData = { { 0, 0 }, { 0 }, { 0 } };

    static constexpr CursorData kUpArrowCursorData = cursorFromBitMap(
        16, 0,
        "                X               "
        "               X.X              "
        "              X...X             "
        "             X.....X            "
        "            X.......X           "
        "           X.........X          "
        "           XXXX...XXXX          "
        "              X...X             "
        "              X...X             "
        "              X...X             "
        "              X...X             "
        "              X...X             "
        "              XXXXX             "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                ");

    static constexpr CursorData kUpDownArrowCursorData = cursorFromBitMap(
        16, 16,
        "                                "
        "                X               "
        "               X.X              "
        "              X.X.X             "
        "             X.X X.X            "
        "            X.X   X.X           "
        "           X.XXX XXX.X          "
        "          X....X X....X         "
        "           XXX.XXX.XXX          "
        "             X.....X            "
        "              XXXXX             "
        "                                "
        "                                "
        "                X               "
        "               X.X              "
        "              X...X             "
        "               X.X              "
        "                X               "
        "                                "
        "                                "
        "              XXXXX             "
        "             X.....X            "
        "             X.XXX.X            "
        "           XXX.X X.XXX          "
        "          X....X X....X         "
        "           X.XXX XXX.X          "
        "            X.X   X.X           "
        "             X.X X.X            "
        "              X.X.X             "
        "               X.X              "
        "                X               "
        "                                ");

    static constexpr CursorData kZzzCursorData = cursorFromBitMap(
        0, 0,
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                           XXX  "
        "                      XXXXX...X "
        "                XXXXXX....XX.X  "
        "               X.....XXX.XX...X "
        "          XXXXXXXXX.X X.XX XXX  "
        "         X......XX.X X....X     "
        "          XXXX.XX.XXX XXXX      "
        "  XXXXXXXX  X.XX.....X          "
        " X........XX.X  XXXXX           "
        " X........X.XXXX                "
        "  XXXX...X......X               "
        "    X...X XXXXXX                "
        "   X...X                        "
        "  X...XXXX                      "
        " X........X                     "
        " X........X                     "
        "  XXXXXXXX                      "
        "                                "
        "                                "
        "                                "
        "                                ");

    static constexpr CursorData kDiagonalArrowCursorData = cursorFromBitMap(
        7, 7,
        ".......                         "
        ".XXXX.                          "
        ".XXX.                           "
        ".XXX.                           "
        ".X..X.                          "
        "..  .X.                         "
        ".    .X.                        "
        "      .X.                       "
        "       .X.    .                 "
        "        .X.  ..                 "
        "         .X..X.                 "
        "          .XXX.                 "
        "          .XXX.                 "
        "         .XXXX.                 "
        "        .......                 "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                ");

    static constexpr CursorData kPickerArrowCursorData = cursorFromBitMap(
        15, 31,
        "             XXXXX              "
        "            X....XX             "
        "           X..XX..XX            "
        "           X.XXXX.XX            "
        "           X.XXXX.XX            "
        "           X..XX..XX            "
        "            X....XX             "
        "            XXXXXXX             "
        "            XXXXXXX             "
        "           X.XXX..XX            "
        "           X..X...XX            "
        "          X....X...XX           "
        "          X...XX...XX           "
        "         X...X  X...XX          "
        "         X...X  X...XX          "
        "        X...X    X...XX         "
        "        X..X      X..XX         "
        "       X...X      X...XX        "
        "       X..X        X..XX        "
        "      X..X          X..XX       "
        "      X..X          X..XX       "
        "     X..X            X..XX      "
        "     X..X            X..XX      "
        "     X..X            X..XX      "
        "     X..X            X..XX      "
        "     X..X            X..XX      "
        "     X...X          X...XX      "
        "      X..X          X..XX       "
        "       X..X        X..XX        "
        "        X..X      X..XX         "
        "         XX.X    X.XXX          "
        "           XXX  XXXX            ");

    static constexpr CursorData kTreeDownCursorData = cursorFromBitMap(
        6, 31,
        "                                "
        "                 XXX            "
        "               XX...X  XXX      "
        "              X......XX...X     "
        "             XXX...........XX   "
        "             XXX.X...........X  "
        "              XXX............X  "
        "               XXX............X "
        "              XXX.....X.......X "
        "              XXXX.X.........X  "
        "             XXXXXXXX..X....X   "
        "            XXXX.XX.XXX......X  "
        "            XXX.X.XXX...X.....X "
        "            XXXX.X...X....X...X "
        "            XXXXX.X.X....X...XX "
        "            XXXXXXXXXX.....X.XX "
        "             XXXXXX.XXX.X...XX  "
        "    XXXXX      XXXXXXXXXXX.XX   "
        "    X...X        XXXXX.XXXXX    "
        "    X...X         XXX.X         "
        "    X...X          XX.X         "
        "    X...X          XX.X         "
        "    X...X          X..X         "
        "    X...X          X..X         "
        "    X...X          X...X        "
        "XXXXX...XXXXX    XX.....X       "
        " X.........X   XX........XX     "
        "  X.......X   X.....X..XX..X    "
        "   X.....X     XXX.X X.X XX     "
        "    X...X         X   X         "
        "     X.X                        "
        "      X                         ");

    static constexpr CursorData kFountainDownCursorData = cursorFromBitMap(
        6, 31,
        "               .     .          "
        "           .       .    .      ."
        "                 .     .   . .  "
        "             . .    .     .     "
        "                  .   . .      ."
        "                .   .      .    "
        "                   .     .      "
        "                      .         "
        "                    .   .       "
        "                      .         "
        "                     X          "
        "                    XXX         "
        "                    X.X         "
        "                 XXXX.XXXX      "
        "               XX...X.XX..XX    "
        "              X....X.XXXXX..X   "
        "              X.....XXXXX...X   "
        "    XXXXX     XXX.........XXX   "
        "    X...X     X..XXXXXXXXX.XX   "
        "    X...X      X.......XXXXX    "
        "    X...X       XXX...XXXXX     "
        "    X...X         XXXXXXX       "
        "    X...X          X.XXX        "
        "    X...X          X..XX        "
        "    X...X          X..XX        "
        "XXXXX...XXXXX     X..XXXX       "
        " X.........X     X..XXX.XX      "
        "  X.......X     X.......XXX     "
        "   X.....X      XX.....XX.X     "
        "    X...X        XXXXXXXXX      "
        "     X.X           XXXXX        "
        "      X                         ");

    static constexpr CursorData kStatueDownCursorData = cursorFromBitMap(
        6, 31,
        "                                "
        "                                "
        "             X             X    "
        "             XXX      XX  XX    "
        "              X.X    X..X  XX   "
        "               X.X   X..X  XX   "
        "                X.XX X.XX X.X   "
        "                 X..X.X XX.X    "
        "                  X....X..X     "
        "                  X......X      "
        "                   X....X       "
        "                   X....X       "
        "                    X..X        "
        "                    X...X       "
        "                    X...X       "
        "                     X.X        "
        "                     X.X        "
        "    XXXXX           X...X       "
        "    X...X            X..X       "
        "    X...X            X.X        "
        "    X...X           XX.X        "
        "    X...X           X..X        "
        "    X...X         XXXXXXX       "
        "    X...X         X.....X       "
        "    X...X         X.....X       "
        "XXXXX...XXXXX     X.....X       "
        " X.........X      X.....X       "
        "  X.......X       X.....X       "
        "   X.....X       XXXXXXXXX      "
        "    X...X       X.........X     "
        "     X.X        XXXXXXXXXXX     "
        "      X                         ");

    static constexpr CursorData kBenchDownCursorData = cursorFromBitMap(
        6, 31,
        "                                "
        "                                "
        "             XX                 "
        "             X.XX               "
        "             X...XX             "
        "             X.....XX           "
        "             XX......XX         "
        "             X.XX......XX       "
        "             X.X XX......XX     "
        "            XX.X   XX......XX   "
        "          XX..XX     XX......X  "
        "        XX......XX     XX....X  "
        "        XXXX......XX     XX..X  "
        "        X.X XX......XX     XXX  "
        "        X.X  XXX......XX   X.X  "
        "         X      XX......XX X.X  "
        "                XXXX......XX.X  "
        "    XXXXX       X.X XX......XX  "
        "    X...X       X.X   XX..XXXX  "
        "    X...X        X     XXX X.X  "
        "    X...X              XXX X.X  "
        "    X...X              X.X  X   "
        "    X...X              X.X      "
        "    X...X               X       "
        "    X...X                       "
        "XXXXX...XXXXX                   "
        " X.........X                    "
        "  X.......X                     "
        "   X.....X                      "
        "    X...X                       "
        "     X.X                        "
        "      X                         ");

    static constexpr CursorData kCrossHairCursorData = cursorFromBitMap(
        15, 15,
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "              X                 "
        "             X.X                "
        "             X.X                "
        "             X.X                "
        "             X.X                "
        "             X.X                "
        "             X.X                "
        "             X.X                "
        "              X                 "
        "                                "
        "    XXXXXXX   X   XXXXXXX       "
        "   X.......X X.X X.......X      "
        "    XXXXXXX   X   XXXXXXX       "
        "                                "
        "              X                 "
        "             X.X                "
        "             X.X                "
        "             X.X                "
        "             X.X                "
        "             X.X                "
        "             X.X                "
        "             X.X                "
        "              X                 "
        "                                "
        "                                "
        "                                "
        "                                ");

    static constexpr CursorData kBinDownCursorData = cursorFromBitMap(
        6, 31,
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                   XXX          "
        "                  X   X         "
        "                  XXXXX         "
        "                XX....XXX       "
        "               X........XX      "
        "              X..X.X.XXXXXX     "
        "              X..........XX     "
        "              XXXXXXXXXXXXX     "
        "               X.......XXX      "
        "               X.X......XX      "
        "               X.X.X....XX      "
        "    XXXXX      X.X.X.....X      "
        "    X...X      X.X.X...X.X      "
        "    X...X      X.X.X...X.X      "
        "    X...X      X.X.X.X.X.X      "
        "    X...X      X.X.X.X.X.X      "
        "    X...X      X.X.X.X.X.X      "
        "    X...X      X.X.X.X.X.X      "
        "    X...X      X.X.X.X.X.X      "
        "XXXXX...XXXXX  X.X.X.X.X.X      "
        " X.........X   X.X.X.X.X.X      "
        "  X.......X    XXXXXXXXXXX      "
        "   X.....X                      "
        "    X...X                       "
        "     X.X                        "
        "      X                         ");

    static constexpr CursorData kLampPostDownCursorData = cursorFromBitMap(
        6, 31,
        "                   XXX          "
        "                  X...X         "
        "                 X.....X        "
        "                XXXXXXXXX       "
        "                 XXXX..X        "
        "                 XX....X        "
        "                 XX....X        "
        "                  XX..X         "
        "                  XXXXX         "
        "                   XXX          "
        "                   X.X          "
        "                X  X.X  X       "
        "               XXXXX.XXXXX      "
        "                X  X.X  X       "
        "                   X.X          "
        "                   X.X          "
        "                   X.X          "
        "    XXXXX          X.X          "
        "    X...X          X.X          "
        "    X...X          X.X          "
        "    X...X          X.X          "
        "    X...X          X.X          "
        "    X...X          X.X          "
        "    X...X          XXX          "
        "    X...X          XXX          "
        "XXXXX...XXXXX     XX..X         "
        " X.........X     XX....X        "
        "  X.......X      XX....X        "
        "   X.....X       XXX...X        "
        "    X...X        XXXXX.X        "
        "     X.X        XXXXXXXXX       "
        "      X                         ");

    static constexpr CursorData kFenceDownCursorData = cursorFromBitMap(
        6, 31,
        "                                "
        "   XX                           "
        "  X.XX                          "
        " X..XXX  XX                     "
        " X....XXX.XX                    "
        "  X.....X.XX   XX               "
        "  X.XX....XXX X.XX              "
        "  X.XXXX....XXX.XX   XX         "
        "  X.XX  X.....X.XX  X.XX        "
        "  X.XX  X.XX.....XX X.XX   XX   "
        " XX.XX  X.XXXX.....XX.XX  X.XX  "
        " XXXXX  X.XX  X.X.....XX  X.XX  "
        "   XXXX X.XX  X.XXX....XX X.XX  "
        "     XXXX.XX  X.XX XX....XX.XX  "
        "       XXXXX  X.XX  X.X.....XX  "
        "         XXXX X.XX  X.XXX....XX "
        "           XXXX.XX  X.XX XX..XX "
        "    XXXXX    XXXXX  X.XX  X.XXX "
        "    X...X      XXXX X.XX  X.XX  "
        "    X...X        XXXX.XX  X.XX  "
        "    X...X          XXXXX  X.XX  "
        "    X...X            XXXX X.XX  "
        "    X...X              XXXX.XX  "
        "    X...X                XXXX   "
        "    X...X                  XX   "
        "XXXXX...XXXXX                   "
        " X.........X                    "
        "  X.......X                     "
        "   X.....X                      "
        "    X...X                       "
        "     X.X                        "
        "      X                         ");

    static constexpr CursorData kFlowerDownCursorData = cursorFromBitMap(
        6, 31,
        "                   X            "
        "                  X.X           "
        "                 X...X          "
        "           XX    X...X    XX    "
        "          X..X   X.X.X   X..X   "
        "          X...X  X.X.X  X...X   "
        "           X...X X.X.X X...X    "
        "            X.X.X XXX X.X.X     "
        "             X.XX XXX XX.X      "
        "              XXXX...XXXX       "
        "         XXXXX  X..X..X  XXXXX  "
        "        X.....XX.X...XXXX.....X "
        "       X...XXX.XX...X..XXXXX...X"
        "        X.....XX..X...XXX.....X "
        "         XXXXX  X...X.X  XXXXX  "
        "              XXXXX..XXXX       "
        "             X.XX XXX XX.X      "
        "    XXXXX   X.X.X XXX X.X.X     "
        "    X...X  X...X X.X.X X...X    "
        "    X...X  X..X  X.X.X  X..X    "
        "    X...X   XX   X...X   XX XX  "
        "    X...X  XX    X...X    XX..X "
        "    X...X X..XX  X...X   X....X "
        "    X...X X....X  X.X   X.....X "
        "    X...X X.....X XXX  X..X..X  "
        "XXXXX...XXXXX.X.X XXX X..X...X  "
        " X.........X...X.XX.X X.X...X   "
        "  X.......X X...XXX.XX.X...X    "
        "   X.....X   XX..X..X.X..XX     "
        "    X...X      XX......XX       "
        "     X.X         XXXXXX         "
        "      X                         ");

    static constexpr CursorData kFootpathDownCursorData = cursorFromBitMap(
        6, 31,
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "           XX                   "
        "         XX..XX                 "
        "       XX....X.XX               "
        "     XX..X..X....XX             "
        "   XX.X...X..X...X.XX           "
        " XX...X...XXX.X..X...XX         "
        " XXXX..XXX.....X.X...X.XX       "
        "   XXXX...X..XXXXXXXXX...XX     "
        "     XXXX..XX...X.....X..X.XX   "
        "       XXXX.....X.....X..X...XX "
        "         XXXX..X.X...X.X.XXXX..X"
        "           XXXX...X.X...X....XXX"
        "    XXXXX    XXXX..X.X..X..XXXX "
        "    X...X      XXXX...X..XXXX   "
        "    X...X        XXXX..XXXX     "
        "    X...X          XXXXXX       "
        "    X...X            XX         "
        "    X...X                       "
        "    X...X                       "
        "    X...X                       "
        "XXXXX...XXXXX                   "
        " X.........X                    "
        "  X.......X                     "
        "   X.....X                      "
        "    X...X                       "
        "     X.X                        "
        "      X                         ");

    static constexpr CursorData kDigDownCursorData = cursorFromBitMap(
        6, 31,
        "                                "
        "                          XX    "
        "                         X..X   "
        "                        X.X..X  "
        "                       X.X X..X "
        "                      X.X   X.XX"
        "                      X.X  X.XX "
        "                      X..XX.XX  "
        "                     X.....XX   "
        "                    X...XXXX    "
        "                   X...XX       "
        "              XX  X.X.X         "
        "             X..XX...XX         "
        "            X..XX...XX          "
        "           X.X.X...XX           "
        "          X...X...XX            "
        "          X..X...XXXX           "
        "    XXXXXX...X.XXXX..X          "
        "    X...XX...XXXXX...X          "
        "    X...XX....XX..X.X           "
        "    X...X X........X            "
        "    X...X  X......X             "
        "    X...X   X...XX              "
        "    X...X    XXX                "
        "    X...X                       "
        "XXXXX...XXXXX                   "
        " X.........X                    "
        "  X.......X                     "
        "   X.....X                      "
        "    X...X                       "
        "     X.X                        "
        "      X                         ");

    static constexpr CursorData kWaterDownCursorData = cursorFromBitMap(
        6, 31,
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "                                "
        "    X     X     X     X         "
        "    X     X     X     X         "
        "   X.X   X.X   X.X   X.X        "
        " XX...XXX...XXX...XXX...XX      "
        "X...X.....X.....X.....X...X     "
        " XXX XXXXX XXXXX XXXXX XXX      "
        "                                "
        "          X     X     X     X   "
        "          X     X     X     X   "
        "         X.X   X.X   X.X   X.X  "
        "        X...XXX...XXX...XXX...X "
        "    XXXXX.X.....X.....X.....X..X"
        "    X...XX XXXXX XXXXX XXXXX XX "
        "    X...X                       "
        "    X...X       X     X         "
        "    X...X       X     X         "
        "    X...X      X.X   X.X        "
        "    X...X    XX...XXX...XX      "
        "XXXXX...XXXXX...X.....X...X     "
        " X.........X XXX XXXXX XXX      "
        "  X.......X                     "
        "   X.....X                      "
        "    X...X                       "
        "     X.X                        "
        "      X                         ");

    static constexpr CursorData kHouseDownCursorData = cursorFromBitMap(
        6, 31,
        "                                "
        "                                "
        "                   X  XXXXXX    "
        "                  X.X X....X    "
        "                 X...X XXXX     "
        "                X..X..XX..X     "
        "               X...XX..X..X     "
        "              X.....XX....X     "
        "             X...XXXXXX...X     "
        "            X.........XX..X     "
        "           X....XXXXXXXXX..X    "
        "          X.............XX..X   "
        "         X...XXXXXXXXXXXXXX..X  "
        "        XXXX.............XXXXXX "
        "           X...............X    "
        "           X...............X    "
        "           X...XXXX.XXXXXX.X    "
        "           X...X  X.X    X.X    "
        "    XXXXX  X...X  X.X    X.X    "
        "    X...X  X...X  X.X    X.X    "
        "    X...X  X...X  X.XXXXXX.X    "
        "    X...X  X...X  X........X    "
        "    X...X  X...X  X........X    "
        "    X...X  XXXXXXXXXXXXXXXXX    "
        "    X...X                       "
        "XXXXX...XXXXX                   "
        " X.........X                    "
        "  X.......X                     "
        "   X.....X                      "
        "    X...X                       "
        "     X.X                        "
        "      X                         ");

    static constexpr CursorData kVolcanoDownCursorData = cursorFromBitMap(
        6, 31,
        "            X   X X             "
        "              X     X           "
        "                 X              "
        "              X  X              "
        "             XXXX XX            "
        "           XXXXXXXXXXX          "
        "           X.XXXX....X          "
        "          XX.......XXXX         "
        "         XXX.XX..XXX..X         "
        "         X.XX.X.X...X..X        "
        "        X......X...XXX..X       "
        "      XX....X.......X.X..XXX    "
        "     X.....XX......X.XXXX...X   "
        "   XX.....XX........X.X.X.X..XX "
        "  X...XXXX....X......XXX.XXXXXXX"
        " XXXXX   X.XXXXXX...XXXXXXX XX  "
        " X      XXXX    XX.XX   XX      "
        "         X        X             "
        "    XXXXX                       "
        "    X...X                       "
        "    X...X                       "
        "    X...X                       "
        "    X...X                       "
        "    X...X                       "
        "    X...X                       "
        "XXXXX...XXXXX                   "
        " X.........X                    "
        "  X.......X                     "
        "   X.....X                      "
        "    X...X                       "
        "     X.X                        "
        "      X                         ");

    static constexpr CursorData kWalkDownCursorData = cursorFromBitMap(
        6, 31,
        "                                "
        "      XXX                       "
        "     X...X                      "
        "     X....X                     "
        "      X....X                    "
        "       X...X                    "
        "        XXX  X                  "
        "           XX.X                 "
        "           X..X                 "
        "            XX                  "
        "                                "
        "       XXX         XXX          "
        "      X...X       X...X         "
        "      X....X      X....X        "
        "       X....X      X....X       "
        "        X...X       X...X       "
        "         XXX  X      XXX  X     "
        "            XX.X        XX.X    "
        "    XXXXX   X..X        X..X    "
        "    X...X    XX          XX     "
        "    X...X                       "
        "    X...X           XXX         "
        "    X...X          X...X        "
        "    X...X          X....X       "
        "    X...X           X....X      "
        "XXXXX...XXXXX        X...X      "
        " X.........X          XXX  X    "
        "  X.......X              XX.X   "
        "   X.....X               X..X   "
        "    X...X                 XX    "
        "     X.X                        "
        "      X                         ");

    static constexpr CursorData kPaintDownCursorData = cursorFromBitMap(
        7, 30,
        "                                "
        "                           XXX  "
        "                         XXXXXX "
        "                        XXXXX.X "
        "                       XXXXX.XX "
        "                      XXXXX..X  "
        "              XX     XXXXX..XX  "
        "             XXXX    X..X..XX   "
        "            XXXXXX  X....XXX    "
        "           XX.XXXXXXX....XX     "
        "          XXXX.XXXX.....XX      "
        "         XXXXXX.XXX...XX        "
        "        XXXXXXXX.XX...X         "
        "        XX.XXXXXX.XXXXX         "
        "         XXXXXXXXX.XXXXX        "
        "          XX.XXXXXX.XXXXX       "
        "           XX.XXXXXX.XXXXX      "
        "     XXXXX  XXXXXXXXX.XXXX      "
        "     X...X   XX.X..XXX.XXX      "
        "     X...X    XX.....XXXX       "
        "     X...X     XX.....XX        "
        "     X...X      XX...XX         "
        "     X...X       XX.XX          "
        "     X...X        X.X           "
        " XXXXX...XXXXX     X            "
        "  X.........X                   "
        "   X.......X                    "
        "    X.....X                     "
        "     X...X                      "
        "      X.X                       "
        "       X                        "
        "                                ");

    static constexpr CursorData kEntranceDownCursorData = cursorFromBitMap(
        6, 31,
        "  X                         X   "
        " X.X                       X.X  "
        "X...X                     X...X "
        " X.X                       X.X  "
        " XXXXXXXXXXXXXXXXXXXXXXXXXXXXX  "
        " X.XX.....................XX.X  "
        " X.X X...................X X.X  "
        " X.X X...................X X.X  "
        " X.X X...................X X.X  "
        " X.XXX....................XX.X  "
        " X.XXXXXXXXXXXXXXXXXXXXXXXXX.X  "
        " X.X                       X.X  "
        " X.X                       X.X  "
        " X.X                       X.X  "
        " X.X                       X.X  "
        " X.X                       X.X  "
        " X.X                       X.X  "
        " X.XXXXXX                  X.X  "
        "XX.XX...X                  X.X  "
        " X.XX...X             X X XX.X X"
        " X.XX...X                  X.X  "
        "XX.XX...X                  X.X  "
        " X.XX...X            X X XXX.X  "
        "    X...X                       "
        "    X...X               XX X  X "
        "XXXXX...XXXXX       XXX         "
        " X.........X                    "
        "  X.......X                     "
        "   X.....X                      "
        "    X...X                       "
        "     X.X                        "
        "      X                         ");

    static constexpr CursorData kHandOpenDownCursorData = cursorFromBitMap(
        15, 17,
        "             XX    X            "
        "            X..X  XXX           "
        "            X..X  X..X          "
        "      XX    X...X X..X          "
        "     X..X   X...X X..X  XX      "
        "     X...X  X....XX...XX..X     "
        "     X....X  X...XX...XX...X    "
        "     XX...X  X...XX....XX..X    "
        "      X....X X....X....XX..X    "
        "      XX....XX....X....X...X    "
        "       X.....X....X....X...X    "
        "       XX....XX........X...X    "
        "        X.....X.............X   "
        "        XX..................X   "
        "         XX.................X   "
        "          X.................X   "
        "          XX................X   "
        "          XX................X   "
        "           X................X   "
        "           X................X   "
        "           X...............X    "
        "           X...............X    "
        "          X................X    "
        "          X................X    "
        "    XXXXXX.................X    "
        "   X........................X   "
        "   X.X......................X   "
        "   X..X.....................X   "
        "    XXXX....XXXX.............X  "
        "       XXXXXX  XX............X  "
        "                XX.........XX   "
        "                 XXXXXXXXXX     ");

    static constexpr CursorData kHandClosedDownCursorData = cursorFromBitMap(
        15, 17,
        "                                "
        "          XXXX  XXX             "
        "         X....XX...X            "
        "         X.....XX...XX          "
        "          X.....X....XXX        "
        "    XXX   X......X....X.X       "
        "   X...XX  XX....XX...X..X      "
        "   X.....XX XX...XX....X..X     "
        "   XX......X X....X....X..X     "
        "    XX......XX....X....X...X    "
        "     XX......X....X....X...X    "
        "      XX.....XX........X...X    "
        "        X.....X.............X   "
        "        XX..................X   "
        "         XX.................X   "
        "          X.................X   "
        "          XX................X   "
        "          XX................X   "
        "           X................X   "
        "           X................X   "
        "     XXX   X...............X    "
        "    X...X  X...............X    "
        "    X....XX................X    "
        "    X.X....................X    "
        "    X.X....................X    "
        "     X......................X   "
        "     X......................X   "
        "      XXX...................X   "
        "         XXXXXXX.............X  "
        "               XX............X  "
        "                XX.........XX   "
        "                 XXXXXXXXXX     ");

    static constexpr CursorData kBulldozerCursorData = cursorFromBitMap(
        6, 31,
        "                 XXX            "
        "               XX...X           "
        "             XX....X            "
        "            X..XX.X             "
        "            X..X X    XXXXXXX   "
        "            XXX      X.......X  "
        " XX         X.X      X.XXX.X.X  "
        " X.X      XXX.XXXXXXXX.XXX.X.X  "
        "  X.X    X...........X.XXX.X.X  "
        "   X.X   X.XXXXXX....X.XXX.X.X  "
        "   X.X   X.XX.X......X.......X  "
        "    X.XXXXXXXXXXX..XXXX......X  "
        "    X.X..........X...........X  "
        "    X.X....XXXXXXXXXXX.XX.XXX   "
        "    X.XXXXX..X..X..X..X..X..XX  "
        "   X.X   X.XX.XX.XX.XX.XX.XX.X  "
        "   X.X   X.XX.XX.XX.XX.XX.XX.X  "
        " XX.X     X..X..X..X..X..X..X   "
        "X..X       XXXXXXXXXXXXXXXXX    "
        " XX XXXXX                       "
        "    X...X                       "
        "    X...X                       "
        "    X...X                       "
        "    X...X                       "
        "    X...X                       "
        "XXXXX...XXXXX                   "
        " X.........X                    "
        "  X.......X                     "
        "   X.....X                      "
        "    X...X                       "
        "     X.X                        "
        "      X                         ");

    static constexpr const CursorData* kRawCursorData[] = {
        nullptr,                    // CursorID::Arrow
        &kBlankCursorData,          // CursorID::Blank
        &kUpArrowCursorData,        // CursorID::UpArrow
        &kUpDownArrowCursorData,    // CursorID::UpDownArrow
        nullptr,                    // CursorID::HandPoint
        &kZzzCursorData,            // CursorID::ZZZ
        &kDiagonalArrowCursorData,  // CursorID::DiagonalArrows
        &kPickerArrowCursorData,    // CursorID::Picker
        &kTreeDownCursorData,       // CursorID::TreeDown
        &kFountainDownCursorData,   // CursorID::FountainDown
        &kStatueDownCursorData,     // CursorID::StatueDown
        &kBenchDownCursorData,      // CursorID::BenchDown
        &kCrossHairCursorData,      // CursorID::CrossHair
        &kBinDownCursorData,        // CursorID::BinDown
        &kLampPostDownCursorData,   // CursorID::LamppostDown
        &kFenceDownCursorData,      // CursorID::FenceDown
        &kFlowerDownCursorData,     // CursorID::FlowerDown
        &kFootpathDownCursorData,   // CursorID::PathDown
        &kDigDownCursorData,        // CursorID::DigDown
        &kWaterDownCursorData,      // CursorID::WaterDown
        &kHouseDownCursorData,      // CursorID::HouseDown
        &kVolcanoDownCursorData,    // CursorID::VolcanoDown
        &kWalkDownCursorData,       // CursorID::WalkDown
        &kPaintDownCursorData,      // CursorID::PaintDown
        &kEntranceDownCursorData,   // CursorID::EntranceDown
        &kHandOpenDownCursorData,   // CursorID::HandOpen
        &kHandClosedDownCursorData, // CursorID::HandClosed
        &kBulldozerCursorData,      // CursorID::Bulldozer
    };

    const CursorData* getCursorData(CursorID cursorId)
    {
        const CursorData* result = nullptr;
        if (cursorId != CursorID::Undefined && cursorId != CursorID::Count)
        {
            result = kRawCursorData[EnumValue(cursorId)];
        }
        return result;
    }
} // namespace OpenRCT2::Ui
