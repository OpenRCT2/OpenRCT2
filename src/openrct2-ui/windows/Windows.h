/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/Identifiers.h>
#include <openrct2/interface/Window.h>
#include <openrct2/world/ScenerySelection.h>
#include <optional>
#include <string_view>

class Formatter;

struct ObjectEntryDescriptor;
struct OpenRCT2String;
struct Peep;
struct Ride;
struct RideSelection;
struct TileElement;
struct TrackDesign;
struct Vehicle;

enum class GuestListFilterType : int32_t;
enum class ScatterToolDensity : uint8_t;

using LoadSaveCallback = void (*)(int32_t result, const utf8* path);
using ScenarioSelectCallback = void (*)(const utf8* path);

namespace OpenRCT2::Ui::Windows
{
    extern bool gWindowSceneryScatterEnabled;
    extern uint16_t gWindowSceneryScatterSize;
    extern ScatterToolDensity gWindowSceneryScatterDensity;
    extern uint8_t gWindowSceneryRotation;
    extern bool gWindowSceneryEyedropperEnabled;
    extern bool gDisableErrorWindowSound;

    // About
    WindowBase* AboutOpen();

    // AssetPacks
    WindowBase* AssetPacksOpen();

    // Banner
    WindowBase* BannerOpen(rct_windownumber number);

    // Changelog
    WindowBase* ChangelogOpen(int personality);

    // Cheats
    WindowBase* CheatsOpen();

    // ClearScenery
    WindowBase* ClearSceneryOpen();
    void ToggleClearSceneryWindow();

    // CustomCurrency
    WindowBase* CustomCurrencyOpen();

    // DebugPaint
    WindowBase* DebugPaintOpen();

    // DemolishRidePrompt
    WindowBase* RideDemolishPromptOpen(const Ride& ride);

    // EditorInventionsList
    WindowBase* EditorInventionsListOpen();

    // EditorBottomToolbar
    WindowBase* EditorBottomToolbarOpen();

    // EditorObjectiveOptions
    WindowBase* EditorObjectiveOptionsOpen();

    // EditorObjectSelection
    WindowBase* EditorObjectSelectionOpen();
    bool EditorObjectSelectionWindowCheck();

    // EditorParkEntrance
    WindowBase* EditorParkEntranceOpen();

    // EditorScenarioOptions
    WindowBase* EditorScenarioOptionsOpen();

    // Error
    WindowBase* ErrorOpen(StringId title, StringId message, const class Formatter& formatter, bool autoClose = false);
    WindowBase* ErrorOpen(std::string_view title, std::string_view message, bool autoClose = false);

    // Finances
    WindowBase* FinancesOpen();
    WindowBase* FinancesResearchOpen();
    WindowBase* FinancesMarketingOpen();

    // Footpath
    WindowBase* FootpathOpen();
    void WindowFootpathResetSelectedPath();
    void ToggleFootpathWindow();
    void WindowFootpathKeyboardShortcutTurnLeft();
    void WindowFootpathKeyboardShortcutTurnRight();
    void WindowFootpathKeyboardShortcutSlopeDown();
    void WindowFootpathKeyboardShortcutSlopeUp();
    void WindowFootpathKeyboardShortcutBuildCurrent();
    void WindowFootpathKeyboardShortcutDemolishCurrent();

    // GameBottomToolbar
    extern uint8_t gToolbarDirtyFlags;
    WindowBase* GameBottomToolbarOpen();
    void WindowGameBottomToolbarInvalidateNewsItem();

    // Guest
    WindowBase* GuestOpen(Peep* peep);

    // GuestList
    WindowBase* GuestListOpen();
    WindowBase* GuestListOpenWithFilter(GuestListFilterType type, int32_t index);
    void WindowGuestListRefreshList();

    // InstallTrack
    WindowBase* InstallTrackOpen(const utf8* path);

    // Land
    WindowBase* LandOpen();
    void ToggleLandWindow();

    // LandRights
    WindowBase* LandRightsOpen();

    // LoadSave
    WindowBase* LoadsaveOpen(
        int32_t type, std::string_view defaultPath, std::function<void(int32_t result, std::string_view)> callback,
        TrackDesign* trackDesign);
    void WindowLoadSaveInputKey(WindowBase* w, uint32_t keycode);
    void WindowLoadSaveOverwritePromptInputKey(WindowBase* w, uint32_t keycode);

    // Main
    WindowBase* MainOpen();

    // Map
    WindowBase* MapOpen();
    void WindowMapReset();

    // MapGen
    WindowBase* MapgenOpen();

