/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../windows/Window.h"
#include "Viewport.h"
#include "Window.h"

#include <algorithm>
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
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/LargeScenery.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/Surface.h>
#include <openrct2/world/Wall.h>

static void ViewportInteractionRemoveScenery(TileElement* tileElement, const CoordsXY& mapCoords);
static void ViewportInteractionRemoveFootpath(TileElement* tileElement, const CoordsXY& mapCoords);
static void ViewportInteractionRemoveFootpathItem(TileElement* tileElement, const CoordsXY& mapCoords);
static void ViewportInteractionRemoveParkWall(TileElement* tileElement, const CoordsXY& mapCoords);
static void ViewportInteractionRemoveLargeScenery(TileElement* tileElement, const CoordsXY& mapCoords);
static void ViewportInteractionRemoveParkEntrance(TileElement* tileElement, CoordsXY mapCoords);
static Peep* ViewportInteractionGetClosestPeep(ScreenCoordsXY screenCoords, int32_t maxDistance);

/**
 *
 *  rct2: 0x006ED9D0
 */
InteractionInfo ViewportInteractionGetItemLeft(const ScreenCoordsXY& screenCoords)
{
    InteractionInfo info{};
    // No click input for scenario editor or track manager
    if (gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER))
        return info;

    //
    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && gEditorStep != EditorStep::RollercoasterDesigner)
        return info;

    info = get_map_coordinates_from_pos(
        screenCoords,
        EnumsToFlags(ViewportInteractionItem::Entity, ViewportInteractionItem::Ride, ViewportInteractionItem::ParkEntrance));
    auto tileElement = info.SpriteType != ViewportInteractionItem::Entity ? info.Element : nullptr;
    // Only valid when info.SpriteType == ViewportInteractionItem::Entity, but can't assign nullptr without compiler
    // complaining
    auto sprite = info.Entity;

    // Allows only balloons to be popped and ducks to be quacked in title screen
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
    {
        if (info.SpriteType == ViewportInteractionItem::Entity && (sprite->Is<Balloon>() || sprite->Is<Duck>()))
            return info;

        info.SpriteType = ViewportInteractionItem::None;
        return info;
    }

    switch (info.SpriteType)
    {
        case ViewportInteractionItem::Entity:
            switch (sprite->Type)
            {
                case EntityType::Vehicle:
                {
                    auto vehicle = sprite->As<Vehicle>();
                    if (vehicle != nullptr && vehicle->ride_subtype != OBJECT_ENTRY_INDEX_NULL)
                        vehicle->SetMapToolbar();
                    else
                        info.SpriteType = ViewportInteractionItem::None;
                }
                break;
                case EntityType::Guest:
                case EntityType::Staff:
                {
                    auto peep = sprite->As<Peep>();
                    if (peep != nullptr)
                    {
                        peep_set_map_tooltip(peep);
                    }
                    else
                    {
                        info.SpriteType = ViewportInteractionItem::None;
                    }
                }
                break;
                default:
                    break;
            }
            break;
        case ViewportInteractionItem::Ride:
            ride_set_map_tooltip(tileElement);
            break;
        case ViewportInteractionItem::ParkEntrance:
        {
            auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
            auto parkName = park.Name.c_str();

            auto ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(parkName);
            SetMapTooltip(ft);
            break;
        }
        default:
            info.SpriteType = ViewportInteractionItem::None;
            break;
    }

    // If nothing is under cursor, find a close by peep
    if (info.SpriteType == ViewportInteractionItem::None)
    {
        auto peep = ViewportInteractionGetClosestPeep(screenCoords, 32);
        if (peep != nullptr)
        {
            info.Entity = peep;
            info.SpriteType = ViewportInteractionItem::Entity;
            info.Loc.x = peep->x;
            info.Loc.y = peep->y;
            peep_set_map_tooltip(peep);
        }
    }

    return info;
}

