/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include <openrct2/core/Guard.hpp>
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

    rct_window* OpenWindow(WindowClass wc) override
    {
        switch (wc)
        {
            case WindowClass::About:
                return WindowAboutOpen();
            case WindowClass::BottomToolbar:
                return WindowGameBottomToolbarOpen();
            case WindowClass::Changelog:
                return OpenView(WV_CHANGELOG);
            case WindowClass::Cheats:
                return WindowCheatsOpen();
            case WindowClass::ClearScenery:
                return WindowClearSceneryOpen();
            case WindowClass::CustomCurrencyConfig:
                return CustomCurrencyWindowOpen();
            case WindowClass::DebugPaint:
                return WindowDebugPaintOpen();
            case WindowClass::EditorInventionList:
                return WindowEditorInventionsListOpen();
            case WindowClass::EditorObjectSelection:
                return WindowEditorObjectSelectionOpen();
            case WindowClass::EditorObjectiveOptions:
                return WindowEditorObjectiveOptionsOpen();
            case WindowClass::EditorScenarioOptions:
                return WindowEditorScenarioOptionsOpen();
            case WindowClass::Finances:
                return WindowFinancesOpen();
            case WindowClass::Footpath:
                return WindowFootpathOpen();
            case WindowClass::GuestList:
                return WindowGuestListOpen();
            case WindowClass::Land:
                return WindowLandOpen();
            case WindowClass::LandRights:
                return WindowLandRightsOpen();
            case WindowClass::MainWindow:
                return WindowMainOpen();
            case WindowClass::Map:
                return WindowMapOpen();
            case WindowClass::Mapgen:
                return WindowMapgenOpen();
            case WindowClass::Multiplayer:
                return WindowMultiplayerOpen();
            case WindowClass::ConstructRide:
                return WindowNewRideOpen();
            case WindowClass::ParkInformation:
                return WindowParkEntranceOpen();
            case WindowClass::RecentNews:
                return WindowNewsOpen();
            case WindowClass::RideConstruction:
                return WindowRideConstructionOpen();
            case WindowClass::Research:
                return WindowResearchOpen();
            case WindowClass::RideList:
                return WindowRideListOpen();
            case WindowClass::NotificationOptions:
                return WindowNewsOptionsOpen();
            case WindowClass::Options:
                return WindowOptionsOpen();
            case WindowClass::SavePrompt:
                return WindowSavePromptOpen();
            case WindowClass::Scenery:
                return WindowSceneryOpen();
            case WindowClass::SceneryScatter:
                return WindowSceneryScatterOpen();
#ifndef DISABLE_NETWORK
            case WindowClass::ServerList:
                return WindowServerListOpen();
            case WindowClass::ServerStart:
                return WindowServerStartOpen();
#endif
            case WindowClass::KeyboardShortcutList:
                return WindowShortcutKeysOpen();
            case WindowClass::StaffList:
                return WindowStaffListOpen();
            case WindowClass::Themes:
                return WindowThemesOpen();
            case WindowClass::TileInspector:
                return WindowTileInspectorOpen();
            case WindowClass::TitleExit:
                return WindowTitleExitOpen();
            case WindowClass::TitleLogo:
                return WindowTitleLogoOpen();
            case WindowClass::TitleMenu:
                return WindowTitleMenuOpen();
            case WindowClass::TitleOptions:
                return WindowTitleOptionsOpen();
            case WindowClass::TopToolbar:
                return WindowTopToolbarOpen();
            case WindowClass::ViewClipping:
                return WindowViewClippingOpen();
            case WindowClass::Viewport:
                return WindowViewportOpen();
            case WindowClass::Water:
                return WindowWaterOpen();
            case WindowClass::Transparency:
                return WindowTransparencyOpen();
            case WindowClass::AssetPacks:
                return WindowAssetPacksOpen();
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
                if (gConfigInterface.ToolbarShowResearch)
                {
                    return this->OpenWindow(WindowClass::Research);
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
            case WV_CONTRIBUTORS:
                return WindowChangelogOpen(WV_CONTRIBUTORS);
            case WV_FINANCE_MARKETING:
                return WindowFinancesMarketingOpen();
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
            case WD_NEW_CAMPAIGN:
                return WindowNewCampaignOpen(id);
            case WD_DEMOLISH_RIDE:
                return WindowRideDemolishPromptOpen(*GetRide(RideId::FromUnderlying(id)));
            case WD_REFURBISH_RIDE:
                return WindowRideRefurbishPromptOpen(*GetRide(RideId::FromUnderlying(id)));
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

    rct_window* ShowError(StringId title, StringId message, const Formatter& args) override
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
            case WindowClass::Peep:
                return WindowGuestOpen(static_cast<Peep*>(intent->GetPointerExtra(INTENT_EXTRA_PEEP)));
            case WindowClass::FirePrompt:
                return WindowStaffFirePromptOpen(static_cast<Peep*>(intent->GetPointerExtra(INTENT_EXTRA_PEEP)));
            case WindowClass::InstallTrack:
                return WindowInstallTrackOpen(intent->GetStringExtra(INTENT_EXTRA_PATH).c_str());
            case WindowClass::GuestList:
                return WindowGuestListOpenWithFilter(
                    static_cast<GuestListFilterType>(intent->GetSIntExtra(INTENT_EXTRA_GUEST_LIST_FILTER)),
                    intent->GetSIntExtra(INTENT_EXTRA_RIDE_ID));
            case WindowClass::Loadsave:
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
            case WindowClass::ManageTrackDesign:
                return WindowTrackManageOpen(
                    static_cast<TrackDesignFileRef*>(intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN)));
            case WindowClass::NetworkStatus:
            {
                std::string message = intent->GetStringExtra(INTENT_EXTRA_MESSAGE);
                close_callback callback = intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK);
                return WindowNetworkStatusOpen(message, callback);
            }
            case WindowClass::ObjectLoadError:
            {
                std::string path = intent->GetStringExtra(INTENT_EXTRA_PATH);
                auto objects = static_cast<const ObjectEntryDescriptor*>(intent->GetPointerExtra(INTENT_EXTRA_LIST));
                size_t count = intent->GetUIntExtra(INTENT_EXTRA_LIST_COUNT);
                WindowObjectLoadErrorOpen(const_cast<utf8*>(path.c_str()), count, objects);

                return nullptr;
            }
            case WindowClass::Ride:
            {
                const auto rideId = RideId::FromUnderlying(intent->GetSIntExtra(INTENT_EXTRA_RIDE_ID));
                auto ride = GetRide(rideId);
                return ride == nullptr ? nullptr : WindowRideMainOpen(*ride);
            }
            case WindowClass::TrackDesignPlace:
                return WindowTrackPlaceOpen(
                    static_cast<TrackDesignFileRef*>(intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN)));
            case WindowClass::TrackDesignList:
            {
                RideSelection rideItem;
                rideItem.Type = intent->GetUIntExtra(INTENT_EXTRA_RIDE_TYPE);
                rideItem.EntryIndex = intent->GetUIntExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX);
                return WindowTrackListOpen(rideItem);
            }
            case WindowClass::ScenarioSelect:
                return WindowScenarioselectOpen(
                    reinterpret_cast<scenarioselect_callback>(intent->GetPointerExtra(INTENT_EXTRA_CALLBACK)), false);

            case WindowClass::Null:
                // Intent does not hold a window class
                break;

            default:
                Guard::Assert(false, "OpenIntent was called for an unhandled window class.");
                break;
        }

        switch (intent->GetAction())
        {
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
                auto* window = WindowBringToFrontByClass(WindowClass::Scenery);
                if (window == nullptr)
                {
                    auto* tlbrWindow = WindowFindByClass(WindowClass::TopToolbar);
                    if (tlbrWindow != nullptr)
                    {
                        tlbrWindow->Invalidate();
                        if (!ToolSet(*tlbrWindow, WC_TOP_TOOLBAR__WIDX_SCENERY, Tool::Arrow))
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

            case INTENT_ACTION_NULL:
                // Intent does not hold an intent action
                break;

            default:
                Guard::Assert(false, "OpenIntent was called for an unhandled intent action.");
                break;
        }

        switch (intent->GetWindowDetail())
        {
            case WD_VEHICLE:
                return WindowRideOpenVehicle(static_cast<Vehicle*>(intent->GetPointerExtra(INTENT_EXTRA_VEHICLE)));
            case WD_TRACK:
                return WindowRideOpenTrack(static_cast<TileElement*>(intent->GetPointerExtra(INTENT_EXTRA_TILE_ELEMENT)));

            case WD_NULL:
                // Intent does not hold an window detail
                break;

            default:
                Guard::Assert(false, "OpenIntent was called for an unhandled window detail.");
                break;
        }

        Console::Error::WriteLine("Unhandled window class for intent (%d)", intent->GetWindowClass());
        return nullptr;
    }

    void BroadcastIntent(const Intent& intent) override
    {
        switch (intent.GetAction())
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
                auto window = WindowFindByClass(WindowClass::RideList);
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
                auto w = WindowFindByClass(WindowClass::RideConstruction);
                if (w == nullptr || w->number != rideIndex)
                {
                    WindowCloseConstructionWindows();
                    _currentRideIndex = RideId::FromUnderlying(rideIndex);
                    OpenWindow(WindowClass::RideConstruction);
                }
                else
                {
                    RideConstructionInvalidateCurrentTrack();
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
                auto* w = WindowFindByNumber(WindowClass::Ride, vehicle->ride.ToUnderlying());
                if (w == nullptr)
                    return;

                auto ride = vehicle->GetRide();
                auto viewVehicleIndex = w->ride.view - 1;
                if (ride == nullptr || viewVehicleIndex < 0 || viewVehicleIndex >= ride->NumTrains)
                    return;

                if (vehicle->sprite_index != ride->vehicles[viewVehicleIndex])
                    return;

                w->Invalidate();
                break;
            }

            case INTENT_ACTION_RIDE_PAINT_RESET_VEHICLE:
            {
                auto rideIndex = intent.GetUIntExtra(INTENT_EXTRA_RIDE_ID);
                auto w = WindowFindByNumber(WindowClass::Ride, rideIndex);
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
                WindowInvalidateByClass(WindowClass::GuestList);
                break;

            case INTENT_ACTION_UPDATE_GUEST_COUNT:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PEEP_COUNT;
                WindowInvalidateByClass(WindowClass::GuestList);
                WindowInvalidateByClass(WindowClass::ParkInformation);
                WindowGuestListRefreshList();
                break;

            case INTENT_ACTION_UPDATE_PARK_RATING:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PARK_RATING;
                WindowInvalidateByClass(WindowClass::ParkInformation);
                break;

            case INTENT_ACTION_UPDATE_DATE:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_DATE;
                break;

            case INTENT_ACTION_UPDATE_CASH:
                WindowInvalidateByClass(WindowClass::Finances);
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_MONEY;
                break;

            case INTENT_ACTION_UPDATE_BANNER:
            {
                uint8_t bannerIndex = static_cast<uint8_t>(intent.GetUIntExtra(INTENT_EXTRA_BANNER_INDEX));

                rct_window* w = WindowFindByNumber(WindowClass::Banner, bannerIndex);
                if (w != nullptr)
                {
                    w->Invalidate();
                }
                break;
            }
            case INTENT_ACTION_UPDATE_RESEARCH:
                WindowInvalidateByClass(WindowClass::Finances);
                WindowInvalidateByClass(WindowClass::Research);
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

            default:
                break;
        }
    }

    void ForceClose(WindowClass windowClass) override
    {
        switch (windowClass)
        {
            case WindowClass::NetworkStatus:
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
        auto mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            auto viewport = WindowGetViewport(mainWindow);
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
            ViewportUpdatePosition(mainWindow);

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

std::unique_ptr<IWindowManager> OpenRCT2::Ui::CreateWindowManager()
{
    return std::make_unique<WindowManager>();
}
