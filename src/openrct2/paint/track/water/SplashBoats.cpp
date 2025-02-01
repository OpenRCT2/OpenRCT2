/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

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

static constexpr uint32_t kSplashBoats25DegUpImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_SE_NW,
};

static constexpr uint32_t kSplashBoats25DegUpFrontImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SE_NW,
};

static constexpr uint32_t kSplashBoats60DegUpImageId[4] = {
    SPR_SPLASH_BOATS_60_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_SE_NW,
};

static constexpr uint32_t kSplashBoats60DegUpFrontImageId[4] = {
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_FRONT_SE_NW,
};

static constexpr uint32_t kSplashBoatsFlatTo25DegUpImageId[4] = {
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_SE_NW,
};

static constexpr uint32_t kSplashBoatsFlatTo25DegUpFrontImageId[4] = {
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SE_NW,
};

static constexpr uint32_t kSplashBoats25DegUpToFlatImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_SE_NW,
};

static constexpr uint32_t kSplashBoats25DegUpToFlatFrontImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SE_NW,
};

static constexpr uint32_t kSplashBoats25DegUpTo60DegUpImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_SE_NW,
};

static constexpr uint32_t kSplashBoats25DegUpTo60DegUpFrontImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_60_DEG_UP_FRONT_SE_NW,
};

static constexpr uint32_t kSplashBoats60DegUpTo25DegUpImageId[4] = {
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_SE_NW,
};

static constexpr uint32_t kSplashBoats60DegUpTo25DegUpFrontImageId[4] = {
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NW_SE,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_60_DEG_UP_TO_25_DEG_UP_FRONT_SE_NW,
};

static constexpr uint32_t kSplashBoats25DegDownImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_DOWN_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_DOWN_SE_NW,
};

static constexpr uint32_t kSplashBoats25DegDownFrontImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SE_NW,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_FRONT_NW_SE,
};

static constexpr uint32_t kSplashBoatsFlatTo25DegDownImageId[4] = {
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NW_SE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_DOWN_SE_NW,
};

static constexpr uint32_t kSplashBoatsFlatTo25DegDownFrontImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SE_NW,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_UP_TO_FLAT_FRONT_NW_SE,
};

static constexpr uint32_t kSplashBoats25DegDownToFlatImageId[4] = {
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SW_NE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NW_SE,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_NE_SW,
    SPR_SPLASH_BOATS_25_DEG_DOWN_TO_FLAT_SE_NW,
};

static constexpr uint32_t kSplashBoats25DegDownToFlatFrontImageId[4] = {
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NE_SW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SE_NW,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_SW_NE,
    SPR_SPLASH_BOATS_FLAT_TO_25_DEG_UP_FRONT_NW_SE,
};

static constexpr SpriteBb kSplashBoatsLeftQuarterTurn5_Top[4][5] = {
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

static constexpr SpriteBb kSplashBoatsLeftQuarterTurn5_Side[4][5] = {
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

static constexpr SpriteBb kSplashBoatsRightQuarterTurn5_Top[4][5] = {
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

static constexpr SpriteBb kSplashBoatsRightQuarterTurn5_Side[4][5] = {
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
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(kSplashBoats25DegUpImageId[direction]);
    auto frontImageId = session.TrackColours.WithIndex(kSplashBoats25DegUpFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 50 } });

    DrawSupportForSequenceA<TrackElemType::Up25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void PaintSplashBoatsTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(kSplashBoats60DegUpImageId[direction]);
    auto frontImageId = session.TrackColours.WithIndex(kSplashBoats60DegUpFrontImageId[direction]);

    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
        session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 98 } });

    DrawSupportForSequenceA<TrackElemType::Up60>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

static void PaintSplashBoatsTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(kSplashBoatsFlatTo25DegUpImageId[direction]);
    auto frontImageId = session.TrackColours.WithIndex(kSplashBoatsFlatTo25DegUpFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 42 } });

    DrawSupportForSequenceA<TrackElemType::FlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void PaintSplashBoatsTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(kSplashBoats25DegUpToFlatImageId[direction]);
    auto frontImageId = session.TrackColours.WithIndex(kSplashBoats25DegUpToFlatFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 34 } });

    DrawSupportForSequenceA<TrackElemType::Up25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

static void PaintSplashBoatsTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(kSplashBoats25DegUpTo60DegUpImageId[direction]);
    auto frontImageId = session.TrackColours.WithIndex(kSplashBoats25DegUpTo60DegUpFrontImageId[direction]);

    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
        session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 66 } });

    DrawSupportForSequenceA<TrackElemType::Up25ToUp60>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void PaintSplashBoatsTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(kSplashBoats60DegUpTo25DegUpImageId[direction]);
    auto frontImageId = session.TrackColours.WithIndex(kSplashBoats60DegUpTo25DegUpFrontImageId[direction]);

    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
        session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 66 } });

    DrawSupportForSequenceA<TrackElemType::Up60ToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void PaintSplashBoatsTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(kSplashBoats25DegDownImageId[direction]);
    auto frontImageId = session.TrackColours.WithIndex(kSplashBoats25DegDownFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 50 } });

    DrawSupportForSequenceA<TrackElemType::Down25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void PaintSplashBoatsTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintSplashBoatsTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void PaintSplashBoatsTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(kSplashBoatsFlatTo25DegDownImageId[direction]);
    auto frontImageId = session.TrackColours.WithIndex(kSplashBoatsFlatTo25DegDownFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 34 } });

    DrawSupportForSequenceA<TrackElemType::FlatToDown25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

