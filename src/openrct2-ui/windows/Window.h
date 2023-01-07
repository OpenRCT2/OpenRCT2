/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include <openrct2/windows/tile_inspector.h>
#include <string_view>

using loadsave_callback = void (*)(int32_t result, const utf8* path);
using scenarioselect_callback = void (*)(const utf8* path);
struct Peep;
struct TileElement;
struct Vehicle;
enum class GuestListFilterType : int32_t;
enum class ScatterToolDensity : uint8_t;

extern bool gWindowSceneryScatterEnabled;
extern uint16_t gWindowSceneryScatterSize;
extern ScatterToolDensity gWindowSceneryScatterDensity;
extern uint8_t gWindowSceneryPaintEnabled;
extern uint8_t gWindowSceneryRotation;
extern colour_t gWindowSceneryPrimaryColour;
extern colour_t gWindowScenerySecondaryColour;
extern colour_t gWindowSceneryTertiaryColour;
extern bool gWindowSceneryEyedropperEnabled;

rct_window* WindowAboutOpen();
void WindowCampaignRefreshRides();
rct_window* WindowChangelogOpen(int personality);
rct_window* WindowCheatsOpen();
rct_window* WindowClearSceneryOpen();
rct_window* CustomCurrencyWindowOpen();
rct_window* WindowDebugPaintOpen();
rct_window* WindowEditorInventionsListOpen();
rct_window* WindowEditorMainOpen();
rct_window* WindowEditorObjectiveOptionsOpen();
rct_window* WindowEditorScenarioOptionsOpen();
rct_window* WindowFootpathOpen();
void WindowFootpathResetSelectedPath();
rct_window* WindowGuestOpen(Peep* peep);
rct_window* WindowLandOpen();
rct_window* WindowLandRightsOpen();
rct_window* WindowMainOpen();
rct_window* WindowMapgenOpen();
rct_window* WindowMultiplayerOpen();
rct_window* WindowNewsOpen();
rct_window* WindowNewsOptionsOpen();
rct_window* WindowOptionsOpen();
rct_window* WindowSavePromptOpen();
#ifndef DISABLE_NETWORK
rct_window* WindowServerListOpen();
rct_window* WindowServerStartOpen();
#endif
rct_window* WindowShortcutKeysOpen();
rct_window* WindowStaffListOpen();
rct_window* WindowStaffOpen(Peep* peep);
void WindowStaffListRefresh();
rct_window* WindowThemesOpen();
rct_window* WindowTitleExitOpen();
rct_window* WindowTitleLogoOpen();
rct_window* WindowTitleMenuOpen();
rct_window* WindowTitleOptionsOpen();
rct_window* WindowViewportOpen();
rct_window* WindowWaterOpen();
rct_window* WindowViewClippingOpen();
rct_window* WindowTransparencyOpen();
rct_window* WindowAssetPacksOpen();

// WC_FINANCES
rct_window* WindowFinancesOpen();
rct_window* WindowFinancesResearchOpen();
rct_window* WindowFinancesMarketingOpen();

// WC_PARK_INFORMATION
rct_window* WindowParkAwardsOpen();
rct_window* WindowParkEntranceOpen();
rct_window* WindowParkGuestsOpen();
rct_window* WindowParkObjectiveOpen();
rct_window* WindowParkRatingOpen();

rct_window* WindowBannerOpen(rct_windownumber number);
rct_window* WindowRideDemolishPromptOpen(const Ride& ride);
rct_window* WindowRideRefurbishPromptOpen(const Ride& ride);
rct_window* WindowSignOpen(rct_windownumber number);
rct_window* WindowSignSmallOpen(rct_windownumber number);
rct_window* WindowPlayerOpen(uint8_t id);
rct_window* WindowNewCampaignOpen(int16_t campaignType);

rct_window* WindowInstallTrackOpen(const utf8* path);
void WindowGuestListRefreshList();
rct_window* WindowGuestListOpen();
rct_window* WindowGuestListOpenWithFilter(GuestListFilterType type, int32_t index);
rct_window* WindowStaffFirePromptOpen(Peep* peep);
rct_window* WindowScenarioselectOpen(scenarioselect_callback callback, bool titleEditor);
rct_window* WindowScenarioselectOpen(std::function<void(std::string_view)> callback, bool titleEditor, bool disableLocking);

rct_window* WindowErrorOpen(StringId title, StringId message, const class Formatter& formatter);
rct_window* WindowErrorOpen(std::string_view title, std::string_view message);
struct TrackDesign;
rct_window* WindowLoadsaveOpen(
    int32_t type, std::string_view defaultPath, std::function<void(int32_t result, std::string_view)> callback,
    TrackDesign* trackDesign);
rct_window* WindowTrackPlaceOpen(const struct TrackDesignFileRef* tdFileRef);
rct_window* WindowTrackManageOpen(struct TrackDesignFileRef* tdFileRef);

void TrackPlaceClearProvisionalTemporarily();
void TrackPlaceRestoreProvisional();

