/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <stdexcept>
#include <string>

/**
 * An exception which wraps an SDL error.
 */
class SDLException : public std::runtime_error
{
public:
    explicit SDLException(const std::string& message);

    explicit SDLException(const char* message);

    /**
     * Throws an SDL exception with a message containing the given call information
     * and the message given by SDL_GetError.
     */
    static void Throw(const char* call);
};
