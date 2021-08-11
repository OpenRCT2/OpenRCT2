/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../Input.h"
#include "../actions/RideEntranceExitRemoveAction.h"
#include "../actions/RideSetSettingAction.h"
#include "../actions/RideSetStatusAction.h"
#include "../actions/RideSetVehicleAction.h"
#include "../actions/TrackRemoveAction.h"
#include "../common.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../network/network.h"
#include "../paint/VirtualFloor.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "../world/Climate.h"
#include "../world/Entity.h"
#include "../world/EntityList.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"
#include "TrainManager.h"
#include "Vehicle.h"

bool gGotoStartPlacementMode = false;

money16 gTotalRideValueForMoney;

money32 _currentTrackPrice;

uint16_t _numCurrentPossibleRideConfigurations;
uint16_t _numCurrentPossibleSpecialTrackPieces;

uint32_t _currentTrackCurve;
RideConstructionState _rideConstructionState;
ride_id_t _currentRideIndex;

CoordsXYZ _currentTrackBegin;

uint8_t _currentTrackPieceDirection;
track_type_t _currentTrackPieceType;
uint8_t _currentTrackSelectionFlags;
uint32_t _rideConstructionNextArrowPulse = 0;
uint8_t _currentTrackSlopeEnd;
uint8_t _currentTrackBankEnd;
uint8_t _currentTrackLiftHill;
uint8_t _currentTrackAlternative;
track_type_t _selectedTrackType;

uint8_t _previousTrackBankEnd;
uint8_t _previousTrackSlopeEnd;

CoordsXYZ _previousTrackPiece;

uint8_t _currentBrakeSpeed2;
uint8_t _currentSeatRotationAngle;

CoordsXYZD _unkF440C5;

uint8_t gLastEntranceStyle;

uint8_t gRideEntranceExitPlaceType;
ride_id_t gRideEntranceExitPlaceRideIndex;
StationIndex gRideEntranceExitPlaceStationIndex;
RideConstructionState gRideEntranceExitPlacePreviousRideConstructionState;
Direction gRideEntranceExitPlaceDirection;

using namespace OpenRCT2;

static int32_t ride_check_if_construction_allowed(Ride* ride)
{
    Formatter ft;
    rct_ride_entry* rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
    {
        context_show_error(STR_INVALID_RIDE_TYPE, STR_CANT_EDIT_INVALID_RIDE_TYPE, ft);
        return 0;
    }
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        ft.Increment(6);
        ride->FormatNameTo(ft);
        context_show_error(STR_CANT_START_CONSTRUCTION_ON, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING, ft);
        return 0;
    }

    if (ride->status != RideStatus::Closed && ride->status != RideStatus::Simulating)
    {
        ft.Increment(6);
        ride->FormatNameTo(ft);
        context_show_error(STR_CANT_START_CONSTRUCTION_ON, STR_MUST_BE_CLOSED_FIRST, ft);
        return 0;
    }

    return 1;
}

static rct_window* ride_create_or_find_construction_window(ride_id_t rideIndex)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_FOCUS);
    intent.putExtra(INTENT_EXTRA_RIDE_ID, rideIndex);
    windowManager->BroadcastIntent(intent);
    return window_find_by_class(WC_RIDE_CONSTRUCTION);
}

/**
 *
 *  rct2: 0x006B4857
 */
void ride_construct(Ride* ride)
{
    CoordsXYE trackElement;
    if (ride_try_get_origin_element(ride, &trackElement))
    {
        ride_find_track_gap(ride, &trackElement, &trackElement);

        rct_window* w = window_get_main();
        if (w != nullptr && ride_modify(&trackElement))
            window_scroll_to_location(w, { trackElement, trackElement.element->GetBaseZ() });
    }
    else
    {
        ride_initialise_construction_window(ride);
    }
}

/**
 *
 *  rct2: 0x006DD4D5
 */
static void ride_remove_cable_lift(Ride* ride)
{
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT)
    {
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CABLE_LIFT;
        uint16_t spriteIndex = ride->cable_lift;
        do
        {
            Vehicle* vehicle = GetEntity<Vehicle>(spriteIndex);
            if (vehicle == nullptr)
            {
                return;
            }
            vehicle->Invalidate();
            spriteIndex = vehicle->next_vehicle_on_train;
            sprite_remove(vehicle);
        } while (spriteIndex != SPRITE_INDEX_NULL);
    }
}

/**
 *
 *  rct2: 0x006DD506
 */
void Ride::RemoveVehicles()
{
    if (lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        lifecycle_flags &= ~RIDE_LIFECYCLE_ON_TRACK;
        lifecycle_flags &= ~(RIDE_LIFECYCLE_TEST_IN_PROGRESS | RIDE_LIFECYCLE_HAS_STALLED_VEHICLE);

        for (size_t i = 0; i <= MAX_VEHICLES_PER_RIDE; i++)
        {
            uint16_t spriteIndex = vehicles[i];
            while (spriteIndex != SPRITE_INDEX_NULL)
            {
                Vehicle* vehicle = GetEntity<Vehicle>(spriteIndex);
                if (vehicle == nullptr)
                {
                    break;
                }
                vehicle->Invalidate();
                spriteIndex = vehicle->next_vehicle_on_train;
                sprite_remove(vehicle);
            }

            vehicles[i] = SPRITE_INDEX_NULL;
        }

        for (size_t i = 0; i < MAX_STATIONS; i++)
            stations[i].TrainAtStation = RideStation::NO_TRAIN;

        // Also clean up orphaned vehicles for good measure.
        for (auto* vehicle : TrainManager::View())
        {
            if (vehicle->ride == id)
            {
                vehicle->Invalidate();
                sprite_remove(vehicle);
            }
        }
    }
}

/**
 *
 *  rct2: 0x006DD4AC
 */
void ride_clear_for_construction(Ride* ride)
{
    ride->measurement = {};

    ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;

    // Open circuit rides will go directly into building mode (creating ghosts) where it would normally clear the stats,
    // however this causes desyncs since it's directly run from the window and other clients would not get it.
    // To prevent these problems, unconditionally invalidate the test results on all clients in multiplayer games.
    if (network_get_mode() != NETWORK_MODE_NONE)
    {
        invalidate_test_results(ride);
    }

    ride_remove_cable_lift(ride);
    ride->RemoveVehicles();
    ride_clear_blocked_tiles(ride);

    auto w = window_find_by_number(WC_RIDE, ride->id);
    if (w != nullptr)
        window_event_resize_call(w);
}

