/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestTrack.hpp"

#include "../../src/openrct2/ride/RideData.h"
#include "Data.h"
#include "FunctionCall.hpp"
#include "GeneralSupportHeightCall.hpp"
#include "PaintIntercept.hpp"
#include "Printer.hpp"
#include "SegmentSupportHeightCall.hpp"
#include "SideTunnelCall.hpp"
#include "String.hpp"
#include "TestPaint.hpp"
#include "Utils.hpp"
#include "VerticalTunnelCall.hpp"

#include <openrct2/paint/Supports.h>
#include <openrct2/paint/tile_element/Paint.TileElement.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <string>
#include <vector>

struct ITestTrackFilter
{
public:
    virtual ~ITestTrackFilter()
    {
    }

    virtual bool AppliesTo(uint8_t rideType, uint8_t trackType) abstract;

    virtual int Variations(uint8_t rideType, uint8_t trackType) abstract;

    virtual std::string VariantName(uint8_t rideType, uint8_t trackType, int variant) abstract;

    virtual void ApplyTo(
        uint8_t rideType, uint8_t trackType, int variant, TileElement* tileElement, TileElement* surfaceElement, Ride* ride,
        rct_ride_entry* rideEntry) abstract;
};

class CableLiftFilter : public ITestTrackFilter
{
public:
    bool AppliesTo(uint8_t rideType, uint8_t trackType) override
    {
        return rideType == RIDE_TYPE_GIGA_COASTER;
    }

    int Variations(uint8_t rideType, uint8_t trackType) override
    {
        return 2;
    }

    std::string VariantName(uint8_t rideType, uint8_t trackType, int variant) override
    {
        return String::Format("cableLift:%d", variant);
    }

    virtual void ApplyTo(
        uint8_t rideType, uint8_t trackType, int variant, TileElement* tileElement, TileElement* surfaceElement, Ride* ride,
        rct_ride_entry* rideEntry) override
    {
        if (variant == 0)
        {
            tileElement->AsTrack()->SetHasCableLift(false);
        }
        else
        {
            tileElement->AsTrack()->SetHasCableLift(true);
        }
    }
};

class ChainLiftFilter : public ITestTrackFilter
{
public:
    bool AppliesTo(uint8_t rideType, uint8_t trackType) override
    {
        return !GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE);
    }

    int Variations(uint8_t rideType, uint8_t trackType) override
    {
        return 2;
    }

    std::string VariantName(uint8_t rideType, uint8_t trackType, int variant) override
    {
        return String::Format("chainLift:%d", variant);
    }

    virtual void ApplyTo(
        uint8_t rideType, uint8_t trackType, int variant, TileElement* tileElement, TileElement* surfaceElement, Ride* ride,
        rct_ride_entry* rideEntry) override
    {
        tileElement->AsTrack()->SetHasChain(variant != 0);
    }
};

class InvertedFilter : public ITestTrackFilter
{
public:
    bool AppliesTo(uint8_t rideType, uint8_t trackType) override
    {
        if (rideType == RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER || rideType == RIDE_TYPE_FLYING_ROLLER_COASTER
            || rideType == RIDE_TYPE_LAY_DOWN_ROLLER_COASTER)
        {
            return true;
        }

        return false;
    }

    int Variations(uint8_t rideType, uint8_t trackType) override
    {
        return 2;
    }

    std::string VariantName(uint8_t rideType, uint8_t trackType, int variant) override
    {
        return String::Format("inverted:%d", variant);
    }

    virtual void ApplyTo(
        uint8_t rideType, uint8_t trackType, int variant, TileElement* tileElement, TileElement* surfaceElement, Ride* ride,
        rct_ride_entry* rideEntry) override
    {
        if (variant == 0)
        {
            tileElement->AsTrack()->SetHasCableLift(false);
        }
        else
        {
            tileElement->AsTrack()->SetHasCableLift(true);
        }
    }
};

class EntranceStyleFilter : public ITestTrackFilter
{
public:
    bool AppliesTo(uint8_t rideType, uint8_t trackType) override
    {
        if (track_type_is_station(trackType))
        {
            return true;
        }

        return false;
    }

