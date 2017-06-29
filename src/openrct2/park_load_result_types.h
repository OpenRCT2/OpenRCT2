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

#ifndef _PARK_LOAD_RESULT_H_
#define _PARK_LOAD_RESULT_H_

#include "object.h"

enum PARK_LOAD_ERROR {
    PARK_LOAD_ERROR_NONE,
    PARK_LOAD_ERROR_BAD_OBJECTS,
    PARK_LOAD_ERROR_INVALID_EXTENSION,
    PARK_LOAD_ERROR_UNKNOWN = 255
};

typedef struct object_validity_result
{
    uint16 invalid_object_count;
    rct_object_entry * * invalid_objects;

} object_validity_result;

typedef struct park_load_result {
    uint8 error;
    object_validity_result* object_validity;
} park_load_result;
#endif
