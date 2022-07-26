/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WindowManager.h"

#include "interface/Theme.h"
#include "windows/Window.h"

#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/input/ShortcutManager.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Input.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/rct2/T6Exporter.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Ui;

class WindowManager final : public IWindowManager
{
public:
    void Init() override
    {
        ThemeManagerInitialise();
        WindowNewRideInitVars();
    }

    rct_window* OpenWindow(rct_windowclass wc) override
    {
        switch (wc)
        {
            case WC_ABOUT:
                return WindowAboutOpen();
            case WC_BOTTOM_TOOLBAR:
                return WindowGameBottomToolbarOpen();
            case WC_CHANGELOG:
                return OpenView(WV_CHANGELOG);
            case WC_CHEATS:
                return WindowCheatsOpen();
            case WC_CLEAR_SCENERY:
                return WindowClearSceneryOpen();
            case WC_CUSTOM_CURRENCY_CONFIG:
                return CustomCurrencyWindowOpen();
            case WC_DEBUG_PAINT:
                return WindowDebugPaintOpen();
            case WC_EDITOR_INVENTION_LIST:
                return WindowEditorInventionsListOpen();
            case WC_EDITOR_OBJECT_SELECTION:
                return WindowEditorObjectSelectionOpen();
            case WC_EDITOR_OBJECTIVE_OPTIONS:
                return WindowEditorObjectiveOptionsOpen();
            case WC_EDITOR_SCENARIO_OPTIONS:
                return WindowEditorScenarioOptionsOpen();
            case WC_FINANCES:
                return WindowFinancesOpen();
            case WC_FOOTPATH:
                return WindowFootpathOpen();
            case WC_GUEST_LIST:
                return WindowGuestListOpen();
            case WC_LAND:
                return WindowLandOpen();
            case WC_LAND_RIGHTS:
                return WindowLandRightsOpen();
            case WC_MAIN_WINDOW:
                return WindowMainOpen();
            case WC_MAP:
                return WindowMapOpen();
            case WC_MAPGEN:
                return WindowMapgenOpen();
            case WC_MULTIPLAYER:
                return WindowMultiplayerOpen();
            case WC_CONSTRUCT_RIDE:
                return WindowNewRideOpen();
            case WC_PARK_INFORMATION:
                return WindowParkEntranceOpen();
            case WC_RECENT_NEWS:
                return WindowNewsOpen();
            case WC_RIDE_CONSTRUCTION:
                return WindowRideConstructionOpen();
            case WC_RESEARCH:
                return WindowResearchOpen();
            case WC_RIDE_LIST:
                return WindowRideListOpen();
            case WC_NOTIFICATION_OPTIONS:
                return WindowNewsOptionsOpen();
            case WC_OPTIONS:
                return WindowOptionsOpen();
            case WC_SAVE_PROMPT:
                return WindowSavePromptOpen();
            case WC_SCENERY:
                return WindowSceneryOpen();
            case WC_SCENERY_SCATTER:
                return WindowSceneryScatterOpen();
#ifndef DISABLE_NETWORK
            case WC_SERVER_LIST:
                return WindowServerListOpen();
            case WC_SERVER_START:
                return WindowServerStartOpen();
#endif
            case WC_KEYBOARD_SHORTCUT_LIST:
                return WindowShortcutKeysOpen();
            case WC_STAFF_LIST:
                return WindowStaffListOpen();
            case WC_THEMES:
                return WindowThemesOpen();
            case WC_TILE_INSPECTOR:
                return WindowTileInspectorOpen();
            case WC_TITLE_EXIT:
                return WindowTitleExitOpen();
            case WC_TITLE_LOGO:
                return WindowTitleLogoOpen();
            case WC_TITLE_MENU:
                return WindowTitleMenuOpen();
            case WC_TITLE_OPTIONS:
                return WindowTitleOptionsOpen();
            case WC_TOP_TOOLBAR:
                return WindowTopToolbarOpen();
            case WC_VIEW_CLIPPING:
                return WindowViewClippingOpen();
            case WC_VIEWPORT:
                return WindowViewportOpen();
            case WC_WATER:
                return WindowWaterOpen();
            case WC_TRANSPARENCY:
                return WindowTransparencyOpen();
            default:
                Console::Error::WriteLine("Unhandled window class (%d)", wc);
                return nullptr;
        }
    }

