#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../world/sprite.h"

static const uint8 PeepSpriteImage_Normal_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Normal_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Normal_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Normal_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_Normal_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Normal_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Normal_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Normal_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Normal_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_Normal_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Normal_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Normal_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Normal_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Normal_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Normal_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Normal_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Normal_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Normal_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Normal_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Normal_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Normal_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Normal_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Normal_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Handyman_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Handyman_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Handyman_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Handyman_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_Handyman_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Handyman_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Handyman_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Handyman_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Handyman_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Handyman_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Handyman_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Handyman_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Handyman_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Handyman_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Handyman_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Handyman_17_sequence[] = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 7 };
static const uint8 PeepSpriteImage_Handyman_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Handyman_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Handyman_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Handyman_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Handyman_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Handyman_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Mechanic_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Mechanic_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Mechanic_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Mechanic_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_Mechanic_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Mechanic_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Mechanic_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Mechanic_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Mechanic_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Mechanic_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Mechanic_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Mechanic_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Mechanic_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Mechanic_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Mechanic_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Mechanic_17_sequence[] = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 7 };
static const uint8 PeepSpriteImage_Mechanic_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Mechanic_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Mechanic_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Mechanic_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Mechanic_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Mechanic_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Security_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Security_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Security_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Security_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_Security_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Security_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Security_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Security_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Security_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_Security_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Security_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Security_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Security_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Security_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Security_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Security_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Security_17_sequence[] = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };
static const uint8 PeepSpriteImage_Security_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Security_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Security_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Security_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Security_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Security_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_EntertainerPanda_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerPanda_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_EntertainerPanda_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_03_sequence[] = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerPanda_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_EntertainerPanda_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerPanda_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerPanda_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerPanda_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_EntertainerPanda_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_EntertainerPanda_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_EntertainerPanda_17_sequence[] = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };
static const uint8 PeepSpriteImage_EntertainerPanda_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static const uint8 PeepSpriteImage_EntertainerPanda_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_30_sequence[] = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPanda_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_EntertainerPanda_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_EntertainerTiger_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerTiger_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_EntertainerTiger_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_03_sequence[] = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerTiger_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_EntertainerTiger_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerTiger_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerTiger_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerTiger_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_EntertainerTiger_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_EntertainerTiger_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_EntertainerTiger_17_sequence[] = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };
static const uint8 PeepSpriteImage_EntertainerTiger_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 25, 25, 25 };
static const uint8 PeepSpriteImage_EntertainerTiger_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_30_sequence[] = { 0, 1, 2, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 4, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerTiger_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_EntertainerTiger_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_EntertainerElephant_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerElephant_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_EntertainerElephant_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerElephant_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_EntertainerElephant_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerElephant_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerElephant_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerElephant_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_EntertainerElephant_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_EntertainerElephant_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_EntertainerElephant_17_sequence[] = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 7, 8, 9 };
static const uint8 PeepSpriteImage_EntertainerElephant_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 };
static const uint8 PeepSpriteImage_EntertainerElephant_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerElephant_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_EntertainerElephant_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_EntertainerRoman_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerRoman_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_EntertainerRoman_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerRoman_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_EntertainerRoman_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerRoman_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerRoman_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerRoman_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_EntertainerRoman_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_EntertainerRoman_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_EntertainerRoman_17_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerRoman_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_26_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 4, 4, 4, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 16, 17, 18, 19, 20, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 24, 25, 26, 27, 28, 28, 28, 29, 30, 31, 32, 32, 32, 32, 32, 33, 34, 35, 36, 37, 38 };
static const uint8 PeepSpriteImage_EntertainerRoman_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerRoman_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_EntertainerRoman_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_EntertainerGorilla_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerGorilla_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_EntertainerGorilla_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerGorilla_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_EntertainerGorilla_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerGorilla_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerGorilla_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerGorilla_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_EntertainerGorilla_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_EntertainerGorilla_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_EntertainerGorilla_17_sequence[] = { 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerGorilla_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerGorilla_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_EntertainerGorilla_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_EntertainerSnowman_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerSnowman_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_EntertainerSnowman_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 31, 31, 32, 33 };
static const uint8 PeepSpriteImage_EntertainerSnowman_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerSnowman_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_EntertainerSnowman_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerSnowman_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerSnowman_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerSnowman_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_EntertainerSnowman_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_EntertainerSnowman_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_EntertainerSnowman_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7, 8, 9 };
static const uint8 PeepSpriteImage_EntertainerSnowman_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 31, 31, 32, 33 };
static const uint8 PeepSpriteImage_EntertainerSnowman_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 31, 31, 32, 33 };
static const uint8 PeepSpriteImage_EntertainerSnowman_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSnowman_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_EntertainerSnowman_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_EntertainerKnight_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerKnight_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_EntertainerKnight_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 23, 23, 23, 23, 23, 24, 25, 26, 27 };
static const uint8 PeepSpriteImage_EntertainerKnight_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerKnight_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_EntertainerKnight_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerKnight_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerKnight_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerKnight_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_EntertainerKnight_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_EntertainerKnight_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_EntertainerKnight_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerKnight_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 23, 23, 23, 23, 23, 24, 25, 26, 27 };
static const uint8 PeepSpriteImage_EntertainerKnight_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 23, 23, 23, 23, 23, 24, 25, 26, 27 };
static const uint8 PeepSpriteImage_EntertainerKnight_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerKnight_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_EntertainerKnight_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_EntertainerAstronaut_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_03_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_17_sequence[] = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_26_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_30_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_EntertainerAstronaut_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_EntertainerBandit_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerBandit_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_EntertainerBandit_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 13, 13, 13, 13, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 };
static const uint8 PeepSpriteImage_EntertainerBandit_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerBandit_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_EntertainerBandit_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerBandit_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerBandit_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerBandit_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_EntertainerBandit_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_EntertainerBandit_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_EntertainerBandit_17_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static const uint8 PeepSpriteImage_EntertainerBandit_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 13, 13, 13, 13, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 };
static const uint8 PeepSpriteImage_EntertainerBandit_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 13, 13, 13, 13, 13, 14, 15, 16, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 21, 20, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 };
static const uint8 PeepSpriteImage_EntertainerBandit_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerBandit_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_EntertainerBandit_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_EntertainerSheriff_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerSheriff_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_EntertainerSheriff_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerSheriff_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_EntertainerSheriff_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerSheriff_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerSheriff_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerSheriff_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_EntertainerSheriff_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_EntertainerSheriff_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_EntertainerSheriff_17_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static const uint8 PeepSpriteImage_EntertainerSheriff_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 9, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 10, 11, 12, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerSheriff_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_EntertainerSheriff_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_EntertainerPirate_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerPirate_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_EntertainerPirate_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 21, 21, 21, 21, 21, 22, 23, 23, 23, 23, 24, 25, 26, 27, 27, 27, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31 };
static const uint8 PeepSpriteImage_EntertainerPirate_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_EntertainerPirate_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_EntertainerPirate_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerPirate_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_EntertainerPirate_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_EntertainerPirate_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_EntertainerPirate_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_EntertainerPirate_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_EntertainerPirate_17_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static const uint8 PeepSpriteImage_EntertainerPirate_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 21, 21, 21, 21, 21, 22, 23, 23, 23, 23, 24, 25, 26, 27, 27, 27, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31 };
static const uint8 PeepSpriteImage_EntertainerPirate_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 21, 21, 21, 21, 21, 22, 23, 23, 23, 23, 24, 25, 26, 27, 27, 27, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31 };
static const uint8 PeepSpriteImage_EntertainerPirate_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_EntertainerPirate_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_EntertainerPirate_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_IceCream_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_IceCream_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_IceCream_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_IceCream_03_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_IceCream_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_IceCream_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_IceCream_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_IceCream_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_IceCream_08_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_IceCream_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_IceCream_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_IceCream_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_IceCream_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_IceCream_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_IceCream_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_IceCream_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_IceCream_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_IceCream_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_IceCream_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_IceCream_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_IceCream_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_IceCream_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_IceCream_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Chips_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Chips_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Chips_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Chips_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Chips_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Chips_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Chips_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Chips_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Chips_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Chips_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Chips_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Chips_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Chips_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Chips_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Chips_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Chips_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Chips_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Chips_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Chips_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Chips_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Chips_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chips_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Chips_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Burger_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Burger_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Burger_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Burger_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Burger_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Burger_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Burger_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Burger_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Burger_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Burger_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Burger_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Burger_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Burger_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Burger_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Burger_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Burger_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Burger_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Burger_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Burger_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Burger_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Burger_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Burger_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Burger_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Drink_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Drink_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Drink_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Drink_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Drink_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Drink_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Drink_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Drink_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Drink_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Drink_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Drink_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Drink_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Drink_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Drink_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Drink_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Drink_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Drink_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Drink_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Drink_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Drink_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Drink_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Drink_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Drink_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Balloon_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Balloon_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Balloon_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Balloon_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_Balloon_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Balloon_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Balloon_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Balloon_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Balloon_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Balloon_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Balloon_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Balloon_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Balloon_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Balloon_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Balloon_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Balloon_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Balloon_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Balloon_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Balloon_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Balloon_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Balloon_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Balloon_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Candyfloss_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Candyfloss_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Candyfloss_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Candyfloss_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Candyfloss_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Candyfloss_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Candyfloss_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Candyfloss_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Candyfloss_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Candyfloss_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Candyfloss_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Candyfloss_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Candyfloss_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Candyfloss_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Candyfloss_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Candyfloss_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Candyfloss_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Candyfloss_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Candyfloss_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Candyfloss_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Candyfloss_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Candyfloss_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Candyfloss_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Umbrella_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Umbrella_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Umbrella_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Umbrella_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_Umbrella_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Umbrella_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Umbrella_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Umbrella_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Umbrella_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Umbrella_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Umbrella_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Umbrella_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Umbrella_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Umbrella_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Umbrella_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Umbrella_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Umbrella_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Umbrella_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_24_sequence[] = {  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Umbrella_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Umbrella_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Umbrella_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Umbrella_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Pizza_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Pizza_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Pizza_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Pizza_03_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Pizza_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Pizza_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Pizza_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Pizza_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Pizza_08_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Pizza_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Pizza_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Pizza_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Pizza_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Pizza_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Pizza_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Pizza_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Pizza_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Pizza_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Pizza_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Pizza_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Pizza_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pizza_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Pizza_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_SecurityAlt_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_SecurityAlt_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_SecurityAlt_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_SecurityAlt_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_SecurityAlt_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_SecurityAlt_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_SecurityAlt_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_SecurityAlt_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_SecurityAlt_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_SecurityAlt_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_SecurityAlt_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_SecurityAlt_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_SecurityAlt_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_SecurityAlt_17_sequence[] = { 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6 };
static const uint8 PeepSpriteImage_SecurityAlt_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_SecurityAlt_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SecurityAlt_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_SecurityAlt_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Popcorn_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Popcorn_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Popcorn_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Popcorn_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Popcorn_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Popcorn_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Popcorn_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Popcorn_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Popcorn_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Popcorn_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Popcorn_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Popcorn_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Popcorn_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Popcorn_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Popcorn_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Popcorn_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Popcorn_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Popcorn_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Popcorn_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Popcorn_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Popcorn_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Popcorn_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Popcorn_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_ArmsCrossed_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_ArmsCrossed_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_ArmsCrossed_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_ArmsCrossed_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_ArmsCrossed_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_ArmsCrossed_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_ArmsCrossed_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_ArmsCrossed_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_ArmsCrossed_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_ArmsCrossed_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_ArmsCrossed_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_ArmsCrossed_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_ArmsCrossed_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_ArmsCrossed_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ArmsCrossed_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_ArmsCrossed_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_HeadDown_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_HeadDown_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_HeadDown_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_HeadDown_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_HeadDown_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_HeadDown_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_HeadDown_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_HeadDown_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_HeadDown_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_HeadDown_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_HeadDown_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_HeadDown_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_HeadDown_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_HeadDown_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_HeadDown_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_HeadDown_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_HeadDown_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_HeadDown_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_HeadDown_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_HeadDown_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HeadDown_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_HeadDown_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Nauseous_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Nauseous_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Nauseous_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Nauseous_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_Nauseous_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Nauseous_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Nauseous_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Nauseous_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Nauseous_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Nauseous_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Nauseous_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Nauseous_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Nauseous_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Nauseous_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Nauseous_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Nauseous_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Nauseous_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Nauseous_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Nauseous_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Nauseous_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Nauseous_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Nauseous_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_VeryNauseous_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_VeryNauseous_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_VeryNauseous_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_VeryNauseous_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_VeryNauseous_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_VeryNauseous_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_VeryNauseous_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_VeryNauseous_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_VeryNauseous_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_VeryNauseous_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_VeryNauseous_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_VeryNauseous_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_VeryNauseous_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_VeryNauseous_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_VeryNauseous_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_VeryNauseous_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_VeryNauseous_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_VeryNauseous_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_RequireBathroom_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_RequireBathroom_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_RequireBathroom_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_RequireBathroom_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_RequireBathroom_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_RequireBathroom_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_RequireBathroom_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_RequireBathroom_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_RequireBathroom_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_RequireBathroom_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_RequireBathroom_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_RequireBathroom_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_RequireBathroom_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_RequireBathroom_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_RequireBathroom_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_RequireBathroom_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_RequireBathroom_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_RequireBathroom_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Hat_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Hat_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Hat_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Hat_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_Hat_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Hat_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Hat_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Hat_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Hat_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_Hat_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Hat_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Hat_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Hat_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Hat_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Hat_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Hat_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Hat_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Hat_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Hat_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Hat_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Hat_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Hat_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Hat_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_HotDog_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_HotDog_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_HotDog_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_HotDog_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_HotDog_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_HotDog_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_HotDog_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_HotDog_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_HotDog_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_HotDog_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_HotDog_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_HotDog_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_HotDog_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_HotDog_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_HotDog_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_HotDog_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_HotDog_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_HotDog_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_HotDog_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_HotDog_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_HotDog_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_HotDog_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_HotDog_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Tentacle_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
static const uint8 PeepSpriteImage_Tentacle_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Tentacle_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Tentacle_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };
static const uint8 PeepSpriteImage_Tentacle_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Tentacle_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Tentacle_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Tentacle_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Tentacle_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };
static const uint8 PeepSpriteImage_Tentacle_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Tentacle_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Tentacle_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Tentacle_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Tentacle_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Tentacle_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Tentacle_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Tentacle_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Tentacle_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Tentacle_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Tentacle_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Tentacle_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Tentacle_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Tentacle_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_ToffeeApple_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_ToffeeApple_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_ToffeeApple_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_ToffeeApple_03_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_ToffeeApple_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_ToffeeApple_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_ToffeeApple_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_ToffeeApple_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_ToffeeApple_08_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_ToffeeApple_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_ToffeeApple_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_ToffeeApple_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_ToffeeApple_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_ToffeeApple_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_ToffeeApple_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_ToffeeApple_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_ToffeeApple_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_ToffeeApple_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_ToffeeApple_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Donut_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Donut_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Donut_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Donut_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Donut_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Donut_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Donut_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Donut_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Donut_08_sequence[] = { 0, 1, 2, 3, 3, 3, 3, 3, 3, 4, 5 };
static const uint8 PeepSpriteImage_Donut_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Donut_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Donut_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Donut_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Donut_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Donut_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Donut_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Donut_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Donut_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Donut_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Donut_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Donut_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Donut_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Donut_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Coffee_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Coffee_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Coffee_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Coffee_03_sequence[] = { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1 };
static const uint8 PeepSpriteImage_Coffee_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Coffee_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Coffee_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Coffee_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Coffee_08_sequence[] = { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1 };
static const uint8 PeepSpriteImage_Coffee_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Coffee_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Coffee_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Coffee_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Coffee_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Coffee_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Coffee_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Coffee_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Coffee_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Coffee_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Coffee_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Coffee_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Coffee_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Coffee_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Chicken_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Chicken_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Chicken_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Chicken_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Chicken_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Chicken_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Chicken_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Chicken_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Chicken_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Chicken_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Chicken_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Chicken_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Chicken_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Chicken_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Chicken_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Chicken_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Chicken_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Chicken_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Chicken_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Chicken_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Chicken_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Chicken_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Chicken_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Lemonade_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Lemonade_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Lemonade_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Lemonade_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Lemonade_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Lemonade_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Lemonade_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Lemonade_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Lemonade_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Lemonade_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Lemonade_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Lemonade_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Lemonade_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Lemonade_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Lemonade_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Lemonade_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Lemonade_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Lemonade_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Lemonade_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Lemonade_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Lemonade_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Lemonade_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Lemonade_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Watching_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Watching_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Watching_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Watching_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_Watching_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Watching_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Watching_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Watching_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Watching_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_Watching_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Watching_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Watching_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Watching_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Watching_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Watching_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Watching_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Watching_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Watching_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Watching_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Watching_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Watching_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Watching_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Watching_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Pretzel_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Pretzel_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Pretzel_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Pretzel_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Pretzel_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Pretzel_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Pretzel_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Pretzel_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Pretzel_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Pretzel_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Pretzel_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Pretzel_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Pretzel_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Pretzel_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Pretzel_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Pretzel_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Pretzel_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Pretzel_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Pretzel_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Pretzel_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Pretzel_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Pretzel_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Pretzel_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Sunglasses_00_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Sunglasses_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Sunglasses_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Sunglasses_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10 };
static const uint8 PeepSpriteImage_Sunglasses_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Sunglasses_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Sunglasses_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Sunglasses_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Sunglasses_08_sequence[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Sunglasses_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Sunglasses_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Sunglasses_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Sunglasses_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Sunglasses_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Sunglasses_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Sunglasses_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Sunglasses_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Sunglasses_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Sunglasses_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Sunglasses_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sunglasses_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Sunglasses_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_SuJongkwa_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_SuJongkwa_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_SuJongkwa_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_SuJongkwa_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_SuJongkwa_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_SuJongkwa_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_SuJongkwa_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_SuJongkwa_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_SuJongkwa_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_SuJongkwa_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_SuJongkwa_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_SuJongkwa_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_SuJongkwa_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_SuJongkwa_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_SuJongkwa_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_SuJongkwa_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_SuJongkwa_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_SuJongkwa_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_SuJongkwa_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Juice_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Juice_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Juice_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Juice_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Juice_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Juice_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Juice_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Juice_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Juice_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Juice_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Juice_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Juice_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Juice_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Juice_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Juice_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Juice_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Juice_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Juice_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Juice_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Juice_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Juice_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Juice_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Juice_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_FunnelCake_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_FunnelCake_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_FunnelCake_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_FunnelCake_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_FunnelCake_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_FunnelCake_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_FunnelCake_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_FunnelCake_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_FunnelCake_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_FunnelCake_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_FunnelCake_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_FunnelCake_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_FunnelCake_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_FunnelCake_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_FunnelCake_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_FunnelCake_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_FunnelCake_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_FunnelCake_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_FunnelCake_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_FunnelCake_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_FunnelCake_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_FunnelCake_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_FunnelCake_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Noodles_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Noodles_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Noodles_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Noodles_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Noodles_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Noodles_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Noodles_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Noodles_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Noodles_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Noodles_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Noodles_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Noodles_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Noodles_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Noodles_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Noodles_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Noodles_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Noodles_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Noodles_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Noodles_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Noodles_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Noodles_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Noodles_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Noodles_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Sausage_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Sausage_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Sausage_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Sausage_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Sausage_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Sausage_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Sausage_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Sausage_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Sausage_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Sausage_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Sausage_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Sausage_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Sausage_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Sausage_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Sausage_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Sausage_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Sausage_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Sausage_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Sausage_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Sausage_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Sausage_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sausage_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Sausage_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Soup_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Soup_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Soup_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Soup_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Soup_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Soup_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Soup_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Soup_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Soup_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Soup_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Soup_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Soup_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Soup_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Soup_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Soup_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Soup_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Soup_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Soup_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Soup_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Soup_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Soup_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Soup_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Soup_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };

static const uint8 PeepSpriteImage_Sandwich_00_sequence[] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
static const uint8 PeepSpriteImage_Sandwich_01_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8 };
static const uint8 PeepSpriteImage_Sandwich_02_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Sandwich_03_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Sandwich_04_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
static const uint8 PeepSpriteImage_Sandwich_05_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
static const uint8 PeepSpriteImage_Sandwich_06_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Sandwich_07_sequence[] = { 0 };
static const uint8 PeepSpriteImage_Sandwich_08_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Sandwich_09_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Sandwich_10_sequence[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
static const uint8 PeepSpriteImage_Sandwich_11_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const uint8 PeepSpriteImage_Sandwich_12_sequence[] = { 0, 1, 2, 3, 4, 5 };
static const uint8 PeepSpriteImage_Sandwich_13_sequence[] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_14_sequence[] = { 8, 8, 9, 9, 10, 10, 9, 9, 8, 8, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 13, 13, 8, 8, 9, 9, 8, 8 };
static const uint8 PeepSpriteImage_Sandwich_15_sequence[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11 };
static const uint8 PeepSpriteImage_Sandwich_16_sequence[] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
static const uint8 PeepSpriteImage_Sandwich_17_sequence[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, 6, 7 };
static const uint8 PeepSpriteImage_Sandwich_18_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 17, 18, 18, 18, 18, 18, 18, 18, 17, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 15, 16, 15, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_19_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 16, 15, 14, 14, 14, 14, 14, 14, 19, 20, 20, 20, 19, 14, 14, 14, 14, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_20_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 14, 15, 15, 15, 16, 17, 17, 17, 14, 11, 12, 13, 12, 13, 12, 11, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_21_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_22_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 21, 22, 21, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 29, 30, 31, 32, 31, 30, 31, 32, 31, 30, 31, 32, 31, 30, 29, 0 };
static const uint8 PeepSpriteImage_Sandwich_23_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 16, 17, 18, 19, 18, 17, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_24_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 11, 12, 13, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 15, 16, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 18, 19, 20, 21, 22, 21, 20, 21, 22, 21, 20, 21, 22, 20, 19, 18, 0, 0, 23, 24, 25, 26, 27, 28, 28, 26, 24, 0, 0, 0, 0, 0, 0 };
static const uint8 PeepSpriteImage_Sandwich_25_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_26_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const uint8 PeepSpriteImage_Sandwich_27_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 11, 12, 11, 10, 10, 10, 13, 14, 14, 14, 13, 10, 10, 10, 11, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_28_sequence[] = { 0, 1, 2, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_29_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_30_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 5, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_31_sequence[] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 6, 7, 6, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_32_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 6, 7, 8, 7, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_33_sequence[] = { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_34_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 15, 14, 8, 9, 10, 11, 12, 13, 8, 9, 10, 11, 12, 13, 7, 6, 5, 4, 3, 2, 1, 0 };
static const uint8 PeepSpriteImage_Sandwich_35_sequence[] = { 0, 1, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 4 };
static const uint8 PeepSpriteImage_Sandwich_36_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11, 12, 11, 12, 11, 12, 11, 12, 11, 11, 11, 11, 11, 13, 14, 15 };


