/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/StringTypes.h"

struct MapGenSettings;

bool MapGenLoadHeightmapImage(const utf8* path);
void MapGenUnloadHeightmapImage();
void MapGenGenerateFromHeightmapImage(MapGenSettings* settings);
