#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include <SDL_opengl.h>

#if __WINDOWS__

#ifdef NO_EXTERN_GLAPI
    #define GLAPI_DECL
    #define GLAPI_SET   = nullptr
#else
    #define GLAPI_DECL  extern
    #define GLAPI_SET
#endif

GLAPI_DECL PFNGLCREATESHADERPROC glCreateShader GLAPI_SET;
GLAPI_DECL PFNGLDELETESHADERPROC glDeleteShader GLAPI_SET;

#endif /* __WINDOWS__ */

namespace OpenGLAPI
{
    bool Initialise();
}
