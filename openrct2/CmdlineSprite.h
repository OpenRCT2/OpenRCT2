/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"
#include "drawing/ImageImporter.h"

int32_t cmdline_for_sprite(const char** argv, int32_t argc);
extern OpenRCT2::Drawing::ImageImporter::ImportMode gSpriteMode;
