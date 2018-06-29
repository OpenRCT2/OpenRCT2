/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include "../common.h"

struct server_entry
{
    std::string address;
    std::string name;
    std::string description;
    std::string version;
    bool        requiresPassword    = false;
    bool        favourite           = false;
    uint8_t       players             = 0;
    uint8_t       maxplayers          = 0;
};

std::vector<server_entry> server_list_read();
bool server_list_write(const std::vector<server_entry> &entries);