    // MapTooltip
    void SetMapTooltip(Formatter& ft);
    const Formatter& GetMapTooltip();
    void WindowMapTooltipUpdateVisibility();

    // MazeConstruction
    WindowBase* MazeConstructionOpen();
    void WindowMazeConstructionUpdatePressedWidgets();

    // Multiplatyer
    WindowBase* MultiplayerOpen();

    // NewCampaign
    WindowBase* NewCampaignOpen(int16_t campaignType);
    void WindowCampaignRefreshRides();

    // NewRide
    WindowBase* NewRideOpen();
    WindowBase* NewRideOpenResearch();
    void WindowNewRideInitVars();
    void WindowNewRideFocus(RideSelection rideItem);

    // News
    WindowBase* NewsOpen();

    // NewsOptions
    WindowBase* NewsOptionsOpen();

    // NetworkStatus
    WindowBase* NetworkStatusOpen(const std::string& text, CloseCallback onClose);
    WindowBase* NetworkStatusOpenPassword();
    void WindowNetworkStatusClose();

    // ObjectLoadError
    WindowBase* ObjectLoadErrorOpen(utf8* path, size_t numMissingObjects, const ObjectEntryDescriptor* missingObjects);

    // Options
    WindowBase* OptionsOpen();

    // Park
    WindowBase* ParkAwardsOpen();
    WindowBase* ParkEntranceOpen();
    WindowBase* ParkGuestsOpen();
    WindowBase* ParkObjectiveOpen();
    WindowBase* ParkRatingOpen();

    // Player
    WindowBase* PlayerOpen(uint8_t id);

    // ProgressWindow
    WindowBase* ProgressWindowOpen(const std::string& text, CloseCallback onClose = nullptr);
    void ProgressWindowSet(uint32_t currentProgress, uint32_t totalCount, StringId format = kStringIdNone);
    void ProgressWindowClose();

    // PatrolArea
    WindowBase* PatrolAreaOpen(EntityId staffId);
    EntityId WindowPatrolAreaGetCurrentStaffId();

    // RefurbishRidePrompt
    WindowBase* RideRefurbishPromptOpen(const Ride& ride);

    // Research
    WindowBase* ResearchOpen();
    void WindowResearchDevelopmentMouseUp(WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex);
    void WindowResearchDevelopmentPrepareDraw(WindowBase* w, WidgetIndex baseWidgetIndex);
    void WindowResearchDevelopmentDraw(WindowBase* w, DrawPixelInfo& dpi, WidgetIndex baseWidgetIndex);
    void WindowResearchFundingMouseDown(WindowBase* w, WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex);
    void WindowResearchFundingMouseUp(WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex);
    void WindowResearchFundingDropdown(WidgetIndex widgetIndex, int32_t selectedIndex, WidgetIndex baseWidgetIndex);
    void WindowResearchFundingPrepareDraw(WindowBase* w, WidgetIndex baseWidgetIndex);
    void WindowResearchFundingDraw(WindowBase* w, DrawPixelInfo& dpi);

    // Ride
    WindowBase* RideMainOpen(const Ride& ride);
    WindowBase* RideOpenTrack(TileElement* tileElement);
    WindowBase* RideOpenVehicle(Vehicle* vehicle);
    void WindowRideInvalidateVehicle(const Vehicle& vehicle);
    void WindowRidePaintResetVehicle(RideId rideIndex);
    void WindowRideMeasurementsDesignCancel();

    // RideConstruction
    WindowBase* RideConstructionOpen();
    void WindowRideConstructionUpdateActiveElementsImpl();
    void WindowRideConstructionUpdateEnabledTrackPieces();
    void RideRestoreProvisionalTrackPiece();
    void RideRemoveProvisionalTrackPiece();
    void RideConstructionToolupdateEntranceExit(const ScreenCoordsXY& screenCoords);
    void RideConstructionToolupdateConstruct(const ScreenCoordsXY& screenCoords);
    void RideConstructionTooldownConstruct(const ScreenCoordsXY& screenCoords);
    void UpdateGhostTrackAndArrow();
    void WindowRideConstructionKeyboardShortcutTurnLeft();
    void WindowRideConstructionKeyboardShortcutTurnRight();
    void WindowRideConstructionKeyboardShortcutUseTrackDefault();
    void WindowRideConstructionKeyboardShortcutSlopeDown();
    void WindowRideConstructionKeyboardShortcutSlopeUp();
    void WindowRideConstructionKeyboardShortcutChainLiftToggle();
    void WindowRideConstructionKeyboardShortcutBankLeft();
    void WindowRideConstructionKeyboardShortcutBankRight();
    void WindowRideConstructionKeyboardShortcutPreviousTrack();
    void WindowRideConstructionKeyboardShortcutNextTrack();
    void WindowRideConstructionKeyboardShortcutBuildCurrent();
    void WindowRideConstructionKeyboardShortcutDemolishCurrent();

