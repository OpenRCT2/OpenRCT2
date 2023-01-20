/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideConstruction.h"

#include "../Context.h"
#include "../Input.h"
#include "../actions/MazeSetTrackAction.h"
#include "../actions/RideEntranceExitRemoveAction.h"
#include "../actions/RideSetSettingAction.h"
#include "../actions/RideSetStatusAction.h"
#include "../actions/RideSetVehicleAction.h"
#include "../actions/TrackRemoveAction.h"
#include "../common.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Formatter.h"
#include "../localisation/Localisation.h"
#include "../network/network.h"
#include "../paint/VirtualFloor.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/TileElementsView.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"
#include "TrainManager.h"
#include "Vehicle.h"

using namespace OpenRCT2::TrackMetaData;

money16 gTotalRideValueForMoney;

money32 _currentTrackPrice;

uint32_t _currentTrackCurve;
RideConstructionState _rideConstructionState;
RideId _currentRideIndex;

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

ObjectEntryIndex gLastEntranceStyle;

uint8_t gRideEntranceExitPlaceType;
RideId gRideEntranceExitPlaceRideIndex;
StationIndex gRideEntranceExitPlaceStationIndex;
RideConstructionState gRideEntranceExitPlacePreviousRideConstructionState;
Direction gRideEntranceExitPlaceDirection;

using namespace OpenRCT2;
using namespace OpenRCT2::TrackMetaData;

static int32_t ride_check_if_construction_allowed(Ride& ride)
{
    Formatter ft;
    RideObjectEntry* rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
    {
        ContextShowError(STR_INVALID_RIDE_TYPE, STR_CANT_EDIT_INVALID_RIDE_TYPE, ft);
        return 0;
    }
    if (ride.lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        ride.FormatNameTo(ft);
        ContextShowError(STR_CANT_START_CONSTRUCTION_ON, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING, ft);
        return 0;
    }

    if (ride.status != RideStatus::Closed && ride.status != RideStatus::Simulating)
    {
        ride.FormatNameTo(ft);
        ContextShowError(STR_CANT_START_CONSTRUCTION_ON, STR_MUST_BE_CLOSED_FIRST, ft);
        return 0;
    }

    return 1;
}

static rct_window* ride_create_or_find_construction_window(RideId rideIndex)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_FOCUS);
    intent.PutExtra(INTENT_EXTRA_RIDE_ID, rideIndex.ToUnderlying());
    windowManager->BroadcastIntent(intent);
    return WindowFindByClass(WindowClass::RideConstruction);
}

/**
 *
 *  rct2: 0x006B4857
 */
void RideConstructionStart(Ride& ride)
{
    CoordsXYE trackElement;
    if (RideTryGetOriginElement(ride, &trackElement))
    {
        ride.FindTrackGap(trackElement, &trackElement);

        rct_window* w = WindowGetMain();
        if (w != nullptr && RideModify(trackElement))
            WindowScrollToLocation(*w, { trackElement, trackElement.element->GetBaseZ() });
    }
    else
    {
        RideInitialiseConstructionWindow(ride);
    }
}

/**
 *
 *  rct2: 0x006DD4D5
 */
