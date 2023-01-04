/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../Input.h"
#include "../actions/MazeSetTrackAction.h"
#include "../actions/TrackPlaceAction.h"
#include "../audio/audio.h"
#include "../entity/Staff.h"
#include "../interface/Viewport.h"
#include "../network/network.h"
#include "../paint/VirtualFloor.h"
#include "../ride/RideConstruction.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../util/Math.hpp"
#include "../world/Banner.h"
#include "../world/Scenery.h"
#include "Intent.h"

#include <iterator>
#include <tuple>

using namespace OpenRCT2::TrackMetaData;
bool gDisableErrorWindowSound = false;

RideConstructionState _rideConstructionState2;

/**
 *
 *  rct2: 0x006CA162
 */
money32 place_provisional_track_piece(
    RideId rideIndex, int32_t trackType, int32_t trackDirection, int32_t liftHillAndAlternativeState, const CoordsXYZ& trackPos)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return MONEY32_UNDEFINED;

    ride_construction_remove_ghosts();
    const auto& rtd = ride->GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
    {
        int32_t flags = GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST;
        auto gameAction = MazeSetTrackAction(CoordsXYZD{ trackPos, 0 }, true, rideIndex, GC_SET_MAZE_TRACK_BUILD);
        gameAction.SetFlags(flags);
        auto result = GameActions::Execute(&gameAction);

        if (result.Error != GameActions::Status::Ok)
            return MONEY32_UNDEFINED;

        _unkF440C5 = { trackPos, static_cast<Direction>(trackDirection) };
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK;
        viewport_set_visibility(3);
        if (_currentTrackSlopeEnd != 0)
            viewport_set_visibility(2);

        // Invalidate previous track piece (we may not be changing height!)
        VirtualFloorInvalidate();

        if (!scenery_tool_is_active())
        {
            // Set new virtual floor height.
            VirtualFloorSetHeight(trackPos.z);
        }

        return result.Cost;
    }

    auto trackPlaceAction = TrackPlaceAction(
        rideIndex, trackType, ride->type, { trackPos, static_cast<uint8_t>(trackDirection) }, 0, 0, 0,
        liftHillAndAlternativeState, false);
    trackPlaceAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
    // This command must not be sent over the network
    auto res = GameActions::Execute(&trackPlaceAction);
    if (res.Error != GameActions::Status::Ok)
        return MONEY32_UNDEFINED;

    int16_t z_begin, z_end;
    const auto& ted = GetTrackElementDescriptor(trackType);
    const rct_track_coordinates& coords = ted.Coordinates;
    if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK))
    {
        z_begin = coords.z_begin;
        z_end = coords.z_end;
    }
    else
    {
        z_end = z_begin = coords.z_begin;
    }

    _unkF440C5 = { trackPos.x, trackPos.y, trackPos.z + z_begin, static_cast<Direction>(trackDirection) };
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK;

    const auto resultData = res.GetData<TrackPlaceActionResult>();
    viewport_set_visibility((resultData.GroundFlags & ELEMENT_IS_UNDERGROUND) ? 1 : 3);
    if (_currentTrackSlopeEnd != 0)
        viewport_set_visibility(2);

    // Invalidate previous track piece (we may not be changing height!)
    VirtualFloorInvalidate();

    if (!scenery_tool_is_active())
    {
        // Set height to where the next track piece would begin
        VirtualFloorSetHeight(trackPos.z - z_begin + z_end);
    }

    return res.Cost;
}