    int Variations(uint8_t rideType, uint8_t trackType) override
    {
        return 12;
    }

    std::string VariantName(uint8_t rideType, uint8_t trackType, int variant) override
    {
        return String::Format("entranceStyle:%d", variant);
    }

    virtual void ApplyTo(
        uint8_t rideType, uint8_t trackType, int variant, TileElement* tileElement, TileElement* surfaceElement, Ride* ride,
        rct_ride_entry* rideEntry) override
    {
        ride->entrance_style = variant;
        RCT2_Rides[0].entrance_style = variant;
    }
};

static void CallOriginal(
    uint8_t rideType, uint8_t trackType, uint8_t direction, uint8_t trackSequence, uint16_t height, TileElement* tileElement)
{
    uint32_t* trackDirectionList = (uint32_t*)RideTypeTrackPaintFunctionsOld[rideType][trackType];
    const uint8_t rideIndex = 0;

    // Have to call from this point as it pushes esi and expects callee to pop it
    RCT2_CALLPROC_X(
        0x006C4934, rideType, (int)trackDirectionList, direction, height, (int)tileElement, rideIndex * sizeof(Ride),
        trackSequence);
}

static void CallNew(
    uint8_t rideType, uint8_t trackType, uint8_t direction, uint8_t trackSequence, uint16_t height, TileElement* tileElement)
{
    TRACK_PAINT_FUNCTION_GETTER newPaintFunctionGetter = GetRideTypeDescriptor(rideType).TrackPaintFunction;
    TRACK_PAINT_FUNCTION newPaintFunction = newPaintFunctionGetter(trackType);

    newPaintFunction(&gPaintSession, 0, trackSequence, direction, height, tileElement);
}

using TestFunction = uint8_t (*)(uint8_t, uint8_t, uint8_t, std::string*);

static uint8_t TestTrackElementPaintCalls(uint8_t rideType, uint8_t trackType, uint8_t trackSequence, std::string* error);

static uint8_t TestTrackElementSegmentSupportHeight(
    uint8_t rideType, uint8_t trackType, uint8_t trackSequence, std::string* error);

static uint8_t TestTrackElementGeneralSupportHeight(
    uint8_t rideType, uint8_t trackType, uint8_t trackSequence, std::string* error);

static uint8_t TestTrackElementSideTunnels(uint8_t rideType, uint8_t trackType, uint8_t trackSequence, std::string* error);

static uint8_t TestTrackElementVerticalTunnels(uint8_t rideType, uint8_t trackType, uint8_t trackSequence, std::string* error);

uint8_t TestTrack::TestPaintTrackElement(uint8_t rideType, uint8_t trackType, std::string* out)
{
    if (!Utils::rideSupportsTrackType(rideType, trackType))
    {
        return TEST_FAILED;
    }

    if (rideType == RIDE_TYPE_CHAIRLIFT)
    {
        if (track_type_is_station(trackType))
        {
            // These rides check neighbouring tiles for tracks
            return TEST_SKIPPED;
        }
    }

    int sequenceCount = Utils::getTrackSequenceCount(rideType, trackType);
    std::string error = String::Format("rct2: 0x%08X\n", RideTypeTrackPaintFunctionsOld[rideType][trackType]);

    uint8_t retVal = TEST_SUCCESS;

    static TestFunction functions[] = {
        TestTrackElementPaintCalls,  TestTrackElementSegmentSupportHeight, TestTrackElementGeneralSupportHeight,
        TestTrackElementSideTunnels, TestTrackElementVerticalTunnels,
    };

    for (int trackSequence = 0; trackSequence < sequenceCount; trackSequence++)
    {
        for (auto&& function : functions)
        {
            retVal = function(rideType, trackType, trackSequence, &error);

            if (retVal != TEST_SUCCESS)
            {
                *out += error + "\n";
                return retVal;
            }
        }
    }

    return retVal;
}