static const rct_peep_animation PeepSpriteImage_Normal[] = {
	{ 6409,  countof(PeepSpriteImage_Normal_00_sequence), PeepSpriteImage_Normal_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Normal_01_sequence), PeepSpriteImage_Normal_01_sequence },
	{ 6621,  countof(PeepSpriteImage_Normal_02_sequence), PeepSpriteImage_Normal_02_sequence },
	{ 6653,  countof(PeepSpriteImage_Normal_03_sequence), PeepSpriteImage_Normal_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Normal_04_sequence), PeepSpriteImage_Normal_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Normal_05_sequence), PeepSpriteImage_Normal_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Normal_06_sequence), PeepSpriteImage_Normal_06_sequence },
	{ 6625,  countof(PeepSpriteImage_Normal_07_sequence), PeepSpriteImage_Normal_07_sequence },
	{ 6629,  countof(PeepSpriteImage_Normal_08_sequence), PeepSpriteImage_Normal_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Normal_09_sequence), PeepSpriteImage_Normal_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Normal_10_sequence), PeepSpriteImage_Normal_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Normal_11_sequence), PeepSpriteImage_Normal_11_sequence },
	{ 11301, countof(PeepSpriteImage_Normal_12_sequence), PeepSpriteImage_Normal_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Normal_13_sequence), PeepSpriteImage_Normal_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Normal_14_sequence), PeepSpriteImage_Normal_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Normal_15_sequence), PeepSpriteImage_Normal_15_sequence },
	{ 11325, countof(PeepSpriteImage_Normal_16_sequence), PeepSpriteImage_Normal_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Normal_17_sequence), PeepSpriteImage_Normal_17_sequence },
	{ 11513, countof(PeepSpriteImage_Normal_18_sequence), PeepSpriteImage_Normal_18_sequence },
	{ 11513, countof(PeepSpriteImage_Normal_19_sequence), PeepSpriteImage_Normal_19_sequence },
	{ 11597, countof(PeepSpriteImage_Normal_20_sequence), PeepSpriteImage_Normal_20_sequence },
	{ 11669, countof(PeepSpriteImage_Normal_21_sequence), PeepSpriteImage_Normal_21_sequence },
	{ 11669, countof(PeepSpriteImage_Normal_22_sequence), PeepSpriteImage_Normal_22_sequence },
	{ 11801, countof(PeepSpriteImage_Normal_23_sequence), PeepSpriteImage_Normal_23_sequence },
	{ 11669, countof(PeepSpriteImage_Normal_24_sequence), PeepSpriteImage_Normal_24_sequence },
	{ 11377, countof(PeepSpriteImage_Normal_25_sequence), PeepSpriteImage_Normal_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Normal_26_sequence), PeepSpriteImage_Normal_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Normal_27_sequence), PeepSpriteImage_Normal_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Normal_28_sequence), PeepSpriteImage_Normal_28_sequence },
	{ 11401, countof(PeepSpriteImage_Normal_29_sequence), PeepSpriteImage_Normal_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Normal_30_sequence), PeepSpriteImage_Normal_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Normal_31_sequence), PeepSpriteImage_Normal_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Normal_32_sequence), PeepSpriteImage_Normal_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Normal_33_sequence), PeepSpriteImage_Normal_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Normal_34_sequence), PeepSpriteImage_Normal_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Normal_35_sequence), PeepSpriteImage_Normal_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Normal_36_sequence), PeepSpriteImage_Normal_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Handyman[] = {
	{ 11261, countof(PeepSpriteImage_Handyman_00_sequence), PeepSpriteImage_Handyman_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Handyman_01_sequence), PeepSpriteImage_Handyman_01_sequence },
	{ 11285, countof(PeepSpriteImage_Handyman_02_sequence), PeepSpriteImage_Handyman_02_sequence },
	{ 6653,  countof(PeepSpriteImage_Handyman_03_sequence), PeepSpriteImage_Handyman_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Handyman_04_sequence), PeepSpriteImage_Handyman_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Handyman_05_sequence), PeepSpriteImage_Handyman_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Handyman_06_sequence), PeepSpriteImage_Handyman_06_sequence },
	{ 6625,  countof(PeepSpriteImage_Handyman_07_sequence), PeepSpriteImage_Handyman_07_sequence },
	{ 6629,  countof(PeepSpriteImage_Handyman_08_sequence), PeepSpriteImage_Handyman_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Handyman_09_sequence), PeepSpriteImage_Handyman_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Handyman_10_sequence), PeepSpriteImage_Handyman_10_sequence },
	{ 11289, countof(PeepSpriteImage_Handyman_11_sequence), PeepSpriteImage_Handyman_11_sequence },
	{ 11301, countof(PeepSpriteImage_Handyman_12_sequence), PeepSpriteImage_Handyman_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Handyman_13_sequence), PeepSpriteImage_Handyman_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Handyman_14_sequence), PeepSpriteImage_Handyman_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Handyman_15_sequence), PeepSpriteImage_Handyman_15_sequence },
	{ 11325, countof(PeepSpriteImage_Handyman_16_sequence), PeepSpriteImage_Handyman_16_sequence },
	{ 11345, countof(PeepSpriteImage_Handyman_17_sequence), PeepSpriteImage_Handyman_17_sequence },
	{ 11513, countof(PeepSpriteImage_Handyman_18_sequence), PeepSpriteImage_Handyman_18_sequence },
	{ 11513, countof(PeepSpriteImage_Handyman_19_sequence), PeepSpriteImage_Handyman_19_sequence },
	{ 11597, countof(PeepSpriteImage_Handyman_20_sequence), PeepSpriteImage_Handyman_20_sequence },
	{ 11669, countof(PeepSpriteImage_Handyman_21_sequence), PeepSpriteImage_Handyman_21_sequence },
	{ 11669, countof(PeepSpriteImage_Handyman_22_sequence), PeepSpriteImage_Handyman_22_sequence },
	{ 11801, countof(PeepSpriteImage_Handyman_23_sequence), PeepSpriteImage_Handyman_23_sequence },
	{ 11669, countof(PeepSpriteImage_Handyman_24_sequence), PeepSpriteImage_Handyman_24_sequence },
	{ 11377, countof(PeepSpriteImage_Handyman_25_sequence), PeepSpriteImage_Handyman_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Handyman_26_sequence), PeepSpriteImage_Handyman_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Handyman_27_sequence), PeepSpriteImage_Handyman_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Handyman_28_sequence), PeepSpriteImage_Handyman_28_sequence },
	{ 11401, countof(PeepSpriteImage_Handyman_29_sequence), PeepSpriteImage_Handyman_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Handyman_30_sequence), PeepSpriteImage_Handyman_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Handyman_31_sequence), PeepSpriteImage_Handyman_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Handyman_32_sequence), PeepSpriteImage_Handyman_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Handyman_33_sequence), PeepSpriteImage_Handyman_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Handyman_34_sequence), PeepSpriteImage_Handyman_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Handyman_35_sequence), PeepSpriteImage_Handyman_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Handyman_36_sequence), PeepSpriteImage_Handyman_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Mechanic[] = {
	{ 11441, countof(PeepSpriteImage_Mechanic_00_sequence), PeepSpriteImage_Mechanic_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Mechanic_01_sequence), PeepSpriteImage_Mechanic_01_sequence },
	{ 11465, countof(PeepSpriteImage_Mechanic_02_sequence), PeepSpriteImage_Mechanic_02_sequence },
	{ 6653,  countof(PeepSpriteImage_Mechanic_03_sequence), PeepSpriteImage_Mechanic_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Mechanic_04_sequence), PeepSpriteImage_Mechanic_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Mechanic_05_sequence), PeepSpriteImage_Mechanic_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Mechanic_06_sequence), PeepSpriteImage_Mechanic_06_sequence },
	{ 6625,  countof(PeepSpriteImage_Mechanic_07_sequence), PeepSpriteImage_Mechanic_07_sequence },
	{ 6629,  countof(PeepSpriteImage_Mechanic_08_sequence), PeepSpriteImage_Mechanic_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Mechanic_09_sequence), PeepSpriteImage_Mechanic_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Mechanic_10_sequence), PeepSpriteImage_Mechanic_10_sequence },
	{ 11469, countof(PeepSpriteImage_Mechanic_11_sequence), PeepSpriteImage_Mechanic_11_sequence },
	{ 11301, countof(PeepSpriteImage_Mechanic_12_sequence), PeepSpriteImage_Mechanic_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Mechanic_13_sequence), PeepSpriteImage_Mechanic_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Mechanic_14_sequence), PeepSpriteImage_Mechanic_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Mechanic_15_sequence), PeepSpriteImage_Mechanic_15_sequence },
	{ 11325, countof(PeepSpriteImage_Mechanic_16_sequence), PeepSpriteImage_Mechanic_16_sequence },
	{ 11481, countof(PeepSpriteImage_Mechanic_17_sequence), PeepSpriteImage_Mechanic_17_sequence },
	{ 11513, countof(PeepSpriteImage_Mechanic_18_sequence), PeepSpriteImage_Mechanic_18_sequence },
	{ 11513, countof(PeepSpriteImage_Mechanic_19_sequence), PeepSpriteImage_Mechanic_19_sequence },
	{ 11597, countof(PeepSpriteImage_Mechanic_20_sequence), PeepSpriteImage_Mechanic_20_sequence },
	{ 11669, countof(PeepSpriteImage_Mechanic_21_sequence), PeepSpriteImage_Mechanic_21_sequence },
	{ 11669, countof(PeepSpriteImage_Mechanic_22_sequence), PeepSpriteImage_Mechanic_22_sequence },
	{ 11801, countof(PeepSpriteImage_Mechanic_23_sequence), PeepSpriteImage_Mechanic_23_sequence },
	{ 11669, countof(PeepSpriteImage_Mechanic_24_sequence), PeepSpriteImage_Mechanic_24_sequence },
	{ 11377, countof(PeepSpriteImage_Mechanic_25_sequence), PeepSpriteImage_Mechanic_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Mechanic_26_sequence), PeepSpriteImage_Mechanic_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Mechanic_27_sequence), PeepSpriteImage_Mechanic_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Mechanic_28_sequence), PeepSpriteImage_Mechanic_28_sequence },
	{ 11401, countof(PeepSpriteImage_Mechanic_29_sequence), PeepSpriteImage_Mechanic_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Mechanic_30_sequence), PeepSpriteImage_Mechanic_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Mechanic_31_sequence), PeepSpriteImage_Mechanic_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Mechanic_32_sequence), PeepSpriteImage_Mechanic_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Mechanic_33_sequence), PeepSpriteImage_Mechanic_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Mechanic_34_sequence), PeepSpriteImage_Mechanic_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Mechanic_35_sequence), PeepSpriteImage_Mechanic_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Mechanic_36_sequence), PeepSpriteImage_Mechanic_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Security[] = {
	{ 11881, countof(PeepSpriteImage_Security_00_sequence), PeepSpriteImage_Security_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Security_01_sequence), PeepSpriteImage_Security_01_sequence },
	{ 11905, countof(PeepSpriteImage_Security_02_sequence), PeepSpriteImage_Security_02_sequence },
	{ 6653,  countof(PeepSpriteImage_Security_03_sequence), PeepSpriteImage_Security_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Security_04_sequence), PeepSpriteImage_Security_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Security_05_sequence), PeepSpriteImage_Security_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Security_06_sequence), PeepSpriteImage_Security_06_sequence },
	{ 6625,  countof(PeepSpriteImage_Security_07_sequence), PeepSpriteImage_Security_07_sequence },
	{ 6629,  countof(PeepSpriteImage_Security_08_sequence), PeepSpriteImage_Security_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Security_09_sequence), PeepSpriteImage_Security_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Security_10_sequence), PeepSpriteImage_Security_10_sequence },
	{ 11909, countof(PeepSpriteImage_Security_11_sequence), PeepSpriteImage_Security_11_sequence },
	{ 11301, countof(PeepSpriteImage_Security_12_sequence), PeepSpriteImage_Security_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Security_13_sequence), PeepSpriteImage_Security_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Security_14_sequence), PeepSpriteImage_Security_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Security_15_sequence), PeepSpriteImage_Security_15_sequence },
	{ 11325, countof(PeepSpriteImage_Security_16_sequence), PeepSpriteImage_Security_16_sequence },
	{ 11921, countof(PeepSpriteImage_Security_17_sequence), PeepSpriteImage_Security_17_sequence },
	{ 11513, countof(PeepSpriteImage_Security_18_sequence), PeepSpriteImage_Security_18_sequence },
	{ 11513, countof(PeepSpriteImage_Security_19_sequence), PeepSpriteImage_Security_19_sequence },
	{ 11597, countof(PeepSpriteImage_Security_20_sequence), PeepSpriteImage_Security_20_sequence },
	{ 11669, countof(PeepSpriteImage_Security_21_sequence), PeepSpriteImage_Security_21_sequence },
	{ 11669, countof(PeepSpriteImage_Security_22_sequence), PeepSpriteImage_Security_22_sequence },
	{ 11801, countof(PeepSpriteImage_Security_23_sequence), PeepSpriteImage_Security_23_sequence },
	{ 11669, countof(PeepSpriteImage_Security_24_sequence), PeepSpriteImage_Security_24_sequence },
	{ 11377, countof(PeepSpriteImage_Security_25_sequence), PeepSpriteImage_Security_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Security_26_sequence), PeepSpriteImage_Security_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Security_27_sequence), PeepSpriteImage_Security_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Security_28_sequence), PeepSpriteImage_Security_28_sequence },
	{ 11401, countof(PeepSpriteImage_Security_29_sequence), PeepSpriteImage_Security_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Security_30_sequence), PeepSpriteImage_Security_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Security_31_sequence), PeepSpriteImage_Security_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Security_32_sequence), PeepSpriteImage_Security_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Security_33_sequence), PeepSpriteImage_Security_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Security_34_sequence), PeepSpriteImage_Security_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Security_35_sequence), PeepSpriteImage_Security_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Security_36_sequence), PeepSpriteImage_Security_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_EntertainerPanda[] = {
	{ 11973, countof(PeepSpriteImage_EntertainerPanda_00_sequence), PeepSpriteImage_EntertainerPanda_00_sequence },
	{ 6585,  countof(PeepSpriteImage_EntertainerPanda_01_sequence), PeepSpriteImage_EntertainerPanda_01_sequence },
	{ 11997, countof(PeepSpriteImage_EntertainerPanda_02_sequence), PeepSpriteImage_EntertainerPanda_02_sequence },
	{ 12041, countof(PeepSpriteImage_EntertainerPanda_03_sequence), PeepSpriteImage_EntertainerPanda_03_sequence },
	{ 6697,  countof(PeepSpriteImage_EntertainerPanda_04_sequence), PeepSpriteImage_EntertainerPanda_04_sequence },
	{ 6757,  countof(PeepSpriteImage_EntertainerPanda_05_sequence), PeepSpriteImage_EntertainerPanda_05_sequence },
	{ 6561,  countof(PeepSpriteImage_EntertainerPanda_06_sequence), PeepSpriteImage_EntertainerPanda_06_sequence },
	{ 6625,  countof(PeepSpriteImage_EntertainerPanda_07_sequence), PeepSpriteImage_EntertainerPanda_07_sequence },
	{ 6629,  countof(PeepSpriteImage_EntertainerPanda_08_sequence), PeepSpriteImage_EntertainerPanda_08_sequence },
	{ 6637,  countof(PeepSpriteImage_EntertainerPanda_09_sequence), PeepSpriteImage_EntertainerPanda_09_sequence },
	{ 6645,  countof(PeepSpriteImage_EntertainerPanda_10_sequence), PeepSpriteImage_EntertainerPanda_10_sequence },
	{ 12001, countof(PeepSpriteImage_EntertainerPanda_11_sequence), PeepSpriteImage_EntertainerPanda_11_sequence },
	{ 11301, countof(PeepSpriteImage_EntertainerPanda_12_sequence), PeepSpriteImage_EntertainerPanda_12_sequence },
	{ 6825,  countof(PeepSpriteImage_EntertainerPanda_13_sequence), PeepSpriteImage_EntertainerPanda_13_sequence },
	{ 6845,  countof(PeepSpriteImage_EntertainerPanda_14_sequence), PeepSpriteImage_EntertainerPanda_14_sequence },
	{ 6909,  countof(PeepSpriteImage_EntertainerPanda_15_sequence), PeepSpriteImage_EntertainerPanda_15_sequence },
	{ 11325, countof(PeepSpriteImage_EntertainerPanda_16_sequence), PeepSpriteImage_EntertainerPanda_16_sequence },
	{ 12013, countof(PeepSpriteImage_EntertainerPanda_17_sequence), PeepSpriteImage_EntertainerPanda_17_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerPanda_18_sequence), PeepSpriteImage_EntertainerPanda_18_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerPanda_19_sequence), PeepSpriteImage_EntertainerPanda_19_sequence },
	{ 11597, countof(PeepSpriteImage_EntertainerPanda_20_sequence), PeepSpriteImage_EntertainerPanda_20_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerPanda_21_sequence), PeepSpriteImage_EntertainerPanda_21_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerPanda_22_sequence), PeepSpriteImage_EntertainerPanda_22_sequence },
	{ 11801, countof(PeepSpriteImage_EntertainerPanda_23_sequence), PeepSpriteImage_EntertainerPanda_23_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerPanda_24_sequence), PeepSpriteImage_EntertainerPanda_24_sequence },
	{ 11377, countof(PeepSpriteImage_EntertainerPanda_25_sequence), PeepSpriteImage_EntertainerPanda_25_sequence },
	{ 12065, countof(PeepSpriteImage_EntertainerPanda_26_sequence), PeepSpriteImage_EntertainerPanda_26_sequence },
	{ 7041,  countof(PeepSpriteImage_EntertainerPanda_27_sequence), PeepSpriteImage_EntertainerPanda_27_sequence },
	{ 7101,  countof(PeepSpriteImage_EntertainerPanda_28_sequence), PeepSpriteImage_EntertainerPanda_28_sequence },
	{ 11401, countof(PeepSpriteImage_EntertainerPanda_29_sequence), PeepSpriteImage_EntertainerPanda_29_sequence },
	{ 12041, countof(PeepSpriteImage_EntertainerPanda_30_sequence), PeepSpriteImage_EntertainerPanda_30_sequence },
	{ 7149,  countof(PeepSpriteImage_EntertainerPanda_31_sequence), PeepSpriteImage_EntertainerPanda_31_sequence },
	{ 7249,  countof(PeepSpriteImage_EntertainerPanda_32_sequence), PeepSpriteImage_EntertainerPanda_32_sequence },
	{ 7309,  countof(PeepSpriteImage_EntertainerPanda_33_sequence), PeepSpriteImage_EntertainerPanda_33_sequence },
	{ 7329,  countof(PeepSpriteImage_EntertainerPanda_34_sequence), PeepSpriteImage_EntertainerPanda_34_sequence },
	{ 7285,  countof(PeepSpriteImage_EntertainerPanda_35_sequence), PeepSpriteImage_EntertainerPanda_35_sequence },
	{ 7181,  countof(PeepSpriteImage_EntertainerPanda_36_sequence), PeepSpriteImage_EntertainerPanda_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_EntertainerTiger[] = {
	{ 12129, countof(PeepSpriteImage_EntertainerTiger_00_sequence), PeepSpriteImage_EntertainerTiger_00_sequence },
	{ 6585,  countof(PeepSpriteImage_EntertainerTiger_01_sequence), PeepSpriteImage_EntertainerTiger_01_sequence },
	{ 12153, countof(PeepSpriteImage_EntertainerTiger_02_sequence), PeepSpriteImage_EntertainerTiger_02_sequence },
	{ 12197, countof(PeepSpriteImage_EntertainerTiger_03_sequence), PeepSpriteImage_EntertainerTiger_03_sequence },
	{ 6697,  countof(PeepSpriteImage_EntertainerTiger_04_sequence), PeepSpriteImage_EntertainerTiger_04_sequence },
	{ 6757,  countof(PeepSpriteImage_EntertainerTiger_05_sequence), PeepSpriteImage_EntertainerTiger_05_sequence },
	{ 6561,  countof(PeepSpriteImage_EntertainerTiger_06_sequence), PeepSpriteImage_EntertainerTiger_06_sequence },
	{ 6625,  countof(PeepSpriteImage_EntertainerTiger_07_sequence), PeepSpriteImage_EntertainerTiger_07_sequence },
	{ 6629,  countof(PeepSpriteImage_EntertainerTiger_08_sequence), PeepSpriteImage_EntertainerTiger_08_sequence },
	{ 6637,  countof(PeepSpriteImage_EntertainerTiger_09_sequence), PeepSpriteImage_EntertainerTiger_09_sequence },
	{ 6645,  countof(PeepSpriteImage_EntertainerTiger_10_sequence), PeepSpriteImage_EntertainerTiger_10_sequence },
	{ 12157, countof(PeepSpriteImage_EntertainerTiger_11_sequence), PeepSpriteImage_EntertainerTiger_11_sequence },
	{ 11301, countof(PeepSpriteImage_EntertainerTiger_12_sequence), PeepSpriteImage_EntertainerTiger_12_sequence },
	{ 6825,  countof(PeepSpriteImage_EntertainerTiger_13_sequence), PeepSpriteImage_EntertainerTiger_13_sequence },
	{ 6845,  countof(PeepSpriteImage_EntertainerTiger_14_sequence), PeepSpriteImage_EntertainerTiger_14_sequence },
	{ 6909,  countof(PeepSpriteImage_EntertainerTiger_15_sequence), PeepSpriteImage_EntertainerTiger_15_sequence },
	{ 11325, countof(PeepSpriteImage_EntertainerTiger_16_sequence), PeepSpriteImage_EntertainerTiger_16_sequence },
	{ 12169, countof(PeepSpriteImage_EntertainerTiger_17_sequence), PeepSpriteImage_EntertainerTiger_17_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerTiger_18_sequence), PeepSpriteImage_EntertainerTiger_18_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerTiger_19_sequence), PeepSpriteImage_EntertainerTiger_19_sequence },
	{ 11597, countof(PeepSpriteImage_EntertainerTiger_20_sequence), PeepSpriteImage_EntertainerTiger_20_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerTiger_21_sequence), PeepSpriteImage_EntertainerTiger_21_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerTiger_22_sequence), PeepSpriteImage_EntertainerTiger_22_sequence },
	{ 11801, countof(PeepSpriteImage_EntertainerTiger_23_sequence), PeepSpriteImage_EntertainerTiger_23_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerTiger_24_sequence), PeepSpriteImage_EntertainerTiger_24_sequence },
	{ 11377, countof(PeepSpriteImage_EntertainerTiger_25_sequence), PeepSpriteImage_EntertainerTiger_25_sequence },
	{ 12221, countof(PeepSpriteImage_EntertainerTiger_26_sequence), PeepSpriteImage_EntertainerTiger_26_sequence },
	{ 7041,  countof(PeepSpriteImage_EntertainerTiger_27_sequence), PeepSpriteImage_EntertainerTiger_27_sequence },
	{ 7101,  countof(PeepSpriteImage_EntertainerTiger_28_sequence), PeepSpriteImage_EntertainerTiger_28_sequence },
	{ 11401, countof(PeepSpriteImage_EntertainerTiger_29_sequence), PeepSpriteImage_EntertainerTiger_29_sequence },
	{ 12197, countof(PeepSpriteImage_EntertainerTiger_30_sequence), PeepSpriteImage_EntertainerTiger_30_sequence },
	{ 7149,  countof(PeepSpriteImage_EntertainerTiger_31_sequence), PeepSpriteImage_EntertainerTiger_31_sequence },
	{ 7249,  countof(PeepSpriteImage_EntertainerTiger_32_sequence), PeepSpriteImage_EntertainerTiger_32_sequence },
	{ 7309,  countof(PeepSpriteImage_EntertainerTiger_33_sequence), PeepSpriteImage_EntertainerTiger_33_sequence },
	{ 7329,  countof(PeepSpriteImage_EntertainerTiger_34_sequence), PeepSpriteImage_EntertainerTiger_34_sequence },
	{ 7285,  countof(PeepSpriteImage_EntertainerTiger_35_sequence), PeepSpriteImage_EntertainerTiger_35_sequence },
	{ 7181,  countof(PeepSpriteImage_EntertainerTiger_36_sequence), PeepSpriteImage_EntertainerTiger_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_EntertainerElephant[] = {
	{ 12325, countof(PeepSpriteImage_EntertainerElephant_00_sequence), PeepSpriteImage_EntertainerElephant_00_sequence },
	{ 6585,  countof(PeepSpriteImage_EntertainerElephant_01_sequence), PeepSpriteImage_EntertainerElephant_01_sequence },
	{ 12349, countof(PeepSpriteImage_EntertainerElephant_02_sequence), PeepSpriteImage_EntertainerElephant_02_sequence },
	{ 12365, countof(PeepSpriteImage_EntertainerElephant_03_sequence), PeepSpriteImage_EntertainerElephant_03_sequence },
	{ 6697,  countof(PeepSpriteImage_EntertainerElephant_04_sequence), PeepSpriteImage_EntertainerElephant_04_sequence },
	{ 6757,  countof(PeepSpriteImage_EntertainerElephant_05_sequence), PeepSpriteImage_EntertainerElephant_05_sequence },
	{ 6561,  countof(PeepSpriteImage_EntertainerElephant_06_sequence), PeepSpriteImage_EntertainerElephant_06_sequence },
	{ 6625,  countof(PeepSpriteImage_EntertainerElephant_07_sequence), PeepSpriteImage_EntertainerElephant_07_sequence },
	{ 6629,  countof(PeepSpriteImage_EntertainerElephant_08_sequence), PeepSpriteImage_EntertainerElephant_08_sequence },
	{ 6637,  countof(PeepSpriteImage_EntertainerElephant_09_sequence), PeepSpriteImage_EntertainerElephant_09_sequence },
	{ 6645,  countof(PeepSpriteImage_EntertainerElephant_10_sequence), PeepSpriteImage_EntertainerElephant_10_sequence },
	{ 12353, countof(PeepSpriteImage_EntertainerElephant_11_sequence), PeepSpriteImage_EntertainerElephant_11_sequence },
	{ 11301, countof(PeepSpriteImage_EntertainerElephant_12_sequence), PeepSpriteImage_EntertainerElephant_12_sequence },
	{ 6825,  countof(PeepSpriteImage_EntertainerElephant_13_sequence), PeepSpriteImage_EntertainerElephant_13_sequence },
	{ 6845,  countof(PeepSpriteImage_EntertainerElephant_14_sequence), PeepSpriteImage_EntertainerElephant_14_sequence },
	{ 6909,  countof(PeepSpriteImage_EntertainerElephant_15_sequence), PeepSpriteImage_EntertainerElephant_15_sequence },
	{ 11325, countof(PeepSpriteImage_EntertainerElephant_16_sequence), PeepSpriteImage_EntertainerElephant_16_sequence },
	{ 12393, countof(PeepSpriteImage_EntertainerElephant_17_sequence), PeepSpriteImage_EntertainerElephant_17_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerElephant_18_sequence), PeepSpriteImage_EntertainerElephant_18_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerElephant_19_sequence), PeepSpriteImage_EntertainerElephant_19_sequence },
	{ 11597, countof(PeepSpriteImage_EntertainerElephant_20_sequence), PeepSpriteImage_EntertainerElephant_20_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerElephant_21_sequence), PeepSpriteImage_EntertainerElephant_21_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerElephant_22_sequence), PeepSpriteImage_EntertainerElephant_22_sequence },
	{ 11801, countof(PeepSpriteImage_EntertainerElephant_23_sequence), PeepSpriteImage_EntertainerElephant_23_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerElephant_24_sequence), PeepSpriteImage_EntertainerElephant_24_sequence },
	{ 11377, countof(PeepSpriteImage_EntertainerElephant_25_sequence), PeepSpriteImage_EntertainerElephant_25_sequence },
	{ 12433, countof(PeepSpriteImage_EntertainerElephant_26_sequence), PeepSpriteImage_EntertainerElephant_26_sequence },
	{ 7041,  countof(PeepSpriteImage_EntertainerElephant_27_sequence), PeepSpriteImage_EntertainerElephant_27_sequence },
	{ 7101,  countof(PeepSpriteImage_EntertainerElephant_28_sequence), PeepSpriteImage_EntertainerElephant_28_sequence },
	{ 11401, countof(PeepSpriteImage_EntertainerElephant_29_sequence), PeepSpriteImage_EntertainerElephant_29_sequence },
	{ 12365, countof(PeepSpriteImage_EntertainerElephant_30_sequence), PeepSpriteImage_EntertainerElephant_30_sequence },
	{ 7149,  countof(PeepSpriteImage_EntertainerElephant_31_sequence), PeepSpriteImage_EntertainerElephant_31_sequence },
	{ 7249,  countof(PeepSpriteImage_EntertainerElephant_32_sequence), PeepSpriteImage_EntertainerElephant_32_sequence },
	{ 7309,  countof(PeepSpriteImage_EntertainerElephant_33_sequence), PeepSpriteImage_EntertainerElephant_33_sequence },
	{ 7329,  countof(PeepSpriteImage_EntertainerElephant_34_sequence), PeepSpriteImage_EntertainerElephant_34_sequence },
	{ 7285,  countof(PeepSpriteImage_EntertainerElephant_35_sequence), PeepSpriteImage_EntertainerElephant_35_sequence },
	{ 7181,  countof(PeepSpriteImage_EntertainerElephant_36_sequence), PeepSpriteImage_EntertainerElephant_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_EntertainerRoman[] = {
	{ 13897, countof(PeepSpriteImage_EntertainerRoman_00_sequence), PeepSpriteImage_EntertainerRoman_00_sequence },
	{ 6585,  countof(PeepSpriteImage_EntertainerRoman_01_sequence), PeepSpriteImage_EntertainerRoman_01_sequence },
	{ 13921, countof(PeepSpriteImage_EntertainerRoman_02_sequence), PeepSpriteImage_EntertainerRoman_02_sequence },
	{ 13937, countof(PeepSpriteImage_EntertainerRoman_03_sequence), PeepSpriteImage_EntertainerRoman_03_sequence },
	{ 6697,  countof(PeepSpriteImage_EntertainerRoman_04_sequence), PeepSpriteImage_EntertainerRoman_04_sequence },
	{ 6757,  countof(PeepSpriteImage_EntertainerRoman_05_sequence), PeepSpriteImage_EntertainerRoman_05_sequence },
	{ 6561,  countof(PeepSpriteImage_EntertainerRoman_06_sequence), PeepSpriteImage_EntertainerRoman_06_sequence },
	{ 6625,  countof(PeepSpriteImage_EntertainerRoman_07_sequence), PeepSpriteImage_EntertainerRoman_07_sequence },
	{ 6629,  countof(PeepSpriteImage_EntertainerRoman_08_sequence), PeepSpriteImage_EntertainerRoman_08_sequence },
	{ 6637,  countof(PeepSpriteImage_EntertainerRoman_09_sequence), PeepSpriteImage_EntertainerRoman_09_sequence },
	{ 6645,  countof(PeepSpriteImage_EntertainerRoman_10_sequence), PeepSpriteImage_EntertainerRoman_10_sequence },
	{ 13925, countof(PeepSpriteImage_EntertainerRoman_11_sequence), PeepSpriteImage_EntertainerRoman_11_sequence },
	{ 11301, countof(PeepSpriteImage_EntertainerRoman_12_sequence), PeepSpriteImage_EntertainerRoman_12_sequence },
	{ 6825,  countof(PeepSpriteImage_EntertainerRoman_13_sequence), PeepSpriteImage_EntertainerRoman_13_sequence },
	{ 6845,  countof(PeepSpriteImage_EntertainerRoman_14_sequence), PeepSpriteImage_EntertainerRoman_14_sequence },
	{ 6909,  countof(PeepSpriteImage_EntertainerRoman_15_sequence), PeepSpriteImage_EntertainerRoman_15_sequence },
	{ 11325, countof(PeepSpriteImage_EntertainerRoman_16_sequence), PeepSpriteImage_EntertainerRoman_16_sequence },
	{ 13969, countof(PeepSpriteImage_EntertainerRoman_17_sequence), PeepSpriteImage_EntertainerRoman_17_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerRoman_18_sequence), PeepSpriteImage_EntertainerRoman_18_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerRoman_19_sequence), PeepSpriteImage_EntertainerRoman_19_sequence },
	{ 11597, countof(PeepSpriteImage_EntertainerRoman_20_sequence), PeepSpriteImage_EntertainerRoman_20_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerRoman_21_sequence), PeepSpriteImage_EntertainerRoman_21_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerRoman_22_sequence), PeepSpriteImage_EntertainerRoman_22_sequence },
	{ 11801, countof(PeepSpriteImage_EntertainerRoman_23_sequence), PeepSpriteImage_EntertainerRoman_23_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerRoman_24_sequence), PeepSpriteImage_EntertainerRoman_24_sequence },
	{ 11377, countof(PeepSpriteImage_EntertainerRoman_25_sequence), PeepSpriteImage_EntertainerRoman_25_sequence },
	{ 14029, countof(PeepSpriteImage_EntertainerRoman_26_sequence), PeepSpriteImage_EntertainerRoman_26_sequence },
	{ 7041,  countof(PeepSpriteImage_EntertainerRoman_27_sequence), PeepSpriteImage_EntertainerRoman_27_sequence },
	{ 7101,  countof(PeepSpriteImage_EntertainerRoman_28_sequence), PeepSpriteImage_EntertainerRoman_28_sequence },
	{ 11401, countof(PeepSpriteImage_EntertainerRoman_29_sequence), PeepSpriteImage_EntertainerRoman_29_sequence },
	{ 13937, countof(PeepSpriteImage_EntertainerRoman_30_sequence), PeepSpriteImage_EntertainerRoman_30_sequence },
	{ 7149,  countof(PeepSpriteImage_EntertainerRoman_31_sequence), PeepSpriteImage_EntertainerRoman_31_sequence },
	{ 7249,  countof(PeepSpriteImage_EntertainerRoman_32_sequence), PeepSpriteImage_EntertainerRoman_32_sequence },
	{ 7309,  countof(PeepSpriteImage_EntertainerRoman_33_sequence), PeepSpriteImage_EntertainerRoman_33_sequence },
	{ 7329,  countof(PeepSpriteImage_EntertainerRoman_34_sequence), PeepSpriteImage_EntertainerRoman_34_sequence },
	{ 7285,  countof(PeepSpriteImage_EntertainerRoman_35_sequence), PeepSpriteImage_EntertainerRoman_35_sequence },
	{ 7181,  countof(PeepSpriteImage_EntertainerRoman_36_sequence), PeepSpriteImage_EntertainerRoman_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_EntertainerGorilla[] = {
	{ 12549, countof(PeepSpriteImage_EntertainerGorilla_00_sequence), PeepSpriteImage_EntertainerGorilla_00_sequence },
	{ 6585,  countof(PeepSpriteImage_EntertainerGorilla_01_sequence), PeepSpriteImage_EntertainerGorilla_01_sequence },
	{ 12545, countof(PeepSpriteImage_EntertainerGorilla_02_sequence), PeepSpriteImage_EntertainerGorilla_02_sequence },
	{ 12573, countof(PeepSpriteImage_EntertainerGorilla_03_sequence), PeepSpriteImage_EntertainerGorilla_03_sequence },
	{ 6697,  countof(PeepSpriteImage_EntertainerGorilla_04_sequence), PeepSpriteImage_EntertainerGorilla_04_sequence },
	{ 6757,  countof(PeepSpriteImage_EntertainerGorilla_05_sequence), PeepSpriteImage_EntertainerGorilla_05_sequence },
	{ 6561,  countof(PeepSpriteImage_EntertainerGorilla_06_sequence), PeepSpriteImage_EntertainerGorilla_06_sequence },
	{ 6625,  countof(PeepSpriteImage_EntertainerGorilla_07_sequence), PeepSpriteImage_EntertainerGorilla_07_sequence },
	{ 6629,  countof(PeepSpriteImage_EntertainerGorilla_08_sequence), PeepSpriteImage_EntertainerGorilla_08_sequence },
	{ 6637,  countof(PeepSpriteImage_EntertainerGorilla_09_sequence), PeepSpriteImage_EntertainerGorilla_09_sequence },
	{ 6645,  countof(PeepSpriteImage_EntertainerGorilla_10_sequence), PeepSpriteImage_EntertainerGorilla_10_sequence },
	{ 12665, countof(PeepSpriteImage_EntertainerGorilla_11_sequence), PeepSpriteImage_EntertainerGorilla_11_sequence },
	{ 11301, countof(PeepSpriteImage_EntertainerGorilla_12_sequence), PeepSpriteImage_EntertainerGorilla_12_sequence },
	{ 6825,  countof(PeepSpriteImage_EntertainerGorilla_13_sequence), PeepSpriteImage_EntertainerGorilla_13_sequence },
	{ 6845,  countof(PeepSpriteImage_EntertainerGorilla_14_sequence), PeepSpriteImage_EntertainerGorilla_14_sequence },
	{ 6909,  countof(PeepSpriteImage_EntertainerGorilla_15_sequence), PeepSpriteImage_EntertainerGorilla_15_sequence },
	{ 11325, countof(PeepSpriteImage_EntertainerGorilla_16_sequence), PeepSpriteImage_EntertainerGorilla_16_sequence },
	{ 12617, countof(PeepSpriteImage_EntertainerGorilla_17_sequence), PeepSpriteImage_EntertainerGorilla_17_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerGorilla_18_sequence), PeepSpriteImage_EntertainerGorilla_18_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerGorilla_19_sequence), PeepSpriteImage_EntertainerGorilla_19_sequence },
	{ 11597, countof(PeepSpriteImage_EntertainerGorilla_20_sequence), PeepSpriteImage_EntertainerGorilla_20_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerGorilla_21_sequence), PeepSpriteImage_EntertainerGorilla_21_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerGorilla_22_sequence), PeepSpriteImage_EntertainerGorilla_22_sequence },
	{ 11801, countof(PeepSpriteImage_EntertainerGorilla_23_sequence), PeepSpriteImage_EntertainerGorilla_23_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerGorilla_24_sequence), PeepSpriteImage_EntertainerGorilla_24_sequence },
	{ 11377, countof(PeepSpriteImage_EntertainerGorilla_25_sequence), PeepSpriteImage_EntertainerGorilla_25_sequence },
	{ 12573, countof(PeepSpriteImage_EntertainerGorilla_26_sequence), PeepSpriteImage_EntertainerGorilla_26_sequence },
	{ 7041,  countof(PeepSpriteImage_EntertainerGorilla_27_sequence), PeepSpriteImage_EntertainerGorilla_27_sequence },
	{ 7101,  countof(PeepSpriteImage_EntertainerGorilla_28_sequence), PeepSpriteImage_EntertainerGorilla_28_sequence },
	{ 11401, countof(PeepSpriteImage_EntertainerGorilla_29_sequence), PeepSpriteImage_EntertainerGorilla_29_sequence },
	{ 12573, countof(PeepSpriteImage_EntertainerGorilla_30_sequence), PeepSpriteImage_EntertainerGorilla_30_sequence },
	{ 7149,  countof(PeepSpriteImage_EntertainerGorilla_31_sequence), PeepSpriteImage_EntertainerGorilla_31_sequence },
	{ 7249,  countof(PeepSpriteImage_EntertainerGorilla_32_sequence), PeepSpriteImage_EntertainerGorilla_32_sequence },
	{ 7309,  countof(PeepSpriteImage_EntertainerGorilla_33_sequence), PeepSpriteImage_EntertainerGorilla_33_sequence },
	{ 7329,  countof(PeepSpriteImage_EntertainerGorilla_34_sequence), PeepSpriteImage_EntertainerGorilla_34_sequence },
	{ 7285,  countof(PeepSpriteImage_EntertainerGorilla_35_sequence), PeepSpriteImage_EntertainerGorilla_35_sequence },
	{ 7181,  countof(PeepSpriteImage_EntertainerGorilla_36_sequence), PeepSpriteImage_EntertainerGorilla_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_EntertainerSnowman[] = {
	{ 12681, countof(PeepSpriteImage_EntertainerSnowman_00_sequence), PeepSpriteImage_EntertainerSnowman_00_sequence },
	{ 6585,  countof(PeepSpriteImage_EntertainerSnowman_01_sequence), PeepSpriteImage_EntertainerSnowman_01_sequence },
	{ 12677, countof(PeepSpriteImage_EntertainerSnowman_02_sequence), PeepSpriteImage_EntertainerSnowman_02_sequence },
	{ 12705, countof(PeepSpriteImage_EntertainerSnowman_03_sequence), PeepSpriteImage_EntertainerSnowman_03_sequence },
	{ 6697,  countof(PeepSpriteImage_EntertainerSnowman_04_sequence), PeepSpriteImage_EntertainerSnowman_04_sequence },
	{ 6757,  countof(PeepSpriteImage_EntertainerSnowman_05_sequence), PeepSpriteImage_EntertainerSnowman_05_sequence },
	{ 6561,  countof(PeepSpriteImage_EntertainerSnowman_06_sequence), PeepSpriteImage_EntertainerSnowman_06_sequence },
	{ 6625,  countof(PeepSpriteImage_EntertainerSnowman_07_sequence), PeepSpriteImage_EntertainerSnowman_07_sequence },
	{ 6629,  countof(PeepSpriteImage_EntertainerSnowman_08_sequence), PeepSpriteImage_EntertainerSnowman_08_sequence },
	{ 6637,  countof(PeepSpriteImage_EntertainerSnowman_09_sequence), PeepSpriteImage_EntertainerSnowman_09_sequence },
	{ 6645,  countof(PeepSpriteImage_EntertainerSnowman_10_sequence), PeepSpriteImage_EntertainerSnowman_10_sequence },
	{ 12881, countof(PeepSpriteImage_EntertainerSnowman_11_sequence), PeepSpriteImage_EntertainerSnowman_11_sequence },
	{ 11301, countof(PeepSpriteImage_EntertainerSnowman_12_sequence), PeepSpriteImage_EntertainerSnowman_12_sequence },
	{ 6825,  countof(PeepSpriteImage_EntertainerSnowman_13_sequence), PeepSpriteImage_EntertainerSnowman_13_sequence },
	{ 6845,  countof(PeepSpriteImage_EntertainerSnowman_14_sequence), PeepSpriteImage_EntertainerSnowman_14_sequence },
	{ 6909,  countof(PeepSpriteImage_EntertainerSnowman_15_sequence), PeepSpriteImage_EntertainerSnowman_15_sequence },
	{ 11325, countof(PeepSpriteImage_EntertainerSnowman_16_sequence), PeepSpriteImage_EntertainerSnowman_16_sequence },
	{ 12841, countof(PeepSpriteImage_EntertainerSnowman_17_sequence), PeepSpriteImage_EntertainerSnowman_17_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerSnowman_18_sequence), PeepSpriteImage_EntertainerSnowman_18_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerSnowman_19_sequence), PeepSpriteImage_EntertainerSnowman_19_sequence },
	{ 11597, countof(PeepSpriteImage_EntertainerSnowman_20_sequence), PeepSpriteImage_EntertainerSnowman_20_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerSnowman_21_sequence), PeepSpriteImage_EntertainerSnowman_21_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerSnowman_22_sequence), PeepSpriteImage_EntertainerSnowman_22_sequence },
	{ 11801, countof(PeepSpriteImage_EntertainerSnowman_23_sequence), PeepSpriteImage_EntertainerSnowman_23_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerSnowman_24_sequence), PeepSpriteImage_EntertainerSnowman_24_sequence },
	{ 11377, countof(PeepSpriteImage_EntertainerSnowman_25_sequence), PeepSpriteImage_EntertainerSnowman_25_sequence },
	{ 12705, countof(PeepSpriteImage_EntertainerSnowman_26_sequence), PeepSpriteImage_EntertainerSnowman_26_sequence },
	{ 7041,  countof(PeepSpriteImage_EntertainerSnowman_27_sequence), PeepSpriteImage_EntertainerSnowman_27_sequence },
	{ 7101,  countof(PeepSpriteImage_EntertainerSnowman_28_sequence), PeepSpriteImage_EntertainerSnowman_28_sequence },
	{ 11401, countof(PeepSpriteImage_EntertainerSnowman_29_sequence), PeepSpriteImage_EntertainerSnowman_29_sequence },
	{ 12705, countof(PeepSpriteImage_EntertainerSnowman_30_sequence), PeepSpriteImage_EntertainerSnowman_30_sequence },
	{ 7149,  countof(PeepSpriteImage_EntertainerSnowman_31_sequence), PeepSpriteImage_EntertainerSnowman_31_sequence },
	{ 7249,  countof(PeepSpriteImage_EntertainerSnowman_32_sequence), PeepSpriteImage_EntertainerSnowman_32_sequence },
	{ 7309,  countof(PeepSpriteImage_EntertainerSnowman_33_sequence), PeepSpriteImage_EntertainerSnowman_33_sequence },
	{ 7329,  countof(PeepSpriteImage_EntertainerSnowman_34_sequence), PeepSpriteImage_EntertainerSnowman_34_sequence },
	{ 7285,  countof(PeepSpriteImage_EntertainerSnowman_35_sequence), PeepSpriteImage_EntertainerSnowman_35_sequence },
	{ 7181,  countof(PeepSpriteImage_EntertainerSnowman_36_sequence), PeepSpriteImage_EntertainerSnowman_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_EntertainerKnight[] = {
	{ 12897, countof(PeepSpriteImage_EntertainerKnight_00_sequence), PeepSpriteImage_EntertainerKnight_00_sequence },
	{ 6585,  countof(PeepSpriteImage_EntertainerKnight_01_sequence), PeepSpriteImage_EntertainerKnight_01_sequence },
	{ 12893, countof(PeepSpriteImage_EntertainerKnight_02_sequence), PeepSpriteImage_EntertainerKnight_02_sequence },
	{ 12993, countof(PeepSpriteImage_EntertainerKnight_03_sequence), PeepSpriteImage_EntertainerKnight_03_sequence },
	{ 6697,  countof(PeepSpriteImage_EntertainerKnight_04_sequence), PeepSpriteImage_EntertainerKnight_04_sequence },
	{ 6757,  countof(PeepSpriteImage_EntertainerKnight_05_sequence), PeepSpriteImage_EntertainerKnight_05_sequence },
	{ 6561,  countof(PeepSpriteImage_EntertainerKnight_06_sequence), PeepSpriteImage_EntertainerKnight_06_sequence },
	{ 6625,  countof(PeepSpriteImage_EntertainerKnight_07_sequence), PeepSpriteImage_EntertainerKnight_07_sequence },
	{ 6629,  countof(PeepSpriteImage_EntertainerKnight_08_sequence), PeepSpriteImage_EntertainerKnight_08_sequence },
	{ 6637,  countof(PeepSpriteImage_EntertainerKnight_09_sequence), PeepSpriteImage_EntertainerKnight_09_sequence },
	{ 6645,  countof(PeepSpriteImage_EntertainerKnight_10_sequence), PeepSpriteImage_EntertainerKnight_10_sequence },
	{ 12981, countof(PeepSpriteImage_EntertainerKnight_11_sequence), PeepSpriteImage_EntertainerKnight_11_sequence },
	{ 11301, countof(PeepSpriteImage_EntertainerKnight_12_sequence), PeepSpriteImage_EntertainerKnight_12_sequence },
	{ 6825,  countof(PeepSpriteImage_EntertainerKnight_13_sequence), PeepSpriteImage_EntertainerKnight_13_sequence },
	{ 6845,  countof(PeepSpriteImage_EntertainerKnight_14_sequence), PeepSpriteImage_EntertainerKnight_14_sequence },
	{ 6909,  countof(PeepSpriteImage_EntertainerKnight_15_sequence), PeepSpriteImage_EntertainerKnight_15_sequence },
	{ 11325, countof(PeepSpriteImage_EntertainerKnight_16_sequence), PeepSpriteImage_EntertainerKnight_16_sequence },
	{ 12921, countof(PeepSpriteImage_EntertainerKnight_17_sequence), PeepSpriteImage_EntertainerKnight_17_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerKnight_18_sequence), PeepSpriteImage_EntertainerKnight_18_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerKnight_19_sequence), PeepSpriteImage_EntertainerKnight_19_sequence },
	{ 11597, countof(PeepSpriteImage_EntertainerKnight_20_sequence), PeepSpriteImage_EntertainerKnight_20_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerKnight_21_sequence), PeepSpriteImage_EntertainerKnight_21_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerKnight_22_sequence), PeepSpriteImage_EntertainerKnight_22_sequence },
	{ 11801, countof(PeepSpriteImage_EntertainerKnight_23_sequence), PeepSpriteImage_EntertainerKnight_23_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerKnight_24_sequence), PeepSpriteImage_EntertainerKnight_24_sequence },
	{ 11377, countof(PeepSpriteImage_EntertainerKnight_25_sequence), PeepSpriteImage_EntertainerKnight_25_sequence },
	{ 12993, countof(PeepSpriteImage_EntertainerKnight_26_sequence), PeepSpriteImage_EntertainerKnight_26_sequence },
	{ 7041,  countof(PeepSpriteImage_EntertainerKnight_27_sequence), PeepSpriteImage_EntertainerKnight_27_sequence },
	{ 7101,  countof(PeepSpriteImage_EntertainerKnight_28_sequence), PeepSpriteImage_EntertainerKnight_28_sequence },
	{ 11401, countof(PeepSpriteImage_EntertainerKnight_29_sequence), PeepSpriteImage_EntertainerKnight_29_sequence },
	{ 12993, countof(PeepSpriteImage_EntertainerKnight_30_sequence), PeepSpriteImage_EntertainerKnight_30_sequence },
	{ 7149,  countof(PeepSpriteImage_EntertainerKnight_31_sequence), PeepSpriteImage_EntertainerKnight_31_sequence },
	{ 7249,  countof(PeepSpriteImage_EntertainerKnight_32_sequence), PeepSpriteImage_EntertainerKnight_32_sequence },
	{ 7309,  countof(PeepSpriteImage_EntertainerKnight_33_sequence), PeepSpriteImage_EntertainerKnight_33_sequence },
	{ 7329,  countof(PeepSpriteImage_EntertainerKnight_34_sequence), PeepSpriteImage_EntertainerKnight_34_sequence },
	{ 7285,  countof(PeepSpriteImage_EntertainerKnight_35_sequence), PeepSpriteImage_EntertainerKnight_35_sequence },
	{ 7181,  countof(PeepSpriteImage_EntertainerKnight_36_sequence), PeepSpriteImage_EntertainerKnight_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_EntertainerAstronaut[] = {
	{ 13769, countof(PeepSpriteImage_EntertainerAstronaut_00_sequence), PeepSpriteImage_EntertainerAstronaut_00_sequence },
	{ 6585,  countof(PeepSpriteImage_EntertainerAstronaut_01_sequence), PeepSpriteImage_EntertainerAstronaut_01_sequence },
	{ 13765, countof(PeepSpriteImage_EntertainerAstronaut_02_sequence), PeepSpriteImage_EntertainerAstronaut_02_sequence },
	{ 13857, countof(PeepSpriteImage_EntertainerAstronaut_03_sequence), PeepSpriteImage_EntertainerAstronaut_03_sequence },
	{ 6697,  countof(PeepSpriteImage_EntertainerAstronaut_04_sequence), PeepSpriteImage_EntertainerAstronaut_04_sequence },
	{ 6757,  countof(PeepSpriteImage_EntertainerAstronaut_05_sequence), PeepSpriteImage_EntertainerAstronaut_05_sequence },
	{ 6561,  countof(PeepSpriteImage_EntertainerAstronaut_06_sequence), PeepSpriteImage_EntertainerAstronaut_06_sequence },
	{ 6625,  countof(PeepSpriteImage_EntertainerAstronaut_07_sequence), PeepSpriteImage_EntertainerAstronaut_07_sequence },
	{ 6629,  countof(PeepSpriteImage_EntertainerAstronaut_08_sequence), PeepSpriteImage_EntertainerAstronaut_08_sequence },
	{ 6637,  countof(PeepSpriteImage_EntertainerAstronaut_09_sequence), PeepSpriteImage_EntertainerAstronaut_09_sequence },
	{ 6645,  countof(PeepSpriteImage_EntertainerAstronaut_10_sequence), PeepSpriteImage_EntertainerAstronaut_10_sequence },
	{ 13845, countof(PeepSpriteImage_EntertainerAstronaut_11_sequence), PeepSpriteImage_EntertainerAstronaut_11_sequence },
	{ 11301, countof(PeepSpriteImage_EntertainerAstronaut_12_sequence), PeepSpriteImage_EntertainerAstronaut_12_sequence },
	{ 6825,  countof(PeepSpriteImage_EntertainerAstronaut_13_sequence), PeepSpriteImage_EntertainerAstronaut_13_sequence },
	{ 6845,  countof(PeepSpriteImage_EntertainerAstronaut_14_sequence), PeepSpriteImage_EntertainerAstronaut_14_sequence },
	{ 6909,  countof(PeepSpriteImage_EntertainerAstronaut_15_sequence), PeepSpriteImage_EntertainerAstronaut_15_sequence },
	{ 11325, countof(PeepSpriteImage_EntertainerAstronaut_16_sequence), PeepSpriteImage_EntertainerAstronaut_16_sequence },
	{ 13793, countof(PeepSpriteImage_EntertainerAstronaut_17_sequence), PeepSpriteImage_EntertainerAstronaut_17_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerAstronaut_18_sequence), PeepSpriteImage_EntertainerAstronaut_18_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerAstronaut_19_sequence), PeepSpriteImage_EntertainerAstronaut_19_sequence },
	{ 11597, countof(PeepSpriteImage_EntertainerAstronaut_20_sequence), PeepSpriteImage_EntertainerAstronaut_20_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerAstronaut_21_sequence), PeepSpriteImage_EntertainerAstronaut_21_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerAstronaut_22_sequence), PeepSpriteImage_EntertainerAstronaut_22_sequence },
	{ 11801, countof(PeepSpriteImage_EntertainerAstronaut_23_sequence), PeepSpriteImage_EntertainerAstronaut_23_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerAstronaut_24_sequence), PeepSpriteImage_EntertainerAstronaut_24_sequence },
	{ 11377, countof(PeepSpriteImage_EntertainerAstronaut_25_sequence), PeepSpriteImage_EntertainerAstronaut_25_sequence },
	{ 13857, countof(PeepSpriteImage_EntertainerAstronaut_26_sequence), PeepSpriteImage_EntertainerAstronaut_26_sequence },
	{ 7041,  countof(PeepSpriteImage_EntertainerAstronaut_27_sequence), PeepSpriteImage_EntertainerAstronaut_27_sequence },
	{ 7101,  countof(PeepSpriteImage_EntertainerAstronaut_28_sequence), PeepSpriteImage_EntertainerAstronaut_28_sequence },
	{ 11401, countof(PeepSpriteImage_EntertainerAstronaut_29_sequence), PeepSpriteImage_EntertainerAstronaut_29_sequence },
	{ 13857, countof(PeepSpriteImage_EntertainerAstronaut_30_sequence), PeepSpriteImage_EntertainerAstronaut_30_sequence },
	{ 7149,  countof(PeepSpriteImage_EntertainerAstronaut_31_sequence), PeepSpriteImage_EntertainerAstronaut_31_sequence },
	{ 7249,  countof(PeepSpriteImage_EntertainerAstronaut_32_sequence), PeepSpriteImage_EntertainerAstronaut_32_sequence },
	{ 7309,  countof(PeepSpriteImage_EntertainerAstronaut_33_sequence), PeepSpriteImage_EntertainerAstronaut_33_sequence },
	{ 7329,  countof(PeepSpriteImage_EntertainerAstronaut_34_sequence), PeepSpriteImage_EntertainerAstronaut_34_sequence },
	{ 7285,  countof(PeepSpriteImage_EntertainerAstronaut_35_sequence), PeepSpriteImage_EntertainerAstronaut_35_sequence },
	{ 7181,  countof(PeepSpriteImage_EntertainerAstronaut_36_sequence), PeepSpriteImage_EntertainerAstronaut_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_EntertainerBandit[] = {
	{ 13109, countof(PeepSpriteImage_EntertainerBandit_00_sequence), PeepSpriteImage_EntertainerBandit_00_sequence },
	{ 6585,  countof(PeepSpriteImage_EntertainerBandit_01_sequence), PeepSpriteImage_EntertainerBandit_01_sequence },
	{ 13105, countof(PeepSpriteImage_EntertainerBandit_02_sequence), PeepSpriteImage_EntertainerBandit_02_sequence },
	{ 13209, countof(PeepSpriteImage_EntertainerBandit_03_sequence), PeepSpriteImage_EntertainerBandit_03_sequence },
	{ 6697,  countof(PeepSpriteImage_EntertainerBandit_04_sequence), PeepSpriteImage_EntertainerBandit_04_sequence },
	{ 6757,  countof(PeepSpriteImage_EntertainerBandit_05_sequence), PeepSpriteImage_EntertainerBandit_05_sequence },
	{ 6561,  countof(PeepSpriteImage_EntertainerBandit_06_sequence), PeepSpriteImage_EntertainerBandit_06_sequence },
	{ 6625,  countof(PeepSpriteImage_EntertainerBandit_07_sequence), PeepSpriteImage_EntertainerBandit_07_sequence },
	{ 6629,  countof(PeepSpriteImage_EntertainerBandit_08_sequence), PeepSpriteImage_EntertainerBandit_08_sequence },
	{ 6637,  countof(PeepSpriteImage_EntertainerBandit_09_sequence), PeepSpriteImage_EntertainerBandit_09_sequence },
	{ 6645,  countof(PeepSpriteImage_EntertainerBandit_10_sequence), PeepSpriteImage_EntertainerBandit_10_sequence },
	{ 13197, countof(PeepSpriteImage_EntertainerBandit_11_sequence), PeepSpriteImage_EntertainerBandit_11_sequence },
	{ 11301, countof(PeepSpriteImage_EntertainerBandit_12_sequence), PeepSpriteImage_EntertainerBandit_12_sequence },
	{ 6825,  countof(PeepSpriteImage_EntertainerBandit_13_sequence), PeepSpriteImage_EntertainerBandit_13_sequence },
	{ 6845,  countof(PeepSpriteImage_EntertainerBandit_14_sequence), PeepSpriteImage_EntertainerBandit_14_sequence },
	{ 6909,  countof(PeepSpriteImage_EntertainerBandit_15_sequence), PeepSpriteImage_EntertainerBandit_15_sequence },
	{ 11325, countof(PeepSpriteImage_EntertainerBandit_16_sequence), PeepSpriteImage_EntertainerBandit_16_sequence },
	{ 13133, countof(PeepSpriteImage_EntertainerBandit_17_sequence), PeepSpriteImage_EntertainerBandit_17_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerBandit_18_sequence), PeepSpriteImage_EntertainerBandit_18_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerBandit_19_sequence), PeepSpriteImage_EntertainerBandit_19_sequence },
	{ 11597, countof(PeepSpriteImage_EntertainerBandit_20_sequence), PeepSpriteImage_EntertainerBandit_20_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerBandit_21_sequence), PeepSpriteImage_EntertainerBandit_21_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerBandit_22_sequence), PeepSpriteImage_EntertainerBandit_22_sequence },
	{ 11801, countof(PeepSpriteImage_EntertainerBandit_23_sequence), PeepSpriteImage_EntertainerBandit_23_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerBandit_24_sequence), PeepSpriteImage_EntertainerBandit_24_sequence },
	{ 11377, countof(PeepSpriteImage_EntertainerBandit_25_sequence), PeepSpriteImage_EntertainerBandit_25_sequence },
	{ 13209, countof(PeepSpriteImage_EntertainerBandit_26_sequence), PeepSpriteImage_EntertainerBandit_26_sequence },
	{ 7041,  countof(PeepSpriteImage_EntertainerBandit_27_sequence), PeepSpriteImage_EntertainerBandit_27_sequence },
	{ 7101,  countof(PeepSpriteImage_EntertainerBandit_28_sequence), PeepSpriteImage_EntertainerBandit_28_sequence },
	{ 11401, countof(PeepSpriteImage_EntertainerBandit_29_sequence), PeepSpriteImage_EntertainerBandit_29_sequence },
	{ 13209, countof(PeepSpriteImage_EntertainerBandit_30_sequence), PeepSpriteImage_EntertainerBandit_30_sequence },
	{ 7149,  countof(PeepSpriteImage_EntertainerBandit_31_sequence), PeepSpriteImage_EntertainerBandit_31_sequence },
	{ 7249,  countof(PeepSpriteImage_EntertainerBandit_32_sequence), PeepSpriteImage_EntertainerBandit_32_sequence },
	{ 7309,  countof(PeepSpriteImage_EntertainerBandit_33_sequence), PeepSpriteImage_EntertainerBandit_33_sequence },
	{ 7329,  countof(PeepSpriteImage_EntertainerBandit_34_sequence), PeepSpriteImage_EntertainerBandit_34_sequence },
	{ 7285,  countof(PeepSpriteImage_EntertainerBandit_35_sequence), PeepSpriteImage_EntertainerBandit_35_sequence },
	{ 7181,  countof(PeepSpriteImage_EntertainerBandit_36_sequence), PeepSpriteImage_EntertainerBandit_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_EntertainerSheriff[] = {
	{ 13613, countof(PeepSpriteImage_EntertainerSheriff_00_sequence), PeepSpriteImage_EntertainerSheriff_00_sequence },
	{ 6585,  countof(PeepSpriteImage_EntertainerSheriff_01_sequence), PeepSpriteImage_EntertainerSheriff_01_sequence },
	{ 13609, countof(PeepSpriteImage_EntertainerSheriff_02_sequence), PeepSpriteImage_EntertainerSheriff_02_sequence },
	{ 13713, countof(PeepSpriteImage_EntertainerSheriff_03_sequence), PeepSpriteImage_EntertainerSheriff_03_sequence },
	{ 6697,  countof(PeepSpriteImage_EntertainerSheriff_04_sequence), PeepSpriteImage_EntertainerSheriff_04_sequence },
	{ 6757,  countof(PeepSpriteImage_EntertainerSheriff_05_sequence), PeepSpriteImage_EntertainerSheriff_05_sequence },
	{ 6561,  countof(PeepSpriteImage_EntertainerSheriff_06_sequence), PeepSpriteImage_EntertainerSheriff_06_sequence },
	{ 6625,  countof(PeepSpriteImage_EntertainerSheriff_07_sequence), PeepSpriteImage_EntertainerSheriff_07_sequence },
	{ 6629,  countof(PeepSpriteImage_EntertainerSheriff_08_sequence), PeepSpriteImage_EntertainerSheriff_08_sequence },
	{ 6637,  countof(PeepSpriteImage_EntertainerSheriff_09_sequence), PeepSpriteImage_EntertainerSheriff_09_sequence },
	{ 6645,  countof(PeepSpriteImage_EntertainerSheriff_10_sequence), PeepSpriteImage_EntertainerSheriff_10_sequence },
	{ 13701, countof(PeepSpriteImage_EntertainerSheriff_11_sequence), PeepSpriteImage_EntertainerSheriff_11_sequence },
	{ 11301, countof(PeepSpriteImage_EntertainerSheriff_12_sequence), PeepSpriteImage_EntertainerSheriff_12_sequence },
	{ 6825,  countof(PeepSpriteImage_EntertainerSheriff_13_sequence), PeepSpriteImage_EntertainerSheriff_13_sequence },
	{ 6845,  countof(PeepSpriteImage_EntertainerSheriff_14_sequence), PeepSpriteImage_EntertainerSheriff_14_sequence },
	{ 6909,  countof(PeepSpriteImage_EntertainerSheriff_15_sequence), PeepSpriteImage_EntertainerSheriff_15_sequence },
	{ 11325, countof(PeepSpriteImage_EntertainerSheriff_16_sequence), PeepSpriteImage_EntertainerSheriff_16_sequence },
	{ 13637, countof(PeepSpriteImage_EntertainerSheriff_17_sequence), PeepSpriteImage_EntertainerSheriff_17_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerSheriff_18_sequence), PeepSpriteImage_EntertainerSheriff_18_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerSheriff_19_sequence), PeepSpriteImage_EntertainerSheriff_19_sequence },
	{ 11597, countof(PeepSpriteImage_EntertainerSheriff_20_sequence), PeepSpriteImage_EntertainerSheriff_20_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerSheriff_21_sequence), PeepSpriteImage_EntertainerSheriff_21_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerSheriff_22_sequence), PeepSpriteImage_EntertainerSheriff_22_sequence },
	{ 11801, countof(PeepSpriteImage_EntertainerSheriff_23_sequence), PeepSpriteImage_EntertainerSheriff_23_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerSheriff_24_sequence), PeepSpriteImage_EntertainerSheriff_24_sequence },
	{ 11377, countof(PeepSpriteImage_EntertainerSheriff_25_sequence), PeepSpriteImage_EntertainerSheriff_25_sequence },
	{ 13713, countof(PeepSpriteImage_EntertainerSheriff_26_sequence), PeepSpriteImage_EntertainerSheriff_26_sequence },
	{ 7041,  countof(PeepSpriteImage_EntertainerSheriff_27_sequence), PeepSpriteImage_EntertainerSheriff_27_sequence },
	{ 7101,  countof(PeepSpriteImage_EntertainerSheriff_28_sequence), PeepSpriteImage_EntertainerSheriff_28_sequence },
	{ 11401, countof(PeepSpriteImage_EntertainerSheriff_29_sequence), PeepSpriteImage_EntertainerSheriff_29_sequence },
	{ 13713, countof(PeepSpriteImage_EntertainerSheriff_30_sequence), PeepSpriteImage_EntertainerSheriff_30_sequence },
	{ 7149,  countof(PeepSpriteImage_EntertainerSheriff_31_sequence), PeepSpriteImage_EntertainerSheriff_31_sequence },
	{ 7249,  countof(PeepSpriteImage_EntertainerSheriff_32_sequence), PeepSpriteImage_EntertainerSheriff_32_sequence },
	{ 7309,  countof(PeepSpriteImage_EntertainerSheriff_33_sequence), PeepSpriteImage_EntertainerSheriff_33_sequence },
	{ 7329,  countof(PeepSpriteImage_EntertainerSheriff_34_sequence), PeepSpriteImage_EntertainerSheriff_34_sequence },
	{ 7285,  countof(PeepSpriteImage_EntertainerSheriff_35_sequence), PeepSpriteImage_EntertainerSheriff_35_sequence },
	{ 7181,  countof(PeepSpriteImage_EntertainerSheriff_36_sequence), PeepSpriteImage_EntertainerSheriff_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_EntertainerPirate[] = {
	{ 13381, countof(PeepSpriteImage_EntertainerPirate_00_sequence), PeepSpriteImage_EntertainerPirate_00_sequence },
	{ 6585,  countof(PeepSpriteImage_EntertainerPirate_01_sequence), PeepSpriteImage_EntertainerPirate_01_sequence },
	{ 13377, countof(PeepSpriteImage_EntertainerPirate_02_sequence), PeepSpriteImage_EntertainerPirate_02_sequence },
	{ 13481, countof(PeepSpriteImage_EntertainerPirate_03_sequence), PeepSpriteImage_EntertainerPirate_03_sequence },
	{ 6697,  countof(PeepSpriteImage_EntertainerPirate_04_sequence), PeepSpriteImage_EntertainerPirate_04_sequence },
	{ 6757,  countof(PeepSpriteImage_EntertainerPirate_05_sequence), PeepSpriteImage_EntertainerPirate_05_sequence },
	{ 6561,  countof(PeepSpriteImage_EntertainerPirate_06_sequence), PeepSpriteImage_EntertainerPirate_06_sequence },
	{ 6625,  countof(PeepSpriteImage_EntertainerPirate_07_sequence), PeepSpriteImage_EntertainerPirate_07_sequence },
	{ 6629,  countof(PeepSpriteImage_EntertainerPirate_08_sequence), PeepSpriteImage_EntertainerPirate_08_sequence },
	{ 6637,  countof(PeepSpriteImage_EntertainerPirate_09_sequence), PeepSpriteImage_EntertainerPirate_09_sequence },
	{ 6645,  countof(PeepSpriteImage_EntertainerPirate_10_sequence), PeepSpriteImage_EntertainerPirate_10_sequence },
	{ 13469, countof(PeepSpriteImage_EntertainerPirate_11_sequence), PeepSpriteImage_EntertainerPirate_11_sequence },
	{ 11301, countof(PeepSpriteImage_EntertainerPirate_12_sequence), PeepSpriteImage_EntertainerPirate_12_sequence },
	{ 6825,  countof(PeepSpriteImage_EntertainerPirate_13_sequence), PeepSpriteImage_EntertainerPirate_13_sequence },
	{ 6845,  countof(PeepSpriteImage_EntertainerPirate_14_sequence), PeepSpriteImage_EntertainerPirate_14_sequence },
	{ 6909,  countof(PeepSpriteImage_EntertainerPirate_15_sequence), PeepSpriteImage_EntertainerPirate_15_sequence },
	{ 11325, countof(PeepSpriteImage_EntertainerPirate_16_sequence), PeepSpriteImage_EntertainerPirate_16_sequence },
	{ 13405, countof(PeepSpriteImage_EntertainerPirate_17_sequence), PeepSpriteImage_EntertainerPirate_17_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerPirate_18_sequence), PeepSpriteImage_EntertainerPirate_18_sequence },
	{ 11513, countof(PeepSpriteImage_EntertainerPirate_19_sequence), PeepSpriteImage_EntertainerPirate_19_sequence },
	{ 11597, countof(PeepSpriteImage_EntertainerPirate_20_sequence), PeepSpriteImage_EntertainerPirate_20_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerPirate_21_sequence), PeepSpriteImage_EntertainerPirate_21_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerPirate_22_sequence), PeepSpriteImage_EntertainerPirate_22_sequence },
	{ 11801, countof(PeepSpriteImage_EntertainerPirate_23_sequence), PeepSpriteImage_EntertainerPirate_23_sequence },
	{ 11669, countof(PeepSpriteImage_EntertainerPirate_24_sequence), PeepSpriteImage_EntertainerPirate_24_sequence },
	{ 11377, countof(PeepSpriteImage_EntertainerPirate_25_sequence), PeepSpriteImage_EntertainerPirate_25_sequence },
	{ 13481, countof(PeepSpriteImage_EntertainerPirate_26_sequence), PeepSpriteImage_EntertainerPirate_26_sequence },
	{ 7041,  countof(PeepSpriteImage_EntertainerPirate_27_sequence), PeepSpriteImage_EntertainerPirate_27_sequence },
	{ 7101,  countof(PeepSpriteImage_EntertainerPirate_28_sequence), PeepSpriteImage_EntertainerPirate_28_sequence },
	{ 11401, countof(PeepSpriteImage_EntertainerPirate_29_sequence), PeepSpriteImage_EntertainerPirate_29_sequence },
	{ 13481, countof(PeepSpriteImage_EntertainerPirate_30_sequence), PeepSpriteImage_EntertainerPirate_30_sequence },
	{ 7149,  countof(PeepSpriteImage_EntertainerPirate_31_sequence), PeepSpriteImage_EntertainerPirate_31_sequence },
	{ 7249,  countof(PeepSpriteImage_EntertainerPirate_32_sequence), PeepSpriteImage_EntertainerPirate_32_sequence },
	{ 7309,  countof(PeepSpriteImage_EntertainerPirate_33_sequence), PeepSpriteImage_EntertainerPirate_33_sequence },
	{ 7329,  countof(PeepSpriteImage_EntertainerPirate_34_sequence), PeepSpriteImage_EntertainerPirate_34_sequence },
	{ 7285,  countof(PeepSpriteImage_EntertainerPirate_35_sequence), PeepSpriteImage_EntertainerPirate_35_sequence },
	{ 7181,  countof(PeepSpriteImage_EntertainerPirate_36_sequence), PeepSpriteImage_EntertainerPirate_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_IceCream[] = {
	{ 7505,  countof(PeepSpriteImage_IceCream_00_sequence), PeepSpriteImage_IceCream_00_sequence },
	{ 6585,  countof(PeepSpriteImage_IceCream_01_sequence), PeepSpriteImage_IceCream_01_sequence },
	{ 7501,  countof(PeepSpriteImage_IceCream_02_sequence), PeepSpriteImage_IceCream_02_sequence },
	{ 7581,  countof(PeepSpriteImage_IceCream_03_sequence), PeepSpriteImage_IceCream_03_sequence },
	{ 6697,  countof(PeepSpriteImage_IceCream_04_sequence), PeepSpriteImage_IceCream_04_sequence },
	{ 6757,  countof(PeepSpriteImage_IceCream_05_sequence), PeepSpriteImage_IceCream_05_sequence },
	{ 6561,  countof(PeepSpriteImage_IceCream_06_sequence), PeepSpriteImage_IceCream_06_sequence },
	{ 7553,  countof(PeepSpriteImage_IceCream_07_sequence), PeepSpriteImage_IceCream_07_sequence },
	{ 7557,  countof(PeepSpriteImage_IceCream_08_sequence), PeepSpriteImage_IceCream_08_sequence },
	{ 6637,  countof(PeepSpriteImage_IceCream_09_sequence), PeepSpriteImage_IceCream_09_sequence },
	{ 6645,  countof(PeepSpriteImage_IceCream_10_sequence), PeepSpriteImage_IceCream_10_sequence },
	{ 6813,  countof(PeepSpriteImage_IceCream_11_sequence), PeepSpriteImage_IceCream_11_sequence },
	{ 11301, countof(PeepSpriteImage_IceCream_12_sequence), PeepSpriteImage_IceCream_12_sequence },
	{ 6825,  countof(PeepSpriteImage_IceCream_13_sequence), PeepSpriteImage_IceCream_13_sequence },
	{ 6845,  countof(PeepSpriteImage_IceCream_14_sequence), PeepSpriteImage_IceCream_14_sequence },
	{ 6909,  countof(PeepSpriteImage_IceCream_15_sequence), PeepSpriteImage_IceCream_15_sequence },
	{ 11325, countof(PeepSpriteImage_IceCream_16_sequence), PeepSpriteImage_IceCream_16_sequence },
	{ 6957,  countof(PeepSpriteImage_IceCream_17_sequence), PeepSpriteImage_IceCream_17_sequence },
	{ 11513, countof(PeepSpriteImage_IceCream_18_sequence), PeepSpriteImage_IceCream_18_sequence },
	{ 11513, countof(PeepSpriteImage_IceCream_19_sequence), PeepSpriteImage_IceCream_19_sequence },
	{ 11597, countof(PeepSpriteImage_IceCream_20_sequence), PeepSpriteImage_IceCream_20_sequence },
	{ 11669, countof(PeepSpriteImage_IceCream_21_sequence), PeepSpriteImage_IceCream_21_sequence },
	{ 11669, countof(PeepSpriteImage_IceCream_22_sequence), PeepSpriteImage_IceCream_22_sequence },
	{ 11801, countof(PeepSpriteImage_IceCream_23_sequence), PeepSpriteImage_IceCream_23_sequence },
	{ 11669, countof(PeepSpriteImage_IceCream_24_sequence), PeepSpriteImage_IceCream_24_sequence },
	{ 11377, countof(PeepSpriteImage_IceCream_25_sequence), PeepSpriteImage_IceCream_25_sequence },
	{ 6989,  countof(PeepSpriteImage_IceCream_26_sequence), PeepSpriteImage_IceCream_26_sequence },
	{ 7041,  countof(PeepSpriteImage_IceCream_27_sequence), PeepSpriteImage_IceCream_27_sequence },
	{ 7101,  countof(PeepSpriteImage_IceCream_28_sequence), PeepSpriteImage_IceCream_28_sequence },
	{ 11401, countof(PeepSpriteImage_IceCream_29_sequence), PeepSpriteImage_IceCream_29_sequence },
	{ 7121,  countof(PeepSpriteImage_IceCream_30_sequence), PeepSpriteImage_IceCream_30_sequence },
	{ 7149,  countof(PeepSpriteImage_IceCream_31_sequence), PeepSpriteImage_IceCream_31_sequence },
	{ 7249,  countof(PeepSpriteImage_IceCream_32_sequence), PeepSpriteImage_IceCream_32_sequence },
	{ 7309,  countof(PeepSpriteImage_IceCream_33_sequence), PeepSpriteImage_IceCream_33_sequence },
	{ 7329,  countof(PeepSpriteImage_IceCream_34_sequence), PeepSpriteImage_IceCream_34_sequence },
	{ 7285,  countof(PeepSpriteImage_IceCream_35_sequence), PeepSpriteImage_IceCream_35_sequence },
	{ 7181,  countof(PeepSpriteImage_IceCream_36_sequence), PeepSpriteImage_IceCream_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Chips[] = {
	{ 7609,  countof(PeepSpriteImage_Chips_00_sequence), PeepSpriteImage_Chips_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Chips_01_sequence), PeepSpriteImage_Chips_01_sequence },
	{ 7605,  countof(PeepSpriteImage_Chips_02_sequence), PeepSpriteImage_Chips_02_sequence },
	{ 7733,  countof(PeepSpriteImage_Chips_03_sequence), PeepSpriteImage_Chips_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Chips_04_sequence), PeepSpriteImage_Chips_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Chips_05_sequence), PeepSpriteImage_Chips_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Chips_06_sequence), PeepSpriteImage_Chips_06_sequence },
	{ 7681,  countof(PeepSpriteImage_Chips_07_sequence), PeepSpriteImage_Chips_07_sequence },
	{ 7685,  countof(PeepSpriteImage_Chips_08_sequence), PeepSpriteImage_Chips_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Chips_09_sequence), PeepSpriteImage_Chips_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Chips_10_sequence), PeepSpriteImage_Chips_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Chips_11_sequence), PeepSpriteImage_Chips_11_sequence },
	{ 11301, countof(PeepSpriteImage_Chips_12_sequence), PeepSpriteImage_Chips_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Chips_13_sequence), PeepSpriteImage_Chips_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Chips_14_sequence), PeepSpriteImage_Chips_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Chips_15_sequence), PeepSpriteImage_Chips_15_sequence },
	{ 11325, countof(PeepSpriteImage_Chips_16_sequence), PeepSpriteImage_Chips_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Chips_17_sequence), PeepSpriteImage_Chips_17_sequence },
	{ 11513, countof(PeepSpriteImage_Chips_18_sequence), PeepSpriteImage_Chips_18_sequence },
	{ 11513, countof(PeepSpriteImage_Chips_19_sequence), PeepSpriteImage_Chips_19_sequence },
	{ 11597, countof(PeepSpriteImage_Chips_20_sequence), PeepSpriteImage_Chips_20_sequence },
	{ 11669, countof(PeepSpriteImage_Chips_21_sequence), PeepSpriteImage_Chips_21_sequence },
	{ 11669, countof(PeepSpriteImage_Chips_22_sequence), PeepSpriteImage_Chips_22_sequence },
	{ 11801, countof(PeepSpriteImage_Chips_23_sequence), PeepSpriteImage_Chips_23_sequence },
	{ 11669, countof(PeepSpriteImage_Chips_24_sequence), PeepSpriteImage_Chips_24_sequence },
	{ 11377, countof(PeepSpriteImage_Chips_25_sequence), PeepSpriteImage_Chips_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Chips_26_sequence), PeepSpriteImage_Chips_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Chips_27_sequence), PeepSpriteImage_Chips_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Chips_28_sequence), PeepSpriteImage_Chips_28_sequence },
	{ 11401, countof(PeepSpriteImage_Chips_29_sequence), PeepSpriteImage_Chips_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Chips_30_sequence), PeepSpriteImage_Chips_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Chips_31_sequence), PeepSpriteImage_Chips_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Chips_32_sequence), PeepSpriteImage_Chips_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Chips_33_sequence), PeepSpriteImage_Chips_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Chips_34_sequence), PeepSpriteImage_Chips_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Chips_35_sequence), PeepSpriteImage_Chips_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Chips_36_sequence), PeepSpriteImage_Chips_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Burger[] = {
	{ 8385,  countof(PeepSpriteImage_Burger_00_sequence), PeepSpriteImage_Burger_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Burger_01_sequence), PeepSpriteImage_Burger_01_sequence },
	{ 8381,  countof(PeepSpriteImage_Burger_02_sequence), PeepSpriteImage_Burger_02_sequence },
	{ 8509,  countof(PeepSpriteImage_Burger_03_sequence), PeepSpriteImage_Burger_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Burger_04_sequence), PeepSpriteImage_Burger_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Burger_05_sequence), PeepSpriteImage_Burger_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Burger_06_sequence), PeepSpriteImage_Burger_06_sequence },
	{ 8457,  countof(PeepSpriteImage_Burger_07_sequence), PeepSpriteImage_Burger_07_sequence },
	{ 8461,  countof(PeepSpriteImage_Burger_08_sequence), PeepSpriteImage_Burger_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Burger_09_sequence), PeepSpriteImage_Burger_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Burger_10_sequence), PeepSpriteImage_Burger_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Burger_11_sequence), PeepSpriteImage_Burger_11_sequence },
	{ 11301, countof(PeepSpriteImage_Burger_12_sequence), PeepSpriteImage_Burger_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Burger_13_sequence), PeepSpriteImage_Burger_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Burger_14_sequence), PeepSpriteImage_Burger_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Burger_15_sequence), PeepSpriteImage_Burger_15_sequence },
	{ 11325, countof(PeepSpriteImage_Burger_16_sequence), PeepSpriteImage_Burger_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Burger_17_sequence), PeepSpriteImage_Burger_17_sequence },
	{ 11513, countof(PeepSpriteImage_Burger_18_sequence), PeepSpriteImage_Burger_18_sequence },
	{ 11513, countof(PeepSpriteImage_Burger_19_sequence), PeepSpriteImage_Burger_19_sequence },
	{ 11597, countof(PeepSpriteImage_Burger_20_sequence), PeepSpriteImage_Burger_20_sequence },
	{ 11669, countof(PeepSpriteImage_Burger_21_sequence), PeepSpriteImage_Burger_21_sequence },
	{ 11669, countof(PeepSpriteImage_Burger_22_sequence), PeepSpriteImage_Burger_22_sequence },
	{ 11801, countof(PeepSpriteImage_Burger_23_sequence), PeepSpriteImage_Burger_23_sequence },
	{ 11669, countof(PeepSpriteImage_Burger_24_sequence), PeepSpriteImage_Burger_24_sequence },
	{ 11377, countof(PeepSpriteImage_Burger_25_sequence), PeepSpriteImage_Burger_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Burger_26_sequence), PeepSpriteImage_Burger_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Burger_27_sequence), PeepSpriteImage_Burger_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Burger_28_sequence), PeepSpriteImage_Burger_28_sequence },
	{ 11401, countof(PeepSpriteImage_Burger_29_sequence), PeepSpriteImage_Burger_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Burger_30_sequence), PeepSpriteImage_Burger_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Burger_31_sequence), PeepSpriteImage_Burger_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Burger_32_sequence), PeepSpriteImage_Burger_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Burger_33_sequence), PeepSpriteImage_Burger_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Burger_34_sequence), PeepSpriteImage_Burger_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Burger_35_sequence), PeepSpriteImage_Burger_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Burger_36_sequence), PeepSpriteImage_Burger_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Drink[] = {
	{ 8713,  countof(PeepSpriteImage_Drink_00_sequence), PeepSpriteImage_Drink_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Drink_01_sequence), PeepSpriteImage_Drink_01_sequence },
	{ 8709,  countof(PeepSpriteImage_Drink_02_sequence), PeepSpriteImage_Drink_02_sequence },
	{ 8837,  countof(PeepSpriteImage_Drink_03_sequence), PeepSpriteImage_Drink_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Drink_04_sequence), PeepSpriteImage_Drink_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Drink_05_sequence), PeepSpriteImage_Drink_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Drink_06_sequence), PeepSpriteImage_Drink_06_sequence },
	{ 8785,  countof(PeepSpriteImage_Drink_07_sequence), PeepSpriteImage_Drink_07_sequence },
	{ 8789,  countof(PeepSpriteImage_Drink_08_sequence), PeepSpriteImage_Drink_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Drink_09_sequence), PeepSpriteImage_Drink_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Drink_10_sequence), PeepSpriteImage_Drink_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Drink_11_sequence), PeepSpriteImage_Drink_11_sequence },
	{ 11301, countof(PeepSpriteImage_Drink_12_sequence), PeepSpriteImage_Drink_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Drink_13_sequence), PeepSpriteImage_Drink_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Drink_14_sequence), PeepSpriteImage_Drink_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Drink_15_sequence), PeepSpriteImage_Drink_15_sequence },
	{ 11325, countof(PeepSpriteImage_Drink_16_sequence), PeepSpriteImage_Drink_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Drink_17_sequence), PeepSpriteImage_Drink_17_sequence },
	{ 11513, countof(PeepSpriteImage_Drink_18_sequence), PeepSpriteImage_Drink_18_sequence },
	{ 11513, countof(PeepSpriteImage_Drink_19_sequence), PeepSpriteImage_Drink_19_sequence },
	{ 11597, countof(PeepSpriteImage_Drink_20_sequence), PeepSpriteImage_Drink_20_sequence },
	{ 11669, countof(PeepSpriteImage_Drink_21_sequence), PeepSpriteImage_Drink_21_sequence },
	{ 11669, countof(PeepSpriteImage_Drink_22_sequence), PeepSpriteImage_Drink_22_sequence },
	{ 11801, countof(PeepSpriteImage_Drink_23_sequence), PeepSpriteImage_Drink_23_sequence },
	{ 11669, countof(PeepSpriteImage_Drink_24_sequence), PeepSpriteImage_Drink_24_sequence },
	{ 11377, countof(PeepSpriteImage_Drink_25_sequence), PeepSpriteImage_Drink_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Drink_26_sequence), PeepSpriteImage_Drink_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Drink_27_sequence), PeepSpriteImage_Drink_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Drink_28_sequence), PeepSpriteImage_Drink_28_sequence },
	{ 11401, countof(PeepSpriteImage_Drink_29_sequence), PeepSpriteImage_Drink_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Drink_30_sequence), PeepSpriteImage_Drink_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Drink_31_sequence), PeepSpriteImage_Drink_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Drink_32_sequence), PeepSpriteImage_Drink_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Drink_33_sequence), PeepSpriteImage_Drink_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Drink_34_sequence), PeepSpriteImage_Drink_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Drink_35_sequence), PeepSpriteImage_Drink_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Drink_36_sequence), PeepSpriteImage_Drink_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Balloon[] = {
	{ 10785, countof(PeepSpriteImage_Balloon_00_sequence), PeepSpriteImage_Balloon_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Balloon_01_sequence), PeepSpriteImage_Balloon_01_sequence },
	{ 10781, countof(PeepSpriteImage_Balloon_02_sequence), PeepSpriteImage_Balloon_02_sequence },
	{ 6653,  countof(PeepSpriteImage_Balloon_03_sequence), PeepSpriteImage_Balloon_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Balloon_04_sequence), PeepSpriteImage_Balloon_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Balloon_05_sequence), PeepSpriteImage_Balloon_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Balloon_06_sequence), PeepSpriteImage_Balloon_06_sequence },
	{ 10809, countof(PeepSpriteImage_Balloon_07_sequence), PeepSpriteImage_Balloon_07_sequence },
	{ 6629,  countof(PeepSpriteImage_Balloon_08_sequence), PeepSpriteImage_Balloon_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Balloon_09_sequence), PeepSpriteImage_Balloon_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Balloon_10_sequence), PeepSpriteImage_Balloon_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Balloon_11_sequence), PeepSpriteImage_Balloon_11_sequence },
	{ 11301, countof(PeepSpriteImage_Balloon_12_sequence), PeepSpriteImage_Balloon_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Balloon_13_sequence), PeepSpriteImage_Balloon_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Balloon_14_sequence), PeepSpriteImage_Balloon_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Balloon_15_sequence), PeepSpriteImage_Balloon_15_sequence },
	{ 11325, countof(PeepSpriteImage_Balloon_16_sequence), PeepSpriteImage_Balloon_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Balloon_17_sequence), PeepSpriteImage_Balloon_17_sequence },
	{ 11513, countof(PeepSpriteImage_Balloon_18_sequence), PeepSpriteImage_Balloon_18_sequence },
	{ 11513, countof(PeepSpriteImage_Balloon_19_sequence), PeepSpriteImage_Balloon_19_sequence },
	{ 11597, countof(PeepSpriteImage_Balloon_20_sequence), PeepSpriteImage_Balloon_20_sequence },
	{ 11669, countof(PeepSpriteImage_Balloon_21_sequence), PeepSpriteImage_Balloon_21_sequence },
	{ 11669, countof(PeepSpriteImage_Balloon_22_sequence), PeepSpriteImage_Balloon_22_sequence },
	{ 11801, countof(PeepSpriteImage_Balloon_23_sequence), PeepSpriteImage_Balloon_23_sequence },
	{ 11669, countof(PeepSpriteImage_Balloon_24_sequence), PeepSpriteImage_Balloon_24_sequence },
	{ 11377, countof(PeepSpriteImage_Balloon_25_sequence), PeepSpriteImage_Balloon_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Balloon_26_sequence), PeepSpriteImage_Balloon_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Balloon_27_sequence), PeepSpriteImage_Balloon_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Balloon_28_sequence), PeepSpriteImage_Balloon_28_sequence },
	{ 11401, countof(PeepSpriteImage_Balloon_29_sequence), PeepSpriteImage_Balloon_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Balloon_30_sequence), PeepSpriteImage_Balloon_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Balloon_31_sequence), PeepSpriteImage_Balloon_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Balloon_32_sequence), PeepSpriteImage_Balloon_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Balloon_33_sequence), PeepSpriteImage_Balloon_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Balloon_34_sequence), PeepSpriteImage_Balloon_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Balloon_35_sequence), PeepSpriteImage_Balloon_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Balloon_36_sequence), PeepSpriteImage_Balloon_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Candyfloss[] = {
	{ 10849, countof(PeepSpriteImage_Candyfloss_00_sequence), PeepSpriteImage_Candyfloss_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Candyfloss_01_sequence), PeepSpriteImage_Candyfloss_01_sequence },
	{ 10845, countof(PeepSpriteImage_Candyfloss_02_sequence), PeepSpriteImage_Candyfloss_02_sequence },
	{ 10973, countof(PeepSpriteImage_Candyfloss_03_sequence), PeepSpriteImage_Candyfloss_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Candyfloss_04_sequence), PeepSpriteImage_Candyfloss_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Candyfloss_05_sequence), PeepSpriteImage_Candyfloss_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Candyfloss_06_sequence), PeepSpriteImage_Candyfloss_06_sequence },
	{ 10921, countof(PeepSpriteImage_Candyfloss_07_sequence), PeepSpriteImage_Candyfloss_07_sequence },
	{ 10925, countof(PeepSpriteImage_Candyfloss_08_sequence), PeepSpriteImage_Candyfloss_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Candyfloss_09_sequence), PeepSpriteImage_Candyfloss_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Candyfloss_10_sequence), PeepSpriteImage_Candyfloss_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Candyfloss_11_sequence), PeepSpriteImage_Candyfloss_11_sequence },
	{ 11301, countof(PeepSpriteImage_Candyfloss_12_sequence), PeepSpriteImage_Candyfloss_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Candyfloss_13_sequence), PeepSpriteImage_Candyfloss_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Candyfloss_14_sequence), PeepSpriteImage_Candyfloss_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Candyfloss_15_sequence), PeepSpriteImage_Candyfloss_15_sequence },
	{ 11325, countof(PeepSpriteImage_Candyfloss_16_sequence), PeepSpriteImage_Candyfloss_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Candyfloss_17_sequence), PeepSpriteImage_Candyfloss_17_sequence },
	{ 11513, countof(PeepSpriteImage_Candyfloss_18_sequence), PeepSpriteImage_Candyfloss_18_sequence },
	{ 11513, countof(PeepSpriteImage_Candyfloss_19_sequence), PeepSpriteImage_Candyfloss_19_sequence },
	{ 11597, countof(PeepSpriteImage_Candyfloss_20_sequence), PeepSpriteImage_Candyfloss_20_sequence },
	{ 11669, countof(PeepSpriteImage_Candyfloss_21_sequence), PeepSpriteImage_Candyfloss_21_sequence },
	{ 11669, countof(PeepSpriteImage_Candyfloss_22_sequence), PeepSpriteImage_Candyfloss_22_sequence },
	{ 11801, countof(PeepSpriteImage_Candyfloss_23_sequence), PeepSpriteImage_Candyfloss_23_sequence },
	{ 11669, countof(PeepSpriteImage_Candyfloss_24_sequence), PeepSpriteImage_Candyfloss_24_sequence },
	{ 11377, countof(PeepSpriteImage_Candyfloss_25_sequence), PeepSpriteImage_Candyfloss_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Candyfloss_26_sequence), PeepSpriteImage_Candyfloss_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Candyfloss_27_sequence), PeepSpriteImage_Candyfloss_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Candyfloss_28_sequence), PeepSpriteImage_Candyfloss_28_sequence },
	{ 11401, countof(PeepSpriteImage_Candyfloss_29_sequence), PeepSpriteImage_Candyfloss_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Candyfloss_30_sequence), PeepSpriteImage_Candyfloss_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Candyfloss_31_sequence), PeepSpriteImage_Candyfloss_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Candyfloss_32_sequence), PeepSpriteImage_Candyfloss_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Candyfloss_33_sequence), PeepSpriteImage_Candyfloss_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Candyfloss_34_sequence), PeepSpriteImage_Candyfloss_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Candyfloss_35_sequence), PeepSpriteImage_Candyfloss_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Candyfloss_36_sequence), PeepSpriteImage_Candyfloss_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Umbrella[] = {
	{ 11197, countof(PeepSpriteImage_Umbrella_00_sequence), PeepSpriteImage_Umbrella_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Umbrella_01_sequence), PeepSpriteImage_Umbrella_01_sequence },
	{ 11221, countof(PeepSpriteImage_Umbrella_02_sequence), PeepSpriteImage_Umbrella_02_sequence },
	{ 6653,  countof(PeepSpriteImage_Umbrella_03_sequence), PeepSpriteImage_Umbrella_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Umbrella_04_sequence), PeepSpriteImage_Umbrella_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Umbrella_05_sequence), PeepSpriteImage_Umbrella_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Umbrella_06_sequence), PeepSpriteImage_Umbrella_06_sequence },
	{ 11225, countof(PeepSpriteImage_Umbrella_07_sequence), PeepSpriteImage_Umbrella_07_sequence },
	{ 6629,  countof(PeepSpriteImage_Umbrella_08_sequence), PeepSpriteImage_Umbrella_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Umbrella_09_sequence), PeepSpriteImage_Umbrella_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Umbrella_10_sequence), PeepSpriteImage_Umbrella_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Umbrella_11_sequence), PeepSpriteImage_Umbrella_11_sequence },
	{ 11301, countof(PeepSpriteImage_Umbrella_12_sequence), PeepSpriteImage_Umbrella_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Umbrella_13_sequence), PeepSpriteImage_Umbrella_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Umbrella_14_sequence), PeepSpriteImage_Umbrella_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Umbrella_15_sequence), PeepSpriteImage_Umbrella_15_sequence },
	{ 11325, countof(PeepSpriteImage_Umbrella_16_sequence), PeepSpriteImage_Umbrella_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Umbrella_17_sequence), PeepSpriteImage_Umbrella_17_sequence },
	{ 11513, countof(PeepSpriteImage_Umbrella_18_sequence), PeepSpriteImage_Umbrella_18_sequence },
	{ 11513, countof(PeepSpriteImage_Umbrella_19_sequence), PeepSpriteImage_Umbrella_19_sequence },
	{ 11597, countof(PeepSpriteImage_Umbrella_20_sequence), PeepSpriteImage_Umbrella_20_sequence },
	{ 11669, countof(PeepSpriteImage_Umbrella_21_sequence), PeepSpriteImage_Umbrella_21_sequence },
	{ 11669, countof(PeepSpriteImage_Umbrella_22_sequence), PeepSpriteImage_Umbrella_22_sequence },
	{ 11801, countof(PeepSpriteImage_Umbrella_23_sequence), PeepSpriteImage_Umbrella_23_sequence },
	{ 11669, countof(PeepSpriteImage_Umbrella_24_sequence), PeepSpriteImage_Umbrella_24_sequence },
	{ 11377, countof(PeepSpriteImage_Umbrella_25_sequence), PeepSpriteImage_Umbrella_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Umbrella_26_sequence), PeepSpriteImage_Umbrella_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Umbrella_27_sequence), PeepSpriteImage_Umbrella_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Umbrella_28_sequence), PeepSpriteImage_Umbrella_28_sequence },
	{ 11401, countof(PeepSpriteImage_Umbrella_29_sequence), PeepSpriteImage_Umbrella_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Umbrella_30_sequence), PeepSpriteImage_Umbrella_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Umbrella_31_sequence), PeepSpriteImage_Umbrella_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Umbrella_32_sequence), PeepSpriteImage_Umbrella_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Umbrella_33_sequence), PeepSpriteImage_Umbrella_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Umbrella_34_sequence), PeepSpriteImage_Umbrella_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Umbrella_35_sequence), PeepSpriteImage_Umbrella_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Umbrella_36_sequence), PeepSpriteImage_Umbrella_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Pizza[] = {
	{ 7785,  countof(PeepSpriteImage_Pizza_00_sequence), PeepSpriteImage_Pizza_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Pizza_01_sequence), PeepSpriteImage_Pizza_01_sequence },
	{ 7781,  countof(PeepSpriteImage_Pizza_02_sequence), PeepSpriteImage_Pizza_02_sequence },
	{ 7861,  countof(PeepSpriteImage_Pizza_03_sequence), PeepSpriteImage_Pizza_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Pizza_04_sequence), PeepSpriteImage_Pizza_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Pizza_05_sequence), PeepSpriteImage_Pizza_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Pizza_06_sequence), PeepSpriteImage_Pizza_06_sequence },
	{ 7833,  countof(PeepSpriteImage_Pizza_07_sequence), PeepSpriteImage_Pizza_07_sequence },
	{ 7837,  countof(PeepSpriteImage_Pizza_08_sequence), PeepSpriteImage_Pizza_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Pizza_09_sequence), PeepSpriteImage_Pizza_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Pizza_10_sequence), PeepSpriteImage_Pizza_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Pizza_11_sequence), PeepSpriteImage_Pizza_11_sequence },
	{ 11301, countof(PeepSpriteImage_Pizza_12_sequence), PeepSpriteImage_Pizza_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Pizza_13_sequence), PeepSpriteImage_Pizza_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Pizza_14_sequence), PeepSpriteImage_Pizza_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Pizza_15_sequence), PeepSpriteImage_Pizza_15_sequence },
	{ 11325, countof(PeepSpriteImage_Pizza_16_sequence), PeepSpriteImage_Pizza_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Pizza_17_sequence), PeepSpriteImage_Pizza_17_sequence },
	{ 11513, countof(PeepSpriteImage_Pizza_18_sequence), PeepSpriteImage_Pizza_18_sequence },
	{ 11513, countof(PeepSpriteImage_Pizza_19_sequence), PeepSpriteImage_Pizza_19_sequence },
	{ 11597, countof(PeepSpriteImage_Pizza_20_sequence), PeepSpriteImage_Pizza_20_sequence },
	{ 11669, countof(PeepSpriteImage_Pizza_21_sequence), PeepSpriteImage_Pizza_21_sequence },
	{ 11669, countof(PeepSpriteImage_Pizza_22_sequence), PeepSpriteImage_Pizza_22_sequence },
	{ 11801, countof(PeepSpriteImage_Pizza_23_sequence), PeepSpriteImage_Pizza_23_sequence },
	{ 11669, countof(PeepSpriteImage_Pizza_24_sequence), PeepSpriteImage_Pizza_24_sequence },
	{ 11377, countof(PeepSpriteImage_Pizza_25_sequence), PeepSpriteImage_Pizza_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Pizza_26_sequence), PeepSpriteImage_Pizza_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Pizza_27_sequence), PeepSpriteImage_Pizza_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Pizza_28_sequence), PeepSpriteImage_Pizza_28_sequence },
	{ 11401, countof(PeepSpriteImage_Pizza_29_sequence), PeepSpriteImage_Pizza_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Pizza_30_sequence), PeepSpriteImage_Pizza_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Pizza_31_sequence), PeepSpriteImage_Pizza_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Pizza_32_sequence), PeepSpriteImage_Pizza_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Pizza_33_sequence), PeepSpriteImage_Pizza_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Pizza_34_sequence), PeepSpriteImage_Pizza_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Pizza_35_sequence), PeepSpriteImage_Pizza_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Pizza_36_sequence), PeepSpriteImage_Pizza_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_SecurityAlt[] = {
	{ 11949, countof(PeepSpriteImage_SecurityAlt_00_sequence),PeepSpriteImage_SecurityAlt_00_sequence },
	{ 6585,  countof(PeepSpriteImage_SecurityAlt_01_sequence),PeepSpriteImage_SecurityAlt_01_sequence },
	{ 11905, countof(PeepSpriteImage_SecurityAlt_02_sequence),PeepSpriteImage_SecurityAlt_02_sequence },
	{ 6653,  countof(PeepSpriteImage_SecurityAlt_03_sequence),PeepSpriteImage_SecurityAlt_03_sequence },
	{ 6697,  countof(PeepSpriteImage_SecurityAlt_04_sequence),PeepSpriteImage_SecurityAlt_04_sequence },
	{ 6757,  countof(PeepSpriteImage_SecurityAlt_05_sequence),PeepSpriteImage_SecurityAlt_05_sequence },
	{ 6561,  countof(PeepSpriteImage_SecurityAlt_06_sequence),PeepSpriteImage_SecurityAlt_06_sequence },
	{ 6625,  countof(PeepSpriteImage_SecurityAlt_07_sequence),PeepSpriteImage_SecurityAlt_07_sequence },
	{ 6629,  countof(PeepSpriteImage_SecurityAlt_08_sequence),PeepSpriteImage_SecurityAlt_08_sequence },
	{ 6637,  countof(PeepSpriteImage_SecurityAlt_09_sequence),PeepSpriteImage_SecurityAlt_09_sequence },
	{ 6645,  countof(PeepSpriteImage_SecurityAlt_10_sequence),PeepSpriteImage_SecurityAlt_10_sequence },
	{ 11909, countof(PeepSpriteImage_SecurityAlt_11_sequence),PeepSpriteImage_SecurityAlt_11_sequence },
	{ 11301, countof(PeepSpriteImage_SecurityAlt_12_sequence),PeepSpriteImage_SecurityAlt_12_sequence },
	{ 6825,  countof(PeepSpriteImage_SecurityAlt_13_sequence),PeepSpriteImage_SecurityAlt_13_sequence },
	{ 6845,  countof(PeepSpriteImage_SecurityAlt_14_sequence),PeepSpriteImage_SecurityAlt_14_sequence },
	{ 6909,  countof(PeepSpriteImage_SecurityAlt_15_sequence),PeepSpriteImage_SecurityAlt_15_sequence },
	{ 11325, countof(PeepSpriteImage_SecurityAlt_16_sequence),PeepSpriteImage_SecurityAlt_16_sequence },
	{ 11921, countof(PeepSpriteImage_SecurityAlt_17_sequence),PeepSpriteImage_SecurityAlt_17_sequence },
	{ 11513, countof(PeepSpriteImage_SecurityAlt_18_sequence),PeepSpriteImage_SecurityAlt_18_sequence },
	{ 11513, countof(PeepSpriteImage_SecurityAlt_19_sequence),PeepSpriteImage_SecurityAlt_19_sequence },
	{ 11597, countof(PeepSpriteImage_SecurityAlt_20_sequence),PeepSpriteImage_SecurityAlt_20_sequence },
	{ 11669, countof(PeepSpriteImage_SecurityAlt_21_sequence),PeepSpriteImage_SecurityAlt_21_sequence },
	{ 11669, countof(PeepSpriteImage_SecurityAlt_22_sequence),PeepSpriteImage_SecurityAlt_22_sequence },
	{ 11801, countof(PeepSpriteImage_SecurityAlt_23_sequence),PeepSpriteImage_SecurityAlt_23_sequence },
	{ 11669, countof(PeepSpriteImage_SecurityAlt_24_sequence),PeepSpriteImage_SecurityAlt_24_sequence },
	{ 11377, countof(PeepSpriteImage_SecurityAlt_25_sequence),PeepSpriteImage_SecurityAlt_25_sequence },
	{ 6989,  countof(PeepSpriteImage_SecurityAlt_26_sequence),PeepSpriteImage_SecurityAlt_26_sequence },
	{ 7041,  countof(PeepSpriteImage_SecurityAlt_27_sequence),PeepSpriteImage_SecurityAlt_27_sequence },
	{ 7101,  countof(PeepSpriteImage_SecurityAlt_28_sequence),PeepSpriteImage_SecurityAlt_28_sequence },
	{ 11401, countof(PeepSpriteImage_SecurityAlt_29_sequence),PeepSpriteImage_SecurityAlt_29_sequence },
	{ 7121,  countof(PeepSpriteImage_SecurityAlt_30_sequence),PeepSpriteImage_SecurityAlt_30_sequence },
	{ 7149,  countof(PeepSpriteImage_SecurityAlt_31_sequence),PeepSpriteImage_SecurityAlt_31_sequence },
	{ 7249,  countof(PeepSpriteImage_SecurityAlt_32_sequence),PeepSpriteImage_SecurityAlt_32_sequence },
	{ 7309,  countof(PeepSpriteImage_SecurityAlt_33_sequence),PeepSpriteImage_SecurityAlt_33_sequence },
	{ 7329,  countof(PeepSpriteImage_SecurityAlt_34_sequence),PeepSpriteImage_SecurityAlt_34_sequence },
	{ 7285,  countof(PeepSpriteImage_SecurityAlt_35_sequence),PeepSpriteImage_SecurityAlt_35_sequence },
	{ 7181,  countof(PeepSpriteImage_SecurityAlt_36_sequence),PeepSpriteImage_SecurityAlt_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Popcorn[] = {
	{ 11025, countof(PeepSpriteImage_Popcorn_00_sequence), PeepSpriteImage_Popcorn_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Popcorn_01_sequence), PeepSpriteImage_Popcorn_01_sequence },
	{ 11021, countof(PeepSpriteImage_Popcorn_02_sequence), PeepSpriteImage_Popcorn_02_sequence },
	{ 11149, countof(PeepSpriteImage_Popcorn_03_sequence), PeepSpriteImage_Popcorn_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Popcorn_04_sequence), PeepSpriteImage_Popcorn_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Popcorn_05_sequence), PeepSpriteImage_Popcorn_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Popcorn_06_sequence), PeepSpriteImage_Popcorn_06_sequence },
	{ 11097, countof(PeepSpriteImage_Popcorn_07_sequence), PeepSpriteImage_Popcorn_07_sequence },
	{ 11101, countof(PeepSpriteImage_Popcorn_08_sequence), PeepSpriteImage_Popcorn_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Popcorn_09_sequence), PeepSpriteImage_Popcorn_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Popcorn_10_sequence), PeepSpriteImage_Popcorn_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Popcorn_11_sequence), PeepSpriteImage_Popcorn_11_sequence },
	{ 11301, countof(PeepSpriteImage_Popcorn_12_sequence), PeepSpriteImage_Popcorn_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Popcorn_13_sequence), PeepSpriteImage_Popcorn_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Popcorn_14_sequence), PeepSpriteImage_Popcorn_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Popcorn_15_sequence), PeepSpriteImage_Popcorn_15_sequence },
	{ 11325, countof(PeepSpriteImage_Popcorn_16_sequence), PeepSpriteImage_Popcorn_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Popcorn_17_sequence), PeepSpriteImage_Popcorn_17_sequence },
	{ 11513, countof(PeepSpriteImage_Popcorn_18_sequence), PeepSpriteImage_Popcorn_18_sequence },
	{ 11513, countof(PeepSpriteImage_Popcorn_19_sequence), PeepSpriteImage_Popcorn_19_sequence },
	{ 11597, countof(PeepSpriteImage_Popcorn_20_sequence), PeepSpriteImage_Popcorn_20_sequence },
	{ 11669, countof(PeepSpriteImage_Popcorn_21_sequence), PeepSpriteImage_Popcorn_21_sequence },
	{ 11669, countof(PeepSpriteImage_Popcorn_22_sequence), PeepSpriteImage_Popcorn_22_sequence },
	{ 11801, countof(PeepSpriteImage_Popcorn_23_sequence), PeepSpriteImage_Popcorn_23_sequence },
	{ 11669, countof(PeepSpriteImage_Popcorn_24_sequence), PeepSpriteImage_Popcorn_24_sequence },
	{ 11377, countof(PeepSpriteImage_Popcorn_25_sequence), PeepSpriteImage_Popcorn_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Popcorn_26_sequence), PeepSpriteImage_Popcorn_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Popcorn_27_sequence), PeepSpriteImage_Popcorn_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Popcorn_28_sequence), PeepSpriteImage_Popcorn_28_sequence },
	{ 11401, countof(PeepSpriteImage_Popcorn_29_sequence), PeepSpriteImage_Popcorn_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Popcorn_30_sequence), PeepSpriteImage_Popcorn_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Popcorn_31_sequence), PeepSpriteImage_Popcorn_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Popcorn_32_sequence), PeepSpriteImage_Popcorn_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Popcorn_33_sequence), PeepSpriteImage_Popcorn_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Popcorn_34_sequence), PeepSpriteImage_Popcorn_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Popcorn_35_sequence), PeepSpriteImage_Popcorn_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Popcorn_36_sequence), PeepSpriteImage_Popcorn_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_ArmsCrossed[] = {
	{ 6433,  countof(PeepSpriteImage_ArmsCrossed_00_sequence), PeepSpriteImage_ArmsCrossed_00_sequence },
	{ 6585,  countof(PeepSpriteImage_ArmsCrossed_01_sequence), PeepSpriteImage_ArmsCrossed_01_sequence },
	{ 6621,  countof(PeepSpriteImage_ArmsCrossed_02_sequence), PeepSpriteImage_ArmsCrossed_02_sequence },
	{ 6653,  countof(PeepSpriteImage_ArmsCrossed_03_sequence), PeepSpriteImage_ArmsCrossed_03_sequence },
	{ 6697,  countof(PeepSpriteImage_ArmsCrossed_04_sequence), PeepSpriteImage_ArmsCrossed_04_sequence },
	{ 6757,  countof(PeepSpriteImage_ArmsCrossed_05_sequence), PeepSpriteImage_ArmsCrossed_05_sequence },
	{ 6561,  countof(PeepSpriteImage_ArmsCrossed_06_sequence), PeepSpriteImage_ArmsCrossed_06_sequence },
	{ 6625,  countof(PeepSpriteImage_ArmsCrossed_07_sequence), PeepSpriteImage_ArmsCrossed_07_sequence },
	{ 6629,  countof(PeepSpriteImage_ArmsCrossed_08_sequence), PeepSpriteImage_ArmsCrossed_08_sequence },
	{ 6637,  countof(PeepSpriteImage_ArmsCrossed_09_sequence), PeepSpriteImage_ArmsCrossed_09_sequence },
	{ 6645,  countof(PeepSpriteImage_ArmsCrossed_10_sequence), PeepSpriteImage_ArmsCrossed_10_sequence },
	{ 6813,  countof(PeepSpriteImage_ArmsCrossed_11_sequence), PeepSpriteImage_ArmsCrossed_11_sequence },
	{ 11301, countof(PeepSpriteImage_ArmsCrossed_12_sequence), PeepSpriteImage_ArmsCrossed_12_sequence },
	{ 6825,  countof(PeepSpriteImage_ArmsCrossed_13_sequence), PeepSpriteImage_ArmsCrossed_13_sequence },
	{ 6845,  countof(PeepSpriteImage_ArmsCrossed_14_sequence), PeepSpriteImage_ArmsCrossed_14_sequence },
	{ 6909,  countof(PeepSpriteImage_ArmsCrossed_15_sequence), PeepSpriteImage_ArmsCrossed_15_sequence },
	{ 11325, countof(PeepSpriteImage_ArmsCrossed_16_sequence), PeepSpriteImage_ArmsCrossed_16_sequence },
	{ 6957,  countof(PeepSpriteImage_ArmsCrossed_17_sequence), PeepSpriteImage_ArmsCrossed_17_sequence },
	{ 11513, countof(PeepSpriteImage_ArmsCrossed_18_sequence), PeepSpriteImage_ArmsCrossed_18_sequence },
	{ 11513, countof(PeepSpriteImage_ArmsCrossed_19_sequence), PeepSpriteImage_ArmsCrossed_19_sequence },
	{ 11597, countof(PeepSpriteImage_ArmsCrossed_20_sequence), PeepSpriteImage_ArmsCrossed_20_sequence },
	{ 11669, countof(PeepSpriteImage_ArmsCrossed_21_sequence), PeepSpriteImage_ArmsCrossed_21_sequence },
	{ 11669, countof(PeepSpriteImage_ArmsCrossed_22_sequence), PeepSpriteImage_ArmsCrossed_22_sequence },
	{ 11801, countof(PeepSpriteImage_ArmsCrossed_23_sequence), PeepSpriteImage_ArmsCrossed_23_sequence },
	{ 11669, countof(PeepSpriteImage_ArmsCrossed_24_sequence), PeepSpriteImage_ArmsCrossed_24_sequence },
	{ 11377, countof(PeepSpriteImage_ArmsCrossed_25_sequence), PeepSpriteImage_ArmsCrossed_25_sequence },
	{ 6989,  countof(PeepSpriteImage_ArmsCrossed_26_sequence), PeepSpriteImage_ArmsCrossed_26_sequence },
	{ 7041,  countof(PeepSpriteImage_ArmsCrossed_27_sequence), PeepSpriteImage_ArmsCrossed_27_sequence },
	{ 7101,  countof(PeepSpriteImage_ArmsCrossed_28_sequence), PeepSpriteImage_ArmsCrossed_28_sequence },
	{ 11401, countof(PeepSpriteImage_ArmsCrossed_29_sequence), PeepSpriteImage_ArmsCrossed_29_sequence },
	{ 7121,  countof(PeepSpriteImage_ArmsCrossed_30_sequence), PeepSpriteImage_ArmsCrossed_30_sequence },
	{ 7149,  countof(PeepSpriteImage_ArmsCrossed_31_sequence), PeepSpriteImage_ArmsCrossed_31_sequence },
	{ 7249,  countof(PeepSpriteImage_ArmsCrossed_32_sequence), PeepSpriteImage_ArmsCrossed_32_sequence },
	{ 7309,  countof(PeepSpriteImage_ArmsCrossed_33_sequence), PeepSpriteImage_ArmsCrossed_33_sequence },
	{ 7329,  countof(PeepSpriteImage_ArmsCrossed_34_sequence), PeepSpriteImage_ArmsCrossed_34_sequence },
	{ 7285,  countof(PeepSpriteImage_ArmsCrossed_35_sequence), PeepSpriteImage_ArmsCrossed_35_sequence },
	{ 7181,  countof(PeepSpriteImage_ArmsCrossed_36_sequence), PeepSpriteImage_ArmsCrossed_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_HeadDown[] = {
	{ 6457,  countof(PeepSpriteImage_HeadDown_00_sequence), PeepSpriteImage_HeadDown_00_sequence },
	{ 6585,  countof(PeepSpriteImage_HeadDown_01_sequence), PeepSpriteImage_HeadDown_01_sequence },
	{ 6621,  countof(PeepSpriteImage_HeadDown_02_sequence), PeepSpriteImage_HeadDown_02_sequence },
	{ 6653,  countof(PeepSpriteImage_HeadDown_03_sequence), PeepSpriteImage_HeadDown_03_sequence },
	{ 6697,  countof(PeepSpriteImage_HeadDown_04_sequence), PeepSpriteImage_HeadDown_04_sequence },
	{ 6757,  countof(PeepSpriteImage_HeadDown_05_sequence), PeepSpriteImage_HeadDown_05_sequence },
	{ 6561,  countof(PeepSpriteImage_HeadDown_06_sequence), PeepSpriteImage_HeadDown_06_sequence },
	{ 6625,  countof(PeepSpriteImage_HeadDown_07_sequence), PeepSpriteImage_HeadDown_07_sequence },
	{ 6629,  countof(PeepSpriteImage_HeadDown_08_sequence), PeepSpriteImage_HeadDown_08_sequence },
	{ 6637,  countof(PeepSpriteImage_HeadDown_09_sequence), PeepSpriteImage_HeadDown_09_sequence },
	{ 6645,  countof(PeepSpriteImage_HeadDown_10_sequence), PeepSpriteImage_HeadDown_10_sequence },
	{ 6813,  countof(PeepSpriteImage_HeadDown_11_sequence), PeepSpriteImage_HeadDown_11_sequence },
	{ 11301, countof(PeepSpriteImage_HeadDown_12_sequence), PeepSpriteImage_HeadDown_12_sequence },
	{ 6825,  countof(PeepSpriteImage_HeadDown_13_sequence), PeepSpriteImage_HeadDown_13_sequence },
	{ 6845,  countof(PeepSpriteImage_HeadDown_14_sequence), PeepSpriteImage_HeadDown_14_sequence },
	{ 6909,  countof(PeepSpriteImage_HeadDown_15_sequence), PeepSpriteImage_HeadDown_15_sequence },
	{ 11325, countof(PeepSpriteImage_HeadDown_16_sequence), PeepSpriteImage_HeadDown_16_sequence },
	{ 6957,  countof(PeepSpriteImage_HeadDown_17_sequence), PeepSpriteImage_HeadDown_17_sequence },
	{ 11513, countof(PeepSpriteImage_HeadDown_18_sequence), PeepSpriteImage_HeadDown_18_sequence },
	{ 11513, countof(PeepSpriteImage_HeadDown_19_sequence), PeepSpriteImage_HeadDown_19_sequence },
	{ 11597, countof(PeepSpriteImage_HeadDown_20_sequence), PeepSpriteImage_HeadDown_20_sequence },
	{ 11669, countof(PeepSpriteImage_HeadDown_21_sequence), PeepSpriteImage_HeadDown_21_sequence },
	{ 11669, countof(PeepSpriteImage_HeadDown_22_sequence), PeepSpriteImage_HeadDown_22_sequence },
	{ 11801, countof(PeepSpriteImage_HeadDown_23_sequence), PeepSpriteImage_HeadDown_23_sequence },
	{ 11669, countof(PeepSpriteImage_HeadDown_24_sequence), PeepSpriteImage_HeadDown_24_sequence },
	{ 11377, countof(PeepSpriteImage_HeadDown_25_sequence), PeepSpriteImage_HeadDown_25_sequence },
	{ 6989,  countof(PeepSpriteImage_HeadDown_26_sequence), PeepSpriteImage_HeadDown_26_sequence },
	{ 7041,  countof(PeepSpriteImage_HeadDown_27_sequence), PeepSpriteImage_HeadDown_27_sequence },
	{ 7101,  countof(PeepSpriteImage_HeadDown_28_sequence), PeepSpriteImage_HeadDown_28_sequence },
	{ 11401, countof(PeepSpriteImage_HeadDown_29_sequence), PeepSpriteImage_HeadDown_29_sequence },
	{ 7121,  countof(PeepSpriteImage_HeadDown_30_sequence), PeepSpriteImage_HeadDown_30_sequence },
	{ 7149,  countof(PeepSpriteImage_HeadDown_31_sequence), PeepSpriteImage_HeadDown_31_sequence },
	{ 7249,  countof(PeepSpriteImage_HeadDown_32_sequence), PeepSpriteImage_HeadDown_32_sequence },
	{ 7309,  countof(PeepSpriteImage_HeadDown_33_sequence), PeepSpriteImage_HeadDown_33_sequence },
	{ 7329,  countof(PeepSpriteImage_HeadDown_34_sequence), PeepSpriteImage_HeadDown_34_sequence },
	{ 7285,  countof(PeepSpriteImage_HeadDown_35_sequence), PeepSpriteImage_HeadDown_35_sequence },
	{ 7181,  countof(PeepSpriteImage_HeadDown_36_sequence), PeepSpriteImage_HeadDown_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Nauseous[] = {
	{ 6481,  countof(PeepSpriteImage_Nauseous_00_sequence), PeepSpriteImage_Nauseous_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Nauseous_01_sequence), PeepSpriteImage_Nauseous_01_sequence },
	{ 6621,  countof(PeepSpriteImage_Nauseous_02_sequence), PeepSpriteImage_Nauseous_02_sequence },
	{ 6653,  countof(PeepSpriteImage_Nauseous_03_sequence), PeepSpriteImage_Nauseous_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Nauseous_04_sequence), PeepSpriteImage_Nauseous_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Nauseous_05_sequence), PeepSpriteImage_Nauseous_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Nauseous_06_sequence), PeepSpriteImage_Nauseous_06_sequence },
	{ 6625,  countof(PeepSpriteImage_Nauseous_07_sequence), PeepSpriteImage_Nauseous_07_sequence },
	{ 6629,  countof(PeepSpriteImage_Nauseous_08_sequence), PeepSpriteImage_Nauseous_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Nauseous_09_sequence), PeepSpriteImage_Nauseous_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Nauseous_10_sequence), PeepSpriteImage_Nauseous_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Nauseous_11_sequence), PeepSpriteImage_Nauseous_11_sequence },
	{ 11301, countof(PeepSpriteImage_Nauseous_12_sequence), PeepSpriteImage_Nauseous_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Nauseous_13_sequence), PeepSpriteImage_Nauseous_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Nauseous_14_sequence), PeepSpriteImage_Nauseous_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Nauseous_15_sequence), PeepSpriteImage_Nauseous_15_sequence },
	{ 11325, countof(PeepSpriteImage_Nauseous_16_sequence), PeepSpriteImage_Nauseous_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Nauseous_17_sequence), PeepSpriteImage_Nauseous_17_sequence },
	{ 11513, countof(PeepSpriteImage_Nauseous_18_sequence), PeepSpriteImage_Nauseous_18_sequence },
	{ 11513, countof(PeepSpriteImage_Nauseous_19_sequence), PeepSpriteImage_Nauseous_19_sequence },
	{ 11597, countof(PeepSpriteImage_Nauseous_20_sequence), PeepSpriteImage_Nauseous_20_sequence },
	{ 11669, countof(PeepSpriteImage_Nauseous_21_sequence), PeepSpriteImage_Nauseous_21_sequence },
	{ 11669, countof(PeepSpriteImage_Nauseous_22_sequence), PeepSpriteImage_Nauseous_22_sequence },
	{ 11801, countof(PeepSpriteImage_Nauseous_23_sequence), PeepSpriteImage_Nauseous_23_sequence },
	{ 11669, countof(PeepSpriteImage_Nauseous_24_sequence), PeepSpriteImage_Nauseous_24_sequence },
	{ 11377, countof(PeepSpriteImage_Nauseous_25_sequence), PeepSpriteImage_Nauseous_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Nauseous_26_sequence), PeepSpriteImage_Nauseous_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Nauseous_27_sequence), PeepSpriteImage_Nauseous_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Nauseous_28_sequence), PeepSpriteImage_Nauseous_28_sequence },
	{ 11401, countof(PeepSpriteImage_Nauseous_29_sequence), PeepSpriteImage_Nauseous_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Nauseous_30_sequence), PeepSpriteImage_Nauseous_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Nauseous_31_sequence), PeepSpriteImage_Nauseous_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Nauseous_32_sequence), PeepSpriteImage_Nauseous_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Nauseous_33_sequence), PeepSpriteImage_Nauseous_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Nauseous_34_sequence), PeepSpriteImage_Nauseous_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Nauseous_35_sequence), PeepSpriteImage_Nauseous_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Nauseous_36_sequence), PeepSpriteImage_Nauseous_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_VeryNauseous[] = {
	{ 6505,  countof(PeepSpriteImage_VeryNauseous_00_sequence), PeepSpriteImage_VeryNauseous_00_sequence },
	{ 6585,  countof(PeepSpriteImage_VeryNauseous_01_sequence), PeepSpriteImage_VeryNauseous_01_sequence },
	{ 6529,  countof(PeepSpriteImage_VeryNauseous_02_sequence), PeepSpriteImage_VeryNauseous_02_sequence },
	{ 6653,  countof(PeepSpriteImage_VeryNauseous_03_sequence), PeepSpriteImage_VeryNauseous_03_sequence },
	{ 6697,  countof(PeepSpriteImage_VeryNauseous_04_sequence), PeepSpriteImage_VeryNauseous_04_sequence },
	{ 6757,  countof(PeepSpriteImage_VeryNauseous_05_sequence), PeepSpriteImage_VeryNauseous_05_sequence },
	{ 6561,  countof(PeepSpriteImage_VeryNauseous_06_sequence), PeepSpriteImage_VeryNauseous_06_sequence },
	{ 6533,  countof(PeepSpriteImage_VeryNauseous_07_sequence), PeepSpriteImage_VeryNauseous_07_sequence },
	{ 6629,  countof(PeepSpriteImage_VeryNauseous_08_sequence), PeepSpriteImage_VeryNauseous_08_sequence },
	{ 6637,  countof(PeepSpriteImage_VeryNauseous_09_sequence), PeepSpriteImage_VeryNauseous_09_sequence },
	{ 6645,  countof(PeepSpriteImage_VeryNauseous_10_sequence), PeepSpriteImage_VeryNauseous_10_sequence },
	{ 6813,  countof(PeepSpriteImage_VeryNauseous_11_sequence), PeepSpriteImage_VeryNauseous_11_sequence },
	{ 11301, countof(PeepSpriteImage_VeryNauseous_12_sequence), PeepSpriteImage_VeryNauseous_12_sequence },
	{ 6825,  countof(PeepSpriteImage_VeryNauseous_13_sequence), PeepSpriteImage_VeryNauseous_13_sequence },
	{ 6845,  countof(PeepSpriteImage_VeryNauseous_14_sequence), PeepSpriteImage_VeryNauseous_14_sequence },
	{ 6909,  countof(PeepSpriteImage_VeryNauseous_15_sequence), PeepSpriteImage_VeryNauseous_15_sequence },
	{ 11325, countof(PeepSpriteImage_VeryNauseous_16_sequence), PeepSpriteImage_VeryNauseous_16_sequence },
	{ 6957,  countof(PeepSpriteImage_VeryNauseous_17_sequence), PeepSpriteImage_VeryNauseous_17_sequence },
	{ 11513, countof(PeepSpriteImage_VeryNauseous_18_sequence), PeepSpriteImage_VeryNauseous_18_sequence },
	{ 11513, countof(PeepSpriteImage_VeryNauseous_19_sequence), PeepSpriteImage_VeryNauseous_19_sequence },
	{ 11597, countof(PeepSpriteImage_VeryNauseous_20_sequence), PeepSpriteImage_VeryNauseous_20_sequence },
	{ 11669, countof(PeepSpriteImage_VeryNauseous_21_sequence), PeepSpriteImage_VeryNauseous_21_sequence },
	{ 11669, countof(PeepSpriteImage_VeryNauseous_22_sequence), PeepSpriteImage_VeryNauseous_22_sequence },
	{ 11801, countof(PeepSpriteImage_VeryNauseous_23_sequence), PeepSpriteImage_VeryNauseous_23_sequence },
	{ 11669, countof(PeepSpriteImage_VeryNauseous_24_sequence), PeepSpriteImage_VeryNauseous_24_sequence },
	{ 11377, countof(PeepSpriteImage_VeryNauseous_25_sequence), PeepSpriteImage_VeryNauseous_25_sequence },
	{ 6989,  countof(PeepSpriteImage_VeryNauseous_26_sequence), PeepSpriteImage_VeryNauseous_26_sequence },
	{ 7041,  countof(PeepSpriteImage_VeryNauseous_27_sequence), PeepSpriteImage_VeryNauseous_27_sequence },
	{ 7101,  countof(PeepSpriteImage_VeryNauseous_28_sequence), PeepSpriteImage_VeryNauseous_28_sequence },
	{ 11401, countof(PeepSpriteImage_VeryNauseous_29_sequence), PeepSpriteImage_VeryNauseous_29_sequence },
	{ 7121,  countof(PeepSpriteImage_VeryNauseous_30_sequence), PeepSpriteImage_VeryNauseous_30_sequence },
	{ 7149,  countof(PeepSpriteImage_VeryNauseous_31_sequence), PeepSpriteImage_VeryNauseous_31_sequence },
	{ 7249,  countof(PeepSpriteImage_VeryNauseous_32_sequence), PeepSpriteImage_VeryNauseous_32_sequence },
	{ 7309,  countof(PeepSpriteImage_VeryNauseous_33_sequence), PeepSpriteImage_VeryNauseous_33_sequence },
	{ 7329,  countof(PeepSpriteImage_VeryNauseous_34_sequence), PeepSpriteImage_VeryNauseous_34_sequence },
	{ 7285,  countof(PeepSpriteImage_VeryNauseous_35_sequence), PeepSpriteImage_VeryNauseous_35_sequence },
	{ 7181,  countof(PeepSpriteImage_VeryNauseous_36_sequence), PeepSpriteImage_VeryNauseous_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_RequireBathroom[] = {
	{ 6537,  countof(PeepSpriteImage_RequireBathroom_00_sequence), PeepSpriteImage_RequireBathroom_00_sequence },
	{ 6585,  countof(PeepSpriteImage_RequireBathroom_01_sequence), PeepSpriteImage_RequireBathroom_01_sequence },
	{ 6621,  countof(PeepSpriteImage_RequireBathroom_02_sequence), PeepSpriteImage_RequireBathroom_02_sequence },
	{ 6653,  countof(PeepSpriteImage_RequireBathroom_03_sequence), PeepSpriteImage_RequireBathroom_03_sequence },
	{ 6697,  countof(PeepSpriteImage_RequireBathroom_04_sequence), PeepSpriteImage_RequireBathroom_04_sequence },
	{ 6757,  countof(PeepSpriteImage_RequireBathroom_05_sequence), PeepSpriteImage_RequireBathroom_05_sequence },
	{ 6561,  countof(PeepSpriteImage_RequireBathroom_06_sequence), PeepSpriteImage_RequireBathroom_06_sequence },
	{ 6625,  countof(PeepSpriteImage_RequireBathroom_07_sequence), PeepSpriteImage_RequireBathroom_07_sequence },
	{ 6629,  countof(PeepSpriteImage_RequireBathroom_08_sequence), PeepSpriteImage_RequireBathroom_08_sequence },
	{ 6637,  countof(PeepSpriteImage_RequireBathroom_09_sequence), PeepSpriteImage_RequireBathroom_09_sequence },
	{ 6645,  countof(PeepSpriteImage_RequireBathroom_10_sequence), PeepSpriteImage_RequireBathroom_10_sequence },
	{ 6813,  countof(PeepSpriteImage_RequireBathroom_11_sequence), PeepSpriteImage_RequireBathroom_11_sequence },
	{ 11301, countof(PeepSpriteImage_RequireBathroom_12_sequence), PeepSpriteImage_RequireBathroom_12_sequence },
	{ 6825,  countof(PeepSpriteImage_RequireBathroom_13_sequence), PeepSpriteImage_RequireBathroom_13_sequence },
	{ 6845,  countof(PeepSpriteImage_RequireBathroom_14_sequence), PeepSpriteImage_RequireBathroom_14_sequence },
	{ 6909,  countof(PeepSpriteImage_RequireBathroom_15_sequence), PeepSpriteImage_RequireBathroom_15_sequence },
	{ 11325, countof(PeepSpriteImage_RequireBathroom_16_sequence), PeepSpriteImage_RequireBathroom_16_sequence },
	{ 6957,  countof(PeepSpriteImage_RequireBathroom_17_sequence), PeepSpriteImage_RequireBathroom_17_sequence },
	{ 11513, countof(PeepSpriteImage_RequireBathroom_18_sequence), PeepSpriteImage_RequireBathroom_18_sequence },
	{ 11513, countof(PeepSpriteImage_RequireBathroom_19_sequence), PeepSpriteImage_RequireBathroom_19_sequence },
	{ 11597, countof(PeepSpriteImage_RequireBathroom_20_sequence), PeepSpriteImage_RequireBathroom_20_sequence },
	{ 11669, countof(PeepSpriteImage_RequireBathroom_21_sequence), PeepSpriteImage_RequireBathroom_21_sequence },
	{ 11669, countof(PeepSpriteImage_RequireBathroom_22_sequence), PeepSpriteImage_RequireBathroom_22_sequence },
	{ 11801, countof(PeepSpriteImage_RequireBathroom_23_sequence), PeepSpriteImage_RequireBathroom_23_sequence },
	{ 11669, countof(PeepSpriteImage_RequireBathroom_24_sequence), PeepSpriteImage_RequireBathroom_24_sequence },
	{ 11377, countof(PeepSpriteImage_RequireBathroom_25_sequence), PeepSpriteImage_RequireBathroom_25_sequence },
	{ 6989,  countof(PeepSpriteImage_RequireBathroom_26_sequence), PeepSpriteImage_RequireBathroom_26_sequence },
	{ 7041,  countof(PeepSpriteImage_RequireBathroom_27_sequence), PeepSpriteImage_RequireBathroom_27_sequence },
	{ 7101,  countof(PeepSpriteImage_RequireBathroom_28_sequence), PeepSpriteImage_RequireBathroom_28_sequence },
	{ 11401, countof(PeepSpriteImage_RequireBathroom_29_sequence), PeepSpriteImage_RequireBathroom_29_sequence },
	{ 7121,  countof(PeepSpriteImage_RequireBathroom_30_sequence), PeepSpriteImage_RequireBathroom_30_sequence },
	{ 7149,  countof(PeepSpriteImage_RequireBathroom_31_sequence), PeepSpriteImage_RequireBathroom_31_sequence },
	{ 7249,  countof(PeepSpriteImage_RequireBathroom_32_sequence), PeepSpriteImage_RequireBathroom_32_sequence },
	{ 7309,  countof(PeepSpriteImage_RequireBathroom_33_sequence), PeepSpriteImage_RequireBathroom_33_sequence },
	{ 7329,  countof(PeepSpriteImage_RequireBathroom_34_sequence), PeepSpriteImage_RequireBathroom_34_sequence },
	{ 7285,  countof(PeepSpriteImage_RequireBathroom_35_sequence), PeepSpriteImage_RequireBathroom_35_sequence },
	{ 7181,  countof(PeepSpriteImage_RequireBathroom_36_sequence), PeepSpriteImage_RequireBathroom_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Hat[] = {
	{ 10721, countof(PeepSpriteImage_Hat_00_sequence), PeepSpriteImage_Hat_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Hat_01_sequence), PeepSpriteImage_Hat_01_sequence },
	{ 10717, countof(PeepSpriteImage_Hat_02_sequence), PeepSpriteImage_Hat_02_sequence },
	{ 6653,  countof(PeepSpriteImage_Hat_03_sequence), PeepSpriteImage_Hat_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Hat_04_sequence), PeepSpriteImage_Hat_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Hat_05_sequence), PeepSpriteImage_Hat_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Hat_06_sequence), PeepSpriteImage_Hat_06_sequence },
	{ 10745, countof(PeepSpriteImage_Hat_07_sequence), PeepSpriteImage_Hat_07_sequence },
	{ 6629,  countof(PeepSpriteImage_Hat_08_sequence), PeepSpriteImage_Hat_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Hat_09_sequence), PeepSpriteImage_Hat_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Hat_10_sequence), PeepSpriteImage_Hat_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Hat_11_sequence), PeepSpriteImage_Hat_11_sequence },
	{ 11301, countof(PeepSpriteImage_Hat_12_sequence), PeepSpriteImage_Hat_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Hat_13_sequence), PeepSpriteImage_Hat_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Hat_14_sequence), PeepSpriteImage_Hat_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Hat_15_sequence), PeepSpriteImage_Hat_15_sequence },
	{ 11325, countof(PeepSpriteImage_Hat_16_sequence), PeepSpriteImage_Hat_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Hat_17_sequence), PeepSpriteImage_Hat_17_sequence },
	{ 11513, countof(PeepSpriteImage_Hat_18_sequence), PeepSpriteImage_Hat_18_sequence },
	{ 11513, countof(PeepSpriteImage_Hat_19_sequence), PeepSpriteImage_Hat_19_sequence },
	{ 11597, countof(PeepSpriteImage_Hat_20_sequence), PeepSpriteImage_Hat_20_sequence },
	{ 11669, countof(PeepSpriteImage_Hat_21_sequence), PeepSpriteImage_Hat_21_sequence },
	{ 11669, countof(PeepSpriteImage_Hat_22_sequence), PeepSpriteImage_Hat_22_sequence },
	{ 11801, countof(PeepSpriteImage_Hat_23_sequence), PeepSpriteImage_Hat_23_sequence },
	{ 11669, countof(PeepSpriteImage_Hat_24_sequence), PeepSpriteImage_Hat_24_sequence },
	{ 11377, countof(PeepSpriteImage_Hat_25_sequence), PeepSpriteImage_Hat_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Hat_26_sequence), PeepSpriteImage_Hat_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Hat_27_sequence), PeepSpriteImage_Hat_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Hat_28_sequence), PeepSpriteImage_Hat_28_sequence },
	{ 11401, countof(PeepSpriteImage_Hat_29_sequence), PeepSpriteImage_Hat_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Hat_30_sequence), PeepSpriteImage_Hat_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Hat_31_sequence), PeepSpriteImage_Hat_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Hat_32_sequence), PeepSpriteImage_Hat_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Hat_33_sequence), PeepSpriteImage_Hat_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Hat_34_sequence), PeepSpriteImage_Hat_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Hat_35_sequence), PeepSpriteImage_Hat_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Hat_36_sequence), PeepSpriteImage_Hat_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_HotDog[] = {
	{ 7889,  countof(PeepSpriteImage_HotDog_00_sequence), PeepSpriteImage_HotDog_00_sequence },
	{ 6585,  countof(PeepSpriteImage_HotDog_01_sequence), PeepSpriteImage_HotDog_01_sequence },
	{ 7885,  countof(PeepSpriteImage_HotDog_02_sequence), PeepSpriteImage_HotDog_02_sequence },
	{ 8013,  countof(PeepSpriteImage_HotDog_03_sequence), PeepSpriteImage_HotDog_03_sequence },
	{ 6697,  countof(PeepSpriteImage_HotDog_04_sequence), PeepSpriteImage_HotDog_04_sequence },
	{ 6757,  countof(PeepSpriteImage_HotDog_05_sequence), PeepSpriteImage_HotDog_05_sequence },
	{ 6561,  countof(PeepSpriteImage_HotDog_06_sequence), PeepSpriteImage_HotDog_06_sequence },
	{ 7961,  countof(PeepSpriteImage_HotDog_07_sequence), PeepSpriteImage_HotDog_07_sequence },
	{ 7965,  countof(PeepSpriteImage_HotDog_08_sequence), PeepSpriteImage_HotDog_08_sequence },
	{ 6637,  countof(PeepSpriteImage_HotDog_09_sequence), PeepSpriteImage_HotDog_09_sequence },
	{ 6645,  countof(PeepSpriteImage_HotDog_10_sequence), PeepSpriteImage_HotDog_10_sequence },
	{ 6813,  countof(PeepSpriteImage_HotDog_11_sequence), PeepSpriteImage_HotDog_11_sequence },
	{ 11301, countof(PeepSpriteImage_HotDog_12_sequence), PeepSpriteImage_HotDog_12_sequence },
	{ 6825,  countof(PeepSpriteImage_HotDog_13_sequence), PeepSpriteImage_HotDog_13_sequence },
	{ 6845,  countof(PeepSpriteImage_HotDog_14_sequence), PeepSpriteImage_HotDog_14_sequence },
	{ 6909,  countof(PeepSpriteImage_HotDog_15_sequence), PeepSpriteImage_HotDog_15_sequence },
	{ 11325, countof(PeepSpriteImage_HotDog_16_sequence), PeepSpriteImage_HotDog_16_sequence },
	{ 6957,  countof(PeepSpriteImage_HotDog_17_sequence), PeepSpriteImage_HotDog_17_sequence },
	{ 11513, countof(PeepSpriteImage_HotDog_18_sequence), PeepSpriteImage_HotDog_18_sequence },
	{ 11513, countof(PeepSpriteImage_HotDog_19_sequence), PeepSpriteImage_HotDog_19_sequence },
	{ 11597, countof(PeepSpriteImage_HotDog_20_sequence), PeepSpriteImage_HotDog_20_sequence },
	{ 11669, countof(PeepSpriteImage_HotDog_21_sequence), PeepSpriteImage_HotDog_21_sequence },
	{ 11669, countof(PeepSpriteImage_HotDog_22_sequence), PeepSpriteImage_HotDog_22_sequence },
	{ 11801, countof(PeepSpriteImage_HotDog_23_sequence), PeepSpriteImage_HotDog_23_sequence },
	{ 11669, countof(PeepSpriteImage_HotDog_24_sequence), PeepSpriteImage_HotDog_24_sequence },
	{ 11377, countof(PeepSpriteImage_HotDog_25_sequence), PeepSpriteImage_HotDog_25_sequence },
	{ 6989,  countof(PeepSpriteImage_HotDog_26_sequence), PeepSpriteImage_HotDog_26_sequence },
	{ 7041,  countof(PeepSpriteImage_HotDog_27_sequence), PeepSpriteImage_HotDog_27_sequence },
	{ 7101,  countof(PeepSpriteImage_HotDog_28_sequence), PeepSpriteImage_HotDog_28_sequence },
	{ 11401, countof(PeepSpriteImage_HotDog_29_sequence), PeepSpriteImage_HotDog_29_sequence },
	{ 7121,  countof(PeepSpriteImage_HotDog_30_sequence), PeepSpriteImage_HotDog_30_sequence },
	{ 7149,  countof(PeepSpriteImage_HotDog_31_sequence), PeepSpriteImage_HotDog_31_sequence },
	{ 7249,  countof(PeepSpriteImage_HotDog_32_sequence), PeepSpriteImage_HotDog_32_sequence },
	{ 7309,  countof(PeepSpriteImage_HotDog_33_sequence), PeepSpriteImage_HotDog_33_sequence },
	{ 7329,  countof(PeepSpriteImage_HotDog_34_sequence), PeepSpriteImage_HotDog_34_sequence },
	{ 7285,  countof(PeepSpriteImage_HotDog_35_sequence), PeepSpriteImage_HotDog_35_sequence },
	{ 7181,  countof(PeepSpriteImage_HotDog_36_sequence), PeepSpriteImage_HotDog_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Tentacle[] = {
	{ 8065,  countof(PeepSpriteImage_Tentacle_00_sequence), PeepSpriteImage_Tentacle_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Tentacle_01_sequence), PeepSpriteImage_Tentacle_01_sequence },
	{ 8061,  countof(PeepSpriteImage_Tentacle_02_sequence), PeepSpriteImage_Tentacle_02_sequence },
	{ 8285,  countof(PeepSpriteImage_Tentacle_03_sequence), PeepSpriteImage_Tentacle_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Tentacle_04_sequence), PeepSpriteImage_Tentacle_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Tentacle_05_sequence), PeepSpriteImage_Tentacle_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Tentacle_06_sequence), PeepSpriteImage_Tentacle_06_sequence },
	{ 8185,  countof(PeepSpriteImage_Tentacle_07_sequence), PeepSpriteImage_Tentacle_07_sequence },
	{ 8189,  countof(PeepSpriteImage_Tentacle_08_sequence), PeepSpriteImage_Tentacle_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Tentacle_09_sequence), PeepSpriteImage_Tentacle_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Tentacle_10_sequence), PeepSpriteImage_Tentacle_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Tentacle_11_sequence), PeepSpriteImage_Tentacle_11_sequence },
	{ 11301, countof(PeepSpriteImage_Tentacle_12_sequence), PeepSpriteImage_Tentacle_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Tentacle_13_sequence), PeepSpriteImage_Tentacle_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Tentacle_14_sequence), PeepSpriteImage_Tentacle_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Tentacle_15_sequence), PeepSpriteImage_Tentacle_15_sequence },
	{ 11325, countof(PeepSpriteImage_Tentacle_16_sequence), PeepSpriteImage_Tentacle_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Tentacle_17_sequence), PeepSpriteImage_Tentacle_17_sequence },
	{ 11513, countof(PeepSpriteImage_Tentacle_18_sequence), PeepSpriteImage_Tentacle_18_sequence },
	{ 11513, countof(PeepSpriteImage_Tentacle_19_sequence), PeepSpriteImage_Tentacle_19_sequence },
	{ 11597, countof(PeepSpriteImage_Tentacle_20_sequence), PeepSpriteImage_Tentacle_20_sequence },
	{ 11669, countof(PeepSpriteImage_Tentacle_21_sequence), PeepSpriteImage_Tentacle_21_sequence },
	{ 11669, countof(PeepSpriteImage_Tentacle_22_sequence), PeepSpriteImage_Tentacle_22_sequence },
	{ 11801, countof(PeepSpriteImage_Tentacle_23_sequence), PeepSpriteImage_Tentacle_23_sequence },
	{ 11669, countof(PeepSpriteImage_Tentacle_24_sequence), PeepSpriteImage_Tentacle_24_sequence },
	{ 11377, countof(PeepSpriteImage_Tentacle_25_sequence), PeepSpriteImage_Tentacle_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Tentacle_26_sequence), PeepSpriteImage_Tentacle_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Tentacle_27_sequence), PeepSpriteImage_Tentacle_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Tentacle_28_sequence), PeepSpriteImage_Tentacle_28_sequence },
	{ 11401, countof(PeepSpriteImage_Tentacle_29_sequence), PeepSpriteImage_Tentacle_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Tentacle_30_sequence), PeepSpriteImage_Tentacle_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Tentacle_31_sequence), PeepSpriteImage_Tentacle_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Tentacle_32_sequence), PeepSpriteImage_Tentacle_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Tentacle_33_sequence), PeepSpriteImage_Tentacle_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Tentacle_34_sequence), PeepSpriteImage_Tentacle_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Tentacle_35_sequence), PeepSpriteImage_Tentacle_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Tentacle_36_sequence), PeepSpriteImage_Tentacle_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_ToffeeApple[] = {
	{ 7401,  countof(PeepSpriteImage_ToffeeApple_00_sequence), PeepSpriteImage_ToffeeApple_00_sequence },
	{ 6585,  countof(PeepSpriteImage_ToffeeApple_01_sequence), PeepSpriteImage_ToffeeApple_01_sequence },
	{ 7397,  countof(PeepSpriteImage_ToffeeApple_02_sequence), PeepSpriteImage_ToffeeApple_02_sequence },
	{ 7477,  countof(PeepSpriteImage_ToffeeApple_03_sequence), PeepSpriteImage_ToffeeApple_03_sequence },
	{ 6697,  countof(PeepSpriteImage_ToffeeApple_04_sequence), PeepSpriteImage_ToffeeApple_04_sequence },
	{ 6757,  countof(PeepSpriteImage_ToffeeApple_05_sequence), PeepSpriteImage_ToffeeApple_05_sequence },
	{ 6561,  countof(PeepSpriteImage_ToffeeApple_06_sequence), PeepSpriteImage_ToffeeApple_06_sequence },
	{ 7449,  countof(PeepSpriteImage_ToffeeApple_07_sequence), PeepSpriteImage_ToffeeApple_07_sequence },
	{ 7453,  countof(PeepSpriteImage_ToffeeApple_08_sequence), PeepSpriteImage_ToffeeApple_08_sequence },
	{ 6637,  countof(PeepSpriteImage_ToffeeApple_09_sequence), PeepSpriteImage_ToffeeApple_09_sequence },
	{ 6645,  countof(PeepSpriteImage_ToffeeApple_10_sequence), PeepSpriteImage_ToffeeApple_10_sequence },
	{ 6813,  countof(PeepSpriteImage_ToffeeApple_11_sequence), PeepSpriteImage_ToffeeApple_11_sequence },
	{ 11301, countof(PeepSpriteImage_ToffeeApple_12_sequence), PeepSpriteImage_ToffeeApple_12_sequence },
	{ 6825,  countof(PeepSpriteImage_ToffeeApple_13_sequence), PeepSpriteImage_ToffeeApple_13_sequence },
	{ 6845,  countof(PeepSpriteImage_ToffeeApple_14_sequence), PeepSpriteImage_ToffeeApple_14_sequence },
	{ 6909,  countof(PeepSpriteImage_ToffeeApple_15_sequence), PeepSpriteImage_ToffeeApple_15_sequence },
	{ 11325, countof(PeepSpriteImage_ToffeeApple_16_sequence), PeepSpriteImage_ToffeeApple_16_sequence },
	{ 6957,  countof(PeepSpriteImage_ToffeeApple_17_sequence), PeepSpriteImage_ToffeeApple_17_sequence },
	{ 11513, countof(PeepSpriteImage_ToffeeApple_18_sequence), PeepSpriteImage_ToffeeApple_18_sequence },
	{ 11513, countof(PeepSpriteImage_ToffeeApple_19_sequence), PeepSpriteImage_ToffeeApple_19_sequence },
	{ 11597, countof(PeepSpriteImage_ToffeeApple_20_sequence), PeepSpriteImage_ToffeeApple_20_sequence },
	{ 11669, countof(PeepSpriteImage_ToffeeApple_21_sequence), PeepSpriteImage_ToffeeApple_21_sequence },
	{ 11669, countof(PeepSpriteImage_ToffeeApple_22_sequence), PeepSpriteImage_ToffeeApple_22_sequence },
	{ 11801, countof(PeepSpriteImage_ToffeeApple_23_sequence), PeepSpriteImage_ToffeeApple_23_sequence },
	{ 11669, countof(PeepSpriteImage_ToffeeApple_24_sequence), PeepSpriteImage_ToffeeApple_24_sequence },
	{ 11377, countof(PeepSpriteImage_ToffeeApple_25_sequence), PeepSpriteImage_ToffeeApple_25_sequence },
	{ 6989,  countof(PeepSpriteImage_ToffeeApple_26_sequence), PeepSpriteImage_ToffeeApple_26_sequence },
	{ 7041,  countof(PeepSpriteImage_ToffeeApple_27_sequence), PeepSpriteImage_ToffeeApple_27_sequence },
	{ 7101,  countof(PeepSpriteImage_ToffeeApple_28_sequence), PeepSpriteImage_ToffeeApple_28_sequence },
	{ 11401, countof(PeepSpriteImage_ToffeeApple_29_sequence), PeepSpriteImage_ToffeeApple_29_sequence },
	{ 7121,  countof(PeepSpriteImage_ToffeeApple_30_sequence), PeepSpriteImage_ToffeeApple_30_sequence },
	{ 7149,  countof(PeepSpriteImage_ToffeeApple_31_sequence), PeepSpriteImage_ToffeeApple_31_sequence },
	{ 7249,  countof(PeepSpriteImage_ToffeeApple_32_sequence), PeepSpriteImage_ToffeeApple_32_sequence },
	{ 7309,  countof(PeepSpriteImage_ToffeeApple_33_sequence), PeepSpriteImage_ToffeeApple_33_sequence },
	{ 7329,  countof(PeepSpriteImage_ToffeeApple_34_sequence), PeepSpriteImage_ToffeeApple_34_sequence },
	{ 7285,  countof(PeepSpriteImage_ToffeeApple_35_sequence), PeepSpriteImage_ToffeeApple_35_sequence },
	{ 7181,  countof(PeepSpriteImage_ToffeeApple_36_sequence), PeepSpriteImage_ToffeeApple_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Donut[] = {
	{ 8561,  countof(PeepSpriteImage_Donut_00_sequence), PeepSpriteImage_Donut_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Donut_01_sequence), PeepSpriteImage_Donut_01_sequence },
	{ 8557,  countof(PeepSpriteImage_Donut_02_sequence), PeepSpriteImage_Donut_02_sequence },
	{ 8661,  countof(PeepSpriteImage_Donut_03_sequence), PeepSpriteImage_Donut_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Donut_04_sequence), PeepSpriteImage_Donut_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Donut_05_sequence), PeepSpriteImage_Donut_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Donut_06_sequence), PeepSpriteImage_Donut_06_sequence },
	{ 8633,  countof(PeepSpriteImage_Donut_07_sequence), PeepSpriteImage_Donut_07_sequence },
	{ 8637,  countof(PeepSpriteImage_Donut_08_sequence), PeepSpriteImage_Donut_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Donut_09_sequence), PeepSpriteImage_Donut_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Donut_10_sequence), PeepSpriteImage_Donut_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Donut_11_sequence), PeepSpriteImage_Donut_11_sequence },
	{ 11301, countof(PeepSpriteImage_Donut_12_sequence), PeepSpriteImage_Donut_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Donut_13_sequence), PeepSpriteImage_Donut_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Donut_14_sequence), PeepSpriteImage_Donut_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Donut_15_sequence), PeepSpriteImage_Donut_15_sequence },
	{ 11325, countof(PeepSpriteImage_Donut_16_sequence), PeepSpriteImage_Donut_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Donut_17_sequence), PeepSpriteImage_Donut_17_sequence },
	{ 11513, countof(PeepSpriteImage_Donut_18_sequence), PeepSpriteImage_Donut_18_sequence },
	{ 11513, countof(PeepSpriteImage_Donut_19_sequence), PeepSpriteImage_Donut_19_sequence },
	{ 11597, countof(PeepSpriteImage_Donut_20_sequence), PeepSpriteImage_Donut_20_sequence },
	{ 11669, countof(PeepSpriteImage_Donut_21_sequence), PeepSpriteImage_Donut_21_sequence },
	{ 11669, countof(PeepSpriteImage_Donut_22_sequence), PeepSpriteImage_Donut_22_sequence },
	{ 11801, countof(PeepSpriteImage_Donut_23_sequence), PeepSpriteImage_Donut_23_sequence },
	{ 11669, countof(PeepSpriteImage_Donut_24_sequence), PeepSpriteImage_Donut_24_sequence },
	{ 11377, countof(PeepSpriteImage_Donut_25_sequence), PeepSpriteImage_Donut_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Donut_26_sequence), PeepSpriteImage_Donut_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Donut_27_sequence), PeepSpriteImage_Donut_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Donut_28_sequence), PeepSpriteImage_Donut_28_sequence },
	{ 11401, countof(PeepSpriteImage_Donut_29_sequence), PeepSpriteImage_Donut_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Donut_30_sequence), PeepSpriteImage_Donut_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Donut_31_sequence), PeepSpriteImage_Donut_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Donut_32_sequence), PeepSpriteImage_Donut_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Donut_33_sequence), PeepSpriteImage_Donut_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Donut_34_sequence), PeepSpriteImage_Donut_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Donut_35_sequence), PeepSpriteImage_Donut_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Donut_36_sequence), PeepSpriteImage_Donut_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Coffee[] = {
	{ 8885,  countof(PeepSpriteImage_Coffee_00_sequence), PeepSpriteImage_Coffee_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Coffee_01_sequence), PeepSpriteImage_Coffee_01_sequence },
	{ 8981,  countof(PeepSpriteImage_Coffee_02_sequence), PeepSpriteImage_Coffee_02_sequence },
	{ 8981,  countof(PeepSpriteImage_Coffee_03_sequence), PeepSpriteImage_Coffee_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Coffee_04_sequence), PeepSpriteImage_Coffee_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Coffee_05_sequence), PeepSpriteImage_Coffee_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Coffee_06_sequence), PeepSpriteImage_Coffee_06_sequence },
	{ 8957,  countof(PeepSpriteImage_Coffee_07_sequence), PeepSpriteImage_Coffee_07_sequence },
	{ 8957,  countof(PeepSpriteImage_Coffee_08_sequence), PeepSpriteImage_Coffee_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Coffee_09_sequence), PeepSpriteImage_Coffee_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Coffee_10_sequence), PeepSpriteImage_Coffee_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Coffee_11_sequence), PeepSpriteImage_Coffee_11_sequence },
	{ 11301, countof(PeepSpriteImage_Coffee_12_sequence), PeepSpriteImage_Coffee_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Coffee_13_sequence), PeepSpriteImage_Coffee_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Coffee_14_sequence), PeepSpriteImage_Coffee_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Coffee_15_sequence), PeepSpriteImage_Coffee_15_sequence },
	{ 11325, countof(PeepSpriteImage_Coffee_16_sequence), PeepSpriteImage_Coffee_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Coffee_17_sequence), PeepSpriteImage_Coffee_17_sequence },
	{ 11513, countof(PeepSpriteImage_Coffee_18_sequence), PeepSpriteImage_Coffee_18_sequence },
	{ 11513, countof(PeepSpriteImage_Coffee_19_sequence), PeepSpriteImage_Coffee_19_sequence },
	{ 11597, countof(PeepSpriteImage_Coffee_20_sequence), PeepSpriteImage_Coffee_20_sequence },
	{ 11669, countof(PeepSpriteImage_Coffee_21_sequence), PeepSpriteImage_Coffee_21_sequence },
	{ 11669, countof(PeepSpriteImage_Coffee_22_sequence), PeepSpriteImage_Coffee_22_sequence },
	{ 11801, countof(PeepSpriteImage_Coffee_23_sequence), PeepSpriteImage_Coffee_23_sequence },
	{ 11669, countof(PeepSpriteImage_Coffee_24_sequence), PeepSpriteImage_Coffee_24_sequence },
	{ 11377, countof(PeepSpriteImage_Coffee_25_sequence), PeepSpriteImage_Coffee_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Coffee_26_sequence), PeepSpriteImage_Coffee_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Coffee_27_sequence), PeepSpriteImage_Coffee_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Coffee_28_sequence), PeepSpriteImage_Coffee_28_sequence },
	{ 11401, countof(PeepSpriteImage_Coffee_29_sequence), PeepSpriteImage_Coffee_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Coffee_30_sequence), PeepSpriteImage_Coffee_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Coffee_31_sequence), PeepSpriteImage_Coffee_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Coffee_32_sequence), PeepSpriteImage_Coffee_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Coffee_33_sequence), PeepSpriteImage_Coffee_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Coffee_34_sequence), PeepSpriteImage_Coffee_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Coffee_35_sequence), PeepSpriteImage_Coffee_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Coffee_36_sequence), PeepSpriteImage_Coffee_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Chicken[] = {
	{ 9005,  countof(PeepSpriteImage_Chicken_00_sequence), PeepSpriteImage_Chicken_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Chicken_01_sequence), PeepSpriteImage_Chicken_01_sequence },
	{ 9125,  countof(PeepSpriteImage_Chicken_02_sequence), PeepSpriteImage_Chicken_02_sequence },
	{ 9125,  countof(PeepSpriteImage_Chicken_03_sequence), PeepSpriteImage_Chicken_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Chicken_04_sequence), PeepSpriteImage_Chicken_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Chicken_05_sequence), PeepSpriteImage_Chicken_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Chicken_06_sequence), PeepSpriteImage_Chicken_06_sequence },
	{ 9077,  countof(PeepSpriteImage_Chicken_07_sequence), PeepSpriteImage_Chicken_07_sequence },
	{ 9077,  countof(PeepSpriteImage_Chicken_08_sequence), PeepSpriteImage_Chicken_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Chicken_09_sequence), PeepSpriteImage_Chicken_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Chicken_10_sequence), PeepSpriteImage_Chicken_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Chicken_11_sequence), PeepSpriteImage_Chicken_11_sequence },
	{ 11301, countof(PeepSpriteImage_Chicken_12_sequence), PeepSpriteImage_Chicken_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Chicken_13_sequence), PeepSpriteImage_Chicken_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Chicken_14_sequence), PeepSpriteImage_Chicken_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Chicken_15_sequence), PeepSpriteImage_Chicken_15_sequence },
	{ 11325, countof(PeepSpriteImage_Chicken_16_sequence), PeepSpriteImage_Chicken_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Chicken_17_sequence), PeepSpriteImage_Chicken_17_sequence },
	{ 11513, countof(PeepSpriteImage_Chicken_18_sequence), PeepSpriteImage_Chicken_18_sequence },
	{ 11513, countof(PeepSpriteImage_Chicken_19_sequence), PeepSpriteImage_Chicken_19_sequence },
	{ 11597, countof(PeepSpriteImage_Chicken_20_sequence), PeepSpriteImage_Chicken_20_sequence },
	{ 11669, countof(PeepSpriteImage_Chicken_21_sequence), PeepSpriteImage_Chicken_21_sequence },
	{ 11669, countof(PeepSpriteImage_Chicken_22_sequence), PeepSpriteImage_Chicken_22_sequence },
	{ 11801, countof(PeepSpriteImage_Chicken_23_sequence), PeepSpriteImage_Chicken_23_sequence },
	{ 11669, countof(PeepSpriteImage_Chicken_24_sequence), PeepSpriteImage_Chicken_24_sequence },
	{ 11377, countof(PeepSpriteImage_Chicken_25_sequence), PeepSpriteImage_Chicken_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Chicken_26_sequence), PeepSpriteImage_Chicken_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Chicken_27_sequence), PeepSpriteImage_Chicken_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Chicken_28_sequence), PeepSpriteImage_Chicken_28_sequence },
	{ 11401, countof(PeepSpriteImage_Chicken_29_sequence), PeepSpriteImage_Chicken_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Chicken_30_sequence), PeepSpriteImage_Chicken_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Chicken_31_sequence), PeepSpriteImage_Chicken_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Chicken_32_sequence), PeepSpriteImage_Chicken_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Chicken_33_sequence), PeepSpriteImage_Chicken_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Chicken_34_sequence), PeepSpriteImage_Chicken_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Chicken_35_sequence), PeepSpriteImage_Chicken_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Chicken_36_sequence), PeepSpriteImage_Chicken_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Lemonade[] = {
	{ 9173,  countof(PeepSpriteImage_Lemonade_00_sequence), PeepSpriteImage_Lemonade_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Lemonade_01_sequence), PeepSpriteImage_Lemonade_01_sequence },
	{ 9293,  countof(PeepSpriteImage_Lemonade_02_sequence), PeepSpriteImage_Lemonade_02_sequence },
	{ 9293,  countof(PeepSpriteImage_Lemonade_03_sequence), PeepSpriteImage_Lemonade_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Lemonade_04_sequence), PeepSpriteImage_Lemonade_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Lemonade_05_sequence), PeepSpriteImage_Lemonade_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Lemonade_06_sequence), PeepSpriteImage_Lemonade_06_sequence },
	{ 9245,  countof(PeepSpriteImage_Lemonade_07_sequence), PeepSpriteImage_Lemonade_07_sequence },
	{ 9245,  countof(PeepSpriteImage_Lemonade_08_sequence), PeepSpriteImage_Lemonade_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Lemonade_09_sequence), PeepSpriteImage_Lemonade_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Lemonade_10_sequence), PeepSpriteImage_Lemonade_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Lemonade_11_sequence), PeepSpriteImage_Lemonade_11_sequence },
	{ 11301, countof(PeepSpriteImage_Lemonade_12_sequence), PeepSpriteImage_Lemonade_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Lemonade_13_sequence), PeepSpriteImage_Lemonade_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Lemonade_14_sequence), PeepSpriteImage_Lemonade_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Lemonade_15_sequence), PeepSpriteImage_Lemonade_15_sequence },
	{ 11325, countof(PeepSpriteImage_Lemonade_16_sequence), PeepSpriteImage_Lemonade_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Lemonade_17_sequence), PeepSpriteImage_Lemonade_17_sequence },
	{ 11513, countof(PeepSpriteImage_Lemonade_18_sequence), PeepSpriteImage_Lemonade_18_sequence },
	{ 11513, countof(PeepSpriteImage_Lemonade_19_sequence), PeepSpriteImage_Lemonade_19_sequence },
	{ 11597, countof(PeepSpriteImage_Lemonade_20_sequence), PeepSpriteImage_Lemonade_20_sequence },
	{ 11669, countof(PeepSpriteImage_Lemonade_21_sequence), PeepSpriteImage_Lemonade_21_sequence },
	{ 11669, countof(PeepSpriteImage_Lemonade_22_sequence), PeepSpriteImage_Lemonade_22_sequence },
	{ 11801, countof(PeepSpriteImage_Lemonade_23_sequence), PeepSpriteImage_Lemonade_23_sequence },
	{ 11669, countof(PeepSpriteImage_Lemonade_24_sequence), PeepSpriteImage_Lemonade_24_sequence },
	{ 11377, countof(PeepSpriteImage_Lemonade_25_sequence), PeepSpriteImage_Lemonade_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Lemonade_26_sequence), PeepSpriteImage_Lemonade_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Lemonade_27_sequence), PeepSpriteImage_Lemonade_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Lemonade_28_sequence), PeepSpriteImage_Lemonade_28_sequence },
	{ 11401, countof(PeepSpriteImage_Lemonade_29_sequence), PeepSpriteImage_Lemonade_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Lemonade_30_sequence), PeepSpriteImage_Lemonade_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Lemonade_31_sequence), PeepSpriteImage_Lemonade_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Lemonade_32_sequence), PeepSpriteImage_Lemonade_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Lemonade_33_sequence), PeepSpriteImage_Lemonade_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Lemonade_34_sequence), PeepSpriteImage_Lemonade_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Lemonade_35_sequence), PeepSpriteImage_Lemonade_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Lemonade_36_sequence), PeepSpriteImage_Lemonade_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Watching[] = {
	{ 6409,  countof(PeepSpriteImage_Watching_00_sequence), PeepSpriteImage_Watching_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Watching_01_sequence), PeepSpriteImage_Watching_01_sequence },
	{ 7245,  countof(PeepSpriteImage_Watching_02_sequence), PeepSpriteImage_Watching_02_sequence },
	{ 6653,  countof(PeepSpriteImage_Watching_03_sequence), PeepSpriteImage_Watching_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Watching_04_sequence), PeepSpriteImage_Watching_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Watching_05_sequence), PeepSpriteImage_Watching_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Watching_06_sequence), PeepSpriteImage_Watching_06_sequence },
	{ 6625,  countof(PeepSpriteImage_Watching_07_sequence), PeepSpriteImage_Watching_07_sequence },
	{ 6629,  countof(PeepSpriteImage_Watching_08_sequence), PeepSpriteImage_Watching_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Watching_09_sequence), PeepSpriteImage_Watching_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Watching_10_sequence), PeepSpriteImage_Watching_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Watching_11_sequence), PeepSpriteImage_Watching_11_sequence },
	{ 11301, countof(PeepSpriteImage_Watching_12_sequence), PeepSpriteImage_Watching_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Watching_13_sequence), PeepSpriteImage_Watching_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Watching_14_sequence), PeepSpriteImage_Watching_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Watching_15_sequence), PeepSpriteImage_Watching_15_sequence },
	{ 11325, countof(PeepSpriteImage_Watching_16_sequence), PeepSpriteImage_Watching_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Watching_17_sequence), PeepSpriteImage_Watching_17_sequence },
	{ 11513, countof(PeepSpriteImage_Watching_18_sequence), PeepSpriteImage_Watching_18_sequence },
	{ 11513, countof(PeepSpriteImage_Watching_19_sequence), PeepSpriteImage_Watching_19_sequence },
	{ 11597, countof(PeepSpriteImage_Watching_20_sequence), PeepSpriteImage_Watching_20_sequence },
	{ 11669, countof(PeepSpriteImage_Watching_21_sequence), PeepSpriteImage_Watching_21_sequence },
	{ 11669, countof(PeepSpriteImage_Watching_22_sequence), PeepSpriteImage_Watching_22_sequence },
	{ 11801, countof(PeepSpriteImage_Watching_23_sequence), PeepSpriteImage_Watching_23_sequence },
	{ 11669, countof(PeepSpriteImage_Watching_24_sequence), PeepSpriteImage_Watching_24_sequence },
	{ 11377, countof(PeepSpriteImage_Watching_25_sequence), PeepSpriteImage_Watching_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Watching_26_sequence), PeepSpriteImage_Watching_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Watching_27_sequence), PeepSpriteImage_Watching_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Watching_28_sequence), PeepSpriteImage_Watching_28_sequence },
	{ 11401, countof(PeepSpriteImage_Watching_29_sequence), PeepSpriteImage_Watching_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Watching_30_sequence), PeepSpriteImage_Watching_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Watching_31_sequence), PeepSpriteImage_Watching_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Watching_32_sequence), PeepSpriteImage_Watching_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Watching_33_sequence), PeepSpriteImage_Watching_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Watching_34_sequence), PeepSpriteImage_Watching_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Watching_35_sequence), PeepSpriteImage_Watching_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Watching_36_sequence), PeepSpriteImage_Watching_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Pretzel[] = {
	{ 9341,  countof(PeepSpriteImage_Pretzel_00_sequence), PeepSpriteImage_Pretzel_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Pretzel_01_sequence), PeepSpriteImage_Pretzel_01_sequence },
	{ 9461,  countof(PeepSpriteImage_Pretzel_02_sequence), PeepSpriteImage_Pretzel_02_sequence },
	{ 9461,  countof(PeepSpriteImage_Pretzel_03_sequence), PeepSpriteImage_Pretzel_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Pretzel_04_sequence), PeepSpriteImage_Pretzel_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Pretzel_05_sequence), PeepSpriteImage_Pretzel_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Pretzel_06_sequence), PeepSpriteImage_Pretzel_06_sequence },
	{ 9413,  countof(PeepSpriteImage_Pretzel_07_sequence), PeepSpriteImage_Pretzel_07_sequence },
	{ 9413,  countof(PeepSpriteImage_Pretzel_08_sequence), PeepSpriteImage_Pretzel_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Pretzel_09_sequence), PeepSpriteImage_Pretzel_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Pretzel_10_sequence), PeepSpriteImage_Pretzel_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Pretzel_11_sequence), PeepSpriteImage_Pretzel_11_sequence },
	{ 11301, countof(PeepSpriteImage_Pretzel_12_sequence), PeepSpriteImage_Pretzel_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Pretzel_13_sequence), PeepSpriteImage_Pretzel_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Pretzel_14_sequence), PeepSpriteImage_Pretzel_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Pretzel_15_sequence), PeepSpriteImage_Pretzel_15_sequence },
	{ 11325, countof(PeepSpriteImage_Pretzel_16_sequence), PeepSpriteImage_Pretzel_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Pretzel_17_sequence), PeepSpriteImage_Pretzel_17_sequence },
	{ 11513, countof(PeepSpriteImage_Pretzel_18_sequence), PeepSpriteImage_Pretzel_18_sequence },
	{ 11513, countof(PeepSpriteImage_Pretzel_19_sequence), PeepSpriteImage_Pretzel_19_sequence },
	{ 11597, countof(PeepSpriteImage_Pretzel_20_sequence), PeepSpriteImage_Pretzel_20_sequence },
	{ 11669, countof(PeepSpriteImage_Pretzel_21_sequence), PeepSpriteImage_Pretzel_21_sequence },
	{ 11669, countof(PeepSpriteImage_Pretzel_22_sequence), PeepSpriteImage_Pretzel_22_sequence },
	{ 11801, countof(PeepSpriteImage_Pretzel_23_sequence), PeepSpriteImage_Pretzel_23_sequence },
	{ 11669, countof(PeepSpriteImage_Pretzel_24_sequence), PeepSpriteImage_Pretzel_24_sequence },
	{ 11377, countof(PeepSpriteImage_Pretzel_25_sequence), PeepSpriteImage_Pretzel_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Pretzel_26_sequence), PeepSpriteImage_Pretzel_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Pretzel_27_sequence), PeepSpriteImage_Pretzel_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Pretzel_28_sequence), PeepSpriteImage_Pretzel_28_sequence },
	{ 11401, countof(PeepSpriteImage_Pretzel_29_sequence), PeepSpriteImage_Pretzel_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Pretzel_30_sequence), PeepSpriteImage_Pretzel_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Pretzel_31_sequence), PeepSpriteImage_Pretzel_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Pretzel_32_sequence), PeepSpriteImage_Pretzel_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Pretzel_33_sequence), PeepSpriteImage_Pretzel_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Pretzel_34_sequence), PeepSpriteImage_Pretzel_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Pretzel_35_sequence), PeepSpriteImage_Pretzel_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Pretzel_36_sequence), PeepSpriteImage_Pretzel_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Sunglasses[] = {
	{ 10685, countof(PeepSpriteImage_Sunglasses_00_sequence), PeepSpriteImage_Sunglasses_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Sunglasses_01_sequence), PeepSpriteImage_Sunglasses_01_sequence },
	{ 10713, countof(PeepSpriteImage_Sunglasses_02_sequence), PeepSpriteImage_Sunglasses_02_sequence },
	{ 6653,  countof(PeepSpriteImage_Sunglasses_03_sequence), PeepSpriteImage_Sunglasses_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Sunglasses_04_sequence), PeepSpriteImage_Sunglasses_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Sunglasses_05_sequence), PeepSpriteImage_Sunglasses_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Sunglasses_06_sequence), PeepSpriteImage_Sunglasses_06_sequence },
	{ 10709, countof(PeepSpriteImage_Sunglasses_07_sequence), PeepSpriteImage_Sunglasses_07_sequence },
	{ 6629,  countof(PeepSpriteImage_Sunglasses_08_sequence), PeepSpriteImage_Sunglasses_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Sunglasses_09_sequence), PeepSpriteImage_Sunglasses_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Sunglasses_10_sequence), PeepSpriteImage_Sunglasses_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Sunglasses_11_sequence), PeepSpriteImage_Sunglasses_11_sequence },
	{ 11301, countof(PeepSpriteImage_Sunglasses_12_sequence), PeepSpriteImage_Sunglasses_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Sunglasses_13_sequence), PeepSpriteImage_Sunglasses_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Sunglasses_14_sequence), PeepSpriteImage_Sunglasses_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Sunglasses_15_sequence), PeepSpriteImage_Sunglasses_15_sequence },
	{ 11325, countof(PeepSpriteImage_Sunglasses_16_sequence), PeepSpriteImage_Sunglasses_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Sunglasses_17_sequence), PeepSpriteImage_Sunglasses_17_sequence },
	{ 11513, countof(PeepSpriteImage_Sunglasses_18_sequence), PeepSpriteImage_Sunglasses_18_sequence },
	{ 11513, countof(PeepSpriteImage_Sunglasses_19_sequence), PeepSpriteImage_Sunglasses_19_sequence },
	{ 11597, countof(PeepSpriteImage_Sunglasses_20_sequence), PeepSpriteImage_Sunglasses_20_sequence },
	{ 11669, countof(PeepSpriteImage_Sunglasses_21_sequence), PeepSpriteImage_Sunglasses_21_sequence },
	{ 11669, countof(PeepSpriteImage_Sunglasses_22_sequence), PeepSpriteImage_Sunglasses_22_sequence },
	{ 11801, countof(PeepSpriteImage_Sunglasses_23_sequence), PeepSpriteImage_Sunglasses_23_sequence },
	{ 11669, countof(PeepSpriteImage_Sunglasses_24_sequence), PeepSpriteImage_Sunglasses_24_sequence },
	{ 11377, countof(PeepSpriteImage_Sunglasses_25_sequence), PeepSpriteImage_Sunglasses_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Sunglasses_26_sequence), PeepSpriteImage_Sunglasses_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Sunglasses_27_sequence), PeepSpriteImage_Sunglasses_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Sunglasses_28_sequence), PeepSpriteImage_Sunglasses_28_sequence },
	{ 11401, countof(PeepSpriteImage_Sunglasses_29_sequence), PeepSpriteImage_Sunglasses_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Sunglasses_30_sequence), PeepSpriteImage_Sunglasses_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Sunglasses_31_sequence), PeepSpriteImage_Sunglasses_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Sunglasses_32_sequence), PeepSpriteImage_Sunglasses_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Sunglasses_33_sequence), PeepSpriteImage_Sunglasses_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Sunglasses_34_sequence), PeepSpriteImage_Sunglasses_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Sunglasses_35_sequence), PeepSpriteImage_Sunglasses_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Sunglasses_36_sequence), PeepSpriteImage_Sunglasses_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_SuJongkwa[] = {
	{ 9509,  countof(PeepSpriteImage_SuJongkwa_00_sequence), PeepSpriteImage_SuJongkwa_00_sequence },
	{ 6585,  countof(PeepSpriteImage_SuJongkwa_01_sequence), PeepSpriteImage_SuJongkwa_01_sequence },
	{ 9629,  countof(PeepSpriteImage_SuJongkwa_02_sequence), PeepSpriteImage_SuJongkwa_02_sequence },
	{ 9629,  countof(PeepSpriteImage_SuJongkwa_03_sequence), PeepSpriteImage_SuJongkwa_03_sequence },
	{ 6697,  countof(PeepSpriteImage_SuJongkwa_04_sequence), PeepSpriteImage_SuJongkwa_04_sequence },
	{ 6757,  countof(PeepSpriteImage_SuJongkwa_05_sequence), PeepSpriteImage_SuJongkwa_05_sequence },
	{ 6561,  countof(PeepSpriteImage_SuJongkwa_06_sequence), PeepSpriteImage_SuJongkwa_06_sequence },
	{ 9581,  countof(PeepSpriteImage_SuJongkwa_07_sequence), PeepSpriteImage_SuJongkwa_07_sequence },
	{ 9581,  countof(PeepSpriteImage_SuJongkwa_08_sequence), PeepSpriteImage_SuJongkwa_08_sequence },
	{ 6637,  countof(PeepSpriteImage_SuJongkwa_09_sequence), PeepSpriteImage_SuJongkwa_09_sequence },
	{ 6645,  countof(PeepSpriteImage_SuJongkwa_10_sequence), PeepSpriteImage_SuJongkwa_10_sequence },
	{ 6813,  countof(PeepSpriteImage_SuJongkwa_11_sequence), PeepSpriteImage_SuJongkwa_11_sequence },
	{ 11301, countof(PeepSpriteImage_SuJongkwa_12_sequence), PeepSpriteImage_SuJongkwa_12_sequence },
	{ 6825,  countof(PeepSpriteImage_SuJongkwa_13_sequence), PeepSpriteImage_SuJongkwa_13_sequence },
	{ 6845,  countof(PeepSpriteImage_SuJongkwa_14_sequence), PeepSpriteImage_SuJongkwa_14_sequence },
	{ 6909,  countof(PeepSpriteImage_SuJongkwa_15_sequence), PeepSpriteImage_SuJongkwa_15_sequence },
	{ 11325, countof(PeepSpriteImage_SuJongkwa_16_sequence), PeepSpriteImage_SuJongkwa_16_sequence },
	{ 6957,  countof(PeepSpriteImage_SuJongkwa_17_sequence), PeepSpriteImage_SuJongkwa_17_sequence },
	{ 11513, countof(PeepSpriteImage_SuJongkwa_18_sequence), PeepSpriteImage_SuJongkwa_18_sequence },
	{ 11513, countof(PeepSpriteImage_SuJongkwa_19_sequence), PeepSpriteImage_SuJongkwa_19_sequence },
	{ 11597, countof(PeepSpriteImage_SuJongkwa_20_sequence), PeepSpriteImage_SuJongkwa_20_sequence },
	{ 11669, countof(PeepSpriteImage_SuJongkwa_21_sequence), PeepSpriteImage_SuJongkwa_21_sequence },
	{ 11669, countof(PeepSpriteImage_SuJongkwa_22_sequence), PeepSpriteImage_SuJongkwa_22_sequence },
	{ 11801, countof(PeepSpriteImage_SuJongkwa_23_sequence), PeepSpriteImage_SuJongkwa_23_sequence },
	{ 11669, countof(PeepSpriteImage_SuJongkwa_24_sequence), PeepSpriteImage_SuJongkwa_24_sequence },
	{ 11377, countof(PeepSpriteImage_SuJongkwa_25_sequence), PeepSpriteImage_SuJongkwa_25_sequence },
	{ 6989,  countof(PeepSpriteImage_SuJongkwa_26_sequence), PeepSpriteImage_SuJongkwa_26_sequence },
	{ 7041,  countof(PeepSpriteImage_SuJongkwa_27_sequence), PeepSpriteImage_SuJongkwa_27_sequence },
	{ 7101,  countof(PeepSpriteImage_SuJongkwa_28_sequence), PeepSpriteImage_SuJongkwa_28_sequence },
	{ 11401, countof(PeepSpriteImage_SuJongkwa_29_sequence), PeepSpriteImage_SuJongkwa_29_sequence },
	{ 7121,  countof(PeepSpriteImage_SuJongkwa_30_sequence), PeepSpriteImage_SuJongkwa_30_sequence },
	{ 7149,  countof(PeepSpriteImage_SuJongkwa_31_sequence), PeepSpriteImage_SuJongkwa_31_sequence },
	{ 7249,  countof(PeepSpriteImage_SuJongkwa_32_sequence), PeepSpriteImage_SuJongkwa_32_sequence },
	{ 7309,  countof(PeepSpriteImage_SuJongkwa_33_sequence), PeepSpriteImage_SuJongkwa_33_sequence },
	{ 7329,  countof(PeepSpriteImage_SuJongkwa_34_sequence), PeepSpriteImage_SuJongkwa_34_sequence },
	{ 7285,  countof(PeepSpriteImage_SuJongkwa_35_sequence), PeepSpriteImage_SuJongkwa_35_sequence },
	{ 7181,  countof(PeepSpriteImage_SuJongkwa_36_sequence), PeepSpriteImage_SuJongkwa_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Juice[] = {
	{ 9677,  countof(PeepSpriteImage_Juice_00_sequence), PeepSpriteImage_Juice_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Juice_01_sequence), PeepSpriteImage_Juice_01_sequence },
	{ 9797,  countof(PeepSpriteImage_Juice_02_sequence), PeepSpriteImage_Juice_02_sequence },
	{ 9797,  countof(PeepSpriteImage_Juice_03_sequence), PeepSpriteImage_Juice_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Juice_04_sequence), PeepSpriteImage_Juice_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Juice_05_sequence), PeepSpriteImage_Juice_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Juice_06_sequence), PeepSpriteImage_Juice_06_sequence },
	{ 9749,  countof(PeepSpriteImage_Juice_07_sequence), PeepSpriteImage_Juice_07_sequence },
	{ 9749,  countof(PeepSpriteImage_Juice_08_sequence), PeepSpriteImage_Juice_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Juice_09_sequence), PeepSpriteImage_Juice_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Juice_10_sequence), PeepSpriteImage_Juice_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Juice_11_sequence), PeepSpriteImage_Juice_11_sequence },
	{ 11301, countof(PeepSpriteImage_Juice_12_sequence), PeepSpriteImage_Juice_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Juice_13_sequence), PeepSpriteImage_Juice_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Juice_14_sequence), PeepSpriteImage_Juice_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Juice_15_sequence), PeepSpriteImage_Juice_15_sequence },
	{ 11325, countof(PeepSpriteImage_Juice_16_sequence), PeepSpriteImage_Juice_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Juice_17_sequence), PeepSpriteImage_Juice_17_sequence },
	{ 11513, countof(PeepSpriteImage_Juice_18_sequence), PeepSpriteImage_Juice_18_sequence },
	{ 11513, countof(PeepSpriteImage_Juice_19_sequence), PeepSpriteImage_Juice_19_sequence },
	{ 11597, countof(PeepSpriteImage_Juice_20_sequence), PeepSpriteImage_Juice_20_sequence },
	{ 11669, countof(PeepSpriteImage_Juice_21_sequence), PeepSpriteImage_Juice_21_sequence },
	{ 11669, countof(PeepSpriteImage_Juice_22_sequence), PeepSpriteImage_Juice_22_sequence },
	{ 11801, countof(PeepSpriteImage_Juice_23_sequence), PeepSpriteImage_Juice_23_sequence },
	{ 11669, countof(PeepSpriteImage_Juice_24_sequence), PeepSpriteImage_Juice_24_sequence },
	{ 11377, countof(PeepSpriteImage_Juice_25_sequence), PeepSpriteImage_Juice_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Juice_26_sequence), PeepSpriteImage_Juice_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Juice_27_sequence), PeepSpriteImage_Juice_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Juice_28_sequence), PeepSpriteImage_Juice_28_sequence },
	{ 11401, countof(PeepSpriteImage_Juice_29_sequence), PeepSpriteImage_Juice_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Juice_30_sequence), PeepSpriteImage_Juice_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Juice_31_sequence), PeepSpriteImage_Juice_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Juice_32_sequence), PeepSpriteImage_Juice_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Juice_33_sequence), PeepSpriteImage_Juice_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Juice_34_sequence), PeepSpriteImage_Juice_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Juice_35_sequence), PeepSpriteImage_Juice_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Juice_36_sequence), PeepSpriteImage_Juice_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_FunnelCake[] = {
	{ 9845,  countof(PeepSpriteImage_FunnelCake_00_sequence), PeepSpriteImage_FunnelCake_00_sequence },
	{ 6585,  countof(PeepSpriteImage_FunnelCake_01_sequence), PeepSpriteImage_FunnelCake_01_sequence },
	{ 9965,  countof(PeepSpriteImage_FunnelCake_02_sequence), PeepSpriteImage_FunnelCake_02_sequence },
	{ 9965,  countof(PeepSpriteImage_FunnelCake_03_sequence), PeepSpriteImage_FunnelCake_03_sequence },
	{ 6697,  countof(PeepSpriteImage_FunnelCake_04_sequence), PeepSpriteImage_FunnelCake_04_sequence },
	{ 6757,  countof(PeepSpriteImage_FunnelCake_05_sequence), PeepSpriteImage_FunnelCake_05_sequence },
	{ 6561,  countof(PeepSpriteImage_FunnelCake_06_sequence), PeepSpriteImage_FunnelCake_06_sequence },
	{ 9917,  countof(PeepSpriteImage_FunnelCake_07_sequence), PeepSpriteImage_FunnelCake_07_sequence },
	{ 9917,  countof(PeepSpriteImage_FunnelCake_08_sequence), PeepSpriteImage_FunnelCake_08_sequence },
	{ 6637,  countof(PeepSpriteImage_FunnelCake_09_sequence), PeepSpriteImage_FunnelCake_09_sequence },
	{ 6645,  countof(PeepSpriteImage_FunnelCake_10_sequence), PeepSpriteImage_FunnelCake_10_sequence },
	{ 6813,  countof(PeepSpriteImage_FunnelCake_11_sequence), PeepSpriteImage_FunnelCake_11_sequence },
	{ 11301, countof(PeepSpriteImage_FunnelCake_12_sequence), PeepSpriteImage_FunnelCake_12_sequence },
	{ 6825,  countof(PeepSpriteImage_FunnelCake_13_sequence), PeepSpriteImage_FunnelCake_13_sequence },
	{ 6845,  countof(PeepSpriteImage_FunnelCake_14_sequence), PeepSpriteImage_FunnelCake_14_sequence },
	{ 6909,  countof(PeepSpriteImage_FunnelCake_15_sequence), PeepSpriteImage_FunnelCake_15_sequence },
	{ 11325, countof(PeepSpriteImage_FunnelCake_16_sequence), PeepSpriteImage_FunnelCake_16_sequence },
	{ 6957,  countof(PeepSpriteImage_FunnelCake_17_sequence), PeepSpriteImage_FunnelCake_17_sequence },
	{ 11513, countof(PeepSpriteImage_FunnelCake_18_sequence), PeepSpriteImage_FunnelCake_18_sequence },
	{ 11513, countof(PeepSpriteImage_FunnelCake_19_sequence), PeepSpriteImage_FunnelCake_19_sequence },
	{ 11597, countof(PeepSpriteImage_FunnelCake_20_sequence), PeepSpriteImage_FunnelCake_20_sequence },
	{ 11669, countof(PeepSpriteImage_FunnelCake_21_sequence), PeepSpriteImage_FunnelCake_21_sequence },
	{ 11669, countof(PeepSpriteImage_FunnelCake_22_sequence), PeepSpriteImage_FunnelCake_22_sequence },
	{ 11801, countof(PeepSpriteImage_FunnelCake_23_sequence), PeepSpriteImage_FunnelCake_23_sequence },
	{ 11669, countof(PeepSpriteImage_FunnelCake_24_sequence), PeepSpriteImage_FunnelCake_24_sequence },
	{ 11377, countof(PeepSpriteImage_FunnelCake_25_sequence), PeepSpriteImage_FunnelCake_25_sequence },
	{ 6989,  countof(PeepSpriteImage_FunnelCake_26_sequence), PeepSpriteImage_FunnelCake_26_sequence },
	{ 7041,  countof(PeepSpriteImage_FunnelCake_27_sequence), PeepSpriteImage_FunnelCake_27_sequence },
	{ 7101,  countof(PeepSpriteImage_FunnelCake_28_sequence), PeepSpriteImage_FunnelCake_28_sequence },
	{ 11401, countof(PeepSpriteImage_FunnelCake_29_sequence), PeepSpriteImage_FunnelCake_29_sequence },
	{ 7121,  countof(PeepSpriteImage_FunnelCake_30_sequence), PeepSpriteImage_FunnelCake_30_sequence },
	{ 7149,  countof(PeepSpriteImage_FunnelCake_31_sequence), PeepSpriteImage_FunnelCake_31_sequence },
	{ 7249,  countof(PeepSpriteImage_FunnelCake_32_sequence), PeepSpriteImage_FunnelCake_32_sequence },
	{ 7309,  countof(PeepSpriteImage_FunnelCake_33_sequence), PeepSpriteImage_FunnelCake_33_sequence },
	{ 7329,  countof(PeepSpriteImage_FunnelCake_34_sequence), PeepSpriteImage_FunnelCake_34_sequence },
	{ 7285,  countof(PeepSpriteImage_FunnelCake_35_sequence), PeepSpriteImage_FunnelCake_35_sequence },
	{ 7181,  countof(PeepSpriteImage_FunnelCake_36_sequence), PeepSpriteImage_FunnelCake_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Noodles[] = {
	{ 10013, countof(PeepSpriteImage_Noodles_00_sequence), PeepSpriteImage_Noodles_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Noodles_01_sequence), PeepSpriteImage_Noodles_01_sequence },
	{ 10133, countof(PeepSpriteImage_Noodles_02_sequence), PeepSpriteImage_Noodles_02_sequence },
	{ 10133, countof(PeepSpriteImage_Noodles_03_sequence), PeepSpriteImage_Noodles_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Noodles_04_sequence), PeepSpriteImage_Noodles_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Noodles_05_sequence), PeepSpriteImage_Noodles_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Noodles_06_sequence), PeepSpriteImage_Noodles_06_sequence },
	{ 10085, countof(PeepSpriteImage_Noodles_07_sequence), PeepSpriteImage_Noodles_07_sequence },
	{ 10085, countof(PeepSpriteImage_Noodles_08_sequence), PeepSpriteImage_Noodles_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Noodles_09_sequence), PeepSpriteImage_Noodles_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Noodles_10_sequence), PeepSpriteImage_Noodles_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Noodles_11_sequence), PeepSpriteImage_Noodles_11_sequence },
	{ 11301, countof(PeepSpriteImage_Noodles_12_sequence), PeepSpriteImage_Noodles_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Noodles_13_sequence), PeepSpriteImage_Noodles_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Noodles_14_sequence), PeepSpriteImage_Noodles_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Noodles_15_sequence), PeepSpriteImage_Noodles_15_sequence },
	{ 11325, countof(PeepSpriteImage_Noodles_16_sequence), PeepSpriteImage_Noodles_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Noodles_17_sequence), PeepSpriteImage_Noodles_17_sequence },
	{ 11513, countof(PeepSpriteImage_Noodles_18_sequence), PeepSpriteImage_Noodles_18_sequence },
	{ 11513, countof(PeepSpriteImage_Noodles_19_sequence), PeepSpriteImage_Noodles_19_sequence },
	{ 11597, countof(PeepSpriteImage_Noodles_20_sequence), PeepSpriteImage_Noodles_20_sequence },
	{ 11669, countof(PeepSpriteImage_Noodles_21_sequence), PeepSpriteImage_Noodles_21_sequence },
	{ 11669, countof(PeepSpriteImage_Noodles_22_sequence), PeepSpriteImage_Noodles_22_sequence },
	{ 11801, countof(PeepSpriteImage_Noodles_23_sequence), PeepSpriteImage_Noodles_23_sequence },
	{ 11669, countof(PeepSpriteImage_Noodles_24_sequence), PeepSpriteImage_Noodles_24_sequence },
	{ 11377, countof(PeepSpriteImage_Noodles_25_sequence), PeepSpriteImage_Noodles_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Noodles_26_sequence), PeepSpriteImage_Noodles_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Noodles_27_sequence), PeepSpriteImage_Noodles_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Noodles_28_sequence), PeepSpriteImage_Noodles_28_sequence },
	{ 11401, countof(PeepSpriteImage_Noodles_29_sequence), PeepSpriteImage_Noodles_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Noodles_30_sequence), PeepSpriteImage_Noodles_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Noodles_31_sequence), PeepSpriteImage_Noodles_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Noodles_32_sequence), PeepSpriteImage_Noodles_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Noodles_33_sequence), PeepSpriteImage_Noodles_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Noodles_34_sequence), PeepSpriteImage_Noodles_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Noodles_35_sequence), PeepSpriteImage_Noodles_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Noodles_36_sequence), PeepSpriteImage_Noodles_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Sausage[] = {
	{ 10181, countof(PeepSpriteImage_Sausage_00_sequence), PeepSpriteImage_Sausage_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Sausage_01_sequence), PeepSpriteImage_Sausage_01_sequence },
	{ 10301, countof(PeepSpriteImage_Sausage_02_sequence), PeepSpriteImage_Sausage_02_sequence },
	{ 10301, countof(PeepSpriteImage_Sausage_03_sequence), PeepSpriteImage_Sausage_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Sausage_04_sequence), PeepSpriteImage_Sausage_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Sausage_05_sequence), PeepSpriteImage_Sausage_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Sausage_06_sequence), PeepSpriteImage_Sausage_06_sequence },
	{ 10253, countof(PeepSpriteImage_Sausage_07_sequence), PeepSpriteImage_Sausage_07_sequence },
	{ 10253, countof(PeepSpriteImage_Sausage_08_sequence), PeepSpriteImage_Sausage_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Sausage_09_sequence), PeepSpriteImage_Sausage_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Sausage_10_sequence), PeepSpriteImage_Sausage_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Sausage_11_sequence), PeepSpriteImage_Sausage_11_sequence },
	{ 11301, countof(PeepSpriteImage_Sausage_12_sequence), PeepSpriteImage_Sausage_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Sausage_13_sequence), PeepSpriteImage_Sausage_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Sausage_14_sequence), PeepSpriteImage_Sausage_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Sausage_15_sequence), PeepSpriteImage_Sausage_15_sequence },
	{ 11325, countof(PeepSpriteImage_Sausage_16_sequence), PeepSpriteImage_Sausage_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Sausage_17_sequence), PeepSpriteImage_Sausage_17_sequence },
	{ 11513, countof(PeepSpriteImage_Sausage_18_sequence), PeepSpriteImage_Sausage_18_sequence },
	{ 11513, countof(PeepSpriteImage_Sausage_19_sequence), PeepSpriteImage_Sausage_19_sequence },
	{ 11597, countof(PeepSpriteImage_Sausage_20_sequence), PeepSpriteImage_Sausage_20_sequence },
	{ 11669, countof(PeepSpriteImage_Sausage_21_sequence), PeepSpriteImage_Sausage_21_sequence },
	{ 11669, countof(PeepSpriteImage_Sausage_22_sequence), PeepSpriteImage_Sausage_22_sequence },
	{ 11801, countof(PeepSpriteImage_Sausage_23_sequence), PeepSpriteImage_Sausage_23_sequence },
	{ 11669, countof(PeepSpriteImage_Sausage_24_sequence), PeepSpriteImage_Sausage_24_sequence },
	{ 11377, countof(PeepSpriteImage_Sausage_25_sequence), PeepSpriteImage_Sausage_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Sausage_26_sequence), PeepSpriteImage_Sausage_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Sausage_27_sequence), PeepSpriteImage_Sausage_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Sausage_28_sequence), PeepSpriteImage_Sausage_28_sequence },
	{ 11401, countof(PeepSpriteImage_Sausage_29_sequence), PeepSpriteImage_Sausage_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Sausage_30_sequence), PeepSpriteImage_Sausage_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Sausage_31_sequence), PeepSpriteImage_Sausage_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Sausage_32_sequence), PeepSpriteImage_Sausage_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Sausage_33_sequence), PeepSpriteImage_Sausage_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Sausage_34_sequence), PeepSpriteImage_Sausage_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Sausage_35_sequence), PeepSpriteImage_Sausage_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Sausage_36_sequence), PeepSpriteImage_Sausage_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Soup[] = {
	{ 10349, countof(PeepSpriteImage_Soup_00_sequence), PeepSpriteImage_Soup_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Soup_01_sequence), PeepSpriteImage_Soup_01_sequence },
	{ 10469, countof(PeepSpriteImage_Soup_02_sequence), PeepSpriteImage_Soup_02_sequence },
	{ 10469, countof(PeepSpriteImage_Soup_03_sequence), PeepSpriteImage_Soup_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Soup_04_sequence), PeepSpriteImage_Soup_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Soup_05_sequence), PeepSpriteImage_Soup_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Soup_06_sequence), PeepSpriteImage_Soup_06_sequence },
	{ 10421, countof(PeepSpriteImage_Soup_07_sequence), PeepSpriteImage_Soup_07_sequence },
	{ 10421, countof(PeepSpriteImage_Soup_08_sequence), PeepSpriteImage_Soup_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Soup_09_sequence), PeepSpriteImage_Soup_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Soup_10_sequence), PeepSpriteImage_Soup_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Soup_11_sequence), PeepSpriteImage_Soup_11_sequence },
	{ 11301, countof(PeepSpriteImage_Soup_12_sequence), PeepSpriteImage_Soup_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Soup_13_sequence), PeepSpriteImage_Soup_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Soup_14_sequence), PeepSpriteImage_Soup_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Soup_15_sequence), PeepSpriteImage_Soup_15_sequence },
	{ 11325, countof(PeepSpriteImage_Soup_16_sequence), PeepSpriteImage_Soup_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Soup_17_sequence), PeepSpriteImage_Soup_17_sequence },
	{ 11513, countof(PeepSpriteImage_Soup_18_sequence), PeepSpriteImage_Soup_18_sequence },
	{ 11513, countof(PeepSpriteImage_Soup_19_sequence), PeepSpriteImage_Soup_19_sequence },
	{ 11597, countof(PeepSpriteImage_Soup_20_sequence), PeepSpriteImage_Soup_20_sequence },
	{ 11669, countof(PeepSpriteImage_Soup_21_sequence), PeepSpriteImage_Soup_21_sequence },
	{ 11669, countof(PeepSpriteImage_Soup_22_sequence), PeepSpriteImage_Soup_22_sequence },
	{ 11801, countof(PeepSpriteImage_Soup_23_sequence), PeepSpriteImage_Soup_23_sequence },
	{ 11669, countof(PeepSpriteImage_Soup_24_sequence), PeepSpriteImage_Soup_24_sequence },
	{ 11377, countof(PeepSpriteImage_Soup_25_sequence), PeepSpriteImage_Soup_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Soup_26_sequence), PeepSpriteImage_Soup_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Soup_27_sequence), PeepSpriteImage_Soup_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Soup_28_sequence), PeepSpriteImage_Soup_28_sequence },
	{ 11401, countof(PeepSpriteImage_Soup_29_sequence), PeepSpriteImage_Soup_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Soup_30_sequence), PeepSpriteImage_Soup_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Soup_31_sequence), PeepSpriteImage_Soup_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Soup_32_sequence), PeepSpriteImage_Soup_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Soup_33_sequence), PeepSpriteImage_Soup_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Soup_34_sequence), PeepSpriteImage_Soup_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Soup_35_sequence), PeepSpriteImage_Soup_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Soup_36_sequence), PeepSpriteImage_Soup_36_sequence },
};

