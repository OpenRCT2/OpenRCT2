--[[
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
--]]

TrackMap3x3 = {
	[0]={ [0] = 0, 1, 2, 3, 4, 5, 6, 7, 8 },
		{ [0] = 0, 3, 5, 7, 2, 8, 1, 6, 4 },
		{ [0] = 0, 7, 8, 6, 5, 4, 3, 1, 2 },
		{ [0] = 0, 6, 4, 1, 8, 2, 7, 3, 5 }
}

EDGE_NE = 1
EDGE_SE = 2
EDGE_SW = 4
EDGE_NW = 8
EDGE_BOTTOMLEFT = EDGE_SW
EDGE_BOTTOMRIGHT = EDGE_SE
EDGE_TOPLEFT = EDGE_NW
EDGE_TOPRIGHT = EDGE_NE

Edges3x3 = {
	[0] = 0,
	bit32.bor(EDGE_NE, EDGE_NW),
	EDGE_NE,
	bit32.bor(EDGE_NE, EDGE_SE),
	EDGE_NW,
	EDGE_SE,
	bit32.bor(EDGE_SW, EDGE_NW),
	bit32.bor(EDGE_SW, EDGE_SE),
	EDGE_SW
}

SCHEME_TRACK = 0
SCHEME_SUPPORTS = 1
SCHEME_MISC = 2
SCHEME_3 = 3

SPR_FLOOR_CORK_SE_SW = 22134
SPR_FLOOR_CORK_SW = 22135
SPR_FLOOR_CORK_SE = 22136
SPR_FLOOR_CORK = 22137

FloorSpritesCork = {
	[0] = SPR_FLOOR_CORK_SE_SW,
	SPR_FLOOR_CORK_SW,
	SPR_FLOOR_CORK_SE,
	SPR_FLOOR_CORK
}

SPR_FENCE_ROPE_NE = 22138
SPR_FENCE_ROPE_SE = 22139
SPR_FENCE_ROPE_SW = 22140
SPR_FENCE_ROPE_NW = 22141

FenceSpritesRope = {
	[0] = SPR_FENCE_ROPE_NE,
	SPR_FENCE_ROPE_SE,
	SPR_FENCE_ROPE_SW,
	SPR_FENCE_ROPE_NW
}

RIDE_LIFECYCLE_ON_TRACK = 1

SEGMENT_B4 = bit32.lshift(1, 0)
SEGMENT_CC = bit32.lshift(1, 1)
SEGMENT_BC = bit32.lshift(1, 2)
SEGMENT_D4 = bit32.lshift(1, 3)
SEGMENT_C0 = bit32.lshift(1, 4)
SEGMENT_D0 = bit32.lshift(1, 5)
SEGMENT_B8 = bit32.lshift(1, 6)
SEGMENT_C8 = bit32.lshift(1, 7)
SEGMENT_C4 = bit32.lshift(1, 8)

SEGMENTS_ALL = bit32.bor(SEGMENT_B4, SEGMENT_CC, SEGMENT_BC, SEGMENT_D4, SEGMENT_C0, SEGMENT_D0, SEGMENT_B8, SEGMENT_C8, SEGMENT_C4)