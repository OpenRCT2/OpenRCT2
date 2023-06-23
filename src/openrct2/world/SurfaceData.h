#pragma once

#include <cstdint>
#include <cstdio>

constexpr auto SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT = 0x20;

uint8_t LowerSurfaceCornerFlags(size_t SelectedCorner, size_t CurrentSlope);
uint8_t RaiseSurfaceCornerFlags(size_t SelectedCorner, size_t CurrentSlope);
