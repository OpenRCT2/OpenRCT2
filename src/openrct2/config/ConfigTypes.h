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

enum class VirtualFloorStyles : int32_t;
enum class DrawingEngine : int32_t;
enum class TitleMusicKind : int32_t;

namespace OpenRCT2
{
    enum class CurrencyType : uint8_t;
    enum class CurrencyAffix : uint8_t;

    enum class FileBrowserSort : int32_t
    {
        nameAscending,
        nameDescending,
        sizeAscending,
        sizeDescending,
        dateAscending,
        dateDescending,
    };

    enum class ParkPreviewPref : int32_t
    {
        disabled,
        miniMap,
        screenshot,
    };

    enum class TemperatureUnit : int32_t
    {
        celsius,
        fahrenheit
    };

    enum class ScaleQuality : int32_t
    {
        nearestNeighbour,
        linear,
        smoothNearestNeighbour
    };

    enum class MeasurementFormat : int32_t
    {
        imperial,
        metric,
        SI
    };

    enum class TitleMusicKind : int32_t
    {
        none,
        random,
        OpenRCT2,
        RCT1,
        RCT2,
    };
    enum class AudioEngineType : int32_t
    {
        legacy,
        newEngine,
    };
} // namespace OpenRCT2
