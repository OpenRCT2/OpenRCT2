/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

struct ZoomLevel;

namespace OpenRCT2
{
    using WindowNumber = int16_t;
    using WidgetIndex = uint16_t;
    using CloseCallback = void (*)();

    struct Viewport;
    struct Widget;
    struct WindowBase;

    enum class WindowClass : uint8_t;

    enum class WindowView : uint8_t
    {
        parkAwards,
        parkRating,
        parkObjective,
        parkGuests,
        financesResearch,
        rideResearch,
        mazeConstruction,
        networkPassword,
        changelog,
        newVersionInfo,
        financeMarketing,
        contributors,
    };

    enum class WindowDetail : uint8_t
    {
        banner,
        newCampaign,
        demolishRide,
        refurbishRide,
        sign,
        signSmall,

        player,

        vehicle,
        track,

        null = 255,
    };

    struct WidgetRef
    {
        WindowClass windowClassification;
        WindowNumber windowNumber;
        WidgetIndex widgetIndex;
    };

    struct WindowIdentifier
    {
        WindowClass classification;
        WindowNumber number;
    };

    struct WidgetIdentifier
    {
        WindowIdentifier window;
        WidgetIndex widgetIndex;
    };

    enum class CloseWindowModifier : uint8_t
    {
        none,
        shift,
        control
    };

    struct WindowCloseModifier
    {
        WindowIdentifier window;
        CloseWindowModifier modifier;
    };

    enum class PromptMode : uint8_t
    {
        saveBeforeLoad = 0,
        saveBeforeQuit,
        saveBeforeQuit2,
        saveBeforeNewGame,
        quit
    };

    enum class LoadSaveAction : uint8_t
    {
        load,
        save,
    };

    enum class LoadSaveType : uint8_t
    {
        park,
        landscape,
        scenario,
        track,
        heightmap,
        mapgenSettings
    };

    enum class ModalResult : int8_t
    {
        fail = -1,
        cancel,
        ok,
    };

    enum class GuestListFilterType : int32_t
    {
        guestsOnRide,
        guestsInQueue,
        guestsThinkingAboutRide,
        guestsThinkingX,
    };

    enum class Tool
    {
        arrow = 0,
        upArrow = 2,
        upDownArrow = 3,
        picker = 7,
        crosshair = 12,
        pathDown = 17,
        digDown = 18,
        waterDown = 19,
        walkDown = 22,
        paintDown = 23,
        entranceDown = 24,
        bulldozer = 27,
    };
} // namespace OpenRCT2
