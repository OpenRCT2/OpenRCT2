/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

namespace OpenRCT2::Scripting
{
    class ScriptEngine;

    class UiScriptExtensions
    {
    public:
        static void Extend(ScriptEngine& scriptEngine);
    };
} // namespace OpenRCT2::Scripting

#endif
