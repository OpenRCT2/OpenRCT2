/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideConstruction.h"

#include "../Context.h"
#include "../GameState.h"
#include "../Input.h"
#include "../actions/ResultWithMessage.h"
#include "../actions/RideEntranceExitRemoveAction.h"
#include "../actions/RideSetSettingAction.h"
#include "../actions/RideSetStatusAction.h"
#include "../actions/TrackRemoveAction.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../interface/WindowBase.h"
#include "../localisation/Formatter.h"
#include "../network/Network.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/MapSelection.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"
#include "TrainManager.h"
#include "Vehicle.h"

#include <cassert>
#include <sfl/static_vector.hpp>

using namespace OpenRCT2;
using namespace OpenRCT2::TrackMetaData;
using OpenRCT2::GameActions::CommandFlag;
using OpenRCT2::GameActions::CommandFlags;

money64 _currentTrackPrice;

TypeOrCurve _currentlySelectedTrack{};
RideConstructionState _rideConstructionState;
RideId _currentRideIndex;

CoordsXYZ _currentTrackBegin;

uint8_t _currentTrackPieceDirection;
TrackElemType _currentTrackPieceType;
TrackSelectionFlags _currentTrackSelectionFlags;
uint32_t _rideConstructionNextArrowPulse = 0;
TrackPitch _currentTrackPitchEnd;
TrackRoll _currentTrackRollEnd;
bool _currentTrackHasLiftHill;
SelectedAlternative _currentTrackAlternative{};
TrackElemType _selectedTrackType;

TrackRoll _previousTrackRollEnd;
TrackPitch _previousTrackPitchEnd;

CoordsXYZ _previousTrackPiece;

uint8_t _currentBrakeSpeed;
RideColourScheme _currentColourScheme;
uint8_t _currentSeatRotationAngle;

CoordsXYZD _unkF440C5;

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
    const auto* rideEntry = ride.getRideEntry();
    if (rideEntry == nullptr)
    {
        ContextShowError(STR_INVALID_RIDE_TYPE, STR_CANT_EDIT_INVALID_RIDE_TYPE, ft);
        return 0;
    }
    if (ride.lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        ride.formatNameTo(ft);
        ContextShowError(STR_CANT_START_CONSTRUCTION_ON, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING, ft);
        return 0;
    }

    if (ride.status != RideStatus::closed && ride.status != RideStatus::simulating)
    {
        ride.formatNameTo(ft);
        ContextShowError(STR_CANT_START_CONSTRUCTION_ON, STR_MUST_BE_CLOSED_FIRST, ft);
        return 0;
    }

    return 1;
}

static WindowBase* ride_create_or_find_construction_window(RideId rideIndex)
{
    auto* windowManager = Ui::GetWindowManager();
    auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_FOCUS);
    intent.PutExtra(INTENT_EXTRA_RIDE_ID, rideIndex.ToUnderlying());
    windowManager->BroadcastIntent(intent);
    return windowManager->FindByClass(WindowClass::rideConstruction);
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
        ride.findTrackGap(trackElement, &trackElement);

        WindowBase* w = WindowGetMain();
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
    if (ride.lifecycleFlags & RIDE_LIFECYCLE_CABLE_LIFT)
    {
        ride.lifecycleFlags &= ~RIDE_LIFECYCLE_CABLE_LIFT;
        auto spriteIndex = ride.cableLift;
        do
        {
            Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(spriteIndex);
            if (vehicle == nullptr)
            {
                return;
            }
            vehicle->Invalidate();
            spriteIndex = vehicle->next_vehicle_on_train;
            getGameState().entities.EntityRemove(vehicle);
        } while (!spriteIndex.IsNull());
    }
}

/**
 *
 *  rct2: 0x006DD506
 */
