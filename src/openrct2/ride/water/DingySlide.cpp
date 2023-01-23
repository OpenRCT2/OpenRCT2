/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Track.h"
#include "../TrackPaint.h"
namespace OpenRCT2
{
    enum
    {
        SPR_DINGHY_SLIDE_FLAT_SW_NE = 19720,
        SPR_DINGHY_SLIDE_FLAT_NW_SE = 19721,
        SPR_DINGHY_SLIDE_FLAT_FRONT_SW_NE = 19722,
        SPR_DINGHY_SLIDE_FLAT_FRONT_NW_SE = 19723,
        SPR_DINGHY_SLIDE_FLAT_CHAIN_SW_NE = 19724,
        SPR_DINGHY_SLIDE_FLAT_CHAIN_NW_SE = 19725,
        SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_SW_NE = 19726,
        SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_NW_SE = 19727,
        SPR_DINGHY_SLIDE_FLAT_CHAIN_NE_SW = 19728,
        SPR_DINGHY_SLIDE_FLAT_CHAIN_SE_NW = 19729,
        SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_NE_SW = 19730,
        SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_SE_NW = 19731,
        SPR_DINGHY_SLIDE_STATION_SW_NE = 19732,
        SPR_DINGHY_SLIDE_STATION_NW_SE = 19733,

