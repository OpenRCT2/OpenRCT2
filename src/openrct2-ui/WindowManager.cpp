/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WindowManager.h"

#include "interface/Theme.h"
#include "ride/VehicleSounds.h"
#include "windows/Window.h"

#include <openrct2-ui/ProvisionalElements.h>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/input/MouseInput.h>
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
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;
using namespace OpenRCT2::Ui::Windows;

class WindowManager final : public IWindowManager
{
public:
    void Init() override
    {
        ThemeManagerInitialise();
        WindowNewRideInitVars();
    }

    WindowBase* OpenWindow(WindowClass wc) override
    {
        switch (wc)
        {
            case WindowClass::About:
                return AboutOpen();
            case WindowClass::BottomToolbar:
                return GameBottomToolbarOpen();
            case WindowClass::Changelog:
                return OpenView(WV_CHANGELOG);
            case WindowClass::Cheats:
                return CheatsOpen();
            case WindowClass::ClearScenery:
                return ClearSceneryOpen();
            case WindowClass::CustomCurrencyConfig:
                return CustomCurrencyOpen();
            case WindowClass::DebugPaint:
                return DebugPaintOpen();
            case WindowClass::EditorInventionList:
                return EditorInventionsListOpen();
            case WindowClass::EditorObjectSelection:
                return EditorObjectSelectionOpen();
            case WindowClass::EditorObjectiveOptions:
                return EditorObjectiveOptionsOpen();
            case WindowClass::EditorScenarioOptions:
                return EditorScenarioOptionsOpen();
            case WindowClass::Finances:
                return FinancesOpen();
            case WindowClass::Footpath:
                return FootpathOpen();
            case WindowClass::GuestList:
                return GuestListOpen();
            case WindowClass::Land:
                return LandOpen();
            case WindowClass::LandRights:
                return LandRightsOpen();
            case WindowClass::MainWindow:
                return MainOpen();
            case WindowClass::Map:
                return MapOpen();
            case WindowClass::Mapgen:
                return MapgenOpen();
            case WindowClass::Multiplayer:
                return MultiplayerOpen();
            case WindowClass::ConstructRide:
                return NewRideOpen();
            case WindowClass::ParkInformation:
                return ParkEntranceOpen();
            case WindowClass::RecentNews:
                return NewsOpen();
            case WindowClass::RideConstruction:
                return RideConstructionOpen();
            case WindowClass::Research:
                return ResearchOpen();
            case WindowClass::RideList:
                return RideListOpen();
            case WindowClass::NotificationOptions:
                return NewsOptionsOpen();
            case WindowClass::Options:
                return OptionsOpen();
            case WindowClass::SavePrompt:
                return SavePromptOpen();
            case WindowClass::Scenery:
                return SceneryOpen();
            case WindowClass::SceneryScatter:
                return SceneryScatterOpen();
#ifndef DISABLE_NETWORK
            case WindowClass::ServerList:
                return ServerListOpen();
            case WindowClass::ServerStart:
                return ServerStartOpen();
#endif
            case WindowClass::KeyboardShortcutList:
                return ShortcutKeysOpen();
            case WindowClass::StaffList:
                return StaffListOpen();
            case WindowClass::Themes:
                return ThemesOpen();
            case WindowClass::TileInspector:
                return TileInspectorOpen();
            case WindowClass::TitleExit:
                return TitleExitOpen();
            case WindowClass::TitleLogo:
                return TitleLogoOpen();
            case WindowClass::TitleMenu:
                return TitleMenuOpen();
            case WindowClass::TitleOptions:
                return TitleOptionsOpen();
            case WindowClass::TitleVersion:
                return TitleVersionOpen();
            case WindowClass::TopToolbar:
                return TopToolbarOpen();
            case WindowClass::ViewClipping:
                return ViewClippingOpen();
            case WindowClass::Viewport:
                return ViewportOpen();
            case WindowClass::Water:
                return WaterOpen();
            case WindowClass::Transparency:
                return TransparencyOpen();
            case WindowClass::AssetPacks:
                return AssetPacksOpen();
            case WindowClass::EditorParkEntrance:
                return EditorParkEntranceOpen();
            default:
                Console::Error::WriteLine("Unhandled window class (%d)", wc);
                return nullptr;
        }
    }