void Ride::removeVehicles()
{
    if (lifecycleFlags & RIDE_LIFECYCLE_ON_TRACK)
    {
        lifecycleFlags &= ~RIDE_LIFECYCLE_ON_TRACK;
        lifecycleFlags &= ~(RIDE_LIFECYCLE_TEST_IN_PROGRESS | RIDE_LIFECYCLE_HAS_STALLED_VEHICLE);

        for (size_t i = 0; i <= Limits::kMaxTrainsPerRide; i++)
        {
            auto spriteIndex = vehicles[i];
            while (!spriteIndex.IsNull())
            {
                Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(spriteIndex);
                if (vehicle == nullptr)
                {
                    break;
                }
                vehicle->Invalidate();
                spriteIndex = vehicle->next_vehicle_on_train;
                getGameState().entities.EntityRemove(vehicle);
            }

            vehicles[i] = EntityId::GetNull();
        }

        for (size_t i = 0; i < Limits::kMaxStationsPerRide; i++)
            stations[i].TrainAtStation = RideStation::kNoTrain;

        // Also clean up orphaned vehicles for good measure.
        for (auto* vehicle : TrainManager::View())
        {
            if (vehicle->ride == id)
            {
                vehicle->Invalidate();
                getGameState().entities.EntityRemove(vehicle);
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

    ride.lifecycleFlags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
    ride.windowInvalidateFlags.set(RideInvalidateFlag::main, RideInvalidateFlag::list);

    // Open circuit rides will go directly into building mode (creating ghosts) where it would normally clear the stats,
    // however this causes desyncs since it's directly run from the window and other clients would not get it.
    // To prevent these problems, unconditionally invalidate the test results on all clients in multiplayer games.
    if (Network::GetMode() != Network::Mode::none)
    {
        InvalidateTestResults(ride);
    }

    ride_remove_cable_lift(ride);
    ride.removeVehicles();
    RideClearBlockedTiles(ride);

    // Force close simulating rides, to reset flag widget in Construction UI
    if (ride.status == RideStatus::simulating)
    {
        ride.status = RideStatus::closed;
    }

    auto* windowMgr = Ui::GetWindowManager();
    auto w = windowMgr->FindByNumber(WindowClass::ride, ride.id.ToUnderlying());
    if (w != nullptr)
        w->onResize();
}

/**
 *
 *  rct2: 0x006664DF
 */
void Ride::removePeeps()
{
    // Find first station
    auto stationIndex = RideGetFirstValidStationStart(*this);

    // Get exit position and direction
    auto exitPosition = CoordsXYZD{ 0, 0, 0, kInvalidDirection };
    if (!stationIndex.IsNull())
    {
        auto location = getStation(stationIndex).Exit.ToCoordsXYZD();
        if (!location.IsNull())
        {
            auto direction = DirectionReverse(location.direction);
            exitPosition = location;
            exitPosition.x += (DirectionOffsets[direction].x * 20) + kCoordsXYHalfTile;
            exitPosition.y += (DirectionOffsets[direction].y * 20) + kCoordsXYHalfTile;
            exitPosition.z += 2;

            // Reverse direction
            exitPosition.direction = DirectionReverse(exitPosition.direction);

            exitPosition.direction *= 8;
        }
    }

    // Place all the guests at exit
    for (auto peep : EntityList<Guest>())
    {
        if (peep->State == PeepState::queuingFront || peep->State == PeepState::enteringRide
            || peep->State == PeepState::leavingRide || peep->State == PeepState::onRide)
        {
            if (peep->CurrentRide != id)
                continue;

            PeepDecrementNumRiders(peep);
            if (peep->State == PeepState::queuingFront && peep->RideSubState == PeepRideSubState::atEntrance)
                peep->RemoveFromQueue();

            if (exitPosition.direction == kInvalidDirection)
            {
                CoordsXYZ newLoc = { peep->NextLoc.ToTileCentre(), peep->NextLoc.z };
                if (peep->GetNextIsSloped())
                    newLoc.z += kCoordsZStep;
                newLoc.z++;
                peep->MoveTo(newLoc);
            }
            else
            {
                peep->MoveTo(exitPosition);
                peep->Orientation = exitPosition.direction;
            }

            peep->State = PeepState::falling;
            peep->SwitchToSpecialSprite(0);

            peep->Happiness = std::min(peep->Happiness, peep->HappinessTarget) / 2;
            peep->HappinessTarget = peep->Happiness;
            peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_STATS;
        }
    }
    // Place all the staff at exit
    for (auto peep : EntityList<Staff>())
    {
        if (peep->State == PeepState::fixing || peep->State == PeepState::inspecting)
        {
            if (peep->CurrentRide != id)
                continue;

            if (exitPosition.direction == kInvalidDirection)
            {
                CoordsXYZ newLoc = { peep->NextLoc.ToTileCentre(), peep->NextLoc.z };
                if (peep->GetNextIsSloped())
                    newLoc.z += kCoordsZStep;
                newLoc.z++;
                peep->MoveTo(newLoc);
            }
            else
            {
                peep->MoveTo(exitPosition);
                peep->Orientation = exitPosition.direction;
            }

            peep->State = PeepState::falling;
            peep->SwitchToSpecialSprite(0);

            peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_STATS;
        }
    }
    numRiders = 0;
    slideInUse = 0;
    windowInvalidateFlags.set(RideInvalidateFlag::main);
}

void RideClearBlockedTiles(const Ride& ride)
{
    auto& gameState = getGameState();
    for (TileCoordsXY tilePos = {}; tilePos.x < gameState.mapSize.x; ++tilePos.x)
    {
        for (tilePos.y = 0; tilePos.y < gameState.mapSize.y; ++tilePos.y)
        {
            for (auto* trackElement : TileElementsView<TrackElement>(tilePos.ToCoordsXY()))
            {
                if (trackElement->GetRideIndex() != ride.id)
                    continue;

                // Unblock footpath element that is at same position
                auto* footpathElement = MapGetFootpathElement(TileCoordsXYZ{ tilePos, trackElement->BaseHeight }.ToCoordsXYZ());

                if (footpathElement == nullptr)
                    continue;

                footpathElement->SetIsBlockedByVehicle(false);
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
    const CoordsXYZD& location, TrackElemType type, uint16_t extra_params, TileElement** output_element,
    TrackElementSetFlags flags)
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
    if (sequence >= ted.numSequences)
        return std::nullopt;

    const auto& trackBlock = ted.sequences[sequence].clearance;

    CoordsXY offsets = { trackBlock.x, trackBlock.y };
    CoordsXY newCoords = location;
    newCoords += offsets.Rotate(DirectionReverse(mapDirection));

    auto retCoordsXYZ = CoordsXYZ{ newCoords.x, newCoords.y, location.z - trackBlock.z };

    int32_t start_z = retCoordsXYZ.z;
    assert(ted.numSequences > 0);
    const auto block0 = ted.sequences[0].clearance;

    retCoordsXYZ.z += block0.z;
    for (int32_t i = 0; i < ted.numSequences; i++)
    {
        const auto& block = ted.sequences[i].clearance;
        CoordsXY cur = { retCoordsXYZ };
        offsets = { block.x, block.y };
        cur += offsets.Rotate(mapDirection);
        int32_t cur_z = start_z + block.z;

        MapInvalidateTileFull(cur);

        trackElement = MapGetTrackElementAtOfTypeSeq({ cur, cur_z, static_cast<Direction>(location.direction) }, type, i);
        if (trackElement == nullptr)
        {
            return std::nullopt;
        }
        if (i == 0 && output_element != nullptr)
        {
            *output_element = reinterpret_cast<TileElement*>(trackElement);
        }
        if (flags.has(TrackElementSetFlag::highlightOff))
        {
            trackElement->SetHighlight(false);
        }
        if (flags.has(TrackElementSetFlag::highlightOn))
        {
            trackElement->SetHighlight(true);
        }
        if (flags.has(TrackElementSetFlag::colourScheme))
        {
            auto newScheme = static_cast<RideColourScheme>(extra_params & 0xFF);
            trackElement->SetColourScheme(newScheme);

            if (_previousTrackPiece == retCoordsXYZ)
            {
                _currentColourScheme = newScheme;
            }
        }
        if (flags.has(TrackElementSetFlag::seatRotation))
        {
            trackElement->SetSeatRotation(static_cast<uint8_t>(extra_params & 0xFF));
        }
        if (flags.has(TrackElementSetFlag::cableLiftOn))
        {
            trackElement->SetHasCableLift(true);
        }
        if (flags.has(TrackElementSetFlag::cableLiftOff))
        {
            trackElement->SetHasCableLift(false);
        }
        if (flags.has(TrackElementSetFlag::brakeClosed))
        {
            trackElement->SetBrakeClosed(extra_params != 0);
        }
        if (flags.has(TrackElementSetFlag::brakeBoosterSpeed))
        {
            trackElement->SetBrakeBoosterSpeed(static_cast<uint8_t>(extra_params & 0xFF));
        }
    }
    return retCoordsXYZ;
}

static void WindowRideConstructionUpdateActiveElements()
{
    auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_ACTIVE_ELEMENTS);
    ContextBroadcastIntent(&intent);
}

/**
 *
 *  rct2: 0x006C96C0
 */
void RideConstructionRemoveGhosts()
{
    if (_currentTrackSelectionFlags.has(TrackSelectionFlag::entranceOrExit))
    {
        RideEntranceExitRemoveGhost();
        _currentTrackSelectionFlags.unset(TrackSelectionFlag::entranceOrExit);
    }
    if (_currentTrackSelectionFlags.has(TrackSelectionFlag::track))
    {
        auto intent = Intent(INTENT_ACTION_REMOVE_PROVISIONAL_TRACK_PIECE);
        ContextBroadcastIntent(&intent);
        _currentTrackSelectionFlags.unset(TrackSelectionFlag::track);
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
                nullptr, { TrackElementSetFlag::highlightOff });
            break;
        case RideConstructionState::MazeBuild:
        case RideConstructionState::MazeMove:
        case RideConstructionState::MazeFill:
        case RideConstructionState::Front:
        case RideConstructionState::Back:
            if (_currentTrackSelectionFlags.has(TrackSelectionFlag::arrow))
            {
                MapInvalidateTileFull(_currentTrackBegin.ToTileStart());
            }
            RideConstructionRemoveGhosts();
            break;
        case RideConstructionState::Place:
        case RideConstructionState::EntranceExit:
        default:
            if (_currentTrackSelectionFlags.has(TrackSelectionFlag::arrow))
            {
                _currentTrackSelectionFlags.unset(TrackSelectionFlag::arrow);
                gMapSelectFlags.unset(MapSelectFlag::enableArrow);
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

    const auto& rtd = ride->getRideTypeDescriptor();

    if (rtd.HasFlag(RtdFlag::hasTrack) || ride->numStations == 0)
    {
        _currentlySelectedTrack = rtd.StartTrackPiece;
        _currentTrackPitchEnd = TrackPitch::none;
        _currentTrackRollEnd = TrackRoll::none;
        _currentTrackHasLiftHill = false;
        _currentTrackAlternative.clearAll();
        if (rtd.HasFlag(RtdFlag::startConstructionInverted))
        {
            _currentTrackAlternative.set(AlternativeTrackFlag::inverted);
        }
        _previousTrackPitchEnd = TrackPitch::none;
        _previousTrackRollEnd = TrackRoll::none;
    }
    else
    {
        _currentlySelectedTrack = TrackElemType::none;
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

    const auto& rtd = ride->getRideTypeDescriptor();

    int32_t z, direction;
    TrackElemType trackType;
    TrackBeginEnd trackBeginEnd;
    CoordsXYE xyElement;
    TileElement* tileElement;
    _currentTrackPrice = kMoney64Undefined;

    const TrackElementDescriptor* ted;
    switch (_rideConstructionState)
    {
        case RideConstructionState::Front:
        {
            direction = _currentTrackPieceDirection;
            if (!TrackBlockGetPreviousFromZero(_currentTrackBegin, *ride, direction, &trackBeginEnd))
            {
                ride_construction_reset_current_piece();
                return;
            }
            tileElement = trackBeginEnd.begin_element;
            trackType = tileElement->AsTrack()->GetTrackType();

            if (!ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
            {
                ride_construction_reset_current_piece();
                return;
            }

            // Set whether track is covered
            _currentTrackAlternative.unset(AlternativeTrackFlag::inverted);
            if (rtd.HasFlag(RtdFlag::hasInvertedVariant))
            {
                if (tileElement->AsTrack()->IsInverted())
                {
                    _currentTrackAlternative.set(AlternativeTrackFlag::inverted);
                }
            }

            ted = &GetTrackElementDescriptor(trackType);
            auto bank = ted->definition.rollEnd;
            auto slope = ted->definition.pitchEnd;

            // Set track curve
            _currentlySelectedTrack = ted->curveChain.next;

            // Set track banking
            if (rtd.HasFlag(RtdFlag::hasInvertedVariant))
            {
                if (bank == TrackRoll::upsideDown)
                {
                    bank = TrackRoll::none;
                    _currentTrackAlternative.flip(AlternativeTrackFlag::inverted);
                }
            }
            _currentTrackRollEnd = bank;
            _previousTrackRollEnd = bank;

            const auto& trackElement = tileElement->AsTrack();

            // Set track slope and lift hill
            _currentTrackPitchEnd = slope;
            _previousTrackPitchEnd = slope;
            _currentTrackHasLiftHill = trackElement->HasChain()
                && ((slope != TrackPitch::down25 && slope != TrackPitch::down60)
                    || getGameState().cheats.enableChainLiftOnAllTrack);

            if (TrackTypeHasSpeedSetting(trackElement->GetTrackType()))
                _currentBrakeSpeed = trackElement->GetBrakeBoosterSpeed();
            _currentColourScheme = static_cast<RideColourScheme>(trackElement->GetColourScheme());
            _currentSeatRotationAngle = trackElement->GetSeatRotation();

            _previousTrackPiece.x = trackBeginEnd.begin_x;
            _previousTrackPiece.y = trackBeginEnd.begin_y;
            _previousTrackPiece.z = trackElement->GetBaseZ();
            break;
        }
        case RideConstructionState::Back:
        {
            direction = DirectionReverse(_currentTrackPieceDirection);
            if (!TrackBlockGetNextFromZero(_currentTrackBegin, *ride, direction, &xyElement, &z, &direction, false))
            {
                ride_construction_reset_current_piece();
                return;
            }
            tileElement = xyElement.element;
            trackType = tileElement->AsTrack()->GetTrackType();

            // Set whether track is covered
            _currentTrackAlternative.unset(AlternativeTrackFlag::inverted);
            if (rtd.HasFlag(RtdFlag::hasInvertedVariant))
            {
                if (tileElement->AsTrack()->IsInverted())
                {
                    _currentTrackAlternative.set(AlternativeTrackFlag::inverted);
                }
            }

            ted = &GetTrackElementDescriptor(trackType);
            auto bank = ted->definition.rollStart;
            auto slope = ted->definition.pitchStart;

            // Set track curve
            _currentlySelectedTrack = ted->curveChain.previous;

            // Set track banking
            if (rtd.HasFlag(RtdFlag::hasInvertedVariant))
            {
                if (bank == TrackRoll::upsideDown)
                {
                    bank = TrackRoll::none;
                    _currentTrackAlternative.flip(AlternativeTrackFlag::inverted);
                }
            }
            _currentTrackRollEnd = bank;
            _previousTrackRollEnd = bank;

            const auto& trackElement = tileElement->AsTrack();

            // Set track slope and lift hill
            _currentTrackPitchEnd = slope;
            _previousTrackPitchEnd = slope;
            if (!getGameState().cheats.enableChainLiftOnAllTrack)
            {
                _currentTrackHasLiftHill = trackElement->HasChain();
            }

            if (TrackTypeHasSpeedSetting(trackElement->GetTrackType()))
                _currentBrakeSpeed = trackElement->GetBrakeBoosterSpeed();
            _currentColourScheme = static_cast<RideColourScheme>(trackElement->GetColourScheme());
            _currentSeatRotationAngle = trackElement->GetSeatRotation();

            _previousTrackPiece.x = xyElement.x;
            _previousTrackPiece.y = xyElement.y;
            _previousTrackPiece.z = trackElement->GetBaseZ();
            break;
        }
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
        auto type = _currentTrackPieceType;
        TileElement* tileElement;
        auto newCoords = GetTrackElementOriginAndApplyChanges(
            { _currentTrackBegin, static_cast<Direction>(direction & 3) }, type, 0, &tileElement, {});
        if (!newCoords.has_value())
        {
            _rideConstructionState = RideConstructionState::State0;
            WindowRideConstructionUpdateActiveElements();
            return;
        }

        CoordsXYE inputElement, outputElement;
        inputElement.x = newCoords->x;
        inputElement.y = newCoords->y;
        inputElement.element = tileElement;
        if (TrackBlockGetNext(&inputElement, &outputElement, &newCoords->z, &direction))
        {
            newCoords->x = outputElement.x;
            newCoords->y = outputElement.y;
            tileElement = outputElement.element;
            _currentTrackBegin = *newCoords;
            _currentTrackPieceDirection = tileElement->GetDirection();
            _currentTrackPieceType = tileElement->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags.clearAll();
            WindowRideConstructionUpdateActiveElements();
        }
        else
        {
            _rideConstructionState = RideConstructionState::Front;
            _currentTrackBegin = { outputElement, newCoords->z };
            _currentTrackPieceDirection = direction;
            _currentTrackPieceType = tileElement->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags.clearAll();
            RideConstructionSetDefaultNextPiece();
            WindowRideConstructionUpdateActiveElements();
        }
    }
    else if (_rideConstructionState == RideConstructionState::Back)
    {
        gMapSelectFlags.unset(MapSelectFlag::enableArrow);

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
        auto type = _currentTrackPieceType;
        TileElement* tileElement;
        auto newCoords = GetTrackElementOriginAndApplyChanges(
            { _currentTrackBegin, static_cast<Direction>(direction & 3) }, type, 0, &tileElement, {});
        if (newCoords == std::nullopt)
        {
            _rideConstructionState = RideConstructionState::State0;
            WindowRideConstructionUpdateActiveElements();
            return;
        }

        TrackBeginEnd trackBeginEnd;
        if (TrackBlockGetPrevious({ *newCoords, tileElement }, &trackBeginEnd))
        {
            _currentTrackBegin.x = trackBeginEnd.begin_x;
            _currentTrackBegin.y = trackBeginEnd.begin_y;
            _currentTrackBegin.z = trackBeginEnd.begin_z;
            _currentTrackPieceDirection = trackBeginEnd.begin_direction;
            _currentTrackPieceType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags.clearAll();
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
            _currentTrackSelectionFlags.clearAll();
            RideConstructionSetDefaultNextPiece();
            WindowRideConstructionUpdateActiveElements();
        }
    }
    else if (_rideConstructionState == RideConstructionState::Front)
    {
        gMapSelectFlags.unset(MapSelectFlag::enableArrow);

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
    auto* windowMgr = Ui::GetWindowManager();
    auto constructionWindow = windowMgr->FindByClass(WindowClass::rideConstruction);
    if (constructionWindow == nullptr)
    {
        if (!RideInitialiseConstructionWindow(*ride))
            return false;

        constructionWindow = windowMgr->FindByClass(WindowClass::rideConstruction);
        if (constructionWindow == nullptr)
            return false;
    }

    RideConstructionInvalidateCurrentTrack();
    if (_rideConstructionState != RideConstructionState::EntranceExit || !isToolActive(WindowClass::rideConstruction))
    {
        // Replace entrance / exit
        ToolSet(
            *constructionWindow,
            entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE ? WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE : WC_RIDE_CONSTRUCTION__WIDX_EXIT,
            Tool::crosshair);
        gRideEntranceExitPlaceType = entranceType;
        gRideEntranceExitPlaceRideIndex = rideIndex;
        gRideEntranceExitPlaceStationIndex = stationIndex;
        gInputFlags.set(InputFlag::allowRightMouseRemoval);
        if (_rideConstructionState != RideConstructionState::EntranceExit)
        {
            gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
            _rideConstructionState = RideConstructionState::EntranceExit;
        }

        WindowRideConstructionUpdateActiveElements();
        gMapSelectFlags.unset(MapSelectFlag::enableConstruct);
    }
    else
    {
        // Remove entrance / exit
        auto rideEntranceExitRemove = GameActions::RideEntranceExitRemoveAction(
            { tileElement.x, tileElement.y }, rideIndex, stationIndex, entranceType == ENTRANCE_TYPE_RIDE_EXIT);

        rideEntranceExitRemove.SetCallback([=](const GameActions::GameAction* ga, const GameActions::Result* result) {
            gRideEntranceExitPlaceType = entranceType;
            windowMgr->InvalidateByClass(WindowClass::rideConstruction);

            auto newToolWidgetIndex = (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE) ? WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE
                                                                                    : WC_RIDE_CONSTRUCTION__WIDX_EXIT;

            ToolCancel();
            ToolSet(*constructionWindow, newToolWidgetIndex, Tool::crosshair);
        });

        GameActions::Execute(&rideEntranceExitRemove, getGameState());
    }

    windowMgr->InvalidateByClass(WindowClass::rideConstruction);
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
            _currentTrackSelectionFlags.clearAll();
            _rideConstructionNextArrowPulse = 0;
            gMapSelectFlags.unset(MapSelectFlag::enableArrow);

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

    auto rideEntry = ride->getRideEntry();
    if (rideEntry == nullptr || !ride_check_if_construction_allowed(*ride))
        return false;

    if (ride->lifecycleFlags & RIDE_LIFECYCLE_INDESTRUCTIBLE && !getGameState().cheats.makeAllDestructible)
    {
        Formatter ft;
        ride->formatNameTo(ft);
        ContextShowError(
            STR_CANT_START_CONSTRUCTION_ON, STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE, ft);
        return false;
    }

    // Stop the ride again to clear all vehicles and peeps (compatible with network games)
    if (ride->status != RideStatus::simulating)
    {
        auto gameAction = GameActions::RideSetStatusAction(ride->id, RideStatus::closed);
        GameActions::Execute(&gameAction, getGameState());
    }

    // Check if element is a station entrance or exit
    if (tileElement.element->GetType() == TileElementType::Entrance)
        return ride_modify_entrance_or_exit(tileElement);

    if (tileElement.element->GetType() != TileElementType::Track)
        return false;

    if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::cannotHaveGaps))
    {
        CoordsXYE endOfTrackElement{};
        if (ride->findTrackGap(tileElement, &endOfTrackElement))
            tileElement = endOfTrackElement;
    }

    const auto tileCoords = CoordsXYZ{ tileElement, tileElement.element->GetBaseZ() };
    const auto direction = tileElement.element->GetDirection();
    const auto type = tileElement.element->AsTrack()->GetTrackType();

    ride_create_or_find_construction_window(rideIndex);

    const auto& rtd = ride->getRideTypeDescriptor();
    if (rtd.specialType == RtdSpecialType::maze)
    {
        return ride_modify_maze(tileElement);
    }

    auto newCoords = GetTrackElementOriginAndApplyChanges({ tileCoords, direction }, type, 0, nullptr, {});
    if (!newCoords.has_value())
        return false;

    _currentRideIndex = rideIndex;
    _rideConstructionState = RideConstructionState::Selected;
    _currentTrackBegin = newCoords.value();
    _currentTrackPieceDirection = direction;
    _currentTrackPieceType = type;
    _currentTrackSelectionFlags.clearAll();
    _rideConstructionNextArrowPulse = 0;
    gMapSelectFlags.unset(MapSelectFlag::enableArrow);

    if (!ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
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
    _currentTrackSelectionFlags.clearAll();

    RideSelectPreviousSection();

    if (_rideConstructionState != RideConstructionState::Back)
    {
        _rideConstructionState = RideConstructionState::Selected;
        _currentTrackBegin = *newCoords;
        _currentTrackPieceDirection = direction;
        _currentTrackPieceType = type;
        _currentTrackSelectionFlags.clearAll();
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
    WindowBase* w;

    ToolCancel();

    if (!ride_check_if_construction_allowed(ride))
        return 0;

    RideClearForConstruction(ride);
    ride.removePeeps();

    w = ride_create_or_find_construction_window(ride.id);

    ToolSet(*w, WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT, Tool::crosshair);
    gInputFlags.set(InputFlag::allowRightMouseRemoval);

    _currentlySelectedTrack = ride.getRideTypeDescriptor().StartTrackPiece;
    _currentTrackPitchEnd = TrackPitch::none;
    _currentTrackRollEnd = TrackRoll::none;
    _currentTrackHasLiftHill = false;
    _currentTrackAlternative.clearAll();

    if (ride.getRideTypeDescriptor().HasFlag(RtdFlag::startConstructionInverted))
        _currentTrackAlternative.set(AlternativeTrackFlag::inverted);

    _previousTrackRollEnd = TrackRoll::none;
    _previousTrackPitchEnd = TrackPitch::none;

    _currentTrackPieceDirection = 0;
    _rideConstructionState = RideConstructionState::Place;
    _currentTrackSelectionFlags.clearAll();

    WindowRideConstructionUpdateActiveElements();
    return 1;
}

/**
 *
 *  rct2: 0x006CB7FB
 */
money64 RideGetRefundPrice(const Ride& ride)
{
    CoordsXYE trackElement;
    money64 cost = 0;

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
        auto trackRemoveAction = GameActions::TrackRemoveAction(
            trackElement.element->AsTrack()->GetTrackType(), trackElement.element->AsTrack()->GetSequenceIndex(),
            { trackElement.x, trackElement.y, trackElement.element->GetBaseZ(), direction });
        trackRemoveAction.SetFlags(CommandFlag::allowDuringPaused);

        auto res = GameActions::Query(&trackRemoveAction, getGameState());

        cost += res.cost;

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

money64 SetOperatingSetting(RideId rideId, GameActions::RideSetSetting setting, uint8_t value)
{
    auto rideSetSetting = GameActions::RideSetSettingAction(rideId, setting, value);
    auto res = GameActions::Execute(&rideSetSetting, getGameState());
    return res.error == GameActions::Status::ok ? 0 : kMoney64Undefined;
}

money64 SetOperatingSettingNested(RideId rideId, GameActions::RideSetSetting setting, uint8_t value, CommandFlags flags)
{
    auto rideSetSetting = GameActions::RideSetSettingAction(rideId, setting, value);
    rideSetSetting.SetFlags(flags);

    auto& gameState = getGameState();
    auto res = flags.has(CommandFlag::apply) ? GameActions::ExecuteNested(&rideSetSetting, gameState)
                                             : GameActions::QueryNested(&rideSetSetting, gameState);
    return res.error == GameActions::Status::ok ? 0 : kMoney64Undefined;
}

/**
 *
 *  rct2: 0x006CB945
 */
void Ride::validateStations()
{
    const auto& rtd = getRideTypeDescriptor();
    if (rtd.specialType != RtdSpecialType::maze)
    {
        // find the stations of the ride to begin stepping over track elements from
        for (const auto& station : stations)
        {
            if (station.Start.IsNull())
                continue;

            CoordsXYZ location = station.GetStart();
            uint8_t direction = kInvalidDirection;

            bool specialTrack = false;
            TileElement* tileElement = nullptr;
            while (true)
            {
                // search backwards for the previous station TrackElement (only if the first station TrackElement is found)
                if (direction != kInvalidDirection)
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

                    const auto& ted = GetTrackElementDescriptor(tileElement->AsTrack()->GetTrackType());
                    // keep searching for a station piece (coaster station, tower ride base, shops, and flat ride base)
                    if (!ted.sequences[0].flags.has(SequenceFlag::trackOrigin))
                        continue;

                    trackFound = true;
                    break;
                } while (!(tileElement++)->IsLastForTile());

                if (!trackFound)
                {
                    break;
                }
                // update the StationIndex, get the TrackElement's rotation
                tileElement->AsTrack()->SetStationIndex(getStationIndex(&station));
                direction = tileElement->GetDirection();

                // In the future this could look at the TED and see if the station has a sequence longer than 1
                // tower ride, flat ride, shop
                if (getRideTypeDescriptor().HasFlag(RtdFlag::hasSinglePieceStation))
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
            const auto& ted = GetTrackElementDescriptor(tileElement->AsTrack()->GetTrackType());
            for (uint8_t i = 0; i < ted.numSequences; i++)
            {
                const auto& block = ted.sequences[i].clearance;
                CoordsXYZ blockLocation = location + CoordsXYZ{ CoordsXY{ block.x, block.y }.Rotate(direction), 0 };

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

                    const auto& ted2 = GetTrackElementDescriptor(tileElement->AsTrack()->GetTrackType());
                    if (!ted2.sequences[0].flags.has(SequenceFlag::trackOrigin))
                        continue;

                    trackFound = true;
                    break;
                } while (!(tileElement++)->IsLastForTile());

                if (!trackFound)
                {
                    // Critical error! Stop trying to find the next sequence to set StationIndex.
                    break;
                }

                tileElement->AsTrack()->SetStationIndex(getStationIndex(&station));
            }
        }
    }
    // determine what entrances and exits exist
    sfl::static_vector<TileCoordsXYZD, kMaxStationLocations> locations;
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
                if (trackType != TrackElemType::maze)
                {
                    uint8_t trackSequence = trackElement->AsTrack()->GetSequenceIndex();

                    // determine where the ride entrance is relative to the station track
                    Direction direction = (tileElement->GetDirection() - DirectionReverse(trackElement->GetDirection())) & 3;

                    // if the ride entrance is not on a valid side, remove it
                    const auto& ted = GetTrackElementDescriptor(trackType);
                    auto connectionSides = ted.sequences[trackSequence].getEntranceConnectionSides();
                    if (!(connectionSides & (1 << direction)))
                    {
                        continue;
                    }

                    stationId = trackElement->AsTrack()->GetStationIndex();
                }

                auto& station = getStation(stationId);
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
            _currentTrackSelectionFlags.clearAll();
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
            _currentTrackSelectionFlags.clearAll();
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
    if (ride.getRideTypeDescriptor().HasFlag(RtdFlag::isShopOrFacility))
        return { true };

    for (auto& station : ride.getStations())
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

TrackDrawerDescriptor getCurrentTrackDrawerDescriptor(const RideTypeDescriptor& rtd)
{
    const bool isInverted = _currentTrackAlternative.has(AlternativeTrackFlag::inverted);
    return getTrackDrawerDescriptor(rtd, isInverted);
}

TrackDrawerEntry getCurrentTrackDrawerEntry(const RideTypeDescriptor& rtd)
{
    const bool isInverted = _currentTrackAlternative.has(AlternativeTrackFlag::inverted);
    const bool isCovered = _currentTrackAlternative.has(AlternativeTrackFlag::alternativePieces);
    return getTrackDrawerEntry(rtd, isInverted, isCovered);
}

TrackElemType GetTrackTypeFromCurve(
    TrackCurve curve, bool startsDiagonal, TrackPitch startSlope, TrackPitch endSlope, TrackRoll startBank, TrackRoll endBank)
{
    for (uint32_t i = 0; i < std::size(gTrackDescriptors); i++)
    {
        const TrackDescriptor* trackDescriptor = &gTrackDescriptors[i];

        if (trackDescriptor->trackCurve != curve)
            continue;
        if (trackDescriptor->startsDiagonally != startsDiagonal)
            continue;
        if (trackDescriptor->slopeStart != startSlope)
            continue;
        if (trackDescriptor->slopeEnd != endSlope)
            continue;
        if (trackDescriptor->rollStart != startBank)
            continue;
        if (trackDescriptor->rollEnd != endBank)
            continue;

        return trackDescriptor->trackElement;
    }

    return TrackElemType::none;
}
