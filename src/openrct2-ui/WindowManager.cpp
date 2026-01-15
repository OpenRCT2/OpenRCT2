/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WindowManager.h"

#include "interface/FileBrowser.h"
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
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/rct2/T6Exporter.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;
using namespace OpenRCT2::Ui::Windows;

namespace OpenRCT2
{
    class Formatter;
}

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
            case WindowClass::about:
                return AboutOpen();
            case WindowClass::bottomToolbar:
                return GameBottomToolbarOpen();
            case WindowClass::changelog:
                return openView(WindowView::changelog);
            case WindowClass::cheats:
                return CheatsOpen();
            case WindowClass::clearScenery:
                return ClearSceneryOpen();
            case WindowClass::customCurrencyConfig:
                return CustomCurrencyOpen();
            case WindowClass::debugPaint:
                return DebugPaintOpen();
            case WindowClass::editorInventionList:
                return EditorInventionsListOpen();
            case WindowClass::editorObjectSelection:
                return EditorObjectSelectionOpen();
            case WindowClass::editorScenarioOptions:
                return EditorScenarioOptionsOpen();
            case WindowClass::finances:
                return FinancesOpen();
            case WindowClass::footpath:
                return FootpathOpen();
            case WindowClass::guestList:
                return GuestListOpen();
            case WindowClass::land:
                return LandOpen();
            case WindowClass::landRights:
                return LandRightsOpen();
            case WindowClass::mainWindow:
                return MainOpen();
            case WindowClass::map:
                return MapOpen();
            case WindowClass::mapgen:
                return MapgenOpen();
            case WindowClass::multiplayer:
                return MultiplayerOpen();
            case WindowClass::constructRide:
                return NewRideOpen();
            case WindowClass::parkInformation:
                return ParkEntranceOpen();
            case WindowClass::recentNews:
                return NewsOpen();
            case WindowClass::rideConstruction:
                return RideConstructionOpen();
            case WindowClass::research:
                return ResearchOpen();
            case WindowClass::rideList:
                return RideListOpen();
            case WindowClass::options:
                return OptionsOpen();
            case WindowClass::savePrompt:
                return SavePromptOpen();
            case WindowClass::scenery:
                return SceneryOpen();
            case WindowClass::sceneryScatter:
                return SceneryScatterOpen();
#ifndef DISABLE_NETWORK
            case WindowClass::serverList:
                return ServerListOpen();
            case WindowClass::serverStart:
                return ServerStartOpen();