/**
 *
 *  rct2: 0x006664DF
 */
void ride_remove_peeps(Ride* ride)
{
    // Find first station
    auto stationIndex = ride_get_first_valid_station_start(ride);

    // Get exit position and direction
    auto exitPosition = CoordsXYZD{ 0, 0, 0, INVALID_DIRECTION };
    if (stationIndex != STATION_INDEX_NULL)
    {
        auto location = ride_get_exit_location(ride, stationIndex).ToCoordsXYZD();
        if (!location.isNull())
        {
            auto direction = direction_reverse(location.direction);
            exitPosition = location;
            exitPosition.x += (DirectionOffsets[direction].x * 20) + COORDS_XY_HALF_TILE;
            exitPosition.y += (DirectionOffsets[direction].y * 20) + COORDS_XY_HALF_TILE;
            exitPosition.z += 2;

            // Reverse direction
            exitPosition.direction = direction_reverse(exitPosition.direction);

            exitPosition.direction *= 8;
        }
    }

    // Place all the guests at exit
    for (auto peep : EntityList<Guest>())
    {
        if (peep->State == PeepState::QueuingFront || peep->State == PeepState::EnteringRide
            || peep->State == PeepState::LeavingRide || peep->State == PeepState::OnRide)
        {
            if (peep->CurrentRide != ride->id)
                continue;

            peep_decrement_num_riders(peep);
            if (peep->State == PeepState::QueuingFront && peep->RideSubState == PeepRideSubState::AtEntrance)
                peep->RemoveFromQueue();

            if (exitPosition.direction == INVALID_DIRECTION)
            {
                CoordsXYZ newLoc = { peep->NextLoc.ToTileCentre(), peep->NextLoc.z };
                if (peep->GetNextIsSloped())
                    newLoc.z += COORDS_Z_STEP;
                newLoc.z++;
                peep->MoveTo(newLoc);
            }
            else
            {
                peep->MoveTo(exitPosition);
                peep->sprite_direction = exitPosition.direction;
            }

            peep->State = PeepState::Falling;
            peep->SwitchToSpecialSprite(0);

            peep->Happiness = std::min(peep->Happiness, peep->HappinessTarget) / 2;
            peep->HappinessTarget = peep->Happiness;
            peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_STATS;
        }
    }
    // Place all the staff at exit
    for (auto peep : EntityList<Staff>())
    {
        if (peep->State == PeepState::Fixing || peep->State == PeepState::Inspecting)
        {
            if (peep->CurrentRide != ride->id)
                continue;

            if (exitPosition.direction == INVALID_DIRECTION)
            {
                CoordsXYZ newLoc = { peep->NextLoc.ToTileCentre(), peep->NextLoc.z };
                if (peep->GetNextIsSloped())
                    newLoc.z += COORDS_Z_STEP;
                newLoc.z++;
                peep->MoveTo(newLoc);
            }
            else
            {
                peep->MoveTo(exitPosition);
                peep->sprite_direction = exitPosition.direction;
            }

            peep->State = PeepState::Falling;
            peep->SwitchToSpecialSprite(0);

            peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_STATS;
        }
    }
    ride->num_riders = 0;
    ride->slide_in_use = 0;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN;
}

void ride_clear_blocked_tiles(Ride* ride)
{
    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            auto element = map_get_first_element_at(TileCoordsXY{ x, y }.ToCoordsXY());
            if (element != nullptr)
            {
                do
                {
                    if (element->GetType() == TILE_ELEMENT_TYPE_TRACK && element->AsTrack()->GetRideIndex() == ride->id)
                    {
                        // Unblock footpath element that is at same position
                        auto footpathElement = map_get_footpath_element(
                            TileCoordsXYZ{ x, y, element->base_height }.ToCoordsXYZ());
                        if (footpathElement != nullptr)
                        {
                            footpathElement->AsPath()->SetIsBlockedByVehicle(false);
                        }
                    }
                } while (!(element++)->IsLastForTile());
            }
        }
    }
}

/**
 * Gets the origin track element (sequence 0). Seems to do more than that though and even invalidates track.
 *  rct2: 0x006C683D
 * ax : x
 * bx : direction << 8, type
 * cx : y
 * dx : z
 * si : extra_params
 * di : output_element
 * bp : flags
 */
std::optional<CoordsXYZ> sub_6C683D(
    const CoordsXYZD& location, track_type_t type, uint16_t extra_params, TileElement** output_element, uint16_t flags)
{
    // Find the relevant track piece, prefer sequence 0 (this ensures correct behaviour for diagonal track pieces)
    auto trackElement = map_get_track_element_at_of_type_seq(location, type, 0);
    if (trackElement == nullptr)
    {
        trackElement = map_get_track_element_at_of_type(location, type);
        if (trackElement == nullptr)
        {
            return std::nullopt;
        }
    }

    // Possibly z should be & 0xF8
    auto trackBlock = TrackBlocks[type];
    if (trackBlock == nullptr)
        return std::nullopt;

    // Now find all the elements that belong to this track piece
    int32_t sequence = trackElement->GetSequenceIndex();
    uint8_t mapDirection = trackElement->GetDirection();

    CoordsXY offsets = { trackBlock[sequence].x, trackBlock[sequence].y };
    CoordsXY newCoords = location;
    newCoords += offsets.Rotate(direction_reverse(mapDirection));

    auto retCoordsXYZ = CoordsXYZ{ newCoords.x, newCoords.y, location.z - trackBlock[sequence].z };

    int32_t start_z = retCoordsXYZ.z;
    retCoordsXYZ.z += trackBlock[0].z;
    for (int32_t i = 0; trackBlock[i].index != 0xFF; ++i)
    {
        CoordsXY cur = { retCoordsXYZ };
        offsets = { trackBlock[i].x, trackBlock[i].y };
        cur += offsets.Rotate(mapDirection);
        int32_t cur_z = start_z + trackBlock[i].z;

        map_invalidate_tile_full(cur);

        trackElement = map_get_track_element_at_of_type_seq(
            { cur, cur_z, static_cast<Direction>(location.direction) }, type, trackBlock[i].index);
        if (trackElement == nullptr)
        {
            return std::nullopt;
        }
        if (i == 0 && output_element != nullptr)
        {
            *output_element = reinterpret_cast<TileElement*>(trackElement);
        }
        if (flags & TRACK_ELEMENT_SET_HIGHLIGHT_FALSE)
        {
            trackElement->SetHighlight(false);
        }
        if (flags & TRACK_ELEMENT_SET_HIGHLIGHT_TRUE)
        {
            trackElement->SetHighlight(true);
        }
        if (flags & TRACK_ELEMENT_SET_COLOUR_SCHEME)
        {
            trackElement->SetColourScheme(static_cast<uint8_t>(extra_params & 0xFF));
        }
        if (flags & TRACK_ELEMENT_SET_SEAT_ROTATION)
        {
            trackElement->SetSeatRotation(static_cast<uint8_t>(extra_params & 0xFF));
        }
        if (flags & TRACK_ELEMENT_SET_HAS_CABLE_LIFT_TRUE)
        {
            trackElement->SetHasCableLift(true);
        }
        if (flags & TRACK_ELEMENT_SET_HAS_CABLE_LIFT_FALSE)
        {
            trackElement->SetHasCableLift(false);
        }
    }
    return retCoordsXYZ;
}

