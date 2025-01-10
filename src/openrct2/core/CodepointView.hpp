/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "String.hpp"
#include "UTF8.h"

namespace OpenRCT2
{
    class CodepointView
    {
    private:
        std::string_view _str;

    public:
        class iterator
        {
        private:
            std::string_view _str;
            size_t _index;

        public:
            iterator(std::string_view str, size_t index)
                : _str(str)
                , _index(index)
            {
            }

            bool operator==(const iterator& rhs) const
            {
                return _index == rhs._index;
            }
            bool operator!=(const iterator& rhs) const
            {
                return _index != rhs._index;
            }
            char32_t operator*() const
            {
                return GetNextCodepoint(&_str[_index], nullptr);
            }
            iterator& operator++()
            {
                if (_index < _str.size())
                {
                    const utf8* nextch;
                    GetNextCodepoint(&_str[_index], &nextch);
                    _index = std::min<size_t>(nextch - _str.data(), _str.size());
                }
                return *this;
            }
            iterator operator++(int)
            {
                auto result = *this;
                if (_index < _str.size())
                {
                    const utf8* nextch;
                    GetNextCodepoint(&_str[_index], &nextch);
                    _index = std::min<size_t>(nextch - _str.data(), _str.size());
                }
                return result;
            }

            size_t GetIndex() const
            {
                return _index;
            }

            static char32_t GetNextCodepoint(const char* ch, const char** next)
            {
                return UTF8GetNext(ch, next);
            }
        };

        CodepointView(std::string_view str)
            : _str(String::utf8Truncate(str, str.size()))
        {
        }

        iterator begin() const
        {
            return iterator(_str, 0);
        }

        iterator end() const
        {
            return iterator(_str, _str.size());
        }
    };
} // namespace OpenRCT2