bool ViewportInteractionLeftOver(const ScreenCoordsXY& screenCoords)
{
    auto info = ViewportInteractionGetItemLeft(screenCoords);

    switch (info.SpriteType)
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

    switch (info.SpriteType)
    {
        case ViewportInteractionItem::Entity:
        {
            auto entity = info.Entity;
            switch (entity->Type)
            {
                case EntityType::Vehicle:
                {
                    auto intent = Intent(WD_VEHICLE);
                    intent.putExtra(INTENT_EXTRA_VEHICLE, entity);
                    context_open_intent(&intent);
                    break;
                }
                case EntityType::Guest:
                case EntityType::Staff:
                {
                    auto intent = Intent(WindowClass::Peep);
                    intent.putExtra(INTENT_EXTRA_PEEP, entity);
                    context_open_intent(&intent);
                    break;
                }
                case EntityType::Balloon:
                {
                    if (game_is_not_paused())
                    {
                        auto balloonPress = BalloonPressAction(entity->sprite_index);
                        GameActions::Execute(&balloonPress);
                    }
                }
                break;
                case EntityType::Duck:
                {
                    if (game_is_not_paused())
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
            intent.putExtra(INTENT_EXTRA_TILE_ELEMENT, info.Element);
            context_open_intent(&intent);
            return true;
        }
        case ViewportInteractionItem::ParkEntrance:
            context_open_window(WindowClass::ParkInformation);
            return true;
        default:
            return false;
    }
}

/**
 *
 *  rct2: 0x006EDE88
 */
InteractionInfo ViewportInteractionGetItemRight(const ScreenCoordsXY& screenCoords)
{
    Ride* ride;
    int32_t i;
    InteractionInfo info{};
    // No click input for title screen or track manager
    if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_TRACK_MANAGER))
        return info;

    //
    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && gEditorStep != EditorStep::RollercoasterDesigner)
        return info;

    constexpr auto flags = static_cast<int32_t>(
        ~EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Water));
    info = get_map_coordinates_from_pos(screenCoords, flags);
    auto tileElement = info.Element;

    switch (info.SpriteType)
    {
        case ViewportInteractionItem::Entity:
        {
            auto sprite = info.Entity;
            if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || sprite->Type != EntityType::Vehicle)
            {
                info.SpriteType = ViewportInteractionItem::None;
                return info;
            }

            auto vehicle = sprite->As<Vehicle>();
            if (vehicle == nullptr)
            {
                info.SpriteType = ViewportInteractionItem::None;
                return info;
            }
            ride = get_ride(vehicle->ride);
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
                info.SpriteType = ViewportInteractionItem::None;
                return info;
            }
            if (tileElement->GetType() == TileElementType::Path)
            {
                info.SpriteType = ViewportInteractionItem::None;
                return info;
            }

            ride = get_ride(tileElement->GetRideIndex());
            if (ride == nullptr)
            {
                info.SpriteType = ViewportInteractionItem::None;
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
                if (!gCheatsSandboxMode && !map_is_location_owned({ info.Loc, tileElement->GetBaseZ() * 2 }))
                {
                    info.SpriteType = ViewportInteractionItem::None;
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
                auto* bannerEntry = GetBannerEntry(banner->type);

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

    if (!(input_test_flag(INPUT_FLAG_6)) || !(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
    {
        if (window_find_by_class(WindowClass::RideConstruction) == nullptr
            && window_find_by_class(WindowClass::Footpath) == nullptr)
        {
            info.SpriteType = ViewportInteractionItem::None;
            return info;
        }
    }

    auto ft = Formatter();
    switch (info.SpriteType)
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

        case ViewportInteractionItem::FootpathItem:
        {
            auto* pathAddEntry = tileElement->AsPath()->GetAdditionEntry();
            ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
            if (tileElement->AsPath()->IsBroken())
            {
                ft.Add<StringId>(STR_BROKEN);
            }
            ft.Add<StringId>(pathAddEntry != nullptr ? pathAddEntry->name : STR_NONE);
            SetMapTooltip(ft);
            return info;
        }
        case ViewportInteractionItem::ParkEntrance:
            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
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

    info.SpriteType = ViewportInteractionItem::None;
    return info;
}

bool ViewportInteractionRightOver(const ScreenCoordsXY& screenCoords)
{
    auto info = ViewportInteractionGetItemRight(screenCoords);

    return info.SpriteType != ViewportInteractionItem::None;
}

/**
 *
 *  rct2: 0x006E8A62
 */
bool ViewportInteractionRightClick(const ScreenCoordsXY& screenCoords)
{
    CoordsXYE tileElement;
    auto info = ViewportInteractionGetItemRight(screenCoords);

    switch (info.SpriteType)
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
                auto ride = get_ride(vehicle->ride);
                if (ride != nullptr)
                {
                    ride_construct(ride);
                }
            }
        }
        break;
        case ViewportInteractionItem::Ride:
            tileElement = { info.Loc, info.Element };
            ride_modify(tileElement);
            break;
        case ViewportInteractionItem::Scenery:
            ViewportInteractionRemoveScenery(info.Element, info.Loc);
            break;
        case ViewportInteractionItem::Footpath:
            ViewportInteractionRemoveFootpath(info.Element, info.Loc);
            break;
        case ViewportInteractionItem::FootpathItem:
            ViewportInteractionRemoveFootpathItem(info.Element, info.Loc);
            break;
        case ViewportInteractionItem::ParkEntrance:
            ViewportInteractionRemoveParkEntrance(info.Element, info.Loc);
            break;
        case ViewportInteractionItem::Wall:
            ViewportInteractionRemoveParkWall(info.Element, info.Loc);
            break;
        case ViewportInteractionItem::LargeScenery:
            ViewportInteractionRemoveLargeScenery(info.Element, info.Loc);
            break;
        case ViewportInteractionItem::Banner:
            context_open_detail_window(WD_BANNER, info.Element->AsBanner()->GetIndex().ToUnderlying());
            break;
    }

    return true;
}