static uint8_t TestTrackElementPaintCalls(uint8_t rideType, uint8_t trackType, uint8_t trackSequence, std::string* error)
{
    uint16_t height = 3 * 16;

    TileElement tileElement = {};
    tileElement.SetType(TILE_ELEMENT_TYPE_TRACK);
    tileElement.SetLastForTile(true);
    tileElement.AsTrack()->SetTrackType(trackType);
    tileElement.base_height = height / 16;
    g_currently_drawn_item = &tileElement;

    TileElement surfaceElement = {};
    surfaceElement.SetType(TILE_ELEMENT_TYPE_SURFACE);
    surfaceElement.base_height = MINIMUM_LAND_HEIGHT;
    gSurfaceElement = &surfaceElement;
    gDidPassSurface = true;

    gPaintSession.CurrentlyDrawnTileElement = &tileElement;
    gPaintSession.SurfaceElement = &surfaceElement;
    gPaintSession.DidPassSurface = true;

    TestPaint::ResetEnvironment();
    TestPaint::ResetTunnels();

    function_call callBuffer[256] = {};
    int callCount = 0;

    // TODO: test supports
    // TODO: test flat rides
    // TODO: test mazes
    // TODO: test underground (Wooden RC)
    // TODO: test station fences

    std::vector<ITestTrackFilter*> filters;
    filters.push_back(new CableLiftFilter());
    filters.push_back(new ChainLiftFilter());
    filters.push_back(new InvertedFilter());
    filters.push_back(new EntranceStyleFilter());

    std::vector<ITestTrackFilter*> activeFilters;

    for (auto&& filter : filters)
    {
        if (filter->AppliesTo(rideType, trackType))
        {
            activeFilters.push_back(filter);
        }
    }

    // Add an element so there's always something to add to
    std::vector<uint8_t> filler;
    filler.push_back(0);

    std::vector<std::vector<uint8_t>> argumentPermutations;
    argumentPermutations.push_back(filler);
    for (size_t filterIndex = 0; filterIndex < activeFilters.size(); ++filterIndex)
    {
        ITestTrackFilter* filter = activeFilters[filterIndex];
        uint8_t variantCount = filter->Variations(rideType, trackType);

        std::vector<std::vector<uint8_t>> newArgumentPermutations;
        for (int variant = 0; variant < variantCount; variant++)
        {
            for (auto&& oldPermutation : argumentPermutations)
            {
                std::vector<uint8_t> permutation;
                permutation.insert(permutation.begin(), oldPermutation.begin(), oldPermutation.end());
                permutation.push_back(variant);
                newArgumentPermutations.push_back(permutation);
            }
        }

        argumentPermutations.clear();
        argumentPermutations.insert(
            argumentPermutations.begin(), newArgumentPermutations.begin(), newArgumentPermutations.end());
    }

    for (auto&& arguments : argumentPermutations)
    {
        std::string baseCaseName = "[";

        for (size_t filterIndex = 0; filterIndex < activeFilters.size(); ++filterIndex)
        {
            uint8_t& variant = arguments[1 + filterIndex];
            baseCaseName += activeFilters[filterIndex]->VariantName(rideType, trackType, variant);
            baseCaseName += " ";

            activeFilters[filterIndex]->ApplyTo(
                rideType, trackType, variant, &tileElement, &surfaceElement, &(gRideList[0]), gRideEntries[0]);
        }

        for (int currentRotation = 0; currentRotation < 4; currentRotation++)
        {
            gCurrentRotation = currentRotation;
            RCT2_CurrentRotation = currentRotation;
            gPaintSession.CurrentRotation = currentRotation;
            for (int direction = 0; direction < 4; direction++)
            {
                RCT2_GLOBAL(0x009DE56A, int16_t) = 64; // x
                RCT2_GLOBAL(0x009DE56E, int16_t) = 64; // y

                std::string caseName = String::Format(
                    "%srotation:%d direction:%d trackSequence:%d]", baseCaseName.c_str(), currentRotation, direction,
                    trackSequence);

                PaintIntercept::ClearCalls();
                TestPaint::ResetSupportHeights();
                gWoodenSupportsPrependTo = nullptr;

                CallOriginal(rideType, trackType, direction, trackSequence, height, &tileElement);

                callCount = PaintIntercept::GetCalls(callBuffer);
                std::vector<function_call> oldCalls;
                oldCalls.insert(oldCalls.begin(), callBuffer, callBuffer + callCount);

                PaintIntercept::ClearCalls();
                TestPaint::testClearIgnore();
                TestPaint::ResetSupportHeights();
                gPaintSession.WoodenSupportsPrependTo = nullptr;

                CallNew(rideType, trackType, direction, trackSequence, height, &tileElement);

                if (TestPaint::testIsIgnored(direction, trackSequence))
                {
                    *error += String::Format("[  IGNORED ]   %s\n", caseName.c_str());
                    continue;
                }

                callCount = PaintIntercept::GetCalls(callBuffer);
                std::vector<function_call> newCalls;
                newCalls.insert(newCalls.begin(), callBuffer, callBuffer + callCount);

                bool success = true;
                if (oldCalls.size() != newCalls.size())
                {
                    *error += String::Format(
                        "Call counts don't match (was %d, expected %d). %s\n", newCalls.size(), oldCalls.size(),
                        caseName.c_str());
                    success = false;
                }
                else if (!FunctionCall::AssertsEquals(oldCalls, newCalls))
                {
                    *error += String::Format("Calls don't match. %s\n", caseName.c_str());
                    success = false;
                }

                if (!success)
                {
                    *error += " Expected:\n";
                    *error += Printer::PrintFunctionCalls(oldCalls, height);
                    *error += "   Actual:\n";
                    *error += Printer::PrintFunctionCalls(newCalls, height);

                    return TEST_FAILED;
                }
            }
        }
    }

    return TEST_SUCCESS;
}

