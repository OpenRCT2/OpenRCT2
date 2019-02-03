/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <array>

template<typename _TType, size_t _TMax> class CircularBuffer
{
public:
    typedef _TType value_type;
    typedef _TType* pointer;
    typedef const _TType* const_pointer;
    typedef _TType& reference;
    typedef const _TType& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    reference front()
    {
        return _elements[_head];
    }

    const_reference front() const
    {
        return _elements[_head];
    }

    reference back()
    {
        return _elements[_tail];
    }

    const_reference back() const
    {
        return _elements[_tail];
    }

    reference operator[](size_type idx)
    {
        idx = (_head + idx) % capacity();
        return _elements[idx];
    }

    const_reference operator[](size_type idx) const
    {
        idx = (_head + idx) % capacity();
        return _elements[idx];
    }

    void clear()
    {
        _head = 0;
        _tail = 0;
        _size = 0;
    }

    size_type size() const
    {
        return _size;
    }

    bool empty() const
    {
        return _size == 0;
    }

    constexpr size_type capacity() const
    {
        return _elements.size();
    }

    void push_back(const value_type& val)
    {
        if (_size == capacity())
        {
            _head = (_head + 1) % capacity();
        }
        else
        {
            _size++;
        }
        _elements[_tail] = val;
        _tail = (_tail + 1) % capacity();
    }

private:
    size_t _head = 0;
    size_t _tail = 0;
    size_t _size = 0;
    std::array<_TType, _TMax> _elements;
};