    WindowBase* OpenView(uint8_t view) override
    {
        switch (view)
        {
            case WV_PARK_AWARDS:
                return ParkAwardsOpen();
            case WV_PARK_RATING:
                return ParkRatingOpen();
            case WV_PARK_OBJECTIVE:
                return ParkObjectiveOpen();
            case WV_PARK_GUESTS:
                return ParkGuestsOpen();
            case WV_FINANCES_RESEARCH:
                return FinancesResearchOpen();
            case WV_RIDE_RESEARCH:
                if (Config::Get().interface.ToolbarShowResearch)
                {
                    return this->OpenWindow(WindowClass::Research);
                }
                return NewRideOpenResearch();
            case WV_MAZE_CONSTRUCTION:
                return MazeConstructionOpen();
            case WV_NETWORK_PASSWORD:
                return NetworkStatusOpenPassword();
            case WV_EDITOR_BOTTOM_TOOLBAR:
                return EditorBottomToolbarOpen();
            case WV_CHANGELOG:
                return ChangelogOpen(WV_CHANGELOG);
            case WV_NEW_VERSION_INFO:
                return ChangelogOpen(WV_NEW_VERSION_INFO);
            case WV_CONTRIBUTORS:
                return ChangelogOpen(WV_CONTRIBUTORS);
            case WV_FINANCE_MARKETING:
                return FinancesMarketingOpen();
            default:
                return nullptr;
        }
    }

    WindowBase* OpenDetails(uint8_t type, int32_t id) override
    {
        switch (type)
        {
            case WD_BANNER:
                return BannerOpen(id);
            case WD_NEW_CAMPAIGN:
                return NewCampaignOpen(id);
            case WD_DEMOLISH_RIDE:
                return RideDemolishPromptOpen(*GetRide(RideId::FromUnderlying(id)));
            case WD_REFURBISH_RIDE:
                return RideRefurbishPromptOpen(*GetRide(RideId::FromUnderlying(id)));
            case WD_SIGN:
                return SignOpen(id);
            case WD_SIGN_SMALL:
                return SignSmallOpen(id);
            case WD_PLAYER:
                return PlayerOpen(id);

            default:
                return nullptr;
        }
    }

    WindowBase* ShowError(StringId title, StringId message, const Formatter& args, bool autoClose /* = false */) override
    {
        return ErrorOpen(title, message, args, autoClose);
    }

    WindowBase* ShowError(std::string_view title, std::string_view message, bool autoClose /* = false */) override
    {
        return ErrorOpen(title, message, autoClose);
    }

