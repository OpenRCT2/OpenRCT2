/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <string_view>

namespace OpenRCT2::Ui::ShortcutId
{
    // Interface / general
    constexpr std::string_view kInterfaceCloseTop = "interface.general.close_top";
    constexpr std::string_view kInterfaceCloseAll = "interface.general.close_all";
    constexpr std::string_view kInterfaceRotateConstruction = "interface.general.rotate_construction";
    constexpr std::string_view kInterfaceCancelConstruction = "interface.general.cancel_construction";
    constexpr std::string_view kInterfacePause = "interface.general.pause";
    constexpr std::string_view kInterfaceDecreaseSpeed = "interface.misc.decrease_speed";
    constexpr std::string_view kInterfaceIncreaseSpeed = "interface.misc.increase_speed";
    constexpr std::string_view kInterfaceToggleToolbars = "interface.misc.toggle_toolbars";
    constexpr std::string_view kInterfaceScreenshot = "interface.misc.screenshot";
    constexpr std::string_view kInterfaceGiantScreenshot = "interface.misc.giant_screenshot";
    constexpr std::string_view kInterfaceLoadGame = "interface.misc.load_game";
    constexpr std::string_view kInterfaceSaveGame = "interface.misc.save_game";
    constexpr std::string_view kInterfaceMute = "interface.misc.mute";
    constexpr std::string_view kInterfaceSceneryPicker = "interface.misc.scenery_picker";
    constexpr std::string_view kInterfaceDisableClearance = "interface.misc.disable_clearance";
    constexpr std::string_view kInterfaceMultiplayerChat = "interface.misc.multiplayer_chat";

    constexpr std::string_view kInterfaceScaleToggleWindowMode = "interface.graphics.toggle_window_mode";
    constexpr std::string_view kInterfaceScaleIncrease = "interface.graphics.increase";
    constexpr std::string_view kInterfaceScaleDecrease = "interface.graphics.decrease";

    // Interface / open
    constexpr std::string_view kInterfaceOpenLand = "interface.open.land";
    constexpr std::string_view kInterfaceOpenWater = "interface.open.water";
    constexpr std::string_view kInterfaceClearScenery = "interface.open.clear_scenery";
    constexpr std::string_view kInterfaceOpenScenery = "interface.open.scenery";
    constexpr std::string_view kInterfaceOpenFootpaths = "interface.open.footpaths";
    constexpr std::string_view kInterfaceOpenNewRide = "interface.open.new_ride";
    constexpr std::string_view kInterfaceOpenFinances = "interface.open.finances";
    constexpr std::string_view kInterfaceOpenResearch = "interface.open.research";
    constexpr std::string_view kInterfaceOpenRides = "interface.open.rides";
    constexpr std::string_view kInterfaceOpenPark = "interface.open.park";
    constexpr std::string_view kInterfaceOpenGuests = "interface.open.guests";
    constexpr std::string_view kInterfaceOpenStaff = "interface.open.staff";
    constexpr std::string_view kInterfaceOpenMessages = "interface.open.messages";
    constexpr std::string_view kInterfaceOpenMap = "interface.open.map";
    constexpr std::string_view kInterfaceShowOptions = "interface.open.options";
    constexpr std::string_view kInterfaceOpenKeyboardShortcuts = "interface.open.keyboard_shortcuts";
    constexpr std::string_view kInterfaceOpenTransparencyOptions = "interface.open.transparency_options";
    constexpr std::string_view kInterfaceOpenCheats = "interface.open.cheats";
    constexpr std::string_view kInterfaceOpenTileInspector = "interface.open.tileinspector";
    constexpr std::string_view kInterfaceMultiplayerShow = "interface.open.multiplayer";

    // View / general
    constexpr std::string_view kViewGeneralZoomOut = "view.general.zoom_out";
    constexpr std::string_view kViewGeneralZoomIn = "view.general.zoom_in";
    constexpr std::string_view kViewGeneralRotateClockwise = "view.general.rotate_clockwise";
    constexpr std::string_view kViewGeneralRotateAnticlockwise = "view.general.rotate_anticlockwise";

    // View / scroll
    constexpr std::string_view kViewScrollUp = "view.scroll.up";
    constexpr std::string_view kViewScrollLeft = "view.scroll.left";
    constexpr std::string_view kViewScrollRight = "view.scroll.right";
    constexpr std::string_view kViewScrollDown = "view.scroll.down";

