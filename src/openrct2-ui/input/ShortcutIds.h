/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
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
    constexpr std::string_view InterfaceCloseTop = "interface.general.close_top";
    constexpr std::string_view InterfaceCloseAll = "interface.general.close_all";
    constexpr std::string_view InterfaceCancelConstruction = "interface.general.cancel_construction";
    constexpr std::string_view InterfacePause = "interface.general.pause";
    constexpr std::string_view InterfaceZoomOut = "interface.general.zoom_out";
    constexpr std::string_view InterfaceZoomIn = "interface.general.zoom_in";
    constexpr std::string_view InterfaceRotateClockwise = "interface.general.rotate_clockwise";
    constexpr std::string_view InterfaceRotateAnticlockwise = "interface.general.rotate_anticlockwise";
    constexpr std::string_view InterfaceRotateConstruction = "interface.general.rotate_construction";
    constexpr std::string_view InterfaceDecreaseSpeed = "interface.misc.decrease_speed";
    constexpr std::string_view InterfaceIncreaseSpeed = "interface.misc.increase_speed";
    constexpr std::string_view InterfaceToggleToolbars = "interface.misc.toggle_toolbars";
    constexpr std::string_view InterfaceScreenshot = "interface.misc.screenshot";
    constexpr std::string_view InterfaceLoadGame = "interface.misc.load_game";
    constexpr std::string_view InterfaceSaveGame = "interface.misc.save_game";
    constexpr std::string_view InterfaceMute = "interface.misc.mute";
    constexpr std::string_view InterfaceSceneryPicker = "interface.misc.scenery_picker";
    constexpr std::string_view InterfaceDisableClearance = "interface.misc.disable_clearance";
    constexpr std::string_view MultiplayerChat = "interface.misc.multiplayer_chat";

    // Interface / scroll
    constexpr std::string_view ScrollUp = "interface.scroll.up";
    constexpr std::string_view ScrollLeft = "interface.scroll.left";
    constexpr std::string_view ScrollRight = "interface.scroll.right";
    constexpr std::string_view ScrollDown = "interface.scroll.down";

    // Interface / graphics
    constexpr std::string_view ScaleToggleWindowMode = "interface.graphics.toggle_window_mode";
    constexpr std::string_view InterfaceScaleIncrease = "interface.graphics.increase";
    constexpr std::string_view InterfaceScaleDecrease = "interface.graphics.decrease";

    // Interface / open
    constexpr std::string_view InterfaceOpenLand = "interface.open.land";
    constexpr std::string_view InterfaceOpenWater = "interface.open.water";
    constexpr std::string_view InterfaceClearScenery = "interface.open.clear_scenery";
    constexpr std::string_view InterfaceOpenScenery = "interface.open.scenery";
    constexpr std::string_view InterfaceOpenFootpaths = "interface.open.footpaths";
    constexpr std::string_view InterfaceOpenNewRide = "interface.open.new_ride";
    constexpr std::string_view InterfaceOpenFinances = "interface.open.finances";
    constexpr std::string_view InterfaceOpenResearch = "interface.open.research";
    constexpr std::string_view InterfaceOpenRides = "interface.open.rides";
    constexpr std::string_view InterfaceOpenPark = "interface.open.park";
    constexpr std::string_view InterfaceOpenGuests = "interface.open.guests";
    constexpr std::string_view InterfaceOpenStaff = "interface.open.staff";
    constexpr std::string_view InterfaceOpenMessages = "interface.open.messages";
    constexpr std::string_view InterfaceOpenMap = "interface.open.map";
    constexpr std::string_view InterfaceShowOptions = "interface.open.options";
    constexpr std::string_view InterfaceOpenCheats = "interface.open.cheats";
    constexpr std::string_view InterfaceOpenTileInspector = "interface.open.tileinspector";
    constexpr std::string_view MultiplayerShow = "interface.open.multiplayer";

    // View
    constexpr std::string_view ViewToggleUnderground = "view.show_underground";
    constexpr std::string_view ViewToggleBaseLand = "view.hide_base_land";
    constexpr std::string_view ViewToggleVerticalLand = "view.hide_vertical_land";
    constexpr std::string_view ViewToggleRides = "view.transparent_rides";
    constexpr std::string_view ViewToggleScenery = "view.transparent_scenery";
    constexpr std::string_view ViewToggleSupports = "view.hide_supports";
    constexpr std::string_view ViewTogglePeeps = "view.hide_peeps";
    constexpr std::string_view ViewToggleLandHeightMarkers = "view.show_land_height";
    constexpr std::string_view ViewToggleTrackHeightMarkers = "view.show_track_height";
    constexpr std::string_view ViewToggleFootpathHeightMarkers = "view.show_footpath_height";
    constexpr std::string_view ViewToggleFootpaths = "view.transparent_footpaths";
    constexpr std::string_view ViewToggleGridlines = "view.show_gridlines";
    constexpr std::string_view ViewToggleCutAway = "view.toggle_cut_away";
    constexpr std::string_view ViewToogleFootpathIssues = "view.highlight_path_issues";

    // Window / ride construction
    constexpr std::string_view WindowRideConstructionTurnLeft = "window.rideconstruction.turn_left";
    constexpr std::string_view WindowRideConstructionTurnRight = "window.rideconstruction.turn_right";
    constexpr std::string_view WindowRideConstructionDefault = "window.rideconstruction.default";
    constexpr std::string_view WindowRideConstructionSlopeDown = "window.rideconstruction.slope_down";
    constexpr std::string_view WindowRideConstructionSlopeUp = "window.rideconstruction.slope_up";
    constexpr std::string_view WindowRideConstructionChainLift = "window.rideconstruction.chain_lift";
    constexpr std::string_view WindowRideConstructionBankLeft = "window.rideconstruction.bank_left";
    constexpr std::string_view WindowRideConstructionBankRight = "window.rideconstruction.bank_right";
    constexpr std::string_view WindowRideConstructionPrevious = "window.rideconstruction.previous";
    constexpr std::string_view WindowRideConstructionNext = "window.rideconstruction.next";
    constexpr std::string_view WindowRideConstructionBuild = "window.rideconstruction.build";
    constexpr std::string_view WindowRideConstructionDemolish = "window.rideconstruction.demolish";

    // Window / tile inspector
    constexpr std::string_view WindowTileInspectorInsertCorrupt = "window.tileinspector.insert_corrupt";
    constexpr std::string_view WindowTileInspectorCopy = "window.tileinspector.copy";
    constexpr std::string_view WindowTileInspectorPaste = "window.tileinspector.paste";
    constexpr std::string_view WindowTileInspectorRemove = "window.tileinspector.remove";
    constexpr std::string_view WindowTileInspectorMoveUp = "window.tileinspector.move_up";
    constexpr std::string_view WindowTileInspectorMoveDown = "window.tileinspector.move_down";
    constexpr std::string_view WindowTileInspectorIncreaseX = "window.tileinspector.increase_x";
    constexpr std::string_view WindowTileInspectorDecreaseX = "window.tileinspector.decrease_x";
    constexpr std::string_view WindowTileInspectorIncreaseY = "window.tileinspector.increase_y";
    constexpr std::string_view WindowTileInspectorDecreaseY = "window.tileinspector.decrease_y";
    constexpr std::string_view WindowTileInspectorIncreaseHeight = "window.tileinspector.increase_height";
    constexpr std::string_view WindowTileInspectorDecreaseHeight = "window.tileinspector.decrease_height";

    // Debug
    constexpr std::string_view DebugToggleConsole = "debug.console";
    constexpr std::string_view DebugTogglePaintDebugWindow = "debug.toggle_paint_debug_window";
    constexpr std::string_view DebugAdvanceTick = "debug.advance_tick";
} // namespace OpenRCT2::Ui::ShortcutId