static void ride_remove_cable_lift(Ride& ride)
{
    if (ride.lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT)
    {
        ride.lifecycle_flags &= ~RIDE_LIFECYCLE_CABLE_LIFT;
        auto spriteIndex = ride.cable_lift;
        do
        {
            Vehicle* vehicle = GetEntity<Vehicle>(spriteIndex);
            if (vehicle == nullptr)
            {
                return;
            }
            vehicle->Invalidate();
            spriteIndex = vehicle->next_vehicle_on_train;
            EntityRemove(vehicle);
        } while (!spriteIndex.IsNull());
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

        for (size_t i = 0; i <= OpenRCT2::Limits::MaxTrainsPerRide; i++)
        {
            auto spriteIndex = vehicles[i];
            while (!spriteIndex.IsNull())
            {
                Vehicle* vehicle = GetEntity<Vehicle>(spriteIndex);
                if (vehicle == nullptr)
                {
                    break;
                }
                vehicle->Invalidate();
                spriteIndex = vehicle->next_vehicle_on_train;
                EntityRemove(vehicle);
            }

            vehicles[i] = EntityId::GetNull();
        }

        for (size_t i = 0; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
            stations[i].TrainAtStation = RideStation::NO_TRAIN;

        // Also clean up orphaned vehicles for good measure.
        for (auto* vehicle : TrainManager::View())
        {
            if (vehicle->ride == id)
            {
                vehicle->Invalidate();
                EntityRemove(vehicle);
            }
        }
    }
}

/**
 *
 *  rct2: 0x006DD4AC
 */
void RideClearForConstruction(Ride& ride)
{
    ride.measurement = {};

    ride.lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;

    // Open circuit rides will go directly into building mode (creating ghosts) where it would normally clear the stats,
    // however this causes desyncs since it's directly run from the window and other clients would not get it.
    // To prevent these problems, unconditionally invalidate the test results on all clients in multiplayer games.
    if (NetworkGetMode() != NETWORK_MODE_NONE)
    {
        InvalidateTestResults(ride);
    }

    ride_remove_cable_lift(ride);
    ride.RemoveVehicles();
    RideClearBlockedTiles(ride);

    auto w = WindowFindByNumber(WindowClass::Ride, ride.id.ToUnderlying());
    if (w != nullptr)
        WindowEventResizeCall(w);
}

/**
 *
 *  rct2: 0x006664DF
 */
void Ride::RemovePeeps()
{
    // Find first station
    auto stationIndex = RideGetFirstValidStationStart(*this);

    // Get exit position and direction
    auto exitPosition = CoordsXYZD{ 0, 0, 0, INVALID_DIRECTION };
    if (!stationIndex.IsNull())
    {
        auto location = GetStation(stationIndex).Exit.ToCoordsXYZD();
        if (!location.IsNull())
        {
            auto direction = DirectionReverse(location.direction);
            exitPosition = location;
            exitPosition.x += (DirectionOffsets[direction].x * 20) + COORDS_XY_HALF_TILE;
            exitPosition.y += (DirectionOffsets[direction].y * 20) + COORDS_XY_HALF_TILE;
            exitPosition.z += 2;

            // Reverse direction
            exitPosition.direction = DirectionReverse(exitPosition.direction);

            exitPosition.direction *= 8;
        }
    }

    // Place all the guests at exit
    for (auto peep : EntityList<Guest>())
    {
        if (peep->State == PeepState::QueuingFront || peep->State == PeepState::EnteringRide
            || peep->State == PeepState::LeavingRide || peep->State == PeepState::OnRide)
        {
            if (peep->CurrentRide != id)
                continue;

            PeepDecrementNumRiders(peep);
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
            if (peep->CurrentRide != id)
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
    num_riders = 0;
    slide_in_use = 0;
    window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN;
}

void RideClearBlockedTiles(const Ride& ride)
{
    for (TileCoordsXY tilePos = {}; tilePos.x < gMapSize.x; ++tilePos.x)
    {
        for (tilePos.y = 0; tilePos.y < gMapSize.y; ++tilePos.y)
        {
            for (auto* trackElement : TileElementsView<TrackElement>(tilePos.ToCoordsXY()))
            {
                if (trackElement->GetRideIndex() != ride.id)
                    continue;

                // Unblock footpath element that is at same position
                auto* footpathElement = MapGetFootpathElement(TileCoordsXYZ{ tilePos, trackElement->BaseHeight }.ToCoordsXYZ());

                if (footpathElement == nullptr)
                    continue;

                footpathElement->AsPath()->SetIsBlockedByVehicle(false);
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
std::optional<CoordsXYZ> GetTrackElementOriginAndApplyChanges(
    const CoordsXYZD& location, track_type_t type, uint16_t extra_params, TileElement** output_element, uint16_t flags)
{
    // Find the relevant track piece, prefer sequence 0 (this ensures correct behaviour for diagonal track pieces)
    auto trackElement = MapGetTrackElementAtOfTypeSeq(location, type, 0);
    if (trackElement == nullptr)
    {
        trackElement = MapGetTrackElementAtOfType(location, type);
        if (trackElement == nullptr)
        {
            return std::nullopt;
        }
    }

    // Possibly z should be & 0xF8
    const auto& ted = GetTrackElementDescriptor(type);

    // Now find all the elements that belong to this track piece
    int32_t sequence = trackElement->GetSequenceIndex();
    uint8_t mapDirection = trackElement->GetDirection();
    const auto* trackBlock = ted.GetBlockForSequence(sequence);
    if (trackBlock == nullptr)
        return std::nullopt;

    CoordsXY offsets = { trackBlock->x, trackBlock->y };
    CoordsXY newCoords = location;
    newCoords += offsets.Rotate(DirectionReverse(mapDirection));

    auto retCoordsXYZ = CoordsXYZ{ newCoords.x, newCoords.y, location.z - trackBlock->z };

    int32_t start_z = retCoordsXYZ.z;
    const auto block0 = ted.GetBlockForSequence(0);
    assert(block0 != nullptr);

    retCoordsXYZ.z += block0->z;
    for (int32_t i = 0; ted.Block[i].index != 0xFF; ++i)
    {
        CoordsXY cur = { retCoordsXYZ };
        offsets = { ted.Block[i].x, ted.Block[i].y };
        cur += offsets.Rotate(mapDirection);
        int32_t cur_z = start_z + ted.Block[i].z;

        MapInvalidateTileFull(cur);

        trackElement = MapGetTrackElementAtOfTypeSeq(
            { cur, cur_z, static_cast<Direction>(location.direction) }, type, ted.Block[i].index);
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

void RideRestoreProvisionalTrackPiece()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK)
    {
        RideId rideIndex;
        int32_t direction, type, liftHillAndAlternativeState;
        CoordsXYZ trackPos;
        if (WindowRideConstructionUpdateState(&type, &direction, &rideIndex, &liftHillAndAlternativeState, &trackPos, nullptr))
        {
            RideConstructionRemoveGhosts();
        }
        else
        {
            _currentTrackPrice = PlaceProvisionalTrackPiece(rideIndex, type, direction, liftHillAndAlternativeState, trackPos);
            WindowRideConstructionUpdateActiveElements();
        }
    }
}

void RideRemoveProvisionalTrackPiece()
{
    auto rideIndex = _currentRideIndex;
    auto ride = GetRide(rideIndex);
    if (ride == nullptr || !(_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK))
    {
        return;
    }

    int32_t x = _unkF440C5.x;
    int32_t y = _unkF440C5.y;
    int32_t z = _unkF440C5.z;

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
    {
        const int32_t flags = GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST;
        const CoordsXYZD quadrants[NumOrthogonalDirections] = {
            { x, y, z, 0 },
            { x, y + 16, z, 1 },
            { x + 16, y + 16, z, 2 },
            { x + 16, y, z, 3 },
        };
        for (const auto& quadrant : quadrants)
        {
            auto gameAction = MazeSetTrackAction(quadrant, false, rideIndex, GC_SET_MAZE_TRACK_FILL);
            gameAction.SetFlags(flags);
            auto res = GameActions::Execute(&gameAction);
        }
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
        if (TrackBlockGetNextFromZero({ x, y, z }, *ride, direction, &next_track, &z, &direction, true))
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
void RideConstructionRemoveGhosts()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT)
    {
        RideEntranceExitRemoveGhost();
        _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT;
    }
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK)
    {
        RideRemoveProvisionalTrackPiece();
        _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_TRACK;
    }
}

/*
 *  rct2: 0x006C9627
 */
void RideConstructionInvalidateCurrentTrack()
{
    switch (_rideConstructionState)
    {
        case RideConstructionState::Selected:
            GetTrackElementOriginAndApplyChanges(
                { _currentTrackBegin, static_cast<Direction>(_currentTrackPieceDirection & 3) }, _currentTrackPieceType, 0,
                nullptr, TRACK_ELEMENT_SET_HIGHLIGHT_FALSE);
            break;
        case RideConstructionState::MazeBuild:
        case RideConstructionState::MazeMove:
        case RideConstructionState::MazeFill:
        case RideConstructionState::Front:
        case RideConstructionState::Back:
            if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW)
            {
                MapInvalidateTileFull(_currentTrackBegin.ToTileStart());
            }
            RideConstructionRemoveGhosts();
            break;
        case RideConstructionState::Place:
        case RideConstructionState::EntranceExit:
        default:
            if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW)
            {
                _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_ARROW;
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
                MapInvalidateTileFull(_currentTrackBegin);
            }
            RideConstructionRemoveGhosts();
            break;
    }
}

/**
 *
 *  rct2: 0x006C9B19
 */
static void ride_construction_reset_current_piece()
{
    auto ride = GetRide(_currentRideIndex);
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
void RideConstructionSetDefaultNextPiece()
{
    auto rideIndex = _currentRideIndex;
    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
        return;

    const auto& rtd = ride->GetRideTypeDescriptor();

    int32_t z, direction, trackType, curve, bank, slope;
    TrackBeginEnd trackBeginEnd;
    CoordsXYE xyElement;
    TileElement* tileElement;
    _currentTrackPrice = MONEY32_UNDEFINED;

    const TrackElementDescriptor* ted;
    switch (_rideConstructionState)
    {
        case RideConstructionState::Front:
            direction = _currentTrackPieceDirection;
            if (!TrackBlockGetPreviousFromZero(_currentTrackBegin, *ride, direction, &trackBeginEnd))
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

            ted = &GetTrackElementDescriptor(trackType);
            curve = ted->CurveChain.next;
            bank = ted->Definition.bank_end;
            slope = ted->Definition.vangle_end;

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
            _currentTrackLiftHill = tileElement->AsTrack()->HasChain()
                && ((slope != TRACK_SLOPE_DOWN_25 && slope != TRACK_SLOPE_DOWN_60) || gCheatsEnableChainLiftOnAllTrack);
            break;
        case RideConstructionState::Back:
            direction = DirectionReverse(_currentTrackPieceDirection);
            if (!TrackBlockGetNextFromZero(_currentTrackBegin, *ride, direction, &xyElement, &z, &direction, false))
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

            ted = &GetTrackElementDescriptor(trackType);
            curve = ted->CurveChain.previous;
            bank = ted->Definition.bank_start;
            slope = ted->Definition.vangle_start;

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
void RideSelectNextSection()
{
    if (_rideConstructionState == RideConstructionState::Selected)
    {
        RideConstructionInvalidateCurrentTrack();
        int32_t direction = _currentTrackPieceDirection;
        int32_t type = _currentTrackPieceType;
        TileElement* tileElement;
        auto newCoords = GetTrackElementOriginAndApplyChanges(
            { _currentTrackBegin, static_cast<Direction>(direction & 3) }, type, 0, &tileElement, 0);
        if (!newCoords.has_value())
        {
            _rideConstructionState = RideConstructionState::State0;
            WindowRideConstructionUpdateActiveElements();
            return;
        }

        // Invalidate previous track piece (we may not be changing height!)
        VirtualFloorInvalidate();

        CoordsXYE inputElement, outputElement;
        inputElement.x = newCoords->x;
        inputElement.y = newCoords->y;
        inputElement.element = tileElement;
        if (TrackBlockGetNext(&inputElement, &outputElement, &newCoords->z, &direction))
        {
            newCoords->x = outputElement.x;
            newCoords->y = outputElement.y;
            tileElement = outputElement.element;
            if (!SceneryToolIsActive())
            {
                // Set next element's height.
                VirtualFloorSetHeight(tileElement->GetBaseZ());
            }

            _currentTrackBegin = *newCoords;
            _currentTrackPieceDirection = tileElement->GetDirection();
            _currentTrackPieceType = tileElement->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            WindowRideConstructionUpdateActiveElements();
        }
        else
        {
            _rideConstructionState = RideConstructionState::Front;
            _currentTrackBegin = { outputElement, newCoords->z };
            _currentTrackPieceDirection = direction;
            _currentTrackPieceType = tileElement->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            RideConstructionSetDefaultNextPiece();
            WindowRideConstructionUpdateActiveElements();
        }
    }
    else if (_rideConstructionState == RideConstructionState::Back)
    {
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        if (RideSelectForwardsFromBack())
        {
            WindowRideConstructionUpdateActiveElements();
        }
    }
}

/**
 *
 *  rct2: 0x006C93B8
 */
void RideSelectPreviousSection()
{
    if (_rideConstructionState == RideConstructionState::Selected)
    {
        RideConstructionInvalidateCurrentTrack();
        int32_t direction = _currentTrackPieceDirection;
        int32_t type = _currentTrackPieceType;
        TileElement* tileElement;
        auto newCoords = GetTrackElementOriginAndApplyChanges(
            { _currentTrackBegin, static_cast<Direction>(direction & 3) }, type, 0, &tileElement, 0);
        if (newCoords == std::nullopt)
        {
            _rideConstructionState = RideConstructionState::State0;
            WindowRideConstructionUpdateActiveElements();
            return;
        }

        // Invalidate previous track piece (we may not be changing height!)
        VirtualFloorInvalidate();

        TrackBeginEnd trackBeginEnd;
        if (TrackBlockGetPrevious({ *newCoords, tileElement }, &trackBeginEnd))
        {
            _currentTrackBegin.x = trackBeginEnd.begin_x;
            _currentTrackBegin.y = trackBeginEnd.begin_y;
            _currentTrackBegin.z = trackBeginEnd.begin_z;
            _currentTrackPieceDirection = trackBeginEnd.begin_direction;
            _currentTrackPieceType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            if (!SceneryToolIsActive())
            {
                // Set previous element's height.
                VirtualFloorSetHeight(trackBeginEnd.begin_element->GetBaseZ());
            }
            WindowRideConstructionUpdateActiveElements();
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
            RideConstructionSetDefaultNextPiece();
            WindowRideConstructionUpdateActiveElements();
        }
    }
    else if (_rideConstructionState == RideConstructionState::Front)
    {
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        if (RideSelectBackwardsFromFront())
        {
            WindowRideConstructionUpdateActiveElements();
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
    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
        return false;

    auto entranceType = entranceElement->GetEntranceType();
    if (entranceType != ENTRANCE_TYPE_RIDE_ENTRANCE && entranceType != ENTRANCE_TYPE_RIDE_EXIT)
        return false;

    auto stationIndex = entranceElement->GetStationIndex();

    // Get or create construction window for ride
    auto constructionWindow = WindowFindByClass(WindowClass::RideConstruction);
    if (constructionWindow == nullptr)
    {
        if (!RideInitialiseConstructionWindow(*ride))
            return false;

        constructionWindow = WindowFindByClass(WindowClass::RideConstruction);
        if (constructionWindow == nullptr)
            return false;
    }

    RideConstructionInvalidateCurrentTrack();
    if (_rideConstructionState != RideConstructionState::EntranceExit || !(InputTestFlag(INPUT_FLAG_TOOL_ACTIVE))
        || gCurrentToolWidget.window_classification != WindowClass::RideConstruction)
    {
        // Replace entrance / exit
        ToolSet(
            *constructionWindow,
            entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE ? WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE : WC_RIDE_CONSTRUCTION__WIDX_EXIT,
            Tool::Crosshair);
        gRideEntranceExitPlaceType = entranceType;
        gRideEntranceExitPlaceRideIndex = rideIndex;
        gRideEntranceExitPlaceStationIndex = stationIndex;
        InputSetFlag(INPUT_FLAG_6, true);
        if (_rideConstructionState != RideConstructionState::EntranceExit)
        {
            gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
            _rideConstructionState = RideConstructionState::EntranceExit;
        }

        WindowRideConstructionUpdateActiveElements();
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
            WindowInvalidateByClass(WindowClass::RideConstruction);
        });

        GameActions::Execute(&rideEntranceExitRemove);
    }

    WindowInvalidateByClass(WindowClass::RideConstruction);
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
            _rideConstructionState = RideConstructionState::MazeBuild;
            _currentTrackBegin.x = tileElement.x;
            _currentTrackBegin.y = tileElement.y;
            _currentTrackBegin.z = trackElement->GetBaseZ();
            _currentTrackSelectionFlags = 0;
            _rideConstructionNextArrowPulse = 0;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

            auto intent = Intent(INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION);
            ContextBroadcastIntent(&intent);
            return true;
        }
    }
    return false;
}

/**
 *
 *  rct2: 0x006CC056
 */
bool RideModify(const CoordsXYE& input)
{
    auto tileElement = input;
    if (tileElement.element == nullptr)
        return false;

    auto rideIndex = tileElement.element->GetRideIndex();
    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
    {
        return false;
    }

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr || !ride_check_if_construction_allowed(*ride))
        return false;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
    {
        Formatter ft;
        ride->FormatNameTo(ft);
        ContextShowError(
            STR_CANT_START_CONSTRUCTION_ON, STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE, ft);
        return false;
    }

    // Stop the ride again to clear all vehicles and peeps (compatible with network games)
    if (ride->status != RideStatus::Simulating)
    {
        auto gameAction = RideSetStatusAction(ride->id, RideStatus::Closed);
        GameActions::Execute(&gameAction);
    }

    // Check if element is a station entrance or exit
    if (tileElement.element->GetType() == TileElementType::Entrance)
        return ride_modify_entrance_or_exit(tileElement);

    ride_create_or_find_construction_window(rideIndex);

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
    {
        return ride_modify_maze(tileElement);
    }

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS))
    {
        CoordsXYE endOfTrackElement{};
        if (ride->FindTrackGap(tileElement, &endOfTrackElement))
            tileElement = endOfTrackElement;
    }

    if (tileElement.element == nullptr || tileElement.element->GetType() != TileElementType::Track)
        return false;

    auto tileCoords = CoordsXYZ{ tileElement, tileElement.element->GetBaseZ() };
    auto direction = tileElement.element->GetDirection();
    auto type = tileElement.element->AsTrack()->GetTrackType();
    auto newCoords = GetTrackElementOriginAndApplyChanges({ tileCoords, direction }, type, 0, nullptr, 0);
    if (!newCoords.has_value())
        return false;

    _currentRideIndex = rideIndex;
    _rideConstructionState = RideConstructionState::Selected;
    _currentTrackBegin = newCoords.value();
    _currentTrackPieceDirection = direction;
    _currentTrackPieceType = type;
    _currentTrackSelectionFlags = 0;
    _rideConstructionNextArrowPulse = 0;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK))
    {
        WindowRideConstructionUpdateActiveElements();
        return true;
    }

    RideSelectNextSection();
    if (_rideConstructionState == RideConstructionState::Front)
    {
        WindowRideConstructionUpdateActiveElements();
        return true;
    }

    _rideConstructionState = RideConstructionState::Selected;
    _currentTrackBegin = *newCoords;
    _currentTrackPieceDirection = direction;
    _currentTrackPieceType = type;
    _currentTrackSelectionFlags = 0;

    RideSelectPreviousSection();

    if (_rideConstructionState != RideConstructionState::Back)
    {
        _rideConstructionState = RideConstructionState::Selected;
        _currentTrackBegin = *newCoords;
        _currentTrackPieceDirection = direction;
        _currentTrackPieceType = type;
        _currentTrackSelectionFlags = 0;
    }

    WindowRideConstructionUpdateActiveElements();
    return true;
}

