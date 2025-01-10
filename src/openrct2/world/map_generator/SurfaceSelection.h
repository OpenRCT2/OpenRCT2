/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../object/ObjectTypes.h"

namespace OpenRCT2::World::MapGenerator
{
    struct Settings;

    ObjectEntryIndex generateSurfaceTextureId(Settings* settings);
    ObjectEntryIndex generateEdgeTextureId(Settings* settings, ObjectEntryIndex surfaceTextureId);
    ObjectEntryIndex generateBeachTextureId();
} // namespace OpenRCT2::World::MapGenerator
