/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/entity/EntityList.h>
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
        if (gLegacyScene == LegacyScene::scenarioEditor || gLegacyScene == LegacyScene::trackDesignsManager)
            return info;

        //
        if (gLegacyScene == LegacyScene::trackDesigner && getGameState().editorStep != EditorStep::RollercoasterDesigner)
            return info;

        info = GetMapCoordinatesFromPos(
            screenCoords,
            EnumsToFlags(
                ViewportInteractionItem::entity, ViewportInteractionItem::ride, ViewportInteractionItem::parkEntrance));
        auto tileElement = info.interactionType != ViewportInteractionItem::entity ? info.Element : nullptr;
        // Only valid when info.interactionType == ViewportInteractionItem::entity, but can't assign nullptr without compiler
        // complaining
        auto sprite = info.Entity;

        // Allows only balloons to be popped and ducks to be quacked in title screen
        if (gLegacyScene == LegacyScene::titleSequence)
        {
            if (info.interactionType == ViewportInteractionItem::entity && (sprite->Is<Balloon>() || sprite->Is<Duck>()))
                return info;

            info.interactionType = ViewportInteractionItem::none;
            return info;
        }

        switch (info.interactionType)
        {
            case ViewportInteractionItem::entity:
                switch (sprite->Type)
                {
                    case EntityType::vehicle:
                    {
                        auto vehicle = sprite->As<Vehicle>();
                        if (vehicle != nullptr && !vehicle->IsCableLift())
                            vehicle->SetMapToolbar();
                        else
                            info.interactionType = ViewportInteractionItem::none;
                    }
                    break;
                    case EntityType::guest:
                    case EntityType::staff:
                    {
                        auto peep = sprite->As<Peep>();
                        if (peep != nullptr)
                        {
                            PeepSetMapTooltip(peep);
                        }
                        else
                        {
                            info.interactionType = ViewportInteractionItem::none;
                        }
                    }
                    break;
                    default:
                        break;
                }
                break;
            case ViewportInteractionItem::ride:
                Guard::ArgumentNotNull(tileElement);
                RideSetMapTooltip(*tileElement);
                break;
            case ViewportInteractionItem::parkEntrance:
            {
                auto& gameState = getGameState();
                auto parkName = gameState.park.name.c_str();

                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(parkName);
                SetMapTooltip(ft);
                break;
            }
            default:
                info.interactionType = ViewportInteractionItem::none;
                break;
        }

        // If nothing is under cursor, find a close by peep
        if (info.interactionType == ViewportInteractionItem::none)
        {
            auto peep = ViewportInteractionGetClosestPeep(screenCoords, 32);
            if (peep != nullptr)
            {
                info.Entity = peep;
                info.interactionType = ViewportInteractionItem::entity;
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
            case ViewportInteractionItem::entity:
            case ViewportInteractionItem::ride:
            case ViewportInteractionItem::parkEntrance:
                return true;
            default:
                return false;
        }
    }

    bool ViewportInteractionLeftClick(const ScreenCoordsXY& screenCoords)
    {
        auto info = ViewportInteractionGetItemLeft(screenCoords);
        auto& gameState = getGameState();

        switch (info.interactionType)
        {
            case ViewportInteractionItem::entity:
            {
                auto entity = info.Entity;
                switch (entity->Type)
                {
                    case EntityType::vehicle:
                    {
                        auto intent = Intent(WindowDetail::vehicle);
                        intent.PutExtra(INTENT_EXTRA_VEHICLE, entity);
                        ContextOpenIntent(&intent);
                        break;
                    }
                    case EntityType::guest:
                    case EntityType::staff:
                    {
                        auto intent = Intent(WindowClass::peep);
                        intent.PutExtra(INTENT_EXTRA_PEEP, entity);
                        ContextOpenIntent(&intent);
                        break;
                    }
                    case EntityType::balloon:
                    {
                        if (GameIsNotPaused())
                        {
                            auto balloonPress = GameActions::BalloonPressAction(entity->Id);
                            GameActions::Execute(&balloonPress, gameState);
                        }
                    }
                    break;
                    case EntityType::duck:
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
            case ViewportInteractionItem::ride:
            {
                auto intent = Intent(WindowDetail::track);
                intent.PutExtra(INTENT_EXTRA_TILE_ELEMENT, info.Element);
                ContextOpenIntent(&intent);
                return true;
            }
            case ViewportInteractionItem::parkEntrance:
                ContextOpenWindow(WindowClass::parkInformation);
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
        InteractionInfo info{};
        // No click input for title screen or track manager
        if (gLegacyScene == LegacyScene::titleSequence || gLegacyScene == LegacyScene::trackDesignsManager)
            return info;

        //
        if (gLegacyScene == LegacyScene::trackDesigner && getGameState().editorStep != EditorStep::RollercoasterDesigner)
            return info;

        constexpr auto flags = static_cast<int32_t>(
            ~EnumsToFlags(ViewportInteractionItem::terrain, ViewportInteractionItem::water));
        info = GetMapCoordinatesFromPos(screenCoords, flags);
        auto tileElement = info.Element;

        switch (info.interactionType)
        {
            case ViewportInteractionItem::entity:
            {
                auto sprite = info.Entity;
                if (gLegacyScene == LegacyScene::scenarioEditor || sprite->Type != EntityType::vehicle)
                {
                    info.interactionType = ViewportInteractionItem::none;
                    return info;
                }

                auto vehicle = sprite->As<Vehicle>();
                if (vehicle == nullptr)
                {
                    info.interactionType = ViewportInteractionItem::none;
                    return info;
                }
                ride = GetRide(vehicle->ride);
                if (ride != nullptr && ride->status == RideStatus::closed)
                {
                    auto ft = Formatter();
                    ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
                    ride->formatNameTo(ft);
                    SetMapTooltip(ft);
                }
                return info;
            }
            case ViewportInteractionItem::ride:
            {
                if (gLegacyScene == LegacyScene::scenarioEditor)
                {
                    info.interactionType = ViewportInteractionItem::none;
                    return info;
                }
                if (tileElement->GetType() == TileElementType::Path)
                {
                    info.interactionType = ViewportInteractionItem::none;
                    return info;
                }

                ride = GetRide(tileElement->GetRideIndex());
                if (ride == nullptr)
                {
                    info.interactionType = ViewportInteractionItem::none;
                    return info;
                }

                if (ride->status != RideStatus::closed)
                    return info;

                auto ft = Formatter();
                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);

                if (tileElement->GetType() == TileElementType::Entrance)
                {
                    StringId stringId;
                    if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
                    {
                        if (ride->numStations > 1)
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
                        if (ride->numStations > 1)
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
                    if (ride->numStations > 1)
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
                    if (!getGameState().cheats.sandboxMode && !MapIsLocationOwned({ info.Loc, tileElement->GetBaseZ() * 2 }))
                    {
                        info.interactionType = ViewportInteractionItem::none;
                        return info;
                    }

                    ride->formatNameTo(ft);
                    return info;
                }

                ride->formatNameTo(ft);

                const auto& rtd = ride->getRideTypeDescriptor();
                ft.Add<StringId>(GetRideComponentName(rtd.NameConvention.station).capitalised);

                StationIndex::UnderlyingType stationIndex;
                if (tileElement->GetType() == TileElementType::Entrance)
                    stationIndex = tileElement->AsEntrance()->GetStationIndex().ToUnderlying();
                else
                    stationIndex = tileElement->AsTrack()->GetStationIndex().ToUnderlying();

                for (int32_t i = stationIndex; i >= 0; i--)
                    if (ride->getStations()[i].Start.IsNull())
                        stationIndex--;
                stationIndex++;
                ft.Add<uint16_t>(stationIndex);
                SetMapTooltip(ft);
                return info;
            }
            case ViewportInteractionItem::wall:
            {
                auto* wallEntry = tileElement->AsWall()->GetEntry();
                if (wallEntry->scrolling_mode != kScrollingModeNone)
                {
                    auto banner = tileElement->AsWall()->GetBanner();
                    if (banner != nullptr)
                    {
                        auto ft = Formatter();
                        ft.Add<StringId>(STR_MAP_TOOLTIP_BANNER_STRINGID_STRINGID);
                        banner->formatTextTo(ft);
                        ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
                        ft.Add<StringId>(wallEntry->name);
                        SetMapTooltip(ft);
                        return info;
                    }
                }
                break;
            }
            case ViewportInteractionItem::largeScenery:
            {
                auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
                if (sceneryEntry->scrolling_mode != kScrollingModeNone)
                {
                    auto banner = tileElement->AsLargeScenery()->GetBanner();
                    if (banner != nullptr)
                    {
                        auto ft = Formatter();
                        ft.Add<StringId>(STR_MAP_TOOLTIP_BANNER_STRINGID_STRINGID);
                        banner->formatTextTo(ft);
                        ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
                        ft.Add<StringId>(sceneryEntry->name);
                        SetMapTooltip(ft);
                        return info;
                    }
                }
                break;
            }
            case ViewportInteractionItem::banner:
            {
                auto banner = tileElement->AsBanner()->GetBanner();
                if (banner != nullptr)
                {
                    auto* bannerEntry = ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);

                    auto ft = Formatter();
                    ft.Add<StringId>(STR_MAP_TOOLTIP_BANNER_STRINGID_STRINGID);
                    banner->formatTextWithColourTo(ft);
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

        if (!gInputFlags.has(InputFlag::allowRightMouseRemoval) || !gInputFlags.has(InputFlag::toolActive))
        {
            auto* windowMgr = GetWindowManager();
            if (windowMgr->FindByClass(WindowClass::rideConstruction) == nullptr
                && windowMgr->FindByClass(WindowClass::footpath) == nullptr)
            {
                info.interactionType = ViewportInteractionItem::none;
                return info;
            }
        }

        auto ft = Formatter();
        switch (info.interactionType)
        {
            case ViewportInteractionItem::scenery:
            {
                auto* sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
                ft.Add<StringId>(sceneryEntry->name);
                SetMapTooltip(ft);
                return info;
            }
            case ViewportInteractionItem::footpath:
                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
                if (tileElement->AsPath()->IsQueue())
                    ft.Add<StringId>(STR_QUEUE_LINE_MAP_TIP);
                else
                    ft.Add<StringId>(STR_FOOTPATH_MAP_TIP);
                SetMapTooltip(ft);
                return info;

            case ViewportInteractionItem::pathAddition:
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
            case ViewportInteractionItem::parkEntrance:
                if (gLegacyScene != LegacyScene::scenarioEditor && !getGameState().cheats.sandboxMode)
                    break;

                if (tileElement->GetType() != TileElementType::Entrance)
                    break;

                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
                ft.Add<StringId>(STR_OBJECT_SELECTION_PARK_ENTRANCE);
                SetMapTooltip(ft);
                return info;

            case ViewportInteractionItem::wall:
            {
                auto* wallEntry = tileElement->AsWall()->GetEntry();
                ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
                ft.Add<StringId>(wallEntry->name);
                SetMapTooltip(ft);
                return info;
            }
            case ViewportInteractionItem::largeScenery:
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

        info.interactionType = ViewportInteractionItem::none;
        return info;
    }

    bool ViewportInteractionRightOver(const ScreenCoordsXY& screenCoords)
    {
        auto info = ViewportInteractionGetItemRight(screenCoords);

        return info.interactionType != ViewportInteractionItem::none;
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
            case ViewportInteractionItem::none:
            case ViewportInteractionItem::terrain:
            case ViewportInteractionItem::water:
            case ViewportInteractionItem::label:
                return false;

            case ViewportInteractionItem::entity:
            {
                auto entity = info.Entity;
                if (entity->Type == EntityType::vehicle)
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
            case ViewportInteractionItem::ride:
                tileElement = { info.Loc, info.Element };
                RideModify(tileElement);
                break;
            case ViewportInteractionItem::scenery:
                ViewportInteractionRemoveScenery(*info.Element->AsSmallScenery(), info.Loc);
                break;
            case ViewportInteractionItem::footpath:
                ViewportInteractionRemoveFootpath(*info.Element->AsPath(), info.Loc);
                break;
            case ViewportInteractionItem::pathAddition:
                ViewportInteractionRemovePathAddition(*info.Element->AsPath(), info.Loc);
                break;
            case ViewportInteractionItem::parkEntrance:
                ViewportInteractionRemoveParkEntrance(*info.Element->AsEntrance(), info.Loc);
                break;
            case ViewportInteractionItem::wall:
                ViewportInteractionRemoveParkWall(*info.Element->AsWall(), info.Loc);
                break;
            case ViewportInteractionItem::largeScenery:
                ViewportInteractionRemoveLargeScenery(*info.Element->AsLargeScenery(), info.Loc);
                break;
            case ViewportInteractionItem::banner:
                ContextOpenDetailWindow(WindowDetail::banner, info.Element->AsBanner()->GetIndex().ToUnderlying());
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
        auto removeSceneryAction = GameActions::SmallSceneryRemoveAction(
            { mapCoords.x, mapCoords.y, smallSceneryElement.GetBaseZ() }, smallSceneryElement.GetSceneryQuadrant(),
            smallSceneryElement.GetEntryIndex());

        GameActions::Execute(&removeSceneryAction, getGameState());
    }

    /**
     *
     *  rct2: 0x006A614A
     */
    static void ViewportInteractionRemoveFootpath(const PathElement& pathElement, const CoordsXY& mapCoords)
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::footpath);
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
                auto action = GameActions::FootpathRemoveAction({ mapCoords, z });
                GameActions::Execute(&action, getGameState());
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
        auto footpathAdditionRemoveAction = GameActions::FootpathAdditionRemoveAction(
            { mapCoords.x, mapCoords.y, pathElement.GetBaseZ() });
        GameActions::Execute(&footpathAdditionRemoveAction, getGameState());
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
        auto parkEntranceRemoveAction = GameActions::ParkEntranceRemoveAction(
            { mapCoords.x, mapCoords.y, entranceElement.GetBaseZ() });
        GameActions::Execute(&parkEntranceRemoveAction, getGameState());
    }

    /**
     *
     *  rct2: 0x006E57A9
     */
    static void ViewportInteractionRemoveParkWall(const WallElement& wallElement, const CoordsXY& mapCoords)
    {
        auto* wallEntry = wallElement.GetEntry();
        if (wallEntry->scrolling_mode != kScrollingModeNone)
        {
            ContextOpenDetailWindow(WindowDetail::signSmall, wallElement.GetBannerIndex().ToUnderlying());
        }
        else
        {
            CoordsXYZD wallLocation = { mapCoords.x, mapCoords.y, wallElement.GetBaseZ(), wallElement.GetDirection() };
            auto wallRemoveAction = GameActions::WallRemoveAction(wallLocation);
            GameActions::Execute(&wallRemoveAction, getGameState());
        }
    }

    /**
     *
     *  rct2: 0x006B88DC
     */
    static void ViewportInteractionRemoveLargeScenery(const LargeSceneryElement& largeSceneryElement, const CoordsXY& mapCoords)
    {
        auto* sceneryEntry = largeSceneryElement.GetEntry();

        if (sceneryEntry->scrolling_mode != kScrollingModeNone)
        {
            auto bannerIndex = largeSceneryElement.GetBannerIndex();
            ContextOpenDetailWindow(WindowDetail::sign, bannerIndex.ToUnderlying());
        }
        else
        {
            auto removeSceneryAction = GameActions::LargeSceneryRemoveAction(
                { mapCoords.x, mapCoords.y, largeSceneryElement.GetBaseZ(), largeSceneryElement.GetDirection() },
                largeSceneryElement.GetSequenceIndex());
            GameActions::Execute(&removeSceneryAction, getGameState());
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
            window, screenCoords, EnumsToFlags(ViewportInteractionItem::terrain, ViewportInteractionItem::water));
        auto initialPos = info.Loc;

        if (info.interactionType == ViewportInteractionItem::none)
        {
            initialPos.SetNull();
            return initialPos;
        }

        int16_t waterHeight = 0;
        if (info.interactionType == ViewportInteractionItem::water)
        {
            waterHeight = info.Element->AsSurface()->GetWaterHeight();
        }

        auto initialVPPos = viewport->ScreenToViewportCoord(screenCoords);
        CoordsXY mapPos = initialPos + CoordsXY{ 16, 16 };

        for (int32_t i = 0; i < 5; i++)
        {
            int16_t z = waterHeight;
            if (info.interactionType != ViewportInteractionItem::water)
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