static uint8_t TestTrackElementSegmentSupportHeight(
    uint8_t rideType, uint8_t trackType, uint8_t trackSequence, std::string* error)
{
    uint16_t height = 3 * 16;

    TileElement tileElement = {};
    tileElement.SetType(TILE_ELEMENT_TYPE_TRACK);
    tileElement.SetLastForTile(true);
    tileElement.AsTrack()->SetTrackType(trackType);
    tileElement.base_height = height / 16;
    g_currently_drawn_item = &tileElement;

    TileElement surfaceElement = {};
    surfaceElement.SetType(TILE_ELEMENT_TYPE_SURFACE);
    surfaceElement.base_height = MINIMUM_LAND_HEIGHT;
    gSurfaceElement = &surfaceElement;
    gDidPassSurface = true;

    gPaintSession.CurrentlyDrawnTileElement = &tileElement;
    gPaintSession.SurfaceElement = &surfaceElement;
    gPaintSession.DidPassSurface = true;

    TestPaint::ResetEnvironment();
    TestPaint::ResetTunnels();

    // TODO: Test Chainlift
    // TODO: Test Maze
    // TODO: Allow skip

    std::string state = String::Format("[trackSequence:%d chainLift:%d]", trackSequence, 0);

    std::vector<SegmentSupportCall> tileSegmentSupportCalls[4];

    for (int direction = 0; direction < 4; direction++)
    {
        TestPaint::ResetSupportHeights();

        CallOriginal(rideType, trackType, direction, trackSequence, height, &tileElement);

        tileSegmentSupportCalls[direction] = SegmentSupportHeightCall::getSegmentCalls(gSupportSegments, direction);
    }

    std::vector<SegmentSupportCall> referenceCalls = tileSegmentSupportCalls[0];

    if (!SegmentSupportHeightCall::CallsMatch(tileSegmentSupportCalls))
    {
        bool success = SegmentSupportHeightCall::FindMostCommonSupportCall(tileSegmentSupportCalls, &referenceCalls);
        if (!success)
        {
            *error += String::Format("Original segment calls didn't match. %s\n", state.c_str());
            for (int direction = 0; direction < 4; direction++)
            {
                *error += String::Format("# %d\n", direction);
                *error += Printer::PrintSegmentSupportHeightCalls(tileSegmentSupportCalls[direction]);
            }
            return TEST_FAILED;
        }
    }

    for (int direction = 0; direction < 4; direction++)
    {
        TestPaint::ResetSupportHeights();

        TestPaint::testClearIgnore();
        CallNew(rideType, trackType, direction, trackSequence, height, &tileElement);
        if (TestPaint::testIsIgnored(direction, trackSequence))
        {
            continue;
        }

        std::vector<SegmentSupportCall> newCalls = SegmentSupportHeightCall::getSegmentCalls(
            gPaintSession.SupportSegments, direction);
        if (!SegmentSupportHeightCall::CallsEqual(referenceCalls, newCalls))
        {
            *error += String::Format("Segment support heights didn't match. [direction:%d] %s\n", direction, state.c_str());
            *error += " Expected:\n";
            *error += Printer::PrintSegmentSupportHeightCalls(referenceCalls);
            *error += "   Actual:\n";
            *error += Printer::PrintSegmentSupportHeightCalls(newCalls);

            return TEST_FAILED;
        }
    }

    return TEST_SUCCESS;
}

