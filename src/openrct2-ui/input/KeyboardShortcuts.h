/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <openrct2/common.h>

#define SHIFT 0x100
#define CTRL 0x200
#define ALT 0x400
#define CMD 0x800
#ifdef __MACOSX__
#    define PLATFORM_MODIFIER CMD
#else
#    define PLATFORM_MODIFIER CTRL
#endif

struct ScreenCoordsXY;

#include <string>

namespace OpenRCT2
{
    struct IPlatformEnvironment;

    namespace Input
    {
        enum class Shortcut : size_t
        {
            CloseTopMostWindow,
            CloseAllFloatingWindows,
            CancelConstructionMode,
            PauseGame,
            ZoomViewOut,
            ZoomViewIn,
            RotateViewClockwise,
            RotateViewAnticlockwise,
            RotateConstructionObject,
            UndergroundViewToggle,
            RemoveBaseLandToggle,
            RemoveVerticalLandToggle,
            SeeThroughRidesToggle,
            SeeThroughSceneryToggle,
            InvisibleSupportsToggle,
            InvisiblePeopleToggle,
            HeightMarksOnLandToggle,
            HeightMarksOnRideTracksToggle,
            HeightMarksOnPathsToggle,
            AdjustLand,
            AdjustWater,
            BuildScenery,
            BuildPaths,
            BuildNewRide,
            ShowFinancialInformation,
            ShowResearchInformation,
            ShowRidesList,
            ShowParkInformation,
            ShowGuestList,
            ShowStaffList,
            ShowRecentMessages,
            ShowMap,
            Screenshot,

            // New
            ReduceGameSpeed,
            IncreaseGameSpeed,
            OpenCheatWindow,
            RemoveTopBottomToolbarToggle,
            ScrollMapUp,
            ScrollMapLeft,
            ScrollMapDown,
            ScrollMapRight,
            OpenChatWindow,
            QuickSaveGame,
            ShowOptions,
            MuteSound,
            WindowedModeToggle,
            ShowMultiplayer,
            PaintOriginalToggle,
            DebugPaintToggle,
            SeeThroughPathsToggle,
            RideConstructionTurnLeft,
            RideConstructionTurnRight,
            RideConstructionUseTrackDefault,
            RideConstructionSlopeDown,
            RideConstructionSlopeUp,
            RideConstructionChainLiftToggle,
            RideConstructionBankLeft,
            RideConstructionBankRight,
            RideConstructionPreviousTrack,
            RideConstructionNextTrack,
            RideConstructionBuildCurrent,
            RideConstructionDemolishCurrent,
            LoadGame,
            ClearScenery,
            GridlinesDisplayToggle,
            ViewClipping,
            HighlightPathIssuesToggle,
            TileInspector,
            AdvanceToNextTick,
            SceneryPicker,
            ScaleUp,
            ScaleDown,
            ToggleInvisibility,
            CopyElement,
            PasteElement,
            RemoveElement,
            MoveElementUp,
            MoveElementDown,
            IncreaseXCoord,
            DecreaseXCoord,
            IncreaseYCoord,
            DecreaseYCoord,
            IncreaseElementHeight,
            DecreaseElementHeight,
            ToggleClearanceChecks,

            Count,

            Undefined = 0xFFFF,
        };
        constexpr size_t ShortcutsCount = static_cast<size_t>(Shortcut::Count);

        class KeyboardShortcuts
        {
        private:
            constexpr static int32_t CURRENT_FILE_VERSION = 1;
            static const uint16_t DefaultKeys[ShortcutsCount];

            std::shared_ptr<IPlatformEnvironment> const _env;
            uint16_t _keys[ShortcutsCount];

        public:
            KeyboardShortcuts(const std::shared_ptr<IPlatformEnvironment>& env);
            ~KeyboardShortcuts();

            void Reset();
            bool Load();
            bool Save();

            std::string GetShortcutString(int32_t shortcut) const;

            void Set(int32_t key);
            Shortcut GetFromKey(int32_t key);
            ScreenCoordsXY GetKeyboardMapScroll(const uint8_t* keysState) const;
        };
        const uint16_t ScanCodeUndefined = 0xFFFF;
    } // namespace Input
} // namespace OpenRCT2

// The current shortcut being changed.
extern OpenRCT2::Input::Shortcut gKeyboardShortcutChangeId;

void KeyboardShortcutsReset();
bool KeyboardShortcutsLoad();
bool KeyboardShortcutsSave();
void KeyboardShortcutsSet(int32_t key);
OpenRCT2::Input::Shortcut KeyboardShortcutsGetFromKey(int32_t key);
void KeyboardShortcutsFormatString(char* buffer, size_t bufferSize, int32_t shortcut);

void KeyboardShortcutHandle(int32_t key);
void KeyboardShortcutHandleCommand(OpenRCT2::Input::Shortcut shortcut);

ScreenCoordsXY GetKeyboardMapScroll(const uint8_t* keysState);
