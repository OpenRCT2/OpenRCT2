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
#include <openrct2/windows/TileInspectorGlobals.h>
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

WindowBase* WindowAboutOpen();
void WindowCampaignRefreshRides();
WindowBase* WindowChangelogOpen(int personality);
WindowBase* WindowCheatsOpen();
WindowBase* WindowClearSceneryOpen();
WindowBase* CustomCurrencyWindowOpen();
WindowBase* WindowDebugPaintOpen();
WindowBase* WindowEditorInventionsListOpen();
WindowBase* WindowEditorMainOpen();
WindowBase* WindowEditorObjectiveOptionsOpen();
WindowBase* WindowEditorScenarioOptionsOpen();
WindowBase* WindowFootpathOpen();
void WindowFootpathResetSelectedPath();
WindowBase* WindowGuestOpen(Peep* peep);
WindowBase* WindowLandOpen();
WindowBase* WindowLandRightsOpen();
WindowBase* WindowMainOpen();
WindowBase* WindowMapgenOpen();
WindowBase* WindowMultiplayerOpen();
WindowBase* WindowNewsOpen();
WindowBase* WindowNewsOptionsOpen();
WindowBase* WindowOptionsOpen();
WindowBase* WindowSavePromptOpen();
#ifndef DISABLE_NETWORK
WindowBase* WindowServerListOpen();
WindowBase* WindowServerStartOpen();
#endif
WindowBase* WindowShortcutKeysOpen();
WindowBase* WindowStaffListOpen();
WindowBase* WindowStaffOpen(Peep* peep);
void WindowStaffListRefresh();
WindowBase* WindowThemesOpen();
WindowBase* WindowTitleExitOpen();
WindowBase* WindowTitleLogoOpen();
WindowBase* WindowTitleMenuOpen();
WindowBase* WindowTitleOptionsOpen();
WindowBase* WindowViewportOpen();
WindowBase* WindowWaterOpen();
WindowBase* WindowViewClippingOpen();
WindowBase* WindowTransparencyOpen();
WindowBase* WindowAssetPacksOpen();

// WC_FINANCES
WindowBase* WindowFinancesOpen();
WindowBase* WindowFinancesResearchOpen();
WindowBase* WindowFinancesMarketingOpen();

// WC_PARK_INFORMATION
WindowBase* WindowParkAwardsOpen();
WindowBase* WindowParkEntranceOpen();
WindowBase* WindowParkGuestsOpen();
WindowBase* WindowParkObjectiveOpen();
WindowBase* WindowParkRatingOpen();

WindowBase* WindowBannerOpen(rct_windownumber number);
WindowBase* WindowRideDemolishPromptOpen(const Ride& ride);
WindowBase* WindowRideRefurbishPromptOpen(const Ride& ride);
WindowBase* WindowSignOpen(rct_windownumber number);
WindowBase* WindowSignSmallOpen(rct_windownumber number);
WindowBase* WindowPlayerOpen(uint8_t id);
WindowBase* WindowNewCampaignOpen(int16_t campaignType);

WindowBase* WindowInstallTrackOpen(const utf8* path);
void WindowGuestListRefreshList();
WindowBase* WindowGuestListOpen();
WindowBase* WindowGuestListOpenWithFilter(GuestListFilterType type, int32_t index);
WindowBase* WindowStaffFirePromptOpen(Peep* peep);
WindowBase* WindowScenarioselectOpen(scenarioselect_callback callback, bool titleEditor);
WindowBase* WindowScenarioselectOpen(std::function<void(std::string_view)> callback, bool titleEditor);

WindowBase* WindowErrorOpen(StringId title, StringId message, const class Formatter& formatter);
WindowBase* WindowErrorOpen(std::string_view title, std::string_view message);
struct TrackDesign;
WindowBase* WindowLoadsaveOpen(
    int32_t type, std::string_view defaultPath, std::function<void(int32_t result, std::string_view)> callback,
    TrackDesign* trackDesign);
WindowBase* WindowTrackPlaceOpen(const struct TrackDesignFileRef* tdFileRef);
WindowBase* WindowTrackManageOpen(struct TrackDesignFileRef* tdFileRef);

void TrackPlaceClearProvisionalTemporarily();
void TrackPlaceRestoreProvisional();

WindowBase* WindowMapOpen();
void WindowMapReset();