void ride_restore_provisional_track_piece()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK)
    {
        ride_id_t rideIndex;
        int32_t direction, type, liftHillAndAlternativeState;
        CoordsXYZ trackPos;
        if (window_ride_construction_update_state(
                &type, &direction, &rideIndex, &liftHillAndAlternativeState, &trackPos, nullptr))
        {
            ride_construction_remove_ghosts();
        }
        else
        {
            _currentTrackPrice = place_provisional_track_piece(
                rideIndex, type, direction, liftHillAndAlternativeState, trackPos);
            window_ride_construction_update_active_elements();
        }
    }
}

void ride_remove_provisional_track_piece()
{
    auto rideIndex = _currentRideIndex;
    auto ride = get_ride(rideIndex);
    if (ride == nullptr || !(_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK))
    {
        return;
    }

    int32_t x = _unkF440C5.x;
    int32_t y = _unkF440C5.y;
    int32_t z = _unkF440C5.z;
    if (ride->type == RIDE_TYPE_MAZE)
    {
        int32_t flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
            | GAME_COMMAND_FLAG_GHOST;
        maze_set_track(x, y, z, flags, false, 0, rideIndex, GC_SET_MAZE_TRACK_FILL);
        maze_set_track(x, y + 16, z, flags, false, 1, rideIndex, GC_SET_MAZE_TRACK_FILL);
        maze_set_track(x + 16, y + 16, z, flags, false, 2, rideIndex, GC_SET_MAZE_TRACK_FILL);
        maze_set_track(x + 16, y, z, flags, false, 3, rideIndex, GC_SET_MAZE_TRACK_FILL);
    }
    else
    {
        int32_t direction = _unkF440C5.direction;
        if (!(direction & 4))
        {
            x -= CoordsDirectionDelta[direction].x;
            y -= CoordsDirectionDelta[direction].y;
        }
        CoordsXYE next_track;
        if (track_block_get_next_from_zero({ x, y, z }, ride, direction, &next_track, &z, &direction, true))
        {
            auto trackType = next_track.element->AsTrack()->GetTrackType();
            int32_t trackSequence = next_track.element->AsTrack()->GetSequenceIndex();
            auto trackRemoveAction = TrackRemoveAction{ trackType,
                                                        trackSequence,
                                                        { next_track.x, next_track.y, z, static_cast<Direction>(direction) } };
            trackRemoveAction.SetFlags(
                GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
            GameActions::Execute(&trackRemoveAction);
        }
    }
}

/**
 *
 *  rct2: 0x006C96C0
 */
void ride_construction_remove_ghosts()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT)
    {
        ride_entrance_exit_remove_ghost();
        _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT;
    }
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK)
    {
        ride_remove_provisional_track_piece();
        _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_TRACK;
    }
}

/*
 *  rct2: 0x006C9627
 */
void ride_construction_invalidate_current_track()
{
    switch (_rideConstructionState)
    {
        case RideConstructionState::Selected:
            sub_6C683D(
                { _currentTrackBegin, static_cast<Direction>(_currentTrackPieceDirection & 3) }, _currentTrackPieceType, 0,
                nullptr, TRACK_ELEMENT_SET_HIGHLIGHT_FALSE);
            break;
        case RideConstructionState::Build:
        case RideConstructionState::MazeMove:
        case RideConstructionState::MazeFill:
        case RideConstructionState::Front:
        case RideConstructionState::Back:
            if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW)
            {
                map_invalidate_tile_full(_currentTrackBegin.ToTileStart());
            }
            ride_construction_remove_ghosts();
            break;
        case RideConstructionState::Place:
        case RideConstructionState::EntranceExit:
        default:
            if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW)
            {
                _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_ARROW;
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
                map_invalidate_tile_full(_currentTrackBegin);
            }
            ride_construction_remove_ghosts();
            break;
    }
}

/**
 *
 *  rct2: 0x006C9B19
 */
static void ride_construction_reset_current_piece()
{
    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
        return;

    const auto& rtd = ride->GetRideTypeDescriptor();

    if (!rtd.HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK) || ride->num_stations == 0)
    {
        _currentTrackCurve = rtd.StartTrackPiece | RideConstructionSpecialPieceSelected;
        _currentTrackSlopeEnd = 0;
        _currentTrackBankEnd = 0;
        _currentTrackLiftHill = 0;
        _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;
        if (rtd.HasFlag(RIDE_TYPE_FLAG_START_CONSTRUCTION_INVERTED))
        {
            _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
        }
        _previousTrackSlopeEnd = 0;
        _previousTrackBankEnd = 0;
    }
    else
    {
        _currentTrackCurve = TrackElemType::None;
        _rideConstructionState = RideConstructionState::State0;
    }
}

/**
 *
 *  rct2: 0x006C9800
 */
