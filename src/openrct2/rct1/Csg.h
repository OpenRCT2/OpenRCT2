/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"

struct Gx;

bool RCT1DataPresentAtLocation(u8string_view path);
std::string FindCsg1datAtLocation(u8string_view path);
bool Csg1datPresentAtLocation(u8string_view path);
std::string FindCsg1idatAtLocation(u8string_view path);
bool Csg1idatPresentAtLocation(u8string_view path);
bool CsgIsUsable(const Gx& csg);
bool CsgAtLocationIsUsable(u8string_view path);
