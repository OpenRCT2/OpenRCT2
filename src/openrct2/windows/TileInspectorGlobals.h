/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"

#include <cstdint>

struct TileCoordsXY;

enum class TileInspectorPage : int16_t
{
    standard = 0,
    surface,
    path,
    track,
    scenery,
    entrance,
    wall,
    largeScenery,
    banner,
};

extern TileCoordsXY windowTileInspectorTile;
extern int32_t windowTileInspectorElementCount;
extern int32_t windowTileInspectorSelectedIndex;

constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE = 15;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_COPY = 18;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE = 17;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_SORT = 16;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE = 12;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP = 13;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN = 14;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE = 6;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE = 7;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE = 10;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE = 11;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SURFACE = EnumValue(TileInspectorPage::surface);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_PATH = EnumValue(TileInspectorPage::path);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_TRACK = EnumValue(TileInspectorPage::track);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_INCREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_DECREASE = 31;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SCENERY = EnumValue(TileInspectorPage::scenery);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_ENTRANCE = EnumValue(TileInspectorPage::entrance);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_WALL = EnumValue(TileInspectorPage::wall);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_LARGE_SCENERY = EnumValue(TileInspectorPage::largeScenery);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_BANNER = EnumValue(TileInspectorPage::banner);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_DECREASE = 30;