    rct_window* OpenView(uint8_t view) override
    {
        switch (view)
        {
            case WV_PARK_AWARDS:
                return WindowParkAwardsOpen();
            case WV_PARK_RATING:
                return WindowParkRatingOpen();
            case WV_PARK_OBJECTIVE:
                return WindowParkObjectiveOpen();
            case WV_PARK_GUESTS:
                return WindowParkGuestsOpen();
            case WV_FINANCES_RESEARCH:
                return WindowFinancesResearchOpen();
            case WV_RIDE_RESEARCH:
                if (gConfigInterface.toolbar_show_research)
                {
                    return this->OpenWindow(WC_RESEARCH);
                }
                return WindowNewRideOpenResearch();
            case WV_MAZE_CONSTRUCTION:
                return WindowMazeConstructionOpen();
            case WV_NETWORK_PASSWORD:
                return WindowNetworkStatusOpenPassword();
            case WV_EDITOR_BOTTOM_TOOLBAR:
                return WindowEditorBottomToolbarOpen();
            case WV_CHANGELOG:
                return WindowChangelogOpen(WV_CHANGELOG);
            case WV_NEW_VERSION_INFO:
                return WindowChangelogOpen(WV_NEW_VERSION_INFO);
            default:
                return nullptr;
        }
    }

    rct_window* OpenDetails(uint8_t type, int32_t id) override
    {
        switch (type)
        {
            case WD_BANNER:
                return WindowBannerOpen(id);
            case WD_DEMOLISH_RIDE:
                return WindowRideDemolishPromptOpen(get_ride(RideId::FromUnderlying(id)));
            case WD_REFURBISH_RIDE:
                return WindowRideRefurbishPromptOpen(get_ride(RideId::FromUnderlying(id)));
            case WD_NEW_CAMPAIGN:
                return WindowNewCampaignOpen(id);
            case WD_SIGN:
                return WindowSignOpen(id);
            case WD_SIGN_SMALL:
                return WindowSignSmallOpen(id);

            case WD_PLAYER:
                return WindowPlayerOpen(id);

            default:
                return nullptr;
        }
    }

    rct_window* ShowError(rct_string_id title, rct_string_id message, const Formatter& args) override
    {
        return WindowErrorOpen(title, message, args);
    }

    rct_window* ShowError(std::string_view title, std::string_view message) override
    {
        return WindowErrorOpen(title, message);
    }

