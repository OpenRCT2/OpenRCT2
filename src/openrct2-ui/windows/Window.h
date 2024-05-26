/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2-ui/interface/Window.h>
#include <openrct2/Identifiers.h>
#include <openrct2/common.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/windows/TileInspectorGlobals.h>
#include <string_view>

// TODO: only for WINDOW_SHIM_RAW below; we can do better.
#include "../UiStringIds.h"

struct Peep;
struct TileElement;
struct Vehicle;
struct TrackDesign;
enum class GuestListFilterType : int32_t;
enum class ScatterToolDensity : uint8_t;
using loadsave_callback = void (*)(int32_t result, const utf8* path);
using scenarioselect_callback = void (*)(const utf8* path);

namespace OpenRCT2::Ui::Windows
{
    extern bool gWindowSceneryScatterEnabled;
    extern uint16_t gWindowSceneryScatterSize;
    extern ScatterToolDensity gWindowSceneryScatterDensity;
    extern uint8_t gWindowSceneryPaintEnabled;
    extern uint8_t gWindowSceneryRotation;
    extern colour_t gWindowSceneryPrimaryColour;
    extern colour_t gWindowScenerySecondaryColour;
    extern colour_t gWindowSceneryTertiaryColour;
    extern bool gWindowSceneryEyedropperEnabled;

    WindowBase* AboutOpen();
    void WindowCampaignRefreshRides();
    WindowBase* ChangelogOpen(int personality);
    WindowBase* CheatsOpen();
    WindowBase* ClearSceneryOpen();
    WindowBase* CustomCurrencyOpen();
    WindowBase* DebugPaintOpen();
    WindowBase* EditorInventionsListOpen();
    WindowBase* EditorMainOpen();
    WindowBase* EditorObjectiveOptionsOpen();
    WindowBase* EditorScenarioOptionsOpen();
    WindowBase* FootpathOpen();
    void WindowFootpathResetSelectedPath();
    WindowBase* GuestOpen(Peep* peep);
    WindowBase* LandOpen();
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
    WindowBase* ViewportOpen();
    WindowBase* WaterOpen();
    WindowBase* ViewClippingOpen();
    WindowBase* TransparencyOpen();
    WindowBase* AssetPacksOpen();

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
    WindowBase* ScenarioselectOpen(scenarioselect_callback callback);
    WindowBase* ScenarioselectOpen(std::function<void(std::string_view)> callback);

    WindowBase* ErrorOpen(StringId title, StringId message, const class Formatter& formatter, bool autoClose = false);
    WindowBase* ErrorOpen(std::string_view title, std::string_view message, bool autoClose = false);

    WindowBase* LoadsaveOpen(
        int32_t type, std::string_view defaultPath, std::function<void(int32_t result, std::string_view)> callback,
        TrackDesign* trackDesign);
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

    WindowBase* NetworkStatusOpen(const std::string& text, close_callback onClose);
    WindowBase* NetworkStatusOpenPassword();
    void WindowNetworkStatusClose();

    WindowBase* ProgressWindowOpen(const std::string& text, close_callback onClose = nullptr);
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

    WindowBase* TopToolbarOpen();
    bool LandToolIsActive();
    bool ClearSceneryToolIsActive();
    bool WaterToolIsActive();

    WindowBase* SceneryOpen();
    void WindowScenerySetSelectedItem(
        const ScenerySelection& sceneryconst, std::optional<colour_t> primary, const std::optional<colour_t> secondary,
        const std::optional<colour_t> tertiary, const std::optional<colour_t> rotation);
    void WindowScenerySetSelectedTab(const ObjectEntryIndex sceneryGroupIndex);
    void WindowScenerySetDefaultPlacementConfiguration();
    void WindowSceneryInit();
    void WindowSceneryResetSelectedSceneryItems();
    const ScenerySelection WindowSceneryGetTabSelection();

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

    // clang-format off
#define WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, CLOSE_STR) \
    { WindowWidgetType::Frame,    0,  0,          WIDTH - 1, 0, HEIGHT - 1, 0xFFFFFFFF,  STR_NONE }, \
    { WindowWidgetType::Caption,  0,  1,          WIDTH - 2, 1, 14,         TITLE,       STR_WINDOW_TITLE_TIP }, \
    { WindowWidgetType::CloseBox, 0,  WIDTH - 13, WIDTH - 3, 2, 13,         CLOSE_STR, STR_CLOSE_WINDOW_TIP }

#define WINDOW_SHIM(TITLE, WIDTH, HEIGHT) WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, STR_CLOSE_X)
#define WINDOW_SHIM_WHITE(TITLE, WIDTH, HEIGHT) WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, STR_CLOSE_X_WHITE)

    // clang-format on
} // namespace OpenRCT2::Ui::Windows