/**
 *
 *  rct2: 0x006CC3FB
 */
int32_t RideInitialiseConstructionWindow(Ride& ride)
{
    rct_window* w;

    ToolCancel();

    if (!ride_check_if_construction_allowed(ride))
        return 0;

    RideClearForConstruction(ride);
    ride.RemovePeeps();

    w = ride_create_or_find_construction_window(ride.id);

    ToolSet(*w, WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT, Tool::Crosshair);
    InputSetFlag(INPUT_FLAG_6, true);

    _currentTrackCurve = ride.GetRideTypeDescriptor().StartTrackPiece | RideConstructionSpecialPieceSelected;
    _currentTrackSlopeEnd = 0;
    _currentTrackBankEnd = 0;
    _currentTrackLiftHill = 0;
    _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;

    if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_START_CONSTRUCTION_INVERTED))
        _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;

    _previousTrackBankEnd = 0;
    _previousTrackSlopeEnd = 0;

    _currentTrackPieceDirection = 0;
    _rideConstructionState = RideConstructionState::Place;
    _currentTrackSelectionFlags = 0;

    WindowRideConstructionUpdateActiveElements();
    return 1;
}

/**
 *
 *  rct2: 0x006CB7FB
 */
int32_t RideGetRefundPrice(const Ride& ride)
{
    CoordsXYE trackElement;
    money32 cost = 0;

    if (!RideTryGetOriginElement(ride, &trackElement))
    {
        return 0; // Ride has no track to refund
    }

    // Find the start in case it is not a complete circuit
    RideGetStartOfTrack(&trackElement);

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

        cost += res.Cost;

        if (!TrackBlockGetNext(&trackElement, &trackElement, nullptr, nullptr))
        {
            break;
        }

        moveSlowIt = !moveSlowIt;
        if (moveSlowIt)
        {
            if (!TrackBlockGetNext(&slowIt, &slowIt, nullptr, nullptr) || slowIt.element == trackElement.element)
            {
                break;
            }
        }

        direction = trackElement.element->GetDirection();

    } while (trackElement.element != initial_map);

    return cost;
}