static uint8_t TestTrackElementGeneralSupportHeight(
    uint8_t rideType, uint8_t trackType, uint8_t trackSequence, std::string* error)
{
    uint16_t height = 3 * 16;

    TileElement tileElement = {};
    tileElement.SetType(TILE_ELEMENT_TYPE_TRACK);
    tileElement.SetLastForTile(true);
    tileElement.AsTrack()->SetTrackType(trackType);
    tileElement.base_height = height / 16;
    g_currently_drawn_item = &tileElement;

    TileElement surfaceElement = {};
    surfaceElement.SetType(TILE_ELEMENT_TYPE_SURFACE);
    surfaceElement.base_height = MINIMUM_LAND_HEIGHT;
    gSurfaceElement = &surfaceElement;
    gDidPassSurface = true;

    gPaintSession.CurrentlyDrawnTileElement = &tileElement;
    gPaintSession.SurfaceElement = &surfaceElement;
    gPaintSession.DidPassSurface = true;

    TestPaint::ResetEnvironment();
    TestPaint::ResetTunnels();

    // TODO: Test Chainlift
    // TODO: Test Maze
    // TODO: Allow skip

    std::string state = String::Format("[trackSequence:%d chainLift:%d]", trackSequence, 0);

    SupportCall tileGeneralSupportCalls[4];
    for (int direction = 0; direction < 4; direction++)
    {
        TestPaint::ResetSupportHeights();

        CallOriginal(rideType, trackType, direction, trackSequence, height, &tileElement);

        tileGeneralSupportCalls[direction].height = -1;
        tileGeneralSupportCalls[direction].slope = -1;
        if (gSupport.height != 0)
        {
            tileGeneralSupportCalls[direction].height = gSupport.height;
        }
        if (gSupport.slope != 0xFF)
        {
            tileGeneralSupportCalls[direction].slope = gSupport.slope;
        }
    }

    SupportCall referenceCall = tileGeneralSupportCalls[0];
    if (!GeneralSupportHeightCall::CallsMatch(tileGeneralSupportCalls))
    {
        bool success = GeneralSupportHeightCall::FindMostCommonSupportCall(tileGeneralSupportCalls, &referenceCall);
        if (!success)
        {
            *error += String::Format("Original support calls didn't match. %s\n", state.c_str());
            for (int i = 0; i < 4; ++i)
            {
                *error += String::Format("[%d, 0x%02X] ", tileGeneralSupportCalls[i].height, tileGeneralSupportCalls[i].slope);
            }
            *error += "\n";
            return TEST_FAILED;
        }
    }

    for (int direction = 0; direction < 4; direction++)
    {
        TestPaint::ResetSupportHeights();

        TestPaint::testClearIgnore();
        CallNew(rideType, trackType, direction, trackSequence, height, &tileElement);
        if (TestPaint::testIsIgnored(direction, trackSequence))
        {
            continue;
        }

        if (referenceCall.height != -1)
        {
            if (gPaintSession.Support.height != referenceCall.height)
            {
                *error += String::Format(
                    "General support heights didn't match. (expected height + %d, actual: height + %d) [direction:%d] %s\n",
                    referenceCall.height - height, gPaintSession.Support.height - height, direction, state.c_str());
                return TEST_FAILED;
            }
        }
        if (referenceCall.slope != -1)
        {
            if (gPaintSession.Support.slope != referenceCall.slope)
            {
                *error += String::Format(
                    "General support slopes didn't match. (expected 0x%02X, actual: 0x%02X) [direction:%d] %s\n",
                    referenceCall.slope, gPaintSession.Support.slope, direction, state.c_str());
                return TEST_FAILED;
            }
        }
    }

    return TEST_SUCCESS;
}

