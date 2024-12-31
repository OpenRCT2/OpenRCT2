/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2-ui/interface/Window.h>
#include <openrct2/Identifiers.h>
#include <string_view>

struct ObjectEntryDescriptor;
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

    WindowBase* AboutOpen();
    void WindowCampaignRefreshRides();
    WindowBase* ChangelogOpen(int personality);
    WindowBase* CheatsOpen();

    WindowBase* ClearSceneryOpen();
    void ToggleClearSceneryWindow();

    WindowBase* CustomCurrencyOpen();
    WindowBase* DebugPaintOpen();
    WindowBase* EditorInventionsListOpen();
    WindowBase* EditorMainOpen();
    WindowBase* EditorObjectiveOptionsOpen();
    WindowBase* EditorScenarioOptionsOpen();

    WindowBase* FootpathOpen();
    void WindowFootpathResetSelectedPath();
    void ToggleFootpathWindow();

    WindowBase* GuestOpen(Peep* peep);

    WindowBase* LandOpen();
    void ToggleLandWindow();

    WindowBase* LandRightsOpen();

    WindowBase* MainOpen();
    WindowBase* MapgenOpen();
    WindowBase* MultiplayerOpen();
    WindowBase* NewsOpen();
    WindowBase* NewsOptionsOpen();
    WindowBase* OptionsOpen();
    WindowBase* SavePromptOpen();
#ifndef DISABLE_NETWORK
    WindowBase* ServerListOpen();
    WindowBase* ServerStartOpen();
