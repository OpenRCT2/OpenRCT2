/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ViewportInteraction.h"

#include "../ProvisionalElements.h"
#include "../UiStringIds.h"
#include "../windows/Windows.h"
#include "Viewport.h"
#include "Window.h"

#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/BalloonPressAction.h>
#include <openrct2/actions/FootpathAdditionRemoveAction.h>
#include <openrct2/actions/FootpathRemoveAction.h>
#include <openrct2/actions/LargeSceneryRemoveAction.h>
#include <openrct2/actions/ParkEntranceRemoveAction.h>
#include <openrct2/actions/SmallSceneryRemoveAction.h>
#include <openrct2/actions/WallRemoveAction.h>
#include <openrct2/entity/Balloon.h>
#include <openrct2/entity/Duck.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/object/BannerSceneryEntry.h>
#include <openrct2/object/LargeSceneryEntry.h>
#include <openrct2/object/ObjectEntryManager.h>
#include <openrct2/object/PathAdditionEntry.h>
#include <openrct2/object/SmallSceneryEntry.h>
#include <openrct2/object/WallSceneryEntry.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/tile_element/BannerElement.h>
#include <openrct2/world/tile_element/EntranceElement.h>
#include <openrct2/world/tile_element/LargeSceneryElement.h>
#include <openrct2/world/tile_element/PathElement.h>
#include <openrct2/world/tile_element/SmallSceneryElement.h>
#include <openrct2/world/tile_element/SurfaceElement.h>
#include <openrct2/world/tile_element/TrackElement.h>
#include <openrct2/world/tile_element/WallElement.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui::Windows;

namespace OpenRCT2::Ui
{
    static void ViewportInteractionRemoveScenery(const SmallSceneryElement& smallSceneryElement, const CoordsXY& mapCoords);
    static void ViewportInteractionRemoveFootpath(const PathElement& pathElement, const CoordsXY& mapCoords);
    static void ViewportInteractionRemovePathAddition(const PathElement& pathElement, const CoordsXY& mapCoords);
    static void ViewportInteractionRemoveParkWall(const WallElement& wallElement, const CoordsXY& mapCoords);
    static void ViewportInteractionRemoveLargeScenery(
        const LargeSceneryElement& largeSceneryElement, const CoordsXY& mapCoords);
    static void ViewportInteractionRemoveParkEntrance(const EntranceElement& entranceElement, CoordsXY mapCoords);
    static Peep* ViewportInteractionGetClosestPeep(ScreenCoordsXY screenCoords, int32_t maxDistance);

    /**
     *
     *  rct2: 0x006ED9D0
     */
    static InteractionInfo ViewportInteractionGetItemLeft(const ScreenCoordsXY& screenCoords)
    {
        InteractionInfo info{};
        // No click input for scenario editor or track manager
        if (gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER))
            return info;

