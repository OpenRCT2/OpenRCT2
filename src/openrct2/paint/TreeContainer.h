#pragma once

#include <unordered_map>
#include <vector>
#include "KeyGenerator.h"

template<class KeyType, class ValueType> class Node
{
public:
    Node(){};
    void Insert(uint32_t index);
    template<class KeyType, class ValueType> friend class TreeContainer;

protected:
    std::vector<ValueType> _values;
    std::unordered_map<uint32_t, Node> _children;
};

struct PaintStructDescriptorKey;
template<class KeyType, class ValueType>
class TreeContainer : public Node<KeyType, ValueType>
{
public:
    TreeContainer(const std::vector<KeyType>& keys)
        : _keyGenerator(keys)
    {
    }

    void Add(const KeyType& location, const ValueType& value);

    const std::vector<ValueType>* Get(const KeyType& location) const;

private:
    const std::vector<ValueType>* Get(const std::vector<uint32_t>& location) const;
    void Add(const std::vector<uint32_t>& location, const ValueType& value);
    KeyGenerator<KeyType> _keyGenerator;
};

template<class KeyType, class ValueType>
void Node<KeyType, ValueType>::Insert(uint32_t index)
{
    _children[index] = Node<KeyType, ValueType>();
}

template<class KeyType, class ValueType>
const std::vector<ValueType>* TreeContainer<KeyType, ValueType>::Get(
    const std::vector<uint32_t>& location) const
{
    const Node<KeyType, ValueType>* nextNode = this;
    for (const auto& loc : location)
    {
        const auto it = nextNode->_children.find(loc);
        if (it != nextNode->_children.end())
            nextNode = &(it->second);
        else
            return nullptr;
    }
    return &nextNode->_values;
}

template<class KeyType, class ValueType>
void TreeContainer<KeyType, ValueType>::Add(const std::vector<uint32_t>& location, const ValueType& value)
{
    Node<KeyType, ValueType>* nextNode = this;
    for (const auto& loc : location)
    {
        auto it = nextNode->_children.find(loc);
        if (it == nextNode->_children.end())
            nextNode->Insert(loc);
        nextNode = &nextNode->_children[loc];
    }
    nextNode->_values.push_back(value);
}

template<class KeyType, class ValueType>
void TreeContainer<KeyType, ValueType>::Add(const KeyType& location, const ValueType& value)
{
    auto keys = _keyGenerator.GenerateKeys(location);
    for (const auto& key : keys)
    {
        auto params = _keyGenerator.GetParams(key);
        Add(params, value);
    }
}

template<class KeyType, class ValueType>
const std::vector<ValueType>* TreeContainer<KeyType, ValueType>::Get(
    const KeyType& location) const
{
    return Get(_keyGenerator.GetParams(location));
}