money32 SetOperatingSetting(RideId rideId, RideSetSetting setting, uint8_t value)
{
    auto rideSetSetting = RideSetSettingAction(rideId, setting, value);
    auto res = GameActions::Execute(&rideSetSetting);
    return res.Error == GameActions::Status::Ok ? 0 : MONEY32_UNDEFINED;
}

money32 SetOperatingSettingNested(RideId rideId, RideSetSetting setting, uint8_t value, uint8_t flags)
{
    auto rideSetSetting = RideSetSettingAction(rideId, setting, value);
    rideSetSetting.SetFlags(flags);
    auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&rideSetSetting)
                                               : GameActions::QueryNested(&rideSetSetting);
    return res.Error == GameActions::Status::Ok ? 0 : MONEY32_UNDEFINED;
}

/**
 *
 *  rct2: 0x006CCF70
 */
CoordsXYZD RideGetEntranceOrExitPositionFromScreenPosition(const ScreenCoordsXY& screenCoords)
{
    CoordsXYZD entranceExitCoords{};
    gRideEntranceExitPlaceDirection = INVALID_DIRECTION;
    // determine if the mouse is hovering over a station - that's the station to add the entrance to
    auto info = GetMapCoordinatesFromPos(screenCoords, EnumsToFlags(ViewportInteractionItem::Ride));
    if (info.SpriteType != ViewportInteractionItem::None)
    {
        if (info.Element->GetType() == TileElementType::Track)
        {
            const auto* trackElement = info.Element->AsTrack();
            if (trackElement->GetRideIndex() == gRideEntranceExitPlaceRideIndex)
            {
                const auto& ted = GetTrackElementDescriptor(trackElement->GetTrackType());
                if (std::get<0>(ted.SequenceProperties) & TRACK_SEQUENCE_FLAG_ORIGIN)
                {
                    if (trackElement->GetTrackType() == TrackElemType::Maze)
                    {
                        gRideEntranceExitPlaceStationIndex = StationIndex::FromUnderlying(0);
                    }
                    else
                    {
                        gRideEntranceExitPlaceStationIndex = trackElement->GetStationIndex();
                    }
                }
            }
        }
    }

    auto ride = GetRide(gRideEntranceExitPlaceRideIndex);
    if (ride == nullptr)
    {
        entranceExitCoords.SetNull();
        return entranceExitCoords;
    }

    auto stationBaseZ = ride->GetStation(gRideEntranceExitPlaceStationIndex).GetBaseZ();

    auto coordsAtHeight = ScreenGetMapXYWithZ(screenCoords, stationBaseZ);
    if (!coordsAtHeight.has_value())
    {
        entranceExitCoords.SetNull();
        return entranceExitCoords;
    }

    entranceExitCoords = { coordsAtHeight->ToTileStart(), stationBaseZ, INVALID_DIRECTION };

    if (ride->type == RIDE_TYPE_NULL)
    {
        entranceExitCoords.SetNull();
        return entranceExitCoords;
    }

    auto stationStart = ride->GetStation(gRideEntranceExitPlaceStationIndex).Start;
    if (stationStart.IsNull())
    {
        entranceExitCoords.SetNull();
        return entranceExitCoords;
    }

    // find the quadrant the mouse is hovering over - that's the direction to start searching for a station TileElement
    Direction startDirection = 0;
    auto mapX = (coordsAtHeight->x & 0x1F) - 16;
    auto mapY = (coordsAtHeight->y & 0x1F) - 16;
    if (std::abs(mapX) < std::abs(mapY))
    {
        startDirection = mapY < 0 ? 3 : 1;
    }
    else
    {
        startDirection = mapX < 0 ? 0 : 2;
    }
    // check all 4 directions, starting with the mouse's quadrant
    for (uint8_t directionIncrement = 0; directionIncrement < 4; directionIncrement++)
    {
        entranceExitCoords.direction = (startDirection + directionIncrement) & 3;
        // search for TrackElement one tile over, shifted in the search direction
        auto nextLocation = entranceExitCoords;
        nextLocation += CoordsDirectionDelta[entranceExitCoords.direction];
        if (MapIsLocationValid(nextLocation))
        {
            // iterate over every element in the tile until we find what we want
            auto* tileElement = MapGetFirstElementAt(nextLocation);
            if (tileElement == nullptr)
                continue;
            do
            {
                if (tileElement->GetType() != TileElementType::Track)
                    continue;
                if (tileElement->GetBaseZ() != stationBaseZ)
                    continue;
                auto* trackElement = tileElement->AsTrack();
                if (trackElement->GetRideIndex() != gRideEntranceExitPlaceRideIndex)
                    continue;
                if (trackElement->GetTrackType() == TrackElemType::Maze)
                {
                    // if it's a maze, it can place the entrance and exit immediately
                    entranceExitCoords.direction = DirectionReverse(entranceExitCoords.direction);
                    gRideEntranceExitPlaceDirection = entranceExitCoords.direction;
                    return entranceExitCoords;
                }
                // if it's not a maze, the sequence properties for the TrackElement must be found to determine if an
                // entrance can be placed on that side

                gRideEntranceExitPlaceStationIndex = trackElement->GetStationIndex();

                // get the ride entrance's side relative to the TrackElement
                Direction direction = (DirectionReverse(entranceExitCoords.direction) - tileElement->GetDirection()) & 3;
                const auto& ted = GetTrackElementDescriptor(trackElement->GetTrackType());
                if (ted.SequenceProperties[trackElement->GetSequenceIndex()] & (1 << direction))
                {
                    // if that side of the TrackElement supports stations, the ride entrance is valid and faces away from
                    // the station
                    entranceExitCoords.direction = DirectionReverse(entranceExitCoords.direction);
                    gRideEntranceExitPlaceDirection = entranceExitCoords.direction;
                    return entranceExitCoords;
                }
            } while (!(tileElement++)->IsLastForTile());
        }
    }
    gRideEntranceExitPlaceDirection = INVALID_DIRECTION;
    return entranceExitCoords;
}