#endif
    WindowBase* ShortcutKeysOpen();
    WindowBase* StaffListOpen();
    WindowBase* StaffOpen(Peep* peep);
    void WindowStaffListRefresh();
    WindowBase* ThemesOpen();
    WindowBase* TitleExitOpen();
    WindowBase* TitleLogoOpen();
    WindowBase* TitleMenuOpen();
    WindowBase* TitleOptionsOpen();
    WindowBase* TitleVersionOpen();
    WindowBase* ViewportOpen();

    WindowBase* WaterOpen();
    void ToggleWaterWindow();

    WindowBase* ViewClippingOpen();
    WindowBase* TransparencyOpen();
    WindowBase* AssetPacksOpen();
    WindowBase* EditorParkEntranceOpen();

    // WC_FINANCES
    WindowBase* FinancesOpen();
    WindowBase* FinancesResearchOpen();
    WindowBase* FinancesMarketingOpen();

    // WC_PARK_INFORMATION
    WindowBase* ParkAwardsOpen();
    WindowBase* ParkEntranceOpen();
    WindowBase* ParkGuestsOpen();
    WindowBase* ParkObjectiveOpen();
    WindowBase* ParkRatingOpen();

    WindowBase* BannerOpen(rct_windownumber number);
    WindowBase* RideDemolishPromptOpen(const Ride& ride);
    WindowBase* RideRefurbishPromptOpen(const Ride& ride);
    WindowBase* SignOpen(rct_windownumber number);
    WindowBase* SignSmallOpen(rct_windownumber number);
    WindowBase* PlayerOpen(uint8_t id);
    WindowBase* NewCampaignOpen(int16_t campaignType);

    WindowBase* InstallTrackOpen(const utf8* path);
    void WindowGuestListRefreshList();
    WindowBase* GuestListOpen();
    WindowBase* GuestListOpenWithFilter(GuestListFilterType type, int32_t index);
    WindowBase* StaffFirePromptOpen(Peep* peep);
    WindowBase* ScenarioselectOpen(ScenarioSelectCallback callback);
    WindowBase* ScenarioselectOpen(std::function<void(std::string_view)> callback);

    WindowBase* ErrorOpen(StringId title, StringId message, const class Formatter& formatter, bool autoClose = false);
    WindowBase* ErrorOpen(std::string_view title, std::string_view message, bool autoClose = false);

    WindowBase* LoadsaveOpen(
        int32_t type, std::string_view defaultPath, std::function<void(int32_t result, std::string_view)> callback,
        TrackDesign* trackDesign);
    void WindowLoadSaveInputKey(WindowBase* w, uint32_t keycode);
    void WindowLoadSaveOverwritePromptInputKey(WindowBase* w, uint32_t keycode);

    WindowBase* TrackPlaceOpen(const struct TrackDesignFileRef* tdFileRef);
    WindowBase* TrackManageOpen(struct TrackDesignFileRef* tdFileRef);

    void TrackPlaceClearProvisionalTemporarily();
    void TrackPlaceRestoreProvisional();

    WindowBase* MapOpen();
    void WindowMapReset();

    WindowBase* ResearchOpen();
    void WindowResearchDevelopmentMouseUp(WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex);
    void WindowResearchDevelopmentPrepareDraw(WindowBase* w, WidgetIndex baseWidgetIndex);
    void WindowResearchDevelopmentDraw(WindowBase* w, DrawPixelInfo& dpi, WidgetIndex baseWidgetIndex);
    void WindowResearchFundingMouseDown(WindowBase* w, WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex);
    void WindowResearchFundingMouseUp(WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex);
    void WindowResearchFundingDropdown(WidgetIndex widgetIndex, int32_t selectedIndex, WidgetIndex baseWidgetIndex);
    void WindowResearchFundingPrepareDraw(WindowBase* w, WidgetIndex baseWidgetIndex);
    void WindowResearchFundingDraw(WindowBase* w, DrawPixelInfo& dpi);

    WindowBase* NewRideOpen();
    WindowBase* NewRideOpenResearch();
    void WindowNewRideInitVars();
    void WindowNewRideFocus(RideSelection rideItem);

    WindowBase* RideListOpen();
    void WindowRideListRefreshList(WindowBase* w);

    WindowBase* RideMainOpen(const Ride& ride);
    WindowBase* RideOpenTrack(TileElement* tileElement);
    WindowBase* RideOpenVehicle(Vehicle* vehicle);
    void WindowRideInvalidateVehicle(const Vehicle& vehicle);
    void WindowRidePaintResetVehicle(RideId rideIndex);
    void WindowRideMeasurementsDesignCancel();

    // rct2: 0x00F635EE
    extern RideSelection _window_track_list_item;
    WindowBase* TrackListOpen(RideSelection item);
    void WindowTrackDesignListReloadTracks();
    void WindowTrackDesignListSetBeingUpdated(bool beingUpdated);

    void SetMapTooltip(Formatter& ft);
    const Formatter& GetMapTooltip();
    void WindowMapTooltipUpdateVisibility();

    WindowBase* MazeConstructionOpen();
    void WindowMazeConstructionUpdatePressedWidgets();

    WindowBase* NetworkStatusOpen(const std::string& text, CloseCallback onClose);
    WindowBase* NetworkStatusOpenPassword();
    void WindowNetworkStatusClose();

    WindowBase* ProgressWindowOpen(const std::string& text, CloseCallback onClose = nullptr);
    void ProgressWindowSet(uint32_t currentProgress, uint32_t totalCount, StringId format = STR_NONE);
    void ProgressWindowClose();

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

    WindowBase* ObjectLoadErrorOpen(utf8* path, size_t numMissingObjects, const ObjectEntryDescriptor* missingObjects);

    WindowBase* RideConstructionOpen();
    void WindowRideConstructionUpdateActiveElementsImpl();
    void WindowRideConstructionUpdateEnabledTrackPieces();
    void RideRestoreProvisionalTrackPiece();
    void RideRemoveProvisionalTrackPiece();

    WindowBase* TopToolbarOpen();

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

    extern uint8_t gToolbarDirtyFlags;
    WindowBase* GameBottomToolbarOpen();
    void WindowGameBottomToolbarInvalidateNewsItem();

    WindowBase* EditorBottomToolbarOpen();

    WindowBase* TileInspectorOpen();
    void WindowTileInspectorClearClipboard();

    WindowBase* EditorObjectSelectionOpen();
    bool EditorObjectSelectionWindowCheck();

    void WindowTooltipReset(const ScreenCoordsXY& screenCoords);
    void WindowTooltipShow(const OpenRCT2String& message, ScreenCoordsXY screenCoords);
    void WindowTooltipOpen(WindowBase* widgetWindow, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
    void WindowTooltipClose();

    WindowBase* SceneryScatterOpen();
    WindowBase* PatrolAreaOpen(EntityId staffId);
    EntityId WindowPatrolAreaGetCurrentStaffId();
} // namespace OpenRCT2::Ui::Windows