static std::tuple<bool, track_type_t> window_ride_construction_update_state_get_track_element()
{
    auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_PIECES);
    ContextBroadcastIntent(&intent);

    uint8_t startSlope = _previousTrackSlopeEnd;
    uint8_t endSlope = _currentTrackSlopeEnd;
    uint8_t startBank = _previousTrackBankEnd;
    uint8_t endBank = _currentTrackBankEnd;

    if (_rideConstructionState == RideConstructionState::Back)
    {
        startSlope = _currentTrackSlopeEnd;
        endSlope = _previousTrackSlopeEnd;
        startBank = _currentTrackBankEnd;
        endBank = _previousTrackBankEnd;
    }

    auto curve = _currentTrackCurve;
    if (curve == TrackElemType::None)
    {
        return std::make_tuple(false, 0);
    }

    bool startsDiagonal = (_currentTrackPieceDirection & (1 << 2)) != 0;
    if (curve == TRACK_CURVE_LEFT_LARGE || curve == TRACK_CURVE_RIGHT_LARGE)
    {
        if (_rideConstructionState == RideConstructionState::Back)
        {
            startsDiagonal = !startsDiagonal;
        }
    }

    if (curve <= 8)
    {
        for (uint32_t i = 0; i < std::size(gTrackDescriptors); i++)
        {
            const track_descriptor* trackDescriptor = &gTrackDescriptors[i];

            if (trackDescriptor->track_curve != curve)
                continue;
            if (trackDescriptor->starts_diagonal != startsDiagonal)
                continue;
            if (trackDescriptor->slope_start != startSlope)
                continue;
            if (trackDescriptor->slope_end != endSlope)
                continue;
            if (trackDescriptor->bank_start != startBank)
                continue;
            if (trackDescriptor->bank_end != endBank)
                continue;

            return std::make_tuple(true, trackDescriptor->track_element);
        }

        return std::make_tuple(false, 0);
    }

    switch (curve & 0xFFFF)
    {
        case TrackElemType::EndStation:
        case TrackElemType::SBendLeft:
        case TrackElemType::SBendRight:
            if (startSlope != TRACK_SLOPE_NONE || endSlope != TRACK_SLOPE_NONE)
            {
                return std::make_tuple(false, 0);
            }

            if (startBank != TRACK_BANK_NONE || endBank != TRACK_BANK_NONE)
            {
                return std::make_tuple(false, 0);
            }

            return std::make_tuple(true, static_cast<track_type_t>(curve & 0xFFFF));

        case TrackElemType::LeftVerticalLoop:
        case TrackElemType::RightVerticalLoop:
            if (startBank != TRACK_BANK_NONE || endBank != TRACK_BANK_NONE)
            {
                return std::make_tuple(false, 0);
            }

            if (_rideConstructionState == RideConstructionState::Back)
            {
                if (endSlope != TRACK_SLOPE_DOWN_25)
                {
                    return std::make_tuple(false, 0);
                }
            }
            else
            {
                if (startSlope != TRACK_SLOPE_UP_25)
                {
                    return std::make_tuple(false, 0);
                }
            }

            return std::make_tuple(true, static_cast<track_type_t>(curve & 0xFFFF));

        default:
            return std::make_tuple(true, static_cast<track_type_t>(curve & 0xFFFF));
    }
}

/**
 * rct2: 0x006CA2DF
 *
 * @param[out] _trackType (dh)
 * @param[out] _trackDirection (bh)
 * @param[out] _rideIndex (dl)
 * @param[out] _liftHillAndInvertedState (liftHillAndInvertedState)
 * @param[out] _x (ax)
 * @param[out] _y (cx)
 * @param[out] _z (di)
 * @param[out] _properties (edirs16)
 * @return (CF)
 */