void ride_construction_set_default_next_piece()
{
    auto rideIndex = _currentRideIndex;
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return;

    const auto& rtd = ride->GetRideTypeDescriptor();

    int32_t z, direction, trackType, curve, bank, slope;
    track_begin_end trackBeginEnd;
    CoordsXYE xyElement;
    TileElement* tileElement;
    _currentTrackPrice = MONEY32_UNDEFINED;
    switch (_rideConstructionState)
    {
        case RideConstructionState::Front:
            direction = _currentTrackPieceDirection;
            if (!track_block_get_previous_from_zero(_currentTrackBegin, ride, direction, &trackBeginEnd))
            {
                ride_construction_reset_current_piece();
                return;
            }
            tileElement = trackBeginEnd.begin_element;
            trackType = tileElement->AsTrack()->GetTrackType();

            if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK))
            {
                ride_construction_reset_current_piece();
                return;
            }

            // Set whether track is covered
            _currentTrackAlternative &= ~RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
            if (rtd.HasFlag(RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE))
            {
                if (tileElement->AsTrack()->IsInverted())
                {
                    _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
                }
            }

            curve = gTrackCurveChain[trackType].next;
            bank = TrackDefinitions[trackType].bank_end;
            slope = TrackDefinitions[trackType].vangle_end;

            // Set track curve
            _currentTrackCurve = curve;

            // Set track banking
            if (rtd.HasFlag(RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE))
            {
                if (bank == TRACK_BANK_UPSIDE_DOWN)
                {
                    bank = TRACK_BANK_NONE;
                    _currentTrackAlternative ^= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
                }
            }
            _currentTrackBankEnd = bank;
            _previousTrackBankEnd = bank;

            // Set track slope and lift hill
            _currentTrackSlopeEnd = slope;
            _previousTrackSlopeEnd = slope;
            _currentTrackLiftHill = tileElement->AsTrack()->HasChain() && slope != TRACK_SLOPE_DOWN_25
                && slope != TRACK_SLOPE_DOWN_60;
            break;
        case RideConstructionState::Back:
            direction = direction_reverse(_currentTrackPieceDirection);
            if (!track_block_get_next_from_zero(_currentTrackBegin, ride, direction, &xyElement, &z, &direction, false))
            {
                ride_construction_reset_current_piece();
                return;
            }
            tileElement = xyElement.element;
            trackType = tileElement->AsTrack()->GetTrackType();

            // Set whether track is covered
            _currentTrackAlternative &= ~RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
            if (rtd.HasFlag(RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE))
            {
                if (tileElement->AsTrack()->IsInverted())
                {
                    _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
                }
            }

            curve = gTrackCurveChain[trackType].previous;
            bank = TrackDefinitions[trackType].bank_start;
            slope = TrackDefinitions[trackType].vangle_start;

            // Set track curve
            _currentTrackCurve = curve;

            // Set track banking
            if (rtd.HasFlag(RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE))
            {
                if (bank == TRACK_BANK_UPSIDE_DOWN)
                {
                    bank = TRACK_BANK_NONE;
                    _currentTrackAlternative ^= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
                }
            }
            _currentTrackBankEnd = bank;
            _previousTrackBankEnd = bank;

            // Set track slope and lift hill
            _currentTrackSlopeEnd = slope;
            _previousTrackSlopeEnd = slope;
            if (!gCheatsEnableChainLiftOnAllTrack)
            {
                _currentTrackLiftHill = tileElement->AsTrack()->HasChain();
            }
            break;
        default:
            break;
    }
}

/**
 *
 *  rct2: 0x006C9296
 */
void ride_select_next_section()
{
    if (_rideConstructionState == RideConstructionState::Selected)
    {
        ride_construction_invalidate_current_track();
        int32_t direction = _currentTrackPieceDirection;
        int32_t type = _currentTrackPieceType;
        TileElement* tileElement;
        auto newCoords = sub_6C683D({ _currentTrackBegin, static_cast<Direction>(direction & 3) }, type, 0, &tileElement, 0);
        if (newCoords == std::nullopt)
        {
            _rideConstructionState = RideConstructionState::State0;
            window_ride_construction_update_active_elements();
            return;
        }

        // Invalidate previous track piece (we may not be changing height!)
        virtual_floor_invalidate();

        CoordsXYE inputElement, outputElement;
        inputElement.x = newCoords->x;
        inputElement.y = newCoords->y;
        inputElement.element = tileElement;
        if (track_block_get_next(&inputElement, &outputElement, &newCoords->z, &direction))
        {
            newCoords->x = outputElement.x;
            newCoords->y = outputElement.y;
            tileElement = outputElement.element;
            if (!scenery_tool_is_active())
            {
                // Set next element's height.
                virtual_floor_set_height(tileElement->GetBaseZ());
            }

            _currentTrackBegin = *newCoords;
            _currentTrackPieceDirection = tileElement->GetDirection();
            _currentTrackPieceType = tileElement->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            window_ride_construction_update_active_elements();
        }
        else
        {
            _rideConstructionState = RideConstructionState::Front;
            _currentTrackBegin = { outputElement, newCoords->z };
            _currentTrackPieceDirection = direction;
            _currentTrackPieceType = tileElement->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            ride_construction_set_default_next_piece();
            window_ride_construction_update_active_elements();
        }
    }
    else if (_rideConstructionState == RideConstructionState::Back)
    {
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        if (ride_select_forwards_from_back())
        {
            window_ride_construction_update_active_elements();
        }
    }
}

/**
 *
 *  rct2: 0x006C93B8
 */
void ride_select_previous_section()
{
    if (_rideConstructionState == RideConstructionState::Selected)
    {
        ride_construction_invalidate_current_track();
        int32_t direction = _currentTrackPieceDirection;
        int32_t type = _currentTrackPieceType;
        TileElement* tileElement;
        auto newCoords = sub_6C683D({ _currentTrackBegin, static_cast<Direction>(direction & 3) }, type, 0, &tileElement, 0);
        if (newCoords == std::nullopt)
        {
            _rideConstructionState = RideConstructionState::State0;
            window_ride_construction_update_active_elements();
            return;
        }

        // Invalidate previous track piece (we may not be changing height!)
        virtual_floor_invalidate();

        track_begin_end trackBeginEnd;
        if (track_block_get_previous({ *newCoords, tileElement }, &trackBeginEnd))
        {
            _currentTrackBegin.x = trackBeginEnd.begin_x;
            _currentTrackBegin.y = trackBeginEnd.begin_y;
            _currentTrackBegin.z = trackBeginEnd.begin_z;
            _currentTrackPieceDirection = trackBeginEnd.begin_direction;
            _currentTrackPieceType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            if (!scenery_tool_is_active())
            {
                // Set previous element's height.
                virtual_floor_set_height(trackBeginEnd.begin_element->GetBaseZ());
            }
            window_ride_construction_update_active_elements();
        }
        else
        {
            _rideConstructionState = RideConstructionState::Back;
            _currentTrackBegin.x = trackBeginEnd.end_x;
            _currentTrackBegin.y = trackBeginEnd.end_y;
            _currentTrackBegin.z = trackBeginEnd.begin_z;
            _currentTrackPieceDirection = trackBeginEnd.end_direction;
            _currentTrackPieceType = tileElement->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            ride_construction_set_default_next_piece();
            window_ride_construction_update_active_elements();
        }
    }
    else if (_rideConstructionState == RideConstructionState::Front)
    {
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        if (ride_select_backwards_from_front())
        {
            window_ride_construction_update_active_elements();
        }
    }
}

