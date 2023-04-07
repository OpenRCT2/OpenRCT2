/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../config/Config.h"
#include "../../entity/EntityRegistry.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"

enum
{
    SPR_SPLASH_BOATS_FLAT_TOP_SW_NE = 20820,
    SPR_SPLASH_BOATS_FLAT_TOP_NW_SE = 20821,
    SPR_SPLASH_BOATS_FLAT_TOP_NE_SW = 20822,
    SPR_SPLASH_BOATS_FLAT_TOP_SE_NW = 20823,

    SPR_SPLASH_BOATS_FLAT_SIDE_SW_NE = 20824,
    SPR_SPLASH_BOATS_FLAT_SIDE_NW_SE = 20825,
    SPR_SPLASH_BOATS_FLAT_SIDE_NE_SW = 20826,
    SPR_SPLASH_BOATS_FLAT_SIDE_SE_NW = 20827,

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

    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_0 = 20888,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_2 = 20889,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_3 = 20890,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_5 = 20891,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_6 = 20892,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_0 = 20893,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_2 = 20894,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_3 = 20895,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_5 = 20896,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_6 = 20897,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_0 = 20898,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_2 = 20899,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_3 = 20900,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_5 = 20901,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_6 = 20902,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_0 = 20903,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_2 = 20904,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_3 = 20905,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_5 = 20906,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_6 = 20907,

    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_0 = 20908,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_2 = 20909,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_3 = 20910,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_5 = 20911,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_6 = 20912,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_0 = 20913,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_2 = 20914,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_3 = 20915,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_5 = 20916,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_6 = 20917,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_0 = 20918,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_2 = 20919,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_3 = 20920,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_5 = 20921,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_6 = 20922,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_0 = 20923,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_2 = 20924,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_3 = 20925,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_5 = 20926,
    SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_6 = 20927,

    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_6 = 20908,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_5 = 20909,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_3 = 20910,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_2 = 20911,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_0 = 20912,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_6 = 20913,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_5 = 20914,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_3 = 20915,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_2 = 20916,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_0 = 20917,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_6 = 20918,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_5 = 20919,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_3 = 20920,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_2 = 20921,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_0 = 20922,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_6 = 20923,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_5 = 20924,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_3 = 20925,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_2 = 20926,
    SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_0 = 20927,

    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_6 = 20928,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_5 = 20929,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_3 = 20930,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_2 = 20931,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_0 = 20932,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_6 = 20933,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_5 = 20934,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_3 = 20935,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_2 = 20936,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_0 = 20937,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_6 = 20938,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_5 = 20939,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_3 = 20940,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_2 = 20941,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_0 = 20942,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_6 = 20943,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_5 = 20944,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_3 = 20945,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_2 = 20946,
    SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_0 = 20947,

    SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_0 = 20948,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_1 = 20949,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_2 = 20950,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_3 = 20951,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_0 = 20952,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_1 = 20953,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_2 = 20954,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_3 = 20955,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_0 = 20956,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_1 = 20957,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_2 = 20958,
    SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_3 = 20959,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_0 = 20960,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_1 = 20961,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_2 = 20962,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_3 = 20963,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_0 = 20964,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_1 = 20965,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_2 = 20966,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_3 = 20967,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_0 = 20968,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_1 = 20969,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_2 = 20970,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_3 = 20971,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_0 = 20972,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_1 = 20973,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_2 = 20974,
    SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_3 = 20975,
    SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_0 = 20976,
    SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_1 = 20977,
    SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_2 = 20978,
    SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_3 = 20979,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_3 = 20980,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_2 = 20981,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_1 = 20982,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_0 = 20983,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_3 = 20984,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_2 = 20985,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_1 = 20986,
    SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_0 = 20987,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_3 = 20988,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_2 = 20989,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_1 = 20990,
    SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_0 = 20991,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_3 = 20992,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_2 = 20993,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_1 = 20994,
    SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_0 = 20995,
};

static constexpr const uint32_t SplashBoats25DegUpImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_SE_NW,
};

static constexpr const uint32_t SplashBoats25DegUpFrontImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SE_NW,
};

