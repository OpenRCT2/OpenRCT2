/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <string>

using CExceptionHandler = void*;

extern bool gOpenRCT2SilentBreakpad;
CExceptionHandler crash_init();
void crash_register_additional_file(const std::string& key, const std::string& path);
void crash_unregister_additional_file(const std::string& key);
