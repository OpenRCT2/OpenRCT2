/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "InputManager.h"

#include <cstdint>
#include <functional>
#include <openrct2/core/FileSystem.hpp>
#include <openrct2/localisation/StringIds.h>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace OpenRCT2
{
    struct IPlatformEnvironment;
}

namespace OpenRCT2::Ui
{
    struct ShortcutInput
    {
    public:
        InputDeviceKind Kind{};
        uint32_t Modifiers{};
        uint32_t Button{};

        ShortcutInput() = default;
        ShortcutInput(const std::string_view& value);
        std::string ToString() const;

        bool Matches(const InputEvent& e) const;

        static std::optional<ShortcutInput> FromInputEvent(const InputEvent& e);

    private:
        bool AppendModifier(std::string& s, const std::string_view& text, uint32_t left, uint32_t right) const;
    };

    class RegisteredShortcut
    {
    public:
        std::string Id;
        rct_string_id LocalisedName = STR_NONE;
        std::string CustomName;
        std::vector<ShortcutInput> Default;
        std::vector<ShortcutInput> Current;
        std::function<void()> Action;

        RegisteredShortcut() = default;
        RegisteredShortcut(const std::string_view& id, std::string_view name, const std::function<void()>& action)
            : Id(id)
            , CustomName(name)
            , Action(action)
        {
        }

        RegisteredShortcut(const std::string_view& id, rct_string_id localisedName, const std::function<void()>& action)
            : Id(id)
            , LocalisedName(localisedName)
            , Action(action)
        {
        }

        RegisteredShortcut(
            const std::string_view& id, rct_string_id localisedName, const std::string_view& defaultChord,
            const std::function<void()>& action)
            : Id(id)
            , LocalisedName(localisedName)
            , Default({ defaultChord })
            , Current(Default)
            , Action(action)
        {
        }

        RegisteredShortcut(
            const std::string_view& id, rct_string_id localisedName, const std::string_view& defaultChordA,
            const std::string_view& defaultChordB, const std::function<void()>& action)
            : Id(id)
            , LocalisedName(localisedName)
            , Default({ defaultChordA, defaultChordB })
            , Current(Default)
            , Action(action)
        {
        }

        std::string_view GetTopLevelGroup() const;
        std::string_view GetGroup() const;
        bool Matches(const InputEvent& e) const;
        bool IsSuitableInputEvent(const InputEvent& e) const;

    private:
    };

    class ShortcutManager
    {
    private:
        std::shared_ptr<IPlatformEnvironment> _env;
        std::string _pendingShortcutChange;

        static std::optional<ShortcutInput> ConvertLegacyBinding(uint16_t binding);
        void LoadLegacyBindings(const fs::path& path);
        void LoadUserBindings(const fs::path& path);
        void SaveUserBindings(const fs::path& path);

    public:
        std::vector<RegisteredShortcut> Shortcuts;

        ShortcutManager(const std::shared_ptr<IPlatformEnvironment>& env);
        ShortcutManager(const ShortcutManager&) = delete;

        void LoadUserBindings();
        void SaveUserBindings();

        void RegisterShortcut(RegisteredShortcut&& shortcut);
        template<typename... Args> void RegisterShortcut(Args&&... args)
        {
            RegisterShortcut(RegisteredShortcut(std::forward<Args>(args)...));
        }
        void RegisterDefaultShortcuts();
        RegisteredShortcut* GetShortcut(std::string_view id);
        void RemoveShortcut(std::string_view id);
        void SetPendingShortcutChange(std::string_view id);
        void ProcessEvent(const InputEvent& e);
        bool ProcessEventForSpecificShortcut(const InputEvent& e, std::string_view id);

        static std::string_view GetLegacyShortcutId(size_t index);
    };

    ShortcutManager& GetShortcutManager();

    namespace ShortcutId
    {
        // Interface / general
        constexpr const char* InterfaceCloseTop = "interface.general.close_top";
        constexpr const char* InterfaceCloseAll = "interface.general.close_all";
        constexpr const char* InterfaceCancelConstruction = "interface.general.cancel_construction";
        constexpr const char* InterfacePause = "interface.general.pause";
        constexpr const char* InterfaceZoomOut = "interface.general.zoom_out";
        constexpr const char* InterfaceZoomIn = "interface.general.zoom_in";
        constexpr const char* InterfaceRotateClockwise = "interface.general.rotate_clockwise";
        constexpr const char* InterfaceRotateAnticlockwise = "interface.general.rotate_anticlockwise";
        constexpr const char* InterfaceRotateConstruction = "interface.general.rotate_construction";
        constexpr const char* InterfaceDecreaseSpeed = "interface.misc.decrease_speed";
        constexpr const char* InterfaceIncreaseSpeed = "interface.misc.increase_speed";
        constexpr const char* InterfaceToggleToolbars = "interface.misc.toggle_toolbars";
        constexpr const char* InterfaceScreenshot = "interface.misc.screenshot";
        constexpr const char* InterfaceLoadGame = "interface.misc.load_game";
        constexpr const char* InterfaceSaveGame = "interface.misc.save_game";
        constexpr const char* InterfaceMute = "interface.misc.mute";
        constexpr const char* InterfaceSceneryPicker = "interface.misc.scenery_picker";
        constexpr const char* InterfaceDisableClearance = "interface.misc.disable_clearance";
        constexpr const char* MultiplayerChat = "interface.misc.multiplayer_chat";

        // Interface / scroll
        constexpr const char* ScrollUp = "interface.scroll.up";
        constexpr const char* ScrollLeft = "interface.scroll.left";
        constexpr const char* ScrollRight = "interface.scroll.right";
        constexpr const char* ScrollDown = "interface.scroll.down";

