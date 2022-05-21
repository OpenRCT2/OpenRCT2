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

#include <cstring>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace OpenRCT2
{
    template<typename T, size_t StackSize = 256, typename TTraits = std::char_traits<T>> class FormatBufferBase
    {
        T _storage[StackSize];
        T* _buffer;
        size_t _size;
        // NOTE: Capacity is on purpose uint32_t to have a fixed position for the flag on each architecture.
        uint32_t _capacity;
        TTraits _traits;

        static constexpr uint32_t FlagLocalStorage = (1u << 31);

    public:
        explicit FormatBufferBase()
            : _storage{}
            , _buffer(_storage)
            , _size{}
            , _capacity(FlagLocalStorage | static_cast<uint32_t>(StackSize))
        {
        }

        ~FormatBufferBase()
        {
            if (_capacity & FlagLocalStorage)
                return;
            delete[] _buffer;
        }

        size_t size() const
        {
            return _size;
        }

        size_t capacity() const
        {
            return _capacity & ~FlagLocalStorage;
        }

        void clear()
        {
            _size = 0;
            _buffer[0] = T{};
        }

        const T* data() const
        {
            return _buffer;
        }

        T* data()
        {
            return _buffer;
        }

        template<size_t N> auto& operator<<(T const (&v)[N])
        {
            append(v, N);
            return *this;
        }

        auto& operator<<(const T v)
        {
            append(&v, 1);
            return *this;
        }

        auto& operator<<(const T* v)
        {
            if (!v)
                return *this;

            append(v, _traits.length(v));
            return *this;
        }

        auto& operator<<(const std::basic_string_view<T> v)
        {
            append(v.data(), v.size());
            return *this;
        }

        auto& operator<<(const std::basic_string<T>& v)
        {
            append(v.data(), v.size());
            return *this;
        }

        void append(const T* buf, size_t len)
        {
            ensure_capacity(len);

            std::copy(buf, buf + len, _buffer + _size);

            _size += len;
            _buffer[_size] = T{};
        }

    private:
        void ensure_capacity(size_t additionalSize)
        {
            const size_t curSize = size();
            const size_t curCapacity = capacity();
            const bool isLocalStorage = _capacity & FlagLocalStorage;

            if (curSize + additionalSize < curCapacity)
                return;

            const size_t newCapacity = (curCapacity + additionalSize + 1) << 1;

            T* newBuf = new T[newCapacity];
            std::copy(_buffer, _buffer + curSize, newBuf);

            if (!isLocalStorage)
                delete[] _buffer;

            _capacity = static_cast<uint32_t>(newCapacity);
            _buffer = newBuf;
        }
    };

    using FormatBuffer = FormatBufferBase<char>;

    using FormatArg_t = std::variant<uint16_t, int32_t, int64_t, const char*, std::string>;

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
            uint32_t parameter{};

            token() = default;
            token(FormatToken k, std::string_view s, uint32_t p = 0);
            bool IsLiteral() const;
            bool IsCodepoint() const;
            codepoint_t GetCodepoint() const;
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

        FmtString() = default;
        FmtString(std::string&& s);
        FmtString(std::string_view s);
        FmtString(const char* s);
        iterator begin() const;
        iterator end() const;

        std::string WithoutFormatTokens() const;
    };

    template<typename T> void FormatArgument(FormatBuffer& ss, FormatToken token, T arg);

    bool IsRealNameStringId(rct_string_id id);
    void FormatRealName(FormatBuffer& ss, rct_string_id id);
    FmtString GetFmtStringById(rct_string_id id);
    FormatBuffer& GetThreadFormatStream();
    size_t CopyStringStreamToBuffer(char* buffer, size_t bufferLen, FormatBuffer& ss);

    inline void FormatString(FormatBuffer& ss, std::stack<FmtString::iterator>& stack)
    {
        while (!stack.empty())
        {
            auto& it = stack.top();
            while (!it.eol())
            {
                const auto& token = *it;
                if (!FormatTokenTakesArgument(token.kind))
                {
                    ss << token.text;
                }
                it++;
            }
            stack.pop();
        }
    }

    template<typename TArg0, typename... TArgs>
    static void FormatString(FormatBuffer& ss, std::stack<FmtString::iterator>& stack, TArg0 arg0, TArgs&&... argN)
    {
        while (!stack.empty())
        {
            auto& it = stack.top();
            while (!it.eol())
            {
                auto token = *it++;
                if (token.kind == FormatToken::StringId)
                {
                    if constexpr (std::is_integral<TArg0>())
                    {
                        auto stringId = static_cast<rct_string_id>(arg0);
                        if (IsRealNameStringId(stringId))
                        {
                            FormatRealName(ss, stringId);
                            return FormatString(ss, stack, argN...);
                        }

                        auto subfmt = GetFmtStringById(stringId);
                        auto subit = subfmt.begin();
                        stack.push(subit);
                        return FormatString(ss, stack, argN...);
                    }
                }
                else if (FormatTokenTakesArgument(token.kind))
                {
                    FormatArgument(ss, token.kind, arg0);
                    return FormatString(ss, stack, argN...);
                }

                ss << token.text;
            }
            stack.pop();
        }
    }

    template<typename... TArgs> static void FormatString(FormatBuffer& ss, const FmtString& fmt, TArgs&&... argN)
    {
        std::stack<FmtString::iterator> stack;
        stack.push(fmt.begin());
        FormatString(ss, stack, argN...);
    }

    template<typename... TArgs> std::string FormatString(const FmtString& fmt, TArgs&&... argN)
    {
        auto& ss = GetThreadFormatStream();
        FormatString(ss, fmt, argN...);
        return ss.data();
    }

    template<typename... TArgs>
    size_t FormatStringToBuffer(char* buffer, size_t bufferLen, const FmtString& fmt, TArgs&&... argN)
    {
        auto& ss = GetThreadFormatStream();
        FormatString(ss, fmt, argN...);
        return CopyStringStreamToBuffer(buffer, bufferLen, ss);
    }

    template<typename... TArgs> static void FormatStringId(FormatBuffer& ss, rct_string_id id, TArgs&&... argN)
    {
        auto fmt = GetFmtStringById(id);
        FormatString(ss, fmt, argN...);
    }

    template<typename... TArgs> std::string FormatStringId(rct_string_id id, TArgs&&... argN)
    {
        auto fmt = GetFmtStringById(id);
        return FormatString(fmt, argN...);
    }

    template<typename... TArgs> size_t FormatStringId(char* buffer, size_t bufferLen, rct_string_id id, TArgs&&... argN)
    {
        auto& ss = GetThreadFormatStream();
        FormatStringId(ss, id, argN...);
        return CopyStringStreamToBuffer(buffer, bufferLen, ss);
    }

    std::string FormatStringAny(const FmtString& fmt, const std::vector<FormatArg_t>& args);
    size_t FormatStringAny(char* buffer, size_t bufferLen, const FmtString& fmt, const std::vector<FormatArg_t>& args);
    size_t FormatStringLegacy(char* buffer, size_t bufferLen, rct_string_id id, const void* args);
} // namespace OpenRCT2
