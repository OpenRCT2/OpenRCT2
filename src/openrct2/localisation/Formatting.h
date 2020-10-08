/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Language.h"

#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace OpenRCT2
{
    using FormatToken = uint32_t;

    template<typename T> void FormatArgument(std::stringstream& ss, FormatToken token, T arg);

    std::pair<std::string_view, uint32_t> FormatNextPart(std::string_view& fmt);
    bool CanFormatToken(FormatToken t);

    inline void FormatString(std::stringstream& ss, std::string_view& fmtc)
    {
        while (!fmtc.empty())
        {
            auto [part, token] = FormatNextPart(fmtc);
            if (!CanFormatToken(token))
            {
                ss << part;
                FormatString(ss, fmtc);
            }
        }
    }

    template<typename TArg0> static void FormatString(std::stringstream& ss, std::string_view& fmtc, TArg0 arg0)
    {
        if (!fmtc.empty())
        {
            auto [part, token] = FormatNextPart(fmtc);
            if (CanFormatToken(token))
            {
                FormatArgument(ss, token, arg0);
                FormatString(ss, fmtc);
            }
            else
            {
                ss << part;
                FormatString(ss, fmtc, arg0);
            }
        }
    }

    template<typename TArg0, typename... TArgs>
    static void FormatString(std::stringstream& ss, std::string_view& fmtc, TArg0 arg0, TArgs&&... argN)
    {
        if (!fmtc.empty())
        {
            auto [part, token] = FormatNextPart(fmtc);
            if (CanFormatToken(token))
            {
                FormatArgument(ss, token, arg0);
                return FormatString(ss, fmtc, argN...);
            }
            else
            {
                ss << part;
                return FormatString(ss, fmtc, arg0, argN...);
            }
        }
    }

    template<typename... TArgs> std::string FormatString(std::string_view fmt, TArgs&&... argN)
    {
        thread_local std::stringstream ss;
        // Reset the buffer (reported as most efficient way)
        std::stringstream().swap(ss);
        FormatString(ss, fmt, argN...);
        return ss.str();
    }

    template<typename... TArgs> static void FormatStringId(std::stringstream& ss, rct_string_id fmt, TArgs&&... argN)
    {
        auto lang = language_get_string(fmt);
        auto fmtsz = language_convert_string_to_tokens(lang);
        auto fmtc = std::string_view(fmtsz);
        FormatString(ss, fmtc, argN...);
    }

    template<typename... TArgs> std::string FormatStringId(rct_string_id fmt, TArgs&&... argN)
    {
        auto lang = language_get_string(fmt);
        auto fmtc = language_convert_string_to_tokens(lang);
        return FormatString(fmtc, argN...);
    }
} // namespace OpenRCT2
