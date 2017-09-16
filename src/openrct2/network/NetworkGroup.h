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

#ifdef __cplusplus

#include <array>
#include <jansson.h>
#include <string>
#include "NetworkPacket.h"
#include "../common.h"

class NetworkGroup final
{
public:
    std::array<uint8, 8>    ActionsAllowed;
    uint8                   Id = 0;

    static NetworkGroup FromJson(const json_t * json);

    NetworkGroup();
    ~NetworkGroup();

    const std::string & GetName() const;
    void SetName(std::string name);

    void Read(NetworkPacket &packet);
    void Write(NetworkPacket &packet);
    void ToggleActionPermission(size_t index);
    bool CanPerformAction(size_t index) const;
    bool CanPerformCommand(sint32 command) const;

    json_t * ToJson() const;

private:
    std::string _name;
};

#endif