static const rct_peep_animation PeepSpriteImage_Sandwich[] = {
	{ 10517, countof(PeepSpriteImage_Sandwich_00_sequence), PeepSpriteImage_Sandwich_00_sequence },
	{ 6585,  countof(PeepSpriteImage_Sandwich_01_sequence), PeepSpriteImage_Sandwich_01_sequence },
	{ 10637, countof(PeepSpriteImage_Sandwich_02_sequence), PeepSpriteImage_Sandwich_02_sequence },
	{ 10637, countof(PeepSpriteImage_Sandwich_03_sequence), PeepSpriteImage_Sandwich_03_sequence },
	{ 6697,  countof(PeepSpriteImage_Sandwich_04_sequence), PeepSpriteImage_Sandwich_04_sequence },
	{ 6757,  countof(PeepSpriteImage_Sandwich_05_sequence), PeepSpriteImage_Sandwich_05_sequence },
	{ 6561,  countof(PeepSpriteImage_Sandwich_06_sequence), PeepSpriteImage_Sandwich_06_sequence },
	{ 10589, countof(PeepSpriteImage_Sandwich_07_sequence), PeepSpriteImage_Sandwich_07_sequence },
	{ 10589, countof(PeepSpriteImage_Sandwich_08_sequence), PeepSpriteImage_Sandwich_08_sequence },
	{ 6637,  countof(PeepSpriteImage_Sandwich_09_sequence), PeepSpriteImage_Sandwich_09_sequence },
	{ 6645,  countof(PeepSpriteImage_Sandwich_10_sequence), PeepSpriteImage_Sandwich_10_sequence },
	{ 6813,  countof(PeepSpriteImage_Sandwich_11_sequence), PeepSpriteImage_Sandwich_11_sequence },
	{ 11301, countof(PeepSpriteImage_Sandwich_12_sequence), PeepSpriteImage_Sandwich_12_sequence },
	{ 6825,  countof(PeepSpriteImage_Sandwich_13_sequence), PeepSpriteImage_Sandwich_13_sequence },
	{ 6845,  countof(PeepSpriteImage_Sandwich_14_sequence), PeepSpriteImage_Sandwich_14_sequence },
	{ 6909,  countof(PeepSpriteImage_Sandwich_15_sequence), PeepSpriteImage_Sandwich_15_sequence },
	{ 11325, countof(PeepSpriteImage_Sandwich_16_sequence), PeepSpriteImage_Sandwich_16_sequence },
	{ 6957,  countof(PeepSpriteImage_Sandwich_17_sequence), PeepSpriteImage_Sandwich_17_sequence },
	{ 11513, countof(PeepSpriteImage_Sandwich_18_sequence), PeepSpriteImage_Sandwich_18_sequence },
	{ 11513, countof(PeepSpriteImage_Sandwich_19_sequence), PeepSpriteImage_Sandwich_19_sequence },
	{ 11597, countof(PeepSpriteImage_Sandwich_20_sequence), PeepSpriteImage_Sandwich_20_sequence },
	{ 11669, countof(PeepSpriteImage_Sandwich_21_sequence), PeepSpriteImage_Sandwich_21_sequence },
	{ 11669, countof(PeepSpriteImage_Sandwich_22_sequence), PeepSpriteImage_Sandwich_22_sequence },
	{ 11801, countof(PeepSpriteImage_Sandwich_23_sequence), PeepSpriteImage_Sandwich_23_sequence },
	{ 11669, countof(PeepSpriteImage_Sandwich_24_sequence), PeepSpriteImage_Sandwich_24_sequence },
	{ 11377, countof(PeepSpriteImage_Sandwich_25_sequence), PeepSpriteImage_Sandwich_25_sequence },
	{ 6989,  countof(PeepSpriteImage_Sandwich_26_sequence), PeepSpriteImage_Sandwich_26_sequence },
	{ 7041,  countof(PeepSpriteImage_Sandwich_27_sequence), PeepSpriteImage_Sandwich_27_sequence },
	{ 7101,  countof(PeepSpriteImage_Sandwich_28_sequence), PeepSpriteImage_Sandwich_28_sequence },
	{ 11401, countof(PeepSpriteImage_Sandwich_29_sequence), PeepSpriteImage_Sandwich_29_sequence },
	{ 7121,  countof(PeepSpriteImage_Sandwich_30_sequence), PeepSpriteImage_Sandwich_30_sequence },
	{ 7149,  countof(PeepSpriteImage_Sandwich_31_sequence), PeepSpriteImage_Sandwich_31_sequence },
	{ 7249,  countof(PeepSpriteImage_Sandwich_32_sequence), PeepSpriteImage_Sandwich_32_sequence },
	{ 7309,  countof(PeepSpriteImage_Sandwich_33_sequence), PeepSpriteImage_Sandwich_33_sequence },
	{ 7329,  countof(PeepSpriteImage_Sandwich_34_sequence), PeepSpriteImage_Sandwich_34_sequence },
	{ 7285,  countof(PeepSpriteImage_Sandwich_35_sequence), PeepSpriteImage_Sandwich_35_sequence },
	{ 7181,  countof(PeepSpriteImage_Sandwich_36_sequence), PeepSpriteImage_Sandwich_36_sequence },
};