    rct_window* OpenIntent(Intent* intent) override
    {
        switch (intent->GetWindowClass())
        {
            case WC_PEEP:
                return WindowGuestOpen(static_cast<Peep*>(intent->GetPointerExtra(INTENT_EXTRA_PEEP)));
            case WC_FIRE_PROMPT:
                return WindowStaffFirePromptOpen(static_cast<Peep*>(intent->GetPointerExtra(INTENT_EXTRA_PEEP)));
            case WC_INSTALL_TRACK:
                return WindowInstallTrackOpen(intent->GetStringExtra(INTENT_EXTRA_PATH).c_str());
            case WC_GUEST_LIST:
                return WindowGuestListOpenWithFilter(
                    static_cast<GuestListFilterType>(intent->GetSIntExtra(INTENT_EXTRA_GUEST_LIST_FILTER)),
                    intent->GetSIntExtra(INTENT_EXTRA_RIDE_ID));
            case WC_LOADSAVE:
            {
                uint32_t type = intent->GetUIntExtra(INTENT_EXTRA_LOADSAVE_TYPE);
                std::string defaultName = intent->GetStringExtra(INTENT_EXTRA_PATH);
                loadsave_callback callback = reinterpret_cast<loadsave_callback>(
                    intent->GetPointerExtra(INTENT_EXTRA_CALLBACK));
                TrackDesign* trackDesign = static_cast<TrackDesign*>(intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN));
                auto* w = WindowLoadsaveOpen(
                    type, defaultName,
                    [callback](int32_t result, std::string_view path) {
                        if (callback != nullptr)
                        {
                            callback(result, std::string(path).c_str());
                        }
                    },
                    trackDesign);
                return w;
            }
            case WC_MANAGE_TRACK_DESIGN:
                return WindowTrackManageOpen(
                    static_cast<TrackDesignFileRef*>(intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN)));
            case WC_NETWORK_STATUS:
            {
                std::string message = intent->GetStringExtra(INTENT_EXTRA_MESSAGE);
                close_callback callback = intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK);
                return WindowNetworkStatusOpen(message.c_str(), callback);
            }
            case WC_OBJECT_LOAD_ERROR:
            {
                std::string path = intent->GetStringExtra(INTENT_EXTRA_PATH);
                auto objects = static_cast<const ObjectEntryDescriptor*>(intent->GetPointerExtra(INTENT_EXTRA_LIST));
                size_t count = intent->GetUIntExtra(INTENT_EXTRA_LIST_COUNT);
                WindowObjectLoadErrorOpen(const_cast<utf8*>(path.c_str()), count, objects);

                return nullptr;
            }
            case WC_RIDE:
            {
                const auto rideId = RideId::FromUnderlying(intent->GetSIntExtra(INTENT_EXTRA_RIDE_ID));
                auto ride = get_ride(rideId);
                return ride == nullptr ? nullptr : WindowRideMainOpen(ride);
            }
            case WC_TRACK_DESIGN_PLACE:
                return WindowTrackPlaceOpen(
                    static_cast<TrackDesignFileRef*>(intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN)));
            case WC_TRACK_DESIGN_LIST:
            {
                RideSelection rideItem;
                rideItem.Type = intent->GetUIntExtra(INTENT_EXTRA_RIDE_TYPE);
                rideItem.EntryIndex = intent->GetUIntExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX);
                return WindowTrackListOpen(rideItem);
            }
            case WC_SCENARIO_SELECT:
                return WindowScenarioselectOpen(
                    reinterpret_cast<scenarioselect_callback>(intent->GetPointerExtra(INTENT_EXTRA_CALLBACK)), false);
            case WD_VEHICLE:
                return WindowRideOpenVehicle(static_cast<Vehicle*>(intent->GetPointerExtra(INTENT_EXTRA_VEHICLE)));
            case WD_TRACK:
                return WindowRideOpenTrack(static_cast<TileElement*>(intent->GetPointerExtra(INTENT_EXTRA_TILE_ELEMENT)));
            case INTENT_ACTION_NEW_RIDE_OF_TYPE:
            {
                // Open ride list window
                auto w = WindowNewRideOpen();

                // Switch to right tab and scroll to ride location
                RideSelection rideItem;
                rideItem.Type = intent->GetUIntExtra(INTENT_EXTRA_RIDE_TYPE);
                rideItem.EntryIndex = intent->GetUIntExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX);
                WindowNewRideFocus(rideItem);

                return w;
            }
            case INTENT_ACTION_NEW_SCENERY:
            {
                // Check if window is already open
                auto* window = window_bring_to_front_by_class(WC_SCENERY);
                if (window == nullptr)
                {
                    auto* tlbrWindow = window_find_by_class(WC_TOP_TOOLBAR);
                    if (tlbrWindow != nullptr)
                    {
                        tlbrWindow->Invalidate();
                        if (!tool_set(tlbrWindow, WC_TOP_TOOLBAR__WIDX_SCENERY, Tool::Arrow))
                        {
                            input_set_flag(INPUT_FLAG_6, true);
                            window = WindowSceneryOpen();
                        }
                    }
                }

                // Switch to new scenery tab
                WindowScenerySetSelectedTab(intent->GetUIntExtra(INTENT_EXTRA_SCENERY_GROUP_ENTRY_INDEX));
                return window;
            }
            default:
                Console::Error::WriteLine("Unhandled window class for intent (%d)", intent->GetWindowClass());
                return nullptr;
        }
    }

    void BroadcastIntent(const Intent& intent) override
    {
        switch (intent.GetWindowClass())
        {
            case INTENT_ACTION_MAP:
                WindowMapReset();
                break;

            case INTENT_ACTION_REFRESH_NEW_RIDES:
                WindowNewRideInitVars();
                break;

            case INTENT_ACTION_REFRESH_CAMPAIGN_RIDE_LIST:
            {
                WindowCampaignRefreshRides();
                break;
            }

            case INTENT_ACTION_REFRESH_RIDE_LIST:
            {
                auto window = window_find_by_class(WC_RIDE_LIST);
                if (window != nullptr)
                {
                    WindowRideListRefreshList(window);
                }

                break;
            }

            case INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION:
                WindowMazeConstructionUpdatePressedWidgets();
                break;

            case INTENT_ACTION_RIDE_CONSTRUCTION_FOCUS:
            {
                auto rideIndex = intent.GetUIntExtra(INTENT_EXTRA_RIDE_ID);
                auto w = window_find_by_class(WC_RIDE_CONSTRUCTION);
                if (w == nullptr || w->number != rideIndex)
                {
                    window_close_construction_windows();
                    _currentRideIndex = RideId::FromUnderlying(rideIndex);
                    OpenWindow(WC_RIDE_CONSTRUCTION);
                }
                else
                {
                    ride_construction_invalidate_current_track();
                    _currentRideIndex = RideId::FromUnderlying(rideIndex);
                }
                break;
            }

            case INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_PIECES:
                WindowRideConstructionUpdateEnabledTrackPieces();
                break;

            case INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_ACTIVE_ELEMENTS:
                WindowRideConstructionUpdateActiveElementsImpl();
                break;

            case INTENT_ACTION_INIT_SCENERY:
                WindowSceneryInit();
                break;

            case INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG:
                WindowScenerySetDefaultPlacementConfiguration();
                break;

            case INTENT_ACTION_REFRESH_SCENERY:
                WindowSceneryResetSelectedSceneryItems();
                break;

            case INTENT_ACTION_INVALIDATE_TICKER_NEWS:
                WindowGameBottomToolbarInvalidateNewsItem();
                break;

            case INTENT_ACTION_REFRESH_GUEST_LIST:
                WindowGuestListRefreshList();
                break;

            case INTENT_ACTION_REFRESH_STAFF_LIST:
            {
                WindowStaffListRefresh();
                break;
            }

            case INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD:
                WindowTileInspectorClearClipboard();
                break;

            case INTENT_ACTION_INVALIDATE_VEHICLE_WINDOW:
            {
                auto vehicle = static_cast<Vehicle*>(intent.GetPointerExtra(INTENT_EXTRA_VEHICLE));
                auto* w = window_find_by_number(WC_RIDE, vehicle->ride.ToUnderlying());
                if (w == nullptr)
                    return;

                auto ride = vehicle->GetRide();
                auto viewVehicleIndex = w->ride.view - 1;
                if (ride == nullptr || viewVehicleIndex < 0 || viewVehicleIndex >= ride->num_vehicles)
                    return;

                if (vehicle->sprite_index != ride->vehicles[viewVehicleIndex])
                    return;

                w->Invalidate();
                break;
            }

            case INTENT_ACTION_RIDE_PAINT_RESET_VEHICLE:
            {
                auto rideIndex = intent.GetUIntExtra(INTENT_EXTRA_RIDE_ID);
                auto w = window_find_by_number(WC_RIDE, rideIndex);
                if (w != nullptr)
                {
                    if (w->page == 4) // WINDOW_RIDE_PAGE_COLOUR
                    {
                        w->vehicleIndex = 0;
                    }
                    w->Invalidate();
                }
                break;
            }

            case INTENT_ACTION_UPDATE_CLIMATE:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_CLIMATE;
                window_invalidate_by_class(WC_GUEST_LIST);
                break;

            case INTENT_ACTION_UPDATE_GUEST_COUNT:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PEEP_COUNT;
                window_invalidate_by_class(WC_GUEST_LIST);
                window_invalidate_by_class(WC_PARK_INFORMATION);
                WindowGuestListRefreshList();
                break;

            case INTENT_ACTION_UPDATE_PARK_RATING:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PARK_RATING;
                window_invalidate_by_class(WC_PARK_INFORMATION);
                break;

            case INTENT_ACTION_UPDATE_DATE:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_DATE;
                break;

            case INTENT_ACTION_UPDATE_CASH:
                window_invalidate_by_class(WC_FINANCES);
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_MONEY;
                break;

            case INTENT_ACTION_UPDATE_BANNER:
            {
                uint8_t bannerIndex = static_cast<uint8_t>(intent.GetUIntExtra(INTENT_EXTRA_BANNER_INDEX));

                rct_window* w = window_find_by_number(WC_BANNER, bannerIndex);
                if (w != nullptr)
                {
                    w->Invalidate();
                }
                break;
            }
            case INTENT_ACTION_UPDATE_RESEARCH:
                window_invalidate_by_class(WC_FINANCES);
                window_invalidate_by_class(WC_RESEARCH);
                break;

            case INTENT_ACTION_TRACK_DESIGN_REMOVE_PROVISIONAL:
                TrackPlaceClearProvisionalTemporarily();
                break;

            case INTENT_ACTION_TRACK_DESIGN_RESTORE_PROVISIONAL:
                TrackPlaceRestoreProvisional();
                break;

            case INTENT_ACTION_SET_MAP_TOOLTIP:
            {
                auto ft = static_cast<Formatter*>(intent.GetPointerExtra(INTENT_EXTRA_FORMATTER));
                if (ft != nullptr)
                {
                    SetMapTooltip(*ft);
                }
                break;
            }
        }
    }

    void ForceClose(rct_windowclass windowClass) override
    {
        switch (windowClass)
        {
            case WC_NETWORK_STATUS:
                WindowNetworkStatusClose();
                break;

            default:
                break;
        }
    }

    void UpdateMapTooltip() override
    {
        WindowMapTooltipUpdateVisibility();
    }

    void HandleInput() override
    {
        GameHandleInput();
    }

    void HandleKeyboard(bool isTitle) override
    {
        auto& inputManager = GetInputManager();
        inputManager.Process();
    }

    std::string GetKeyboardShortcutString(std::string_view shortcutId) override
    {
        auto& shortcutManager = GetShortcutManager();
        auto* shortcut = shortcutManager.GetShortcut(shortcutId);
        return shortcut != nullptr ? shortcut->GetDisplayString() : std::string();
    }

    void SetMainView(const ScreenCoordsXY& viewPos, ZoomLevel zoom, int32_t rotation) override
    {
        auto mainWindow = window_get_main();
        if (mainWindow != nullptr)
        {
            auto viewport = window_get_viewport(mainWindow);
            auto zoomDifference = zoom - viewport->zoom;

            mainWindow->viewport_target_sprite = EntityId::GetNull();
            mainWindow->savedViewPos = viewPos;
            viewport->zoom = zoom;
            gCurrentRotation = rotation;

            if (zoomDifference != ZoomLevel{ 0 })
            {
                viewport->view_width = zoomDifference.ApplyTo(viewport->view_width);
                viewport->view_height = zoomDifference.ApplyTo(viewport->view_height);
            }
            mainWindow->savedViewPos.x -= viewport->view_width >> 1;
            mainWindow->savedViewPos.y -= viewport->view_height >> 1;

            // Make sure the viewport has correct coordinates set.
            viewport_update_position(mainWindow);

            mainWindow->Invalidate();
        }
    }

    void UpdateMouseWheel() override
    {
        WindowAllWheelInput();
    }

    rct_window* GetOwner(const rct_viewport* viewport) override
    {
        for (auto& w : g_window_list)
        {
            if (w->viewport == viewport)
            {
                return w.get();
            }
        }
        return nullptr;
    }
};

IWindowManager* OpenRCT2::Ui::CreateWindowManager()
{
    return new WindowManager();
}
