/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"
#include "drawing/ImageImporter.h"

int32_t CommandLineForSprite(const char** argv, int32_t argc);
extern OpenRCT2::Drawing::ImageImporter::ImportMode gSpriteMode;
