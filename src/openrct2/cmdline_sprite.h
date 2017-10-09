#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _CMDLINE_SPRITE_H_
#define _CMDLINE_SPRITE_H_

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

sint32 cmdline_for_sprite(const char **argv, sint32 argc);
extern sint32 gSpriteMode;

#ifdef __cplusplus
}
#endif

#endif