    WindowBase* OpenIntent(Intent* intent) override
    {
        switch (intent->GetWindowClass())
        {
            case WindowClass::Peep:
                return GuestOpen(static_cast<Peep*>(intent->GetPointerExtra(INTENT_EXTRA_PEEP)));
            case WindowClass::FirePrompt:
                return StaffFirePromptOpen(static_cast<Peep*>(intent->GetPointerExtra(INTENT_EXTRA_PEEP)));
            case WindowClass::InstallTrack:
                return InstallTrackOpen(intent->GetStringExtra(INTENT_EXTRA_PATH).c_str());
            case WindowClass::GuestList:
                return GuestListOpenWithFilter(
                    static_cast<GuestListFilterType>(intent->GetSIntExtra(INTENT_EXTRA_GUEST_LIST_FILTER)),
                    intent->GetSIntExtra(INTENT_EXTRA_RIDE_ID));
            case WindowClass::Loadsave:
            {
                uint32_t type = intent->GetUIntExtra(INTENT_EXTRA_LOADSAVE_TYPE);
                std::string defaultName = intent->GetStringExtra(INTENT_EXTRA_PATH);
                loadsave_callback callback = reinterpret_cast<loadsave_callback>(
                    intent->GetPointerExtra(INTENT_EXTRA_CALLBACK));
                TrackDesign* trackDesign = static_cast<TrackDesign*>(intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN));
                auto* w = LoadsaveOpen(
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
                return TrackManageOpen(static_cast<TrackDesignFileRef*>(intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN)));
            case WindowClass::NetworkStatus:
            {
                std::string message = intent->GetStringExtra(INTENT_EXTRA_MESSAGE);
                close_callback callback = intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK);
                return NetworkStatusOpen(message, callback);
            }
            case WindowClass::ObjectLoadError:
            {
                std::string path = intent->GetStringExtra(INTENT_EXTRA_PATH);
                auto objects = static_cast<const ObjectEntryDescriptor*>(intent->GetPointerExtra(INTENT_EXTRA_LIST));
                size_t count = intent->GetUIntExtra(INTENT_EXTRA_LIST_COUNT);
                ObjectLoadErrorOpen(const_cast<utf8*>(path.c_str()), count, objects);

                return nullptr;
            }
            case WindowClass::Ride:
            {
                const auto rideId = RideId::FromUnderlying(intent->GetSIntExtra(INTENT_EXTRA_RIDE_ID));
                auto ride = GetRide(rideId);
                return ride == nullptr ? nullptr : RideMainOpen(*ride);
            }
            case WindowClass::TrackDesignPlace:
                return TrackPlaceOpen(static_cast<TrackDesignFileRef*>(intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN)));
            case WindowClass::TrackDesignList:
            {
                RideSelection rideItem;
                rideItem.Type = intent->GetUIntExtra(INTENT_EXTRA_RIDE_TYPE);
                rideItem.EntryIndex = intent->GetUIntExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX);
                return TrackListOpen(rideItem);
            }
            case WindowClass::ScenarioSelect:
                return ScenarioselectOpen(
                    reinterpret_cast<scenarioselect_callback>(intent->GetPointerExtra(INTENT_EXTRA_CALLBACK)));

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
                auto w = NewRideOpen();

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
                    ToggleSceneryWindow();

                // Switch to new scenery tab
                WindowScenerySetSelectedTab(intent->GetUIntExtra(INTENT_EXTRA_SCENERY_GROUP_ENTRY_INDEX));
                return window;
            }

            case INTENT_ACTION_PROGRESS_OPEN:
            {
                std::string message = intent->GetStringExtra(INTENT_EXTRA_MESSAGE);
                close_callback callback = intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK);
                return ProgressWindowOpen(message, callback);
            }

            case INTENT_ACTION_PROGRESS_SET:
            {
                uint32_t currentProgress = intent->GetUIntExtra(INTENT_EXTRA_PROGRESS_OFFSET);
                uint32_t totalCount = intent->GetUIntExtra(INTENT_EXTRA_PROGRESS_TOTAL);
                StringId format = intent->GetUIntExtra(INTENT_EXTRA_STRING_ID);
                ProgressWindowSet(currentProgress, totalCount, format);
                return nullptr;
            }

            case INTENT_ACTION_PROGRESS_CLOSE:
            {
                ProgressWindowClose();
                return nullptr;
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
                return RideOpenVehicle(static_cast<Vehicle*>(intent->GetPointerExtra(INTENT_EXTRA_VEHICLE)));
            case WD_TRACK:
                return RideOpenTrack(static_cast<TileElement*>(intent->GetPointerExtra(INTENT_EXTRA_TILE_ELEMENT)));

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
                if (vehicle != nullptr)
                {
                    WindowRideInvalidateVehicle(*vehicle);
                }
                break;
            }

            case INTENT_ACTION_RIDE_PAINT_RESET_VEHICLE:
            {
                auto rideIndex = intent.GetUIntExtra(INTENT_EXTRA_RIDE_ID);
                WindowRidePaintResetVehicle(RideId::FromUnderlying(rideIndex));
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
                rct_windownumber bannerIndex = static_cast<rct_windownumber>(intent.GetUIntExtra(INTENT_EXTRA_BANNER_INDEX));

                WindowBase* w = WindowFindByNumber(WindowClass::Banner, bannerIndex);
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

            case INTENT_ACTION_UPDATE_VEHICLE_SOUNDS:
                OpenRCT2::Audio::UpdateVehicleSounds();
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

            case INTENT_ACTION_TILE_MODIFY:
            {
                WindowInvalidateByClass(WindowClass::TileInspector);
                break;
            }

            case INTENT_ACTION_REMOVE_PROVISIONAL_ELEMENTS:
                ProvisionalElementsRemove();
                break;
            case INTENT_ACTION_RESTORE_PROVISIONAL_ELEMENTS:
                ProvisionalElementsRestore();
                break;
            case INTENT_ACTION_REMOVE_PROVISIONAL_FOOTPATH:
                FootpathRemoveProvisional();
                break;

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

            case WindowClass::ProgressWindow:
                ProgressWindowClose();
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
            viewport->rotation = rotation;

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

    WindowBase* GetOwner(const Viewport* viewport) override
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