/**
 *
 *  rct2: 0x006CB945
 */
void Ride::ValidateStations()
{
    const TrackElementDescriptor* ted;
    const auto& rtd = GetRideTypeDescriptor();
    if (!rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
    {
        // find the stations of the ride to begin stepping over track elements from
        for (const auto& station : stations)
        {
            if (station.Start.IsNull())
                continue;

            CoordsXYZ location = station.GetStart();
            uint8_t direction = INVALID_DIRECTION;

            bool specialTrack = false;
            TileElement* tileElement = nullptr;
            while (true)
            {
                // search backwards for the previous station TrackElement (only if the first station TrackElement is found)
                if (direction != INVALID_DIRECTION)
                {
                    location.x -= CoordsDirectionDelta[direction].x;
                    location.y -= CoordsDirectionDelta[direction].y;
                }
                tileElement = MapGetFirstElementAt(location);
                if (tileElement == nullptr)
                    break;

                // find the target TrackElement on the tile it's supposed to appear on
                bool trackFound = false;
                do
                {
                    if (tileElement->GetBaseZ() != location.z)
                        continue;
                    if (tileElement->GetType() != TileElementType::Track)
                        continue;
                    if (tileElement->AsTrack()->GetRideIndex() != id)
                        continue;
                    if (tileElement->AsTrack()->GetSequenceIndex() != 0)
                        continue;

                    ted = &GetTrackElementDescriptor(tileElement->AsTrack()->GetTrackType());
                    // keep searching for a station piece (coaster station, tower ride base, shops, and flat ride base)
                    if (!(std::get<0>(ted->SequenceProperties) & TRACK_SEQUENCE_FLAG_ORIGIN))
                        continue;

                    trackFound = true;
                    break;
                } while (!(tileElement++)->IsLastForTile());

                if (!trackFound)
                {
                    break;
                }
                // update the StationIndex, get the TrackElement's rotation
                tileElement->AsTrack()->SetStationIndex(GetStationIndex(&station));
                direction = tileElement->GetDirection();

                // In the future this could look at the TED and see if the station has a sequence longer than 1
                // tower ride, flat ride, shop
                if (GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION))
                {
                    // if the track has multiple sequences, stop looking for the next one.
                    specialTrack = true;
                    break;
                }
            }

            // if the track piece is not a tower ride, flat ride, or shop, continue to the next StationIndex
            if (!specialTrack)
            {
                continue;
            }
            // update all the blocks with StationIndex
            ted = &GetTrackElementDescriptor(tileElement->AsTrack()->GetTrackType());
            const rct_preview_track* trackBlock = ted->Block;
            while ((++trackBlock)->index != 0xFF)
            {
                CoordsXYZ blockLocation = location + CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(direction), 0 };

                bool trackFound = false;
                tileElement = MapGetFirstElementAt(blockLocation);
                if (tileElement == nullptr)
                    break;
                // find the target TrackElement on the tile it's supposed to appear on
                do
                {
                    if (blockLocation.z != tileElement->GetBaseZ())
                        continue;
                    if (tileElement->GetType() != TileElementType::Track)
                        continue;

                    ted = &GetTrackElementDescriptor(tileElement->AsTrack()->GetTrackType());
                    if (!(std::get<0>(ted->SequenceProperties) & TRACK_SEQUENCE_FLAG_ORIGIN))
                        continue;

                    trackFound = true;
                    break;
                } while (!(tileElement++)->IsLastForTile());

                if (!trackFound)
                {
                    // Critical error! Stop trying to find the next sequence to set StationIndex.
                    break;
                }

                tileElement->AsTrack()->SetStationIndex(GetStationIndex(&station));
            }
        }
    }
    // determine what entrances and exits exist
    FixedVector<TileCoordsXYZD, MAX_STATION_LOCATIONS> locations;
    for (auto& station : stations)
    {
        if (!station.Entrance.IsNull())
        {
            locations.push_back(station.Entrance);
            station.Entrance.SetNull();
        }

        if (!station.Exit.IsNull())
        {
            locations.push_back(station.Exit);
            station.Exit.SetNull();
        }
    }

    auto locationListIter = locations.cbegin();
    for (const TileCoordsXYZD& locationCoords : locations)
    {
        auto locationList = ++locationListIter;
        // determine if there's another ride entrance at this location later in the array
        // if there is, skip it. The last ride entrance in the array at the location is not skipped
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
            // if it's a duplicate continue to the next ride entrance
            continue;
        }
        // if it's not a duplicate location
        CoordsXY location = locationCoords.ToCoordsXY();

        TileElement* tileElement = MapGetFirstElementAt(location);
        if (tileElement == nullptr)
            continue;
        do
        {
            if (tileElement->GetType() != TileElementType::Entrance)
                continue;
            if (tileElement->BaseHeight != locationCoords.z)
                continue;
            if (tileElement->AsEntrance()->GetRideIndex() != id)
                continue;
            // if it's a park entrance continue to the next tile element
            if (tileElement->AsEntrance()->GetEntranceType() > ENTRANCE_TYPE_RIDE_EXIT)
                continue;

            // find the station that's connected to this ride entrance
            CoordsXY nextLocation = location;
            nextLocation.x += CoordsDirectionDelta[tileElement->GetDirection()].x;
            nextLocation.y += CoordsDirectionDelta[tileElement->GetDirection()].y;

            // if there's no connected station, remove the ride entrance (see below)
            bool shouldRemove = true;
            TileElement* trackElement = MapGetFirstElementAt(nextLocation);
            if (trackElement == nullptr)
                continue;
            do
            {
                if (trackElement->GetType() != TileElementType::Track)
                    continue;
                if (trackElement->AsTrack()->GetRideIndex() != id)
                    continue;
                if (trackElement->BaseHeight != tileElement->BaseHeight)
                    continue;

                auto trackType = trackElement->AsTrack()->GetTrackType();

                // get the StationIndex for the station
                StationIndex stationId = StationIndex::FromUnderlying(0);
                if (trackType != TrackElemType::Maze)
                {
                    uint8_t trackSequence = trackElement->AsTrack()->GetSequenceIndex();

                    // determine where the ride entrance is relative to the station track
                    Direction direction = (tileElement->GetDirection() - DirectionReverse(trackElement->GetDirection())) & 3;

                    // if the ride entrance is not on a valid side, remove it
                    ted = &GetTrackElementDescriptor(trackType);
                    if (!(ted->SequenceProperties[trackSequence] & (1 << direction)))
                    {
                        continue;
                    }

                    stationId = trackElement->AsTrack()->GetStationIndex();
                }

                auto& station = GetStation(stationId);
                if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT)
                {
                    // if the location is already set for this station, big problem!
                    if (!station.Exit.IsNull())
                        break;
                    // set the station's exit location to this one
                    CoordsXYZD loc = { location, station.GetBaseZ(), tileElement->GetDirection() };
                    station.Exit = TileCoordsXYZD{ loc };
                }
                else
                {
                    // if the location is already set for this station, big problem!
                    if (!station.Entrance.IsNull())
                        break;
                    // set the station's entrance location to this one
                    CoordsXYZD loc = { location, station.GetBaseZ(), tileElement->GetDirection() };
                    station.Entrance = TileCoordsXYZD{ loc };
                }
                // set the entrance's StationIndex as this station
                tileElement->AsEntrance()->SetStationIndex(stationId);
                shouldRemove = false;
            } while (!(trackElement++)->IsLastForTile());

            // remove the ride entrance and clean up if necessary
            if (shouldRemove)
            {
                FootpathQueueChainReset();
                MazeEntranceHedgeReplacement({ location, tileElement });
                FootpathRemoveEdgesAt(location, tileElement);
                FootpathUpdateQueueChains();
                MapInvalidateTileFull(location);
                TileElementRemove(tileElement);
                tileElement--;
            }
        } while (!(tileElement++)->IsLastForTile());
    }
}