    // RideList
    WindowBase* RideListOpen();
    void WindowRideListRefreshList(WindowBase* w);

    // SavePrompt
    WindowBase* SavePromptOpen();

    // ScenarioSelect
    WindowBase* ScenarioselectOpen(ScenarioSelectCallback callback);
    WindowBase* ScenarioselectOpen(std::function<void(std::string_view)> callback);

    // Scenery
    WindowBase* SceneryOpen();
    void WindowScenerySetSelectedItem(
        const ScenerySelection& sceneryconst, std::optional<colour_t> primary, const std::optional<colour_t> secondary,
        const std::optional<colour_t> tertiary, const std::optional<colour_t> rotation);
    void WindowScenerySetSelectedTab(const ObjectEntryIndex sceneryGroupIndex);
    void WindowScenerySetDefaultPlacementConfiguration();
    void WindowSceneryInit();
    void WindowSceneryResetSelectedSceneryItems();
    const ScenerySelection WindowSceneryGetTabSelection();
    void ToggleSceneryWindow();

    // SceneryScatter
    WindowBase* SceneryScatterOpen();

#ifndef DISABLE_NETWORK
    // ServerList
    WindowBase* ServerListOpen();

    // ServerStart
    WindowBase* ServerStartOpen();
#endif

    // ShortcutKeys
    WindowBase* ShortcutKeysOpen();

    // Sign
    WindowBase* SignOpen(rct_windownumber number);
    WindowBase* SignSmallOpen(rct_windownumber number);

    // Staff
    WindowBase* StaffOpen(Peep* peep);

    // StaffFirePrompt
    WindowBase* StaffFirePromptOpen(Peep* peep);

    // StaffList
    WindowBase* StaffListOpen();
    void WindowStaffListRefresh();

    // TextInput
    void WindowTextInputKey(WindowBase* w, uint32_t keycode);
    void WindowTextInputOpen(
        WindowBase* call_w, WidgetIndex call_widget, StringId title, StringId description, const Formatter& descriptionArgs,
        StringId existing_text, uintptr_t existing_args, int32_t maxLength);
    void WindowTextInputRawOpen(
        WindowBase* call_w, WidgetIndex call_widget, StringId title, StringId description, const Formatter& descriptionArgs,
        const_utf8string existing_text, int32_t maxLength);
    void WindowTextInputOpen(
        std::string_view title, std::string_view description, std::string_view initialValue, size_t maxLength,
        std::function<void(std::string_view)> okCallback, std::function<void()> cancelCallback);

    // Themes
    WindowBase* ThemesOpen();

    // TileInspector
    WindowBase* TileInspectorOpen();
    void WindowTileInspectorClearClipboard();
    void WindowTileInspectorKeyboardShortcutToggleInvisibility();

    // TitleExit
    WindowBase* TitleExitOpen();

    // TitleLogo
    WindowBase* TitleLogoOpen();

    // TitleMenu
    WindowBase* TitleMenuOpen();

    // TitleOptions
    WindowBase* TitleOptionsOpen();

    // TitleVersion
    WindowBase* TitleVersionOpen();

    // Tooltip
    void WindowTooltipReset(const ScreenCoordsXY& screenCoords);
    void WindowTooltipShow(const OpenRCT2String& message, ScreenCoordsXY screenCoords);
    void WindowTooltipOpen(WindowBase* widgetWindow, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
    void WindowTooltipClose();

    // TopToolbar
    WindowBase* TopToolbarOpen();

    // TrackDesignPlace
    WindowBase* TrackPlaceOpen(const struct TrackDesignFileRef* tdFileRef);
    void TrackPlaceClearProvisionalTemporarily();
    void TrackPlaceRestoreProvisional();

    // TrackDesignManage
    WindowBase* TrackManageOpen(struct TrackDesignFileRef* tdFileRef);

    // TrackList
    // rct2: 0x00F635EE
    extern RideSelection _window_track_list_item;
    WindowBase* TrackListOpen(RideSelection item);
    void WindowTrackDesignListReloadTracks();
    void WindowTrackDesignListSetBeingUpdated(bool beingUpdated);

    // Transparency
    WindowBase* TransparencyOpen();

    // ViewClipping
    WindowBase* ViewClippingOpen();

    // Viewport
    WindowBase* ViewportOpen();

    // Water
    WindowBase* WaterOpen();
    void ToggleWaterWindow();
} // namespace OpenRCT2::Ui::Windows
