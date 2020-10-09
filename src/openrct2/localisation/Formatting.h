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

#include <any>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace OpenRCT2
{
    using FormatToken = uint32_t;

    class FmtString
    {
    private:
        std::string_view _str;
        std::string _strOwned;

    public:
        struct token
        {
            FormatToken kind{};
            std::string_view text;

            token() = default;
            token(FormatToken k, std::string_view s);
            bool IsLiteral() const;
        };

        struct iterator
        {
            std::string_view str;
            size_t index;
            token current;

            iterator(std::string_view s, size_t i);
            void update();
            bool operator==(iterator& rhs);
            bool operator!=(iterator& rhs);
            token CreateToken(size_t len);
            const token* operator->() const;
            const token& operator*();
            iterator& operator++();
        };

        FmtString(std::string&& s);
        FmtString(std::string_view s);
        FmtString(const char* s);
        iterator begin() const;
        iterator end() const;

        std::string WithoutFormatTokens() const;
    };

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

    std::string FormatStringAny(std::string_view fmt, const std::vector<std::any>& args);
} // namespace OpenRCT2