    // View / toggle
    constexpr std::string_view kViewToggleUnderground = "view.toggle.show_underground";
    constexpr std::string_view kViewToggleTransparentWater = "view.toggle.transparent_water";
    constexpr std::string_view kViewToggleBaseLand = "view.toggle.hide_base_land";
    constexpr std::string_view kViewToggleVerticalLand = "view.toggle.hide_vertical_land";
    constexpr std::string_view kViewToggleRides = "view.toggle.transparent_rides";
    constexpr std::string_view kViewToggleVehicles = "view.toggle.transparent_vehicles";
    constexpr std::string_view kViewToggleVegetation = "view.toggle.transparent_vegetation";
    constexpr std::string_view kViewToggleScenery = "view.toggle.transparent_scenery";
    constexpr std::string_view kViewToggleFootpaths = "view.toggle.transparent_footpaths";
    constexpr std::string_view kViewToggleSupports = "view.toggle.hide_supports";
    constexpr std::string_view kViewToggleGuests = "view.toggle.hide_guests";
    constexpr std::string_view kViewToggleStaff = "view.toggle.hide_staff";
    constexpr std::string_view kViewToggleLandHeightMarkers = "view.toggle.show_land_height";
    constexpr std::string_view kViewToggleTrackHeightMarkers = "view.toggle.show_track_height";
    constexpr std::string_view kViewToggleFootpathHeightMarkers = "view.toggle.show_footpath_height";
    constexpr std::string_view kViewToggleGridlines = "view.toggle.show_gridlines";
    constexpr std::string_view kViewToggleCutAway = "view.toggle.toggle_cut_away";
    constexpr std::string_view kViewToggleFootpathIssues = "view.toggle.highlight_path_issues";

    // Window / ride construction
    constexpr std::string_view kWindowRideConstructionTurnLeft = "window.rideconstruction.turn_left";
    constexpr std::string_view kWindowRideConstructionTurnRight = "window.rideconstruction.turn_right";
    constexpr std::string_view kWindowRideConstructionDefault = "window.rideconstruction.default";
    constexpr std::string_view kWindowRideConstructionSlopeDown = "window.rideconstruction.slope_down";
    constexpr std::string_view kWindowRideConstructionSlopeUp = "window.rideconstruction.slope_up";
    constexpr std::string_view kWindowRideConstructionChainLift = "window.rideconstruction.chain_lift";
    constexpr std::string_view kWindowRideConstructionBankLeft = "window.rideconstruction.bank_left";
    constexpr std::string_view kWindowRideConstructionBankRight = "window.rideconstruction.bank_right";
    constexpr std::string_view kWindowRideConstructionPrevious = "window.rideconstruction.previous";
    constexpr std::string_view kWindowRideConstructionNext = "window.rideconstruction.next";
    constexpr std::string_view kWindowRideConstructionBuild = "window.rideconstruction.build";
    constexpr std::string_view kWindowRideConstructionDemolish = "window.rideconstruction.demolish";

    // Window / tile inspector
    constexpr std::string_view kWindowTileInspectorToggleInvisibility = "window.tileinspector.toggle_invisibility";
    constexpr std::string_view kWindowTileInspectorCopy = "window.tileinspector.copy";
    constexpr std::string_view kWindowTileInspectorPaste = "window.tileinspector.paste";
    constexpr std::string_view kWindowTileInspectorSort = "window.tileinspector.sort";
    constexpr std::string_view kWindowTileInspectorRemove = "window.tileinspector.remove";
    constexpr std::string_view kWindowTileInspectorMoveUp = "window.tileinspector.move_up";
    constexpr std::string_view kWindowTileInspectorMoveDown = "window.tileinspector.move_down";
    constexpr std::string_view kWindowTileInspectorIncreaseX = "window.tileinspector.increase_x";
    constexpr std::string_view kWindowTileInspectorDecreaseX = "window.tileinspector.decrease_x";
    constexpr std::string_view kWindowTileInspectorIncreaseY = "window.tileinspector.increase_y";
    constexpr std::string_view kWindowTileInspectorDecreaseY = "window.tileinspector.decrease_y";
    constexpr std::string_view kWindowTileInspectorIncreaseHeight = "window.tileinspector.increase_height";
    constexpr std::string_view kWindowTileInspectorDecreaseHeight = "window.tileinspector.decrease_height";
    constexpr std::string_view kWindowTileInspectorChangeWallSlope = "window.tileinspector.toggle_wall_slope";

    // Debug
    constexpr std::string_view kDebugToggleConsole = "debug.console";
    constexpr std::string_view kDebugTogglePaintDebugWindow = "debug.toggle_paint_debug_window";
    constexpr std::string_view kDebugAdvanceTick = "debug.advance_tick";
} // namespace OpenRCT2::Ui::ShortcutId