WindowBase* WindowResearchOpen();
void WindowResearchDevelopmentMouseUp(WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex);
void WindowResearchDevelopmentPrepareDraw(WindowBase* w, WidgetIndex baseWidgetIndex);
void WindowResearchDevelopmentDraw(WindowBase* w, DrawPixelInfo& dpi, WidgetIndex baseWidgetIndex);
void WindowResearchFundingMouseDown(WindowBase* w, WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex);
void WindowResearchFundingMouseUp(WidgetIndex widgetIndex, WidgetIndex baseWidgetIndex);
void WindowResearchFundingDropdown(WidgetIndex widgetIndex, int32_t selectedIndex, WidgetIndex baseWidgetIndex);
void WindowResearchFundingPrepareDraw(WindowBase* w, WidgetIndex baseWidgetIndex);
void WindowResearchFundingDraw(WindowBase* w, DrawPixelInfo& dpi);

WindowBase* WindowNewRideOpen();
WindowBase* WindowNewRideOpenResearch();
void WindowNewRideInitVars();
void WindowNewRideFocus(RideSelection rideItem);

WindowBase* WindowRideListOpen();
void WindowRideListRefreshList(WindowBase* w);

WindowBase* WindowRideMainOpen(const Ride& ride);
WindowBase* WindowRideOpenTrack(TileElement* tileElement);
WindowBase* WindowRideOpenVehicle(Vehicle* vehicle);
void WindowRideMeasurementsDesignCancel();

// rct2: 0x00F635EE
extern RideSelection _window_track_list_item;
WindowBase* WindowTrackListOpen(RideSelection item);

void SetMapTooltip(Formatter& ft);
const Formatter& GetMapTooltip();
void WindowMapTooltipUpdateVisibility();

WindowBase* WindowMazeConstructionOpen();
void WindowMazeConstructionUpdatePressedWidgets();

WindowBase* WindowNetworkStatusOpen(const std::string& text, close_callback onClose);
WindowBase* WindowNetworkStatusOpenPassword();
void WindowNetworkStatusClose();

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

WindowBase* WindowObjectLoadErrorOpen(utf8* path, size_t numMissingObjects, const ObjectEntryDescriptor* missingObjects);

WindowBase* WindowRideConstructionOpen();
void WindowRideConstructionUpdateActiveElementsImpl();
void WindowRideConstructionUpdateEnabledTrackPieces();

WindowBase* WindowTopToolbarOpen();
bool LandToolIsActive();
bool ClearSceneryToolIsActive();
bool WaterToolIsActive();

WindowBase* WindowSceneryOpen();
void WindowScenerySetSelectedItem(
    const ScenerySelection& sceneryconst, std::optional<colour_t> primary, const std::optional<colour_t> secondary,
    const std::optional<colour_t> tertiary, const std::optional<colour_t> rotation);
void WindowScenerySetSelectedTab(const ObjectEntryIndex sceneryGroupIndex);
void WindowScenerySetDefaultPlacementConfiguration();
void WindowSceneryInit();
void WindowSceneryResetSelectedSceneryItems();
const ScenerySelection WindowSceneryGetTabSelection();

extern uint8_t gToolbarDirtyFlags;
WindowBase* WindowGameBottomToolbarOpen();
void WindowGameBottomToolbarInvalidateNewsItem();

WindowBase* WindowEditorBottomToolbarOpen();

WindowBase* WindowTileInspectorOpen();
void WindowTileInspectorClearClipboard();

WindowBase* WindowEditorObjectSelectionOpen();

void WindowTooltipReset(const ScreenCoordsXY& screenCoords);
void WindowTooltipShow(const OpenRCT2String& message, ScreenCoordsXY screenCoords);
void WindowTooltipOpen(WindowBase* widgetWindow, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void WindowTooltipClose();

WindowBase* WindowSceneryScatterOpen();
WindowBase* WindowPatrolAreaOpen(EntityId staffId);
EntityId WindowPatrolAreaGetCurrentStaffId();

// clang-format off
#define WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, CLOSE_STR) \
    { WindowWidgetType::Frame,    0,  0,          WIDTH - 1, 0, HEIGHT - 1, 0xFFFFFFFF,  STR_NONE }, \
    { WindowWidgetType::Caption,  0,  1,          WIDTH - 2, 1, 14,         TITLE,       STR_WINDOW_TITLE_TIP }, \
    { WindowWidgetType::CloseBox, 0,  WIDTH - 13, WIDTH - 3, 2, 13,         CLOSE_STR, STR_CLOSE_WINDOW_TIP }

#define WINDOW_SHIM(TITLE, WIDTH, HEIGHT) WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, STR_CLOSE_X)
#define WINDOW_SHIM_WHITE(TITLE, WIDTH, HEIGHT) WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, STR_CLOSE_X_WHITE)

// clang-format on