        // Interface / graphics
        constexpr const char* ScaleToggleWindowMode = "interface.graphics.toggle_window_mode";
        constexpr const char* InterfaceScaleIncrease = "interface.graphics.increase";
        constexpr const char* InterfaceScaleDecrease = "interface.graphics.decrease";

        // Interface / open
        constexpr const char* InterfaceOpenLand = "interface.open.land";
        constexpr const char* InterfaceOpenWater = "interface.open.water";
        constexpr const char* InterfaceClearScenery = "interface.open.clear_scenery";
        constexpr const char* InterfaceOpenScenery = "interface.open.scenery";
        constexpr const char* InterfaceOpenFootpaths = "interface.open.footpaths";
        constexpr const char* InterfaceOpenNewRide = "interface.open.new_ride";
        constexpr const char* InterfaceOpenFinances = "interface.open.finances";
        constexpr const char* InterfaceOpenResearch = "interface.open.research";
        constexpr const char* InterfaceOpenRides = "interface.open.rides";
        constexpr const char* InterfaceOpenPark = "interface.open.park";
        constexpr const char* InterfaceOpenGuests = "interface.open.guests";
        constexpr const char* InterfaceOpenStaff = "interface.open.staff";
        constexpr const char* InterfaceOpenMessages = "interface.open.messages";
        constexpr const char* InterfaceOpenMap = "interface.open.map";
        constexpr const char* InterfaceShowOptions = "interface.open.options";
        constexpr const char* InterfaceOpenCheats = "interface.open.cheats";
        constexpr const char* InterfaceOpenTileInspector = "interface.open.tileinspector";
        constexpr const char* MultiplayerShow = "interface.open.multiplayer";

        // View
        constexpr const char* ViewToggleUnderground = "view.show_underground";
        constexpr const char* ViewToggleBaseLand = "view.hide_base_land";
        constexpr const char* ViewToggleVerticalLand = "view.hide_vertical_land";
        constexpr const char* ViewToggleRides = "view.transparent_rides";
        constexpr const char* ViewToggleScenery = "view.transparent_scenery";
        constexpr const char* ViewToggleSupports = "view.hide_supports";
        constexpr const char* ViewTogglePeeps = "view.hide_peeps";
        constexpr const char* ViewToggleLandHeightMarkers = "view.show_land_height";
        constexpr const char* ViewToggleTrackHeightMarkers = "view.show_track_height";
        constexpr const char* ViewToggleFootpathHeightMarkers = "view.show_footpath_height";
        constexpr const char* ViewToggleFootpaths = "view.transparent_footpaths";
        constexpr const char* ViewToggleGridlines = "view.show_gridlines";
        constexpr const char* ViewToggleCutAway = "view.toggle_cut_away";
        constexpr const char* ViewToogleFootpathIssues = "view.highlight_path_issues";

        // Window / ride construction
        constexpr const char* WindowRideConstructionTurnLeft = "window.rideconstruction.turn_left";
        constexpr const char* WindowRideConstructionTurnRight = "window.rideconstruction.turn_right";
        constexpr const char* WindowRideConstructionDefault = "window.rideconstruction.default";
        constexpr const char* WindowRideConstructionSlopeDown = "window.rideconstruction.slope_down";
        constexpr const char* WindowRideConstructionSlopeUp = "window.rideconstruction.slope_up";
        constexpr const char* WindowRideConstructionChainLift = "window.rideconstruction.chain_lift";
        constexpr const char* WindowRideConstructionBankLeft = "window.rideconstruction.bank_left";
        constexpr const char* WindowRideConstructionBankRight = "window.rideconstruction.bank_right";
        constexpr const char* WindowRideConstructionPrevious = "window.rideconstruction.previous";
        constexpr const char* WindowRideConstructionNext = "window.rideconstruction.next";
        constexpr const char* WindowRideConstructionBuild = "window.rideconstruction.build";
        constexpr const char* WindowRideConstructionDemolish = "window.rideconstruction.demolish";

        // Window / tile inspector
        constexpr const char* WindowTileInspectorInsertCorrupt = "window.tileinspector.insert_corrupt";
        constexpr const char* WindowTileInspectorCopy = "window.tileinspector.copy";
        constexpr const char* WindowTileInspectorPaste = "window.tileinspector.paste";
        constexpr const char* WindowTileInspectorRemove = "window.tileinspector.remove";
        constexpr const char* WindowTileInspectorMoveUp = "window.tileinspector.move_up";
        constexpr const char* WindowTileInspectorMoveDown = "window.tileinspector.move_down";
        constexpr const char* WindowTileInspectorIncreaseX = "window.tileinspector.increase_x";
        constexpr const char* WindowTileInspectorDecreaseX = "window.tileinspector.decrease_x";
        constexpr const char* WindowTileInspectorIncreaseY = "window.tileinspector.increase_y";
        constexpr const char* WindowTileInspectorDecreaseY = "window.tileinspector.decrease_y";
        constexpr const char* WindowTileInspectorIncreaseHeight = "window.tileinspector.increase_height";
        constexpr const char* WindowTileInspectorDecreaseHeight = "window.tileinspector.decrease_height";

        // Debug
        constexpr const char* DebugToggleConsole = "debug.console";
        constexpr const char* DebugTogglePaintDebugWindow = "debug.toggle_paint_debug_window";
        constexpr const char* DebugAdvanceTick = "debug.advance_tick";
    } // namespace ShortcutId

} // namespace OpenRCT2::Ui
