/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

enum class MeasurementFormat : int32_t;
enum class TemperatureUnit : int32_t;
enum class CurrencyType : uint8_t;
enum class ScaleQuality : int32_t;
enum class VirtualFloorStyles : int32_t;
enum class DrawingEngine : int32_t;
enum class TitleMusicKind : int32_t;

enum class FileBrowserSort : int32_t
{
    NameAscending,
    NameDescending,
    SizeAscending,
    SizeDescending,
    DateAscending,
    DateDescending,
};

enum class TemperatureUnit : int32_t
{
    Celsius,
    Fahrenheit
};

enum class ScaleQuality : int32_t
{
    NearestNeighbour,
    Linear,
    SmoothNearestNeighbour
};

enum class MeasurementFormat : int32_t
{
    Imperial,
    Metric,
    SI
};

enum class TitleMusicKind : int32_t
{
    None,
    Random,
    OpenRCT2,
    RCT1,
    RCT2,
};