rct_window* WindowMapOpen();
void WindowMapReset();

rct_window* WindowResearchOpen();
void WindowResearchDevelopmentPagePaint(rct_window* w, rct_drawpixelinfo* dpi, WidgetIndex baseWidgetIndex);
void WindowResearchFundingPagePaint(rct_window* w, rct_drawpixelinfo* dpi, WidgetIndex baseWidgetIndex);

rct_window* WindowNewRideOpen();
rct_window* WindowNewRideOpenResearch();
void WindowNewRideInitVars();
void WindowNewRideFocus(RideSelection rideItem);

rct_window* WindowRideListOpen();
void WindowRideListRefreshList(rct_window* w);

rct_window* WindowRideMainOpen(const Ride& ride);
rct_window* WindowRideOpenTrack(TileElement* tileElement);
rct_window* WindowRideOpenVehicle(Vehicle* vehicle);
void WindowRideMeasurementsDesignCancel();

// rct2: 0x00F635EE
extern RideSelection _window_track_list_item;
rct_window* WindowTrackListOpen(RideSelection item);

void SetMapTooltip(Formatter& ft);
const Formatter& GetMapTooltip();
void WindowMapTooltipUpdateVisibility();

rct_window* WindowMazeConstructionOpen();
void WindowMazeConstructionUpdatePressedWidgets();

rct_window* WindowNetworkStatusOpen(const std::string& text, close_callback onClose);
rct_window* WindowNetworkStatusOpenPassword();
void WindowNetworkStatusClose();

void WindowTextInputKey(rct_window* w, uint32_t keycode);
void WindowTextInputOpen(
    rct_window* call_w, WidgetIndex call_widget, StringId title, StringId description, const Formatter& descriptionArgs,
    StringId existing_text, uintptr_t existing_args, int32_t maxLength);
void WindowTextInputRawOpen(
    rct_window* call_w, WidgetIndex call_widget, StringId title, StringId description, const Formatter& descriptionArgs,
    const_utf8string existing_text, int32_t maxLength);

void WindowTextInputOpen(
    std::string_view title, std::string_view description, std::string_view initialValue, size_t maxLength,
    std::function<void(std::string_view)> okCallback, std::function<void()> cancelCallback);

rct_window* WindowObjectLoadErrorOpen(utf8* path, size_t numMissingObjects, const ObjectEntryDescriptor* missingObjects);

rct_window* WindowRideConstructionOpen();
void WindowRideConstructionUpdateActiveElementsImpl();
void WindowRideConstructionUpdateEnabledTrackPieces();

rct_window* WindowTopToolbarOpen();
bool LandToolIsActive();
bool ClearSceneryToolIsActive();
bool WaterToolIsActive();

rct_window* WindowSceneryOpen();
void WindowScenerySetSelectedItem(
    const ScenerySelection& sceneryconst, std::optional<colour_t> primary, const std::optional<colour_t> secondary,
    const std::optional<colour_t> tertiary, const std::optional<colour_t> rotation);
void WindowScenerySetSelectedTab(const ObjectEntryIndex sceneryGroupIndex);
void WindowScenerySetDefaultPlacementConfiguration();
void WindowSceneryInit();
void WindowSceneryResetSelectedSceneryItems();
const ScenerySelection WindowSceneryGetTabSelection();

extern uint8_t gToolbarDirtyFlags;
rct_window* WindowGameBottomToolbarOpen();
void WindowGameBottomToolbarInvalidateNewsItem();

rct_window* WindowEditorBottomToolbarOpen();

rct_window* WindowTileInspectorOpen();
void WindowTileInspectorClearClipboard();

rct_window* WindowEditorObjectSelectionOpen();

void WindowTooltipReset(const ScreenCoordsXY& screenCoords);
void WindowTooltipShow(const OpenRCT2String& message, ScreenCoordsXY screenCoords);
void WindowTooltipOpen(rct_window* widgetWindow, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void WindowTooltipClose();

rct_window* WindowSceneryScatterOpen();
rct_window* WindowPatrolAreaOpen(EntityId staffId);
EntityId WindowPatrolAreaGetCurrentStaffId();

// clang-format off
#define WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, CLOSE_STR) \
    { WindowWidgetType::Frame,    0,  0,          WIDTH - 1, 0, HEIGHT - 1, 0xFFFFFFFF,  STR_NONE }, \
    { WindowWidgetType::Caption,  0,  1,          WIDTH - 2, 1, 14,         TITLE,       STR_WINDOW_TITLE_TIP }, \
    { WindowWidgetType::CloseBox, 0,  WIDTH - 13, WIDTH - 3, 2, 13,         CLOSE_STR, STR_CLOSE_WINDOW_TIP }

#define WINDOW_SHIM(TITLE, WIDTH, HEIGHT) WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, STR_CLOSE_X)
#define WINDOW_SHIM_WHITE(TITLE, WIDTH, HEIGHT) WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, STR_CLOSE_X_WHITE)

// clang-format on