        //
        if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && GetGameState().EditorStep != EditorStep::RollercoasterDesigner)
            return info;

        info = GetMapCoordinatesFromPos(
            screenCoords,
            EnumsToFlags(
                ViewportInteractionItem::Entity, ViewportInteractionItem::Ride, ViewportInteractionItem::ParkEntrance));
        auto tileElement = info.interactionType != ViewportInteractionItem::Entity ? info.Element : nullptr;
        // Only valid when info.interactionType == ViewportInteractionItem::Entity, but can't assign nullptr without compiler
        // complaining
        auto sprite = info.Entity;

        // Allows only balloons to be popped and ducks to be quacked in title screen
        if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        {
            if (info.interactionType == ViewportInteractionItem::Entity && (sprite->Is<Balloon>() || sprite->Is<Duck>()))
                return info;

            info.interactionType = ViewportInteractionItem::None;
            return info;
        }

        switch (info.interactionType)
        {
            case ViewportInteractionItem::Entity:
                switch (sprite->Type)
                {
                    case EntityType::Vehicle:
                    {
                        auto vehicle = sprite->As<Vehicle>();
                        if (vehicle != nullptr && !vehicle->IsCableLift())
                            vehicle->SetMapToolbar();
                        else
                            info.interactionType = ViewportInteractionItem::None;
                    }
                    break;
                    case EntityType::Guest:
                    case EntityType::Staff:
                    {
                        auto peep = sprite->As<Peep>();
                        if (peep != nullptr)
                        {
                            PeepSetMapTooltip(peep);
                        }
                        else
                        {
                            info.interactionType = ViewportInteractionItem::None;
                        }
                    }
                    break;
                    default:
                        break;
                }
                break;
            case ViewportInteractionItem::Ride:
                Guard::ArgumentNotNull(tileElement);
                RideSetMapTooltip(*tileElement);
                break;
            case ViewportInteractionItem::ParkEntrance:
            {
                auto& gameState = GetGameState();
                auto parkName = gameState.Park.Name.c_str();

                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(parkName);
                SetMapTooltip(ft);
                break;
            }
            default:
                info.interactionType = ViewportInteractionItem::None;
                break;
        }

        // If nothing is under cursor, find a close by peep
        if (info.interactionType == ViewportInteractionItem::None)
        {
            auto peep = ViewportInteractionGetClosestPeep(screenCoords, 32);
            if (peep != nullptr)
            {
                info.Entity = peep;
                info.interactionType = ViewportInteractionItem::Entity;
                info.Loc.x = peep->x;
                info.Loc.y = peep->y;
                PeepSetMapTooltip(peep);
            }
        }

        return info;
    }

    bool ViewportInteractionLeftOver(const ScreenCoordsXY& screenCoords)
    {
        auto info = ViewportInteractionGetItemLeft(screenCoords);

        switch (info.interactionType)
        {
            case ViewportInteractionItem::Entity:
            case ViewportInteractionItem::Ride:
            case ViewportInteractionItem::ParkEntrance:
                return true;
            default:
                return false;
        }
    }

    bool ViewportInteractionLeftClick(const ScreenCoordsXY& screenCoords)
    {
        auto info = ViewportInteractionGetItemLeft(screenCoords);

        switch (info.interactionType)
        {
            case ViewportInteractionItem::Entity:
            {
                auto entity = info.Entity;
                switch (entity->Type)
                {
                    case EntityType::Vehicle:
                    {
                        auto intent = Intent(WD_VEHICLE);
                        intent.PutExtra(INTENT_EXTRA_VEHICLE, entity);
                        ContextOpenIntent(&intent);
                        break;
                    }
                    case EntityType::Guest:
                    case EntityType::Staff:
                    {
                        auto intent = Intent(WindowClass::Peep);
                        intent.PutExtra(INTENT_EXTRA_PEEP, entity);
                        ContextOpenIntent(&intent);
                        break;
                    }
                    case EntityType::Balloon:
                    {
                        if (GameIsNotPaused())
                        {
                            auto balloonPress = BalloonPressAction(entity->Id);
                            GameActions::Execute(&balloonPress);
                        }
                    }
                    break;
                    case EntityType::Duck:
                    {
                        if (GameIsNotPaused())
                        {
                            auto duck = entity->As<Duck>();
                            if (duck != nullptr)
                            {
                                duck->Press();
                            }
                        }
                    }
                    break;
                    default:
                        break;
                }
                return true;
            }
            case ViewportInteractionItem::Ride:
            {
                auto intent = Intent(WD_TRACK);
                intent.PutExtra(INTENT_EXTRA_TILE_ELEMENT, info.Element);
                ContextOpenIntent(&intent);
                return true;
            }
            case ViewportInteractionItem::ParkEntrance:
                ContextOpenWindow(WindowClass::ParkInformation);
                return true;
            default:
                return false;
        }
    }

    /**
     *
     *  rct2: 0x006EDE88
     */
    static InteractionInfo ViewportInteractionGetItemRight(const ScreenCoordsXY& screenCoords)
    {
        Ride* ride;
        int32_t i;
        InteractionInfo info{};
        // No click input for title screen or track manager
        if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_TRACK_MANAGER))
            return info;

        //
        if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && GetGameState().EditorStep != EditorStep::RollercoasterDesigner)
            return info;

        constexpr auto flags = static_cast<int32_t>(
            ~EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Water));
        info = GetMapCoordinatesFromPos(screenCoords, flags);
        auto tileElement = info.Element;

        switch (info.interactionType)
        {
            case ViewportInteractionItem::Entity:
            {
                auto sprite = info.Entity;
                if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || sprite->Type != EntityType::Vehicle)
                {
                    info.interactionType = ViewportInteractionItem::None;
                    return info;
                }

                auto vehicle = sprite->As<Vehicle>();
                if (vehicle == nullptr)
                {
                    info.interactionType = ViewportInteractionItem::None;
                    return info;
                }
                ride = GetRide(vehicle->ride);
                if (ride != nullptr && ride->status == RideStatus::Closed)
                {
                    auto ft = Formatter();
                    ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
                    ride->FormatNameTo(ft);
                    SetMapTooltip(ft);
                }
                return info;
            }
            case ViewportInteractionItem::Ride:
            {
                if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                {
                    info.interactionType = ViewportInteractionItem::None;
                    return info;
                }
                if (tileElement->GetType() == TileElementType::Path)
                {
                    info.interactionType = ViewportInteractionItem::None;
                    return info;
                }

                ride = GetRide(tileElement->GetRideIndex());
                if (ride == nullptr)
                {
                    info.interactionType = ViewportInteractionItem::None;
                    return info;
                }

                if (ride->status != RideStatus::Closed)
                    return info;

                auto ft = Formatter();
                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);

                if (tileElement->GetType() == TileElementType::Entrance)
                {
                    StringId stringId;
                    if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
                    {
                        if (ride->num_stations > 1)
                        {
                            stringId = STR_RIDE_STATION_X_ENTRANCE;
                        }
                        else
                        {
                            stringId = STR_RIDE_ENTRANCE;
                        }
                    }
                    else
                    {
                        if (ride->num_stations > 1)
                        {
                            stringId = STR_RIDE_STATION_X_EXIT;
                        }
                        else
                        {
                            stringId = STR_RIDE_EXIT;
                        }
                    }
                    ft.Add<StringId>(stringId);
                }
                else if (tileElement->AsTrack()->IsStation())
                {
                    StringId stringId;
                    if (ride->num_stations > 1)
                    {
                        stringId = STR_RIDE_STATION_X;
                    }
                    else
                    {
                        stringId = STR_RIDE_STATION;
                    }
                    ft.Add<StringId>(stringId);
                }
                else
                {
                    // FIXME: Why does it *2 the value?
                    if (!GetGameState().Cheats.sandboxMode && !MapIsLocationOwned({ info.Loc, tileElement->GetBaseZ() * 2 }))
                    {
                        info.interactionType = ViewportInteractionItem::None;
                        return info;
                    }

                    ride->FormatNameTo(ft);
                    return info;
                }

                ride->FormatNameTo(ft);

                const auto& rtd = ride->GetRideTypeDescriptor();
                ft.Add<StringId>(GetRideComponentName(rtd.NameConvention.station).capitalised);

                StationIndex::UnderlyingType stationIndex;
                if (tileElement->GetType() == TileElementType::Entrance)
                    stationIndex = tileElement->AsEntrance()->GetStationIndex().ToUnderlying();
                else
                    stationIndex = tileElement->AsTrack()->GetStationIndex().ToUnderlying();

                for (i = stationIndex; i >= 0; i--)
                    if (ride->GetStations()[i].Start.IsNull())
                        stationIndex--;
                stationIndex++;
                ft.Add<uint16_t>(stationIndex);
                SetMapTooltip(ft);
                return info;
            }
            case ViewportInteractionItem::Wall:
            {
                auto* wallEntry = tileElement->AsWall()->GetEntry();
                if (wallEntry->scrolling_mode != SCROLLING_MODE_NONE)
                {
                    auto banner = tileElement->AsWall()->GetBanner();
                    if (banner != nullptr)
                    {
                        auto ft = Formatter();
                        ft.Add<StringId>(STR_MAP_TOOLTIP_BANNER_STRINGID_STRINGID);
                        banner->FormatTextTo(ft);
                        ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
                        ft.Add<StringId>(wallEntry->name);
                        SetMapTooltip(ft);
                        return info;
                    }
                }
                break;
            }
            case ViewportInteractionItem::LargeScenery:
            {
                auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
                if (sceneryEntry->scrolling_mode != SCROLLING_MODE_NONE)
                {
                    auto banner = tileElement->AsLargeScenery()->GetBanner();
                    if (banner != nullptr)
                    {
                        auto ft = Formatter();
                        ft.Add<StringId>(STR_MAP_TOOLTIP_BANNER_STRINGID_STRINGID);
                        banner->FormatTextTo(ft);
                        ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
                        ft.Add<StringId>(sceneryEntry->name);
                        SetMapTooltip(ft);
                        return info;
                    }
                }
                break;
            }
            case ViewportInteractionItem::Banner:
            {
                auto banner = tileElement->AsBanner()->GetBanner();
                if (banner != nullptr)
                {
                    auto* bannerEntry = ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);

                    auto ft = Formatter();
                    ft.Add<StringId>(STR_MAP_TOOLTIP_BANNER_STRINGID_STRINGID);
                    banner->FormatTextTo(ft, /*addColour*/ true);
                    ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
                    ft.Add<StringId>(bannerEntry->name);
                    SetMapTooltip(ft);
                    return info;
                }
                break;
            }
            default:
                break;
        }

        if (!(InputTestFlag(INPUT_FLAG_6)) || !(InputTestFlag(INPUT_FLAG_TOOL_ACTIVE)))
        {
            auto* windowMgr = GetWindowManager();
            if (windowMgr->FindByClass(WindowClass::RideConstruction) == nullptr
                && windowMgr->FindByClass(WindowClass::Footpath) == nullptr)
            {
                info.interactionType = ViewportInteractionItem::None;
                return info;
            }
        }

        auto ft = Formatter();
        switch (info.interactionType)
        {
            case ViewportInteractionItem::Scenery:
            {
                auto* sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
                ft.Add<StringId>(sceneryEntry->name);
                SetMapTooltip(ft);
                return info;
            }
            case ViewportInteractionItem::Footpath:
                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
                if (tileElement->AsPath()->IsQueue())
                    ft.Add<StringId>(STR_QUEUE_LINE_MAP_TIP);
                else
                    ft.Add<StringId>(STR_FOOTPATH_MAP_TIP);
                SetMapTooltip(ft);
                return info;

            case ViewportInteractionItem::PathAddition:
            {
                auto* pathAddEntry = tileElement->AsPath()->GetAdditionEntry();
                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
                if (tileElement->AsPath()->IsBroken())
                {
                    ft.Add<StringId>(STR_BROKEN);
                }
                ft.Add<StringId>(pathAddEntry != nullptr ? pathAddEntry->name : kStringIdNone);
                SetMapTooltip(ft);
                return info;
            }
            case ViewportInteractionItem::ParkEntrance:
                if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !GetGameState().Cheats.sandboxMode)
                    break;

                if (tileElement->GetType() != TileElementType::Entrance)
                    break;

                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
                ft.Add<StringId>(STR_OBJECT_SELECTION_PARK_ENTRANCE);
                SetMapTooltip(ft);
                return info;

            case ViewportInteractionItem::Wall:
            {
                auto* wallEntry = tileElement->AsWall()->GetEntry();
                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
                ft.Add<StringId>(wallEntry->name);
                SetMapTooltip(ft);
                return info;
            }
            case ViewportInteractionItem::LargeScenery:
            {
                auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
                ft.Add<StringId>(sceneryEntry->name);
                SetMapTooltip(ft);
                return info;
            }
            default:
                break;
        }

        info.interactionType = ViewportInteractionItem::None;
        return info;
    }

    bool ViewportInteractionRightOver(const ScreenCoordsXY& screenCoords)
    {
        auto info = ViewportInteractionGetItemRight(screenCoords);

        return info.interactionType != ViewportInteractionItem::None;
    }

    /**
     *
     *  rct2: 0x006E8A62
     */
    bool ViewportInteractionRightClick(const ScreenCoordsXY& screenCoords)
    {
        CoordsXYE tileElement;
        auto info = ViewportInteractionGetItemRight(screenCoords);

        switch (info.interactionType)
        {
            case ViewportInteractionItem::None:
            case ViewportInteractionItem::Terrain:
            case ViewportInteractionItem::Water:
            case ViewportInteractionItem::Label:
                return false;

            case ViewportInteractionItem::Entity:
            {
                auto entity = info.Entity;
                if (entity->Type == EntityType::Vehicle)
                {
                    auto vehicle = entity->As<Vehicle>();
                    if (vehicle == nullptr)
                    {
                        break;
                    }
                    auto ride = GetRide(vehicle->ride);
                    if (ride != nullptr)
                    {
                        RideConstructionStart(*ride);
                    }
                }
            }
            break;
            case ViewportInteractionItem::Ride:
                tileElement = { info.Loc, info.Element };
                RideModify(tileElement);
                break;
            case ViewportInteractionItem::Scenery:
                ViewportInteractionRemoveScenery(*info.Element->AsSmallScenery(), info.Loc);
                break;
            case ViewportInteractionItem::Footpath:
                ViewportInteractionRemoveFootpath(*info.Element->AsPath(), info.Loc);
                break;
            case ViewportInteractionItem::PathAddition:
                ViewportInteractionRemovePathAddition(*info.Element->AsPath(), info.Loc);
                break;
            case ViewportInteractionItem::ParkEntrance:
                ViewportInteractionRemoveParkEntrance(*info.Element->AsEntrance(), info.Loc);
                break;
            case ViewportInteractionItem::Wall:
                ViewportInteractionRemoveParkWall(*info.Element->AsWall(), info.Loc);
                break;
            case ViewportInteractionItem::LargeScenery:
                ViewportInteractionRemoveLargeScenery(*info.Element->AsLargeScenery(), info.Loc);
                break;
            case ViewportInteractionItem::Banner:
                ContextOpenDetailWindow(WD_BANNER, info.Element->AsBanner()->GetIndex().ToUnderlying());
                break;
        }

        return true;
    }

    /**
     *
     *  rct2: 0x006E08D2
     */
    static void ViewportInteractionRemoveScenery(const SmallSceneryElement& smallSceneryElement, const CoordsXY& mapCoords)
    {
        auto removeSceneryAction = SmallSceneryRemoveAction(
            { mapCoords.x, mapCoords.y, smallSceneryElement.GetBaseZ() }, smallSceneryElement.GetSceneryQuadrant(),
            smallSceneryElement.GetEntryIndex());

        GameActions::Execute(&removeSceneryAction);
    }

    /**
     *
     *  rct2: 0x006A614A
     */
    static void ViewportInteractionRemoveFootpath(const PathElement& pathElement, const CoordsXY& mapCoords)
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::Footpath);
        if (w != nullptr)
            FootpathUpdateProvisional();

        TileElement* tileElement2 = MapGetFirstElementAt(mapCoords);
        if (tileElement2 == nullptr)
            return;

        auto z = pathElement.GetBaseZ();
        do
        {
            if (tileElement2->GetType() == TileElementType::Path && tileElement2->GetBaseZ() == z)
            {
                auto action = FootpathRemoveAction({ mapCoords, z });
                GameActions::Execute(&action);
                break;
            }
        } while (!(tileElement2++)->IsLastForTile());
    }

    /**
     *
     *  rct2: 0x006A61AB
     */
    static void ViewportInteractionRemovePathAddition(const PathElement& pathElement, const CoordsXY& mapCoords)
    {
        auto footpathAdditionRemoveAction = FootpathAdditionRemoveAction({ mapCoords.x, mapCoords.y, pathElement.GetBaseZ() });
        GameActions::Execute(&footpathAdditionRemoveAction);
    }

    /**
     *
     *  rct2: 0x00666C0E
     */
    void ViewportInteractionRemoveParkEntrance(const EntranceElement& entranceElement, CoordsXY mapCoords)
    {
        int32_t rotation = entranceElement.GetDirectionWithOffset(1);
        switch (entranceElement.GetSequenceIndex())
        {
            case 1:
                mapCoords += CoordsDirectionDelta[rotation];
                break;
            case 2:
                mapCoords -= CoordsDirectionDelta[rotation];
                break;
        }
        auto parkEntranceRemoveAction = ParkEntranceRemoveAction({ mapCoords.x, mapCoords.y, entranceElement.GetBaseZ() });
        GameActions::Execute(&parkEntranceRemoveAction);
    }

    /**
     *
     *  rct2: 0x006E57A9
     */
    static void ViewportInteractionRemoveParkWall(const WallElement& wallElement, const CoordsXY& mapCoords)
    {
        auto* wallEntry = wallElement.GetEntry();
        if (wallEntry->scrolling_mode != SCROLLING_MODE_NONE)
        {
            ContextOpenDetailWindow(WD_SIGN_SMALL, wallElement.GetBannerIndex().ToUnderlying());
        }
        else
        {
            CoordsXYZD wallLocation = { mapCoords.x, mapCoords.y, wallElement.GetBaseZ(), wallElement.GetDirection() };
            auto wallRemoveAction = WallRemoveAction(wallLocation);
            GameActions::Execute(&wallRemoveAction);
        }
    }

    /**
     *
     *  rct2: 0x006B88DC
     */
    static void ViewportInteractionRemoveLargeScenery(const LargeSceneryElement& largeSceneryElement, const CoordsXY& mapCoords)
    {
        auto* sceneryEntry = largeSceneryElement.GetEntry();

        if (sceneryEntry->scrolling_mode != SCROLLING_MODE_NONE)
        {
            auto bannerIndex = largeSceneryElement.GetBannerIndex();
            ContextOpenDetailWindow(WD_SIGN, bannerIndex.ToUnderlying());
        }
        else
        {
            auto removeSceneryAction = LargeSceneryRemoveAction(
                { mapCoords.x, mapCoords.y, largeSceneryElement.GetBaseZ(), largeSceneryElement.GetDirection() },
                largeSceneryElement.GetSequenceIndex());
            GameActions::Execute(&removeSceneryAction);
        }
    }

    struct PeepDistance
    {
        Peep* peep = nullptr;
        int32_t distance = std::numeric_limits<decltype(distance)>::max();
    };

    template<typename T>
    static PeepDistance GetClosestPeep(
        const ScreenCoordsXY& viewportCoords, uint8_t rotation, const int32_t maxDistance, PeepDistance goal)
    {
        for (auto peep : EntityList<T>())
        {
            if (peep->x == kLocationNull)
                continue;

            auto screenCoords = Translate3DTo2DWithZ(rotation, peep->GetLocation());
            auto spriteRect = ScreenRect(
                screenCoords - ScreenCoordsXY{ peep->SpriteData.Width, peep->SpriteData.HeightMin },
                screenCoords + ScreenCoordsXY{ peep->SpriteData.Width, peep->SpriteData.HeightMax });

            auto distance = abs(((spriteRect.GetLeft() + spriteRect.GetRight()) / 2) - viewportCoords.x)
                + abs(((spriteRect.GetTop() + spriteRect.GetBottom()) / 2) - viewportCoords.y);
            if (distance > maxDistance)
                continue;

            if (distance < goal.distance)
            {
                goal.peep = peep;
                goal.distance = distance;
            }
        }
        return goal;
    }

    static Peep* ViewportInteractionGetClosestPeep(ScreenCoordsXY screenCoords, int32_t maxDistance)
    {
        auto* windowMgr = GetWindowManager();
        auto* w = windowMgr->FindFromPoint(screenCoords);
        if (w == nullptr)
            return nullptr;

        auto* viewport = w->viewport;
        if (viewport == nullptr || viewport->zoom >= ZoomLevel{ 2 })
            return nullptr;

        auto viewportCoords = viewport->ScreenToViewportCoord(screenCoords);

        PeepDistance goal;
        if (!(viewport->flags & VIEWPORT_FLAG_HIDE_GUESTS))
            goal = GetClosestPeep<Guest>(viewportCoords, viewport->rotation, maxDistance, goal);
        if (!(viewport->flags & VIEWPORT_FLAG_HIDE_STAFF))
            goal = GetClosestPeep<Staff>(viewportCoords, viewport->rotation, maxDistance, goal);
        return goal.peep;
    }

    /**
     *
     *  rct2: 0x0068A15E
     */
    CoordsXY ViewportInteractionGetTileStartAtCursor(const ScreenCoordsXY& screenCoords)
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* window = windowMgr->FindFromPoint(screenCoords);
        if (window == nullptr || window->viewport == nullptr)
        {
            CoordsXY ret{};
            ret.SetNull();
            return ret;
        }
        auto viewport = window->viewport;
        auto info = GetMapCoordinatesFromPosWindow(
            window, screenCoords, EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Water));
        auto initialPos = info.Loc;

        if (info.interactionType == ViewportInteractionItem::None)
        {
            initialPos.SetNull();
            return initialPos;
        }

        int16_t waterHeight = 0;
        if (info.interactionType == ViewportInteractionItem::Water)
        {
            waterHeight = info.Element->AsSurface()->GetWaterHeight();
        }

        auto initialVPPos = viewport->ScreenToViewportCoord(screenCoords);
        CoordsXY mapPos = initialPos + CoordsXY{ 16, 16 };

        for (int32_t i = 0; i < 5; i++)
        {
            int16_t z = waterHeight;
            if (info.interactionType != ViewportInteractionItem::Water)
            {
                z = TileElementHeight(mapPos);
            }
            mapPos = ViewportPosToMapPos(initialVPPos, z, viewport->rotation);
            mapPos.x = std::clamp(mapPos.x, initialPos.x, initialPos.x + 31);
            mapPos.y = std::clamp(mapPos.y, initialPos.y, initialPos.y + 31);
        }

        return mapPos.ToTileStart();
    }
} // namespace OpenRCT2::Ui
