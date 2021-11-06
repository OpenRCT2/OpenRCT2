/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../common.h"

struct paint_session;
struct CoordsXY;

void sprite_paint_setup(paint_session* session, const CoordsXY& pos);

template<typename T> void PaintEntity(paint_session* session, const T* entity, int32_t imageDirection);

extern const uint32_t vehicle_particle_base_sprites[5];