/**
 *
 *  rct2: 0x006CC2CA
 */
static bool ride_modify_entrance_or_exit(const CoordsXYE& tileElement)
{
    if (tileElement.element == nullptr)
        return false;

    auto entranceElement = tileElement.element->AsEntrance();
    if (entranceElement == nullptr)
        return false;

    auto rideIndex = entranceElement->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    auto entranceType = entranceElement->GetEntranceType();
    if (entranceType != ENTRANCE_TYPE_RIDE_ENTRANCE && entranceType != ENTRANCE_TYPE_RIDE_EXIT)
        return false;

    auto stationIndex = entranceElement->GetStationIndex();

    // Get or create construction window for ride
    auto constructionWindow = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (constructionWindow == nullptr)
    {
        if (!ride_initialise_construction_window(ride))
            return false;

        constructionWindow = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (constructionWindow == nullptr)
            return false;
    }

    ride_construction_invalidate_current_track();
    if (_rideConstructionState != RideConstructionState::EntranceExit || !(input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        || gCurrentToolWidget.window_classification != WC_RIDE_CONSTRUCTION)
    {
        // Replace entrance / exit
        tool_set(
            constructionWindow,
            entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE ? WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE : WC_RIDE_CONSTRUCTION__WIDX_EXIT,
            Tool::Crosshair);
        gRideEntranceExitPlaceType = entranceType;
        gRideEntranceExitPlaceRideIndex = rideIndex;
        gRideEntranceExitPlaceStationIndex = stationIndex;
        input_set_flag(INPUT_FLAG_6, true);
        if (_rideConstructionState != RideConstructionState::EntranceExit)
        {
            gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
            _rideConstructionState = RideConstructionState::EntranceExit;
        }

        window_ride_construction_update_active_elements();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    }
    else
    {
        // Remove entrance / exit
        auto rideEntranceExitRemove = RideEntranceExitRemoveAction(
            { tileElement.x, tileElement.y }, rideIndex, stationIndex, entranceType == ENTRANCE_TYPE_RIDE_EXIT);

        rideEntranceExitRemove.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
            gCurrentToolWidget.widget_index = entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE ? WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE
                                                                                          : WC_RIDE_CONSTRUCTION__WIDX_EXIT;
            gRideEntranceExitPlaceType = entranceType;
            window_invalidate_by_class(WC_RIDE_CONSTRUCTION);
        });

        GameActions::Execute(&rideEntranceExitRemove);
    }

    window_invalidate_by_class(WC_RIDE_CONSTRUCTION);
    return true;
}

/**
 *
 *  rct2: 0x006CC287
 */
static bool ride_modify_maze(const CoordsXYE& tileElement)
{
    if (tileElement.element != nullptr)
    {
        auto trackElement = tileElement.element->AsTrack();
        if (trackElement != nullptr)
        {
            _currentRideIndex = trackElement->GetRideIndex();
            _rideConstructionState = RideConstructionState::Build;
            _currentTrackBegin.x = tileElement.x;
            _currentTrackBegin.y = tileElement.y;
            _currentTrackBegin.z = trackElement->GetBaseZ();
            _currentTrackSelectionFlags = 0;
            _rideConstructionNextArrowPulse = 0;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

            auto intent = Intent(INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION);
            context_broadcast_intent(&intent);
            return true;
        }
    }
    return false;
}

/**
 *
 *  rct2: 0x006CC056
 */
bool ride_modify(CoordsXYE* input)
{
    auto tileElement = *input;
    if (tileElement.element == nullptr)
        return false;

    auto rideIndex = tileElement.element->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
    {
        return false;
    }

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr || !ride_check_if_construction_allowed(ride))
        return false;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
    {
        Formatter ft;
        ft.Increment(6);
        ride->FormatNameTo(ft);
        context_show_error(
            STR_CANT_START_CONSTRUCTION_ON, STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE, ft);
        return false;
    }

    // Stop the ride again to clear all vehicles and peeps (compatible with network games)
    if (ride->status != RideStatus::Simulating)
    {
        ride_set_status(ride, RideStatus::Closed);
    }

    // Check if element is a station entrance or exit
    if (tileElement.element->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
        return ride_modify_entrance_or_exit(tileElement);

    ride_create_or_find_construction_window(rideIndex);

    if (ride->type == RIDE_TYPE_MAZE)
    {
        return ride_modify_maze(tileElement);
    }

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS))
    {
        CoordsXYE endOfTrackElement{};
        if (ride_find_track_gap(ride, &tileElement, &endOfTrackElement))
            tileElement = endOfTrackElement;
    }

    if (tileElement.element == nullptr || tileElement.element->GetType() != TILE_ELEMENT_TYPE_TRACK)
        return false;

    auto tileCoords = CoordsXYZ{ tileElement, tileElement.element->GetBaseZ() };
    auto direction = tileElement.element->GetDirection();
    auto type = tileElement.element->AsTrack()->GetTrackType();
    auto newCoords = sub_6C683D({ tileCoords, direction }, type, 0, nullptr, 0);
    if (newCoords == std::nullopt)
        return false;

    _currentRideIndex = rideIndex;
    _rideConstructionState = RideConstructionState::Selected;
    _currentTrackBegin = *newCoords;
    _currentTrackPieceDirection = direction;
    _currentTrackPieceType = type;
    _currentTrackSelectionFlags = 0;
    _rideConstructionNextArrowPulse = 0;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK))
    {
        window_ride_construction_update_active_elements();
        return true;
    }

    ride_select_next_section();
    if (_rideConstructionState == RideConstructionState::Front)
    {
        window_ride_construction_update_active_elements();
        return true;
    }

    _rideConstructionState = RideConstructionState::Selected;
    _currentTrackBegin = *newCoords;
    _currentTrackPieceDirection = direction;
    _currentTrackPieceType = type;
    _currentTrackSelectionFlags = 0;

    ride_select_previous_section();

    if (_rideConstructionState != RideConstructionState::Back)
    {
        _rideConstructionState = RideConstructionState::Selected;
        _currentTrackBegin = *newCoords;
        _currentTrackPieceDirection = direction;
        _currentTrackPieceType = type;
        _currentTrackSelectionFlags = 0;
    }

    window_ride_construction_update_active_elements();
    return true;
}

/**
 *
 *  rct2: 0x006CC3FB
 */
