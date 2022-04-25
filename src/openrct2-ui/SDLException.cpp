/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SDLException.h"

#include <SDL.h>

SDLException::SDLException(const std::string& message)
    : runtime_error(message.c_str())
{
}

SDLException::SDLException(const char* message)
    : runtime_error(message)
{
}

void SDLException::Throw(const char* call)
{
    std::string message = std::string(call) + ": " + std::string(SDL_GetError());
    throw SDLException(message);
}