static constexpr const uint32_t SplashBoats60DegUpImageId[4] = {
    SPR_SPLASH_BOATS_60_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_SE_NW,
};

static constexpr const uint32_t SplashBoats60DegUpFrontImageId[4] = {
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SE_NW,
};

static constexpr const uint32_t SplashBoatsFlatTo25DegUpImageId[4] = {
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SE_NW,
};

static constexpr const uint32_t SplashBoatsFlatTo25DegUpFrontImageId[4] = {
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SE_NW,
};

static constexpr const uint32_t SplashBoats25DegUpToFlatImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SE_NW,
};

static constexpr const uint32_t SplashBoats25DegUpToFlatFrontImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SE_NW,
};

static constexpr const uint32_t SplashBoats25DegUpTo60DegUpImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SE_NW,
};

static constexpr const uint32_t SplashBoats25DegUpTo60DegUpFrontImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SE_NW,
};

static constexpr const uint32_t SplashBoats60DegUpTo25DegUpImageId[4] = {
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SE_NW,
};

static constexpr const uint32_t SplashBoats60DegUpTo25DegUpFrontImageId[4] = {
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SE_NW,
};

static constexpr const uint32_t SplashBoats25DegDownImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_DOWN_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_DOWN_SE_NW,
};

static constexpr const uint32_t SplashBoats25DegDownFrontImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SE_NW,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NW_SE,
};

static constexpr const uint32_t SplashBoatsFlatTo25DegDownImageId[4] = {
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NW_SE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SE_NW,
};

static constexpr const uint32_t SplashBoatsFlatTo25DegDownFrontImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SE_NW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NW_SE,
};

static constexpr const uint32_t SplashBoats25DegDownToFlatImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SE_NW,
};

static constexpr const uint32_t SplashBoats25DegDownToFlatFrontImageId[4] = {
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SE_NW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NW_SE,
};