int32_t ride_initialise_construction_window(Ride* ride)
{
    rct_window* w;

    tool_cancel();

    if (!ride_check_if_construction_allowed(ride))
        return 0;

    ride_clear_for_construction(ride);
    ride_remove_peeps(ride);

    w = ride_create_or_find_construction_window(ride->id);

    tool_set(w, WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT, Tool::Crosshair);
    input_set_flag(INPUT_FLAG_6, true);

    ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
        return 0;

    _currentTrackCurve = ride->GetRideTypeDescriptor().StartTrackPiece | RideConstructionSpecialPieceSelected;
    _currentTrackSlopeEnd = 0;
    _currentTrackBankEnd = 0;
    _currentTrackLiftHill = 0;
    _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_START_CONSTRUCTION_INVERTED))
        _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;

    _previousTrackBankEnd = 0;
    _previousTrackSlopeEnd = 0;

    _currentTrackPieceDirection = 0;
    _rideConstructionState = RideConstructionState::Place;
    _currentTrackSelectionFlags = 0;

    window_ride_construction_update_active_elements();
    return 1;
}

/**
 *
 *  rct2: 0x006CB7FB
 */
int32_t ride_get_refund_price(const Ride* ride)
{
    CoordsXYE trackElement;
    money32 cost = 0;

    if (!ride_try_get_origin_element(ride, &trackElement))
    {
        return 0; // Ride has no track to refund
    }

    // Find the start in case it is not a complete circuit
    ride_get_start_of_track(&trackElement);

    uint8_t direction = trackElement.element->GetDirection();

    // Used in the following loop to know when we have
    // completed all of the elements and are back at the
    // start.
    TileElement* initial_map = trackElement.element;
    CoordsXYE slowIt = trackElement;
    bool moveSlowIt = true;

    do
    {
        auto trackRemoveAction = TrackRemoveAction(
            trackElement.element->AsTrack()->GetTrackType(), trackElement.element->AsTrack()->GetSequenceIndex(),
            { trackElement.x, trackElement.y, trackElement.element->GetBaseZ(), direction });
        trackRemoveAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);

        auto res = GameActions::Query(&trackRemoveAction);

        cost += res->Cost;

        if (!track_block_get_next(&trackElement, &trackElement, nullptr, nullptr))
        {
            break;
        }

        moveSlowIt = !moveSlowIt;
        if (moveSlowIt)
        {
            if (!track_block_get_next(&slowIt, &slowIt, nullptr, nullptr) || slowIt.element == trackElement.element)
            {
                break;
            }
        }

        direction = trackElement.element->GetDirection();

    } while (trackElement.element != initial_map);

    return cost;
}

money32 set_operating_setting(ride_id_t rideId, RideSetSetting setting, uint8_t value)
{
    auto rideSetSetting = RideSetSettingAction(rideId, setting, value);
    auto res = GameActions::Execute(&rideSetSetting);
    return res->Error == GameActions::Status::Ok ? 0 : MONEY32_UNDEFINED;
}

money32 set_operating_setting_nested(ride_id_t rideId, RideSetSetting setting, uint8_t value, uint8_t flags)
{
    auto rideSetSetting = RideSetSettingAction(rideId, setting, value);
    rideSetSetting.SetFlags(flags);
    auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&rideSetSetting)
                                               : GameActions::QueryNested(&rideSetSetting);
    return res->Error == GameActions::Status::Ok ? 0 : MONEY32_UNDEFINED;
}

static int32_t loc_6CD18E(
    int16_t mapX, int16_t mapY, int16_t entranceMinX, int16_t entranceMinY, int16_t entranceMaxX, int16_t entranceMaxY)
{
    int32_t direction = 0;
    if (mapX == entranceMinX)
    {
        if (mapY > entranceMinY && mapY < entranceMaxY)
        {
            return direction;
        }
    }
    direction = 1;
    if (mapY == entranceMaxY)
    {
        if (mapX > entranceMinX && mapX < entranceMaxX)
        {
            return direction;
        }
    }
    direction = 2;
    if (mapX == entranceMaxX)
    {
        if (mapY > entranceMinY && mapY < entranceMaxY)
        {
            return direction;
        }
    }
    direction = 3;
    if (mapY == entranceMinY)
    {
        if (mapX > entranceMinX && mapX < entranceMaxX)
        {
            return direction;
        }
    }
    return -1;
}

/**
 *
 *  rct2: 0x006CCF70
 */
