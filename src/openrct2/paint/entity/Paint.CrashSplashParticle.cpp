/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.CrashSplashParticle.h"

#include "../../SpriteIds.h"
#include "../../entity/Particle.h"
#include "../../profiling/Profiling.h"
#include "../Paint.h"

using namespace OpenRCT2;

void PaintCrashSplashParticle(PaintSession& session, const CrashSplashParticle& particle)
{
    PROFILED_FUNCTION();

    uint32_t imageId = SPR_CRASH_SPLASH_PARTICLE + (particle.frame / 256);
    paintAddImageAsParent(session, ImageId(imageId), { 0, 0, particle.z }, { 1, 1, 0 });
}
