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

#pragma once

#include <openrct2/common.h>

#pragma pack(push, 1)

struct vec2i
{
    union { sint32 x; sint32 s; sint32 r; };
    union { sint32 y; sint32 t; sint32 g; };
};

struct vec2f
{
    union { float x; float s; float r; };
    union { float y; float t; float g; };
};

struct vec3f
{
    union { float x; float s; float r; };
    union { float y; float t; float g; };
    union { float z; float p; float b; };
};

struct vec4i
{
    union { sint32 x; sint32 s; sint32 r; };
    union { sint32 y; sint32 t; sint32 g; };
    union { sint32 z; sint32 p; sint32 b; };
    union { sint32 w; sint32 q; sint32 a; };
};

struct vec4f
{
    union { float x; float s; float r; };
    union { float y; float t; float g; };
    union { float z; float p; float b; };
    union { float w; float q; float a; };
};

#pragma pack(pop)