        SPR_DINGHY_SLIDE_FLAT_COVERED_SW_NE = 19736,
        SPR_DINGHY_SLIDE_FLAT_COVERED_NW_SE = 19737,
        SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_SW_NE = 19738,
        SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_NW_SE = 19739,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_SW_NE = 19740,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_NW_SE = 19741,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_NE_SW = 19742,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_SE_NW = 19743,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_SW_NE = 19744,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_NW_SE = 19745,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_NE_SW = 19746,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_SE_NW = 19747,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_SW_NE = 19748,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_NW_SE = 19749,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_NE_SW = 19750,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_SE_NW = 19751,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_SW_NE = 19752,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_NW_SE = 19753,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_NE_SW = 19754,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_SE_NW = 19755,
        SPR_DINGHY_SLIDE_25_DEG_SW_NE = 19756,
        SPR_DINGHY_SLIDE_25_DEG_NW_SE = 19757,
        SPR_DINGHY_SLIDE_25_DEG_NE_SW = 19758,
        SPR_DINGHY_SLIDE_25_DEG_SE_NW = 19759,
        SPR_DINGHY_SLIDE_25_DEG_FRONT_SW_NE = 19760,
        SPR_DINGHY_SLIDE_25_DEG_FRONT_NW_SE = 19761,
        SPR_DINGHY_SLIDE_25_DEG_FRONT_NE_SW = 19762,
        SPR_DINGHY_SLIDE_25_DEG_FRONT_SE_NW = 19763,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_SW_NE = 19764,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_NW_SE = 19765,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_NE_SW = 19766,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_SE_NW = 19767,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_SW_NE = 19768,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_NW_SE = 19769,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_NE_SW = 19770,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_SE_NW = 19771,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_SW_NE = 19772,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_NW_SE = 19773,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_NE_SW = 19774,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_SE_NW = 19775,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_SW_NE = 19776,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_NW_SE = 19777,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_NE_SW = 19778,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_SE_NW = 19779,
        SPR_DINGHY_SLIDE_60_DEG_SW_NE = 19780,
        SPR_DINGHY_SLIDE_60_DEG_NW_SE = 19781,
        SPR_DINGHY_SLIDE_60_DEG_NE_SW = 19782,
        SPR_DINGHY_SLIDE_60_DEG_SE_NW = 19783,
        SPR_DINGHY_SLIDE_60_DEG_FRONT_SW_NE = 19784,
        SPR_DINGHY_SLIDE_60_DEG_FRONT_NW_SE = 19785,
        SPR_DINGHY_SLIDE_60_DEG_FRONT_NE_SW = 19786,
        SPR_DINGHY_SLIDE_60_DEG_FRONT_SE_NW = 19787,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_SW_NE = 19788,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_NW_SE = 19789,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_NE_SW = 19790,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_SE_NW = 19791,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_SW_NE = 19792,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_NW_SE = 19793,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_NE_SW = 19794,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_SE_NW = 19795,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_SW_NE = 19796,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_NW_SE = 19797,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_NE_SW = 19798,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_SE_NW = 19799,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_SW_NE = 19800,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_NW_SE = 19801,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_NE_SW = 19802,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_SE_NW = 19803,
        SPR_DINGHY_SLIDE_25_DEG_CHAIN_SW_NE = 19804,
        SPR_DINGHY_SLIDE_25_DEG_CHAIN_NW_SE = 19805,
        SPR_DINGHY_SLIDE_25_DEG_CHAIN_NE_SW = 19806,
        SPR_DINGHY_SLIDE_25_DEG_CHAIN_SE_NW = 19807,
        SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_SW_NE = 19808,
        SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_NW_SE = 19809,
        SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_NE_SW = 19810,
        SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_SE_NW = 19811,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_SW_NE = 19812,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_NW_SE = 19813,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_NE_SW = 19814,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_SE_NW = 19815,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_SW_NE = 19816,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_NW_SE = 19817,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_NE_SW = 19818,
        SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_SE_NW = 19819,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_SW_NE = 19820,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_NW_SE = 19821,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_NE_SW = 19822,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_SE_NW = 19823,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_SW_NE = 19824,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_NW_SE = 19825,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_NE_SW = 19826,
        SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_SE_NW = 19827,
        SPR_DINGHY_SLIDE_25_DEG_COVERED_SW_NE = 19828,
        SPR_DINGHY_SLIDE_25_DEG_COVERED_NW_SE = 19829,
        SPR_DINGHY_SLIDE_25_DEG_COVERED_NE_SW = 19830,
        SPR_DINGHY_SLIDE_25_DEG_COVERED_SE_NW = 19831,
        SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_SW_NE = 19832,
        SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_NW_SE = 19833,
        SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_NE_SW = 19834,
        SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_SE_NW = 19835,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_SW_NE = 19836,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_NW_SE = 19837,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_NE_SW = 19838,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_SE_NW = 19839,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_SW_NE = 19840,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_NW_SE = 19841,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_NE_SW = 19842,
        SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_SE_NW = 19843,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_SW_NE = 19844,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_NW_SE = 19845,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_NE_SW = 19846,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_SE_NW = 19847,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_SW_NE = 19848,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_NW_SE = 19849,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_NE_SW = 19850,
        SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_SE_NW = 19851,
        SPR_DINGHY_SLIDE_60_DEG_COVERED_SW_NE = 19852,
        SPR_DINGHY_SLIDE_60_DEG_COVERED_NW_SE = 19853,
        SPR_DINGHY_SLIDE_60_DEG_COVERED_NE_SW = 19854,
        SPR_DINGHY_SLIDE_60_DEG_COVERED_SE_NW = 19855,
        SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_SW_NE = 19856,
        SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_NW_SE = 19857,
        SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_NE_SW = 19858,
        SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_SE_NW = 19859,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_0 = 19860,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_1 = 19861,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_2 = 19862,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_0 = 19863,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_1 = 19864,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_2 = 19865,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_0 = 19866,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_1 = 19867,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_2 = 19868,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_0 = 19869,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_1 = 19870,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_2 = 19871,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_0 = 19872,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_1 = 19873,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_2 = 19874,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_0 = 19875,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_1 = 19876,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_2 = 19877,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_0 = 19878,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_1 = 19879,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_2 = 19880,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_0 = 19881,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_1 = 19882,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_2 = 19883,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_0 = 19884,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_1 = 19885,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_2 = 19886,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_0 = 19887,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_1 = 19888,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_2 = 19889,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_0 = 19890,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_1 = 19891,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_2 = 19892,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_0 = 19893,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_1 = 19894,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_2 = 19895,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_0 = 19896,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_1 = 19897,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_2 = 19898,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_0 = 19899,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_1 = 19900,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_2 = 19901,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_0 = 19902,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_1 = 19903,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_2 = 19904,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_0 = 19905,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_1 = 19906,
        SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_2 = 19907,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_0 = 19908,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_1 = 19909,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_2 = 19910,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_3 = 19911,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_4 = 19912,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_0 = 19913,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_1 = 19914,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_2 = 19915,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_3 = 19916,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_4 = 19917,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_0 = 19918,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_1 = 19919,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_2 = 19920,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_3 = 19921,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_4 = 19922,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_0 = 19923,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_1 = 19924,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_2 = 19925,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_3 = 19926,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_4 = 19927,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_0 = 19928,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_1 = 19929,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_2 = 19930,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_3 = 19931,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_4 = 19932,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_0 = 19933,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_1 = 19934,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_2 = 19935,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_3 = 19936,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_4 = 19937,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_0 = 19938,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_1 = 19939,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_2 = 19940,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_3 = 19941,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_4 = 19942,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_0 = 19943,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_1 = 19944,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_2 = 19945,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_3 = 19946,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_4 = 19947,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_0 = 19948,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_1 = 19949,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_2 = 19950,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_3 = 19951,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_4 = 19952,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_0 = 19953,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_1 = 19954,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_2 = 19955,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_3 = 19956,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_4 = 19957,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_0 = 19958,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_1 = 19959,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_2 = 19960,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_3 = 19961,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_4 = 19962,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_0 = 19963,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_1 = 19964,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_2 = 19965,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_3 = 19966,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_4 = 19967,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_0 = 19968,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_1 = 19969,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_2 = 19970,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_3 = 19971,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_4 = 19972,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_0 = 19973,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_1 = 19974,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_2 = 19975,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_3 = 19976,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_4 = 19977,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_0 = 19978,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_1 = 19979,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_2 = 19980,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_3 = 19981,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_4 = 19982,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_0 = 19983,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_1 = 19984,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_2 = 19985,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_3 = 19986,
        SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_4 = 19987,
        SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_3 = 19988,
        SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_2 = 19989,
        SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_1 = 19990,
        SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_0 = 19991,
        SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_0 = 19992,
        SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_1 = 19993,
        SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_2 = 19994,
        SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_3 = 19995,
        SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_3 = 19996,
        SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_2 = 19997,
        SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_1 = 19998,
        SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_0 = 19999,
        SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_0 = 20000,
        SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_1 = 20001,
        SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_2 = 20002,
        SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_3 = 20003,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_0 = 20004,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_1 = 20005,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_2 = 20006,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_3 = 20007,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_0 = 20008,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_1 = 20009,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_2 = 20010,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_3 = 20011,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_0 = 20012,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_1 = 20013,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_2 = 20014,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_3 = 20015,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_0 = 20016,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_1 = 20017,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_2 = 20018,
        SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_3 = 20019,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_3 = 20020,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_2 = 20021,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_1 = 20022,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_0 = 20023,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_0 = 20024,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_1 = 20025,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_2 = 20026,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_3 = 20027,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_3 = 20028,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_2 = 20029,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_1 = 20030,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_0 = 20031,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_0 = 20032,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_1 = 20033,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_2 = 20034,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_3 = 20035,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_0 = 20036,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_1 = 20037,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_2 = 20038,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_3 = 20039,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_0 = 20040,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_1 = 20041,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_2 = 20042,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_3 = 20043,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_0 = 20044,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_1 = 20045,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_2 = 20046,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_3 = 20047,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_0 = 20048,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_1 = 20049,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_2 = 20050,
        SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_3 = 20051,
    };