CoordsXYZD ride_get_entrance_or_exit_position_from_screen_position(const ScreenCoordsXY& screenCoords)
{
    CoordsXYZD entranceExitCoords{};

    gRideEntranceExitPlaceDirection = INVALID_DIRECTION;
    auto info = get_map_coordinates_from_pos(screenCoords, EnumsToFlags(ViewportInteractionItem::Ride));
    if (info.SpriteType != ViewportInteractionItem::None)
    {
        if (info.Element->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            if (info.Element->AsTrack()->GetRideIndex() == gRideEntranceExitPlaceRideIndex)
            {
                if (TrackSequenceProperties[info.Element->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN)
                {
                    if (info.Element->AsTrack()->GetTrackType() == TrackElemType::Maze)
                    {
                        gRideEntranceExitPlaceStationIndex = 0;
                    }
                    else
                    {
                        gRideEntranceExitPlaceStationIndex = info.Element->AsTrack()->GetStationIndex();
                    }
                }
            }
        }
    }

    auto ride = get_ride(gRideEntranceExitPlaceRideIndex);
    if (ride == nullptr)
    {
        entranceExitCoords.setNull();
        return entranceExitCoords;
    }

    auto stationBaseZ = ride->stations[gRideEntranceExitPlaceStationIndex].GetBaseZ();

    auto coords = screen_get_map_xy_with_z(screenCoords, stationBaseZ);
    if (!coords)
    {
        entranceExitCoords.setNull();
        return entranceExitCoords;
    }

    auto word_F4418C = coords->x;
    auto word_F4418E = coords->y;

    entranceExitCoords = { coords->ToTileStart(), stationBaseZ, INVALID_DIRECTION };

    if (ride->type == RIDE_TYPE_NULL)
    {
        entranceExitCoords.setNull();
        return entranceExitCoords;
    }

    auto stationStart = ride->stations[gRideEntranceExitPlaceStationIndex].Start;
    if (stationStart.isNull())
    {
        entranceExitCoords.setNull();
        return entranceExitCoords;
    }

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION))
    {
        auto mapX = (word_F4418C & 0x1F) - 16;
        auto mapY = (word_F4418E & 0x1F) - 16;
        if (std::abs(mapX) < std::abs(mapY))
        {
            entranceExitCoords.direction = mapY < 0 ? 3 : 1;
        }
        else
        {
            entranceExitCoords.direction = mapX < 0 ? 0 : 2;
        }

        for (int32_t i = 0; i < MAX_STATIONS; i++)
        {
            mapX = entranceExitCoords.x + CoordsDirectionDelta[entranceExitCoords.direction].x;
            mapY = entranceExitCoords.y + CoordsDirectionDelta[entranceExitCoords.direction].y;
            if (map_is_location_valid({ mapX, mapY }))
            {
                auto tileElement = map_get_first_element_at({ mapX, mapY });
                if (tileElement == nullptr)
                    continue;
                do
                {
                    if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                        continue;
                    if (tileElement->GetBaseZ() != stationBaseZ)
                        continue;
                    if (tileElement->AsTrack()->GetRideIndex() != gRideEntranceExitPlaceRideIndex)
                        continue;
                    if (tileElement->AsTrack()->GetTrackType() == TrackElemType::Maze)
                    {
                        entranceExitCoords.direction = direction_reverse(entranceExitCoords.direction);
                        gRideEntranceExitPlaceDirection = entranceExitCoords.direction;
                        return entranceExitCoords;
                    }
                    if (tileElement->AsTrack()->GetStationIndex() != gRideEntranceExitPlaceStationIndex)
                        continue;

                    int32_t eax = (entranceExitCoords.direction + 2 - tileElement->GetDirection())
                        & TILE_ELEMENT_DIRECTION_MASK;
                    if (TrackSequenceProperties[tileElement->AsTrack()->GetTrackType()]
                                               [tileElement->AsTrack()->GetSequenceIndex()]
                        & (1 << eax))
                    {
                        entranceExitCoords.direction = direction_reverse(entranceExitCoords.direction);
                        gRideEntranceExitPlaceDirection = entranceExitCoords.direction;
                        return entranceExitCoords;
                    }
                } while (!(tileElement++)->IsLastForTile());
            }
            entranceExitCoords.direction = (entranceExitCoords.direction + 1) & 3;
        }
        gRideEntranceExitPlaceDirection = INVALID_DIRECTION;
    }
    else
    {
        auto mapX = stationStart.x;
        auto mapY = stationStart.y;
        auto entranceMinX = mapX;
        auto entranceMinY = mapY;
        auto entranceMaxX = mapX;
        auto entranceMaxY = mapY;

        auto tileElement = ride_get_station_start_track_element(ride, gRideEntranceExitPlaceStationIndex);
        if (tileElement == nullptr)
        {
            entranceExitCoords.setNull();
            return entranceExitCoords;
        }
        entranceExitCoords.direction = tileElement->GetDirection();
        auto stationDirection = entranceExitCoords.direction;

        while (true)
        {
            entranceMaxX = mapX;
            entranceMaxY = mapY;
            mapX -= CoordsDirectionDelta[entranceExitCoords.direction].x;
            mapY -= CoordsDirectionDelta[entranceExitCoords.direction].y;
            tileElement = map_get_first_element_at({ mapX, mapY });
            if (tileElement == nullptr)
                break;
            bool goToNextTile = false;

            do
            {
                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;
                if (tileElement->AsTrack()->GetRideIndex() != gRideEntranceExitPlaceRideIndex)
                    continue;
                if (tileElement->AsTrack()->GetStationIndex() != gRideEntranceExitPlaceStationIndex)
                    continue;

                if (tileElement->AsTrack()->IsStation())
                {
                    goToNextTile = true;
                }
            } while (!goToNextTile && !(tileElement++)->IsLastForTile());

            if (!goToNextTile)
                break;
        }

        mapX = entranceMinX;
        if (mapX > entranceMaxX)
        {
            entranceMinX = entranceMaxX;
            entranceMaxX = mapX;
        }

        mapY = entranceMinY;
        if (mapY > entranceMaxY)
        {
            entranceMinY = entranceMaxY;
            entranceMaxY = mapY;
        }

        auto direction = loc_6CD18E(
            entranceExitCoords.x, entranceExitCoords.y, entranceMinX - 32, entranceMinY - 32, entranceMaxX + 32,
            entranceMaxY + 32);
        if (direction != -1 && direction != stationDirection && direction != direction_reverse(stationDirection))
        {
            entranceExitCoords.direction = direction;
            gRideEntranceExitPlaceDirection = entranceExitCoords.direction;
            return entranceExitCoords;
        }
    }
    return entranceExitCoords;
}

/**
 *
 *  rct2: 0x006CB945
 */
