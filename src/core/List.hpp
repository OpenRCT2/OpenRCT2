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
        for (T item : initializerList)
        {
            this->push_back(item);
        }
    }

    void Insert(T item, size_t index)
    {
        Guard::ArgumentInRange(index, (size_t)0, this->size());
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
        Guard::ArgumentInRange(index, (size_t)0, this->size() - 1);
        this->erase(this->begin() + index);
    }

    const T * ToArray() const
    {
        return Memory::DuplicateArray(this->data(), this->size());
    }

    const_reference operator[](size_t index) const
    {
        Guard::ArgumentInRange(index, (size_t)0, this->size() - 1);
        return std::vector<T>::operator[](index);
    }

    reference operator[](size_t index)
    {
        Guard::ArgumentInRange(index, (size_t)0, this->size() - 1);
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
};