static void PaintSplashBoatsTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintSplashBoatsTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void PaintSplashBoatsTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintSplashBoatsTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void PaintSplashBoatsTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(kSplashBoats25DegDownToFlatImageId[direction]);
    auto frontImageId = session.TrackColours.WithIndex(kSplashBoats25DegDownToFlatFrontImageId[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 42 } });

    DrawSupportForSequenceA<TrackElemType::Down25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** Start of elements originally from River Rafts */
/** rct2: 0x0089B170 */
static void PaintSplashBoatsTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex(
            (direction == 1 ? SPR_SPLASH_BOATS_FLAT_TOP_NW_SE : SPR_SPLASH_BOATS_FLAT_TOP_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 2 } });

        imageId = session.TrackColours.WithIndex(
            (direction == 1 ? SPR_SPLASH_BOATS_FLAT_SIDE_NW_SE : SPR_SPLASH_BOATS_FLAT_SIDE_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height }, { 1, 32, 26 } });
    }
    else
    {
        imageId = session.TrackColours.WithIndex(
            (direction == 0 ? SPR_SPLASH_BOATS_FLAT_TOP_SW_NE : SPR_SPLASH_BOATS_FLAT_TOP_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

        imageId = session.TrackColours.WithIndex(
            (direction == 0 ? SPR_SPLASH_BOATS_FLAT_SIDE_SW_NE : SPR_SPLASH_BOATS_FLAT_SIDE_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 26 } });
    }

    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0089B1A0 */
static void PaintSplashBoatsStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (direction & 1)
    {
        auto imageId = session.TrackColours.WithIndex(
            (direction == 1 ? SPR_SPLASH_BOATS_FLAT_TOP_NW_SE : SPR_SPLASH_BOATS_FLAT_TOP_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height + 3 }, { 20, 32, 1 } });

        imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 });
    }
    else
    {
        auto imageId = session.TrackColours.WithIndex(
            (direction == 0 ? SPR_SPLASH_BOATS_FLAT_TOP_SW_NE : SPR_SPLASH_BOATS_FLAT_TOP_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height + 3 }, { 32, 20, 1 } });

        imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 });
    }

    DrawSupportForSequenceA<TrackElemType::EndStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    TrackPaintUtilDrawNarrowStationPlatform(session, ride, direction, height, 7, trackElement);

    TrackPaintUtilDrawStationTunnel(session, direction, height);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0089B1D0 */
static void PaintSplashBoatsTrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilRightQuarterTurn5TilesPaint2(
        session, height, direction, trackSequence, session.TrackColours, kSplashBoatsLeftQuarterTurn5_Top);
    TrackPaintUtilRightQuarterTurn5TilesPaint2(
        session, height, direction, trackSequence, session.TrackColours, kSplashBoatsLeftQuarterTurn5_Side);

    DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::topCorner,
                        PaintSegment::leftCorner, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topRightSide, PaintSegment::bottomRightSide, PaintSegment::rightCorner,
                        PaintSegment::centre, PaintSegment::topCorner, PaintSegment::bottomCorner, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topLeftSide, PaintSegment::centre, PaintSegment::bottomRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomCorner, PaintSegment::leftCorner),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 6:
            if (direction == 2 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0089B1D0 */
static void PaintSplashBoatsTrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilRightQuarterTurn5TilesPaint2(
        session, height, direction, trackSequence, session.TrackColours, kSplashBoatsRightQuarterTurn5_Top);
    TrackPaintUtilRightQuarterTurn5TilesPaint2(
        session, height, direction, trackSequence, session.TrackColours, kSplashBoatsRightQuarterTurn5_Side);

    DrawSupportForSequenceA<TrackElemType::RightQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::bottomRightSide,
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::centre,
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRightSide, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::topLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 6:
            if (direction == 0 || direction == 1)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0089B180 */
static void PaintSplashBoatsTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][4][2] = {
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

    auto imageId = session.TrackColours.WithIndex(imageIds[direction][trackSequence][0]);
    auto frontImageId = session.TrackColours.WithIndex(imageIds[direction][trackSequence][1]);
    int16_t bboy;

    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 1:
            bboy = (direction == 0 || direction == 1) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });

            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            bboy = (direction == 2 || direction == 3) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });

            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    if (trackSequence == 0)
    {
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else if (trackSequence == 3)
    {
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0089B190 */
static void PaintSplashBoatsTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][4][2] = {
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

    auto imageId = session.TrackColours.WithIndex(imageIds[direction][trackSequence][0]);
    auto frontImageId = session.TrackColours.WithIndex(imageIds[direction][trackSequence][1]);
    int16_t bboy;

    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            DrawSupportForSequenceA<TrackElemType::SBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 1:
            bboy = (direction == 2 || direction == 3) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });

            DrawSupportForSequenceA<TrackElemType::SBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            bboy = (direction == 0 || direction == 1) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });

            DrawSupportForSequenceA<TrackElemType::SBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            DrawSupportForSequenceA<TrackElemType::SBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    if (trackSequence == 0)
    {
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else if (trackSequence == 3)
    {
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void PaintSplashBoatsTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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

    PaintSplashBoatsTrackFlat(session, ride, trackSequence, direction, height, trackElement, supportType);

    TrackPaintUtilOnridePhotoPaint2(session, direction, trackElement, height);
}

TrackPaintFunction GetTrackPaintFunctionSplashBoats(OpenRCT2::TrackElemType trackType)
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
        default:
            return TrackPaintFunctionDummy;
    }
}
