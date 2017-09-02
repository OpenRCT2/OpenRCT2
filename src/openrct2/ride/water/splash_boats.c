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

#include "../../config/Config.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../../world/sprite.h"
#include "../track.h"
#include "../track_paint.h"
#include "../vehicle_paint.h"

enum {
    SPR_SPLASH_BOATS_FLAT_TOP_SW_NE                 = 20820,
    SPR_SPLASH_BOATS_FLAT_TOP_NW_SE                 = 20821,
    SPR_SPLASH_BOATS_FLAT_TOP_NE_SW                 = 20822,
    SPR_SPLASH_BOATS_FLAT_TOP_SE_NW                 = 20823,

    SPR_SPLASH_BOATS_FLAT_SIDE_SW_NE                = 20824,
    SPR_SPLASH_BOATS_FLAT_SIDE_NW_SE                = 20825,
    SPR_SPLASH_BOATS_FLAT_SIDE_NE_SW                = 20826,
    SPR_SPLASH_BOATS_FLAT_SIDE_SE_NW                = 20827,

    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SW_NE = 20828,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NW_SE = 20829,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NE_SW = 20830,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SE_NW = 20831,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 20832,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 20833,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 20834,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 20835,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SW_NE = 20836,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NW_SE = 20837,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NE_SW = 20838,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SE_NW = 20839,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SW_NE = 20840,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NW_SE = 20841,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NE_SW = 20842,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SE_NW = 20843,
    SPR_SPLASH_BOATS_25_DEG_UP_SW_NE = 20844,
    SPR_SPLASH_BOATS_25_DEG_UP_NW_SE = 20845,
    SPR_SPLASH_BOATS_25_DEG_UP_NE_SW = 20846,
    SPR_SPLASH_BOATS_25_DEG_UP_SE_NW = 20847,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SW_NE = 20848,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NW_SE = 20849,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NE_SW = 20850,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SE_NW = 20851,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NE_SW = 20852,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SE_NW = 20853,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SW_NE = 20854,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NW_SE = 20855,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NE_SW = 20856,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SE_NW = 20857,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SW_NE = 20858,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NW_SE = 20859,
    SPR_SPLASH_BOATS_25_DEG_DOWN_NE_SW = 20860,
    SPR_SPLASH_BOATS_25_DEG_DOWN_SE_NW = 20861,
    SPR_SPLASH_BOATS_25_DEG_DOWN_SW_NE = 20862,
    SPR_SPLASH_BOATS_25_DEG_DOWN_NW_SE = 20863,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SW_NE = 20864,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NW_SE = 20865,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NE_SW = 20866,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SE_NW = 20867,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SW_NE = 20868,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NW_SE = 20869,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NE_SW = 20870,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SE_NW = 20871,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SW_NE = 20872,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NW_SE = 20873,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NE_SW = 20874,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SE_NW = 20875,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SW_NE = 20876,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NW_SE = 20877,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NE_SW = 20878,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SE_NW = 20879,
    SPR_SPLASH_BOATS_60_DEG_UP_SW_NE = 20880,
    SPR_SPLASH_BOATS_60_DEG_UP_NW_SE = 20881,
    SPR_SPLASH_BOATS_60_DEG_UP_NE_SW = 20882,
    SPR_SPLASH_BOATS_60_DEG_UP_SE_NW = 20883,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SW_NE = 20884,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NW_SE = 20885,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NE_SW = 20886,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SE_NW = 20887,

    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_0   = 20888,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_2   = 20889,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_3   = 20890,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_5   = 20891,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_6   = 20892,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_0   = 20893,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_2   = 20894,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_3   = 20895,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_5   = 20896,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_6   = 20897,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_0   = 20898,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_2   = 20899,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_3   = 20900,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_5   = 20901,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_6   = 20902,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_0   = 20903,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_2   = 20904,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_3   = 20905,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_5   = 20906,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_6   = 20907,

    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_0  = 20908,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_2  = 20909,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_3  = 20910,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_5  = 20911,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_6  = 20912,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_0  = 20913,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_2  = 20914,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_3  = 20915,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_5  = 20916,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_6  = 20917,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_0  = 20918,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_2  = 20919,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_3  = 20920,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_5  = 20921,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_6  = 20922,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_0  = 20923,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_2  = 20924,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_3  = 20925,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_5  = 20926,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_6  = 20927,

    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_6   = 20908,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_5   = 20909,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_3   = 20910,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_2   = 20911,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_0   = 20912,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_6   = 20913,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_5   = 20914,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_3   = 20915,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_2   = 20916,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_0   = 20917,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_6   = 20918,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_5   = 20919,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_3   = 20920,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_2   = 20921,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_0   = 20922,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_6   = 20923,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_5   = 20924,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_3   = 20925,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_2   = 20926,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_0   = 20927,

    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_6    = 20928,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_5    = 20929,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_3    = 20930,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_2    = 20931,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_0    = 20932,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_6    = 20933,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_5    = 20934,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_3    = 20935,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_2    = 20936,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_0    = 20937,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_6    = 20938,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_5    = 20939,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_3    = 20940,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_2    = 20941,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_0    = 20942,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_6    = 20943,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_5    = 20944,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_3    = 20945,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_2    = 20946,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_0    = 20947,

    SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_0      = 20948,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_1      = 20949,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_2      = 20950,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_3      = 20951,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_0      = 20952,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_1      = 20953,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_2      = 20954,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_3      = 20955,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_0      = 20956,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_1      = 20957,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_2      = 20958,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_3      = 20959,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_0      = 20960,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_1      = 20961,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_2      = 20962,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_3      = 20963,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_0     = 20964,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_1     = 20965,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_2     = 20966,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_3     = 20967,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_0     = 20968,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_1     = 20969,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_2     = 20970,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_3     = 20971,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_0     = 20972,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_1     = 20973,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_2     = 20974,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_3     = 20975,
    SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_0     = 20976,
    SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_1     = 20977,
    SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_2     = 20978,
    SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_3     = 20979,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_3      = 20980,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_2      = 20981,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_1      = 20982,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_0      = 20983,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_3      = 20984,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_2      = 20985,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_1      = 20986,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_0      = 20987,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_3      = 20988,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_2      = 20989,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_1      = 20990,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_0      = 20991,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_3      = 20992,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_2      = 20993,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_1      = 20994,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_0      = 20995,
};

uint32 SplashBoats25DegUpImageId[4] =
{
    SPR_SPLASH_BOATS_25_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_SE_NW,
};

uint32 SplashBoats25DegUpFrontImageId[4] =
{
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SE_NW,
};

uint32 SplashBoats60DegUpImageId[4] =
{
    SPR_SPLASH_BOATS_60_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_SE_NW,
};

uint32 SplashBoats60DegUpFrontImageId[4] =
{
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SE_NW,
};

uint32 SplashBoatsFlatTo25DegUpImageId[4] =
{
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SE_NW,
};

uint32 SplashBoatsFlatTo25DegUpFrontImageId[4] =
{
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SE_NW,
};

uint32 SplashBoats25DegUpToFlatImageId[4] =
{
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SE_NW,
};

uint32 SplashBoats25DegUpToFlatFrontImageId[4] =
{
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SE_NW,
};

uint32 SplashBoats25DegUpTo60DegUpImageId[4] =
{
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SE_NW,
};

uint32 SplashBoats25DegUpTo60DegUpFrontImageId[4] =
{
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SE_NW,
};

uint32 SplashBoats60DegUpTo25DegUpImageId[4] =
{
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SE_NW,
};

uint32 SplashBoats60DegUpTo25DegUpFrontImageId[4] =
{
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SE_NW,
};

uint32 SplashBoats25DegDownImageId[4] =
{
    SPR_SPLASH_BOATS_25_DEG_DOWN_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_DOWN_SE_NW,
};

uint32 SplashBoats25DegDownFrontImageId[4] =
{
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SE_NW,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NW_SE,
};

uint32 SplashBoatsFlatTo25DegDownImageId[4] =
{
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NW_SE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SE_NW,
};

uint32 SplashBoatsFlatTo25DegDownFrontImageId[4] =
{
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SE_NW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NW_SE,
};

uint32 SplashBoats25DegDownToFlatImageId[4] =
{
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SE_NW,
};