void sub_6CB945(Ride* ride)
{
    if (ride->type != RIDE_TYPE_MAZE)
    {
        for (StationIndex stationId = 0; stationId < MAX_STATIONS; ++stationId)
        {
            if (ride->stations[stationId].Start.isNull())
                continue;

            CoordsXYZ location = ride->stations[stationId].GetStart();
            uint8_t direction = INVALID_DIRECTION;

            bool specialTrack = false;
            TileElement* tileElement = nullptr;

            while (true)
            {
                if (direction != INVALID_DIRECTION)
                {
                    location.x -= CoordsDirectionDelta[direction].x;
                    location.y -= CoordsDirectionDelta[direction].y;
                }
                tileElement = map_get_first_element_at(location);
                if (tileElement == nullptr)
                    break;

                bool trackFound = false;
                do
                {
                    if (tileElement->GetBaseZ() != location.z)
                        continue;
                    if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                        continue;
                    if (tileElement->AsTrack()->GetRideIndex() != ride->id)
                        continue;
                    if (tileElement->AsTrack()->GetSequenceIndex() != 0)
                        continue;
                    if (!(TrackSequenceProperties[tileElement->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
                        continue;

                    trackFound = true;
                    break;
                } while (!(tileElement++)->IsLastForTile());

                if (!trackFound)
                {
                    break;
                }

                tileElement->AsTrack()->SetStationIndex(stationId);
                direction = tileElement->GetDirection();

                if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION))
                {
                    specialTrack = true;
                    break;
                }
            }

            if (!specialTrack)
            {
                continue;
            }

            const rct_preview_track* trackBlock = TrackBlocks[tileElement->AsTrack()->GetTrackType()];
            while ((++trackBlock)->index != 0xFF)
            {
                CoordsXYZ blockLocation = location + CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(direction), 0 };

                bool trackFound = false;
                tileElement = map_get_first_element_at(blockLocation);
                if (tileElement == nullptr)
                    break;
                do
                {
                    if (blockLocation.z != tileElement->GetBaseZ())
                        continue;
                    if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                        continue;
                    if (!(TrackSequenceProperties[tileElement->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
                        continue;

                    trackFound = true;
                    break;
                } while (!(tileElement++)->IsLastForTile());

                if (!trackFound)
                {
                    break;
                }

                tileElement->AsTrack()->SetStationIndex(stationId);
            }
        }
    }

    FixedVector<TileCoordsXYZD, MAX_STATION_LOCATIONS> locations;
    for (StationIndex stationId = 0; stationId < MAX_STATIONS; ++stationId)
    {
        auto entrance = ride_get_entrance_location(ride, stationId);
        if (!entrance.isNull())
        {
            locations.push_back(entrance);
            ride_clear_entrance_location(ride, stationId);
        }

        auto exit = ride_get_exit_location(ride, stationId);
        if (!exit.isNull())
        {
            locations.push_back(exit);
            ride_clear_exit_location(ride, stationId);
        }
    }

    auto locationListIter = locations.cbegin();
    for (const TileCoordsXYZD& locationCoords : locations)
    {
        auto locationList = ++locationListIter;

        bool duplicateLocation = false;
        while (locationList != locations.cend())
        {
            const TileCoordsXYZD& locationCoords2 = *locationList++;
            if (locationCoords.x == locationCoords2.x && locationCoords.y == locationCoords2.y
                && locationCoords.z == locationCoords2.z)
            {
                duplicateLocation = true;
                break;
            }
        }

        if (duplicateLocation)
        {
            continue;
        }

        CoordsXY location = locationCoords.ToCoordsXY();

        TileElement* tileElement = map_get_first_element_at(location);
        if (tileElement == nullptr)
            continue;
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;
            if (tileElement->base_height != locationCoords.z)
                continue;
            if (tileElement->AsEntrance()->GetRideIndex() != ride->id)
                continue;
            if (tileElement->AsEntrance()->GetEntranceType() > ENTRANCE_TYPE_RIDE_EXIT)
                continue;

            CoordsXY nextLocation = location;
            nextLocation.x += CoordsDirectionDelta[tileElement->GetDirection()].x;
            nextLocation.y += CoordsDirectionDelta[tileElement->GetDirection()].y;

            bool shouldRemove = true;
            TileElement* trackElement = map_get_first_element_at(nextLocation);
            if (trackElement == nullptr)
                continue;
            do
            {
                if (trackElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;
                if (trackElement->AsTrack()->GetRideIndex() != ride->id)
                    continue;
                if (trackElement->base_height != tileElement->base_height)
                    continue;

                auto trackType = trackElement->AsTrack()->GetTrackType();
                uint8_t trackSequence = trackElement->AsTrack()->GetSequenceIndex();

                Direction direction = (tileElement->GetDirection() - direction_reverse(trackElement->GetDirection())) & 3;

                if (!(TrackSequenceProperties[trackType][trackSequence] & (1 << direction)))
                {
                    continue;
                }

                StationIndex stationId = 0;
                if (trackType != TrackElemType::Maze)
                {
                    stationId = trackElement->AsTrack()->GetStationIndex();
                }

                if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT)
                {
                    if (!ride_get_exit_location(ride, stationId).isNull())
                        break;

                    CoordsXYZD loc = { location, ride->stations[stationId].GetBaseZ(), tileElement->GetDirection() };
                    ride_set_exit_location(ride, stationId, TileCoordsXYZD{ loc });
                }
                else
                {
                    if (!ride_get_entrance_location(ride, stationId).isNull())
                        break;

                    CoordsXYZD loc = { location, ride->stations[stationId].GetBaseZ(), tileElement->GetDirection() };
                    ride_set_entrance_location(ride, stationId, TileCoordsXYZD{ loc });
                }

                tileElement->AsEntrance()->SetStationIndex(stationId);
                shouldRemove = false;
            } while (!(trackElement++)->IsLastForTile());

            if (shouldRemove)
            {
                footpath_queue_chain_reset();
                maze_entrance_hedge_replacement({ location, tileElement });
                footpath_remove_edges_at(location, tileElement);
                footpath_update_queue_chains();
                map_invalidate_tile_full(location);
                tile_element_remove(tileElement);
                tileElement--;
            }
        } while (!(tileElement++)->IsLastForTile());
    }
}

bool ride_select_backwards_from_front()
{
    auto ride = get_ride(_currentRideIndex);
    if (ride != nullptr)
    {
        ride_construction_invalidate_current_track();
        track_begin_end trackBeginEnd;
        if (track_block_get_previous_from_zero(_currentTrackBegin, ride, _currentTrackPieceDirection, &trackBeginEnd))
        {
            _rideConstructionState = RideConstructionState::Selected;
            _currentTrackBegin.x = trackBeginEnd.begin_x;
            _currentTrackBegin.y = trackBeginEnd.begin_y;
            _currentTrackBegin.z = trackBeginEnd.begin_z;
            _currentTrackPieceDirection = trackBeginEnd.begin_direction;
            _currentTrackPieceType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            return true;
        }
    }
    return false;
}

bool ride_select_forwards_from_back()
{
    auto ride = get_ride(_currentRideIndex);
    if (ride != nullptr)
    {
        ride_construction_invalidate_current_track();

        int32_t z = _currentTrackBegin.z;
        int32_t direction = direction_reverse(_currentTrackPieceDirection);
        CoordsXYE next_track;
        if (track_block_get_next_from_zero(_currentTrackBegin, ride, direction, &next_track, &z, &direction, false))
        {
            _rideConstructionState = RideConstructionState::Selected;
            _currentTrackBegin.x = next_track.x;
            _currentTrackBegin.y = next_track.y;
            _currentTrackBegin.z = z;
            _currentTrackPieceDirection = next_track.element->GetDirection();
            _currentTrackPieceType = next_track.element->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            return true;
        }
    }
    return false;
}

/**
 *
 *  rct2: 0x006B58EF
 */
bool ride_are_all_possible_entrances_and_exits_built(Ride* ride)
{
    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP))
        return true;

    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->stations[i].Start.isNull())
        {
            continue;
        }
        if (ride_get_entrance_location(ride, i).isNull())
        {
            gGameCommandErrorText = STR_ENTRANCE_NOT_YET_BUILT;
            return false;
        }
        if (ride_get_exit_location(ride, i).isNull())
        {
            gGameCommandErrorText = STR_EXIT_NOT_YET_BUILT;
            return false;
        }
    }
    return true;
}