static constexpr const SpriteBb RiverRaftsLeftQuarterTurn5_Top[4][5] = {
    {
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_0, { 0, 0, 0 }, { 0, 2, 0 },  { 32, 27, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_2, { 0, 0, 0 }, { 0, 0, 0 },  { 32, 16, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_3, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_5, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 34, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SW_NW_SEQ_6, { 0, 0, 0 }, { 2, 0, 0 },  { 32, 32, 2 },
    },
    {
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_0, { 0, 0, 0 }, { 2, 0, 0 },   { 27, 32, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_2, { 0, 0, 0 }, { 0, 0, 0 },   { 16, 32, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_3, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_5, { 0, 0, 0 }, { 0, 0, 0 },   { 32, 16, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NW_NE_SEQ_6, { 0, 0, 0 }, { 0, 2, 0 },   { 32, 27, 2 },
    },
    {
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_0, { 0, 0, 0 }, { 0, 2, 0 },  { 32, 32, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_2, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_3, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_5, { 0, 0, 0 }, { 0, 0, 0 },  { 16, 32, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_NE_SE_SEQ_6, { 0, 0, 0 }, { 2, 0, 0 },  { 27, 32, 2 },
    },
    {
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_0, { 0, 0, 0 }, { 2, 0, 0 },  { 32, 32, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_2, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 32, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_3, { 0, 0, 0 }, { 0, 0, 0 },  { 16, 16, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_5, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 2 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_TOP_SE_SW_SEQ_6, { 0, 0, 0 }, { 0, 2, 0 },  { 32, 32, 2 },
    },
};

static constexpr const SpriteBb RiverRaftsLeftQuarterTurn5_Side[4][5] = {
    {
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_0, { 0, 0, 0 }, { 0, 2, 27 },  { 32, 27, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_2, { 0, 0, 0 }, { 0, 0, 27 },  { 32, 16, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_3, { 0, 0, 0 }, { 0, 16, 27 }, { 16, 16, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_5, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 34, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SW_NW_SEQ_6, { 0, 0, 0 }, { 2, 0, 27 },  { 32, 32, 0 },
    },
    {
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_0, { 0, 0, 0 }, { 2, 0, 27 },   { 27, 32, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_2, { 0, 0, 0 }, { 0, 0, 27 },   { 16, 32, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_3, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_5, { 0, 0, 0 }, { 0, 0, 27 },   { 32, 16, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NW_NE_SEQ_6, { 0, 0, 0 }, { 0, 2, 27 },   { 32, 27, 0 },
    },
    {
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_0, { 0, 0, 0 }, { 0, 2, 27 },  { 32, 32, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_2, { 0, 0, 0 }, { 0, 16, 27 }, { 32, 16, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_3, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 16, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_5, { 0, 0, 0 }, { 0, 0, 27 },  { 16, 32, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_NE_SE_SEQ_6, { 0, 0, 0 }, { 2, 0, 27 },  { 27, 32, 0 },
    },
    {
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_0, { 0, 0, 0 }, { 2, 0, 27 },  { 32, 32, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_2, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 32, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_3, { 0, 0, 0 }, { 0, 0, 27 },  { 16, 16, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_5, { 0, 0, 0 }, { 0, 16, 27 }, { 32, 16, 0 },
        SPR_SPLASH_BOATS_TURN_LEFT_5_SIDE_SE_SW_SEQ_6, { 0, 0, 0 }, { 0, 2, 27 },  { 32, 32, 0 },
    },
};

static constexpr const SpriteBb RiverRaftsRightQuarterTurn5_Top[4][5] = {
    {
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_0, { 0, 0, 0 }, { 0, 2, 0 },  { 32, 32, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_2, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_3, { 0, 0, 0 }, { 0, 0, 0 },  { 16, 16, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_5, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 32, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SW_SE_SEQ_6, { 0, 0, 0 }, { 2, 0, 0 },  { 32, 32, 2 },
    },
    {
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_0, { 0, 0, 0 }, { 2, 0, 0 },  { 32, 32, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_2, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 34, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_3, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_5, { 0, 0, 0 }, { 0, 0, 0 },  { 32, 16, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NW_SW_SEQ_6, { 0, 0, 0 }, { 0, 2, 0 },  { 32, 27, 2 },
    },
    {
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_0, { 0, 0, 0 }, { 0, 2, 0 },   { 32, 27, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_2, { 0, 0, 0 }, { 0, 0, 0 },   { 32, 16, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_3, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_5, { 0, 0, 0 }, { 0, 0, 0 },   { 16, 32, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_NE_NW_SEQ_6, { 0, 0, 0 }, { 2, 0, 0 },   { 27, 32, 2 },
    },
    {
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_0, { 0, 0, 0 }, { 2, 0, 0 },  { 27, 32, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_2, { 0, 0, 0 }, { 0, 0, 0 },  { 16, 32, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_3, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_5, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 2 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_TOP_SE_NE_SEQ_6, { 0, 0, 0 }, { 0, 2, 0 },  { 32, 32, 2 },
    },
};

static constexpr const SpriteBb RiverRaftsRightQuarterTurn5_Side[4][5] = {
    {
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_0, { 0, 0, 0 }, { 0, 2, 27 },  { 32, 32, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_2, { 0, 0, 0 }, { 0, 16, 27 }, { 32, 16, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_3, { 0, 0, 0 }, { 0, 0, 27 },  { 16, 16, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_5, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 32, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SW_SE_SEQ_6, { 0, 0, 0 }, { 2, 0, 27 },  { 32, 32, 0 },
    },
    {
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_0, { 0, 0, 0 }, { 2, 0, 27 },  { 32, 32, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_2, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 34, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_3, { 0, 0, 0 }, { 0, 16, 27 }, { 16, 16, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_5, { 0, 0, 0 }, { 0, 0, 27 },  { 32, 16, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NW_SW_SEQ_6, { 0, 0, 0 }, { 0, 2, 27 },  { 32, 27, 0 },
    },
    {
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_0, { 0, 0, 0 }, { 0, 2, 27 },   { 32, 27, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_2, { 0, 0, 0 }, { 0, 0, 27 },   { 32, 16, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_3, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_5, { 0, 0, 0 }, { 0, 0, 27 },   { 16, 32, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_NE_NW_SEQ_6, { 0, 0, 0 }, { 2, 0, 27 },   { 27, 32, 0 },
    },
    {
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_0, { 0, 0, 0 }, { 2, 0, 27 },  { 27, 32, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_2, { 0, 0, 0 }, { 0, 0, 27 },  { 16, 32, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_3, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 16, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_5, { 0, 0, 0 }, { 0, 16, 27 }, { 32, 16, 0 },
        SPR_SPLASH_BOATS_TURN_RIGHT_5_SIDE_SE_NE_SEQ_6, { 0, 0, 0 }, { 0, 2, 27 },  { 32, 32, 0 },
    },
};

static void PaintSplashBoatsTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats25DegUpImageId[direction]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats25DegUpFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 50 } });

    WoodenASupportsPaintSetup(session, (direction & 1), 9 + direction, height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

static void PaintSplashBoatsTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats60DegUpImageId[direction]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats60DegUpFrontImageId[direction]);

    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
        session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 98 } });

    WoodenASupportsPaintSetup(session, (direction & 1), 21 + direction, height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

static void PaintSplashBoatsTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoatsFlatTo25DegUpImageId[direction]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoatsFlatTo25DegUpFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 42 } });

    WoodenASupportsPaintSetup(session, (direction & 1), 1 + direction, height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

static void PaintSplashBoatsTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats25DegUpToFlatImageId[direction]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats25DegUpToFlatFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 34 } });

    WoodenASupportsPaintSetup(session, (direction & 1), 5 + direction, height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

static void PaintSplashBoatsTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats25DegUpTo60DegUpImageId[direction]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats25DegUpTo60DegUpFrontImageId[direction]);

    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
        session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 66 } });

    WoodenASupportsPaintSetup(session, (direction & 1), 13 + direction, height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

static void PaintSplashBoatsTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats60DegUpTo25DegUpImageId[direction]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats60DegUpTo25DegUpFrontImageId[direction]);

    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
        session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 66 } });

    WoodenASupportsPaintSetup(session, (direction & 1), 17 + direction, height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

static void PaintSplashBoatsTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats25DegDownImageId[direction]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats25DegDownFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 50 } });

    static constexpr const uint8_t specialSupport[] = { 11, 12, 9, 10 };
    WoodenASupportsPaintSetup(
        session, (direction & 1), specialSupport[direction], height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

static void PaintSplashBoatsTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintSplashBoatsTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void PaintSplashBoatsTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoatsFlatTo25DegDownImageId[direction]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoatsFlatTo25DegDownFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 34 } });

    static constexpr const uint8_t specialSupport[] = { 7, 8, 5, 6 };
    WoodenASupportsPaintSetup(
        session, (direction & 1), specialSupport[direction], height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

static void PaintSplashBoatsTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintSplashBoatsTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void PaintSplashBoatsTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintSplashBoatsTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void PaintSplashBoatsTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats25DegDownToFlatImageId[direction]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(SplashBoats25DegDownToFlatFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 42 } });

    static constexpr const uint8_t specialSupport[] = { 3, 4, 1, 2 };
    WoodenASupportsPaintSetup(
        session, (direction & 1), specialSupport[direction], height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** Start of elements originally from River Rafts */
/** rct2: 0x0089B170 */
static void PaintSplashBoatsTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 1 ? SPR_SPLASH_BOATS_FLAT_TOP_NW_SE : SPR_SPLASH_BOATS_FLAT_TOP_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 2 } });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 1 ? SPR_SPLASH_BOATS_FLAT_SIDE_NW_SE : SPR_SPLASH_BOATS_FLAT_SIDE_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height }, { 1, 32, 26 } });
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 0 ? SPR_SPLASH_BOATS_FLAT_TOP_SW_NE : SPR_SPLASH_BOATS_FLAT_TOP_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 0 ? SPR_SPLASH_BOATS_FLAT_SIDE_SW_NE : SPR_SPLASH_BOATS_FLAT_SIDE_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 26 } });
    }

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0089B1A0 */
static void PaintSplashBoatsStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (direction & 1)
    {
        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 1 ? SPR_SPLASH_BOATS_FLAT_TOP_NW_SE : SPR_SPLASH_BOATS_FLAT_TOP_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height + 3 }, { 20, 32, 1 } });

        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 });
    }
    else
    {
        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 0 ? SPR_SPLASH_BOATS_FLAT_TOP_SW_NE : SPR_SPLASH_BOATS_FLAT_TOP_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height + 3 }, { 32, 20, 1 } });

        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 });
    }

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    TrackPaintUtilDrawNarrowStationPlatform(session, ride, direction, height, 7, trackElement);

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0089B1D0 */
static void PaintSplashBoatsTrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    TrackPaintUtilRightQuarterTurn5TilesPaint2(
        session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], RiverRaftsLeftQuarterTurn5_Top);
    TrackPaintUtilRightQuarterTurn5TilesPaint2(
        session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], RiverRaftsLeftQuarterTurn5_Side);

    if (trackSequence != 1 && trackSequence != 4)
    {
        static constexpr const uint8_t supportTypes[][7] = {
            { 0, 0xFF, 5, 3, 0xFF, 5, 1 },
            { 1, 0xFF, 2, 4, 0xFF, 2, 0 },
            { 0, 0xFF, 3, 5, 0xFF, 3, 1 },
            { 1, 0xFF, 4, 2, 0xFF, 4, 0 },
        };
        uint8_t supportType = supportTypes[direction][trackSequence];
        WoodenASupportsPaintSetup(session, supportType, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4 | SEGMENT_B8 | SEGMENT_D0, direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_CC | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_B4 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_D0,
                    direction),
                0xFFFF, 0);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_B8, direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
    }

    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            break;
        case 6:
            if (direction == 2 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, TUNNEL_SQUARE_FLAT);
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0089B1D0 */
static void PaintSplashBoatsTrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    TrackPaintUtilRightQuarterTurn5TilesPaint2(
        session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], RiverRaftsRightQuarterTurn5_Top);
    TrackPaintUtilRightQuarterTurn5TilesPaint2(
        session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], RiverRaftsRightQuarterTurn5_Side);

    if (trackSequence != 1 && trackSequence != 4)
    {
        static constexpr const uint8_t supportTypes[][7] = {
            { 0, 0xFF, 4, 2, 0xFF, 4, 1 },
            { 1, 0xFF, 5, 3, 0xFF, 5, 0 },
            { 0, 0xFF, 2, 4, 0xFF, 2, 1 },
            { 1, 0xFF, 3, 5, 0xFF, 3, 0 },
        };
        uint8_t supportType = supportTypes[direction][trackSequence];
        WoodenASupportsPaintSetup(session, supportType, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4,
                    direction),
                0xFFFF, 0);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
    }

    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            break;
        case 6:
            if (direction == 0 || direction == 1)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, TUNNEL_SQUARE_FLAT);
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0089B180 */
static void PaintSplashBoatsTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][4][2] = {
        {
            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_0 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_1 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_2 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_NW_NE_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_3 },
        },
        {
            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_0 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_1 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_2 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_SW_NW_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_3 },
        },
        {
            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_3 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_2 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_1 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_SE_SW_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_NW_NE_SEQ_0 },
        },
        {
            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_3 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_2 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_1 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_NE_SE_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NW_SW_NW_SEQ_0 },
        },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][1]);
    int16_t bboy;
    static constexpr const int32_t supportTypes1[] = { 5, 2, 3, 4 };
    static constexpr const int32_t supportTypes2[] = { 3, 4, 5, 2 };

    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            WoodenASupportsPaintSetup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
        case 1:
            bboy = (direction == 0 || direction == 1) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });

            WoodenASupportsPaintSetup(session, supportTypes1[direction], 0, height, session.TrackColours[SCHEME_SUPPORTS]);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            break;
        case 2:
            bboy = (direction == 2 || direction == 3) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });

            WoodenASupportsPaintSetup(session, supportTypes2[direction], 0, height, session.TrackColours[SCHEME_SUPPORTS]);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            WoodenASupportsPaintSetup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
    }

    if (trackSequence == 0)
    {
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
    }
    else if (trackSequence == 3)
    {
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0089B190 */
static void PaintSplashBoatsTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][4][2] = {
        {
            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_0 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_1 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_2 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NE_SE_NE_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_3 },
        },
        {
            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_0 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_1 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_2 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SE_SW_SE_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_3 },
        },
        {
            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_3 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_2 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_1 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_SW_NW_SW_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_NE_SE_NE_SEQ_0 },
        },
        {
            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_0, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_3 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_1, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_2 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_2, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_1 },
            { SPR_SPLASH_BOATS_S_BEND_TOP_NW_NE_NW_SEQ_3, SPR_SPLASH_BOATS_S_BEND_SIDE_SE_SW_SE_SEQ_0 },
        },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][1]);
    int16_t bboy;
    static constexpr const int32_t supportTypes1[] = { 4, 5, 2, 3 };
    static constexpr const int32_t supportTypes2[] = { 2, 3, 4, 5 };

    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            WoodenASupportsPaintSetup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
        case 1:
            bboy = (direction == 2 || direction == 3) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });

            WoodenASupportsPaintSetup(session, supportTypes1[direction], 0, height, session.TrackColours[SCHEME_SUPPORTS]);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            break;
        case 2:
            bboy = (direction == 0 || direction == 1) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });

            WoodenASupportsPaintSetup(session, supportTypes2[direction], 0, height, session.TrackColours[SCHEME_SUPPORTS]);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            WoodenASupportsPaintSetup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
    }

    if (trackSequence == 0)
    {
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
    }
    else if (trackSequence == 3)
    {
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void PaintSplashBoatsTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 0, 1 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 0, 1 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 0, 1 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 0, 1 });
            break;
    }

    PaintSplashBoatsTrackFlat(session, ride, trackSequence, direction, height, trackElement);

    TrackPaintUtilOnridePhotoPaint(session, direction, height + 3, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionSplashBoats(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Up25:
            return PaintSplashBoatsTrack25DegUp;
        case TrackElemType::Up60:
            return PaintSplashBoatsTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return PaintSplashBoatsTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return PaintSplashBoatsTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return PaintSplashBoatsTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintSplashBoatsTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return PaintSplashBoatsTrack25DegDown;
        case TrackElemType::Down60:
            return PaintSplashBoatsTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return PaintSplashBoatsTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return PaintSplashBoatsTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return PaintSplashBoatsTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintSplashBoatsTrack25DegDownToFlat;

        // Originally taken from River Rafts
        case TrackElemType::Flat:
            return PaintSplashBoatsTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintSplashBoatsStation;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return PaintSplashBoatsTrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return PaintSplashBoatsTrackRightQuarterTurn5Tiles;
        case TrackElemType::SBendLeft:
            return PaintSplashBoatsTrackSBendLeft;
        case TrackElemType::SBendRight:
            return PaintSplashBoatsTrackSBendRight;

        // Added by OpenRCT2
        case TrackElemType::OnRidePhoto:
            return PaintSplashBoatsTrackOnRidePhoto;
    }

    return nullptr;
}

#ifndef NO_VEHICLES
/**
 *
 *  rct2: 0x006D4295
 */
void VehicleVisualSplashBoatsOrWaterCoaster(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry)
{
    auto* vehicleToPaint = vehicle->IsHead() ? GetEntity<Vehicle>(vehicle->next_vehicle_on_ride)
                                             : GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
    if (vehicleToPaint == nullptr)
    {
        return;
    }

    session.CurrentlyDrawnEntity = vehicleToPaint;
    imageDirection = OpenRCT2::Entity::Yaw::Add(
        OpenRCT2::Entity::Yaw::YawFrom4(session.CurrentRotation), vehicleToPaint->SpriteData.Direction);
    session.SpritePosition.x = vehicleToPaint->x;
    session.SpritePosition.y = vehicleToPaint->y;
    vehicleToPaint->Paint(session, imageDirection);
}
#endif
