/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WindowManager.h"

#include "interface/Theme.h"
#include "interface/Window.h"
#include "ride/VehicleSounds.h"
#include "windows/Windows.h"

#include <openrct2-ui/ProvisionalElements.h>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/input/MouseInput.h>
#include <openrct2-ui/input/ShortcutManager.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
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
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;
using namespace OpenRCT2::Ui::Windows;

namespace WindowCloseFlags
{
    static constexpr uint32_t None = 0;
    static constexpr uint32_t CloseSingle = (1 << 0);
} // namespace WindowCloseFlags

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
                LoadSaveCallback callback = reinterpret_cast<LoadSaveCallback>(
                    intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK));
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
                CloseCallback callback = intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK);
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
                    reinterpret_cast<ScenarioSelectCallback>(intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK)));

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
                auto* window = BringToFrontByClass(WindowClass::Scenery);
                if (window == nullptr)
                    ToggleSceneryWindow();

                // Switch to new scenery tab
                WindowScenerySetSelectedTab(intent->GetUIntExtra(INTENT_EXTRA_SCENERY_GROUP_ENTRY_INDEX));
                return window;
            }

            case INTENT_ACTION_PROGRESS_OPEN:
            {
                std::string message = intent->GetStringExtra(INTENT_EXTRA_MESSAGE);
                CloseCallback callback = intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK);
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
                auto window = FindByClass(WindowClass::RideList);
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
                auto w = FindByClass(WindowClass::RideConstruction);
                if (w == nullptr || w->number != static_cast<int16_t>(rideIndex))
                {
                    CloseConstructionWindows();
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
                InvalidateByClass(WindowClass::GuestList);
                break;

            case INTENT_ACTION_UPDATE_GUEST_COUNT:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PEEP_COUNT;
                InvalidateByClass(WindowClass::GuestList);
                InvalidateByClass(WindowClass::ParkInformation);
                WindowGuestListRefreshList();
                break;

            case INTENT_ACTION_UPDATE_PARK_RATING:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PARK_RATING;
                InvalidateByClass(WindowClass::ParkInformation);
                break;

            case INTENT_ACTION_UPDATE_DATE:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_DATE;
                break;

            case INTENT_ACTION_UPDATE_CASH:
                InvalidateByClass(WindowClass::Finances);
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_MONEY;
                break;

            case INTENT_ACTION_UPDATE_BANNER:
            {
                rct_windownumber bannerIndex = static_cast<rct_windownumber>(intent.GetUIntExtra(INTENT_EXTRA_BANNER_INDEX));

                WindowBase* w = FindByNumber(WindowClass::Banner, bannerIndex);
                if (w != nullptr)
                {
                    w->Invalidate();
                }
                break;
            }
            case INTENT_ACTION_UPDATE_RESEARCH:
                InvalidateByClass(WindowClass::Finances);
                InvalidateByClass(WindowClass::Research);
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
                InvalidateByClass(WindowClass::TileInspector);
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
            case INTENT_ACTION_REMOVE_PROVISIONAL_TRACK_PIECE:
                RideRemoveProvisionalTrackPiece();
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

            mainWindow->viewport_target_sprite = EntityId::GetNull();
            mainWindow->savedViewPos = viewPos;
            viewport->zoom = zoom;
            viewport->rotation = rotation;

            mainWindow->savedViewPos.x -= viewport->ViewWidth() / 2;
            mainWindow->savedViewPos.y -= viewport->ViewHeight() / 2;

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

    static bool WindowFitsBetweenOthers(const ScreenCoordsXY& loc, int32_t width, int32_t height)
    {
        for (auto& w : g_window_list)
        {
            if (w->flags & WF_DEAD)
                continue;
            if (w->flags & WF_STICK_TO_BACK)
                continue;

            if (loc.x + width <= w->windowPos.x)
                continue;
            if (loc.x >= w->windowPos.x + w->width)
                continue;
            if (loc.y + height <= w->windowPos.y)
                continue;
            if (loc.y >= w->windowPos.y + w->height)
                continue;
            return false;
        }

        return true;
    }

    static bool WindowFitsWithinSpace(const ScreenCoordsXY& loc, int32_t width, int32_t height)
    {
        if (loc.x < 0)
            return false;
        if (loc.y <= kTopToolbarHeight && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
            return false;
        if (loc.x + width > ContextGetWidth())
            return false;
        if (loc.y + height > ContextGetHeight())
            return false;
        return WindowFitsBetweenOthers(loc, width, height);
    }

    static bool WindowFitsOnScreen(const ScreenCoordsXY& loc, int32_t width, int32_t height)
    {
        uint16_t screenWidth = ContextGetWidth();
        uint16_t screenHeight = ContextGetHeight();
        int32_t unk;

        unk = -(width / 4);
        if (loc.x < unk)
            return false;
        unk = screenWidth + (unk * 2);
        if (loc.x > unk)
            return false;
        if (loc.y <= kTopToolbarHeight && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
            return false;
        unk = screenHeight - (height / 4);
        if (loc.y > unk)
            return false;
        return WindowFitsBetweenOthers(loc, width, height);
    }

    static ScreenCoordsXY ClampWindowToScreen(
        const ScreenCoordsXY& pos, const int32_t screenWidth, const int32_t screenHeight, const int32_t width,
        const int32_t height)
    {
        auto screenPos = pos;
        if (width > screenWidth || screenPos.x < 0)
            screenPos.x = 0;
        else if (screenPos.x + width > screenWidth)
            screenPos.x = screenWidth - width;

        auto toolbarAllowance = (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) ? 0 : (kTopToolbarHeight + 1);
        if (height - toolbarAllowance > screenHeight || screenPos.y < toolbarAllowance)
            screenPos.y = toolbarAllowance;
        else if (screenPos.y + height - toolbarAllowance > screenHeight)
            screenPos.y = screenHeight + toolbarAllowance - height;

        return screenPos;
    }

    static ScreenCoordsXY GetAutoPositionForNewWindow(int32_t width, int32_t height)
    {
        auto uiContext = GetContext()->GetUiContext();
        auto screenWidth = uiContext->GetWidth();
        auto screenHeight = uiContext->GetHeight();

        // Place window in an empty corner of the screen
        const ScreenCoordsXY cornerPositions[] = {
            { 0, 30 },                                           // topLeft
            { screenWidth - width, 30 },                         // topRight
            { 0, screenHeight - 34 - height },                   // bottomLeft
            { screenWidth - width, screenHeight - 34 - height }, // bottomRight
        };

        for (const auto& cornerPos : cornerPositions)
        {
            if (WindowFitsWithinSpace(cornerPos, width, height))
            {
                return ClampWindowToScreen(cornerPos, screenWidth, screenHeight, width, height);
            }
        }

        // Place window next to another
        for (auto& w : g_window_list)
        {
            if (w->flags & WF_DEAD)
                continue;
            if (w->flags & WF_STICK_TO_BACK)
                continue;

            const ScreenCoordsXY offsets[] = {
                { w->width + 2, 0 },
                { -w->width - 2, 0 },
                { 0, w->height + 2 },
                { 0, -w->height - 2 },
                { w->width + 2, -w->height - 2 },
                { -w->width - 2, -w->height - 2 },
                { w->width + 2, w->height + 2 },
                { -w->width - 2, w->height + 2 },
            };

            for (const auto& offset : offsets)
            {
                auto screenPos = w->windowPos + offset;
                if (WindowFitsWithinSpace(screenPos, width, height))
                {
                    return ClampWindowToScreen(screenPos, screenWidth, screenHeight, width, height);
                }
            }
        }

        // Overlap
        for (auto& w : g_window_list)
        {
            if (w->flags & WF_DEAD)
                continue;
            if (w->flags & WF_STICK_TO_BACK)
                continue;

            const ScreenCoordsXY offsets[] = {
                { w->width + 2, 0 },
                { -w->width - 2, 0 },
                { 0, w->height + 2 },
                { 0, -w->height - 2 },
            };

            for (const auto& offset : offsets)
            {
                auto screenPos = w->windowPos + offset;
                if (WindowFitsOnScreen(screenPos, width, height))
                {
                    return ClampWindowToScreen(screenPos, screenWidth, screenHeight, width, height);
                }
            }
        }

        // Cascade
        auto screenPos = ScreenCoordsXY{ 0, 30 };
        for (auto& w : g_window_list)
        {
            if (screenPos == w->windowPos)
            {
                screenPos.x += 5;
                screenPos.y += 5;
            }
        }

        return ClampWindowToScreen(screenPos, screenWidth, screenHeight, width, height);
    }

    static ScreenCoordsXY GetCentrePositionForNewWindow(int32_t width, int32_t height)
    {
        auto uiContext = GetContext()->GetUiContext();
        auto screenWidth = uiContext->GetWidth();
        auto screenHeight = uiContext->GetHeight();
        return ScreenCoordsXY{ (screenWidth - width) / 2, std::max(kTopToolbarHeight + 1, (screenHeight - height) / 2) };
    }

    WindowBase* Create(
        std::unique_ptr<WindowBase>&& wp, WindowClass cls, ScreenCoordsXY pos, int32_t width, int32_t height,
        uint32_t flags) override
    {
        if (flags & WF_AUTO_POSITION)
        {
            if (flags & WF_CENTRE_SCREEN)
            {
                pos = GetCentrePositionForNewWindow(width, height);
            }
            else
            {
                pos = GetAutoPositionForNewWindow(width, height);
            }
        }

        // Check if there are any window slots left
        // include kWindowLimitReserved for items such as the main viewport and toolbars to not appear to be counted.
        if (g_window_list.size() >= static_cast<size_t>(Config::Get().general.WindowLimit + kWindowLimitReserved))
        {
            // Close least recently used window
            for (auto& w : g_window_list)
            {
                if (w->flags & WF_DEAD)
                    continue;
                if (!(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT | WF_NO_AUTO_CLOSE)))
                {
                    Close(*w.get());
                    break;
                }
            }
        }

        // Find right position to insert new window
        auto itDestPos = g_window_list.end();
        if (flags & WF_STICK_TO_BACK)
        {
            for (auto it = g_window_list.begin(); it != g_window_list.end(); it++)
            {
                if ((*it)->flags & WF_DEAD)
                    continue;
                if (!((*it)->flags & WF_STICK_TO_BACK))
                {
                    itDestPos = it;
                }
            }
        }
        else if (!(flags & WF_STICK_TO_FRONT))
        {
            for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); it++)
            {
                if ((*it)->flags & WF_DEAD)
                    continue;
                if (!((*it)->flags & WF_STICK_TO_FRONT))
                {
                    itDestPos = it.base();
                    break;
                }
            }
        }

        auto itNew = g_window_list.insert(itDestPos, std::move(wp));
        auto w = itNew->get();

        // Setup window
        w->classification = cls;
        w->flags = flags;

        // Play sounds and flash the window
        if (!(flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)))
        {
            w->flags |= WF_WHITE_BORDER_MASK;
            OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::WindowOpen, 0, pos.x + (width / 2));
        }

        w->windowPos = pos;
        w->width = width;
        w->height = height;
        w->min_width = width;
        w->max_width = width;
        w->min_height = height;
        w->max_height = height;

        w->focus = std::nullopt;

        ColourSchemeUpdate(w);
        w->Invalidate();
        w->OnOpen();
        return w;
    }

    /**
     * Closes the specified window.
     *  rct2: 0x006ECD4C
     */
    void Close(WindowBase& w) override
    {
        w.OnClose();

        // Remove viewport
        w.RemoveViewport();

        // Invalidate the window (area)
        w.Invalidate();

        w.flags |= WF_DEAD;
    }

    void CloseSurplus(int32_t cap, WindowClass avoid_classification) override
    {
        // find the amount of windows that are currently open
        auto count = static_cast<int32_t>(g_window_list.size());
        // difference between amount open and cap = amount to close
        auto diff = count - kWindowLimitReserved - cap;
        for (auto i = 0; i < diff; i++)
        {
            // iterates through the list until it finds the newest window, or a window that can be closed
            WindowBase* foundW{};
            for (auto& w : g_window_list)
            {
                if (w->flags & WF_DEAD)
                    continue;
                if (!(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT | WF_NO_AUTO_CLOSE)))
                {
                    foundW = w.get();
                    break;
                }
            }
            // skip window if window matches specified WindowClass (as user may be modifying via options)
            if (avoid_classification != WindowClass::Null && foundW != nullptr
                && foundW->classification == avoid_classification)
            {
                continue;
            }
            if (foundW != nullptr)
                Close(*foundW);
        }
    }

    template<typename TPred>
    void CloseByCondition(TPred pred, uint32_t flags = WindowCloseFlags::None)
    {
        for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); ++it)
        {
            auto& wnd = *(*it);
            if (wnd.flags & WF_DEAD)
                continue;

            if (pred(&wnd))
            {
                Close(wnd);
                if (flags & WindowCloseFlags::CloseSingle)
                {
                    return;
                }
            }
        }
    }

    /**
     * Closes all windows with the specified window class.
     *  rct2: 0x006ECCF4
     */
    void CloseByClass(WindowClass cls) override
    {
        CloseByCondition([&](WindowBase* w) -> bool { return w->classification == cls; });
    }

    /**
     * Closes all windows with specified window class and number.
     *  rct2: 0x006ECCF4
     */
    void CloseByNumber(WindowClass cls, rct_windownumber number) override
    {
        CloseByCondition([cls, number](WindowBase* w) -> bool { return w->classification == cls && w->number == number; });
    }

    // TODO: Refactor this to use variant once the new window class is done.
    void CloseByNumber(WindowClass cls, EntityId number) override
    {
        CloseByNumber(cls, static_cast<rct_windownumber>(number.ToUnderlying()));
    }

    /**
     * Closes the top-most window
     *
     *  rct2: 0x006E403C
     */
    void CloseTop() override
    {
        CloseByClass(WindowClass::Dropdown);

        if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        {
            if (GetGameState().EditorStep != EditorStep::LandscapeEditor)
                return;
        }

        auto pred = [](WindowBase* w) -> bool { return !(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)); };
        CloseByCondition(pred, WindowCloseFlags::CloseSingle);
    }

    /**
     * Closes all open windows
     *
     *  rct2: 0x006EE927
     */
    void CloseAll() override
    {
        CloseByClass(WindowClass::Dropdown);
        CloseByCondition([](WindowBase* w) -> bool { return !(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)); });
    }

    void CloseAllExceptClass(WindowClass cls) override
    {
        CloseByClass(WindowClass::Dropdown);
        CloseByCondition([cls](WindowBase* w) -> bool {
            return w->classification != cls && !(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT));
        });
    }

    /**
     * Closes all windows, save for those having any of the passed flags.
     */
    void CloseAllExceptFlags(uint16_t flags) override
    {
        CloseByCondition([flags](WindowBase* w) -> bool { return !(w->flags & flags); });
    }

    /**
     * Closes all windows except the specified window number and class.
     */
    void CloseAllExceptNumberAndClass(rct_windownumber number, WindowClass cls) override
    {
        CloseByClass(WindowClass::Dropdown);
        CloseByCondition([cls, number](WindowBase* w) -> bool {
            return (!(w->number == number && w->classification == cls) && !(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)));
        });
    }

    /**
     *
     *  rct2: 0x006CBCC3
     */
    void CloseConstructionWindows() override
    {
        CloseByClass(WindowClass::RideConstruction);
        CloseByClass(WindowClass::Footpath);
        CloseByClass(WindowClass::TrackDesignList);
        CloseByClass(WindowClass::TrackDesignPlace);
    }

    /**
     * Finds the first window with the specified window class.
     *  rct2: 0x006EA8A0
     * @returns the window or nullptr if no window was found.
     */
    WindowBase* FindByClass(WindowClass cls) override
    {
        for (auto& w : g_window_list)
        {
            if (w->flags & WF_DEAD)
                continue;
            if (w->classification == cls)
            {
                return w.get();
            }
        }
        return nullptr;
    }

    /**
     * Finds the first window with the specified window class and number.
     *  rct2: 0x006EA8A0
     * @returns the window or nullptr if no window was found.
     */
    WindowBase* FindByNumber(WindowClass cls, rct_windownumber number) override
    {
        for (auto& w : g_window_list)
        {
            if (w->flags & WF_DEAD)
                continue;
            if (w->classification == cls && w->number == number)
            {
                return w.get();
            }
        }
        return nullptr;
    }

    // TODO: Use variant for this once the window framework is done.
    WindowBase* FindByNumber(WindowClass cls, EntityId id) override
    {
        return FindByNumber(cls, static_cast<rct_windownumber>(id.ToUnderlying()));
    }

    /**
     *
     *  rct2: 0x006EA845
     */
    WindowBase* FindFromPoint(const ScreenCoordsXY& screenCoords) override
    {
        for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); it++)
        {
            auto& w = *it;
            if (w->flags & WF_DEAD)
                continue;

            if (screenCoords.x < w->windowPos.x || screenCoords.x >= w->windowPos.x + w->width
                || screenCoords.y < w->windowPos.y || screenCoords.y >= w->windowPos.y + w->height)
                continue;

            if (w->flags & WF_NO_BACKGROUND)
            {
                auto widgetIndex = FindWidgetFromPoint(*w.get(), screenCoords);
                if (widgetIndex == kWidgetIndexNull)
                    continue;
            }

            return w.get();
        }

        return nullptr;
    }

    /**
     *
     *  rct2: 0x006EA594
     * returns widget_index if found, -1 otherwise
     */
    WidgetIndex FindWidgetFromPoint(WindowBase& w, const ScreenCoordsXY& screenCoords) override
    {
        // Invalidate the window
        w.OnPrepareDraw();

        // Find the widget at point x, y
        WidgetIndex widget_index = kWidgetIndexNull;
        for (auto i = 0u; i < w.widgets.size(); i++)
        {
            const auto& widget = w.widgets[i];

            if (widget.type != WindowWidgetType::Empty && widget.IsVisible())
            {
                if (screenCoords.x >= w.windowPos.x + widget.left && screenCoords.x <= w.windowPos.x + widget.right
                    && screenCoords.y >= w.windowPos.y + widget.top && screenCoords.y <= w.windowPos.y + widget.bottom)
                {
                    widget_index = i;
                }
            }
        }

        // Return next widget if a dropdown
        if (widget_index != kWidgetIndexNull)
        {
            const auto& widget = w.widgets[widget_index];
            if (widget.type == WindowWidgetType::DropdownMenu)
                widget_index++;
        }

        // Return the widget index
        return widget_index;
    }

    /**
     * Invalidates the specified window.
     *  rct2: 0x006EB13A
     */
    template<typename TPred>
    static void InvalidateByCondition(TPred pred)
    {
        WindowVisitEach([pred](WindowBase* w) {
            if (pred(w))
            {
                w->Invalidate();
            }
        });
    }

    /**
     * Invalidates all windows with the specified window class.
     *  rct2: 0x006EC3AC
     */
    void InvalidateByClass(WindowClass cls) override
    {
        InvalidateByCondition([cls](WindowBase* w) -> bool { return w->classification == cls; });
    }

    /**
     * Invalidates all windows with the specified window class and number.
     *  rct2: 0x006EC3AC
     */
    void InvalidateByNumber(WindowClass cls, rct_windownumber number) override
    {
        InvalidateByCondition([cls, number](WindowBase* w) -> bool { return w->classification == cls && w->number == number; });
    }

    // TODO: Use variant for this once the window framework is done.
    void InvalidateByNumber(WindowClass cls, EntityId id) override
    {
        InvalidateByNumber(cls, static_cast<rct_windownumber>(id.ToUnderlying()));
    }

    /**
     * Invalidates all windows.
     */
    void InvalidateAll() override
    {
        WindowVisitEach([](WindowBase* w) { w->Invalidate(); });
    }

    /**
     * Invalidates the specified widget of a window.
     *  rct2: 0x006EC402
     */
    void InvalidateWidget(WindowBase& w, WidgetIndex widgetIndex) override
    {
        if (w.widgets.empty())
        {
            // This might be called before the window is fully created.
            return;
        }

        if (static_cast<size_t>(widgetIndex) >= w.widgets.size())
        {
            return;
        }

        const auto& widget = w.widgets[widgetIndex];
        if (widget.left == -2)
            return;

        GfxSetDirtyBlocks({ { w.windowPos + ScreenCoordsXY{ widget.left, widget.top } },
                            { w.windowPos + ScreenCoordsXY{ widget.right + 1, widget.bottom + 1 } } });
    }

    /**
     * Invalidates the specified widget of all windows that match the specified window class.
     */
    void InvalidateWidgetByClass(WindowClass cls, WidgetIndex widgetIndex) override
    {
        WindowVisitEach([this, cls, widgetIndex](WindowBase* w) {
            if (w->classification == cls)
            {
                InvalidateWidget(*w, widgetIndex);
            }
        });
    }

    /**
     * Invalidates the specified widget of all windows that match the specified window class and number.
     *  rct2: 0x006EC3AC
     */
    void InvalidateWidgetByNumber(WindowClass cls, rct_windownumber number, WidgetIndex widgetIndex) override
    {
        WindowVisitEach([this, cls, number, widgetIndex](WindowBase* w) {
            if (w->classification == cls && w->number == number)
            {
                InvalidateWidget(*w, widgetIndex);
            }
        });
    }

    /**
     *
     *  rct2: 0x006ECDA4
     */
    WindowBase* BringToFront(WindowBase& w) override
    {
        if (!(w.flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)))
        {
            auto itSourcePos = WindowGetIterator(&w);
            if (itSourcePos != g_window_list.end())
            {
                // Insert in front of the first non-stick-to-front window
                auto itDestPos = g_window_list.begin();
                for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); it++)
                {
                    auto& w2 = *it;
                    if (!(w2->flags & WF_STICK_TO_FRONT))
                    {
                        itDestPos = it.base();
                        break;
                    }
                }

                g_window_list.splice(itDestPos, g_window_list, itSourcePos);
                w.Invalidate();

                if (w.windowPos.x + w.width < 20)
                {
                    int32_t i = 20 - w.windowPos.x;
                    w.windowPos.x += i;
                    if (w.viewport != nullptr)
                        w.viewport->pos.x += i;
                    w.Invalidate();
                }
            }
        }
        return &w;
    }

    WindowBase* BringToFrontByClassWithFlags(WindowClass cls, uint16_t flags) override
    {
        WindowBase* w = FindByClass(cls);
        if (w != nullptr)
        {
            w->flags |= flags;
            w->Invalidate();
            w = BringToFront(*w);
        }

        return w;
    }

    WindowBase* BringToFrontByClass(WindowClass cls) override
    {
        return BringToFrontByClassWithFlags(cls, WF_WHITE_BORDER_MASK);
    }

    /**
     *
     *  rct2: 0x006ED78A
     */
    WindowBase* BringToFrontByNumber(WindowClass cls, rct_windownumber number) override
    {
        WindowBase* w = FindByNumber(cls, number);
        if (w != nullptr)
        {
            w->flags |= WF_WHITE_BORDER_MASK;
            w->Invalidate();
            w = BringToFront(*w);
        }

        return w;
    }
};

std::unique_ptr<IWindowManager> OpenRCT2::Ui::CreateWindowManager()
{
    return std::make_unique<WindowManager>();
}
