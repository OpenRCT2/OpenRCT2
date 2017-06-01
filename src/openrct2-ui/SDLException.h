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

#include <SDL.h>
#include <stdexcept>

/**
 * An exception which wraps an SDL error.
 */
class SDLException : public std::runtime_error
{
public:
    explicit SDLException(const std::string& message)
        : runtime_error(message.c_str())
    {
        SDL_GetError();
    }

    explicit SDLException(const char * message)
        : runtime_error(message)
    {
    }

    /**
     * Throws an SDL exception with a message containing the given call information
     * and the message given by SDL_GetError.
     */
    static void Throw(const char * call)
    {
        std::string message = std::string(call) + ": " + std::string(SDL_GetError());
        throw SDLException(message);
    }
};
