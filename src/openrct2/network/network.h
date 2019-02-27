/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#define NETWORK_DEFAULT_PORT 11753
#define MAX_SERVER_DESCRIPTION_LENGTH 256

#ifdef _WIN32
#    pragma comment(lib, "Ws2_32.lib")
#endif

#include "../common.h"
#include "../localisation/StringIds.h"
#include "NetworkClient.h"
#include "NetworkLegacy.h"
#include "NetworkServer.h"
#include "NetworkTypes.h"

#include <memory>
#include <string>

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}