uint32 SplashBoats25DegDownToFlatFrontImageId[4] =
{
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SE_NW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NW_SE,
};

static const sprite_bb RiverRaftsLeftQuarterTurn5_Top[4][5] = {
        {
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_0, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 27, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_2, { 0, 0, 0 }, {  0,  0, 0 }, { 32, 16, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_3, { 0, 0, 0 }, {  0, 16, 0 }, { 16, 16, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_5, { 0, 0, 0 }, { 16,  0, 0 }, { 16, 34, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_6, { 0, 0, 0 }, {  2,  0, 0 }, { 32, 32, 2 },
        },
        {
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_0, { 0, 0, 0 }, {  2,  0, 0 }, { 27, 32, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_2, { 0, 0, 0 }, {  0,  0, 0 }, { 16, 32, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_3, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_5, { 0, 0, 0 }, {  0,  0, 0 }, { 32, 16, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_6, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 27, 2 },
        },
        {
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_0, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 32, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_2, { 0, 0, 0 }, {  0, 16, 0 }, { 32, 16, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_3, { 0, 0, 0 }, { 16,  0, 0 }, { 16, 16, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_5, { 0, 0, 0 }, {  0,  0, 0 }, { 16, 32, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_6, { 0, 0, 0 }, {  2,  0, 0 }, { 27, 32, 2 },
        },
        {
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_0, { 0, 0, 0 }, {  2,  0, 0 }, { 32, 32, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_2, { 0, 0, 0 }, { 16,  0, 0 }, { 16, 32, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_3, { 0, 0, 0 }, {  0,  0, 0 }, { 16, 16, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_5, { 0, 0, 0 }, {  0, 16, 0 }, { 32, 16, 2 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_6, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 32, 2 },
        }
};

static const sprite_bb RiverRaftsLeftQuarterTurn5_Side[4][5] = {
        {
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_0, { 0, 0, 0 }, {  0,  2, 27 }, { 32, 27, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_2, { 0, 0, 0 }, {  0,  0, 27 }, { 32, 16, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_3, { 0, 0, 0 }, {  0, 16, 27 }, { 16, 16, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_5, { 0, 0, 0 }, { 16,  0, 27 }, { 16, 34, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_6, { 0, 0, 0 }, {  2,  0, 27 }, { 32, 32, 0 },
        },
        {
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_0, { 0, 0, 0 }, {  2,  0, 27 }, { 27, 32, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_2, { 0, 0, 0 }, {  0,  0, 27 }, { 16, 32, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_3, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_5, { 0, 0, 0 }, {  0,  0, 27 }, { 32, 16, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_6, { 0, 0, 0 }, {  0,  2, 27 }, { 32, 27, 0 },
        },
        {
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_0, { 0, 0, 0 }, {  0,  2, 27 }, { 32, 32, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_2, { 0, 0, 0 }, {  0, 16, 27 }, { 32, 16, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_3, { 0, 0, 0 }, { 16,  0, 27 }, { 16, 16, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_5, { 0, 0, 0 }, {  0,  0, 27 }, { 16, 32, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_6, { 0, 0, 0 }, {  2,  0, 27 }, { 27, 32, 0 },
        },
        {
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_0, { 0, 0, 0 }, {  2,  0, 27 }, { 32, 32, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_2, { 0, 0, 0 }, { 16,  0, 27 }, { 16, 32, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_3, { 0, 0, 0 }, {  0,  0, 27 }, { 16, 16, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_5, { 0, 0, 0 }, {  0, 16, 27 }, { 32, 16, 0 },
                SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_6, { 0, 0, 0 }, {  0,  2, 27 }, { 32, 32, 0 },
        }
};

static const sprite_bb RiverRaftsRightQuarterTurn5_Top[4][5] = {
        {
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_0, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 32, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_2, { 0, 0, 0 }, {  0, 16, 0 }, { 32, 16, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_3, { 0, 0, 0 }, {  0,  0, 0 }, { 16, 16, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_5, { 0, 0, 0 }, { 16,  0, 0 }, { 16, 32, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_6, { 0, 0, 0 }, {  2,  0, 0 }, { 32, 32, 2 },
        },
        {
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_0, { 0, 0, 0 }, {  2,  0, 0 }, { 32, 32, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_2, { 0, 0, 0 }, { 16,  0, 0 }, { 16, 34, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_3, { 0, 0, 0 }, {  0, 16, 0 }, { 16, 16, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_5, { 0, 0, 0 }, {  0,  0, 0 }, { 32, 16, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_6, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 27, 2 },
        },
        {
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_0, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 27, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_2, { 0, 0, 0 }, {  0,  0, 0 }, { 32, 16, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_3, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_5, { 0, 0, 0 }, {  0,  0, 0 }, { 16, 32, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_6, { 0, 0, 0 }, {  2,  0, 0 }, { 27, 32, 2 },
        },
        {
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_0, { 0, 0, 0 }, {  2,  0, 0 }, { 27, 32, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_2, { 0, 0, 0 }, {  0,  0, 0 }, { 16, 32, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_3, { 0, 0, 0 }, { 16,  0, 0 }, { 16, 16, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_5, { 0, 0, 0 }, {  0, 16, 0 }, { 32, 16, 2 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_6, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 32, 2 },
        }
};

static const sprite_bb RiverRaftsRightQuarterTurn5_Side[4][5] = {
        {
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_0, { 0, 0, 0 }, {  0,  2, 27 }, { 32, 32, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_2, { 0, 0, 0 }, {  0, 16, 27 }, { 32, 16, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_3, { 0, 0, 0 }, {  0,  0, 27 }, { 16, 16, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_5, { 0, 0, 0 }, { 16,  0, 27 }, { 16, 32, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_6, { 0, 0, 0 }, {  2,  0, 27 }, { 32, 32, 0 },
        },
        {
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_0, { 0, 0, 0 }, {  2,  0, 27 }, { 32, 32, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_2, { 0, 0, 0 }, { 16,  0, 27 }, { 16, 34, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_3, { 0, 0, 0 }, {  0, 16, 27 }, { 16, 16, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_5, { 0, 0, 0 }, {  0,  0, 27 }, { 32, 16, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_6, { 0, 0, 0 }, {  0,  2, 27 }, { 32, 27, 0 },
        },
        {
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_0, { 0, 0, 0 }, {  0,  2, 27 }, { 32, 27, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_2, { 0, 0, 0 }, {  0,  0, 27 }, { 32, 16, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_3, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_5, { 0, 0, 0 }, {  0,  0, 27 }, { 16, 32, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_6, { 0, 0, 0 }, {  2,  0, 27 }, { 27, 32, 0 },
        },
        {
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_0, { 0, 0, 0 }, {  2,  0, 27 }, { 27, 32, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_2, { 0, 0, 0 }, {  0,  0, 27 }, { 16, 32, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_3, { 0, 0, 0 }, { 16,  0, 27 }, { 16, 16, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_5, { 0, 0, 0 }, {  0, 16, 27 }, { 32, 16, 0 },
                SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_6, { 0, 0, 0 }, {  0,  2, 27 }, { 32, 32, 0 },
        }
};


static void paint_splash_boats_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    uint32 imageId = SplashBoats25DegUpImageId[direction] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = SplashBoats25DegUpFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 50, height, 0, 27, height);

    wooden_a_supports_paint_setup((direction & 1), 9 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

static void paint_splash_boats_track_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    uint32 imageId = SplashBoats60DegUpImageId[direction] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = SplashBoats60DegUpFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

    gWoodenSupportsPrependTo = sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 98, height, 0, 27, height);

    wooden_a_supports_paint_setup((direction & 1), 21 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 56, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 104, 0x20);
}

static void paint_splash_boats_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    uint32 imageId = SplashBoatsFlatTo25DegUpImageId[direction] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = SplashBoatsFlatTo25DegUpFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 42, height, 0, 27, height);

    wooden_a_supports_paint_setup((direction & 1), 1 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

static void paint_splash_boats_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    uint32 imageId = SplashBoats25DegUpToFlatImageId[direction] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = SplashBoats25DegUpToFlatFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 34, height, 0, 27, height);

    wooden_a_supports_paint_setup((direction & 1), 5 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

static void paint_splash_boats_track_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    uint32 imageId = SplashBoats25DegUpTo60DegUpImageId[direction] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = SplashBoats25DegUpTo60DegUpFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

    gWoodenSupportsPrependTo = sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 66, height, 0, 27, height);

    wooden_a_supports_paint_setup((direction & 1), 13 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 72, 0x20);
}

static void paint_splash_boats_track_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    uint32 imageId = SplashBoats60DegUpTo25DegUpImageId[direction] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = SplashBoats60DegUpTo25DegUpFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

    gWoodenSupportsPrependTo = sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 66, height, 0, 27, height);

    wooden_a_supports_paint_setup((direction & 1), 17 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 72, 0x20);
}

static void paint_splash_boats_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    uint32 imageId = SplashBoats25DegDownImageId[direction] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = SplashBoats25DegDownFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 50, height, 0, 27, height);

    static const uint8 specialSupport[] = { 11, 12, 9, 10 };
    wooden_a_supports_paint_setup((direction & 1), specialSupport[direction], height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
    } else {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

static void paint_splash_boats_track_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    paint_splash_boats_track_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void paint_splash_boats_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    uint32 imageId = SplashBoatsFlatTo25DegDownImageId[direction] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = SplashBoatsFlatTo25DegDownFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 34, height, 0, 27, height);

    static const uint8 specialSupport[] = { 7, 8, 5, 6 };
    wooden_a_supports_paint_setup((direction & 1), specialSupport[direction], height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
    } else {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

static void paint_splash_boats_track_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    paint_splash_boats_track_60_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void paint_splash_boats_track_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    paint_splash_boats_track_25_deg_up_to_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void paint_splash_boats_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    uint32 imageId = SplashBoats25DegDownToFlatImageId[direction] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = SplashBoats25DegDownToFlatFrontImageId[direction] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 42, height, 0, 27, height);

    static const uint8 specialSupport[] = { 3, 4, 1, 2 };
    wooden_a_supports_paint_setup((direction & 1), specialSupport[direction], height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** Start of elements originally from River Rafts */
/** rct2: 0x0089B170 */
static void paint_splash_boats_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    if (direction & 1) {
        imageId = (direction == 1 ? SPR_SPLASH_BOATS_FLAT_TOP_NW_SE : SPR_SPLASH_BOATS_FLAT_TOP_SE_NW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 20, 32, 2, height, 6, 0, height, get_current_rotation());

        imageId = (direction == 1 ? SPR_SPLASH_BOATS_FLAT_SIDE_NW_SE : SPR_SPLASH_BOATS_FLAT_SIDE_SE_NW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 1, 32, 26, height, 27, 0, height, get_current_rotation());
    } else {
        imageId = (direction == 0 ? SPR_SPLASH_BOATS_FLAT_TOP_SW_NE : SPR_SPLASH_BOATS_FLAT_TOP_NE_SW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 32, 20, 2, height, 0, 6, height, get_current_rotation());

        imageId = (direction == 0 ? SPR_SPLASH_BOATS_FLAT_SIDE_SW_NE : SPR_SPLASH_BOATS_FLAT_SIDE_NE_SW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 32, 1, 26, height, 0, 27, height, get_current_rotation());
    }

    wooden_a_supports_paint_setup((direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction & 1) {
        paint_util_push_tunnel_right(height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_left(height, TUNNEL_6);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0089B1A0 */
static void paint_splash_boats_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_ride *ride = get_ride(rideIndex);

    if (direction & 1) {
        uint32 imageId = (direction == 1 ? SPR_SPLASH_BOATS_FLAT_TOP_NW_SE : SPR_SPLASH_BOATS_FLAT_TOP_SE_NW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 20, 32, 1, height, 6, 0, height + 3, get_current_rotation());

        imageId = SPR_STATION_BASE_B_NW_SE | gTrackColours[SCHEME_MISC];
        sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());
    } else {
        uint32 imageId = (direction == 0 ? SPR_SPLASH_BOATS_FLAT_TOP_SW_NE : SPR_SPLASH_BOATS_FLAT_TOP_NE_SW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 32, 20, 1, height, 0, 6, height + 3, get_current_rotation());

        imageId = SPR_STATION_BASE_B_SW_NE | gTrackColours[SCHEME_MISC];
        sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());
    }

    wooden_a_supports_paint_setup((direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    track_paint_util_draw_station_platform(ride, direction, height, 7, mapElement);

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0089B1D0 */
static void paint_splash_boats_track_left_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    track_paint_util_right_quarter_turn_5_tiles_paint_2(height, direction, get_current_rotation(), trackSequence, gTrackColours[SCHEME_TRACK], RiverRaftsLeftQuarterTurn5_Top);
    track_paint_util_right_quarter_turn_5_tiles_paint_2(height, direction, get_current_rotation(), trackSequence, gTrackColours[SCHEME_TRACK], RiverRaftsLeftQuarterTurn5_Side);

    if (trackSequence != 1 && trackSequence != 4) {
        static const uint8 supportTypes[][7] = {
                { 0, 0xFF, 5, 3, 0xFF, 5, 1 },
                { 1, 0xFF, 2, 4, 0xFF, 2, 0 },
                { 0, 0xFF, 3, 5, 0xFF, 3, 1 },
                { 1, 0xFF, 4, 2, 0xFF, 4, 0 },
        };
        uint8 supportType = supportTypes[direction][trackSequence];
        wooden_a_supports_paint_setup(supportType, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    switch (trackSequence) {
        case 0: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
        case 1: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0); break;
        case 2: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4 | SEGMENT_B8 | SEGMENT_D0, direction), 0xFFFF, 0); break;
        case 3: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_CC | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_B4 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0); break;
        case 4: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0); break;
        case 5: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_B8, direction), 0xFFFF, 0); break;
        case 6: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
    }

    switch (trackSequence) {
        case 0:
            if (direction == 0 || direction == 3) {
                paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
            }
            break;
        case 6:
            if (direction == 2 || direction == 3) {
                paint_util_push_tunnel_rotated(direction ^ 1, height, TUNNEL_6);
            }
            break;
    }

    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0089B1D0 */
static void paint_splash_boats_track_right_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    track_paint_util_right_quarter_turn_5_tiles_paint_2(height, direction, get_current_rotation(), trackSequence, gTrackColours[SCHEME_TRACK], RiverRaftsRightQuarterTurn5_Top);
    track_paint_util_right_quarter_turn_5_tiles_paint_2(height, direction, get_current_rotation(), trackSequence, gTrackColours[SCHEME_TRACK], RiverRaftsRightQuarterTurn5_Side);

    if (trackSequence != 1 && trackSequence != 4) {
        static const uint8 supportTypes[][7] = {
                { 0, 0xFF, 4, 2, 0xFF, 4, 1 },
                { 1, 0xFF, 5, 3, 0xFF, 5, 0 },
                { 0, 0xFF, 2, 4, 0xFF, 2, 1 },
                { 1, 0xFF, 3, 5, 0xFF, 3, 0 },
        };
        uint8 supportType = supportTypes[direction][trackSequence];
        wooden_a_supports_paint_setup(supportType, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    switch (trackSequence) {
        case 0: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
        case 1: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0); break;
        case 2: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 0xFFFF, 0); break;
        case 3: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0); break;
        case 4: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0); break;
        case 5: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 0xFFFF, 0); break;
        case 6: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
    }

    switch (trackSequence) {
        case 0:
            if (direction == 0 || direction == 3) {
                paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
            }
            break;
        case 6:
            if (direction == 0 || direction == 1) {
                paint_util_push_tunnel_rotated(direction ^ 1, height, TUNNEL_6);
            }
            break;
    }

    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0089B180 */
static void paint_splash_boats_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4][2] =
            {
                    {   { SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_0 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_1 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_2 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_3 } },
                    {   { SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_0 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_1 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_2 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_3 } },
                    {   { SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_3 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_2 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_1 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_0 } },
                    {   { SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_3 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_2 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_1 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_0 } },
            };

    uint32 imageId = imageIds[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = imageIds[direction][trackSequence][1] | gTrackColours[SCHEME_TRACK];
    sint16 bboy;

    switch (trackSequence) {
        case 0:
            sub_98197C_rotated(direction, imageId, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
            break;
        case 1:
            bboy = (direction == 0 || direction == 1) ? 0 : 6;
            sub_98197C_rotated(direction, imageId, 0, 0, 32, 26, 2, height, 0, bboy, height);
            sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 26, 0, height, 0, bboy, height + 27);

            static const sint32 supportTypes1[] = { 5, 2, 3, 4 };
            wooden_a_supports_paint_setup(supportTypes1[direction], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
        case 2:
            bboy = (direction == 2 || direction == 3) ? 0 : 6;
            sub_98197C_rotated(direction, imageId, 0, 0, 32, 26, 2, height, 0, bboy, height);
            sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 26, 0, height, 0, bboy, height + 27);

            static const sint32 supportTypes2[] = { 3, 4, 5, 2 };
            wooden_a_supports_paint_setup(supportTypes2[direction], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            sub_98197C_rotated(direction, imageId, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
            break;
    }

    if (trackSequence == 0) {
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
    } else if (trackSequence == 3) {
        if (direction == 1 || direction == 2) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
    }

    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0089B190 */
static void paint_splash_boats_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4][2] =
            {
                    {   { SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_0 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_1 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_2 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_3 } },
                    {   { SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_0 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_1 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_2 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_3 } },
                    {   { SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_3 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_2 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_1 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_0 } },
                    {   { SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_3 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_2 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_1 },
                            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_0 } },
            };

    uint32 imageId = imageIds[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = imageIds[direction][trackSequence][1] | gTrackColours[SCHEME_TRACK];
    sint16 bboy;

    switch (trackSequence) {
        case 0:
            sub_98197C_rotated(direction, imageId, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
            break;
        case 1:
            bboy = (direction == 2 || direction == 3) ? 0 : 6;
            sub_98197C_rotated(direction, imageId, 0, 0, 32, 26, 2, height, 0, bboy, height);
            sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 26, 0, height, 0, bboy, height + 27);

            static const sint32 supportTypes1[] = { 4, 5, 2, 3 };
            wooden_a_supports_paint_setup(supportTypes1[direction], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 2:
            bboy = (direction == 0 || direction == 1) ? 0 : 6;
            sub_98197C_rotated(direction, imageId, 0, 0, 32, 26, 2, height, 0, bboy, height);
            sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 26, 0, height, 0, bboy, height + 27);

            static const sint32 supportTypes2[] = { 2, 3, 4, 5 };
            wooden_a_supports_paint_setup(supportTypes2[direction], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
        case 3:
            sub_98197C_rotated(direction, imageId, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
            break;
    }

    if (trackSequence == 0) {
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
    } else if (trackSequence == 3) {
        if (direction == 1 || direction == 2) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
    }

    paint_util_set_general_support_height(height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_splash_boats(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_25_DEG_UP:
        return paint_splash_boats_track_25_deg_up;
    case TRACK_ELEM_60_DEG_UP:
        return paint_splash_boats_track_60_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return paint_splash_boats_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return paint_splash_boats_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return paint_splash_boats_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return paint_splash_boats_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return paint_splash_boats_track_25_deg_down;
    case TRACK_ELEM_60_DEG_DOWN:
        return paint_splash_boats_track_60_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return paint_splash_boats_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return paint_splash_boats_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return paint_splash_boats_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return paint_splash_boats_track_25_deg_down_to_flat;

    // Originally taken from River Rafts
    case TRACK_ELEM_FLAT:
        return paint_splash_boats_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return paint_splash_boats_station;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return paint_splash_boats_track_left_quarter_turn_5_tiles;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return paint_splash_boats_track_right_quarter_turn_5_tiles;
    case TRACK_ELEM_S_BEND_LEFT:
        return paint_splash_boats_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return paint_splash_boats_track_s_bend_right;
    }

    return NULL;
}

#ifndef NO_VEHICLES
/**
 *
 *  rct2: 0x006D4295
 */
void vehicle_visual_splash_boats_or_water_coaster(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
    if (vehicle->is_child) {
        vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
    } else {
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_ride);
    }
    g_currently_drawn_item = vehicle;
    imageDirection = ((get_current_rotation() * 8) + vehicle->sprite_direction) & 0x1F;
    gPaintSpritePosition.x = vehicle->x;
    gPaintSpritePosition.y = vehicle->y;
    vehicle_paint(vehicle, imageDirection);
}
#endif
