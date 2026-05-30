/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.VehicleCrashParticle.h"

#include "../../SpriteIds.h"
#include "../../entity/Particle.h"
#include "../../profiling/Profiling.h"
#include "../Paint.h"

namespace OpenRCT2
{
    static constexpr uint32_t kVehicleCrashParticleSprites[kCrashedVehicleParticleNumberTypes] = {
        SPR_VEHICLE_CRASH_PARTICLE_1, SPR_VEHICLE_CRASH_PARTICLE_2, SPR_VEHICLE_CRASH_PARTICLE_3,
        SPR_VEHICLE_CRASH_PARTICLE_4, SPR_VEHICLE_CRASH_PARTICLE_5,
    };

    void PaintVehicleCrashParticle(PaintSession& session, const VehicleCrashParticle& particle)
    {
        PROFILED_FUNCTION();

        auto& rt = session.rt;
        if (rt.zoom_level > ZoomLevel{ 0 })
        {
            return;
        }

        uint32_t imageId = kVehicleCrashParticleSprites[particle.crashed_sprite_base] + particle.frame / 256;
        auto image = ImageId(imageId, particle.colour[0], particle.colour[1]);
        paintAddImageAsParent(session, image, { 0, 0, particle.z }, { 1, 1, 0 });
    }
} // namespace OpenRCT2
