#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <functional>
#include <vector>

#include "../common.h"
#include "Guard.hpp"
#include "Memory.hpp"

/**
 * A container that stores elements in contiguous memory. Automatically reallocates memory when required. Equivalent to
 * std::vector.
 */
template<typename T>
class List : public std::vector<T>
{
public:
    typedef typename std::vector<T>::const_reference const_reference;
    typedef typename std::vector<T>::reference reference;
    size_t    GetCapacity() const { return this->capacity(); }
    size_t    GetCount()    const { return this->size(); }
    const T * GetItems()    const { return this->data(); }

    List() : std::vector<T>() { }

    List(std::initializer_list<T> initializerList) : std::vector<T>(initializerList) { }

    List(size_t capacity) : std::vector<T>(capacity) { }

    List(const T * items, size_t count) : std::vector<T>(items, items + count) { }

    void EnsureCapacity(size_t capacity)
    {
        this->reserve(capacity);
    }

    void ShrinkToLength()
    {
        this->shrink_to_fit();
    }

    void Clear()
    {
        this->clear();
    }

    void Add(T item)
    {
        this->push_back(item);
    }

    void AddRange(std::initializer_list<T> initializerList)
    {
        this->insert(this->end(), initializerList.begin(), initializerList.end());
    }

    void Insert(T item, size_t index)
    {
        Guard::ArgumentInRange(index, (size_t)0, this->size(), GUARD_LINE);
        this->insert(this->begin() + index, item);
    }

    bool Remove(T item)
    {
        for (size_t i = 0; i < this->size(); i++)
        {
            if (*this[i] == item)
            {
                RemoveAt(i);
                return true;
            }
        }
        return false;
    }

    void RemoveAt(size_t index)
    {
        Guard::ArgumentInRange(index, (size_t)0, this->size() - 1, GUARD_LINE);
        this->erase(this->begin() + index);
    }

    const T * ToArray() const
    {
        return Memory::DuplicateArray(this->data(), this->size());
    }

    const_reference operator[](size_t index) const
    {
        Guard::ArgumentInRange(index, (size_t)0, this->size() - 1, GUARD_LINE);
        return std::vector<T>::operator[](index);
    }

    reference operator[](size_t index)
    {
        Guard::ArgumentInRange(index, (size_t)0, this->size() - 1, GUARD_LINE);
        return std::vector<T>::operator[](index);
    }

    size_t IndexOf(std::function<bool(T)> predicate)
    {
        for (size_t i = 0; i < this->size(); i++)
        {
            T item = std::vector<T>::operator[](i);
            if (predicate(item))
            {
                return i;
            }
        }
        return SIZE_MAX;
    }

    size_t IndexOf(T item, std::function<bool(T, T)> comparer)
    {
        for (size_t i = 0; i < this->size(); i++)
        {
            T element = std::vector<T>::operator[](i);
            if (comparer(item, element))
            {
                return i;
            }
        }
        return SIZE_MAX;
    }

    bool Contains(std::function<bool(T)> predicate)
    {
        return IndexOf(predicate) != SIZE_MAX;
    }

    bool Contains(T item, std::function<bool(T, T)> comparer)
    {
        return IndexOf(item, comparer) != SIZE_MAX;
    }
};