bool window_ride_construction_update_state(
    int32_t* _trackType, int32_t* _trackDirection, RideId* _rideIndex, int32_t* _liftHillAndInvertedState, CoordsXYZ* _trackPos,
    int32_t* _properties)
{
    RideId rideIndex;
    uint8_t trackDirection;
    uint16_t x, y, liftHillAndInvertedState, properties;

    auto updated_element = window_ride_construction_update_state_get_track_element();
    if (!std::get<0>(updated_element))
    {
        return true;
    }

    track_type_t trackType = std::get<1>(updated_element);
    liftHillAndInvertedState = 0;
    rideIndex = _currentRideIndex;
    if (_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED)
    {
        liftHillAndInvertedState |= CONSTRUCTION_LIFT_HILL_SELECTED;
    }

    if (_currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE)
    {
        liftHillAndInvertedState |= CONSTRUCTION_INVERTED_TRACK_SELECTED;
    }

    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return true;

    if (IsTrackEnabled(TRACK_SLOPE_STEEP_LONG))
    {
        switch (trackType)
        {
            case TrackElemType::FlatToUp60:
                trackType = TrackElemType::FlatToUp60LongBase;
                break;

            case TrackElemType::Up60ToFlat:
                trackType = TrackElemType::Up60ToFlatLongBase;
                break;

            case TrackElemType::FlatToDown60:
                trackType = TrackElemType::FlatToDown60LongBase;
                break;

            case TrackElemType::Down60ToFlat:
                trackType = TrackElemType::Down60ToFlatLongBase;
                break;

            case TrackElemType::DiagFlatToUp60:
            case TrackElemType::DiagUp60ToFlat:
            case TrackElemType::DiagFlatToDown60:
            case TrackElemType::DiagDown60ToFlat:
                return true;
        }
    }

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES)
        && _currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_PIECES)
    {
        auto availablePieces = rtd.CoveredTrackPieces;
        const auto& ted = GetTrackElementDescriptor(trackType);
        auto alternativeType = ted.AlternativeType;
        // this method limits the track element types that can be used
        if (alternativeType != TrackElemType::None && (availablePieces.get(trackType)))
        {
            trackType = alternativeType;
            if (!gCheatsEnableChainLiftOnAllTrack)
                liftHillAndInvertedState &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
        }
    }

    const auto& ted = GetTrackElementDescriptor(trackType);
    const rct_track_coordinates& trackCoordinates = ted.Coordinates;

    x = _currentTrackBegin.x;
    y = _currentTrackBegin.y;
    auto z = _currentTrackBegin.z;
    if (_rideConstructionState == RideConstructionState::Back)
    {
        z -= trackCoordinates.z_end;
        trackDirection = _currentTrackPieceDirection ^ 0x02;
        trackDirection -= trackCoordinates.rotation_end;
        trackDirection += trackCoordinates.rotation_begin;
        trackDirection &= 0x03;

        if (trackCoordinates.rotation_begin & (1 << 2))
        {
            trackDirection |= 0x04;
        }

        CoordsXY offsets = { trackCoordinates.x, trackCoordinates.y };
        CoordsXY coords = { x, y };
        coords += offsets.Rotate(DirectionReverse(trackDirection));
        x = static_cast<uint16_t>(coords.x);
        y = static_cast<uint16_t>(coords.y);
    }
    else
    {
        z -= trackCoordinates.z_begin;
        trackDirection = _currentTrackPieceDirection;
    }

    bool turnOffLiftHill = false;
    if (!IsTrackEnabled(TRACK_LIFT_HILL_CURVE))
    {
        if (ted.Flags & TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT)
        {
            turnOffLiftHill = true;
        }
    }

    if (!(ted.Flags & TRACK_ELEM_FLAG_ALLOW_LIFT_HILL))
    {
        turnOffLiftHill = true;
    }

    if (turnOffLiftHill && !gCheatsEnableChainLiftOnAllTrack)
    {
        liftHillAndInvertedState &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
        _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;

        if (trackType == TrackElemType::LeftCurvedLiftHill || trackType == TrackElemType::RightCurvedLiftHill)
        {
            liftHillAndInvertedState |= CONSTRUCTION_LIFT_HILL_SELECTED;
        }
    }

    if (TrackTypeHasSpeedSetting(trackType))
    {
        properties = _currentBrakeSpeed2;
    }
    else
    {
        properties = _currentSeatRotationAngle << 12;
    }

    if (_trackType != nullptr)
        *_trackType = trackType;
    if (_trackDirection != nullptr)
        *_trackDirection = trackDirection;
    if (_rideIndex != nullptr)
        *_rideIndex = rideIndex;
    if (_liftHillAndInvertedState != nullptr)
        *_liftHillAndInvertedState = liftHillAndInvertedState;
    if (_trackPos != nullptr)
        *_trackPos = { x, y, z };
    if (_properties != nullptr)
        *_properties = properties;

    return false;
}

/**
 *
 *  rct2: 0x006C84CE
 */
void window_ride_construction_update_active_elements()
{
    auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_ACTIVE_ELEMENTS);
    ContextBroadcastIntent(&intent);
}

/**
 *
 *  rct2: 0x0066DB3D
 */
bool scenery_tool_is_active()
{
    auto toolWindowClassification = gCurrentToolWidget.window_classification;
    WidgetIndex toolWidgetIndex = gCurrentToolWidget.widget_index;
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        if (toolWindowClassification == WindowClass::TopToolbar && toolWidgetIndex == WC_TOP_TOOLBAR__WIDX_SCENERY)
            return true;

    return false;
}

void SceneryInit()
{
    auto intent = Intent(INTENT_ACTION_INIT_SCENERY);
    ContextBroadcastIntent(&intent);
}

void ScenerySetDefaultPlacementConfiguration()
{
    auto intent = Intent(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG);
    ContextBroadcastIntent(&intent);
}
