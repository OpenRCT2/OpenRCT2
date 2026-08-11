/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.ExplosionFlare.h"

#include "../../SpriteIds.h"
#include "../../entity/Particle.h"
#include "../../profiling/Profiling.h"
#include "../Paint.h"

using namespace OpenRCT2;

void PaintExplosionFlare(PaintSession& session, const ExplosionFlare& explosionFare)
{
    PROFILED_FUNCTION();

    uint32_t imageId = SPR_EXPLOSION_FLARE + (explosionFare.frame / 256);
    PaintAddImageAsParent(session, ImageId(imageId), { 0, 0, explosionFare.z }, { 1, 1, 0 });
}