bool RideSelectBackwardsFromFront()
{
    auto ride = GetRide(_currentRideIndex);
    if (ride != nullptr)
    {
        RideConstructionInvalidateCurrentTrack();
        TrackBeginEnd trackBeginEnd;
        if (TrackBlockGetPreviousFromZero(_currentTrackBegin, *ride, _currentTrackPieceDirection, &trackBeginEnd))
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

bool RideSelectForwardsFromBack()
{
    auto ride = GetRide(_currentRideIndex);
    if (ride != nullptr)
    {
        RideConstructionInvalidateCurrentTrack();

        int32_t z = _currentTrackBegin.z;
        int32_t direction = DirectionReverse(_currentTrackPieceDirection);
        CoordsXYE next_track;
        if (TrackBlockGetNextFromZero(_currentTrackBegin, *ride, direction, &next_track, &z, &direction, false))
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
ResultWithMessage RideAreAllPossibleEntrancesAndExitsBuilt(const Ride& ride)
{
    if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
        return { true };

    for (auto& station : ride.GetStations())
    {
        if (station.Start.IsNull())
        {
            continue;
        }
        if (station.Entrance.IsNull())
        {
            return { false, STR_ENTRANCE_NOT_YET_BUILT };
        }
        if (station.Exit.IsNull())
        {
            return { false, STR_EXIT_NOT_YET_BUILT };
        }
    }
    return { true };
}