    static void DinghySlideTrackFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[2][4][2] = {
            {
                { SPR_DINGHY_SLIDE_FLAT_SW_NE, SPR_DINGHY_SLIDE_FLAT_FRONT_SW_NE },
                { SPR_DINGHY_SLIDE_FLAT_NW_SE, SPR_DINGHY_SLIDE_FLAT_FRONT_NW_SE },
                { SPR_DINGHY_SLIDE_FLAT_SW_NE, SPR_DINGHY_SLIDE_FLAT_FRONT_SW_NE },
                { SPR_DINGHY_SLIDE_FLAT_NW_SE, SPR_DINGHY_SLIDE_FLAT_FRONT_NW_SE },
            },
            {
                { SPR_DINGHY_SLIDE_FLAT_CHAIN_SW_NE, SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_SW_NE },
                { SPR_DINGHY_SLIDE_FLAT_CHAIN_NW_SE, SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_NW_SE },
                { SPR_DINGHY_SLIDE_FLAT_CHAIN_NE_SW, SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_NE_SW },
                { SPR_DINGHY_SLIDE_FLAT_CHAIN_SE_NW, SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_SE_NW },
            },
        };

        uint8_t isChained = trackElement.HasChain() ? 1 : 0;
        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[isChained][direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[isChained][direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void DinghySlideTrackStation(
        PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { SPR_DINGHY_SLIDE_STATION_SW_NE, SPR_STATION_BASE_B_SW_NE },
            { SPR_DINGHY_SLIDE_STATION_NW_SE, SPR_STATION_BASE_B_NW_SE },
            { SPR_DINGHY_SLIDE_STATION_SW_NE, SPR_STATION_BASE_B_SW_NE },
            { SPR_DINGHY_SLIDE_STATION_NW_SE, SPR_STATION_BASE_B_NW_SE },
        };

        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]), { 0, 0, height },
            { 32, 20, 1 }, { 0, 6, height + 3 });
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][1]), { 0, 0, height },
            { 32, 32, 1 });

        MetalASupportsPaintSetup(
            session, METAL_SUPPORTS_TUBES, 5 + (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        MetalASupportsPaintSetup(
            session, METAL_SUPPORTS_TUBES, 8 - (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

        TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);

        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void DinghySlideTrack25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[2][4][2] = {
            {
                { SPR_DINGHY_SLIDE_25_DEG_SW_NE, SPR_DINGHY_SLIDE_25_DEG_FRONT_SW_NE },
                { SPR_DINGHY_SLIDE_25_DEG_NW_SE, SPR_DINGHY_SLIDE_25_DEG_FRONT_NW_SE },
                { SPR_DINGHY_SLIDE_25_DEG_NE_SW, SPR_DINGHY_SLIDE_25_DEG_FRONT_NE_SW },
                { SPR_DINGHY_SLIDE_25_DEG_SE_NW, SPR_DINGHY_SLIDE_25_DEG_FRONT_SE_NW },
            },
            {
                { SPR_DINGHY_SLIDE_25_DEG_CHAIN_SW_NE, SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_SW_NE },
                { SPR_DINGHY_SLIDE_25_DEG_CHAIN_NW_SE, SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_NW_SE },
                { SPR_DINGHY_SLIDE_25_DEG_CHAIN_NE_SW, SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_NE_SW },
                { SPR_DINGHY_SLIDE_25_DEG_CHAIN_SE_NW, SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_SE_NW },
            },
        };

        uint8_t isChained = trackElement.HasChain() ? 1 : 0;
        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[isChained][direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[isChained][direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 50 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_2);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
    }

    static void DinghySlideTrack60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { SPR_DINGHY_SLIDE_60_DEG_SW_NE, SPR_DINGHY_SLIDE_60_DEG_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_60_DEG_NW_SE, SPR_DINGHY_SLIDE_60_DEG_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_60_DEG_NE_SW, SPR_DINGHY_SLIDE_60_DEG_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_60_DEG_SE_NW, SPR_DINGHY_SLIDE_60_DEG_FRONT_SE_NW },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 98 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 56, TUNNEL_2);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
    }

    static void DinghySlideTrackFlatTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[2][4][2] = {
            {
                { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_SW_NE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_SW_NE },
                { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_NW_SE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_NW_SE },
                { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_NE_SW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_NE_SW },
                { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_SE_NW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_SE_NW },
            },
            {
                { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_SW_NE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_SW_NE },
                { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_NW_SE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_NW_SE },
                { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_NE_SW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_NE_SW },
                { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_SE_NW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_SE_NW },
            },
        };

        uint8_t isChained = trackElement.HasChain() ? 1 : 0;
        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[isChained][direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[isChained][direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 42 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }

    static void DinghySlideTrack25DegUpTo60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_SW_NE, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_NW_SE, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_NE_SW, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_SE_NW, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_SE_NW },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 66 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }

    static void DinghySlideTrack60DegUpTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_SW_NE, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_NW_SE, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_NE_SW, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_SE_NW, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_SE_NW },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 66 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }

    static void DinghySlideTrack25DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[2][4][2] = {
            {
                { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_SW_NE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_SW_NE },
                { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_NW_SE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_NW_SE },
                { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_NE_SW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_NE_SW },
                { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_SE_NW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_SE_NW },
            },
            {
                { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_SW_NE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_SW_NE },
                { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_NW_SE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_NW_SE },
                { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_NE_SW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_NE_SW },
                { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_SE_NW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_SE_NW },
            },
        };

        uint8_t isChained = trackElement.HasChain() ? 1 : 0;
        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[isChained][direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[isChained][direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_12);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
    }

    static void DinghySlideTrack25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void DinghySlideTrack60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void DinghySlideTrackFlatTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void DinghySlideTrack25DegDownTo60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void DinghySlideTrack25DegDownToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void DinghySlideTrackRightQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const SpriteBb imageIds[4][5] = {
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_0, { 0, 2, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_1, { 0, 16, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_2, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_3, { 16, 0, 0 }, { 16, 0, 0 }, { 16, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_4, { 2, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_0, { 2, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_1, { 16, 0, 0 }, { 16, 0, 0 }, { 16, 34, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_2, { 0, 16, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_3, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_4, { 0, 2, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_0, { 0, 2, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_2, { 16, 16, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_3, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_4, { 2, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_0, { 2, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_2, { 16, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_3, { 0, 16, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_4, { 0, 2, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            },
        };
        static constexpr const SpriteBb frontImageIds[4][5] = {
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_0, { 0, 2, 0 }, { 0, 6, 27 }, { 32, 30, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_1, { 0, 16, 0 }, { 0, 16, 27 }, { 32, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_2, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_3, { 16, 0, 0 }, { 16, 0, 27 }, { 16, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_4, { 2, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_0, { 2, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_1, { 16, 0, 0 }, { 16, 0, 27 }, { 16, 34, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_2, { 0, 16, 0 }, { 0, 16, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_3, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_4, { 0, 2, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_0, { 0, 2, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_2, { 16, 16, 0 }, { 16, 16, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_3, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_4, { 2, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_0, { 2, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_2, { 16, 0, 0 }, { 16, 0, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_3, { 0, 16, 0 }, { 0, 16, 27 }, { 32, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_4, { 0, 2, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            },
        };

        TrackPaintUtilRightQuarterTurn5TilesPaint2(
            session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIds);
        TrackPaintUtilRightQuarterTurn5TilesPaint2(
            session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], frontImageIds);

        switch (trackSequence)
        {
            case 0:
            case 6:
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }

        if (direction == 0 && trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
        }
        if (direction == 0 && trackSequence == 6)
        {
            PaintUtilPushTunnelRight(session, height, TUNNEL_0);
        }
        if (direction == 1 && trackSequence == 6)
        {
            PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
        }
        if (direction == 3 && trackSequence == 0)
        {
            PaintUtilPushTunnelRight(session, height, TUNNEL_0);
        }

        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC, direction), 0xFFFF, 0);
                break;
            case 2:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4, direction), 0xFFFF, 0);
                break;
            case 5:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction),
                    0xFFFF, 0);
                break;
            case 6:
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8, direction), 0xFFFF, 0);
                break;
        }

        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void DinghySlideTrackLeftQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        DinghySlideTrackRightQuarterTurn5(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
    }

    static void DinghySlideTrack60DegDownTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void DinghySlideTrackSBendLeft(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][4][2] = {
            {
                { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_0 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_3 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_0 },
                { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_3 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_3 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_0 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_3 },
                { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_0 },
            },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][0]);
        auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][1]);
        int16_t bboy;

        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 27, 2 }, { 0, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 27, 0 }, { 0, 2, height + 27 });
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                break;
            case 1:
                bboy = (direction == 0 || direction == 1) ? 0 : 6;
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 26, 2 }, { 0, bboy, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 26, 0 }, { 0, bboy, height + 27 });
                if (direction == 0 || direction == 1)
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 5 + (direction & 1), (direction & 1), height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                break;
            case 2:
                bboy = (direction == 2 || direction == 3) ? 0 : 6;
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 26, 2 }, { 0, bboy, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 26, 0 }, { 0, bboy, height + 27 });
                if (direction == 2 || direction == 3)
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 5 + (direction & 1), (direction & 1), height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                break;
            case 3:
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 27, 2 }, { 0, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 27, 0 }, { 0, 2, height + 27 });
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                break;
        }

        if (trackSequence == 0)
        {
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
        }
        else if (trackSequence == 3)
        {
            if (direction == 1 || direction == 2)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
        }

        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void DinghySlideTrackSBendRight(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][4][2] = {
            {
                { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_0 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_3 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_0 },
                { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_3 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_3 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_0 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_3 },
                { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_0 },
            },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][0]);
        auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][1]);
        int16_t bboy;

        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 27, 2 }, { 0, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 27, 0 }, { 0, 2, height + 27 });
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                break;
            case 1:
                bboy = (direction == 2 || direction == 3) ? 0 : 6;
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 26, 2 }, { 0, bboy, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 26, 0 }, { 0, bboy, height + 27 });
                if (direction == 0 || direction == 1)
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 8 - (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                break;
            case 2:
                bboy = (direction == 0 || direction == 1) ? 0 : 6;
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 26, 2 }, { 0, bboy, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 26, 0 }, { 0, bboy, height + 27 });
                if (direction == 2 || direction == 3)
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 8 - (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                break;
            case 3:
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 27, 2 }, { 0, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 27, 0 }, { 0, 2, height + 27 });
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                break;
        }

        if (trackSequence == 0)
        {
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
        }
        else if (trackSequence == 3)
        {
            if (direction == 1 || direction == 2)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
        }

        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void DinghySlideTrackRightQuarterTurn3(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const SpriteBb imageIds[4][3] = {
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_1, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_1, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            },
        };
        static constexpr const SpriteBb frontImageIds[4][3] = {
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_1, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_2, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_2, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_2, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_1, { 0, 0, 0 }, { 0, 16, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_2, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            },
        };

        TrackPaintUtilRightQuarterTurn3TilesPaint3(
            session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIds);
        TrackPaintUtilRightQuarterTurn3TilesPaint3(
            session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], frontImageIds);
        TrackPaintUtilRightQuarterTurn3TilesTunnel(session, height, direction, trackSequence, TUNNEL_0);

        switch (trackSequence)
        {
            case 0:
            case 3:
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }

        int32_t blockedSegments = 0;
        switch (trackSequence)
        {
            case 0:
                blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC;
                break;
            case 2:
                blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0;
                break;
            case 3:
                blockedSegments = SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8;
                break;
        }
        PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void DinghySlideTrackLeftQuarterTurn3(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        DinghySlideTrackRightQuarterTurn3(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
    }

    static void DinghySlideTrackFlatCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { SPR_DINGHY_SLIDE_FLAT_COVERED_SW_NE, SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_FLAT_COVERED_NW_SE, SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_FLAT_COVERED_SW_NE, SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_FLAT_COVERED_NW_SE, SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_NW_SE },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void DinghySlideTrack25DegUpCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { SPR_DINGHY_SLIDE_25_DEG_COVERED_SW_NE, SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_25_DEG_COVERED_NW_SE, SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_25_DEG_COVERED_NE_SW, SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_25_DEG_COVERED_SE_NW, SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_SE_NW },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 50 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_2);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
    }

    static void DinghySlideTrack60DegUpCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { SPR_DINGHY_SLIDE_60_DEG_COVERED_SW_NE, SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_60_DEG_COVERED_NW_SE, SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_60_DEG_COVERED_NE_SW, SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_60_DEG_COVERED_SE_NW, SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_SE_NW },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 98 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 56, TUNNEL_2);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
    }

    static void DinghySlideTrackFlatTo25DegUpCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_SW_NE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_NW_SE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_NE_SW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_SE_NW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_SE_NW },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 42 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
    }

    static void DinghySlideTrack25DegUpTo60DegUpCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_SW_NE, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_NW_SE, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_NE_SW, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_SE_NW, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_SE_NW },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 66 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }

    static void DinghySlideTrack60DegUpTo25DegUpCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_SW_NE, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_NW_SE, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_NE_SW, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_SE_NW, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_SE_NW },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 66 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }

    static void DinghySlideTrack25DegUpToFlatCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_SW_NE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_NW_SE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_NE_SW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_SE_NW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_SE_NW },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 6, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_12);
        }

        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
    }

    static void DinghySlideTrack25DegDownCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrack25DegUpCovered(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void DinghySlideTrack60DegDownCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrack60DegUpCovered(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void DinghySlideTrackFlatTo25DegDownCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrack25DegUpToFlatCovered(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void DinghySlideTrack25DegDownTo60DegDownCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrack60DegUpTo25DegUpCovered(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void DinghySlideTrack25DegDownToFlatCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrackFlatTo25DegUpCovered(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void DinghySlideTrackRightQuarterTurn5Covered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const SpriteBb imageIds[4][5] = {
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_1, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_2, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_3, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_4, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 34, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_2, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_3, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_4, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_2, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_3, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_4, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_2, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_3, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_4, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            },
        };
        static constexpr const SpriteBb frontImageIds[4][5] = {
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 30, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_1, { 0, 0, 0 }, { 0, 16, 27 }, { 32, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_2, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_3, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_4, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 34, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_2, { 0, 0, 0 }, { 0, 16, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_3, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_4, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_2, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_3, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_4, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_2, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_3, { 0, 0, 0 }, { 0, 16, 27 }, { 32, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_4, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            },
        };

        TrackPaintUtilRightQuarterTurn5TilesPaint2(
            session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIds);
        TrackPaintUtilRightQuarterTurn5TilesPaint2(
            session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], frontImageIds);

        switch (trackSequence)
        {
            case 0:
            case 6:
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }

        if (direction == 0 && trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
        }
        if (direction == 0 && trackSequence == 6)
        {
            PaintUtilPushTunnelRight(session, height, TUNNEL_0);
        }
        if (direction == 1 && trackSequence == 6)
        {
            PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
        }
        if (direction == 3 && trackSequence == 0)
        {
            PaintUtilPushTunnelRight(session, height, TUNNEL_0);
        }

        switch (trackSequence)
        {
            case 0:
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC, direction), 0xFFFF, 0);
                break;
            case 2:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4, direction), 0xFFFF, 0);
                break;
            case 5:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction),
                    0xFFFF, 0);
                break;
            case 6:
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8, direction), 0xFFFF, 0);
                break;
        }

        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void DinghySlideTrackLeftQuarterTurn5Covered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        DinghySlideTrackRightQuarterTurn5Covered(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
    }

    static void DinghySlideTrackSBendLeftCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][4][2] = {
            {
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_0 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_3 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_0 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_3 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_3 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_0 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_3 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_0 },
            },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][0]);
        auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][1]);
        int16_t bboy;

        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 27, 2 }, { 0, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 27, 0 }, { 0, 2, height + 27 });
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                break;
            case 1:
                bboy = (direction == 0 || direction == 1) ? 0 : 6;
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 26, 2 }, { 0, bboy, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 26, 0 }, { 0, bboy, height + 27 });
                if (direction == 0 || direction == 1)
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 5 + (direction & 1), (direction & 1), height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                break;
            case 2:
                bboy = (direction == 2 || direction == 3) ? 0 : 6;
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 26, 2 }, { 0, bboy, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 26, 0 }, { 0, bboy, height + 27 });
                if (direction == 2 || direction == 3)
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 5 + (direction & 1), (direction & 1), height,
                        session.TrackColours[SCHEME_SUPPORTS]);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                break;
            case 3:
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 27, 2 }, { 0, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 27, 0 }, { 0, 2, height + 27 });
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                break;
        }

        if (trackSequence == 0)
        {
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
        }
        else if (trackSequence == 3)
        {
            if (direction == 1 || direction == 2)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
        }

        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void DinghySlideTrackSBendRightCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][4][2] = {
            {
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_0 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_3 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_0 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_3 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_3 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_0 },
            },
            {
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_3 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_2 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_1 },
                { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_0 },
            },
        };

        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][0]);
        auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][1]);
        int16_t bboy;

        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 27, 2 }, { 0, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 27, 0 }, { 0, 2, height + 27 });
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                break;
            case 1:
                bboy = (direction == 2 || direction == 3) ? 0 : 6;
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 26, 2 }, { 0, bboy, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 26, 0 }, { 0, bboy, height + 27 });
                if (direction == 0 || direction == 1)
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 8 - (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                break;
            case 2:
                bboy = (direction == 0 || direction == 1) ? 0 : 6;
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 26, 2 }, { 0, bboy, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 26, 0 }, { 0, bboy, height + 27 });
                if (direction == 2 || direction == 3)
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 8 - (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                break;
            case 3:
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 27, 2 }, { 0, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, frontImageId, { 0, 0, height }, { 32, 27, 0 }, { 0, 2, height + 27 });
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                break;
        }

        if (trackSequence == 0)
        {
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
        }
        else if (trackSequence == 3)
        {
            if (direction == 1 || direction == 2)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
        }

        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void DinghySlideTrackRightQuarterTurn3Covered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const SpriteBb imageIds[4][3] = {
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_1, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_1, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            },
        };
        static constexpr const SpriteBb frontImageIds[4][3] = {
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_1, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_2, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_2, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_2, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            },
            {
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_1, { 0, 0, 0 }, { 0, 16, 27 }, { 16, 16, 0 } },
                { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_2, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            },
        };

        TrackPaintUtilRightQuarterTurn3TilesPaint3(
            session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIds);
        TrackPaintUtilRightQuarterTurn3TilesPaint3(
            session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], frontImageIds);
        TrackPaintUtilRightQuarterTurn3TilesTunnel(session, height, direction, trackSequence, TUNNEL_0);

        switch (trackSequence)
        {
            case 0:
            case 3:
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }

        int32_t blockedSegments = 0;
        switch (trackSequence)
        {
            case 0:
                blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC;
                break;
            case 2:
                blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0;
                break;
            case 3:
                blockedSegments = SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8;
                break;
        }
        PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void DinghySlideTrackLeftQuarterTurn3Covered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        DinghySlideTrackRightQuarterTurn3Covered(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
    }

    static void DinghySlideTrack60DegDownTo25DegDownCovered(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        DinghySlideTrack25DegUpTo60DegUpCovered(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    TRACK_PAINT_FUNCTION GetTrackPaintFunctionDinghySlide(int32_t trackType)
    {
        switch (trackType)
        {
            case TrackElemType::Flat:
                return DinghySlideTrackFlat;
            case TrackElemType::EndStation:
            case TrackElemType::BeginStation:
            case TrackElemType::MiddleStation:
                return DinghySlideTrackStation;
            case TrackElemType::Up25:
                return DinghySlideTrack25DegUp;
            case TrackElemType::Up60:
                return DinghySlideTrack60DegUp;
            case TrackElemType::FlatToUp25:
                return DinghySlideTrackFlatTo25DegUp;
            case TrackElemType::Up25ToUp60:
                return DinghySlideTrack25DegUpTo60DegUp;
            case TrackElemType::Up60ToUp25:
                return DinghySlideTrack60DegUpTo25DegUp;
            case TrackElemType::Up25ToFlat:
                return DinghySlideTrack25DegUpToFlat;
            case TrackElemType::Down25:
                return DinghySlideTrack25DegDown;
            case TrackElemType::Down60:
                return DinghySlideTrack60DegDown;
            case TrackElemType::FlatToDown25:
                return DinghySlideTrackFlatTo25DegDown;
            case TrackElemType::Down25ToDown60:
                return DinghySlideTrack25DegDownTo60DegDown;
            case TrackElemType::Down60ToDown25:
                return DinghySlideTrack60DegDownTo25DegDown;
            case TrackElemType::Down25ToFlat:
                return DinghySlideTrack25DegDownToFlat;
            case TrackElemType::LeftQuarterTurn5Tiles:
                return DinghySlideTrackLeftQuarterTurn5;
            case TrackElemType::RightQuarterTurn5Tiles:
                return DinghySlideTrackRightQuarterTurn5;
            case TrackElemType::SBendLeft:
                return DinghySlideTrackSBendLeft;
            case TrackElemType::SBendRight:
                return DinghySlideTrackSBendRight;
            case TrackElemType::LeftQuarterTurn3Tiles:
                return DinghySlideTrackLeftQuarterTurn3;
            case TrackElemType::RightQuarterTurn3Tiles:
                return DinghySlideTrackRightQuarterTurn3;
            case TrackElemType::FlatCovered:
                return DinghySlideTrackFlatCovered;
            case TrackElemType::Up25Covered:
                return DinghySlideTrack25DegUpCovered;
            case TrackElemType::Up60Covered:
                return DinghySlideTrack60DegUpCovered;
            case TrackElemType::FlatToUp25Covered:
                return DinghySlideTrackFlatTo25DegUpCovered;
            case TrackElemType::Up25ToUp60Covered:
                return DinghySlideTrack25DegUpTo60DegUpCovered;
            case TrackElemType::Up60ToUp25Covered:
                return DinghySlideTrack60DegUpTo25DegUpCovered;
            case TrackElemType::Up25ToFlatCovered:
                return DinghySlideTrack25DegUpToFlatCovered;
            case TrackElemType::Down25Covered:
                return DinghySlideTrack25DegDownCovered;
            case TrackElemType::Down60Covered:
                return DinghySlideTrack60DegDownCovered;
            case TrackElemType::FlatToDown25Covered:
                return DinghySlideTrackFlatTo25DegDownCovered;
            case TrackElemType::Down25ToDown60Covered:
                return DinghySlideTrack25DegDownTo60DegDownCovered;
            case TrackElemType::Down60ToDown25Covered:
                return DinghySlideTrack60DegDownTo25DegDownCovered;
            case TrackElemType::Down25ToFlatCovered:
                return DinghySlideTrack25DegDownToFlatCovered;
            case TrackElemType::LeftQuarterTurn5TilesCovered:
                return DinghySlideTrackLeftQuarterTurn5Covered;
            case TrackElemType::RightQuarterTurn5TilesCovered:
                return DinghySlideTrackRightQuarterTurn5Covered;
            case TrackElemType::SBendLeftCovered:
                return DinghySlideTrackSBendLeftCovered;
            case TrackElemType::SBendRightCovered:
                return DinghySlideTrackSBendRightCovered;
            case TrackElemType::LeftQuarterTurn3TilesCovered:
                return DinghySlideTrackLeftQuarterTurn3Covered;
            case TrackElemType::RightQuarterTurn3TilesCovered:
                return DinghySlideTrackRightQuarterTurn3Covered;
        }

        return nullptr;
    }
} // namespace OpenRCT2
