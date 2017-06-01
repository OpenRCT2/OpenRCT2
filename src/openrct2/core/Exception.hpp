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

#include "../common.h"

#include <exception>
#include <string>

class Exception : public std::exception
{
public:
    Exception() : Exception("") { }
    explicit Exception(const char * message) : Exception(std::string(message)) { }
    explicit Exception(const std::string &message) : std::exception(), _message(message) { }

    virtual ~Exception() { }

    const char * what()       const throw() override { return _message.c_str(); }
    const char * GetMessage() const                  { return _message.c_str(); }

private:
    std::string _message;
};
