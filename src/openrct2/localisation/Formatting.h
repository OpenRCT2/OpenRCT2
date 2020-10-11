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
#include "FormatCodes.h"
#include "Language.h"

#include <any>
#include <sstream>
#include <stack>
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
        private:
            std::string_view str;
            size_t index;
            token current;

            void update();

        public:
            iterator(std::string_view s, size_t i);
            bool operator==(iterator& rhs);
            bool operator!=(iterator& rhs);
            token CreateToken(size_t len);
            const token* operator->() const;
            const token& operator*();
            iterator& operator++();
            iterator operator++(int);
            bool eol() const;
        };

        FmtString(std::string&& s);
        FmtString(std::string_view s);
        FmtString(const char* s);
        iterator begin() const;
        iterator end() const;

        std::string WithoutFormatTokens() const;
    };

    template<typename T> void FormatArgument(std::stringstream& ss, FormatToken token, T arg);

    bool CanFormatToken(FormatToken t);
    FmtString GetFmtStringById(rct_string_id id);

    inline void FormatString(std::stringstream& ss, std::stack<FmtString::iterator*> stack)
    {
        while (!stack.empty())
        {
            auto& it = *stack.top();
            while (!it.eol())
            {
                const auto& token = *it++;
                if (!CanFormatToken(token.kind))
                {
                    ss << token.text;
                }
            }
            stack.pop();
        }
    }

    template<typename TArg0, typename... TArgs>
    static void FormatString(std::stringstream& ss, std::stack<FmtString::iterator*> stack, TArg0 arg0, TArgs&&... argN)
    {
        while (!stack.empty())
        {
            auto& it = *stack.top();
            while (!it.eol())
            {
                const auto& token = *it++;
                if (token.kind == FORMAT_STRINGID || token.kind == FORMAT_STRINGID2)
                {
                    if constexpr (std::is_integral<TArg0>())
                    {
                        auto subfmt = GetFmtStringById(static_cast<rct_string_id>(arg0));
                        auto subit = subfmt.begin();
                        stack.push(&subit);
                        FormatString(ss, stack, argN...);
                    }
                }
                else if (CanFormatToken(token.kind))
                {
                    FormatArgument(ss, token.kind, arg0);
                    return FormatString(ss, stack, argN...);
                }
                else
                {
                    ss << token.text;
                }
            }
            stack.pop();
        }
    }

    template<typename... TArgs> static void FormatString(std::stringstream& ss, const FmtString& fmt, TArgs&&... argN)
    {
        auto it = fmt.begin();
        std::stack<FmtString::iterator*> stack;
        stack.push(&it);
        FormatString(ss, stack, argN...);
    }

    template<typename... TArgs> std::string FormatString(const FmtString& fmt, TArgs&&... argN)
    {
        thread_local std::stringstream ss;
        // Reset the buffer (reported as most efficient way)
        std::stringstream().swap(ss);
        FormatString(ss, fmt, argN...);
        return ss.str();
    }

    template<typename... TArgs> static void FormatStringId(std::stringstream& ss, rct_string_id id, TArgs&&... argN)
    {
        auto fmt = GetFmtStringById(id);
        FormatString(ss, fmt, argN...);
    }

    template<typename... TArgs> std::string FormatStringId(rct_string_id id, TArgs&&... argN)
    {
        auto fmt = GetFmtStringById(id);
        return FormatString(fmt, argN...);
    }

    std::string FormatStringAny(const FmtString& fmt, const std::vector<std::any>& args);
} // namespace OpenRCT2