#endif
            case WindowClass::keyboardShortcutList:
                return ShortcutKeysOpen();
            case WindowClass::staffList:
                return StaffListOpen();
            case WindowClass::themes:
                return ThemesOpen();
            case WindowClass::tileInspector:
                return TileInspectorOpen();
            case WindowClass::titleExit:
                return TitleExitOpen();
            case WindowClass::titleLogo:
                return TitleLogoOpen();
            case WindowClass::titleMenu:
                return TitleMenuOpen();
            case WindowClass::titleOptions:
                return TitleOptionsOpen();
            case WindowClass::titleVersion:
                return TitleVersionOpen();
            case WindowClass::topToolbar:
                return TopToolbarOpen();
            case WindowClass::viewClipping:
                return ViewClippingOpen();
            case WindowClass::viewport:
                return ViewportOpen();
            case WindowClass::water:
                return WaterOpen();
            case WindowClass::transparency:
                return TransparencyOpen();
            case WindowClass::assetPacks:
                return AssetPacksOpen();
            case WindowClass::editorParkEntrance:
                return EditorParkEntranceOpen();
            default:
                Console::Error::WriteLine("Unhandled window class (%d)", wc);
                return nullptr;
        }
    }

    WindowBase* openView(WindowView view) override
    {
        switch (view)
        {
            case WindowView::parkAwards:
                return ParkAwardsOpen();
            case WindowView::parkRating:
                return ParkRatingOpen();
            case WindowView::parkObjective:
                return ParkObjectiveOpen();
            case WindowView::parkGuests:
                return ParkGuestsOpen();
            case WindowView::financesResearch:
                return FinancesResearchOpen();
            case WindowView::rideResearch:
                if (Config::Get().interface.toolbarShowResearch)
                {
                    return this->OpenWindow(WindowClass::research);
                }
                return NewRideOpenResearch();
            case WindowView::mazeConstruction:
                return MazeConstructionOpen();
            case WindowView::networkPassword:
                return NetworkStatusOpenPassword();
            case WindowView::editorBottomToolbar:
                return EditorBottomToolbarOpen();
            case WindowView::changelog:
                return ChangelogOpen(WindowView::changelog);
            case WindowView::newVersionInfo:
                return ChangelogOpen(WindowView::newVersionInfo);
            case WindowView::contributors:
                return ChangelogOpen(WindowView::contributors);
            case WindowView::financeMarketing:
                return FinancesMarketingOpen();
            default:
                return nullptr;
        }
    }

    WindowBase* openDetails(WindowDetail type, int32_t id) override
    {
        switch (type)
        {
            case WindowDetail::banner:
                return BannerOpen(id);
            case WindowDetail::newCampaign:
                return NewCampaignOpen(id);
            case WindowDetail::demolishRide:
                return RideDemolishPromptOpen(*GetRide(RideId::FromUnderlying(id)));
            case WindowDetail::refurbishRide:
                return RideRefurbishPromptOpen(*GetRide(RideId::FromUnderlying(id)));
            case WindowDetail::sign:
                return SignOpen(id);
            case WindowDetail::signSmall:
                return SignSmallOpen(id);
            case WindowDetail::player:
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
            case WindowClass::peep:
                return GuestOpen(static_cast<Peep*>(intent->GetPointerExtra(INTENT_EXTRA_PEEP)));
            case WindowClass::firePrompt:
                return StaffFirePromptOpen(static_cast<Peep*>(intent->GetPointerExtra(INTENT_EXTRA_PEEP)));
            case WindowClass::installTrack:
                return InstallTrackOpen(intent->GetStringExtra(INTENT_EXTRA_PATH).c_str());
            case WindowClass::guestList:
                return GuestListOpenWithFilter(
                    static_cast<GuestListFilterType>(intent->GetSIntExtra(INTENT_EXTRA_GUEST_LIST_FILTER)),
                    intent->GetSIntExtra(INTENT_EXTRA_RIDE_ID));
            case WindowClass::loadsave:
            {
                auto action = intent->GetEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION);
                auto type = intent->GetEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE);
                std::string defaultPath = intent->GetStringExtra(INTENT_EXTRA_PATH);
                LoadSaveCallback callback = reinterpret_cast<LoadSaveCallback>(
                    intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK));
                TrackDesign* trackDesign = static_cast<TrackDesign*>(intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN));
                auto* w = FileBrowser::OpenPreferred(
                    action, type, defaultPath,
                    [callback](ModalResult result, std::string_view path) {
                        if (callback != nullptr)
                        {
                            callback(result, std::string(path).c_str());
                        }
                    },
                    trackDesign);
                return w;
            }
            case WindowClass::manageTrackDesign:
                return TrackManageOpen(static_cast<TrackDesignFileRef*>(intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN)));
            case WindowClass::networkStatus:
            {
                std::string message = intent->GetStringExtra(INTENT_EXTRA_MESSAGE);
                CloseCallback callback = intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK);
                return NetworkStatusOpen(message, callback);
            }
            case WindowClass::objectLoadError:
            {
                std::string path = intent->GetStringExtra(INTENT_EXTRA_PATH);
                auto objects = static_cast<const ObjectEntryDescriptor*>(intent->GetPointerExtra(INTENT_EXTRA_LIST));
                size_t count = intent->GetUIntExtra(INTENT_EXTRA_LIST_COUNT);
                ObjectLoadErrorOpen(const_cast<utf8*>(path.c_str()), count, objects);

                return nullptr;
            }
            case WindowClass::ride:
            {
                const auto rideId = RideId::FromUnderlying(intent->GetSIntExtra(INTENT_EXTRA_RIDE_ID));
                auto ride = GetRide(rideId);
                return ride == nullptr ? nullptr : RideMainOpen(*ride);
            }
            case WindowClass::trackDesignPlace:
                return TrackPlaceOpen(static_cast<TrackDesignFileRef*>(intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN)));
            case WindowClass::trackDesignList:
            {
                RideSelection rideItem;
                rideItem.Type = intent->GetUIntExtra(INTENT_EXTRA_RIDE_TYPE);
                rideItem.EntryIndex = intent->GetUIntExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX);
                return TrackListOpen(rideItem);
            }
            case WindowClass::scenarioSelect:
                return ScenarioselectOpen(
                    reinterpret_cast<ScenarioSelectCallback>(intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK)));

            case WindowClass::null:
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
                auto* window = BringToFrontByClass(WindowClass::scenery);
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
            case WindowDetail::vehicle:
                return RideOpenVehicle(static_cast<Vehicle*>(intent->GetPointerExtra(INTENT_EXTRA_VEHICLE)));
            case WindowDetail::track:
                return RideOpenTrack(static_cast<TileElement*>(intent->GetPointerExtra(INTENT_EXTRA_TILE_ELEMENT)));

            case WindowDetail::null:
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
                auto window = FindByClass(WindowClass::rideList);
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
                auto w = FindByClass(WindowClass::rideConstruction);
                if (w == nullptr || w->number != static_cast<int16_t>(rideIndex))
                {
                    CloseConstructionWindows();
                    _currentRideIndex = RideId::FromUnderlying(rideIndex);
                    OpenWindow(WindowClass::rideConstruction);
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
                InvalidateByClass(WindowClass::guestList);
                break;

            case INTENT_ACTION_UPDATE_GUEST_COUNT:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PEEP_COUNT;
                InvalidateByClass(WindowClass::guestList);
                InvalidateByClass(WindowClass::parkInformation);
                WindowGuestListRefreshList();
                break;

            case INTENT_ACTION_UPDATE_PARK_RATING:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PARK_RATING;
                InvalidateByClass(WindowClass::parkInformation);
                break;

            case INTENT_ACTION_UPDATE_DATE:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_DATE;
                break;

            case INTENT_ACTION_UPDATE_CASH:
                InvalidateByClass(WindowClass::finances);
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_MONEY;
                break;

            case INTENT_ACTION_UPDATE_BANNER:
            {
                WindowNumber bannerIndex = static_cast<WindowNumber>(intent.GetUIntExtra(INTENT_EXTRA_BANNER_INDEX));

                WindowBase* w = FindByNumber(WindowClass::banner, bannerIndex);
                if (w != nullptr)
                {
                    w->invalidate();
                }
                break;
            }
            case INTENT_ACTION_UPDATE_RESEARCH:
                InvalidateByClass(WindowClass::finances);
                InvalidateByClass(WindowClass::research);
                break;

            case INTENT_ACTION_UPDATE_VEHICLE_SOUNDS:
                Audio::UpdateVehicleSounds();
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
                InvalidateByClass(WindowClass::tileInspector);
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
            case WindowClass::editorObjectSelection:
                EditorObjectSelectionClose();
                break;

            case WindowClass::networkStatus:
                WindowNetworkStatusClose();
                break;

            case WindowClass::progressWindow:
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
        inputManager.process();
    }

    std::string GetKeyboardShortcutString(std::string_view shortcutId) override
    {
        auto& shortcutManager = GetShortcutManager();
        auto* shortcut = shortcutManager.getShortcut(shortcutId);
        return shortcut != nullptr ? shortcut->getDisplayString() : std::string();
    }

    void SetMainView(const ScreenCoordsXY& viewPos, ZoomLevel zoom, int32_t rotation) override
    {
        auto mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            auto viewport = WindowGetViewport(mainWindow);

            mainWindow->viewportTargetSprite = EntityId::GetNull();
            mainWindow->savedViewPos = viewPos;
            viewport->zoom = zoom;
            viewport->rotation = rotation;

            mainWindow->savedViewPos.x -= viewport->ViewWidth() / 2;
            mainWindow->savedViewPos.y -= viewport->ViewHeight() / 2;

            mainWindow->invalidate();
        }
    }

    void UpdateMouseWheel() override
    {
        WindowAllWheelInput();
    }

    WindowBase* GetOwner(const Viewport* viewport) override
    {
        for (auto& w : gWindowList)
        {
            if (w->viewport == viewport)
            {
                return w.get();
            }
        }
        return nullptr;
    }

    static bool WindowFitsBetweenOthers(const ScreenCoordsXY& loc, const ScreenSize windowSize)
    {
        for (auto& w : gWindowList)
        {
            if (w->flags.has(WindowFlag::dead))
                continue;
            if (w->flags.has(WindowFlag::stickToBack))
                continue;

            if (loc.x + windowSize.width <= w->windowPos.x)
                continue;
            if (loc.x >= w->windowPos.x + w->width)
                continue;
            if (loc.y + windowSize.height <= w->windowPos.y)
                continue;
            if (loc.y >= w->windowPos.y + w->height)
                continue;
            return false;
        }

        return true;
    }

    static bool WindowFitsWithinSpace(const ScreenCoordsXY& loc, ScreenSize windowSize)
    {
        if (loc.x < 0)
            return false;
        if (loc.y <= kTopToolbarHeight && gLegacyScene != LegacyScene::titleSequence)
            return false;
        if (loc.x + windowSize.width > ContextGetWidth())
            return false;
        if (loc.y + windowSize.height > ContextGetHeight())
            return false;
        return WindowFitsBetweenOthers(loc, windowSize);
    }

    static bool WindowFitsOnScreen(const ScreenCoordsXY& loc, const ScreenSize windowSize)
    {
        uint16_t screenWidth = ContextGetWidth();
        uint16_t screenHeight = ContextGetHeight();
        int32_t unk;

        unk = -(windowSize.width / 4);
        if (loc.x < unk)
            return false;
        unk = screenWidth + (unk * 2);
        if (loc.x > unk)
            return false;
        if (loc.y <= kTopToolbarHeight && gLegacyScene != LegacyScene::titleSequence)
            return false;
        unk = screenHeight - (windowSize.height / 4);
        if (loc.y > unk)
            return false;
        return WindowFitsBetweenOthers(loc, windowSize);
    }

    static ScreenCoordsXY ClampWindowToScreen(
        const ScreenCoordsXY& pos, const ScreenSize screenSize, const ScreenSize windowSize)
    {
        auto screenPos = pos;
        if (windowSize.width > screenSize.width || screenPos.x < 0)
            screenPos.x = 0;
        else if (screenPos.x + windowSize.width > screenSize.width)
            screenPos.x = screenSize.width - windowSize.width;

        auto toolbarAllowance = gLegacyScene == LegacyScene::titleSequence ? 0 : (kTopToolbarHeight + 1);
        if (windowSize.height - toolbarAllowance > screenSize.height || screenPos.y < toolbarAllowance)
            screenPos.y = toolbarAllowance;
        else if (screenPos.y + windowSize.height - toolbarAllowance > screenSize.height)
            screenPos.y = screenSize.height + toolbarAllowance - windowSize.height;

        return screenPos;
    }

    static ScreenCoordsXY GetAutoPositionForNewWindow(const ScreenSize windowSize)
    {
        auto& uiContext = GetContext()->GetUiContext();
        ScreenSize screenSize = { uiContext.GetWidth(), uiContext.GetHeight() };

        // Place window in an empty corner of the screen
        const ScreenCoordsXY cornerPositions[] = {
            { 0, 30 },                                                                           // topLeft
            { screenSize.width - windowSize.width, 30 },                                         // topRight
            { 0, screenSize.height - 34 - windowSize.height },                                   // bottomLeft
            { screenSize.width - windowSize.width, screenSize.height - 34 - windowSize.height }, // bottomRight
        };

        for (const auto& cornerPos : cornerPositions)
        {
            if (WindowFitsWithinSpace(cornerPos, windowSize))
            {
                return ClampWindowToScreen(cornerPos, screenSize, windowSize);
            }
        }

        // Place window next to another
        for (auto& w : gWindowList)
        {
            if (w->flags.has(WindowFlag::dead))
                continue;
            if (w->flags.has(WindowFlag::stickToBack))
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
                if (WindowFitsWithinSpace(screenPos, windowSize))
                {
                    return ClampWindowToScreen(screenPos, screenSize, windowSize);
                }
            }
        }

        // Overlap
        for (auto& w : gWindowList)
        {
            if (w->flags.has(WindowFlag::dead))
                continue;
            if (w->flags.has(WindowFlag::stickToBack))
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
                if (WindowFitsOnScreen(screenPos, windowSize))
                {
                    return ClampWindowToScreen(screenPos, screenSize, windowSize);
                }
            }
        }

        // Cascade
        auto screenPos = ScreenCoordsXY{ 0, 30 };
        for (auto& w : gWindowList)
        {
            if (screenPos == w->windowPos)
            {
                screenPos.x += 5;
                screenPos.y += 5;
            }
        }

        return ClampWindowToScreen(screenPos, screenSize, windowSize);
    }

    static ScreenCoordsXY GetCentrePositionForNewWindow(ScreenSize size)
    {
        auto& uiContext = GetContext()->GetUiContext();
        auto screenWidth = uiContext.GetWidth();
        auto screenHeight = uiContext.GetHeight();
        return ScreenCoordsXY{ (screenWidth - size.width) / 2,
                               std::max(kTopToolbarHeight + 1, (screenHeight - size.height) / 2) };
    }

    WindowBase* Create(
        std::unique_ptr<WindowBase>&& wp, WindowClass cls, ScreenCoordsXY pos, ScreenSize windowSize,
        WindowFlags flags) override
    {
        windowSize.height += wp->getTitleBarDiffTarget();

        if (flags.has(WindowFlag::autoPosition))
        {
            if (flags.has(WindowFlag::centreScreen))
            {
                pos = GetCentrePositionForNewWindow(windowSize);
            }
            else
            {
                pos = GetAutoPositionForNewWindow(windowSize);
            }
        }

        windowSize.height -= wp->getTitleBarDiffTarget();

        // Check if there are any window slots left
        // include kWindowLimitReserved for items such as the main viewport and toolbars to not appear to be counted.
        if (gWindowList.size() >= static_cast<size_t>(Config::Get().general.windowLimit + kWindowLimitReserved))
        {
            // Close least recently used window
            for (auto& w : gWindowList)
            {
                if (w->flags.has(WindowFlag::dead))
                    continue;
                if (!w->flags.hasAny(WindowFlag::stickToBack, WindowFlag::stickToFront, WindowFlag::noAutoClose))
                {
                    Close(*w.get());
                    break;
                }
            }
        }

        // Find right position to insert new window
        auto itDestPos = gWindowList.end();
        if (flags.has(WindowFlag::stickToBack))
        {
            for (auto it = gWindowList.begin(); it != gWindowList.end(); it++)
            {
                if ((*it)->flags.has(WindowFlag::dead))
                    continue;
                if (!((*it)->flags.has(WindowFlag::stickToBack)))
                {
                    itDestPos = it;
                }
            }
        }
        else if (!flags.has(WindowFlag::stickToFront))
        {
            for (auto it = gWindowList.rbegin(); it != gWindowList.rend(); it++)
            {
                if ((*it)->flags.has(WindowFlag::dead))
                    continue;
                if (!((*it)->flags.has(WindowFlag::stickToFront)))
                {
                    itDestPos = it.base();
                    break;
                }
            }
        }

        // Setup window
        wp->classification = cls;
        wp->flags = flags;

        // Play sounds and flash the window
        if (!flags.hasAny(WindowFlag::stickToBack, WindowFlag::stickToFront))
        {
            wp->flash();
            Audio::Play(Audio::SoundId::windowOpen, 0, pos.x + (windowSize.width / 2));
        }

        wp->windowPos = pos;
        wp->width = windowSize.width;
        wp->height = windowSize.height;
        wp->minWidth = windowSize.width;
        wp->maxWidth = windowSize.width;
        wp->minHeight = windowSize.height;
        wp->maxHeight = windowSize.height;

        wp->focus = std::nullopt;

        ColourSchemeUpdate(wp.get());
        wp->invalidate();
        wp->onOpen();

        auto itNew = gWindowList.insert(itDestPos, std::move(wp));
        return itNew->get();
    }

    /**
     * Closes the specified window.
     *  rct2: 0x006ECD4C
     */
    void Close(WindowBase& w) override
    {
        if (!w.canClose())
        {
            // Something's preventing this window from closing -- bail out early
            return;
        }

        w.onClose();

        // Remove viewport
        w.removeViewport();

        // Invalidate the window (area)
        w.invalidate();

        w.flags |= WindowFlag::dead;
    }

    void CloseSurplus(int32_t cap, WindowClass avoid_classification) override
    {
        // find the amount of windows that are currently open
        auto count = static_cast<int32_t>(gWindowList.size());
        // difference between amount open and cap = amount to close
        auto diff = count - kWindowLimitReserved - cap;
        for (auto i = 0; i < diff; i++)
        {
            // iterates through the list until it finds the newest window, or a window that can be closed
            WindowBase* foundW{};
            for (auto& w : gWindowList)
            {
                if (w->flags.has(WindowFlag::dead))
                    continue;
                if (!w->flags.hasAny(WindowFlag::stickToBack, WindowFlag::stickToFront, WindowFlag::noAutoClose))
                {
                    foundW = w.get();
                    break;
                }
            }
            // skip window if window matches specified WindowClass (as user may be modifying via options)
            if (avoid_classification != WindowClass::null && foundW != nullptr
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
        // Collect windows to close first to avoid iterator invalidation
        // when Close() might trigger window creation via OnClose()
        std::vector<WindowBase*> windowsToClose;

        for (auto it = gWindowList.rbegin(); it != gWindowList.rend(); ++it)
        {
            auto& wnd = *(*it);
            if (wnd.flags.has(WindowFlag::dead))
                continue;

            if (pred(&wnd))
            {
                windowsToClose.push_back(&wnd);
                if (flags & WindowCloseFlags::CloseSingle)
                {
                    break;
                }
            }
        }

        // Now close the collected windows
        for (auto* wnd : windowsToClose)
        {
            if (!wnd->flags.has(WindowFlag::dead))
            {
                Close(*wnd);
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
    void CloseByNumber(WindowClass cls, WindowNumber number) override
    {
        CloseByCondition([cls, number](WindowBase* w) -> bool { return w->classification == cls && w->number == number; });
    }

    // TODO: Refactor this to use variant once the new window class is done.
    void CloseByNumber(WindowClass cls, EntityId number) override
    {
        CloseByNumber(cls, static_cast<WindowNumber>(number.ToUnderlying()));
    }

    /**
     * Closes the top-most window
     *
     *  rct2: 0x006E403C
     */
    void CloseTop() override
    {
        CloseByClass(WindowClass::dropdown);

        if (gLegacyScene == LegacyScene::scenarioEditor)
        {
            if (getGameState().editorStep != EditorStep::LandscapeEditor)
                return;
        }

        auto pred = [](WindowBase* w) -> bool { return !w->flags.hasAny(WindowFlag::stickToBack, WindowFlag::stickToFront); };
        CloseByCondition(pred, WindowCloseFlags::CloseSingle);
    }

    /**
     * Closes all open windows
     *
     *  rct2: 0x006EE927
     */
    void CloseAll() override
    {
        CloseByClass(WindowClass::dropdown);
        CloseByCondition(
            [](WindowBase* w) -> bool { return !w->flags.hasAny(WindowFlag::stickToBack, WindowFlag::stickToFront); });
    }

    void CloseAllExceptClass(WindowClass cls) override
    {
        CloseByClass(WindowClass::dropdown);
        CloseByCondition([cls](WindowBase* w) -> bool {
            return w->classification != cls && !w->flags.hasAny(WindowFlag::stickToBack, WindowFlag::stickToFront);
        });
    }

    /**
     * Closes all windows, save for those having any of the passed flags.
     */
    void CloseAllExceptFlags(WindowFlags flags) override
    {
        CloseByCondition([flags](WindowBase* w) -> bool { return !w->flags.hasAny(flags); });
    }

    /**
     * Closes all windows except the specified window number and class.
     */
    void CloseAllExceptNumberAndClass(WindowNumber number, WindowClass cls) override
    {
        CloseByClass(WindowClass::dropdown);
        CloseByCondition([cls, number](WindowBase* w) -> bool {
            return (
                !(w->number == number && w->classification == cls)
                && !w->flags.hasAny(WindowFlag::stickToBack, WindowFlag::stickToFront));
        });
    }

    /**
     *
     *  rct2: 0x006CBCC3
     */
    void CloseConstructionWindows() override
    {
        CloseByClass(WindowClass::rideConstruction);
        CloseByClass(WindowClass::footpath);
        CloseByClass(WindowClass::trackDesignList);
        CloseByClass(WindowClass::trackDesignPlace);
    }

    /**
     * Finds the first window with the specified window class.
     *  rct2: 0x006EA8A0
     * @returns the window or nullptr if no window was found.
     */
    WindowBase* FindByClass(WindowClass cls) override
    {
        for (auto& w : gWindowList)
        {
            if (w->flags.has(WindowFlag::dead))
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
    WindowBase* FindByNumber(WindowClass cls, WindowNumber number) override
    {
        for (auto& w : gWindowList)
        {
            if (w->flags.has(WindowFlag::dead))
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
        return FindByNumber(cls, static_cast<WindowNumber>(id.ToUnderlying()));
    }

    /**
     *
     *  rct2: 0x006EA845
     */
    WindowBase* FindFromPoint(const ScreenCoordsXY& screenCoords) override
    {
        for (auto it = gWindowList.rbegin(); it != gWindowList.rend(); it++)
        {
            auto& w = *it;
            if (w->flags.has(WindowFlag::dead))
                continue;

            if (screenCoords.x < w->windowPos.x || screenCoords.x >= w->windowPos.x + w->width
                || screenCoords.y < w->windowPos.y || screenCoords.y >= w->windowPos.y + w->height)
                continue;

            if (w->flags.has(WindowFlag::noBackground))
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
        w.onPrepareDraw();

        // Find the widget at point x, y
        WidgetIndex widget_index = kWidgetIndexNull;
        for (auto i = 0u; i < w.widgets.size(); i++)
        {
            const auto& widget = w.widgets[i];

            if (widget.type != WidgetType::empty && widget.IsVisible())
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
            if (widget.type == WidgetType::dropdownMenu)
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
                w->invalidate();
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
    void InvalidateByNumber(WindowClass cls, WindowNumber number) override
    {
        InvalidateByCondition([cls, number](WindowBase* w) -> bool { return w->classification == cls && w->number == number; });
    }

    // TODO: Use variant for this once the window framework is done.
    void InvalidateByNumber(WindowClass cls, EntityId id) override
    {
        InvalidateByNumber(cls, static_cast<WindowNumber>(id.ToUnderlying()));
    }

    /**
     * Invalidates all windows.
     */
    void InvalidateAll() override
    {
        WindowVisitEach([](WindowBase* w) { w->invalidate(); });
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

        GfxSetDirtyBlocks(
            { { w.windowPos + ScreenCoordsXY{ widget.left, widget.top } },
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
    void InvalidateWidgetByNumber(WindowClass cls, WindowNumber number, WidgetIndex widgetIndex) override
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
        if (!w.flags.hasAny(WindowFlag::stickToBack, WindowFlag::stickToFront))
        {
            auto itSourcePos = WindowGetIterator(&w);
            if (itSourcePos != gWindowList.end())
            {
                // Insert in front of the first non-stick-to-front window
                auto itDestPos = gWindowList.begin();
                for (auto it = gWindowList.rbegin(); it != gWindowList.rend(); it++)
                {
                    auto& w2 = *it;
                    if (w2->flags.has(WindowFlag::dead))
                    {
                        continue;
                    }
                    if (!w2->flags.has(WindowFlag::stickToFront))
                    {
                        // base() returns the next element in the list, so we need to decrement it.
                        itDestPos = std::prev(it.base());
                        break;
                    }
                }

                if (itSourcePos != itDestPos)
                {
                    std::iter_swap(itSourcePos, itDestPos);
                }
                w.invalidate();

                if (w.windowPos.x + w.width < 20)
                {
                    int32_t i = 20 - w.windowPos.x;
                    w.windowPos.x += i;
                    if (w.viewport != nullptr)
                        w.viewport->pos.x += i;
                    w.invalidate();
                }
            }
        }
        return &w;
    }

    WindowBase* BringToFrontByClass(WindowClass cls) override
    {
        WindowBase* w = FindByClass(cls);
        if (w != nullptr)
        {
            w->flash();
            w->invalidate();
            w = BringToFront(*w);
        }

        return w;
    }

    /**
     *
     *  rct2: 0x006ED78A
     */
    WindowBase* BringToFrontByNumber(WindowClass cls, WindowNumber number) override
    {
        WindowBase* w = FindByNumber(cls, number);
        if (w != nullptr)
        {
            w->flash();
            w->invalidate();
            w = BringToFront(*w);
        }

        return w;
    }
};

std::unique_ptr<IWindowManager> Ui::CreateWindowManager()
{
    return std::make_unique<WindowManager>();
}
