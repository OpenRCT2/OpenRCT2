#pragma once

#include "Surface.h"

#include <cstdint>

constexpr auto SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT = 0x20;

uint8_t LowerSurfaceCornerFlags(uint8_t SelectedCorner, uint8_t CurrentSlope);
uint8_t RaiseSurfaceCornerFlags(uint8_t SelectedCorner, uint8_t CurrentSlope);