static const rct_sprite_bounds PeepSpriteBounds_Normal[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Handyman[] = {
	{ 12, 16, 6 },
	{ 8, 16, 5 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 15, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Mechanic[] = {
	{ 10, 16, 5 },
	{ 8, 16, 5 },
	{ 10, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 10, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Security[] = {
	{ 8, 18, 5 },
	{ 8, 16, 5 },
	{ 8, 17, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 15, 19, 6 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_EntertainerPanda[] = {
	{ 13, 24, 8 },
	{ 8, 16, 5 },
	{ 10, 23, 7 },
	{ 14, 24, 7 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 19, 30, 8 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 13, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 14, 25, 8 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 14, 24, 7 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_EntertainerTiger[] = {
	{ 13, 24, 8 },
	{ 8, 16, 5 },
	{ 10, 23, 7 },
	{ 16, 24, 8 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 23, 30, 8 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 13, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 16, 28, 9 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 16, 24, 8 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_EntertainerElephant[] = {
	{ 13, 24, 8 },
	{ 8, 16, 5 },
	{ 10, 23, 7 },
	{ 17, 24, 8 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 23, 30, 8 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 17, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 18, 25, 9 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 17, 24, 8 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_EntertainerRoman[] = {
	{ 13, 24, 8 },
	{ 8, 16, 5 },
	{ 10, 23, 7 },
	{ 17, 24, 8 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 23, 30, 8 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 17, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 18, 25, 9 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 17, 24, 8 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_EntertainerGorilla[] = {
	{ 13, 24, 8 },
	{ 8, 16, 5 },
	{ 10, 23, 7 },
	{ 17, 24, 8 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 23, 30, 8 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 17, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 18, 25, 9 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 17, 24, 8 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_EntertainerSnowman[] = {
	{ 13, 24, 8 },
	{ 8, 16, 5 },
	{ 10, 23, 7 },
	{ 17, 28, 9 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 23, 30, 8 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 17, 15, 9 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 18, 28, 9 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 17, 28, 9 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_EntertainerKnight[] = {
	{ 16, 32, 8 },
	{ 8, 16, 5 },
	{ 10, 23, 7 },
	{ 23, 30, 15 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 23, 30, 8 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 21, 32, 9 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 23, 30, 15 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 23, 30, 15 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_EntertainerAstronaut[] = {
	{ 16, 32, 8 },
	{ 8, 16, 5 },
	{ 10, 23, 7 },
	{ 23, 30, 15 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 23, 30, 8 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 21, 32, 9 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 23, 30, 15 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 23, 30, 15 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_EntertainerBandit[] = {
	{ 16, 32, 8 },
	{ 8, 16, 5 },
	{ 10, 23, 7 },
	{ 23, 30, 15 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 23, 30, 8 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 21, 32, 9 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 23, 30, 15 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 23, 30, 15 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_EntertainerSheriff[] = {
	{ 16, 32, 8 },
	{ 8, 16, 5 },
	{ 10, 23, 7 },
	{ 23, 30, 15 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 23, 30, 8 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 21, 32, 9 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 23, 30, 15 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 23, 30, 15 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_EntertainerPirate[] = {
	{ 16, 32, 8 },
	{ 8, 16, 5 },
	{ 10, 23, 7 },
	{ 23, 30, 15 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 23, 30, 8 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 21, 32, 9 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 23, 30, 15 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 23, 30, 15 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_IceCream[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Chips[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Burger[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Drink[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Balloon[] = {
	{ 11, 28, 5 },
	{ 8, 16, 5 },
	{ 11, 28, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Candyfloss[] = {
	{ 11, 19, 5 },
	{ 8, 16, 5 },
	{ 11, 19, 5 },
	{ 11, 19, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 13, 16, 6 },
	{ 13, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Umbrella[] = {
	{ 14, 21, 5 },
	{ 8, 16, 5 },
	{ 14, 21, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 14, 19, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Pizza[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_SecurityAlt[] = {
	{ 8, 18, 5 },
	{ 8, 16, 5 },
	{ 8, 17, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 15, 19, 6 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Popcorn[] = {
	{ 11, 19, 5 },
	{ 8, 16, 5 },
	{ 11, 19, 5 },
	{ 11, 19, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 13, 16, 6 },
	{ 13, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Hat[] = {
	{ 12, 32, 5 },
	{ 8, 16, 5 },
	{ 12, 32, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 10, 20, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_HotDog[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Tentacle[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_ToffeeApple[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Donut[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Coffee[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Chicken[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Lemonade[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Pretzel[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_SuJongkwa[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Juice[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_FunnelCake[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Noodles[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Sausage[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Soup[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};

static const rct_sprite_bounds PeepSpriteBounds_Sandwich[] = {
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 16, 5 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 9, 16, 6 },
	{ 8, 16, 5 },
	{ 18, 16, 11 },
	{ 12, 22, 5 },
	{ 9, 16, 7 },
	{ 10, 22, 5 },
	{ 17, 16, 9 },
	{ 9, 15, 6 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 13, 22, 7 },
	{ 19, 16, 16 },
	{ 13, 22, 7 },
	{ 17, 16, 9 },
	{ 11, 24, 6 },
	{ 11, 16, 5 },
	{ 11, 16, 5 },
	{ 17, 16, 9 },
	{ 11, 16, 5 },
	{ 8, 16, 5 },
	{ 9, 17, 6 },
	{ 9, 16, 5 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
	{ 9, 22, 7 },
};


rct_peep_animation_entry g_peep_animation_entries[] =
{
	{ PeepSpriteImage_Normal, PeepSpriteBounds_Normal },
	{ PeepSpriteImage_Handyman, PeepSpriteBounds_Handyman },
	{ PeepSpriteImage_Mechanic, PeepSpriteBounds_Mechanic },
	{ PeepSpriteImage_Security, PeepSpriteBounds_Security },
	{ PeepSpriteImage_EntertainerPanda, PeepSpriteBounds_EntertainerPanda },
	{ PeepSpriteImage_EntertainerTiger, PeepSpriteBounds_EntertainerTiger },
	{ PeepSpriteImage_EntertainerElephant, PeepSpriteBounds_EntertainerElephant },
	{ PeepSpriteImage_EntertainerRoman, PeepSpriteBounds_EntertainerRoman },
	{ PeepSpriteImage_EntertainerGorilla, PeepSpriteBounds_EntertainerGorilla },
	{ PeepSpriteImage_EntertainerSnowman, PeepSpriteBounds_EntertainerSnowman },
	{ PeepSpriteImage_EntertainerKnight, PeepSpriteBounds_EntertainerKnight },
	{ PeepSpriteImage_EntertainerAstronaut, PeepSpriteBounds_EntertainerAstronaut },
	{ PeepSpriteImage_EntertainerBandit, PeepSpriteBounds_EntertainerBandit },
	{ PeepSpriteImage_EntertainerSheriff, PeepSpriteBounds_EntertainerSheriff },
	{ PeepSpriteImage_EntertainerPirate, PeepSpriteBounds_EntertainerPirate },
	{ PeepSpriteImage_IceCream, PeepSpriteBounds_IceCream },
	{ PeepSpriteImage_Chips, PeepSpriteBounds_Chips },
	{ PeepSpriteImage_Burger, PeepSpriteBounds_Burger },
	{ PeepSpriteImage_Drink, PeepSpriteBounds_Drink },
	{ PeepSpriteImage_Balloon, PeepSpriteBounds_Balloon },
	{ PeepSpriteImage_Candyfloss, PeepSpriteBounds_Candyfloss },
	{ PeepSpriteImage_Umbrella, PeepSpriteBounds_Umbrella },
	{ PeepSpriteImage_Pizza, PeepSpriteBounds_Pizza },
	{ PeepSpriteImage_SecurityAlt, PeepSpriteBounds_SecurityAlt },
	{ PeepSpriteImage_Popcorn, PeepSpriteBounds_Popcorn },
	{ PeepSpriteImage_ArmsCrossed, PeepSpriteBounds_Normal },
	{ PeepSpriteImage_HeadDown, PeepSpriteBounds_Normal },
	{ PeepSpriteImage_Nauseous, PeepSpriteBounds_Normal },
	{ PeepSpriteImage_VeryNauseous, PeepSpriteBounds_Normal },
	{ PeepSpriteImage_RequireBathroom, PeepSpriteBounds_Normal },
	{ PeepSpriteImage_Hat, PeepSpriteBounds_Hat },
	{ PeepSpriteImage_HotDog, PeepSpriteBounds_HotDog },
	{ PeepSpriteImage_Tentacle, PeepSpriteBounds_Tentacle },
	{ PeepSpriteImage_ToffeeApple, PeepSpriteBounds_ToffeeApple },
	{ PeepSpriteImage_Donut, PeepSpriteBounds_Donut },
	{ PeepSpriteImage_Coffee, PeepSpriteBounds_Coffee },
	{ PeepSpriteImage_Chicken, PeepSpriteBounds_Chicken },
	{ PeepSpriteImage_Lemonade, PeepSpriteBounds_Lemonade },
	{ PeepSpriteImage_Watching, PeepSpriteBounds_Normal },
	{ PeepSpriteImage_Pretzel, PeepSpriteBounds_Pretzel },
	{ PeepSpriteImage_Sunglasses, PeepSpriteBounds_Normal },
	{ PeepSpriteImage_SuJongkwa, PeepSpriteBounds_SuJongkwa },
	{ PeepSpriteImage_Juice, PeepSpriteBounds_Juice },
	{ PeepSpriteImage_FunnelCake, PeepSpriteBounds_FunnelCake },
	{ PeepSpriteImage_Noodles, PeepSpriteBounds_Noodles },
	{ PeepSpriteImage_Sausage, PeepSpriteBounds_Sausage },
	{ PeepSpriteImage_Soup, PeepSpriteBounds_Soup },
	{ PeepSpriteImage_Sandwich, PeepSpriteBounds_Sandwich },
};