static uint8_t TestTrackElementSideTunnels(uint8_t rideType, uint8_t trackType, uint8_t trackSequence, std::string* error)
{
    uint16_t height = 3 * 16;

    TileElement tileElement = {};
    tileElement.SetType(TILE_ELEMENT_TYPE_TRACK);
    tileElement.SetLastForTile(true);
    tileElement.AsTrack()->SetTrackType(trackType);
    tileElement.base_height = height / 16;
    g_currently_drawn_item = &tileElement;

    TileElement surfaceElement = {};
    surfaceElement.SetType(TILE_ELEMENT_TYPE_SURFACE);
    surfaceElement.base_height = MINIMUM_LAND_HEIGHT;
    gSurfaceElement = &surfaceElement;
    gDidPassSurface = true;

    gPaintSession.CurrentlyDrawnTileElement = &tileElement;
    gPaintSession.SurfaceElement = &surfaceElement;
    gPaintSession.DidPassSurface = true;

    TestPaint::ResetEnvironment();
    TestPaint::ResetTunnels();

    TunnelCall tileTunnelCalls[4][4];

    // TODO: test inverted tracks

    for (int direction = 0; direction < 4; direction++)
    {
        TestPaint::ResetTunnels();

        for (int8_t offset = -8; offset <= 8; offset += 8)
        {
            CallOriginal(rideType, trackType, direction, trackSequence, height + offset, &tileElement);
        }

        uint8_t rightIndex = (direction + 1) % 4;
        uint8_t leftIndex = direction;

        for (int i = 0; i < 4; ++i)
        {
            tileTunnelCalls[direction][i].call = TUNNELCALL_SKIPPED;
        }

        bool err = false;
        tileTunnelCalls[direction][rightIndex] = SideTunnelCall::ExtractTunnelCalls(
            gRightTunnels, gRightTunnelCount, height, &err);

        tileTunnelCalls[direction][leftIndex] = SideTunnelCall::ExtractTunnelCalls(
            gLeftTunnels, gLeftTunnelCount, height, &err);

        if (err)
        {
            *error += "Multiple tunnels on one side aren't supported.\n";
            return TEST_FAILED;
        }
    }

    TunnelCall newTileTunnelCalls[4][4];
    for (int direction = 0; direction < 4; direction++)
    {
        TestPaint::ResetTunnels();

        TestPaint::testClearIgnore();

        for (int8_t offset = -8; offset <= 8; offset += 8)
        {
            // TODO: move tunnel pushing to interface so we don't have to check the output 3 times
            CallNew(rideType, trackType, direction, trackSequence, height + offset, &tileElement);
        }

        uint8_t rightIndex = (direction + 1) % 4;
        uint8_t leftIndex = direction;

        for (int i = 0; i < 4; ++i)
        {
            newTileTunnelCalls[direction][i].call = TUNNELCALL_SKIPPED;
        }

        bool err = false;
        newTileTunnelCalls[direction][rightIndex] = SideTunnelCall::ExtractTunnelCalls(
            gPaintSession.RightTunnels, gPaintSession.RightTunnelCount, height, &err);
        newTileTunnelCalls[direction][leftIndex] = SideTunnelCall::ExtractTunnelCalls(
            gPaintSession.LeftTunnels, gPaintSession.LeftTunnelCount, height, &err);
        if (err)
        {
            *error += "Multiple tunnels on one side aren't supported.\n";
            return TEST_FAILED;
        }
    }

    if (!SideTunnelCall::TunnelCallsLineUp(tileTunnelCalls))
    {
        // TODO: Check that new pattern uses the same tunnel group (round, big round, etc.)
        *error += String::Format(
            "Original tunnel calls don\'t line up. Skipping tunnel validation [trackSequence:%d].\n", trackSequence);
        *error += Printer::PrintSideTunnelCalls(tileTunnelCalls);

        if (!SideTunnelCall::TunnelCallsLineUp(newTileTunnelCalls))
        {
            *error += String::Format("Decompiled tunnel calls don\'t line up. [trackSequence:%d].\n", trackSequence);
            *error += Printer::PrintSideTunnelCalls(newTileTunnelCalls);
            return TEST_FAILED;
        }

        return TEST_SUCCESS;
    }

    TunnelCall referencePattern[4];
    SideTunnelCall::GetTunnelCallReferencePattern(tileTunnelCalls, &referencePattern);

    TunnelCall actualPattern[4];
    SideTunnelCall::GetTunnelCallReferencePattern(newTileTunnelCalls, &actualPattern);

    if (!SideTunnelCall::TunnelPatternsMatch(referencePattern, actualPattern))
    {
        *error += String::Format("Tunnel calls don't match expected pattern. [trackSequence:%d]\n", trackSequence);
        *error += " Expected:\n";
        *error += Printer::PrintSideTunnelCalls(tileTunnelCalls);
        *error += "   Actual:\n";
        *error += Printer::PrintSideTunnelCalls(newTileTunnelCalls);
        return TEST_FAILED;
    }

    return TEST_SUCCESS;
}

