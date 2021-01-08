/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Guard.hpp"

#include <algorithm>
#include <array>
#include <utility>

template<typename T, size_t MAX> class FixedVector
{
public:
    using container = std::array<T, MAX>;
    using iterator = typename container::iterator;
    using const_iterator = typename container::const_iterator;
    using reverse_iterator = typename container::reverse_iterator;
    using const_reverse_iterator = typename container::const_reverse_iterator;
    using value_type = typename container::value_type;
    using reference_type = value_type&;
    using const_reference_type = const value_type&;

    iterator begin()
    {
        if (_count == 0)
            return _data.end();

        return _data.begin();
    }

    iterator end()
    {
        if (_count == 0)
            return _data.end();
        return begin() + _count;
    }

    const_iterator cbegin() const
    {
        if (_count == 0)
            return _data.cend();

        return _data.cbegin();
    }

    const_iterator cend() const
    {
        if (_count == 0)
            return _data.cend();
        return cbegin() + _count;
    }

    reverse_iterator rbegin()
    {
        if (_count == 0)
            return _data.rend();
        return _data.rbegin() + (MAX - _count);
    }

    reverse_iterator rend()
    {
        return _data.rend();
    }

    const_reverse_iterator rbegin() const
    {
        return _data.rbegin() + (MAX - _count);
    }

    const_reverse_iterator rend() const
    {
        return _data.rend();
    }

    reference_type back()
    {
        return _data[_count - 1];
    }

    const_reference_type back() const
    {
        return _data[_count - 1];
    }

    void push_back(const T& val)
    {
        Guard::Assert(_count < MAX);
        _data[_count++] = val;
    }

    void push_back(T&& val)
    {
        Guard::Assert(_count < MAX);
        _data[_count++] = std::move(val);
    }

    iterator insert(iterator pos, const T& val)
    {
        // Make sure the end iterator is correct.
        auto itCurEnd = end();
        push_back(val);

        // Shift all elements to the right
        auto offset = pos - begin();
        std::rotate(_data.begin(), pos, itCurEnd);

        return _data.begin() + offset;
    }

    iterator insert(iterator pos, T&& val)
    {
        // Make sure the end iterator is correct.
        auto itCurEnd = end();
        emplace_back(std::move(val));

        // Shift all elements to the right
        auto offset = pos - begin();
        std::rotate(_data.begin() + offset, itCurEnd, end());

        return _data.begin() + offset;
    }

    template<typename... Args> reference_type emplace_back(Args&&... args)
    {
        Guard::Assert(_count < MAX);
        reference_type res = _data[_count++];
        ::new (&res) T(std::forward<Args&&>(args)...);
        return res;
    }

    reference_type operator[](const size_t n)
    {
        return _data[n];
    }

    const_reference_type operator[](const size_t n) const
    {
        return _data[n];
    }

    void pop_back()
    {
        _count--;
    }

    void clear()
    {
        _count = 0;
    }

    size_t size() const
    {
        return _count;
    }

    size_t capacity() const
    {
        return _data.size();
    }

    bool empty() const
    {
        return _count == 0;
    }

private:
    size_t _count = 0;
    container _data;
};