/**
 *
 *  rct2: 0x006E08D2
 */
static void ViewportInteractionRemoveScenery(TileElement* tileElement, const CoordsXY& mapCoords)
{
    auto removeSceneryAction = SmallSceneryRemoveAction(
        { mapCoords.x, mapCoords.y, tileElement->GetBaseZ() }, tileElement->AsSmallScenery()->GetSceneryQuadrant(),
        tileElement->AsSmallScenery()->GetEntryIndex());

    GameActions::Execute(&removeSceneryAction);
}

/**
 *
 *  rct2: 0x006A614A
 */
static void ViewportInteractionRemoveFootpath(TileElement* tileElement, const CoordsXY& mapCoords)
{
    rct_window* w;
    TileElement* tileElement2;

    auto z = tileElement->GetBaseZ();

    w = window_find_by_class(WindowClass::Footpath);
    if (w != nullptr)
        FootpathProvisionalUpdate();

    tileElement2 = map_get_first_element_at(mapCoords);
    if (tileElement2 == nullptr)
        return;
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
static void ViewportInteractionRemoveFootpathItem(TileElement* tileElement, const CoordsXY& mapCoords)
{
    auto footpathAdditionRemoveAction = FootpathAdditionRemoveAction({ mapCoords.x, mapCoords.y, tileElement->GetBaseZ() });
    GameActions::Execute(&footpathAdditionRemoveAction);
}

/**
 *
 *  rct2: 0x00666C0E
 */
void ViewportInteractionRemoveParkEntrance(TileElement* tileElement, CoordsXY mapCoords)
{
    int32_t rotation = tileElement->GetDirectionWithOffset(1);
    switch (tileElement->AsEntrance()->GetSequenceIndex())
    {
        case 1:
            mapCoords += CoordsDirectionDelta[rotation];
            break;
        case 2:
            mapCoords -= CoordsDirectionDelta[rotation];
            break;
    }
    auto parkEntranceRemoveAction = ParkEntranceRemoveAction({ mapCoords.x, mapCoords.y, tileElement->GetBaseZ() });
    GameActions::Execute(&parkEntranceRemoveAction);
}

/**
 *
 *  rct2: 0x006E57A9
 */
static void ViewportInteractionRemoveParkWall(TileElement* tileElement, const CoordsXY& mapCoords)
{
    auto* wallEntry = tileElement->AsWall()->GetEntry();
    if (wallEntry->scrolling_mode != SCROLLING_MODE_NONE)
    {
        context_open_detail_window(WD_SIGN_SMALL, tileElement->AsWall()->GetBannerIndex().ToUnderlying());
    }
    else
    {
        CoordsXYZD wallLocation = { mapCoords.x, mapCoords.y, tileElement->GetBaseZ(), tileElement->GetDirection() };
        auto wallRemoveAction = WallRemoveAction(wallLocation);
        GameActions::Execute(&wallRemoveAction);
    }
}

/**
 *
 *  rct2: 0x006B88DC
 */
static void ViewportInteractionRemoveLargeScenery(TileElement* tileElement, const CoordsXY& mapCoords)
{
    auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();

    if (sceneryEntry->scrolling_mode != SCROLLING_MODE_NONE)
    {
        auto bannerIndex = tileElement->AsLargeScenery()->GetBannerIndex();
        context_open_detail_window(WD_SIGN, bannerIndex.ToUnderlying());
    }
    else
    {
        auto removeSceneryAction = LargeSceneryRemoveAction(
            { mapCoords.x, mapCoords.y, tileElement->GetBaseZ(), tileElement->GetDirection() },
            tileElement->AsLargeScenery()->GetSequenceIndex());
        GameActions::Execute(&removeSceneryAction);
    }
}

struct PeepDistance
{
    Peep* peep = nullptr;
    int32_t distance = std::numeric_limits<decltype(distance)>::max();
};

template<typename T>
PeepDistance GetClosestPeep(const ScreenCoordsXY& viewportCoords, const int32_t maxDistance, PeepDistance goal)
{
    for (auto peep : EntityList<T>())
    {
        if (peep->x == LOCATION_NULL)
            continue;

        auto distance = abs(((peep->SpriteRect.GetLeft() + peep->SpriteRect.GetRight()) / 2) - viewportCoords.x)
            + abs(((peep->SpriteRect.GetTop() + peep->SpriteRect.GetBottom()) / 2) - viewportCoords.y);
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
    auto* w = window_find_from_point(screenCoords);
    if (w == nullptr)
        return nullptr;

    auto* viewport = w->viewport;
    if (viewport == nullptr || viewport->zoom >= ZoomLevel{ 2 })
        return nullptr;

    auto viewportCoords = viewport->ScreenToViewportCoord(screenCoords);

    PeepDistance goal;
    if (!(viewport->flags & VIEWPORT_FLAG_HIDE_GUESTS))
        goal = GetClosestPeep<Guest>(viewportCoords, maxDistance, goal);
    if (!(viewport->flags & VIEWPORT_FLAG_HIDE_STAFF))
        goal = GetClosestPeep<Staff>(viewportCoords, maxDistance, goal);
    return goal.peep;
}

/**
 *
 *  rct2: 0x0068A15E
 */
CoordsXY ViewportInteractionGetTileStartAtCursor(const ScreenCoordsXY& screenCoords)
{
    rct_window* window = window_find_from_point(screenCoords);
    if (window == nullptr || window->viewport == nullptr)
    {
        CoordsXY ret{};
        ret.SetNull();
        return ret;
    }
    auto viewport = window->viewport;
    auto info = get_map_coordinates_from_pos_window(
        window, screenCoords, EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Water));
    auto initialPos = info.Loc;

    if (info.SpriteType == ViewportInteractionItem::None)
    {
        initialPos.SetNull();
        return initialPos;
    }

    int16_t waterHeight = 0;
    if (info.SpriteType == ViewportInteractionItem::Water)
    {
        waterHeight = info.Element->AsSurface()->GetWaterHeight();
    }

    auto initialVPPos = viewport->ScreenToViewportCoord(screenCoords);
    CoordsXY mapPos = initialPos + CoordsXY{ 16, 16 };

    for (int32_t i = 0; i < 5; i++)
    {
        int16_t z = waterHeight;
        if (info.SpriteType != ViewportInteractionItem::Water)
        {
            z = tile_element_height(mapPos);
        }
        mapPos = viewport_coord_to_map_coord(initialVPPos, z);
        mapPos.x = std::clamp(mapPos.x, initialPos.x, initialPos.x + 31);
        mapPos.y = std::clamp(mapPos.y, initialPos.y, initialPos.y + 31);
    }

    return mapPos.ToTileStart();
}