static uint8_t TestTrackElementVerticalTunnels(uint8_t rideType, uint8_t trackType, uint8_t trackSequence, std::string* error)
{
    uint16_t height = 3 * 16;

    TileElement tileElement = {};
    tileElement.SetType(TILE_ELEMENT_TYPE_TRACK);
    tileElement.SetLastForTile(true);
    tileElement.AsTrack()->SetTrackType(trackType);
    tileElement.base_height = height / 16;
    g_currently_drawn_item = &tileElement;

    TileElement surfaceElement = {};
    surfaceElement.SetType(TILE_ELEMENT_TYPE_SURFACE);
    surfaceElement.base_height = MINIMUM_LAND_HEIGHT;
    gSurfaceElement = &surfaceElement;
    gDidPassSurface = true;

    gPaintSession.CurrentlyDrawnTileElement = &tileElement;
    gPaintSession.SurfaceElement = &surfaceElement;
    gPaintSession.DidPassSurface = true;

    TestPaint::ResetEnvironment();
    TestPaint::ResetTunnels();

    uint16_t verticalTunnelHeights[4];

    for (int direction = 0; direction < 4; direction++)
    {
        uint8_t tunnelHeights[3] = { 0 };

        for (uint8_t i = 0; i < 3; i++)
        {
            gVerticalTunnelHeight = 0;
            CallOriginal(rideType, trackType, direction, trackSequence, height - 8 + i * 8, &tileElement);
            tunnelHeights[i] = gVerticalTunnelHeight;
        }

        verticalTunnelHeights[direction] = VerticalTunnelCall::GetTunnelHeight(height, tunnelHeights);
    }

    if (!VerticalTunnelCall::HeightIsConsistent(verticalTunnelHeights))
    {
        *error += String::Format(
            "Original vertical tunnel height is inconsistent, skipping test. [trackSequence:%d]\n", trackSequence);
        return TEST_SUCCESS;
    }

    uint16_t referenceHeight = verticalTunnelHeights[0];

    for (int direction = 0; direction < 4; direction++)
    {
        TestPaint::testClearIgnore();

        testPaintVerticalTunnelHeight = 0;
        CallNew(rideType, trackType, direction, trackSequence, height, &tileElement);

        if (TestPaint::testIsIgnored(direction, trackSequence))
        {
            continue;
        }

        if (testPaintVerticalTunnelHeight != referenceHeight)
        {
            if (referenceHeight == 0)
            {
                *error += String::Format(
                    "Expected no tunnel. Actual: %s [trackSequence:%d]\n",
                    Printer::PrintHeightOffset(testPaintVerticalTunnelHeight, height).c_str(), trackSequence);
                return TEST_FAILED;
            }

            *error += String::Format(
                "Expected vertical tunnel height to be `%s`, was `%s`. [trackSequence:%d direction:%d]\n",
                Printer::PrintHeightOffset(referenceHeight, height).c_str(),
                Printer::PrintHeightOffset(testPaintVerticalTunnelHeight, height).c_str(), trackSequence, direction);

            return TEST_FAILED;
        }
    }

    return TEST_SUCCESS;
}
